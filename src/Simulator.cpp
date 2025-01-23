#include "Simulator.hpp"
// #include "Simulator.cuh"

float* Simulator::get_currentTexture()
{
    return density_tgt.src_texture;
}
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
                unsigned int reseq = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                // all_velocity(resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth)) = x_velocity.get_volume_value(i,j,k);
                all_velocity(reseq) = x_velocity.src_texture[reseq];
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight+1;++j)
        {
            for(unsigned int k=0;k<_texdepth;++k)
            {
                // all_velocity(size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth)) = y_velocity.get_volume_value(i,j,k);
                unsigned int reseq = resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                all_velocity(size + reseq) = y_velocity.src_texture[reseq];
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight;++j)
        {
            for(unsigned int k=0;k<_texdepth+1;++k)
            {
                // all_velocity(2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1)) = z_velocity.get_volume_value(i,j,k);
                unsigned int reseq = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                all_velocity(2*size + reseq) = z_velocity.src_texture[reseq];
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
                // x_velocity.set_volume_value(i,j,k,all_velocity(resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth)));
                unsigned int reseq = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                x_velocity.dst_texture[reseq] = all_velocity(reseq);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight+1;++j)
        {
            for(unsigned int k=0;k<_texdepth;++k)
            {
                // y_velocity.set_volume_value(i,j,k,all_velocity(size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth)));
                unsigned int reseq = resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                y_velocity.dst_texture[reseq] = all_velocity(size + reseq);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;++i)
    {
        for(unsigned int j=0;j<_texheight;++j)
        {
            for(unsigned int k=0;k<_texdepth+1;++k)
            {
                // z_velocity.set_volume_value(i,j,k,all_velocity(2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1)));
                unsigned int reseq = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                z_velocity.dst_texture[reseq] = all_velocity(2*size + reseq);
            }
        }
    }
    x_velocity.swap_src_dst();
    y_velocity.swap_src_dst();
    z_velocity.swap_src_dst();
}

void Simulator::oneloop()
{
    init_density(TGT_DENSITY);
    init_templature(TGT_TEMPLATURE);
    addForce();
    init_all_velocity();
    //U0
    write_snapshot(U0_SnapShot, all_velocity);
    write_exact_solution(U0_all_frame, all_velocity);
    faceAdvect();
    //linear
    init_all_velocity();
    //U1
    //Diffusion
    //U2
    write_snapshot(U2_SnapShot, all_velocity);
    write_exact_solution(U2_all_frame, all_velocity);
    project();
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
                float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                float value = TriLinearInterporation(adv_x, adv_y - 0.5*_dx, adv_z- 0.5*_dx, x_velocity);
                x_velocity.set_volume_value(i,j,k,value);
            }
        }
    } 
    for(unsigned int i=0;i<y_velocity._width;++i){
        for(unsigned int j=1;j<y_velocity._height-1;++j){
            for(unsigned int k=0;k<y_velocity._depth;++k){
                float x = (i+0.5)*_dx;float y = j*_dx;float z = (k+0.5)*_dx;
                float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                float value = TriLinearInterporation(adv_x- 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);
                y_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    for(unsigned int i=0;i<z_velocity._width;++i){
        for(unsigned int j=0;j<z_velocity._height;++j){
            for(unsigned int k=1;k<z_velocity._depth-1;++k){
                float x = (i+0.5)*_dx;float y = (j+0.5)*_dx;float z = k*_dx;
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
                // float scale = _dt/(_dx*_dx);
                float scale = 1.0;
                std::vector<int> F = {i<_texwidth-1,j<_texheight-1,i>0,j>0,k>0,k<_texdepth-1};
                float sumP = 0;
                for(int n=0;n<6;n++){
                    sumP += -F[n]*scale;
                    // sumP += -scale;
                }
                unsigned int p_id = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth);
                unsigned int p_x_pre_id  = resequence3to1(i-1,j,k,_texwidth,_texheight,_texdepth);
                unsigned int p_x_post_id = resequence3to1(i+1,j,k,_texwidth,_texheight,_texdepth);
                unsigned int p_y_pre_id  = resequence3to1(i,j-1,k,_texwidth,_texheight,_texdepth);
                unsigned int p_y_post_id = resequence3to1(i,j+1,k,_texwidth,_texheight,_texdepth);
                unsigned int p_z_pre_id  = resequence3to1(i,j,k-1,_texwidth,_texheight,_texdepth);
                unsigned int p_z_post_id = resequence3to1(i,j,k+1,_texwidth,_texheight,_texdepth);

                // triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+k*_texwidth*_texheight, sumP);
                // if(F[0])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+1+j*_texwidth+k*_texwidth*_texheight, F[0]*scale);
                // if(F[1])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+(j+1)*_texwidth+k*_texwidth*_texheight, F[1]*scale);
                // if(F[2])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i-1+j*_texwidth+k*_texwidth*_texheight, F[2]*scale);
                // if(F[3])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+(j-1)*_texwidth+k*_texwidth*_texheight, F[3]*scale);
                // if(F[4])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+(k-1)*_texwidth*_texheight, F[4]*scale);
                // if(F[5])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+(k+1)*_texwidth*_texheight, F[5]*scale);
                triplets.emplace_back(p_id,p_id, sumP);
                if(F[0])triplets.emplace_back(p_id,p_x_post_id, F[0]*scale);
                if(F[1])triplets.emplace_back(p_id,p_y_post_id, F[1]*scale);
                if(F[2])triplets.emplace_back(p_id,p_x_pre_id,  F[2]*scale);
                if(F[3])triplets.emplace_back(p_id,p_y_pre_id,  F[3]*scale);
                if(F[4])triplets.emplace_back(p_id,p_z_pre_id,  F[4]*scale);
                if(F[5])triplets.emplace_back(p_id,p_z_post_id, F[5]*scale);
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
                // unsigned int x_vel_id = i+j*_texwidth+k*_texwidth*_texheight;
                unsigned int x_vel_id = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                if(i ==0 || i == _texwidth)triplets.emplace_back(x_vel_id,x_vel_id, 0);
                else triplets.emplace_back(x_vel_id,x_vel_id, 1);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight+1;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // unsigned int y_vel_id = size + i+j*_texwidth+k*_texwidth*_texheight;
                unsigned int y_vel_id = size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                if(j ==0 || j == _texheight)triplets.emplace_back(y_vel_id,y_vel_id, 0);
                else triplets.emplace_back(y_vel_id,y_vel_id, 1);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth+1;k++){
                // unsigned int z_vel_id = 2*size + i+j*_texwidth+k*_texwidth*_texheight;
                unsigned int z_vel_id = 2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                if(k ==0 || k == _texdepth)triplets.emplace_back(z_vel_id,z_vel_id, 0);
                else triplets.emplace_back(z_vel_id,z_vel_id, 1);
            }
        }
    }
    DirichletBoundaryMatrix.setFromTriplets(triplets.begin(), triplets.end());
}

void Simulator::calVel2DivMatrix()//W
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
                // if(F[0])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+1+j*_texwidth+k*_texwidth*_texheight, (D[0]*F[0])/(_dx));
                // if(F[1])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, size + i+(j+1)*_texwidth+k*_texwidth*_texheight, (D[1]*F[1])/(_dx));
                // if(F[2])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, (D[2]*F[2])/(_dx));
                // if(F[3])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, size + i+(j)*_texwidth+k*_texwidth*_texheight, (D[3]*F[3])/(_dx));
                // if(F[4])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k)*_texwidth*_texheight, (D[4]*F[4])/(_dx));
                // if(F[5])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k+1)*_texwidth*_texheight, (D[5]*F[5])/(_dx));
                unsigned int p_id = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth);
                unsigned int x_pre_id  = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                unsigned int x_post_id = resequence3to1(i+1,j,k,_texwidth+1,_texheight,_texdepth);
                unsigned int y_pre_id  = size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                unsigned int y_post_id = size + resequence3to1(i,j+1,k,_texwidth,_texheight+1,_texdepth);
                unsigned int z_pre_id  = 2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                unsigned int z_post_id = 2*size + resequence3to1(i,j,k+1,_texwidth,_texheight,_texdepth+1);
                if(F[0])triplets.emplace_back(p_id, x_post_id,  (D[0]*F[0])/(_dx));
                if(F[1])triplets.emplace_back(p_id, y_post_id,  (D[1]*F[1])/(_dx));
                if(F[2])triplets.emplace_back(p_id, x_pre_id,   (D[2]*F[2])/(_dx));
                if(F[3])triplets.emplace_back(p_id, y_pre_id,   (D[3]*F[3])/(_dx));
                if(F[4])triplets.emplace_back(p_id, z_pre_id,   (D[4]*F[4])/(_dx));
                if(F[5])triplets.emplace_back(p_id, z_post_id,  (D[5]*F[5])/(_dx));
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
    // float scale = _dt/(_dx);
    float scale = 1.0;
    for(unsigned int i=1;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, _dt/(_dx));
                // triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight, i-1+j*_texwidth+k*_texwidth*_texheight, -1*_dt/(_dx));
                unsigned int x_vel_id = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                unsigned int p_pre_id  = resequence3to1(i-1,j,k,_texwidth,_texheight,_texdepth);
                unsigned int p_post_id = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth);
                triplets.emplace_back(x_vel_id, p_post_id,    scale);
                triplets.emplace_back(x_vel_id, p_pre_id,  -1*scale);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=1;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, _dt/(_dx));
                // triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, i+(j-1)*_texwidth+k*_texwidth*_texheight, -1*_dt/(_dx));
                unsigned int y_vel_id = size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                unsigned int p_pre_id  = resequence3to1(i,j-1,k,_texwidth,_texheight,_texdepth);
                unsigned int p_post_id = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth);
                triplets.emplace_back(y_vel_id, p_post_id,    scale);
                triplets.emplace_back(y_vel_id, p_pre_id,  -1*scale);
            }
        }
    }
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=1;k<_texdepth;k++){
                // triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+k*_texwidth*_texheight, _dt/(_dx));
                // triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, i+j*_texwidth+(k-1)*_texwidth*_texheight, -1*_dt/(_dx));
                unsigned int z_vel_id = 2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                unsigned int p_pre_id  = resequence3to1(i,j,k-1,_texwidth,_texheight,_texdepth);
                unsigned int p_post_id = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth);
                triplets.emplace_back(z_vel_id, p_post_id,    scale);
                triplets.emplace_back(z_vel_id, p_pre_id,  -1*scale);
            }
        }
    }
    Pressure2VelocityMatrix.setFromTriplets(triplets.begin(), triplets.end());
}

