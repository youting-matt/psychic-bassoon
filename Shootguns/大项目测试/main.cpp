#include <windows.h>
#include <graphics.h>
#include "common.h"
#include "game.h"
#include "menu.h"
#pragma comment(lib, "MSIMG32.LIB")
#pragma comment(lib, "Winmm.LIB")

// 定义全局常量
const int WIN_WIDTH = 900;
const int WIN_HEIGHT = 600;
const int GAME_FRAME = 60;
bool clearEnemy = false;
bool paused = false;

// 设置界面函数
void ShowSettings() {
    // 简单实现设置界面
    cleardevice();
    
    IMAGE bg;
    loadimage(&bg, _T("image\\background.jpg"));
    putimage(0, 0, &bg);
    
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 255, 255));
    settextstyle(40, 0, _T("微软雅黑"));
    
    outtextxy(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 3, _T("游戏设置"));
    
    settextstyle(30, 0, _T("宋体"));
    outtextxy(WIN_WIDTH / 2 - 200, WIN_HEIGHT / 2, _T("设置功能正在开发中..."));
    outtextxy(WIN_WIDTH / 2 - 150, WIN_HEIGHT * 2 / 3, _T("按任意键返回"));
    
    FlushBatchDraw();
    
    // 等待按键
    ExMessage msg;
    while (true) {
        if (peekmessage(&msg, EX_KEY)) {
            if (msg.message == WM_KEYDOWN) {
                break;
            }
        }
        Sleep(10);
    }
}

int main() {
    // 初始化游戏窗口
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    BeginBatchDraw();
    
    // 加载音频资源
    LoadAudio();
    
    // 创建菜单
    Menu menu;
    menu.Initialize();
    
    // 菜单主循环
    while (true) {
        menu.Draw();
        
        Menu::MenuOption selected = menu.ProcessInput();
        
        if (selected != Menu::OPTION_COUNT) {
            switch (selected) {
            case Menu::START_GAME: {
                Game* game = new Game();  // 动态创建 Game 对象
                game->Run();              // 运行游戏
                delete game;              // 游戏结束后释放内存
                break;
            }
                case Menu::SETTINGS:
                    ShowSettings();
                    break;
                case Menu::EXIT_GAME:
                    EndBatchDraw();
                    closegraph();
                    return 0;
            }
        }
        
        // 帧率控制
        static ULONGLONG lastTime = GetTickCount64();
        ULONGLONG currentTime = GetTickCount64();
        ULONGLONG elapsedTime = currentTime - lastTime;
        
        if (elapsedTime < 1000 / GAME_FRAME) {
            Sleep(1000 / GAME_FRAME - elapsedTime);
        }
        lastTime = currentTime;
    }
    
    // 清理资源
    EndBatchDraw();
    closegraph();
    return 0;
}