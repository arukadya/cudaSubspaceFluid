//
//  fixedObjectRenderer.cpp
//  VolumeRendering
//
//  Created by 須之内俊樹 on 2024/09/19.
//

#include "FixedObjectRenderer.hpp"
FixedObjectRenderer::FixedObjectRenderer()
{
}
void FixedObjectRenderer::rendering(Matrix4x4 &projection,Matrix4x4 &modelview)
{
    // プログラムオブジェクトを作成する
    const GLuint surfaceProgram(loadVertFragProgram("../shader/point.vert", "../shader/point.frag"));
    
    std::unique_ptr<const Shape>shape_wire(new ShapeIndex(3,8,cubeVertex,24,wireCubeIndex));
    std::unique_ptr<const Shape>shape_obstacle(new SolidShapeIndex(3,36,solidObstacleVertex,36,solidCubeIndex));
    //uniform変数の場所を取得する
    const GLint surface_modelviewLoc(glGetUniformLocation(surfaceProgram, "modelview"));
    const GLint surface_projectionLoc(glGetUniformLocation(surfaceProgram, "projection"));
    const GLint surface_light_vecLoc(glGetUniformLocation(surfaceProgram, "light_vec"));
    //ウィンドウを消去する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //デプスバッファを有効にする
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    //背景カリングを有効にする
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    //シェーダプログラムの利用開始
    //描画処理
    glUseProgram(surfaceProgram);
    
    glUniformMatrix4fv(surface_projectionLoc, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(surface_modelviewLoc, 1, GL_FALSE, modelview.data());
    glUniform4f(surface_light_vecLoc, viewPoint.x(), viewPoint.y(), viewPoint.z(), 0.0f);
    shape_wire->draw();
    shape_obstacle->draw();
}
