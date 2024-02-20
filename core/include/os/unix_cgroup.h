/**
******************************************************************************
* @file           : unix_cgroup.h
* @author         : zhanglei
* @brief          : None
* @attention      : None
* @date           : 2024/2/12
******************************************************************************
*/
//
// Created by zhanglei on 2024/2/12.
//

#pragma once

#include <string>

extern "C" {
#include <libcgroup.h>
}

#include "os/unix_logger.h"

namespace Core {
namespace OS {
class CGroup {
public:
    CGroup(const std::string& name);

    bool run();

    ~CGroup() {
        if (cg) {
            cgroup_delete_cgroup(cg, 0);
            cgroup_free(&cg);
        }
    }

    void setCpuRate(float cpu) {
        rate = cpu;
    }

    void setMemoryLimit(int limit_) {
        limit = limit_;
    }

    bool attach(pid_t pid);

private:
    bool doCpuRate();

    bool doMemoryLimit();

    std::string name_ = "watchermen";
    struct cgroup *cg = nullptr;
    struct cgroup_controller *cpuController = nullptr;
    struct cgroup_controller *memoryController = nullptr;
    int limit = 0;
    float rate = 0;
    bool isRun = false;
};
}
}
