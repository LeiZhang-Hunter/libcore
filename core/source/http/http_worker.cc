#include "http/http_worker.h"
#include <arpa/inet.h>               // for htons
#include <fcntl.h>                   // for fcntl, F_GETFL, F_SETFL, O_NONBLOCK
#include <netinet/in.h>              // for sockaddr_in, INADDR_ANY, in_addr
#include <sys/socket.h>              // for setsockopt, AF_INET, SOL_SOCKET
#include <cstring>                   // for memset
#include <memory>                    // for __shared_ptr_access, shared_ptr
#include "event/event_loop.h"        // for EventLoop
#include "os/unix_current_thread.h"  // for currentLoop

namespace Core::Http {

int HttpWorker::openServer() const {
  int nfd = socket(AF_INET, SOCK_STREAM, 0);
  if (nfd < 0) {
    //        THREAD_NOTICE_LOG_TRACE("create socket error")
    return -1;
  }

  int one = 1;
  setsockopt(nfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));
  setsockopt(nfd, SOL_SOCKET, SO_REUSEPORT, (char *)&one, sizeof(int));

  struct sockaddr_in addr{};
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  int r = bind(nfd, (struct sockaddr *)&addr, sizeof(addr));
  if (r < 0) {
#ifdef USE_DEBUG
    std::cout << "Error11" << std::endl;
#endif
    //        THREAD_NOTICE_LOG_TRACE("bind socket error" + std::to_string(port))
    return -1;
  }
  r = listen(nfd, 1024);
  if (r < 0) {
#ifdef USE_DEBUG
    std::cout << "listen Error" << std::endl;
#endif
    //        THREAD_NOTICE_LOG_TRACE("listen error")
    return -1;
  }

  int flags;
  if ((flags = fcntl(nfd, F_GETFL, 0)) < 0 || fcntl(nfd, F_SETFL, flags | O_NONBLOCK) < 0) {
    //        THREAD_NOTICE_LOG_TRACE("fcntl O_NONBLOCK error")
    return -1;
  }

  return nfd;
}

void HttpWorker::init(Event::EventLoop *loop) {
  if (!httpRequestHandle) {
    return;
  }

  listener = openServer();
  if (listener == -1) {
    return;
  }

  if (loop == nullptr) {
    httpHandle.Reset(evhttp_new(OS::UnixCurrentThread::currentLoop->getEventBase()));
  } else {
    httpHandle.Reset(evhttp_new(loop->getEventBase()));
  }

  bound = evhttp_accept_socket_with_handle(httpHandle.get(), listener);

  if (bound == nullptr) {
    //        THREAD_ERROR_LOG_TRACE("Error evhttp_accept_socket");
    return;
  }

  // 设置请求超时时间(s)
  evhttp_set_timeout(httpHandle.get(), httpTimeout);

  // 设置处理函数
  evhttp_set_gencb(httpHandle.get(), httpRequestHandle, args);
}

HttpWorker::~HttpWorker() {
  if (listener != -1) {
    evhttp_del_accept_socket(httpHandle.get(), bound);
    close(listener);
  }
}
} // namespace Core::Http
