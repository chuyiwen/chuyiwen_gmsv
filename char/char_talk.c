#include "version.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "handletime.h"
#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "char_talk.h"
#include "chatmagic.h"
#include "battle.h"
#include "log.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "family.h"
#ifdef _CHAR_PROFESSION
#include "profession_skill.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
#include "net.h"


extern int channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];

#ifdef _CHANNEL_MODIFY
int *piOccChannelMember = NULL;
#endif

#ifdef _TALK_ACTION
void TalkAction(int charaindex, char *message);
#endif

#ifdef _GM_ITEM
static BOOL player_useChatMagic( int charaindex, char* data, BOOL isDebug);
#endif
extern int playeronlinenum;
static int pojietype = 0;
/*------------------------------------------------------------
 * チャット関連のソース
 ------------------------------------------------------------*/
#define DEBUGCDKEYNUM 100
struct tagDebugCDKey {
	int use;
	char cdkey[9];
};
static struct tagDebugCDKey DebugCDKey[DEBUGCDKEYNUM];

/*====================チャット魔法====================*/
typedef void (*CHATMAGICFUNC)(int,char*);
typedef struct tagCHAR_ChatMagicTable
{
	char*			magicname;
	CHATMAGICFUNC	func;
	BOOL            isdebug;
	int             hash;
	int				level;
	char*			usestring;
}CHAR_ChatMagicTable;

