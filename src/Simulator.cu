//
//  Simulator.cpp
//  OpenGLTest
//
//  Created by 須之内俊樹 on 2024/05/26.

#include "Simulator.hpp"
// #include "Simulator.cuh"
float* Simulator::get_currentTexture()
{
    return density_tgt.src_texture;
}
void CalForceEncoder::encode
(
    Slab &out_y_force,
    Slab &density_tgt,
    Slab &density_amb,
    Slab &templature
)
{
    // GLuint shader_program = loadComputeProgram("../../shader/calForce.comp");
    GLuint shader_program = loadComputeProgram(_shader_path);
    glUseProgram(shader_program);
    //ssboにBindするBufferを用意
    out_y_force.setupBuffer(0);
    density_tgt.setupBuffer(1);
    density_amb.setupBuffer(2);
    templature.setupBuffer(3);
    
    GLfloat uniform_g0 = glGetUniformLocation(shader_program, "g0");
    GLfloat uniform_t_amb = glGetUniformLocation(shader_program, "t_amb");
    GLfloat uniform_beta = glGetUniformLocation(shader_program, "beta");

    glUniform1f(uniform_g0, G0);
    glUniform1f(uniform_t_amb, AMB_TEMPLATURE);
    // glUniform1f(uniform_beta, _beta);
    glDispatchCompute(std::pow(2,10),1,1);
    out_y_force.sl_getSrcBufferSubData();
    // out_y_force.print_src();
    out_y_force.swap_src_dst();
}
void Simulator::inputTXT(std::string &InputFileName)
{
    float max = 0;
    FILE *ifp = fopen(InputFileName.c_str(),"r");
    for(unsigned int k=0;k<_texdepth;++k){
        for(unsigned int j=0;j<_texheight;++j){
            for(unsigned int i=0;i<_texwidth;++i){
                float value;
                if(fscanf(ifp, "%f", &value));
                density_tgt.src_texture[resequence3to1(i, _texheight - j, k, _texwidth, _texheight, _texdepth)] = value;
            }
        }
    }
    fclose(ifp);
}

