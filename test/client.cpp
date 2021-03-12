#include <iostream>
#include <thread>
#include <chrono>
#include "../src/channel.h"
#include "../src/controllor.h"
#include "proto/echo.pb.h"
#include "profiler.h"

#include <libvmem.h>

constexpr size_t thread_count = 1;
constexpr size_t request_count = 1000;
std::mutex mtx;
class Runnable
{
public:
    Runnable(int id, VMEM *vmp) : id_(id), vmp(vmp) {};
    void operator()()
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(id_ * 200)); // can't create multi connections at the same time
        std::unique_lock lck {mtx};
        // std::cout << "lock\n";
        rdmarpc::Channel channel("192.168.98.50", 6688, vmp);
        
        echo::EchoService_Stub stub(&channel);
        rdmarpc::Controller controller;
        // std::cout << "unlock\n";
        lck.unlock();

        dbx1000::Profiler profiler;
        profiler.Start();
        for (auto i = 0; i < request_count; ++i)
        {
            // std::cout << id_ << '-' << i << '\n';
            echo::EchoResponse response;
            echo::EchoRequest request;
            request.set_msg(std::to_string(id_) + "-" + std::to_string(i));

            stub.Echo(&controller, &request, &response, nullptr);
            // if (controller.Failed())
            //     std::cerr << "Request failed: " << controller.ErrorText().c_str();
            // else
            // std::cout << id << '-' << i << ':' << "Response: " << response.msg() << '\n';
        }
        profiler.End();
    };

private:
    int id_;
    VMEM *vmp;
    // static std::mutex mtx;
};

int main()
{

	VMEM *vmp;

	/* create minimum size pool of memory */
	if ((vmp = vmem_create("/home/congyong/mnt/pmem1", (size_t)(1024 * 1024 * 1024))) == NULL) {
		perror("vmem_create");
		exit(1);
	}

    std::vector<std::thread> vector_;
    vector_.resize(thread_count);
    for (auto i = 0; i < thread_count; ++i)
    {
        // vector_[i] = std::thread(fun, i);
        Runnable runnable(i, vmp);
        vector_[i] = std::thread(std::move(runnable));
    }

    dbx1000::Profiler profiler;
    profiler.Start();
    for (auto i = 0; i < thread_count; ++i)
        vector_[i].join();
    profiler.End();
    std::cout << "total:        " << profiler.Millis() << "ms\n";
    // std::cout << "callmethod:   " << channel.time_callmethod << "us\n";
    // std::cout << "send      :   " << channel.time_send << "us\n";
    // std::cout << "send1     :   " << channel.time_send1 << "us\n";
    // std::cout << "send2     :   " << channel.time_send2 << "us\n";
    // std::cout << "wait_response: " << channel.time_wait_rsponse << "us\n";
    vmem_delete(vmp);
    return 0;
}
