#include "event/event_channel.h"
#include <errno.h>
#include <spdlog/spdlog.h>
#include <string.h>
#include "event/event_loop.h"

namespace Core {
namespace Event {
/**
* @brief 关闭描述符
*
*/
void EventChannel::close() {
    if (fd < 0) {
        return;
    }
    loop->deleteChannel(shared_from_this());
}

void EventChannel::shutdownWrite() {
    if (fd < 0) {
        return;
    }
    closeType = SHUT_WR;
    int ret = ::shutdown(fd, SHUT_WR);
    if (ret == -1) {
        SPDLOG_ERROR("{} shutdown SHUT_WR error! error msg: {}", fd, strerror(errno));
    }
}

void EventChannel::shutdownRead() {
    if (fd < 0) {
        return;
    }
    int ret = ::shutdown(fd, SHUT_RD);
    closeType = SHUT_RD;
    if (ret != 0) {
        SPDLOG_ERROR("{} shutdown SHUT_WR error! error msg: {}", fd, strerror(errno));
    }
//    loop->deleteChannel(shared_from_this());
}
}

}
