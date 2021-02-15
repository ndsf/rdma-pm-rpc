#include <iostream>
#include "../src/channel.h"
#include "../src/controllor.h"
#include "proto/echo.pb.h"
#include "profiler.h"

int main()
{
    echo::EchoRequest request;
    echo::EchoResponse response;
    request.set_msg("Hello World!");

    rdmarpc::Channel channel("192.168.98.53", 6688);
    echo::EchoService_Stub stub(&channel);
    rdmarpc::Controller controller;

    dbx1000::Profiler profiler;
    profiler.Start();
    size_t count = 10;
    for (auto i = 0; i < count; i++)
    {
        // printf("i = %d\n", i);
        stub.Echo(&controller, &request, &response, nullptr);
        // if (controller.Failed())
        //     std::cout << "Request failed: %s" << controller.ErrorText().c_str();
        // else
        //     std::cout << "Response: " << response.msg() << '\n';
    }
    profiler.End();

    std::cout << "total:        " << profiler.Micros() / count << '\n';
    std::cout << "callmethod:   " << channel.time_callmethod / count << '\n';
    std::cout << "send      :   " << channel.time_send / count << '\n';
    std::cout << "send1     :   " << channel.time_send1 / count << '\n';
    std::cout << "send2     :   " << channel.time_send2 / count << '\n';
    std::cout << "wait_response: " << channel.time_wait_rsponse / count << '\n';

    return 0;
}
