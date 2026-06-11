//标准库
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
//按键库
#include <conio.h>
//图形界面库
#include <easyx.h>
//Windows API
#include <Windows.h>
#pragma comment(lib,"Winmm.lib")//链接多媒体库


//常量宏
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define PLANE_SIZE 50

#define ENEMY_NUM 8

#define ENEMY_GEN_INTERVAL 1.0

#define BULLET_NUM 10


//定义物体运动状态结构体和飞机结构体
typedef struct kinestate 
{
	int x;
	int y;
	int vx;
	int vy;
}KINESTATE;

typedef struct plane
{
	KINESTATE planeState;
	KINESTATE planeBullet[BULLET_NUM];
	int bulletExistedCount;
}PLANE;


//定义全局变量
PLANE player;
PLANE enemy[ENEMY_NUM];
int enemyExistedCount;
static time_t startTime, endTime;
IMAGE img[10];
int score;

void coverage();
void initGame();
void pastePictures();

int main()
{
	loadimage(&img[0], "../图片素材/封面.png", SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&img[1], "../图片素材/背景1.png", SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&img[2], "../图片素材/魂魄妖梦.png", PLANE_SIZE, PLANE_SIZE);
	loadimage(&img[3], "../图片素材/博丽灵梦.png", PLANE_SIZE, PLANE_SIZE);

	coverage();
	initGame();
	pastePictures();
	
}

void coverage()
{
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	BeginBatchDraw();
	putimage(0, 0, &img[0]);
	settextstyle(30, 0, "微软雅黑");
	int width = textwidth("按任意键开始游戏");
	settextcolor(WHITE);
	setbkmode(TRANSPARENT);
	outtextxy(SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT - 100, "按任意键开始游戏");
	EndBatchDraw();
	_getch();
}

void initGame()
{
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);//打开窗口
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
	putimage(0, 0, &img[1]);
	


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
