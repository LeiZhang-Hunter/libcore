/**
******************************************************************************
* @file           : unix_cgroup.cc
* @author         : zhanglei
* @brief          : None
* @attention      : None
* @date           : 2024/2/12
******************************************************************************
*/
//
// Created by zhanglei on 2024/2/12.
//
#include "os/unix_cgroup.h"

#include <fmt/format.h>

namespace Core {
namespace OS {
CGroup::CGroup(const std::string &name) {
    int ret = cgroup_init();
    if (ret) {
        SYSTEM_EMERGENCY_LOG_TRACE("cgroup_init error:{}", cgroup_strerror(ret))
        return;
    }
    if (!name.empty()) {
        name_ = name;
    }
    cg = cgroup_new_cgroup(name_.c_str());
    if (!cg) {
        SYSTEM_EMERGENCY_LOG_TRACE("cgroup_new_cgroup is null")
        return;
    }
    cpuController = cgroup_get_controller(cg, "cpu");
    if (!cpuController) {
        cpuController = cgroup_add_controller(cg, "cpu");
    }
    memoryController = cgroup_get_controller(cg, "memory");
    if (!memoryController) {
        memoryController = cgroup_add_controller(cg, "memory");
    }
}

bool CGroup::doCpuRate() {
    if (rate <= 0) {
        return true;
    }

    if (!cpuController) {
        SYSTEM_EMERGENCY_LOG_TRACE("cpu controller is not null")
        return false;
    }
    cg_version_t version;
    auto ret = cgroup_get_controller_version("cpu", &version);
    if (ret) {
        SYSTEM_EMERGENCY_LOG_TRACE("cgroup_get_controller_version error({}):{}", ret, cgroup_strerror(ret))
        return false;
    }
    switch (version) {
        case CGROUP_V1: {
            ret = cgroup_add_value_string(cpuController, "cpu.cfs_period_us", "100000");
            if (ret) {
                SYSTEM_EMERGENCY_LOG_TRACE("cgroup_add_value_string error({}):{}", ret, cgroup_strerror(ret))
                return false;
            }
            auto f = (int)(rate * 100000);
            ret = cgroup_add_value_int64(cpuController, "cpu.cfs_quota_us", f);
            if (ret) {
                SYSTEM_EMERGENCY_LOG_TRACE("cgroup_add_value_int64 error:{}", cgroup_strerror(ret))
                return false;
            }
            return true;
            break;
        }
        case CGROUP_V2:
        {
            auto f = (int)(rate * 100000);
            ret = cgroup_add_value_string(cpuController, "cpu.max", fmt::format("{} {}", f, 100000).c_str());
            if (ret) {
                SYSTEM_EMERGENCY_LOG_TRACE("cgroup_add_value_int64 error:{}", cgroup_strerror(ret))
                return false;
            }
            return true;
        }
        default:
            return false;
    }

}

bool CGroup::doMemoryLimit() {
    if (limit <= 0) {
        return true;
    }
    if (!memoryController) {
        SYSTEM_EMERGENCY_LOG_TRACE("memory controller is not null")
        return false;
    }
    cg_version_t version;
    auto ret = cgroup_get_controller_version("cpu", &version);
    if (ret) {
        SYSTEM_EMERGENCY_LOG_TRACE("cgroup_get_controller_version error({}):{}", ret, cgroup_strerror(ret))
        return false;
    }
    switch (version) {
        case CGROUP_V1: {
            ret = cgroup_add_value_string(memoryController, "memory.swappiness", "0");
            if (ret) {
                SYSTEM_EMERGENCY_LOG_TRACE("cgroup_add_value_string error:{}", cgroup_strerror(ret))
                return false;
            }
            auto m = std::to_string(limit) + "M";
            ret = cgroup_add_value_string(memoryController, "memory.limit_in_bytes", m.c_str());
            if (ret) {
                SYSTEM_EMERGENCY_LOG_TRACE("cgroup_add_value_string error:{}", cgroup_strerror(ret))
                return false;
            }
            return true;
            break;
        }
        case CGROUP_V2: {
            auto m = std::to_string(limit) + "M";
            ret = cgroup_add_value_string(memoryController, "memory.max", m.c_str());
            if (ret) {
                SYSTEM_EMERGENCY_LOG_TRACE("cgroup_add_value_string error:{}", cgroup_strerror(ret))
                return false;
            }
            return true;
            break;
        }
        default:
            return false;
    }
    return false;
}

bool CGroup::attach(pid_t pid) {
    if (!cg) {
        return false;
    }
    int ret = cgroup_attach_task_pid(cg, pid);
    if (ret) {
        SYSTEM_EMERGENCY_LOG_TRACE("cgroup_attach_task_pid error:{}", cgroup_strerror(ret))
        return false;
    }
    return true;
}

bool CGroup::run() {
    if (!doCpuRate()) {
        return false;
    }

    if (!doMemoryLimit()) {
        return false;
    }

    int ret = cgroup_create_cgroup(cg, 0);
    if (ret && ret != ECGROUPVALUENOTEXIST) {
        SYSTEM_EMERGENCY_LOG_TRACE("cgroup_attach_task_pid error({}):{}", ret, cgroup_strerror(ret))
        cgroup_free(&cg);
        return false;
    }
    return true;
}
}
}