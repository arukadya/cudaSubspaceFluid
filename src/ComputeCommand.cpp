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
    unsigned int &devide_num,unsigned int &situation){
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
        else if(word == "situation")
        {
            while(getline(ss_nums,word,' ')){
                situation = std::stof(word);
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

// 3次元流速データの構造体
struct Velocity {
    float x, y, z;
    float color;
};
using namespace std;

void generateVelocityData(vector<vector<vector<Velocity>>> &data,Eigen::VectorXf &velocity,int nx, int ny, int nz) {
    int size = nx * ny * nz;
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            for (int k = 0; k < nz; ++k) {
                int x_pre_id  = resequence3to1(i,j,k,nx,ny,nz);
                int x_post_id = resequence3to1(i+1,j,k,nx,ny,nz);
                int y_pre_id  = size + resequence3to1(i,j,k,nx,ny,nz);    
                int y_post_id = size + resequence3to1(i,j+1,k,nx,ny,nz);
                int z_pre_id  = 2*size + resequence3to1(i,j,k,nx,ny,nz);
                int z_post_id = 2*size + resequence3to1(i,j,k+1,nx,ny,nz);
                Eigen::Vector3f vel = {
                    ( velocity(x_pre_id) + velocity(x_post_id) )/2,
                    ( velocity(y_pre_id) + velocity(y_post_id) )/2,
                    ( velocity(z_pre_id) + velocity(z_post_id) )/2};
                    // vel.normalize();
                    // vel = vel * 5;
                data[i][j][k] = {vel.x(), vel.y(),vel.z(),0};
            }
        }
    }
}

void cal_color_and_normalize(vector<vector<vector<Velocity>>> &simulate,vector<vector<vector<Velocity>>> &origin,int nx, int ny, int nz)
{
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            for (int k = 0; k < nz; ++k)
            {
                Eigen::Vector3f error = 
                {
                simulate[i][j][k].x - origin[i][j][k].x,
                simulate[i][j][k].y - origin[i][j][k].y,
                simulate[i][j][k].z - origin[i][j][k].z
                };
                Eigen::Vector3f vel = {
                simulate[i][j][k].x,
                simulate[i][j][k].y,
                simulate[i][j][k].z
                };
                vel.normalize();
                vel = vel * 5;
                simulate[i][j][k] = {vel.x(), vel.y(),vel.z(),error.norm()};
            }
        }
    }
}
// 断面データを適度にサンプリングして出力
void outputSliceData(const vector<vector<vector<Velocity>>> &data, int nx, int ny, int nz, int slice, const string &filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open file!" << endl;
        exit(1);
    }
    
    for (int i = 0; i < nx; i += nx / 32) {
        for (int j = 0; j < ny; j += ny / 32) {
            if(i == 0 && j == 0)file << i << " " << j << " " << data[i][j][slice].x << " " << -data[i][j][slice].y << " " << 200.0<< endl;
            else file << i << " " << j << " " << data[i][j][slice].x << " " << -data[i][j][slice].y << " " << data[i][j][slice].color<< endl;
        }
        file << endl;  // gnuplotの"splot"用の空行
    }
    file.close();
}

// gnuplotで描画
void plotWithGnuplot(const string &filename,std::string &plot_fileName) {
    ofstream gnuplotScript("plot_script.gp");
    gnuplotScript << "set terminal pngcairo enhanced\n";
    // gnuplotScript << "set terminal pdfcairo size 8in,6in\n";
    gnuplotScript << "set output '" << plot_fileName <<"'\n";
    gnuplotScript << "set xlabel 'X'\n";
    gnuplotScript << "set ylabel 'Y'\n";
    gnuplotScript << "set title 'Velocity Field Slice'\n";
    // カラーマップを定義
    gnuplotScript << "set palette defined (0 'blue', 1 'green', 2 'yellow', 3 'red')\n";
    // gnuplotScript << "set palette defined (0 'red', 1 'yellow', 2 'green', 3 'blue')\n";
    // スタイルを統一
    gnuplotScript << "set style line 1 lt 1 lw 2 lc rgb 'blue'\n";
    gnuplotScript << "set style line 2 lt 1 lw 2 lc rgb 'green'\n";
    gnuplotScript << "set style line 3 lt 1 lw 2 lc rgb 'yellow'\n";
    gnuplotScript << "set style line 4 lt 1 lw 2 lc rgb 'red'\n";

    gnuplotScript << "plot '" << filename << "' using 1:2:3:4:5 with vectors lc palette title 'Velocity'\n";
    // gnuplotScript << "plot '" << filename << "' using 1:2:3:4:5 with vectors linestyle 1 title 'Velocity'\n";
    gnuplotScript.close();
    system("gnuplot plot_script.gp");
}
// void plotWithGnuplot(const string &filename, std::string &plot_fileName) {
//     ofstream gnuplotScript("plot_script.gp");
//     gnuplotScript << "set terminal pngcairo enhanced\n";
//     gnuplotScript << "set output '" << plot_fileName << "'\n";
//     gnuplotScript << "set xlabel 'X'\n";
//     gnuplotScript << "set ylabel 'Y'\n";
//     gnuplotScript << "set title 'Velocity Field Slice'\n";

