/**
******************************************************************************
* @file           : watcher.h
* @author         : zhanglei
* @brief          : None
* @attention      : None
* @date           : 2024/2/11
******************************************************************************
*/
//
// Created by zhanglei on 2024/2/11.
//

#pragma once

extern "C" {
#include <sys/inotify.h>
#include <unistd.h>
}
#include <string>

namespace Core {
namespace Component {
namespace Discovery {
class Watcher {
public:
    Watcher() :fd(inotify_init()){};

    bool reg(const std::string path, uint32_t __mask) {
        int watchDescriptor = inotify_add_watch(fd, path.c_str(), __mask);
        if (watchDescriptor == -1) {
            return false;
        }
        return true;
    }

    int getFd() {
        return fd;
    }

    ~Watcher() {
        if (fd != -1) {
            close(fd);
        }
    };
private:
    int fd = -1;
};
}
}
}
