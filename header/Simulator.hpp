#ifndef Simulator_hpp
#define Simulator_hpp
#include "Eigen/Core"
#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>
#include <unsupported/Eigen/NNLS>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "ComputeCommand.hpp"
#include "ShaderCommand.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <random>
#include <unistd.h>

#define G0 9.8f
#define AMB_TEMPLATURE 0.0f
#define AMB_DENSITY 0.0f
#define TGT_TEMPLATURE 100.0f
#define TGT_DENSITY 100.0f
// #define err_threshold 0.005
#define w_threshold 0.01

using ScalarType = float;
using IndexType = int64_t;
using Triplet = Eigen::Triplet<ScalarType,IndexType>;
using SparseMatrix = Eigen::SparseMatrix<ScalarType>;
typedef Eigen::SparseMatrix<float, Eigen::RowMajor, int64_t> SpMat;
struct Slab
{
    unsigned int size;
    unsigned int _width,_height,_depth;
    float* src_texture;
    float* dst_texture;
    GLuint src_ssbo;
    GLuint dst_ssbo;
    void swap_src_dst()
    {
        std::swap(src_texture,dst_texture);
    }
    Slab(){}
    Slab(unsigned int size,float value)
    {
        src_texture = (float*)malloc(sizeof(float) * size);
        dst_texture = (float*)malloc(sizeof(float) * size);
        for(unsigned int i=0;i<size;++i)src_texture[i] = value;
        for(unsigned int i=0;i<size;++i)dst_texture[i] = value;
    }
    Slab(unsigned int width,unsigned int height,unsigned int depth)
    {
        // unsigned int 
        _width = width;_height = height;_depth = depth;
        size = depth * width * height;
        src_texture = (float*)malloc(sizeof(float) * size);
        dst_texture = (float*)malloc(sizeof(float) * size);
    }
    Slab(unsigned int width,unsigned int height,unsigned int depth,float value)
    {
        _width = width;_height = height;_depth = depth;
        size = depth * width * height;
        // std::cout << "size = " << size << std::endl;
        src_texture = (float*)malloc(sizeof(float) * size);
        dst_texture = (float*)malloc(sizeof(float) * size);
        for(unsigned int i=0;i<size;++i)src_texture[i] = value;
        for(unsigned int i=0;i<size;++i)dst_texture[i] = value;
    }
    float get_volume_value(unsigned int x,unsigned int y,unsigned int z)
    {
        // return src_texture[resequence3to1(x,y,z,texwidth,_texheight,_texdepth)];
        return src_texture[resequence3to1(x,y,z,_width,_height,_depth)];
    }
    void set_volume_value(unsigned int x,unsigned int y,unsigned int z,float val)
    {
        // dst_texture[resequence3to1(x,y,z,texwidth,_texheight,_texdepth)] = val;
        dst_texture[resequence3to1(x,y,z,_width,_height,_depth)] = val;
    }
    void setupBuffer(unsigned int src_bind_index,unsigned int dst_bind_index)
    {
        glGenBuffers(1, &src_ssbo);
        glGenBuffers(1, &dst_ssbo);
        //Bufferのデータ形式
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, src_ssbo);
        //BufferにデータをCPUからGPUに転送
        glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), src_texture, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, dst_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), dst_texture, GL_DYNAMIC_COPY);
        //BufferをShaderのindexにBind
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, src_bind_index, src_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, dst_bind_index, dst_ssbo);
    }
    void setupBuffer(unsigned int src_bind_index)
    {
        glGenBuffers(1, &src_ssbo);
        glGenBuffers(1, &dst_ssbo);
        //Bufferのデータ形式
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, src_ssbo);
        //BufferにデータをCPUからGPUに転送
        glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), src_texture, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, dst_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), dst_texture, GL_DYNAMIC_COPY);
        //BufferをShaderのindexにBind
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, src_bind_index, src_ssbo);
    }
    void sl_getSrcBufferSubData()
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, src_ssbo);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * sizeof(float), src_texture);
    }
    void sl_getDstBufferSubData()
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, dst_ssbo);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * sizeof(float), dst_texture);
    }
    void print_src()
    {
        for(unsigned int i=0;i<size;++i)
        {
            if(std::fabs(src_texture[i]) > 1e-6)std::cout << i << "," << src_texture[i] <<std::endl;
        }
    }
    void outputTXT(std::string OutputFileName){
        std::ofstream writing_file;
        std::cout << OutputFileName << std::endl;
        writing_file.open(OutputFileName, std::ios::out);
        std::string writing_text;
        int nx = _width;
        int ny = _height;
        int nz = _depth;
        for(int k=0;k<nz;k++){
            for(int j=0;j<ny;j++){
                for(int i=0;i<nx;i++){
                    if(abs(get_volume_value(i,j,k)) < 1e-6) writing_file << 0 << std::endl;
                    else writing_file << get_volume_value(i,j,k) << std::endl;
                }
            }
        }
        writing_file.close();
    }
};
struct Encoder
{
    const char *_shader_path;
    Encoder(){}
    Encoder(char *shader_path) : _shader_path(shader_path){}
    virtual ~Encoder() {}
    virtual void encode(){};
};
struct CalForceEncoder : Encoder{
    CalForceEncoder(){}
    CalForceEncoder(char *shader_path) : Encoder(shader_path){}
    virtual void encode(
        Slab &out_y_force,
        Slab &density_tgt,
        Slab &density_amb,
        Slab &templature
    );
};

