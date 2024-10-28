//
//  ComputeCommand.cpp
//  VolumeRendering
//
//  Created by 須之内俊樹 on 2024/07/15.
//

#include "ComputeCommand.hpp"

unsigned int resequence3to1(unsigned int i,unsigned int j,unsigned int k,unsigned int Ni,unsigned int Nj,unsigned int Nk)
{
    return k * Ni*Nj + j * Ni + i;
}
void Timer::startWithMessage(const char* s)
{
    startTime = std::chrono::system_clock::now();
    str = s;
}
double Timer::end()
{
    endTime = std::chrono::system_clock::now();
    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
    std::cout << str << ":" << time << "ms" << std::endl;
    return time;
    //std::cout << std::endl;
}

std::vector<unsigned int>get_init_index_list(unsigned int Ni,unsigned int Nj,unsigned int Nk)
{
    std::vector<unsigned int>ret;
    std::vector<std::vector<unsigned int>>init_index_list;
    for(unsigned int i=0;i<Ni;++i)
    {
        for(unsigned int j=0;j<Nj;++j)
        {
            for(unsigned int k=0;k<Nk;++k)
            {
                if
                (
                    Ni / 2 - Ni / 16 < i && i < Ni / 2 + Ni / 16 &&
                    j == 0 &&
                    Nk / 2 - Nk / 16 < k && k < Nk / 2 + Nk / 16 
                )init_index_list.push_back({i,j,k});
            }
        }
    }
    for(unsigned int i=0;i<init_index_list.size();++i)
    {
        unsigned int x = init_index_list[i][0];
        unsigned int y = init_index_list[i][1];
        unsigned int z = init_index_list[i][2];
        // std::cout << x << "," << y << "," << z << std::endl;
        ret.push_back(resequence3to1(x,y,z,Ni,Nj,Nk));
    }
    return ret;
}