// Enemy.cpp
#include "Enemy.h"
#include"Bullet.h"
#include <graphics.h>
#include <cmath> 

Enemy::Enemy()
{
        // 加载四张敌人图片（注意路径要正确）
        loadimage(&enemyImages[0], _T("image\\enemy1.png"));
        loadimage(&enemyImages[1], _T("image\\enemy2.png"));
        loadimage(&enemyImages[2], _T("image\\enemy3.png"));
        loadimage(&enemyImages[3], _T("image\\enemy4.png"));

        // 随机选择一张作为当前敌人使用的图片索引
        currentImgIndex = rand() % 4;

    // 敌人出现的边界
    enum class Edge
    {
        Up = 0,
        Down,
        Left,
        Right
    };

    // 随机生成敌人出现时的边界，并设置其坐标
    Edge edge = (Edge)(rand() % 4);
    switch (edge)
    {
    case Edge::Up:
        enemyPosition.x = rand() % WIN_WIDTH;
        enemyPosition.y = -ENEMY_IMG_HEIGHT;
        break;
    case Edge::Down:
        enemyPosition.x = rand() % WIN_WIDTH;
        enemyPosition.y = WIN_HEIGHT;
        break;
    case Edge::Left:
        enemyPosition.x = -ENEMY_IMG_WIDTH;
        enemyPosition.y = rand() % WIN_HEIGHT;
        break;
    case Edge::Right:
        enemyPosition.x = WIN_WIDTH;
        enemyPosition.y = rand() % WIN_HEIGHT;
        break;
    default:
        break;
    }
}

Enemy::~Enemy()
{
}

POINT& Enemy::GetPosition()
{
    return enemyPosition;
}

void Enemy::Move(Player* player)
{
    // 获取玩家位置
    POINT& playerPosition = player->GetPosition();

    // 计算不同方向(包括同时)按下时的速度增量
    int directX = playerPosition.x - enemyPosition.x;
    int directY = playerPosition.y - enemyPosition.y;
    double directXY = sqrt(directX * directX + directY * directY);
    if (directXY != 0)
    {
        double factorX = directX / directXY;
        double factorY = directY / directXY;
        enemyPosition.x += (int)ZOMBIE_SPEED * factorX; // 增速 * X方向的标准化分量
        enemyPosition.y += (int)ZOMBIE_SPEED * factorY; // 增速 * Y方向的标准化分量
    }
}

void Enemy::Draw()
{
    // 直接绘制当前选择的敌人图片
    putimage_alpha(enemyPosition.x, enemyPosition.y, &enemyImages[currentImgIndex]);
}

bool Enemy::CheckBulletCollision(Bullet& bullet)
{
    // 将子弹看做一个点，判断其是否落在敌人图片所在矩形区域
    if (bullet.position.x > enemyPosition.x && bullet.position.x < enemyPosition.x + ENEMY_IMG_WIDTH &&
        bullet.position.y > enemyPosition.y && bullet.position.y < enemyPosition.y + ENEMY_IMG_HEIGHT)
        return true;
    return false;
}

bool Enemy::CheckPlayerCollision(Player& player)
{
    // 获取敌人矩形
    RECT rectEnemy = {
        enemyPosition.x,
        enemyPosition.y,
        enemyPosition.x + ENEMY_IMG_WIDTH,
        enemyPosition.y + ENEMY_IMG_HEIGHT
    };

    // 获取玩家矩形
    POINT playerPos = player.GetPosition();
    RECT rectPlayer = {
        playerPos.x,
        playerPos.y,
        playerPos.x + player.GetPeaImgWidth(),
        playerPos.y + player.GetPeaImgHeight()
    };

    // 检测矩形是否相交
    RECT rectTemp;
    bool isCollided = IntersectRect(&rectTemp, &rectEnemy, &rectPlayer);

    if (isCollided) {
        // 碰撞后减少玩家血量
        player.ReduceHealth(10);
    }

    return isCollided;
}

void Enemy::Hurt()
{
    alive = false;
}

bool Enemy::CheckAlive()
{
    return alive;
}