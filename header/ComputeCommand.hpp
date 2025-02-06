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
//Eqn(5)と//Eqn(7)の格子が違うのが問題なのでは
Eigen::VectorXf staggerd_to_colocate(Eigen::VectorXf &staggerd_velocity,unsigned int nx,unsigned int ny,unsigned int nz);
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
// struct Outputer
// {
//     std::string _folder_name;
//     std::string _file_name;
//     Outputer(std::string &folder_name,std::string &file_name)
//     {
//         _folder_name = folder_name;
//         _file_name = file_name;
//         std::filesystem::create_directories(folder_name);
//     }
//     void output_err(unsigned int id, float value)
//     {
//         std::cout << OutputFileName << std::endl;
//         FILE *ofp = fopen(OutputFileName.c_str(),"w");
//         for(int row=0;row<mat.rows();row++)
//         {
//             for(int col=0;col<mat.cols();col++)
//             {
//                 if(fprintf(ofp, "%f\n", mat(row,col)));
//             }
//         }
//         fclose(ofp);
//     }
//     void output_times(unsigned int id, float value)
//     {

//     }
// };
int inputParamator(std::string InputFileName,float &dx,float &dt,float &beta, float &nu,
    unsigned int &texwidth,unsigned int &texheight,unsigned int &texdepth,unsigned int &slice_num,
    unsigned int &flame_num,unsigned int &snap_num,unsigned int &discard_flame,
    float &s_threshold,float &c_threshold,
    unsigned int &devide_num);

void outputMatrix(std::string OutputFileName, Eigen::MatrixXf &mat);
void inputMatrix(std::string InputFileName, Eigen::MatrixXf &mat);
#endif /* ComputeCommand_hpp */