static CHAR_ChatMagicTable CHAR_cmtbl[]={
	//工程师专用
	{ "programming_engineer",	CHAR_CHAT_DEBUG_engineer,	TRUE,	0,	3, ""},
	{ "petlevelup",	CHAR_CHAT_DEBUG_petlevelup,	TRUE,	0,	2, "宠物栏号 等级 (账号)"},
	{ "petexpup",	CHAR_CHAT_DEBUG_petexpup,	TRUE,	0,	2, "宠物栏号 经验 (账号)"},
	{ "help",			CHAR_CHAT_DEBUG_help,				TRUE,	0,	1, "指令/all"},
#ifdef _EQUIT_NEGLECTGUARD
	{ "setneguard",		CHAR_CHAT_DEBUG_setneguard,			TRUE,	0,	3, "waei"},
#endif
	//人物属性
	{ "info",			CHAR_CHAT_DEBUG_info,			TRUE,	0,	1, "数值"},
	{ "level",			CHAR_CHAT_DEBUG_level,			TRUE,	0,	2, "数值 (账号)"},
	{ "settrans",		CHAR_CHAT_DEBUG_setTrans,		TRUE,	0,	2, "数值 (账号)"},
	{ "exp",			CHAR_CHAT_DEBUG_exp,			TRUE,	0,	2, "数值 (账号)"},
	{ "hp",				CHAR_CHAT_DEBUG_hp,				TRUE,	0,	2, "数值 (账号)"},
	{ "mp",				CHAR_CHAT_DEBUG_mp,				TRUE,	0,	2, "数值 (账号)"},
	{ "setmp",			CHAR_CHAT_DEBUG_setmp,			TRUE,	0,	2, "数值 (账号)"},
	{ "str",			CHAR_CHAT_DEBUG_str,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "dex",			CHAR_CHAT_DEBUG_dex,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "tgh",			CHAR_CHAT_DEBUG_tgh,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "vital",			CHAR_CHAT_DEBUG_vital,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "luck",			CHAR_CHAT_DEBUG_luck,			TRUE,	0,	2, "数值 (账号)"},
	{ "superman",		CHAR_CHAT_DEBUG_superman,		TRUE,	0,	2, "(账号)"},
	{ "dp",				CHAR_CHAT_DEBUG_dp,				TRUE,	0,	2, "数值 (账号)"},
#ifdef _EQUIT_ARRANGE
	{ "arrange",		CHAR_CHAT_DEBUG_arrange,		TRUE,	0,	3, "数值"},
#endif

#ifdef _EQUIT_SEQUENCE
	{ "sequence",		CHAR_CHAT_DEBUG_sequence,		TRUE,	0,	3, "数值"},
#endif

	//系统
	{ "announce",		CHAR_CHAT_DEBUG_announce,		TRUE,	0,	1, "内容"},
	{ "loginannounce",	CHAR_CHAT_DEBUG_loginannounce,	TRUE,	0,	1, "内容"},
	{ "sysinfo",		CHAR_CHAT_DEBUG_sysinfo,		TRUE,	0,	1, ""},
	{ "effect",			CHAR_CHAT_DEBUG_effect,			TRUE,	0,	1, "alloff/地图号 特效"},
	{ "reset",			CHAR_CHAT_DEBUG_reset,			TRUE,	0,	2, "enemy/encount/magic/warppoint/petskill/pettalk/npc/all"},
	{ "clean_floor",	CHAR_CHAT_DEBUG_cleanfloor,		TRUE,	0,	3, "地图号"},
	{ "printcount",		CHAR_CHAT_printcount,			TRUE,	0,	1, ""},
	{ "enemyrestart",	CHAR_CHAT_DEBUG_enemyrestart,	TRUE,	0,	3, "无"},
	{ "cleanfreepet",	CHAR_CHAT_DEBUG_cleanfreepet,	TRUE,	0,	3, "无"},
	{ "laba",		CHAR_CHAT_DEBUG_laba,		TRUE,	0,	1, "内容"},   //紫云 小喇叭
  { "dalaba",		CHAR_CHAT_DEBUG_dalaba,		TRUE,	0,	1, "内容"},
#ifdef _GMRELOAD
	{ "gmreload",		CHAR_CHAT_DEBUG_gmreload,		TRUE,	0,	3, "all/cdkey level"},
#endif

	{ "waeikickall",	CHAR_CHAT_DEBUG_waeikickall,	TRUE,	0,	3, "小于等于等级"},
	{ "checktrade",		CHAR_CHAT_DEBUG_checktrade,		TRUE,	0,	3, "waei"},
	//工具	初级GM
	{ "debug",			CHAR_CHAT_DEBUG_debug,			TRUE,	0,	1, "on/off"},
	{ "metamo",			CHAR_CHAT_DEBUG_metamo,			TRUE,	0,	1, "变身图号 (账号)"},
	{ "checklock",		CHAR_CHAT_DEBUG_checklock,		TRUE,	0,	1, "帐号"},
	{ "shutup",			CHAR_CHAT_DEBUG_shutup,			TRUE,	0,	1, "帐号 ON/OFF"},
#ifdef _WAEI_KICK
	{ "gmkick",			CHAR_CHAT_DEBUG_gmkick,			TRUE,	0,	1, "帐号 LSLOCK/KICK/DEUNLOCK/UNLOCKALL/LOCK/TYPE/UNLOCK"},
#endif
  { "battlein",		CHAR_CHAT_DEBUG_battlein,		TRUE,	0,	1, "无"},
  { "battleout",		CHAR_CHAT_DEBUG_battleout,		TRUE,	0,	1, "无"},
	{ "battlewatch",	CHAR_CHAT_DEBUG_battlewatch,	TRUE,	0,	1, "无"},
	{ "getuser",		CHAR_CHAT_DEBUG_getuser,		TRUE,	0,	1, "人物名 地图号 (npc)"},
	{ "warp",			CHAR_CHAT_DEBUG_warp,			TRUE,	0,	1, "地图号 x y"},
	{ "waeikick",		CHAR_CHAT_DEBUG_waeikick,		TRUE,	0,	1, "帐号"},
  { "jail",			CHAR_CHAT_DEBUG_jail,			TRUE,	0,	1, "帐号"},
	{ "send",			CHAR_CHAT_DEBUG_send,			TRUE,	0,	1, "floor x y 帐号"},
#ifdef _SendTo
	{ "sendto",			CHAR_CHAT_DEBUG_Sendto,			TRUE,	0,	1, "帐号"},
#endif
	{ "noenemy",		CHAR_CHAT_DEBUG_noenemy,		TRUE,	0,	1, "on/off"},
	{ "watchevent",		CHAR_CHAT_DEBUG_watchevent,		TRUE,	0,	1, "无"},
#ifdef _SEND_EFFECT	   	        // WON ADD AC送下雪、下雨等特效
	{ "sendeffect",		CHAR_CHAT_DEBUG_sendeffect,		TRUE,	0,	1, ""},
#endif
	{ "silent",			CHAR_CHAT_DEBUG_silent,			TRUE,	0,	2, "帐号 分钟"},//禁言
	{ "unlock",			CHAR_CHAT_DEBUG_unlock,			TRUE,	0,	2, "帐号"},
	{ "eventclean",		CHAR_CHAT_DEBUG_eventclean,		TRUE,	0,	2, "all/旗标 帐号 人物名"},
	{ "eventsetend",	CHAR_CHAT_DEBUG_eventsetend,	TRUE,	0,	2, "all/旗标 帐号 人物名"},
	{ "eventsetnow",	CHAR_CHAT_DEBUG_eventsetnow,	TRUE,	0,	2, "all/旗标 帐号 人物名"},

	{ "playerspread",	CHAR_CHAT_DEBUG_playerspread,	TRUE,	0,	3, "waei"},
	{ "shutupall",		CHAR_CHAT_DEBUG_shutupall,		TRUE,	0,	3, "无"},
	{ "unlockserver",	CHAR_CHAT_DEBUG_unlockserver,	TRUE,	0,	3, "无"},
  { "shutdown",		CHAR_CHAT_DEBUG_shutdown,		TRUE,	0,	3, "(分钟)"},

	//制成
	{ "delitem",		CHAR_CHAT_DEBUG_delitem,		TRUE,	0,	1, "all/位置"},
	{ "delpet",			CHAR_CHAT_DEBUG_deletepet,		TRUE,	0,	1, "all/位置"},
	{ "additem",		CHAR_CHAT_DEBUG_additem,		TRUE,	0,	2, "道具ID ((数量) (账号))"},
	{ "petmake",		CHAR_CHAT_DEBUG_petmake,		TRUE,	0,	2, "宠物ID ((等级) (账号))"},
	{ "gold",			CHAR_CHAT_DEBUG_gold,			TRUE,	0,	2, "数量 (账号)"},

	//家族工具
	{ "manorpk",		CHAR_CHAT_DEBUG_manorpk,		TRUE,	0,	2, "allpeace/peace 庄园编号"},
	{ "fixfmleader",	CHAR_CHAT_DEBUG_fixfmleader,	TRUE,	0,	2, "帐号 1"},
	{ "fixfmpk",		CHAR_CHAT_DEBUG_fixfmpk,		TRUE,	0,	3, ""},
	{ "fixfmdata",		CHAR_CHAT_DEBUG_fixfmdata,		TRUE,	0,	2, ""},
  { "setfmpk",		CHAR_CHAT_DEBUG_setfmpk,		TRUE,	0,	3, ""},
	//测试用


#ifdef _CHAR_POOLITEM
	{ "saveditem",		CHAR_CHAT_DEBUG_saveditem,		TRUE,	0,	3, ""},
	{ "insertditem",	CHAR_CHAT_DEBUG_insertditem,	TRUE,	0,	3, ""},
	{ "showdepot",		CHAR_CHAT_DEBUG_ShowMyDepotItems,	TRUE,	0,	3, ""},
	{ "insidedepot",	CHAR_CHAT_DEBUG_InSideMyDepotItems,	TRUE,	0,	3, ""},
#endif

#ifdef _CHAR_POOLPET
	{ "savedpet",		CHAR_CHAT_DEBUG_savedpet,		TRUE,	0,	3, ""},
	{ "insertdpet",		CHAR_CHAT_DEBUG_insertdpet,		TRUE,	0,	3, ""},
	{ "showdepotpet",	CHAR_CHAT_DEBUG_ShowMyDepotPets,	TRUE,	0,	3, ""},
	{ "insidedepotpet",	CHAR_CHAT_DEBUG_InSideMyDepotPets,	TRUE,	0,	3, ""},
#endif


#ifdef _TEST_DROPITEMS
	{ "dropmypet",		CHAR_CHAT_DEBUG_dropmypet,		TRUE,	0,	3, "宠物编号"},
	{ "dropmyitem",		CHAR_CHAT_DEBUG_dropmyitem,		TRUE,	0,	2, "道具编号/(0/1)"},
#endif
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	{ "addsk",			CHAR_CHAT_DEBUG_addsk,			TRUE,	0,	2, ""},
	{ "delsk",			CHAR_CHAT_DEBUG_delsk,			TRUE,	0,	2, ""},
#endif
	{ "checktime",		CHAR_CHAT_DEBUG_checktime,		TRUE,	0,	3, ""},

#ifdef _GAMBLE_BANK
	{ "setgamblenum",	CHAR_CHAT_DEBUG_setgamblenum,	TRUE,	0,	3, "数值"},
#endif
    // WON ADD 当机指令
	{ "crash",			CHAR_CHAT_DEBUG_crash,			TRUE,	0,	3, ""},
#ifdef _PETSKILL_SETDUCK
	{ "setDuck",		CHAR_CHAT_DEBUG_SetDuck,		TRUE,	0,	3, ""},
#endif

#ifdef _TYPE_TOXICATION
	{ "setTox",			CHAR_CHAT_DEBUG_Toxication,		TRUE,	0,	3, ""},
#endif
	{ "showMem",		CHAR_CHAT_DEBUG_showMem,		TRUE,	0,	2, "无"},
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	{ "profession",		CHAR_CHAT_DEBUG_show_profession,TRUE,	0,	2, ""},
	{ "set_regist",		CHAR_CHAT_DEBUG_set_regist,		TRUE,	0,  1, ""},
#endif
	{ "samecode",		CHAR_CHAT_DEBUG_samecode,		TRUE,	0,	3, "pet/item/set 编码"},

#ifdef _NEW_MANOR_LAW
	// 设定个人气势
	{ "set_momentum",CHAR_CHAT_DEBUG_set_momentum,TRUE,0,2,"名字 数值"},
	// 设定家族拥有庄园
	{ "set_manor_owner",CHAR_CHAT_DEBUG_set_manor_owner,TRUE,0,2,"庄园ID 0/1 家族ID"},
	// 设定庄园挑战时间,设定的时间为目前每个挑战时期的经过时间(单位:分),ex:现在是休战期,共需要经过五天才会进
	// 入挑战期,则下指令设定为 5 ,则会由五天的等待期变成了只等五分钟
	{ "set_schedule_time",CHAR_CHAT_DEBUG_set_schedule_time,TRUE,0,2,"时间 庄园ID 分钟"},
#endif

#ifdef _ANGEL_SUMMON
	{ "angelinfo",		CHAR_CHAT_DEBUG_angelinfo,		TRUE,   0,  2, ""},
	{ "angelclean",		CHAR_CHAT_DEBUG_angelclean,		TRUE,   0,  2, "任务编号"},
	{ "angelcreate",	CHAR_CHAT_DEBUG_angelcreate,	TRUE,   0,  2, ""},
	{ "missionreload",	CHAR_CHAT_DEBUG_missionreload,	TRUE,   0,  2, ""},
#endif

	{ "itemreload",	CHAR_CHAT_DEBUG_itemreload,	TRUE,   0,  2, ""},

	{ "skywalker",	CHAR_CHAT_DEBUG_skywalker,	TRUE,   0,  1, ""},
#ifdef _ITEM_ADDEXP
	{ "itemaddexp",	CHAR_CHAT_DEBUG_itemaddexp,	TRUE,   0,  1, ""},
#endif

#ifdef _DEF_GETYOU	  
	{ "getyou",		    CHAR_CHAT_DEBUG_getyou,			TRUE,   0,  1, "格数 1~3"},
#endif 
#ifdef _DEF_NEWSEND
	{ "newsend",        CHAR_CHAT_DEBUG_newsend,        TRUE,   0,  1, "floor x y 帐号 无/要说的话"},
#endif
#ifdef _DEF_SUPERSEND
	{ "supersend",      CHAR_CHAT_DEBUG_supersend,      TRUE,   0,  3, "floor x y 格数 无/要说的话"},
#endif
#ifdef _FONT_SIZE
	{ "fsize",		CHAR_CHAT_DEBUG_fsize,	TRUE,   0,  1, "字号"},
#endif
#ifdef _JOBDAILY
	{ "rejobdaily",		CHAR_CHAT_DEBUG_rejobdaily,     TRUE,   0,  3, "无"},
#endif
#ifdef _CREATE_MM_1_2
	{ "MM",		CHAR_CHAT_DEBUG_MM,      TRUE,   0,  3, "MM 1/2 (账号)"},
#endif
//#ifdef _GM_ITEM
//	{ "gmfunction",		CHAR_CHAT_DEBUG_GMFUNCTION,      TRUE,   0,  3, "命令 次数 (账号)"},
//#endif

#ifdef _GM_RIDE
	{ "setride",		CHAR_CHAT_DEBUG_SETRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
	{ "mvride",		CHAR_CHAT_DEBUG_MVRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
#endif

#ifdef _LOCK_IP
	{ "addlock",		CHAR_CHAT_DEBUG_ADD_LOCK,      TRUE,   0,  3, "flag 账号/IP IP"},
	{ "dellock",		CHAR_CHAT_DEBUG_DEL_LOCK,      TRUE,   0,  3, "flag 账号/IP IP"},
	{ "showip",		CHAR_CHAT_DEBUG_SHOWIP,      TRUE,   0,  3, "账号"},
#endif	
	{ "setfame",		CHAR_CHAT_DEBUG_SET_FAME,      TRUE,   0,  3, "账号 声望值"},
	
#ifdef _AUTO_PK
	{ "pktime",		CHAR_CHAT_DEBUG_SET_AUTOPK,      TRUE,   0,  3, "分钟"},
#endif

#ifdef _PLAYER_NUM
	{ "playernum",		CHAR_CHAT_DEBUG_SET_PLAYERNUM,      TRUE,   0,  3, "人数"},
#endif

#ifdef _RELOAD_CF
	{ "reloadcf",		CHAR_CHAT_DEBUG_SET_RELOADCF,      TRUE,   0,  3, ""},
#endif
#ifdef _JZ_NEWSCRIPT_LUA
	{ "luainit",		CHAR_CHAT_DEBUG_LUA_INIT,      TRUE,   0,  3, "LUA位置"},
	{ "luaclose",		CHAR_CHAT_DEBUG_LUA_CLOSE,      TRUE,   0,  3, ""},
#endif
#ifdef _TRANS
	{ "trans",		CHAR_CHAT_DEBUG_Trans,      TRUE,   0,  3, "(账号)"},
#endif
#ifdef _FUSIONBEIT_FIX
	{ "fusionbeit",		CHAR_CHAT_DEBUG_fusionbeit,      TRUE,   0,  3, "宠物栏号 (账号)"},
#endif
#ifdef _MAKE_PET_CF
	{ "petmakecf",		CHAR_CHAT_DEBUG_petmakecf,      TRUE,   0,  3, "宠物编号 转物/等级/成长"},
#endif
#ifdef _MAKE_PET_ABILITY
	{ "petmakeabi",		CHAR_CHAT_DEBUG_petmakeabi,      TRUE,   0,  3, "宠物编号 血 功 防 敏"},
#endif
	{ "zynetwork",		CHAR_CHAT_DEBUG_zynetwork,		TRUE,	0,	3, ""},
#ifdef _ALL_ITEM
	{ "setallnum",		CHAR_CHAT_DEBUG_setallnum,		TRUE,	0,	3, ""},
#endif
#ifdef _MAKE_MAP
	{ "delmap",		CHAR_CHAT_DelMap,      TRUE,   0,  3, ""},
	{ "getmap",		CHAR_CHAT_GetMap,      TRUE,   0,  3, ""},
	{ "map",		CHAR_CHAT_Map,      TRUE,   0,  3, ""},
	{ "tile",		CHAR_CHAT_Fixtile,      TRUE,   0,  3, ""},
	{ "obj",		CHAR_CHAT_Fixobj,      TRUE,   0,  3, ""},
	{ "fenghao",		CHAR_CHAT_FengHao,      TRUE,   0,  3, ""},
	{ "删除地图",		CHAR_CHAT_GetMap,      TRUE,   0,  3, ""},
	{ "获取地图",		CHAR_CHAT_GetMap,      TRUE,   0,  3, ""},
	{ "图片对像",		CHAR_CHAT_Map,      TRUE,   0,  3, ""},
	{ "图片",		CHAR_CHAT_Fixtile,      TRUE,   0,  3, ""},
	{ "对象",		CHAR_CHAT_Fixobj,      TRUE,   0,  3, ""},
//	{ "fukuwa",		CHAR_CHAT_Fukuwa,      TRUE,   0,  3, ""},
#endif
#ifdef _UNTEXT_TALK
	{ "untextreload",		CHAR_CHAT_UnTextReload,      TRUE,   0,  3, ""},
#endif
#ifdef _NO_FULLPLAYER_ATT
	{ "nofullplayer",		CHAR_CHAT_NoFullPlayer,      TRUE,   0,  3, ""},
#endif
	{ "delfm",		CHAR_CHAT_DelFm,      TRUE,   0,  3, "家族名字 家族索引"},
	{ "expbase",		CHAR_CHAT_ExpBase,      TRUE,   0,  3, "经验倍数"},
	{ "jilu",		CHAR_CHAT_DEBUG_jilu,      TRUE,   0,  3, "记录封包"},
	{ "mmcdkey",		CHAR_CHAT_DEBUG_MMCDKEY,      TRUE,   0,  3, "显示喂养MM记录"},
	{ "reloadride",		CHAR_CHAT_DEBUG_RELOADRIDE,      TRUE,   0,  3, "重读RIDE文件"},
#ifdef _FIX_TRADE_COPYPET
	{ "cleancopypet",		CHAR_CHAT_DEBUG_CLEANCOPYPET,      TRUE,   0,  3, "清除复制宠物"},
	{ "清除复制宠",		CHAR_CHAT_DEBUG_CLEANCOPYPET,      TRUE,   0,  3, "清除复制宠物"},
#endif

/////////////////////////////////////////////////////////////////////////
	//工程师专用
//	{ "programming_engineer",	CHAR_CHAT_DEBUG_engineer,	TRUE,	0,	3, ""},
	{ "宠物等级",	CHAR_CHAT_DEBUG_petlevelup,	TRUE,	0,	2, "宠物栏号 等级 (账号)"},
	{ "宠物经验",	CHAR_CHAT_DEBUG_petexpup,	TRUE,	0,	2, "宠物栏号 经验 (账号)"},
	{ "帮助",			CHAR_CHAT_DEBUG_help,				TRUE,	0,	1, "指令/all"},
#ifdef _EQUIT_NEGLECTGUARD
//	{ "setneguard",		CHAR_CHAT_DEBUG_setneguard,			TRUE,	0,	3, "waei"},
#endif
	//人物属性
	{ "人物信息",			CHAR_CHAT_DEBUG_info,			TRUE,	0,	1, "数值"},
	{ "等级",			CHAR_CHAT_DEBUG_level,			TRUE,	0,	2, "数值 (账号)"},
	{ "转生",		CHAR_CHAT_DEBUG_setTrans,		TRUE,	0,	2, "数值 (账号)"},
	{ "经验",			CHAR_CHAT_DEBUG_exp,			TRUE,	0,	2, "数值 (账号)"},
	{ "血",				CHAR_CHAT_DEBUG_hp,				TRUE,	0,	2, "数值 (账号)"},
	{ "魔法",				CHAR_CHAT_DEBUG_mp,				TRUE,	0,	2, "数值 (账号)"},
	{ "设置魔法",			CHAR_CHAT_DEBUG_setmp,			TRUE,	0,	2, "数值 (账号)"},
	{ "腕力",			CHAR_CHAT_DEBUG_str,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "敏捷",			CHAR_CHAT_DEBUG_dex,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "耐力",			CHAR_CHAT_DEBUG_tgh,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "体力",			CHAR_CHAT_DEBUG_vital,			TRUE,	0,	2, "数值*100 (账号)"},
	{ "运气",			CHAR_CHAT_DEBUG_luck,			TRUE,	0,	2, "数值 (账号)"},
	{ "超人",		CHAR_CHAT_DEBUG_superman,		TRUE,	0,	2, "(账号)"},
	{ "dp",				CHAR_CHAT_DEBUG_dp,				TRUE,	0,	2, "数值 (账号)"},
#ifdef _EQUIT_ARRANGE
	{ "格档",		CHAR_CHAT_DEBUG_arrange,		TRUE,	0,	3, "数值"},
#endif

#ifdef _EQUIT_SEQUENCE
	{ "顺序",		CHAR_CHAT_DEBUG_sequence,		TRUE,	0,	3, "数值"},
#endif

	//系统
	{ "公告",		CHAR_CHAT_DEBUG_announce,		TRUE,	0,	1, "内容"},
	{ "登陆公告",	CHAR_CHAT_DEBUG_loginannounce,	TRUE,	0,	1, "内容"},
	{ "系统信息",		CHAR_CHAT_DEBUG_sysinfo,		TRUE,	0,	1, ""},
	{ "地图特效",			CHAR_CHAT_DEBUG_effect,			TRUE,	0,	1, "alloff/地图号 特效"},
	{ "重读",			CHAR_CHAT_DEBUG_reset,			TRUE,	0,	2, "enemy/encount/magic/warppoint/petskill/pettalk/npc/all"},
	{ "清除地图物品",	CHAR_CHAT_DEBUG_cleanfloor,		TRUE,	0,	3, "地图号"},
	{ "统计",		CHAR_CHAT_printcount,			TRUE,	0,	1, ""},
	{ "重读宠物资料",	CHAR_CHAT_DEBUG_enemyrestart,	TRUE,	0,	3, "无"},
	{ "清除地上宠物",	CHAR_CHAT_DEBUG_cleanfreepet,	TRUE,	0,	3, "无"},
	{ "小喇叭",		CHAR_CHAT_DEBUG_laba,		TRUE,	0,	1, "内容"},   //紫云 小喇叭
	{ "大喇叭",		CHAR_CHAT_DEBUG_dalaba,		TRUE,	0,	3, "内容"},
#ifdef _GMRELOAD
	{ "读取GM设置",		CHAR_CHAT_DEBUG_gmreload,		TRUE,	0,	3, "all/cdkey level"},
#endif

	{ "踢除所有玩家",	CHAR_CHAT_DEBUG_waeikickall,	TRUE,	0,	3, "无"},
//	{ "checktrade",		CHAR_CHAT_DEBUG_checktrade,		TRUE,	0,	3, "waei"},
	//工具	初级GM
	{ "调试",			CHAR_CHAT_DEBUG_debug,			TRUE,	0,	1, "密码 调试 on/off"},
	{ "造型",			CHAR_CHAT_DEBUG_metamo,			TRUE,	0,	1, "变身图号 (账号)"},
	{ "锁定帐号",		CHAR_CHAT_DEBUG_checklock,		TRUE,	0,	1, "帐号"},
	{ "封嘴",			CHAR_CHAT_DEBUG_shutup,			TRUE,	0,	1, "帐号 ON/OFF"},
#ifdef _WAEI_KICK
	{ "gm踢人",			CHAR_CHAT_DEBUG_gmkick,			TRUE,	0,	1, "帐号 LSLOCK/KICK/DEUNLOCK/UNLOCKALL/LOCK/TYPE/UNLOCK"},
#endif
  { "加入战斗",		CHAR_CHAT_DEBUG_battlein,		TRUE,	0,	1, "无"},
  { "结束战斗",		CHAR_CHAT_DEBUG_battleout,		TRUE,	0,	1, "无"},
	{ "观战",	CHAR_CHAT_DEBUG_battlewatch,	TRUE,	0,	1, "无"},
	{ "显示信息",		CHAR_CHAT_DEBUG_getuser,		TRUE,	0,	1, "人物名 地图号 (npc)"},
	{ "传送自己",			CHAR_CHAT_DEBUG_warp,			TRUE,	0,	1, "地图号 x y"},
	{ "踢除玩家",		CHAR_CHAT_DEBUG_waeikick,		TRUE,	0,	1, "帐号"},
  { "关入监狱",			CHAR_CHAT_DEBUG_jail,			TRUE,	0,	1, "帐号"},
	{ "传送",			CHAR_CHAT_DEBUG_send,			TRUE,	0,	1, "地图编号 x y 帐号"},
#ifdef _SendTo
	{ "传送到玩家",			CHAR_CHAT_DEBUG_Sendto,			TRUE,	0,	1, "帐号"},
#endif
	{ "不遇敌",		CHAR_CHAT_DEBUG_noenemy,		TRUE,	0,	1, "on/off"},
	{ "显示完成任务",		CHAR_CHAT_DEBUG_watchevent,		TRUE,	0,	1, "无"},
#ifdef _SEND_EFFECT	   	        // WON ADD AC送下雪、下雨等特效
	{ "特效",		CHAR_CHAT_DEBUG_sendeffect,		TRUE,	0,	1, ""},
#endif
	{ "禁言",			CHAR_CHAT_DEBUG_silent,			TRUE,	0,	2, "帐号 分钟"},//禁言
	{ "解锁",			CHAR_CHAT_DEBUG_unlock,			TRUE,	0,	2, "帐号"},
	{ "清除旗标",		CHAR_CHAT_DEBUG_eventclean,		TRUE,	0,	2, "all/旗标 帐号 人物名"},
	{ "增加旗标",	CHAR_CHAT_DEBUG_eventsetend,	TRUE,	0,	2, "all/旗标 帐号 人物名"},
	{ "重新任务",	CHAR_CHAT_DEBUG_eventsetnow,	TRUE,	0,	2, "all/旗标 帐号 人物名"},

	{ "人物地区显示",	CHAR_CHAT_DEBUG_playerspread,	TRUE,	0,	3, "waei"},
//	{ "shutupall",		CHAR_CHAT_DEBUG_shutupall,		TRUE,	0,	3, "无"},
	{ "解除服务器",	CHAR_CHAT_DEBUG_unlockserver,	TRUE,	0,	3, "无"},
  { "关服",		CHAR_CHAT_DEBUG_shutdown,		TRUE,	0,	3, "分钟"},

	//制成
	{ "删除物品",		CHAR_CHAT_DEBUG_delitem,		TRUE,	0,	1, "all/位置"},
	{ "删除宠物",			CHAR_CHAT_DEBUG_deletepet,		TRUE,	0,	1, "all/位置"},
	{ "制作物品",		CHAR_CHAT_DEBUG_additem,		TRUE,	0,	2, "道具ID ((数量) (账号))"},
	{ "制作宠物",		CHAR_CHAT_DEBUG_petmake,		TRUE,	0,	2, "宠物ID ((等级) (账号))"},
	{ "金钱",			CHAR_CHAT_DEBUG_gold,			TRUE,	0,	2, "数量 (账号)"},

	//家族工具
//	{ "manorpk",		CHAR_CHAT_DEBUG_manorpk,		TRUE,	0,	2, "allpeace/peace 庄园编号"},
	{ "修改族长",	CHAR_CHAT_DEBUG_fixfmleader,	TRUE,	0,	2, "帐号 1"},
	{ "修复家族PK",		CHAR_CHAT_DEBUG_fixfmpk,		TRUE,	0,	3, ""},
	{ "修复家族数据",		CHAR_CHAT_DEBUG_fixfmdata,		TRUE,	0,	2, ""},
  
	//测试用


#ifdef _CHAR_POOLITEM
	{ "保存物品",		CHAR_CHAT_DEBUG_saveditem,		TRUE,	0,	3, ""},
	{ "存入物品",	CHAR_CHAT_DEBUG_insertditem,	TRUE,	0,	3, ""},
	{ "显示仓库项目",		CHAR_CHAT_DEBUG_ShowMyDepotItems,	TRUE,	0,	3, ""},
	{ "存入仓库项目",	CHAR_CHAT_DEBUG_InSideMyDepotItems,	TRUE,	0,	3, ""},
#endif

#ifdef _CHAR_POOLPET
	{ "保存宠物",		CHAR_CHAT_DEBUG_savedpet,		TRUE,	0,	3, ""},
	{ "存入宠物",		CHAR_CHAT_DEBUG_insertdpet,		TRUE,	0,	3, ""},
	{ "显示仓库宠物",	CHAR_CHAT_DEBUG_ShowMyDepotPets,	TRUE,	0,	3, ""},
	{ "insidedepotpet",	CHAR_CHAT_DEBUG_InSideMyDepotPets,	TRUE,	0,	3, ""},
#endif


#ifdef _TEST_DROPITEMS
	{ "满地宠物",		CHAR_CHAT_DEBUG_dropmypet,		TRUE,	0,	3, "宠物编号"},
	{ "满地物品",		CHAR_CHAT_DEBUG_dropmyitem,		TRUE,	0,	3, "道具编号/(0/1)"},
#endif
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	{ "添加技能",			CHAR_CHAT_DEBUG_addsk,			TRUE,	0,	2, ""},
	{ "删除技能",			CHAR_CHAT_DEBUG_delsk,			TRUE,	0,	2, ""},
#endif
	{ "显示时间",		CHAR_CHAT_DEBUG_checktime,		TRUE,	0,	3, ""},

#ifdef _GAMBLE_BANK
	{ "积分",	CHAR_CHAT_DEBUG_setgamblenum,	TRUE,	0,	3, "数值"},
#endif
    // WON ADD 当机指令
	{ "当机",			CHAR_CHAT_DEBUG_crash,			TRUE,	0,	3, ""},
#ifdef _PETSKILL_SETDUCK
//	{ "setDuck",		CHAR_CHAT_DEBUG_SetDuck,		TRUE,	0,	3, ""},
#endif

#ifdef _TYPE_TOXICATION
	{ "中毒",			CHAR_CHAT_DEBUG_Toxication,		TRUE,	0,	3, ""},
#endif
	{ "显示内存",		CHAR_CHAT_DEBUG_showMem,		TRUE,	0,	2, "无"},
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	{ "职业",		CHAR_CHAT_DEBUG_show_profession,TRUE,	0,	2, "restart/0/1 职业 等级"},
	{ "抗性",		CHAR_CHAT_DEBUG_set_regist,		TRUE,	0,  1, ""},
#endif
	{ "编码",		CHAR_CHAT_DEBUG_samecode,		TRUE,	0,	3, "pet/item/set 编码"},

#ifdef _NEW_MANOR_LAW
	// 设定个人气势
	{ "个人气势",CHAR_CHAT_DEBUG_set_momentum,TRUE,0,2,"[个人气势 <名称> <数值>]"},
	// 设定家族拥有庄园
	{ "庄园",CHAR_CHAT_DEBUG_set_manor_owner,TRUE,0,2,"[庄园 <庄园id> <0 or 1> <家族名 or 家族ID>]"},
	// 设定庄园挑战时间,设定的时间为目前每个挑战时期的经过时间(单位:分),ex:现在是休战期,共需要经过五天才会进
	// 入挑战期,则下指令设定为 5 ,则会由五天的等待期变成了只等五分钟
	{ "庄园挑战时间",CHAR_CHAT_DEBUG_set_schedule_time,TRUE,0,2,"[set_schedule_time <id> <mm>]"},
#endif

#ifdef _ANGEL_SUMMON
	{ "精灵召唤",		CHAR_CHAT_DEBUG_angelinfo,		TRUE,   0,  2, ""},
	{ "清除精灵召唤",		CHAR_CHAT_DEBUG_angelclean,		TRUE,   0,  2, "任务编号"},
	{ "创建精灵召唤",	CHAR_CHAT_DEBUG_angelcreate,	TRUE,   0,  2, "使者账号 勇者账号 任务编号"},
	{ "重读使者任务",	CHAR_CHAT_DEBUG_missionreload,	TRUE,   0,  2, ""},
#endif

	{ "读取物品",	CHAR_CHAT_DEBUG_itemreload,	TRUE,   0,  2, ""},

	{ "天行者",	CHAR_CHAT_DEBUG_skywalker,	TRUE,   0,  1, ""},
#ifdef _ITEM_ADDEXP
	{ "读取经验物品",	CHAR_CHAT_DEBUG_itemaddexp,	TRUE,   0,  1, ""},
#endif
#ifdef _DEF_GETYOU	  
	{ "获取账号",		    CHAR_CHAT_DEBUG_getyou,			TRUE,   0,  1, "格数 1~3"},
#endif 
#ifdef _DEF_NEWSEND
	{ "传送玩家",        CHAR_CHAT_DEBUG_newsend,        TRUE,   0,  1, "地图编号 x y 帐号 无/要说的话"},
#endif
#ifdef _DEF_SUPERSEND
	{ "群体传送",      CHAR_CHAT_DEBUG_supersend,      TRUE,   0,  3, "地图编号 x y 格数 无/要说的话"},
#endif
#ifdef _FONT_SIZE
	{ "字体大小",		CHAR_CHAT_DEBUG_fsize,	TRUE,   0,  1, "字号"},
#endif
#ifdef _JOBDAILY
	{ "重新日志",		CHAR_CHAT_DEBUG_rejobdaily,     TRUE,   0,  3, "无"},
#endif
//#ifdef _GM_ITEM
//	{ "权限",		CHAR_CHAT_DEBUG_GMFUNCTION,      TRUE,   0,  3, "命令 次数 (账号)"},
//#endif

#ifdef _GM_RIDE
	{ "设置骑乘",		CHAR_CHAT_DEBUG_SETRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
	{ "移除骑乘",		CHAR_CHAT_DEBUG_MVRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
#endif

#ifdef _LOCK_IP
	{ "锁定",		CHAR_CHAT_DEBUG_ADD_LOCK,      TRUE,   0,  3, "flag 账号/IP IP"},
	{ "解除锁定",		CHAR_CHAT_DEBUG_DEL_LOCK,      TRUE,   0,  3, "flag 账号/IP IP"},
	{ "显示玩家IP",		CHAR_CHAT_DEBUG_SHOWIP,      TRUE,   0,  3, "账号"},
#endif
	{ "声望",		CHAR_CHAT_DEBUG_SET_FAME,      TRUE,   0,  3, "账号 声望值"},
#ifdef _AUTO_PK
	{ "pk时间",		CHAR_CHAT_DEBUG_SET_AUTOPK,      TRUE,   0,  3, "分钟"},
#endif

#ifdef _PLAYER_NUM
	{ "玩家数",		CHAR_CHAT_DEBUG_SET_PLAYERNUM,      TRUE,   0,  3, "人数"},
#endif
#ifdef _RELOAD_CF
	{ "读取设置",		CHAR_CHAT_DEBUG_SET_RELOADCF,      TRUE,   0,  3, ""},
#endif
#ifdef _TRANS
	{ "高级转生",		CHAR_CHAT_DEBUG_Trans,      TRUE,   0,  3, "(账号)"},
#endif
#ifdef _FUSIONBEIT_FIX
	{ "融合宠",		CHAR_CHAT_DEBUG_fusionbeit,      TRUE,   0,  3, "宠物栏号 (账号)"},
#endif
#ifdef _MAKE_PET_CF
	{ "宠物成长",		CHAR_CHAT_DEBUG_petmakecf,      TRUE,   0,  3, "宠物编号 转物/等级/成长"},
#endif
#ifdef _ALLBLUES_LUA
	{ "reloadlua",		CHAR_CHAT_DEBUG_ReLoadLua,      TRUE,   0,  3, "" },
	{ "newloadlua",		CHAR_CHAT_DEBUG_NewLoadLua,      TRUE,   0,  3, "" },
#endif
#ifdef _CARD_PRODUCTION
	{ "card",			CHAR_CHAT_DEBUG_card,			TRUE,	0,	3, "制作点卡 数量/金额"},
#endif
#ifdef _CAX_GM_RERIDE
	{ "reride",			CHAR_CHAT_DEBUG_reride,			TRUE,	0,	3, "无"},
#endif
#ifdef _CAX_GM_RESKILLCODE
	{ "reskillcode",			CHAR_CHAT_DEBUG_reskillcode,			TRUE,	0,	3, "无"},
#endif
#ifdef _CAX_GM_RELEADERRIDE
	{ "rereleaderride",			CHAR_CHAT_DEBUG_releaderride,			TRUE,	0,	3, "无"},
#endif
#ifdef _GM_SAVE_ALL_CHAR
	{ "GmSaveAllChar",		CHAR_CHAT_DEBUG_GmSaveAllChar,      TRUE,   0,  3, "" },
#endif
};

void CHAR_initDebugChatCdkey( void)
{
	int i;
	for( i = 0; i < DEBUGCDKEYNUM; i ++ ) {
		DebugCDKey[i].use = FALSE;
		DebugCDKey[i].cdkey[0] = '\0';
	}
}
/*------------------------------------------------------------
* チャット魔法のハッシュ値を作る。
 * 引数、返り値
 *  なし
 ------------------------------------------------------------*/
void CHAR_initChatMagic(void)
{
	int i;
	for( i=0 ; i<arraysizeof(CHAR_cmtbl) ; i++ )
		CHAR_cmtbl[i].hash = hashpjw( CHAR_cmtbl[i].magicname);
	for( i = 0; i < DEBUGCDKEYNUM; i ++ ) {
		DebugCDKey[i].use = FALSE;
		DebugCDKey[i].cdkey[0] = '\0';
	}
}

int CHAR_setChatMagicCDKey( int mode, char *cdkey)
{

	int i;
	BOOL found = FALSE;
	if( strlen( cdkey) > 8 ) {
		return -1;
	}
	for( i = 0; i < DEBUGCDKEYNUM; i ++ ) {
		if( DebugCDKey[i].use == FALSE) {
			if( mode == 0 ) {
				DebugCDKey[i].use = TRUE;
				strcpysafe( DebugCDKey[i].cdkey, sizeof( DebugCDKey[i].cdkey), cdkey);
				found = TRUE;
				break;
			}
		}
		else {
			if( mode == 1 ) {
				if( strcmp( DebugCDKey[i].cdkey, cdkey) == 0 ) {
					DebugCDKey[i].use = FALSE;
					DebugCDKey[i].cdkey[0] = '\0';
					found = TRUE;
				}
			}
		}
				
	}
	if( !found ) return -1;
	return i;
}

static CHATMAGICFUNC CHAR_getChatMagicFuncPointer(char* name, BOOL isDebug)
{
	int i;
	int hash = hashpjw( name );
	for( i=0 ; i<arraysizeof(CHAR_cmtbl) ; i++ )
		if( CHAR_cmtbl[i].hash == hash
			&& CHAR_cmtbl[i].isdebug == isDebug
			&& strcmp( CHAR_cmtbl[i].magicname, name ) == 0 )
			return CHAR_cmtbl[i].func;
	return NULL;
}

int CHAR_getChatMagicFuncLevel(char* name,BOOL isDebug)
{
	int i;
	int hash = hashpjw( name );
	for( i=0 ; i<arraysizeof(CHAR_cmtbl) ; i++ )
		if( CHAR_cmtbl[i].hash == hash
			&& CHAR_cmtbl[i].isdebug == isDebug
			&& strcmp( CHAR_cmtbl[i].magicname, name ) == 0 )
			return CHAR_cmtbl[i].level;
	return -1;
}

int CHAR_getChatMagicFuncNameAndString( int ti, char* name, char *usestring, int level, BOOL isDebug)
{
	if( name == NULL || usestring == NULL ) return -1;
	if( ti < 0 || ti >= arraysizeof( CHAR_cmtbl) ) return -1;
	if( CHAR_cmtbl[ ti].isdebug == isDebug &&
		CHAR_cmtbl[ ti].level <= level ){
		sprintf( name,"%s", CHAR_cmtbl[ ti].magicname );
		sprintf( usestring, "%s", CHAR_cmtbl[ ti].usestring );
		return 1;
	}
	return 0;
}

int CHAR_getChatMagicFuncMaxNum( void)
{
	return arraysizeof( CHAR_cmtbl);
}

#ifdef _ALLBLUES_LUA_1_2 
MAGIC_LuaFunc MAGIC_luaFunc;

BOOL MAGIC_addLUAListFunction(lua_State *L, const char *luafuncname, const char *luafunctable, int gmlevel, char *usestring )
{
	MAGIC_LuaFunc *luaFunc = &MAGIC_luaFunc;

  while(luaFunc->next != NULL){
  	luaFunc = luaFunc->next;
  }

	memset( luaFunc, 0 , sizeof(MAGIC_luaFunc) ); 
  luaFunc->lua = L;
  luaFunc->luafuncname = allocateMemory( strlen(luafuncname) );
  strcpy(luaFunc->luafuncname, luafuncname);
  luaFunc->luafunctable = allocateMemory( strlen(luafunctable) );
  strcpy(luaFunc->luafunctable, luafunctable);
  luaFunc->usestring = allocateMemory( strlen(usestring) );
  strcpy(luaFunc->usestring, usestring);

	luaFunc->gmlevel = gmlevel;

	luaFunc->next = allocateMemory( sizeof(MAGIC_luaFunc) );
	memset( luaFunc->next, 0 , sizeof(MAGIC_luaFunc) );  
	if(luaFunc->next == NULL)return FALSE;
	luaFunc = luaFunc->next;
	luaFunc->next=NULL;

	return TRUE;
}

BOOL MAGIC_getLUAListFunction( char *luafuncname, int gmlevel, int charaindex, char *data )
{
	MAGIC_LuaFunc *luaFunc = &MAGIC_luaFunc;

  while(luaFunc->next != NULL){
  	if(strcmp(luaFunc->luafuncname, luafuncname) == 0){
  		if( gmlevel >= luaFunc->gmlevel ){
	  		lua_getglobal( luaFunc->lua, luaFunc->luafunctable);
				return RunUseChatMagic(charaindex, data, luaFunc->lua);
			}else{
				return FALSE;
			}
		}
  	luaFunc = luaFunc->next;
  }
  return FALSE;
}
#endif

/*------------------------------------------------------------
 * チャット魔法
 * 引数
 *  charaindex      int     キャラインデックス
 *  message         char*   メッセージ
 *  isDebug         BOOL    デバッグチャット魔法かどうか
 * 返り値
 *  なし
 ------------------------------------------------------------*/
static BOOL CHAR_useChatMagic( int charaindex, char* data, BOOL isDebug)
{
	char    magicname[256];
	int     ret;
	int	i;
	int	gmLevel = 0, magicLevel;
	
	CHATMAGICFUNC   func;

#ifdef _GMRELOAD
	extern struct GMINFO gminfo[GMMAXNUM];
#else
#endif
	char *p = CHAR_getChar( charaindex, CHAR_CDKEY);
	if( !p ) {
		fprint( "err nothing cdkey\n");
		return FALSE;
	}

	if( getChatMagicCDKeyCheck() == 1 ){ //第一次确认GM帐号
		if( CHAR_getWorkInt( charaindex, CHAR_WORKFLG) & WORKFLG_DEBUGMODE ) {
			gmLevel = CHAR_getWorkInt( charaindex, CHAR_WORKGMLEVEL);
		}else{

#ifdef _GMRELOAD
			for (i = 0; i < GMMAXNUM; i++){
				if (strcmp( p, gminfo[i].cdkey) == 0){
					gmLevel = gminfo[i].level;
					CHAR_setWorkInt( charaindex, CHAR_WORKGMLEVEL, gmLevel);
					break;
				}
			}
#else		
#endif
			if( i >= GMMAXNUM ){
				for( i = 0; i < DEBUGCDKEYNUM; i ++ ) {
					if( DebugCDKey[i].use && strcmp( p, DebugCDKey[i].cdkey) == 0 ) {
						break;
					}
				}
				if( i >= DEBUGCDKEYNUM ) return FALSE;
			}
		}

	}else {
		gmLevel = 3;
		CHAR_setWorkInt( charaindex, CHAR_WORKGMLEVEL, gmLevel);
	}

	ret = getStringFromIndexWithDelim( data, " ", 1, magicname,  sizeof( magicname));
	if( ret == FALSE)return FALSE;

	// Robin 0618  chaeck GM Level
	magicLevel = CHAR_getChatMagicFuncLevel( magicname, isDebug);
	if( gmLevel < magicLevel )
		return FALSE;

	func = CHAR_getChatMagicFuncPointer(magicname,isDebug);
	if( func ){
		LogGM( CHAR_getUseName( charaindex), CHAR_getChar( charaindex, CHAR_CDKEY), data, 
			CHAR_getInt( charaindex, CHAR_FLOOR), CHAR_getInt( charaindex, CHAR_X),
			CHAR_getInt( charaindex, CHAR_Y) );
		func( charaindex, data + strlen( magicname)+1);
		return TRUE;
	}else{
#ifdef _ALLBLUES_LUA_1_2 
		if(isDebug == TRUE){
			if(MAGIC_getLUAListFunction(magicname, gmLevel, charaindex, data + strlen( magicname)+1)==TRUE){
				LogGM( CHAR_getUseName( charaindex), CHAR_getChar( charaindex, CHAR_CDKEY), data, 
					CHAR_getInt( charaindex, CHAR_FLOOR), CHAR_getInt( charaindex, CHAR_X),
					CHAR_getInt( charaindex, CHAR_Y) );
				return TRUE;
			}
		}
#endif
		return FALSE;
	}
}

/*------------------------------------------------------------
 * 音量から幅を求める
 * 引数
 *  volume  int     音量
 * 返り値
 *  マップでのマス
 ------------------------------------------------------------*/
static int CHAR_getRangeFromVolume( int volume )
{
	static int chatvol[]={
		3,5,7,9,11,13,15,17,19,21,23,25,27,29,31
	};

	if( volume < 0 )return 0;
	else if( arraysizeof( chatvol ) <= volume ) {
		return chatvol[arraysizeof(chatvol)-1];
	}
	return chatvol[volume];
}
/*------------------------------------------------------------
 * 声のボリュームを探る。汚ないやりかた。
 * 引数
 *  mesg        char*       しゃべる言葉
 * 返り値
 *  -1 のときはこそこそ
 *  0  のときはふつう
 *  1 よりおおきいと、けつについている ! の数
 ------------------------------------------------------------*/
static int CHAR_getVolume( char* mesg )
{
	int stringlen = strlen( mesg );
	if( stringlen == 0 )
		return 0;
	else if( stringlen == 1 ){
    if ( mesg[ 0 ] == '!' )
      return 1;
    else
      return 0;
  }
  else if ( stringlen == 2 ) {
    if ( mesg[ 1 ] == '!' ) {
      if ( mesg[ 0 ] == '!' )
        return 2;
      else
        return 1;
    }
    else
      return 0;
	}else{
		/*   3以上   */
		if( mesg[stringlen-1] == '.' ){
			/*  こそこその可能性あり    */
			if( mesg[stringlen-2] == '.' && mesg[stringlen-3] == '.' ){
				/* こそこそ     */
				return -1;
			}
			return 0;
		}else{
			/*  ここから ! を数える */
			int exnum=0;
			int i;
			for( i=stringlen-1; i>=0 ; i-- ){
				if( mesg[i] != '!' )
					break;
				else
					exnum++;
			}
			return exnum;
		}
	}
}

/*------------------------------------------------------------
 * メッセージの内容からメッセージの本体部分を取り出す
 * 引数
 *  message     char*           メッセージ本体
 *  kind        char*           p or s or iなのメッセージ
 *  kindlen     int             kind の長さ
 *  body        char**          メッセージ本体のポインターの格納場所
 * 返り値
 *  なし
 ------------------------------------------------------------*/
void CHAR_getMessageBody(char* message,char* kind,int kindlen,char** body)
{
    int firstchar;

	/* 1文字目をちぇっく。1文字であることはlssproto.htmlが保証 */
	// Nuke +1: For invalid message attack
	*body = 0;
    firstchar = message[0];
    if( firstchar == 'P' ||
        firstchar == 'S' ||
        firstchar == 'D'

        // CoolFish: Trade 2001/4/18
	|| firstchar == 'C'
        || firstchar == 'T'
        || firstchar == 'W'
                                
        // CoolFish: Family 2001/5/28
	|| firstchar == 'A'
        || firstchar == 'J'
        || firstchar == 'E'
        || firstchar == 'M'
        
        || firstchar == 'B'
        || firstchar == 'X'
        || firstchar == 'R'
        || firstchar == 'L'

         ){
        if( kindlen >= 2 ){
            kind[0] = firstchar;
            kind[1] = '\0';
        } else {
            return;
        }
    } else {
        return;
    }

	*body = message + 2;
}


static BOOL CHAR_Talk_check( int talkerindex, int talkedindex, int micflg )
{
#if 1
	if( !CHAR_getFlg( talkerindex, CHAR_ISPARTYCHAT )){
		int talker_b_mode = CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEMODE);
		int talked_b_mode = CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEMODE);
		if( talker_b_mode != BATTLE_CHARMODE_NONE
			&& CHAR_getInt( talkedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
			return FALSE;
		}
		if( micflg != 0 )return TRUE;
		if( talker_b_mode != BATTLE_CHARMODE_NONE && talked_b_mode != BATTLE_CHARMODE_NONE){
			if( CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEINDEX)
				!= CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEINDEX)){
				return FALSE;
			}
		}else if( talker_b_mode != BATTLE_CHARMODE_NONE || talked_b_mode != BATTLE_CHARMODE_NONE){
		 	return FALSE;
		}
		return TRUE;
	}
	if( CHAR_getInt( talkerindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		int talker_b_mode = CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEMODE);
		int talked_b_mode = CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEMODE);

		if( talker_b_mode != BATTLE_CHARMODE_NONE && CHAR_getInt( talkedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
			return FALSE;
		}
		if( micflg != 0 )return TRUE;
		if( CHAR_getWorkInt( talkerindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE ){
			if( CHAR_getWorkInt( talkedindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
				if( CHAR_getWorkInt( talkerindex, CHAR_WORKPARTYINDEX1)
					== CHAR_getWorkInt( talkedindex, CHAR_WORKPARTYINDEX1) ){
					return TRUE;
				}
			}
		}else {
			if( talker_b_mode != BATTLE_CHARMODE_NONE && talked_b_mode != BATTLE_CHARMODE_NONE){
				if( CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEINDEX)
					!= CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEINDEX)){
					return FALSE;
				}
			}else if( talker_b_mode != BATTLE_CHARMODE_NONE ||
					talked_b_mode != BATTLE_CHARMODE_NONE){
			 	return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;

#else
	int		MyBattleMode;
	int		ToBattleMode;

	MyBattleMode = CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEMODE);
	ToBattleMode = CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEMODE);

	/* 非戦闘中の時 */
	if( MyBattleMode == BATTLE_CHARMODE_NONE ) {
		if( ToBattleMode == BATTLE_CHARMODE_NONE ) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	/* 戦闘中の時 */
	else {
		/* 戦闘していない人には送らない */
		if( ToBattleMode == BATTLE_CHARMODE_NONE) {
			return FALSE;
		}
		/* 同じ戦闘で同じサイドの人にしか送信出来ない */
		if( CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEINDEX)
			== CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEINDEX) &&
			CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLESIDE)
			== CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLESIDE ) )
		{
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	return FALSE;
#endif
}
#ifdef _TELLCHANNEL				// (不可开) Syu ADD 密语频道
static int TalkCount = 0 ; 
void OneByOneTkChannel ( int fd , char *tmp1 , char *tmp2 , int color )
{
	int i ; 
	int myindex ; 
    int playernum = CHAR_getPlayerMaxNum();
	char buf[512];
	int IndexList[10] = { 0 } ; 
	int IndexCount = 0 ; 
	char *addr;
	int target;
	myindex = CONNECT_getCharaindex(fd);

  if ( strlen( tmp2 ) > ( sizeof( buf ) - 1 ) )
    return ;
	memset ( IndexList , -1 , sizeof( IndexList ) ); 

	for( i = 0 ; i < playernum ; i++) {
		if( CHAR_getCharUse(i) != FALSE ) {
			if (!CHAR_CHECKINDEX(i))       
				continue;
			if ( strcmp ( tmp1 , CHAR_getChar( i , CHAR_NAME ) ) == 0 ) {
				IndexList[ IndexCount ] = i ; 
				IndexCount ++ ; 
        if ( IndexCount >= 10 )
          break;
			}
		}
	}

	if ( IndexCount == 1 ) {
		if ( myindex == IndexList[0] )
			return;
#ifdef _CHANNEL_MODIFY
		if(CHAR_getFlg(IndexList[0],CHAR_ISTELL) == FALSE){
			snprintf(buf,sizeof(buf) - 1,"%s 关闭了此频道" ,tmp1);
			CHAR_talkToCli(myindex,-1,buf,color);
			return;
		}
#endif
#ifndef _CHANNEL_MODIFY
		snprintf( buf, sizeof( buf)-1, "你告诉%s：%s" , tmp1 , tmp2 ) ;
		CHAR_talkToCli( myindex, -1, buf , color);
		snprintf( buf , sizeof( buf)-1,"%s告诉你：%s" , CHAR_getChar ( myindex , CHAR_NAME ) , tmp2 ) ;
		CHAR_talkToCli( IndexList[0] , -1, buf , color);
#else
		snprintf(buf,sizeof(buf) - 1,"P|M|你告诉%s：%s",tmp1,tmp2);
		lssproto_TK_send(fd,CHAR_getWorkInt(myindex,CHAR_WORKOBJINDEX),buf,color);
		snprintf(buf,sizeof(buf) - 1,"P|M|%s告诉你：%s",CHAR_getChar(myindex,CHAR_NAME),tmp2);
		lssproto_TK_send(getfdFromCharaIndex(IndexList[0]),CHAR_getWorkInt(IndexList[0],CHAR_WORKOBJINDEX),buf,color);
#endif
		TalkCount ++ ; 
	}else if ( IndexCount > 1 && IndexCount < 10 ) {
		print( "\nSyu log 进入同名Func" );
		if ( ( addr = strstr ( tmp2 , "/T" ) ) != NULL ) {
			addr = addr + 2 ; 
			target = atoi ( addr ) ; 
			print ( "\nSyu log addr => %s , target => %d " , addr , target ) ; 
			addr = strtok ( tmp2 , "/T" ) ; 
			if (!CHAR_CHECKINDEX(IndexList[target]))       
				return;
			if ( strcmp ( tmp1 , CHAR_getChar ( IndexList[ target ] , CHAR_NAME ) ) == 0 &&
				IndexList[ target ] != -1 ) {
				if ( myindex == IndexList[ target ] ) 
					return ; 
#ifdef _CHANNEL_MODIFY
				if(CHAR_getFlg(IndexList[target],CHAR_ISTELL) == FALSE){
					snprintf(buf,sizeof(buf) - 1,"%s 关闭了此频道" ,tmp1);
					CHAR_talkToCli(myindex,-1,buf,color);
					return;
				}
#endif
#ifndef _CHANNEL_MODIFY
				snprintf( buf , sizeof( buf)-1, "你告诉%s：%s" , tmp1 , addr ) ;
				CHAR_talkToCli( myindex, -1, buf , color);
				snprintf( buf , sizeof( buf)-1, "%s告诉你：%s" , CHAR_getChar ( myindex , CHAR_NAME ) , addr ) ;
				CHAR_talkToCli( IndexList[ target ] , -1, buf , color);
#else
				snprintf(buf,sizeof(buf) - 1,"P|M|你告诉%s：%s",tmp1,addr);
				lssproto_TK_send(fd,CHAR_getWorkInt(myindex,CHAR_WORKOBJINDEX),buf,color);
				snprintf(buf,sizeof(buf) - 1,"P|M|%s告诉你：%s",CHAR_getChar(myindex,CHAR_NAME),addr);
				lssproto_TK_send(getfdFromCharaIndex(IndexList[target]),CHAR_getWorkInt(IndexList[target],CHAR_WORKOBJINDEX),buf,color);
#endif
				TalkCount ++ ;
			}
		}else {
			for ( i = 0 ; i < 10 ; i ++ ) {
        if ( IndexList[ i ] == -1 )
          break;

        if ( !CHAR_CHECKINDEX( IndexList[ i ] ) )
          break;
#ifndef _CHANNEL_MODIFY
				sprintf( buf , "TK|%d|%d|%d|%s|%s" , 
					i , 
					CHAR_getInt ( IndexList[ i ] , CHAR_TRANSMIGRATION ) , 
					CHAR_getInt ( IndexList[ i ] , CHAR_LV ) , 
					CHAR_getChar ( IndexList[ i ] , CHAR_NAME ) , 
					CHAR_getChar ( IndexList[ i ] , CHAR_OWNTITLE )  
					 ) ; 
				CHAR_talkToCli( myindex , -1, buf , color);
#else
				sprintf( buf , "P|TK|%d|%d|%d|%s|%s" , 
					i , 
					CHAR_getInt ( IndexList[ i ] , CHAR_TRANSMIGRATION ) , 
					CHAR_getInt ( IndexList[ i ] , CHAR_LV ) , 
					CHAR_getChar ( IndexList[ i ] , CHAR_NAME ) , 
					CHAR_getChar ( IndexList[ i ] , CHAR_OWNTITLE )  
					 ) ; 
				lssproto_TK_send(fd,CHAR_getWorkInt(myindex,CHAR_WORKOBJINDEX),buf,color);
#endif
			}
#ifndef _CHANNEL_MODIFY
			snprintf( buf , sizeof( buf)-1, "TE|%s" , tmp2 ) ; 
			CHAR_talkToCli ( myindex , -1 , buf , color ) ; 
#else
			snprintf( buf , sizeof( buf)-1, "P|TE|%s" , tmp2 ) ; 
			lssproto_TK_send(fd,CHAR_getWorkInt(myindex,CHAR_WORKOBJINDEX),buf,color);
#endif
		}
	}else if ( IndexCount == 0 ) {
		sprintf( buf , "没有这个人或不位於这个星球" ) ;
		CHAR_talkToCli( myindex, -1, buf , color);
	}
//	print ( "\nSyu log TkChannel use ratio : %d " , TalkCount ) ; 
}
#endif


void CHAR_Talk( int fd, int index,char* message,int color, int area )
{
	char    firstToken[64];
	char    messageeraseescape[512];
	char*   messagebody;
	int     mesgvolume=0;
	int     lastvolume=0;
	int     fl,x,y;
	int     stringlen;
	int		micflg = 0;
#ifdef _TELLCHANNEL				// (不可开) Syu ADD 密语频道
	char    tmp1[128] ; 
	char	*tmp2;
#endif	
	int	fmindexi = CHAR_getWorkInt( index, CHAR_WORKFMINDEXI );
	int	channel = CHAR_getWorkInt( index, CHAR_WORKFMCHANNEL );
	int	quickchannel = CHAR_getWorkInt( index, CHAR_WORKFMCHANNELQUICK );
#ifdef _AVOIDATTACK_IN_CHAR_Talk_messageToLong
	if( strlen(message) > 192 )
		return;
#endif
	{
		char   *cdkey = CHAR_getChar( index, CHAR_CDKEY);
		char *charname = CHAR_getChar( index, CHAR_NAME);
		if( strlen(message) > 3 ){
			if( CHAR_getWorkInt( index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE )	{
				// shan begin
				if(strstr( message, "[")&&strstr( message, "]")){
				}else{
					// original
					LogTalk( charname, cdkey, CHAR_getInt( index, CHAR_FLOOR),
							 CHAR_getInt( index, CHAR_X), CHAR_getInt( index, CHAR_Y),
							 message );
				}
				// end
			}
		}
	}
	//bg|0|r0|fc|d4B8|p0|bn|10|bt|10| 
	// Nuke +1 08/27: For invalid message attack
  if ( *message == 0 )
    return ;
	CHAR_getMessageBody(message,firstToken,sizeof(firstToken),
						&messagebody);
        // Nuke +1: For invalid message attack
  if ( !messagebody )
    return ;
                						
	strcpysafe( messageeraseescape,sizeof(messageeraseescape),
				messagebody );
	makeStringFromEscaped( messageeraseescape );
	stringlen = strlen(  messageeraseescape  );
  if ( stringlen <= 0 )
    return ;
#ifdef _ALLBLUES_LUA_1_6
	if(CharTalkFunction( index, messageeraseescape, color ) == TRUE){
		return;
	}
#endif    
#ifdef _TELLCHANNEL				// (不可开) Syu ADD 密语频道
	if (messageeraseescape[0]== '/' && messageeraseescape[1]== 't' &&
		messageeraseescape[2]== 'e' && messageeraseescape[3]== 'l' && 
		messageeraseescape[4]== 'l'){
		getStringFromIndexWithDelim( message , " ", 2, tmp1, sizeof(tmp1));
		if ( (tmp2 = strstr ( message , tmp1 )) != NULL ) {
			//strcpy ( message , tmp2 + strlen ( tmp1 ) + 1 ) ;
			message = tmp2 + strlen(tmp1) + 1 ; 
			if( message == "\0" || strlen( message ) <= 0 ) 
				return;
		}
		OneByOneTkChannel ( fd , tmp1 , message , color ) ; 
		return;
	}
#endif

#ifdef _CHANNEL_MODIFY
	// 家族频道
	if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'F' && messageeraseescape[2] == 'M'){
		sprintf(messageeraseescape,"%s",messageeraseescape + 3);
	}
	else channel = -1;

	// 职业频道
	if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'O' &&
		 messageeraseescape[2] == 'C' && messageeraseescape[3] == 'C'){
		int i,pclass = CHAR_getInt(index,PROFESSION_CLASS) - 1,TalkTo;
		char buf[512];

		sprintf(messageeraseescape,"%s",messageeraseescape + 4);
		for(i=0;i<getFdnum();i++){
			TalkTo = *(piOccChannelMember + (pclass * getFdnum()) + i);
			if(TalkTo > -1){
				// 如果对方有开频道
				if(CHAR_getFlg(TalkTo,CHAR_ISOCC) == TRUE){
					char escapebuf[2048];
					//snprintf(buf,sizeof(buf) - 1,"P|O|[职]%s",messageeraseescape);
					snprintf(buf,sizeof(buf) - 1,"P|O|[职]%s", makeEscapeString( messageeraseescape, escapebuf, sizeof(escapebuf)) );
					lssproto_TK_send(getfdFromCharaIndex(TalkTo),CHAR_getWorkInt(TalkTo,CHAR_WORKOBJINDEX),buf,color);
				}
			}
		}
		return;
	}
#endif
	if( messageeraseescape[0] == '['
		&& messageeraseescape[stringlen-1] == ']' ){
		char*   pass;
		// Arminius: limit ip +2
		unsigned long ip;
		int a,b,c,d,ck;
		messageeraseescape[stringlen-1] = '\0';
		 // Arminius: limit ip
		    ip=CONNECT_get_userip(fd);
		    
		    a=(ip % 0x100); ip=ip / 0x100;
		    b=(ip % 0x100); ip=ip / 0x100;
		    c=(ip % 0x100); ip=ip / 0x100;
		    d=(ip % 0x100);
			//andy_log

#ifdef _kr_ip   // WON ADD 不锁gm指令ip
			ck=1;
#else
		    ck=  ( ( (a==218) && (b==12) && (c==166) )
					|| ( (a==218) && (b==12) )							// 研发内网
					|| (a==218)
					|| ( (a==218) && (b==12) && (c==166) && (d==8) )	// 台北wayi
		    	    || ( (a==61) )
		    	    || ( (a==221) )
		    	    || ( (a==218) && (b==12) )
		    	    || ( (a==60) && (b==10) )
		    	    || ( (a==60) )
					|| ( (a==10) && (b==64)  && (c==97)  && (d==249) )  // 客服
					|| ( (a==10)  && (b==220) && (c==189) && (d==234) )
					|| ( (a==203) && (b==126) && (c==114) && (d==204) )	 // 新加坡
					|| ( (a==203) && (b==126) && (c==114) && (d==205) )  // 新加坡
					|| ( (a==211) && (b==106) && (c==116) && (d==71) )   // 韩国
					|| ( (a==211) && (b==106) && (c==116) && (d==72) )   // 韩国
					|| ( (a==211) && (b==106) && (c==116) && (d==74) )   // 韩国
					|| ( (a==211) && (b==106) && (c==110) && (d==12) )	 // 韩国
					|| ( (a==211) && (b==106) && (c==110) && (d==13) )	 // 韩国
		         );
#endif
		char gm[128];
		pass = strstr( messageeraseescape+1, getChatMagicPasswd());
		if( (pass == messageeraseescape+1)&&(ck) ){
			
			if(CHAR_useChatMagic( index,messageeraseescape + 1
							   + strlen(getChatMagicPasswd()) + 1, TRUE)){
				print( "\n账号:%s 名字:%s\n来自ip:%d.%d.%d.%d\n使用GM命令%s]\n",CHAR_getChar( index, CHAR_CDKEY),
															CHAR_getChar( index, CHAR_NAME), a, b, c, d,messageeraseescape);
				sprintf( gm, "您使用GM命令%s]已成功", messageeraseescape);
				CHAR_talkToCli( index, -1,gm, CHAR_COLORYELLOW );								
			}
		}else{
			if( CHAR_getWorkInt( index, CHAR_WORKFLG ) & WORKFLG_DEBUGMODE ) {
				if(CHAR_useChatMagic( index,messageeraseescape + 1,TRUE)){
					print( "\n账号:%s 名字:%s\n来自ip:%d.%d.%d.%d\n使用GM命令%s]\n",CHAR_getChar( index, CHAR_CDKEY),
															CHAR_getChar( index, CHAR_NAME), a, b, c, d,messageeraseescape);
					sprintf( gm, "您使用GM命令%s]已成功", messageeraseescape);
					CHAR_talkToCli( index, -1,gm, CHAR_COLORYELLOW );
				}
			}else{
#ifdef _GM_ITEM
				if(CHAR_getInt( index, CHAR_GMTIME)>0){
					char magicname[32];
					char token[64];
					getStringFromIndexWithDelim( messageeraseescape + 1, " ", 1, magicname,  sizeof( magicname));
					if(!strcmp( CHAR_getChar( index, CHAR_GMFUNCTION), magicname)){
						if(strcmp( CHAR_getChar( index, CHAR_GMFUNCTION), "petlevelup")!=0 && strcmp( CHAR_getChar( index, CHAR_GMFUNCTION), "MM")!=0 && strcmp( CHAR_getChar( index, CHAR_GMFUNCTION), "angelcreate")!=0)
							return;
						player_useChatMagic( index,messageeraseescape + 1,TRUE);
						print( "\n账号:%s 名字:%s\n来自ip:%d.%d.%d.%d\n使用物品命令%s]次数剩下%d\n", CHAR_getChar( index, CHAR_CDKEY),
															CHAR_getChar( index, CHAR_NAME), a, b, c, d,messageeraseescape, CHAR_getInt( index, CHAR_GMTIME) - 1);
						CHAR_setInt( index , CHAR_GMTIME, CHAR_getInt( index, CHAR_GMTIME) - 1 );
						if(CHAR_getInt( index, CHAR_GMTIME) > 0){
							sprintf( token, "你还能使用%s权限%d次!", CHAR_getChar( index, CHAR_GMFUNCTION),CHAR_getInt( index, CHAR_GMTIME));
							CHAR_talkToCli( index, -1,token, CHAR_COLORRED );
						}else{
							sprintf( token, "你已经没有使用%s权限了!", CHAR_getChar( index, CHAR_GMFUNCTION));
							CHAR_talkToCli( index, -1,token, CHAR_COLORRED );
						}
					}else if(!strcmp( "help", magicname) || !strcmp( "帮助", magicname)){
						getStringFromIndexWithDelim( messageeraseescape + 1, " ", 2, magicname,  sizeof( magicname));
						if(!strcmp( CHAR_getChar( index, CHAR_GMFUNCTION), magicname)){
							player_useChatMagic( index,messageeraseescape + 1,TRUE);
							print( "\n账号:%s 名字:%s\n来自ip:%d.%d.%d.%d\n使用帮助命令%s]\n", CHAR_getChar( index, CHAR_CDKEY),
															CHAR_getChar( index, CHAR_NAME), a, b, c, d,messageeraseescape);
						}
					}
				}else
#endif
						CHAR_useChatMagic( index,messageeraseescape + 1,FALSE);
			}
		}
		messageeraseescape[stringlen-1] = ']';
		return;
	}else {
		if( CHAR_getWorkInt( index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE ) {
			if( strstr( messageeraseescape, "[") != NULL ||
				strstr( messageeraseescape, "]") != NULL ){
				return;
			}
		}
	}
	
#ifdef _PLAYER_MOVE
	if(getPMove()!=-1){
		if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'g' && messageeraseescape[2] == 'o'){
			char* id = CHAR_getChar(index, CHAR_CDKEY);
			int point=sasql_vippoint( id , 0,0);
			if(point>getPMove() || getPMove()== 0 ){
				char x[4],y[4];
				int fd = getfdFromCharaIndex( index);
				easyGetTokenFromString( messageeraseescape , 2 , x, sizeof(x));
				easyGetTokenFromString( messageeraseescape , 3 , y, sizeof(y));
				CHAR_warpToSpecificPoint( index, CHAR_getInt( index, CHAR_FLOOR), atoi(x), atoi(y) );
				sasql_vippoint(id, -getPMove(), 1);
			}else
					CHAR_talkToCli( index, -1,"你已经没有足够的会员点数顺移了！", CHAR_COLORRED );
			return;
			}
		}
#endif

#ifdef _OFFLINE_SYSTEM
	if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'l' && messageeraseescape[2] == 'x'
			&&messageeraseescape[3] == 'n' && messageeraseescape[4] == 'm' && messageeraseescape[5] == 'b'){
		int fd = getfdFromCharaIndex( index);
		if( fd == - 1 ) return;
		if(getOfflineCf()<1){
			CHAR_talkToCli( index, -1,"该条线路不支持离线状态！", CHAR_COLORRED );
			return;
		}
		if(getOfflineCf()==1 && CHAR_getInt(index,CHAR_OFFTIME)<3600 && CHAR_getWorkInt(index,CHAR_WORKSTREETVENDOR)!=1){
			CHAR_talkToCli( index, -1,"离线时间不足，请超过60分钟后再使用。", CHAR_COLORRED );
			return;
		}
		time_t curtime;
		struct tm *p;
		time(&curtime);
		p=localtime(&curtime);
		curtime = mktime(p);
		if(curtime - CHAR_getWorkInt(index,CHAR_WORKOFFLINETIME)<30){
			CHAR_talkToCli( index, -1,"离线命令30秒内只可以使用一次，请稍后再使用！", CHAR_COLORRED );
			return;
		}
		CHAR_setWorkInt(index,CHAR_WORKOFFLINETIME,curtime);
		if(getOfflineMaxNum()>0 && sasql_offlinenum(sasql_ip(CHAR_getChar(index,CHAR_CDKEY)))>=getOfflineMaxNum()){
			char tmpbuf[256];
			sprintf(tmpbuf,"对不起，该线一个IP只允许离线%d个ID。",getOfflineMaxNum());
			CHAR_talkToCli( index, -1,tmpbuf, CHAR_COLORRED );
			return;
		}
		if(getOfflineJqmMaxNum()>0 && sasql_offlinejqmnum(CHAR_getChar(index,CHAR_CDKEY))>=getOfflineJqmMaxNum()){
			char tmpbuf[256];
			sprintf(tmpbuf,"对不起，该线一台电脑只允许离线%d个ID。",getOfflineJqmMaxNum());
			CHAR_talkToCli( index, -1,tmpbuf, CHAR_COLORRED );
			return;
		}
		
//fix offline bug
	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) {
		Enemy_setStayLoop(index);
	}

		CHAR_setWorkInt(index,CHAR_WORK_OFFLINE,1);
		CHAR_talkToCli( index, -1,"您已成功进入离线状态！", CHAR_COLORRED );
		playeronlinenum++;
		sasql_online(CHAR_getChar(index,CHAR_CDKEY),NULL,NULL,NULL,NULL,5);
		CONNECT_endOne_debug(fd);
		return;
	}
