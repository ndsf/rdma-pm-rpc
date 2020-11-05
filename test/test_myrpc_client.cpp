#include <iostream>
#include <thread>
#include "../src/proto/rpc_meta.pb.h"
#include "../src/channel.h"
#include "../src/controllor.h"
#include "../src/runable.h"
#include "../src/server.h"
#include "echo.pb.h"
#include "profiler.h"

void fun() {
    rdmarpc::Channel channel;
    channel.init("127.0.0.1", 6688);

    echo::EchoRequest request;
    echo::EchoResponse response;
    request.set_msg("hello, myrpc.");

    echo::EchoService_Stub stub(&channel);
    rdmarpc::Controller cntl;
    dbx1000::Profiler profiler;
    profiler.Start();
    size_t count = 1000;
    for(auto i = 0; i< count; i++) {
        stub.Echo(&cntl, &request, &response, NULL);
    }
    profiler.End();
//    std::cout << profiler.Micros() / count << std::endl;
    std::cout << channel.time / count << std::endl;
//    std::cout << channel.time2 / count << std::endl;
}

int main() {
    std::vector<std::thread> vector_;
    int thread_count = 20;
    vector_.resize(thread_count);
    for(auto i = 0; i < thread_count; i++) {
        vector_[i] = std::thread(fun);
    }

    for(auto i = 0; i < thread_count; i++) {
        vector_[i].join();
    }



    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
