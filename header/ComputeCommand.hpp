//
//  ComputeCommand.hpp
//  VolumeRendering
//
//  Created by 須之内俊樹 on 2024/07/15.
//

#ifndef ComputeCommand_hpp
#define ComputeCommand_hpp

#include <iostream>
#include <chrono>
#include <vector>
unsigned int resequence3to1(unsigned int i,unsigned int j,unsigned int k,unsigned int Ni,unsigned int Nj,unsigned int Nk);
std::vector<unsigned int>get_init_index_list(unsigned int Ni,unsigned int Nj,unsigned int Nk);
struct Timer{
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    const char* str;
    void startWithMessage(const char* s);
    double end();
};
#endif /* ComputeCommand_hpp */
