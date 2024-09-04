#include "os/unix_thread_proc.h"
#include <spdlog/spdlog.h>           // for SPDLOG_INFO
#include <functional>                // for function
#include <list>                      // for _List_iterator, list
#include "event/event_loop.h"        // for EventLoop
#include "os/unix_current_thread.h"  // for tid, currentLoop, currentThread
#include "os/unix_thread.h"          // for UnixThread

namespace Core::OS {
/**
 * @brief
 * 线程运行的具体处理入口
 */
void UnixThreadProc::runThread() {
  UnixCurrentThread::currentLoop = loop;
  UnixCurrentThread::currentThread = thread;
  SPDLOG_INFO("thread {} start", UnixCurrentThread::tid());
  for (auto &data : thread->initList) {
    data();
  }
  loop->loop();
}
} // namespace Core::OS
