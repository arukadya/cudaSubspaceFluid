#ifndef SliceRenderer_hpp
#define SliceRenderer_hpp

#define TEXWIDTH 64
#define TEXHEIGHT 64
#define TEXDEPTH 64
#define SLICENUM 64

#include "Eigen/Core"
#include <iostream>
#include <vector>
#include <string>
#include "Object.h"
#include "ComputeCommand.hpp"
#include "ShaderCommand.hpp"
#include "Matrix4x4.h"

struct SliceRenderer{
    static constexpr float sliceThickness = 2.0 / SLICENUM;
    float raySliceAngleCos;//射法投影だと画素ごとに違うはず
    int sliceDirectionID;
    
    GLfloat smokeColor;
    Eigen::Vector3f viewPoint;
    Eigen::Vector3f sliceDirection;
    GLuint cosHandle;
    
    SliceRenderer();
    void setViewPoint(GLfloat ex,GLfloat ey,GLfloat ez);
    void setSliceDirection(Eigen::Vector3f &tgt);
    void setSmokecolor(GLfloat color);
    float getRaySliceAngleCos(Eigen::Vector3f &tgt);
    GLuint makeVolume(float* densityTexture, GLfloat *smokeColor, Eigen::Vector3f &tgt);
    GLuint makeSlice();
    void rendering(Matrix4x4 &projection,Matrix4x4 &modelview,Matrix4x4 &sliceRot,float* densityTexture);
    void makeCosTexture();
};

#endif /* SliceRenderer_hpp */
