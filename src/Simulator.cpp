//
//  Simulator.cpp
//  OpenGLTest
//
//  Created by 須之内俊樹 on 2024/05/26.
//
#include "Simulator.hpp"
float* Simulator::get_currentTexture()
{
    return density_tgt.src_texture;
}
void Simulator::testCompute()
{
    //GPUに置きっぱなしのデータにアクセスしたい。
    //CPUで利用ー＞時系列データの作成。GPUで利用ー＞ほぼこれ。シミュレーションやレンダリングに利用
    uint32_t num = TEXWIDTH * TEXHEIGHT;
    GLuint shader_program = loadComputeProgram("../shader/test.comp");

    glUseProgram(shader_program);
    
    GLuint uniform_element_size = glGetUniformLocation(shader_program, "element_size");
    GLuint texwidth = glGetUniformLocation(shader_program, "texwidth");
    //Bufferの宣言のイメージ。第一引数に指定した数をまとめて宣言できる。
    // glGenBuffers(1, &test.src_ssbo);
    //Bufferの型を決める。
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, test.src_ssbo);
    //BufferにデータをCPUからGPUに転送する。
    // glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(float), nullptr, GL_DYNAMIC_COPY);
    // glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(float), testTexture, GL_DYNAMIC_DRAW);
    test.setupBuffer(3);
    glUniform1ui(uniform_element_size, num);
    glUniform1ui(texwidth, TEXWIDTH);
    // test.sl_bindBufferBase(3);
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, test.src_ssbo);
    glDispatchCompute(num / 64 + 1, 1, 1);

    GLuint shader_program_2 = loadComputeProgram("../shader/test2.comp");

    glUseProgram(shader_program_2);
    GLuint uniform_element_size_2 = glGetUniformLocation(shader_program_2, "element_size");
    GLuint texwidth_2 = glGetUniformLocation(shader_program_2, "texwidth");
    //Bufferの宣言のイメージ。第一引数に指定した数をまとめて宣言できる。
    glGenBuffers(1, &test.dst_ssbo);
    //Bufferの型を決める。
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, test.dst_ssbo);
    //BufferにデータをCPUからGPUに転送する。
    // glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(float), nullptr, GL_DYNAMIC_COPY);
    glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(float), test.src_texture, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, test.src_ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, test.dst_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(float), nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, test.dst_ssbo);

    glUniform1ui(uniform_element_size_2, num);
    glUniform1ui(texwidth_2, TEXWIDTH);
    glDispatchCompute(num / 64 + 1, 1, 1);
    

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, test.dst_ssbo);
    //GPUからBufferを取得
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(float), test.dst_texture);
    test.swap_src_dst();
    glDeleteBuffers(1, &test.src_ssbo);
    deleteComputeShaderProgram(shader_program);
    glDeleteBuffers(2, &test.dst_ssbo);
    deleteComputeShaderProgram(shader_program_2);
}
void CalForceEncoder::encode
(
    Slab &out_y_force,
    Slab &density_tgt,
    Slab &density_amb,
    Slab &templature 
)
{
    // GLuint shader_program = loadComputeProgram("../../shader/calForce.comp");
    GLuint shader_program = loadComputeProgram(_shader_path);
    glUseProgram(shader_program);
    //ssboにBindするBufferを用意
    out_y_force.setupBuffer(0);
    density_tgt.setupBuffer(1);
    density_amb.setupBuffer(2);
    templature.setupBuffer(3);
    
    GLfloat uniform_g0 = glGetUniformLocation(shader_program, "g0");
    GLfloat uniform_t_amb = glGetUniformLocation(shader_program, "t_amb");
    GLfloat uniform_beta = glGetUniformLocation(shader_program, "beta");

    glUniform1f(uniform_g0, G0);
    glUniform1f(uniform_t_amb, AMB_TEMPLATURE);
    glUniform1f(uniform_beta, BETA);
    glDispatchCompute(std::pow(2,10),1,1);
    out_y_force.sl_getSrcBufferSubData();
}
void Simulator::inputTXT(std::string &InputFileName)
{
    float max = 0;
    FILE *ifp = fopen(InputFileName.c_str(),"r");
    for(int k=0;k<TEXDEPTH;++k){
        for(int j=0;j<TEXHEIGHT;++j){
            for(int i=0;i<TEXWIDTH;++i){
                float value;
                fscanf(ifp, "%f", &value);
                density_tgt.src_texture[resequence3to1(i, TEXHEIGHT - j, k, TEXWIDTH, TEXHEIGHT, TEXDEPTH)] = value;
            }
        }
    }
    fclose(ifp);
}

void Simulator::init_density(float init_density_value)
{
    std::vector<unsigned int>init_index = get_init_index_list(TEXWIDTH,TEXHEIGHT,TEXDEPTH);
    for(int i=0;i<density_tgt.size;++i)density_tgt.src_texture[i] = 0.0f;
    for(int i=0;i<density_amb.size;++i)density_amb.src_texture[i] = init_density_value;
    for(int i=0;i<init_index.size();++i)density_tgt.src_texture[init_index[i]] = init_density_value;
}
void Simulator::init_templature(float init_templature_value)
{
    std::vector<unsigned int>init_index = get_init_index_list(TEXWIDTH,TEXHEIGHT,TEXDEPTH); 
    for(int i=0;i<templature.size;++i)templature.src_texture[i] = AMB_TEMPLATURE;
    for(int i=0;i<init_index.size();++i)templature.src_texture[init_index[i]] = init_templature_value;
}
void Simulator::oneloop()
{
    calForceEncoder.encode(y_force,density_tgt,density_tgt,templature);
}