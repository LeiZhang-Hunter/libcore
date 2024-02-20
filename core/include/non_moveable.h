#pragma once

namespace Core {

//不允许移动类
class Nonmoveable {
public:
public:

    Nonmoveable(Nonmoveable&) = delete;

protected:
    Nonmoveable() = default;
    ~Nonmoveable() = default;
};
}
