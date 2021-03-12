cmake_minimum_required(VERSION 2.8.12)
project(rdma-rpc)

add_definitions("-std=c++17")

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

include_directories(include)
link_directories(lib)

add_executable(client test/client.cpp src/channel.cpp src/server.cpp src/runnable.cpp src/proto/rpc_meta.pb.cc test/proto/echo.pb.cc ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(client infinity pthread ibverbs vmem ${Protobuf_LIBRARIES})
add_executable(server test/server.cpp src/channel.cpp src/server.cpp src/runnable.cpp src/proto/rpc_meta.pb.cc test/proto/echo.pb.cc ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(server infinity pthread ibverbs vmem ${Protobuf_LIBRARIES})