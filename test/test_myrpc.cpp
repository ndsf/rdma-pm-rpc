#include <iostream>
#include "../src/proto/rpc_meta.pb.h"
#include "../src/channel.h"
#include "../src/controllor.h"
#include "../src/runable.h"
#include "../src/server.h"
#include "echo.pb.h"

class MyEchoService : public echo::EchoService {
public:
  virtual void Echo(::google::protobuf::RpcController* /* controller */,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done) {
      response->set_msg(request->msg());
      done->Run();
  }
}; //MyEchoService

int main() {
    rdmarpc::Server my_server;
    MyEchoService echo_service;
    my_server.Add(&echo_service);
    my_server.Start("127.0.0.1", 6688);

    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
