0038:npc_poolitemshop.c:0702B:1:14=48041e4b:16=47d21313:#include "version.h"

#include "char.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "npc_poolitemshop.h"
#include "log.h"
#include "handletime.h"

#define		NPC_POOLITEMSHOP_DEFAULT_COST		200

enum {
	NPC_WORK_COST = CHAR_NPCWORKINT1,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
};

enum {
	NPC_POOLITEMSHOP_MSG_MAIN=0,
	NPC_POOLITEMSHOP_MSG_POOL,
	NPC_POOLITEMSHOP_MSG_DRAW,
	NPC_POOLITEMSHOP_MSG_REALY,
	NPC_POOLITEMSHOP_MSG_STONE,
	NPC_POOLITEMSHOP_MSG_POOLFULL,
	NPC_POOLITEMSHOP_MSG_ITEMFULL,

	CHAR_WINDOWTYPE_POOLITEMSHOP_START = 290,
	CHAR_WINDOWTYPE_POOLITEMSHOP_POOL_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_DRAW_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_FULL_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_HAVEITEMFULL_MSG,
	CHAR_WINDOWTYPE_POOLITEMSHOP_END,

#ifdef _NPC_DEPOTITEM
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_MENU = 310,
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_HANDLE,
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_ADD,
	CHAR_WINDOWTYPE_DEPOTITEMSHOP_GET,
#endif
};

typedef struct {
	char	option[32];
	char	defaultmsg[128];
}NPC_POOLITEMSHOP_MSG;

NPC_POOLITEMSHOP_MSG		poolshopmsg[] = {
	{ "main_msg",		"欢迎"},
	{ "pool_main",		"要寄放什麽呢"},
	{ "draw_main",		"要领养什麽呢"},
	{ "realy_msg",		"真的要那个吗"},
	{ "stone_msg",		"钱不够喔!"},
	{ "poolfull_msg",	"超过的无法处理喔"},
	{ "itemfull_msg",	"项目有很多"}
};

static BOOL NPC_PoolItemShop_DrawItem( int meindex, int talkerindex, int num);
static BOOL NPC_PoolItemShop_PoolItem( int meindex, int talkerindex, int num);
static void NPC_PoolItemShop_MakeItemString_Draw( int meindex, int talkerindex, 
								char *retstring,int retstringlen);
static void NPC_PoolItemShop_MakeItemString_Pool( int meindex, int talkerindex, 
								char *retstr