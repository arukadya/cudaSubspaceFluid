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
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "Eigen/Core"
unsigned int resequence3to1(unsigned int i,unsigned int j,unsigned int k,unsigned int Ni,unsigned int Nj,unsigned int Nk);
void resequence1to3(unsigned int val,unsigned int &i,unsigned int &j,unsigned int &k,unsigned int Ni,unsigned int Nj,unsigned int Nk);
std::vector<unsigned int>get_init_index_list(unsigned int Ni,unsigned int Nj,unsigned int Nk);
struct Timer{
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    const char* str;
    void startWithMessage(const char* s);
    double end();
};
int inputParamator(std::string InputFileName,float &dx,float &dt,float &beta, float &nu,
    unsigned int &texwidth,unsigned int &texheight,unsigned int &texdepth,unsigned int &slice_num,
    unsigned int &flame_num,unsigned int &snap_num,unsigned int &discard_flame,float &threshold);

void outputMatrix(std::string OutputFileName, Eigen::MatrixXf &mat);
void inputMatrix(std::string InputFileName, Eigen::MatrixXf &mat);
#endif /* ComputeCommand_hpp */
