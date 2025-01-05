//
//  SliceRenderer.cpp
//  OpenGLTest
//
//  Created by 須之内俊樹 on 2024/06/13.
//

#include "SliceRenderer.hpp"
#include <stdio.h>
void getBackmostSlice(Eigen::Matrix3f &ldVertices,
                      Eigen::Matrix3f &ruVertices,
                      int sliceDirectionID)
{
    if(sliceDirectionID == 0)
    {
        ldVertices <<
        1.0f, -1.0f, -1.0f, // 5
        1.0f, -1.0f,  1.0f, // 6
        1.0f,  1.0f,  1.0f; // 7
        ruVertices <<
        1.0f,  1.0f, -1.0f, // 4
        1.0f, -1.0f, -1.0f, // 5
        1.0f,  1.0f,  1.0f; // 7
    }
    
    if(sliceDirectionID == 1)
    {
        ldVertices <<
        -1.0f, -1.0f, -1.0f, // 0
        -1.0f, -1.0f,  1.0f, // 1
        -1.0f,  1.0f,  1.0f; // 2
        
        ruVertices <<
        -1.0f,  1.0f, -1.0f, // 3
        -1.0f, -1.0f, -1.0f, // 0
        -1.0f, -1.0f,  1.0f; // 1
    }
    if(sliceDirectionID == 2)
    {
        ldVertices <<
        -1.0f,  1.0f, -1.0f, // 3
        1.0f,  1.0f,  1.0f,  // 7
        -1.0f,  1.0f,  1.0f; // 2
        
        ruVertices <<
        1.0f,  1.0f,  1.0f,  // 7
        -1.0f,  1.0f, -1.0f, // 3
        1.0f,  1.0f, -1.0f;  // 4
    }
    if(sliceDirectionID == 3)
    {
        ldVertices <<
        -1.0f, -1.0f, -1.0f, // 0
        1.0f, -1.0f,  1.0f,  // 6
        -1.0f, -1.0f,  1.0f; // 1
        
        ruVertices <<
        1.0f, -1.0f,  1.0f,  // 6
        -1.0f, -1.0f, -1.0f, // 0
        1.0f, -1.0f, -1.0f;  // 5
    }
    if(sliceDirectionID == 4)
    {
        ldVertices <<
        1.0f, -1.0f,  1.0f,  // 6
        -1.0f,  1.0f,  1.0f, // 2
        -1.0f, -1.0f,  1.0f; // 1
        
        ruVertices <<
        -1.0f,  1.0f,  1.0f, // 2
        1.0f, -1.0f,  1.0f,  // 6
        1.0f,  1.0f,  1.0f;  // 7
    }
    if(sliceDirectionID == 5)
    {
        ldVertices <<
        1.0f, -1.0f, -1.0f,  // 5
        -1.0f,  1.0f, -1.0f, // 3
        -1.0f, -1.0f, -1.0f; // 0
        
        ruVertices <<
        -1.0f,  1.0f, -1.0f, // 3
        1.0f, -1.0f, -1.0f,  // 5
        1.0f,  1.0f, -1.0f;  // 4
    }
}

// SliceRenderer::SliceRenderer()
// {
// }
GLuint SliceRenderer::makeSlice()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    static const GLfloat p[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
    glBufferData(GL_ARRAY_BUFFER, sizeof p, p, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    return vao;
}

GLuint SliceRenderer::makeVolume(float* densityTexture, GLfloat *smokeColor,Eigen::Vector3f &tgt)
{
//    std::vector<GLubyte>volume;
    std::vector<GLfloat>volume;
    raySliceAngleCos = getRaySliceAngleCos(tgt);
    float marchingLength = sliceThickness / raySliceAngleCos;
    for(unsigned int k=0;k<_texdepth;++k){
        for(unsigned int j=0;j<_texheight;++j){
            for(unsigned int i=0;i<_texwidth;++i){
                float transparency = exp( -1.0 * densityTexture[resequence3to1(i, j, k, _texwidth, _texheight, _texdepth)] * marchingLength);
                // float transparency10 = exp( -1.0 * 10 * marchingLength);
                // float transparency255 = exp( -1.0 * 255 * marchingLength);
//                std::cout << densityTexture[resequence3to1(i, j, k, texwidth, _texheight, _texdepth)] << std::endl;
                float opacity = 1 - transparency;
//                else volume.push_back(opacity);
                volume.push_back(opacity);
//                std::cout << (float)volume[volume.size()-1] << "," << opacity10 << "," << opacity255 << std::endl;
            }
        }
    }
    // テクスチャオブジェクトを作成して結合する
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_3D, tex);
    
    //GL_RGBA8,GL_RGBA:Each element contains all four components. Each component is clamped to the range [0,1].
    //テクスチャを割り当てる
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, _texwidth, _texheight, _texdepth, 0,
                 GL_RED, GL_FLOAT, &volume[0]);
    //拡大・補間方法の設定
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    //クランプ方法の設定
    //テクスチャ座標s,t,r軸について，テクスチャを繰り返すかどうか
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    
    static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//    static const GLfloat black[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, black);
    return tex;
}

