// game.cpp
#include "game.h"
#include <tchar.h>
#include <windows.h>
#include <graphics.h>
#include <stdio.h>
#include <string>
// 使用全局常量
extern const int WIN_WIDTH;
extern const int WIN_HEIGHT;
extern const int GAME_FRAME;
extern bool clearEnemy;
extern bool paused;

Game::Game()
    : player(nullptr), score(0), successScore(100),
    mouseX(WIN_WIDTH / 2), mouseY(WIN_HEIGHT / 2), 
    winMusicPlayed(false), loseMusicPlayed(false){
}

Game::~Game() {
    Cleanup();
}

void Game::Initialize() {
    loadimage(&imgBG, _T("image\\background-1.png"));
    loadimage(&imgWin, _T("image\\win.png"));
    loadimage(&imgGameOver, _T("image\\fail.jpg"));
    loadimage(&imgCrosshair, _T("image\\crosshair.png"));

    // 初始化所有数据
    score = 0;
    successScore = 100;
    mouseX = 0;
    mouseY = 0;
    winMusicPlayed = false;
    loseMusicPlayed = false;

    player = new Player();
}

bool Game::ShowGameOverDialog(int score, bool isWin) {
    TCHAR message[256];
    _stprintf_s(message, _T("你的得分是：%d\n是否重新开始？"), score);

    int result = MessageBox(GetHWnd(), message, isWin ? _T("恭喜通关！") : _T("游戏失败！"), MB_YESNO | MB_ICONQUESTION);
    return (result == IDYES); // 返回是否重玩
}

bool Game::Run() {
    Initialize();

    PlayBGM();

    while (true) {
        ULONGLONG beginTime = GetTickCount();

        ProcessInput();

        // 统一的暂停处理
        if (paused) {
            // 进入暂停循环
            while (paused) {
                // 只处理按键消息
                ExMessage msg;
                while (peekmessage(&msg, EX_KEY)) {
                    if (msg.message == WM_KEYDOWN) {
                        if (msg.vkcode == VK_ESCAPE || msg.vkcode == 'P') {
                            paused = false; // 按ESC或P退出暂停
                        }
                        else if (msg.vkcode == 'M') {
                            return true; // 返回主菜单
                        }
                    }
                }

                // 渲染暂停界面
                Render();

                // 避免CPU占用过高
                Sleep(100);
            }
            continue; // 退出暂停后继续游戏循环
        }

        Update();
        Render();

        // 帧率控制
        ULONGLONG endTime = GetTickCount();
        ULONGLONG elapsedTime = endTime - beginTime;
        if (elapsedTime < 1000 / GAME_FRAME) {
            Sleep(1000 / GAME_FRAME - elapsedTime);
        }
    }
}

void Game::ProcessInput() {
    ExMessage msg;
    while (peekmessage(&msg)) {
        player->ProcessEvent(msg);

        if (msg.message == WM_MOUSEMOVE) {
            mouseX = msg.x;
            mouseY = msg.y;
        }

        // 添加P键暂停功能
        if (msg.message == WM_KEYDOWN && msg.vkcode == 'P') {
            paused = !paused;
        }

        if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
            paused = !paused;
        }
    }
}

