// Player.h
#pragma once

#include "common.h"
#include "Animation.h"
#include <graphics.h>

class Player
{
public:
    Player();
    ~Player();

    // 获取当前血量
    int GetHealth() const;

    // 减少血量
    void ReduceHealth(int amount);

    // 玩家消息处理
    void ProcessEvent(const ExMessage& msg);

    // 玩家移动
    void Move();

    // 绘制玩家
    void Draw();

    void AddHealth(int amount);

    POINT& GetPosition();

    int GetPeaImgWidth();
    int GetPeaImgHeight();

    // 添加射击方法
    bool CanShoot();

    POINT GetTargetPosition();

    void ResetShoot();

private:
    POINT charaPositon;             // 角色初始位置
    const int CHARA_SPEED = 5;      // 角色移动速度
    const int CHARA_IMG_WIDTH = 75; // 角色图片宽度
    const int CHARA_IMG_HEIGHT = 75;// 角色图片高度
    int health;

    IMAGE imgPlayer;
    bool isMoveUp;                  // 是否向上、下、左、右四个方向移动
    bool isMoveDown;
    bool isMoveLeft;
    bool isMoveRight;
    bool isShooting;                // 是否正在射击
    POINT targetPosition;           // 射击目标位置
};