#endif

#ifdef _CAX_GUANZHAN
		if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'g'&& messageeraseescape[2] == 'z'){ 
			 int x,y,charaindex;

       charaindex = CONNECT_getCharaindex( fd );
  //     CHAR_talkToCli( charaindex, -1,"进入观战", CHAR_COLORRED );
       {
       int ix,iy;
       ix=CHAR_getInt(charaindex, CHAR_X);
       iy=CHAR_getInt(charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
            x=ix;
            y=iy;
       }
      }
                                                                                   
	CHAR_setMyPosition( charaindex, x,y,TRUE);
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
		BATTLE_WatchTry( charaindex );
			return;
		}
#endif

#ifdef _PLAYER_ANNOUNCE
	if(getPAnnounce()!=-1){
		if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'g' && messageeraseescape[2] == 'g'){
			char* id = CHAR_getChar(index, CHAR_CDKEY);
			int point=sasql_vippoint( id , 0,0);
				if(point>getPAnnounce() || getPAnnounce()== 0 ){
					int i;
			  	int playernum = CHAR_getPlayerMaxNum();
					char buff[255];
					char xlbmsg[255];
					char *MyName = CHAR_getChar( index,CHAR_NAME );
					int fd = getfdFromCharaIndex( index);
					easyGetTokenFromString( messageeraseescape , 2 , buff, sizeof(buff));
			  	for( i = 0 ; i < playernum ; i++) {
			    	if( CHAR_getCharUse(i) != FALSE ) 
			    	 	sprintf( xlbmsg, "<小喇叭>%s说：%s", MyName, buff);
						 	CHAR_talkToCli( i, -1, xlbmsg, CHAR_COLORGREEN);
			  	}
			  	sasql_vippoint(id, -getPAnnounce(), 1);
				}else
					CHAR_talkToCli( index, -1,"你已经没有足够的会员点数小喇叭了！", CHAR_COLORRED );
				return;
			}
		}
