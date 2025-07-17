// Player.cpp
#include "Player.h"
#include <math.h>  
#include"game.h"

Player::Player()
    : charaPositon({ 500, 300 }), health(100), isMoveUp(false),
    isMoveDown(false), isMoveLeft(false), isMoveRight(false),
    isShooting(false), targetPosition({ 0, 0 })
{
    // 加载单张角色图片
    loadimage(&imgPlayer, _T("image\\chara.png")); // 替换为您的图片路径
}

Player::~Player()
{
}

int Player::GetHealth() const
{
    return health;
}

void Player::ReduceHealth(int amount)
{
    if (health > 0)
    {
        health -= amount;
        if (health < 0) health = 0; // 防止血量为负数
    }
}

void Player::AddHealth(int amount) {
    health += amount;
    if (health > 100) {
        health = 100;
    }
}

void Player::ProcessEvent(const ExMessage& msg)
{
    // 按下键盘处理
    if (msg.message == WM_KEYDOWN)
    {
        switch (msg.vkcode)
        {
        case VK_UP:
            isMoveUp = true;
            break;
        case VK_DOWN:
            isMoveDown = true;
            break;
        case VK_LEFT:
            isMoveLeft = true;
            break;
        case VK_RIGHT:
            isMoveRight = true;
            break;
        case VK_CONTROL:            // 绝招：一键清敌
            clearEnemy = true;
            break;
        case 'P':
        case 'p':
            paused = !paused;
            break;
        }
    }

    // 松开按键处理
    if (msg.message == WM_KEYUP)
    {
        switch (msg.vkcode)
        {
        case VK_UP:
            isMoveUp = false;
            break;
        case VK_DOWN:
            isMoveDown = false;
            break;
        case VK_LEFT:
            isMoveLeft = false;
            break;
        case VK_RIGHT:
            isMoveRight = false;
            break;
        }
    }

    // 按下鼠标处理
    if (msg.message == WM_LBUTTONDOWN)
    {
        isShooting = true;
        targetPosition.x = msg.x;
        targetPosition.y = msg.y;
    }
}

void Player::Move()
{
    // 计算不同方向(包括同时)按下时的速度增量
    int directX = isMoveRight - isMoveLeft;
    int directY = isMoveDown - isMoveUp;
    double directXY = sqrt(directX * directX + directY * directY);
    if (directXY != 0)
    {
        double factorX = directX / directXY;
        double factorY = directY / directXY;
        charaPositon.x += (int)CHARA_SPEED * factorX;       // 增速 * X方向的标准化分量
        charaPositon.y += (int)CHARA_SPEED * factorY;       // 增速 * Y方向的标准化分量
    }

    // 玩家越界处理
    if (charaPositon.x < 0)                                 // 左边界检测
        charaPositon.x = 0;
    if (charaPositon.x + CHARA_IMG_WIDTH > WIN_WIDTH)     // 右边界检测
        charaPositon.x = WIN_WIDTH - CHARA_IMG_WIDTH;
    if (charaPositon.y < 0)                                 // 上边界检测
        charaPositon.y = 0;
    if (charaPositon.y + CHARA_IMG_HEIGHT > WIN_HEIGHT)   // 下边界检测
        charaPositon.y = WIN_HEIGHT - CHARA_IMG_HEIGHT;
}

void Player::Draw()
{
    // 简单绘制单张图片
    putimage_alpha(charaPositon.x, charaPositon.y, &imgPlayer);
}

POINT& Player::GetPosition()
{
    return charaPositon;
}

int Player::GetPeaImgWidth()
{
    return CHARA_IMG_WIDTH;
}

int Player::GetPeaImgHeight()
{
    return CHARA_IMG_HEIGHT;
}

bool Player::CanShoot()
{
    return isShooting;
}

POINT Player::GetTargetPosition()
{
    return targetPosition;
}

void Player::ResetShoot()
{
    isShooting = false;
}