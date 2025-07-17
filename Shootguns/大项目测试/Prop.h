
// Prop.h
#pragma once

struct Prop {
    int x, y;
    int width, height;
    int type; // 类型：1 = 回血道具

    Prop(int x, int y, int w, int h, int type)
        : x(x), y(y), width(w), height(h), type(type) {
    }
};