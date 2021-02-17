//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_SERVER_H
#define RDMA_SERVER_H

#include "google/protobuf/service.h"

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include "proto/rpc_meta.pb.h"


namespace rdmarpc {

    class Server {
    private:
        struct ServiceInfo {
            ::google::protobuf::Service *service;
            const ::google::protobuf::ServiceDescriptor *sd;
            std::map<std::string, const ::google::protobuf::MethodDescriptor *> mds;
        }; //ServiceInfo

    public:
        void RegisterService(::google::protobuf::Service *service);

        void Start(const std::string &ip, int port);

        // service_name -> {Service*, ServiceDescriptor*, MethodDescriptor* []}
        std::map<std::string, ServiceInfo> _services;

    // private:
        // std::unique_ptr<infinity::core::Context> _context;
    }; //Server
} // rdmarpc


#endif //RDMA_SERVER_H
