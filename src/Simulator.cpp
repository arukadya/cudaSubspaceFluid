#include "Simulator.hpp"
// #include "Simulator.cuh"
float* Simulator::get_currentTexture()
{
    return density_tgt.src_texture;
}
// void CalForceEncoder::encode
// (
//     Slab &out_y_force,
//     Slab &density_tgt,
//     Slab &density_amb,
//     Slab &templature
// )
// {
//     // GLuint shader_program = loadComputeProgram("../../shader/calForce.comp");
//     GLuint shader_program = loadComputeProgram(_shader_path);
//     glUseProgram(shader_program);
//     //ssboにBindするBufferを用意
//     out_y_force.setupBuffer(0);
//     density_tgt.setupBuffer(1);
//     density_amb.setupBuffer(2);
//     templature.setupBuffer(3);
    
//     GLfloat uniform_g0 = glGetUniformLocation(shader_program, "g0");
//     GLfloat uniform_t_amb = glGetUniformLocation(shader_program, "t_amb");
//     // GLfloat uniform_beta = glGetUniformLocation(shader_program, "beta");

//     glUniform1f(uniform_g0, G0);
//     glUniform1f(uniform_t_amb, AMB_TEMPLATURE);
//     // glUniform1f(uniform_beta, _beta);
//     glDispatchCompute(std::pow(2,10),1,1);
//     out_y_force.sl_getSrcBufferSubData();
//     // out_y_force.print_src();
//     out_y_force.swap_src_dst();
// }
//Simulator
void Simulator::inputTXT(std::string &InputFileName)
{
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
    //nonlinear
    // std::cout << "calForce" << std::endl;
    addForce();
    // std::cout << "addForce" << std::endl;
    init_all_velocity();
    // std::cout << "init_all_velocity" << std::endl;
    // std::cout << "U0 snap norm = " << all_velocity.norm() << std::endl;
    //U0
    write_snapshot(U0_SnapShot, all_velocity);
    write_exact_solution(U0_all_frame, all_velocity);
    
    faceAdvect();

    //linear
    init_all_velocity();
    //U1
    //Diffusion
    //U2
    // std::cout << "U2 snap norm = " << all_velocity.norm() << std::endl;
    write_snapshot(U2_SnapShot, all_velocity);
    write_exact_solution(U2_all_frame, all_velocity);
    // std::cout << "U2 norm = " << U2_all_frame.row(_timestamp).norm() << std::endl;
    
    project();
    // std::cout << "px norm = " << px.norm() << std::endl;
    write_snapshot(U3_SnapShot, all_velocity);
    write_snapshot(P_SnapShot, px);
    write_exact_solution(U3_all_frame, all_velocity);
    // std::cout << "U3 norm = " << U3_all_frame.row(_timestamp).norm() << std::endl;
    write_exact_solution(P_all_frame, px);
    // std::cout << "P norm = " << P_all_frame.row(_timestamp).norm() << std::endl;

    //nonlinear
    //U3
    // times.push_back(TD.endTimer());
    // std::cout << "project" << std::endl;
    centerAdvect(templature);
    // std::cout << "centerAdvectTemp" << std::endl;
    centerAdvect(density_tgt);
    centerAdvect(density_amb);
    // std::cout << "centerAdvectRho" << std::endl;
    ++_timestamp;
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
                // float scale = _dt/((density_tgt.get_volume_value(i,j,k) + density_amb.get_volume_value(i,j,k))*_dx*_dx);
                // float scale = _dt/((rho_tgt.value[i][j][k] + rho_amb.value[i][j][k])*_dx*_dx);
                float scale = _dt/(_dx*_dx);
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

void Simulator::calDirichletBoundaryMatrix()
{
    std::vector<Triplet> triplets;
    float size = (_texwidth + 1) * _texheight * _texdepth;
    for(unsigned int i=0;i<_texwidth+1;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                if(i ==0 || i == _texwidth)triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+k*_texwidth*_texheight, 0);
                else triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+k*_texwidth*_texheight, 1);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight+1;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                if(j ==0 || j == _texheight)triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight,size + i+j*_texwidth+k*_texwidth*_texheight, 0);
                else triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight,size + i+j*_texwidth+k*_texwidth*_texheight, 1);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth+1;k++){
                if(k ==0 || k == _texdepth)triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight,2*size + i+j*_texwidth+k*_texwidth*_texheight, 0);
                else triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight,2*size + i+j*_texwidth+k*_texwidth*_texheight, 1);
            }
        }
    }
    DirichletBoundaryMatrix.setFromTriplets(triplets.begin(), triplets.end());
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

