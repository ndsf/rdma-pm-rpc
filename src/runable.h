//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_RUNABLE_H
#define RDMA_RUNABLE_H

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

namespace rdmarpc {
    class Server;



    class DonePara{
    public:
        DonePara(
                ::google::protobuf::Message *recv_msg,
                ::google::protobuf::Message *resp_msg)
                : resp_msg_(resp_msg), recv_msg_(recv_msg){}
        ::google::protobuf::Message *recv_msg_;
        ::google::protobuf::Message *resp_msg_;
    };
    
    class Runnable {
    public:
        explicit Runnable();
    public:
        /** Method to execute the callable
        @param[in]	f		Callable object
        @param[in]	args		Variable number of args to F */
//        template <typename F, typename... Args>
//        void operator()(F &&f, Args &&... args) {
        void operator()();

        void dispatch_msg(
                const std::string &service_name,
                const std::string &method_name,
                const std::string &serialzied_data) ;

        void on_resp_msg_filled(DonePara para);

        void pack_message(
                const ::google::protobuf::Message *msg,
                std::string *serialized_data);

//    private:
        Server *server_;
        std::unique_ptr<infinity::core::Context> context_;
        std::unique_ptr<infinity::queues::QueuePair> qp_;
        std::unique_ptr<infinity::memory::Buffer> requestBuffer_;
    };
}

#endif //RDMA_RUNABLE_H
