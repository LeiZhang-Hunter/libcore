#include "component/logger_thread.h"

#include "component/timer_channel.h"

namespace Core {
namespace Component {

/**
 * @brief buffer的派遣类，写在cc里不支持外部使用
 * 
 */
class LoggerBufferDispatcher {
public:
    LoggerBufferDispatcher(const std::string& name, const std::string& path, const std::string& ext = "", bool enableStdout_ = false)
    :logger(std::make_unique<OS::UnixLogger>(name, path, ext)), enableStdout(enableStdout_) {

    };

    ~LoggerBufferDispatcher() {
        buffer.clear();
    }

    /**
     * @brief 把数据追加到缓冲区
     * 
     * @param msg 
     */
    void load(int level, const std::string& msg) {
        ssize_t size = msg.size();

        //单个包的长度超过了最大包长
        if (build_unlikely(size >= unitMaxSize)) {
            SYSTEM_WARNING_LOG_TRACE("log unit max size more");
            return;
        }


        //超过最大包了，丢弃日志
        if (build_unlikely(nowPackageSize >= bufferMaxPackage)) {
            flush();
            return;
        }

        std::string content = logger->getContext(level, msg);
        try {
            stringBuffer.append(content);
        } catch (std::exception& err) {
            std::cout << err.what() << std::endl;
        }
        nowPackageSize++;

        return;
    }

    /**
     * @brief 将数据追加到日志
     * 
     */
    void flush() {
        if (stringBuffer.empty()) {
            return;
        }
        std::string newBufferStr;
        nowPackageSize = 0;
        newBufferStr.swap(stringBuffer);
        if (enableStdout) {
            std::cout << newBufferStr.data();
        } else {
            logger->input(newBufferStr);
        }
    }

private:
    //缓冲区
    std::vector<uint8_t> buffer;
    std::string stringBuffer;
    //logger
    std::unique_ptr<OS::UnixLogger> logger;
    
    /**
     * @brief 最大的包长度
     * 
     */
    ssize_t unitMaxSize = BUFSIZ;

    /**
     * @brief 缓冲区最多存放多少个包
     * 
     */
    int bufferMaxPackage = 30;

    /**
     * @brief 当前日志包的数目
     * 
     */
    int nowPackageSize = 0;

    /**
     * 是否输出到控制台
     */
    bool enableStdout = false;
};

LoggerThread::LoggerThread(const std::string& name, const std::string& path, const std::string& ext, bool enableStdout)
:bufferDispatcher(std::make_shared<LoggerBufferDispatcher>(name, path, ext, enableStdout)) {}

bool LoggerThread::log(int level, const std::string& msg) {

    addTask(std::bind(&LoggerBufferDispatcher::load, bufferDispatcher, level, msg));
    return true;
}

bool LoggerThread::start() {
    std::shared_ptr<Component::TimerChannel> channel = std::make_shared<Component::TimerChannel>(loop, std::bind(&LoggerBufferDispatcher::flush, bufferDispatcher));
    channel->enable(std::chrono::milliseconds(3000));
    OS::UnixThread::start();
    return true;
}
}
}
