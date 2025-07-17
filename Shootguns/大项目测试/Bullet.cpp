// Bullet.cpp
#include "Bullet.h"
#include <math.h>
#include <tchar.h>

// 构造函数实现
Bullet::Bullet(POINT startPos, POINT targetPos)
{
    // 加载子弹图片
    loadimage(&img, _T("image\\bullet.png"));

    // 设置起始位置为玩家位置
    position = startPos;

    // 计算方向向量
    double deltaX = targetPos.x - startPos.x;
    double deltaY = targetPos.y - startPos.y;

    // 计算向量长度（用于标准化）
    double length = sqrt(deltaX * deltaX + deltaY * deltaY);

    if (length > 0) {
        // 标准化向量并乘以速度
        dx = (deltaX / length) * SPEED;
        dy = (deltaY / length) * SPEED;
    }
}

// 绘制子弹
void Bullet::Draw()
{
    // 计算绘制位置（使图片中心对准子弹位置）
    int drawX = position.x - img.getwidth() / 2;
    int drawY = position.y - img.getheight() / 2;

    // 绘制子弹图片
    putimage_alpha(drawX, drawY, &img);
}

// 子弹移动
bool Bullet::Move()
{
    position.x += static_cast<int>(dx);
    position.y += static_cast<int>(dy);

    // 使用全局常量（通过common.h访问）
    return position.x < 0 || position.x > WIN_WIDTH ||
        position.y < 0 || position.y > WIN_HEIGHT;
}

// 标记子弹为待销毁
void Bullet::SetDead()
{
    isDead_ = true;
}

// 检查子弹是否待销毁
bool Bullet::IsDead() const
{
    return isDead_;
}