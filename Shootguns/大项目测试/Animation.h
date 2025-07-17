// Animation.h
#pragma once

#include <string>
#include <vector>
#include <graphics.h>  // 必须包含这个头文件来支持 IMAGE 和图形函数
#include"common.h"

class Animation
{
public:
    // 构造函数和析构函数声明
    Animation(const std::wstring& imagesPath, int imagesNum, int interval);
    ~Animation();

    void Play(int x, int y, int delta);

private:
    std::vector<IMAGE*> imageList;
    int interval_ms;
    int timer;
    int imageIndex;
};