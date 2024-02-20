#include "os/unix_pid_file.h"

extern "C" {
#include <fcntl.h>
#include <unistd.h>
}

#include <iostream>
#include <sstream>

#include "os/unix_logger.h" 

namespace Core {
namespace OS {
UnixPidFile::UnixPidFile(const std::string &pidFile, int flag) {
    //检查pid文件
    pidFd = ::open(pidFile.c_str(), flag, 0777);
    if (pidFd == -1) {
        SYSTEM_ERROR_LOG_TRACE("pid file({}) error:{}", pidFile, strerror(errno));
        exit(-1);
    }
}

bool UnixPidFile::tryWriteLock() {
    if (pidFd == -1) {
        LOG_CONTENT_ERR("pidFile error!");
        exit(-1);
        return false;
    }
    fileLock.l_type = F_WRLCK;
    fileLock.l_whence = SEEK_SET;
    fileLock.l_start = 0;
    fileLock.l_len = 0;
    int res = fcntl(pidFd, F_SETLK, &fileLock);
    return !res;
}

pid_t UnixPidFile::getPid() {
    if (pidFd <= 0) {
        SYSTEM_ERROR_LOG_TRACE("pidFile error!");
        exit(-1);
    }
    //持久化pid进程文件锁
    char buf[64];
    ssize_t res = ::read(pidFd, buf, sizeof(buf));
    if (res == -1) {
        SYSTEM_ERROR_LOG_TRACE(strerror(errno));
        exit(-1);
    }
    buf[res] = '\0';
    pid_t  process_pid = atoi(buf);
    return process_pid;
}

pid_t UnixPidFile::setPid() {
    std::stringstream pidStr;
    pid = getpid();
    pidStr << pid;
    lseek(pidFd, 0, SEEK_SET);
    int ret = ftruncate(pidFd, 0);
    if (ret == -1) {
        SYSTEM_ERROR_LOG_TRACE(strerror(errno));
    }

    //重新写入pid
    int res = write(pidFd, pidStr.str().c_str(), (pidStr.str().length()));

    if (res <= 0) {
        SYSTEM_ERROR_LOG_TRACE(strerror(errno));
        exit(-1);
    }

    return pid;
}

UnixPidFile::~UnixPidFile() {
    if (pidFd > 0) {
        ::close(pidFd);
    }
}
}
}