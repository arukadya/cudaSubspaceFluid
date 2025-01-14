cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0 LANGUAGES CXX CUDA)

#find_package(CUDA REQUIRED)

#message("-- CUDA --")
#message("Version: " ${CUDA_VERSION})
#message("Library: " ${CUDA_CUDA_LIBRARY})
#message("Runtime: " ${CUDA_CUDART_LIBRARY})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -ccbin g++-10)

#include_directories(SYSTEM ../../../dev/lib/eigen-3.4.0)
include_directories(SYSTEM lib/eigen-3.4.0)
include_directories(header)
#include_directories(cuda_util)

#link_directories(/dev/lib/OpenGL/lib)

file(GLOB_RECURSE CXX_SOURCE_LIST CONFIGURE_DEPENDS "src/*.cpp")
#file(GLOB_RECURSE CUDA_SOURCE_LIST CONFIGURE_DEPENDS "src/*.cu")

add_executable(main ${CXX_SOURCE_LIST})
#add_executable(main ${CXX_SOURCE_LIST} ${CUDA_SOURCE_LIST})
#add_executable(viewer ${CXX_SOURCE_LIST} ${CUDA_SOURCE_LIST})

target_link_libraries(main glfw GLEW GL)
#target_link_libraries(main glfw GLEW GL cublas cusolver cusparse cudart)
#target_link_libraries(viewer glfw GLEW GL cublas cusolver cusparse cudart)

target_compile_options(main PUBLIC
#target_compile_options(viewer PUBLIC
$<$<CONFIG:Release>: -O2 -Wall>          # Release
  $<$<CONFIG:Debug>: -O0 -Wall>            # Debug
)
