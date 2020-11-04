//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_SERVER_H
#define RDMA_SERVER_H

#include <iostream>
#include <thread>
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

    class Server;
    class Runnable {
    public:
        explicit Runnable() {}
    public:
        /** Method to execute the callable
        @param[in]	f		Callable object
        @param[in]	args		Variable number of args to F */
//        template <typename F, typename... Args>
//        void operator()(F &&f, Args &&... args) {
        void operator()() {
            requestBuffer_.reset(new infinity::memory::Buffer(context_.get(), 16384 * 2));
            context_->postReceiveBuffer(requestBuffer_.get());
            while (!server_->s) {
                infinity::core::receive_element_t receiveElement;
                while (!context_->receive(&receiveElement));
                size_t meta_len = *(size_t*)receiveElement.buffer->getData();

                rdmarpc::RpcMeta meta;
                meta.ParseFromString(std::string((char*)receiveElement.buffer->getData() + sizeof(size_t), meta_len));

                std::string request_str = std::string ((char*)receiveElement.buffer->getData() + sizeof(size_t) + meta_len, meta.data_size());

                dispatch_msg(
                        meta.service_name(),
                        meta.method_name(),
                        std::move(request_str),
                        0);

                context_->postReceiveBuffer(requestBuffer_.get());
            }
        }

        void dispatch_msg(
                const std::string &service_name,
                const std::string &method_name,
                const std::string &serialzied_data,
                const boost::shared_ptr<boost::asio::ip::tcp::socket> &sock) {
            auto service = _services[service_name].service;
            auto md = _services[service_name].mds[method_name];

            std::cout << "recv service_name:" << service_name << std::endl;
            std::cout << "recv method_name:" << method_name << std::endl;
            std::cout << "recv type:" << md->input_type()->name() << std::endl;
            std::cout << "resp type:" << md->output_type()->name() << std::endl;

            auto recv_msg = service->GetRequestPrototype(md).New();
            recv_msg->ParseFromString(serialzied_data);
            auto resp_msg = service->GetResponsePrototype(md).New();

            Controller controller;
            DonePara * para = new DonePara(recv_msg, resp_msg, sock);
            auto done = ::google::protobuf::NewCallback(
                    this,
                    &Server::on_resp_msg_filled,
                    *para);
            service->CallMethod(md, &controller, recv_msg, resp_msg, done);
        }

//    private:
        Server *server_;
        std::unique_ptr<infinity::core::Context> context_;
        std::unique_ptr<infinity::queues::QueuePair> qp_;
        std::unique_ptr<infinity::memory::Buffer> requestBuffer_;
    };

    class DonePara{
    public:
        DonePara(
                ::google::protobuf::Message *recv_msg,
                ::google::protobuf::Message *resp_msg,
                const boost::shared_ptr<boost::asio::ip::tcp::socket> sock)
                : resp_msg_(resp_msg), recv_msg_(recv_msg), sock_(sock){}
        ::google::protobuf::Message *recv_msg_;
        ::google::protobuf::Message *resp_msg_;
        const boost::shared_ptr<boost::asio::ip::tcp::socket> sock_;
    };


    class Server {
    public:
        void add(::google::protobuf::Service *service) {
            ServiceInfo service_info;
            service_info.service = service;
            service_info.sd = service->GetDescriptor();
            for (int i = 0; i < service_info.sd->method_count(); ++i) {
                service_info.mds[service_info.sd->method(i)->name()] = service_info.sd->method(i);
            }

            _services[service_info.sd->name()] = service_info;
        }

        void start(const std::string &ip, const int port);

    private:
        void dispatch_msg(
                const std::string &service_name,
                const std::string &method_name,
                const std::string &serialzied_data,
                const boost::shared_ptr<boost::asio::ip::tcp::socket> &sock);

        void on_resp_msg_filled(DonePara para);

        void pack_message(
                const ::google::protobuf::Message *msg,
                std::string *serialized_data) {
            size_t serialized_size = msg->ByteSizeLong();
            serialized_data->assign(
                    (const char *) &serialized_size,
                    sizeof(serialized_size));
            msg->AppendToString(serialized_data);
        }

    private:
        struct ServiceInfo {
            ::google::protobuf::Service *service;
            const ::google::protobuf::ServiceDescriptor *sd;
            std::map<std::string, const ::google::protobuf::MethodDescriptor *> mds;
        };//ServiceInfo

        //service_name -> {Service*, ServiceDescriptor*, MethodDescriptor* []}
        std::map<std::string, ServiceInfo> _services;

        bool shutdown_;
    };//Server

    void Server::start(const std::string &ip, const int port) {
        shutdown_ = false;
        infinity::queues::QueuePairFactory *qpFactory = new infinity::queues::QueuePairFactory();
        qpFactory->bindToPort(port);
        while(!shutdown_) {
            printf("Waiting for incoming connection\n");
            std::unique_ptr<infinity::core::Context> context = std::make_unique<infinity::core::Context>();
            std::unique_ptr<infinity::queues::QueuePair> qp;
            qp.reset(qpFactory->acceptIncomingConnection(context.get()));
            std::cout << "connected." << std::endl;

            Runnable runnable;
            runnable.server_ = this;
            runnable.context_ = std::move(context);
            runnable.qp_ = std::move(qp);
            std::thread server_th(std::move(runnable));
            server_th.detach();
        }
        delete qpFactory;
    }

    void Server::start(const std::string &ip, const int port) {
        boost::asio::io_service io;
        boost::asio::ip::tcp::acceptor acceptor(
                io,
                boost::asio::ip::tcp::endpoint(
                        boost::asio::ip::address::from_string(ip),
                        port));

        while (true) {
            auto sock = boost::make_shared<boost::asio::ip::tcp::socket>(io);
            acceptor.accept(*sock);

            std::cout << "recv from client:"
                      << sock->remote_endpoint().address()
                      << std::endl;

            //接收meta长度
            char meta_size[sizeof(size_t)];
            sock->receive(boost::asio::buffer(meta_size));

            size_t meta_len = *(size_t *) (meta_size);

            //接收meta数据
            std::vector<char> meta_data(meta_len, 0);
            sock->receive(boost::asio::buffer(meta_data));

            myrpc::RpcMeta meta;
            meta.ParseFromString(std::string(&meta_data[0], meta_data.size()));

            //接收数据本身
            std::vector<char> data(meta.data_size(), 0);
            sock->receive(boost::asio::buffer(data));

            //
            dispatch_msg(
                    meta.service_name(),
                    meta.method_name(),
                    std::string(&data[0], data.size()),
                    sock);
        }
    }


    void Server::dispatch_msg(
            const std::string &service_name,
            const std::string &method_name,
            const std::string &serialzied_data,
            const boost::shared_ptr<boost::asio::ip::tcp::socket> &sock) {
        auto service = _services[service_name].service;
        auto md = _services[service_name].mds[method_name];

        std::cout << "recv service_name:" << service_name << std::endl;
        std::cout << "recv method_name:" << method_name << std::endl;
        std::cout << "recv type:" << md->input_type()->name() << std::endl;
        std::cout << "resp type:" << md->output_type()->name() << std::endl;

        auto recv_msg = service->GetRequestPrototype(md).New();
        recv_msg->ParseFromString(serialzied_data);
        auto resp_msg = service->GetResponsePrototype(md).New();

        Controller controller;
        DonePara * para = new DonePara(recv_msg, resp_msg, sock);
        auto done = ::google::protobuf::NewCallback(
                this,
                &Server::on_resp_msg_filled,
                *para);
        service->CallMethod(md, &controller, recv_msg, resp_msg, done);
    }

    void Server::on_resp_msg_filled(DonePara para) {
        boost::scoped_ptr<::google::protobuf::Message> recv_msg_guard(para.recv_msg_);
        boost::scoped_ptr<::google::protobuf::Message> resp_msg_guard(para.resp_msg_);

        std::string resp_str;
        pack_message(para.resp_msg_, &resp_str);

        para.sock_->send(boost::asio::buffer(resp_str));
    }
}


#endif //RDMA_SERVER_H
