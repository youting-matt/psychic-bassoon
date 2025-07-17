// Bullet.h
#pragma once

#include "common.h"
#include <graphics.h>
#include <windows.h>

class Bullet
{
public:
    // 构造函数和析构函数声明
    Bullet(POINT startPos, POINT targetPos);
    ~Bullet() = default;

    // 成员函数声明
    void Draw();
    bool Move();
    void SetDead();
    bool IsDead() const;

    // 成员变量（公开是为了方便访问，也可以设为私有并提供 getter）
    POINT position = { 0, 0 };
    IMAGE img;  // 存储子弹图片
    double dx = 0; // x方向速度分量
    double dy = 0; // y方向速度分量
    const double SPEED = 10.0; // 子弹速度

private:
    bool isDead_ = false; // 销毁标记
};