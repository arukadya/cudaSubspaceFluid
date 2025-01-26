cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0 LANGUAGES CXX)

#find_package(CUDA REQUIRED)

#message("-- CUDA --")
#message("Version: " ${CUDA_VERSION})
#message("Library: " ${CUDA_CUDA_LIBRARY})
#message("Runtime: " ${CUDA_CUDART_LIBRARY})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -ccbin g++-10)

find_package(OpenGL REQUIRED)
include_directories(
    ${OPENGL_INCLUDE_DIR}
)
#include_directories(SYSTEM lib/eigen-3.4.0)
include_directories(SYSTEM lib/eigen-master)
include_directories(header)


include_directories(/opt/homebrew/include)

link_directories(/opt/homebrew/lib)

file(GLOB_RECURSE CXX_SOURCE_LIST CONFIGURE_DEPENDS "src/*.cpp")
#file(GLOB_RECURSE CUDA_SOURCE_LIST CONFIGURE_DEPENDS "src/*.cu")

add_executable(main ${CXX_SOURCE_LIST})
#add_executable(viewer ${CXX_SOURCE_LIST})

target_link_libraries(main -lglfw -lGLEW ${OPENGL_LIBRARIES})
#target_link_libraries(viewer -lglfw -lGLEW ${OPENGL_LIBRARIES})
#target_link_libraries(main glfw GLEW GL cublas cusolver cusparse cudart)
#target_link_libraries(viewer glfw GLEW GL cublas cusolver cusparse cudart)

target_compile_options(main PUBLIC
#target_compile_options(viewer PUBLIC
$<$<CONFIG:Release>: -O2 -Wall>          # Release
  $<$<CONFIG:Debug>: -O0 -Wall>            # Debug
)
