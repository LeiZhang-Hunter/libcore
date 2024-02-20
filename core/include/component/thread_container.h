#pragma once

#include <memory>
#include <unordered_map>

#include "non_copyable.h"
#include "event/event_queue.h"
#include "os/unix_thread.h"

namespace Core {
namespace Component {

typedef std::unordered_map<pid_t, std::shared_ptr<OS::UnixThread>> containerMap;

/**
 * @brief 线程容器，用来存储线程
 * 
 */
class UnixThreadContainer : public Core::Noncopyable, public std::enable_shared_from_this<UnixThreadContainer> {
public:

    UnixThreadContainer() :empty(std::shared_ptr<OS::UnixThread>()) {

    }

    bool task(pid_t index, const Event::Task& task);

    /**
     * @brief 注册线程
     * 
     */
    bool reg(pid_t index, const std::shared_ptr<OS::UnixThread>& thread);

    /**
     * @brief 启动线程容器
     * 
     */
    void start();

    /**
     * @brief 关闭线程容器
     * 
     */
    void stop();

    /**
     * @brief 获取线程对象
     * 
     * @param index 
     * @return const std::shared_ptr<UnixThread>& 
     */
    std::shared_ptr<OS::UnixThread>& getThread(int index);

    /**
     * @brief 获取容器
     * 
     * @return const containerMap& 
     */
    const containerMap& getContainer() {
        return container;
    }

    void broadcastEvent(const Event::Task& task);

    ~UnixThreadContainer() {

    }

    uint16_t size() {
        return container.size();
    }

private:
    std::shared_ptr<OS::UnixThread> empty;
    //线程派遣管理器
    containerMap container;
};
}
}