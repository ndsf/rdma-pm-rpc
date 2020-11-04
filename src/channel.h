//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMARPC_CHANNEL_H
#define RDMARPC_CHANNEL_H

#include <iostream>
#include "proto/rpc_meta.pb.h"
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include <infinity/memory/RegionToken.h>
#include <infinity/requests/RequestToken.h>
#include "unique_ptr.h"

namespace rdmarpc {

    class Channel : public ::google::protobuf::RpcChannel {
    public:
        void init(const std::string &ip, const int port);

        virtual void CallMethod(const ::google::protobuf::MethodDescriptor *method,
                                ::google::protobuf::RpcController * /* controller */,
                                const ::google::protobuf::Message *request,
                                ::google::protobuf::Message *response,
                                ::google::protobuf::Closure *);

    private:
        std::unique_ptr<infinity::core::Context> context_;
        std::unique_ptr<infinity::queues::QueuePair> qp_;
        std::unique_ptr<infinity::memory::Buffer> respnoseBuffer_;
    }; //mychannel

}

#endif //RDMARPC_CHANNEL_H
