#pragma once

extern "C" {
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <errno.h>
}

#include <string>
#include <iostream>
#include <climits>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>

#include <fmt/format.h>

#include "os/unix_util.h"

/**
* @brief 日志登记
*
*/
#define LOG_EMERGENCY 0
#define LOG_ALERT 1
#define LOG_CRITICAL 2
#define LOG_ERROR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

#ifndef LOG_CONTENT
#define LOG_CONTENT(context) std::string content; content = content +  context + " [file:" + __FILE__ +  "]" + "[line:" +  std::to_string(__LINE__)  + "] [errormsg:" + strerror(errno) + "]";
#endif

#ifndef LOG_CONTENT_ERR
#define LOG_CONTENT_ERR(context) \
{ \
LOG_CONTENT(context) \
std::cerr << content << std::endl; \
};
#endif

/**
* @brief 这是系统级别的宏定义，主要是在背景日志没有起来的时候，会写入到/va/log/agent-error.log下面
*
*/
#ifndef SYSTEM_EMERGENCY_LOG_TRACE
#define SYSTEM_EMERGENCY_LOG_TRACE(fmtStr, ...) \
Core::OS::system_write("emergency", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif


#ifndef SYSTEM_ALERT_LOG_TRACE
#define SYSTEM_ALERT_LOG_TRACE(fmtStr, ...) \
Core::OS::system_write("alert", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif

#ifndef SYSTEM_CRITICAL_LOG_TRACE
#define SYSTEM_CRITICAL_LOG_TRACE(fmtStr, ...) Core::OS::system_write("critical", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif

#ifndef SYSTEM_ERROR_LOG_TRACE
#define SYSTEM_ERROR_LOG_TRACE(fmtStr, ...) \
Core::OS::system_write("error", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif

#ifndef SYSTEM_WARNING_LOG_TRACE
#define SYSTEM_WARNING_LOG_TRACE(fmtStr, ...) \
Core::OS::system_write("warning", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif

#ifndef SYSTEM_NOTICE_LOG_TRACE
#define SYSTEM_NOTICE_LOG_TRACE(fmtStr, ...) \
Core::OS::system_write("notice", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif

#ifndef SYSTEM_INFO_LOG_TRACE
#define SYSTEM_INFO_LOG_TRACE(fmtStr, ...) \
Core::OS::system_write("info", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif

#ifndef SYSTEM_DEBUG_LOG_TRACE
#define SYSTEM_DEBUG_LOG_TRACE(fmtStr, ...) \
Core::OS::system_write("debug", fmt::format("{};file:{},line:{}", fmt::format(fmtStr, ##__VA_ARGS__), __FILE__, __LINE__));
#endif

namespace Core {
namespace OS {

/**
* @brief 最底层的日志记录事件
*
*/
class UnixLogger {
public:
    UnixLogger(const std::string &name, const std::string &path, std::string ext = "");

    ~UnixLogger() {

    }

    /**
     * @brief 系统不可用级别
     *
     * @param message
     */
    bool emergency(const std::string &message);

    /**
     * @brief 必须**立刻采取行动
     *
     * @param message
     */
    bool alert(const std::string &message);

    /**
     * @brief 紧急情况
     *
     * @param message
     */
    bool critical(const std::string &message);

    /**
     * @brief 运行时出现的错误，不需要立刻采取行动，但必须记录下来以备检测。
     *
     * @param message
     */
    bool error(const std::string &message);

    /**
     * @brief 出现非错误性的异常。 * * 例如：使用了被弃用的API、错误地使用了API或者非预想的不必要错误。
     *
     * @param message
     */
    bool warning(const std::string &message);

    /**
     * @brief 一般性重要的事件。
     *
     * @param message
     */
    bool notice(const std::string &message);

    /**
     * @brief 重要事件
     *
     * @param message
     */
    bool info(const std::string &message);

    /**
     * @brief debug 详情
     *
     * @param message
     */
    bool debug(const std::string &message);

    /**
     * @brief 任意等级的日志记录
     *
     * @param level
     * @param message
     */
    bool log(int level, const std::string &message);

    /**
     * @brief 获取文本内容
     *
     * @return std::string
     */
    std::string getContext(int level, const std::string &message);

    /**
     * @brief 输入buffer
     *
     * @param buffer
     * @return true
     * @return false
     */
    bool input(const std::vector<uint8_t> &buffer);

    /**
     * @brief 输入buffer
     *
     * @param buffer
     * @return true
     * @return false
     */
    bool input(const std::string &buffer);

private:
    //日志记录文件的名字
    std::string filePath;
    //日志后缀
    std::string fileExt;
};
}
}