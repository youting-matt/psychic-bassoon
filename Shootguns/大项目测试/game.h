// game.h
#pragma once

#include "common.h"
#include "Animation.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include"Prop.h"
#include <vector>

class Game {
public:
    Game();
    ~Game();



    bool Run();

private:
    bool ShowGameOverDialog(int score, bool isWin);

    void Initialize();
    void ProcessInput();
    void Update();
    void Render();
    void Cleanup();

    std::vector<Prop*> propList;   // 所有当前存在的道具
    void CheckPropCollision();     // 检测玩家与道具的碰撞
    // 游戏资源
    IMAGE imgBG, imgWin, imgGameOver, imgCrosshair;
    Player* player;
    std::vector<Enemy*> enemyList;
    std::vector<Bullet*> bulletList;

    // 游戏状态
    int score;
    int successScore;
    int mouseX;
    int mouseY;

    // 辅助函数
    void GenerateEnemy();
    void UpdateBullets();
    void DrawScore();
    void DrawHealthBar();
    void DrawPauseScreen();

    bool winMusicPlayed;   // 标记胜利音乐是否已播放
    bool loseMusicPlayed;  // 标记失败音乐是否已播放
};