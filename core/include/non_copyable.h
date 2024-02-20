#pragma once

namespace Core {

//不允许构造
class Noncopyable {
public:

    Noncopyable(const Noncopyable&) = delete;
    void operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() = default;
    ~Noncopyable() = default;
};
}