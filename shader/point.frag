#version 150 core
flat in vec4 vertex_color;
uniform vec4 light_vec;
out vec4 fragment;
float ambient = 0.1,diffuse = 0.9;

void main()
{
//    vec4 color;
//    float l = ambient + diffuse * max(dot(vertex_color , light_vec), 0.0);
//    int c = int(l) * 256;
//    if(c > 255)c = 255;
    fragment = vertex_color;
//    fragment = vec4(c,c,c,0);
}
