//常量宏声明
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define PLANE_SIZE 50
#define PLAYER_SPEED 8

#define BOSS_SIZE 100
#define BOSS_SPEED 4
#define BOSS_BULLET_SPEED 4   

#define BULLET_SIZE 20
#define BULLET_NUM 1000
#define BULLET_SPEED 25
#define BULLET_GEN_INTERVAL 100
#define BULLET_GEN_BOSS_INTERVAL 50

#define ENEMY_NUM 8
#define ENEMY_GEN_INTERVAL 1000

#define BOSS_BULLET_SIZE 20

#define JUDGE_SCOPE 10

#define REFRESH_INTERVAL 16


//结构体声明
typedef struct kinestate
{
	double x;
	double y;
	double vx;
	double vy;
}KINESTATE;

typedef struct plane
{
	KINESTATE planeState;
	KINESTATE planeBullet[BULLET_NUM];
	int bulletExistedCount;
	int HP;
}PLANE;


//全局变量声明
extern bool gameRunning;
extern bool gameStarted;        // false=选人界面，true=游戏中
extern int selectedCharacter;   // 0=博丽灵梦，1=魔雨雾理沙
extern int score;
extern ExMessage msg;
extern IMAGE img[12];
extern PLANE player, boss;
extern PLANE enemy[ENEMY_NUM];
extern int enemyExistedCount;
extern time_t startTime, endTime;


//函数声明
void updateGame();
void pastePictures();
void bossKineticModule_0();
void bossKineticModule_1();
void bossKineticModule_2();
// Boss弹幕函数声明
void BossPattern_Ring(int bulletCount, double& angleOffset);
void BossPattern_Flower(int bulletCount, double& angleOffset);
void BossPattern_Random(double& ringAngle, double& flowerAngle);