void Simulator::init_density(float init_density_value)
{
    std::vector<unsigned int>init_index = get_init_index_list(_texwidth,_texheight,_texdepth);
    // for(unsigned int i=0;i<density_tgt.size;++i)density_tgt.src_texture[i] = 0.0f;
    for(unsigned int i=0;i<density_amb.size;++i)density_amb.src_texture[i] = init_density_value;
    for(unsigned int i=0;i<init_index.size();++i)density_tgt.src_texture[init_index[i]] = init_density_value;
}
void Simulator::init_templature(float init_templature_value)
{
    std::vector<unsigned int>init_index = get_init_index_list(_texwidth,_texheight,_texdepth); 
    // for(unsigned int i=0;i<templature.size;++i)templature.src_texture[i] = AMB_TEMPLATURE;
    for(unsigned int i=0;i<init_index.size();++i)templature.src_texture[init_index[i]] = init_templature_value;
}
void Simulator::init_velocity(float init_templature_value)
{
    std::vector<unsigned int>init_index = get_init_index_list(_texwidth,_texheight,_texdepth); 
    for(unsigned int i=0;i<init_index.size();++i)y_velocity.src_texture[init_index[i]] = init_templature_value;
}
void Simulator::init_all_velocity()
{
    unsigned int size = (_texwidth + 1) * _texheight * _texdepth;
    for(unsigned int i=0;i<_texwidth+1;++i)
    {
        for(unsigned int j=0;j<_texheight;++j)
        {
            for(unsigned int k=0;k<_texdepth;++k)
            {
                all_velocity(resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)) = x_velocity.get_volume_value(i,j,k);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight+1;++j)
        {
            for(unsigned int k=0;k<_texdepth;++k)
            {
                all_velocity(size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)) = y_velocity.get_volume_value(i,j,k);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight;++j)
        {
            for(unsigned int k=0;k<_texdepth+1;++k)
            {
                all_velocity(2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)) = z_velocity.get_volume_value(i,j,k);
            }
        }
    }
}
void Simulator::all2xyz()
{
    unsigned int size = (_texwidth + 1) * _texheight * _texdepth;
    for(unsigned int i=0;i<_texwidth+1;++i)
    {
        for(unsigned int j=0;j<_texheight;++j)
        {
            for(unsigned int k=0;k<_texdepth;++k)
            {
                x_velocity.set_volume_value(i,j,k,all_velocity(resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)));
                // all_velocity(resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)) = x_velocity.get_volume_value(i,j,k);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight+1;++j)
        {
            for(unsigned int k=0;k<_texdepth;++k)
            {
                y_velocity.set_volume_value(i,j,k,all_velocity(size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)));
                // all_velocity(size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)) = y_velocity.get_volume_value(i,j,k);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight;++j)
        {
            for(unsigned int k=0;k<_texdepth+1;++k)
            {
                z_velocity.set_volume_value(i,j,k,all_velocity(2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)));
                // all_velocity(2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)) = z_velocity.get_volume_value(i,j,k);
            }
        }
    }
    x_velocity.swap_src_dst();
    y_velocity.swap_src_dst();
    z_velocity.swap_src_dst();
}
void Simulator::oneloop()
{
    // calForceEncoder.encode(y_force,density_tgt,density_tgt,templature);
    init_density(TGT_DENSITY);
    init_templature(TGT_TEMPLATURE);
    std::cout << "calForce" << std::endl;
    addForce();
    std::cout << "addForce" << std::endl;
    faceAdvect();
    std::cout << "faceAdvect" << std::endl;
    // TD.startTimer("cd_project");
    project();
    // times.push_back(TD.endTimer());
    std::cout << "project" << std::endl;
    centerAdvect(templature);
    // std::cout << "centerAdvectTemp" << std::endl;
    centerAdvect(density_tgt);
    centerAdvect(density_amb);
    // std::cout << "centerAdvectRho" << std::endl;
}
void Simulator::output_txt(unsigned int id)
{
    std::string density_outputFileName = density_floder_name + "/output" + std::to_string(id) + ".txt";
    // density_tgt.print_src();
    density_tgt.outputTXT(density_outputFileName);
}

float Simulator::TriLinearInterporation(float x,float y,float z,Slab &val)
{
    double fix_x = fmax(0.0, fmin(val._width-1-1e-6,x/_dx));
    double fix_y = fmax(0.0, fmin(val._height-1-1e-6,y/_dx));
    double fix_z = fmax(0.0, fmin(val._depth-1-1e-6,z/_dx));
    int i = fix_x;int j = fix_y;int k = fix_z;
    float s = fix_x-i;float t = fix_y-j;float u = fix_z-k;
    Eigen::Vector<float ,8> f = {
        val.get_volume_value(i,j,k),
        val.get_volume_value(i+1,j,k),
        val.get_volume_value(i+1,j+1,k),
        val.get_volume_value(i,j+1,k),
        val.get_volume_value(i,j,k+1),
        val.get_volume_value(i+1,j,k+1),
        val.get_volume_value(i+1,j+1,k+1),
        val.get_volume_value(i,j+1,k+1)
    };
    Eigen::Vector<float ,8> c = 
    {
        (1-s)*(1-t)*(1-u),s*(1-t)*(1-u),s*t*(1-u),(1-s)*t*(1-u),
        (1-s)*(1-t)*u,s*(1-t)*u,s*t*u,(1-s)*t*u
    };
    return f.dot(c);
}
void Simulator::faceAdvect(){
    for(unsigned int i=1;i<x_velocity._width-1;++i){
        for(unsigned int j=0;j<x_velocity._height;++j){
            for(unsigned int k=0;k<x_velocity._depth;++k){
                float x = i*_dx;float y = (j+0.5)*_dx;float z = (k+0.5)*_dx;
                // float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                // float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                // float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                // float value = TriLinearInterporation(adv_x, adv_y - 0.5*_dx, adv_z- 0.5*_dx, x_velocity);
                float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                float value = TriLinearInterporation(adv_x, adv_y - 0.5*_dx, adv_z- 0.5*_dx, x_velocity);
                x_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    // y_velocity.print_src();
    // std::cout << "y_velocity advect" << std::endl; 
    for(unsigned int i=0;i<y_velocity._width;++i){
        for(unsigned int j=1;j<y_velocity._height-1;++j){
            for(unsigned int k=0;k<y_velocity._depth;++k){
                float x = (i+0.5)*_dx;float y = j*_dx;float z = (k+0.5)*_dx;
                // float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                // float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                // float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                // float value = TriLinearInterporation(adv_x- 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);
                float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                float value = TriLinearInterporation(adv_x- 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);
                // std::cout << value << "= TriLinearInterporation(" << adv_x- 0.5*_dx << "," << adv_y << ","<< adv_z- 0.5*_dx << ",y_velocity)" << std::endl;
                // if(y_velocity.get_volume_value(i,j,k) > 1e-6 )std::cout << i << "," << j << "," << k << " : " << y_velocity.get_volume_value(i,j,k) << std::endl;
                y_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    for(unsigned int i=0;i<z_velocity._width;++i){
        for(unsigned int j=0;j<z_velocity._height;++j){
            for(unsigned int k=1;k<z_velocity._depth-1;++k){
                float x = (i+0.5)*_dx;float y = (j+0.5)*_dx;float z = k*_dx;
                // float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                // float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                // float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                // float value = TriLinearInterporation(adv_x- 0.5*_dx, adv_y- 0.5*_dx, adv_z, z_velocity);
                float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                float value = TriLinearInterporation(adv_x- 0.5*_dx, adv_y- 0.5*_dx, adv_z, z_velocity);
                z_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    x_velocity.swap_src_dst();
    y_velocity.swap_src_dst();
    z_velocity.swap_src_dst();
}

void Simulator::centerAdvect(Slab &val){
    // myArray3<float > old_val = val;
    for(unsigned int i=0;i<val._width;++i){
        for(unsigned int j=0;j<val._height;++j){
            for(unsigned int k=0;k<val._depth;++k){
                float x = (i+0.5)*_dx;float y = (j+0.5)*_dx;float z = (k+0.5)*_dx;
                float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                float value = TriLinearInterporation(adv_x- 0.5*_dx, adv_y- 0.5*_dx, adv_z- 0.5*_dx, val);
                val.set_volume_value(i,j,k,value);
                //if(i==0 || j==0 || k==0 || i == val.texwidth-1 || j==val._texheight-1 ||k==val._texdepth-1)val.value[i][j][k] = boundary_value;
            }
        }
    }
    val.swap_src_dst();
}

void Simulator::calPoissonMatrix()
{
    std::vector<Triplet> triplets;
    // y_velocity.print_src();
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // px[i+j*texwidth+k*texwidth*_texheight] = p.value[i][j][k];
                // float scale = _dt/((density_tgt.get_volume_value(i,j,k) + density_amb.get_volume_value(i,j,k))*_dx*_dx);
                // float scale = _dt/((rho_tgt.value[i][j][k] + rho_amb.value[i][j][k])*_dx*_dx);
                float scale = _dt/(_dx*_dx);
                float D[6] = {1.0,1.0,-1.0,-1.0,-1.0,1.0};//周囲6方向に向かって働く、圧力の向き
                //float F[4] = {(float )(i<texwidth-1),(float )(j<_texheight-1),(float )(i>0),(float )(j>0)};//境界条件。壁なら0,流体なら1

                std::vector<int> F = {i<_texwidth-1,j<_texheight-1,i>0,j>0,k>0,k<_texdepth-1};
                float sumP = 0;
                for(int n=0;n<6;n++){
                    sumP += -F[n]*scale;
                }
                triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+k*_texwidth*_texheight, sumP);
                if(F[0])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+1+j*_texwidth+k*_texwidth*_texheight, F[0]*scale);
                if(F[1])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+(j+1)*_texwidth+k*_texwidth*_texheight, F[1]*scale);
                if(F[2])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i-1+j*_texwidth+k*_texwidth*_texheight, F[2]*scale);
                if(F[3])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+(j-1)*_texwidth+k*_texwidth*_texheight, F[3]*scale);
                if(F[4])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+(k-1)*_texwidth*_texheight, F[4]*scale);
                if(F[5])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+(k+1)*_texwidth*_texheight, F[5]*scale);
            }
        }
    }
    PoissonMatrix.setFromTriplets(triplets.begin(), triplets.end());
}

