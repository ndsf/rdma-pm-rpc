//
// Created by rrzhang on 2020/11/4.
//
#include <iostream>
#include <thread>
#include "server.h"

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>

#include "runable.h"
#include "unique_ptr.h"

namespace rdmarpc {
    void Server::Add(::google::protobuf::Service *service) {
        ServiceInfo service_info;
        service_info.service = service;
        service_info.sd = service->GetDescriptor();
        for (int i = 0; i < service_info.sd->method_count(); ++i) {
            service_info.mds[service_info.sd->method(i)->name()] = service_info.sd->method(i);
        }

        services_[service_info.sd->name()] = service_info;
    }

    void Server::Start(const std::string &ip, int port) {
        shutdown_ = false;
        auto qpFactory = std::make_unique<infinity::queues::QueuePairFactory>();
        qpFactory->bindToPort(port);

        while(!shutdown_) {
            printf("Waiting for incoming connection\n");
            auto context = std::make_unique<infinity::core::Context>();
            auto qp = std::unique_ptr<infinity::queues::QueuePair>(qpFactory->acceptIncomingConnection(context.get()));
            std::cout << "connected." << std::endl;

            Runnable runnable;
            runnable.server_ = this;
            runnable.context_ = std::move(context);
            runnable.qp_ = std::move(qp);
            std::thread server_th(std::move(runnable));
            server_th.detach();
        }
    }

    bool Server::Shutdown() const {
        return this->shutdown_;
    }
}