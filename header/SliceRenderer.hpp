#ifndef SliceRenderer_hpp
#define SliceRenderer_hpp

#include "Eigen/Core"
#include <iostream>
#include <vector>
#include <string>
#include "Object.h"
#include "ComputeCommand.hpp"
#include "ShaderCommand.hpp"
#include "Matrix4x4.h"
struct SliceRenderer{
    const unsigned int _texwidth; const unsigned int _texheight; const unsigned int _texdepth;
    const unsigned int _slice_num;
    float sliceThickness = 2.0 / _slice_num;
    float raySliceAngleCos;//射法投影だと画素ごとに違うはず
    int sliceDirectionID;
    
    GLfloat smokeColor;
    Eigen::Vector3f viewPoint;
    Eigen::Vector3f sliceDirection;
    GLuint cosHandle;
    
    // SliceRenderer();
    SliceRenderer(unsigned int texwidth, unsigned int texheight, unsigned int texdepth, unsigned int slice_num)
     : _texwidth(texwidth),_texheight(texheight),_texdepth(texdepth),_slice_num(slice_num){}
    void setViewPoint(GLfloat ex,GLfloat ey,GLfloat ez);
    void setSliceDirection(Eigen::Vector3f &tgt);
    void setSmokecolor(GLfloat color);
    float getRaySliceAngleCos(Eigen::Vector3f &tgt);
    GLuint makeVolume(float* densityTexture,float* originTexture, GLfloat *smokeColor, Eigen::Vector3f &tgt);
    GLuint makeSlice();
    void rendering(Matrix4x4 &projection,Matrix4x4 &modelview,Matrix4x4 &sliceRot,float* densityTexture,float* originTexture);
    void makeCosTexture();
};

#endif /* SliceRenderer_hpp */
