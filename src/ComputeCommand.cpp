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
    // unsigned int range_x = 1;
    // unsigned int range_y = 1;
    // unsigned int range_z = 1;
    unsigned int range_x = Ni/32;
    // unsigned int range_y = Nj/16;
    unsigned int range_z = Nk/32;
    for(unsigned int i=0;i<Ni;++i)
    {
        for(unsigned int j=0;j<Nj;++j)
        {
            for(unsigned int k=0;k<Nk;++k)
            {
                if
                (
                    Ni / 2 - range_x < i && i < Ni / 2 + range_x &&
                    // j < 3 &&
                    j > Nj - 3 &&
                    Nk / 2 - range_z < k && k < Nk / 2 + range_z
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
int inputParamator(std::string InputFileName,float &dx,float &dt,float &beta,
    unsigned int &texwidth,unsigned int &texheight,unsigned int &texdepth,unsigned int &slice_num,
    unsigned int &flame_num,unsigned int &snap_num,unsigned int &discard_flame ,float &threshold){
    std::ifstream Inputfile(InputFileName);
    if (!Inputfile.is_open()) {
        std::cerr << "Could not open the file - '"
             << InputFileName << "'" << std::endl;
        return EXIT_FAILURE;
    }
    std::string line;
    std::string word;
    
    while(std::getline(Inputfile,line)){
        std::stringstream ss_nums{line};
        getline(ss_nums,word,' ');
        if(word[0] == '#' || word[0] == '\n')continue;
        else if(word == "dx")
        {
            while(getline(ss_nums,word,' ')){
                dx = std::stof(word);
            };
        }
        else if(word == "dt")
        {
            while(getline(ss_nums,word,' ')){
                dt = std::stof(word);
            };
        }
        else if(word == "beta")
        {
            while(getline(ss_nums,word,' ')){
                beta = std::stof(word);
            };
        }
        else if(word == "texwidth")
        {
            while(getline(ss_nums,word,' ')){
                texwidth = std::stoi(word);
            };
        }
        else if(word == "texdepth")
        {
            while(getline(ss_nums,word,' ')){
                texdepth = std::stoi(word);
            };
        }
        else if(word == "texheight")
        {
            while(getline(ss_nums,word,' ')){
                texheight = std::stoi(word);
            };
        }
        else if(word == "slice_num")
        {
            while(getline(ss_nums,word,' ')){
                slice_num = std::stoi(word);
            };
        }
        else if(word == "flame_num")
        {
            while(getline(ss_nums,word,' ')){
                flame_num = std::stoi(word);
            };
        }
        else if(word == "snap_num")
        {
            while(getline(ss_nums,word,' ')){
                snap_num = std::stoi(word);
            };
        }
        else if(word == "threshold")
        {
            while(getline(ss_nums,word,' ')){
                threshold = std::stof(word);
            };
        }
        else if(word == "discard_flame")
        {
            while(getline(ss_nums,word,' ')){
                discard_flame = std::stof(word);
            };
        }
    };
    // std::cout << "dx,dt,beta = " << dx << "," << dt << "," << beta << std::endl;
    // std::cout << "width,height,depth,slice = " << texwidth << "," << texheight << "," << texdepth << "," << slice_num << std::endl;
    // std::cout << "flame_num, snap_num, threshold = " << flame_num << "," << snap_num <<"," << threshold << std::endl;
    Inputfile.close();
    return EXIT_SUCCESS;
}

void outputMatrix(std::string OutputFileName, Eigen::MatrixXf &mat)
{
    std::cout << OutputFileName << std::endl;
    FILE *ofp = fopen(OutputFileName.c_str(),"w");
    for(int row=0;row<mat.rows();row++)
    {
        for(int col=0;col<mat.cols();col++)
        {
            if(fprintf(ofp, "%f\n", mat(row,col)));
        }
    }
    fclose(ofp);
}

void inputMatrix(std::string InputFileName, Eigen::MatrixXf &mat)
{
    FILE *ifp = fopen(InputFileName.c_str(),"r");
    std::cout << InputFileName << std::endl;
    for(int row=0;row<mat.rows();row++)
    {
        for(int col=0;col<mat.cols();col++)
        {
            float value;
            if(fscanf(ifp, "%f", &value));
            mat(row,col) = value;
        }
    }
    fclose(ifp);
}