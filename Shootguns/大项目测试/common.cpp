#include "common.h"

// AlphaªÏ∫œÕºœÒ
void putimage_alpha(int x, int y, IMAGE* img) {
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
        GetImageHDC(img), 0, 0, w, h,
        { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

// º”‘ÿ“Ù∆µ
void LoadAudio() {
    mciSendString(_T("open audio\\bg.mp3 alias bgm"), NULL, 0, NULL);
    mciSendString(_T("open audio\\hit.wav alias hit"), NULL, 0, NULL);
    mciSendString(_T("open audio\\win.wav alias win_music"), NULL, 0, NULL);
    mciSendString(_T("open audio\\lose.wav alias lose_music"), NULL, 0, NULL);
}

// ≤•∑≈±≥æ∞“Ù¿÷
void PlayBGM() {
    mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
}

// ≤•∑≈ª˜÷–“Ù–ß
void PlayHit() {
    mciSendString(_T("play hit from 0"), NULL, 0, NULL);
}