Eigen::MatrixXf cal_Basis(Eigen::MatrixXf &Snapshot, unsigned int &reduce_dimention,float threshold);

struct Simulator{
    //FluidVariables
    const float _dx; const float _dt;const float _beta;const float _epsilon;const float _nu;
    const unsigned int _texwidth; const unsigned int _texheight; const unsigned int _texdepth;
    const unsigned int _flame_num;const unsigned int _snap_num; const unsigned int _discard_flame; 
    const float _singularity_threshold;const float _cubature_threshold;
    const unsigned int _devide_num;
    unsigned int _timestamp;
    float err_threshold;
    unsigned int _delta_snap;
    unsigned int _reduce_dimention;
    unsigned int _snap_devide_num;
    float _sub_dt;

    float basis_time;
    float projection_time;
    // std::vector<float> P_error_vector;
    std::vector<float> U3_error_vector;

    Slab x_velocity;
    Slab y_velocity;
    Slab z_velocity;
    Slab x_force;
    Slab y_force;
    Slab z_force;
    Slab x_omega;
    Slab y_omega;
    Slab z_omega;
    Slab eta;
    Slab N_x;
    Slab N_y;
    Slab N_z;
    Slab pressure;
    Slab density_tgt;
    Slab density_amb;
    Slab templature;
    Slab test;

    Eigen::VectorXf all_velocity;
    Eigen::VectorXf px;
    Eigen::VectorXf b;
    Eigen::VectorXf origin_b;
    Eigen::MatrixXf U0_Snapshot;
    Eigen::MatrixXf U1_Snapshot;
    Eigen::MatrixXf U2_Snapshot;
    Eigen::MatrixXf U3_Snapshot;
    Eigen::MatrixXf P_Snapshot;
    // Eigen::MatrixXf U0_all_frame;
    // Eigen::MatrixXf U1_all_frame;
    // Eigen::MatrixXf U2_all_frame;
    Eigen::MatrixXf U3_all_frame;
    Eigen::MatrixXf P_all_frame;
    // Eigen::MatrixXf b_all_frame;
    SparseMatrix DiffusionMatrix;//V
    SparseMatrix Vel2DivMatrix;//W
    SparseMatrix PoissonMatrix;//X
    SparseMatrix Pressure2VelocityMatrix;//Y
    SparseMatrix DirichletBoundaryMatrix;//D

    //subspace
    Eigen::MatrixXf U0;
    Eigen::MatrixXf U1;
    Eigen::MatrixXf U2;
    Eigen::MatrixXf U3;
    Eigen::MatrixXf P;
    // Eigen::MatrixXf cubatureAdvectMatrix;
    Eigen::VectorXf cubatureWeightVector;
    std::set<unsigned int>cubaturePointSet;
    Eigen::VectorXf reduced_all_velocity;
    Eigen::VectorXf reduced_px;

    Eigen::MatrixXf reduced_DiffusionMatrix;
    Eigen::MatrixXf reduced_Vel2DivMatrix;//W
    Eigen::MatrixXf reduced_PoissonMatrix;//X　???こいつが正定値対称行列になるのすごい　要確認
    Eigen::MatrixXf reduced_Pressure2VelocityMatrix;//Y
    Eigen::MatrixXf reduced_DirichletBoundaryMatrix;//D

