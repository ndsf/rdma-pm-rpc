#include <iostream>
#include "../src/server.h"
#include "proto/echo.pb.h"

#include <chrono>
#include <thread>

class MyEchoService : public echo::EchoService
{
public:
  void Echo(::google::protobuf::RpcController *controller,
            const ::echo::EchoRequest *request,
            ::echo::EchoResponse *response,
            ::google::protobuf::Closure *done) override
  {
    std::cout << "Server received client msg: " << request->msg() << ", wait for 1s...\n";

    using namespace std::this_thread;     // sleep_for, sleep_until
    using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
    using std::chrono::system_clock;

    sleep_for(10ns);
    sleep_until(system_clock::now() + 1s);

    response->set_msg("server say: received msg: ***" + request->msg() + "***");
    done->Run();
  }
};

int main()
{
  rdmarpc::Server my_server;
  MyEchoService echo_service;
  my_server.RegisterService(&echo_service);
  my_server.Start();

  return 0;
}
