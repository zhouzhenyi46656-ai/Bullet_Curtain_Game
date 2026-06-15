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


//定义全局变量
PLANE player;
PLANE enemy[ENEMY_NUM];
int enemyExistedCount;
time_t startTime, endTime;
IMAGE img[10];
ExMessage msg;
int score;
bool gameRunning = true;
bool gameStarted = false;   
int selectedCharacter = 0;  


//文件函数声明
void coverage();
void initGame();
void pastePictures();
void updateGame();


int main()
{
	startTime = time(NULL);

	loadimage(&img[0], "../图片素材/封面.png", SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&img[1], "../图片素材/背景1.png", SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&img[2], "../图片素材/博丽灵梦_src.bmp", PLANE_SIZE, PLANE_SIZE);
	loadimage(&img[3], "../图片素材/博丽灵梦_mask.bmp", PLANE_SIZE, PLANE_SIZE);
	loadimage(&img[4], "../图片素材/魔雨雾理沙_src.bmp", PLANE_SIZE, PLANE_SIZE);
	loadimage(&img[5], "../图片素材/魔雨雾理沙_mask.bmp", PLANE_SIZE, PLANE_SIZE);
	loadimage(&img[6], "../图片素材/子弹_品红_src.bmp", BULLET_SIZE, BULLET_SIZE);
    loadimage(&img[7], "../图片素材/子弹_mask.bmp", BULLET_SIZE, BULLET_SIZE);

    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);//打开窗口
    setbkcolor(BLACK);//设置底色
    settextstyle(30, 0, "微软雅黑");//设置文字样式
    setbkmode(TRANSPARENT);//设置文字背景
    settextcolor(WHITE);//设置文字颜色
    
    coverage();
    while (1) {
   		msg = getmessage(EX_KEY);
 		if (msg.message == WM_KEYDOWN)break;
	}

	initGame();
	pastePictures();
	
	while (gameRunning) {
		updateGame();   
		Sleep(REFRESH_INTERVAL);
	}
	closegraph();
	return 0;
}

void coverage()
{
	
	BeginBatchDraw();
	putimage(0, 0, &img[0]);
	int width = textwidth("按任意键开始游戏");
	outtextxy(SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT - 100, "按任意键开始游戏");
	EndBatchDraw();
}

void initGame()
{
	score = 0;//初始化分数
	srand((unsigned)time(NULL));
	player.bulletExistedCount = 0;//初始化玩家子弹数量
	for (int i = 0;i < 10;i++)player.planeBullet[i] = { 0,0,0,3 };//初始化子弹运动状态
	player.planeState = { SCREEN_WIDTH / 2 - PLANE_SIZE / 2, SCREEN_HEIGHT - PLANE_SIZE, 0, 0 };
	enemyExistedCount = 0;
	startTime = time(NULL);
}

void pastePictures()
{
    BeginBatchDraw();
    cleardevice();

    // 绘制背景
    putimage(0, 0, &img[1]);

    // ========== 选人阶段 ==========
    if (!gameStarted) {
        int leftX = SCREEN_WIDTH / 2 - PLANE_SIZE - 50;
        int rightX = SCREEN_WIDTH / 2 + 50;
        int y = SCREEN_HEIGHT / 2;

        // 绘制博丽灵梦（左边）
        putimage(leftX - PLANE_SIZE / 2, y - PLANE_SIZE / 2, &img[3], SRCAND);
        putimage(leftX - PLANE_SIZE / 2, y - PLANE_SIZE / 2, &img[2], SRCPAINT);

        // 绘制魔雨雾理沙（右边）
        putimage(rightX - PLANE_SIZE / 2, y - PLANE_SIZE / 2, &img[5], SRCAND);
        putimage(rightX - PLANE_SIZE / 2, y - PLANE_SIZE / 2, &img[4], SRCPAINT);

        // 高亮当前选中角色
        int highlightX = (selectedCharacter == 0) ? leftX : rightX;
        setlinecolor(YELLOW);
        setlinestyle(PS_SOLID, 3);
        rectangle(highlightX - PLANE_SIZE / 2 - 5, y - PLANE_SIZE / 2 - 5,
            highlightX + PLANE_SIZE / 2 + 5, y + PLANE_SIZE / 2 + 5);

        // 提示文字
        RECT tipRect_1 = { 0, SCREEN_HEIGHT - 160, SCREEN_WIDTH, SCREEN_HEIGHT },
			 tipRect_2 = { 0, SCREEN_HEIGHT - 80, SCREEN_WIDTH, SCREEN_HEIGHT };
        drawtext( "← 博丽灵梦   魔雨雾理沙 →", &tipRect_1, DT_TOP | DT_CENTER);
        drawtext("按空格/回车开始游戏", &tipRect_2, DT_TOP | DT_CENTER);
    }
    // ========== 游戏阶段 ==========
    else {
        // 1. 绘制玩家（根据选择的角色）
        if (selectedCharacter == 0) {  // 博丽灵梦
            putimage(player.planeState.x - PLANE_SIZE / 2, player.planeState.y - PLANE_SIZE / 2, &img[3], SRCAND);
            putimage(player.planeState.x - PLANE_SIZE / 2, player.planeState.y - PLANE_SIZE / 2, &img[2], SRCPAINT);
        }
        else {  // 魔雨雾理沙
            putimage(player.planeState.x - PLANE_SIZE / 2, player.planeState.y - PLANE_SIZE / 2, &img[5], SRCAND);
            putimage(player.planeState.x - PLANE_SIZE / 2, player.planeState.y - PLANE_SIZE / 2, &img[4], SRCPAINT);
        }

        // 2. 绘制敌人
        for (int i = 0; i < ENEMY_NUM; i++) {
            // 只绘制存在的敌人
            if (enemy[i].HP > 0) {
                putimage(enemy[i].planeState.x - PLANE_SIZE / 2,
                    enemy[i].planeState.y - PLANE_SIZE / 2, &img[5], SRCAND);
                putimage(enemy[i].planeState.x - PLANE_SIZE / 2,
                    enemy[i].planeState.y - PLANE_SIZE / 2, &img[4], SRCPAINT);
            }
        }

        // 3. 绘制玩家子弹
        for (int i = 0; i < player.bulletExistedCount; i++) {
            putimage(player.planeBullet[i].x - BULLET_SIZE / 2, player.planeBullet[i].y - BULLET_SIZE / 2, &img[7], SRCAND);
            putimage(player.planeBullet[i].x - BULLET_SIZE / 2, player.planeBullet[i].y - BULLET_SIZE / 2, &img[6], SRCPAINT);
        }
    }

    // 绘制分数
    RECT scoreRect = { 0, PLANE_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT };
    char str[20];
    sprintf_s(str, "分数：%d", score);
    drawtext(str, &scoreRect, DT_TOP | DT_CENTER);

    EndBatchDraw();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
