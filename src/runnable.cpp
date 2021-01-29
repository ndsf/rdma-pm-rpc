//
// Created by rrzhang on 2020/11/4.
//

#include <thread>
#include "server.h"

#include "runnable.h"

namespace rdmarpc
{
    Runnable::Runnable() {}
    void Runnable::operator()()
    {
        //requestBuffer_.reset(new infinity::memory::Buffer(_context.get(), 16384 * 2));
        //_context->postReceiveBuffer(requestBuffer_.get());
        while (!server_->Shutdown())
        {
            infinity::core::receive_element_t receiveElement;
            while (!_context->receive(&receiveElement))
                ;
            _context->postReceiveBuffer(receiveElement.buffer);

            size_t meta_len = *(size_t *)receiveElement.buffer->getData();

            // std::cout << "Received 23: " << std::string((char*)receiveElement.buffer->getData() + sizeof(size_t), *(size_t*)receiveElement.buffer->getData()) << std::endl;

            rdmarpc::RpcMeta meta;
            meta.ParseFromString(std::string((char *)receiveElement.buffer->getData() + sizeof(size_t), meta_len));

            std::string request_str = std::string((char *)receiveElement.buffer->getData() + sizeof(size_t) + meta_len, meta.data_size());

            // 接收完消息后
            auto service = server_->_services[meta.service_name()].service;
            auto md = server_->_services[meta.service_name()].mds[meta.method_name()];

            // recv_msg 可以再 CallMethod 后立即删除
            auto recv_msg = service->GetRequestPrototype(md).New();
            recv_msg->ParseFromString(request_str);

            // resp_msg 和 controller 在回调函数 done->run() 中删除
            auto resp_msg = service->GetResponsePrototype(md).New();
            Controller *controller = new Controller();

            OnCallDone *done = new OnCallDone(resp_msg, controller);
            done->runnable_ = this;
            service->CallMethod(md, controller, recv_msg, resp_msg, done);
            delete recv_msg;

            // 最后把 requestBuffer_ 放到 qp
            // _context->postReceiveBuffer(requestBuffer_.get()); moved to top
        }
    }

    void OnCallDone::Run()
    {
        // google::protobuf::Closure 会自动调用该类的析构函数
        //        std::unique_ptr<OnCallDone> self_guard(this);
        std::string resp_str;
        size_t serialized_size = resp_msg_->ByteSizeLong();
        resp_str.assign((const char *)&serialized_size, sizeof(serialized_size));
        resp_msg_->AppendToString(&resp_str);

        auto responseBuffer = std::make_unique<infinity::memory::Buffer>(runnable_->_context.get(), (void *)(resp_str.data()), resp_str.size());
        assert(resp_str.size() <= 16384 * 2);
        runnable_->_qp->send(responseBuffer.get(), runnable_->_context->defaultRequestToken);
        runnable_->_context->defaultRequestToken->waitUntilCompleted();
    }
} // namespace rdmarpc