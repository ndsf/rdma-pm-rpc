cmake_minimum_required(VERSION 3.14)
project(rdma C CXX)


SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "compiler options" FORCE)
SET(CMAKE_CXX_FLAGS_RELEASE "-O0 -g" CACHE INTERNAL "compiler options" FORCE)

include_directories(include)
link_directories(lib)

add_executable(main test/main.cpp src/channel.cpp)
target_link_libraries(main lib)