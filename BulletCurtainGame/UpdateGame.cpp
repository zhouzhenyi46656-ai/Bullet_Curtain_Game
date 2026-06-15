//标准库
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

// 声明外部变量
extern PLANE player;
extern IMAGE img[10];
extern ExMessage msg;
extern int score;
extern bool gameStarted;
extern int selectedCharacter;
extern time_t startTime;


void updateGame()
{
    // 1. 处理键盘输入 
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        gameRunning = false;   // 退出主循环
        return;
    }
    static ULONGLONG lastBulletTime = 0;
    while (peekmessage(&msg, EX_KEY)) {  // peekmessage不阻塞，能处理多条消息
        if (msg.message == WM_KEYDOWN) {
            // ----- 选人阶段 -----
            if (!gameStarted) {
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
            // ----- 游戏阶段 -----
            else {
                if (msg.vkcode == VK_SPACE) {
                    ULONGLONG now = GetTickCount();
                    clock_t currentTime = clock();
                    if (player.bulletExistedCount < BULLET_NUM && (now - lastBulletTime) * 1000 / CLOCKS_PER_SEC > BULLET_GEN_INTERVAL || lastBulletTime == 0) {
                        lastBulletTime = now;
                        player.planeBullet[player.bulletExistedCount].x = player.planeState.x;
                        player.planeBullet[player.bulletExistedCount].y = player.planeState.y - PLANE_SIZE / 2;
                        player.planeBullet[player.bulletExistedCount].vx = 0;
                        player.planeBullet[player.bulletExistedCount].vy = 8;
                        player.bulletExistedCount++;
                    }
                }
            }
        }
    }

    // 2. 更新逻辑（仅游戏阶段）
    if (gameStarted) {
        // ----- 玩家移动 -----
        int moveX = 0, moveY = 0;

        if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) moveX = -PLAYER_SPEED;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) moveX = PLAYER_SPEED;
        if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) moveY = -PLAYER_SPEED;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) moveY = PLAYER_SPEED;

        player.planeState.x += moveX;
        player.planeState.y += moveY;

        // 边界限制
        int halfSize = PLANE_SIZE / 2;
        if (player.planeState.x < halfSize) player.planeState.x = halfSize;
        if (player.planeState.x > SCREEN_WIDTH - halfSize) player.planeState.x = SCREEN_WIDTH - halfSize;
        if (player.planeState.y < halfSize) player.planeState.y = halfSize;
        if (player.planeState.y > SCREEN_HEIGHT - halfSize) player.planeState.y = SCREEN_HEIGHT - halfSize;

        // ----- 子弹更新 -----
        for (int i = 0; i < player.bulletExistedCount; i++) {
            player.planeBullet[i].y -= player.planeBullet[i].vy;  // 向上飞

            if (player.planeBullet[i].y < 0) {  // 超出屏幕
                player.planeBullet[i] = player.planeBullet[player.bulletExistedCount - 1];
                player.bulletExistedCount--;
                i--;
            }
        }
        
    }

    //3. 绘制画面 
    pastePictures();
}