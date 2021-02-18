//
// Created by rrzhang on 2020/11/4.
//
#include <thread>
#include "server.h"

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>

#include "runnable.h"
// #include "unique_ptr.h"

namespace rdmarpc
{
    void Server::RegisterService(::google::protobuf::Service *service)
    {
        ServiceInfo service_info;
        service_info.service = service;
        service_info.sd = service->GetDescriptor();
        for (int i = 0; i < service_info.sd->method_count(); ++i)
        {
            service_info.mds[service_info.sd->method(i)->name()] = service_info.sd->method(i);
        }

        _services[service_info.sd->name()] = service_info;
    }

    void Server::Start(const std::string &ip, int port)
    {
        while (true)
        {
            auto context = std::make_unique<infinity::core::Context>();
            auto qpFactory = std::make_unique<infinity::queues::QueuePairFactory>(context.get());
            qpFactory->bindToPort(port);

            printf("Creating buffers to read from and write to\n");
            auto bufferToReadWrite = std::make_unique<infinity::memory::Buffer>(context.get(), 16384 * sizeof(char));

            auto qp = std::unique_ptr<infinity::queues::QueuePair>(qpFactory->acceptIncomingConnection(bufferToReadWrite->createRegionToken(), sizeof(infinity::memory::RegionToken)));

            Runnable runnable(*this);
            // runnable.server_ = *this;
            runnable.context_ = std::move(context);
            runnable.qp_ = std::move(qp);
            std::thread server_th(std::move(runnable));
            server_th.detach();
            
            printf("Server_th detached\n");
        }
    }
} // namespace rdmarpc