#include <iostream>
#include "../src/proto/rpc_meta.pb.h"
#include "../src/channel.h"
#include "../src/controllor.h"
#include "../src/runable.h"
#include "../src/server.h"
#include "echo.pb.h"


int main() {
    rdmarpc::Channel channel;
    channel.init("127.0.0.1", 6688);

    echo::EchoRequest request;
    echo::EchoResponse response;
    request.set_msg("hello, myrpc.");

    echo::EchoService_Stub stub(&channel);
    rdmarpc::Controller cntl;
    stub.Echo(&cntl, &request, &response, NULL);
    std::cout << "resp:" << response.msg() << std::endl;

    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