void Simulator::origin_project()
{
    unsigned int grid_size = _texwidth*_texheight*_texdepth;
    SparseMatrix A(grid_size,grid_size);
    // Eigen::VectorXf origin_b = Eigen::VectorXf::Zero(grid_size);
    Eigen::VectorXf origin_px = Eigen::VectorXf::Zero(grid_size);
    //Tripletの計算
    std::vector<Triplet> triplets;
    for(int i=0;i<_texwidth;i++){
        for(int j=0;j<_texheight;j++){
            for(int k=0;k<_texdepth;k++){
                unsigned int grid_id = resequence3to1(i,j,k,_texdepth,_texheight,_texdepth);
                 // px[i+j*_texwidth+k*_texwidth*_texheight] = p.value[i][j][k];
                // double scale = _dt/((rho_tgt.value[i][j][k] + rho_amb.value[i][j][k])*_dx*_dx);
                float scale = _dt/(_dx*_dx);
                float D[6] = {1.0,1.0,-1.0,-1.0,-1.0,1.0};//周囲6方向に向かって働く、圧力の向き
                std::vector<int> F = {i<_texwidth-1,j<_texheight-1,i>0,j>0,k>0,k<_texdepth-1};
                float U[6] = {
                    x_velocity.get_volume_value(i+1,j,k),
                    y_velocity.get_volume_value(i,j+1,k),
                    x_velocity.get_volume_value(i,j,k),
                    y_velocity.get_volume_value(i,j,k),
                    z_velocity.get_volume_value(i,j,k),
                    z_velocity.get_volume_value(i,j,k+1)
                };
                float sumP = 0;
                for(int n=0;n<6;n++){
                    sumP += -F[n]*scale;
                    origin_b(resequence3to1(i,j,k,_texwidth,_texheight,_texdepth))+= D[n]*F[n]*U[n]/(_dx);
                    // origin_b(i+j*_texwidth+k*_texwidth*_texheight) += D[n]*F[n]*U[n]/(_dx);
                }
                unsigned int p_id = resequence3to1(i,j,k,_texwidth,_texheight,_texdepth);
                unsigned int p_x_pre_id  = resequence3to1(i-1,j,k,_texwidth,_texheight,_texdepth);
                unsigned int p_x_post_id = resequence3to1(i+1,j,k,_texwidth,_texheight,_texdepth);
                unsigned int p_y_pre_id  = resequence3to1(i,j-1,k,_texwidth,_texheight,_texdepth);
                unsigned int p_y_post_id = resequence3to1(i,j+1,k,_texwidth,_texheight,_texdepth);
                unsigned int p_z_pre_id  = resequence3to1(i,j,k-1,_texwidth,_texheight,_texdepth);
                unsigned int p_z_post_id = resequence3to1(i,j,k+1,_texwidth,_texheight,_texdepth);
                
                triplets.emplace_back(p_id,p_id, sumP);
                if(F[0])triplets.emplace_back(p_id,p_x_post_id, F[0]*scale);
                if(F[1])triplets.emplace_back(p_id,p_y_post_id, F[1]*scale);
                if(F[2])triplets.emplace_back(p_id,p_x_pre_id,  F[2]*scale);
                if(F[3])triplets.emplace_back(p_id,p_y_pre_id,  F[3]*scale);
                if(F[4])triplets.emplace_back(p_id,p_z_pre_id,  F[4]*scale);
                if(F[5])triplets.emplace_back(p_id,p_z_post_id, F[5]*scale);

                // triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+k*_texwidth*_texheight, sumP);
                // if(F[0])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+1+j*_texwidth+k*_texwidth*_texheight, F[0]*scale);
                // if(F[1])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+(j+1)*_texwidth+k*_texwidth*_texheight, F[1]*scale);
                // if(F[2])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i-1+j*_texwidth+k*_texwidth*_texheight, F[2]*scale);
                // if(F[3])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+(j-1)*_texwidth+k*_texwidth*_texheight, F[3]*scale);
                // if(F[4])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+(k-1)*_texwidth*_texheight, F[4]*scale);
                // if(F[5])triplets.emplace_back(i+j*_texwidth+k*_texwidth*_texheight,i+j*_texwidth+(k+1)*_texwidth*_texheight, F[5]*scale);
            }
        }
    }
    // origin_b = Vel2DivMatrix * DirichletBoundaryMatrix * all_velocity;
    A.setFromTriplets(triplets.begin(), triplets.end());
    // A = PoissonMatrix;
    Eigen::ConjugateGradient<SparseMatrix> solver;
    solver.setTolerance(1e-6);
    // solver.setMaxIterations(20);
    solver.compute(A);
    // std::cout << "pre linear solve" << std::endl;
    origin_px = solver.solve(origin_b);
    // std::cout << "post linear solve" << std::endl;
    std::cout << "origin_b.norm() = " << origin_b.norm() << std::endl;
    std::cout << "origin_px.norm() = " << origin_px.norm() << std::endl;
    for(int i=0;i<_texwidth;i++){
        for(int j=0;j<_texheight;j++){
            for(int k=0;k<_texdepth;k++){
                // pressure.set_volume_value(i,j,k, origin_px(i+j*_texwidth+k*_texwidth*_texheight));
                // pressure.set_volume_value(i,j,k, px(i+j*_texwidth+k*_texwidth*_texheight));
                // p.value[i][j][k] = px(i+j*_texwidth+k*_texwidth*_texheight);
                pressure.set_volume_value(i,j,k, origin_px(resequence3to1(i,j,k,_texwidth,_texheight,_texdepth)));
            }
        }
    }
    pressure.swap_src_dst();
    for(int i=1; i<_texwidth;i++){
        for(int j=0;j<_texheight;j++){
            for(int k=0;k<_texdepth;k++)
            {
                // u.value[i][j][k] = u.value[i][j][k] - _dt/(rho_tgt.value[i][j][k] +rho_amb.value[i][j][k])* (p.value[i][j][k]-p.value[i-1][j][k])/_dx;
                float value = x_velocity.get_volume_value(i,j,k) - _dt * (pressure.get_volume_value(i,j,k) - pressure.get_volume_value(i-1,j,k))/_dx;
                x_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    for(int i=0;i<_texwidth;i++){
        for(int j=1;j<_texheight;j++){
            for(int k=0;k<_texdepth;k++)
            {
                // v.value[i][j][k] = v.value[i][j][k] - _dt/(rho_tgt.value[i][j][k] +rho_amb.value[i][j][k]) * (p.value[i][j][k]-p.value[i][j-1][k])/_dx;
                float value = y_velocity.get_volume_value(i,j,k) - _dt * (pressure.get_volume_value(i,j,k) - pressure.get_volume_value(i,j-1,k))/_dx;
                y_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    for(int i=0;i<_texwidth;i++){
        for(int j=0;j<_texheight;j++){
            for(int k=1;k<_texdepth;k++){
                // w.value[i][j][k] = w.value[i][j][k] - _dt/(rho_tgt.value[i][j][k] +rho_amb.value[i][j][k]) * (p.value[i][j][k]-p.value[i][j][k-1])/_dx;
                float value = z_velocity.get_volume_value(i,j,k) - _dt * (pressure.get_volume_value(i,j,k) - pressure.get_volume_value(i,j,k-1))/_dx;
                z_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    x_velocity.swap_src_dst();
    y_velocity.swap_src_dst();
    z_velocity.swap_src_dst();
    init_all_velocity();
}

void Simulator::project(){
    Eigen::VectorXf b = Eigen::VectorXf::Zero(_texwidth*_texheight*_texdepth);
    Eigen::ConjugateGradient<SparseMatrix> solver;
    solver.setTolerance(1e-6);//下限は1e-6
    // solver.setMaxIterations(20);//設定すると精度が足りないかも
    b = Vel2DivMatrix * DirichletBoundaryMatrix * all_velocity;
    write_exact_solution(b_all_frame,b);
    solver.compute(_dt/(_dx*_dx) * PoissonMatrix);
    px = solver.solveWithGuess(b,px);
    // px = solver.solve(b);
    all_velocity = DirichletBoundaryMatrix * all_velocity - _dt/(_dx)*Pressure2VelocityMatrix * px;
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

    // for(int i=1;i<_texwidth-1;i++){
    //     for(int j=1;j<_texheight-1;j++){
    //         for(int k=1;k<_texdepth-1;k++){
    //             f.value[i][j][k] += getConfinement(i, j, k);
    //         }
    //     }
    // }
    //x
    for(unsigned int i=1;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // u.value[i][j][k] += dt*(f.value[i-1][j][k].x() + f.value[i][j][k].x())/2;
                float value =  x_velocity.get_volume_value(i,j,k);
                value += _dt * ( x_force.get_volume_value(i-1,j,k) + x_force.get_volume_value(i,j,k) )/2;
                x_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    //y
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=1;j<_texheight;j++){
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
            for(unsigned int k=1;k<_texdepth;k++){
                // w.value[i][j][k] += dt*(f.value[i][j][k-1].z() + f.value[i][j][k].z())/2;
                float value =  z_velocity.get_volume_value(i,j,k);
                value += _dt * ( z_force.get_volume_value(i,j,k-1) + z_force.get_volume_value(i,j,k) )/2;
                z_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    x_velocity.swap_src_dst();
    y_velocity.swap_src_dst();
    z_velocity.swap_src_dst();
    // y_velocity.print_src();
}

//snapshotPOD
void Simulator::write_snapshot(Eigen::MatrixXf &mat, Eigen::VectorXf &snap)
{
    assert(mat.cols != snap.size || mat.rows > _timestamp);
    if(_timestamp < _discard_flame)return;
    // if(_timestamp % _snap_num == 0)mat.row(timestamp) = snap;
    // if(_timestamp % _delta_snap == 0)mat.row(_timestamp / _delta_snap) = snap;
    if((_timestamp - _discard_flame)% _delta_snap == 0)mat.col((_timestamp - _discard_flame)/ _delta_snap) = snap;
}

void Simulator::write_exact_solution(Eigen::MatrixXf &mat, Eigen::VectorXf &snap)
{
    // if(_timestamp < _discard_flame)return;
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

Eigen::MatrixXf cal_PODBasis(Eigen::MatrixXf &SnapShot)
{
    int m = SnapShot.rows();
    int n = SnapShot.cols();
    // Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf>solver(SnapShot.transpose() * SnapShot);
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf>solver(SnapShot * SnapShot.transpose());
    Eigen::MatrixXf Basis(m,n);
    // Basis = solver.eigenvectors();
    for(int i=0;i<n;++i)
    {
        Basis.col(i) = solver.eigenvectors().col(i);
    }
    return Basis;
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
    // U2 = cal_PODBasis(U2_SnapShot);
    // std::cout << "norm, reduce_dimention = " << U2.norm() << ", " << _reduce_dimention << std::endl;
    std::cout << "orthogonomality : " << 
    (U2.transpose() * U2 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;
    // std::cout << U2.transpose() * U2 << std::endl;

    std::cout << "U3" << std::endl;
    U3 = cal_Basis(U3_SnapShot,_reduce_dimention,_threshold);
    // U3 = cal_PODBasis(U3_SnapShot);
    // std::cout << "norm, reduce_dimention = " << U3.norm() << ", " << _reduce_dimention << std::endl;
    std::cout << "orthogonomality : " << 
    (U3.transpose() * U3 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;

    std::cout << "P" << std::endl;
    P = cal_Basis(P_SnapShot,_reduce_dimention,_threshold);
    // P = cal_PODBasis(P_SnapShot);
    // std::cout << "norm, reduce_dimention = " << P.norm() << ", " << _reduce_dimention << std::endl;
    std::cout << "orthogonomality : " << 
    (P.transpose() * P - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;
}

void Simulator::getReducedLinearOperator()
{
    reduced_Vel2DivMatrix = P.transpose() * _sub_dt/(_dx)*Vel2DivMatrix * U2;
    reduced_PoissonMatrix = P.transpose() * _sub_dt/(_dx*_dx) * PoissonMatrix * P;
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
            if( reduced_PoissonMatrix(i,j) - reduced_PoissonMatrix(j,i) > 1e-3 )is_symmetric = false;
        }
    }
    if(!is_symmetric)std::cout << "non symmetric matrix" << std::endl;
    if(is_positive_definite && is_symmetric)std::cout << "reduced_X is symmetric positive definite matrix" << std::endl;
    // std::cout << "reduced_X" << std::endl << reduced_PoissonMatrix << std::endl;
    // std::cout << "(X - P X' P**T) = " << (PoissonMatrix - P * reduced_PoissonMatrix * P.transpose()) << std::endl;
    // std::cout << "(P^T X P)^-1.det = " << reduced_PoissonMatrix.determinant() << std::endl;
    // std::cout << "(P^T X P)^-1 = " << std::endl << reduced_PoissonMatrix.inverse() << std::endl;
    // std::cout << "((P^T X P) * (P^T X P)^-1 - I).norm() = " << std::endl <<
    // (reduced_PoissonMatrix * reduced_PoissonMatrix.inverse() - Eigen::MatrixXf::Identity(reduced_PoissonMatrix.rows(),reduced_PoissonMatrix.cols())).norm() << std::endl;
    // std::cout << "reduced_W size" << std::endl << 
    // "rows,cols = " << reduced_Vel2DivMatrix.rows() << "," << reduced_Vel2DivMatrix.cols() << std::endl;
    // std::cout << "reduced_X size" << std::endl << 
    // "rows,cols = " << reduced_PoissonMatrix.rows() << "," << reduced_PoissonMatrix.cols() << std::endl;
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
    std::cout << "outputbasis" << std::endl;
    std::cout << basis_foldername << std::endl;
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
    float singularity_threshold = 1e-3;
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
    //特異値
    Eigen::VectorXf singular_values = svd.singularValues();
    Eigen::MatrixXf singular_value_matrix = singular_values.asDiagonal();
    for(unsigned int i=0;i<singular_values.size();++i)
    {
        if(singular_values(i) < singularity_threshold)
        {
            if(reduce_dimention < i)reduce_dimention = i;
            break;
        }
    }
    float singularity = *singular_values.begin() / *(singular_values.end() - 1);
    // std::cout << "singular value" << std::endl << singular_values.transpose() << std::endl;
    if(singularity < singularity_threshold)std::cout << "worning!! : singularity is too large : "<< singularity << std::endl;
    // std::cout << "max,min : " << *singular_values.begin() << "," << *(singular_values.end() - 1) << std::endl;
    // Eigen::MatrixXf Basis = HhQR.matrixQ()*svd.matrixU();
    Eigen::MatrixXf Basis(m,n);

    // Eigen::MatrixXf Basis(m,reduce_dimention);
    Basis = HhQR.householderQ()*svd.matrixU(); //m * n
    // Basis = HhQR.householderQ()*svd.matrixU()*singular_value_matrix; //m * n
    std::cout << "Basis size : " << Basis.rows() << "," << Basis.cols() << std::endl;
    // std::cout << "m , n : " << m << "," << n << std::endl;

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
    for(_timestamp = 0; _timestamp< (_dt/_sub_dt) * _flame_num; ++_timestamp)
    {
        subspace_oneloop();
    }
    std::cout << std::endl;
}

void Simulator::subspace_oneloop()
{
    init_density(TGT_DENSITY);
    init_templature(TGT_TEMPLATURE);
    //nonlinear
    // std::cout << "sub_calForce" << std::endl;
    addForce();
    // std::cout << "U0 snap norm = " << all_velocity.norm() << std::endl;
    //U0
    // std::cout << "U0 snap norm = " << U0_SnapShot.row(_timestamp).norm() << std::endl;
    faceAdvect();
    //linear
    init_all_velocity();

    //U1
    //Diffusion
    //U2
    // std::cout << "exact, reduce : " << U2_all_frame.col(_timestamp).norm() << "," <<  (U2 * reduced_all_velocity).norm() << std::endl;
    // if(_timestamp < _discard_flame)
    // std::cout << "U2 restore error = " << (all_velocity - U2 * (U2.transpose() * all_velocity)).norm() / all_velocity.norm() << std::endl;
    reduced_all_velocity = U2.transpose() * all_velocity;
    subspace_project();
    std::cout << "U3 : " << (U3_all_frame.col(_timestamp) - U3 * reduced_all_velocity).norm() / U3_all_frame.col(_timestamp).norm() << std::endl;
    // std::cout << "exact, reduce : " << U3_all_frame.col(_timestamp).norm() << "," <<  (U3 * reduced_all_velocity).norm() << std::endl;
    // std::cout << "P  : " << ( P_all_frame.col(_timestamp) - P * reduced_px).norm() / P_all_frame.col(_timestamp).norm() << std::endl;
    // std::cout << "exact, reduce : " << P_all_frame.col(_timestamp).norm() << "," <<  (P * reduced_px).norm() << std::endl;
    //nonlinear
    //U3
    centerAdvect(templature);
    centerAdvect(density_tgt);
    centerAdvect(density_amb);
    output_txt(_timestamp);
}

void Simulator::subspace_project(){
    Eigen::VectorXf b;
    Eigen::ConjugateGradient<Eigen::MatrixXf> solver;
    solver.setTolerance(1e-6);
    b = reduced_Vel2DivMatrix * reduced_DirichletBoundaryMatrix * reduced_all_velocity;
    solver.compute(reduced_PoissonMatrix);
    reduced_px = solver.solveWithGuess(b, reduced_px);
    reduced_all_velocity = reduced_DirichletBoundaryMatrix * reduced_all_velocity - reduced_Pressure2VelocityMatrix * reduced_px;
    all_velocity = U3 * reduced_all_velocity;
    all2xyz();
}

void Simulator::devided_subspace_execute()
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
        devided_subspace_oneloop();
    }
}

void Simulator::devided_subspace_oneloop()
{
    init_density(TGT_DENSITY);
    init_templature(TGT_TEMPLATURE);
    //nonlinear
    // std::cout << "sub_calForce" << std::endl;
    addForce();
    std::cout << "dev_sub_addForce" << std::endl;
    init_all_velocity();
    std::cout << "dev_sub_init_all_velocity" << std::endl;
    faceAdvect();
    //linear
    init_all_velocity();

    //U1
    //Diffusion
    //U2
    reduced_all_velocity = U2.transpose() * all_velocity;
    reduced_all_velocity = U2.transpose() * U2_all_frame.col(_timestamp);
    subspace_project();
    std::cout << "U3 : " << (U3_all_frame.col(_timestamp) - U3 * reduced_all_velocity).norm() / U3_all_frame.col(_timestamp).norm() << std::endl;
    std::cout << "exact, reduce : " << U3_all_frame.col(_timestamp).norm() << "," <<  (U3 * reduced_all_velocity).norm() << std::endl;
    std::cout << "P  : " << ( P_all_frame.col(_timestamp) - P * reduced_px).norm() / P_all_frame.col(_timestamp).norm() << std::endl;
    std::cout << "exact, reduce : " << P_all_frame.col(_timestamp).norm() << "," <<  (P * reduced_px).norm() << std::endl;
    //nonlinear
    //U3
    // times.push_back(TD.endTimer());
    std::cout << "dev_sub_project" << std::endl;
    centerAdvect(templature);
    // std::cout << "centerAdvectTemp" << std::endl;
    centerAdvect(density_tgt);
    centerAdvect(density_amb);
    output_txt(_timestamp);
    // std::cout << "centerAdvectRho" << std::endl;
}

void Simulator::devided_subspace_project()
{

}

void Simulator::getDevidedBasis()
{
    unsigned int devided_reduce_dimention = _snap_num/_snap_devide_num;
    unsigned int devided_step = _snap_num/_snap_devide_num;
    Eigen::MatrixXf tmp_U0;
    Eigen::MatrixXf tmp_U1;
    Eigen::MatrixXf tmp_U2;
    Eigen::MatrixXf tmp_U3;
    Eigen::MatrixXf tmp_P;
    Eigen::MatrixXf tmp_U0_SnapShot(U0_SnapShot.rows(),devided_step);
    Eigen::MatrixXf tmp_U1_SnapShot(U1_SnapShot.rows(),devided_step);
    Eigen::MatrixXf tmp_U2_SnapShot(U2_SnapShot.rows(),devided_step);
    Eigen::MatrixXf tmp_U3_SnapShot(U3_SnapShot.rows(),devided_step);
    Eigen::MatrixXf tmp_P_SnapShot(P_SnapShot.rows(),devided_step);
    
    for(unsigned int i=0;i<_snap_devide_num;++i)
    {
        for(unsigned int j=0;j<devided_step;++j)
        {
            tmp_U0_SnapShot.col(j) = U0_SnapShot.col(i*devided_step + j);
            tmp_U1_SnapShot.col(j) = U1_SnapShot.col(i*devided_step + j);
            tmp_U2_SnapShot.col(j) = U2_SnapShot.col(i*devided_step + j);
            tmp_U3_SnapShot.col(j) = U3_SnapShot.col(i*devided_step + j);
            tmp_P_SnapShot.col(j) = P_SnapShot.col(i*devided_step + j);
        }
        tmp_U0 = cal_Basis(tmp_U0_SnapShot,devided_reduce_dimention,_threshold);
        tmp_U1 = cal_Basis(tmp_U1_SnapShot,devided_reduce_dimention,_threshold);
        tmp_U2 = cal_Basis(tmp_U2_SnapShot,devided_reduce_dimention,_threshold);
        tmp_U3 = cal_Basis(tmp_U3_SnapShot,devided_reduce_dimention,_threshold);
        tmp_P = cal_Basis(tmp_P_SnapShot,devided_reduce_dimention,_threshold);
        devided_U0.push_back(tmp_U0);
        devided_U1.push_back(tmp_U1);
        devided_U2.push_back(tmp_U2);
        devided_U3.push_back(tmp_U3);
        devided_P.push_back(tmp_P);
    }
}

void Simulator::getDevidedReducedLinearOperator()
{
    Eigen::MatrixXf tmp_reduced_Vel2DivMatrix;
    Eigen::MatrixXf tmp_reduced_PoissonMatrix;
    Eigen::MatrixXf tmp_reduced_Pressure2VelocityMatrix;
    Eigen::MatrixXf tmp_reduced_DirichletBoundaryMatrix;
    for(unsigned int i=0;i<_snap_devide_num;++i)
    {
        tmp_reduced_Vel2DivMatrix = devided_P[i].transpose() * Vel2DivMatrix * devided_U2[i];
        tmp_reduced_PoissonMatrix = devided_P[i].transpose() * PoissonMatrix * devided_P[i];
        tmp_reduced_Pressure2VelocityMatrix = devided_U3[i].transpose() * Pressure2VelocityMatrix * devided_P[i];
        tmp_reduced_DirichletBoundaryMatrix = devided_U2[i].transpose() * DirichletBoundaryMatrix * devided_U2[i];
        devided_reduced_Vel2DivMatrix.push_back(tmp_reduced_Vel2DivMatrix);//W
        devided_reduced_PoissonMatrix.push_back(tmp_reduced_PoissonMatrix);//X
        devided_reduced_Pressure2VelocityMatrix.push_back(tmp_reduced_Pressure2VelocityMatrix);//Y
        devided_reduced_DirichletBoundaryMatrix.push_back(tmp_reduced_DirichletBoundaryMatrix);//D
    }
}

Eigen::MatrixXf Simulator::getRowsCorrespondPoint(Eigen::MatrixXf &Basis, unsigned int x,unsigned int y, unsigned int z)
{
    unsigned int size = (_texwidth + 1) * _texheight * _texdepth;
    Eigen::MatrixXf ret(3,Basis.cols());
    ret.row(0) = Basis.row(resequence3to1(x,y,z,_texwidth+1,_texheight,_texdepth));
    ret.row(1) = Basis.row(size + resequence3to1(x,y,z,_texwidth,_texheight+1,_texdepth));
    ret.row(2) = Basis.row(2*size + resequence3to1(x,y,z,_texwidth,_texheight,_texdepth+1));
    return ret;
}