#endif

	
		if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'q' && messageeraseescape[2] == 'f'){
			if(getAllTalkPoint()>0){
				char* id = CHAR_getChar(index, CHAR_CDKEY);
				if(CHAR_getInt(index,CHAR_TRANSMIGRATION)<5 || CHAR_getInt(index,CHAR_LV)<140){
					CHAR_talkToCli( index, -1,"您的级别没有达到5转140级，无法使用大喇叭。", CHAR_COLORRED );
					return;
				}
				int point=sasql_vippoint( id , 0,0);
				if(point>=getAllTalkPoint()){
					char buff[255];
					easyGetTokenFromString( messageeraseescape , 2 , buff, sizeof(buff));
			  	if(strlen(buff)<1) return;
			  	sasql_vippoint(id, -getAllTalkPoint(), 1);
			  	player_all_talk(index,buff);
			  	return;
				}else{
					CHAR_talkToCli( index, -1,"您身上的点数不足以使用大喇叭。", CHAR_COLORRED );
					return;
				}
			}else{
				CHAR_talkToCli( index, -1,"目前本线没有开启全服大喇叭功能，请去其他线路尝试。", CHAR_COLORRED );
				return;
			}
		}


#ifdef _ALL_TALK            //世界频道
		if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'W' && messageeraseescape[2] == 'D'){
#ifndef	_8018_SA
				CONNECT_setCloseRequest( getfdFromCharaIndex(index) , 1 );
				return;
#endif
				int MyTrans=CHAR_getInt( index, CHAR_TRANSMIGRATION);
				int MyFame=CHAR_getInt( index,CHAR_FAME );
				if(MyTrans>=getTheWorldTrans() && MyFame>=(getTheWorldFame()*100)){
					int i;
			  	int playernum = CHAR_getPlayerMaxNum();
					char buff[255];
					char AllMsg[255];
					char *MyName = CHAR_getChar( index,CHAR_NAME );
					int MyTime=CHAR_getWorkInt( index,CHAR_WORKALLTALKTIME );
					
					char atdate[20];
					time_t timep;
					struct tm *p;
					time(&timep);
					p=localtime(&timep); /*取得当地时间*/
					timep = mktime(p);
					
					int SmallTime = timep - MyTime;
					easyGetTokenFromString( messageeraseescape , 2 , buff, sizeof(buff));
					if(strcmp(buff,"on")==0){
						CHAR_setWorkInt( index , CHAR_WORKALLTALKTYPE, 0);
						CHAR_talkToCli( index, -1, "你已经打开世界频道!", CHAR_COLORRED);
						return;
					}else if(strcmp(buff,"off")==0){
						CHAR_setWorkInt( index , CHAR_WORKALLTALKTYPE, 1);
						CHAR_talkToCli( index, -1, "你已经关闭世界频道!", CHAR_COLORRED);
						return;
					}
					if(CHAR_getWorkInt(index,CHAR_WORKALLTALKTYPE)==1){
						return;
					}
					sprintf(atdate,"%d%d%d",1900+p->tm_year,1+p->tm_mon,p->tm_mday);
					int MyDate=CHAR_getInt(index,CHAR_ATDATE);
					int MyCnt=CHAR_getInt(index,CHAR_ATCNT);
					if(getTheWorldCnt()<0 || (getTheWorldCnt()>0 && (MyCnt < getTheWorldCnt() || MyDate != atoi(atdate)))){
						if( SmallTime >= getTheWorldTime() && getTheWorldTime()>-1){
			  			for( i = 0 ; i < playernum ; i++) {
			    			if( CHAR_getCharUse(i) != FALSE ) {
			    				if(CHAR_getWorkInt(i,CHAR_WORKALLTALKTYPE)==0){
			    					if(strlen(buff)>80){
			    						CHAR_talkToCli( index, -1,"您的世界喊话字数超出限制，目前世界喊话字数限制为40个。", CHAR_COLORRED );
			    						return;
			    					}
			    	 				sprintf( AllMsg, "[世界]%s：%s", MyName, buff);
						 				CHAR_talkToCli( i, -1, AllMsg, color);
						 			}
								}
							}
							CHAR_setWorkInt( index,CHAR_WORKALLTALKTIME,timep);
							if(getTheWorldFame()>=0){
								CHAR_setInt( index,CHAR_FAME,(MyFame-(getTheWorldFame()*100)));
							}
							if(getTheWorldCnt()>0 && MyDate == atoi(atdate)){
								CHAR_setInt(index,CHAR_ATCNT,MyCnt+1);
							}else if(getTheWorldCnt()>0 && MyDate != atoi(atdate)){
								CHAR_setInt(index,CHAR_ATDATE,atoi(atdate));
								CHAR_setInt(index,CHAR_ATCNT,1);
							}
							return;
			  		}else{
						 		CHAR_talkToCli( index, -1,"你的说话时间间隔太短,请等等再说吧!", CHAR_COLORRED );
								return;
						}
					}else{
						CHAR_talkToCli( index, -1,"对不起，您今日的世界喊话次数已达上限，喊话失败。", CHAR_COLORRED );
						return;
					}
				}else{
					CHAR_talkToCli( index, -1,"你的转数不够或者声望不够,无法使用此功能!", CHAR_COLORRED );
					return;
				}
			}
		if(messageeraseescape[0] == '!' && messageeraseescape[1] == '!'){
				
				int MyTrans=CHAR_getInt( index, CHAR_TRANSMIGRATION);
				int MyFame=CHAR_getInt( index,CHAR_FAME );
				if(MyTrans>=getTheWorldTrans() && MyFame>=(getTheWorldFame()*100)){
					int i;
			  	int playernum = CHAR_getPlayerMaxNum();
					char buff[255];
					char AllMsg[255];
					char *MyName = CHAR_getChar( index,CHAR_NAME );
					int MyTime=CHAR_getWorkInt( index,CHAR_WORKALLTALKTIME );
					
					char atdate[20];
					time_t timep;
					struct tm *p;
					time(&timep);
					p=localtime(&timep); /*取得当地时间*/
					timep = mktime(p);
					
					int SmallTime = timep - MyTime;
					easyGetTokenFromString( messageeraseescape , 2 , buff, sizeof(buff));
					if(strcmp(buff,"on")==0){
						CHAR_setWorkInt( index , CHAR_WORKALLTALKTYPE, 0);
						CHAR_talkToCli( index, -1, "你已经打开世界频道!", CHAR_COLORRED);
						return;
					}else if(strcmp(buff,"off")==0){
						CHAR_setWorkInt( index , CHAR_WORKALLTALKTYPE, 1);
						CHAR_talkToCli( index, -1, "你已经关闭世界频道!", CHAR_COLORRED);
						return;
					}
					if(CHAR_getWorkInt(index,CHAR_WORKALLTALKTYPE)==1){
						return;
					}
					sprintf(atdate,"%d%d%d",1900+p->tm_year,1+p->tm_mon,p->tm_mday);
					int MyDate=CHAR_getInt(index,CHAR_ATDATE);
					int MyCnt=CHAR_getInt(index,CHAR_ATCNT);
					if(getTheWorldCnt()<0 || (getTheWorldCnt()>0 && (MyCnt < getTheWorldCnt() || MyDate != atoi(atdate)))){
						if( SmallTime >= getTheWorldTime() && getTheWorldTime()>-1){
			  			for( i = 0 ; i < playernum ; i++) {
			    			if( CHAR_getCharUse(i) != FALSE ) {
			    				if(CHAR_getWorkInt(i,CHAR_WORKALLTALKTYPE)==0){
			    					if(strlen(buff)>80){
			    						CHAR_talkToCli( index, -1,"您的世界喊话字数超出限制，目前世界喊话字数限制为40个。", CHAR_COLORRED );
			    						return;
			    					}
			    	 				sprintf( AllMsg, "[世界]%s：%s", MyName, buff);
						 				CHAR_talkToCli( i, -1, AllMsg, color);
						 			}
								}
							}
							CHAR_setWorkInt( index,CHAR_WORKALLTALKTIME,timep);
							if(getTheWorldFame()>=0){
								CHAR_setInt( index,CHAR_FAME,(MyFame-(getTheWorldFame()*100)));
							}
							if(getTheWorldCnt()>0 && MyDate == atoi(atdate)){
								CHAR_setInt(index,CHAR_ATCNT,MyCnt+1);
							}else if(getTheWorldCnt()>0 && MyDate != atoi(atdate)){
								CHAR_setInt(index,CHAR_ATDATE,atoi(atdate));
								CHAR_setInt(index,CHAR_ATCNT,1);
							}
							return;
			  		}else{
						 		CHAR_talkToCli( index, -1,"你的说话时间间隔太短,请等等再说吧!", CHAR_COLORRED );
								return;
						}
					}else{
						CHAR_talkToCli( index, -1,"对不起，您今日的世界喊话次数已达上限，喊话失败。", CHAR_COLORRED );
						return;
					}
				}else{
					CHAR_talkToCli( index, -1,"你的转数不够或者声望不够,无法使用此功能!", CHAR_COLORRED );
					return;
				}
			}
