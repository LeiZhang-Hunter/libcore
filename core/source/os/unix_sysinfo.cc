//
// Created by zhanglei on 10/22/21.
//
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
}
#include "os/unix_logger.h"
#include "os/unix_util.h"
#include "os/unix_sysinfo.h"

namespace Core {
namespace OS {
bool UnixSysInfo::load(SysData& data, StatusData& status) {

    memset(&data, 0, sizeof(data));
    if (!OS::isFile(procPath)) {
        SYSTEM_WARNING_LOG_TRACE("{} is not file", procPath);
        return false;
    }

    int fd = open(procPath.c_str(), O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        SYSTEM_WARNING_LOG_TRACE( "open {} failed", procPath);
        return false;
    }

    std::string context;

    /**
     * 读取文件，这不会执行任何非阻塞, 除非被中断才会重新读
     */
    while(true) {
        char buf[BUFSIZ];
        ssize_t readSize = read(fd, buf, BUFSIZ);
        if (readSize == 0) {
            if (errno == EINTR) {
                continue;
            } else {
                break;
            }
        } else if (readSize < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                break;
            }
        }
        buf[readSize] = '\0';
        context.append(buf);
    }

    int ret = close(fd);
    if (ret != 0) {
        SYSTEM_WARNING_LOG_TRACE( "close {} failed", procPath);
    }
    //解析系统指标到内存
    ret = sscanf(context.c_str(),
                 "%d %s %c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
                 &(data.pid),
                 (data.exName),
                 &(data.state),
                 &(data.ppid),
                 &(data.pgrp),
                 &(data.session),
                 &(data.tty),
                 &(data.tpgid),
                 &(data.flags),
                 &(data.minflt),
                 &(data.cminflt),
                 &(data.majflt),
                 &(data.cmajflt),
                 &(data.utime),
                 &(data.stime),
                 &(data.cutime),
                 &(data.cstime),
                 &(data.counter),
                 &(data.priority),
                 &(data.timeout),
                 &(data.itrealvalue),
                 &(data.starttime),
                 &(data.vsize),
                 &(data.rss),
                 &(data.rlim),
                 &(data.startcode),
                 &(data.endcode),
                 &(data.startstack),
                 &(data.kstkesp),
                 &(data.kstkeip),
                 &(data.signal),
                 &(data.blocked),
                 &(data.sigignore),
                 &(data.sigcatch),
                 &(data.wchan)
          );

    fd = open(procStatusPath.c_str(), O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        SYSTEM_WARNING_LOG_TRACE( "open {} failed", procStatusPath);
        return false;
    }

    char lineBuf[BUFSIZ];
    char name[64];
    uint64_t memSize = 0;

    while(OS::readline(fd, lineBuf, BUFSIZ)) {
        sscanf(lineBuf,"%s %lu",name,&memSize);
        if (strcasecmp("VmRSS:", name) == 0) {
            status.rss = memSize;
        }

        if (strcasecmp("VmSize:", name) == 0) {
            status.vmSize = memSize;
        }
    }

    ret = close(fd);
    if (ret != 0) {
        SYSTEM_WARNING_LOG_TRACE( "close {} failed", procStatusPath);
    }

    //获取总cpu使用情况
    fd = open(statPath.c_str(), O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        SYSTEM_WARNING_LOG_TRACE( "open {} failed", statPath);
        return false;
    }

    std::string statContext;

    /**
     * 读取文件，这不会执行任何非阻塞, 除非被中断才会重新读
     */
    while(true) {
        char buf[BUFSIZ];
        ssize_t readSize = read(fd, buf, BUFSIZ);
        if (readSize == 0) {
            if (errno == EINTR) {
                continue;
            } else {
                break;
            }
        } else if (readSize < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                break;
            }
        }
        buf[readSize] = '\0';
        statContext.append(buf);
    }

    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    sscanf(statContext.c_str(), "%s %ld %ld %ld %ld",name,&user, &nice, &system,&idle);

    unsigned long procCpuTime = data.utime + data.stime + data.cutime + data.cstime;
    unsigned long sysTotalTime = (user + nice + system + idle);
    if (!procCpuRecordTime && !sysCpuRecordTime) {
        status.cpu = 0;
    } else {
        status.cpu = static_cast<float>((((static_cast<float>(procCpuTime - procCpuRecordTime))
                / static_cast<float >(sysTotalTime - sysCpuRecordTime))  * 100.0));
    }


    ret = close(fd);
    if (ret != 0) {
        SYSTEM_WARNING_LOG_TRACE( "close {} failed", procStatusPath);
    }

    procCpuRecordTime = procCpuTime;
    sysCpuRecordTime = sysTotalTime;
    return true;
}
}
}
