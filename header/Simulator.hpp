#ifndef Simulator_hpp
#define Simulator_hpp
#include "Eigen/Core"
#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>
#include <iostream>
#include <vector>
#include <string>
#include "ComputeCommand.hpp"
#include "ShaderCommand.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

#define G0 9.8f
#define AMB_TEMPLATURE 90.0f
#define AMB_DENSITY 100.0f
#define TGT_TEMPLATURE 100.0f
#define TGT_DENSITY 100.0f

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

Eigen::MatrixXf cal_Basis(Eigen::MatrixXf &SnapShot, unsigned int &reduce_dimention,float threshold);

struct Simulator{
    //FluidVariables
    const float _dx; const float _dt;const float _beta;
    const unsigned int _texwidth; const unsigned int _texheight; const unsigned int _texdepth;
    const unsigned int _flame_num;const unsigned int _snap_num; const float _threshold;
    unsigned int _timestamp;
    unsigned int _delta_snap;
    unsigned int _reduce_dimention;
    unsigned int _snap_devide_num;
    Slab x_velocity;
    Slab y_velocity;
    Slab z_velocity;
    Slab x_force;
    Slab y_force;
    Slab z_force;
    Slab pressure;
    Slab density_tgt;
    Slab density_amb;
    Slab templature;
    Slab test;

    Eigen::VectorXf all_velocity;
    Eigen::VectorXf px;
    Eigen::MatrixXf U0_SnapShot;
    Eigen::MatrixXf U1_SnapShot;
    Eigen::MatrixXf U2_SnapShot;
    Eigen::MatrixXf U3_SnapShot;
    Eigen::MatrixXf P_SnapShot;
    Eigen::MatrixXf U0_all_frame;
    Eigen::MatrixXf U1_all_frame;
    Eigen::MatrixXf U2_all_frame;
    Eigen::MatrixXf U3_all_frame;
    Eigen::MatrixXf P_all_frame;
    Eigen::MatrixXf b_all_frame;
    SparseMatrix DiffusionMatrix;//V
    SparseMatrix Vel2DivMatrix;//W
    SparseMatrix PoissonMatrix;//X
    SparseMatrix Pressure2VelocityMatrix;//Y
    SparseMatrix DirichletBoundaryMatrix;//D

    Eigen::MatrixXf reduced_Vel2DivMatrix;//W
    Eigen::MatrixXf reduced_PoissonMatrix;//X　???こいつが正定値対称行列になるのすごい　要確認
    Eigen::MatrixXf reduced_Pressure2VelocityMatrix;//Y
    Eigen::MatrixXf reduced_DirichletBoundaryMatrix;//D

    std::vector<Eigen::MatrixXf> devided_reduced_Vel2DivMatrix;//W
    std::vector<Eigen::MatrixXf> devided_reduced_PoissonMatrix;//X
    std::vector<Eigen::MatrixXf> devided_reduced_Pressure2VelocityMatrix;//Y
    std::vector<Eigen::MatrixXf> devided_reduced_DirichletBoundaryMatrix;//D

    Eigen::MatrixXf U0;
    Eigen::MatrixXf U1;
    Eigen::MatrixXf U2;
    Eigen::MatrixXf U3;
    Eigen::MatrixXf P;
    std::vector<Eigen::MatrixXf> devided_U0;
    std::vector<Eigen::MatrixXf> devided_U1;
    std::vector<Eigen::MatrixXf> devided_U2;
    std::vector<Eigen::MatrixXf> devided_U3;
    std::vector<Eigen::MatrixXf> devided_P;
    // Eigen::MatrixXf reduced_U2_SnapShot;
    // Eigen::MatrixXf reduced_U3_SnapShot;
    Eigen::VectorXf reduced_all_velocity;
    Eigen::VectorXf reduced_px;