void Simulator::calVel2DivMatrix()
{
    std::vector<Triplet> triplets;
    float size = (_texwidth + 1) * _texheight * _texdepth;
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                float D[6] = {1.0,1.0,-1.0,-1.0,-1.0,1.0};//周囲6方向に向かって働く、圧力の向き
                std::vector<int> F = {i<_texwidth-1,j<_texheight-1,i>0,j>0,k>0,k<_texdepth-1};
                //速度の境界値は0に設定しているので、境界成分に対応する係数は0でよい
                // for(int n=0;n<6;n++){
                //     b(i+j*texwidth+k*texwidth*_texheight) += D[n]*F[n]*U[n]/(_dx);
                // }
                if(F[0])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+1+j*_texwidth+k*_texwidth*_texheight, (D[0]*F[0])/(_dx));
                if(F[1])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, size + i+(j+1)*_texwidth+k*_texwidth*_texheight, (D[1]*F[1])/(_dx));
                if(F[2])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, (D[2]*F[2])/(_dx));
                if(F[3])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, size + i+(j)*_texwidth+k*_texwidth*_texheight, (D[3]*F[3])/(_dx));
                if(F[4])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k)*_texwidth*_texheight, (D[4]*F[4])/(_dx));
                if(F[5])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k+1)*_texwidth*_texheight, (D[5]*F[5])/(_dx));
            }
        }
    }
    Vel2DivMatrix.setFromTriplets(triplets.begin(), triplets.end());
}

