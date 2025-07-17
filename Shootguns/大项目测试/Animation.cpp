// Animation.cpp
#include "Animation.h"
#include <string>
#include <vector>
#include <graphics.h>  

Animation::Animation(const std::wstring& imagesPath, int imagesNum, int interval)
    : interval_ms(interval), timer(0), imageIndex(0)
{
    for (int i = 0; i < imagesNum; ++i)
    {
        std::wstring imageFilePath = imagesPath + std::to_wstring(i + 1) + L".png";
        IMAGE* img = new IMAGE();
        loadimage(img, imageFilePath.c_str());
        imageList.push_back(img);
    }
}

Animation::~Animation()
{
    for (auto img : imageList)
    {
        delete img;
    }
}

void Animation::Play(int x, int y, int delta)
{
    timer += delta;
    if (timer > interval_ms)
    {
        imageIndex = (imageIndex + 1) % imageList.size();
        timer = 0;
    }
    putimage_alpha(x, y, imageList[imageIndex]);  // 现在不会报错了
}