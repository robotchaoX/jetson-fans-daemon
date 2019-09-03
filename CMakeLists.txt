#设置最低版本的cmake要求
cmake_minimum_required(VERSION 2.8)

#设置项目信息
project(fan-daemon)

#设置编译器版本
set(CMAKE_CXX_STANDARD 11)

#添加头文件路径
include_directories(./include ./src)

#添加源文件路径 -> 变量DIR_SRCS
aux_source_directory(./src DIR_SRCS)

#打印调试信息
MESSAGE(STATUS "Project:fan-daemon")
MESSAGE(STATUS "Project Dirrectory: ${PROJECT_SOURCE_DIR}")

#设置调试模式，debug会输出调试信息
SET(CMAKE_BUILD_TYPE DEBUG)
#SET(CMAKE_BUILD_TYPE RELEASE)

#添加自己写的其他函数文件，注意这个名字不能和工程名冲突
#ADD_LIBRARY( STATIC imgfeatures.cpp imgfeatures.h )


#主函数生成可执行文件
add_executable(main ${DIR_SRCS})

#设置链接库,将executable目标文件与库文件进行链接
target_link_libraries(main -lpthread)
