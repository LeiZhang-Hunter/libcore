#pragma once

extern "C" {
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
}
#include <string>
#include "build_expect.h"
#include "os/unix_timestamp.h"
namespace Core {
namespace OS {

/**
* @brief linux下的api用来检查是否是目录、文件夹
*
* @param path
* @return true
* @return false
*/
static inline bool isDir(const std::string &path) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) == 0) {
        if (fileInfo.st_mode & S_IFDIR) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

static inline bool isFile(const std::string &path) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) == 0) {
        if (fileInfo.st_mode & S_IFREG) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

static ssize_t writen(int fd, void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = (const char *) vptr;
    nleft = n;

    while (nleft > 0) {
        if ((nwritten = ::write(fd, ptr, nleft)) <= 0) {
            if (build_likely(errno == EINTR)) {
                nwritten = 0;
            } else {
                return -1;
            }


        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

static inline ssize_t system_write(const std::string &level, const std::string &message) {
    std::string data;
    UnixTimestamp time;
    data = data + time.toMicroDate() + " [level:" + level + "]" + " content: " + message + ";errno:" + std::to_string(errno) + ";errmsg:" +
            strerror(errno) + "\n";
    std::cerr << data << std::endl;
    int writeFd = open("/var/log/agent-error.log", O_CREAT | O_RDWR | O_APPEND, 0664);
    ssize_t res = writen(writeFd, (void *) data.c_str(), data.size());
    close(writeFd);
    return res;
}

static inline void set_noblock(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

static inline bool checkEndian() {
    union {
        short s;
        char c[2];
    } endian;
    endian.s = 0x0102;
    if (endian.c[0] == 0x01) {
        return true;
    }
    return false;
}

static inline std::string get_file_contents(const char *fpath) {
    std::ifstream finstream(fpath);
    std::string contents;
    contents.assign((std::istreambuf_iterator<char>(finstream)), std::istreambuf_iterator<char>());
    finstream.close();
    return contents;
}

static inline int64_t getNowTime() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return seconds * 1000 * 1000 + tv.tv_usec;
}



// std::string toFormatString(bool showMicroseconds) {
//     char buf[64] = {0};
//     timeval val;
//     gettimeofday(&val, nullptr);
//     struct tm tm_time;
//     gmtime_r(&val.tv_sec, &tm_time);

//     if (showMicroseconds)
//     {
//         snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
//                     tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
//                     tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
//                     val.tv_usec);
//     }
//     else
//     {
//         snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
//                     tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
//                     tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
//     }
//     return buf;
// }



static ssize_t readch(int fd, char *ptr) {
    static int count = 0;
    static char *read_ptr = 0;
    static char read_buf[1024 * 4] = {0};

    if (count <= 0) {
        again:
        count = read(fd, read_buf, sizeof(read_buf));
        if (-1 == count)
            if (EINTR == errno)
                goto again;
            else
                return -1;
        else if (0 == count)
            return 0;
        read_ptr = read_buf;
    }
    count--;
    *ptr = *read_ptr++;
    return 1;
}

/**
* 读取一行
* @param fd
* @param vptr
* @param maxlen
* @return
*/
static ssize_t readline(int fd, void *vptr, size_t maxlen) {
    ssize_t i = 0;
    ssize_t ret = 0;
    char ch = '\0';
    char *ptr = nullptr;

    ptr = (char *) vptr;

    for (i = 1; i < maxlen; ++i) {
        ret = readch(fd, &ch);
        if (1 == ret) {
            *ptr++ = ch;
            if ('\n' == ch)
                break;
        } else if (0 == ret) {
            *ptr = 0;
            return i - 1;
        } else
            return -1;
    }
    *ptr = 0;
    return i;
}

//谷歌规范
//https://github.com/google/llvm-propeller/blob/45c226984fe8377ebfb2ad7713c680d652ba678d/lldb/test/API/tools/lldb-server/thread-name/main.cpp#L4
static inline void set_thread_name(const char *name) {
#if defined(__APPLE__)
    ::pthread_setname_np(name);
#elif defined(__FreeBSD__)
    ::pthread_set_name_np(::pthread_self(), name);
#elif defined(__linux__)
    ::pthread_setname_np(::pthread_self(), name);
#elif defined(__NetBSD__)
    ::pthread_setname_np(::pthread_self(), "%s", const_cast<char *>(name));
#endif
}

static inline bool getSockAddress(int fd, const struct sockaddr_in &sa) {
    int len = sizeof(sa);
    if (getpeername(fd, (struct sockaddr *) &sa, reinterpret_cast<socklen_t *>(&len))) {
        OS::system_write("warning", strerror(errno));
        return false;
    }
    return true;
}
}
}