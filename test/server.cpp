#include <iostream>
#include "../src/server.h"
#include "proto/echo.pb.h"

class MyEchoService : public echo::EchoService
{
public:
  virtual void Echo(::google::protobuf::RpcController *controller,
                    const ::echo::EchoRequest *request,
                    ::echo::EchoResponse *response,
                    ::google::protobuf::Closure *done)
  {
    std::cout << "server received client msg: " << request->msg() << std::endl;
    response->set_msg("server say: received msg: ***" + request->msg() + std::string("***"));
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