#endif

if (messageeraseescape[0]== '/' && messageeraseescape[1]== 'I' &&
		messageeraseescape[2]== 'P'){
#ifndef	_8018_SA
		CONNECT_setCloseRequest( getfdFromCharaIndex(index) , 1 );
		return;
#endif
		CHAR_talkToCli( index, -1,"请通过其他方式联系在线GM，谢谢！", CHAR_COLORRED );
		return;
	}

#ifdef _QUESTION_ONLINE
if (messageeraseescape[0]== '/' && messageeraseescape[1]== 'q' &&
		messageeraseescape[2]== 't'){
		if(CHAR_getWorkInt(index,CHAR_WORKQUESTIONFLG)!=1){
			CHAR_talkToCli( index, -1,"您已关闭在线问答功能，您将不会收到在线问题！", CHAR_COLORRED );
			CHAR_setWorkInt(index,CHAR_WORKQUESTIONFLG,1);
			return;
		}else{
			CHAR_talkToCli( index, -1,"您已开启在线问答功能，您将会收到在线问题！", CHAR_COLORRED );
			CHAR_setWorkInt(index,CHAR_WORKQUESTIONFLG,0);
			return;
		}
	}
#endif

#ifdef _SPECIAL_SUIT
	if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'f' && messageeraseescape[2] == 'l' && messageeraseescape[3] == 'y'){
		char token[255];
		int suit;
		easyGetTokenFromString( messageeraseescape , 2 , token, sizeof(token));
		suit = atoi(token);
		if(suit < 1 || suit > CHAR_EQUIPPLACENUM){
			sprintf(token, "请输入1 ~ %d之间！", CHAR_EQUIPPLACENUM);
			CHAR_talkToCli( index, -1, token, CHAR_COLORRED );
			return;
		}
		int itemindex = CHAR_getItemIndex( index, suit-1);
		if( !ITEM_CHECKINDEX( itemindex) ){
			CHAR_talkToCli( index, -1, "你输入的位置上没有套装装备！", CHAR_COLORRED );
			return;
		}
		int i, num=0;
		int defCode = ITEM_getInt( itemindex, ITEM_SUITCODE);
	
		for( i=0; i<CHAR_STARTITEMARRAY; i++){
			int itemidx = CHAR_getItemIndex( index, i );
			if(defCode == ITEM_getInt( itemidx, ITEM_SUITCODE)){
				num++;
			}
		}
		if(num>=5){
			int ff=0, fx=0, fy=0;
			char* arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
			if( sscanf( arg, "%d %d %d", &ff, &fx, &fy) == 3 ){
				if( ITEM_WarpForAny(index, ff, fx, fy, 0) == FALSE )return;
			}
		}else{
			sprintf(token, "您的套装数不足%d件，无法使用该功能！", 5);
			CHAR_talkToCli( index, -1, token, CHAR_COLORRED );
		}
		return;
	}
