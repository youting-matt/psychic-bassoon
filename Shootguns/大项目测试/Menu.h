// menu.h
#pragma once
#include "common.h"

class Menu {
public:
    enum MenuOption {
        START_GAME = 0,
        SETTINGS,
        EXIT_GAME,
        OPTION_COUNT
    };

    Menu();
    ~Menu();

    void Initialize();
    void Draw();
    MenuOption ProcessInput();
    void SetSelectedOption(int option);
    int GetSelectedOption() const;

private:
    int selectedOption;
    IMAGE titleImage;
    IMAGE backgroundImage;
    POINT optionPositions[OPTION_COUNT];
    TCHAR optionTexts[OPTION_COUNT][20];
};