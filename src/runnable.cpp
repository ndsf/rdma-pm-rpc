//
// Created by rrzhang on 2020/11/4.
//

#include <thread>
#include "server.h"

#include "runnable.h"

namespace rdmarpc
{
    int Runnable::count = 0;
    int OnCallDone::count = 0;
    // Runnable::Runnable() {
    //     std::cout << "init\n";
    //     // bufferToReceive_ = new infinity::memory::Buffer(context_.get(), 16384 * sizeof(char));
    //     // context_->postReceiveBuffer(bufferToReceive_);
    //     //先init之后才设置context_
    // }
    void Runnable::operator()()
    {
        //requestBuffer_.reset(new infinity::memory::Buffer(context_.get(), 16384 * 2));
        //context_->postReceiveBuffer(requestBuffer_.get());
        // printf("Creating buffers to receive a message\n");
        bufferToReceive_ = new infinity::memory::Buffer(context_.get(), 16384 * sizeof(char), "/home/congyong/mnt/pmem1/bufferToReceiveRunnable" + std::to_string(count), "hello_layout");
        context_->postReceiveBuffer(bufferToReceive_);

        while (true)
        {
            infinity::core::receive_element_t receiveElement;
            while (!context_->receive(&receiveElement))
                ;
            context_->postReceiveBuffer(receiveElement.buffer);

            size_t meta_len = *(size_t *)receiveElement.buffer->getData();

            // std::cout << "Received 23: " << std::string((char*)receiveElement.buffer->getData() + sizeof(size_t), *(size_t*)receiveElement.buffer->getData()) << std::endl;

            rdmarpc::RpcMeta meta;
            meta.ParseFromString(std::string((char *)receiveElement.buffer->getData() + sizeof(size_t), meta_len));

            std::string request_str{(char *)receiveElement.buffer->getData() + sizeof(size_t) + meta_len, meta.data_size()};
            // 接收完消息后
            auto service = server_._services[meta.service_name()].service;
            auto md = server_._services[meta.service_name()].mds[meta.method_name()];

            // recv_msg 可以再 CallMethod 后立即删除
            auto recv_msg = service->GetRequestPrototype(md).New();
            recv_msg->ParseFromString(request_str);

            // resp_msg 和 controller 在回调函数 done->run() 中删除
            auto resp_msg = service->GetResponsePrototype(md).New();

            Controller *controller = new Controller();
            // auto controller = std::unique_ptr<Controller>(new Controller());

            OnCallDone *done = new OnCallDone(resp_msg, controller);
            // auto done = std::unique_ptr<OnCallDone>(new OnCallDone(resp_msg, controller.get()));

            done->runnable_ = this;
            service->CallMethod(md, controller, recv_msg, resp_msg, done);
            delete recv_msg;

            // 最后把 requestBuffer_ 放到 qp
            // context_->postReceiveBuffer(requestBuffer_.get()); moved to top
        }
        // delete bufferToReceive;
        delete bufferToReceive_;
    }

    void OnCallDone::Run()
    {
        // google::protobuf::Closure 会自动调用该类的析构函数
        //        std::unique_ptr<OnCallDone> self_guard(this);
        std::string resp_str;
        size_t serialized_size = resp_msg_->ByteSizeLong();
        resp_str.assign((const char *)&serialized_size, sizeof(serialized_size));
        resp_msg_->AppendToString(&resp_str);

        auto responseBuffer = std::make_unique<infinity::memory::Buffer>(runnable_->context_.get(), (void *)(resp_str.data()), resp_str.size(), "/home/congyong/mnt/pmem1/responseBufferRunnable" + std::to_string(count), "hello_layout");
        assert(resp_str.size() <= 16384 * 2);
        runnable_->qp_->send(responseBuffer.get(), runnable_->context_->defaultRequestToken);
        runnable_->context_->defaultRequestToken->waitUntilCompleted();
    }
} // namespace rdmarpc