//常量宏声明
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define PLANE_SIZE 50
#define BULLET_SIZE 20
#define BULLET_NUM 100
#define BULLET_GEN_INTERVAL 100

#define ENEMY_NUM 8
#define ENEMY_GEN_INTERVAL 1000

#define REFRESH_INTERVAL 16


//全局变量声明
extern bool gameRunning;
extern bool gameStarted;        // false=选人界面，true=游戏中
extern int selectedCharacter;   // 0=博丽灵梦，1=魔雨雾理沙


//结构体声明
#define PLAYER_SPEED 6
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
	int HP;
}PLANE;


//函数声明
void updateGame();
