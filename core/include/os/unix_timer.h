#pragma once

extern "C" {
#include <sys/timerfd.h>
#include <errno.h>
#include <unistd.h>
}
#include <cstring>
#include "build_expect.h"
#include "unix_logger.h"

namespace Core {
namespace OS {

    class UnixTimer {
    public:

        UnixTimer() {

        }

        /**
         * @brief 初始化unix定时器，时间单位是毫秒
         *
         * @param second
         */
        explicit UnixTimer(time_t unit);

        int getTimerFd() {
            return timerFd;
        }

        /**
         * @brief 由于timer 会被绑定到channel，channel析构函数会自动关闭描述符，所以不需要手动再去关闭了
         *
         */
        virtual ~UnixTimer() {
            // ::close(timerFd);
        }

    private:
        int timerFd = -1;
    };
}
}