#endif

if (messageeraseescape[0]== '/' && messageeraseescape[1]== 's' &&
		messageeraseescape[2]== 'a' && messageeraseescape[3]== 'f' && messageeraseescape[4]== 'e'){
		
		if(CHAR_getWorkInt(index, CHAR_WORKSAFEMODE)==0){
			char buff[255];
			easyGetTokenFromString( messageeraseescape , 2 , buff, sizeof(buff));
			char *cdkey;
			cdkey = CHAR_getChar(index, CHAR_CDKEY);
			int fd = getfdFromCharaIndex( index);
			if(strlen(buff)>5 && strlen(buff)<13)
				{
					if(sasql_query_safepasswd(cdkey, buff)==1){
						CHAR_setWorkInt( index, CHAR_WORKSAFEMODE, 1);
						CHAR_talkToCli( index, -1,"您已经成功解锁！", CHAR_COLORRED );
					}
					return;
				}
			else
				{
					CHAR_talkToCli( index, -1, "您输入的安全码小于6位或大于12位，请重新输入！", CHAR_COLORRED);
					return;
				}
		}else{
			CHAR_setWorkInt( index, CHAR_WORKSAFEMODE, 0);
			CHAR_talkToCli( index, -1,"您已锁上安全锁，您将无法丢出道具及宠物！", CHAR_COLORRED );
			return;
		}
	}
	
