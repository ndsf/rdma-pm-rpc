//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMARPC_CHANNEL_H
#define RDMARPC_CHANNEL_H

#include "google/protobuf/service.h"

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>

namespace rdmarpc
{

    class Channel : public ::google::protobuf::RpcChannel
    {
    public:
        Channel(const std::string &ip, int port, int id);

        void CallMethod(const ::google::protobuf::MethodDescriptor *method,
                        ::google::protobuf::RpcController * /* controller */,
                        const ::google::protobuf::Message *request,
                        ::google::protobuf::Message *response,
                        ::google::protobuf::Closure *) override;

    private:
        std::unique_ptr<infinity::core::Context> context_;
        std::unique_ptr<infinity::queues::QueuePair> qp_;
        std::unique_ptr<infinity::memory::Buffer> responseBuffer_;
        size_t bufferSize_;
        static int count;
        int id_;

    public:
        uint64_t time_wait_rsponse;
        uint64_t time_callmethod;
        uint64_t time_send;
        uint64_t time_send1;
        uint64_t time_send2;
    };

} // namespace rdmarpc

#endif //RDMARPC_CHANNEL_H
