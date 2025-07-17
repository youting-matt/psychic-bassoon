// common.h
#pragma once

#include <graphics.h>
#include <windows.h>
#include <string>
#include <vector>

// 前向声明
class Animation;
class Bullet;
class Enemy;
class Player;

// 声明全局常量（在main.cpp中定义）
extern const int WIN_WIDTH;
extern const int WIN_HEIGHT;
extern const int GAME_FRAME;
extern bool clearEnemy;
extern bool paused;

// 公共函数声明
void putimage_alpha(int x, int y, IMAGE* img);
void LoadAudio();
void PlayBGM();
void PlayHit();