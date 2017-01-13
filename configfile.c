#define __CONFIGFILE_C__
#include "version.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "util.h"
//#include "configfile.h"
//ttom
#include "lssproto_util.h"
#include "configfile.h"
#include "net.h"
//ttom end
#include "npcutil.h"
// Arminius 7.12 login announce
#include "char.h"
#include "char_data.h"
// CoolFish: add 
#include "lssproto_serv.h"

#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif
#ifdef _UNTEXT_TALK
int textcnt=0;
char untext[100][64];
#endif

/* 涩烂毛忡  允月厌瞻   */
typedef struct tagConfig
{
    /*皿夫弘仿丞  (愤  读卞菲户凶中仃升引分蛲  */
  char    progname[8];
  char    configfilename[32]; /* config白央奶伙   */
    unsigned int debuglevel;   /* 犯田永弘伊矛伙 */
  unsigned int  usememoryunit;    /*丢乒伉及交瓦永玄扔奶术 */
  unsigned int  usememoryunitnum; /*丢乒伉及交瓦永玄醒 */
  char    asname[32];         /*失市它件玄扔□田及  蟆*/
  unsigned short  acservport; /*失市它件玄扔□田及禾□玄  */
  char    acpasswd[32];       /*失市它件玄扔□田尺及由旦伐□玉*/
  char    gsnamefromas[32];   /*
                                 * 失市它件玄扔□田井日苇尹月
                                 * 必□丞扔□田午仄化及  蟆
                                 */
                                 
    // Arminius 7.24 manor pk
  char gsid[32];	// game server chinese id
  unsigned short allowmanorpk;	// is this server allow manor pk
  unsigned short port;        /* 扔□田及谨切熬仃禾□玄 */
	int				servernumber;	/* 必□丞扔□田及  寞 */
	int				reuseaddr;	/* Address already used... 互鞅引日卅中凛及凶户卞 */
  int             do_nodelay;     /* TCP_NODELAY 卞允月井升丹井 */
  int             log_write_time; /* 踏五仇心夫弘毛允月井升丹井［ */
  int             log_io_time;    /* I/O蝈  及凛棉反井月井升丹井［ */
  int             log_game_time;  /* 必□丞及质  蝈  及凛棉毛反井月 */
  int             log_netloop_faster; /* netloop_faster 及夫弘 */
	int				saacwritenum;	/* 失市它件玄扔□田尺及窒谛  卞write允月井 */
	int				saacreadnum;	/* 失市它件玄扔□田井日及dispatch 毛窒荚允月井 */
  unsigned short fdnum;           /*升木分仃戊生弁扑亦件毛忡  允月井 */
  unsigned int   playercharnum;
  unsigned int   othercharnum;    /*  公及职及平乓仿及醒  */
  unsigned int objnum;            /* 左皮斥尼弁玄及    醒*/
  unsigned int   petcharnum;   	/* 矢永玄及醒    */
  unsigned int itemnum;           /* 失奶  丞及    醒*/
  unsigned int battlenum;         /* 田玄伙及    */
  unsigned int battleexp;         /* 田玄伙及    */
  char    topdir[32];         	/* 玄永皿犯奴伊弁玄伉  */
  char    mapdir[32];         	/* 穴永皿犯奴伊弁玄伉  */
  char    maptilefile[32];    	/* 穴永皿涩烂白央奶伙  */
  char    battlemapfile[32];    	/* 田玄伙穴永皿涩烂白央奶伙  */
  char    itemfile[32];       	/* 失奶  丞涩烂白央奶伙  */
  char    invfile[32];        	/*   衬涩烂白央奶伙  */
  char    appearfile[32];     	/* 请蜇匏  涩烂白央奶伙  */
	char	titlenamefile[32];		/* 惫寞白央奶伙   */
	char	titleconfigfile[32];	/* 惫寞涩烂白央奶伙   */
	char	encountfile[32];		/* 巨件市它件玄涩烂白央奶伙   */
	char	enemybasefile[32];		/* 衬湘  涩烂白央奶伙   */
	char	enemyfile[32];			/* 衬涩烂白央奶伙   */
	char	groupfile[32];			/* 弘伙□皿涩烂白央奶伙   */
	char	magicfile[32];			/* 热诸涩烂白央奶伙   */
#ifdef _ATTACK_MAGIC
  char    attmagicfile[32];       // 攻击性咒术
#endif

	char	petskillfile[32];		/* 矢永玄  热诸涩烂白央奶伙   */
  char    itematomfile[32];       /* 失奶  丞及笺    白央奶伙 */
  char    effectfile[32];     	/* 梢请涩烂白央奶伙  */
  char    quizfile[32];     		/* 弁奶术涩烂白央奶伙  */
  char    lsgenlog[32];       /*扔□田及lsgen 失它玄皿永玄白央奶伙  */
  char    storedir[128];       /*旦玄失犯奴伊弁玄伉    */
  char    npcdir[32];         /*NPC及涩烂白央奶伙毛  仁犯奴伊弁玄伉   */
  char    logdir[32];         /*
                               * 夫弘犯奴伊弁玄伉
                               */
  char    logconfname[32];    /*
                               * 夫弘涩烂白央奶伙
                               */
  char	chatmagicpasswd[32];	/* 民乓永玄  芊由旦伐□玉 */
#ifdef _STORECHAR
  char	storechar[32];
#endif
	unsigned int 	chatmagiccdkeycheck;	/* 民乓永玄  芊匹CDKEY毛民尼永弁允月井 */
  unsigned int    filesearchnum;     /*白央奶伙毛腹绸匹五月白央奶伙及醒*/
  unsigned int    npctemplatenum;     /*NPC及  件皿伊□玄白央奶伙及醒*/
  unsigned int    npccreatenum;       /*NPC及戏遣白央奶伙及醒*/
  unsigned int    walksendinterval;   /* 汹仁及毛霜月棉厥 */
  unsigned int    CAsendinterval_ms;     /* CA毛霜月棉厥 (ms)*/
  unsigned int    CDsendinterval_ms;     /* CD毛霜月棉厥 (ms)*/
  unsigned int    Onelooptime_ms;     	/* 1伙□皿卞井仃月凛棉 */
	unsigned int	Petdeletetime;		/* 矢永玄互壅  允月凛棉 */
	unsigned int	Itemdeletetime;		/* 失奶  丞互壅  允月凛棉 */
    /* 夫弘奶件  及平乓仿及本□皮毛允月棉厥 */
  unsigned int    CharSavesendinterval;
  unsigned int    addressbookoffmsgnum;  /*
                                          * 失玉伊旦皮永弁卞左白仿奶件
                                          * 丢永本□斥毛
                                          * 窒丢永本□斥酸六月井
                                          */
  unsigned int    protocolreadfrequency;  /*
                                           * 皿夫玄戊伙毛窒立伉
                                           * 卞  戈井
                                           */
  unsigned int    allowerrornum;          /*
                                           * 巨仿□毛窒蜊引匹袱允井
                                           */
  unsigned int    fengerrornum;
  unsigned int    loghour;          		/*
                                           * 夫弘毛忡绣允月凛对  ＂凛
                                           */
  unsigned int    battledebugmsg;    		/*
                                           * 田玄伙  及犯田永弘丢永本□斥毛请允井［ㄟ卅日请今卅中
                                           */
  //ttom add this because the second had this                                         
  unsigned int    encodekey;              
  unsigned int    acwbsize;             
  unsigned int    acwritesize;
  unsigned int    ErrUserDownFlg;
  //ttom end
#ifdef _GMRELOAD
	char	gmsetfile[32];				/* GM帐号、权限设定档 */
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	char profession[32];
#endif
#ifdef _ITEM_QUITPARTY
    char itemquitparty[32];
#endif

#ifdef _DEL_DROP_GOLD	
	unsigned int	Golddeletetime;
#endif
#ifdef _NEW_PLAYER_CF
	int	newplayertrans;
	int	newplayerlv;
	int	newplayergivepet[5];
	int newplayergiveitem[15];
	int	newplayerpetlv;
	int newplayergivegold;
	int ridepetlevel;
#ifdef _VIP_SERVER
	int	newplayerpetvip;
#endif
#endif
#ifdef _USER_EXP_CF
	char	expfile[64];
#endif
#ifdef _UNLAW_WARP_FLOOR
	int	unlawwarpfloor[10];
#endif
#ifdef _WATCH_FLOOR
	int	watchfloor[6];
#endif
#ifdef _BATTLE_FLOOR
	int	battlefloor[6];
#endif
#ifdef _UNREG_NEMA
	char	unregname[9][16];
#endif
#ifdef _TRANS_LEVEL_CF
	int chartrans;
	int pettrans;
	int yblevel;
	int maxlevel;
#endif
#ifdef _POINT
int point;
	int transpoint[10];
#endif
#ifdef _VIP_SERVER
	int vippoint;
	int safemode;
#endif
#ifdef _PET_UP
	int petup;
#endif
#ifdef _LOOP_ANNOUNCE
	char loopannouncepath[32];
	int loopannouncetime;
	char loopannounce[10][1024];
	int loopannouncemax;
#endif
#ifdef _SKILLUPPOINT_CF
	int skup;
#endif
#ifdef _RIDELEVEL
	int ridelevel;
#endif
#ifdef _REVLEVEL
	int revlevel;
#endif
#ifdef _NEW_PLAYER_RIDE
	int npride;
#endif
#ifdef _FIX_CHARLOOPS
	int charloops;
#endif
#ifdef _PLAYER_ANNOUNCE
	int pannounce;
#endif
#ifdef _PLAYER_MOVE
	int pmove;
#endif
	int recvbuffer;
	int sendbuffer;
	int recvlowatbuffer;
	int runlevel;
#ifdef _SHOW_VIP_CF
	int showvip;
#endif
	int BattlePoint[4];
#ifdef _ALL_TALK
	int TheWorldTrans;
	int TheWorldFame;
	int TheWorldTime;
	int TheWorldCnt;
#endif
#ifdef _NOWEN_EV
	int NowEvent[10];
	int EndEvent[10];
#endif
#ifdef _PLAYER_NUM
	int playernum;
	char playerbase[10];
#endif
#ifdef _BATTLE_GOLD
	int battlegold;
#endif
#ifdef _ANGEL_TIME
	int angelplayertime;
	int angelplayermun;
#endif
#ifdef _RIDEMODE_20
	int ridemode;
#endif
#ifdef _FM_POINT_PK
	int fmpointpk;
#endif
#ifdef _ENEMY_ACTION
	int	enemyact;
#endif
#ifdef _FUSIONBEIT_TRANS
	char	fusionbeittrans;
#endif
#ifdef _CHECK_PEPEAT
	int	CheckRepeat;
#endif
	int	cpuuse;
#ifdef _FM_JOINLIMIT
	int	joinfamilytime;
#endif
#ifdef _JK_CF_DELPETITEM     //删除CF指定物品和宠物
	char DelPet[256];
	char DelItem[256];
#endif
#ifdef _MAP_HEAL
	int MapHeal[10];
#endif
#ifdef _DIY_INSLAY
	int InslayNum;
#endif
	int TradeTax;
#ifdef _VIP_RIDE
	int VipMsgType;
#endif
#ifdef _VIP_BATTLE_EXP
	int VipBattleExp;
#endif
	int ItemPoolBug;
	int SameIpLogin;
	int PetRideTrans;
#ifdef _LUCK_MAN
	int lucktime;
	char luckitem[256];
#endif
#ifdef _QUESTION_ONLINE
	int questiontime;
	char questionitem[50];
#endif
#ifdef _NO_STREET_MAP
	char nostreetmap[50];
#endif
#ifdef _STREET_FAX
	char streetfax[64];
#endif
	char fmwartime[10];
#ifdef _JZ_NEWSCRIPT_LUA
	char luafile[256];
#endif
#ifdef _TRANS7_POINT
	int trans7point;
#endif
#ifdef _NOJOB_PK
	int nojobpkmap;
#endif
#ifdef _NO_ATTACK
	int atttime;
	int attsafetime;
	int attcnt;
	int latetime;
	int attdmetime;
	int attdmecnt;
#endif
	char noattip[5][18];
#ifdef _NO_TRANS_ANGLE
	int trans6angle;
#endif
#ifdef _PET_BUG
	int petpoint[4];
#endif
#ifdef _VIGOR_SYS
	int vigortime[8];
	int vigormax;
#endif
#ifdef _AUTO_PK
	int autopktime;
	int autopkminnum;
	int autopkbattletime;
	int autopkminlv;
	int autopkvigorpknum;
	int autopkvigorpkcnt;
#endif
#ifdef	_BATTLEMAP_CTRL
	char BattleTime[10][20];
#endif
#ifdef	_FM_SERVERNO_SYS
	int fmserverno;
	char fmservermsg[256];
#endif
#ifdef	_ZHIPIAO_SYSTEM
	int zhipiao;
	int piaotime;
#endif
#ifdef	_FMWAR_MSG	
	char fmwarmsg[256];
#endif
#ifdef	_FM_WELFARE	
	char fmwelfare[10][20];
#endif
#ifdef	_FMZUZHANG_MSG	
	char fmzuzhang[256];
#endif
#ifdef	_PETMAIL_LV	
	int petmaillv;
	int petmailcf;
#endif
#ifdef	_FMWAR_PLAYERNUM	
	int fmwarplayernum;
#endif
#ifdef _WAN_FIX
	int vsflg;
#endif
#ifdef	_WARNPC_CTRL
	char partymap[128];
	char mapsameip[32];
#endif
#ifdef	_NO_DAOLUAN
	int streettrn;
	int talktrn;
	int talklv;
	char talkmsg[128];
	char talkname[128];
#endif
#ifdef _NO_FULLPLAYER_ATT
	int nofullplayer;
	int nofull2player;
	int nocdkeyplayer;
	int nocdkeymode;
	int nocdkeytype;
	int nofulltime;
	int fengtype;
	int nofullendplayer;
	int nofullendtime;
	int manrennum;
	int bigbao;
	int bigbao2;
#endif
#ifdef _ATTREVERSE_FIX
	int attreverse;
#endif
#ifdef	_NEWFM_GOLD
	int newfmgold;
#endif
#ifdef	_MERGE_TIME
	int mergetime;
#endif
#ifdef	_PETUP_GET_EXP
	int petupgetexp;
#endif
#ifdef	_MM_NO_JIAOBEN
	int nommjiaoben;
	int editbasetime;
#endif
#ifdef	_DP_140_160_MODE
	int newdpmode;
#endif
#ifdef	_LOOK_STREET_TIME
	int lookstreettime;
#endif
#ifdef	_BATTLE_EQUIT_OTHER
	int battleequitother;
#endif
#ifdef	_PK_LUCK_MAN
	char pkluckfloor[256];
	char pkluckmapname[10][32];
	char pklucktime[10];
	char pklucknum[10];
	char pkluckbuf[512];
	int pklucklevel;
	int pkluckbattleturn;
#endif
#ifdef	_NO_DAOLUAN
	char talkfloor[512];
#endif
#ifdef	_SUPERMAN_FIX
	int supermanpoint;
#endif
#ifdef	_PICKUP_ITEM_OTHER
	int pickupitem;
	char pickitemid[512];
#endif
#ifdef	_FM_LIAOLI
	int fmliaolitype;
#endif
#ifdef	_TRADE_ITEM_FIX
	int tradeitemfix;
#endif
#ifdef	_PETMAIL_TIME
	int petmailtime;
#endif
	int randmin;
	int randmax;
#ifdef	_GJS_TYPE
	int gjstype;
#endif
	int ticketcf;
	char ticketmsg[256];
	int satype;
#ifdef	_BATTLEEND_FIX
	int battleendmode;
#endif
#ifdef	_BIG_POOL_TYPE
	int petpooltype;
	int itempooltype;
#endif
	int nostaybattle;
	int rightnum;
	int rightmode;
	int errormode;
#ifdef	_FLOOR_PARTY_CTRL
	char partyfloor[128];
#endif
	int battletimeout;
	int reloadnpctime;
	int reloadnpctype;
	int offlinebattletime;
	int offlinecf;
	int offlinemaxnum;
	int offlinejqmmaxnum;
	int kongtype;
	int jifenbaitan;
	char streetitemunname[2048];
	int nopkmap;
	int fmbufftrans;
	char nompmagic[1024];
	
	int loginjqmtype;
	char fmmagicmprate[2][10];
	int sameipnum;
	int samejqmnum;
	int samejqmallnum;
	
	int mmtype;
	char mmmsg[1024];
	
	int locktype;
	int alltalkpoint;
#ifdef	_NO_RIDE_ID
	char norideid[1024];
#endif

	int logintype;



#ifdef _PETTRANS_RANGE
	int pettransrangex;
	int pettransrangey;
	int pettransrangez;
#endif

#ifdef _ITEM_LUA
char itemluafile[256];
#endif
#ifdef _SAME_IP_ONLINE_NUM
	int sameiponlinenum;
#endif
#ifdef _CHECK_SEVER_IP
	char serverip[256];
#endif
#ifdef _CAX_ESC_REPORT
	int reportitem[5]; /*每日签到赠送东西*/
	int vipreportitem[5]; /*每日签到会员赠送东西*/
	int reportlv;//签到等级
	int reportta;//签到转数
#endif
#ifdef _PETSKILL_SHOP_LUA
	char freepetskillshoppath[256];
#endif
#ifdef _CAX_FAMEBUTTON
	int	famebutton;
#endif
#ifdef _PET_TRANS_ABILITY
	int	pettransability;
	int	pettransability1;
	int	pettransability2;
	int	pettransability3;
#endif
#ifdef _PET_3TRANS
  int MMPETID1;
  int MMPETID2;
#endif
#ifdef _NO_MAGIC
  char nomagicmap[128];
#endif
#ifdef _illegalpetskill_CF
	int IllegalPetskill[10];
#endif
#ifdef _CAX_PET_EVOLUTION
	char petevotyb;
	int  petevotyb1;
#endif
#ifdef _TRANS_7_COLOR
//	int TranColor;
	char ping;
	char petsummon;
#endif
#ifdef _CAX_PET_ITEMRIDE
	char petitemride;
#endif
#ifdef _DAMMAGE_CALC
	int dammagecalc;
#endif
#ifdef _CAX_LNS_NLSUOXU
  int mapstart;
#endif
#ifdef _CAX_FAME_KOUFEI
	int famekoufei;
#endif
#ifdef _CAX_ITEM_ADDEXP
	int exptime;
#endif
#ifdef _SHARE_EXP
	int expshare;
#endif
#ifdef _TEAM_ADDEXP
	int teamaddexp;
#endif
#ifdef _NO_STW_ENEMY
	int nostwenemy;
	int nostwenemypoint;
#endif
#ifdef _DEX_FIX		 //乱敏系数
int dexfixper;
#endif
#ifdef _FM_EXP_ADD
  unsigned int fmaddexp;
#endif

}Config;

Config config;

#ifdef _USER_EXP_CF
int NeedLevelUpTbls[200];
#endif

/*
 *  戊件白奴弘白央奶伙毛  戈凛卞银丹厌瞻
 *  xxxx=yyyy 心凶中卅及毛  戈
 */