void Game::Update() {
    // 玩家移动
    player->Move();

    // 玩家射击
    if (player->CanShoot()) {
        POINT playerCenter = {
            player->GetPosition().x + player->GetPeaImgWidth() / 2,
            player->GetPosition().y + player->GetPeaImgHeight() / 2
        };
        bulletList.push_back(new Bullet(playerCenter, player->GetTargetPosition()));
        player->ResetShoot();
    }

    for (auto it = enemyList.begin(); it != enemyList.end();) {
        Enemy* enemy = *it;

        // 获取敌人位置
        POINT enemyPos = enemy->GetPosition();



        // 判断敌人是否死亡
        if (!enemy->CheckAlive()) {
            // 在敌人死亡位置生成道具
            propList.push_back(new Prop(enemyPos.x, enemyPos.y, 30, 30, 1));
            _tprintf(_T("生成了一个道具，位置: (%d, %d)\n"), enemyPos.x, enemyPos.y);
        

            // 删除敌人
            delete enemy;
            it = enemyList.erase(it);
        }
        else {
            ++it;
        }
    }

    // 更新子弹位置
    for (int i = 0; i < bulletList.size();) {
        Bullet* bullet = bulletList[i];
        if (bullet->Move() || bullet->IsDead()) {
            delete bullet;
            bulletList[i] = bulletList.back();
            bulletList.pop_back();
        }
        else {
            i++;
        }
    }

    // 生成敌人
    GenerateEnemy();

    // 敌人移动
    for (Enemy* enemy : enemyList)
        enemy->Move(player);

    // 检测敌人与玩家碰撞
    for (int i = 0; i < enemyList.size();) {
        Enemy* enemy = enemyList[i];
        if (enemy->CheckPlayerCollision(*player)) {
            delete enemy;
            enemyList[i] = enemyList.back();
            enemyList.pop_back();

            player->ReduceHealth(10);

            // 检查玩家是否死亡
            if (player->GetHealth() <= 0) {
                paused = true;
                break;
            }
            continue;
        }
        i++;
    }

    // 检测子弹与敌人碰撞
    for (int i = 0; i < bulletList.size(); i++) {
        Bullet* bullet = bulletList[i];
        bool bulletHit = false;

        for (int j = 0; j < enemyList.size(); j++) {
            if (enemyList[j]->CheckBulletCollision(*bullet)) {
                enemyList[j]->Hurt();
                PlayHit();
                score++;
                bullet->SetDead();
                bulletHit = true;
                break;
            }
        }

        if (bulletHit) {
            delete bullet;
            bulletList[i] = bulletList.back();
            bulletList.pop_back();
            i--;
        }
    }

    // 清理死亡的敌人
    for (int i = 0; i < enemyList.size();) {
        Enemy* enemy = enemyList[i];
        if (!enemyList[i]->CheckAlive()) {
            std::swap(enemyList[i], enemyList.back());
            enemyList.pop_back();
            delete enemy;
        }
        else {
            i++;
        }
    }

    // 一键清敌
    if (clearEnemy) {
        for (Enemy* enemy : enemyList) delete enemy;
        enemyList.clear();
        clearEnemy = false;
    }

    // 检查胜利条件
    if (score >= successScore) {
        paused = true;
    }
}

void Game::Render() {
    cleardevice();

    // 绘制背景
    if (player->GetHealth() <= 0) {
        putimage(0, 0, &imgGameOver);
    }
    else if (score >= successScore) {
        putimage(0, 0, &imgWin);
    }
    else {
        putimage(0, 0, &imgBG);
    }

    // 绘制道具
    setfillcolor(YELLOW);  // 黄色代表回血道具
    for (auto prop : propList) {
        // 绘制道具（使用填充矩形表示）
        solidrectangle(prop->x, prop->y,
            prop->x + prop->width,
            prop->y + prop->height);
    }
    // 绘制准心
    putimage_alpha(mouseX - imgCrosshair.getwidth() / 2,
        mouseY - imgCrosshair.getheight() / 2,
        &imgCrosshair);

    // 绘制游戏对象
    if (player->GetHealth() > 0 && score < successScore) {
        player->Draw();
        for (Enemy* enemy : enemyList) enemy->Draw();
        for (Bullet* bullet : bulletList) bullet->Draw();
    }

    // 绘制UI
    DrawScore();
    DrawHealthBar();
    DrawPauseScreen();

    // 添加暂停提示 - 只在游戏进行中显示
    if (player->GetHealth() > 0 && score < successScore && !paused) {
        settextcolor(RGB(0, 0, 0)); // 浅灰色
        settextstyle(16, 0, _T("宋体"));
        setbkmode(TRANSPARENT);

        const TCHAR* pauseHint = _T("按 [P] 暂停游戏");
        int textWidth = textwidth(pauseHint);

        // 在右上角显示提示，距离边缘10像素
        outtextxy(WIN_WIDTH - textWidth - 10, 10, pauseHint);
    }

    FlushBatchDraw();
}

void Game::Cleanup() {
    // 删除动态分配的对象
    delete player;
    for (auto enemy : enemyList) delete enemy;
    for (auto bullet : bulletList) delete bullet;
    enemyList.clear();
    bulletList.clear();

    // 停止所有音效
    mciSendString(_T("stop bgm"), NULL, 0, NULL);
    mciSendString(_T("stop win_music"), NULL, 0, NULL);
    mciSendString(_T("stop lose_music"), NULL, 0, NULL);

    // 重置播放状态
    winMusicPlayed = false;
    loseMusicPlayed = false;
}

void Game::GenerateEnemy() {
    // 基本间隔：50帧（初始速度）
    const int BASE_INTERVAL = 50;
    // 最小间隔：30帧（最快速度）
    const int MIN_INTERVAL = 10;

    // 每得5分减少5帧生成间隔，但不会低于最小间隔
    int dynamicInterval = max(MIN_INTERVAL, BASE_INTERVAL - (score / 5) * 5);

    static int counter = 0;                  //计数器
    if ((++counter) % dynamicInterval == 0)
        enemyList.push_back(new Enemy());    //创建并添加敌人到列表
}

