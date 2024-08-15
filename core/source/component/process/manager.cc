/**
******************************************************************************
* @file           : manager.cc
* @author         : zhanglei
* @brief          : None
* @attention      : None
* @date           : 2024/2/11
******************************************************************************
*/
//
// Created by zhanglei on 2024/2/11.
//

#include "component/process/manager.h"

namespace Core {
namespace Component {
namespace Process {
void Manager::onExit(int /* sig */, short /* events */, void *param) {
    pid_t pid;
    int stat;
    pid = waitpid(-1, &stat, WNOHANG);
    if (pid <= 0) {
        return;
    }

    auto manager = static_cast<Manager*>(param);
    if (!manager) {
        return;
    }

    auto iter = manager->maps.find(pid);
    if (iter == manager->maps.end()) {
        return;
    }
    if (iter->second->getStatus() == STOPPING) {
        iter->second->setStatus(STOPPED);
        iter->second->onStop();
        manager->maps.erase(iter);
        return;
    }
    if (iter->second->getStatus() == RELOADING) {
        iter->second->setStatus(RELOAD);
        iter->second->onReload();
        manager->maps.erase(iter);
        return;
    }
    if (iter->second->getStatus() == DELETING) {
        iter->second->setStatus(DELETED);
        iter->second->onDestroy();
        manager->maps.erase(iter);
        return;
    }
    iter->second->setStatus(EXITED);
    iter->second->onExit(manager);
    return;
}
}
}
}