typedef struct tagReadConf
{
    char    name[32];       /*xxxx卞丐凶月袄*/

    /*戚及2勾反NULL毛  木月午窒手质  仄卅中*/
    char  *charvalue;      /*yyyy毛公及引引医  允月凛及医  燮*/
    size_t  charsize;       /*charvalue及扔奶术*/

    /*
     *     酷  午仄化=及  互 "ON"分匀凶日 intvalue 卞反1毛医  允月
     * 公木动陆反 atoi 及瑛绊
     */
    void*    value;       /*yyyy毛  晶允月医  允月凛及医  燮*/
    CTYPE    valuetype;
}ReadConf;

ReadConf readconf[]=
{
    { "debuglevel"      , NULL ,0 , (void*)&config.debuglevel      ,CHAR},

    { "usememoryunit"   , NULL ,0 , (void*)&config.usememoryunit   ,INT},
    { "usememoryunitnum", NULL ,0 , (void*)&config.usememoryunitnum,INT},

    { "acserv",			config.asname,sizeof(config.asname) ,NULL , 0},
    { "acservport",		NULL ,0 , (void*)&config.acservport     ,SHORT},
    { "acpasswd",		config.acpasswd,sizeof( config.acpasswd),NULL,0},
    { "gameservname",	config.gsnamefromas,sizeof(config.gsnamefromas), NULL,0},

    // Arminius 7.24 manor pk
    { "gameservid", config.gsid, sizeof(config.gsid), NULL, 0}, 
    { "allowmanorpk", NULL, 0, (void*)&config.allowmanorpk, SHORT},

    { "port",			NULL ,0 , (void*)&config.port           ,SHORT},
    { "servernumber",	NULL ,0 , (void*)&config.servernumber           ,INT},

    { "reuseaddr",			NULL ,0 , (void*)&config.reuseaddr  ,		INT},
    { "nodelay",			NULL , 0 , (void*)&config.do_nodelay ,		INT},
    { "log_write_time", 	NULL, 0 , (void*)&config.log_write_time,	INT},
    { "log_io_time", 		NULL, 0 , (void*)&config.log_io_time, 		INT},
    { "log_game_time",		NULL, 0 , (void*)&config.log_game_time,		INT},
    { "log_netloop_faster", NULL,0,(void*)&config.log_netloop_faster,	INT},
    { "saacwritenum",		NULL,0,(void*)&config.saacwritenum, 		INT},
    { "saacreadnum",		NULL,0,(void*)&config.saacreadnum, 			INT},
    { "fdnum",				NULL ,0 , (void*)&config.fdnum,				SHORT},
    { "playernum",				NULL ,0 , (void*)&config.playercharnum,		INT},
    { "petnum",				NULL ,0 , (void*)&config.petcharnum,		INT},
    { "othercharnum",		NULL ,0 , (void*)&config.othercharnum,		INT},

    { "objnum",			NULL ,0 , (void*)&config.objnum,				INT},
    { "itemnum",		NULL ,0 , (void*)&config.itemnum,				INT},
    { "battlenum",		NULL ,0 , (void*)&config.battlenum,				INT},
    { "battleexp",		NULL ,0 , (void*)&config.battleexp,				INT},
    { "topdir"          , config.topdir,sizeof(config.topdir),NULL,0},
    { "mapdir"          , config.mapdir,sizeof(config.mapdir),NULL,0},
    { "maptilefile"     , config.maptilefile,sizeof(config.maptilefile),NULL,0},
    { "battlemapfile"   , config.battlemapfile,sizeof(config.battlemapfile),NULL,0},

#ifdef _ITEMSET6_TXT
	{ "itemset6file",	config.itemfile,	sizeof(config.invfile),	NULL,	0},
#else
#ifdef _ITEMSET5_TXT
	{ "itemset5file",	config.itemfile,	sizeof(config.invfile),	NULL,	0},
#else
#ifdef _ITEMSET4_TXT
	{ "itemset4file"  , config.itemfile,sizeof(config.invfile),NULL,0},
#else
#ifdef _ITEMSET3_ITEM
	{ "itemset3file"  , config.itemfile,sizeof(config.invfile),NULL,0},
#endif
#endif
#endif
#endif
  { "invinciblefile"  , config.invfile,sizeof(config.invfile),NULL,0},
  { "appearpositionfile"  , config.appearfile,sizeof(config.appearfile),NULL,0},
	{ "titlenamefile", config.titlenamefile, sizeof( config.titlenamefile),NULL,0},
	{ "titleconfigfile", config.titleconfigfile, sizeof( config.titleconfigfile),NULL,0},
	{ "encountfile", config.encountfile, sizeof( config.encountfile),NULL,0},
	{ "enemyfile", config.enemyfile, sizeof( config.enemyfile),NULL,0},
	{ "enemybasefile", config.enemybasefile, sizeof( config.enemybasefile),NULL,0},
	{ "groupfile", config.groupfile, sizeof( config.groupfile),NULL,0},
	{ "magicfile", config.magicfile, sizeof( config.magicfile),NULL,0},
#ifdef _ATTACK_MAGIC
	{ "attmagicfile" , config.attmagicfile , sizeof( config.attmagicfile )  , NULL , 0 },
#endif

#ifdef _PETSKILL2_TXT
	{ "petskillfile2", config.petskillfile, sizeof( config.petskillfile),NULL,0},
#else
	{ "petskillfile1", config.petskillfile, sizeof( config.petskillfile),NULL,0},
#endif

    { "itematomfile" , config.itematomfile, sizeof( config.itematomfile),NULL,0},
    { "effectfile"  , config.effectfile,sizeof(config.effectfile),NULL,0},
    { "quizfile"  , config.quizfile,sizeof(config.quizfile),NULL,0},

    { "lsgenlogfilename", config.lsgenlog,sizeof(config.lsgenlog),NULL,0},
#ifdef _GMRELOAD
	{ "gmsetfile", config.gmsetfile, sizeof( config.gmsetfile),NULL,0},
#endif

    { "storedir"        ,config.storedir,sizeof(config.storedir),NULL,0},
    { "npcdir"          ,config.npcdir,sizeof(config.npcdir),NULL,0},
    { "logdir"          ,config.logdir,sizeof(config.logdir),NULL,0},
    { "logconfname"     ,config.logconfname,sizeof(config.logconfname),NULL,0},
    { "chatmagicpasswd", config.chatmagicpasswd, sizeof( config.chatmagicpasswd),NULL,0},
#ifdef _STORECHAR
    { "storechar", config.storechar, sizeof( config.storechar),NULL,0},
#endif
    { "chatmagiccdkeycheck",  NULL,0, &config.chatmagiccdkeycheck,INT},
    { "filesearchnum",  NULL,0, &config.filesearchnum,INT},
    { "npctemplatenum",  NULL,0, &config.npctemplatenum,INT},
    { "npccreatenum",    NULL,0, &config.npccreatenum,INT},
    { "walkinterval" ,NULL,0,(void*)&config.walksendinterval,INT},
    { "CAinterval" ,NULL,0,(void*)&config.CAsendinterval_ms,INT},
    { "CDinterval" ,NULL,0,(void*)&config.CDsendinterval_ms,INT},
    { "CharSaveinterval" ,NULL,0,(void*)&config.CharSavesendinterval,INT},
    { "Onelooptime" ,NULL,0,(void*)&config.Onelooptime_ms,INT},
    { "Petdeletetime" ,NULL,0,(void*)&config.Petdeletetime,INT},
    { "Itemdeletetime" ,NULL,0,(void*)&config.Itemdeletetime,INT},
	 { "addressbookoffmesgnum" ,NULL,0,
      (void*)&config.addressbookoffmsgnum,INT},

    { "protocolreadfrequency" ,NULL,0,
      (void*)&config.protocolreadfrequency,INT},

    { "allowerrornum" ,NULL,0,(void*)&config.allowerrornum,INT},
    { "fengerrornum" ,NULL,0,(void*)&config.fengerrornum,INT},
    { "loghour" ,NULL,0,(void*)&config.loghour,INT},
    { "battledebugmsg" ,NULL,0,(void*)&config.battledebugmsg,INT},
    //ttom add because the second had
    { "encodekey" ,NULL,0,(void*)&config.encodekey,INT},
    { "acwritesize" ,NULL,0,(void*)&config.acwritesize,INT},
    { "acwbsize" ,NULL,0,(void*)&config.acwbsize,INT},
    { "erruser_down" ,NULL,0,(void*)&config.ErrUserDownFlg,INT},    

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
    { "profession",		config.profession, sizeof(config.profession) ,NULL , 0},
#endif

#ifdef _ITEM_QUITPARTY
    { "itemquitparty",	config.itemquitparty, sizeof(config.itemquitparty) ,NULL , 0},
#endif

#ifdef _DEL_DROP_GOLD
	{ "Golddeletetime" ,NULL,0,(void*)&config.Golddeletetime,	INT},
#endif

#ifdef _MAP_HEAL
	{ "MAPHEAL1" ,NULL,0,(void*)&config.MapHeal[0],	INT},
	{ "MAPHEAL2" ,NULL,0,(void*)&config.MapHeal[1],	INT},	
	{ "MAPHEAL3" ,NULL,0,(void*)&config.MapHeal[2],	INT},	
	{ "MAPHEAL4" ,NULL,0,(void*)&config.MapHeal[3],	INT},	
	{ "MAPHEAL5" ,NULL,0,(void*)&config.MapHeal[4],	INT},
	{ "MAPHEAL6" ,NULL,0,(void*)&config.MapHeal[5],	INT},
	{ "MAPHEAL7" ,NULL,0,(void*)&config.MapHeal[6],	INT},	
	{ "MAPHEAL8" ,NULL,0,(void*)&config.MapHeal[7],	INT},	
	{ "MAPHEAL9" ,NULL,0,(void*)&config.MapHeal[8],	INT},	
	{ "MAPHEAL10" ,NULL,0,(void*)&config.MapHeal[9],	INT},
#endif

#ifdef _NEW_PLAYER_CF
	{ "TRANS" ,NULL,0,(void*)&config.newplayertrans,	INT},
	{ "LV" ,NULL,0,(void*)&config.newplayerlv,	INT},	
	{ "PET1" ,NULL,0,(void*)&config.newplayergivepet[1],	INT},
	{ "PET2" ,NULL,0,(void*)&config.newplayergivepet[2],	INT},	
	{ "PET3" ,NULL,0,(void*)&config.newplayergivepet[3],	INT},	
	{ "PET4" ,NULL,0,(void*)&config.newplayergivepet[4],	INT},	
	{ "ITEM1" ,NULL,0,(void*)&config.newplayergiveitem[0],	INT},
	{ "ITEM2" ,NULL,0,(void*)&config.newplayergiveitem[1],	INT},	
	{ "ITEM3" ,NULL,0,(void*)&config.newplayergiveitem[2],	INT},	
	{ "ITEM4" ,NULL,0,(void*)&config.newplayergiveitem[3],	INT},	
	{ "ITEM5" ,NULL,0,(void*)&config.newplayergiveitem[4],	INT},
	{ "ITEM6" ,NULL,0,(void*)&config.newplayergiveitem[5],	INT},	
	{ "ITEM7" ,NULL,0,(void*)&config.newplayergiveitem[6],	INT},	
	{ "ITEM8" ,NULL,0,(void*)&config.newplayergiveitem[7],	INT},	
	{ "ITEM9" ,NULL,0,(void*)&config.newplayergiveitem[8],	INT},
	{ "ITEM10" ,NULL,0,(void*)&config.newplayergiveitem[9],	INT},	
	{ "ITEM11" ,NULL,0,(void*)&config.newplayergiveitem[10],	INT},	
	{ "ITEM12" ,NULL,0,(void*)&config.newplayergiveitem[11],	INT},	
	{ "ITEM13" ,NULL,0,(void*)&config.newplayergiveitem[12],	INT},	
	{ "ITEM14" ,NULL,0,(void*)&config.newplayergiveitem[13],	INT},	
	{ "ITEM15" ,NULL,0,(void*)&config.newplayergiveitem[14],	INT},
	{ "PETLV" ,NULL,0,(void*)&config.newplayerpetlv,	INT},	
	{ "GOLD" ,NULL,0,(void*)&config.newplayergivegold,	INT},
	{ "RIDEPETLEVEL" ,NULL,0,(void*)&config.ridepetlevel,	INT},
#ifdef _VIP_SERVER
	{ "GIVEVIPPOINT" ,NULL,0,(void*)&config.newplayerpetvip,	INT},
	{ "SAFEMODE" ,NULL,0,(void*)&config.safemode,	INT},
#endif
#endif

#ifdef _USER_EXP_CF
	{ "USEREXP", config.expfile, sizeof( config.expfile),NULL,0},
#endif

#ifdef _UNLAW_WARP_FLOOR
	{ "FLOOR1" ,NULL,0,(void*)&config.unlawwarpfloor[0],	INT},
	{ "FLOOR2" ,NULL,0,(void*)&config.unlawwarpfloor[1],	INT},	
	{ "FLOOR3" ,NULL,0,(void*)&config.unlawwarpfloor[2],	INT},	
	{ "FLOOR4" ,NULL,0,(void*)&config.unlawwarpfloor[3],	INT},	
	{ "FLOOR5" ,NULL,0,(void*)&config.unlawwarpfloor[4],	INT},
	{ "FLOOR6" ,NULL,0,(void*)&config.unlawwarpfloor[5],	INT},
	{ "FLOOR7" ,NULL,0,(void*)&config.unlawwarpfloor[6],	INT},	
	{ "FLOOR8" ,NULL,0,(void*)&config.unlawwarpfloor[7],	INT},	
	{ "FLOOR9" ,NULL,0,(void*)&config.unlawwarpfloor[8],	INT},	
	{ "FLOOR10" ,NULL,0,(void*)&config.unlawwarpfloor[9],	INT},
#endif

#ifdef _WATCH_FLOOR
	{ "WATCHFLOOR" ,NULL,0,(void*)&config.watchfloor[0],	INT},
	{ "WATCHFLOOR1" ,NULL,0,(void*)&config.watchfloor[1],	INT},
	{ "WATCHFLOOR2" ,NULL,0,(void*)&config.watchfloor[2],	INT},	
	{ "WATCHFLOOR3" ,NULL,0,(void*)&config.watchfloor[3],	INT},	
	{ "WATCHFLOOR4" ,NULL,0,(void*)&config.watchfloor[4],	INT},	
	{ "WATCHFLOOR5" ,NULL,0,(void*)&config.watchfloor[5],	INT},
#endif

#ifdef _BATTLE_FLOOR
	{ "BATTLEFLOOR" ,NULL,0,(void*)&config.battlefloor[0],	INT},
	{ "BATTLEFLOOR1" ,NULL,0,(void*)&config.battlefloor[1],	INT},
	{ "BATTLEFLOOR2" ,NULL,0,(void*)&config.battlefloor[2],	INT},	
	{ "BATTLEFLOOR3" ,NULL,0,(void*)&config.battlefloor[3],	INT},	
	{ "BATTLEFLOOR4" ,NULL,0,(void*)&config.battlefloor[4],	INT},	
	{ "BATTLEFLOOR5" ,NULL,0,(void*)&config.battlefloor[5],	INT},
#endif
#ifdef _JK_CF_DELPETITEM        //删除CF指定物品和宠物
		{ "DELPET", config.DelPet, sizeof( config.DelPet),NULL,0},
		{ "DELITEM", config.DelItem, sizeof( config.DelItem),NULL,0},
#endif
#ifdef _UNREG_NEMA
	{ "NAME1" ,config.unregname[0], sizeof( config.unregname[0]),NULL,0},
	{ "NAME2" ,config.unregname[1], sizeof( config.unregname[1]),NULL,0},
	{ "NAME3" ,config.unregname[2], sizeof( config.unregname[2]),NULL,0},
	{ "NAME4" ,config.unregname[3], sizeof( config.unregname[3]),NULL,0},
	{ "NAME5" ,config.unregname[4], sizeof( config.unregname[4]),NULL,0},
	{ "NAME6" ,config.unregname[5], sizeof( config.unregname[5]),NULL,0},
	{ "NAME7" ,config.unregname[6], sizeof( config.unregname[6]),NULL,0},
	{ "NAME8" ,config.unregname[7], sizeof( config.unregname[7]),NULL,0},
	{ "NAME9" ,config.unregname[8], sizeof( config.unregname[8]),NULL,0},
#endif
#ifdef _TRANS_LEVEL_CF
	{ "CHARTRANS" ,NULL,0,(void*)&config.chartrans,	INT},
	{ "PETTRANS" ,NULL,0,(void*)&config.pettrans,	INT},	
	{ "LEVEL" ,NULL,0,(void*)&config.yblevel,	INT},	
	{ "MAXLEVEL" ,NULL,0,(void*)&config.maxlevel,	INT},	
#endif
#ifdef _POINT
	{ "POINT" ,NULL,0,(void*)&config.point,	INT},
	{ "TRANS0" ,NULL,0,(void*)&config.transpoint[0],	INT},	
	{ "TRANS1" ,NULL,0,(void*)&config.transpoint[1],	INT},	
	{ "TRANS2" ,NULL,0,(void*)&config.transpoint[2],	INT},	
	{ "TRANS3" ,NULL,0,(void*)&config.transpoint[3],	INT},	
	{ "TRANS4" ,NULL,0,(void*)&config.transpoint[4],	INT},	
	{ "TRANS5" ,NULL,0,(void*)&config.transpoint[5],	INT},	
	{ "TRANS6" ,NULL,0,(void*)&config.transpoint[6],	INT},	
	{ "TRANS7" ,NULL,0,(void*)&config.transpoint[7],	INT},	
	{ "TRANS8" ,NULL,0,(void*)&config.transpoint[8],	INT},
	{ "TRANS9" ,NULL,0,(void*)&config.transpoint[9],	INT},
#endif

#ifdef _NOWEN_EV
		{ "endevent01"      , NULL ,0 , (void*)&config.EndEvent[0]      ,INT},
		{ "endevent02"      , NULL ,0 , (void*)&config.EndEvent[1]      ,INT},
		{ "endevent03"      , NULL ,0 , (void*)&config.EndEvent[2]      ,INT},
		{ "endevent04"      , NULL ,0 , (void*)&config.EndEvent[3]      ,INT},
		{ "endevent05"      , NULL ,0 , (void*)&config.EndEvent[4]      ,INT},
		{ "endevent06"      , NULL ,0 , (void*)&config.EndEvent[5]      ,INT},
		{ "endevent07"      , NULL ,0 , (void*)&config.EndEvent[6]      ,INT},
		{ "endevent08"      , NULL ,0 , (void*)&config.EndEvent[7]      ,INT},
		{ "endevent09"      , NULL ,0 , (void*)&config.EndEvent[8]      ,INT},
		{ "endevent10"      , NULL ,0 , (void*)&config.EndEvent[9]      ,INT},
		{ "nowevent01"      , NULL ,0 , (void*)&config.NowEvent[0]      ,INT},
		{ "nowevent02"      , NULL ,0 , (void*)&config.NowEvent[1]      ,INT},
		{ "nowevent03"      , NULL ,0 , (void*)&config.NowEvent[2]      ,INT},
		{ "nowevent04"      , NULL ,0 , (void*)&config.NowEvent[3]      ,INT},
		{ "nowevent05"      , NULL ,0 , (void*)&config.NowEvent[4]      ,INT},
		{ "nowevent06"      , NULL ,0 , (void*)&config.NowEvent[5]      ,INT},
		{ "nowevent07"      , NULL ,0 , (void*)&config.NowEvent[6]      ,INT},
		{ "nowevent08"      , NULL ,0 , (void*)&config.NowEvent[7]      ,INT},
		{ "nowevent09"      , NULL ,0 , (void*)&config.NowEvent[8]      ,INT},
		{ "nowevent10"      , NULL ,0 , (void*)&config.NowEvent[9]      ,INT},
#endif
		{ "battlepoint1"      , NULL ,0 , (void*)&config.BattlePoint[0]      ,INT},
		{ "battlepoint2"      , NULL ,0 , (void*)&config.BattlePoint[1]      ,INT},
		{ "battlepoint3"      , NULL ,0 , (void*)&config.BattlePoint[2]      ,INT},
		{ "battlepoint4"      , NULL ,0 , (void*)&config.BattlePoint[3]      ,INT},
#ifdef _ALL_TALK	
		{ "THEWORLDTRANS"      , NULL ,0 , (void*)&config.TheWorldTrans      ,INT},
		{ "THEWORLDFAME"      , NULL ,0 , (void*)&config.TheWorldFame      ,INT},
		{ "THEWORLDTIME"      , NULL ,0 , (void*)&config.TheWorldTime      ,INT},
		{ "THEWORLDCNT"      , NULL ,0 , (void*)&config.TheWorldCnt      ,INT},
#endif
#ifdef _DIY_INSLAY
		{ "InslayNum"      , NULL ,0 , (void*)&config.InslayNum      ,INT},
#endif
		{ "TradeTax"      , NULL ,0 , (void*)&config.TradeTax      ,INT},
		{ "VipMsgType"      , NULL ,0 , (void*)&config.VipMsgType      ,INT},
#ifdef _VIP_BATTLE_EXP
		{ "VipBattleExp"      , NULL ,0 , (void*)&config.VipBattleExp      ,INT},
#endif
		{ "ItemPoolBug"      , NULL ,0 , (void*)&config.ItemPoolBug      ,INT},
#ifdef _PET_UP
	{ "PETUP" ,NULL,0,(void*)&config.petup,	INT},
#endif
#ifdef _LOOP_ANNOUNCE
	{ "ANNOUNCEPATH" ,config.loopannouncepath, sizeof( config.loopannouncepath),NULL,0},
	{ "ANNOUNCETIME" ,NULL,0,(void*)&config.loopannouncetime,	INT},
#endif
#ifdef _SKILLUPPOINT_CF
	{ "SKILLUPPOINT" ,NULL,0,(void*)&config.skup,	INT},	
#endif
#ifdef _RIDELEVEL
	{ "RIDELEVEL" ,NULL,0,(void*)&config.ridelevel,	INT},	
#endif
	{ "PETRIDETRANS" ,NULL,0,(void*)&config.PetRideTrans,	INT},	
#ifdef _REVLEVEL
	{ "REVLEVEL" ,NULL,0,(void*)&config.revlevel,	INT},	
#endif
#ifdef _NEW_PLAYER_RIDE
	{ "NPRIDE" ,NULL,0,(void*)&config.npride,	INT},	
#endif
#ifdef _FIX_CHARLOOPS
	{ "CHARLOOPS" ,NULL,0,(void*)&config.charloops,	INT},	
#endif
#ifdef _PLAYER_ANNOUNCE
	{ "PANNOUNCE" ,NULL,0,(void*)&config.pannounce,	INT},
#endif
#ifdef _PLAYER_MOVE
	{ "PMOVE" ,NULL,0,(void*)&config.pmove,	INT},
#endif

	{ "recvbuffer" ,NULL,0,(void*)&config.recvbuffer,	INT},
	{ "sendbuffer" ,NULL,0,(void*)&config.sendbuffer,	INT},
	{ "recvlowatbuffer" ,NULL,0,(void*)&config.recvlowatbuffer,	INT},
	{ "runlevel" ,NULL,0,(void*)&config.runlevel,	INT},
	
#ifdef _SHOW_VIP_CF
	{ "SHOWVIP" ,NULL,0,(void*)&config.showvip,	INT},
#endif

#ifdef _PLAYER_NUM
	{ "PLAYERNUM" ,NULL,0,(void*)&config.playernum,	INT},
	{ "PLAYERBASE"  , config.playerbase,sizeof(config.playerbase),NULL,0},
#endif

#ifdef _BATTLE_GOLD
	{ "BATTLEGOLD" ,NULL,0,(void*)&config.battlegold,	INT},
#endif
#ifdef _ANGEL_TIME
	{ "ANGELPLAYERTIME" ,NULL,0,(void*)&config.angelplayertime,	INT},
	{ "ANGELPLAYERMUN" ,NULL,0,(void*)&config.angelplayermun,	INT},
#endif
#ifdef _RIDEMODE_20
	{ "RIDEMODE" ,NULL,0,(void*)&config.ridemode,	INT},
#endif
#ifdef _FM_POINT_PK
	{ "FMPOINTPK" ,NULL,0,(void*)&config.fmpointpk,	INT},
#endif
#ifdef _ENEMY_ACTION
	{ "ENEMYACTION" ,NULL,0,(void*)&config.enemyact,	INT},
#endif
#ifdef _FUSIONBEIT_TRANS
	{ "FUSIONBEIT" ,NULL,0,(void*)&config.fusionbeittrans,	INT},
#endif
#ifdef _CHECK_PEPEAT
	{ "CHECKPEPEAT" ,NULL,0,(void*)&config.CheckRepeat,	INT},
#endif
	{ "CPUUSE" ,NULL,0,(void*)&config.cpuuse,	INT},
#ifdef _FM_JOINLIMIT
	{ "JOINFAMILYTIME" ,NULL,0,(void*)&config.joinfamilytime,	INT},
#endif
	{ "SAMEIPLOGIN" ,NULL,0,(void*)&config.SameIpLogin,	INT},
#ifdef _LUCK_MAN
	{ "LUCKTIME" ,NULL,0,(void*)&config.lucktime,	INT},
	{ "LUCKITEM"  , config.luckitem,sizeof(config.luckitem),NULL,0},
#endif
#ifdef _QUESTION_ONLINE
	{ "QUESTIONTIME" ,NULL,0,(void*)&config.questiontime,	INT},
	{ "QUESTIONITEM"  , config.questionitem,sizeof(config.questionitem),NULL,0},
#endif
#ifdef _NO_STREET_MAP
	{ "NOSTREETMAP"  , config.nostreetmap,sizeof(config.nostreetmap),NULL,0},
#endif
#ifdef _STREET_FAX
	{ "STREETFAX" ,config.streetfax,sizeof(config.streetfax),NULL,0},
#endif
	{ "FMWARTIME"  , config.fmwartime,sizeof(config.fmwartime),NULL,0},
#ifdef _JZ_NEWSCRIPT_LUA
	{ "LUAFILE"  , config.luafile,sizeof(config.luafile),NULL,0},
#endif
#ifdef _TRANS7_POINT
	{ "TRANS7POINT" ,NULL,0,(void*)&config.trans7point,	INT},
#endif
#ifdef _NOJOB_PK
	{ "NOJOBPKMAP" ,NULL,0,(void*)&config.nojobpkmap,	INT},
#endif
#ifdef _NO_ATTACK
	{ "ATTTIME" ,NULL,0,(void*)&config.atttime,	INT},
	{ "ATTSAFETIME" ,NULL,0,(void*)&config.attsafetime,	INT},
	{ "ATTCNT" ,NULL,0,(void*)&config.attcnt,	INT},
	{ "LATETIME" ,NULL,0,(void*)&config.latetime,	INT},
	{ "ATTDMETIME" ,NULL,0,(void*)&config.attdmetime,	INT},
	{ "ATTDMECNT" ,NULL,0,(void*)&config.attdmecnt,	INT},
#endif
	{ "NOATTIP1" ,config.noattip[0], sizeof( config.noattip[0]),NULL,0},
	{ "NOATTIP2" ,config.noattip[1], sizeof( config.noattip[1]),NULL,0},
	{ "NOATTIP3" ,config.noattip[2], sizeof( config.noattip[2]),NULL,0},
	{ "NOATTIP4" ,config.noattip[3], sizeof( config.noattip[3]),NULL,0},
	{ "NOATTIP5" ,config.noattip[4], sizeof( config.noattip[4]),NULL,0},
#ifdef _NO_TRANS_ANGLE
	{ "TRANS6ANGLE" ,NULL,0,(void*)&config.trans6angle,	INT},
#endif
#ifdef _PET_BUG
	{ "PETVITAL" ,NULL,0,(void*)&config.petpoint[0],	INT},
	{ "PETSTR" ,NULL,0,(void*)&config.petpoint[1],	INT},
	{ "PETTOUGH" ,NULL,0,(void*)&config.petpoint[2],	INT},
	{ "PETDEX" ,NULL,0,(void*)&config.petpoint[3],	INT},
#endif
#ifdef _VIGOR_SYS
	{ "VIGORTR0TIME" ,NULL,0,(void*)&config.vigortime[0],	INT},
	{ "VIGORTR1TIME" ,NULL,0,(void*)&config.vigortime[1],	INT},
	{ "VIGORTR2TIME" ,NULL,0,(void*)&config.vigortime[2],	INT},
	{ "VIGORTR3TIME" ,NULL,0,(void*)&config.vigortime[3],	INT},
	{ "VIGORTR4TIME" ,NULL,0,(void*)&config.vigortime[4],	INT},
	{ "VIGORTR5TIME" ,NULL,0,(void*)&config.vigortime[5],	INT},
	{ "VIGORTR6TIME" ,NULL,0,(void*)&config.vigortime[6],	INT},
	{ "VIGORTR7TIME" ,NULL,0,(void*)&config.vigortime[7],	INT},
	{ "VIGORMAX" ,NULL,0,(void*)&config.vigormax,	INT},
#endif
#ifdef _AUTO_PK
	{ "AUTOPKTIME" ,NULL,0,(void*)&config.autopktime,	INT},
	{ "AUTOPKMINNUM" ,NULL,0,(void*)&config.autopkminnum,	INT},
	{ "AUTOPKBATTLETIME" ,NULL,0,(void*)&config.autopkbattletime,	INT},
	{ "AUTOPKMINLV" ,NULL,0,(void*)&config.autopkminlv,	INT},
	{ "AUTOPKVIGORPKNUM" ,NULL,0,(void*)&config.autopkvigorpknum,	INT},
	{ "AUTOPKVIGORPKCNT" ,NULL,0,(void*)&config.autopkvigorpkcnt,	INT},
#endif
#ifdef	_BATTLEMAP_CTRL
	{ "BATTLETIME0" ,config.BattleTime[0], sizeof( config.BattleTime[0]),NULL,0},
	{ "BATTLETIME1" ,config.BattleTime[1], sizeof( config.BattleTime[1]),NULL,0},
	{ "BATTLETIME2" ,config.BattleTime[2], sizeof( config.BattleTime[2]),NULL,0},
	{ "BATTLETIME3" ,config.BattleTime[3], sizeof( config.BattleTime[3]),NULL,0},
	{ "BATTLETIME4" ,config.BattleTime[4], sizeof( config.BattleTime[4]),NULL,0},
	{ "BATTLETIME5" ,config.BattleTime[5], sizeof( config.BattleTime[5]),NULL,0},
	{ "BATTLETIME6" ,config.BattleTime[6], sizeof( config.BattleTime[6]),NULL,0},
	{ "BATTLETIME7" ,config.BattleTime[7], sizeof( config.BattleTime[7]),NULL,0},
	{ "BATTLETIME8" ,config.BattleTime[8], sizeof( config.BattleTime[8]),NULL,0},
	{ "BATTLETIME9" ,config.BattleTime[9], sizeof( config.BattleTime[9]),NULL,0},
#endif
#ifdef	_FM_SERVERNO_SYS	
	{ "FMSERVERNO" ,NULL,0,(void*)&config.fmserverno,	INT},
	{ "FMSERVERMSG"          , config.fmservermsg,sizeof(config.fmservermsg),NULL,0},
#endif
#ifdef	_ZHIPIAO_SYSTEM	
	{ "ZHIPIAO" ,NULL,0,(void*)&config.zhipiao,	INT},
	{ "PIAOTIME" ,NULL,0,(void*)&config.piaotime,	INT},
#endif
#ifdef	_FMWAR_MSG	
	{ "FMWARMSG"          , config.fmwarmsg,sizeof(config.fmwarmsg),NULL,0},
#endif
#ifdef	_FM_WELFARE	
	{ "FMWELFARE0" ,config.fmwelfare[0], sizeof( config.fmwelfare[0]),NULL,0},
	{ "FMWELFARE1" ,config.fmwelfare[1], sizeof( config.fmwelfare[1]),NULL,0},
	{ "FMWELFARE2" ,config.fmwelfare[2], sizeof( config.fmwelfare[2]),NULL,0},
	{ "FMWELFARE3" ,config.fmwelfare[3], sizeof( config.fmwelfare[3]),NULL,0},
	{ "FMWELFARE4" ,config.fmwelfare[4], sizeof( config.fmwelfare[4]),NULL,0},
	{ "FMWELFARE5" ,config.fmwelfare[5], sizeof( config.fmwelfare[5]),NULL,0},
	{ "FMWELFARE6" ,config.fmwelfare[6], sizeof( config.fmwelfare[6]),NULL,0},
	{ "FMWELFARE7" ,config.fmwelfare[7], sizeof( config.fmwelfare[7]),NULL,0},
	{ "FMWELFARE8" ,config.fmwelfare[8], sizeof( config.fmwelfare[8]),NULL,0},
	{ "FMWELFARE9" ,config.fmwelfare[9], sizeof( config.fmwelfare[9]),NULL,0},
#endif
#ifdef	_FMZUZHANG_MSG	
	{ "FMZUZHANG" ,config.fmzuzhang, sizeof( config.fmzuzhang),NULL,0},
#endif
#ifdef	_PETMAIL_LV	
	{ "PETMAILLV" ,NULL,0,(void*)&config.petmaillv,	INT},
	{ "PETMAILCF" ,NULL,0,(void*)&config.petmailcf,	INT},
#endif
#ifdef	_FMWAR_PLAYERNUM		
	{ "FMWARPLAYERNUM" ,NULL,0,(void*)&config.fmwarplayernum,	INT},
#endif
#ifdef _WAN_FIX
	{ "VSFLG" ,NULL,0,(void*)&config.vsflg,	INT},
#endif
#ifdef	_WARNPC_CTRL
	{ "PARTYMAP" ,config.partymap, sizeof( config.partymap),NULL,0},
	{ "MAPSAMEIP" ,config.mapsameip, sizeof( config.mapsameip),NULL,0},
#endif
#ifdef	_NO_DAOLUAN
	{ "STREETTRN" ,NULL,0,(void*)&config.streettrn,	INT},
	{ "TALKTRN" ,NULL,0,(void*)&config.talktrn,	INT},
	{ "TALKLV" ,NULL,0,(void*)&config.talklv,	INT},
	{ "TALKMSG" ,config.talkmsg, sizeof( config.talkmsg),NULL,0},
	{ "TALKNAME" ,config.talkname, sizeof( config.talkname),NULL,0},
#endif
#ifdef _NO_FULLPLAYER_ATT
	{ "NOFULLPLAYER" ,NULL,0,(void*)&config.nofullplayer,	INT},
	{ "NOFULL2PLAYER" ,NULL,0,(void*)&config.nofull2player,	INT},
	{ "NOCDKEYPLAYER" ,NULL,0,(void*)&config.nocdkeyplayer,	INT},
	{ "NOCDKEYMODE" ,NULL,0,(void*)&config.nocdkeymode,	INT},
	{ "NOCDKEYTYPE" ,NULL,0,(void*)&config.nocdkeytype,	INT},
	{ "NOFULLTIME" ,NULL,0,(void*)&config.nofulltime,	INT},
	{ "FENGTYPE" ,NULL,0,(void*)&config.fengtype,	INT},
	{ "NOFULLENDPLAYER" ,NULL,0,(void*)&config.nofullendplayer,	INT},
	{ "NOFULLENDTIME" ,NULL,0,(void*)&config.nofullendtime,	INT},
	{ "MANRENNUM" ,NULL,0,(void*)&config.manrennum,	INT},
	{ "BIGBAO" ,NULL,0,(void*)&config.bigbao,	INT},
	{ "BIGBAO2" ,NULL,0,(void*)&config.bigbao2,	INT},

#endif
#ifdef _ATTREVERSE_FIX
	{ "ATTREVERSE" ,NULL,0,(void*)&config.attreverse,	INT},
#endif
#ifdef	_NEWFM_GOLD
	{ "NEWFMGOLD" ,NULL,0,(void*)&config.newfmgold,	INT},
#endif
#ifdef	_MERGE_TIME
	{ "MERGETIME" ,NULL,0,(void*)&config.mergetime,	INT},
#endif
#ifdef	_PETUP_GET_EXP
	{ "PETUPGETEXP" ,NULL,0,(void*)&config.petupgetexp,	INT},
#endif
#ifdef	_MM_NO_JIAOBEN
	{ "NOMMJIAOBEN" ,NULL,0,(void*)&config.nommjiaoben,	INT},
	{ "EDITBASETIME" ,NULL,0,(void*)&config.editbasetime,	INT},
#endif
#ifdef	_DP_140_160_MODE
	{ "NEWDPMODE" ,NULL,0,(void*)&config.newdpmode,	INT},
#endif
#ifdef	_LOOK_STREET_TIME
	{ "LOOKSTREETTIME" ,NULL,0,(void*)&config.lookstreettime,	INT},
#endif
#ifdef	_BATTLE_EQUIT_OTHER
	{ "BATTLEEQUITOTHER" ,NULL,0,(void*)&config.battleequitother,	INT},
#endif
#ifdef	_PK_LUCK_MAN
	{ "PKLUCKFLOOR" ,config.pkluckfloor, sizeof( config.pkluckfloor),NULL,0},
	{ "PKLUCKMAPNAME0" ,config.pkluckmapname[0], sizeof( config.pkluckmapname[0]),NULL,0},
	{ "PKLUCKMAPNAME1" ,config.pkluckmapname[1], sizeof( config.pkluckmapname[1]),NULL,0},
	{ "PKLUCKMAPNAME2" ,config.pkluckmapname[2], sizeof( config.pkluckmapname[2]),NULL,0},
	{ "PKLUCKMAPNAME3" ,config.pkluckmapname[3], sizeof( config.pkluckmapname[3]),NULL,0},
	{ "PKLUCKMAPNAME4" ,config.pkluckmapname[4], sizeof( config.pkluckmapname[4]),NULL,0},
	{ "PKLUCKMAPNAME5" ,config.pkluckmapname[5], sizeof( config.pkluckmapname[5]),NULL,0},
	{ "PKLUCKMAPNAME6" ,config.pkluckmapname[6], sizeof( config.pkluckmapname[6]),NULL,0},
	{ "PKLUCKMAPNAME7" ,config.pkluckmapname[7], sizeof( config.pkluckmapname[7]),NULL,0},
	{ "PKLUCKMAPNAME8" ,config.pkluckmapname[8], sizeof( config.pkluckmapname[8]),NULL,0},
	{ "PKLUCKMAPNAME9" ,config.pkluckmapname[9], sizeof( config.pkluckmapname[9]),NULL,0},
	{ "PKLUCKTIME" ,config.pklucktime, sizeof( config.pklucktime),NULL,0},
	{ "PKLUCKNUM" ,config.pklucknum, sizeof( config.pklucknum),NULL,0},
	{ "PKLUCKBUF" ,config.pkluckbuf, sizeof( config.pkluckbuf),NULL,0},
	{ "PKLUCKLEVEL" ,NULL,0,(void*)&config.pklucklevel,	INT},
	{ "PKLUCKBATTLETURN" ,NULL,0,(void*)&config.pkluckbattleturn,	INT},
#endif
#ifdef	_NO_DAOLUAN
	{ "TALKFLOOR" ,config.talkfloor, sizeof( config.talkfloor),NULL,0},
#endif
#ifdef	_SUPERMAN_FIX
	{ "SUPERMANPOINT" ,NULL,0,(void*)&config.supermanpoint,	INT},
#endif
#ifdef	_PICKUP_ITEM_OTHER
	{ "PICKUPITEM" ,NULL,0,(void*)&config.pickupitem,	INT},
	{ "PICKITEMID" ,config.pickitemid, sizeof( config.pickitemid),NULL,0},
#endif
#ifdef	_FM_LIAOLI
	{ "FMLIAOLITYPE" ,NULL,0,(void*)&config.fmliaolitype,	INT},
#endif
#ifdef	_TRADE_ITEM_FIX
	{ "TRADEITEMFIX" ,NULL,0,(void*)&config.tradeitemfix,	INT},
#endif
#ifdef	_PETMAIL_TIME
	{ "PETMAILTIME" ,NULL,0,(void*)&config.petmailtime,	INT},
#endif
	{ "RANDMIN" ,NULL,0,(void*)&config.randmin,	INT},
	{ "RANDMAX" ,NULL,0,(void*)&config.randmax,	INT},
#ifdef	_GJS_TYPE
	{ "GJSTYPE" ,NULL,0,(void*)&config.gjstype,	INT},
#endif
	{ "TICKETCF" ,NULL,0,(void*)&config.ticketcf,	INT},
	{ "TICKETMSG" ,config.ticketmsg, sizeof( config.ticketmsg),NULL,0},

	{ "SATYPE" ,NULL,0,(void*)&config.satype,	INT},
#ifdef	_BATTLEEND_FIX
	{ "BATTLEENDMODE" ,NULL,0,(void*)&config.battleendmode,	INT},
#endif
#ifdef	_BIG_POOL_TYPE
	{ "PETPOOLTYPE" ,NULL,0,(void*)&config.petpooltype,	INT},
	{ "ITEMPOOLTYPE" ,NULL,0,(void*)&config.itempooltype,	INT},
	
	{ "NOSTAYBATTLE" ,NULL,0,(void*)&config.nostaybattle,	INT},
	{ "RIGHTNUM" ,NULL,0,(void*)&config.rightnum,	INT},
	{ "RIGHTMODE" ,NULL,0,(void*)&config.rightmode,	INT},
	{ "ERRORMODE" ,NULL,0,(void*)&config.errormode,	INT},
#endif
#ifdef	_FLOOR_PARTY_CTRL
	{ "PARTYFLOOR" ,config.partyfloor, sizeof( config.partyfloor),NULL,0},
#endif
	{ "BATTLETIMEOUT" ,NULL,0,(void*)&config.battletimeout,	INT},
	{ "RELOADNPCTIME" ,NULL,0,(void*)&config.reloadnpctime,	INT},
	{ "RELOADNPCTYPE" ,NULL,0,(void*)&config.reloadnpctype,	INT},
	{ "OFFLINEBATTLETIME" ,NULL,0,(void*)&config.offlinebattletime,	INT},
	{ "OFFLINECF" ,NULL,0,(void*)&config.offlinecf,	INT},
	{ "OFFLINEMAXNUM" ,NULL,0,(void*)&config.offlinemaxnum,	INT},
	{ "OFFLINEJQMMAXNUM" ,NULL,0,(void*)&config.offlinejqmmaxnum,	INT},
	{ "KONGTYPE" ,NULL,0,(void*)&config.kongtype,	INT},
	{ "JIFENBAITAN" ,NULL,0,(void*)&config.jifenbaitan,	INT},
	{ "STREETITEMUNNAME" ,config.streetitemunname, sizeof( config.streetitemunname),NULL,0},
	{ "NOPKMAP" ,NULL,0,(void*)&config.nopkmap,	INT},
	{ "FMBUFFTRANS" ,NULL,0,(void*)&config.fmbufftrans,	INT},
	{ "NOMPMAGIC" ,config.nompmagic, sizeof( config.nompmagic),NULL,0},
	{ "LOGINJQMTYPE" ,NULL,0,(void*)&config.loginjqmtype,	INT},
	{ "FMMAGICMPRATE1" ,config.fmmagicmprate[0], sizeof( config.fmmagicmprate[0]),NULL,0},
	{ "FMMAGICMPRATE2" ,config.fmmagicmprate[1], sizeof( config.fmmagicmprate[1]),NULL,0},
	{ "SAMEIPNUM" ,NULL,0,(void*)&config.sameipnum,	INT},
	{ "SAMEJQMNUM" ,NULL,0,(void*)&config.samejqmnum,	INT},
	{ "SAMEJQMALLNUM" ,NULL,0,(void*)&config.samejqmallnum,	INT},
	
	{ "MMTYPE" ,NULL,0,(void*)&config.mmtype,	INT},
	{ "MMMSG" ,config.mmmsg, sizeof( config.mmmsg),NULL,0},
	
	{ "LOCKTYPE" ,NULL,0,(void*)&config.locktype,	INT},
	{ "ALLTALKPOINT" ,NULL,0,(void*)&config.alltalkpoint,	INT},
#ifdef	_NO_RIDE_ID
	{ "NORIDEID" ,config.norideid, sizeof( config.norideid),NULL,0},
#endif
	{ "LOGINTYPE" ,NULL,0,(void*)&config.logintype,	INT},

#ifdef _PETTRANS_RANGE
	{ "PETTRANSRANGE_X" ,NULL,0,(void*)&config.pettransrangex,	INT},
	{ "PETTRANSRANGE_Y" ,NULL,0,(void*)&config.pettransrangey,	INT},
	{ "PETTRANSRANGE_Z" ,NULL,0,(void*)&config.pettransrangez,	INT},
#endif
#ifdef _ITEM_LUA
	{ "ITEMLUAFILE"  , config.itemluafile,sizeof(config.itemluafile),NULL,0},
#endif
#ifdef _SAME_IP_ONLINE_NUM
	{ "sameiponlinenum" ,NULL,0,(void*)&config.sameiponlinenum,	INT},
#endif
#ifdef _CHECK_SEVER_IP
	{ "serverip" ,config.serverip, sizeof( config.serverip),NULL,0},
#endif
#ifdef _CAX_ESC_REPORT
		{ "REPORTITEM1", NULL, 0,(void*) &config.reportitem[0], INT },
		{ "REPORTITEM2", NULL, 0,(void*) &config.reportitem[1], INT },
		{ "REPORTITEM3", NULL, 0,(void*) &config.reportitem[2], INT },
		{ "REPORTITEM4", NULL, 0,(void*) &config.reportitem[3], INT },
		{ "REPORTITEM5", NULL, 0,(void*) &config.reportitem[4], INT },		
		{ "VIPREPORTITEM1", NULL, 0,(void*) &config.vipreportitem[0], INT },
		{ "VIPREPORTITEM2", NULL, 0,(void*) &config.vipreportitem[1], INT },
		{ "VIPREPORTITEM3", NULL, 0,(void*) &config.vipreportitem[2], INT },
		{ "VIPREPORTITEM4", NULL, 0,(void*) &config.vipreportitem[3], INT },
		{ "VIPREPORTITEM5", NULL, 0,(void*) &config.vipreportitem[4], INT },
		{ "reportlv", NULL, 0,(void*) &config.reportlv, INT },
		{ "reportta", NULL, 0,(void*) &config.reportta, INT },
#endif
#ifdef _PETSKILL_SHOP_LUA
	  { "PETSKILLSHOPPATH" ,config.freepetskillshoppath, sizeof( config.freepetskillshoppath),NULL,0},
#endif
#ifdef _CAX_FAMEBUTTON
    { "famebutton" ,NULL,0,(void*)&config.famebutton,	INT},
#endif
#ifdef _PET_TRANS_ABILITY
	  { "PETTRANSABILITY" ,NULL,0,(void*)&config.pettransability,	INT},
	  { "PETTRANSABILITY1" ,NULL,0,(void*)&config.pettransability1,	INT},
	  { "PETTRANSABILITY2" ,NULL,0,(void*)&config.pettransability2,	INT},
	  { "PETTRANSABILITY3" ,NULL,0,(void*)&config.pettransability3,	INT},
#endif
#ifdef _PET_3TRANS
		{ "MMPETID1" ,NULL,0,(void*)&config.MMPETID1,INT},
		{ "MMPETID2" ,NULL,0,(void*)&config.MMPETID2,INT},
#endif
#ifdef _NO_MAGIC
    { "NOMAGICMAP" ,config.nomagicmap, sizeof( config.nomagicmap),NULL,0},
#endif
#ifdef _illegalpetskill_CF
		{ "IllegalPetskill1"  ,NULL,0,(void*)&config.IllegalPetskill[0],	INT},
		{ "IllegalPetskill2"  ,NULL,0,(void*)&config.IllegalPetskill[1],	INT},
		{ "IllegalPetskill3"  ,NULL,0,(void*)&config.IllegalPetskill[2],	INT},
		{ "IllegalPetskill4"  ,NULL,0,(void*)&config.IllegalPetskill[3],	INT},
		{ "IllegalPetskill5"  ,NULL,0,(void*)&config.IllegalPetskill[4],	INT},
		{ "IllegalPetskill6"  ,NULL,0,(void*)&config.IllegalPetskill[5],	INT},
		{ "IllegalPetskill7"  ,NULL,0,(void*)&config.IllegalPetskill[6],	INT},
		{ "IllegalPetskill8"  ,NULL,0,(void*)&config.IllegalPetskill[7],	INT},
		{ "IllegalPetskill9"  ,NULL,0,(void*)&config.IllegalPetskill[8],	INT},
		{ "IllegalPetskill10" ,NULL,0,(void*)&config.IllegalPetskill[9],	INT},							
#endif
#ifdef _CAX_PET_EVOLUTION
		{ "petevotyb"      , NULL ,0 , (void*)&config.petevotyb      ,CHAR},
		{ "petevotyb1"      , NULL ,0 , (void*)&config.petevotyb1      ,INT},
#endif
#ifdef _TRANS_7_COLOR
//		{ "tranclolr_7"      , NULL ,0 , (void*)&config.TranColor      ,INT},
		{ "ping"      , NULL ,0 , (void*)&config.ping      ,CHAR},
		{ "petsummon"      , NULL ,0 , (void*)&config.petsummon      ,CHAR},
#endif
#ifdef _CAX_PET_ITEMRIDE
		{ "petitemride"      , NULL ,0 , (void*)&config.petitemride      ,CHAR},
#endif
#ifdef _DAMMAGE_CALC
	{ "dammagecalc" ,NULL,0,(void*)&config.dammagecalc,	INT},
#endif
#ifdef _CAX_FAME_KOUFEI
	{ "FAMEKOUFEI" ,NULL,0,(void*)&config.famekoufei,	INT},
#endif
#ifdef _CAX_ITEM_ADDEXP
	{ "EXPTIME" ,NULL,0,(void*)&config.exptime,	INT},
#endif
#ifdef _SHARE_EXP
		{ "EXPSHARE"      , NULL ,0 , (void*)&config.expshare      ,INT},
#endif
#ifdef _TEAM_ADDEXP
		{ "TEAMADDEXP"      , NULL ,0 , (void*)&config.teamaddexp      ,INT},
#endif
#ifdef _NO_STW_ENEMY
	{ "NOSTWENEMY" ,NULL,0,(void*)&config.nostwenemy,	INT},
	{ "NOSTWENEMYGOLD" ,NULL,0,(void*)&config.nostwenemypoint,	INT},
#endif
#ifdef _DEX_FIX
	{ "DEXFIXPER" ,NULL,0,(void*)&config.dexfixper,	INT},
#endif
#ifdef _FM_EXP_ADD
    { "FMADDEXP",		NULL ,0 , (void*)&config.fmaddexp,				INT},
#endif
};