    //devide
    std::vector<Eigen::MatrixXf> devided_DiffusionMatrix_List;//V
    std::vector<Eigen::MatrixXf> devided_Vel2DivMatrix_List;//W
    std::vector<Eigen::MatrixXf> devided_PoissonMatrix_List;//X
    std::vector<Eigen::MatrixXf> devided_Pressure2VelocityMatrix_List;//Y
    std::vector<Eigen::MatrixXf> devided_DirichletBoundaryMatrix_List;//D

    std::vector<Eigen::MatrixXf> devided_U0_List;
    std::vector<Eigen::MatrixXf> devided_U1_List;
    std::vector<Eigen::MatrixXf> devided_U2_List;
    std::vector<Eigen::MatrixXf> devided_U3_List;
    std::vector<Eigen::MatrixXf> devided_P_List;
    std::vector<Eigen::MatrixXf> devided_U1_Snapshot_List;
    std::vector<Eigen::VectorXf> devided_cubatureWeightVectorList;
    std::vector<std::set<unsigned int>>devided_cubaturePointSetList;


    // CalForceEncoder calForceEncoder;
    std::string origin_exact_folder_name;
    std::string origin_density_floder_name;
    std::string subspace_density_floder_name;
    std::string devided_density_floder_name;
    Simulator(float dx,float dt,float beta,float epsilon,float nu,
    unsigned int texwidth, unsigned int texheight, unsigned int texdepth, 
    unsigned int flame_num, unsigned int snap_num, unsigned int discard_flame,
    float s_threshold,float c_threshold,
    unsigned int devide_num) 
    // : _dx(dx/texwidth),_dt(dt * texwidth),
    : _dx(dx),_dt(dt),
    _beta(beta),_epsilon(epsilon),_nu(nu),
    _texwidth(texwidth),_texheight(texheight),_texdepth(texdepth),
    _flame_num(flame_num),_snap_num(snap_num),_discard_flame(discard_flame),
    _singularity_threshold(s_threshold),_cubature_threshold(c_threshold),
    _devide_num(devide_num)
    {
        _timestamp = 0;
        _delta_snap = _flame_num / _snap_num;
        // _snap_devide_num = _snap_num / 3;
        // err_threshold = threshold;
        _sub_dt = _dt;
        if(_flame_num % _snap_num != 0)std::cout << " Warning : _flame_num % _snap_num != 0" << std::endl;
        std::cout << "dx,dt,beta,epsilon,nu = " << _dx << "," << _dt << "," << _beta << "," << _epsilon << "," << _nu << std::endl;
        std::cout << "width,height,depth = " << _texwidth << "," << _texheight << "," << _texdepth << std::endl;
        std::cout << "flame_num,snap_num = " << _flame_num << "," << _snap_num << std::endl;
        std::cout << "s_threshold, c_threshold = " << s_threshold << "," << c_threshold << std::endl;
        std::cout << "discard_flame, devide_num = " << _discard_flame << "," << devide_num << std::endl; 
        x_velocity = Slab(_texwidth+1,_texheight,_texdepth,0.0f);
        y_velocity = Slab(_texwidth,_texheight+1,_texdepth,0.0f);
        z_velocity = Slab(_texwidth,_texheight,_texdepth+1,0.0f);
        x_omega = Slab(_texwidth,_texheight,_texdepth,0.0f);
        y_omega = Slab(_texwidth,_texheight,_texdepth,0.0f);
        z_omega = Slab(_texwidth,_texheight,_texdepth,0.0f);
        eta = Slab(_texwidth,_texheight,_texdepth,0.0f);
        N_x = Slab(_texwidth,_texheight,_texdepth,0.0f);
        N_y = Slab(_texwidth,_texheight,_texdepth,0.0f);
        N_z = Slab(_texwidth,_texheight,_texdepth,0.0f);
        x_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
        y_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
        z_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
        pressure = Slab(_texwidth,_texheight,_texdepth,0.0f);
        density_tgt = Slab(_texwidth,_texheight,_texdepth,0.0f);
        density_amb = Slab(_texwidth,_texheight,_texdepth,AMB_DENSITY);
        templature = Slab(_texwidth,_texheight,_texdepth,AMB_TEMPLATURE);
        test = Slab(_texwidth,_texheight,1,0.0f);
        init_density(TGT_DENSITY);
        init_templature(TGT_TEMPLATURE);
        // calForceEncoder = CalForceEncoder("../shader/calForce.comp");
        origin_exact_folder_name = "origin_exact_txt";
        origin_density_floder_name = "origin_density_txt";
        subspace_density_floder_name = "subspace_density_txt";
        devided_density_floder_name = "devided_density_txt";
        std::filesystem::create_directories(origin_exact_folder_name);
        std::filesystem::create_directories(origin_density_floder_name);
        std::filesystem::create_directories(subspace_density_floder_name);
        std::filesystem::create_directories(devided_density_floder_name);
        PoissonMatrix = SparseMatrix(_texwidth*_texheight*_texdepth,texwidth*_texheight*_texdepth);
        Vel2DivMatrix = SparseMatrix(_texwidth*_texheight*_texdepth, 3*(texwidth + 1)*_texheight*_texdepth);
        DiffusionMatrix = SparseMatrix(3*(_texwidth + 1)*_texheight*_texdepth, 3*(texwidth + 1)*_texheight*_texdepth);
        DirichletBoundaryMatrix = SparseMatrix(3*(texwidth + 1)*_texheight*_texdepth, 3*(texwidth + 1)*_texheight*_texdepth);
        Pressure2VelocityMatrix = SparseMatrix(3*(_texwidth + 1)*_texheight*_texdepth, texwidth*_texheight*_texdepth);
        all_velocity = Eigen::VectorXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth);
        px = Eigen::VectorXf::Zero(_texwidth *_texheight*_texdepth);
        b = Eigen::VectorXf::Zero(_texwidth *_texheight*_texdepth);
        U0_Snapshot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        U1_Snapshot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        U2_Snapshot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        U3_Snapshot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        P_Snapshot = Eigen::MatrixXf::Zero(_texwidth*_texheight*_texdepth, _snap_num);
        // U0_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        // U1_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        // U2_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        U3_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        P_all_frame = Eigen::MatrixXf::Zero(_texwidth*_texheight*_texdepth, _flame_num);
        // b_all_frame = Eigen::MatrixXf::Zero(_texwidth*_texheight*_texdepth, _flame_num);
        calPoissonMatrix(_dt);
        std::cout << "Poison" << std::endl;
        calDiffusionMatrix(_dt);
        std::cout << "Diffusion" << std::endl;
        calVel2DivMatrix();
        std::cout << "V2D" << std::endl;
        calPressure2VelocityMatrix(_dt);
        std::cout << "P2V" << std::endl;
        calDirichletBoundaryMatrix();
        std::cout << "DB" << std::endl;
        projection_time = 0;
        basis_time = 0;
    };
    //full simulator
    void oneloop();
    void calDiffusionMatrix(float dt);
    void calPoissonMatrix(float dt);
    void calVel2DivMatrix();
    void calPressure2VelocityMatrix(float dt);
    void calDirichletBoundaryMatrix();
    void init_all_velocity();
    void init_velocity();
    void init_density(float init_density_value);
    void init_templature(float init_templature_value);
    void init_pressure(float init_pressure_value);
    void init_velocity(float init_pressure_value);
    float TriLinearInterporation(float x,float y,float z,Slab &val);
    // float TriLinearInterporation(float x,float y,float z,unsigned int nx,unsigned int ny,unsigned int nz,Eigen::VectorXf &val);
    float* get_currentTexture();
    void faceAdvect();
    void centerAdvect(Slab &val);
    void project();
    void addForce(float dt);
    Eigen::Vector3f getBuoyanacy(int i,int j, int k);
    void calConfinent();
    Eigen::Vector3f getConfinent(int i,int j,int k);

    //construct basis
    
    void write_snapshot(Eigen::MatrixXf &mat, Eigen::VectorXf &snap);
    void write_exact_solution(Eigen::MatrixXf &mat, Eigen::VectorXf &snap);
    void getBasisQRSVD(
        Eigen::MatrixXf &devided_U0_Snapshot,
        Eigen::MatrixXf &devided_U1_Snapshot,
        Eigen::MatrixXf &devided_U2_Snapshot,
        Eigen::MatrixXf &devided_U3_Snapshot,
        Eigen::MatrixXf &devided_P_Snapshot,
        Eigen::MatrixXf &devided_U0,
        Eigen::MatrixXf &devided_U1,
        Eigen::MatrixXf &devided_U2,
        Eigen::MatrixXf &devided_U3,
        Eigen::MatrixXf &devided_P
    );
    void getReducedLinearOperator(
        Eigen::MatrixXf &devided_U0,
        Eigen::MatrixXf &devided_U1,
        Eigen::MatrixXf &devided_U2,
        Eigen::MatrixXf &devided_U3,
        Eigen::MatrixXf &devided_P
    );

    //subspace
    Eigen::MatrixXf getRowsCorrespondPoint(Eigen::MatrixXf &Mat, unsigned int x,unsigned int y, unsigned int z);//Basis
    // Eigen::Vector3f getVelocityFromSnapshot(Eigen::MatrixXf &Snapshot,unsigned int x,unsigned int y,unsigned int z,unsigned int T);
    Eigen::VectorXf getVelocityFromSnapshot(Eigen::MatrixXf &Snapshot,unsigned int x,unsigned int y,unsigned int z,unsigned int T);
    Eigen::MatrixXf getSubspaceAdvect_A(std::set<unsigned int> &CubaturePointSet,Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &Basis);
    Eigen::VectorXf getColACoresspondCubaturePoint(unsigned int point_id,Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &Basis);
    float probablity_distribution_function(unsigned int point_id,Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &Basis,unsigned int restPoints_num,Eigen::VectorXf &residual);
    void largeSamplingCubature(
        std::set<unsigned int> &CubaturePointSet,
        Eigen::VectorXf &weight_vector,
        Eigen::MatrixXf &devided_U1_Snapshot,
        Eigen::MatrixXf &devided_U1,
        float error_threshold,float weight_threshold);
    Eigen::VectorXf getSubspaceAdvect_b(Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &Basis);
    void subspace_execute();
    void subspace_oneloop(
        Eigen::MatrixXf &devided_U0,
        Eigen::MatrixXf &devided_U1,
        Eigen::MatrixXf &devided_U2,
        Eigen::MatrixXf &devided_U3,
        Eigen::MatrixXf &devided_P,
        Eigen::MatrixXf &devided_DiffusionMatrix,
        Eigen::MatrixXf &devided_DirichletBoundaryMatrix,
        Eigen::MatrixXf &devided_Vel2DivMatrix,
        Eigen::MatrixXf &devided_PoissonMatrix,
        Eigen::MatrixXf &devided_Pressure2VelocityMatrix,
        std::set<unsigned int> &CubaturePointSet,
        Eigen::VectorXf &weight_vector);
    void subspace_project(
        Eigen::MatrixXf &devided_DirichletBoundaryMatrix,
        Eigen::MatrixXf &devided_Vel2DivMatrix,
        Eigen::MatrixXf &devided_PoissonMatrix,
        Eigen::MatrixXf &devided_Pressure2VelocityMatrix);
    void subspace_advect(
        std::set<unsigned int> &CubaturePointSet,
        Eigen::VectorXf &weight_vector,
        Eigen::MatrixXf &devided_U0,
        Eigen::MatrixXf &devided_U1);
    // Eigen::Vector3f face_advect_function(Eigen::Vector3i &pos,Eigen::VectorXf &velocity,float dt);
    Eigen::VectorXf face_advect_function(Eigen::Vector3i &pos,Eigen::VectorXf &velocity,float dt);

    //devideSnapshot
    void calDevidedList();
    void calDevidedOperatorList();
    void calCubatureList();
    void getDevidedBasis(unsigned int start_snap_id,unsigned int end_snap_id,
        Eigen::MatrixXf &devided_U0,
        Eigen::MatrixXf &devided_U1,
        Eigen::MatrixXf &devided_U2,
        Eigen::MatrixXf &devided_U3,
        Eigen::MatrixXf &devided_P);
    //test
    void inputTXT(std::string &InputFileName);
    void output_txt(std::string &density_floder_name,unsigned int id);
    void output_Basis(unsigned int devided_id);
    void output_Snapshot(unsigned int devided_id,Eigen::MatrixXf &devided_Snapshot);
    void output_exact();
    void input_exact();
    void input_Basis(unsigned int devided_id);
    void input_Snapshot(unsigned int devided_id);
    void all2xyz();
    void testCompute();
    void origin_project();

private:
    void testSDF();
};
#endif /* Simulator_hpp */
