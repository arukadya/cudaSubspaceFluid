#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ボリュームテクスチャのサンプラ
uniform sampler3D volume;

uniform vec3 smoke_color;

// テクスチャ座標
in vec3 t;
// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;

void main()
{
    vec3 fixt = vec3(t.x,t.y,t.z);
//    float opacity = texture(volume, fixt).r;
//    vec3 smoke_color = texture(volume, fixt);
//    vec3 smoke_color = vec3(0.0, 0.0, error*10);
    vec4 color = texture(volume, fixt);
    fc = vec4(color.rgb, color.a * 1);
//    fc = vec4(smoke_color, opacity*10);
}