// Arminius 7.12 login announce
char announcetext[8192];
void AnnounceToPlayer(int charaindex)
{
  char *ptr,*qtr;
  
  ptr=announcetext;
  while ((qtr=strstr(ptr,"\n"))!=NULL) {
    qtr[0]='\0';
//    printf("ptr=%s\n",ptr);
    CHAR_talkToCli(charaindex, -1, ptr, CHAR_COLORYELLOW);
    qtr[0]='\n';
    ptr=qtr+1;
  }
  CHAR_talkToCli(charaindex, -1, ptr, CHAR_COLORYELLOW);
  
}

// Robin 0720
void AnnounceToPlayerWN(int fd)
{
	char buf[8192];
	char token[8192];
	int charaindex = CONNECT_getCharaindex( fd );
	long lastleavetime = CHAR_getInt( charaindex , CHAR_LASTLEAVETIME);
	struct tm *p;
	p=localtime(&lastleavetime);
			
	sprintf( token , "上一次离线时间 %d年%d月%d日 %d:%d:%d\n%s",p->tm_year + 1900,
																																		p->tm_mon + 1,
																																		p->tm_mday,
																																		p->tm_hour,
																																		p->tm_min,
																																		p->tm_sec,
																																		announcetext);
	lssproto_WN_send( fd , WINDOW_MESSAGETYPE_LOGINMESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1,-1,
			makeEscapeString( token, buf, sizeof(buf)));
}


