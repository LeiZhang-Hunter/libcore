#include "os/unix_current_thread.h"
namespace Core { namespace Event { class EventLoop; } }
namespace Core { namespace OS { class UnixThread; } }
namespace Core {
namespace OS {
namespace UnixCurrentThread {
thread_local int t_cachedTid = 0;
thread_local std::shared_ptr<Event::EventLoop> currentLoop;
thread_local std::shared_ptr<UnixThread> currentThread;

}
}
}