void Simulator::calDiffusionMatrix()
{
    std::vector<Triplet> triplets;
    float size = (_texwidth + 1) * _texheight * _texdepth;
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // float D[6] = {1.0,1.0,-1.0,-1.0,-1.0,1.0};//周囲6方向に向かって働く、圧力の向き
                float scale = _dt / (4 * _dx* _dx);
                std::vector<int> F = {i<_texwidth-2,j<_texheight-2,i>1,j>1,k>1,k<_texdepth-2};
                //速度の境界値は0に設定しているので、境界成分に対応する係数は0でよい
                if(F[0])
                {
                    triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+2+j*_texwidth+k*_texwidth*_texheight, scale);//x
                    triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+2+j*_texwidth+k*_texwidth*_texheight, scale);//y
                    triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+2+j*_texwidth+k*_texwidth*_texheight, scale);//z
                }
                if(F[1])
                {
                    triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+(j+2)*_texwidth+k*_texwidth*_texheight, scale);
                    triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+(j+2)*_texwidth+k*_texwidth*_texheight, scale);
                    triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+(j+2)*_texwidth+k*_texwidth*_texheight, scale);
                }
                // if(F[0] && F[1])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, -2 * scale);
                if(F[2])
                {
                    triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i-2+j*_texwidth+k*_texwidth*_texheight, scale);
                    triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i-2+j*_texwidth+k*_texwidth*_texheight, scale);
                    triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i-2+j*_texwidth+k*_texwidth*_texheight, scale);
                }
                if(F[3])
                {
                    triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+(j-2)*_texwidth+k*_texwidth*_texheight, scale);
                    triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+(j-2)*_texwidth+k*_texwidth*_texheight, scale);
                    triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+(j-2)*_texwidth+k*_texwidth*_texheight, scale);
                }
                // if(F[2] && F[3])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, -2 * scale);
                if(F[4])
                {
                    triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+(k-2)*_texwidth*_texheight, scale);
                    triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+j*_texwidth+(k-2)*_texwidth*_texheight, scale);
                    triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k-2)*_texwidth*_texheight, scale);
                }
                if(F[5])
                {
                    triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
                    triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
                    triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
                }
                if(F[0] && F[1] && F[2] && F[3] && F[4] && F[5])
                {
                    triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, 1 - 6 * scale);
                    triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+j*_texwidth+k*_texwidth*_texheight, 1 - 6 * scale);
                    triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+k*_texwidth*_texheight, 1 - 6 * scale);
                }
            }
        }
    }
    DiffusionMatrix.setFromTriplets(triplets.begin(), triplets.end());
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
    // Eigen::VectorXf px(_texwidth*_texheight*_texdepth);
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
    b = Vel2DivMatrix * DirichletBoundaryMatrix * all_velocity;
    write_exact_solution(b_all_frame,b);
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

//snapshotPOD
void Simulator::write_snapshot(Eigen::MatrixXf &mat, Eigen::VectorXf &snap)
{
    assert(mat.cols != snap.size || mat.rows > _timestamp);
    // if(_timestamp % _snap_num == 0)mat.row(timestamp) = snap;
    // if(_timestamp % _delta_snap == 0)mat.row(_timestamp / _delta_snap) = snap;
    if(_timestamp % _delta_snap == 0)mat.col(_timestamp / _delta_snap) = snap;
}

