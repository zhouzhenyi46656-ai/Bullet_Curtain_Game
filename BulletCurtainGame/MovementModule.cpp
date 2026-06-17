//标准库
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
//按键库
#include <conio.h>
//图形界面库
#include <easyx.h>
#include <graphics.h>
//Windows API
#include <Windows.h>
#pragma comment(lib,"Winmm.lib")//链接多媒体库
//自定义头文件
#include "BulletCurtainGame.h"


void bossKineticModule_0()
{
	boss.planeState.vx = 0;
	boss.planeState.vy = BOSS_SPEED;
	boss.planeState.x = SCREEN_WIDTH / 2; // 固定在屏幕中央
	boss.planeState.y += boss.planeState.vy;
}

void bossKineticModule_1()
{
	boss.planeState.vx = BOSS_SPEED;
	// 简单的左右移动模式
	static int direction = 1; // 1=右，-1=左
	boss.planeState.x += boss.planeState.vx * direction;
	// 到达边界时改变方向
	int halfSize = BOSS_SIZE / 2;
	if (boss.planeState.x < halfSize) {
		boss.planeState.x = halfSize;
		direction = 1; // 向右移动
	}
	else if (boss.planeState.x > SCREEN_WIDTH - halfSize) {
		boss.planeState.x = SCREEN_WIDTH - halfSize;
		direction = -1; // 向左移动
	}
}

void bossKineticModule_2()
{
	// 复杂的追踪玩家模式
	int dx = player.planeState.x - boss.planeState.x;
	if (dx > 0) boss.planeState.vx = BOSS_SPEED;
	else if (dx < 0) boss.planeState.vx = -BOSS_SPEED;
	else boss.planeState.vx = 0;

	if (dx<BOSS_SPEED && dx>-BOSS_SPEED) boss.planeState.vx = dx; // 防止过度追踪导致抖动
	boss.planeState.x += boss.planeState.vx;

	if (boss.planeState.x > SCREEN_WIDTH - BOSS_SIZE / 2) boss.planeState.x = SCREEN_WIDTH - BOSS_SIZE / 2;
	else if (boss.planeState.x < BOSS_SIZE / 2) boss.planeState.x = BOSS_SIZE / 2;
	/*int dx = player.planeState.x - boss.planeState.x;
	int dy = player.planeState.y - boss.planeState.y;
	double distance = sqrt(dx * dx + dy * dy);
	if (distance > 0) {
		boss.planeState.vx = (int)(BOSS_SPEED * dx / distance);
		boss.planeState.vy = (int)(BOSS_SPEED * dy / distance);
	}
	else {
		boss.planeState.vx = 0;
		boss.planeState.vy = 0;
	}
	boss.planeState.x += boss.planeState.vx;
	boss.planeState.y += boss.planeState.vy;*/
}
