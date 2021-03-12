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
        int port_;
        VMEM *vmp;
        Server(VMEM *vmp, int port = 6688): port_(port), vmp(vmp) {};
        void RegisterService(::google::protobuf::Service *service);
        void Start();

        // service_name -> {Service*, ServiceDescriptor*, MethodDescriptor* []}
        std::map<std::string, ServiceInfo> _services;
    }; //Server
} // rdmarpc


#endif //RDMA_SERVER_H