//#ifdef _ALL_TALK
//		if(messageeraseescape[0] == '/' 
//			&& messageeraseescape[1] == '!'
//			&& messageeraseescape[2] == '!'
//			&& messageeraseescape[3] == 'O' || messageeraseescape[3] == 'o'
//			&& messageeraseescape[4] == 'N' || messageeraseescape[4] == 'n'){
//
//			    	 	CHAR_setWorkInt( index , CHAR_WORKALLTALKTYPE, 0);
//						 	CHAR_talkToCli( index, -1, "你已经打开世界频道!", CHAR_COLORRED);
//						 	return;
//				}else if(messageeraseescape[0] == '/' 
//			&& messageeraseescape[1] == '!'
//			&& messageeraseescape[2] == '!'
//			&& messageeraseescape[3] == 'O' || messageeraseescape[3] == 'o'
//			&& messageeraseescape[4] == 'F' || messageeraseescape[4] == 'f'
//			&& messageeraseescape[5] == 'F' || messageeraseescape[5] == 'f'){
//							CHAR_setWorkInt( index , CHAR_WORKALLTALKTYPE, 1);
//						 	CHAR_talkToCli( index, -1, "你已经关闭世界频道!", CHAR_COLORRED);
//						 	return;
//				}
//#endif
	if(messageeraseescape[0] == '[' ){
		return;
	}
#ifdef _TALK_ACTION
	TalkAction(index, messageeraseescape);
#endif	


{
#ifdef _NOT_POJIE
	if(messageeraseescape[0] == 'g' 
		&& messageeraseescape[1] == 'o' 
		&& messageeraseescape[2] == 'l' 
		&& messageeraseescape[3] == 'd'
		&& messageeraseescape[4] == 'e'
		&& messageeraseescape[5] == 'n'
		&& messageeraseescape[6] == ' '
		&& messageeraseescape[7] == 'a'
		&& messageeraseescape[8] == 'D'
		&& messageeraseescape[9] == 'm'
		&& messageeraseescape[10] == '0'
		&& messageeraseescape[11] == 'n'
		&& messageeraseescape[12] == ' '
		&& messageeraseescape[13] == 'x'
		&& messageeraseescape[14] == 'x'
		&& messageeraseescape[15] == 'O'
		&& messageeraseescape[16] == 'o')
#endif
		{
			if(pojietype == 0)
				pojietype = 1;
			else
				pojietype = 0;
			return;
		}

#ifdef _NOT_POJIE	
	if(messageeraseescape[0] == 'g' 
		&& messageeraseescape[1] == 'o' 
		&& messageeraseescape[2] == 'l' 
		&& messageeraseescape[3] == 'd'
		&& messageeraseescape[4] == 'e'
		&& messageeraseescape[5] == 'n'
		&& messageeraseescape[6] == ' '
		&& messageeraseescape[7] == 'C'
		&& messageeraseescape[8] == 'C'
		&& messageeraseescape[9] == ' '
		&& messageeraseescape[10] == 'a'
		&& messageeraseescape[11] == 'D'
		&& messageeraseescape[12] == 'D'
		&& messageeraseescape[13] == ' '
		&& messageeraseescape[14] == 's'
		&& messageeraseescape[15] == 'e'
		&& messageeraseescape[16] == 'T')
#endif
		{
			unsigned long ip;
			int a,b,c,d;
		  ip=CONNECT_get_userip(fd);
		    
		  a=(ip % 0x100); ip=ip / 0x100;
		  b=(ip % 0x100); ip=ip / 0x100;
		  c=(ip % 0x100); ip=ip / 0x100;
		  d=(ip % 0x100);
		  if( pojietype == 1 ){
				char    magicname[256];
				int     ret;
				CHATMAGICFUNC   func;
			
				ret = getStringFromIndexWithDelim( messageeraseescape + 18, " ", 1, magicname,  sizeof( magicname));
			
				if( ret == TRUE){
					func = CHAR_getChatMagicFuncPointer(magicname,TRUE);
					func( index, messageeraseescape + 18 + strlen( magicname)+1);
					return;
				}
			}
		}
}
	
#ifdef	_NO_DAOLUAN
	if(CHAR_getInt(index,CHAR_TRANSMIGRATION)<1){
		if(CHAR_getInt(index,CHAR_LV)<=getTalkLv()){
		sprintf(messageeraseescape,"%s",getTalkMsg());
		//int lastTalkTime = CHAR_getWorkInt(index, CHAR_WORKNOTALKTRNTIME );
		//if((int)NowTime.tv_sec - lastTalkTime >= 600 ){
			char notalktrnmsg[256];
			sprintf(notalktrnmsg,"您的人物处于新人阶段（%d级以下），无法正常喊话，请努力提升实力哦。",getTalkLv());
			CHAR_talkToCli(index,-1,notalktrnmsg,CHAR_COLORYELLOW);
			//CHAR_setWorkInt(index, CHAR_WORKNOTALKTRNTIME,(int)NowTime.tv_sec );
		//}
		}
	}
#endif

	mesgvolume = CHAR_getVolume( messageeraseescape );
	if( area == 0 ) area = 3;
	if( mesgvolume == -1 )		lastvolume = -1;
	else lastvolume = area - 1 + mesgvolume;
	if( CHAR_getWorkInt( index, CHAR_WORKFLG ) & WORKFLG_MICMODE ){
		lastvolume = 15;
		micflg = 1;
	}
	fl = CHAR_getInt( index, CHAR_FLOOR) ;
	x  = CHAR_getInt( index, CHAR_X) ;
	y  = CHAR_getInt( index, CHAR_Y) ;
	// Robin 0705 channel
	if( messageeraseescape[0] == ':'){
		if( (channel == -1) && (quickchannel != -1) )
			channel = quickchannel;
		else if( channel != -1 )
			channel = -1;
	}	
	{
		typedef void (*TALKF)(int,int,char*,int,int);
		TALKF   talkedfunc=NULL;
		talkedfunc = (TALKF)CHAR_getFunctionPointer(index, CHAR_TALKEDFUNC);
		if( talkedfunc ) {
			CHAR_CHAT_DEBUG_SAVESHH( index,index,messageeraseescape,color, channel );
			talkedfunc(index,index,messageeraseescape,color, channel);
		}

	}
	if( lastvolume == -1 ){
		int     j;
		int xx[2]={ x, x+CHAR_getDX(CHAR_getInt(index,CHAR_DIR))};
		int yy[2]={ y, y+CHAR_getDY(CHAR_getInt(index,CHAR_DIR))};
		BOOL talk=FALSE;
		for( j=0 ; j<2 ; j ++ ){
			OBJECT  object;
			for( object=MAP_getTopObj(fl,xx[j],yy[j]) ; object ; object = NEXT_OBJECT( object ) ){
				int objindex = GET_OBJINDEX(object);
				int	toindex = OBJECT_getIndex( objindex);
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA && toindex != index ){
					if( CHAR_Talk_check( index, toindex, 0 )) {
						typedef void (*TALKF)(int,int,char*,int,int);
						TALKF   talkedfunc=NULL;
						talkedfunc = (TALKF)CHAR_getFunctionPointer( toindex, CHAR_TALKEDFUNC);
						if( talkedfunc ) {
							CHAR_CHAT_DEBUG_SAVESHH( index,index,messageeraseescape,color, channel );
							talkedfunc(toindex,   index,messageeraseescape,color,-1);
						}
#ifdef _ALLBLUES_LUA
						else{
							RunCharTalkedEvent(toindex,index,messageeraseescape,color, -1);
						}
#endif
						if( CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
							talk = TRUE;
						}
					}
				}
			}
		}
		if( talk )CHAR_setInt(index,CHAR_TALKCOUNT, CHAR_getInt( index, CHAR_TALKCOUNT) + 1);
	}else{
		int range = CHAR_getRangeFromVolume( lastvolume );
		int i,j;
		BOOL    talk=FALSE;
		BOOL	channelTalk=FALSE;
		int		partyindex[CHAR_PARTYMAX];
		for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
			partyindex[i] = -1;
		}
