//
// Created by rrzhang on 2020/11/4.
//

#include "server.h"

namespace rdmarpc {
    void Server::add(::google::protobuf::Service *service) {
        ServiceInfo service_info;
        service_info.service = service;
        service_info.sd = service->GetDescriptor();
        for (int i = 0; i < service_info.sd->method_count(); ++i) {
            service_info.mds[service_info.sd->method(i)->name()] = service_info.sd->method(i);
        }

        _services[service_info.sd->name()] = service_info;
    }

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

}