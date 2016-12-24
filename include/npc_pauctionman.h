0034:npc_petfusion.h:014E:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETFUSION_H__
#define __NPC_PETFUSION_H__


void NPC_PetFusionManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_PetFusionManInit( int meindex );
void NPC_PetFusionManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetFusionManLoop( int meindex);

#endif

0032:npc_petmaker.h:0E4:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETMAKER_H__
#define __NPC_PETMAKER_H__

void NPC_PetMakerTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetMakerInit( int meindex );

#endif
 /*__NPC_PETMAKER_H__*/
0038:npc_petracemaster.h:0156:1:14=48041e4b:16=47d21313:#ifndef NPC_PetRace_Master_
#define NPC_PetRace_Master_

BOOL NPC_PetRaceMasterInit( int meindex );
void NPC_PetRaceMasterLoop( int meindex );
void NPC_PetRaceMasterTalked( int meindex , int talkerindex , char *szMes , int color );
void NPC_PetRaceMasterWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data);

#endif

0034:npc_petracepet.h:0E2:1:14=48041e4b:16=47d21313:#ifndef NPC_PetRace_Pet_
#define NPC_PetRace_Pet_

BOOL NPC_PetRacePetInit( int meindex );
void NPC_PetRacePetLoop( int meindex );
void NPC_PetRacePetTalked( int meindex , int talkerindex , char *szMes , int color );

#endif

0032:npc_petshop.h:0178:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETSHOP_H__
#define __NPC_PETSHOP_H__

void NPC_PetShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetShopInit( int meindex );
void NPC_PetShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetShopLooked( int meindex , int lookedindex);


#endif 
/*__NPC_PETSHOP_H__*/
0037:npc_petskillshop.h:01E1:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETSKILLSHOP_H__
#define __NPC_PETSKILLSHOP_H__

void NPC_PetSkillShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetSkillShopInit( int meindex );
void NPC_PetSkillShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetSkillShopLooked( int meindex , int lookedindex);

void NPC_PetSkillMakeStr(int meindex,int toindex,int select);
		
#endif
 /*__NPC_WINDOWPETSKILLSHOP_H__*/
0036:npc_pettransman.h:0206:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETTRANSMAN_H__
#define __NPC_PETTRANSMAN_H__

BOOL NPC_PetTransManInit(int meindex);
void NPC_PetTransMan_selectWindow(int meindex,int toindex,int num,int select);
void NPC_PetTransManWindowTalked(int meindex,int talkerindex,int seqno,int select,char *data);
void NPC_PetTransManTalked(int meindex,int talkerindex,char *szMes,int color);
int NPC_PetTransManCheck( int meindex, int toindex, int select);
BOOL NPC_PetTransManStatus( int meindex, int toindex, int petNo);
#endif
 /*__NPC_PETTRANSMAN_H__*/
0032:npc_pkpetshop.h:00:1:14=48041e4b:16=47d21313:0037:npc_poolitemshop.h:0184:1:14=48041e4b:16=47d21313:#ifndef __NPC_POOLITEMSHOP_H__
#define __NPC_POOLITEMSHOP_H__


void NPC_PoolItemShopLoop( int meindex);
BOOL NPC_PoolItemShopInit( int meindex);
void NPC_PoolItemShopTalked( int meindex , int talkerindex , 
							char *szMes ,int color );
void NPC_PoolItemShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data);


#endif
 /*__NPC_POOLITEMSHOP_H__*/
002F:npc_quiz.h:02C0:1:14=48041e4b:16=47d21313:#ifndef __NPC_QUIZ_H__
#define __NPC_QUIZ_H__

