#pragma once
#include <shared_mutex>

namespace Core {
namespace Component {
class SharedLock {
public:

    void shared_lock() {
        mutex_.lock_shared();
    }

    void shared_unlock() {
        mutex_.unlock_shared();
    }

    void mutex_lock() {
        mutex_.lock();
    }

    void mutex_unlock() {
        mutex_.unlock();
    }

private:
    std::shared_mutex mutex_;
};
}
}
