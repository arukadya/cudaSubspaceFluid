cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0 LANGUAGES CXX)

find_package(CUDA REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(../../../dev/lib/eigen-3.4.0)
include_directories(header)

link_directories(/dev/lib/OpenGL/lib)

file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/*.cpp")
#add_executable(main ${SOURCE_LIST})

cuda_add_executable(main ${SOURCE_LIST})

#target_link_libraries(main glfw3 libglew32d opengl32)
target_link_libraries(main glfw GLEW GL cublas)

target_compile_options(main PUBLIC
$<$<CONFIG:Release>: -O2 -Wall>          # Release
  $<$<CONFIG:Debug>: -O0 -Wall>            # Debug
)
