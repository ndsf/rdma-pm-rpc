#include <iostream>
#include <thread>
#include "../src/proto/rpc_meta.pb.h"
#include "../src/channel.h"
#include "../src/controllor.h"
#include "../src/runable.h"
#include "../src/server.h"
#include "echo.pb.h"
#include "profiler.h"
#include <cstring>

void fun(int thread) {
    std::string str(64, 0);
    rdmarpc::Channel channel;
    channel.init("10.11.6.113", 6688);

    echo::EchoRequest request;
    echo::EchoResponse response;
    request.set_msg(str);

    echo::EchoService_Stub stub(&channel);
    rdmarpc::Controller cntl;
    dbx1000::Profiler profiler;
    profiler.Start();
    size_t count = 1000;
    for(auto i = 0; i< count; i++) {
        stub.Echo(&cntl, &request, &response, NULL);
    }
    profiler.End();

    if(thread == 2) {
        std::cout << "total:        " << profiler.Micros() / count << std::endl;
        std::cout << "callmethod:   " << channel.time_callmethod / count << std::endl;
        std::cout << "send      :   " << channel.time_send / count << std::endl;
        std::cout << "send1     :   " << channel.time_send1 / count << std::endl;
        std::cout << "send2     :   " << channel.time_send2 / count << std::endl;
        std::cout << "wait_rsponse: " << channel.time_wait_rsponse / count << std::endl;
    }
}

int main() {
    std::vector<std::thread> vector_;
    int thread_count = 15;
    vector_.resize(thread_count);
    for(auto i = 0; i < thread_count; i++) {
        vector_[i] = std::thread(fun, i);
    }

    for(auto i = 0; i < thread_count; i++) {
        vector_[i].join();
    }



    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