void Simulator::calPressure2VelocityMatrix()
{
    std::vector<Triplet> triplets;
    float size = (_texwidth + 1) * _texheight * _texdepth;
    for(unsigned int i=1;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, _dt/(_dx));
                triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i-1+j*_texwidth+k*_texwidth*_texheight, -1*_dt/(_dx));
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=1;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, _dt/(_dx));
                triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, i+(j-1)*_texwidth+k*_texwidth*_texheight, -1*_dt/(_dx));
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=1;k<_texdepth;k++){
                triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, _dt/(_dx));
                triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+(k-1)*_texwidth*_texheight, -1*_dt/(_dx));
            }
        }
    }
    Pressure2VelocityMatrix.setFromTriplets(triplets.begin(), triplets.end());
}

void Simulator::project(){
    Eigen::VectorXf b = Eigen::VectorXf::Zero(_texwidth*_texheight*_texdepth);
    // Eigen::VectorXf test_b = Eigen::VectorXf::Zero(_texwidth*_texheight*_texdepth);
    Eigen::VectorXf px(_texwidth*_texheight*_texdepth);
    Eigen::ConjugateGradient<SparseMatrix> solver;

    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                px[i+j*_texwidth+k*_texwidth*_texheight] = pressure.get_volume_value(i,j,k);
            }
        }
    }
    solver.setTolerance(1e-6);
    solver.setMaxIterations(20);
    // solver.compute(A);
    init_all_velocity();
    b = Vel2DivMatrix * all_velocity;
    //initialize
    solver.compute(PoissonMatrix);
    px = solver.solveWithGuess(b, px);

    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                pressure.set_volume_value(i,j,k,px(i+j*_texwidth+k*_texwidth*_texheight));
            }
        }
    }
    pressure.swap_src_dst();

    all_velocity = all_velocity - Pressure2VelocityMatrix * px;
    all2xyz();
}

void Simulator::addForce(){

    // setCenterRot();
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // f.value[i][j][k] = getBuoyanacy(i, j, k);
                y_force.set_volume_value(i,j,k,getBuoyanacy(i, j, k).y());
                // if(getBuoyanacy(i, j, k).y() > 1e-6)std::cout << i << "," << j << "," << k << ":" <<getBuoyanacy(i, j, k).y() << std::endl;
            }
        }
    }
    y_force.swap_src_dst();
    // y_force.print_src();

    // for(int i=1;i<Nx-1;i++){
    //     for(int j=1;j<Ny-1;j++){
    //         for(int k=1;k<Nz-1;k++){
    //             f.value[i][j][k] += getConfinement(i, j, k);
    //         }
    //     }
    // }
    //x
    for(unsigned int i=1;i<_texwidth-1;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // u.value[i][j][k] += dt*(f.value[i-1][j][k].x() + f.value[i][j][k].x())/2;
            }
        }
    }
    //y
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=1;j<_texheight-1;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // v.value[i][j][k] += dt*(f.value[i][j-1][k].y() + f.value[i][j][k].y())/2;
                float value =  y_velocity.get_volume_value(i,j,k);
                value += _dt * ( y_force.get_volume_value(i,j-1,k) + y_force.get_volume_value(i,j,k) )/2;
                y_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    //z
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=1;k<_texdepth-1;k++){
                // w.value[i][j][k] += dt*(f.value[i][j][k-1].z() + f.value[i][j][k].z())/2;
            }
        }
    }
    y_velocity.swap_src_dst();
    // y_velocity.print_src();
}

Eigen::Vector3d Simulator::getBuoyanacy(int i,int j, int k){
    Eigen::Vector3d dir_gravity = {0.0,1.0,0.0};
    float rho = density_tgt.get_volume_value(i,j,k);
    float rho_amb = density_amb.get_volume_value(i,j,k);
    float temp = templature.get_volume_value(i,j,k);
    // float value = -(-G0*(rho +rho_amb) + BETA*(temp - AMB_TEMPLATURE));
    return -(-G0*(rho +rho_amb) + _beta*(temp - AMB_TEMPLATURE))*dir_gravity;
}