void SliceRenderer::setViewPoint(GLfloat ex,GLfloat ey,GLfloat ez)
{
    viewPoint = {ex,ey,ez};
}
void SliceRenderer::setSmokecolor(GLfloat color)
{
    smokeColor = color;
}
void SliceRenderer::setSliceDirection(Eigen::Vector3f &tgt)
{
    Eigen::Vector3f viewDirection = tgt - viewPoint;
    std::vector<Eigen::Vector3f> directionVectors;
    directionVectors.resize(6);
    directionVectors[0] = {1.0,0.0,0.0};
    directionVectors[1] = {-1.0,0.0,0.0};
    directionVectors[2] = {0.0,1.0,0.0};
    directionVectors[3] = {0.0,-1.0,0.0};
    directionVectors[4] = {0.0,0.0,1.0};
    directionVectors[5] = {0.0,0.0,-1.0};
    float min = viewDirection.norm() * 2;
    for(int i=0;i<6;++i)
    {
        if(min > viewDirection.dot(directionVectors[i]) )
        {
            min = viewDirection.dot(directionVectors[i]);
                                    sliceDirection = directionVectors[i];
            sliceDirectionID = i;
        }
    }
}

float SliceRenderer::getRaySliceAngleCos(Eigen::Vector3f &tgt)
{
    Eigen::Vector3f viewDirection = tgt - viewPoint;
    return (-viewDirection).dot(sliceDirection) / ( viewDirection.norm() * sliceDirection.norm());
}


GLuint makeSlice()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    static const GLfloat p[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
//    static const GLfloat p[] = { -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f };
    glBufferData(GL_ARRAY_BUFFER, sizeof p, p, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    return vao;
}
void SliceRenderer::rendering(Matrix4x4 &projection,Matrix4x4 &modelview,Matrix4x4 &sliceRot,float* densityTexture)
{
    const GLuint volumeProgram(loadVertFragProgram("../shader/volume.vert", "../shader/volume.frag"));
    const GLint mwLoc(glGetUniformLocation(volumeProgram, "mw"));
    const GLint mpLoc(glGetUniformLocation(volumeProgram, "mp"));
    const GLint spacingLoc(glGetUniformLocation(volumeProgram, "spacing"));
    const GLint volume_light_vecLoc(glGetUniformLocation(volumeProgram, "light_vec"));
    const GLint sliceRot_Loc(glGetUniformLocation(volumeProgram, "sliceRot"));
    const GLint volumeLoc(glGetUniformLocation(volumeProgram, "volume"));

    glUseProgram(volumeProgram);
    glUniform1f(spacingLoc, 1.0f / static_cast<GLfloat>(_slice_num - 1));
    glUniformMatrix4fv(mpLoc, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(mwLoc, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(sliceRot_Loc, 1, GL_FALSE, sliceRot.data());
    glUniform4f(volume_light_vecLoc, 3.0f, 4.0f, 5.0f, 0.0f);
    Timer timer;
//    timer.startWithMessage("rendering");
    //スライスの図形データを作成
    const GLuint slice(makeSlice());
    //ボリュームテクスチャを設定
    Eigen::Vector3f tgt = {0.0f,0.0f,0.0f};
    GLfloat smokeColor[3] = {0.0f,0.0f,1.0f};
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_BLEND);
    glUniform1f(volumeLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, makeVolume(densityTexture, smokeColor,tgt));
    glBindVertexArray(slice);
    //複製する描画方法．第四引数がインスタンスの数
    
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, _slice_num);
//    timer.end();
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_BLEND);
}

void SliceRenderer::makeCosTexture()
{
    const GLuint program(loadComputeProgram("Shader/cos.comp"));
    const GLint eye_posLoc(glGetUniformLocation(program, "eye_pos"));
    // const GLint volumeLoc(glGetUniformLocation(program, "volume"));

    glUseProgram(program);
    glUniform4f(eye_posLoc, 3.0f, 4.0f, 5.0f, 0.0f);

//    GLuint shader_program = createComputeShaderProgram(compute_shader_source);

    // create buffer
//    GLuint uniform_element_size = glGetUniformLocation(shader_program, "element_size");
    uint32_t num = _texdepth * _texheight * _texwidth;
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, num * sizeof(float), nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

//    glUniform1ui(uniform_element_size, num);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

    glDispatchCompute(num / 256 + 1, 1, 1);

    glUseProgram(0);

    std::vector<float> data(num);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, num * sizeof(float), data.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    for (auto v : data) { std::cout << v << '\n'; }

    glDeleteBuffers(1, &ssbo);
}