//     // カラーマップの定義
//     gnuplotScript << "set palette defined (0 'blue', 1 'green', 2 'yellow', 3 'red')\n";

//     // 凡例を統一するためのダミーデータ（矢印としてプロット）
//     gnuplotScript << "set key outside\n";
//     gnuplotScript << "plot '-' using 1:2:3:4 with vectors lc rgb 'blue' title 'Low Velocity', \\\n";
//     gnuplotScript << "     '-' using 1:2:3:4 with vectors lc rgb 'green' title 'Medium Velocity', \\\n";
//     gnuplotScript << "     '-' using 1:2:3:4 with vectors lc rgb 'yellow' title 'High Velocity', \\\n";
//     gnuplotScript << "     '-' using 1:2:3:4 with vectors lc rgb 'red' title 'Max Velocity'\n";

//     // ダミーデータ（凡例のみ表示するための矢印）
//     gnuplotScript << "0 0 0.1 0.1\ne\n"; // Low Velocity
//     gnuplotScript << "0 0 0.1 0.1\ne\n"; // Medium Velocity
//     gnuplotScript << "0 0 0.1 0.1\ne\n"; // High Velocity
//     gnuplotScript << "0 0 0.1 0.1\ne\n"; // Max Velocity

//     // 実際のデータをプロット（lc variable で色をデータから指定）
//     gnuplotScript << "replot '" << filename << "' using 1:2:3:4:5 with vectors lc variable title ''\n";

//     gnuplotScript.close();
//     system("gnuplot plot_script.gp");
// }

void plotVelocity(unsigned int nx,unsigned int ny,unsigned int nz,Eigen::VectorXf &velocity,Eigen::VectorXf &origin,std::string &plot_fileName)
{
    // int nx = 20, ny = 20, nz = 10;  // グリッドサイズ
    int slice = nz / 2;  // z = 中央の断面を取得
    vector<vector<vector<Velocity>>> velocityData(nx, vector<vector<Velocity>>(ny, vector<Velocity>(nz)));
    vector<vector<vector<Velocity>>> originData(nx, vector<vector<Velocity>>(ny, vector<Velocity>(nz)));
    generateVelocityData(velocityData,velocity, nx, ny, nz);
    generateVelocityData(originData,origin, nx, ny, nz);
    cal_color_and_normalize(velocityData, originData, nx, ny, nz);
    string dataFile = "velocity_slice.dat";
    outputSliceData(velocityData,nx, ny, nz, slice, dataFile);
    plotWithGnuplot(dataFile,plot_fileName);
    cout << "Plot generated: " << plot_fileName << endl;
}

void outputVTK(std::string OutputFileName,float* val,int nx,int ny,int nz,double dx)
{
    std::vector<float> origin = {0,0,0};
    std::ofstream writing_file;
    std::cout << OutputFileName << std::endl;
    writing_file.open(OutputFileName, std::ios::out);
    std::string writing_text ="# vtk DataFile Version 2.0\nIsosurface\nASCII\nDATASET STRUCTURED_POINTS\n";
    writing_file << writing_text << std::endl;
    writing_file << "DIMENSIONS " << nx <<" "<< ny <<" "<< nz << std::endl;
    writing_file << "ORIGIN " << origin[0] <<" "<< origin[1] <<" "<< origin[2] << std::endl;
    writing_file << "SPACING " << dx <<" "<< dx <<" "<< dx << std::endl;
    writing_file << "POINT_DATA " << nx*ny*nz << std::endl;
    writing_file << "SCALARS value float 1" << std::endl;
    writing_file << "LOOKUP_TABLE default" << std::endl;
    for(int k=0;k<nz;k++){
        for(int j=0;j<ny;j++){
            for(int i=0;i<nx;i++){
                if(abs(val[resequence3to1(i,j,k,nx,ny,nz)]) < 1e-10)writing_file << 0 << std::endl;
                else writing_file << val[resequence3to1(i,j,k,nx,ny,nz)] << std::endl;
            }
        }
    }
    writing_file.close();
}