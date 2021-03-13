#include <iostream>
#include "../src/server.h"
#include "proto/echo.pb.h"

#ifdef DELAY
#include <chrono>
#include <thread>
#endif 

#include <libvmem.h>

class MyEchoService : public echo::EchoService
{
public:
  void Echo(::google::protobuf::RpcController *controller,
            const ::echo::EchoRequest *request,
            ::echo::EchoResponse *response,
            ::google::protobuf::Closure *done) override
  {

    #ifdef DELAY
    std::cout << "Server received client msg, waiting for 1s: " << request->msg() << '\n';
    using namespace std::this_thread;     // sleep_for, sleep_until
    using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
    using std::chrono::system_clock;

    sleep_for(10ns);
    sleep_until(system_clock::now() + 1s);
    #endif

    response->set_msg("server say: received msg: ***" + request->msg() + "***");
    done->Run();
  }
};

int main()
{

  VMEM *vmp;

	/* create minimum size pool of memory */
	if ((vmp = vmem_create("/home/congyong/mnt/pmem1", (size_t)(1024 * 1024 * 1024))) == NULL) {
		perror("vmem_create");
		exit(1);
	}

  rdmarpc::Server my_server(vmp);
  MyEchoService echo_service;
  my_server.RegisterService(&echo_service);
  my_server.Start();
  
  vmem_delete(vmp);
  return 0;
}
