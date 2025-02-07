//
//  ComputeCommand.cpp
//  VolumeRendering
//
//  Created by 須之内俊樹 on 2024/07/15.
//

#include "ComputeCommand.hpp"

Eigen::VectorXf staggerd_to_colocate(Eigen::VectorXf &staggerd_velocity,unsigned int nx,unsigned int ny,unsigned int nz)
{
    unsigned int staggerd_size = (nx+1)*ny*nz;
    unsigned int colocate_size = nx*ny*nz;
    Eigen::VectorXf colocate_vector(colocate_size);
    //x
    for(unsigned int i = 0; i < nx; ++i)
    {
        for(unsigned int j = 0; j < ny; ++j)
        {
            for(unsigned int k = 0; k < nz; ++k)
            {
                unsigned int colocate_id = resequence3to1(i,j,k,nx,ny,nz);
                unsigned int staggerd_pre = resequence3to1(i,j,k,nx+1,ny,nz);
                unsigned int staggerd_post = resequence3to1(i+1,j,k,nx+1,ny,nz);
                colocate_vector(colocate_id) = (staggerd_velocity(staggerd_pre) + staggerd_velocity(staggerd_post))/2;
            }
        }
    }
    //y
    for(unsigned int i = 0; i < nx; ++i)
    {
        for(unsigned int j = 0; j < ny; ++j)
        {
            for(unsigned int k = 0; k < nz; ++k)
            {
                unsigned int colocate_id = colocate_size + resequence3to1(i,j,k,nx,ny,nz);
                unsigned int staggerd_pre = staggerd_size + resequence3to1(i,j,k,nx,ny+1,nz);
                unsigned int staggerd_post = staggerd_size + resequence3to1(i,j+1,k,nx,ny+1,nz);
                colocate_vector(colocate_id) = (staggerd_velocity(staggerd_pre) + staggerd_velocity(staggerd_post))/2;
            }
        }
    }
    //z
    for(unsigned int i = 0; i < nx; ++i)
    {
        for(unsigned int j = 0; j < ny; ++j)
        {
            for(unsigned int k = 0; k < nz; ++k)
            {
                unsigned int colocate_id = 2*colocate_size + resequence3to1(i,j,k,nx,ny,nz);
                unsigned int staggerd_pre = 2*staggerd_size + resequence3to1(i,j,k,nx,ny,nz+1);
                unsigned int staggerd_post =2*staggerd_size + resequence3to1(i,j,k+1,nx,ny,nz+1);
                colocate_vector(colocate_id) = (staggerd_velocity(staggerd_pre) + staggerd_velocity(staggerd_post))/2;
            }
        }
    }
    return colocate_vector;
}

unsigned int resequence3to1(unsigned int i,unsigned int j,unsigned int k,unsigned int Ni,unsigned int Nj,unsigned int Nk)
{
    unsigned int value = k * Ni*Nj + j * Ni + i;
    // if(value > Ni * Nj * Nk)std::cout << "reseq overflow" <<std::endl;
    return value;
}

void resequence1to3(unsigned int val,unsigned int &i,unsigned int &j,unsigned int &k,unsigned int Ni,unsigned int Nj,unsigned int Nk)
{
    k = val / (Ni*Nj);
    val = val % (Ni*Nj);
    j = val / Ni;
    val = val % Ni;
    i = val;
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
    // unsigned int range_x = 4;
    // unsigned int range_y = 4;
    // unsigned int range_z = 4;
    unsigned int range_x = Ni/16;
    unsigned int range_y = Nj/8;
    unsigned int range_z = Nk/16;
    for(unsigned int i=0;i<Ni;++i)
    {
        for(unsigned int j=0;j<Nj;++j)
        {
            for(unsigned int k=0;k<Nk;++k)
            {
                if
                (
                    Ni / 2 - range_x < i && i < Ni / 2 + range_x &&
                    j > Nj - range_y &&
                    // 3 * Nj / 4 - range_y < j && j < 3 * Nj / 4 + range_y &&
                    // j > Nj - 3 &&
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
int inputParamator(std::string InputFileName,float &dx,float &dt,float &beta, float &epsilon, float &nu,
    unsigned int &texwidth,unsigned int &texheight,unsigned int &texdepth,unsigned int &slice_num,
    unsigned int &flame_num,unsigned int &snap_num,unsigned int &discard_flame ,
    float &s_threshold,float &c_threshold,
    unsigned int &devide_num){
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
        else if(word == "epsilon")
        {
            while(getline(ss_nums,word,' ')){
                epsilon = std::stof(word);
            };
        }
        else if(word == "nu")
        {
            while(getline(ss_nums,word,' ')){
                nu = std::stof(word);
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
        else if(word == "singularity_threshold")
        {
            while(getline(ss_nums,word,' ')){
                s_threshold = std::stof(word);
            };
        }
        else if(word == "cubature_threshold")
        {
            while(getline(ss_nums,word,' ')){
                c_threshold = std::stof(word);
            };
        }
        else if(word == "discard_flame")
        {
            while(getline(ss_nums,word,' ')){
                discard_flame = std::stof(word);
            };
        }
        else if(word == "devide_num")
        {
            while(getline(ss_nums,word,' ')){
                devide_num = std::stof(word);
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
    if(!ofp)std::cout << "failed open file" << std::endl;
    if(fprintf(ofp, "%d\n%d\n", mat.rows(),mat.cols()));
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
    if(!ifp)std::cout << "failed open file" << std::endl;
    std::cout << InputFileName << std::endl;
    int rows;int cols;
    if(fscanf(ifp, "%d", &rows));
    std::cout << rows << std::endl;
    if(fscanf(ifp, "%d", &cols));
    // std::cout << cols << std::endl;
    std::cout << rows << "," << cols << std::endl;
    mat = Eigen::MatrixXf(rows,cols);
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