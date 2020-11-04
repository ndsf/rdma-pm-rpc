//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_SERVER_H
#define RDMA_SERVER_H

#include <iostream>
#include <thread>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/common.h"

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include <infinity/memory/RegionToken.h>
#include <infinity/requests/RequestToken.h>


#include "proto/rpc_meta.pb.h"
#include "controllor.h"
#include "unique_ptr.h"
#include "runable.h"


namespace rdmarpc {
    class Runnable;





    class Server {
    public:
        void add(::google::protobuf::Service *service);

        void start(const std::string &ip, const int port);

//    private:
//        void dispatch_msg(
//                const std::string &service_name,
//                const std::string &method_name,
//                const std::string &serialzied_data,
//                const boost::shared_ptr<boost::asio::ip::tcp::socket> &sock);
//
//        void on_resp_msg_filled(DonePara para);
//
//        void pack_message(
//                const ::google::protobuf::Message *msg,
//                std::string *serialized_data) {
//            size_t serialized_size = msg->ByteSizeLong();
//            serialized_data->assign(
//                    (const char *) &serialized_size,
//                    sizeof(serialized_size));
//            msg->AppendToString(serialized_data);
//        }

//    private:
        struct ServiceInfo {
            ::google::protobuf::Service *service;
            const ::google::protobuf::ServiceDescriptor *sd;
            std::map<std::string, const ::google::protobuf::MethodDescriptor *> mds;
        };//ServiceInfo

        //service_name -> {Service*, ServiceDescriptor*, MethodDescriptor* []}
        std::map<std::string, ServiceInfo> _services;

        bool shutdown_;
    };//Server


//    void Server::start(const std::string &ip, const int port) {
//        boost::asio::io_service io;
//        boost::asio::ip::tcp::acceptor acceptor(
//                io,
//                boost::asio::ip::tcp::endpoint(
//                        boost::asio::ip::address::from_string(ip),
//                        port));
//
//        while (true) {
//            auto sock = boost::make_shared<boost::asio::ip::tcp::socket>(io);
//            acceptor.accept(*sock);
//
//            std::cout << "recv from client:"
//                      << sock->remote_endpoint().address()
//                      << std::endl;
//
//            //接收meta长度
//            char meta_size[sizeof(size_t)];
//            sock->receive(boost::asio::buffer(meta_size));
//
//            size_t meta_len = *(size_t *) (meta_size);
//
//            //接收meta数据
//            std::vector<char> meta_data(meta_len, 0);
//            sock->receive(boost::asio::buffer(meta_data));
//
//            myrpc::RpcMeta meta;
//            meta.ParseFromString(std::string(&meta_data[0], meta_data.size()));
//
//            //接收数据本身
//            std::vector<char> data(meta.data_size(), 0);
//            sock->receive(boost::asio::buffer(data));
//
//            //
//            dispatch_msg(
//                    meta.service_name(),
//                    meta.method_name(),
//                    std::string(&data[0], data.size()),
//                    sock);
//        }
//    }
//
//
//    void Server::dispatch_msg(
//            const std::string &service_name,
//            const std::string &method_name,
//            const std::string &serialzied_data,
//            const boost::shared_ptr<boost::asio::ip::tcp::socket> &sock) {
//        auto service = _services[service_name].service;
//        auto md = _services[service_name].mds[method_name];
//
//        std::cout << "recv service_name:" << service_name << std::endl;
//        std::cout << "recv method_name:" << method_name << std::endl;
//        std::cout << "recv type:" << md->input_type()->name() << std::endl;
//        std::cout << "resp type:" << md->output_type()->name() << std::endl;
//
//        auto recv_msg = service->GetRequestPrototype(md).New();
//        recv_msg->ParseFromString(serialzied_data);
//        auto resp_msg = service->GetResponsePrototype(md).New();
//
//        Controller controller;
//        DonePara * para = new DonePara(recv_msg, resp_msg, sock);
//        auto done = ::google::protobuf::NewCallback(
//                this,
//                &Server::on_resp_msg_filled,
//                *para);
//        service->CallMethod(md, &controller, recv_msg, resp_msg, done);
//    }
//
//    void Server::on_resp_msg_filled(DonePara para) {
//        std::string resp_str;
//        pack_message(para.resp_msg_, &resp_str);
//
//        para.sock_->send(boost::asio::buffer(resp_str));
//    }










} // rdmarpc


#endif //RDMA_SERVER_H
