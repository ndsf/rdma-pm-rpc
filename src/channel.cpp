//
// Created by rrzhang on 2020/11/4.
//

#include "channel.h"

namespace rdmarpc {


    void Channel::init(const std::string &ip, const int port) {
        context_ = std::make_unique<infinity::core::Context>();
        infinity::queues::QueuePairFactory *qpFactory = new infinity::queues::QueuePairFactory(context_.get());
        printf("Connecting to remote node\n");
        qp_.reset(qpFactory->connectToRemoteHost(ip.data(), port));
        printf("Creating buffers\n");

        respnoseBuffer_ = std::make_unique<infinity::memory::Buffer>
                (context_.get(), 16384 * 2);
        context_->postReceiveBuffer(respnoseBuffer_.get());
    }

    void Channel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
            ::google::protobuf::RpcController * /* controller */,
            const ::google::protobuf::Message *request,
            ::google::protobuf::Message *response,
            ::google::protobuf::Closure *) {
        std::string request_data = request->SerializeAsString();

        rdmarpc::RpcMeta rpc_meta;
        rpc_meta.set_service_name(method->service()->name());
        rpc_meta.set_method_name(method->name());
        rpc_meta.set_data_size(request_data.size());

//            std::cout << "CallMethod, service_name:" << method->service()->name() << std::endl;
//            std::cout << "CallMethod,  method_name:" << method->name() << std::endl;

        std::string meta_data = rpc_meta.SerializeAsString();

        size_t meta_size = meta_data.size();
        meta_data.insert(0, std::string((const char *) &meta_size, sizeof(size_t)));
        meta_data += request_data;

        // 发送 request
        std::unique_ptr<infinity::memory::Buffer> requestBuffer = std::make_unique<infinity::memory::Buffer>
                (context_.get(), (void*)(meta_data.data()), meta_data.size());
        assert(meta_data.size() <= 16384 *2);
        qp_->send(requestBuffer.get(), context_->defaultRequestToken);
        context_->defaultRequestToken->waitUntilCompleted();



        infinity::core::receive_element_t receiveElement;
//        std::unique_ptr<infinity::memory::Buffer> sizeBuffer = std::make_unique<infinity::memory::Buffer>
//                (context_.get(), 16384 *2);
//        context_->postReceiveBuffer(sizeBuffer.get());
//        while (!context_->receive(&receiveElement));
//        size_t len = *(size_t*)receiveElement.buffer->getData();

        // 再接收 response 本身
        while (!context_->receive(&receiveElement));
        size_t len = *(size_t*)receiveElement.buffer->getData();

        response->ParseFromString(std::string((char*)receiveElement.buffer->getData() + sizeof(size_t), len));

        context_->postReceiveBuffer(respnoseBuffer_.get());
    }

}