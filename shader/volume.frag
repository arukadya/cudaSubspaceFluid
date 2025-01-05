#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ボリュームテクスチャのサンプラ
uniform sampler3D volume;

// テクスチャ座標
in vec3 t;
// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;

void main()
{
    vec3 fixt = vec3(t.x,t.y,t.z);
    float opacity = texture(volume, fixt).r;
    vec3 smoke_color = vec3(1.0, 0.0, 0.0);
    fc = vec4(smoke_color, opacity*10);
}

