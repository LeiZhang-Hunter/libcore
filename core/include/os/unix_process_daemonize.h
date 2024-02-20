#pragma once

extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
}

#include "unix_logger.h"

namespace Core {
namespace OS {

/**
* @brief 设置进程为精灵进程
*
*/
class UnixProcessDaemonize {
public:
    UnixProcessDaemonize() {};

    bool setDaemonize() {
        pid_t pid = fork();

        if (pid < 0) {
            SYSTEM_ERROR_LOG_TRACE("create daemon process error");
            return false;
        } else if (pid > 0) {
            //杀死掉父进程
            exit(0);
        }

        //如果说设置进程组组长失败，
        if (setsid() < 0) {
            SYSTEM_ERROR_LOG_TRACE("create daemon process error");
            return false;
        }


        //这里需要忽略掉信号
        signal(SIGHUP, SIG_IGN);

        pid = fork();

        if (pid < 0)
            return false;
        else if (pid)
            exit(0);

        int ret = chdir("/");
        if (ret < 0) {
            SYSTEM_ERROR_LOG_TRACE("chdir error");
            return false;
        }

        //重定向 输入 输出 错误输出
        for (int i = 0; i < 3; i++)
            close(i);

        open("/dev/null", O_RDONLY);
        open("/dev/null", O_RDWR);
        open("/dev/null", O_RDWR);

        return true;
    }

    ~UnixProcessDaemonize() {};
};
}
}