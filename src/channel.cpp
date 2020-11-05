//
// Created by rrzhang on 2020/11/4.
//

#include <cassert>
#include "channel.h"

#include <infinity/queues/QueuePairFactory.h>

#include "../src/proto/rpc_meta.pb.h"

#include "../test/profiler.h"

namespace rdmarpc {

    void Channel::init(const std::string &ip, int port) {
        context_ = std::make_unique<infinity::core::Context>();
        auto qpFactory = std::make_unique<infinity::queues::QueuePairFactory>(context_.get());
        printf("Channel connecting to remote node\n");
        qp_ = std::unique_ptr<infinity::queues::QueuePair>(qpFactory->connectToRemoteHost(ip.data(), port));
        printf("Creating buffers\n");

        bufferSize_ = 16384 * 2;
        respnoseBuffer_ = std::make_unique<infinity::memory::Buffer>(context_.get(), bufferSize_);
        context_->postReceiveBuffer(respnoseBuffer_.get());
        requestBuffer_ = std::make_unique<infinity::memory::Buffer>(context_.get(), bufferSize_);

        time = 0;
        time2 = 0;
    }

    void Channel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
            ::google::protobuf::RpcController * /* controller */,
            const ::google::protobuf::Message *request,
            ::google::protobuf::Message *response,
            ::google::protobuf::Closure *) {
        dbx1000::Profiler profiler1;
        profiler1.Start();
        // request 原始数据
        std::string request_data = request->SerializeAsString();

        rdmarpc::RpcMeta rpc_meta;
        rpc_meta.set_service_name(method->service()->name());
        rpc_meta.set_method_name(method->name());
        rpc_meta.set_data_size(request_data.size());

        std::string meta_data = rpc_meta.SerializeAsString();

        size_t meta_size = meta_data.size();
        meta_data.insert(0, std::string((const char *) &meta_size, sizeof(size_t)));
        meta_data += request_data;
        // 最后发送的格式为 [meta_size][meta_data][request_data], meta_data = [sevice_name, method_name, request_data size]

        // 发送 request
        auto requestBuffer = std::make_unique<infinity::memory::Buffer>(context_.get(), (void*)(meta_data.data()), meta_data.size());
//        memcpy(requestBuffer_->getData(), meta_data.data(), meta_data.size());
        assert(meta_data.size() <= bufferSize_);
        qp_->send(requestBuffer.get(), context_->defaultRequestToken);
//        qp_->send(requestBuffer_.get(), context_->defaultRequestToken);
        context_->defaultRequestToken->waitUntilCompleted();

        dbx1000::Profiler profiler;
        profiler.Start();
        // 等待 server 回复
        // respone 格式为 [response size][response data]
        infinity::core::receive_element_t receiveElement;
        while (!context_->receive(&receiveElement));
        // response size
        size_t len = *(size_t*)receiveElement.buffer->getData();
        // response data
        response->ParseFromString(std::string((char*)receiveElement.buffer->getData() + sizeof(size_t), len));
        profiler.End();
        context_->postReceiveBuffer(respnoseBuffer_.get());

        time += profiler.Micros();

        profiler1.End();
        time2 += profiler1.Micros();
    }

}