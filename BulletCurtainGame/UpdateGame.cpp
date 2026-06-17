#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//按键库
#include <conio.h>
//图形界面库
#include <easyx.h>
#include <graphics.h>
//Windows API
#include <Windows.h>
#pragma comment(lib,"Winmm.lib")//链接多媒体库
#include "BulletCurtainGame.h"

void pastePictures();
void updateGame();
void selectCharacter();  // 选人函数声明

// 声明外部变量
extern PLANE player;
extern IMAGE img[10];
extern ExMessage msg;
extern int score;
extern bool gameStarted;
extern int selectedCharacter;
extern time_t startTime;


// ========== 选人函数 ==========
void selectCharacter()
{
    // 使用外部传入的 msg
    switch (msg.vkcode) {
    case VK_LEFT:
    case 'A':
        selectedCharacter = 0;  // 选博丽灵梦
        break;
    case VK_RIGHT:
    case 'D':
        selectedCharacter = 1;  // 选魔雨雾理沙
        break;
    case VK_RETURN:   // 回车
    case VK_SPACE:    // 空格
        gameStarted = true;     // 开始游戏
        break;
    }
}


// ========== updateGame 主函数 ==========
void updateGame()
{
    // 1. 处理键盘输入 
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        gameRunning = false;   // 退出主循环
        return;
    }

    static ULONGLONG lastBulletTime = 0;

    while (peekmessage(&msg, EX_KEY)) {
        if (msg.message == WM_KEYDOWN && !gameStarted) {
            selectCharacter();  // 选人功能
        }
    }

    // 2. 更新逻辑（仅游戏阶段）
    if (gameStarted) {
        player.planeState.vx = 0;
        player.planeState.vy = 0;

        if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
            player.planeState.vx = -PLAYER_SPEED;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) {
            player.planeState.vx = PLAYER_SPEED;
        }
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
            player.planeState.vy = -PLAYER_SPEED;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
            player.planeState.vy = PLAYER_SPEED;
        }

        player.planeState.x += player.planeState.vx;
        player.planeState.y += player.planeState.vy;

        // 边界限制
        int halfSize = PLANE_SIZE / 2;
        if (player.planeState.x < halfSize) player.planeState.x = halfSize;
        if (player.planeState.x > SCREEN_WIDTH - halfSize) player.planeState.x = SCREEN_WIDTH - halfSize;
        if (player.planeState.y < halfSize) player.planeState.y = halfSize;
        if (player.planeState.y > SCREEN_HEIGHT - halfSize) player.planeState.y = SCREEN_HEIGHT - halfSize;

        // ===== 改动2：子弹发射移到外面（用 GetAsyncKeyState 实时检测）=====
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            ULONGLONG now = GetTickCount();
            // BULLET_GEN_INTERVAL_MS 在头文件中定义为毫秒
            if (player.bulletExistedCount < BULLET_NUM &&
                (now - lastBulletTime > BULLET_GEN_INTERVAL_MS || lastBulletTime == 0)) {
                lastBulletTime = now;
                player.planeBullet[player.bulletExistedCount].x = player.planeState.x;
                player.planeBullet[player.bulletExistedCount].y = player.planeState.y - PLANE_SIZE / 2;
                player.planeBullet[player.bulletExistedCount].vx = 0;
                player.planeBullet[player.bulletExistedCount].vy = 8;
                player.bulletExistedCount++;
            }
        }

        // ----- 子弹更新 -----
        for (int i = 0; i < player.bulletExistedCount; i++) {
            player.planeBullet[i].y -= player.planeBullet[i].vy;

            if (player.planeBullet[i].y < 0) {
                player.planeBullet[i] = player.planeBullet[player.bulletExistedCount - 1];
                player.bulletExistedCount--;
                i--;
            }
        }
    }

    //3. 绘制画面 
    pastePictures();
}