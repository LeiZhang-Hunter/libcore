#pragma once

extern "C" {
#include <sys/types.h>
#include <sys/wait.h>
}


#include "os/unix_sigset.h"
#include "component/api.h"
#include "component/discovery/component.h"
#include "process_event.h"
#include "event/event_loop.h"
#include "process.h"

namespace Core {
namespace Component {
namespace Process {
class Manager : public Component, public Noncopyable, public Nonmoveable {
public:
    Manager()
    :loop(std::make_shared<Event::EventLoop>()), sigset(std::make_unique<OS::UnixSigSet>()) {
    }

    void init() override {

    }

    void start() override {
        sigset->remove(SIGCHLD);
        sigset->block();
        loop->sigAdd(SIGCHLD, onExit, this);
        loop->loop();
    }

    void stop() override {
        auto iter = maps.begin();
        for (; iter != maps.end(); iter++) {
            iter->second->syncStop();
        }
    }

    static void onExit(evutil_socket_t sig, short events, void *param);

    void onCreate() {

    }
    void onStart() {

    };
    void onStop() {

    }
    void onDestroy() {}

    bool addProcess(std::unique_ptr<Process> process) {
        if (!process) {
            return false;
        }
        auto iter = maps.find(process->getPid());
        if (iter != maps.end()) {
            return false;
        }
        maps[process->getPid()] = std::move(process);
        return true;
    }

    std::unordered_map<pid_t, std::unique_ptr<Process>>& all() {
        return maps;
    }

    void stopProcess(const std::string& name) {
      for (auto &[pid, process]: maps) {
        if (process->name() == name) {
          process->stop();
        }
      }
    }

    Event::EventLoop* getLoop() const {return loop.get();}

protected:
    friend class Process;
    std::shared_ptr<Event::EventLoop> loop;
    std::shared_ptr<OS::UnixSigSet> sigset;
    std::unordered_map<pid_t, std::unique_ptr<Process>> maps;
};
}
}
}
