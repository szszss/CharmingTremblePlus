#pragma execution_character_set("utf-8")
#pragma once
#define WINDOW_TITLE "Charming Tremble"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH_FLOAT 800.0f
#define WINDOW_HEIGHT_FLOAT 600.0f
#define WINDOW_FRAME 1000/60

#if defined(_WIN32) || defined(WIN32)
	#define OS_WINDOWS
	#define FONT_DEFAULT "msyh.ttf"
	#define FONT_BACKUP "simsun.ttc"
#elif defined(__LINUX) || defined(__LINUX__) || defined(LINUX)
	#define OS_LINUX
#elif defined(__APPLE__) || defined(__MACOSX__)
	#define OS_MAC
#endif

#ifdef OS_WINDOWS //let's fuck WinDef
	typedef int BOOL;
	#define TRUE 1
	#define FALSE 0
#else
	#ifndef BOOL
	#define BOOL int
	#define TRUE 1
	#define FALSE 0
	#endif
#endif

typedef unsigned char byte;
typedef unsigned long Hash;

typedef struct implWorld                World;              //游戏世界
typedef struct implAttribute            Attribute;          //属性
typedef struct implAttributeInstance    AttributeInstance;  //属性实例
typedef struct implEntityPrototype      EntityPrototype;    //实体原型
typedef struct implEntity               Entity;             //实体
typedef struct implEntityPlayer         EntityPlayer;       //玩家实体
typedef struct implEntityBlockPrototype EntityBlockPrototype;//砖块原型
typedef struct implEntityBlock          EntityBlock;        //普通砖块实体
typedef struct implEntityBlockBonus     EntityBlockBonus;   //带特殊加成的砖块实体

typedef struct implTexture              Texture;            //纹理
typedef struct implTextTexture          TextTexture;        //文字纹理

typedef struct implPMD_Model            PMD_Model;          //PMD模型
typedef struct implPMD_ModelInstance    PMD_ModelInstance;  //PMD模型实例
//typedef struct implPMD_Texture        PMD_Texture;        //纹理
typedef struct implPMD_Material         PMD_Material;       //材质
typedef struct implPMD_Vertex           PMD_Vertex;         //顶点
typedef struct implPMD_Bone             PMD_Bone;           //骨骼
typedef struct implPMD_BoneInstance     PMD_BoneInstance;   //骨骼实例
typedef struct implPMD_BoneHierarchy    PMD_BoneHierarchy;  //骨骼层次体系
typedef struct implPMD_Animation        PMD_Animation;      //动画
typedef struct implPMD_KeyFrame         PMD_KeyFrame;       //动画关键帧
typedef struct implPMD_AnimationPlayer  PMD_AnimationPlayer;//动画播放器

typedef struct implGuiScreen            GuiScreen;          //界面
typedef struct implGuiButton            GuiButton;          //按钮

typedef struct implOSM                  OSM;                //操作状态机

typedef struct implLinkedList           LinkedList;         //链表
typedef struct implLinkedListIterator   LinkedListIterator; //链表遍历器
typedef struct implLinkedListNode       _LinkedListNode;    //链表节点
typedef struct implHashTree             HashTree;           //哈希二叉树
typedef struct implHashTreeNode         _HashTreeNode;      //哈希二叉树节点

typedef struct implMTRandomGen          MTRandomGen;        //梅森旋转算法随机数发生器
typedef struct implLogger               Logger;             //日志记录器
typedef struct implStringBuilder        StringBuilder;      //字符串构造器
typedef struct implRank                 Rank;               //玩家排名

typedef struct implQuaternion           Quaternion;         //四元数
typedef struct implMatrix               Matrix;             //矩阵

/*
enum ScreenState {
	SS_MAINMENU,
	SS_GAME};*/

enum WorldType {
	TYPE_UNKNOWN=-1,
	TYPE_NORMAL=1,
	TYPE_REPLAY=10,
	TYPE_DEMO=100};

enum WorldDifficulty {
	DIFF_UNKNOWN=-1,
	DIFF_PEACEFUL=0,
	DIFF_EASY=1,
	DIFF_NORMAL=2,
	DIFF_HARD=3,
	DIFF_LUNATIC=4,
	DIFF_EXTRA=5,
	DIFF_PHANTOM=6};