typedef struct NPC_Quiz{

	int 	no;				//  锁及  寞
	int 	type;			//  锁及正奶皿  必□丞    ｝旦禾□汁)	
	int		level;  		//  锁及伊矛伙
	int 	answertype;		//蚕尹及蓟太井凶(ㄡ  ｝ㄢ  ｝签贿穴永民  
	int 	answerNo;		//  锁及蚕尹
	char	question[512]; 	//  锁
	char	select1[128]; 	//蓟  靼1
	char	select2[128]; 	//蓟  靼2
	char	select3[128]; 	//蓟  靼3

}NPC_QUIZ;


void NPC_QuizTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_QuizInit( int meindex );
void NPC_QuizWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

BOOL QUIZ_initQuiz( char *filename);


#endif 

/*__NPC_QUIZ_H__*/
0033:npc_riderman.h:017F:1:14=48041e4b:16=47d21313:#ifndef __NPC_RIDERMAN_H__
#define __NPC_RIDERMAN_H__

void NPC_RidermanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_RidermanInit( int meindex );
void NPC_RidermanLooked( int meindex, int lookedindex );
void NPC_RidermanWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif
 /*__NPC_RIDERMAN_H__*/
0037:npc_roomadminnew.h:03CA:1:14=48041e4b:16=47d21313:#ifndef __NPC_ROOMADMINNEW_H__
#define __NPC_ROOMADMINNEW_H__


void NPC_RoomAdminNewTalked( int meindex , int talkerindex , char *msg , int color );
void NPC_RoomAdminNewLoop( int meindex );
BOOL NPC_RoomAdminNewInit( int meindex );

BOOL NPC_RankingInit( int meindex );
void NPC_RankingTalked( int meindex , int talkerindex , char *msg ,int color );

BOOL NPC_PrintpassmanInit( int meindex );
void NPC_PrintpassmanTalked( int meindex , int talkerindex , char *msg ,int color );

typedef struct npc_roomadminnew_tag {
	int		expire;
	char	cdkey[CDKEYLEN];
	char	charaname[32];
	char	passwd[9];
}NPC_ROOMINFO;

typedef struct npc_roomadminnew_ranking_tag {
	int		gold;				/*   诳嗯喊 */
	int		biddate;			/*   诳凛棉 */
	char	cdkey[CDKEYLEN];	/*   午仄凶谛及    平□ */
	char	charaname[32];		/*   午仄凶谛及  蟆 */
	char	owntitle[32];		/* 惫寞*/
	
}NPC_RANKING_INFO;

#endif /*__NPC_ROOMADMINNEW_H__*/

BOOL NPC_RoomAdminNew_ReadFile( char *roomname, NPC_ROOMINFO *data);
0030:npc_sample.h:075:1:14=48041e4b:16=47d21313:#ifndef __NPC_SAMPLE_H__
#define __NPC_SAMPLE_H__

void NPC_SampleLoop( int index );

#endif 

 /*__NPC_SAMPLE_H__*/
0034:npc_savepoint.h:014F:1:14=48041e4b:16=47d21313:#ifndef __NPC_SAVEPOINT_H__
#define __NPC_SAVEPOINT_H__


void NPC_SavePointTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_SavePointInit( int meindex );
void NPC_SavePointWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);


#endif 

/*__NPC_SAVEPOINT_H__*/
0036:npc_scheduleman.h:0A5A:1:14=48041e4b:16=47d21313:#ifndef __NPC_SCHEDULEMAN_H__
#define __NPC_SCHEDULEMAN_H__

#define CHARNAME_MAX	32

// 最大 scheduleman 数量
#ifdef _FAMILY_MANORNUM_CHANGE
#define MAX_SCHEDULEMAN	20
#else
#define MAX_SCHEDULEMAN	12
#endif

// 每一个 scheduleman 控管的 schedule 数量
#define MAX_SCHEDULE	24

// schedule 列表的一页所能列出的最大数量
#define MAXSCHEDULEINONEWINDOW	10

// family 列表的一页所能列出的最大数量
#define MAXFAMILYINONEWINDOW	10

