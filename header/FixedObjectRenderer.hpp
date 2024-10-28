//
//  fixedObjectRenderer.hpp
//  VolumeRendering
//
//  Created by 須之内俊樹 on 2024/09/19.
//

#ifndef FixedObjectRenderer_hpp
#define FixedObjectRenderer_hpp

#define TEXWIDTH 64
#define TEXHEIGHT 64
#define TEXDEPTH 64
#define SLICENUM 64

#include "Eigen/Core"
#include <iostream>
#include <vector>
#include <string>
#include "Object.h"
#include "Shape.h"
#include "SolidShapeIndex.h"
#include "SolidShape.h"
#include "ComputeCommand.hpp"
#include "ShaderCommand.hpp"
#include "Matrix4x4.h"
// 六面体の頂点の位置
constexpr Object::Vertex cubeVertex[] =
{
    { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f }, // (0)
    { -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.8f }, // (1)
    { -0.5f,  0.5f,  0.5f, 0.0f, 0.8f, 0.0f }, // (2)
    { -0.5f,  0.5f, -0.5f, 0.0f, 0.8f, 0.8f }, // (3)
    {  0.5f,  0.5f, -0.5f, 0.8f, 0.0f, 0.0f }, // (4)
    {  0.5f, -0.5f, -0.5f, 0.8f, 0.0f, 0.8f }, // (5)
    {  0.5f, -0.5f,  0.5f, 0.8f, 0.8f, 0.8f }, // (6)
    {  0.5f,  0.5f,  0.5f, 0.8f, 0.8f, 0.8f } // (7)
};

// 六面体の稜線の両端点のインデックス
constexpr GLuint wireCubeIndex[] =
{
    1, 0, // (a)
    2, 7, // (b)
    3, 0, // (c)
    4, 7, // (d)
    5, 0, // (e)
    6, 7, // (f)
    1, 2, // (g)
    2, 3, // (h)
    3, 4, // (i)
    4, 5, // (j)
    5, 6, // (k)
    6, 1 // (l)
};
// 六面体の面を塗りつぶす三角形の頂点のインデックス
constexpr GLuint solidCubeIndex[] =
{
    0, 1, 2, 3, 4, 5, // 左
    6, 7, 8, 9,10,11,//裏
    12, 13, 14, 15, 16, 17, // 下
    18, 19, 20, 21, 22, 23, // 右
    24, 25, 26, 27, 28, 29, // 上
    30,31,32,33,34,35 //前
};

// 面ごとに色を変えた六面体の頂点属性
constexpr Object::Vertex solidCubeVertex[] =
{
    // 左
    { -0.1f, -0.1f, -0.1f, 0.1f, 0.8f, 0.1f },
    { -0.1f, -0.1f, 0.1f, 0.1f, 0.8f, 0.1f },
    { -0.1f, 0.1f, 0.1f, 0.1f, 0.8f, 0.1f },
    { -0.1f, -0.1f, -0.1f, 0.1f, 0.8f, 0.1f },
    { -0.1f, 0.1f, 0.1f, 0.1f, 0.8f, 0.1f },
    { -0.1f, 0.1f, -0.1f, 0.1f, 0.8f, 0.1f },
    
    // 裏
    { 0.1f, -0.1f, -0.1f, 0.8f, 0.1f, 0.8f },
    { -0.1f, -0.1f, -0.1f, 0.8f, 0.1f, 0.8f },
    { -0.1f, 0.1f, -0.1f, 0.8f, 0.1f, 0.8f },
    { 0.1f, -0.1f, -0.1f, 0.8f, 0.1f, 0.8f },
    { -0.1f, 0.1f, -0.1f, 0.8f, 0.1f, 0.8f },
    { 0.1f, 0.1f, -0.1f, 0.8f, 0.1f, 0.8f },
    
    // 下
    { -0.1f, -0.1f, -0.1f, 0.1f, 0.8f, 0.8f },
    { 0.1f, -0.1f, -0.1f, 0.1f, 0.8f, 0.8f },
    { 0.1f, -0.1f, 0.1f, 0.1f, 0.8f, 0.8f },
    { -0.1f, -0.1f, -0.1f, 0.1f, 0.8f, 0.8f },
    { 0.1f, -0.1f, 0.1f, 0.1f, 0.8f, 0.8f },
    { -0.1f, -0.1f, 0.1f, 0.1f, 0.8f, 0.8f },
    // 右
    { 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.8f },
    { 0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.8f },
    { 0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.8f },
    { 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.8f },
    { 0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.8f },
    { 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.8f },
    
    // 上
    { -0.1f, 0.1f, -0.1f, 0.8f, 0.1f, 0.1f },
    { -0.1f, 0.1f, 0.1f, 0.8f, 0.1f, 0.1f },
    { 0.1f, 0.1f, 0.1f, 0.8f, 0.1f, 0.1f },
    { -0.1f, 0.1f, -0.1f, 0.8f, 0.1f, 0.1f },
    { 0.1f, 0.1f, 0.1f, 0.8f, 0.1f, 0.1f },
    { 0.1f, 0.1f, -0.1f, 0.8f, 0.1f, 0.1f },
    // 前
    { -0.1f, -0.1f, 0.1f, 0.8f, 0.8f, 0.1f },
    { 0.1f, -0.1f, 0.1f, 0.8f, 0.8f, 0.1f },
    { 0.1f, 0.1f, 0.1f, 0.8f, 0.8f, 0.1f },
    { -0.1f, -0.1f, 0.1f, 0.8f, 0.8f, 0.1f },
    { 0.1f, 0.1f, 0.1f, 0.8f, 0.8f, 0.1f },
    { -0.1f, 0.1f, 0.1f, 0.8f, 0.8f, 0.1f }
};
struct FixedObjectRenderer{
    Eigen::Vector3f viewPoint;
    Eigen::Vector3f sliceDirection;
    FixedObjectRenderer();
    void setViewPoint(GLfloat ex,GLfloat ey,GLfloat ez);
    void rendering(Matrix4x4 &projection,Matrix4x4 &modelview);
};
#endif /* FixedObjectRenderer_hpp */
