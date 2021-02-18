//
// Created by rrzhang on 2020/11/4.
//

#include <cassert>
#include "channel.h"

#include <infinity/queues/QueuePairFactory.h>
#include <infinity/memory/RegisteredMemory.h>

#include "../src/proto/rpc_meta.pb.h"

#include "../test/profiler.h"

namespace rdmarpc
{
    Channel::Channel(const std::string &ip, int port)
    {
        context_ = std::make_unique<infinity::core::Context>();
        auto qpFactory = std::make_unique<infinity::queues::QueuePairFactory>(context_.get());
        printf("Channel connecting to remote node\n");
        qp_ = std::unique_ptr<infinity::queues::QueuePair>(qpFactory->connectToRemoteHost(ip.data(), port));
        printf("Creating buffers\n");

        bufferSize_ = 1024;
        responseBuffer_ = std::make_unique<infinity::memory::Buffer>(context_.get(), bufferSize_);
        context_->postReceiveBuffer(responseBuffer_.get());

        time_wait_rsponse = 0;
        time_callmethod = 0;
        time_send = 0;
        time_send1 = 0;
        time_send2 = 0;
    }

    void Channel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
                             ::google::protobuf::RpcController * /* controller */,
                             const ::google::protobuf::Message *request,
                             ::google::protobuf::Message *response,
                             ::google::protobuf::Closure *)
    {
        dbx1000::Profiler profiler_callmethod;
        profiler_callmethod.Start();
        // request 原始数据
        std::string request_data = request->SerializeAsString();
        rdmarpc::RpcMeta rpc_meta;
        rpc_meta.set_service_name(method->service()->name());
        rpc_meta.set_method_name(method->name());
        rpc_meta.set_data_size(request_data.size());
        std::string meta_data = rpc_meta.SerializeAsString();

        size_t meta_size = meta_data.size();
        meta_data.insert(0, std::string((const char *)&meta_size, sizeof(size_t)));
        meta_data += request_data;

        // std::cout << "Meta_size = " << meta_size << std::endl;
        // std::cout << "Inserted: " << std::string((const char *) &meta_size, sizeof(size_t)) << std::endl;
        // std::cout << "meta: " << meta_data << std::endl;
        // 最后发送的格式为 [meta_size][meta_data][request_data], meta_data = [sevice_name, method_name, request_data size]
        // printf("meta_data ready\n");

        dbx1000::Profiler profiler_send;
        profiler_send.Start();
        assert(meta_data.size() <= bufferSize_);
        dbx1000::Profiler profiler_send2;

        dbx1000::Profiler profiler_send1;
        profiler_send1.Start();
        auto requestBuffer = std::make_unique<infinity::memory::Buffer>(context_.get(), (void *)(meta_data.data()), meta_data.size());
        profiler_send1.End();
        time_send1 += profiler_send1.Micros();
        profiler_send2.Start();
        qp_->send(requestBuffer.get(), context_->defaultRequestToken);

        context_->defaultRequestToken->waitUntilCompleted();

        profiler_send2.End();
        time_send2 += profiler_send2.Micros();
        profiler_send.End();
        time_send += profiler_send.Micros();

        dbx1000::Profiler profiler_wait_rsponse;
        profiler_wait_rsponse.Start();
        // 等待 server 回复
        // respone 格式为 [response size][response data]
        infinity::core::receive_element_t receiveElement;
        while (!context_->receive(&receiveElement))
            ;
        // std::cout << "Received 89: " << std::string((char*)receiveElement.buffer->getData() + sizeof(size_t), *(size_t*)receiveElement.buffer->getData()) << std::endl;

        // response size
        size_t len = *(size_t *)receiveElement.buffer->getData();
        // response data
        response->ParseFromString(std::string((char *)receiveElement.buffer->getData() + sizeof(size_t), len));
        profiler_wait_rsponse.End();
        context_->postReceiveBuffer(responseBuffer_.get());

        time_wait_rsponse += profiler_wait_rsponse.Micros();

        profiler_callmethod.End();
        time_callmethod += profiler_callmethod.Micros();
    }

} // namespace rdmarpc