void Game::UpdateBullets() {
    const double RADIAL_SPEED = 0.001;
    const double TANGENT_SPEED = 0.003;
    double radianInterval = 2 * 3.1415926 / bulletList.size();
    double radius = 150 + 50 * sin(GetTickCount() * RADIAL_SPEED);
    POINT playerPosition = player->GetPosition();

    for (int i = 0; i < bulletList.size(); i++) {
        double radian = GetTickCount() * TANGENT_SPEED + radianInterval * i;
        bulletList[i]->position.x = playerPosition.x + player->GetPeaImgWidth() / 2 + (int)(radius * cos(radian));
        bulletList[i]->position.y = playerPosition.y + player->GetPeaImgHeight() / 2 + (int)(radius * sin(radian));
    }
}

void Game::DrawScore() {
    TCHAR txtScore[64];
    _stprintf_s(txtScore, _T("当前得分：%d"), score);
    setbkmode(TRANSPARENT);
    settextcolor(RGB(0, 0, 255));
    settextstyle(40, 0, _T("微软雅黑"));
    outtextxy(WIN_WIDTH / 2 - textwidth(txtScore) / 2, 10, txtScore);
}

void Game::DrawHealthBar() {
    if (player->GetHealth() > 0 && score < successScore) {
        int barWidth = 300;
        int barHeight = 20;
        int barX = (WIN_WIDTH - barWidth) / 2;
        int barY = WIN_HEIGHT / 6;
        int currentHealth = player->GetHealth();

        setfillcolor(LIGHTGRAY);
        solidrectangle(barX, barY, barX + barWidth, barY + barHeight);

        float healthRatio = (float)currentHealth / 100;
        int currentBarWidth = (int)(barWidth * healthRatio);
        setfillcolor(RED);
        solidrectangle(barX, barY, barX + currentBarWidth, barY + barHeight);

        setlinecolor(BLACK);
        rectangle(barX, barY, barX + barWidth, barY + barHeight);
    }
}

void Game::CheckPropCollision() {
    POINT playerPos = player->GetPosition();
    RECT rectPlayer = {
        playerPos.x,
        playerPos.y,
        playerPos.x + player->GetPeaImgWidth(),
        playerPos.y + player->GetPeaImgHeight()
    };

    for (auto it = propList.begin(); it != propList.end();) {
        Prop* prop = *it;

        RECT rectProp = {
            prop->x,
            prop->y,
            prop->x + prop->width,
            prop->y + prop->height
        };

        RECT intersectRect;
        if (IntersectRect(&intersectRect, &rectPlayer, &rectProp)) {
            if (prop->type == 1) {
                player->AddHealth(20); // 回复20点生命值
            }

            delete prop;
            it = propList.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Game::DrawPauseScreen() {
    if (paused) {
        if (player->GetHealth() <= 0) {
            // 确保只播放一次失败音乐
            if (!loseMusicPlayed) {
                mciSendString(_T("stop bgm"), NULL, 0, NULL); // 停止背景音乐
                mciSendString(_T("play lose_music from 0"), NULL, 0, NULL); // 从头播放
                loseMusicPlayed = true;
            }

            settextcolor(LIGHTRED);
            settextstyle(36, 0, _T("宋体"));
            outtextxy(WIN_WIDTH / 2 - 80, WIN_HEIGHT / 2 - 30, _T("游戏失败"));

            FlushBatchDraw();

            if (ShowGameOverDialog(score, false)) {
                // 重新开始：清理并初始化
                Cleanup();
                Initialize();
                PlayBGM(); // 重新开始后播放BGM
            }
            else {
                exit(0);
            }
        }
        else if (score >= successScore) {
            // 确保只播放一次胜利音乐
            if (!winMusicPlayed) {
                mciSendString(_T("stop bgm"), NULL, 0, NULL); // 停止背景音乐
                mciSendString(_T("play win_music from 0"), NULL, 0, NULL); // 从头播放
                winMusicPlayed = true;
            }

            settextcolor(LIGHTGREEN);
            settextstyle(36, 0, _T("宋体"));
            outtextxy(WIN_WIDTH / 2 - 80, WIN_HEIGHT / 2 - 30, _T("游戏胜利"));

            FlushBatchDraw();

            if (ShowGameOverDialog(score, true)) {
                Cleanup();
                Initialize();
                PlayBGM(); // 重新开始后播放BGM
            }
            else {
                exit(0);
            }
        }
        else {
            settextcolor(LIGHTBLUE);
            settextstyle(36, 0, _T("宋体"));
            outtextxy(WIN_WIDTH / 2 - 80, WIN_HEIGHT / 2 - 30, _T("游戏暂停"));
            settextstyle(16, 0, _T("宋体"));
        }
    }
}