void Simulator::write_exact_solution(Eigen::MatrixXf &mat, Eigen::VectorXf &snap)
{
    assert(mat.cols != snap.size || mat.rows > _timestamp);
    // if(_timestamp % _snap_num == 0)mat.row(timestamp) = snap;
    // mat.row(_timestamp) = snap;
    mat.col(_timestamp) = snap;
}

Eigen::Vector3d Simulator::getBuoyanacy(int i,int j, int k){
    Eigen::Vector3d dir_gravity = {0.0,1.0,0.0};
    float rho = density_tgt.get_volume_value(i,j,k);
    float rho_amb = density_amb.get_volume_value(i,j,k);
    float temp = templature.get_volume_value(i,j,k);
    // float value = -(-G0*(rho +rho_amb) + BETA*(temp - AMB_TEMPLATURE));
    return -(-G0*_dx*(rho +rho_amb) + _beta*(temp - AMB_TEMPLATURE))*dir_gravity;
}

void Simulator::getBasisQRSVD()
{
    _reduce_dimention = _snap_num;
    std::cout << "U0" << std::endl;
    U0 = cal_Basis(U0_SnapShot,_reduce_dimention,_threshold);
    // std::cout << "norm, reduce_dimention = " << U0.norm() << ", " << _reduce_dimention << std::endl;
    // std::cout << "orthogonomality : " << 
    // (U0.transpose() * U0 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;

    std::cout << "U2" << std::endl;
    U2 = cal_Basis(U2_SnapShot,_reduce_dimention,_threshold);
    // std::cout << "norm, reduce_dimention = " << U2.norm() << ", " << _reduce_dimention << std::endl;
    // std::cout << "orthogonomality : " << 
    // (U2.transpose() * U2 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;

    std::cout << "U3" << std::endl;
    U3 = cal_Basis(U3_SnapShot,_reduce_dimention,_threshold);
    // std::cout << "norm, reduce_dimention = " << U3.norm() << ", " << _reduce_dimention << std::endl;
    // std::cout << "orthogonomality : " << 
    // (U3.transpose() * U3 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;

    std::cout << "P" << std::endl;
    P = cal_Basis(P_SnapShot,_reduce_dimention,_threshold);
    std::cout << "norm, reduce_dimention = " << P.norm() << ", " << _reduce_dimention << std::endl;
    std::cout << "orthogonomality : " << 
    (P.transpose() * P - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;
}



void Simulator::getReducedLinearOperator()
{
    reduced_Vel2DivMatrix = P.transpose() * Vel2DivMatrix * U2;
    reduced_PoissonMatrix = P.transpose() * PoissonMatrix * P;
    reduced_Pressure2VelocityMatrix = U3.transpose() * Pressure2VelocityMatrix * P;
    reduced_DirichletBoundaryMatrix = U2.transpose() * DirichletBoundaryMatrix * U2;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> ES(reduced_PoissonMatrix);
    Eigen::VectorXf reduced_Poisson_eigenval = ES.eigenvalues();
    bool is_positive_definite = true;
    for(int i=0;i<reduced_Poisson_eigenval.size();++i)
    {
        if(reduced_Poisson_eigenval(i) < 0)is_positive_definite = false;
    }
    if(!is_positive_definite)std::cout << "non positive definite matrix" << std::endl;
    bool is_symmetric = true;
    for(int i=0;i<reduced_PoissonMatrix.rows();++i)
    {
        for(int j=0;j<reduced_PoissonMatrix.cols();++j)
        {
            if( reduced_PoissonMatrix(i,j) - reduced_PoissonMatrix(j,i) > 1e-6 )is_symmetric = false;
        }
    }
    if(!is_symmetric)std::cout << "non symmetric matrix" << std::endl;
    if(is_positive_definite && is_symmetric)std::cout << "reduced_X is symmetric positive definite matrix" << std::endl;
    std::cout << "(P^T X P)^-1.det = " << reduced_PoissonMatrix.determinant() << std::endl;
    // std::cout << "(P^T X P)^-1 = " << std::endl << reduced_PoissonMatrix.inverse() << std::endl;
    std::cout << "((P^T X P) * (P^T X P)^-1 - I).norm() = " << std::endl <<
    (reduced_PoissonMatrix * reduced_PoissonMatrix.inverse() - Eigen::MatrixXf::Identity(reduced_PoissonMatrix.rows(),reduced_PoissonMatrix.cols())).norm() << std::endl;
    // std::cout << "X^-1.det = " << PoissonMatrix. << std::endl;
    // std::cout << "reduced_W size" << std::endl << 
    // "rows,cols = " << reduced_Vel2DivMatrix.rows() << "," << reduced_Vel2DivMatrix.cols() << std::endl;
    // std::cout << "reduced_X size" << std::endl << 
    // "rows,cols = " << reduced_PoissonMatrix.rows() << "," << reduced_PoissonMatrix.cols() << std::endl;
    // std::cout << "reduced_X" << std::endl << reduced_PoissonMatrix << std::endl;
    // std::cout << "reduced_Y size" << std::endl << 
    // "rows,cols = " << reduced_Pressure2VelocityMatrix.rows() << "," << reduced_Pressure2VelocityMatrix.cols() << std::endl;
    // std::cout << "reduced_D size" << std::endl << 
    // "rows,cols = " << reduced_DirichletBoundaryMatrix.rows() << "," << reduced_DirichletBoundaryMatrix.cols() << std::endl;
    // std::cout << "reduced_D" << std::endl << reduced_DirichletBoundaryMatrix << std::endl;
}

void Simulator::output_Basis()
{
    std::filesystem::create_directories("basis");
    int n = _texdepth * _texheight * _texdepth;
    // int r = _reduce_dimention;
    int r = _snap_num;
    std::string basis_foldername = "basis/n" + std::to_string(n) + "r" + std::to_string(r); 
    std::filesystem::create_directories(basis_foldername);
    std::string U0FileName = basis_foldername + "/U0.txt";
    std::string U2FileName = basis_foldername + "/U2.txt";
    std::string U3FileName = basis_foldername + "/U3.txt";
    std::string PFileName  = basis_foldername +  "/P.txt";
    outputMatrix(U0FileName,U0);
    outputMatrix(U2FileName,U2);
    outputMatrix(U3FileName,U3);
    outputMatrix(PFileName,P);
}

void Simulator::input_Basis()
{
    std::filesystem::create_directories("basis");
    int n = _texdepth * _texheight * _texdepth;
    // int r = _reduce_dimention;
    int r = _snap_num;
    std::string basis_foldername = "basis/n" + std::to_string(n) + "r" + std::to_string(r); 
    std::string U0FileName = basis_foldername + "/U0.txt";
    std::string U2FileName = basis_foldername + "/U2.txt";
    std::string U3FileName = basis_foldername + "/U3.txt";
    std::string PFileName  = basis_foldername +  "/P.txt";
    n = (_texdepth + 1)* _texheight * _texdepth;
    U0 = Eigen::MatrixXf(3*n,r);
    U2 = Eigen::MatrixXf(3*n,r);
    U3 = Eigen::MatrixXf(3*n,r);
    P  = Eigen::MatrixXf(_texdepth * _texheight * _texdepth,r);
    inputMatrix(U0FileName,U0);
    inputMatrix(U2FileName,U2);
    inputMatrix(U3FileName,U3);
    inputMatrix(PFileName,P);
}

Eigen::MatrixXf cal_Basis(Eigen::MatrixXf &SnapShot, unsigned int &reduce_dimention,float threshold)
{
    Eigen::HouseholderQR<Eigen::MatrixXf> QRSolver(SnapShot);
    int m = SnapShot.rows();
    int n = SnapShot.cols();
    Eigen::HouseholderQR<Eigen::MatrixXf> HhQR(SnapShot);
    // std::cout << "QRfactorization" << std::endl;
    Eigen::MatrixXf R = HhQR.matrixQR();
    // std::cout << "R" << std::endl;
    Eigen::MatrixXf thin_R(n,n);
    Eigen::MatrixXf _R(m,n);
    for(int i=0;i<n;++i)
    {
        for(int j=0;j<n;++j)
        {
            if(j >= i)thin_R(i,j) = R(i,j);
            else thin_R(i,j) = 0;
        }
    }
    for(int i=0;i<m;++i)
    {
        if(i < n)_R.row(i) = thin_R.row(i);
        else _R.row(i) = Eigen::VectorXf::Zero(n).transpose();
    }
    // std::cout << "fix_R" << std::endl;
    Eigen::JacobiSVD<Eigen::MatrixXf> svd(_R,Eigen::ComputeThinU);
    // std::cout << "svd" << std::endl;
//    //特異値
    Eigen::VectorXf singular_values = svd.singularValues();
    Eigen::MatrixXf singular_value_matrix = singular_values.asDiagonal();
    for(unsigned int i=0;i<singular_values.size();++i)
    {
        if(singular_values(i) < threshold)
        {
            if(reduce_dimention < i)reduce_dimention = i;
            break;
        }
    }
    std::cout << "singular value" << std::endl << singular_values.transpose() << std::endl;
//    Eigen::MatrixXf Basis = HhQR.matrixQ()*svd.matrixU();
    Eigen::MatrixXf Basis(m,n);

    // Eigen::MatrixXf Basis(m,reduce_dimention);
    Basis = HhQR.householderQ()*svd.matrixU(); //m * n
    // Basis = HhQR.householderQ()*svd.matrixU()*singular_value_matrix; //m * n
    std::cout << "Basis size : " << Basis.rows() << "," << Basis.cols() << std::endl;
    std::cout << "m , n : " << m << "," << n << std::endl;

    // std::cout << "Basis is orthonomality check : " << (Basis.transpose() * Basis - Eigen::MatrixXf::Identity(n,n)).norm() << std::endl;
    return Basis;
}

//subspace
void Simulator::subspace_execute()
{
    
    x_velocity = Slab(_texwidth+1,_texheight,_texdepth,0.0f);
    y_velocity = Slab(_texwidth,_texheight+1,_texdepth,0.0f);
    z_velocity = Slab(_texwidth,_texheight,_texdepth+1,0.0f);
    x_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
    y_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
    z_force = Slab(_texwidth,_texheight,_texdepth,0.0f);
    pressure = Slab(_texwidth,_texheight,_texdepth,0.0f);
    density_tgt = Slab(_texwidth,_texheight,_texdepth,0.0f);
    density_amb = Slab(_texwidth,_texheight,_texdepth,AMB_DENSITY);
    templature = Slab(_texwidth,_texheight,_texdepth,AMB_TEMPLATURE);
    init_all_velocity();
    for(_timestamp = 0; _timestamp<_flame_num; ++_timestamp)
    {
        subspace_oneloop();
    }
    
}

void Simulator::subspace_oneloop()
{
    init_density(TGT_DENSITY);
    init_templature(TGT_TEMPLATURE);
    //nonlinear
    // std::cout << "sub_calForce" << std::endl;
    addForce();
    std::cout << "sub_addForce" << std::endl;
    init_all_velocity();
    std::cout << "sub_init_all_velocity" << std::endl;
    // std::cout << "U0 snap norm = " << all_velocity.norm() << std::endl;
    //U0
    // std::cout << "U0 snap norm = " << U0_SnapShot.row(_timestamp).norm() << std::endl;
    faceAdvect();
    //linear
    init_all_velocity();

    //U1
    //Diffusion
    //U2
    reduced_all_velocity = U2.transpose() * all_velocity;
    // std::cout << "all velocity    size, norm = " << all_velocity.size()<< "," << all_velocity.norm() << std::endl;
    // std::cout << "U2              size, norm = " << U2.rows() << "," << U2.cols() << "," << U2.norm() << std::endl;
    // std::cout << "reduce velocity size, norm = " << reduced_all_velocity.size()<< "," << reduced_all_velocity.norm() << std::endl;
    // std::cout << "accuracy check flame : " << _timestamp << std::endl;
    // std::cout << "U2 : " << (U2_all_frame.col(_timestamp) - U2 * reduced_all_velocity).norm() / U2_all_frame.col(_timestamp).norm() << std::endl;
    // std::cout << "exact, reduce : " << U2_all_frame.col(_timestamp).norm() << "," <<  (U2 * reduced_all_velocity).norm() << std::endl;
    reduced_all_velocity = U2.transpose() * U2_all_frame.col(_timestamp);
    subspace_project();
    std::cout << "U3 : " << (U3_all_frame.col(_timestamp) - U3 * reduced_all_velocity).norm() / U3_all_frame.col(_timestamp).norm() << std::endl;
    std::cout << "exact, reduce : " << U3_all_frame.col(_timestamp).norm() << "," <<  (U3 * reduced_all_velocity).norm() << std::endl;
    std::cout << "P  : " << ( P_all_frame.col(_timestamp) - P * reduced_px).norm() / P_all_frame.col(_timestamp).norm() << std::endl;
    std::cout << "exact, reduce : " << P_all_frame.col(_timestamp).norm() << "," <<  (P * reduced_px).norm() << std::endl;
    //nonlinear
    //U3
    // times.push_back(TD.endTimer());
    std::cout << "sub_project" << std::endl;
    centerAdvect(templature);
    // std::cout << "centerAdvectTemp" << std::endl;
    centerAdvect(density_tgt);
    centerAdvect(density_amb);
    output_txt(_timestamp);
    // std::cout << "centerAdvectRho" << std::endl;
}

void Simulator::subspace_project(){
    Eigen::VectorXf b;
    Eigen::ConjugateGradient<Eigen::MatrixXf> solver;
    // Eigen::FullPivHouseholderQR<Eigen::MatrixXf> solver;
    // solver.setTolerance(1e-6);
    // solver.setMaxIterations(20);
    // b = P.transpose() * b_all_frame.col(_timestamp);
    // std::cout << "exact    reduced_b" << std::endl << b.transpose() << std::endl;
    b = reduced_Vel2DivMatrix * reduced_DirichletBoundaryMatrix * reduced_all_velocity;
    // std::cout << "subspace reduced_b" << std::endl << b.transpose() << std::endl;
    solver.compute(reduced_PoissonMatrix);
    // std::cout << "before reduced_pressure" << std::endl << reduced_px.transpose() << std::endl;
    reduced_px = solver.solveWithGuess(b, reduced_px);
    // reduced_px = solver.solve(b);
    // std::cout << "subspace reduced_pressure" << std::endl << reduced_px.transpose() << std::endl;
    // reduced_px = reduced_PoissonMatrix.inverse() * b;
    // std::cout << "inverce  reduced_pressure" << std::endl << reduced_px.transpose() << std::endl;

    // all_velocity = all_velocity - Pressure2VelocityMatrix * px;
    // std::cout << "before reduced_velocity" << std::endl << reduced_all_velocity.transpose() << std::endl;
    reduced_all_velocity = reduced_DirichletBoundaryMatrix * reduced_all_velocity - reduced_Pressure2VelocityMatrix * reduced_px;
    // std::cout << "after reduced_velocity" << std::endl << reduced_all_velocity.transpose() << std::endl;
    all_velocity = U3 * reduced_all_velocity;
    all2xyz();
}


void getDevidedBasis();
void getDevidedReducedLinearOperator();