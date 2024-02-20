#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "os/unix_logger.h"
#include "os/unix_thread.h"
#include "build_expect.h"

/**
 * @brief 引用线程日志
 * 
 */
#ifndef THREAD_LOGGER_EXTERN
#define THREAD_LOGGER_EXTERN extern std::shared_ptr<Core::Component::LoggerThread> loggerThreadHandle;
#endif

/**
 * @brief 声明线程日志
 * 
 */
#ifndef THREAD_LOGGER_DECLARE
#define THREAD_LOGGER_DECLARE std::shared_ptr<Core::Component::LoggerThread> loggerThreadHandle;
#endif

/**
 * @brief 初始化日志
 * 
 */
#ifndef THREAD_LOGGER_INIT
#define THREAD_LOGGER_INIT(name, path, ext, enableStdout) loggerThreadHandle = std::make_shared<Core::Component::LoggerThread>(name, path, ext, enableStdout);
#endif

/**
 * @brief 启动线程日志
 * 
 */
#ifndef THREAD_LOGGER_START
#define THREAD_LOGGER_START loggerThreadHandle->start();
#endif

/**
 * @brief 启动线程日志
 * 
 */
#ifndef THREAD_LOGGER_STOP
#define THREAD_LOGGER_STOP loggerThreadHandle->stop();
#endif

#ifndef THREAD_EMERGENCY_LOG_TRACE
#define THREAD_EMERGENCY_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_EMERGENCY, content);\
} else { OS::system_write("emergency", content); } \
};
#endif


#ifndef THREAD_ALERT_LOG_TRACE
#define THREAD_ALERT_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_ALERT, content);\
} else { OS::system_write("alert", content); } \
};
#endif

#ifndef THREAD_CRITICAL_LOG_TRACE
#define THREAD_CRITICAL_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_CRITICAL, content);\
} else { OS::system_write("critical", content); } \
};
#endif

#ifndef THREAD_ERROR_LOG_TRACE
#define THREAD_ERROR_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_ERROR, content);\
} else { OS::system_write("error", content); } \
};
#endif

#ifndef THREAD_WARNING_LOG_TRACE
#define THREAD_WARNING_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_WARNING, content);\
} else { OS::system_write("warning", content); } \
};
#endif

#ifndef THREAD_NOTICE_LOG_TRACE
#define THREAD_NOTICE_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_NOTICE, content);\
} else { OS::system_write("notice", context); } \
};
#endif

#ifndef THREAD_INFO_LOG_TRACE
#define THREAD_INFO_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_INFO, content);\
} else { OS::system_write("info", context); } \
};
#endif

#ifndef THREAD_DEBUG_LOG_TRACE
#define THREAD_DEBUG_LOG_TRACE(context) \
{ \
LOG_CONTENT(context) \
if (build_likely(loggerThreadHandle)) { \
loggerThreadHandle->log(LOG_DEBUG, content);\
} else { OS::system_write("debug", context); } \
};
#endif

namespace Core {
namespace Component {

class LoggerBufferDispatcher;

class LoggerThread :public OS::UnixThread {
public:
    /**
     * @brief Construct a new Logger Thread object
     * 
     * 日志文件名字
     * @param name 
     * 日志文件路径名字
     * @param path 
     * 日志文件后缀
     * @param ext 
     */
    LoggerThread(const std::string& name, const std::string& path, const std::string& ext = "", bool enableStdout = false);

    ~LoggerThread() {

    }

    /**
     * @brief 写入日志
     * 
     */
    bool log(int level, const std::string& msg);

    bool start() override;

private:
    /**
     * @brief 日志缓冲区装载的类
     * 
     */
    std::shared_ptr<LoggerBufferDispatcher> bufferDispatcher;

    
};
}
}
