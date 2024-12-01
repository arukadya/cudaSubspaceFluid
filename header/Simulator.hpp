#ifndef Simulator_hpp
#define Simulator_hpp
#define TEXWIDTH 64
#define TEXHEIGHT 64
#define TEXDEPTH 64
#define TIMESTEP 0.125f
#define G0 9.8f
#define AMB_TEMPLATURE 25
#define DX 0.1f
#define BETA 5000

#include "Eigen/Core"
#include <iostream>
#include <vector>
#include <string>
#include "ComputeCommand.hpp"
#include "ShaderCommand.hpp"
struct Slab
{
    unsigned int size;
    float* src_texture;
    float* dst_texture;
    GLuint src_ssbo;
    GLuint dst_ssbo;
    void swap_src_dst()
    {
        std::swap(src_texture,dst_texture);
    }
    Slab(){}
    Slab(unsigned int width,unsigned int height,unsigned int depth)
    {
        // unsigned int 
        size = depth * width * height;
        src_texture = (float*)malloc(sizeof(float) * size);
        dst_texture = (float*)malloc(sizeof(float) * size);
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
        for(int i=0;i<size;++i)std::cout << i << "," << src_texture[i] <<std::endl;
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
struct AddForceEncoder : Encoder{
    AddForceEncoder(){}
    AddForceEncoder(char *shader_path) : Encoder(shader_path){}
    virtual void encode(
        Slab &out_y_force,
        Slab &density_tgt,
        Slab &density_amb,
        Slab &templature
    );
};
struct Simulator{
    //FluidVariables
    const float _dx;
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
    CalForceEncoder calForceEncoder;
    Simulator(float dx) :_dx(dx)
    {
        x_velocity = Slab(TEXWIDTH+1,TEXHEIGHT,TEXDEPTH);
        x_velocity = Slab(TEXWIDTH,TEXHEIGHT+1,TEXDEPTH);
        x_velocity = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH+1);
        x_force = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
        y_force = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
        z_force = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
        pressure = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
        density_tgt = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
        density_amb = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
        templature = Slab(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
        test = Slab(TEXWIDTH,TEXHEIGHT,1);
        calForceEncoder = CalForceEncoder("../shader/calForce.comp");
        init_density(100);
        init_templature(100);
    };
    void oneloop();
    void testCompute();
    void init_density(float init_density_value);
    void init_templature(float init_templature_value);
    void inputTXT(std::string &InputFileName);
    float* get_currentTexture();
private:
//    void makeSDF();
    //test
    
    void testSDF();
};
#endif /* Simulator_hpp */
