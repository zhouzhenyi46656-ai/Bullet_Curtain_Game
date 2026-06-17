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

int areIntersecting(KINESTATE a, int aSize, KINESTATE b, int bSize);

// ========== updateGame 主函数 ==========
void updateGame()
{
    static ULONGLONG lastBulletTime = 0;
    // 处理键盘输入 
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        gameRunning = false;   // 退出主循环
        return;
    }
    

    // 一. 更新玩家状态
	// 1. 更新玩家位置
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

	// 2. 限制玩家在窗口内
    int halfSize = PLANE_SIZE / 2;
    if (player.planeState.x < halfSize) player.planeState.x = halfSize;
    if (player.planeState.x > SCREEN_WIDTH - halfSize) player.planeState.x = SCREEN_WIDTH - halfSize;
    if (player.planeState.y < halfSize) player.planeState.y = halfSize;
    if (player.planeState.y > SCREEN_HEIGHT - halfSize) player.planeState.y = SCREEN_HEIGHT - halfSize;

	// 3. 生成子弹
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        ULONGLONG now = GetTickCount();
        if (player.bulletExistedCount < BULLET_NUM &&
            (now - lastBulletTime > BULLET_GEN_INTERVAL || lastBulletTime == 0)) {
            lastBulletTime = now;
            player.planeBullet[player.bulletExistedCount].x = player.planeState.x;
            player.planeBullet[player.bulletExistedCount].y = player.planeState.y - PLANE_SIZE / 2;
            player.planeBullet[player.bulletExistedCount].vx = 0;
            player.planeBullet[player.bulletExistedCount].vy = BULLET_SPEED;
            player.bulletExistedCount++;
        }
    }


	// 二. 更新敌人状态



    // 三. 更新boss状态
	if (score >= 0 && boss.planeState.y < BOSS_SIZE * 2) {
		bossKineticModule_0();
	}
    else if (score >= 100 && score <= 200) {
		bossKineticModule_1();
    }
    else if (score > 200) {
		bossKineticModule_2();
    }

	// 四. 检测碰撞和越界
    //检查玩家子弹越界
    for (int i = 0; i < player.bulletExistedCount; i++) {
        player.planeBullet[i].y -= player.planeBullet[i].vy;
        if (player.planeBullet[i].y < 0) {
            player.planeBullet[i] = player.planeBullet[player.bulletExistedCount - 1];
            player.bulletExistedCount--;
            i--;
        }
    }
	//检查敌人子弹越界
    //检查敌人越界
	//检查玩家与敌人碰撞
	//检查玩家与boss碰撞
    
	//检查玩家子弹与敌人碰撞
	//检查玩家子弹与boss碰撞
    for (int i = 0;i < player.bulletExistedCount;i++) {
        if (areIntersecting(player.planeBullet[i], JUDGE_SCOPE, boss.planeState, JUDGE_SCOPE * 3)) {
            score++;
            player.planeBullet[i] = player.planeBullet[player.bulletExistedCount - 1];
            player.bulletExistedCount--;
            i--;
        }
    }
	//检查敌人子弹与玩家碰撞
	//检查boss子弹与玩家碰撞
	

    
    // 五. 绘制画面 
    pastePictures();
}

int areIntersecting(KINESTATE a, int aSize, KINESTATE b, int bSize)
{
    return (a.x - b.x) * (a.x - b.x) + (b.y - a.y) * (b.y - a.y) < (aSize + bSize) * (aSize + bSize);
}