void LoadAnnounce(void)
{
    FILE *f;

    memset(announcetext, 0, sizeof(announcetext));
    if ((f=fopen("./announce.txt","r"))!=NULL) {
      fread(announcetext, sizeof(announcetext), 1, f);
      announcetext[sizeof(announcetext)-1]='\0';
      fclose(f);
    }
}
#ifdef _PET_TALKPRO
PTALK pettalktext[PETTALK_MAXID];

void LoadPetTalk(void)
{
	FILE *fp;
	char fn[256];
	char line[ 4096];
	char talkmem[4096];
	int maxid=0;
	char buf1[256], buf2[256], buf3[256];
	int talkNO=-1, mark=-1, i;
	int len = sizeof( talkmem);
	
	memset(talkmem, 0, sizeof(talkmem));
	sprintf(fn, "%s/pettalk/pettalk.menu", getNpcdir());

	for( i=0;i<PETTALK_MAXID;i++)	{
		pettalktext[i].ID = -1;
		strcpy( pettalktext[i].DATA, "\0");
	}

	print("\n装载宠物对话文件:%s...", fn);
	fp = fopen( fn, "r");
    if( fp != NULL ) {
		while( fgets( line, sizeof( line), fp)) {
			if( strlen( talkmem) != 0 ) {
				if( talkmem[strlen( talkmem) -1] != '|' ) {
					strcatsafe( talkmem, len, "|");		
				}
			}
			chompex( line);
			strcatsafe( talkmem,len,  line);
		}
		fclose( fp);
    }else	{
		print("错误:找不到文件!");
	}

	talkNO=1;
	while( getStringFromIndexWithDelim( talkmem,"END",talkNO, buf1, sizeof( buf1)) != FALSE	){
		talkNO++;
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "PETTEMPNO", buf2, sizeof( buf2)) == NULL  )
			continue;
		mark=1;
		strcpy( fn,"\0");

		if( getStringFromIndexWithDelim( buf2,",", mark+1,buf3,sizeof( buf3)) != FALSE )	{
			pettalktext[maxid].ID = atoi( buf3);
			if( getStringFromIndexWithDelim( buf2,",", mark,buf3,sizeof( buf3)) != FALSE )	{
				sprintf(fn, "%s/pettalk/%s", getNpcdir(), buf3);
				//print("\n ...file:%s", fn);
				fp = fopen( fn, "r");
				if( fp != NULL )	{
					char line[4096];
					while( fgets( line, sizeof( line), fp ) ) {
						if( strlen( pettalktext[maxid].DATA) != 0 ) {
							if( pettalktext[maxid].DATA[strlen( pettalktext[maxid].DATA) -1] != '|' ) {
								strcatsafe( pettalktext[maxid].DATA, sizeof( pettalktext[maxid].DATA), "|");
							}
						}
						chompex( line);
						strcatsafe( pettalktext[maxid].DATA, sizeof( pettalktext[maxid].DATA), line);
					}
					maxid++;
					fclose( fp);
				}else	{
					print("错误:[%s] 找不到!", fn);
					pettalktext[maxid].ID=-1;
				}
			}else	{
				pettalktext[maxid].ID=-1;
			}
		}
		print(".");
		if( maxid >= PETTALK_MAXID )
			break;
	}
	print("最大ID=%d...", maxid);
	{
		int haveid=0;
		for( i=0;i<PETTALK_MAXID;i++)	{
			if( pettalktext[i].ID >= 0 )	{
				haveid++;
			}
		}
		print("载入总数=%d", haveid);
	}

}

#else
char pettalktext[4096];
void LoadPetTalk(void)
{
  FILE *fp;
  char fn[256];
  char	line[ 4096];
  int len = sizeof( pettalktext);
  
  memset(pettalktext, 0, sizeof(pettalktext));
  sprintf(fn, "%s/pettalk/pettalk.mem", getNpcdir());
  
  fp = fopen( fn, "r");
    if( fp != NULL ) {
		print("\n\n 读取 pettalk.mem");
		while( fgets( line, sizeof( line), fp)) {
			if( strlen( pettalktext) != 0 ) {
				if( pettalktext[strlen( pettalktext) -1] != '|' ) {
					strcatsafe( pettalktext, len, "|");		
				}
			}
			chompex( line);
			strcatsafe( pettalktext,len,  line);
		}
		fclose( fp);
		print("\n %s", pettalktext);
    }else	{
		print("\n 不能找到 pettalk.mem");
	}
}
#endif

#ifdef _GAMBLE_BANK
GAMBLEBANK_ITEMS GB_ITEMS[GAMBLEBANK_ITEMSMAX];

void Load_GambleBankItems( void)
{

	FILE *fp;
	char filename[256];
	char buf1[256];
	char name[128];
	int num,ID,type;
	int i=0;
	sprintf(filename, "./data/gambleitems.txt" );
	print("\n加载赌博物品文件 %s ...", filename);
	fp = fopen( filename, "r");
    if( fp != NULL ) {
		while( fgets( buf1, sizeof( buf1), fp) != NULL )	{
			if( strstr( buf1, "#") != 0 ) continue;
			sscanf( buf1,"%s %d %d %d", name, &ID, &num , &type);
			strcpy( GB_ITEMS[i].name, name);
			GB_ITEMS[i].Gnum = num;
			GB_ITEMS[i].ItemId = ID;
			GB_ITEMS[i].type = type;
			i++;
		}
		print("最大ID: %d ", i);
		fclose( fp);
    }else	{
		print("错误 找不到文件 %s", filename);
	}

}
#endif


