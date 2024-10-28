#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//視線直交スライスの回転行列
uniform mat4 sliceRot;

// モデルビュー変換行列
uniform mat4 mw;

// プロジェクション変換行列
uniform mat4 mp;

// スライスの間隔
uniform float spacing;

// [-0.5, 0.5] の正方形の頂点位置
layout (location = 0) in vec2 pv;

// スライスの頂点位置
out vec4 p;

// スライスのテクスチャ座標
out vec3 t;

void main()
{
    // スライスを gl_InstanceID でずらす
    p = vec4(pv, (float(gl_InstanceID) + 0.5) * spacing - 0.5, 1.0);
    t = p.xyz + vec4(0.5 , 0.5 , 0.5, 1.0).xyz;
//    vec4 p_world = vec4(pv, (float(gl_InstanceID) + 0.5) * spacing - 0.5, 1.0);
//    p = sliceRot * p_world * 1.732;
//    t =( sliceRot * p_world * 1.732 + vec4(0.5 , 0.5 , 0.5, 1.0)).xyz;
    // 頂点位置を視点座標系に移す
    p = mw * p;
    // モデルビュープロジェクション変換をしてからラスタライザに送る
    gl_Position = mp * p;
}

