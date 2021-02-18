//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_RUNNABLE_H
#define RDMA_RUNNABLE_H

#include <iostream>

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>


#include "controllor.h"
// #include "unique_ptr.h"

namespace rdmarpc {
    class Server;
    
    class Runnable {
    public:
        explicit Runnable();
    public:
        /** Method to execute the callable
        @param[in]	f		Callable object
        @param[in]	args		Variable number of args to F */
//        template <typename F, typename... Args>
//        void operator()(F &&f, Args &&... args) {
        void operator()();

//    private:
        Server *server_;
        infinity::memory::Buffer *bufferToReceive_;
        std::unique_ptr<infinity::core::Context> context_;
        std::unique_ptr<infinity::queues::QueuePair> qp_;
        // std::unique_ptr<infinity::memory::Buffer> requestBuffer_;
        // infinity::core::Context *context_;
        // infinity::queues::QueuePair *qp_;
        // infinity::memory::Buffer *requestBuffer_;

    };

    class OnCallDone: public google::protobuf::Closure {
    public:
        OnCallDone(
                ::google::protobuf::Message *resp_msg, Controller *controller)
                : resp_msg_(resp_msg) , controller_(controller) {}

        ~OnCallDone() override {
            delete resp_msg_;
            delete controller_;
        }

        void Run() override;
        ::google::protobuf::Message *resp_msg_;
        Controller *controller_;

        Runnable *runnable_{};
    };
}

#endif //RDMA_RUNNABLE_H
