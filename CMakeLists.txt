cmake_minimum_required(VERSION 3.14)
project(rdma C CXX)


SET(CMAKE_CXX_FLAGS "-O0 -g" CACHE INTERNAL "compiler options" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "compiler options" FORCE)
SET(CMAKE_CXX_FLAGS_RELEASE "-O0 -g" CACHE INTERNAL "compiler options" FORCE)

include_directories(include)
link_directories(lib)

add_executable(client test/test_myrpc_client.cpp src/channel.cpp src/server.cpp src/runable.cpp src/proto/rpc_meta.pb.cc test/echo.pb.cc)
target_link_libraries(client infinity pthread libprotobuf.a ibverbs)
add_executable(server test/test_myrpc.cpp src/channel.cpp src/server.cpp src/runable.cpp src/proto/rpc_meta.pb.cc test/echo.pb.cc)
target_link_libraries(server infinity pthread libprotobuf.a ibverbs)