#ifdef _CFREE_petskill
PETSKILL_CODES Code_skill[PETSKILL_CODE];
void Load_PetSkillCodes( void)
{
	FILE *fp;
	char filename[256];
	char buf1[256];
	char name[128];
	char type[256];
	int num,ID;
	int i=0;
	sprintf(filename, "./data/skillcode.txt" );
	print("\n加载宠物技能编码文件:%s...", filename);
	fp = fopen( filename, "r");
    if( fp != NULL ) {
		while( fgets( buf1, sizeof( buf1), fp) != NULL )	{
			sscanf( buf1,"%s %d %d %s", name, &num, &ID, type);
			strcpy( Code_skill[i].name, name);
			Code_skill[i].TempNo = num;
			Code_skill[i].PetId = ID;
			strcpy( Code_skill[i].Code, type);
			//print("\n %s|%d|%d|%s|", Code_skill[i].name, Code_skill[i].TempNo, 
			//	Code_skill[i].PetId, Code_skill[i].Code);
			i++;
			if( i >= PETSKILL_CODE ) break;
		}
		fclose( fp);
    }else	{
		print("打不到文件 %s", filename);
	}
	print("完成\n");
}
#endif

#ifdef _GMRELOAD
BOOL LoadGMSet( char* filename )
{
	FILE* fp;
	int i = 0, gm_num = 0;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	for (i = 0; i < GMMAXNUM; i++)
	{
		strcpy(gminfo[i].cdkey, "");
		gminfo[i].level = 0;
	}
	while(1){
		char	line[64], cdkey[64], level[64];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);
		//change 使gmset.txt可以增加注解*******
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		gm_num = gm_num + 1;
		if (gm_num > GMMAXNUM)	break;
		easyGetTokenFromString(line, 1, cdkey, sizeof(cdkey));
		if (strcmp(cdkey, "") == 0)	break;
		strncpy(gminfo[gm_num].cdkey, cdkey, sizeof(gminfo[gm_num].cdkey));
		easyGetTokenFromString(line, 2, level, sizeof(level));
		if (strcmp(level, "") == 0)	break;
		gminfo[gm_num].level = atoi(level);
//		print("\ncdkey:%s, level:%d", gminfo[gm_num].cdkey, gminfo[gm_num].level);
	}
	fclose(fp);
	return TRUE;
}
#endif

/*------------------------------------------------------------
 * 皿夫弘仿丞  毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  cahr*
 ------------------------------------------------------------*/
char* getProgname( void )
{
    return config.progname;
}
/*------------------------------------------------------------
 * configfilename 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getConfigfilename( void )
{
    return config.configfilename;
}
/*------------------------------------------------------------
 * configfilename 毛涩烂允月［
 * 娄醒
 *  newv    char*   蕙仄中袄
 * 忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
void setConfigfilename( char* newv )
{
    strcpysafe( config.configfilename, sizeof( config.configfilename ),
                newv );
}

/*------------------------------------------------------------
 * 犯田永弘伊矛伙毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getDebuglevel( void )
{
    return config.debuglevel;
}
/*------------------------------------------------------------
 * 犯田永弘伊矛伙毛涩烂允月
 * 娄醒
 *  newv    int     蕙仄中袄
 * 忒曰袄
 *  unsigned int    樯及袄
 ------------------------------------------------------------*/
unsigned int setDebuglevel( unsigned int newv )
{
    int old;
    old = config.debuglevel;
    config.debuglevel = newv;
    return old;
}
/*------------------------------------------------------------
 * memoryunit 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getMemoryunit( void )
{
    return config.usememoryunit;
}
/*------------------------------------------------------------
 * memoryunitnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getMemoryunitnum( void )
{
    return config.usememoryunitnum;
}

/*------------------------------------------------------------
 * 失市它件玄扔□田及失玉伊旦毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char*   getAccountservername( void )
{
    return config.asname;
}
/*------------------------------------------------------------
 * 失市它件玄扔□田及禾□玄毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
unsigned short   getAccountserverport( void )
{
    return config.acservport;
}
/*------------------------------------------------------------
 * 失市它件玄扔□田尺及由旦伐□玉毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
char*   getAccountserverpasswd( void )
{
    return config.acpasswd;
}
/*------------------------------------------------------------
 * 失市它件玄扔□田井日苇尹月必□丞扔□田午仄化及  蟆毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
char*   getGameservername( void )
{
    return config.gsnamefromas;
}

// Arminius 7.24 manor pk
char* getGameserverID( void )
{
    if (config.gsid[strlen(config.gsid)-1]=='\n')
      config.gsid[strlen(config.gsid)-1]='\0';
      
    return config.gsid;
}

unsigned short getAllowManorPK( void )
{
    return config.allowmanorpk;
}

unsigned short getPortnumber( void )
{
    return config.port;
}
/*------------------------------------------------------------
 * 必□丞扔□田及  寞   寞毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
int getServernumber( void )
{
    return config.servernumber;
}
/*------------------------------------------------------------
 * reuseaddr 及袄毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned short
 ------------------------------------------------------------*/
int getReuseaddr( void )
{
    return config.reuseaddr;
}

int getNodelay( void )
{
    return config.do_nodelay;
}
int getLogWriteTime(void)
{
    return config.log_write_time;
}
int getLogIOTime( void)
{
    return config.log_io_time;
}
int getLogGameTime(void)
{
    return config.log_game_time;
}
int getLogNetloopFaster(void)
{
    return config.log_netloop_faster;
}

/*------------------------------------------------------------
 * saacwritenum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
int getSaacwritenum( void )
{
    return config.saacwritenum;
}
/*------------------------------------------------------------
 * saacwritenum 毛涩烂允月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
void setSaacwritenum( int num )
{
    config.saacwritenum = num;
}
/*------------------------------------------------------------
 * saacreadnum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
int getSaacreadnum( void )
{
    return config.saacreadnum;
}
/*------------------------------------------------------------
 * saacreadnum 毛涩烂允月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *   int
 ------------------------------------------------------------*/
void setSaacreadnum( int num )
{
    config.saacreadnum = num;
}
/*------------------------------------------------------------
 * fdnum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getFdnum( void )
{
#ifdef _TEST_SERVER
		if(config.fdnum>30)
			return 30;
		else
			return config.fdnum;
#else
    return config.fdnum;
#endif
}

unsigned int getPlayercharnum( void )
{
	if(config.playercharnum<config.fdnum) return config.fdnum;
  return config.playercharnum;
}
/*------------------------------------------------------------
 * petcharanum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getPetcharnum( void )
{
    return config.petcharnum;
}


/*------------------------------------------------------------
 * othercharnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getOtherscharnum( void )
{
    return config.othercharnum;
}

/*------------------------------------------------------------
 * objnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getObjnum( void )
{
    return config.objnum;
}

/*------------------------------------------------------------
 * itemnum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getItemnum( void )
{
    return config.itemnum;
}


/*------------------------------------------------------------
 * battlenum 毛  月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getBattlenum( void )
{
    return config.battlenum;
}

#ifdef _GET_BATTLE_EXP
unsigned int getBattleexp( void )
{
    return config.battleexp;
}
void setBattleexp( int exp )
{
    config.battleexp = exp;
    return;
}
#endif
/*------------------------------------------------------------
 * topdir 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getTopdir( void )
{
    return config.topdir;
}
/*------------------------------------------------------------
 * mapdir 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getMapdir( void )
{
    return config.mapdir;
}
/*------------------------------------------------------------
 * maptilefile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getMaptilefile( void )
{
    return config.maptilefile;
}
/*------------------------------------------------------------
 * battlemapfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getBattleMapfile( void )
{
    return config.battlemapfile;
}
/*------------------------------------------------------------
 * itemfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getItemfile( void )
{
    return config.itemfile;
}
/*------------------------------------------------------------
 * invfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getInvfile( void )
{
    return config.invfile;
}
/*------------------------------------------------------------
 * appearfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getAppearfile( void )
{
    return config.appearfile;
}
/*------------------------------------------------------------
 * effectfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEffectfile( void )
{
    return config.effectfile;
}
/*------------------------------------------------------------
 * titlenamefile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getTitleNamefile( void )
{
    return config.titlenamefile;
}
/*------------------------------------------------------------
 * titleconfigfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getTitleConfigfile( void )
{
    return config.titleconfigfile;
}
/*------------------------------------------------------------
 * encountfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEncountfile( void )
{
    return config.encountfile;
}
/*------------------------------------------------------------
 * enemyfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEnemyfile( void )
{
    return config.enemyfile;
}
/*------------------------------------------------------------
 * enemybasefile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getEnemyBasefile( void )
{
    return config.enemybasefile;
}
/*------------------------------------------------------------
 * groupfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getGroupfile( void )
{
    return config.groupfile;
}
/*------------------------------------------------------------
 * magicfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getMagicfile( void )
{
    return config.magicfile;
}

#ifdef _ATTACK_MAGIC

/*------------------------------------------------------------
 * 取得攻击性的咒术
 * 参数
 * None
 * 返回值
 * char*
 ------------------------------------------------------------*/
char* getAttMagicfileName( void )
{
    return config.attmagicfile;
}

#endif


char* getPetskillfile( void )
{
    return config.petskillfile;
}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
char* getProfession( void )
{
    return config.profession;
}
#endif

#ifdef _ITEM_QUITPARTY
char* getitemquitparty( void )
{
    return config.itemquitparty;
}
#endif

char *getItematomfile( void )
{
    return config.itematomfile;
}


char* getQuizfile( void )
{
    return config.quizfile;
}

/*------------------------------------------------------------
 * lsgenlogfile 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getLsgenlogfilename( void )
{
    return config.lsgenlog;
}

#ifdef _GMRELOAD
char* getGMSetfile( void )
{
    return config.gmsetfile;
}
#endif

/*------------------------------------------------------------
 * storedir 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getStoredir( void )
{
    return config.storedir;
}
#ifdef _STORECHAR
/*------------------------------------------------------------
 ------------------------------------------------------------*/
char* getStorechar( void )
{
    return config.storechar;
}
#endif

/*------------------------------------------------------------
 * NPC 迕及犯奴伊弁玄伉毛  化仁月楮醒
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getNpcdir( void )
{
    return config.npcdir;
}
/*------------------------------------------------------------
 * 夫弘犯奴伊弁玄伉毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getLogdir( void )
{
    return config.logdir;
}

/*------------------------------------------------------------
 * 夫弘涩烂白央奶伙  毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getLogconffile( void )
{
    return config.logconfname;
}
/*------------------------------------------------------------
 * 民乓永玄  芊由旦伐□玉 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
char* getChatMagicPasswd( void )
{
    return config.chatmagicpasswd;
}
/*------------------------------------------------------------
 * 犯田永弘民乓永玄  芊匹及CDKEY民尼永弁毛允月井升丹井毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  char*
 ------------------------------------------------------------*/
unsigned getChatMagicCDKeyCheck( void )
{
    return config.chatmagiccdkeycheck;
}

/*------------------------------------------------------------
 * filesearchnum毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getFilesearchnum( void )
{
    return config.filesearchnum;
}
/*------------------------------------------------------------
 * npctemplatenum毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getNpctemplatenum( void )
{
    return config.npctemplatenum;
}
/*------------------------------------------------------------
 * npccreatenum毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getNpccreatenum( void )
{
    return config.npccreatenum;
}

/*------------------------------------------------------------
 * walksendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getWalksendinterval( void )
{
    return config.walksendinterval;
}
/*------------------------------------------------------------
 * walksendinterval毛本永玄允月［
 * 娄醒
 *  unsigned int 	interval	凛棉  立伉
 * 忒曰袄
 *  void
 ------------------------------------------------------------*/
void setWalksendinterval( unsigned int interval )
{
    config.walksendinterval = interval;
}
/*------------------------------------------------------------
 * CAsendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCAsendinterval_ms( void )
{
    return config.CAsendinterval_ms;
}
/*------------------------------------------------------------
 * CAsendinterval毛本永玄允月［
 * 娄醒
 *  unsigned int interval
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
void setCAsendinterval_ms( unsigned int interval_ms )
{
    config.CAsendinterval_ms = interval_ms;
}
/*------------------------------------------------------------
 * CDsendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCDsendinterval_ms( void )
{
    return config.CDsendinterval_ms;
}
/*------------------------------------------------------------
 * CDsendinterval毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setCDsendinterval_ms( unsigned int interval_ms )
{
	config.CDsendinterval_ms = interval_ms;
}
/*------------------------------------------------------------
 * Onelooptime毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getOnelooptime_ms( void )
{
    return config.Onelooptime_ms;
}
/*------------------------------------------------------------
 * Onelooptime毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setOnelooptime_ms( unsigned int interval_ms )
{
	config.Onelooptime_ms = interval_ms;
}
/*------------------------------------------------------------
 * Petdeletetime毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getPetdeletetime( void )
{
    return config.Petdeletetime;
}
/*------------------------------------------------------------
 * Petdeletetime毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setPetdeletetime( unsigned int interval )
{
	config.Petdeletetime = interval;
}
/*------------------------------------------------------------
 * Itemdeletetime毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getItemdeletetime( void )
{
    return config.Itemdeletetime;
}
/*------------------------------------------------------------
 * Itemdeletetime毛本永玄允月［
 * 娄醒
 *  interval		unsigned int
 * 忒曰袄
 * void
 ------------------------------------------------------------*/
void setItemdeletetime( unsigned int interval )
{
	config.Itemdeletetime = interval;
}

/*------------------------------------------------------------
 * CharSavesendinterval毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getCharSavesendinterval( void )
{
    return config.CharSavesendinterval;
}
/*------------------------------------------------------------
 * CharSavesendinterval毛本永玄允月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
void setCharSavesendinterval( unsigned int interval)
{
	config.CharSavesendinterval = interval;
}

/*------------------------------------------------------------
 * Addressbookoffmsgnum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getAddressbookoffmsgnum( void )
{
    return config.addressbookoffmsgnum;
}
/*------------------------------------------------------------
 * Protocolreadfrequency 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getProtocolreadfrequency( void )
{
    return config.protocolreadfrequency;
}

/*------------------------------------------------------------
 * Allowerrornum 毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getAllowerrornum( void )
{
    return config.allowerrornum;
}

unsigned int getFengerrornum( void )
{
    return config.fengerrornum;
}

/*------------------------------------------------------------
 * 夫弘毛潸  允月凛对毛  月［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int
 ------------------------------------------------------------*/
unsigned int getLogHour( void )
{
    return config.loghour;
}

/*------------------------------------------------------------
 * 田玄伙  及犯田永弘丢永本□斥毛请允井［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int 1卅日请允
 ------------------------------------------------------------*/
unsigned int getBattleDebugMsg( void )
{
    return config.battledebugmsg;
}
/*------------------------------------------------------------
 * 田玄伙  及犯田永弘丢永本□斥毛请允井［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int 1卅日请允
 ------------------------------------------------------------*/
void setBattleDebugMsg( unsigned int num )
{
    config.battledebugmsg = num;
}



/*
 * Config及犯白巧伙玄袄毛瑁户月楮醒
 * 娄醒
 *  argv0   char*   戊穴件玉仿奶件娄醒及  赓
 */
void  defaultConfig( char* argv0 )
{
    char* program;                  /* program  毛菲户月及卞银丹 */

    /* 犯白巧伙玄袄毛  木月 */

    /*皿夫弘仿丞  */
    program = rindex(argv0, '/');
    if (program == NULL)
        program = argv0;
    else
        program++;   /* "/"及戚井日卞仄凶中及匹++允月*/
    strcpysafe( config.progname , sizeof( config.progname ) ,program );

    /*涩烂白央奶伙  */
    strcpysafe( config.configfilename,
                sizeof( config.configfilename ),"setup.cf" );

}

/*
 * 涩烂白央奶伙  毛  氏分  匹及质  毛垫丹［
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  卅仄
 */
void lastConfig( void )
{
    char    entry[256];
    /*  穴永皿犯奴伊弁玄伉及涩烂    */
    snprintf(entry, sizeof(entry), "%s/%s", config.topdir, config.mapdir);
    strcpysafe(config.mapdir, sizeof(config.mapdir), entry);

    /*  穴永皿涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry),"%s/%s",
             config.topdir,config.maptilefile);
    strcpysafe(config.maptilefile, sizeof(config.maptilefile), entry);

    /*  田玄伙穴永皿涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry),"%s/%s",
             config.topdir,config.battlemapfile);
    strcpysafe(config.battlemapfile, sizeof(config.battlemapfile), entry);

    /*  失奶  丞涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.itemfile);
    strcpysafe(config.itemfile, sizeof(config.itemfile), entry);

    /*    衬涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.invfile);
    strcpysafe(config.invfile, sizeof(config.invfile), entry);

    /*  请蜇匏  涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.appearfile);
    strcpysafe(config.appearfile, sizeof(config.appearfile), entry);

    /*  梢请涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.effectfile);
    strcpysafe(config.effectfile, sizeof(config.effectfile), entry);

    /*  弁奶术涩烂白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.quizfile);
    strcpysafe(config.quizfile, sizeof(config.quizfile), entry);

    /*  惫寞  白央奶伙  及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s", config.topdir,config.titlenamefile);
    strcpysafe(config.titlenamefile, sizeof(config.titlenamefile), entry);

    /*  lsgen 失它玄皿永玄白央奶伙      */
    snprintf(entry,sizeof(entry),"%s/%s", config.topdir,config.lsgenlog);
    strcpysafe(config.lsgenlog, sizeof(config.lsgenlog), entry);

    /*  旦玄失犯奴伊弁玄伉及涩烂    */
/*
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.storedir);
    strcpysafe(config.storedir, sizeof(config.storedir), entry);
*/
    /*  NPC涩烂玄永皿犯奴伊弁玄伉及涩烂    */
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.npcdir);
    strcpysafe(config.npcdir, sizeof(config.npcdir), entry);

#ifdef _STORECHAR
    /*   */
    snprintf(entry,sizeof(entry), "%s/%s",config.topdir,config.storechar);
    strcpysafe(config.storechar, sizeof(config.storechar), entry);
#endif

}


/*
 * 禾奶件正□午赝濠卅滇树  毛  匀化医  毛允月楮醒
 * 娄醒
 *  to      void*   袄毛医  允月禾奶件正
 *  type    CTYPE   to及滇毛瑁户月
 *  value   double  to卞医  允月袄
 * 忒曰袄
 *  卅仄
 */
void substitutePointerFromType( void* to , CTYPE type ,double value)
{
    switch( type  ){
    case CHAR:
        *(char*)to = (char)value;
        break;
    case SHORT:
        *(short*)to = (short)value;
        break;
    case INT:
        *(int*)to = (int)value;
        break;
    case DOUBLE:
        *(double*)to = (double)value;
        break;
    }
}

