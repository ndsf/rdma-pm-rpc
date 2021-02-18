#include <iostream>
#include "../src/channel.h"
#include "../src/controllor.h"
#include "proto/echo.pb.h"
#include "profiler.h"

int main()
{
    echo::EchoResponse response;

    rdmarpc::Channel channel("192.168.98.53", 6688);
    echo::EchoService_Stub stub(&channel);
    rdmarpc::Controller controller;

    dbx1000::Profiler profiler;
    profiler.Start();
    size_t count = 10000;
    for (auto i = 0; i < count; i++)
    {
        echo::EchoRequest request;
        request.set_msg(std::to_string(i));

        stub.Echo(&controller, &request, &response, nullptr);
        // if (controller.Failed())
        //     std::cerr << "Request failed: " << controller.ErrorText().c_str();
        // else
        //     std::cout << "Response: " << response.msg() << '\n';
    }
    profiler.End();

    std::cout << "total:        " << profiler.Millis() << "ms\n";
    std::cout << "callmethod:   " << channel.time_callmethod << "us\n";
    std::cout << "send      :   " << channel.time_send << "us\n";
    std::cout << "send1     :   " << channel.time_send1 << "us\n";
    std::cout << "send2     :   " << channel.time_send2 << "us\n";
    std::cout << "wait_response: " << channel.time_wait_rsponse << "us\n";

    return 0;
}
