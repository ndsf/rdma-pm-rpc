//
// Created by rrzhang on 2020/11/4.
//

#include "runable.h"
#include "server.h"

namespace rdmarpc{
    Runnable::Runnable(){}
    void Runnable::operator()() {
        requestBuffer_.reset(new infinity::memory::Buffer(context_.get(), 16384 * 2));
        context_->postReceiveBuffer(requestBuffer_.get());
        while (!server_->shutdown_) {
            infinity::core::receive_element_t receiveElement;
            while (!context_->receive(&receiveElement));
            size_t meta_len = *(size_t*)receiveElement.buffer->getData();

            rdmarpc::RpcMeta meta;
            meta.ParseFromString(std::string((char*)receiveElement.buffer->getData() + sizeof(size_t), meta_len));

            std::string request_str = std::string ((char*)receiveElement.buffer->getData() + sizeof(size_t) + meta_len, meta.data_size());

            dispatch_msg(
                    meta.service_name(),
                    meta.method_name(),
//                        std::move(request_str),
                    request_str);

            context_->postReceiveBuffer(requestBuffer_.get());
        }
    }

    void Runnable::dispatch_msg(
            const std::string &service_name,
            const std::string &method_name,
            const std::string &serialzied_data) {
        auto service = server_->_services[service_name].service;
        auto md = server_->_services[service_name].mds[method_name];

        std::cout << "recv service_name:" << service_name << std::endl;
        std::cout << "recv method_name:" << method_name << std::endl;
        std::cout << "recv type:" << md->input_type()->name() << std::endl;
        std::cout << "resp type:" << md->output_type()->name() << std::endl;

        auto recv_msg = service->GetRequestPrototype(md).New();
        recv_msg->ParseFromString(serialzied_data);
        auto resp_msg = service->GetResponsePrototype(md).New();

        Controller controller;
        DonePara * para = new DonePara(recv_msg, resp_msg);
        auto done = ::google::protobuf::NewCallback(
                this,
                &Runnable::on_resp_msg_filled,
                *para);
        service->CallMethod(md, &controller, recv_msg, resp_msg, done);
    }

    void Runnable::on_resp_msg_filled(DonePara para) {
        std::string resp_str;
        pack_message(para.resp_msg_, &resp_str);

        std::unique_ptr<infinity::memory::Buffer> responseBuffer = std::make_unique<infinity::memory::Buffer>
                (context_.get(), (void*)(resp_str.data()), resp_str.size());
        assert(resp_str.size() <= 16384 *2);
        qp_->send(responseBuffer.get(), context_->defaultRequestToken);
        context_->defaultRequestToken->waitUntilCompleted();
    }

    void Runnable::pack_message(
            const ::google::protobuf::Message *msg,
            std::string *serialized_data) {
        size_t serialized_size = msg->ByteSizeLong();
        serialized_data->assign(
                (const char *) &serialized_size,
                sizeof(serialized_size));
        msg->AppendToString(serialized_data);
    }
}