//
// Created by rrzhang on 2020/11/4.
//

#ifndef RDMA_CONTROLLOR_H
#define RDMA_CONTROLLOR_H


#include "google/protobuf/service.h"

namespace rdmarpc {
    class Controller : public ::google::protobuf::RpcController {

    public:
        virtual void Reset() {};

        virtual bool Failed() const { return false; };

        virtual std::string ErrorText() const { return ""; };

        virtual void StartCancel() {};

        virtual void SetFailed(const std::string & /* reason */) {};

        virtual bool IsCanceled() const { return false; };

        virtual void NotifyOnCancel(::google::protobuf::Closure * /* callback */) {};
    };//Controller
}

#endif //RDMA_CONTROLLOR_H
