//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMARPC_CHANNEL_H
#define RDMARPC_CHANNEL_H

#include "google/protobuf/service.h"

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include "unique_ptr.h"

namespace rdmarpc {

    class Channel : public ::google::protobuf::RpcChannel {
    public:
        void init(const std::string &ip, int port);

        void CallMethod(const ::google::protobuf::MethodDescriptor *method,
                                ::google::protobuf::RpcController * /* controller */,
                                const ::google::protobuf::Message *request,
                                ::google::protobuf::Message *response,
                                ::google::protobuf::Closure *) override;

    private:
        std::unique_ptr<infinity::core::Context> context_;
        std::unique_ptr<infinity::queues::QueuePair> qp_;
        std::unique_ptr<infinity::memory::Buffer> respnoseBuffer_;
        std::unique_ptr<infinity::memory::Buffer> requestBuffer_;
        size_t bufferSize_;
    public:
        uint64_t time;
        uint64_t time2;
    }; //mychannel

}

#endif //RDMARPC_CHANNEL_H