BOOL luareadconfigfile( char* data )
{
	char firstToken[256];
	int ret = getStringFromIndexWithDelim( data , "=",  1, firstToken, sizeof(firstToken) );
	if( ret == FALSE ){
    return FALSE;
  }
	int i;
  for( i = 0 ; i < arraysizeof( readconf ) ; i ++ ){
    if( strcmp( readconf[i].name ,firstToken ) == 0 ){
      /* match */
      char secondToken[256];      /*2    及  侬  */
      /* delim "=" 匹2    及玄□弁件毛  月*/
      ret = getStringFromIndexWithDelim( data , "=" , 2
                                         , secondToken ,
                                         sizeof(secondToken) );

      /* NULL  侬井升丹井毛譬屯月 */
      if( ret == FALSE ){
        break;
      }

      if( readconf[i].charvalue != NULL )
          strcpysafe( readconf[i].charvalue
                      ,readconf[i].charsize, secondToken);
      if( readconf[i].value != NULL ) {
        if( strcmp( "ON" ,secondToken ) == 0 ) {

            substitutePointerFromType( readconf[i].value,
                                       readconf[i].valuetype,
                                       1.0);

        }else if( strcmp( "OFF" ,secondToken ) == 0 ) {

            substitutePointerFromType( readconf[i].value,
                                       readconf[i].valuetype,
                                       1.0);
        }else {
            strtolchecknum(secondToken,
                           (int*)readconf[i].value,
                           10, readconf[i].valuetype);
				}
			}
      break;
    }
  }
  return TRUE;
}
/*------------------------------------------------------------
 * 涩烂白央奶伙毛  戈
 * 娄醒
 *      filename            白央奶伙
 * 忒曰袄
 *      TRUE(1)     岳
 *      FALSE(0)    撩      -> 白央奶伙及左□皿件卞撩  仄凶
 ------------------------------------------------------------*/
BOOL readconfigfile( char* filename )
{
    FILE* f=NULL;
    char linebuf[256];                  /* 域垫  心  戈田永白央 */
    int linenum=0;                      /* 垫醒毛醒尹月 */
    char    realopenfilename[256];      /*    端卞open 允月白央奶伙  */

    char    hostname[128];

    /*  石旦玄  毛菲户月    */
    if( gethostname( hostname, sizeof(hostname) ) != -1 ){
        char*   initdot;
        initdot = index( hostname, '.' );
        if( initdot != NULL )
            *initdot = '\0';
        snprintf( realopenfilename, sizeof(realopenfilename),
                  "%s.%s" , filename, hostname);

        /* 白央奶伙及左□皿件 */
        f=fopen( realopenfilename, "r" );
        if( f == NULL )
            print( "Can't open %s.  use %s instead\n", realopenfilename,
                   filename );
    }
    if( f == NULL ){
        f=fopen( filename , "r" );          /* 白央奶伙及左□皿件 */
        if( f == NULL ){
            print( "Can't open %s\n", filename );
            return FALSE;
        }
    }

    /* 域垫勿勾  心  戈 */
    while( fgets( linebuf , sizeof( linebuf ), f ) ){
        char firstToken[256];       /*1    及  侬  */
        int i;                      /*伙□皿  醒*/
        int ret;                    /*伉正□件戊□玉*/

        linenum ++;

        deleteWhiteSpace(linebuf);          /* remove whitespace    */

        if( linebuf[0] == '#' )continue;        /* comment */
        if( linebuf[0] == '\n' )continue;       /* none    */

        chomp( linebuf );                    /* remove tail newline  */

        /* delim "=" 匹  赓(1)及玄□弁件毛  月*/
        ret = getStringFromIndexWithDelim( linebuf , "=",  1, firstToken,
                                           sizeof(firstToken) );
        if( ret == FALSE ){
            print( "Find error at %s in line %d. Ignore\n",
                     filename , linenum);
            continue;
        }

        /* readconf 及扔奶术匹伙□皿 */
        for( i = 0 ; i < arraysizeof( readconf ) ; i ++ ){
            if( strcmp( readconf[i].name ,firstToken ) == 0 ){
                /* match */
                char secondToken[256];      /*2    及  侬  */
                /* delim "=" 匹2    及玄□弁件毛  月*/
                ret = getStringFromIndexWithDelim( linebuf , "=" , 2
                                                   , secondToken ,
                                                   sizeof(secondToken) );

                /* NULL  侬井升丹井毛譬屯月 */
                if( ret == FALSE ){
                    print( "Find error at %s in line %d. Ignore",
                           filename , linenum);
                    break;
                }


                /*NULL分匀凶日医  仄卅中*/
                if( readconf[i].charvalue != NULL )
                    strcpysafe( readconf[i].charvalue
                                ,readconf[i].charsize, secondToken);

                /*NULL分匀凶日医  仄卅中*/
                if( readconf[i].value != NULL ) {
                    if( strcmp( "ON" ,secondToken ) == 0 ) {
                        /*ON分匀凶日1毛  木月*/
                        substitutePointerFromType( readconf[i].value,
                                                   readconf[i].valuetype,
                                                   1.0);

                    }else if( strcmp( "OFF" ,secondToken ) == 0 ) {
                        /*OFF分匀凶日1毛  木月*/
                        substitutePointerFromType( readconf[i].value,
                                                   readconf[i].valuetype,
                                                   1.0);
                    }else {
                        strtolchecknum(secondToken,
                                       (int*)readconf[i].value,
                                       10, readconf[i].valuetype);
					}
				}
                break;
            }
        }
    }
    fclose( f );
    lastConfig();
    return TRUE;
}
//ttom add this becaus the second had this function 
/*------------------------------------------------------------
 * 巨件戊□玉平□毛涩烂允月
 * 娄醒
 *  卅仄
 * 忒曰袄
 *  unsigned int 平□毛忒允
------------------------------------------------------------*/
unsigned int setEncodeKey( void )
{
   JENCODE_KEY = config.encodekey;
   return JENCODE_KEY;
}
/*------------------------------------------------------------
* 失市它件玄扔□田□卞踏五  戈田永白央及扔奶术毛涩烂允月
* 娄醒
*  卅仄
* 忒曰袄
*  unsigned int 平□毛忒允
------------------------------------------------------------*/
unsigned int setAcWBSize( void )
{
    AC_WBSIZE = config.acwbsize;
    return AC_WBSIZE;
}
unsigned int getAcwriteSize( void )
{
    return config.acwritesize;
}
unsigned int getErrUserDownFlg( void )
{
    return config.ErrUserDownFlg;
}
    

#ifdef _DEL_DROP_GOLD
unsigned int getGolddeletetime( void )
{
    if( config.Golddeletetime > 0 )
		return config.Golddeletetime;
	else
		return config.Itemdeletetime;
}
void setIGolddeletetime( unsigned int interval )
{
	config.Golddeletetime = interval;
}
#endif

#ifdef _NEW_PLAYER_CF
int getNewplayertrans( void )
{
  if(config.newplayertrans > 7)
		return 7;
	else if(config.newplayertrans >= 0)
		return config.newplayertrans;
	else
		return 0;
}
int getNewplayerlv( void )
{
  if(config.newplayerlv > 160)
		return 160;
	else if(config.newplayerlv >0)
		return config.newplayerlv;
	else
		return 0;
}
int getNewplayerpetlv( void )
{
  if(config.newplayerpetlv > 160)
		return 160;
	else if(config.newplayerpetlv > 0)
		return config.newplayerpetlv;
	else
		return 0;
}

int getNewplayergivepet( unsigned int index )
{
  if(config.newplayergivepet[index] > 0 )
		return config.newplayergivepet[index];
	else
		return -1;
}

int getNewplayergiveitem( unsigned int index )
{
  if(config.newplayergiveitem[index] > 0 )
		return config.newplayergiveitem[index];
	else
		return -1;
}

void setNewplayergivepet( unsigned int index ,unsigned int interval)
{
	config.newplayergivepet[index] = interval;
}

int getNewplayergivegold( void )
{
  if(config.newplayergivegold > 1000000)
		return 1000000;
	else if(config.newplayergivegold < 0)
		return 0;
	else
		return config.newplayergivegold;
}
int getRidePetLevel( void )
{
  if(config.ridepetlevel > 0 )
		return config.ridepetlevel;
	else
		return -1;
}
#ifdef _VIP_SERVER
int getNewplayergivevip( void )
{
	return config.newplayerpetvip < 0?0:config.newplayerpetvip;
}
#endif
#endif
#ifdef _JK_CF_DELPETITEM          ////删除CF指定物品和宠物
char *getDelPet(void)
{
	return config.DelPet;
}
char *getDelItem(void)
{
	return config.DelItem;
}
#endif
#ifdef _UNLAW_WARP_FLOOR
int getUnlawwarpfloor( unsigned int index )
{
  if(config.unlawwarpfloor[index] > 0 )
		return config.unlawwarpfloor[index];
	else
		return -1;
}
#endif

#ifdef _WATCH_FLOOR
int getWatchFloor( unsigned int index )
{
  if(config.watchfloor[index] > 0 )
		return config.watchfloor[index];
	else
		return -1;
}
char* getWatchFloorCF( void )
{
	return (config.watchfloor[0]>0)? "是":"否";
}
#endif

#ifdef _BATTLE_FLOOR
int getBattleFloor( unsigned int index )
{
  if(config.battlefloor[index] > 0 )
		return config.battlefloor[index];
	else
		return -1;
}
char* getBattleFloorCF( void )
{
	return (config.battlefloor[0]>0)? "是":"否";
}
#endif

#ifdef _ANGEL_SUMMON

extern int mission_num;

