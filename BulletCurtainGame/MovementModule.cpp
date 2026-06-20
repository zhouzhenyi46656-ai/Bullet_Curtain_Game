//标准库
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>          
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


// 圆周率常量
#ifndef PI
#define PI 3.1415926535
#endif


// 一、Boss 移动模块
// 1. boss入场
void bossKineticModule_0()
{
    boss.planeState.vx = 0;
    boss.planeState.vy = BOSS_SPEED;
    boss.planeState.x = SCREEN_WIDTH / 2; // 固定在屏幕中央
    boss.planeState.y += boss.planeState.vy;
}

// 2. 左右移动模式
void bossKineticModule_1()
{
    boss.planeState.vx = BOSS_SPEED;
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

// 3. 追踪玩家模式
void bossKineticModule_2()
{
    int dx = player.planeState.x - boss.planeState.x;
    if (dx > 0) boss.planeState.vx = BOSS_SPEED;
    else if (dx < 0) boss.planeState.vx = -BOSS_SPEED;
    else boss.planeState.vx = 0;

    if (dx < BOSS_SPEED && dx > -BOSS_SPEED) boss.planeState.vx = dx; // 防止过度追踪导致抖动
    boss.planeState.x += boss.planeState.vx;

    if (boss.planeState.x > SCREEN_WIDTH - BOSS_SIZE / 2) boss.planeState.x = SCREEN_WIDTH - BOSS_SIZE / 2;
    else if (boss.planeState.x < BOSS_SIZE / 2) boss.planeState.x = BOSS_SIZE / 2;
}


// 二、Boss 弹幕函数
// 1. 环形弹幕（以boss为中心向四周发射）
// 参数：bulletCount - 一圈发射多少颗子弹
//       angleOffset - 角度偏移
void BossPattern_Ring(PLANE* plp, int bulletCount, double& angleOffset)
{
    angleOffset += 0.10;  // 旋转速度

    if (plp -> bulletExistedCount >= BULLET_NUM) {
        return;
    }

    for (int i = 0; i < bulletCount; i++) {
        double angle = (2 * PI / bulletCount) * i + angleOffset;

        // 计算子弹速度方向
        int speed = BOSS_BULLET_SPEED;
        double vx = speed * cos(angle);
        double vy = speed * sin(angle);

        // 设置子弹位置
        int index = plp -> bulletExistedCount;
        plp -> planeBullet[index].x = plp -> planeState.x;
        plp -> planeBullet[index].y = plp -> planeState.y;
        plp -> planeBullet[index].vx = vx;
        plp -> planeBullet[index].vy = vy;
        plp -> bulletExistedCount++;

        // 如果子弹已满，停止继续添加
        if (plp -> bulletExistedCount >= BULLET_NUM) {
            break;
        }
    }
}

// 2. 花瓣形弹幕
// 参数：bulletCount - 发射子弹数量
//       angleOffset - 角度偏移
void BossPattern_Flower(int bulletCount, double& angleOffset)
{
    angleOffset += 0.40;

    if (boss.bulletExistedCount >= BULLET_NUM) {
        return;
    }

    // 每次发射一颗子弹
    double angle = angleOffset;

    // 从boss中心发射
    int index = boss.bulletExistedCount;

    // 计算速度
    int speed = BOSS_BULLET_SPEED;
    double vx = speed * cos(angle);
    double vy = speed * sin(angle);

    // 设置子弹位置
    boss.planeBullet[index].x = boss.planeState.x;
    boss.planeBullet[index].y = boss.planeState.y;
    boss.planeBullet[index].vx = vx;
    boss.planeBullet[index].vy = vy;
    boss.bulletExistedCount++;
}

// 3. 随机选择弹幕（用于200分以上）
void BossPattern_Random(double& ringAngle, double& flowerAngle)
{
    // 随机选择 0 或 1
    int choice = rand() % 2;

    if (choice == 0) {
        BossPattern_Ring(&boss, 16, ringAngle);
    }
    else {
        BossPattern_Flower(1, flowerAngle);
    }
}


// 三、敌人与敌人子弹移动模块
// 1. 从上到下移动
void enemyKineticModule1()
{   
    double x = rand() % (SCREEN_WIDTH - PLANE_SIZE) + PLANE_SIZE / 2;
    enemy[enemyExistedCount].planeState = {x, -PLANE_SIZE, 0, BOSS_SPEED}; // 固定速度向下移动
	enemy[enemyExistedCount].bulletExistedCount = 0;
    enemy[enemyExistedCount].HP = 6;
    enemy[enemyExistedCount].enemyType = 0;
	enemyExistedCount++;
}

// 2. 从左到右移动
void enemyKineticModule2()
{
	double direction = rand() % 2 * 2 - 1;
	double y = SCREEN_HEIGHT / 2 + 100 - rand() % 200;
	enemy[enemyExistedCount].planeState = { SCREEN_WIDTH / 2 - (direction * (SCREEN_WIDTH / 2 + PLANE_SIZE)), y, direction * BOSS_SPEED, 0};//随机左右出现
	enemy[enemyExistedCount].bulletExistedCount = 0;
	enemy[enemyExistedCount].HP = 4;
    enemy[enemyExistedCount].enemyType = 1;
	enemyExistedCount++;
}

// 3.追踪玩家移动
void enemyKineticModule3()
{
    //敌人出现
    double direction = rand() % 2 * 2 - 1;
    double y = SCREEN_HEIGHT / 2 + 100 - rand() % 200;
    enemy[enemyExistedCount].planeState = { SCREEN_WIDTH / 2 - (direction * (SCREEN_WIDTH / 2 + PLANE_SIZE)), y, 0, 0 };//随机左右出现
    enemy[enemyExistedCount].bulletExistedCount = 0;
    enemy[enemyExistedCount].HP = 2;
    enemy[enemyExistedCount].enemyType = 2;
    enemyExistedCount++;
}