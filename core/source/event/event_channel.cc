#include "event/event_channel.h"

#include "os/unix_logger.h"
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
        std::string error;
        error += std::to_string(fd) + " shutdown SHUT_WR error! error msg:" + strerror(errno);
        SYSTEM_ERROR_LOG_TRACE(error);
    }
//    loop->deleteChannel(shared_from_this());
}

void EventChannel::shutdownRead() {
    if (fd < 0) {
        return;
    }
    int ret = ::shutdown(fd, SHUT_RD);
    closeType = SHUT_RD;
    if (ret != 0) {
        std::string error;
        error += std::to_string(fd) + " shutdown SHUT_RD error! error msg:" + strerror(errno);
        SYSTEM_ERROR_LOG_TRACE(error);
    }
//    loop->deleteChannel(shared_from_this());
}
}

}