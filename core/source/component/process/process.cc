/**
******************************************************************************
* @file           : process.cc
* @author         : zhanglei
* @brief          : None
* @attention      : None
* @date           : 2024/2/11
******************************************************************************
*/
//
// Created by zhanglei on 2024/2/11.
//
#include "component/process/process.h"
#include <chrono>

#include "component/process/manager.h"
#include "os/unix_logger.h"


namespace Core {
namespace Component {
namespace Process {

Process::Process(const std::string &command, const std::shared_ptr<Event::EventLoop> &loop)
: Process(command, loop.get()){}
Process::Process(const std::string &command, Event::EventLoop* loop)
:timer(std::make_unique<TimerChannel>(loop, [this]() {
    onRestore();
    return;
})), loop_(loop) {
    command_ = command;
    std::stringstream ss(command);
    std::string token;
    while (ss >> token) {
        if (binary.empty()) {
            binary = token;
            args.push_back(token);
            continue;
        }
        args.push_back(token);
    }
}

bool Process::execute() {
    this->onCreate();
    pid_t pid = fork();
    status = RUNNING;
    if (pid == -1) {
        // fork失败
        return false;
    } else if (pid == 0) {
        std::vector<char*> argv(args.size() + 1);
        for (size_t i = 0; i < args.size(); i++) {
            std::cout << args[i].c_str() << std::endl;
            argv[i] = (char*)args[i].c_str();
        }
        argv[args.size()] = nullptr;
        // 子进程
        int ret = execvp(binary.c_str(), argv.data());
        if (ret == -1) {
            SYSTEM_ERROR_LOG_TRACE("execvp {} failed", binary);
            _exit(errno);
        }
        _exit(0);
    } else {
        // 父进程
        pid_ = pid;
        status = RUN;
        start_time_ = time(nullptr);
        if (cgroup_) {
            cgroup_->run();
            cgroup_->attach(pid);
        }
        this->onStart();
        return true;
    }
}

void Process::onExit(Manager* manager) {
    auto rCnt = restoreCount % 6;
    if (rCnt == 6) {
        restoreCount = 0;
        rCnt = 0;
    }
    restoreCount++;
    this->m = manager;
    auto timeout = (1 << rCnt);
    SYSTEM_INFO_LOG_TRACE("process {} restart,after {}s", binary, timeout);
    timer->enable(std::chrono::milliseconds(timeout * 1000));
}

void Process::onRestore() {
    if (!this->m) {
        return;
    }
    std::unique_ptr<Process> process = std::make_unique<Process>(command_, loop_);
    process->restoreCount = restoreCount;
    process->setCGroup(cgroup_);
    auto iter = this->m->maps.find(getPid());
    if (iter != this->m->maps.end()) {
        this->m->maps.erase(iter);
    }

    if (!process->execute()) {
        return;
    }
    if (this->m) {
        this->m->maps[process->getPid()] = std::move(process);
    }
}
}
}
}