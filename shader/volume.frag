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
//    float transparency = texture(volume, t).r;
    float opacity = texture(volume, t).r;
//    vec3 smoke_color = vec3(1.0, 0.0, 0.0);
    vec3 smoke_color = vec3(1.0, 0.0, 0.0);
//    fc = vec4(smoke_color, transparency);
    fc = vec4(smoke_color, opacity*10);
//    fc = vec4(smoke_color, 1.0);
}

