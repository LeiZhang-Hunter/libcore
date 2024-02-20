#pragma once

extern "C" {
#include <unistd.h>
}

#include <functional>
#include <map>
#include <iostream>

namespace Core {
namespace OS {

class UnixCommand {

    typedef std::function<bool(int argc, char **argv, const char &)> cmdClosureType;
    typedef std::map<char, cmdClosureType> cmdMapType;
    typedef std::map<const char, const std::string &> cmdResultMapType;

public:
    UnixCommand() {

    }

    ~UnixCommand() {

    }

    void setCmdArgC(int argc) {
        cmdArgC = argc;
    }

    void setCmdArgV(char **argv) {
        cmdArgV = argv;
    }

    /**
     * 将注册的命令加入到Map中
     * @param cmd
     */
    void reg(const char &cmd, const cmdClosureType &closure) {
        cmdMap[cmd] = closure;
    }

    char **getCmdArgv() {
        return cmdArgV;
    }

    int getCmdArgC() {
        return cmdArgC;
    }

    //执行解析
    bool parse();


private:
    cmdMapType cmdMap;
    cmdResultMapType cmdResultMap;
    int cmdArgC = 0;
    char **cmdArgV = nullptr;
};
} // namespace app
}