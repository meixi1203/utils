//
// Created by zhoupenghui on 2021/1/31.
//

#ifndef _FINALIZER_H_
#define _FINALIZER_H_

#include <functional>

class Finalizer {
public:
    explicit Finalizer(std::function<void()> finalizer_) : finalizer_(std::move(finalizer_)) {}

    ~Finalizer() {
        finalizer_();
    }

private:
    std::function<void ()> finalizer_;
};

#endif //_FINALIZER_H_