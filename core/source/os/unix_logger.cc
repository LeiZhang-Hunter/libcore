#include "os/unix_logger.h"

namespace Core {
namespace OS {

/**
* @brief 日志等级对照表
*
*/
static std::string loggerLevelTable[8] = {
        "emergency",
        "alert",
        "critical",
        "error",
        "warning",
        "notice",
        "info",
        "debug"
};

UnixLogger::UnixLogger(const std::string &name, const std::string &path, std::string ext) {
    if (build_unlikely(name.empty())) {
        throw std::runtime_error("name not empty!");
    }

    if (build_unlikely(path.empty())) {
        throw std::runtime_error("log path not empty!");
    }

    if (build_unlikely(ext.empty())) {
        ext = "log";
    }

    if (build_unlikely(!isDir(path))) {
        throw std::runtime_error("path(" + path + ") error!");
    }

    char buf[PATH_MAX];
    realpath((path + "/" + name).c_str(), buf);
    filePath = buf;
}

bool UnixLogger::emergency(const std::string &message) {
    return log(LOG_EMERGENCY, message);
}

bool UnixLogger::alert(const std::string &message) {
    return log(LOG_ALERT, message);
}

bool UnixLogger::critical(const std::string &message) {
    return log(LOG_CRITICAL, message);
}

bool UnixLogger::error(const std::string &message) {
    return log(LOG_ERROR, message);
}

bool UnixLogger::warning(const std::string &message) {
    return log(LOG_WARNING, message);
}

bool UnixLogger::notice(const std::string &message) {
    return log(LOG_NOTICE, message);
}

bool UnixLogger::info(const std::string &message) {
    return log(LOG_INFO, message);
}

bool UnixLogger::debug(const std::string &message) {
    return log(LOG_DEBUG, message);
}

std::string UnixLogger::getContext(int level, const std::string &message) {
    UnixTimestamp time;
    std::string data;
    data = data + time.toMicroDate() + " [level:" + loggerLevelTable[level] + "]" + " content: " + message +
           "\n";
    return data;

}

bool UnixLogger::log(int level, const std::string &message) {
    std::string context;
    UnixTimestamp time;
    std::string logPath = filePath + "-" + time.toHourDateForLog() + ".log";
    int writeFd;
    std::string data;
    data = data + time.toMicroDate() + " [level:" + loggerLevelTable[level] + "]" + " content: " + message +
           "\n";

    /**
     * @brief 这里直接调用的write，我对ofstream和write做了压测
     * ofstream 的10000次写入性能在 18338 ms
     * write在 13026 ms
     *
     */
    writeFd = open(logPath.c_str(), O_CREAT | O_RDWR | O_APPEND, 0664);
    ssize_t res = writen(writeFd, (void *) data.c_str(), data.size());
    close(writeFd);

    // std::ofstream file;
    // file.open(logPath, std::ios::app);
    // if (!file.is_open()) {
    //     LOG_CONTENT("open " + filePath + " failed");
    //     throw std::runtime_error(content);
    // }
    // file << time.toMicroDate() << " [level:" + loggerLevelTable[level] + "]" << " content: " << message  << std::endl;
    // file.close();
    return true;
}

bool UnixLogger::input(const std::string &buffer) {
    if (buffer.empty()) {
        return false;
    }
    std::string context;
    UnixTimestamp time;
    std::string logPath = filePath + "-" + time.toHourDateForLog() + ".log";
    int writeFd = open(logPath.c_str(), O_CREAT | O_RDWR | O_APPEND | O_NONBLOCK, 0664);
    ssize_t res = writen(writeFd, (void *) buffer.data(), buffer.size());
    close(writeFd);
    return true;
}
}
}