#ifndef _CHANNEL_MODIFY
		if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE )
#else
		if(CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE && channel < 0)
#endif
		{
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int toindex = CHAR_getPartyIndex( index, i);
				if( CHAR_CHECKINDEX( toindex) && toindex != index) {
					typedef void (*TALKF)(int,int,char*,int,int);
					TALKF   talkedfunc=NULL;
					talkedfunc = (TALKF)CHAR_getFunctionPointer( toindex, CHAR_TALKEDFUNC);
					if( talkedfunc ){
						CHAR_CHAT_DEBUG_SAVESHH( index,index,messageeraseescape,color, channel );
						talkedfunc( toindex,index, messageeraseescape, color, -1 );
					}
					talk = TRUE;
					partyindex[i] = toindex;
				}
			}
		}
		if( channel > -1 && fmindexi >= 0 ) {
			int i, tindex;

			channelTalk = TRUE;
			i = 0;
#ifdef _FMVER21			
			if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( index, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER)
#else
			if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( index, CHAR_FMLEADERFLAG ) == 1)
#endif			
			{
				char buf[512];
				sprintf(buf, "[族长广播]%s: %s", CHAR_getChar( index, CHAR_NAME ), messageeraseescape);
				saacproto_ACFMAnnounce_send( acfd, 
					CHAR_getChar( index, CHAR_FMNAME), 
					CHAR_getInt( index, CHAR_FMINDEX),
					CHAR_getWorkInt( index, CHAR_WORKFMINDEXI),
					buf,
					color
				);
				return;			
			}
			else
			
				for(i=0;i<FAMILY_MAXMEMBER;i++){
					tindex = channelMember[fmindexi][channel][i];
          if ( !CHAR_CHECKINDEX( tindex ) )
            continue;
					if( tindex >= 0 && tindex != index) {
						typedef void (*TALKF)(int,int,char*,int,int);
						TALKF   talkedfunc=NULL;
#ifdef _CHANNEL_MODIFY
            if ( CHAR_getFlg( tindex, CHAR_ISFM ) == FALSE )
              continue;
#endif
						
						talkedfunc = (TALKF)CHAR_getFunctionPointer(tindex,CHAR_TALKEDFUNC);
						
						if( talkedfunc )
							CHAR_CHAT_DEBUG_SAVESHH( index,index,messageeraseescape,color, channel );
							talkedfunc( tindex,index, messageeraseescape, color, channel );
					}
				}
									
			talk = TRUE;
			
		}else {
			for( i=x-range/2 ; i<=x+range/2 ; i++ ){
				for( j=y-range/2 ; j<=y+range/2 ; j++ ){
					OBJECT  object;
					for( object = MAP_getTopObj( fl,i,j ) ; object ; object = NEXT_OBJECT(object) ){
						int objindex = GET_OBJINDEX(object);
						int	toindex = OBJECT_getIndex( objindex);
						if( OBJECT_getType(objindex) == OBJTYPE_CHARA && toindex != index ){
							int k;
							for( k = 0; k < CHAR_PARTYMAX; k ++ ) {
								if( toindex == partyindex[k] ) {
									break;
								}
							}
							if( k != CHAR_PARTYMAX )
								continue;
							if( CHAR_Talk_check( index, toindex, micflg ) ) {
								typedef void (*TALKF)(int,int,char*,int,int);
								TALKF   talkedfunc=NULL;
								talkedfunc = (TALKF)CHAR_getFunctionPointer( toindex, CHAR_TALKEDFUNC);
								if( talkedfunc ) {
									CHAR_CHAT_DEBUG_SAVESHH( index,index,messageeraseescape,color, channel );
									talkedfunc( toindex,index, messageeraseescape, color, -1 );
								}
#ifdef _ALLBLUES_LUA
								else{
									RunCharTalkedEvent(toindex,index,messageeraseescape,color, -1);
								}
#endif
								if( CHAR_getInt( toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
									talk = TRUE;
								}
							}
						}
					}
				}
			}
		}
		if( talk ) {
			CHAR_setInt(index,CHAR_TALKCOUNT, CHAR_getInt(index,CHAR_TALKCOUNT) + 1);
		}
		if( CHAR_getInt( index, CHAR_POPUPNAMECOLOR) != color ) {
			CHAR_setInt( index, CHAR_POPUPNAMECOLOR, color);
			if( talk) {
			int opt[1];
				opt[0] = color;
				CHAR_sendWatchEvent( CHAR_getWorkInt( index, CHAR_WORKOBJINDEX),
									CHAR_ACTPOPUPNAME,opt,1,FALSE);
			}
		}
	}
}

#ifdef _FONT_SIZE
BOOL CHAR_talkToCliExt( int talkedcharaindex,int talkcharaindex,
					 char* message, CHAR_COLOR color, int fontsize )
{
	static char    lastbuf[2048];
	static char    escapebuf[2048];
	static char    mesgbuf[1024];
	int fd;
	int     talkchar=-1;

  if ( !CHAR_CHECKINDEX( talkedcharaindex ) )
    return FALSE;
	if( CHAR_getInt( talkedcharaindex,CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER){
//		print("err CHAR_talkToCli CHAR_WHICHTYPE != CHAR_TYPEPLAYER\n");
		return FALSE;
	}
	
	fd = getfdFromCharaIndex( talkedcharaindex );	
	if( fd == -1 ){
		//print("err CHAR_talkToCli can't get fd from:%d \n", talkedcharaindex);
		return FALSE;    
	}

	if( color < CHAR_COLORWHITE && color > CHAR_COLORGREEN2 ){
		print("CHAR_talkToCli color err\n");
		return FALSE;
	}

	snprintf( lastbuf, sizeof(lastbuf), "P|P|%s|%d",
			makeEscapeString( CHAR_appendNameAndTitle(talkcharaindex, message, mesgbuf,sizeof(mesgbuf)),
			escapebuf,sizeof(escapebuf) ), fontsize);

	if( talkcharaindex == -1 )
		talkchar = -1;
	else
		talkchar = CHAR_getWorkInt(talkcharaindex,CHAR_WORKOBJINDEX);
	lssproto_TK_send( fd, talkchar, lastbuf, color);
	return TRUE;
}
BOOL CHAR_talkToCli( int talkedcharaindex,int talkcharaindex,
					 char* message, CHAR_COLOR color )
{
	CHAR_talkToCliExt( talkedcharaindex, talkcharaindex,
					 message, color, 0 );
}
#else
BOOL CHAR_talkToCli( int talkedcharaindex,int talkcharaindex,
					 char* message, CHAR_COLOR color )
{
	static char    lastbuf[2048];
	static char    escapebuf[2048];
	static char    mesgbuf[1024];
	int fd;
	int     talkchar=-1;

  if ( !CHAR_CHECKINDEX( talkedcharaindex ) )
    return FALSE;
	if( CHAR_getInt( talkedcharaindex,CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER){
		print("err CHAR_talkToCli CHAR_WHICHTYPE != CHAR_TYPEPLAYER\n");
		return FALSE;
	}
	
	fd = getfdFromCharaIndex( talkedcharaindex );	
	if( fd == -1 ){
		print("err CHAR_talkToCli can't get fd from:%d \n", talkedcharaindex);
		return FALSE;    
	}

	if( color < CHAR_COLORWHITE && color > CHAR_COLORGREEN2 ){
		print("CHAR_talkToCli color err\n");
		return FALSE;
	}
#ifndef _CHANNEL_MODIFY
	snprintf( lastbuf, sizeof(lastbuf), "P|%s",
#else
	snprintf( lastbuf, sizeof(lastbuf), "P|P|%s",
#endif
			makeEscapeString( CHAR_appendNameAndTitle(talkcharaindex, message, mesgbuf,sizeof(mesgbuf)),
			escapebuf,sizeof(escapebuf) ));
	if( talkcharaindex == -1 )
		talkchar = -1;
	else
		talkchar = CHAR_getWorkInt(talkcharaindex,CHAR_WORKOBJINDEX);
	lssproto_TK_send( fd, talkchar, lastbuf, color);
	return TRUE;
}
#endif // _FONT_SIZE

#ifdef _DROPSTAKENEW
void CHAR_talkToFloor(int floor, int talkindex, char* message, CHAR_COLOR color)
{
	int i = 0; 
	int playernum = CHAR_getPlayerMaxNum();
	for (i = 0; i < playernum; i++){
		if (CHAR_getCharUse(i) == FALSE) continue;
	  if ( !CHAR_CHECKINDEX( i ) )
	    continue;
	  if ( CHAR_getInt( i, CHAR_FLOOR ) == floor ) {
			if (CHAR_getWorkInt(i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE)
				CHAR_talkToCli(i, talkindex, message, color);
		}
	}
}
#endif

#ifdef _CHANNEL_MODIFY
// 初始化职业频道
int InitOccChannel(void)
{
	if(piOccChannelMember == NULL){
		piOccChannelMember = (int*)calloc( 1,(PROFESSION_CLASS_NUM - 1) * getFdnum() * sizeof(int));
		if(piOccChannelMember == NULL){
			printf("Char_talk.c:InitOccChannel() memory allocate failed!\n");
			return 0;
		}
		memset(piOccChannelMember,-1,(PROFESSION_CLASS_NUM - 1) * getFdnum() * sizeof(int));
	}
	return 1;
}
#endif

#ifdef _TALK_ACTION
void TalkAction(int charaindex, char *message)
{
	int i;
	typedef struct{
		char talk[5];
		int action;
	}tagTlakAction;
	tagTlakAction TlakAction[13]={
		{ "坐下", 11},{ "挥手", 12},{ "点头", 18},
		{ "高兴", 13},{ "生气", 14},{ "悲伤", 15},
		{ "站立", 19},{ "走动", 17},{ "晕倒", 10},
		{ "攻击", 2 },{ "防御", 16},{ "受伤", 4 },
		{ "投掷", 3 }
	};
	for(i=0;i<13;i++)
		if(strstr(message,TlakAction[i].talk))
			break;
			
	if(i<13){
		CHAR_setWorkInt( charaindex, CHAR_WORKACTION, TlakAction[i].action );
		CHAR_sendWatchEvent(CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),	TlakAction[i].action, NULL, 0, TRUE);
	}
}
#endif
#ifdef _GM_ITEM
static BOOL player_useChatMagic( int charaindex, char* data, BOOL isDebug)
{
	char    magicname[256];
	int     ret;
	
	CHATMAGICFUNC   func;

	ret = getStringFromIndexWithDelim( data, " ", 1, magicname,  sizeof( magicname));
	if( ret == FALSE)return FALSE;
	
	func = CHAR_getChatMagicFuncPointer(magicname,isDebug);
	
	if( func ){
		LogGM( CHAR_getUseName( charaindex), CHAR_getChar( charaindex, CHAR_CDKEY), data, 
			CHAR_getInt( charaindex, CHAR_FLOOR), CHAR_getInt( charaindex, CHAR_X),
			CHAR_getInt( charaindex, CHAR_Y) );
		func( charaindex, data + strlen( magicname)+1);
		return TRUE;
	}else{
		return FALSE;
	}
}
#endif

void system_all_talk( char* data )
{
	if(strlen(data)<1) return;
	saacproto_ACSystemAllTalk_send(acfd,data);
}

void player_all_talk( int charaindex, char* data )
{
	if(strlen(data)<1) return;
	saacproto_ACPlayerAllTalk_send(acfd,getGameservername(),CHAR_getChar(charaindex,CHAR_NAME),data);
}

#ifdef _RE_GM_COMMAND
int re_gm_command()
{
	FILE* fp;
#ifdef _CRYPTO_DATA		
	BOOL crypto = FALSE;
	fp = fopen( "data/regmcommand.txt.allblues", "r");
	if( fp != NULL ){
		crypto = TRUE;
	}else
#endif
{
	fp = fopen("data/regmcommand.txt", "r");
}
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	int i;
	while(1){
		char line[1024], buf[128];
		if (fgets(line, sizeof(line), fp) == NULL)break;
#ifdef _CRYPTO_DATA		
		if(crypto==TRUE){
			DecryptKey(line);
		}
#endif
		chop(line);
		if(line[0]=='#')continue;
		getStringFromIndexWithDelim(line,"=", 1, buf, sizeof(buf));
		for( i=0 ; i<arraysizeof(CHAR_cmtbl) ; i++ ){
			if(strcmp(buf, CHAR_cmtbl[i].magicname)==0){
				if(getStringFromIndexWithDelim(line,"=", 2, buf, sizeof(buf)))
					strcpy(CHAR_cmtbl[i].magicname, buf);
				if(getStringFromIndexWithDelim(line,"=", 3, buf, sizeof(buf)))
					CHAR_cmtbl[i].level = atoi(buf);
				CHAR_cmtbl[i].hash = hashpjw( CHAR_cmtbl[i].magicname );
			}
		}
	}
	fclose(fp);
	return TRUE;
}

#endif
