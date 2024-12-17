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
#include "cuda_util.cuh"
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
struct CudaSimulator{
    const float _dx;const float _dt;const float _beta;
    const unsigned int _texwidth; const unsigned int _texheight; const unsigned int _texdepth;
    myCUDA_Array<float> x_velocity;
    myCUDA_Array<float> y_velocity;
    myCUDA_Array<float> z_velocity;
    myCUDA_Array<float> x_force;
    myCUDA_Array<float> y_force;
    myCUDA_Array<float> z_force;
    myCUDA_Array<float> pressure;
    myCUDA_Array<float> density_tgt;
    myCUDA_Array<float> density_amb;
    myCUDA_Array<float> templature;
    CudaSimulator(float dx,float dt,unsigned int texwidth, unsigned int texheight, unsigned int texdepth, float beta) 
    : _dx(dx),_dt(dt),_texwidth(texwidth),_texheight(texheight),_texdepth(texdepth),_beta(beta)
    {
        x_velocity  = myCUDA_Array<float>(_texwidth+1,_texheight,_texdepth);
        x_velocity  = myCUDA_Array<float>(_texwidth,_texheight+1,_texdepth);
        x_velocity  = myCUDA_Array<float>(_texwidth,_texheight,_texdepth+1);
        x_force     = myCUDA_Array<float>(_texwidth,_texheight,_texdepth);
        y_force     = myCUDA_Array<float>(_texwidth,_texheight,_texdepth);
        z_force     = myCUDA_Array<float>(_texwidth,_texheight,_texdepth);
        pressure    = myCUDA_Array<float>(_texwidth,_texheight,_texdepth);
        density_tgt = myCUDA_Array<float>(_texwidth,_texheight,_texdepth);
        density_amb = myCUDA_Array<float>(_texwidth,_texheight,_texdepth);
        templature  = myCUDA_Array<float>(_texwidth,_texheight,_texdepth);
        init_density(100);
        init_templature(100);
    }
    void oneloop();
    void testCompute();
    void init_density(float init_density_value);
    void init_templature(float init_templature_value);
    void inputTXT(std::string &InputFileName);
    float* get_currentTexture();
};

struct Simulator{
    //FluidVariables
    const float _dx; const float _dt;const float _beta;
    const unsigned int _texwidth; const unsigned int _texheight; const unsigned int _texdepth;
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

    SparseMatrix Vel2DivMatrix;
    SparseMatrix PoissonMatrix;
    SparseMatrix Pre2DivMatrix;

    CalForceEncoder calForceEncoder;
    std::string density_floder_name;
    Simulator(float dx,float dt,unsigned int texwidth, unsigned int texheight, unsigned int texdepth, float beta) 
    : _dx(dx/texwidth),_dt(dt),_texwidth(texwidth),_texheight(texheight),_texdepth(texdepth),_beta(beta)
    {
        std::cout << "dx,dt,beta = " << _dx << "," << _dt << "," << _beta << std::endl;
        std::cout << "width,height,depth,slice = " << _texwidth << "," << _texheight << "," << _texdepth << std::endl;
        x_velocity = Slab(texwidth+1,_texheight,_texdepth,0.0f);
        y_velocity = Slab(texwidth,_texheight+1,_texdepth,0.0f);
        z_velocity = Slab(texwidth,_texheight,_texdepth+1,0.0f);
        x_force = Slab(texwidth,_texheight,_texdepth,0.0f);
        y_force = Slab(texwidth,_texheight,_texdepth,0.0f);
        z_force = Slab(texwidth,_texheight,_texdepth,0.0f);
        pressure = Slab(texwidth,_texheight,_texdepth,0.0f);
        density_tgt = Slab(texwidth,_texheight,_texdepth,0.0f);
        density_amb = Slab(texwidth,_texheight,_texdepth,AMB_DENSITY);
        templature = Slab(texwidth,_texheight,_texdepth,AMB_TEMPLATURE);
        test = Slab(texwidth,_texheight,1,0.0f);
        calForceEncoder = CalForceEncoder("../shader/calForce.comp");
        init_density(TGT_DENSITY);
        init_templature(TGT_TEMPLATURE);
        // init_velocity(-_dx);
        density_floder_name = "density_txt";
        std::filesystem::create_directories(density_floder_name);
        PoissonMatrix = SparseMatrix(texwidth*_texheight*_texdepth,texwidth*_texheight*_texdepth);
        Vel2DivMatrix = SparseMatrix(3*texwidth*_texheight*_texdepth,texwidth*_texheight*_texdepth);

        all_velocity = Slab(3 * (texwidth+1) * _texheight * _texdepth, 0.0f);

    };
    void oneloop();
    void testCompute();
    void init_velocity();
    void init_density(float init_density_value);
    void init_templature(float init_templature_value);
    void init_pressure(float init_pressure_value);
    void init_velocity(float init_pressure_value);
    void init_all__velocity();
    void inputTXT(std::string &InputFileName);
    float TriLinearInterporation(float x,float y,float z,Slab &val);
    float* get_currentTexture();
    void output_txt(unsigned int id);
    void faceAdvect();
    void centerAdvect(Slab &val);
    void project();
    void addForce();

    Eigen::Vector3d getBuoyanacy(int i,int j, int k);

    void calPoissonMatrix();
    void calVel2DivMatrix();
private:
    void testSDF();
};
#endif /* Simulator_hpp */
