#version 150 core
in vec4 position;
in vec4 color;
flat out vec4 vertex_color;
uniform mat4 modelview;
uniform mat4 projection;
void main()
{
    vertex_color = color;
    gl_Position = projection * modelview * position;
}


