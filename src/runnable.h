//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_RUNNABLE_H
#define RDMA_RUNNABLE_H

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>

#include "controllor.h"

namespace rdmarpc
{
    class Server;

    class Runnable
    {
    public:
        explicit Runnable(Server &server) : server_(server) {count++;};
        void operator()();

        Server &server_;
        infinity::memory::Buffer *bufferToReceive_;
        std::unique_ptr<infinity::core::Context> context_;
        std::unique_ptr<infinity::queues::QueuePair> qp_;

    private:
        static int count;
    };

    class OnCallDone : public google::protobuf::Closure
    {
    public:
        OnCallDone(
            ::google::protobuf::Message *resp_msg, Controller *controller)
            : resp_msg_(resp_msg), controller_(controller) {count++;}

        ~OnCallDone() override
        {
            delete resp_msg_;
            delete controller_;
        }

        void Run() override;
        ::google::protobuf::Message *resp_msg_;
        Controller *controller_;

        Runnable *runnable_{};
    private:
        static int count;
    };
}

#endif //RDMA_RUNNABLE_H