    // CalForceEncoder calForceEncoder;
    std::string density_floder_name;
    Simulator(float dx,float dt,float beta,
    unsigned int texwidth, unsigned int texheight, unsigned int texdepth, 
    unsigned int flame_num, unsigned int snap_num, float threshold) 
    : _dx(dx/texwidth),_dt(dt * texwidth),_beta(beta),
    _texwidth(texwidth),_texheight(texheight),_texdepth(texdepth),
    _flame_num(flame_num),_snap_num(snap_num),_threshold(threshold)
    {
        _timestamp = 0;
        _delta_snap = _flame_num / _snap_num;
        _snap_devide_num = _snap_num / 3;
        if(_flame_num % _snap_num != 0)std::cout << " Warning : _flame_num % _snap_num != 0" << std::endl;
        std::cout << "dx,dt,beta = " << _dx << "," << _dt << "," << _beta << std::endl;
        std::cout << "width,height,depth = " << _texwidth << "," << _texheight << "," << _texdepth << std::endl;
        std::cout << "flame_num,snap_num,threshold = " << _flame_num << "," << _snap_num << "," << threshold << std::endl;
        x_velocity = Slab(_texwidth+1,_texheight,_texdepth,0.0f);
        y_velocity = Slab(_texwidth,_texheight+1,_texdepth,0.0f);
        z_velocity = Slab(_texwidth,_texheight,_texdepth+1,0.0f);
        x_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
        y_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
        z_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
        pressure = Slab(_texwidth,_texheight,_texdepth,0.0f);
        density_tgt = Slab(_texwidth,_texheight,_texdepth,0.0f);
        density_amb = Slab(_texwidth,_texheight,_texdepth,AMB_DENSITY);
        templature = Slab(_texwidth,_texheight,_texdepth,AMB_TEMPLATURE);
        test = Slab(_texwidth,_texheight,1,0.0f);
        // calForceEncoder = CalForceEncoder("../shader/calForce.comp");
        density_floder_name = "density_txt";
        std::filesystem::create_directories(density_floder_name);
        PoissonMatrix = SparseMatrix(_texwidth*_texheight*_texdepth,texwidth*_texheight*_texdepth);
        Vel2DivMatrix = SparseMatrix(_texwidth*_texheight*_texdepth, 3*(texwidth + 1)*_texheight*_texdepth);
        // DiffusionMatrix = SparseMatrix(3*_texwidth*_texheight*_texdepth, 3*(texwidth + 1)*_texheight*_texdepth);
        DirichletBoundaryMatrix = SparseMatrix(3*(texwidth + 1)*_texheight*_texdepth, 3*(texwidth + 1)*_texheight*_texdepth);
        Pressure2VelocityMatrix = SparseMatrix(3*(_texwidth + 1)*_texheight*_texdepth, texwidth*_texheight*_texdepth);
        all_velocity = Eigen::VectorXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth);
        px = Eigen::VectorXf::Zero(_texwidth *_texheight*_texdepth);

        U0_SnapShot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        U1_SnapShot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        U2_SnapShot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        U3_SnapShot = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _snap_num);
        P_SnapShot = Eigen::MatrixXf::Zero(_texwidth*_texheight*_texdepth, _snap_num);
        U0_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        U1_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        U2_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        U3_all_frame = Eigen::MatrixXf::Zero(3*(_texwidth + 1)*_texheight*_texdepth, _flame_num);
        P_all_frame = Eigen::MatrixXf::Zero(_texwidth*_texheight*_texdepth, _flame_num);
        b_all_frame = Eigen::MatrixXf::Zero(_texwidth*_texheight*_texdepth, _flame_num);
        calPoissonMatrix();
        // std::cout << "Poison" << std::endl;
        calVel2DivMatrix();
        // std::cout << "V2D" << std::endl;
        calPressure2VelocityMatrix();
        // std::cout << "P2V" << std::endl;
        calDirichletBoundaryMatrix();
    };
    //full simulator
    void oneloop();
    void calDiffusionMatrix();
    void calPoissonMatrix();
    void calVel2DivMatrix();
    void calPressure2VelocityMatrix();
    void calDirichletBoundaryMatrix();
    void init_all_velocity();
    void init_velocity();
    void init_density(float init_density_value);
    void init_templature(float init_templature_value);
    void init_pressure(float init_pressure_value);
    void init_velocity(float init_pressure_value);
    float TriLinearInterporation(float x,float y,float z,Slab &val);
    float* get_currentTexture();
    void faceAdvect();
    void centerAdvect(Slab &val);
    void project();
    void diffusion();
    void addForce();
    Eigen::Vector3d getBuoyanacy(int i,int j, int k);

    //construct basis
    
    void write_snapshot(Eigen::MatrixXf &mat, Eigen::VectorXf &snap);
    void write_exact_solution(Eigen::MatrixXf &mat, Eigen::VectorXf &snap);
    void getBasisQRSVD();
    void getReducedLinearOperator();
    void getDevidedBasis();
    void getDevidedReducedLinearOperator();

    //subspace
    void subspace_execute();
    void subspace_oneloop();
    void subspace_project();

    void devided_subspace_execute();
    void devided_subspace_oneloop();
    void devided_subspace_project();
    
    //test
    void inputTXT(std::string &InputFileName);
    void output_txt(unsigned int id);
    void output_Basis();
    void input_Basis();
    void all2xyz();
    void testCompute();
private:
    void testSDF();
};
#endif /* Simulator_hpp */
