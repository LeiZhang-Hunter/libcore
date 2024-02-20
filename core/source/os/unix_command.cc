#include "os/unix_command.h"

#include "os/unix_logger.h"

namespace Core {
namespace OS {

bool UnixCommand::parse() {
    //遍历map组装结果
    cmdMapType::iterator cmdMapIterator;
    std::string cmdOptionString;
    std::string option;

    //检查argc
    if (cmdArgC == 0) {
        SYSTEM_CRITICAL_LOG_TRACE("Please set argc");
        std::cerr << "Please set argc" << std::endl;
        exit(-1);
    }

    if (!cmdArgV) {
        SYSTEM_CRITICAL_LOG_TRACE("Please set argv");
        std::cerr << "Please set argv" << std::endl;
        exit(-1);
    }

    for (cmdMapIterator = cmdMap.begin(); cmdMapIterator != cmdMap.end(); cmdMapIterator++) {
        cmdOptionString.append(&cmdMapIterator->first);
    }

    int opt = 0;
    char charOpt;
    while ((opt = getopt(cmdArgC, cmdArgV, cmdOptionString.c_str())) != -1) {
        charOpt = static_cast<char>(opt);
        cmdMapIterator = cmdMap.find(charOpt);
        if (cmdMapIterator != cmdMap.end()) {
            cmdMapIterator->second(cmdArgC, cmdArgV, charOpt);
        } else {
            SYSTEM_CRITICAL_LOG_TRACE("args not found");
            std::cerr << "args" << charOpt - '0' << " not found" << std::endl;
            exit(-1);
        }
    }
    return true;
}
}
}