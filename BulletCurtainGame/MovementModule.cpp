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

// Boss 移动模块


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

    if (dx < BOSS_SPEED && dx > -BOSS_SPEED) boss.planeState.vx = dx; // 防止过度追踪导致抖动
    boss.planeState.x += boss.planeState.vx;

    if (boss.planeState.x > SCREEN_WIDTH - BOSS_SIZE / 2) boss.planeState.x = SCREEN_WIDTH - BOSS_SIZE / 2;
    else if (boss.planeState.x < BOSS_SIZE / 2) boss.planeState.x = BOSS_SIZE / 2;
}


// Boss 弹幕函数

// 第一种：环形弹幕（以boss为中心向四周发射）
// 参数：bulletCount - 一圈发射多少颗子弹
//       angleOffset - 角度偏移
void BossPattern_Ring(int bulletCount, double& angleOffset)
{
    angleOffset += 0.10;  // 旋转速度

    if (boss.bulletExistedCount >= BULLET_NUM) {
        return;
    }

    for (int i = 0; i < bulletCount; i++) {
        double angle = (2 * PI / bulletCount) * i + angleOffset;

        // 计算子弹速度方向
        int speed = BOSS_BULLET_SPEED;
        double vx = speed * cos(angle);
        double vy = speed * sin(angle);

        // 设置子弹位置
        int index = boss.bulletExistedCount;
        boss.planeBullet[index].x = boss.planeState.x;
        boss.planeBullet[index].y = boss.planeState.y;
        boss.planeBullet[index].vx = vx;
        boss.planeBullet[index].vy = vy;
        boss.bulletExistedCount++;

        // 如果子弹已满，停止继续添加
        if (boss.bulletExistedCount >= BULLET_NUM) {
            break;
        }
    }
}


// 第二种：花瓣形弹幕
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


// 第三种：随机选择弹幕（用于200分以上）
void BossPattern_Random(double& ringAngle, double& flowerAngle)
{
    // 随机选择 0 或 1
    int choice = rand() % 2;

    if (choice == 0) {
        BossPattern_Ring(16, ringAngle);
    }
    else {
        BossPattern_Flower(1, flowerAngle);
    }
}