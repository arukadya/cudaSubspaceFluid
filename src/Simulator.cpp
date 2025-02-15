#include "Simulator.hpp"
// #include "Simulator.cuh"
void Simulator::plot(std::string &plot_fileName,unsigned int id)
{
    origin_velocity = U3_all_frame.col(id);
    plotVelocity(_texwidth,_texheight,_texdepth,all_velocity,origin_velocity,plot_fileName);
}

void Simulator::load_vel(unsigned int id)
{
    all_velocity = U3_all_frame.col(id);
}

float* Simulator::get_currentTexture()
{
    return density_tgt.src_texture;
}
//Simulator
void Simulator::inputTXT(std::string &InputFileName,Slab &density)
{
    FILE *ifp = fopen(InputFileName.c_str(),"r");
    for(unsigned int k=0;k<_texdepth;++k){
        for(unsigned int j=0;j<_texheight;++j){
            for(unsigned int i=0;i<_texwidth;++i){
                float value;
                if(fscanf(ifp, "%f", &value));
                density.src_texture[resequence3to1(i, _texheight - j, k, _texwidth, _texheight, _texdepth)] = value;
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
    // if(_timestamp < _flame_num / 2)addForce(_dt);
    addForce(_dt);
    init_all_velocity();
    //U0
    write_snapshot(U0_Snapshot, all_velocity);
    // write_exact_solution(U0_all_frame, all_velocity);
    faceAdvect();
    //linear
    init_all_velocity();
    write_snapshot(U1_Snapshot, all_velocity);
    // write_exact_solution(U1_all_frame, all_velocity);
    //U1
    //Diffusion
    all_velocity = DiffusionMatrix * DirichletBoundaryMatrix * all_velocity;
    //U2
    write_snapshot(U2_Snapshot, all_velocity);
    // write_exact_solution(U2_all_frame, all_velocity);
    project();
    write_snapshot(U3_Snapshot, all_velocity);
    write_snapshot(P_Snapshot, px);
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
    output_txt(origin_density_floder_name ,_timestamp);
    ++_timestamp;
}

void Simulator::output_txt(std::string &density_floder_name ,unsigned int id)
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
    // for(int cnt = 0;cnt < 8;++cnt)
    // {
    //     if(isnan(f(cnt)))std::cout << "isnan" << std::endl;
    // }
    Eigen::Vector<float ,8> c = 
    {
        (1-s)*(1-t)*(1-u),s*(1-t)*(1-u),s*t*(1-u),(1-s)*t*(1-u),
        (1-s)*(1-t)*u,s*(1-t)*u,s*t*u,(1-s)*t*u
    };
    return f.dot(c);
}

// float Simulator::TriLinearInterporation(float x,float y,float z,unsigned int nx,unsigned int ny,unsigned int nz,Eigen::VectorXf &val)
// {
//     double fix_x = fmax(0.0, fmin(nx-1-1e-6,x/_dx));
//     double fix_y = fmax(0.0, fmin(ny-1-1e-6,y/_dx));
//     double fix_z = fmax(0.0, fmin(nz-1-1e-6,z/_dx));
//     int i = fix_x;int j = fix_y;int k = fix_z;
//     float s = fix_x-i;float t = fix_y-j;float u = fix_z-k;
//     Eigen::Vector<float ,8> f = {
//         val(resequence3to1(i,j,k)),
//         val(resequence3to1(i+1,j,k)),
//         val(resequence3to1(i+1,j+1,k)),
//         val(resequence3to1(i,j+1,k)),
//         val(resequence3to1(i,j,k+1)),
//         val(resequence3to1(i+1,j,k+1)),
//         val(resequence3to1(i+1,j+1,k+1)),
//         val(resequence3to1(i,j+1,k+1))
//         // val.get_volume_value(i,j,k),
//         // val.get_volume_value(i+1,j,k),
//         // val.get_volume_value(i+1,j+1,k),
//         // val.get_volume_value(i,j+1,k),
//         // val.get_volume_value(i,j,k+1),
//         // val.get_volume_value(i+1,j,k+1),
//         // val.get_volume_value(i+1,j+1,k+1),
//         // val.get_volume_value(i,j+1,k+1)
//     };
//     Eigen::Vector<float ,8> c = 
//     {
//         (1-s)*(1-t)*(1-u),s*(1-t)*(1-u),s*t*(1-u),(1-s)*t*(1-u),
//         (1-s)*(1-t)*u,s*(1-t)*u,s*t*u,(1-s)*t*u
//     };
//     return f.dot(c);
// }

void Simulator::faceAdvect(){
    float size = (_texwidth + 1) * _texheight * _texdepth;
    for(unsigned int i=1;i<x_velocity._width-1;++i){
        for(unsigned int j=0;j<x_velocity._height;++j){
            for(unsigned int k=0;k<x_velocity._depth;++k){
                unsigned int x_vel_id = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                if(DirichletBoundaryMatrix.coeff(x_vel_id,x_vel_id) == 0)continue;
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
                unsigned int y_vel_id = size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                if(DirichletBoundaryMatrix.coeff(y_vel_id,y_vel_id) == 0)continue;
                float x = (i+0.5)*_dx;float y = j*_dx;float z = (k+0.5)*_dx;
                float adv_x = x - _dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
                float adv_y = y - _dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
                float adv_z = z - _dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
                float value = TriLinearInterporation(adv_x - 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);
                y_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    for(unsigned int i=0;i<z_velocity._width;++i){
        for(unsigned int j=0;j<z_velocity._height;++j){
            for(unsigned int k=1;k<z_velocity._depth-1;++k){
                unsigned int z_vel_id = 2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                if(DirichletBoundaryMatrix.coeff(z_vel_id,z_vel_id) == 0)continue;
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
                // if(_situation == 1)
                // {
                //     unsigned int range_x = _texwidth/8;
                //     unsigned int range_y = _texheight/8;
                //     bool is_inrange_x = (_texwidth / 2 - range_x < i && i < _texwidth / 2 + range_x);
                //     bool is_inrange_y = (_texheight / 2 - range_y < j && j < _texheight / 2 + range_y);
                //     if(is_inrange_x && is_inrange_y)continue;
                // }
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

void Simulator::calPoissonMatrix(float dt)
{
    std::vector<Triplet> triplets;
    // y_velocity.print_src();
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                float scale = dt/(_dx*_dx);
                // float scale = 1.0;
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
    if(_situation == 0)
    {
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
    }
    if(_situation == 1)
    {
        unsigned int range_x = _texwidth/8;
        unsigned int range_y = _texheight/8;
        for(unsigned int i=0;i<_texwidth+1;i++){
            for(unsigned int j=0;j<_texheight;j++){
                for(unsigned int k=0;k<_texdepth;k++){
                    bool is_inrange_x = (_texwidth / 2 - range_x < i && i < _texwidth / 2 + range_x);
                    bool is_inrange_y = (_texheight / 2 - range_y < j && j < _texheight / 2 + range_y);
                    unsigned int x_vel_id = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                    if((is_inrange_x && is_inrange_y) || i ==0 || i == _texwidth )triplets.emplace_back(x_vel_id,x_vel_id, 0);
                    else triplets.emplace_back(x_vel_id,x_vel_id, 1);
                }
            }
        }
        for(unsigned int i=0;i<_texwidth;i++){
            for(unsigned int j=0;j<_texheight+1;j++){
                for(unsigned int k=0;k<_texdepth;k++){
                    bool is_inrange_x = (_texwidth / 2 - range_x < i && i < _texwidth / 2 + range_x);
                    bool is_inrange_y = (_texheight / 2 - range_y < j && j < _texheight / 2 + range_y);
                    unsigned int y_vel_id = size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                    if((is_inrange_x && is_inrange_y)  || j ==0 || j == _texheight)triplets.emplace_back(y_vel_id,y_vel_id, 0);
                    else triplets.emplace_back(y_vel_id,y_vel_id, 1);
                }
            }
        }
        for(unsigned int i=0;i<_texwidth;i++){
            for(unsigned int j=0;j<_texheight;j++){
                for(unsigned int k=0;k<_texdepth+1;k++){
                    bool is_inrange_x = (_texwidth / 2 - range_x < i && i < _texwidth / 2 + range_x);
                    bool is_inrange_y = (_texheight / 2 - range_y < j && j < _texheight / 2 + range_y);
                    unsigned int z_vel_id = 2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                    // if((_texdepth / 2 - range_z < k && k < _texdepth / 2 + range_z ) || k ==0 || k == _texdepth)triplets.emplace_back(z_vel_id,z_vel_id, 0);
                    if((is_inrange_x && is_inrange_y) || k ==0 || k == _texdepth)triplets.emplace_back(z_vel_id,z_vel_id, 0);
                    else triplets.emplace_back(z_vel_id,z_vel_id, 1);
                }
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

void Simulator::calDiffusionMatrix(float dt)
{
    std::vector<Triplet> triplets;
    float size = (_texwidth + 1) * _texheight * _texdepth;
    float scale = _nu * dt / (4 * _dx* _dx);
    //x
    for(unsigned int i=1;i<_texwidth;i++){
        for(unsigned int j=1;j<_texheight-1;j++){
            for(unsigned int k=1;k<_texdepth-1;k++){
                // float D[6] = {1.0,1.0,-1.0,-1.0,-1.0,1.0};//周囲6方向に向かって働く、圧力の向き
                std::vector<int> F = {i<_texwidth,j<_texheight-1,i>0,j>0,k>0,k<_texdepth-1};
                //速度の境界値は0に設定しているので、境界成分に対応する係数は0でよい
                unsigned int center_id = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                unsigned int x_pre_id  = resequence3to1(i-1,j,k,_texwidth+1,_texheight,_texdepth);
                unsigned int x_post_id = resequence3to1(i+1,j,k,_texwidth+1,_texheight,_texdepth);
                unsigned int y_pre_id  = resequence3to1(i,j-1,k,_texwidth+1,_texheight,_texdepth);
                unsigned int y_post_id = resequence3to1(i,j+1,k,_texwidth+1,_texheight,_texdepth);
                unsigned int z_pre_id  = resequence3to1(i,j,k-1,_texwidth+1,_texheight,_texdepth);
                unsigned int z_post_id = resequence3to1(i,j,k+1,_texwidth+1,_texheight,_texdepth);

                if(F[0])triplets.emplace_back(center_id, x_post_id, scale);
                if(F[1])triplets.emplace_back(center_id, y_post_id, scale);
                if(F[2])triplets.emplace_back(center_id, x_pre_id,  scale);
                if(F[3])triplets.emplace_back(center_id, y_pre_id,  scale);
                if(F[4])triplets.emplace_back(center_id, z_pre_id,  scale);
                if(F[5])triplets.emplace_back(center_id, z_post_id, scale);
                triplets.emplace_back(center_id,center_id, 1-6.0*scale);
                    // triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
                    // triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
            }
        }
    }

    //y
    for(unsigned int i=1;i<_texwidth-1;i++){
        for(unsigned int j=1;j<_texheight;j++){
            for(unsigned int k=1;k<_texdepth-1;k++){
                // float D[6] = {1.0,1.0,-1.0,-1.0,-1.0,1.0};//周囲6方向に向かって働く、圧力の向き
                std::vector<int> F = {i<_texwidth-1,j<_texheight,i>0,j>0,k>0,k<_texdepth-1};
                //速度の境界値は0に設定しているので、境界成分に対応する係数は0でよい
                unsigned int center_id = size + resequence3to1(i,  j,k,_texwidth,_texheight+1,_texdepth);
                unsigned int x_pre_id  = size + resequence3to1(i-1,j,k,_texwidth,_texheight+1,_texdepth);
                unsigned int x_post_id = size + resequence3to1(i+1,j,k,_texwidth,_texheight+1,_texdepth);
                unsigned int y_pre_id  = size + resequence3to1(i,j-1,k,_texwidth,_texheight+1,_texdepth);
                unsigned int y_post_id = size + resequence3to1(i,j+1,k,_texwidth,_texheight+1,_texdepth);
                unsigned int z_pre_id  = size + resequence3to1(i,j,k-1,_texwidth,_texheight+1,_texdepth);
                unsigned int z_post_id = size + resequence3to1(i,j,k+1,_texwidth,_texheight+1,_texdepth);

                if(F[0])triplets.emplace_back(center_id, x_post_id, scale);
                if(F[1])triplets.emplace_back(center_id, y_post_id, scale);
                if(F[2])triplets.emplace_back(center_id, x_pre_id,  scale);
                if(F[3])triplets.emplace_back(center_id, y_pre_id,  scale);
                if(F[4])triplets.emplace_back(center_id, z_pre_id,  scale);
                if(F[5])triplets.emplace_back(center_id, z_post_id, scale);
                triplets.emplace_back(center_id,center_id, 1-6.0*scale);
                    // triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
                    // triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
            }
        }
    }

    //z
    for(unsigned int i=1;i<_texwidth-1;i++){
        for(unsigned int j=1;j<_texheight-1;j++){
            for(unsigned int k=1;k<_texdepth;k++){
                // float D[6] = {1.0,1.0,-1.0,-1.0,-1.0,1.0};//周囲6方向に向かって働く、圧力の向き
                std::vector<int> F = {i<_texwidth-1,j<_texheight-1,i>0,j>0,k>0,k<_texdepth};
                //速度の境界値は0に設定しているので、境界成分に対応する係数は0でよい
                unsigned int center_id = 2*size + resequence3to1(i,  j,k,_texwidth,_texheight,_texdepth+1);
                unsigned int x_pre_id  = 2*size + resequence3to1(i-1,j,k,_texwidth,_texheight,_texdepth+1);
                unsigned int x_post_id = 2*size + resequence3to1(i+1,j,k,_texwidth,_texheight,_texdepth+1);
                unsigned int y_pre_id  = 2*size + resequence3to1(i,j-1,k,_texwidth,_texheight,_texdepth+1);
                unsigned int y_post_id = 2*size + resequence3to1(i,j+1,k,_texwidth,_texheight,_texdepth+1);
                unsigned int z_pre_id  = 2*size + resequence3to1(i,j,k-1,_texwidth,_texheight,_texdepth+1);
                unsigned int z_post_id = 2*size + resequence3to1(i,j,k+1,_texwidth,_texheight,_texdepth+1);

                if(F[0])triplets.emplace_back(center_id, x_post_id, scale);
                if(F[1])triplets.emplace_back(center_id, y_post_id, scale);
                if(F[2])triplets.emplace_back(center_id, x_pre_id,  scale);
                if(F[3])triplets.emplace_back(center_id, y_pre_id,  scale);
                if(F[4])triplets.emplace_back(center_id, z_pre_id,  scale);
                if(F[5])triplets.emplace_back(center_id, z_post_id, scale);
                triplets.emplace_back(center_id,center_id, 1-6.0*scale);
                    // triplets.emplace_back(size + i+j*_texwidth+k*_texwidth*_texheight, size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
                    // triplets.emplace_back(2*size + i+j*_texwidth+k*_texwidth*_texheight, 2*size + i+j*_texwidth+(k+2)*_texwidth*_texheight, scale);
            }
        }
    }
    DiffusionMatrix.setFromTriplets(triplets.begin(), triplets.end());
}

void Simulator::calPressure2VelocityMatrix(float dt)
{
    std::vector<Triplet> triplets;
    float size = (_texwidth + 1) * _texheight * _texdepth;
    float scale = dt/(_dx);
    // float scale = 1.0;
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
                // unsigned int grid_id = resequence3to1(i,j,k,_texdepth,_texheight,_texdepth);
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
    solver.setTolerance(1e-6);
    // if(_texwidth == 64)solver.setTolerance(1e-6);//64下限は1e-6 128下限は1e-4
    // if(_texwidth == 128)solver.setTolerance(1e-5);
    // if(_texwidth == 256)solver.setTolerance(1e-4);
    solver.setMaxIterations(20);//設定すると精度が足りないかも
    b = Vel2DivMatrix * all_velocity;
    solver.compute(PoissonMatrix);
    px = solver.solveWithGuess(b,px);
    // std::cout << "iteration = " << solver.iterations() << std::endl;
    // px = solver.solve(b);
    all_velocity = DirichletBoundaryMatrix*(all_velocity - Pressure2VelocityMatrix * px);
    all2xyz();
}

void Simulator::addForce(float dt)
{
    // std::cout << "begin add force" << std::endl;
    float size = (_texwidth + 1) * _texheight * _texdepth;
    calConfinent();
    // std::cout << "calConf" << std::endl;
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // f.value[i][j][k] = getBuoyanacy(i, j, k);
                Eigen::Vector3f buoy = getBuoyanacy(i, j, k);
                Eigen::Vector3f conf = getConfinent(i, j, k);
                Eigen::Vector3f force = buoy + conf;
                // std::cout << i << "," << j << "," << k  << " : buoy" << std::endl << buoy.transpose() << std::endl; 
                // if(conf.norm() > 1e-5)std::cout << i << "," << j << "," << k  << " : conf" << std::endl << conf.transpose() << std::endl;
                // if(isnan(conf.norm()))std::cout << i << "," << j << "," << k  << " : nan" << std::endl;
                // float fx = buoy.x();
                // float fy = buoy.y();
                // float fz = buoy.z();
                float fx = force.x();
                float fy = force.y();
                float fz = force.z();
                x_force.set_volume_value(i,j,k,fx);
                y_force.set_volume_value(i,j,k,fy);
                z_force.set_volume_value(i,j,k,fz);
            }
        }
    }
    x_force.swap_src_dst();
    y_force.swap_src_dst();
    z_force.swap_src_dst();
    //x
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // u.value[i][j][k] += dt*(f.value[i-1][j][k].x() + f.value[i][j][k].x())/2;
                unsigned int x_vel_id = resequence3to1(i,j,k,_texwidth+1,_texheight,_texdepth);
                if(DirichletBoundaryMatrix.coeff(x_vel_id,x_vel_id) == 0)continue;
                float value =  x_velocity.get_volume_value(i,j,k);
                value += dt * ( x_force.get_volume_value(i,j,k) + x_force.get_volume_value(i+1,j,k) )/2;
                x_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    //y
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // v.value[i][j][k] += dt*(f.value[i][j-1][k].y() + f.value[i][j][k].y())/2;
                unsigned int y_vel_id = size + resequence3to1(i,j,k,_texwidth,_texheight+1,_texdepth);
                if(DirichletBoundaryMatrix.coeff(y_vel_id,y_vel_id) == 0)continue;
                float value =  y_velocity.get_volume_value(i,j,k);
                value += dt * ( y_force.get_volume_value(i,j,k) + y_force.get_volume_value(i,j+1,k) )/2;
                y_velocity.set_volume_value(i,j,k,value);
            }
        }
    }
    //z
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                // w.value[i][j][k] += dt*(f.value[i][j][k-1].z() + f.value[i][j][k].z())/2;
                unsigned int z_vel_id = 2*size + resequence3to1(i,j,k,_texwidth,_texheight,_texdepth+1);
                if(DirichletBoundaryMatrix.coeff(z_vel_id,z_vel_id) == 0)continue;
                float value =  z_velocity.get_volume_value(i,j,k);
                value += dt * ( z_force.get_volume_value(i,j,k) + z_force.get_volume_value(i,j,k+1) )/2;
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
    // assert(mat.cols() != snap.size() || mat.rows() > _timestamp);
    if(_timestamp < _discard_flame)return;
    // if(_timestamp % _snap_num == 0)mat.row(timestamp) = snap;
    // if(_timestamp % _delta_snap == 0)mat.row(_timestamp / _delta_snap) = snap;
    if((_timestamp - _discard_flame)% _delta_snap == 0)mat.col((_timestamp - _discard_flame)/ _delta_snap) = snap;
}

void Simulator::write_exact_solution(Eigen::MatrixXf &mat, Eigen::VectorXf &snap)
{
    // if(_timestamp < _discard_flame)return;
    // assert(mat.cols() != snap.size() || mat.rows() > _timestamp);
    // if(_timestamp % _snap_num == 0)mat.row(timestamp) = snap;
    // mat.row(_timestamp) = snap;
    mat.col(_timestamp) = snap;
}

void Simulator::calConfinent()
{
    //omega
    for(unsigned int i=0;i<_texwidth;i++){
        for(unsigned int j=0;j<_texheight;j++){
            for(unsigned int k=0;k<_texdepth;k++){
                float sub_x = (x_velocity.get_volume_value(i+1,j,k) - x_velocity.get_volume_value(i,j,k));//rotの分子
                float sub_y = (y_velocity.get_volume_value(i,j+1,k) - y_velocity.get_volume_value(i,j,k));
                float sub_z = (z_velocity.get_volume_value(i,j,k+1) - z_velocity.get_volume_value(i,j,k));
                Eigen::Vector3f sub{(sub_y - sub_z)/_dx, (sub_z - sub_x)/_dx, (sub_x - sub_y)/_dx};//rot
                x_omega.set_volume_value(i,j,k,sub.x());
                y_omega.set_volume_value(i,j,k,sub.y());
                z_omega.set_volume_value(i,j,k,sub.z());
                eta.set_volume_value(i,j,k,sub.norm());
            }
        }
    }
    x_omega.swap_src_dst();
    y_omega.swap_src_dst();
    z_omega.swap_src_dst();
    eta.swap_src_dst();

    //Nの勾配
    for(unsigned int i=1;i<_texwidth-1;i++){
        for(unsigned int j=1;j<_texheight-1;j++){
            for(unsigned int k=1;k<_texdepth-1;k++){
                float sub_x = (eta.get_volume_value(i+1,j,k) - x_velocity.get_volume_value(i-1,j,k));//gradの分子
                float sub_y = (eta.get_volume_value(i,j+1,k) - y_velocity.get_volume_value(i,j-1,k));
                float sub_z = (eta.get_volume_value(i,j,k+1) - z_velocity.get_volume_value(i,j,k-1));
                Eigen::Vector3f sub{(sub_y - sub_z)/(2*_dx), (sub_z - sub_x)/(2*_dx), (sub_x - sub_y)/(2*_dx)};//grad
                N_x.set_volume_value(i,j,k,sub.x()/(1e-5 + sub.norm()));
                N_y.set_volume_value(i,j,k,sub.y()/(1e-5 + sub.norm()));
                N_z.set_volume_value(i,j,k,sub.z()/(1e-5 + sub.norm()));
            }
        }
    }
    N_x.swap_src_dst();
    N_y.swap_src_dst();
    N_z.swap_src_dst();
}

Eigen::Vector3f Simulator::getConfinent(int i,int j,int k)
{
    // float _epsilon = 50.0;
    Eigen::Vector3f ret;
    float nx = N_x.get_volume_value(i,j,k);
    float ny = N_y.get_volume_value(i,j,k);
    float nz = N_z.get_volume_value(i,j,k);
    float ox = x_omega.get_volume_value(i,j,k);
    float oy = y_omega.get_volume_value(i,j,k);
    float oz = z_omega.get_volume_value(i,j,k);
    ret.x()  = ny * oz - nz * oy;
    ret.y()  = nz * ox - nx * oz;
    ret.z()  = nx * oy - ny * ox;
    return _epsilon * _dx * ret;
}

Eigen::Vector3f Simulator::getBuoyanacy(int i,int j, int k){
    Eigen::Vector3f dir_gravity = {0.0,1.0,0.0};
    float rho = density_tgt.get_volume_value(i,j,k);
    float rho_amb = density_amb.get_volume_value(i,j,k);
    float temp = templature.get_volume_value(i,j,k);
    // float value = -(-G0*(rho +rho_amb) + BETA*(temp - AMB_TEMPLATURE));
    return -_dx*(-G0*(rho +rho_amb) + _beta*(temp - AMB_TEMPLATURE))*dir_gravity;
}

Eigen::MatrixXf cal_PODBasis(Eigen::MatrixXf &Snapshot)
{
    int m = Snapshot.rows();
    int n = Snapshot.cols();
    // Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf>solver(Snapshot.transpose() * Snapshot);
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf>solver(Snapshot * Snapshot.transpose());
    Eigen::MatrixXf Basis(m,n);
    // Basis = solver.eigenvectors();
    for(int i=0;i<n;++i)
    {
        Basis.col(i) = solver.eigenvectors().col(i);
    }
    return Basis;
}

void Simulator::getBasisQRSVD(
    Eigen::MatrixXf &devided_U0_Snapshot,
    Eigen::MatrixXf &devided_U1_Snapshot,
    Eigen::MatrixXf &devided_U2_Snapshot,
    Eigen::MatrixXf &devided_U3_Snapshot,
    Eigen::MatrixXf &devided_P_Snapshot,
    Eigen::MatrixXf &devided_U0,
    Eigen::MatrixXf &devided_U1,
    Eigen::MatrixXf &devided_U2,
    Eigen::MatrixXf &devided_U3,
    Eigen::MatrixXf &devided_P
)
{
    Timer timer;
    timer.startWithMessage("getBasisQRSVD");
    _reduce_dimention = devided_P_Snapshot.cols();
    // _reduce_dimention = -1;
    std::cout << "U0" << std::endl;
    devided_U0 = cal_Basis(devided_U0_Snapshot,_reduce_dimention,_singularity_threshold);
    // std::cout << "norm, reduce_dimention = " << U0.norm() << ", " << _reduce_dimention << std::endl;
    // std::cout << "orthogonomality : " << 
    // (U0.transpose() * U0 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;
    std::cout << "U1" << std::endl;
    devided_U1 = cal_Basis(devided_U1_Snapshot,_reduce_dimention,_singularity_threshold);
    // std::cout << "orthogonomality : " << (U1.transpose() * U1 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;
    std::cout << "U2" << std::endl;
    devided_U2 = cal_Basis(devided_U2_Snapshot,_reduce_dimention,_singularity_threshold);
    // U2 = cal_PODBasis(U2_Snapshot);
    // std::cout << "norm, reduce_dimention = " << U2.norm() << ", " << _reduce_dimention << std::endl;
    // std::cout << "orthogonomality : " << (U2.transpose() * U2 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;
    // std::cout << U2.transpose() * U2 << std::endl;

    std::cout << "U3" << std::endl;
    devided_U3 = cal_Basis(devided_U3_Snapshot,_reduce_dimention,_singularity_threshold);
    // U3 = cal_PODBasis(U3_Snapshot);
    // std::cout << "norm, reduce_dimention = " << U3.norm() << ", " << _reduce_dimention << std::endl;
    // std::cout << "orthogonomality : " << (U3.transpose() * U3 - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;

    std::cout << "P" << std::endl;
    devided_P = cal_Basis(devided_P_Snapshot,_reduce_dimention,_singularity_threshold);
    // P = cal_PODBasis(P_Snapshot);
    // std::cout << "norm, reduce_dimention = " << P.norm() << ", " << _reduce_dimention << std::endl;
    // std::cout << "orthogonomality : " << (P.transpose() * P - Eigen::MatrixXf::Identity(_snap_num,_snap_num)).norm() << std::endl;
    basis_time += timer.end();
}

void Simulator::getReducedLinearOperator
(
    Eigen::MatrixXf &devided_U0,
    Eigen::MatrixXf &devided_U1,
    Eigen::MatrixXf &devided_U2,
    Eigen::MatrixXf &devided_U3,
    Eigen::MatrixXf &devided_P
)
{
    calPoissonMatrix(_sub_dt);
    calDiffusionMatrix(_sub_dt);
    calPressure2VelocityMatrix(_sub_dt);
    Timer timer;
    timer.startWithMessage("getReducedLinearOperator");
    reduced_DiffusionMatrix = devided_U2.transpose() * DiffusionMatrix * devided_U1;
    reduced_Vel2DivMatrix = devided_P.transpose() * Vel2DivMatrix * devided_U2;
    reduced_PoissonMatrix = devided_P.transpose() * PoissonMatrix * devided_P;
    reduced_Pressure2VelocityMatrix = devided_U3.transpose() * Pressure2VelocityMatrix * devided_P;
    reduced_DirichletBoundaryMatrix = devided_U1.transpose() * DirichletBoundaryMatrix * devided_U1;
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
    projection_time += timer.end();
}

void Simulator::output_exact()
{
    std::filesystem::create_directories("exact");
    int n = _texdepth * _texheight * _texdepth;
    std::string exact_foldername = 
    "exact/n" + std::to_string(n) + "T" + std::to_string(_snap_num); 
    std::filesystem::create_directories(exact_foldername);
    std::string U3exactFileName = exact_foldername + "/U3exact.txt";
    outputMatrix(U3exactFileName,U3_all_frame);
    std::string PexactFileName = exact_foldername + "/Pexact.txt";
    outputMatrix(PexactFileName,P_all_frame);
}

void Simulator::input_exact()
{
    int n = _texdepth * _texheight * _texdepth;
    std::string exact_foldername = 
    "exact/n" + std::to_string(n) + "T" + std::to_string(_snap_num); 
    std::string U3exactFileName = exact_foldername + "/U3exact.txt";
    inputMatrix(U3exactFileName,U3_all_frame);
    // std::string PexactFileName = exact_foldername + "/Pexact.txt";
    // inputMatrix(PexactFileName,P_all_frame);
}

void Simulator::output_Snapshot(unsigned int devided_id,Eigen::MatrixXf &devided_Snapshot)
{
    std::filesystem::create_directories("snapshot");
    int n = _texdepth * _texheight * _texdepth;
    std::string snap_foldername = 
    "snapshot/n" + std::to_string(n) + "T" + std::to_string(_snap_num) + 
    "dev" + std::to_string(_devide_num) + "id" + std::to_string(devided_id); 
    std::cout << "outputsnap" << std::endl;
    std::cout << snap_foldername << std::endl;
    std::filesystem::create_directories(snap_foldername);
    std::string U1snapFileName = snap_foldername + "/U1snap.txt";
    outputMatrix(U1snapFileName,devided_Snapshot);
}

void Simulator::output_Basis(unsigned int devided_id)
{
    std::filesystem::create_directories("basis");
    int n = _texdepth * _texheight * _texdepth;
    // int r = _reduce_dimention;
    int r = _snap_num;
    std::string basis_foldername =
    "basis/n" + std::to_string(n) + "r" + std::to_string(r) + 
    "dev" + std::to_string(_devide_num) +"id" + std::to_string(devided_id); 
    std::cout << "outputbasis" << std::endl;
    std::cout << basis_foldername << std::endl;
    std::filesystem::create_directories(basis_foldername);
    std::string U0FileName = basis_foldername + "/U0.txt";
    std::string U1FileName = basis_foldername + "/U1.txt";
    std::string U2FileName = basis_foldername + "/U2.txt";
    std::string U3FileName = basis_foldername + "/U3.txt";
    std::string PFileName  = basis_foldername +  "/P.txt";
    outputMatrix(U0FileName,U0);
    outputMatrix(U1FileName,U1);
    outputMatrix(U2FileName,U2);
    outputMatrix(U3FileName,U3);
    outputMatrix(PFileName,P);
}

void Simulator::input_Snapshot(unsigned int devided_id)
{
    int n = _texdepth * _texheight * _texdepth;
    std::string snap_foldername = 
    "snapshot/n" + std::to_string(n) + "T" + std::to_string(_snap_num) + 
    "dev" + std::to_string(_devide_num) + "id" + std::to_string(devided_id); 
    std::string U1snapFileName = snap_foldername + "/U1snap.txt";
    n = (_texdepth + 1)* _texheight * _texdepth;
    unsigned int r = _reduce_dimention / _devide_num;
    // Eigen::MatrixXf devided_U1_Snapshot = Eigen::MatrixXf(3*n,r);
    Eigen::MatrixXf devided_U1_Snapshot;
    inputMatrix(U1snapFileName,devided_U1_Snapshot);
    devided_U1_Snapshot_List.push_back(devided_U1_Snapshot);
    std::cout << "U1snap :" << devided_U1_Snapshot.rows() << "," << devided_U1_Snapshot.cols() << "," << devided_U1_Snapshot.norm() << std::endl;
}

void Simulator::input_Basis(unsigned int devided_id)
{
    // std::filesystem::create_directories("basis");
    int n = _texdepth * _texheight * _texdepth;
    int r = _reduce_dimention;
    std::string basis_foldername = 
    "basis/n" + std::to_string(n) + "r" + std::to_string(r) + 
    "dev" + std::to_string(_devide_num) +"id" + std::to_string(devided_id); 
    std::string U0FileName = basis_foldername + "/U0.txt";
    std::string U1FileName = basis_foldername + "/U1.txt";
    std::string U2FileName = basis_foldername + "/U2.txt";
    std::string U3FileName = basis_foldername + "/U3.txt";
    std::string PFileName  = basis_foldername +  "/P.txt";
    n = (_texdepth + 1)* _texheight * _texdepth;
    r = _reduce_dimention / _devide_num;
    // U0 = Eigen::MatrixXf(3*n,r);
    // U1 = Eigen::MatrixXf(3*n,r);
    // U2 = Eigen::MatrixXf(3*n,r);
    // U3 = Eigen::MatrixXf(3*n,r);
    // P  = Eigen::MatrixXf(_texdepth * _texheight * _texdepth,r);
    inputMatrix(U0FileName,U0);
    inputMatrix(U1FileName,U1);
    inputMatrix(U2FileName,U2);
    inputMatrix(U3FileName,U3);
    inputMatrix(PFileName,P);
    std::cout << "input check" << std::endl;
    std::cout << "U0:" << (U0.transpose() * U0 - Eigen::MatrixXf::Identity(U0.cols(),U0.cols())).norm() << std::endl;
    std::cout << "U1:" << (U1.transpose() * U1 - Eigen::MatrixXf::Identity(U1.cols(),U1.cols())).norm() << std::endl;
    std::cout << "U2:" << (U2.transpose() * U2 - Eigen::MatrixXf::Identity(U2.cols(),U2.cols())).norm() << std::endl;
    std::cout << "U3:" << (U3.transpose() * U3 - Eigen::MatrixXf::Identity(U3.cols(),U3.cols())).norm() << std::endl;
    std::cout << "P :" << ( P.transpose() * P  - Eigen::MatrixXf::Identity( P.cols(), P.cols())).norm() << std::endl;

    devided_U0_List.push_back(U0);
    devided_U1_List.push_back(U1);
    devided_U2_List.push_back(U2);
    devided_U3_List.push_back(U3);
    devided_P_List.push_back(P);
}

Eigen::MatrixXf cal_Basis(Eigen::MatrixXf &Snapshot, unsigned int &reduce_dimention,float threshold)
{
    float singularity_threshold = threshold;
    Eigen::HouseholderQR<Eigen::MatrixXf> QRSolver(Snapshot);
    int m = Snapshot.rows();
    int n = Snapshot.cols();
    Eigen::HouseholderQR<Eigen::MatrixXf> HhQR(Snapshot);
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
    float cumulative_contribution_ratio;
    float sum_used_singular_values = 0.0;
    if(reduce_dimention == n)
    {
        for(unsigned int i=0;i<singular_values.size();++i)
        {
            if(singular_values(i) / singular_values(0) < singularity_threshold)
            {
                // if(reduce_dimention < i)reduce_dimention = i;
                reduce_dimention = i;
                break;
            }
        }
    }
    for(unsigned int i=0;i<reduce_dimention;++i)sum_used_singular_values += singular_values(i) * singular_values(i);
    cumulative_contribution_ratio = sum_used_singular_values / (singular_values.norm() * singular_values.norm());
    float singularity = *singular_values.begin() / *(singular_values.begin() + reduce_dimention);
    Eigen::MatrixXf Basis(m,reduce_dimention);
    Eigen::MatrixXf matU(m,reduce_dimention);matU = svd.matrixU();
    Basis = HhQR.householderQ()*matU.leftCols(reduce_dimention); //m * n
    // Basis = HhQR.householderQ()*svd.matrixU()*singular_value_matrix; //m * n
    std::cout << "Basis size : " << Basis.rows() << "," << Basis.cols() << std::endl;
    std::cout << "cumulative_contribution_ratio : " << cumulative_contribution_ratio << std::endl;
    // std::cout << "used, all : " << sum_used_singular_values << "," << singular_values.sum() << std::endl;
    // std::cout << "m , n : " << m << "," << n << std::endl;
    // std::cout << "Basis is orthonomality check : " << (Basis.transpose() * Basis - Eigen::MatrixXf::Identity(n,n)).norm() << std::endl;
    return Basis;
}

//subspace
void Simulator::subspace_execute()
{
    std::string rootFolderName = "Result";
    std::string densityFolderName = rootFolderName + "/density";
    std::filesystem::create_directories(rootFolderName);
    std::filesystem::create_directories(densityFolderName);

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
    init_density(TGT_DENSITY);
    init_templature(TGT_TEMPLATURE);
    init_all_velocity();
    // std::cout << "reduced_Diff" << std::endl << devided_DiffusionMatrix_List[0] << std::endl;
    // std::cout << "reduced_V2D" << std::endl << devided_Vel2DivMatrix_List[0] << std::endl;
    // std::cout << "reduced_Po" << std::endl << devided_PoissonMatrix_List[0] << std::endl;
    // std::cout << "reduced_P2V" << std::endl << devided_Pressure2VelocityMatrix_List[0] << std::endl;
    // std::cout << "reduced_Dirich" << std::endl << devided_DirichletBoundaryMatrix_List[0]  << std::endl;
    unsigned int subspace_flame_num = (_dt/_sub_dt) * _flame_num;
    for(_timestamp = 0; _timestamp < subspace_flame_num; ++_timestamp)
    {
        if(_timestamp == 99 || _timestamp == 100 || _timestamp == 101 || _timestamp == 102)
        {
            std::string plot_filename = "velocity_plot_" + std::to_string(_timestamp) + ".png";
            plot(plot_filename,_timestamp);
        }
        unsigned int devided_id = _timestamp / (subspace_flame_num / _devide_num);
        if(_timestamp * (_dt/_sub_dt) < _discard_flame )
        {
            oneloop();
        }
        else 
        {
            subspace_oneloop(
            devided_U0_List[devided_id],
            devided_U1_List[devided_id],
            devided_U2_List[devided_id],
            devided_U3_List[devided_id],
            devided_P_List[devided_id],
            devided_DiffusionMatrix_List[devided_id],
            devided_DirichletBoundaryMatrix_List[devided_id],
            devided_Vel2DivMatrix_List[devided_id],
            devided_PoissonMatrix_List[devided_id],
            devided_Pressure2VelocityMatrix_List[devided_id],
            devided_cubaturePointSetList[devided_id],
            devided_cubatureWeightVectorList[devided_id]);
        }
        std::string OutputVTK_den = densityFolderName+  "/output"+std::to_string(_timestamp)+".vtk";
        cal_density_err();
        outputVTK(OutputVTK_den,density_err.src_texture,_texwidth,_texheight,_texdepth,_dx);
        if(_devide_num > 1)output_txt(devided_density_floder_name ,_timestamp);
        else output_txt(subspace_density_floder_name ,_timestamp);
    }
    int n = _texdepth;
    std::string foldername = "result";
    std::string U3errorFileName = std::to_string(n) + "div" + std::to_string(_devide_num) + "U3L2.txt";
    std::string timeFileName = std::to_string(n) + "div" + std::to_string(_devide_num) + "time.txt";
    // FILE *ofp = fopen("fileist.txt","a");
    // if(fprintf(ofp, "%c\n",U3errorFileName.c_str()));
    // fclose(ofp);
    Outputer outputer(foldername,U3errorFileName,timeFileName);
    outputer.output_error(U3_error_vector);
    outputer.output_time(basis_time,projection_time);
}

void Simulator::subspace_oneloop(
    Eigen::MatrixXf &devided_U0,
    Eigen::MatrixXf &devided_U1,
    Eigen::MatrixXf &devided_U2,
    Eigen::MatrixXf &devided_U3,
    Eigen::MatrixXf &devided_P,
    Eigen::MatrixXf &devided_DiffusionMatrix,
    Eigen::MatrixXf &devided_DirichletBoundaryMatrix,
    Eigen::MatrixXf &devided_Vel2DivMatrix,
    Eigen::MatrixXf &devided_PoissonMatrix,
    Eigen::MatrixXf &devided_Pressure2VelocityMatrix,
    std::set<unsigned int> &CubaturePointSet,
    Eigen::VectorXf &weight_vector
    )
{
    init_density(TGT_DENSITY);
    init_templature(TGT_TEMPLATURE);
    // std::cout << "devided_U0.size() = " << devided_U0.rows() << "," << devided_U0.cols() << std::endl;
    // std::cout << "devided_U1.size() = " << devided_U1.rows() << "," << devided_U1.cols() << std::endl;
    // std::cout << "devided_U2.size() = " << devided_U2.rows() << "," << devided_U2.cols() << std::endl;
    // std::cout << "devided_U3.size() = " << devided_U3.rows() << "," << devided_U3.cols() << std::endl;
    // std::cout << "devided_P.size() = " << devided_P.rows() << "," << devided_P.cols() << std::endl;
    //nonlinear
    std::cout << "sub_addForce" << std::endl;
    addForce(_sub_dt);
    // if(_timestamp < _flame_num / 2)addForce(_dt);
    init_all_velocity();
    reduced_all_velocity = devided_U0.transpose() * all_velocity;
    //U1
    // if(_devide_num == 1)
    // {
        // subspace_advect(CubaturePointSet,weight_vector,devided_U0,devided_U1);
        // std::cout << "sub_advect" << std::endl;
    // }
    // else
    // {
        faceAdvect();
        init_all_velocity();
        reduced_all_velocity = devided_U1.transpose() * all_velocity;
    // }
    //U2
    //Diffusion
    std::cout << "sub_diffusion" << std::endl;
    reduced_all_velocity = devided_DiffusionMatrix * devided_DirichletBoundaryMatrix * reduced_all_velocity;
    // std::cout << "exact, reduce : " << U2_all_frame.col(_timestamp).norm() << "," <<  (U2 * reduced_all_velocity).norm() << std::endl;
    // if(_timestamp < _discard_flame)
    // std::cout << "U2 restore error = " << (all_velocity - U2 * (U2.transpose() * all_velocity)).norm() / all_velocity.norm() << std::endl;
    // reduced_all_velocity = U2.transpose() * all_velocity;
    std::cout << "sub_project" << std::endl;
    subspace_project(
    devided_DirichletBoundaryMatrix,
    devided_Vel2DivMatrix,
    devided_PoissonMatrix,
    devided_Pressure2VelocityMatrix);
    float U3L2 = (U3_all_frame.col(_timestamp) - U3 * reduced_all_velocity).norm() / U3_all_frame.col(_timestamp).norm();
    float PL2 = ( P_all_frame.col(_timestamp) - P * reduced_px).norm() / P_all_frame.col(_timestamp).norm();
    // float U3L2 = (U3.transpose() * U3_all_frame.col(_timestamp) - reduced_all_velocity).norm() / ((U3.transpose() * U3_all_frame.col(_timestamp)).norm());
    // float PL2 = ( P.transpose() * P_all_frame.col(_timestamp) - reduced_px).norm() / ((P.transpose() * P_all_frame.col(_timestamp)).norm());
    std::cout << "U3 : " << U3L2 << std::endl;
    std::cout << "P  : " << PL2 << std::endl;
    U3_error_vector.push_back(U3L2);
    // P_error_vector.push_back(PL2);
    //nonlinear
    //U3
    all_velocity = devided_U3 * reduced_all_velocity;
    all2xyz();
    centerAdvect(templature);
    centerAdvect(density_tgt);
    centerAdvect(density_amb);
}

void Simulator::subspace_project(
    Eigen::MatrixXf &devided_DirichletBoundaryMatrix,
    Eigen::MatrixXf &devided_Vel2DivMatrix,
    Eigen::MatrixXf &devided_PoissonMatrix,
    Eigen::MatrixXf &devided_Pressure2VelocityMatrix
){
    Eigen::VectorXf b;
    Eigen::ConjugateGradient<Eigen::MatrixXf> solver;
    solver.setTolerance(1e-6);
    // if(_texwidth == 64)solver.setTolerance(1e-6);//64下限は1e-6 128下限は1e-4
    // if(_texwidth == 128)solver.setTolerance(1e-5);
    // if(_texwidth == 256)solver.setTolerance(1e-4);
    // solver.setMaxIterations(20);
    b = devided_Vel2DivMatrix * reduced_all_velocity;
    solver.compute(devided_PoissonMatrix);
    reduced_px = solver.solveWithGuess(b, reduced_px);
    reduced_all_velocity = devided_DirichletBoundaryMatrix * reduced_all_velocity - devided_Pressure2VelocityMatrix * reduced_px;
}

Eigen::VectorXf Simulator::face_advect_function(Eigen::Vector3i &pos,Eigen::VectorXf &velocity,float dt)
{
    // Eigen::Vector3f ret;
    Eigen::VectorXf ret(6);
    // float x = i*_dx;float y = (j+0.5)*_dx;float z = (k+0.5)*_dx;
    // std::cout << "pos : " << pos.transpose() << std::endl;
    // std::cout << "x" << std::endl;
    float x = pos.x()*_dx;float y = (pos.y() + 0.5)*_dx;float z = (pos.z() + 0.5)*_dx;
    // std::cout << "x, y, z = " << x << "," << y << "," << z << std::endl;
    float adv_x = x - dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
    float adv_y = y - dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
    float adv_z = z - dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx,z, z_velocity);
    // std::cout << "ax, ay, az = " << adv_x << "," << adv_y << "," << adv_z << std::endl;
    // ret.x() = TriLinearInterporation(adv_x, adv_y - 0.5*_dx, adv_z- 0.5*_dx, x_velocity);
    ret(0) = TriLinearInterporation(adv_x, adv_y - 0.5*_dx, adv_z- 0.5*_dx, x_velocity);

    // std::cout << "y" << std::endl;
    x = (pos.x() + 0.5)*_dx; y = pos.y()*_dx; z = (pos.z() + 0.5)*_dx;
    // std::cout << "x, y, z = " << x << "," << y << "," << z << std::endl;
    adv_x = x - dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
    adv_y = y - dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
    adv_z = z - dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
    // std::cout << "ax, ay, az = " << adv_x << "," << adv_y << "," << adv_z << std::endl;
    // ret.y() = TriLinearInterporation(adv_x - 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);
    ret(1) = TriLinearInterporation(adv_x - 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);

    // std::cout << "z" << std::endl;
    x = (pos.x() + 0.5)*_dx; y = (pos.y() + 0.5)*_dx; z = pos.z() * _dx;
    // std::cout << "x, y, z = " << x << "," << y << "," << z << std::endl;
    adv_x = x - dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
    adv_y = y - dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
    adv_z = z - dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
    // std::cout << "ax, ay, az = " << adv_x << "," << adv_y << "," << adv_z << std::endl;
    // ret.z() = TriLinearInterporation(adv_x - 0.5*_dx, adv_y- 0.5*_dx, adv_z, z_velocity);
    ret(2) = TriLinearInterporation(adv_x - 0.5*_dx, adv_y- 0.5*_dx, adv_z, z_velocity);

    x = (pos.x() + 1)*_dx;y = (pos.y() + 0.5)*_dx;z = (pos.z() + 0.5)*_dx;
    // std::cout << "x, y, z = " << x << "," << y << "," << z << std::endl;
    adv_x = x - dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
    adv_y = y - dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
    adv_z = z - dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx,z, z_velocity);
    // std::cout << "ax, ay, az = " << adv_x << "," << adv_y << "," << adv_z << std::endl;
    // ret.x() = TriLinearInterporation(adv_x, adv_y - 0.5*_dx, adv_z- 0.5*_dx, x_velocity);
    ret(3) = TriLinearInterporation(adv_x, adv_y - 0.5*_dx, adv_z- 0.5*_dx, x_velocity);

    // std::cout << "y" << std::endl;
    x = (pos.x() + 0.5)*_dx; y = (pos.y() + 1)*_dx; z = (pos.z() + 0.5)*_dx;
    // std::cout << "x, y, z = " << x << "," << y << "," << z << std::endl;
    adv_x = x - dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
    adv_y = y - dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
    adv_z = z - dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
    // std::cout << "ax, ay, az = " << adv_x << "," << adv_y << "," << adv_z << std::endl;
    // ret.y() = TriLinearInterporation(adv_x - 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);
    ret(4) = TriLinearInterporation(adv_x - 0.5*_dx, adv_y, adv_z- 0.5*_dx, y_velocity);

    // std::cout << "z" << std::endl;
    x = (pos.x() + 0.5)*_dx; y = (pos.y() + 0.5)*_dx; z = (pos.z() + 1) * _dx;
    // std::cout << "x, y, z = " << x << "," << y << "," << z << std::endl;
    adv_x = x - dt*TriLinearInterporation(x, y-0.5*_dx, z-0.5*_dx, x_velocity);
    adv_y = y - dt*TriLinearInterporation(x-0.5*_dx, y, z-0.5*_dx, y_velocity);
    adv_z = z - dt*TriLinearInterporation(x-0.5*_dx, y-0.5*_dx, z, z_velocity);
    // std::cout << "ax, ay, az = " << adv_x << "," << adv_y << "," << adv_z << std::endl;
    // ret.z() = TriLinearInterporation(adv_x - 0.5*_dx, adv_y- 0.5*_dx, adv_z, z_velocity);
    ret(5) = TriLinearInterporation(adv_x - 0.5*_dx, adv_y- 0.5*_dx, adv_z, z_velocity);
    return ret;
}

void Simulator::subspace_advect(
    std::set<unsigned int> &CubaturePointSet,
    Eigen::VectorXf &weight_vector,
    Eigen::MatrixXf &devided_U0,
    Eigen::MatrixXf &devided_U1)
{
    all2xyz();
    Eigen::VectorXf updated_reduced_velocity = Eigen::VectorXf::Zero(reduced_all_velocity.size());
    auto itr = cubaturePointSet.begin();
    int weight_id = 0;
    while(itr != cubaturePointSet.end())
    {
        unsigned int point_id = *(itr);
        unsigned int p_x;unsigned int p_y;unsigned int p_z;
        resequence1to3(point_id,p_x,p_y,p_z,_texwidth,_texheight,_texdepth);
        Eigen::Vector3i pre_advected_id_pos = {p_x,p_y,p_z};
        Eigen::Vector3i post_advected_id_pos = {p_x+1,p_y+1,p_z+1};
        // std::cout << "point_pos" << std::endl << pre_advected_id_pos.transpose() << std::endl;
        // Eigen::MatrixXf subU0_pre = getRowsCorrespondPoint(U0,p_x,p_y,p_z);
        // Eigen::MatrixXf subU0_post = getRowsCorrespondPoint(U0,p_x+1,p_y+1,p_z+1);
        Eigen::MatrixXf subU0 = getRowsCorrespondPoint(U0,p_x,p_y,p_z);
        // std::cout << "subU0" << std::endl;
        // Eigen::MatrixXf subU1_pre = getRowsCorrespondPoint(U1,p_x,p_y,p_z);
        // Eigen::MatrixXf subU1_post = getRowsCorrespondPoint(U1,p_x+1,p_y+1,p_z+1);
        Eigen::MatrixXf subU1 = getRowsCorrespondPoint(U1,p_x,p_y,p_z);
        // std::cout << "subU1" << std::endl;
        // Eigen::Vector3f unreduced_point_velocity_pre = subU0_pre * reduced_all_velocity;
        // Eigen::Vector3f unreduced_point_velocity_post = subU0_post * reduced_all_velocity;
        Eigen::VectorXf unreduced_point_velocity = subU0 * reduced_all_velocity;
        // std::cout << "unreduced_point_vel" << std::endl << unreduced_point_velocity.transpose() << std::endl;
        Eigen::VectorXf all_v = U0 * reduced_all_velocity;
        // updated_reduced_velocity += cubatureWeightVector(weight_id) * 
        // (subU1_pre  * face_advect_function(pre_advected_id_pos, all_v ,_sub_dt)
        // +subU1_post * face_advect_function(post_advected_id_pos,all_v ,_sub_dt)) / 2;
        // std::cout << (cubatureWeightVector(weight_id) * subU1.transpose() * face_advect_function(pre_advected_id_pos,all_v ,_sub_dt)).transpose() << std::endl;
        updated_reduced_velocity += cubatureWeightVector(weight_id) * subU1.transpose() * face_advect_function(pre_advected_id_pos,all_v ,_sub_dt);;
        ++itr;
        ++weight_id;
    }
    reduced_all_velocity = updated_reduced_velocity;
}

Eigen::MatrixXf Simulator::getRowsCorrespondPoint(Eigen::MatrixXf &Mat, unsigned int x,unsigned int y, unsigned int z)
{
    unsigned int size = Mat.rows() / 3;
    // Eigen::MatrixXf ret(3,Mat.cols());
    // unsigned int x_id  = resequence3to1(x,y,z,_texwidth+1,_texheight,_texdepth);
    // unsigned int y_id  = size + resequence3to1(x,y,z,_texwidth,_texheight+1,_texdepth);
    // unsigned int z_id  = 2 * size + resequence3to1(x,y,z,_texwidth,_texheight,_texdepth+1);
    // ret.row(0) = Mat.row(x_id);
    // ret.row(1) = Mat.row(y_id);
    // ret.row(2) = Mat.row(z_id);
    // return ret;
    Eigen::MatrixXf ret(6,Mat.cols());
    unsigned int x_id_pre   = resequence3to1(x,y,z,_texwidth+1,_texheight,_texdepth);
    unsigned int x_id_post  = resequence3to1(x+1,y,z,_texwidth+1,_texheight,_texdepth);
    unsigned int y_id_pre   = size + resequence3to1(x,y,z,_texwidth,_texheight+1,_texdepth);
    unsigned int y_id_post  = size + resequence3to1(x,y+1,z,_texwidth,_texheight+1,_texdepth);
    unsigned int z_id_pre   = 2 * size + resequence3to1(x,y,z,_texwidth,_texheight,_texdepth+1);
    unsigned int z_id_post  = 2 * size + resequence3to1(x,y,z+1,_texwidth,_texheight,_texdepth+1);
    ret.row(0) = Mat.row(x_id_pre);
    ret.row(1) = Mat.row(x_id_post);
    ret.row(2) = Mat.row(y_id_pre);
    ret.row(3) = Mat.row(y_id_post);
    ret.row(4) = Mat.row(z_id_pre);
    ret.row(5) = Mat.row(z_id_post);
    return ret;
}

Eigen::VectorXf Simulator::getVelocityFromSnapshot(Eigen::MatrixXf &Snapshot,unsigned int x,unsigned int y,unsigned int z,unsigned int T)
{
    unsigned int size = Snapshot.rows() / 3;
    // Eigen::Vector3f ret;
    // ret.x() = Snapshot(resequence3to1(x,y,z,_texwidth+1,_texheight,_texdepth),T);
    // ret.y() = Snapshot(size + resequence3to1(x,y,z,_texwidth,_texheight+1,_texdepth),T);
    // ret.z() = Snapshot(2*size + resequence3to1(x,y,z,_texwidth,_texheight,_texdepth+1),T);
    Eigen::VectorXf ret(6);
    ret(0) = Snapshot(resequence3to1(x,y,z,_texwidth+1,_texheight,_texdepth),T);
    ret(1) = Snapshot(resequence3to1(x+1,y,z,_texwidth+1,_texheight,_texdepth),T);
    ret(2) = Snapshot(size + resequence3to1(x,y,z,_texwidth,_texheight+1,_texdepth),T);
    ret(3) = Snapshot(size + resequence3to1(x,y+1,z,_texwidth,_texheight+1,_texdepth),T);
    ret(4) = Snapshot(2*size + resequence3to1(x,y,z,_texwidth,_texheight,_texdepth+1),T);
    ret(5) = Snapshot(2*size + resequence3to1(x,y,z+1,_texwidth,_texheight,_texdepth+1),T);
    return ret;
}

void Simulator::largeSamplingCubature(
    std::set<unsigned int> &CubaturePointSet,
    Eigen::VectorXf &weight_vector,
    Eigen::MatrixXf &devided_U1_Snapshot,
    Eigen::MatrixXf &devided_U1,
    float error_threshold,float weight_threshold)
{
    Timer timer;
    timer.startWithMessage("largeSamplingCubature");
    Eigen::MatrixXf A;
    std::cout << "snap.cols, basis.cols = " << devided_U1_Snapshot.cols() << "," << devided_U1.cols() << std::endl;
    Eigen::VectorXf b = getSubspaceAdvect_b(devided_U1_Snapshot,devided_U1);
    std::cout << "b : " << b.size() << std::endl << b.transpose() << std::endl;
    Eigen::VectorXf w;
    Eigen::VectorXf residual = b;
    float err_real_value = (residual.norm())*residual.norm() * _cubature_threshold;
    // std::cout << "err_real_value : " << err_real_value << std::endl; 
    Eigen::NNLS<Eigen::MatrixXf> nnls_solver;
    int space_resolution = _texwidth * _texheight * _texdepth;
    std::uniform_int_distribution uid(0,space_resolution-1);
    std::uniform_real_distribution<float> urd(0,1);
    std::mt19937_64 mt_point(0);
    std::mt19937_64 mt_probablity(0);
    CubaturePointSet.clear();
    std::cout << "fin cubature init" << std::endl;
    while(residual.norm() * residual.norm() > err_real_value)
    {
        // std::cout << "point set num = " << CubaturePointSet.size() << std::endl;
        unsigned int point_id = uid(mt_point);
        // point_id = 188719;
        // point_id = 121918;
        // std::cout << "point id = " << point_id << std::endl;
        if(CubaturePointSet.find(point_id) != CubaturePointSet.end())
        {
            // std::cout << "already added cubature point set" << std::endl;
            continue;
        }
        unsigned int p_x;unsigned int p_y;unsigned int p_z;
        resequence1to3(point_id,p_x,p_y,p_z,_texwidth,_texheight,_texdepth);
        if(p_x == 0 || p_y == 0 || p_z == 0)continue;
        Eigen::VectorXf Acol = getColACoresspondCubaturePoint(point_id,devided_U1_Snapshot,devided_U1);
        // std::cout << "Acol" << std::endl;
        unsigned int restPoint_num = space_resolution - CubaturePointSet.size();
        float probablity = restPoint_num * ( std::fabs(Acol.dot(residual)) / (residual.dot(residual)));
        if(probablity < urd(mt_probablity))continue;
        // std::cout << "p_id = " << point_id << std::endl;
        // std::cout << "px,py,pz = " << p_x << "," << p_y << "," << p_z << std::endl;
        // std::cout << "probablity = " << probablity << std::endl;
        CubaturePointSet.insert(point_id);
        A = getSubspaceAdvect_A(CubaturePointSet,devided_U1_Snapshot,devided_U1);
        // std::cout << "fin calA" << std::endl;
        nnls_solver.compute(A);
        w = nnls_solver.solve(b);
        // std::cout << "w" << std::endl << w.transpose() << std::endl;
        // std::cout << "solve" << std::endl;
        auto itr = CubaturePointSet.begin();
        // std::cout << "A.rows, A.cols, b.size(), w.size() = " << A.rows() << ", " << A.cols() << ", " << b.size() << ", " << w.size() << std::endl;
        residual = b - A*w;
        // std::cout << "residual" << std::endl << residual.transpose() << std::endl;
        // std::cout << "err % = " << residual.norm() * residual.norm() / err_real_value << std::endl;
        // std::cout << "Cubature point set num, residual.norm()^2 = " 
        // << CubaturePointSet.size() << ", " << residual.norm() * residual.norm() << std::endl; 
        std::set<unsigned int>culledPointSet;
        for(int i=0;i<w.size();++i)
        {
            // if(w(i) > weight_threshold)residual(i) = b(i) - (A*w)(i);//ここの添字みす
            // else CubaturePointSet.erase(itr);
            // std::cout << "point " << *(itr) << ", w = " << w(i) << std::endl;
            if(w(i) > weight_threshold)
            {
                // CubaturePointSet.erase(itr);
                culledPointSet.insert(*(itr));
            }
            ++itr;
        }
        CubaturePointSet = culledPointSet;
        // std::cout << "update cubature point set" << std::endl;
        // std::cout << "fin cubature oneloop" << std::endl;
        int microsecond = 0.3 * 1000000;
        // usleep(microsecond);
    }
    weight_vector = w;
    // std::cout << w.transpose() << std::endl;
    std::cout << "cubature point num = " << CubaturePointSet.size() << std::endl;
    timer.end();
}
//Up * ¥tilde{x} = xpではなく，ちゃんと計算するとSnapshotが不要になる．基底の次元とxpの次元が合ってない弊害出てそう

Eigen::VectorXf Simulator::getColACoresspondCubaturePoint(unsigned int point_id,Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &Basis)
{
    unsigned int snap_num = _snap_num / _devide_num;
    unsigned int r = Basis.cols();
    Eigen::VectorXf ret(r*snap_num);
    unsigned int p_x;unsigned int p_y;unsigned int p_z;
    resequence1to3(point_id,p_x,p_y,p_z,_texwidth,_texheight,_texdepth);
    // std::cout << "px,py,pz = " << p_x << "," << p_y << "," << p_z << std::endl;
    // std::cout << point_id << "," << resequence3to1(p_x,p_y,p_z,_texwidth,_texheight,_texdepth) << std::endl;
    // Eigen::MatrixXf subBasis_pre = getRowsCorrespondPoint(Basis,p_x,p_y,p_z);
    // Eigen::MatrixXf subBasis_post = getRowsCorrespondPoint(Basis,p_x+1,p_y+1,p_z+1);
    Eigen::MatrixXf subBasis = getRowsCorrespondPoint(Basis,p_x,p_y,p_z);
    // std::cout << "subBasis" << std::endl;
    for(int snap=0; snap<snap_num; ++snap)
    {
        // Eigen::Vector3f pointVelocity_pre = getVelocityFromSnapshot(Snapshot,p_x,p_y,p_z,snap);
        // Eigen::Vector3f pointVelocity_post = getVelocityFromSnapshot(Snapshot,p_x+1,p_y+1,p_z+1,snap);
        // Eigen::VectorXf pointVelocity = getVelocityFromSnapshot(Snapshot,p_x,p_y,p_z,snap);
        Eigen::VectorXf pointVelocity = getVelocityFromSnapshot(Snapshot,p_x,p_y,p_z,snap);
        // std::cout << "snap point velocity = " << pointVelocity.transpose() << std::endl;
        // std::cout << "snap point velocity" << std::endl;
        // Eigen::VectorXf projectedPointVelocity_pre = subBasis_pre.transpose() * pointVelocity_pre;
        // Eigen::VectorXf projectedPointVelocity_post = subBasis_post.transpose() * pointVelocity_post;
        // Eigen::VectorXf projectedPointVelocity = (projectedPointVelocity_post + projectedPointVelocity_pre) / 2;
        Eigen::VectorXf projectedPointVelocity = subBasis.transpose() * pointVelocity;
        // std::cout << "projected point velocity = " <<projectedPointVelocity.transpose() << std::endl;
        // std::cout << "projected point velocity" << std::endl;
        for(int i = 0; i < r;++i)
        {
            // std::cout << "ret(" << r * snap + i << ") = pPV(" << i << ")" << std::endl;
            ret(r * snap + i) = projectedPointVelocity(i);
        }
    }
    return ret;
}

Eigen::MatrixXf Simulator::getSubspaceAdvect_A(std::set<unsigned int> &CubaturePointSet,Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &Basis)
{
    unsigned int snap_num = _snap_num / _devide_num;
    unsigned int P = CubaturePointSet.size();
    unsigned int r = Basis.cols();
    Eigen::MatrixXf ret(r * _snap_num, P);
    int col = 0;
    for(auto p:CubaturePointSet)
    {
        Eigen::VectorXf Acol = getColACoresspondCubaturePoint(p,Snapshot,Basis);
        ret.col(col) = Acol;
        ++col;
    }
    return ret;
}

Eigen::VectorXf Simulator::getSubspaceAdvect_b(Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &Basis)
{
    unsigned int snap_num = _snap_num / _devide_num;
    unsigned int r = Basis.cols();
    Eigen::VectorXf ret(r * _snap_num);
    for(unsigned int snap = 0; snap < _snap_num; ++snap)
    {
        Eigen::VectorXf projected_vector = Basis.transpose() * Snapshot.col(snap);
        // std::cout << "projected_vector[" << snap << "]" << std::endl << projected_vector.transpose() << std::endl;
        for(unsigned int i=0;i<r;++i)
        {
            ret(r * snap + i) = projected_vector(i);
        }
    }
    return ret;
}

void getDevidedSnapshot(
    unsigned int start_snap_id,unsigned int end_snap_id,
    Eigen::MatrixXf &Snapshot,Eigen::MatrixXf &devided_Snapshot)
{
    unsigned int devided_dimention = end_snap_id - start_snap_id + 1;
    devided_Snapshot = Eigen::MatrixXf(Snapshot.rows(),devided_dimention);
    for(unsigned int i = start_snap_id;i<end_snap_id;++i)
    {
        devided_Snapshot.col(i - start_snap_id) = Snapshot.col(i);
        // devided_Snapshot.col(i - start_snap_id) = Snapshot.col(i - start_snap_id);
    }
}

void Simulator::getDevidedBasis(
    unsigned int start_snap_id,unsigned int end_snap_id,
    Eigen::MatrixXf &devided_U0,
    Eigen::MatrixXf &devided_U1,
    Eigen::MatrixXf &devided_U2,
    Eigen::MatrixXf &devided_U3,
    Eigen::MatrixXf &devided_P)
{
    // unsigned int devided_reduce_dimention = _snap_num/_snap_devide_num;
    // unsigned int devided_step = _snap_num/_snap_devide_num;
    unsigned int devided_dimention = end_snap_id - start_snap_id + 1;
    Eigen::MatrixXf devided_U0_Snapshot(U0_Snapshot.rows(),devided_dimention);
    Eigen::MatrixXf devided_U1_Snapshot(U1_Snapshot.rows(),devided_dimention);
    Eigen::MatrixXf devided_U2_Snapshot(U2_Snapshot.rows(),devided_dimention);
    Eigen::MatrixXf devided_U3_Snapshot(U3_Snapshot.rows(),devided_dimention);
    Eigen::MatrixXf devided_P_Snapshot( P_Snapshot.rows() ,devided_dimention);

    for(unsigned int i = start_snap_id;i<end_snap_id;++i)
    {
        devided_U0_Snapshot.col(i - start_snap_id) = U0_Snapshot.col(i);
        devided_U1_Snapshot.col(i - start_snap_id) = U1_Snapshot.col(i);
        devided_U2_Snapshot.col(i - start_snap_id) = U2_Snapshot.col(i);
        devided_U3_Snapshot.col(i - start_snap_id) = U3_Snapshot.col(i);
        devided_P_Snapshot.col( i - start_snap_id) =  P_Snapshot.col(i);
    }
    getBasisQRSVD(
        devided_U0_Snapshot,
        devided_U1_Snapshot,
        devided_U2_Snapshot,
        devided_U3_Snapshot,
        devided_P_Snapshot,
        devided_U0,
        devided_U1,
        devided_U2,
        devided_U3,
        devided_P);
}

void Simulator::calDevidedList()
{
    unsigned int devide_snap_num = _snap_num / _devide_num;
    for(int i=0;i<_devide_num;++i)
    {
        unsigned int start_snap_id = i * devide_snap_num;
        unsigned int end_snap_id = start_snap_id + devide_snap_num - 1;
        // std::cout << "start,end" << start_snap_id << "," << end_snap_id << std::endl;
        Eigen::MatrixXf devided_U1_snap;
        getDevidedBasis(start_snap_id,end_snap_id,U0,U1,U2,U3,P);
        getDevidedSnapshot(start_snap_id,end_snap_id,U1_Snapshot,devided_U1_snap);
        devided_U0_List.push_back(U0);
        devided_U1_List.push_back(U1);
        devided_U2_List.push_back(U2);
        devided_U3_List.push_back(U3);
        devided_P_List.push_back(P);
        devided_U1_Snapshot_List.push_back(devided_U1_snap);
        output_Basis(i);
        output_Snapshot(i,devided_U1_snap);
        std::cout << "output check" << std::endl;
        std::cout << "U0:" << (U0.transpose() * U0 - Eigen::MatrixXf::Identity(U0.cols(),U0.cols())).norm() << std::endl;
        std::cout << "U1:" << (U1.transpose() * U1 - Eigen::MatrixXf::Identity(U1.cols(),U1.cols())).norm() << std::endl;
        std::cout << "U2:" << (U2.transpose() * U2 - Eigen::MatrixXf::Identity(U2.cols(),U2.cols())).norm() << std::endl;
        std::cout << "U3:" << (U3.transpose() * U3 - Eigen::MatrixXf::Identity(U3.cols(),U3.cols())).norm() << std::endl;
        std::cout << "P :" << ( P.transpose() * P  - Eigen::MatrixXf::Identity( P.cols(), P.cols())).norm() << std::endl;
        std::cout << "U1snap :" << devided_U1_snap.rows() << "," << devided_U1_snap.cols() << "," << devided_U1_snap.norm() << std::endl;
    }
}

void Simulator::calDevidedOperatorList()
{
    unsigned int devide_snap_num = _snap_num / _devide_num;
    for(int i=0;i<_devide_num;++i)
    {
        getReducedLinearOperator(
            devided_U0_List[i],
            devided_U1_List[i],
            devided_U2_List[i],
            devided_U3_List[i],
            devided_P_List[i]
        );
        devided_DiffusionMatrix_List.push_back(reduced_DiffusionMatrix);
        devided_Vel2DivMatrix_List.push_back(reduced_Vel2DivMatrix);
        devided_PoissonMatrix_List.push_back(reduced_PoissonMatrix);
        devided_Pressure2VelocityMatrix_List.push_back(reduced_Pressure2VelocityMatrix);
        devided_DirichletBoundaryMatrix_List.push_back(reduced_DirichletBoundaryMatrix);
    }
}

void Simulator::calCubatureList()
{
    for(int i=0;i<_devide_num;++i)
    {
        std::cout << "devide" << i << std::endl;
        largeSamplingCubature(
            cubaturePointSet,
            cubatureWeightVector,
            devided_U1_Snapshot_List[i],
            devided_U1_List[i],
            err_threshold,
            w_threshold
        );
        devided_cubaturePointSetList.push_back(cubaturePointSet);
        devided_cubatureWeightVectorList.push_back(cubatureWeightVector);
    }
}

void Simulator::cal_density_err()
{
    for(unsigned int k=0;k<_texdepth;++k){
        for(unsigned int j=0;j<_texheight;++j){
            for(unsigned int i=0;i<_texwidth;++i){
                int index = resequence3to1(i, j, k, _texwidth, _texheight, _texdepth);
                float err;
                if(origin_density.get_volume_value(i,j,k) < 1e-10 && std::fabs(density_tgt.get_volume_value(i,j,k) - origin_density.get_volume_value(i,j,k)) < 1e-6)err = 0;
                else err = (density_tgt.get_volume_value(i,j,k) - origin_density.get_volume_value(i,j,k));
                density_err.set_volume_value(i,j,k,err);
            }
        }
    }
    density_err.swap_src_dst();
}

//元データ計算時は成功し，読み込むと失敗する．かつ線形項は成功→Snapshotミス