BOOL LoadMissionList( )
{
	FILE* fp;
	int i = 0;

	mission_num = 0;

	fp = fopen("./data/mission.txt", "r");
	if (fp == NULL)
	{
		print("任务文件打开错误\n");
		return FALSE;
	}
	
	memset( missionlist, 0, sizeof(missionlist));

	while(1){
		char	line[1024];
		char	token[1024];
		int		mindex;
		if (fgets(line, sizeof(line), fp) == NULL)	break;
//		print("\n %s ", line);
		chop(line);
		// 以#为注解*******
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************

		//格式 #任务编号,必要等级,任务说明,奖品ID,限制时间(小时)
		
		getStringFromIndexWithDelim(line, ",", 1, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		mindex = atoi( token);

		if( mindex <= 0 || mindex >= MAXMISSION) 
			break;

		missionlist[mindex].id = mindex;

		getStringFromIndexWithDelim(line, ",", 2, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missionlist[mindex].level = atoi( token);

		getStringFromIndexWithDelim(line, ",", 3, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missionlist[mindex].eventflag, token);

		getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missionlist[mindex].detail, token);

		//getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
		//if (strcmp(token, "") == 0)	break;
		//strcpy( missionlist[mindex].bonus, token);

		getStringFromIndexWithDelim(line, ",", 5, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missionlist[mindex].limittime = atoi( token);
/*
		print("\nMISSION[%d] lv:%d ef:%s detail:%s limit:%d ", mindex,
			missionlist[mindex].level, missionlist[mindex].eventflag,
			missionlist[mindex].detail, missionlist[mindex].limittime );
*/
		mission_num++;
		//if (mission_num > MAXMISSION)	break;
	}
	fclose(fp);
	return TRUE;
}



BOOL LoadMissionCleanList( )
{
	// 格式... 使者,勇者,任务,奖赏
	FILE* fp;
	int	listindex =0;
	int i = 0;

	memset( missiontable, 0, sizeof(missiontable));
	fp = fopen("./data/missionclean.txt", "r");
	if (fp == NULL)
	{
		print("清除任务文件打开错误\n");
		return FALSE;
	}

	while(1){
		char	line[1024];
		char	token[1024];
		
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		print("\n %s ", line);
		chop(line);
		// 以#为注解*******
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		
		getStringFromIndexWithDelim(line, ",", 1, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missiontable[listindex].angelinfo, token);

		getStringFromIndexWithDelim(line, ",", 2, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( missiontable[listindex].heroinfo, token);

		getStringFromIndexWithDelim(line, ",", 3, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missiontable[listindex].mission = atoi( token);
		
		getStringFromIndexWithDelim(line, ",", 4, token, sizeof(token));
		//if (strcmp(token, "") == 0)	break;
		missiontable[listindex].flag = atoi( token);

		getStringFromIndexWithDelim(line, ",", 5, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		missiontable[listindex].time = atoi( token);

		print("\nMISSIONCLEAN[%d] %s %s %d %d %d", listindex,
			missiontable[listindex].angelinfo,
			missiontable[listindex].heroinfo,
			missiontable[listindex].mission,
			missiontable[listindex].flag,
			missiontable[listindex].time );

		listindex++;
		if ( listindex >= MAXMISSIONTABLE)	break;
	}
	fclose(fp);
	return TRUE;
}


#endif

#ifdef _JOBDAILY
extern  DailyFileType dailyfile[MAXDAILYLIST];
BOOL LoadJobdailyfile(void)
{
	char	line[20000];
	char	token[16384];
	int		listindex =0;
	int     i;
	FILE* fp;

	fp = fopen("./data/jobdaily.txt", "r");
	if (fp == NULL)
	{
		print("日常工作文件打开错误\n");
		return FALSE;
	}

	memset( dailyfile, 0, sizeof(dailyfile));

	while(1){		
		line[0]='\0';	
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		//print("\n %s ", line);
		chop(line);

		// #为注解
		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		
		getStringFromIndexWithDelim(line, "|", 1, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		dailyfile[listindex].jobid = atoi(token);

		getStringFromIndexWithDelim(line, "|", 2, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( dailyfile[listindex].rule, token);

		getStringFromIndexWithDelim(line, "|", 3, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		if(strlen(token)>64){
			print("\n任务说明过长:%d\n",strlen(token));
			return FALSE;
		}
		strcpy( dailyfile[listindex].explain, token);
		
		getStringFromIndexWithDelim(line, "|", 4, token, sizeof(token));
		if (strcmp(token, "") == 0)	break;
		strcpy( dailyfile[listindex].state, token);

		/*print("\ndailyfile[%d] %s %s %s %s", listindex,
			dailyfile[listindex].jobid,
			dailyfile[listindex].rule,
			dailyfile[listindex].explain,
			dailyfile[listindex].state);	
		*/
		listindex++;
		if ( listindex >= MAXDAILYLIST)	break;
	}
	fclose(fp);
	return TRUE;
}
#endif

#ifdef _USER_EXP_CF
BOOL LoadEXP( char* filename )
{
	FILE* fp;
	int i = 0;
	int MaxLevel=0;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}

	while(1){
		char	line[64], exp[64];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);

		if( line[0] == '#' )
			continue;
		for( i=0; i<strlen(line); i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		MaxLevel = MaxLevel + 1;
		if (MaxLevel >= 200)	break;
		easyGetTokenFromString(line, 2, exp, sizeof(exp));
		NeedLevelUpTbls[MaxLevel]=atoi(exp);
	}
	fclose(fp);
	return TRUE;
}

char* getEXPfile( void )
{
    return config.expfile;
}

int getNeedLevelUpTbls( int level )
{
    return NeedLevelUpTbls[level];
}

#endif

#ifdef _UNREG_NEMA
char* getUnregname( int index )
{
    return config.unregname[index];
}
#endif

#ifdef _TRANS_LEVEL_CF
int getChartrans( void )
{
		if(config.chartrans>6)
			config.chartrans=6;
    return config.chartrans;
}
int getPettrans( void )
{
		if(config.pettrans>2)
			return 2;
		else if(config.pettrans<-1)
			return -1;
    return config.pettrans;
}
int getYBLevel( void )
{
		if(config.yblevel>config.maxlevel)
			config.yblevel=config.maxlevel;
    return config.yblevel;
}
int getMaxLevel( void )
{
    return config.maxlevel;
}
#endif

#ifdef _POINT
char* getPoint( void )
{
		return (config.point>0)? "是":"否";
}
int getTransPoint( int index )
{
		return config.transpoint[index];
}
#endif

#ifdef _PET_UP
int getPetup( void )
{
		return config.petup;
}
#endif
#ifdef _LOOP_ANNOUNCE
char* getLoopAnnouncePath( void )
{
		return config.loopannouncepath;
}
int loadLoopAnnounce( void )
{
	FILE* fp;
	int i = 0;
	config.loopannouncemax=0;
	fp = fopen(config.loopannouncepath, "r");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	while(1){
		char	line[1024];
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);

		if( line[0] == '#' )
			continue;
		for( i=0; i<10; i++ ){
            if( line[i] == '#' ){
			    line[i] = '\0';
		        break;
			}
		}
		//*************************************
		strcpy(config.loopannounce[config.loopannouncemax],line);
		config.loopannouncemax++;
		if(config.loopannouncemax>=10) break;
	}
	fclose(fp);
	return TRUE;
}
int getLoopAnnounceTime( void )
{
    return (config.loopannouncetime<0)?-1:config.loopannouncetime;
}
int getLoopAnnounceMax( void )
{
    return (config.loopannouncemax>0)?config.loopannouncemax:0;
}
char* getLoopAnnounce( int index )
{
    return config.loopannounce[index];
}
#endif

#ifdef _SKILLUPPOINT_CF
int getSkup( void )
{
    return (config.skup>0)?config.skup:0;
}
#endif
#ifdef _RIDELEVEL
int getRideLevel( void )
{
    return config.ridelevel;
}
#endif
#ifdef _REVLEVEL
char* getRevLevel( void )
{
		return (config.revlevel>0)?"是":"否";
}
#endif
#ifdef _NEW_PLAYER_RIDE
char* getPlayerRide( void )
{
		if(config.npride>2)
			return "配套送虎加雷";
		else if(config.npride==2)
			return "配套送雷";
		else if(config.npride==1)
			return "配套送虎";
		else
			return "不送配套骑宠";
}
#endif

#ifdef _FIX_CHARLOOPS
int getCharloops( void )
{
    return config.charloops-1;
}
#endif

#ifdef _PLAYER_ANNOUNCE
int getPAnnounce( void )
{
    return (config.pannounce>-1)?config.pannounce:-1;
}
#endif
#ifdef _PLAYER_MOVE
int getPMove( void )
{
    return (config.pmove>-1)?config.pmove:-1;
}
#endif

int getrecvbuffer( void )
{
		if(config.recvbuffer<0)
	    return 0;
	  else if(config.recvbuffer>128)
	    return 128;
	  else
	  	return config.recvbuffer;
}

int getsendbuffer( void )
{
		if(config.sendbuffer<0)
	    return 0;
	  else if(config.sendbuffer>128)
	    return 128;
	  else
	  	return config.sendbuffer;
}

int getrecvlowatbuffer( void )
{
		if(config.recvlowatbuffer<0)
	    return 0;
	  else if(config.recvlowatbuffer>1024)
	    return 1024;
	  else
	  	return config.recvlowatbuffer;
}

int getrunlevel( void )
{
		if(config.runlevel<-20)
	    return -20;
	  else if(config.runlevel>19)
	    return 19;
	  else
	  	return config.runlevel;

}

#ifdef _SHOW_VIP_CF
int getShowVip( void )
{
		if(config.showvip>2)
			return 2;
		else if(config.showvip<0)
			return 0;
		else
			return config.showvip;
}
#endif


#ifdef _PLAYER_NUM
int getPlayerNum( void )
{
		return config.playernum;
}
void setPlayerNum( int num )
{
		config.playernum=num;
}
char* getPlayerBase( void )
{
		return config.playerbase;
}
#endif

#ifdef _BATTLE_GOLD
int getBattleGold( void )
{
		if(config.battlegold<0)
	    return 0;
	  else if(config.battlegold>100)
	    return 100;
	  else
	  	return config.battlegold;

}
#endif

#ifdef _ANGEL_TIME
int getAngelPlayerTime( void )
{
		return (config.angelplayertime>1)?config.angelplayertime:1;
}
int getAngelPlayerMun( void )
{
		return (config.angelplayermun>2)?config.angelplayermun:2;
}
#endif

#ifdef _RIDEMODE_20
int getRideMode( void )
{
		if(config.ridemode<0)
			config.ridemode=0;
		return config.ridemode;
}
#endif
#ifdef _FM_POINT_PK
char *getFmPointPK( void )
{
		return (config.fmpointpk>0)?"是":"否";
}
#endif
#ifdef _ENEMY_ACTION
int getEnemyAction( void )
{
		if(config.enemyact>100)
			return 100;
		else if(config.enemyact<1)
			return 1;
		else
			return config.enemyact;
}
#endif

#ifdef _FUSIONBEIT_TRANS
int getFusionbeitTrans( void )
{
		if(config.fusionbeittrans>2)
			return 2;
		else if(config.fusionbeittrans<0)
			return 0;
		else
			return config.fusionbeittrans;
}
#endif
int getCpuUse( void )
{
	return config.cpuuse;
}
#ifdef _CHECK_PEPEAT
int getCheckRepeat( void )
{
		return (config.CheckRepeat>0)?1:0;
}
#endif

#ifdef _FM_JOINLIMIT
int getJoinFamilyTime( void )
{
		return config.joinfamilytime;
}
#endif

#ifdef _NOWEN_EV
int getNowEvent(int Num)
{
	int i = config.NowEvent[Num];
	if (i <= 0)return -1;
	return i;
}

int getEndEvent(int Num)
{
	int i = config.EndEvent[Num];
	if (i <= 0)return -1;
	return i;
}

#endif
int getBalltePoint(int Num)
{
	int i = config.BattlePoint[Num];
	if (i <= 0)return -1;
	return i;
}
#ifdef _ALL_TALK
int getTheWorldTrans(void)
{
	int i = config.TheWorldTrans;
	if (i <= 0)return -1;
	return i;
}

int getTheWorldFame(void)
{
	int i = config.TheWorldFame;
	if (i <= 0)return -1;
	return i;
}
int getTheWorldTime(void)
{
	int i = config.TheWorldTime;
	if (i <= 0)return -1;
	return i;
}
int getTheWorldCnt(void)
{
	int i = config.TheWorldCnt;
	if (i <= 0)return -1;
	return i;
}
#endif
#ifdef _DIY_INSLAY
int getInslayNum(void)
{
	int i = config.InslayNum;
	if (i < 1)return 1;
	if (i > 5)return 5;
	return i;
}
#endif
int getSafeMode(void)
{
	int i = config.safemode;
	if (i < 1)return 0;
	if (i > 1)return 1;
	return i;
}

int getTradeTax(void)
{
	int i = config.TradeTax;
	if (i < 1)return 1;
	if (i > 100)return 100;
	return i;
}

#ifdef _MAP_HEAL
int getMapHeal(int Num)
{
	int i = config.MapHeal[Num];
	if(i>0)
		return i;
	else
		return -1;
}
#endif

#ifdef _VIP_RIDE
int getVipMsgType(void)
{
	int i = config.VipMsgType;
	if (i < 0)return 0;
	if (i > 3)return 3;
	return i;
}
#endif

#ifdef _VIP_BATTLE_EXP
int getVipBattleExp( void )
{
		if(config.VipBattleExp<0)
			return 0;
		return config.VipBattleExp;
}
#endif

int getItemPoolBug(void)
{
	int i = config.ItemPoolBug;
	if (i < 0)return 0;
	if (i > 1)return 1;
	return i;
}

int getSameIpLogin( void )
{
		if(config.SameIpLogin<0)
			return 0;
		return config.SameIpLogin;
}

int getPetRideTrans( void )
{
		if(config.PetRideTrans<0)
			return 0;
		else if (config.PetRideTrans>2)
			return 2;
		return config.PetRideTrans;
}

#ifdef _LUCK_MAN
int getLuckTime( void )
{
		if(config.lucktime<0)
			return 0;
		return config.lucktime;
}

int getLuckItem( void )
{
		char *itemarg=NULL;
		char itemnumstr[32];
		int itemnum=0;
		int present[20];
    int si=0,i;
		itemarg=config.luckitem;
		if( getStringFromIndexWithDelim( itemarg, "|", 1, itemnumstr, sizeof( itemnumstr)) == FALSE )
		return;
    itemnum = atoi(itemnumstr);
    if( itemnum > 20 )
		itemnum = 20;
		for(i=0;i<itemnum;i++){
		if( getStringFromIndexWithDelim( itemarg, "|", 2+i, itemnumstr, sizeof( itemnumstr)) )
		    present[i] = atoi(itemnumstr);
		}
		si = rand()%itemnum;
		return present[ si];
}
#endif

#ifdef _QUESTION_ONLINE
int getQustionTime( void )
{
		if(config.questiontime<0)
			return 0;
		return config.questiontime;
}

int getQuestionItem( int num )
{
		if(num<1) num=1;
		if(num>3) num=3;
		char *itemarg=NULL;
		char itemnumstr[32];
		int itemnum1=-1;
		int itemnum2=-1;
		int itemnum3=-1;
		itemarg=config.questionitem;
		if( getStringFromIndexWithDelim( itemarg, "|", 1, itemnumstr, sizeof( itemnumstr)) == FALSE )
		return -1;
    itemnum1 = atoi(itemnumstr);
    if( getStringFromIndexWithDelim( itemarg, "|", 2, itemnumstr, sizeof( itemnumstr)) == FALSE )
		return -1;
    itemnum2 = atoi(itemnumstr);
    if( getStringFromIndexWithDelim( itemarg, "|", 3, itemnumstr, sizeof( itemnumstr)) == FALSE )
		return -1;
    itemnum3 = atoi(itemnumstr);
    
    if(num==1)
    	return itemnum1;
    else if(num==2)
    	return itemnum2;
    else if(num==3)
    	return itemnum3;
}
#endif

#ifdef _NO_STREET_MAP
int getNoStreetMap( int num )
{
		if(num<1) num=1;
		if(num>5) num=5;
		char *maparg=NULL;
		char mapnumstr[32];
		int mapnum1=-1;
		int mapnum2=-1;
		int mapnum3=-1;
		int mapnum4=-1;
		int mapnum5=-1;
		maparg=config.nostreetmap;
		if( getStringFromIndexWithDelim( maparg, ",", 1, mapnumstr, sizeof( mapnumstr)) == FALSE )
		return -1;
    mapnum1 = atoi(mapnumstr);
    if( getStringFromIndexWithDelim( maparg, ",", 2, mapnumstr, sizeof( mapnumstr)) == FALSE )
		return -1;
    mapnum2 = atoi(mapnumstr);
    if( getStringFromIndexWithDelim( maparg, ",", 3, mapnumstr, sizeof( mapnumstr)) == FALSE )
		return -1;
    mapnum3 = atoi(mapnumstr);
    if( getStringFromIndexWithDelim( maparg, ",", 4, mapnumstr, sizeof( mapnumstr)) == FALSE )
		return -1;
    mapnum4 = atoi(mapnumstr);
    if( getStringFromIndexWithDelim( maparg, ",", 5, mapnumstr, sizeof( mapnumstr)) == FALSE )
		return -1;
    mapnum5 = atoi(mapnumstr);
    
    if(num==1)
    	return mapnum1;
    else if(num==2)
    	return mapnum2;
    else if(num==3)
    	return mapnum3;
    else if(num==4)
    	return mapnum4;
    else if(num==5)
    	return mapnum5;
}
#endif

#ifdef _STREET_FAX
int getStreetFax( int num )
{
	if(num<0 || num>4 )
		return 100;
	char *streetfaxarg=config.streetfax;
	char streetfaxstr[32];
	if( getStringFromIndexWithDelim( streetfaxarg, ",", num+1, streetfaxstr, sizeof( streetfaxstr)) != FALSE )
		return atoi(streetfaxstr);
	return 100;
}
#endif

int getFmWarTime( int num )
{
	char *timearg=NULL;
	char timestr[5];
	timearg = config.fmwartime;
	if(num==1){
		if( getStringFromIndexWithDelim( timearg, ",", 1, timestr, sizeof( timestr)) == FALSE )
			return 18;
		return atoi(timestr);
	}
	else if(num==2){
		if( getStringFromIndexWithDelim( timearg, ",", 2, timestr, sizeof( timestr)) == FALSE )
			return 1;
		return atoi(timestr);
	}
	else{
		return -1;
	}
}

#ifdef _JZ_NEWSCRIPT_LUA
char *getLuaFile(void)
{
	return config.luafile;
}
#endif

#ifdef _TRANS7_POINT
int getTrans7Point(void)
{
	return config.trans7point;
}
#endif
#ifdef _NOJOB_PK
int getNoJobPkMap(void)
{
	return config.nojobpkmap;
}
#endif

#ifdef _UNTEXT_TALK
BOOL LoadUnText( void )
{
	FILE* untextfp;
	int uni;
	textcnt = 0;
//	untext = (char **)malloc(50 * sizeof(char *));
	untextfp = fopen("./data/untext.txt", "r");
	if (untextfp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}

	while(1){
		char	untestline[64];
		if (fgets(untestline, sizeof(untestline), untextfp) == NULL)	break;
		chop(untestline);

		if( untestline[0] == '#' )
			continue;
		for( uni=0; uni<strlen(untestline); uni++ ){
            if( untestline[uni] == '#' ){
			    untestline[uni] = '\0';
		        break;
			}
		}
		strcpy(untext[textcnt],untestline);
		textcnt++;
		if(textcnt>=100) break;
	}
	fclose(untextfp);
	return TRUE;
}

char *getUnText(int num)
{
	if(num<textcnt)
		return untext[num];
	else
		return untext[0];
}
#endif


char *getStreetItemUnName(int num)
{
	char *streetarg=NULL;
	char streetstr[64];
	streetarg = config.streetitemunname;
	if(num>30) num = 1;
	if( getStringFromIndexWithDelim( streetarg, ",", num, streetstr, sizeof( streetstr)) == FALSE )
		return "空";
	return streetstr;
}

#ifdef _NO_ATTACK
int getAttTime(void)
{
	return config.atttime;
}
int getAttSafeTime(void)
{
	return config.attsafetime;
}
int getAttCnt(void)
{
	return config.attcnt;
}
int getLateTime(void)
{
	return config.latetime;
}
int getAttDmeTime(void)
{
	return config.attdmetime;
}
int getAttDmeCnt(void)
{
	return config.attdmecnt;
}
#endif
char* getNoAttIp(int num)
{
	if(num<0) num=0;
	else if(num>4) num=4;
	return config.noattip[num];
}
#ifdef _NO_TRANS_ANGLE
int getTrans6Angle(void)
{
	if(config.trans6angle<0)
		config.trans6angle=0;
	else if(config.trans6angle>1)
		config.trans6angle=1;
	return config.trans6angle;
}
#endif
#ifdef _PET_BUG
int getPetPoint(int num)
{
	if(num<0) num=0;
	else if(num>3) num=3;
	return config.petpoint[num];
}
#endif
#ifdef _VIGOR_SYS
int getVigorTime(int num)
{
	if(num<0) num==0;
	else if(num >7) num==7;
	int i;
	i = config.vigortime[num];
	if(i<0) i=0;
	return i;
}
int getVigorMax(void)
{
	return config.vigormax;
}
#endif
#ifdef _AUTO_PK
int getAutoPkTime(void)
{
	return config.autopktime;
}
int getAutoPkMinNum(void)
{
	return config.autopkminnum;
}
int getAutoPkBattleTime(void)
{
	return config.autopkbattletime;
}
int getAutoPkMinLv(void)
{
	return config.autopkminlv;
}
int getAutoPkVigorPkNum(void)
{
	return config.autopkvigorpknum;
}
int getAutoPkVigorPkCnt(void)
{
	return config.autopkvigorpkcnt;
}
#endif
#ifdef	_BATTLEMAP_CTRL
int getBattleTimeFloor(int num)
{
	char *timearg=NULL;
	char timestr[10];
	timearg = config.BattleTime[num];
	if( getStringFromIndexWithDelim( timearg, ",", 1, timestr, sizeof( timestr)) == FALSE )
		return -1;
	return atoi(timestr);
}
int getBattleTimePlayerNum(int num)
{
	char *timearg=NULL;
	char timestr[10];
	timearg = config.BattleTime[num];
	if( getStringFromIndexWithDelim( timearg, ",", 2, timestr, sizeof( timestr)) == FALSE )
		return -1;
	return atoi(timestr);
}
int getBattleTimeSec(int num)
{
	char *timearg=NULL;
	char timestr[10];
	timearg = config.BattleTime[num];
	if( getStringFromIndexWithDelim( timearg, ",", 3, timestr, sizeof( timestr)) == FALSE )
		return -1;
	return atoi(timestr);
}
#endif
#ifdef	_FM_SERVERNO_SYS
int getFmServerNo(void)
{
	return config.fmserverno;
}
char* getFmServerMsg(void)
{
	return config.fmservermsg;
}
#endif
#ifdef	_ZHIPIAO_SYSTEM
int getZhiPiao(void)
{
	return config.zhipiao;
}
int getPiaoTime(void)
{
	return config.piaotime;
}
#endif
#ifdef	_FMWAR_MSG
char* getFmWarMsg(void)
{
	return config.fmwarmsg;
}
#endif
#ifdef	_FM_WELFARE
char* getFmWelfare(int num)
{
	return config.fmwelfare[num];
}
#endif
#ifdef	_FMZUZHANG_MSG
char* getFmZuzhang(void)
{
	return config.fmzuzhang;
}
#endif
#ifdef	_PETMAIL_LV
int getPetMailLv(void){
	return config.petmaillv;
}
int getPetMailCf(void){
	return config.petmailcf;
}
#endif
#ifdef	_FMWAR_PLAYERNUM	
int getFmWarPlayerNum(void){
	if(config.fmwarplayernum<50) return 50;
	return config.fmwarplayernum;
}
#endif
#ifdef _WAN_FIX
int getVsFlg(void){
	if(config.vsflg>3 || config.vsflg<0) return 0;
	return config.vsflg;
}
#endif
#ifdef	_WARNPC_CTRL
int getPartyMap(int num){
	char *patymaparg=NULL;
	char patymapstr[10];
	patymaparg = config.partymap;
	if( getStringFromIndexWithDelim( patymaparg, ",", num, patymapstr, sizeof( patymapstr)) == FALSE )
		return -1;
	return atoi(patymapstr);
}
int getMapSameIp(int num){
	char *mapsameiparg=NULL;
	char mapsameipstr[10];
	mapsameiparg = config.mapsameip;
	if( getStringFromIndexWithDelim( mapsameiparg, ",", num, mapsameipstr, sizeof( mapsameipstr)) == FALSE )
		return -1;
	return atoi(mapsameipstr);
}
#endif
#ifdef	_NO_DAOLUAN
int getStreetTrn(void){
	return config.streettrn;
}
int getTalkTrn(void){
	return config.talktrn;
}
int getTalkLv(void){
	return config.talklv;
}
char* getTalkMsg(void){
	return config.talkmsg;
}
char* getTalkName(void){
	return config.talkname;
}
#endif
#ifdef _NO_FULLPLAYER_ATT
int getNoFullPlayer(void){
	return config.nofullplayer;
}
int getNoFull2Player(void){
	return config.nofull2player;
}
int getNoCdkeyPlayer(void){
	if(config.nocdkeyplayer<0 || config.nocdkeyplayer>2000)
		return 0;
	return config.nocdkeyplayer;
}
int getNoCdkeyMode(void){
	if(config.nocdkeymode<0 || config.nocdkeymode>2)
		return 0;
	return config.nocdkeymode;
}
int getNoCdkeyType(void){
	if(config.nocdkeytype<0 || config.nocdkeytype>2)
		return 0;
	return config.nocdkeytype;
}
int getNoFullTime(void){
	return config.nofulltime;
}
int getFengType(void){
	return config.fengtype;
}
int getNoFullEndPlayer(void){
	return config.nofullendplayer;
}
int getNoFullEndTime(void){
	return config.nofullendtime;
}
int getManRenNum(void){
	return config.manrennum;
}
int getBigBao(void){
	if(config.bigbao<=0) return 1024;
	return config.bigbao;
}
int getBigBao2(void){
	if(config.bigbao2<=0) return 1024;
	return config.bigbao2;
}
#endif
#ifdef _ATTREVERSE_FIX
int getAttReverse(void){
	return config.attreverse;
}
#endif
#ifdef _NEWFM_GOLD
int getNewFmGold(void){
	return config.newfmgold;
}
#endif
#ifdef	_MERGE_TIME
int getMergeTime(void){
	if(config.mergetime<0) return 0;
	else if(config.mergetime>5) return 5;
	return config.mergetime;
}
#endif
#ifdef	_PETUP_GET_EXP
int getPetUpGetExp(void){
	if(config.petupgetexp<0) return 0;
	else if(config.petupgetexp>1) return 1;
	return config.petupgetexp;
}
#endif
#ifdef	_MM_NO_JIAOBEN
int getNoMMJiaoben(void){
	if(config.nommjiaoben<0) return 0;
	else if(config.nommjiaoben>1) return 1;
	return config.nommjiaoben;
}
int getEditBaseTime(void){
	if(config.editbasetime<0) return 0;
	else if(config.editbasetime>100) return 100;
	return config.editbasetime;
}
#endif
#ifdef	_DP_140_160_MODE
int getNewDpMode(void){
	if(config.newdpmode!=1) return 0;
	return 1;
}
#endif
#ifdef	_LOOK_STREET_TIME
int getLookStreetTime(void){
	if(config.lookstreettime<0) return 0;
	else if(config.lookstreettime>3600) return 3600;
	return config.lookstreettime;
}
#endif	
#ifdef	_BATTLE_EQUIT_OTHER
int getBattleEquitOther(void){
	if(config.battleequitother<0) return 0;
	else if(config.battleequitother>1) return 1;
	return config.battleequitother;
}
#endif
#ifdef	_PK_LUCK_MAN
int getPkLuckFloor(int floor){
	char *pkluckfloorarg=NULL;
	char pkluckfloorstr[10];
	pkluckfloorarg = config.pkluckfloor;
	int i;
	for(i=1;i<=10;i++){
		if( getStringFromIndexWithDelim( pkluckfloorarg, ",", i, pkluckfloorstr, sizeof( pkluckfloorstr)) == FALSE )
			return -1;
		if(floor == atoi(pkluckfloorstr))
			return i;
	}
	return -1;
}
char* getPkLuckMapName(int num){
	if(num<0) num = 0;
	else if(num>9) num = 9;
	return config.pkluckmapname[num];
}
int getPkLuckTimeMin(void){
	char *pklucktimearg=NULL;
	char pklucktimestr[10];
	pklucktimearg = config.pklucktime;
	if( getStringFromIndexWithDelim( pklucktimearg, ",", 1, pklucktimestr, sizeof( pklucktimestr)) == FALSE )
		return -1;
	return atoi(pklucktimestr);
}
int getPkLuckTimeMax(void){
	char *pklucktimearg=NULL;
	char pklucktimestr[10];
	pklucktimearg = config.pklucktime;
	if( getStringFromIndexWithDelim( pklucktimearg, ",", 2, pklucktimestr, sizeof( pklucktimestr)) == FALSE )
		return -1;
	return atoi(pklucktimestr);
}
int getPkLuckNum(void){
	char *pklucknumarg=NULL;
	char pklucknumstr[10];
	int pklucknummin,pklucknummax;
	pklucknumarg = config.pklucknum;
	if( getStringFromIndexWithDelim( pklucknumarg, "-", 1, pklucknumstr, sizeof( pklucknumstr)) == FALSE )
		pklucknummin = 1;
	pklucknummin = atoi(pklucknumstr);
	if( getStringFromIndexWithDelim( pklucknumarg, "-", 2, pklucknumstr, sizeof( pklucknumstr)) == FALSE )
		pklucknummax = 1;
	pklucknummax = atoi(pklucknumstr);
	if(pklucknummin == pklucknummax){
		return pklucknummin;
	}else{
		return RAND(pklucknummin,pklucknummax);
	}
	return pklucknummin;
}
int getPkLuckFame(void){
	char *pkluckarg=NULL;
	char pkluckstr[256];
	char pkluckbuf[10];
	pkluckarg = config.pkluckbuf;
	if( getStringFromIndexWithDelim( pkluckarg, "|", 1, pkluckstr, sizeof( pkluckstr)) == FALSE )
		return 0;
	int cnt = 1;
	while(1){
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			break;
		cnt++;
	}
	if(cnt==1)
		return atoi(pkluckstr);
	else{
		cnt = RAND(1,cnt);
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			return 0;
		return atoi(pkluckbuf);
	}
}
int getPkLuckVigor(void){
	char *pkluckarg=NULL;
	char pkluckstr[256];
	char pkluckbuf[10];
	pkluckarg = config.pkluckbuf;
	if( getStringFromIndexWithDelim( pkluckarg, "|", 2, pkluckstr, sizeof( pkluckstr)) == FALSE )
		return 0;
	int cnt = 1;
	while(1){
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			break;
		cnt++;
	}
	if(cnt==1)
		return atoi(pkluckstr);
	else{
		cnt = RAND(1,cnt);
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			return 0;
		return atoi(pkluckbuf);
	}
}
int getPkLuckVipPoint(void){
	char *pkluckarg=NULL;
	char pkluckstr[256];
	char pkluckbuf[10];
	pkluckarg = config.pkluckbuf;
	if( getStringFromIndexWithDelim( pkluckarg, "|", 3, pkluckstr, sizeof( pkluckstr)) == FALSE )
		return 0;
	int cnt = 1;
	while(1){
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			break;
		cnt++;
	}
	if(cnt==1)
		return atoi(pkluckstr);
	else{
		cnt = RAND(1,cnt);
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			return 0;
		return atoi(pkluckbuf);
	}
}
int getPkLuckAmPoint(void){
	char *pkluckarg=NULL;
	char pkluckstr[256];
	char pkluckbuf[10];
	pkluckarg = config.pkluckbuf;
	if( getStringFromIndexWithDelim( pkluckarg, "|", 4, pkluckstr, sizeof( pkluckstr)) == FALSE )
		return 0;
	int cnt = 1;
	while(1){
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			break;
		cnt++;
	}
	if(cnt==1)
		return atoi(pkluckstr);
	else{
		cnt = RAND(1,cnt);
		if(getStringFromIndexWithDelim( pkluckstr, ",", cnt, pkluckbuf, sizeof( pkluckbuf)) == FALSE)
			return 0;
		return atoi(pkluckbuf);
	}
}
int getPkLuckLevel(void){
	return config.pklucklevel;
}
int getPkLuckBattleTurn(void){
	return config.pkluckbattleturn;
}
#endif
#ifdef	_NO_DAOLUAN
int getTalkFloor(int num){
	char *talkfloorarg=NULL;
	char talkfloorstr[10];
	talkfloorarg = config.talkfloor;
	if( getStringFromIndexWithDelim( talkfloorarg, ",", num, talkfloorstr, sizeof( talkfloorstr)) == FALSE )
		return -1;
	return atoi(talkfloorstr);
}
#endif
#ifdef	_SUPERMAN_FIX
int getSuperManPoint(void){
	if(config.supermanpoint<=0) return -1;
	return config.supermanpoint;
}
#endif
#ifdef	_PICKUP_ITEM_OTHER
int getPickupItem(void){
	if(config.pickupitem<=0) return 0;
	return 1;
}
char* getPickItemId(void){
	return config.pickitemid;
}
#endif
#ifdef	_FM_LIAOLI
int getFmLiaoLiType(void){
	if(config.fmliaolitype<=0) return 0;
	return 1;
}
#endif
#ifdef	_TRADE_ITEM_FIX
int getTradeItemFix(void){
	if(config.tradeitemfix<=0) return 0;
	return 1;
}
#endif
#ifdef	_PETMAIL_TIME
int getPetMailTime(void){
	if(config.petmailtime<=0) return 0;
	return config.petmailtime;
}
#endif
int getRandMin(void){
	if(config.randmin<=1) return 1;
	return config.randmin;
}
int getRandMax(void){
	if(config.randmax<=1) return 1;
	return config.randmax;
}
#ifdef	_GJS_TYPE
int getGjsType(void){
	if(config.gjstype<1) return 0;
	return 1;
}
#endif
int getTicketCf(void){
	if(config.ticketcf<1) return 0;
	return 1;
}
char* getTicketMsg(void){
	return config.ticketmsg;
}

char* getDefaultKey(int cli_fd){
	if(config.satype==95){
		if(cli_fd == -1)
			return "www.longzoro.com";//2.5
		char fddefaultkey[32];
		memset(fddefaultkey, 0, sizeof(fddefaultkey));
		CONNECT_getDefaultKey( cli_fd, fddefaultkey, sizeof(fddefaultkey));
		if(strlen(fddefaultkey)<1)
			return "www.longzoro.com";//2.5
		else
			return fddefaultkey;//2.5
	}
	return "www.longzoro.com";
}
char* getRunningKey(void){
		return "www.longzoro.com";//2.5
}
char* getSaVersion(void){
	if(config.satype==95){
		return "Z";//9.5
	}
	return "K";//2.5
}
int getSaType(void){
	return config.satype;
}
#ifdef	_BATTLEEND_FIX
int getBattleEndMode(void){
	if(config.battleendmode!=1) return 0;
	return 1;
}
#endif

#ifdef	_BIG_POOL_TYPE
int getPetPoolType(void){
	if(config.petpooltype!=1) return 0;
	return 1;
}
int getItemPoolType(void){
	if(config.itempooltype!=1) return 0;
	return 1;
}
#endif

int getNoStayBattle(void){
	if(config.nostaybattle!=1) return 0;
	return 1;
}

int getRightNum(void){
	return config.rightnum;
}

int getRightMode(void){
	if(config.rightmode!=1) return 0;
	return 1;
}

int getErrorMode(void){
	if(config.errormode!=1) return 0;
	return 1;
}

#ifdef	_FLOOR_PARTY_CTRL
int getPartyFloor(int num){
	char *partyfloorarg=NULL;
	char partyfloorstr[10];
	partyfloorarg = config.partyfloor;
	if( getStringFromIndexWithDelim( partyfloorarg, ",", num, partyfloorstr, sizeof( partyfloorstr)) == FALSE )
		return -1;
	return atoi(partyfloorstr);
}
#endif

int getBattleTimtOut(void){
	if(config.battletimeout<=0) return 0;
	return config.battletimeout;
}

int getReloadNpcTime(void){
	if(config.reloadnpctime<=0) return 0;
	return config.reloadnpctime;
}

int getReloadNpcType(void){
	if(config.reloadnpctype<=0) return 0;
	return 1;
}

int getOfflineBattleTime(void){
	if(config.offlinebattletime<=0) return 2500;
	return config.offlinebattletime;
}

int getOfflineCf(void){
	if(config.offlinecf<=0 || config.offlinecf>2) return 0;
	return config.offlinecf;
}

int getOfflineMaxNum(void){
	if(config.offlinemaxnum<=0) return 0;
	return config.offlinemaxnum;
}

int getOfflineJqmMaxNum(void){
	if(config.offlinejqmmaxnum<=0) return 0;
	return config.offlinejqmmaxnum;
}

int getKongType(void){
	if(config.kongtype!=1) return 0;
	return 1;
}

int getJiFenBaiTan(void){
	if(config.jifenbaitan!=1) return 0;
	return 1;
}

int getNoPkMap(void){
	if(config.nopkmap<=0) return 0;
	return config.nopkmap;
}

int getFmBuffTrans(void){
	if(config.fmbufftrans<=0) return 0;
	return config.fmbufftrans;
}

char* getNoMpMagic(void){
	return config.nompmagic;
}

int getLoginJqmType(void){
	if(config.loginjqmtype!=1) return 0;
	return config.loginjqmtype;
}

char* getFmMagicMpRate(int num){
	if(num<=0) return config.fmmagicmprate[0];
	else return config.fmmagicmprate[1];
}

int getSameIpNum(void){
	if(config.sameipnum<=0) return 0;
	return config.sameipnum;
}

int getSameJqmNum(void){
	if(config.samejqmnum<=0) return 0;
	return config.samejqmnum;
}

int getSameJqmAllNum(void){
	if(config.samejqmallnum<=0) return 0;
	return config.samejqmallnum;
}

int getMmType(void){
	if(config.mmtype<=0) return 0;
	return 1;
}

char* getMmMsg(void){
	return config.mmmsg;
}

int getLockType(void){
	if(config.locktype<=0) return 0;
	return config.locktype;
}

int getAllTalkPoint(void){
	if(config.alltalkpoint<=0) return 0;
	return config.alltalkpoint;
}

#ifdef	_NO_RIDE_ID
char* getNoRideId(void){
	return config.norideid;
}
#endif

int getLoginType(void){
	if(config.logintype<=0) return 0;
	return 1;
}



#ifdef _PETTRANS_RANGE
void setPetTransRangeX(int num){
	if(config.pettransrangex<=0) return ;
	if(config.pettransrangex>=4) return ;
	config.pettransrangex=num;
	return;
}

int  getPetTransRangeX(void){
	if(config.pettransrangex<=0) return 0;
	return config.pettransrangex;
}

void setPetTransRangeY(int num){
	if(config.pettransrangey<=0) return ;
	if(config.pettransrangey>=8) return ;
	config.pettransrangey=num;
	return;
}

int  getPetTransRangeY(void){
	if(config.pettransrangey<=0) return 0;
	return config.pettransrangey;
}

void setPetTransRangeZ(int num){
	if(config.pettransrangez<=-5) return ;
	if(config.pettransrangez>=5) return ;
	config.pettransrangez=num;
	return;
}

int  getPetTransRangeZ(void){
	if(config.pettransrangez<=-5) return -5;
	return config.pettransrangez;
}
#endif

#ifdef _ITEM_LUA
const char *getitemluafile(void){
	return config.itemluafile;
}
BOOL ITEM_LuaInit(const char * _FileName){
	if (strlen( _FileName) != 0) return TRUE;
	return FALSE;
}

#endif

#ifdef _SAME_IP_ONLINE_NUM
int getSameIpOnlineNum()
{
		return config.sameiponlinenum;
}
#endif

#ifdef _CHECK_SEVER_IP
int checkServerIp( unsigned int ip )
{
	char serverip[64];
	char cliip[64];
	int i = 1;

	int a, b, c, d;
	
	a=(ip % 0x100); ip=ip / 0x100;
	b=(ip % 0x100); ip=ip / 0x100;
	c=(ip % 0x100); ip=ip / 0x100;
	d=(ip % 0x100);
	
	sprintf(cliip, "%d.%d.%d.%d", a, b, c, d);

	while(getStringFromIndexWithDelim(config.serverip,",", i++, serverip, sizeof(serverip))==TRUE){
		if(strcmp(cliip, serverip) == 0){
			return TRUE;
		}
	}
	
	return FALSE;
}
#endif

#ifdef _CAX_ESC_REPORT
/**
 * 普通会员每日签到
 */
int getReportItem(unsigned int index) {
	if (config.reportitem[index] > 0)
		return config.reportitem[index];
	else
		return -1;
}

/**
 * VIP会员每日签到
 */
int getVipReportItem(unsigned int index) {
	if (config.vipreportitem[index] > 0)
		return config.vipreportitem[index];
	else
		return -1;
}
/**
 * 签到等级
 */
int getReportLv(void){
	return config.reportlv;
}
/**
 * 签到转数
 */
int getReportTa(void){
	return config.reportta;
}
#endif

#ifdef _PETSKILL_SHOP_LUA
char *getFreePetSkillShopPath()
{
		return config.freepetskillshoppath;
}
#endif

#ifdef _CAX_FAMEBUTTON
char* getfamebutton( void )
{
	return (config.famebutton>0)? "是":"否";
}
#endif

#ifdef _PET_TRANS_ABILITY
int getPetTransAbility()
{
	if(config.pettransability<=0)config.pettransability=0;
	return config.pettransability;
}
int getPetTransAbility1()
{
	if(config.pettransability1<=0)config.pettransability1=150;
	return config.pettransability1;
}
int getPetTransAbility2()
{
	if(config.pettransability2<=0)config.pettransability2=200;
	return config.pettransability2;
}
int getPetTransAbility3()
{
	if(config.pettransability3<=0)config.pettransability3=250;
	return config.pettransability3;
}
#endif

#ifdef _PET_3TRANS
int getMMPETID1(void)
{
	return config.MMPETID1;
}
int getMMPETID2(void)
{
	return config.MMPETID2;
}
#endif

#ifdef _NO_MAGIC
int getnomagicmap(int num )
{
	char buf[128];
	getStringFromIndexWithDelim(config.nomagicmap,",", num+1, buf, sizeof(buf));
  return atoi(buf);
}
#endif

#ifdef _illegalpetskill_CF
int getIllegalPetskill(int index)
{
	return config.IllegalPetskill[index];
}
#endif

#ifdef _CAX_PET_EVOLUTION
char getpetevotyb(void)
{
	return config.petevotyb;
}

int getpetevotyb1(void)
{
		if(config.petevotyb1>250)
			return 250;
		else if(config.petevotyb1<1)
			return 150;
		else
			return config.petevotyb1;
}

#endif

#ifdef _TRANS_7_COLOR
/*
int getTranColor(void)
{
	return config.TranColor;
}
*/
char getping(void)
{
	return config.ping;
}

char getpetsummon(void)
{
	return config.petsummon;
}

#endif

#ifdef _CAX_PET_ITEMRIDE
char getpetitemride(void)
{
	return config.petitemride;
}
#endif

#ifdef _DAMMAGE_CALC
int getDamageCalc()
{
	if(config.dammagecalc > 0){
		return config.dammagecalc;
	}else{
		return 70;
	}
}
#endif

#ifdef _CAX_FAME_KOUFEI
int getfamekoufei()
{
	if(config.famekoufei > 0){
		return config.famekoufei;
	}
}
#endif

#ifdef _CAX_ITEM_ADDEXP
int getexptime()
{
	if(config.exptime > 0 && config.exptime <= 72000 ){
		return config.exptime;
	}else{
		return 72000;
		}
}
#endif

#ifdef _CAX_LNS_NLSUOXU
int GetConfigLineType(char *TM_ConfigName)
{
	int i;

	for( i = 0 ; i < arraysizeof( readconf ) ; i ++ ){
		if (strcmp(readconf[i].name,TM_ConfigName) == 0) break;
	}
	if (i == arraysizeof( readconf )) return -1;
	switch( readconf[i].valuetype  ){
	case CHAR:
		return 0;
		break;
	case SHORT:
		return 1;
		break;
	case INT:
		return 2;
		break;
	case DOUBLE:
		return 3;
		break;
	}
}

char* GetConfigLineVal( char* TM_ConfigName )
{
	char* filename = getConfigfilename();
	FILE* f=NULL;
	char linebuf[256];                  /* 域垫  心  戈田永白央 */
	int linenum=0;                      /* 垫醒毛醒尹月 */
	char    realopenfilename[256];      /*    端卞open 允月白央奶伙  */
	char secondToken[256];
	char    hostname[128];


	if( f == NULL ){
		f=fopen( filename , "r" );          /* 白央奶伙及左□皿件 */
		if( f == NULL ){
			print( "Can't open %s\n", filename );
			return FALSE;
		}
	}

	/* 域垫勿勾  心  戈 */
	while( fgets( linebuf , sizeof( linebuf ), f ) ){
		char firstToken[256];       /*1    及  侬  */
		int i;                      /*伙□皿  醒*/
		int ret;                    /*伉正□件戊□玉*/

		linenum ++;

		deleteWhiteSpace(linebuf);          /* remove whitespace    */

		if( linebuf[0] == '#' )continue;        /* comment */
		if( linebuf[0] == '\n' )continue;       /* none    */

		chomp( linebuf );                    /* remove tail newline  */

		/* delim "=" 匹  赓(1)及玄□弁件毛  月*/
		ret = getStringFromIndexWithDelim( linebuf , "=",  1, firstToken,
			sizeof(firstToken) );
		if( ret == FALSE ){
			print( "Find error at %s in line %d. Ignore\n",
				filename , linenum);
			continue;
		}
		if (strcmp(firstToken,TM_ConfigName) != 0) continue;
		/* readconf 及扔奶术匹伙□皿 */
		for( i = 0 ; i < arraysizeof( readconf ) ; i ++ ){
			if( strcmp( readconf[i].name ,firstToken ) == 0 ){
				/* match */
				/* delim "=" 匹2    及玄□弁件毛  月*/
				ret = getStringFromIndexWithDelim( linebuf , "=" , 2
					, secondToken ,
					sizeof(secondToken) );

				/* NULL  侬井升丹井毛譬屯月 */
				if( ret == FALSE ){
					print( "Find error at %s in line %d. Ignore",
						filename , linenum);
					break;
				}
				return secondToken;
				break;
			}
		}
		break;
	}
	fclose( f );
	lastConfig();
	return NULL;
}

int getCopymapstartingID( void )
{
	return config.mapstart;
}
#endif

#ifdef _SHARE_EXP
int getExpShare(void){
	if (config.expshare ==1)
		return 1;
	else
		return 0;
}
#endif

#ifdef _TEAM_ADDEXP
int getTeamAddExp(void){
	if (config.teamaddexp >0)
		return config.teamaddexp;
	else
		return 0;
}
#endif

#ifdef _NO_STW_ENEMY
int getNoSTWNenemy( void )
{
	return config.nostwenemy;
}

int getNoSTWNenemyPoint( void )
{
	return config.nostwenemypoint;
}
#endif

#ifdef _DEX_FIX
int  getDexFixPer(void){
	if(config.dexfixper<=0) return 1;
	return config.dexfixper;
}
#endif
#ifdef _FM_EXP_ADD
unsigned int getFmAddExp( void )
{
    return config.fmaddexp;
}
#endif