// 每一个 scheduleman 所使用的 schedule 储存位置
// 对应在 fmpks 的 ID*MAX_SCHEDULE 上 (ID=0~MAX_SCHEDULEMAN-1)
// ID 是写在 data/npc 中指定给 "id:" 的值

// dueltime = (隔天?10000:0) + (小时*100)

typedef struct tagFamilyPKSchedule {
	int	dueltime;	// 比赛时刻
	int	host_index;	// 主队家族
	char host_name[CHARNAME_MAX];	// 主队家族名称
	int	guest_index;	// 客队家族
	char guest_name[CHARNAME_MAX];	// 客队家族名称
	int	prepare_time;	// 准备时间 (1~40 分钟)
	int	max_player;	// 最大出场人数 (1~家族人数上限)
	int	flag;		// 状态
	int	win;		// 胜利条件设定
	int	challenge_timeout;	// 挑战时限
	int	setting_timeout;	// 设定状态的 timeout
	char gmsv_name[256];
} FamilyPKSchedule;

// 纪录在 FamilyPKSchedule (fmpks) 的 flag 值
#define FMPKS_FLAG_NONE		-1	// 没有任何排程
#define FMPKS_FLAG_CHALLENGE	0	// 等待客队同意中
#define FMPKS_FLAG_SETTING	1	// 主队正在设定排程 (取消时变成 NONE)
#define FMPKS_FLAG_CONFIRMING	2	// 客队正在同意中
#define FMPKS_FLAG_SCHEDULED	3	// 已经排好排程，尚未开打
#define FMPKS_FLAG_DUEL		4	// 开打中
#define FMPKS_FLAG_HOSTWIN	5	// 主队胜
#define FMPKS_FLAG_GUESTWIN	6	// 客队胜
#define FMPKS_FLAG_MANOR_BATTLEBEGIN	7	// 庄园挑战 战斗中
#define FMPKS_FLAG_MANOR_PREPARE	8	// 庄园挑战 准备中
#define FMPKS_FLAG_MANOR_PEACE	9	// 庄园挑战 休战中
#define FMPKS_FLAG_MANOR_OTHERPLANET	10	// 庄园挑战 在别的星球决战
#define FMPKS_FLAG_MANOR_BATTLEEND	11	// 庄园挑战 战斗结束
#define FMPKS_FLAG_MANOR_PEACE_SAVE	12	// 将庄园挑战存档
#define FMPKS_FLAG_MANOR_READYTOFIGHT	13	// (GM: manorpk) 全星系变成可约战状态
#define FMPKS_FLAG_MANOR_CLEANFLAG	14	// (GM: manorpk) 本星球清除状态
#ifdef _NEW_MANOR_LAW
#define FMPKS_FLAG_WAIT		15	// 进入挑战期,已记录家族气势,等待挑战排程
#endif

// 传给 client 的 flag
#define FLAG_NONE	-1
#define FLAG_MODIFY	0
#define FLAG_ACCEPT	1
#define FLAG_SETTING	2
#define FLAG_CONFIRM	3
#define FLAG_SCHEDULED	4

extern FamilyPKSchedule fmpks[MAX_SCHEDULE*MAX_SCHEDULEMAN];

BOOL NPC_SchedulemanInit(int meindex);
void NPC_SchedulemanTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_SchedulemanWindowTalked(int meindex, int talkerindex,
				 int seqno, int select, char *data);
void NPC_SchedulemanLoop(int meindex);

#endif 

/*__NPC_SCHEDULEMAN_H__*/
0035:npc_sellsthman.h:013A:1:14=48041e4b:16=47d21313:#ifndef _SELLSTH_MAN_H
#define _SELLSTH_MAN_H

void NPC_SellsthManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_SellsthManInit( int meindex );
void NPC_SellsthManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_SellsthManLoop( int meindex);

#endif
0034:npc_signboard.h:0124:1:14=48041e4b:16=47d21313:#ifndef __NPC_SIGNBOARD_H__
#define __NPC_SIGNBOARD_H__


