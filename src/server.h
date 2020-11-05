//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_SERVER_H
#define RDMA_SERVER_H

#include "google/protobuf/service.h"

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
        void Add(::google::protobuf::Service *service);

        void Start(const std::string &ip, int port);

        bool Shutdown() const;


        // service_name -> {Service*, ServiceDescriptor*, MethodDescriptor* []}
        std::map<std::string, ServiceInfo> services_;

    private:
        bool shutdown_;
    }; //Server
} // rdmarpc


#endif //RDMA_SERVER_H
