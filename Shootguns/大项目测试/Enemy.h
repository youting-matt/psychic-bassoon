// Enemy.h
#pragma once

#include "common.h"
#include <graphics.h>
#include "Player.h"

class Enemy
{
public:
    Enemy();
    ~Enemy();

    // 敌人移动
    void Move(Player* player);

    // 绘制敌人
    void Draw();

    // 检测敌人与子弹碰撞
    bool CheckBulletCollision(Bullet& bullet);

    // 检测敌人与玩家碰撞
    bool CheckPlayerCollision(Player& player);

    // 敌人受击处理
    void Hurt();

    // 判断敌人是否存活
    bool CheckAlive();

    POINT &GetPosition();
private:
    POINT enemyPosition;        // 敌人位置
    const int ZOMBIE_SPEED = 2; // 移动速度
    const int ENEMY_IMG_WIDTH = 166;
    const int ENEMY_IMG_HEIGHT = 144;
    IMAGE enemyImages[4];       // 存储 Enemy1-4 四张图片
    int currentImgIndex;        // 当前使用哪一张图
    bool alive = true;
};