BOOL NPC_SignBoardInit( int meindex );
void NPC_SignBoardWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_SignBoardLooked( int meindex , int lookedindex);


#endif
 /*__NPC_WINDOWSIGNBOAD_H__*/
0035:npc_simpleshop.h:0176:1:14=48041e4b:16=47d21313:#ifndef __NPC_SIMPLESHOP_H__
#define __NPC_SIMPLESHOP_H__

void NPC_SimpleShopTalked( int index, int talker, char *msg, int color );
void NPC_SimpleShopSpecialTalked( int index, int talker, char *msg, int color );
void NPC_LimitBuyInShopSpecialTalked( int index, int talker, char *msg, int color );
void NPC_SimpleShopInit( int meindex );

#endif 
 /*__NPC_SIMPLESHOP_H__*/
003A:npc_stoneserviceman.h:02AE:1:14=48041e4b:16=47d21313:#ifndef __NPC_STONESERVICEMAN_H__
#define __NPC_STONESERVICEMAN_H__

// 资料串列
typedef struct _List
{
  char Data[32];        // 资料
  struct _List *Next;   // 指向下一个List
}List,*LPList;    

BOOL NPC_StoneServiceManInit(int meindex);
void NPC_StoneServiceManLoop(int meindex);
void NPC_StoneServiceManTalked(int meindex,int talkerindex,char *msg,int color);
void NPC_StoneServiceManWindowTalked(int meindex,int talkerindex,int seqno,
                                     int select,char *data);
int SendToAP(int nAid,int nIndex,int nLen,void *Data1,void *Data2);
int RecvFromAP(int *nAid,int nIndex,int *nLen,LPList *Data);
int GetMsg(int meindex);
int ConnectToAP(void);
#endif
0035:npc_storyteller.h:0B9:1:14=48041e4b:16=47d21313:#ifndef _NPC_STORYTELLER_H_
#define _NPC_STORYTELLER_H_

BOOL NPC_StoryTellerInit( int meindex );
void NPC_StoryTellerTalked( int meindex , int talker , char *msg , int col );

#endif

0031:npc_sysinfo.h:0E4:1:14=48041e4b:16=47d21313:#ifndef __NPC_SYSINFO_H__
#define __NPC_SYSINFO_H__

BOOL NPC_SysinfoInit( int meindex );
void NPC_SysinfoLoop( int meindex );
void NPC_SysinfoTalked( int meindex, int tindex, char *msg, int color);
#endif /*__NPC_SYSINFO_H__*/
0032:npc_timeman.h:0167:1:14=48041e4b:16=47d21313:#ifndef __NPC_TIMEMAN_H__
#define __NPC_TIMEMAN_H__


void NPC_TimeManTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_TimeManInit( int meindex );

void NPC_TimeManWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );

#endif /*__NPC_TIMEMAN_H__*/
0034:npc_townpeople.h:0D1:1:14=48041e4b:16=47d21313:#ifndef __NPC_TOWNPEOPLE_H__
#define __NPC_TOWNPEOPLE_H__

void NPC_TownPeopleTalked( int index, int talker, char *msg, int color );
void NPC_TownPeopleInit( int meindex );

#endif  /* __NPC_TOWNPEOPLE_H__ */
0035:npc_transerman.h:013A:1:14=48041e4b:16=47d21313:#ifndef _TRANSER_MAN_H
#define _TRANSER_MAN_H

void NPC_TranserManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_TranserManInit( int meindex );
void NPC_TranserManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_TranserManLoop( int meindex);

#endif
0039:npc_transmigration.h:016A:1:14=48041e4b:16=47d21313:#ifndef __NPC_TRANSMIGRATION_H__
#define __NPC_TRANSMIGRATION_H__

void NPC_TransmigrationTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_TransmigrationInit( int meindex );
void NPC_TransmigrationWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

/*__NPC_TRANSMIGRATION_H__*/
