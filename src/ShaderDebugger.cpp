#include "ShaderDebugger.hpp"
void normalize(unsigned int buffer_width, unsigned int buffer_height, unsigned int buffer_depth,float* src_buffer,float* dst_buffer)
{
    float max = 1.0f;
    unsigned int size = buffer_width * buffer_height * buffer_depth;
    for(int i=0;i<size;++i)
    {
        if(max < src_buffer[i])max = src_buffer[i];
    }
    for(int i=0;i<size;++i)
    {
       dst_buffer[i] = src_buffer[i]/max;
    }
}
int buffer_write_png(
    unsigned int buffer_width, unsigned int buffer_height, unsigned int buffer_depth, 
    unsigned int delta_z, float* src_buffer, std::string &bufferName)
{
    float* buffer = (float*)malloc(sizeof(float) * buffer_width * buffer_height * buffer_depth);
    normalize(buffer_width,buffer_height,buffer_depth,src_buffer,buffer);
    constexpr std::size_t width{ 1024 }, height{ 1024 }; //幅と高さ
    unsigned int magnificate_width = width / buffer_width;
    unsigned int magnificate_height = height / buffer_height;
    std::unique_ptr<RGBA[][width]> rgba(new(std::nothrow) RGBA[height][width]);
    if (!rgba) return -1;
    
    for (unsigned int z = 0; z < buffer_depth; z += delta_z){
        for (std::size_t row{}; row < height; ++row) {
            for (std::size_t col{}; col < width; ++col) {
                int bufnum = resequence3to1(col/magnificate_width, row/magnificate_height, z, buffer_width, buffer_height, buffer_depth);
                rgba[row][col].r = 255 - buffer[bufnum]*255; //赤
                rgba[row][col].g = 255;
                rgba[row][col].b = 255;
                rgba[row][col].a = 255; //不透過
            }
        }
        std::filesystem::create_directories(bufferName);
        std::string fileName = bufferName + "/z" + std::to_string(z) + ".png";
        stbi_write_png(fileName.c_str(), static_cast<int>(width), static_cast<int>(height), static_cast<int>(sizeof(RGBA)), rgba.get(), 0);
    }
    return 0;
}
