// menu.cpp
#include "Menu.h"
#include <graphics.h>
#include <windows.h>
#include <conio.h>
#include <stdio.h>

extern const int WIN_WIDTH;
extern const int WIN_HEIGHT;

Menu::Menu() : selectedOption(0) {
    // 初始化选项文本
    _tcscpy_s(optionTexts[START_GAME], _T("开始游戏"));
    _tcscpy_s(optionTexts[SETTINGS], _T("设置"));
    _tcscpy_s(optionTexts[EXIT_GAME], _T("退出游戏"));
}

Menu::~Menu() {
    // 释放资源
}

void Menu::Initialize() {
    // 加载菜单资源
    loadimage(&backgroundImage, _T("image\\background.jpg")); // 使用游戏背景

    // 计算选项位置
    int centerX = WIN_WIDTH / 2;
    int startY = WIN_HEIGHT * 2 / 3;
    int spacing = 60;

    for (int i = 0; i < OPTION_COUNT; i++) {
        optionPositions[i].x = centerX;
        optionPositions[i].y = startY + i * spacing;
    }
}

void Menu::Draw() {
    cleardevice();

    // 绘制背景（使用纯色覆盖实现半透明效果）
    putimage(0, 0, &backgroundImage);

    // 创建半透明覆盖层 - 使用纯黑色覆盖层
    setfillcolor(BLACK);
    solidrectangle(0, 0, WIN_WIDTH, WIN_HEIGHT);

    // 绘制背景图片
    IMAGE title;
    loadimage(&title, _T("image\\background.jpg"), 0, 0, true); // 透明背景加载
    putimage_alpha(0, 0, &title);

    // 绘制菜单选项
    setbkmode(TRANSPARENT);

    // 创建更美观的字体
    LOGFONT f;

    // ===== 新增：绘制标题 "Shootguns" =====
    gettextstyle(&f);
    f.lfHeight = 72; // 更大的字号
    f.lfWeight = FW_HEAVY; // 更粗的字体
    _tcscpy_s(f.lfFaceName, _T("Arial Black")); // 更粗的字体
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);

    const TCHAR* titleText = _T("SHOOTGUNS");
    int titleWidth = textwidth(titleText);
    int titleHeight = textheight(titleText);
    int titleX = (WIN_WIDTH - titleWidth) / 2;
    int titleY = 100; // 顶部位置

    // 绘制标题阴影（深红色）
    settextcolor(RGB(100, 0, 0));
    outtextxy(titleX + 4, titleY + 4, titleText);

    // 绘制主标题（金色渐变）
    settextcolor(RGB(255, 215, 0)); // 金色
    outtextxy(titleX, titleY, titleText);
    // ===== 标题绘制结束 =====

    gettextstyle(&f);
    f.lfHeight = 46; // 增大字号
    f.lfWeight = FW_BOLD; // 加粗
    _tcscpy_s(f.lfFaceName, _T("Microsoft YaHei UI")); // 使用更现代的字体
    f.lfQuality = ANTIALIASED_QUALITY; // 抗锯齿渲染
    settextstyle(&f);

    for (int i = 0; i < OPTION_COUNT; i++) {
        int textWidth = textwidth(optionTexts[i]);
        int textHeight = textheight(optionTexts[i]);

        // 设置文本颜色 - 选中的选项使用更醒目的颜色
        settextcolor(i == selectedOption ? RGB(255, 223, 0) : RGB(240, 240, 240));

        // 添加文本阴影效果（使用两次绘制）
        // 先绘制深色阴影（偏移位置）
        settextcolor(RGB(40, 40, 40)); // 深灰色阴影
        outtextxy(optionPositions[i].x - textWidth / 2 + 2,
            optionPositions[i].y - textHeight / 2 + 2,
            optionTexts[i]);

        // 再绘制实际文本（原始位置）
        settextcolor(i == selectedOption ? RGB(255, 223, 0) : RGB(240, 240, 240));
        outtextxy(optionPositions[i].x - textWidth / 2,
            optionPositions[i].y - textHeight / 2,
            optionTexts[i]);

        // 选中的选项下方显示横线
        if (i == selectedOption) {
            // 设置横线属性
            setlinecolor(RGB(255, 223, 0)); // 金色横线
            setlinestyle(PS_SOLID, 4);       // 4像素宽的实线

            // 计算横线位置（文本下方居中）
            int lineY = optionPositions[i].y + textHeight / 2 + 8;
            int lineStartX = optionPositions[i].x - textWidth / 2 - 10;
            int lineEndX = optionPositions[i].x + textWidth / 2 + 10;

            // 绘制横线
            line(lineStartX, lineY, lineEndX, lineY);

            // 添加横线两端装饰
            setfillcolor(RGB(255, 223, 0));
            solidcircle(lineStartX, lineY, 4);
            solidcircle(lineEndX, lineY, 4);
        }
    }

    // 绘制提示信息 - 使用更小的字号和更柔和的颜色
    settextstyle(24, 0, _T("Microsoft YaHei UI"));
    settextcolor(RGB(180, 180, 180)); // 浅灰色
    outtextxy(WIN_WIDTH / 2 - textwidth(_T("使用方向键选择，回车确认")) / 2,
        WIN_HEIGHT - 50,
        _T("使用方向键选择，回车确认"));

    FlushBatchDraw();
}

Menu::MenuOption Menu::ProcessInput() {
    ExMessage msg;
    if (peekmessage(&msg, EX_KEY)) {
        if (msg.message == WM_KEYDOWN) {
            switch (msg.vkcode) {
            case VK_UP:
                selectedOption = (selectedOption - 1 + OPTION_COUNT) % OPTION_COUNT;
                break;
            case VK_DOWN:
                selectedOption = (selectedOption + 1) % OPTION_COUNT;
                break;
            case VK_RETURN:
                return static_cast<MenuOption>(selectedOption);
            }
        }
    }
    return OPTION_COUNT; // 表示没有选择
}

void Menu::SetSelectedOption(int option) {
    if (option >= 0 && option < OPTION_COUNT) {
        selectedOption = option;
    }
}

int Menu::GetSelectedOption() const {
    return selectedOption;
}