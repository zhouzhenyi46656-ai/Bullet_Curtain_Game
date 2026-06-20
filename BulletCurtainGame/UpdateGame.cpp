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
#include <math.h>


// 函数声明
int areIntersecting(KINESTATE a, int aSize, KINESTATE b, int bSize);
bool SegmentCircleIntersect(KINESTATE bullet, KINESTATE enemy, double r);


// ========== updateGame 主函数 ==========
void updateGame()
{
    static ULONGLONG lastBulletTime = 0;
    static ULONGLONG lastBossBulletTime = 0;
	static ULONGLONG lastEnemyGenTime = 0;
	static ULONGLONG lastEnemyBulletGenTime = 0;
    static ULONGLONG lastPlayerBossHit = 0;
    static ULONGLONG now = 0;

    // Boss 弹幕角度偏移（用于旋转效果）
    static double ringAngle = 0.0;      // 环形弹幕角度
    static double flowerAngle = 20.0;    // 花瓣弹幕角度

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
        now = GetTickCount();
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
	// 1. 生成敌人
	now = GetTickCount();
	if (now - lastEnemyGenTime > ENEMY_GEN_INTERVAL) {
		lastEnemyGenTime = now;
		if (enemyExistedCount < ENEMY_NUM) {
            srand(time(NULL));
			int type = rand() % 6; // 随机选择敌人类型
			if (type < 3) {
				enemyKineticModule1();
			}
			else if(type == 5){
				enemyKineticModule3();
			}
            else {
				enemyKineticModule2();
            }
		}
	}

	// 2. 更新敌人位置
	for (int i = 0;i < enemyExistedCount;i++) {
        if (enemy[i].enemyType != 2) {
            enemy[i].planeState.x += enemy[i].planeState.vx;
            enemy[i].planeState.y += enemy[i].planeState.vy;
        }
        else {
            //追踪玩家
            int dx = player.planeState.x - enemy[i].planeState.x,
                dy = player.planeState.y - enemy[i].planeState.y;
            double angle = atan2(dy, dx);
            enemy[i].planeState.x += BOSS_SPEED * cos(angle);
            enemy[i].planeState.y += BOSS_SPEED * sin(angle);
		}
	}

	// 3. 更新敌人子弹位置
	now = GetTickCount();
        // 1. 更新子弹
    for (int i = 0;i < enemyExistedCount;i++) {
        if (now - enemy[i].lastBulletTime > ENEMY_BULLET_GEN_INTERVAL) {
            if (!enemy[i].enemyType) {
                for (int j = 0;j < enemy[i].bulletExistedCount;j++) {
                    enemy[i].planeBullet[j].vy = -BOSS_BULLET_SPEED;
                    enemy[i].planeBullet[j].y += enemy[i].planeBullet[j].vy;
                }
            }
            else if (enemy[i].enemyType & 1) {
                for (int j = 0;j < enemy[i].bulletExistedCount;j++) {
                    BossPattern_Ring(&enemy[i], 16, ringAngle);
                }
            } 
        }
    }
    
        // 2. 生成子弹
    if (now - lastEnemyBulletGenTime > ENEMY_BULLET_GEN_INTERVAL) {
        lastEnemyBulletGenTime = now;
        for (int i = 0; i < enemyExistedCount; i++) {
            if (enemy[i].bulletExistedCount < BULLET_NUM) {
                // 从敌人位置发射子弹
                int index = enemy[i].bulletExistedCount;
                enemy[i].planeBullet[index].x = enemy[i].planeState.x;
                enemy[i].planeBullet[index].y = enemy[i].planeState.y;
                enemy[i].planeBullet[index].vx = 0;
                enemy[i].planeBullet[index].vy = -BOSS_BULLET_SPEED;
                enemy[i].bulletExistedCount++;
            }
        }
    }


    // 三. 更新boss状态
	// 1. 更新boss位置
    if (score >= 0 && boss.planeState.y < BOSS_SIZE * 2) {
        bossKineticModule_0();
    }
    else if (score >= 100 && score <= 200) {
        bossKineticModule_1();
    }
    else if (score > 200) {
        bossKineticModule_2();
    }

    // 2. 生成Boss弹幕并更新子弹位置
    now = GetTickCount();
    // 根据分数选择弹幕模式
    // 模式1：花瓣弹幕（分数 0-100）
    if (score >= 0 && score < 100 && now - lastBossBulletTime > BULLET_GEN_BOSS_INTERVAL) {
        lastBossBulletTime = now;
        BossPattern_Flower(1, flowerAngle);
    }
    // 模式2：环形弹幕（分数 100-200）
    else if (score >= 100 && score < 200 && now - lastBossBulletTime > BULLET_GEN_BOSS_INTERVAL*10) {
        lastBossBulletTime = now;
        BossPattern_Ring(&boss, 32, ringAngle);
    }
    // 模式3：随机混合（分数 200+）
    else if (score >= 200) {
        static int actionTimer = 0;
        static bool isShooting = true;

        actionTimer++;

        if (isShooting) {
            if (now - lastBossBulletTime > BULLET_GEN_BOSS_INTERVAL*8) {
                lastBossBulletTime = now;
                BossPattern_MultiPetal(6, flowerAngle);
            }

            if (actionTimer >= SHOOT_DURATION) {
                actionTimer = 0;
                isShooting = false;
            }
        }
        else {
            if (actionTimer >= GAP_DURATION) {
                actionTimer = 0;
                isShooting = true;
            }
        }
    }


    // 四. 检测碰撞和越界
    // 1. 检查玩家子弹越界
    for (int i = 0; i < player.bulletExistedCount; i++) {
        player.planeBullet[i].y -= player.planeBullet[i].vy;

        if (player.planeBullet[i].y < 0) {
            player.planeBullet[i] = player.planeBullet[player.bulletExistedCount - 1];
            player.bulletExistedCount--;
            i--;
        }
    }
    
    // 2. 检查boss子弹越界
    for (int i = 0; i < boss.bulletExistedCount; i++) {
        boss.planeBullet[i].x += boss.planeBullet[i].vx;
        boss.planeBullet[i].y += boss.planeBullet[i].vy;

        if (boss.planeBullet[i].x < -BOSS_BULLET_SIZE ||boss.planeBullet[i].x > SCREEN_WIDTH + BOSS_BULLET_SIZE ||
            boss.planeBullet[i].y < -BOSS_BULLET_SIZE ||boss.planeBullet[i].y > SCREEN_HEIGHT + BOSS_BULLET_SIZE) 
        {
            boss.planeBullet[i] = boss.planeBullet[boss.bulletExistedCount - 1];
            boss.bulletExistedCount--;
            i--;  
        }
    }

    // 3. 检查敌人越界
    for (int i = 0;i < enemyExistedCount;i++) {
        if (enemy[i].planeState.x < -PLANE_SIZE || enemy[i].planeState.x > SCREEN_WIDTH + PLANE_SIZE ||
            enemy[i].planeState.y < -PLANE_SIZE || enemy[i].planeState.y > SCREEN_HEIGHT + PLANE_SIZE)
        {
            enemy[i] = enemy[enemyExistedCount - 1];
            enemyExistedCount--;
            i--;
        }
    }

    // 4. 检查玩家与敌人碰撞
    for (int i = 0;i < enemyExistedCount;i++) {
        if (areIntersecting(player.planeState, JUDGE_SCOPE, enemy[i].planeState, JUDGE_SCOPE)) {
            enemy[i] = enemy[enemyExistedCount - 1];
            enemyExistedCount--;
            i--;
            player.HP--;
        }
    }

    // 5. 检查玩家与boss碰撞
    now = GetTickCount();
    if (now - lastPlayerBossHit > 2000) {
        if (areIntersecting(player.planeState, JUDGE_SCOPE / 2, boss.planeState, JUDGE_SCOPE * 3)) {
            player.HP--;
        }
    }

    // 6. 检查玩家子弹与敌人碰撞
    for (int i = 0; i < player.bulletExistedCount; i++) {
        for (int j = 0; j < enemyExistedCount; j++) {
            if (areIntersecting(player.planeBullet[i], JUDGE_SCOPE, enemy[j].planeState, JUDGE_SCOPE*1.5)) {
                enemy[j].HP--;
                player.planeBullet[i] = player.planeBullet[player.bulletExistedCount - 1];
                player.bulletExistedCount--;
                i--;

               // 根据敌人类型增加不同分数
                if (enemy[j].enemyType == 0) {
                    score += 2;  // 普通敌人（HP=6）
                }
                else if (enemy[j].enemyType == 1) {
                    score += 4;  // 中型敌人（HP=4）
                }
                else if (enemy[j].enemyType == 2) {
                    score += 6;  // 追踪敌人（HP=2）
                }

                // 如果敌人 HP 归零，移除敌人
                if (enemy[j].HP <= 0) {
                    enemy[j] = enemy[enemyExistedCount - 1];
                    enemyExistedCount--;
                    j--;
                }

                break;  // 子弹已消失，跳出内层循环
            }
        }
    }

    // 7. 检查玩家子弹与boss碰撞
    for (int i = 0;i < player.bulletExistedCount;i++) {
        if (areIntersecting(player.planeBullet[i], JUDGE_SCOPE, boss.planeState, JUDGE_SCOPE * 3)) {
            score++;
            player.planeBullet[i] = player.planeBullet[player.bulletExistedCount - 1];
            player.bulletExistedCount--;
            i--;
            boss.HP--;
        }
    }

    // 8. 检查敌人子弹与玩家碰撞
    for (int i = 0; i < enemyExistedCount; i++) {
        for (int j = 0; j < enemy[i].bulletExistedCount; j++) {
            if (areIntersecting(enemy[i].planeBullet[j], JUDGE_SCOPE, player.planeState, JUDGE_SCOPE / 2)) {
                // 敌人子弹命中玩家
                enemy[i].planeBullet[j] = enemy[i].planeBullet[enemy[i].bulletExistedCount - 1];
                enemy[i].bulletExistedCount--;
                j--;
                player.HP--;
            }
        }
    }


    // 9. 检查boss子弹与玩家碰撞
    for (int i = 0;i < boss.bulletExistedCount;i++) {
        if (areIntersecting(boss.planeBullet[i], JUDGE_SCOPE, player.planeState, JUDGE_SCOPE/2)) {
            boss.planeBullet[i] = boss.planeBullet[boss.bulletExistedCount - 1];
            boss.bulletExistedCount--;
            i--;
            player.HP--;
        }
    }
    // 10. 检查敌人子弹越界
    for (int i = 0; i < enemyExistedCount; i++) {
        for (int j = 0; j < enemy[i].bulletExistedCount; j++) {
            if (enemy[i].planeBullet[j].x < -BULLET_SIZE ||
                enemy[i].planeBullet[j].x > SCREEN_WIDTH + BULLET_SIZE ||
                enemy[i].planeBullet[j].y < -BULLET_SIZE ||
                enemy[i].planeBullet[j].y > SCREEN_HEIGHT + BULLET_SIZE) {
                enemy[i].planeBullet[j] = enemy[i].planeBullet[enemy[i].bulletExistedCount - 1];
                enemy[i].bulletExistedCount--;
                j--;
            }
        }
    }

    // 五. 绘制画面 
    pastePictures();
}


int areIntersecting(KINESTATE a, int aSize, KINESTATE b, int bSize)
{
    return (a.x - b.x) * (a.x - b.x) + (b.y - a.y) * (b.y - a.y) < (aSize + bSize) * (aSize + bSize);
}
