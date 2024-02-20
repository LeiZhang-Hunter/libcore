/**
******************************************************************************
* @file           : unix_sigset.h
* @author         : zhanglei
* @brief          : None
* @attention      : None
* @date           : 2024/2/7
******************************************************************************
*/
//
// Created by zhanglei on 2024/2/7.
//

#pragma once
extern "C" {
#include <signal.h>
#include <errno.h>
}

#include <iostream>

#include "os/unix_logger.h"

namespace Core {
namespace OS {
class UnixSigSet {
public:
    UnixSigSet() {
        sigfillset(&set);
    }

    void remove(int sig) {
        int ret = sigdelset(&set, sig);
        if (ret == -1) {
            SYSTEM_ERROR_LOG_TRACE("sigdelset has error")
        }
    }

    void block() {
        int ret = sigprocmask(SIG_BLOCK, &set, nullptr);
        if (ret == -1) {
            SYSTEM_ERROR_LOG_TRACE("sigprocmask has error")
        }
        return;
    }

    void unblock() {
        sigprocmask(SIG_UNBLOCK, &set, &oldSet);
    }

    ~UnixSigSet() {
    }

private:
    sigset_t set = {};
    sigset_t oldSet = {};
};
}
}
