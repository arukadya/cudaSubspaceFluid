#include <iostream>
#include <vector>
#include <cusparse.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#define CHECK_CUDA(func)                                                       \
{                                                                              \
    cudaError_t status = (func);                                               \
    if (status != cudaSuccess) {                                               \
        printf("CUDA API failed at line %d with error: %s (%d)\n",             \
               __LINE__, cudaGetErrorString(status), status);                  \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

#define CHECK_CUSPARSE(func)                                                   \
{                                                                              \
    cusparseStatus_t status = (func);                                          \
    if (status != CUSPARSE_STATUS_SUCCESS) {                                   \
        printf("cuSPARSE API failed at line %d with error: %s (%d)\n",         \
               __LINE__, cusparseGetErrorString(status), status);              \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

#define CHECK_CUBLAS(func)                                                     \
{                                                                              \
    cublasStatus_t status = (func);                                            \
    if (status != CUBLAS_STATUS_SUCCESS) {                                     \
        printf("CUBLAS API failed at line %d with error: %d\n",                \
               __LINE__, status);                                              \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

#if defined(NDEBUG)
#   define PRINT_INFO(var)
#else
#   define PRINT_INFO(var) printf("  " #var ": %f\n", var);
#endif

typedef struct VecStruct {
    cusparseDnVecDescr_t vec;
    double*              ptr;
} Vec;

template <typename T>
struct myCUDA_Array
{
    public:
    T *_d_pointer;
    myCUDA_Array(){}
    myCUDA_Array(unsigned int width,unsigned int height)
    {
        _width = width;
        _height = height;
        _size = width * height;
        cudaMalloc((void **)&_d_pointer, sizeof(T) * _size);
        _data.resize(_size);
    }
    myCUDA_Array(unsigned int width,unsigned int height,unsigned int depth)
    {
        _width = width;
        _height = height;
        _depth = depth;
        _size = width * height * depth;
        cudaMalloc((void **)&_d_pointer, sizeof(T) * _size);
        _data.resize(_size);
    }
    myCUDA_Array(unsigned int width,unsigned int height,std::vector<T> &a)
    {
        _width = width;
        _height = height;
        _size = width * height;
        cudaMalloc((void **)&_d_pointer, sizeof(T) * _size);
        _data = a;
        cudaMemcpyAsync(_d_pointer, _data.data(), sizeof(T) * _size, cudaMemcpyHostToDevice);
    }
    myCUDA_Array(unsigned int width,unsigned int height,unsigned int depth,std::vector<T> &a)
    {
        _width = width;
        _height = height;
        _depth = depth;
        _size = width * height * depth;
        cudaMalloc((void **)&_d_pointer, sizeof(T) * _size);
        _data = a;
        cudaMemcpyAsync(_d_pointer, _data.data(), sizeof(T) * _size, cudaMemcpyHostToDevice);
    }
    void set(std::vector<T> &a)
    {
        _data = a;
        cudaMemcpyAsync(_d_pointer, _data.data(), sizeof(T) * _size, cudaMemcpyHostToDevice);
        // for(auto &x:data)std::cout << x << " ";
        // std::cout << std::endl;
    }
    std::vector<T> get()
    {
        cudaMemcpyAsync(_data.data(), _d_pointer, sizeof(T) * _size, cudaMemcpyDeviceToHost);
        return _data;
    }
    ~myCUDA_Array()noexcept
    {
        cudaFree(_d_pointer);
        //_dataは解放しなくていいのか？
    }
    void print()
    {
        for(int i=0;i<100;++i)std::cout << _data[i] << " ";
        std::cout << std::endl;
    }
    private:
    std::vector<T> _data;
    unsigned int _width,_height,_depth,_size;
};