enum WorldState{
	WSTATE_STOP,
	WSTATE_RUN,
	WSTATE_GAMEOVERING,
	WSTATE_GAMEOVERED};

/*
enum PMD_Texture_Status {
	PMD_Texture_Status_Init       = 1,  //The data of texture hasn't been loaded. Unavailable.
	PMD_Texture_Status_Loading    = 2,  //The data of texture is been loading. Unavailable.
	PMD_Texture_Status_Processing = 6,  //The data of texture has been loaded but its data hasn't been processed. Unavailable.
	PMD_Texture_Status_Loaded     = 8,  //The texture has been loaded and processed and it's AVAILABLE now.
	PMD_Texture_Status_Unload     = 16, //The texture has been unloaded. Unavailable.
	PMD_Texture_Status_Error      = 0   //Something is wrong. PMD use "!status" to check error.
};

enum PMD_Texture_Type {
	PMD_Texture_Type_2DTexture,
	PMD_Texture_Type_Spa,
	PMD_Texture_Type_Toon
};

enum PMD_Texture_Format {
	PMD_Texture_Type_Bmp,
	PMD_Texture_Type_Jpg,
	PMD_Texture_Type_Png,
	PMD_Texture_Type_Tga,
	PMD_Texture_Type_Dds
};*/

BOOL GameSetPause(BOOL pause);
BOOL GameGetPause();
//崩溃掉游戏...并抛出原因
void GameCrash(char* cause);
void GameExit();

long long GameGetMaxScore();
void GameUpdateMaxScore(long long score);


/*
　　　　　　　　　　          _,○､ 
　　　　　　　○､　　　　 ,　'´　〈ﾍ.） 
　　　　　　／,ﾊ_）　　／　　　　｀メ. 
　　　　 ／　　└-‐'´　　　　　 [__,ﾊ 
　　 　./＼＿i＞-‐‐ｧ'´￣｀Yヽ､_　 | 
　　　/_ｒ'"´Y_＞-‐'"´￣￣｀ヽ./｀ヽ! 
　　ｒ'´7-‐'"´ 　 i　　　ﾊ_＿i　 ＼__,ﾊ 
　 〈Y´　　 i　__ ,ﾊ 　 / _」__,ﾊ　　| ｀ヽ〉　　　 
　　`!　/　ﾊ´ _」_ |／ '´i,ﾉ ｀ﾘ　 /　　 !／! 
　　 |　! 　 |ｨ´i,ﾉ !　 　　'､__ﾝ|／ 　　/　/ 　ﾄ 、 
　　 ',　＼,ﾊ.` '､.ﾉ　 .　　　 ⊂⊃ .／､_,.!　 /　,ﾊ　　 ,| 
　　　＼ | ヽ!⊃　 　 -‐ '　 ,.ｲ　 〈 　 　|／レへ,! 　/ ! 
　　　 　｀ヽ,|｀＞ｧ‐---rｧ' /`ﾄ､へ） .／ 　 　 　 !／　| 
　　　　　　 ﾚへ!ノレアiレ'::::/　ﾒ、 /　　 　　 　 |､/ヽ| 
　　　　 ＿_ ｀＼ 　_/ ,|.!:::::ｲ|＿　∨　　 　 　　,ﾊ　　 | 
　　　　 ＼/￣ /´￣`ｧt‐ｧ'´　 ｀　i 　　　 　　/　　　/ 
　　　　　　｀ヽ〈　　　 | |　|　　 　 人/ 　 　／　　 ／ 
　　　　 ＜⌒７`ーｧ'i |￣｀i-r‐＜ヽ.__,,..イ‐--‐＜ 
　　　　　　＼! .／　//::::::::| |　　　,ハ　 　　　 　く＼ 
　　　　　　　rく/　く〈:ﾛ::ﾛ::|/　 　　　 ト､　　 　＜　 ｀＞ 
　　　　　　 /＼ﾝ､_ `ー-‐'　　/ 　　 ﾉ'〉＞､,_＿,.＞'´ 
　　　　　∠..＿,ﾍ./こ＞-＜こン‐ｧ´!ノ　 　 
　　　　　　　　　 ﾄ-‐/´￣｀'トｰ‐ｧ'´ 
　　　　　　　 　　!__/　　 　 ﾄ-‐/ 
　　　　　　　　　　　　　　　 !__/

				   Grace and Mercy
*/