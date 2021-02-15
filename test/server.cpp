#include <iostream>
#include "../src/server.h"
#include "proto/echo.pb.h"

class MyEchoService : public echo::EchoService
{
public:
  void Echo(::google::protobuf::RpcController *controller,
                    const ::echo::EchoRequest *request,
                    ::echo::EchoResponse *response,
                    ::google::protobuf::Closure *done) override
  {
    std::cout << "server received client msg: " << request->msg() << '\n';
    response->set_msg("server say: received msg: ***" + request->msg() + "***");
    done->Run();
  }
};

int main()
{
  rdmarpc::Server my_server;
  MyEchoService echo_service;
  my_server.RegisterService(&echo_service);
  my_server.Start("127.0.0.1", 6688);

  return 0;
}
