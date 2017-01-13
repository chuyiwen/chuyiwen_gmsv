#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "map_deal.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "family.h"

#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#include "chatmagic.h"
#endif

enum{
	NPC_EVENTMODE_NONE,
	NPC_EVENTMODE_OPEN,
	NPC_EVENTMODE_EVENT,
};

typedef struct {
	char	arg[32];
	int		born;
	int 	dead;
}NPC_TimeMan;

#define YOAKE 700
#define NICHIBOTU 300
#define SHOUGO 125
#define YONAKA 500

static NPC_TimeMan	TimeTble[] = {
	{"ALLNIGHT",	NICHIBOTU+1,	YOAKE},
	{"ALLNOON",		YOAKE+1,		NICHIBOTU},
	{"AM",			YONAKA+1,		SHOUGO},
	{"PM",			SHOUGO+1,		YONAKA},
	{"FORE",		YOAKE+1,		SHOUGO},
	{"AFTER",		SHOUGO+1,		NICHIBOTU},
	{"EVNING",		NICHIBOTU+1,	YONAKA},
	{"MORNING",		YONAKA+1,		YOAKE},
	{"FREE",		0,				1024},
};

extern void BATTLE_changeRideImage( int index );


#define MAXNPCPOINT 10

int year,mon,date,day,hour,min,sec;
BOOL ActionNpc_WarpPoint(int meindex,int talker,char *npcarg)
{
	char data[1024], buf1[256], buf2[256];
	int i=1, j=0;
	BOOL EvFlg=FALSE;

	struct	{
		int FLOOR;
		int X;
		int Y;
	}Points[MAXNPCPOINT];
	
	for( i=0;i<MAXNPCPOINT;i++)	{
		Points[i].FLOOR = -1;
		Points[i].X = -1;
		Points[i].Y = -1;
	}
	//NPC_POINT
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NPC_POINT", data, sizeof( data)) != NULL ) {
		int where=0;
		i=1;
		j=0;
		while( getStringFromIndexWithDelim( data, ";", i, buf1, sizeof( buf1)) != FALSE )	{
			i++;
			getStringFromIndexWithDelim( buf1, ",", 1, buf2, sizeof( buf2));
			Points[j].FLOOR = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 2, buf2, sizeof( buf2));
			Points[j].X = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 3, buf2, sizeof( buf2));
			Points[j].Y = atoi( buf2);
			j++;
			if( j >= MAXNPCPOINT )
				break;
		}
		where = RAND( 0, j-1);
		if( MAP_IsValidCoordinate( Points[where].FLOOR,	Points[where].X, Points[where].Y ) == TRUE ){
			CHAR_warpToSpecificPoint( meindex, Points[where].FLOOR,	Points[where].X, Points[where].Y);
			EvFlg = TRUE;
		}else	{
			for( i=0; i<10; i++)	{
				print("%d.[%d,%d,%d]\n", i, Points[i].FLOOR, Points[i].X, Points[i].Y);
			}
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "WARPPOINT", data, sizeof( data)) != NULL ) {
		int where=0;
		i=1;
		j=0;
		while( getStringFromIndexWithDelim( data, ";", i, buf1, sizeof( buf1)) != FALSE )	{
			i++;
			getStringFromIndexWithDelim( buf1, ",", 1, buf2, sizeof( buf2));
			Points[j].FLOOR = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 2, buf2, sizeof( buf2));
			Points[j].X = atoi( buf2);
			getStringFromIndexWithDelim( buf1, ",", 3, buf2, sizeof( buf2));
			Points[j].Y = atoi( buf2);
			j++;
			if( j >= MAXNPCPOINT )
				break;
		}
		where = RAND( 0, j-1);
		if( MAP_IsValidCoordinate( Points[where].FLOOR,	Points[where].X, Points[where].Y ) == TRUE ){
			CHAR_warpToSpecificPoint( talker, Points[where].FLOOR,	Points[where].X, Points[where].Y);
			EvFlg = TRUE;
		}else	{
			for( i=0; i<10; i++)	{
				print("%d.[%d,%d,%d]\n", i, Points[i].FLOOR, Points[i].X, Points[i].Y);
			}
		}

	}

	return EvFlg;
}

BOOL ActionNpc_CheckMenuFree( int meindex, int toindex, char *npcarg, int nums)
{
	char buf1[1024], buf2[256];
	if( nums < 1 ) return FALSE;

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CHECKPARTY", buf1, sizeof( buf1) ) != NULL){
		if( strstr( buf1, "TRUE") != 0 )	{
			if( Action_PartyCheck( meindex, toindex) == FALSE)	{
				CHAR_talkToCli( toindex, meindex, "请一个一个来！", CHAR_COLORYELLOW);
				return FALSE;
			}
		}
	}
	if( getStringFromIndexWithDelim( npcarg,"}", nums, buf1, sizeof( buf1)) == FALSE )
		return FALSE;

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2) ) == NULL)
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			if(NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Item", buf2, sizeof( buf2) ) == NULL)
			if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Pet", buf2, sizeof( buf2) ) == NULL)
				if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Abi_Pet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _TEACHER_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuItem", buf2, sizeof( buf2) ) == NULL)
			if(NPC_Util_GetStrFromStrWithDelim( buf1, "StuPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _RMB_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPoint", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbItem", buf2, sizeof( buf2) ) == NULL)
				if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPet", buf2, sizeof( buf2) ) == NULL)
#endif
			return FALSE;


	if( NPC_ActionPassCheck( meindex, toindex, buf2) == FALSE )	{
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			  if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Item", buf2, sizeof( buf2) ) == NULL)
			if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Pet", buf2, sizeof( buf2) ) == NULL)
				if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Abi_Pet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _TEACHER_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuItem", buf2, sizeof( buf2) ) == NULL)
			  if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _RMB_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPoint", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbItem", buf2, sizeof( buf2) ) == NULL)
				if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPet", buf2, sizeof( buf2) ) == NULL)
#endif
		{
			CHAR_talkToCli( toindex, meindex, "条件不足！", CHAR_COLORYELLOW);
			return FALSE;
		}
	}
	return Action_RunDoEventAction( meindex, toindex, buf1);
}

BOOL ActionNpc_CheckFree( int meindex, int toindex, char *npcarg)
{
	char buf1[2048], buf2[256];
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "CHECKPARTY", buf1, sizeof( buf1) ) != NULL){
		if( strstr( buf1, "TRUE") != 0 )	{
			if( Action_PartyCheck( meindex, toindex) == FALSE)	{
				CHAR_talkToCli( toindex, meindex, "请一个一个来！", CHAR_COLORYELLOW);
				return FALSE;
			}
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FREE", buf2, sizeof( buf2) ) == NULL)
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Item", buf2, sizeof( buf2) ) == NULL)
			if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Pet", buf2, sizeof( buf2) ) == NULL)
				if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Abi_Pet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _TEACHER_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuItem", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _RMB_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPoint", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbItem", buf2, sizeof( buf2) ) == NULL)
				if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPet", buf2, sizeof( buf2) ) == NULL)
#endif
			return FALSE;

	if( NPC_ActionPassCheck( meindex, toindex, buf2) == FALSE )	{
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _VIP_SHOP
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Item", buf2, sizeof( buf2) ) == NULL)
			if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Pet", buf2, sizeof( buf2) ) == NULL)
				if(NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Abi_Pet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _TEACHER_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuItem", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuPet", buf2, sizeof( buf2) ) == NULL)
#endif
#ifdef _RMB_SYSTEM
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPoint", buf2, sizeof( buf2) ) == NULL)
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbItem", buf2, sizeof( buf2) ) == NULL)
				if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPet", buf2, sizeof( buf2) ) == NULL)
#endif
		{
			CHAR_talkToCli( toindex, meindex, "条件不足！", CHAR_COLORYELLOW);
			return FALSE;
		}
	}
	return Action_RunDoEventAction( meindex, toindex, buf1);
}

#ifdef _CHANNEL_MODIFY
extern int *piOccChannelMember;
#endif

BOOL Action_RunDoEventAction( int meindex, int toindex, char *buf1)
{
	char buf2[2048];

#if 1 // Robin NPC执行动作前再一次检查FREE条件
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionPassCheck( meindex, toindex, buf2 ) == FALSE && buf2[0] == '\n' )	{
			print("\n 改封包!?跳过NPC的FREE检查!!:%s:%d,%d,%d ",
					CHAR_getChar(toindex, CHAR_CDKEY),
					CHAR_getInt(toindex, CHAR_FLOOR),
					CHAR_getInt(toindex, CHAR_X),
					CHAR_getInt(toindex, CHAR_Y) );
			CHAR_warpToSpecificPoint(toindex,117,225,13);
			return FALSE;
		}
	}
#endif


	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddGold", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddGold2( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelGold", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelGold( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddFame", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddFame( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelFame", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelFame( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddVipPoint", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddVipPoint( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelVipPoint", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelVipPoint( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddPoint", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddPoint( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelPoint", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelPoint( toindex, buf2) == FALSE )
			return FALSE;
	}
#ifdef	_VIGOR_SYS	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddVigor", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddVigor( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelVigor", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelVigor( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif	
	
#ifdef _MOMENTUM_NPC
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelMom", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionMomentum( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif
#ifdef _DP_NPC
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelDP", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDP( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelItem", buf2, sizeof( buf2)) != NULL ) {
		//print("\n DelItem:%s:%d ", __FILE__, __LINE__);
		if( NPC_ActionDelItem( toindex, buf2) == FALSE ) {
			//print("\n 改封包??NPC_ActionDelItem:%s:%s ", CHAR_getChar(toindex, CHAR_CDKEY), buf2);
			//CHAR_warpToSpecificPoint(toindex,117,225,13);
			return FALSE;
		}
	}

	//条件成立後npc所给的道具
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddItem( toindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "GetUpItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionUpItem( toindex, buf2) == FALSE )
			return FALSE;
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "HeChengMsg", buf2, sizeof( buf2)) != NULL ) {	
			int k;
			int playernum = CHAR_getPlayerMaxNum();
			char hcmsg[256];
			sprintf(hcmsg,"「合成达人」：恭喜玩家「%s」成功合成%s",CHAR_getChar(toindex,CHAR_NAME),buf2);
			for(k=0;k<playernum;k++){
				CHAR_talkToCli( k, -1, hcmsg, CHAR_COLORRED);
			}
		}
	}
#ifdef _VIP_SHOP
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionVipItem( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "VipPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionVipPet( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif	
#ifdef _VIP_SHOP
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Item", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionNewVipItem( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Pet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionNewVipPet( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "Vip_Abi_Pet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionNewVipAbiPet( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif	
#ifdef _STU_SYS
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionStuItem( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "StuPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionStuPet( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif	
#ifdef _RMB_SYSTEM
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPoint", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionRmbPoint( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbItem", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionRmbItem( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "RmbPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionRmbPet( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif	
#ifdef _TRANS_7_NPC
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TRANS7", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionTrans7( toindex, buf2) == FALSE )
			return FALSE;
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TRANSNEW7", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionTransNew7( toindex, buf2) == FALSE )
			return FALSE;
	}
#ifdef _TRANS7_POINT
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TRANSNEWSEVEN", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionTransNewSeven( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "NpcWarp", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionWarpNpc( meindex, buf2) == FALSE )
			return FALSE;
	}
	
#endif
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "DelPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionDelPet( toindex, buf2) == FALSE )
			return FALSE;
	}

#ifdef _NPC_NewDelPet
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "NewDelPet", buf2, sizeof( buf2)) != NULL ) {
        if( NPC_ActionNewDelPet( toindex, buf2) == FALSE )
			return FALSE;
	}
#endif

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddPet", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionAddPet( toindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TiaoZhanMsg", buf2, sizeof( buf2)) != NULL ) {
        if( NPC_ActionTiaoZhanMsg( toindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvEnd", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionSetEend( toindex, buf2) == FALSE )
			return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvNow", buf2, sizeof( buf2)) != NULL ) {
		if( NPC_ActionSetNow( toindex, buf2) == FALSE )
			return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "Event_End", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionSetEend( toindex, buf2) == FALSE )	{
			return FALSE;
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "Event_Now", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionSetNow( toindex, buf2) == FALSE )
			return FALSE;
	}


	if( NPC_Util_GetStrFromStrWithDelim( buf1, "EvClr", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionClearEvent( toindex, buf2) == FALSE )
			return FALSE;
	}

#ifdef _NPCCHANGE_PLAYERIMG
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHANGEBBI", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionChangePlayerBBI( meindex, toindex, buf2) == FALSE )
			return FALSE;
	}
#endif

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "SetLastTalkelder", buf2, sizeof( buf2) ) != NULL) {
		CHAR_setInt( toindex, CHAR_LASTTALKELDER, atoi( buf2));
	}

#ifdef _TYPE_TOXICATION
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TOXICATION", buf2, sizeof( buf2) ) != NULL) {
		int fd = getfdFromCharaIndex( toindex );
		if( strstr( buf2, "TRUE") != 0 ){
			setToxication( fd, 1);
			CHAR_setWorkInt( toindex, CHAR_WORKTOXICATION, 0 );
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "TOXICATIONMSG", buf2, sizeof( buf2) ) != NULL) {
				CHAR_talkToCli( toindex, -1, buf1,  CHAR_COLORYELLOW);
			}
		}else	{
			CHAR_setWorkInt( toindex, CHAR_WORKTOXICATION, 0 );
			setToxication( fd, 0);
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "DETOXIFYMSG", buf2, sizeof( buf2) ) != NULL) {
				CHAR_talkToCli( toindex, -1, buf2,  CHAR_COLORYELLOW);
			}
		}
	}
#endif

	if(NPC_Util_GetStrFromStrWithDelim( buf1, "GetRandItem", buf2, sizeof( buf2) ) != NULL ){
		int j=1, rand_cnt=1;
		char buf3[256];
		while( getStringFromIndexWithDelim( buf2 , "," , j, buf3, sizeof( buf3)) != FALSE ){
			j++;
			rand_cnt++;
		}
		NPC_ActionTreasureRandItemGet( meindex, toindex, rand_cnt, buf2);
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	//andy_add 2003/07/17
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddPFSkillPoint", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionAddProfessionSkillPoint( toindex, buf2 ) == FALSE ){
			return FALSE;
		}
	}
	//andy_add 2003/07/17
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "CleanProfession", buf2, sizeof( buf2) ) != NULL) {
		if( NPC_ActionCleanProfession( toindex, buf2 ) == FALSE ){
			sprintf( buf2, "处理职业参数失败!!" );
			CHAR_talkToCli( toindex, -1, buf2,  CHAR_COLORYELLOW);
			return FALSE;
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "PROFESSION", buf2, sizeof( buf2) ) != NULL) {
		int p_class = atoi(buf2);
		// 判断职业
		if( CHAR_getInt( toindex, PROFESSION_CLASS ) != PROFESSION_CLASS_NONE && (p_class != 0) ){
			sprintf( buf2, "你已经有职业了!!" );
			CHAR_talkToCli( toindex, -1, buf2,  CHAR_COLORYELLOW);
			return FALSE;
		}

		if( p_class > PROFESSION_CLASS_NONE && p_class < PROFESSION_CLASS_NUM ) //	1:战士 2:巫师 3:猎人
		{
			int i, value = 0;
			int skill_id = -1;

			// 还原点数	
			if( PROFESSION_RESTORE_POINT( toindex ) != 1 )	return FALSE;

			// 设定职业
			CHAR_setInt( toindex, PROFESSION_CLASS, p_class );
#ifdef _CHANNEL_MODIFY
			if(CHAR_getInt(toindex,PROFESSION_CLASS) > 0){
				int i,pclass = CHAR_getInt(toindex,PROFESSION_CLASS) - 1;
				for(i=0;i<getFdnum();i++){
					if(*(piOccChannelMember + (pclass * getFdnum()) + i) == -1){
						*(piOccChannelMember + (pclass * getFdnum()) + i) = toindex;
						break;
					}
				}
			}
#endif			

			// 设定职业等级 1
			CHAR_setInt( toindex, PROFESSION_LEVEL, 1 );	
		
			switch( p_class ){
				case PROFESSION_CLASS_FIGHTER:	value = 0;	skill_id = 37; break;	// 勇士
				case PROFESSION_CLASS_WIZARD:	value = 10;	skill_id = 1;  break;	// 巫师
				case PROFESSION_CLASS_HUNTER:	value = 25;	skill_id = 47; break;	// 猎人
				default: break;
			}
/*
			// 给第一个职业技能
			if( skill_id != -1 ){
				if( PROFESSION_SKILL_ADDSK( toindex, skill_id, 10 ) == -1 ) return FALSE;
			}
*/
			// 给1点技能点数
			CHAR_setInt(toindex, PROFESSION_SKILL_POINT, 1 );
			CHAR_send_P_StatusString(  toindex, CHAR_P_STRING_DUELPOINT );


			// 增加抗性
			for( i=0; i<3; i++ )
				CHAR_setInt( toindex, PROFESSION_FIRE_R+i, CHAR_getInt( toindex, PROFESSION_FIRE_R+i) + value );

			// 巫师增加MP上限
			if( p_class == PROFESSION_CLASS_WIZARD ){
				CHAR_setInt( toindex , CHAR_MAXMP , 150 );
			}

			// 欢迎讯息
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "OK_MSG", buf2, sizeof( buf2) ) == NULL){
				sprintf( buf2, "欢迎加入" );
				CHAR_talkToCli( toindex, -1, buf2,  CHAR_COLORYELLOW);
			}

			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		}
#if 1 // Robin add 消除职业
		else if( p_class == PROFESSION_CLASS_NONE)
		{
			int i;

			print(" ====清除职业==== ");
			
			// 还原点数	
			//if( PROFESSION_RESTORE_POINT( toindex ) != 1 )	return FALSE;

#ifdef _CHANNEL_MODIFY
			if(CHAR_getInt(toindex,PROFESSION_CLASS) > 0){
				int i,pclass = CHAR_getInt(toindex,PROFESSION_CLASS) - 1;
				for(i=0;i<getFdnum();i++){
					if(*(piOccChannelMember + (pclass * getFdnum()) + i) == toindex){
						*(piOccChannelMember + (pclass * getFdnum()) + i) = -1;
						break;
					}
				}
			}
#endif
			// 设定职业归零
			CHAR_setInt( toindex, PROFESSION_CLASS, p_class );
			
			// 设定职业等级 0
			CHAR_setInt( toindex, PROFESSION_LEVEL, 0 );

			// 技能点数归零
			CHAR_setInt(toindex, PROFESSION_SKILL_POINT, 0 );

			// 还原抗性
			for( i=0; i<3; i++ )
				CHAR_setInt( toindex, PROFESSION_FIRE_R+i, /*CHAR_getInt( toindex, PROFESSION_FIRE_R+i) + value*/0 );

			// 还原巫师MP上限
			CHAR_setInt( toindex , CHAR_MAXMP , 100 );

			// 删除所有技能
			CHAR_CHAT_DEBUG_delsk( toindex, "all" );

			// ?? Andy add
			CHAR_setInt( toindex , ATTACHPILE, 0);

			// 清除任务旗标
			NPC_NowEndEventSetFlgCls( toindex, 145);
			NPC_NowEndEventSetFlgCls( toindex, 146);
			NPC_NowEndEventSetFlgCls( toindex, 147);

			CHAR_sendStatusString( toindex , "S");
			
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
		}
#endif
		else
		{
			return FALSE;
		}
	}
#endif
#ifdef _TREASURE_BOX
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "TREASURE_EVENT", buf2, sizeof( buf2) ) != NULL) {
		if( strstr( buf2, "OPEN") != NULL){
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "TREASURE_TIME", buf2, sizeof( buf2) ) == NULL)
				return FALSE;
			//NPC_TIME_EVENTMODE = CHAR_NPCWORKINT6,// 1
			//NPC_TIME_EVENTTIME = CHAR_NPCWORKINT7,
			CHAR_setWorkInt( meindex, CHAR_NPCWORKINT6, NPC_EVENTMODE_OPEN );
			CHAR_setWorkInt( meindex, CHAR_NPCWORKINT7, NowTime.tv_sec+atoi( buf2) );
			//变图
			//NPC_TIME_EVENONBBI = CHAR_NPCWORKINT8,
			CHAR_setInt( meindex, CHAR_BASEBASEIMAGENUMBER,
				CHAR_getWorkInt( meindex, CHAR_NPCWORKINT8));

			CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER,
				CHAR_getWorkInt( meindex, CHAR_NPCWORKINT8));
			CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );
		}
	}
#endif
#ifdef  _NPC_ADDLEVELUP
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "SetLevel", buf2, sizeof( buf2) ) != NULL) {
		NPC_ActionLevelAndTransUp( meindex, toindex, atoi(buf2), 0, 0, -1);
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddExps", buf2, sizeof( buf2) ) != NULL) {
		NPC_ActionLevelAndTransUp( meindex, toindex, 0, 0, atoi(buf2), -1);
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "AddSkillPoint", buf2, sizeof( buf2) ) != NULL) {
		NPC_ActionLevelAndTransUp( meindex, toindex, 0, atoi(buf2), 0, -1);
	}
	if( NPC_Util_GetStrFromStrWithDelim( buf1, "SetRideType", buf2, sizeof( buf2) ) != NULL) {
		NPC_ActionLevelAndTransUp( meindex, toindex, 0, 0, 0, atoi(buf2));
	}
#endif

#ifdef _NPC_ADDWARPMAN1
/*    if( NPC_Util_GetStrFromStrWithDelim( buf1, "PartyCount", buf2, sizeof( buf2) ) != NULL) {
	    //取得组队人数
		int partys=0;
	    int leaderindex=-1;
        int flg=0; //0:等於 1:大於等於 2:小於等於 3:大於 4:小於 5:不等於
	    char Head[32],buf3[32];
		leaderindex = toindex;
	    if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
		    partys = 0;
		}else if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ){
		    leaderindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1);
		
	        if( CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ){
		        int i;
		        for( i=0; i<5; i++)	{
			        int pindex = CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYINDEX1+i);
			        if( !CHAR_CHECKINDEX( pindex) )
				        partys++;
				}
			}
		}
	   
		memset( Head, 0, sizeof( Head));
		memset( buf3, 0, sizeof( buf3));
	    if( strstr( buf2, "=" ) != NULL ){
		    strcpy( Head, "=");
		    flg = 0;
		    if( strstr( buf2, ">") != NULL ){
			    strcat( Head, ">");
			    flg = 1;
			}else if( strstr( buf2, "<") != NULL ){
			    strcat( Head, "<");
			    flg = 2;
			}else if( strstr( buf2, "!") != NULL )	{
			    strcat( Head, "!");
			    flg = 5;
			}
		}else if(strstr( buf2, "<") != NULL){
		    strcpy( Head, "<");
		    flg = 4;
		}else if(strstr( buf2, ">") != NULL){
		    strcpy( Head, ">");
		    flg = 3;
		}
		Head[ strlen( Head)+1] = 0;
		if( NPC_Util_GetStrFromStrWithDelim( buf2, Head, buf3, sizeof( buf3) ) != NULL) {
		    if( flg == 0 )
                if( atoi(buf3) != partys ) return FALSE;  
            else if( flg == 1 )
                if( atoi(buf3) < partys ) return FALSE;
		    else if( flg == 2 )
			    if( atoi(buf3) > partys ) return FALSE;
            else if( flg == 3 )
			    if( atoi(buf3) <= partys ) return FALSE;
		    else if( flg == 4 )
			    if( atoi(buf3) >= partys ) return FALSE;
		    else if( flg == 5 )
			    if( atoi(buf3) == partys ) return FALSE;
		}
	}*/
#endif

	ActionNpc_WarpPoint( meindex, toindex, buf1);
	return TRUE;
}
BOOL Action_PartyCheck( int meindex, int talker)
{
	if(CHAR_getWorkInt(talker,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		return FALSE;
	}
	return TRUE;
}

BOOL NPC_ActionAddGold( int talker, int Golds)
{
	CHAR_AddGold( talker, Golds);
	return TRUE;
}

BOOL NPC_ActionDelGold( int talker,char *buf)
{
	int Golds=0;
	int PGold=0;
	if( buf == NULL ) return FALSE;
	Golds = atoi( buf);
	PGold = CHAR_getInt( talker, CHAR_GOLD);
	if( PGold < Golds )	{
		CHAR_talkToCli( talker, -1, "你身上的钱不够喔！", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		char buf1[256];
		sprintf( buf1, "交出%d石币", Golds);
		CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	}
	CHAR_setInt( talker, CHAR_GOLD, PGold-Golds);
	CHAR_send_P_StatusString( talker , CHAR_P_STRING_GOLD);
	return TRUE;
}

BOOL NPC_ActionAddFame( int talker,char *buf)
{
	int Fames=0;
	int MFame=0;
	if( buf == NULL ) return FALSE;
	Fames = atoi( buf);
	MFame = CHAR_getInt( talker, CHAR_FAME)+Fames*100;
	if( MFame > 1000000 )	{
		CHAR_talkToCli( talker, -1, "你的声望过高喔！", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		char buf1[256];
		sprintf( buf1, "获得%d声望", Fames);
		CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	}
	CHAR_setInt( talker, CHAR_FAME, MFame);
	return TRUE;
}

BOOL NPC_ActionDelFame( int talker,char *buf)
{
	int Fames=0;
	int MFame=0;
	if( buf == NULL ) return FALSE;
	Fames = atoi( buf);
	MFame = CHAR_getInt( talker, CHAR_FAME)/100;
	if( MFame < Fames )	{
		CHAR_talkToCli( talker, -1, "你的声望不够喔！", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		char buf1[256];
		sprintf( buf1, "付出%d声望", Fames);
		CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	}
	CHAR_setInt( talker, CHAR_FAME, CHAR_getInt( talker, CHAR_FAME)-Fames*100);
	return TRUE;
}
BOOL NPC_ActionAddVipPoint( int talker,char *buf)
{
	int Vippoints=0;
	int MVippoint=0;
	if( buf == NULL ) return FALSE;
	Vippoints = atoi( buf);
	MVippoint = sasql_vippoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	char buf1[256];
	sprintf( buf1, "获得%d会员点", Vippoints);
	CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	sasql_vippoint( CHAR_getChar(talker,CHAR_CDKEY), Vippoints,1);
	return TRUE;
}
BOOL NPC_ActionDelVipPoint( int talker,char *buf)
{
	int Vippoints=0;
	int MVippoint=0;
	if( buf == NULL ) return FALSE;
	Vippoints = atoi( buf);
	MVippoint = sasql_vippoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if(MVippoint<Vippoints){
		CHAR_talkToCli( talker, -1, "您的会员点不足！", CHAR_COLORRED);
		return FALSE;
	}
	char buf1[256];
	sprintf( buf1, "扣除%d会员点", Vippoints);
	CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	sasql_vippoint( CHAR_getChar(talker,CHAR_CDKEY), -Vippoints,1);
	return TRUE;
}
BOOL NPC_ActionAddPoint( int talker,char *buf)
{
	int Points=0;
	int MPoint=0;
	if( buf == NULL ) return FALSE;
	Points = atoi( buf);
	MPoint = sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	char buf1[256];
	sprintf( buf1, "获得%d积分", Points);
	CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), Points,1);
	return TRUE;
}
BOOL NPC_ActionDelPoint( int talker,char *buf)
{
	int Points=0;
	int MPoint=0;
	if( buf == NULL ) return FALSE;
	Points = atoi( buf);
	MPoint = sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if(MPoint<Points){
		CHAR_talkToCli( talker, -1, "您的积分不足！", CHAR_COLORRED);
		return FALSE;
	}
	char buf1[256];
	sprintf( buf1, "扣除%d积分", Points);
	CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), -Points,1);
	return TRUE;
}
#ifdef	_VIGOR_SYS
BOOL NPC_ActionAddVigor( int talker,char *buf)
{
	int Vigors=0;
	int MVigor=0;
	if( buf == NULL ) return FALSE;
	Vigors = atoi( buf);
	MVigor = CHAR_getInt( talker, CHAR_VIGOR);
	char buf1[256];
	sprintf( buf1, "获得%d活力", Vigors);
	CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	CHAR_setInt( talker, CHAR_VIGOR, MVigor+Vigors);
	return TRUE;
}
BOOL NPC_ActionDelVigor( int talker,char *buf)
{
	int Vigors=0;
	int MVigor=0;
	if( buf == NULL ) return FALSE;
	Vigors = atoi( buf);
	MVigor = CHAR_getInt( talker, CHAR_VIGOR);
	if(MVigor<Vigors){
		CHAR_talkToCli( talker, -1, "您的活力不足！", CHAR_COLORRED);
		return FALSE;
	}
	char buf1[256];
	sprintf( buf1, "扣除%d活力", Vigors);
	CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	CHAR_setInt( talker, CHAR_VIGOR, MVigor-Vigors);
	return TRUE;
}
#endif
BOOL NPC_ActionUpItem( int talker,char *buf)
{
	int additemid=0,randnum=100,erritemid=-1,myrand;
	char upitembuf[256];
	if(getStringFromIndexWithDelim( buf,",", 1, upitembuf, sizeof( upitembuf)) != FALSE){
		additemid=atoi(upitembuf);
	}
	if(getStringFromIndexWithDelim( buf,",", 2, upitembuf, sizeof( upitembuf)) != FALSE){
		randnum=atoi(upitembuf);
	}
	if(getStringFromIndexWithDelim( buf,",", 3, upitembuf, sizeof( upitembuf)) != FALSE){
		erritemid=atoi(upitembuf);
	}
	myrand=RAND(1,100);
	if(myrand<=randnum){
		sprintf(upitembuf,"%d",additemid);
		NPC_ActionAddItem(talker,upitembuf);
		sprintf(upitembuf,"恭喜您，合成成功，获得道具【%s】",ITEM_getNameFromNumber( additemid));
		CHAR_talkToCli( talker, -1, upitembuf, CHAR_COLORYELLOW);
		return TRUE;
	}else{
		if(erritemid==-1){
			CHAR_talkToCli( talker, -1, "很遗憾，合成失败！", CHAR_COLORYELLOW);
			return FALSE;
		}else{
			sprintf(upitembuf,"%d",erritemid);
			NPC_ActionAddItem(talker,upitembuf);
			sprintf(upitembuf,"很遗憾，合成失败，您的物品降级成为【%s】",ITEM_getNameFromNumber( erritemid));
			CHAR_talkToCli( talker, -1, upitembuf, CHAR_COLORYELLOW);
			return FALSE;
		}
	}
	return TRUE;
}
#ifdef _MOMENTUM_NPC
BOOL NPC_ActionMomentum( int talker,char *buf)
{
	int Momentums=0;
	int PMomentum=0;
	if( buf == NULL ) return FALSE;
	Momentums = atoi( buf)*100;
	PMomentum = CHAR_getInt( talker, CHAR_MOMENTUM);
	if( PMomentum < Momentums )	{
		CHAR_talkToCli( talker, -1, "你的个人气势值不够喔！", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		char buf1[256];
		sprintf( buf1, "扣除个人气势值%d", Momentums/100);
		CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	}
	CHAR_setInt( talker, CHAR_MOMENTUM, PMomentum-Momentums);
	CHAR_send_P_StatusString( talker , CHAR_P_STRING_GOLD);
	return TRUE;
}
#endif
#ifdef _DP_NPC
BOOL NPC_ActionDP( int talker,char *buf)
{
	int DPs=0;
	int PDP=0;
	if( buf == NULL ) return FALSE;
	DPs = atoi( buf);
	PDP = CHAR_getInt( talker, CHAR_DUELPOINT);
	if( PDP < DPs )	{
		CHAR_talkToCli( talker, -1, "你的DP不够哦！", CHAR_COLORYELLOW);
		return FALSE;
	}else	{
		char buf1[256];
		sprintf( buf1, "扣除DP%d", DPs);
		CHAR_talkToCli( talker, -1, buf1, CHAR_COLORYELLOW);
	}
	CHAR_setInt( talker, CHAR_DUELPOINT, PDP-DPs);
	CHAR_send_P_StatusString( talker ,CHAR_P_STRING_DUELPOINT );
	return TRUE;
}
#endif
// WON ADD
BOOL NPC_ActionAddGold2( int talker,char *buf)
{
	int Golds=0;
	int PGold=0;
	int MaxGold = 0;
	int total_gold = 0;
	if( buf == NULL ) return FALSE;
	Golds = atoi( buf);
	PGold = CHAR_getInt( talker, CHAR_GOLD);
	MaxGold = CHAR_getMaxHaveGold( talker );

	total_gold = PGold + Golds;
	if( total_gold > MaxGold ) total_gold = MaxGold;
	
	CHAR_setInt( talker, CHAR_GOLD, total_gold );
	CHAR_send_P_StatusString( talker , CHAR_P_STRING_GOLD);
	return TRUE;
}

BOOL NPC_ActionDelPet( int talker, char *buf)
{
	int petsel;	 
	int petindex=0, k=1;
	int fd = getfdFromCharaIndex( talker );
	char msgbuf[64], szPet[128],buff2[256],buff3[256];
	char buf2[256],buff1[256];
	int defpet;
	int petno,kNum,find=0,petLv=0,flg=-1;

	while( getStringFromIndexWithDelim(buf , "," , k, buff1, sizeof( buff1)) !=FALSE )	{ 
		k++;
    	if( strstr( buff1,"-") != NULL )	{
		getStringFromIndexWithDelim(buff1,"-",2,buff3,sizeof(buff3));
    		getStringFromIndexWithDelim(buff1,"-",1,buf2,sizeof(buf2));		

			if( strstr( buf2,"!") != NULL )	{
				flg = 0;
				getStringFromIndexWithDelim(buf2,"!=",2,buff2,sizeof(buff2));
			}else if( strstr( buf,"<") != NULL )	{
				flg = 1;
				getStringFromIndexWithDelim(buf2,"<",2,buff2,sizeof(buff2));
			}else if( strstr( buf,">") != NULL )	{
				flg = 2;
				getStringFromIndexWithDelim(buf2,">",2,buff2,sizeof(buff2));
			}else if( strstr( buf,"=") != NULL)	{
				flg = 3;
				getStringFromIndexWithDelim(buf2,"=",2,buff2,sizeof(buff2));
			}else
				return FALSE;
			petLv = atoi( buff2);
    	}
		continue;

		if(strstr(buff3,"*")!=NULL){									
			getStringFromIndexWithDelim(buff3,"*",1,buf2,sizeof(buf2));
			petno = atoi(buf2);
			getStringFromIndexWithDelim(buff3,"*",2,buf2,sizeof(buf2));
			kNum = atoi(buf2);
		}else	{
			petno = atoi( buff3);
			kNum = 30;
		}
		//andy_log
		print("petno:%d kNum:%d \n ", petno, kNum);

		find =0;

		for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++)	{
			petindex = CHAR_getCharPet( talker, petsel);
			if( !CHAR_CHECKINDEX(petindex))	continue;
			if(CHAR_getInt( petindex, CHAR_PETID) !=  petno )continue;

	  		switch( flg)	{
	  		case 0:
	  			if( CHAR_getInt( petindex, CHAR_LV )  != petLv )	{
	  			}else	{
	  				continue;
	  			}
	  		break;
	  		case 1:
				if( CHAR_getInt( petindex, CHAR_LV )  <  petLv )	{
				}else	{
					continue;
				}
			break;
	  		case 2:
				if( CHAR_getInt( petindex, CHAR_LV )  > petLv )		{
				}else	{
					continue;
				}
	  		break;
	  		case 3:
				if( CHAR_getInt( petindex, CHAR_LV ) != petLv )continue;
				break;
	  		}
	  		
 			petindex = CHAR_getCharPet( talker, petsel);
			if( !CHAR_CHECKINDEX(petindex) )
				continue;
			if( CHAR_getWorkInt( CONNECT_getCharaindex( fd), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE )
				continue;
			defpet = CHAR_getInt( talker, CHAR_DEFAULTPET);
			
			if(defpet == petsel)	{
				CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
				lssproto_KS_send( fd, -1, TRUE);
			}

			snprintf( msgbuf,sizeof( msgbuf), "交出%s。", CHAR_getChar( petindex, CHAR_NAME));
			CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
				LogPet(			
					CHAR_getChar( talker, CHAR_NAME ),
					CHAR_getChar( talker, CHAR_CDKEY ),
					CHAR_getChar( petindex, CHAR_NAME),
					CHAR_getInt( petindex, CHAR_LV),
					"TenseiDel",
					CHAR_getInt( talker,CHAR_FLOOR),
					CHAR_getInt( talker,CHAR_X ),
					CHAR_getInt( talker,CHAR_Y ),
					CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
				);
			CHAR_setCharPet( talker, petsel, -1);
			CHAR_endCharOneArray( petindex );
			snprintf( szPet, sizeof( szPet ), "K%d", petsel);
			CHAR_sendStatusString( talker, szPet );

			find++;
			if( find >= kNum )
				break;	//条件数目皆删除完
		}
	}

    return TRUE;
}

#ifdef _NPC_NewDelPet
BOOL NPC_ActionNewDelPet( int toindex, char *msg)
{
   		int i,petindex=0,petlev,petnum,petcount,mypetlevel,petid;
		char buf[32],buf2[32];
		int flg=0,count=0;
		
		//比较方式
		if ( strstr( msg, "=" ) )
			flg = 0;
		else if( strstr( msg, "<" ) )
			flg = 1;
		else
			flg = 2;

		//等级
		if ( getStringFromIndexWithDelim( msg, "-", 1, buf, sizeof( buf )) ){
			char cmp[12];
			if( flg == 0 )
				strcpy(cmp,"=");
			else if( flg == 1 )
				strcpy(cmp,"<");
			else 
				strcpy(cmp,">");
			if ( getStringFromIndexWithDelim( buf, cmp, 2, buf2, sizeof( buf2 )) ){
			    petlev = atoi(buf2);
			}
			else
			    return FALSE;
		}
		else
            return FALSE;

		//宠物编号
        if ( getStringFromIndexWithDelim( msg, "-", 2, buf, sizeof( buf )) ){
			if ( getStringFromIndexWithDelim( buf, "*", 1, buf2, sizeof( buf2 )) ){
                petnum = atoi( buf2 );
			}
			else
				return FALSE;
		}
		else
			return FALSE;

		//数量
        if ( getStringFromIndexWithDelim( msg, "-", 2, buf, sizeof( buf )) ){
			if ( getStringFromIndexWithDelim( buf, "*", 2, buf2, sizeof( buf2 )) ){
			    petcount = atoi( buf2 );
			}
			else
				return FALSE;
		}
		else
			return FALSE;

	    for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
			BOOL cmp=FALSE;
			petindex = CHAR_getCharPet( toindex, i);
			petid = CHAR_getInt( petindex, CHAR_PETID);
			mypetlevel = CHAR_getInt( petindex, CHAR_LV);
        		
			if( flg == 0 )
				if( mypetlevel == petlev ) cmp = TRUE;
			if( flg == 1 )
				if( mypetlevel < petlev ) cmp = TRUE;
			if( flg == 2 )
				if( mypetlevel > petlev ) cmp = TRUE;

			if( petnum == petid && cmp ){//删除宠物
	            char szPet[128];
	            int defpet;
	            char msgbuf[64];
	            int fd = getfdFromCharaIndex( toindex );

                if( !CHAR_CHECKINDEX(petindex) ) return FALSE;

	            if( CHAR_getInt( toindex, CHAR_RIDEPET) == i ) {

		//CHAR_talkToCli( talker, -1, "骑乘中的宠物无法交出！", CHAR_COLORYELLOW );
    	//return	FALSE;

		            CHAR_setInt( toindex, CHAR_RIDEPET, -1);
		            CHAR_send_P_StatusString( toindex, CHAR_P_STRING_RIDEPET );
		            CHAR_complianceParameter( toindex );
		            CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
				}

	            //公及平乓仿互爵    卅日｝矢永玄毛丹日内蔽  
	            if( CHAR_getWorkInt( CONNECT_getCharaindex( fd ), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) 
					return FALSE;
	            //及矢永玄互田玄伙井升丹井及民尼永弁
	            defpet = CHAR_getInt( toindex, CHAR_DEFAULTPET);
	            if(defpet == i){
		            CHAR_setInt( toindex, CHAR_DEFAULTPET, -1);
		            lssproto_KS_send( fd, -1, TRUE);
				}

	            snprintf( msgbuf,sizeof( msgbuf), "交出%s。",
						  CHAR_getChar( petindex, CHAR_NAME));
	            CHAR_talkToCli( toindex, -1, msgbuf,  CHAR_COLORWHITE);

	            // 矢永玄毛澎  仄凶夫弘
	            LogPet( CHAR_getChar( toindex, CHAR_NAME ), // 平乓仿   
		                CHAR_getChar( toindex, CHAR_CDKEY ),
		                CHAR_getChar( petindex, CHAR_NAME),
		                CHAR_getInt( petindex, CHAR_LV),
		                "EvnetDell(任务删除)",
		                CHAR_getInt( toindex,CHAR_FLOOR),
		                CHAR_getInt( toindex,CHAR_X ),
		                CHAR_getInt( toindex,CHAR_Y ),
		                CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	            );


	            //   切  井日壅允 
	            CHAR_setCharPet( toindex, i, -1);

	            CHAR_endCharOneArray( petindex );

	            snprintf( szPet, sizeof( szPet ), "K%d", i);
	            // 蕙仄中矢永玄旦  □正旦霜曰勾仃月
	            CHAR_sendStatusString( toindex, szPet );

				++count;
                if( count >= petcount )
				    break;
			}
		}

    return TRUE;
}
#endif

BOOL NPC_ActionAddPet( int talker, char *buf)
{
	int	ret;
	char msgbuf[64], buff3[64];
	int	enemynum;
	int	i,j,k=1;
	int petindex, petindex2, petid;

  k=1;
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
  	k++;	
	//检查宠物栏是否有空位
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE ){
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}
#ifdef _PET_MM
	if(strstr(buff3,"MM1") || strstr(buff3,"MM3"))
		petid = 1479;
	else if(strstr(buff3,"MM2") || strstr(buff3,"MM4"))
		petid = 2547;
	else
		petid = atoi( buff3);
#else
	petid = atoi( buff3);
#endif
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID ) == petid )
		//if( ENEMY_getInt( i, ENEMY_TEMPNO ) == petid )
			break;
	}
	
	if( i == enemynum )
		continue;
		
	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		i = 0;	

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
#ifdef _PET_MM	
	if(strstr(buff3,"MM1") || strstr(buff3,"MM2")){
		int LevelUpPoint;
		LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
		CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		}else if(strstr(buff3,"MM3") || strstr(buff3,"MM4")){
			while(CHAR_getInt( ret, CHAR_LV)<79){	//升级
				int LevelUpPoint;
				LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
				CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
				CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			}
		}
#endif
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		continue;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}

	LogPet(
		CHAR_getChar( talker, CHAR_NAME ),
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex2, CHAR_NAME),
		CHAR_getInt( petindex2, CHAR_LV),
		"TenseiGet",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex2, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
  }
	return TRUE;
}

BOOL NPC_ActionTiaoZhanMsg( int toindex, char *msg)
{
	int k;
	int playernum = CHAR_getPlayerMaxNum();
	char tzmsg[256];
	sprintf(tzmsg,"「挑战先锋」：恭喜玩家「%s」打败了%s",CHAR_getChar(toindex,CHAR_NAME),msg);
	for(k=0;k<playernum;k++){
		CHAR_talkToCli( k, -1, tzmsg, CHAR_COLORRED);
	}

    return TRUE;
}

#ifdef _ITEM_PILENUMS
BOOL NPC_DelItem( int toindex, int ti, int num)
{
	int itemindex, pilenum;

	itemindex = CHAR_getItemIndex( toindex , ti);
	if( !ITEM_CHECKINDEX(itemindex) ) return FALSE;
	pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
	if( pilenum < num ) return FALSE;

	pilenum = pilenum - num;
	if( pilenum <= 0 ){
	{
		LogItem(
			CHAR_getChar( toindex, CHAR_NAME ),
			CHAR_getChar( toindex, CHAR_CDKEY ),
#ifdef _add_item_log_name
			itemindex,
#else
	   		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"Sell(卖道具)",
			CHAR_getInt( toindex,CHAR_FLOOR),
			CHAR_getInt( toindex,CHAR_X ),
			CHAR_getInt( toindex,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)

		);
	}
		CHAR_setItemIndex( toindex, ti, -1);
		ITEM_endExistItemsOne( itemindex);
	}else{
		ITEM_setInt( itemindex, ITEM_USEPILENUMS, pilenum);
	}
	CHAR_sendItemDataOne( toindex, ti);
	return TRUE;
}


void NPC_ActionDoPileClearItem( int toindex, int itemID)
{
	int i, itemindex;
	char token[256];

	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
		itemindex = CHAR_getItemIndex( toindex , i );
		if( !ITEM_CHECKINDEX(itemindex) ) continue;
		if( ITEM_getInt(itemindex, ITEM_ID) != itemID ) continue;
		sprintf( token, "交出道具%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);

						LogItem(
							CHAR_getChar( toindex, CHAR_NAME ),
							CHAR_getChar( toindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),
#endif
							"任务收取",
							CHAR_getInt( toindex,CHAR_FLOOR),
							CHAR_getInt( toindex,CHAR_X ),
							CHAR_getInt( toindex,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)

						);
			CHAR_setItemIndex( toindex, i, -1);
			ITEM_endExistItemsOne( itemindex);
			CHAR_sendItemDataOne( toindex, i);
	}
}

BOOL NPC_ActionDoPileDelItem( int toindex, int itemID, int num)
{
	int i, itemindex, finditem=0, pilenum=0, onenum=0;
	char token[256];

	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE && (finditem<num || num==-1); i++ ){
		itemindex = CHAR_getItemIndex( toindex , i );
		onenum = 0;
		if( !ITEM_CHECKINDEX(itemindex) ) continue;
		if( ITEM_getInt(itemindex, ITEM_ID) != itemID ) continue;
		pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
		if( pilenum >= (num-finditem) ){
			pilenum -= (num-finditem);
			onenum += (num-finditem);
		}else {
			onenum = pilenum;
			pilenum=0;
		}
		finditem += onenum;
		
		ITEM_setInt( itemindex, ITEM_USEPILENUMS, pilenum);
		sprintf( token, "交出%d个%s", onenum, ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
		if( pilenum <= 0 ){
						LogItem(
							CHAR_getChar( toindex, CHAR_NAME ),
							CHAR_getChar( toindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),
#endif
							"任务收取",
							CHAR_getInt( toindex,CHAR_FLOOR),
							CHAR_getInt( toindex,CHAR_X ),
							CHAR_getInt( toindex,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)

						);


			CHAR_setItemIndex( toindex, i, -1);
			ITEM_endExistItemsOne( itemindex);
		}
		CHAR_sendItemDataOne( toindex, i);
	}

	if( finditem != num && num != -1 ) return FALSE;
	return TRUE;
}

BOOL NPC_ActionDelItem( int talker, char *buf)
{
	char buff3[128], buf2[32];
	int k=1, itemid, neednum;
	if( !CHAR_CHECKINDEX( talker) ) return FALSE;
	while( getStringFromIndexWithDelim(buf , "," , k++, buff3, sizeof(buff3)) !=FALSE )	{
		neednum = 1;
		if( getStringFromIndexWithDelim( buff3, "*", 1, buf2, sizeof(buf2)) == FALSE ) continue;
		itemid = atoi( buf2);
		if( getStringFromIndexWithDelim( buff3, "*", 2, buf2, sizeof(buf2)) == TRUE ){
			neednum = atoi( buf2);
			if( NPC_ActionDoPileDelItem( talker, itemid, neednum) == FALSE ) return FALSE;
		}else{
			NPC_ActionDoPileClearItem( talker, itemid);
		}
	}
	return TRUE;
}
#else
BOOL NPC_ActionDelItem( int talker, char *buf)
{
	int i = 1, j = 1,k = 1;
	char buff3[128];
	char buf2[32];
	int itemindex;
	char token[256];
	
	if( !CHAR_CHECKINDEX( talker) ) return FALSE;
	while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE )	{
		k++;
		if(strstr(buff3,"*")!=NULL){
			int itemno, kosuu=0, id, cnt=0;
			struct _tagItemArray {
				int index;
				int itemindex;
			};
			struct _tagItemArray ItemArray[20];
			for( i=0; i<20 ; i++){
				ItemArray[i].itemindex = -1;
				ItemArray[i].index = -1;
			}
			//取得字串
			getStringFromIndexWithDelim(buff3,"*",1,buf2,sizeof(buf2));
			itemno = atoi(buf2);
			getStringFromIndexWithDelim(buff3,"*",2,buf2,sizeof(buf2));
			kosuu = atoi(buf2);
			//取得 item index
			for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex=CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX(itemindex) ){
					id=ITEM_getInt(itemindex ,ITEM_ID );
					if(itemno==id){
						ItemArray[cnt].itemindex = itemindex;
						ItemArray[cnt].index = i;
						cnt++;
					}
				}
				if(cnt == kosuu)
					break;
			}
			if(cnt != kosuu){
				CHAR_talkToCli( talker, -1,"条件不足", CHAR_COLORYELLOW);
				return FALSE;
			}
			//收取物品
			for( i=0; i<20; i++)	{
				itemindex = ItemArray[i].itemindex;
				if( !ITEM_CHECKINDEX(itemindex) ) break;
				sprintf(token,"交出%s",ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);
				CHAR_setItemIndex( talker, ItemArray[i].index ,-1);
				ITEM_endExistItemsOne( itemindex);
				CHAR_sendItemDataOne( talker, ItemArray[i].index);
				LogItem(
					CHAR_getChar( talker, CHAR_NAME ), CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
					itemindex,
#else
					ITEM_getInt( itemindex, ITEM_ID),
#endif
					"WarpManDelItem",
					CHAR_getInt( talker, CHAR_FLOOR), CHAR_getInt( talker, CHAR_X ),
					CHAR_getInt( talker, CHAR_Y ), ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
					);
			}
		}else{
			for( j = CHAR_STARTITEMARRAY ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);
				if( ITEM_CHECKINDEX(itemindex) ){
					if( atoi( buff3) == ITEM_getInt(itemindex,ITEM_ID)){
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ),
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),
#endif
							"WarpManDelItem",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)

						);
						sprintf( token, "交出%s。", 
										ITEM_getChar( itemindex, ITEM_NAME));
						CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);
						CHAR_DelItem( talker, j);
					}
				}
			}
		}
	}
	return TRUE;
}
#endif

#ifdef _VIP_SHOP
BOOL NPC_ActionVipItem( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
  int itemID,itemindex=-1;
  int pVipPoint, VipPoints;
  char buff[256], token[256];
  int ret;

	if(getStringFromIndexWithDelim(buf , "," , 1, buff, sizeof(buff)) !=FALSE)
		itemID = atoi(buf);

  itemindex = CHAR_findEmptyItemBox( talker );
	if( itemindex < 0 )	{
		CHAR_talkToCli( talker, -1, "物品栏空间不足！！",  CHAR_COLORYELLOW);
		return FALSE;
	}
	
	if(getStringFromIndexWithDelim(buf , "," , 2, buff, sizeof(buff)) !=FALSE)
		VipPoints=atoi(buff);
	if(VipPoints<0)VipPoints=0;

  pVipPoint = sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if( pVipPoint < VipPoints )	{
		CHAR_talkToCli( talker, -1, "你的积分不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}
	
	itemindex = ITEM_makeItemAndRegist( itemID );
  if( itemindex != -1 ){
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}

		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		
		sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), -VipPoints,1);
		sprintf( token, "扣除积分%d", VipPoints);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);

		CHAR_sendItemDataOne( talker, ret);
  }

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionVipPet( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
	int	ret;
	char msgbuf[64], buf1[8], buf2[10];
	int	enemynum;
	int	i,j;
	int petindex, petindex2, petid;
  int pVipPoint, VipPoints;
		
	if(getStringFromIndexWithDelim(buf , "," , 1, buf1, sizeof(buf1)) !=FALSE)
#ifdef _PET_MM
	if(strstr(buf1,"MM1") || strstr(buf1,"MM3"))
		petid = 1479;
	else if(strstr(buf1,"MM2") || strstr(buf1,"MM4"))
		petid = 2547;
	else
		petid = atoi( buf1 );
#else
	petid = atoi( buf1 );
#endif
		
	if(getStringFromIndexWithDelim(buf , "," , 2, buf2, sizeof(buf2)) !=FALSE)
		VipPoints=atoi(buf2);
	if(VipPoints<0)VipPoints=0;

  pVipPoint = sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if( pVipPoint < VipPoints )	{
		CHAR_talkToCli( talker, -1, "你的积分不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
				break;
			}
	}

	if( i == enemynum )
		return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		CHAR_talkToCli( talker, -1, "领取宠物失败！", CHAR_COLORYELLOW);

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
#ifdef _PET_MM
		if(strstr(buf1,"MM1") || strstr(buf1,"MM2")){
			int LevelUpPoint;
			LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		}else if(strstr(buf1,"MM3") || strstr(buf1,"MM4")){
			while(CHAR_getInt( ret, CHAR_LV)<79){	//升级
				int LevelUpPoint;
				LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
				CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
				CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			}
		}
#endif
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), -VipPoints,1);
	snprintf( msgbuf,sizeof( msgbuf), "扣除积分%d", VipPoints);
	CHAR_talkToCli( talker, -1, msgbuf, CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}
	return TRUE;
}
#endif

#ifdef _VIP_SHOP
BOOL NPC_ActionNewVipItem( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
  int itemID,itemindex=-1;
  int pVipPoint, VipPoints, zhe;
  char buff[256], token[256];
  int ret;
  int fd = getfdFromCharaIndex( talker);
  char* id = CHAR_getChar(talker, CHAR_CDKEY);
	if(getStringFromIndexWithDelim(buf , "," , 1, buff, sizeof(buff)) !=FALSE)
		itemID = atoi(buf);

  itemindex = CHAR_findEmptyItemBox( talker );
	if( itemindex < 0 )	{
		CHAR_talkToCli( talker, -1, "物品栏空间不足！！",  CHAR_COLORYELLOW);
		return FALSE;
	}
	
	if(getStringFromIndexWithDelim(buf , "," , 2, buff, sizeof(buff)) !=FALSE)
		VipPoints=atoi(buff);
	if(VipPoints<0)VipPoints=0;

  pVipPoint = sasql_vippoint(id,0,0);
	if( pVipPoint < VipPoints )	{
		CHAR_talkToCli( talker, -1, "你身上的点数不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}
	
	itemindex = ITEM_makeItemAndRegist( itemID );
  if( itemindex != -1 ){
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}
		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		
		sasql_vippoint(id,-VipPoints,1);
		
		sprintf( token, "扣除会员点数%d", VipPoints);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);
		CHAR_sendItemDataOne( talker, ret);
		CHAR_charSaveFromConnect(talker, FALSE);
		CHAR_talkToCli(talker, -1, "系统为你自动存档！", CHAR_COLORRED);
  }

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionNewVipPet( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
	int	ret;
	char msgbuf[64], buf1[8], buf2[10];
	int	enemynum;
	int	i,j;
	int petindex, petindex2, petid;
  int pVipPoint, VipPoints, zhe;
  int fd = getfdFromCharaIndex( talker);
  char* id = CHAR_getChar(talker, CHAR_CDKEY);
	if(getStringFromIndexWithDelim(buf , "," , 1, buf1, sizeof(buf1)) !=FALSE)
#ifdef _PET_MM
	if(strstr(buf1,"MM1") || strstr(buf1,"MM3"))
		petid = 1479;
	else if(strstr(buf1,"MM2") || strstr(buf1,"MM4"))
		petid = 2547;
	else
		petid = atoi( buf1 );
#else
	petid = atoi( buf1 );
#endif
		
	if(getStringFromIndexWithDelim(buf , "," , 2, buf2, sizeof(buf2)) !=FALSE)
		VipPoints=atoi(buf2);
	if(VipPoints<0)VipPoints=0;
		
  pVipPoint = sasql_vippoint(id,0,0);
	if( pVipPoint < VipPoints )	{
		CHAR_talkToCli( talker, -1, "你身上的点数不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
				break;
			}
	}

	if( i == enemynum )
		return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		CHAR_talkToCli( talker, -1, "领取宠物失败！", CHAR_COLORYELLOW);

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
#ifdef _PET_MM
		if(strstr(buf1,"MM1") || strstr(buf1,"MM2")){
			int LevelUpPoint;
			LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		}else if(strstr(buf1,"MM3") || strstr(buf1,"MM4")){
			while(CHAR_getInt( ret, CHAR_LV)<79){	//升级
				int LevelUpPoint;
				LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
				CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
				CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			}
		}
#endif
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	sasql_vippoint(id,-VipPoints,1);
	snprintf( msgbuf,sizeof( msgbuf), "扣除会员点数%d", VipPoints);
	CHAR_talkToCli( talker, -1, msgbuf, CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}
	CHAR_charSaveFromConnect(talker, FALSE);
	CHAR_talkToCli(talker, -1, "系统为你自动存档！", CHAR_COLORRED);
	return TRUE;
}
#endif

BOOL NPC_ActionNewVipAbiPet( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
	int	ret;
	char msgbuf[64], buf1[10], buf2[10];
	int	enemynum;
	int vital,str,tough,dex,petlevel,pettrans;
	int	i,j;
	int petindex, petindex2, petid;
  int pVipPoint, VipPoints;
  int fd = getfdFromCharaIndex( talker);
  char* id = CHAR_getChar(talker, CHAR_CDKEY);
		
	easyGetTokenFromString( buf , 1 , buf1, sizeof(buf1));
	petid=atoi(buf1);
	easyGetTokenFromString( buf , 2 , buf1, sizeof(buf1));
	vital=atoi(buf1);
	easyGetTokenFromString( buf , 3 , buf1, sizeof(buf1));
	str=atoi(buf1);
	easyGetTokenFromString( buf , 4 , buf1, sizeof(buf1));
	tough=atoi(buf1);
	easyGetTokenFromString( buf , 5 , buf1, sizeof(buf1));
	dex=atoi(buf1);
	easyGetTokenFromString( buf , 6 , buf1, sizeof(buf1));
	petlevel=atoi(buf1);
	easyGetTokenFromString( buf , 7 , buf1, sizeof(buf1));
	pettrans=atoi(buf1);

	if( petlevel != -1 ){
#ifdef _TRANS_LEVEL_CF
			if( petlevel > getMaxLevel() ) petlevel = getMaxLevel();
#else
			if( petlevel > 140 ) level = 140;
#endif
	if( petlevel <= 0 ) petlevel = 1;
}
	else{
		petlevel = 1;
}
	if(pettrans>2){
		pettrans=2;
	}
	if(pettrans<0){
		pettrans=0;
	}
	
	if(getStringFromIndexWithDelim(buf , "," , 2, buf2, sizeof(buf2)) !=FALSE)
		VipPoints=atoi(buf2);
	if(VipPoints<0)VipPoints=0;

  pVipPoint = sasql_vippoint(id,0,0);
	if( pVipPoint < VipPoints )	{
		CHAR_talkToCli( talker, -1, "你身上的点数不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
				break;
			}
	}

	if( i == enemynum )
		return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		CHAR_talkToCli( talker, -1, "领取宠物失败！", CHAR_COLORYELLOW);

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setInt( ret, CHAR_VITAL, vital);
		CHAR_setInt( ret, CHAR_STR, str*80);
		CHAR_setInt( ret, CHAR_TOUGH, tough*80);
		CHAR_setInt( ret, CHAR_DEX, dex*100);
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
		CHAR_setInt( ret, CHAR_LV, petlevel);
		CHAR_setInt( ret, CHAR_TRANSMIGRATION, pettrans);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	sasql_vippoint(id,-VipPoints,1);
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	snprintf( msgbuf,sizeof( msgbuf), "扣除会员点数%d", VipPoints);
	CHAR_talkToCli( talker, -1, msgbuf, CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}
	CHAR_charSaveFromConnect(talker, FALSE);
	CHAR_talkToCli(talker, -1, "系统为你自动存档！", CHAR_COLORRED);
	return TRUE;
}

#ifdef _STU_SYS
BOOL NPC_ActionStuItem( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
  int itemID,itemindex=-1;
  int pStuPoint, StuPoints;
  char buff[256], token[256];
  int ret;

	if(getStringFromIndexWithDelim(buf , "," , 1, buff, sizeof(buff)) !=FALSE)
		itemID = atoi(buf);

  itemindex = CHAR_findEmptyItemBox( talker );
	if( itemindex < 0 )	{
		CHAR_talkToCli( talker, -1, "物品栏空间不足！！",  CHAR_COLORYELLOW);
		return FALSE;
	}
	
	if(getStringFromIndexWithDelim(buf , "," , 2, buff, sizeof(buff)) !=FALSE)
		StuPoints=atoi(buff);
	if(StuPoints<0)StuPoints=0;

  pStuPoint = CHAR_getInt( talker, CHAR_STUNUM);
	if( pStuPoint < StuPoints )	{
		CHAR_talkToCli( talker, -1, "你的徒弟数不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}
	
	itemindex = ITEM_makeItemAndRegist( itemID );
  if( itemindex != -1 ){
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}

		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		
		CHAR_setInt( talker, CHAR_STUNUM, pStuPoint-StuPoints);
		sprintf( token, "扣除徒弟数%d", StuPoints);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);

		CHAR_sendItemDataOne( talker, ret);
  }

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionStuPet( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
	int	ret;
	char msgbuf[64], buf1[8], buf2[10];
	int	enemynum;
	int	i,j;
	int petindex, petindex2, petid;
  int pStuPoint, StuPoints;
		
	if(getStringFromIndexWithDelim(buf , "," , 1, buf1, sizeof(buf1)) !=FALSE)
#ifdef _PET_MM
	if(strstr(buf1,"MM1") || strstr(buf1,"MM3"))
		petid = 1479;
	else if(strstr(buf1,"MM2") || strstr(buf1,"MM4"))
		petid = 2547;
	else
		petid = atoi( buf1 );
#else
	petid = atoi( buf1 );
#endif
		
	if(getStringFromIndexWithDelim(buf , "," , 2, buf2, sizeof(buf2)) !=FALSE)
		StuPoints=atoi(buf2);
	if(StuPoints<0)StuPoints=0;

  pStuPoint = CHAR_getInt( talker, CHAR_STUNUM);
	if( pStuPoint < StuPoints )	{
		CHAR_talkToCli( talker, -1, "你的徒弟数不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
				break;
			}
	}

	if( i == enemynum )
		return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		CHAR_talkToCli( talker, -1, "领取宠物失败！", CHAR_COLORYELLOW);

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
#ifdef _PET_MM
		if(strstr(buf1,"MM1") || strstr(buf1,"MM2")){
			int LevelUpPoint;
			LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		}else if(strstr(buf1,"MM3") || strstr(buf1,"MM4")){
			while(CHAR_getInt( ret, CHAR_LV)<79){	//升级
				int LevelUpPoint;
				LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
				CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
				CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			}
		}
#endif
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	CHAR_setInt( talker, CHAR_STUNUM, pStuPoint-StuPoints);
	snprintf( msgbuf,sizeof( msgbuf), "扣除徒弟数%d", StuPoints);
	CHAR_talkToCli( talker, -1, msgbuf, CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}
	return TRUE;
}
#endif

#ifdef _RMB_SYSTEM
BOOL NPC_ActionRmbPoint( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
  int pRmbPoint, RmbPoints;
  char buff[256], token[256];
  int point_num=0;

	if(getStringFromIndexWithDelim(buf , "," , 1, buff, sizeof(buff)) !=FALSE)
		point_num = atoi(buf);
	
	if(point_num<0) return FALSE; 
	
	if(getStringFromIndexWithDelim(buf , "," , 2, buff, sizeof(buff)) !=FALSE)
		RmbPoints=atoi(buff);
	if(RmbPoints<0)RmbPoints=0;

  pRmbPoint = sasql_rmbpoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if( pRmbPoint < RmbPoints )	{
		CHAR_talkToCli( talker, -1, "你的现金不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}
	
	sasql_vippoint( CHAR_getChar(talker,CHAR_CDKEY), point_num,1);

		sprintf( token,"获得%d会员点",point_num);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		
		sasql_rmbpoint( CHAR_getChar(talker,CHAR_CDKEY), -RmbPoints,1);
		sprintf( token, "扣除现金%d", RmbPoints);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionRmbItem( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
  int itemID,itemindex=-1;
  int pRmbPoint, RmbPoints;
  char buff[256], token[256];
  int ret;

	if(getStringFromIndexWithDelim(buf , "," , 1, buff, sizeof(buff)) !=FALSE)
		itemID = atoi(buf);

  itemindex = CHAR_findEmptyItemBox( talker );
	if( itemindex < 0 )	{
		CHAR_talkToCli( talker, -1, "物品栏空间不足！！",  CHAR_COLORYELLOW);
		return FALSE;
	}
	
	if(getStringFromIndexWithDelim(buf , "," , 2, buff, sizeof(buff)) !=FALSE)
		RmbPoints=atoi(buff);
	if(RmbPoints<0)RmbPoints=0;

  pRmbPoint = sasql_rmbpoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if( pRmbPoint < RmbPoints )	{
		CHAR_talkToCli( talker, -1, "你的现金不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}
	
	itemindex = ITEM_makeItemAndRegist( itemID );
  if( itemindex != -1 ){
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}

		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		
		sasql_rmbpoint( CHAR_getChar(talker,CHAR_CDKEY), -RmbPoints,1);
		sprintf( token, "扣除现金%d", RmbPoints);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW);

		CHAR_sendItemDataOne( talker, ret);
  }

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionRmbPet( int talker, char *buf)
{
	if (CHAR_getWorkInt( talker, CHAR_WORKSAFEMODE)==0)
   	{
    	CHAR_SafePassword(talker);
    	return FALSE;
    }
	int	ret;
	char msgbuf[64], buf1[8], buf2[10];
	int	enemynum;
	int	i,j;
	int petindex, petindex2, petid;
  int pRmbPoint, RmbPoints;
		
	if(getStringFromIndexWithDelim(buf , "," , 1, buf1, sizeof(buf1)) !=FALSE)
#ifdef _PET_MM
	if(strstr(buf1,"MM1") || strstr(buf1,"MM3"))
		petid = 1479;
	else if(strstr(buf1,"MM2") || strstr(buf1,"MM4"))
		petid = 2547;
	else
		petid = atoi( buf1 );
#else
	petid = atoi( buf1 );
#endif
		
	if(getStringFromIndexWithDelim(buf , "," , 2, buf2, sizeof(buf2)) !=FALSE)
		RmbPoints=atoi(buf2);
	if(RmbPoints<0)RmbPoints=0;

  pRmbPoint = sasql_rmbpoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if( pRmbPoint < RmbPoints )	{
		CHAR_talkToCli( talker, -1, "你的现金不够哦", CHAR_COLORYELLOW);
		return FALSE;
	}

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
				break;
			}
	}

	if( i == enemynum )
		return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( talker, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		CHAR_talkToCli( talker, -1, "领取宠物失败！", CHAR_COLORYELLOW);

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
#ifdef _PET_MM
		if(strstr(buf1,"MM1") || strstr(buf1,"MM2")){
			int LevelUpPoint;
			LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
			CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
		}else if(strstr(buf1,"MM3") || strstr(buf1,"MM4")){
			while(CHAR_getInt( ret, CHAR_LV)<79){	//升级
				int LevelUpPoint;
				LevelUpPoint = ( 50<< 24) + ( 50<< 16) + ( 50<< 8) + ( 50<< 0);
				CHAR_setInt( ret, CHAR_ALLOCPOINT, LevelUpPoint);
				CHAR_PetLevelUp( ret );
				CHAR_PetAddVariableAi( ret, AI_FIX_PETLEVELUP );
				CHAR_setInt( ret, CHAR_LV, CHAR_getInt( ret, CHAR_LV) +1 );
			}
		}
#endif
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(talker, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
	sasql_rmbpoint( CHAR_getChar(talker,CHAR_CDKEY), -RmbPoints,1);
	snprintf( msgbuf,sizeof( msgbuf), "扣除现金%d", RmbPoints);
	CHAR_talkToCli( talker, -1, msgbuf, CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(talker, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( talker, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( talker, msgbuf );
	}
	return TRUE;
}
#endif

#ifdef _TRANS_7_NPC
BOOL NPC_ActionTrans7( int talker, char *buf)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
#ifdef _ADD_POOL_ITEM			   // WON ADD 增加可寄放的道具	
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// 增加的道具寄放数
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// 增加的宠物寄放数
	int j,item_sum=0, pet_sum=0;
	
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<80){
		sprintf( token, "由于你已七转或等级小于80级，所以转生失败!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
	
	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// 限制最大的寄道具数
	pet_sum  += 5;							    // 限制最大的寄宠数

#endif
	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);
	CHAR_setInt(talker, CHAR_LV ,1);
	CHAR_setMaxExp( talker, 0);
  CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_TRANSMIGRATION)*10);
  CHAR_Skillupsend( talker );
  CHAR_setInt( talker,CHAR_RIDEPET, -1 );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
}

#ifdef _TRANS7_POINT
BOOL NPC_ActionTransNewSeven( int talker, char *buf)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
#ifdef _ADD_POOL_ITEM			   // WON ADD 增加可寄放的道具	
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// 增加的道具寄放数
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// 增加的宠物寄放数
	int j,item_sum=0, pet_sum=0;
	
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<160){
		sprintf( token, "由于你已七转或等级小于160级，所以转生失败!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
	
	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// 限制最大的寄道具数
	pet_sum  += 5;							    // 限制最大的寄宠数

#endif
//	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);

	CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+getTrans7Point());

  CHAR_Skillupsend( talker );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
}
#endif
BOOL NPC_ActionTransNew7( int talker, char *buf)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
#ifdef _ADD_POOL_ITEM			   // WON ADD 增加可寄放的道具	
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// 增加的道具寄放数
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// 增加的宠物寄放数
	int j,item_sum=0, pet_sum=0;
	
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<160){
		sprintf( token, "由于你已七转或等级小于160级，所以转生失败!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
	
	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// 限制最大的寄道具数
	pet_sum  += 5;							    // 限制最大的寄宠数

#endif
//	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);
  if(CHAR_getInt(talker,CHAR_SUPER)==1)
  	CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+80);
  else 
  	{
  		if(CHAR_getInt(talker,CHAR_VIPRIDE)>0)
  			CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+75);
  		else
  			CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+70);
  	}
  CHAR_Skillupsend( talker );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
}
#endif

BOOL NPC_ActionAddItem( int talker, char *buf)
{
  int itemID,k=0,itemindex=-1;
  int spaceNum=5,i;
  char buff3[256], msgbuf[64], token[256];
  int ret;

  k=1;
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
	k++;
		for( i = spaceNum ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex=CHAR_getItemIndex( talker , i );
			if( itemindex == -1 )	{
				spaceNum = i+1;
				break;
			}
		}                                                                                                                                                                                                                                                                                                                                      
		if( i == CHAR_MAXITEMHAVE )	{
			snprintf( msgbuf,sizeof( msgbuf), "物品栏空间不足！！");
			CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORYELLOW);
			return FALSE;
		}
  }
  k=1;
  while( getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3)) !=FALSE ){
	k++;
        itemID = atoi( buff3);
        if( itemID  )
		itemindex = ITEM_makeItemAndRegist( itemID);
		if(itemindex == -1)
			continue;
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			ITEM_endExistItemsOne( itemindex);
			print ("\n ret error!!");
			return FALSE;
		}

		sprintf( token,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		CHAR_sendItemDataOne( talker, ret);
  }

	return TRUE;	                                                                                                                                                                      
}

BOOL NPC_ActionSetEend( int talkerindex, char * buf)
{
	int eveno;
	int talkNo=1, iflg=0;
	char buf1[256];

	while( getStringFromIndexWithDelim( buf,",",talkNo,buf1,sizeof( buf1)) != FALSE)	{ 
		talkNo++;
		eveno = atoi( buf);
		NPC_EventSetFlg( talkerindex, eveno);
		if( ++iflg > 20 ) break;
	}
	return TRUE;
}

BOOL NPC_ActionClearEvent( int toindex, char * buf)
{
	int eveno;
	int talkNo=1, iflg=0;
	char buf1[256];

	while( getStringFromIndexWithDelim( buf,",",talkNo,buf1,sizeof( buf1)) != FALSE)	{ 
		talkNo++;
		eveno = atoi( buf);
		NPC_EndEventSetFlgCls( toindex, eveno);
		if( ++iflg > 20 ) break;
	}
	return TRUE;
}

BOOL NPC_ActionSetNow( int toindex, char * buf)
{
	int eveno;
	int talkNo=1, iflg=0;
	char buf1[256];

	while( getStringFromIndexWithDelim( buf,",",talkNo,buf1,sizeof( buf1)) != FALSE)	{ 
		talkNo++;
		eveno = atoi( buf);
		NPC_NowEventSetFlg( toindex, eveno);
		if( ++iflg > 20 ) break;
	}
	return TRUE;
}


BOOL NPC_ActionPassCheck( int meindex, int talker, char *buf)
{

	char buff2[256];
	char buff3[128];
	int i=1,j=1;
	int loop=0;
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )	{	
		i++;
		if(strstr(buff2,"&")!=NULL){
			j=1;
			loop=0;
			while( getStringFromIndexWithDelim(buff2,"&",j,buff3,sizeof(buff3)) !=FALSE )	{
				j++;
				if(NPC_ActionBigSmallCheck( meindex, talker, buff3)==FALSE)	{
					loop=1;
					break;
				}
			}
			if(loop==0) {
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}else{
			if( NPC_ActionBigSmallCheck( meindex, talker, buff2) == TRUE){
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;

			}
		}
	}
	CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, FALSE );
	return FALSE;

}

BOOL NPC_ActionBigSmallCheck(int meindex,int talker,char* buf)
{

	char buff2[128], buff3[128];
	int kosuu,temp=-1,flg=0;

#ifdef _NEW_WARPMAN
	char buff1[128];
	if(strstr( buf, "-") != NULL)	{
		getStringFromIndexWithDelim( buf, "-", 2, buff3, sizeof(buff3));
		temp = atoi( buff3);
		getStringFromIndexWithDelim( buf, "-", 1, buff1, sizeof(buff1));
		strcpy( buf, buff1);
	}                         
#endif

	if( strstr( buf, "<") != NULL ){
		getStringFromIndexWithDelim( buf, "<", 2, buff3, sizeof(buff2));
		kosuu = atoi( buff3);
		getStringFromIndexWithDelim( buf, "<", 1, buff2, sizeof(buff2));

		if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, 1, temp)==TRUE){
			return TRUE;
		}
	}else if(strstr( buf, ">") != NULL){
		getStringFromIndexWithDelim( buf, ">", 2, buff3, sizeof(buff2));
		kosuu = atoi(buff3);
		getStringFromIndexWithDelim( buf, ">" ,1, buff2, sizeof(buff2));

		if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, 2, temp) == TRUE ){
			return TRUE;
		}

	}else if( strstr( buf, "!" ) != NULL){
		getStringFromIndexWithDelim( buf, "!=", 2, buff3, sizeof(buff2));
		kosuu = atoi( buff3);
		getStringFromIndexWithDelim( buf, "!=", 1, buff2, sizeof(buff2));
		if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, 0, temp) == TRUE ){
			return FALSE;
		}else{
			return TRUE;
		}
	}else if( strstr( buf, "=") != NULL){
		getStringFromIndexWithDelim( buf, "=", 2, buff3, sizeof(buff2));
		kosuu = atoi( buff3);
		getStringFromIndexWithDelim( buf, "=", 1, buff2, sizeof(buff2));
		flg = 0;
		if( strstr( buf, "*") != NULL){
			if( NPC_ActionWarpManReduce( meindex, talker, buf)==TRUE){
				return TRUE;
			}
		}
#ifdef _NPC_ActionFreeCmp
		else if( strstr( buf, "^") != NULL){
		    if( NPC_ActionWarpManReduce2( meindex, talker, buf)==TRUE){
				return TRUE;
			}
		}
#endif
		else if( NPC_ActionFreeIfCheck( meindex, talker, buff2, buff3, kosuu, flg, temp) == TRUE )	{
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef _NPC_ActionFreeCmp 
//其实功能和原本的NPC_ActionWarpManReduce差不多,但为了不影响以前的设定,所以不去修改原本的
BOOL NPC_ActionWarpManReduce2(int meindex,int talker,char *buf)
{
	char buf2[512];
	char buf3[256];
	int i, itemindex, itemno, kosuu;
	int id=0;
	int nums = 0;

	getStringFromIndexWithDelim(buf,"=",2,buf2,sizeof(buf2));
	getStringFromIndexWithDelim(buf2,"^",1,buf3,sizeof(buf3));
	itemno = atoi(buf3);
	getStringFromIndexWithDelim(buf2,"^",2,buf3,sizeof(buf3));
	kosuu = atoi(buf3);

	for( i=CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE;i++ ){
		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( itemno == id ){
				nums += ITEM_getInt( itemindex, ITEM_USEPILENUMS);//数量
			}
		}
	}

	if( nums == kosuu ) return TRUE;

	return FALSE;
}
#endif

#ifdef _NEW_MANOR_LAW
extern struct FM_POINTLIST  fmpointlist;
#endif

BOOL NPC_ActionFreeIfCheck(int meindex,int talker, char* buf, char *opt, int kosuu,int flg, int temp)
{
#ifdef _VIP_SHOP
	if(strcmp(buf,"VIPPOINT")==0){
		if(NPC_ActionVipPointCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif
#ifdef _VIP_RIDE
	if(strcmp(buf,"VIP")==0){
		if(NPC_ActionVipCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif

	if(strcmp(buf,"LV")==0){
		if(NPC_ActionLevelCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	
#ifdef _ALL_ITEM
	if(strcmp(buf,"ALLNUM1")==0){
		if(NPC_ActionAllNum1Check(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	
	if(strcmp(buf,"ALLNUM2")==0){
		if(NPC_ActionAllNum2Check(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	
	if(strcmp(buf,"ALLNUM3")==0){
		if(NPC_ActionAllNum3Check(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	
	if(strcmp(buf,"ALLNUM4")==0){
		if(NPC_ActionAllNum4Check(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	
	if(strcmp(buf,"ALLNUM5")==0){
		if(NPC_ActionAllNum5Check(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif
	
	if(strcmp(buf,"FAME")==0){
		if(NPC_ActionFameCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"VIPPOINT")==0){
		if(NPC_ActionVippointCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"AMPOINT")==0){
		if(NPC_ActionAmpointCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#ifdef	_VIGOR_SYS
	if(strcmp(buf,"VIGOR")==0){
		if(NPC_ActionVigorCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif
#ifndef _CAX_NPCTIME_OFF
	if(strcmp(buf,"YEAR")==0){
		if(NPC_ActionYearCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"MON")==0){
		if(NPC_ActionMonCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"DATE")==0){
		if(NPC_ActionDateCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"DAY")==0){
		if(NPC_ActionDayCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HOUR")==0){
		if(NPC_ActionHourCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"MIN")==0){
		if(NPC_ActionMinCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}

	if(strcmp(buf,"SEC")==0){
		if(NPC_ActionSecCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif	
	if(strcmp(buf,"GOLD")==0){
		if(NPC_ActionGoldCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	
	if( strcmp(buf,"TRANS") == 0 )	{
		if( NPC_ActionCheckTrans( meindex, talker, kosuu, flg) == TRUE )
			return TRUE;
	}
	
	if(strcmp(buf,"SKUP") == 0){
		if( NPC_ActionCheckSkup( meindex, talker, kosuu, flg) == TRUE )
			return TRUE;
	}
	
	if(strcmp(buf,"SAFE") == 0){
		if( NPC_ActionCheckSafe( meindex, talker, kosuu, flg) == TRUE )
			return TRUE;
	}
	
	if( !strcmp( buf, "GTIME") ){
		if( NPC_CheckTimeDefine( opt) == TRUE )
			return TRUE;
	}

	if(strcmp( buf, "TIME") == 0) {
		if(NPC_TiemCheck( meindex, talker, kosuu, flg) == TRUE)
			return TRUE;
	}

	if( strcmp( buf, "PET") == 0 )	{
		if( temp > 0 )	{
			if( ActionCheckMyPet( meindex, talker, kosuu, flg, temp) == TRUE )
				return TRUE;
		}else
			return FALSE;
	}
	if(strcmp(buf,"ITEM")==0){
		if(NPC_ActionItemCheck(meindex,talker,kosuu, flg)==TRUE)
				return TRUE;
	}

	if(strcmp(buf,"EQUIT")==0){
		if(NPC_ActioneEquitCheck(meindex,talker,kosuu, flg)==TRUE)
				return TRUE;
	}

	if( !strcmp(buf,"ENDEV") || !strcmp(buf,"EVEND") ){
		if(NPC_EventCheckFlg( talker, kosuu)==TRUE)
				return TRUE;
	}
	if( !strcmp(buf,"NOWEV") || !strcmp(buf,"EVNOW") ){
		if(NPC_NowEventCheckFlg( talker, kosuu)==TRUE)
				return TRUE;
	}

	if(strcmp(buf,"PARTY")==0){
		if(NPC_ActionPartyCheck( talker, kosuu, flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"reITEM")==0){
		if( NPC_ActionReItemCheck( talker, kosuu, flg)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"rePET")==0){
		if( NPC_ActionRePetCheck( talker, kosuu, flg)==TRUE)
				return TRUE;
	}

#ifdef _NPCCHANGE_PLAYERIMG
	if(strcmp( buf, "BBI")==0)	{
		if( NPC_CheckPlayerBBI( meindex, talker, kosuu, flg)== TRUE)	{
			return TRUE;
		}
	}
#endif
	if( strcmp( buf, "FM") == 0 ){
		if( CHAR_getInt( talker, CHAR_FMLEADERFLAG ) > 0 && 
			CHAR_getInt( talker, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY)
		if( CHAR_getWorkInt( talker, CHAR_WORKFMFLOOR) == kosuu ){
			return TRUE;
		}
	}
	if(strcmp(buf,"DR")==0){
		if(NPC_ActionDuelRankCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	if(strcmp(buf,"DP")==0){
		if(NPC_ActionDuelPointCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( strcmp( buf, "CLASS") == 0 ){
		if(NPC_ActionClassCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
	if( strcmp( buf, "Skill") == 0 ){
		if( NPC_ActionSkillCheck( meindex, talker, kosuu, flg)==TRUE)
				return TRUE;
	}
	if( strcmp( buf, "SkNum") == 0 ){
		if( NPC_ActionSkillNumCheck( meindex, talker, kosuu, flg)==TRUE)
				return TRUE;
	}
	if( strcmp( buf, "SKCP") == 0 ){
		if( NPC_ActionSkillPointAndSkillCheck( meindex, talker, kosuu, flg)==TRUE)
				return TRUE;
	}
#endif
	

#ifdef _TREASURE_BOX
	if( strcmp( buf, "BOX") == 0 ){
		if( NPC_ActionTreasureTypeCheck( meindex, kosuu,
			//NPC_TIME_EVENTMODE = CHAR_NPCWORKINT6,// 1
			CHAR_getWorkInt( meindex, CHAR_NPCWORKINT6) ,flg)==TRUE)
				return TRUE;
	}
#endif

#ifdef _NPC_ADDWARPMAN1
    if( strcmp( buf, "PARTYCOUNT") == 0){
	    if(NPC_ActionPartyCountCheck( talker, kosuu, flg)==TRUE)
			return TRUE;
	}
	if( strcmp( buf, "MANCOUNT" ) == 0 ){
        if(NPC_ActionManCountCheck( talker, kosuu, flg ) == TRUE )
			return TRUE;
	}
	if( strcmp( buf, "WOMANCOUNT" ) == 0 ){
        if(NPC_ActionWomanCountCheck( talker, kosuu, flg ) == TRUE )
			return TRUE;
	}
#endif

#ifdef _ANGEL_SUMMON
	if(strcmp(buf,"ANGEL_NOW")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_DOING, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_NOW")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_DOING, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_OVER")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_OVER")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_OUT")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_TIMEOVER, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_OUT")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_TIMEOVER, 0)==TRUE)
				return TRUE;
	}

	if(strcmp(buf,"ANGEL_I_NOW")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_DOING, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_I_NOW")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_DOING, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_I_OVER")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_I_OVER")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_I_OUT")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_TIMEOVER, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_I_OUT")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_TIMEOVER, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp( buf, "HEROCNT") == 0) {
		//if(NPC_EventLevelCheck( meindex, talker, kosuu, flg) == TRUE) {
		if(NPC_HeroCompleteCountCheck( meindex, talker, kosuu, flg) == TRUE)
			return TRUE;
	}

#endif

#ifdef _CAX_NPCTIME_IF
if(strcmp( buf, "HOUR") == 0) {
		if(NPC_TiemCheckWithHour( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	if(strcmp( buf, "DAY") == 0) {
		if(NPC_TiemCheckWithDay ( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
#endif

#ifdef _NEW_MANOR_LAW
	if(strcmp(buf,"MANOR") == 0){
		// 大於-1表示要检查是否为庄园家族成员才能warp
		if(kosuu > -1){
			int iFmIndex = -1,iHadFmindex = -1;
			char token[256];
			
			iFmIndex = CHAR_getInt(talker,CHAR_FMINDEX);
			getStringFromIndexWithDelim(fmpointlist.pointlistarray[kosuu],"|",5,token,sizeof(token));
			iHadFmindex = atoi(token);
			if(iFmIndex > -1){
				// 是庄园拥有家族的成员
				if(iFmIndex == iHadFmindex){
					// 必须有家族且是正式族员
					if(CHAR_getInt(talker,CHAR_FMLEADERFLAG) != FMMEMBER_NONE &&
						 CHAR_getInt(talker,CHAR_FMLEADERFLAG) != FMMEMBER_APPLY) return TRUE;
				}
			}
		}
	}
#endif

	return FALSE;
}

BOOL NPC_ActioneEquitCheck(int meindex,int talker,int itemNo, int flg)
{
	int i;
	int itemindex=-1;
	int id;
	for( i=0;i<CHAR_STARTITEMARRAY;i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( NPC_ActionBigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL NPC_ActionItemCheck(int meindex,int talker,int itemNo, int flg)
{
	int i;
	int itemindex=-1;
	int id;
	for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( NPC_ActionBigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}

	return FALSE;
}

BOOL NPC_ActionBigSmallLastCheck(int point1,int mypoint,int flg)
{
	if(flg==0){
		if(point1==mypoint)
			return TRUE;
	}else if(flg==1){
		if(mypoint < point1)
			return TRUE;
	}else if(flg==2){
		if(mypoint > point1)
			return TRUE;
	}

	return FALSE;
}

BOOL ActionCheckMyPet( int meindex, int talker, int petLv, int flg, int petid)
{
	int petsel,petindex=-1;	
	int lv=-1;
	for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++)     {
		petindex = CHAR_getCharPet( talker, petsel);
		if( petindex == -1  )
			continue;
	        if(CHAR_getInt( petindex, CHAR_PETID) !=  petid )
			continue;
		break;
	}
	if( petsel  == CHAR_MAXPETHAVE )	{
		return FALSE;
	}else	{	//找到条件宠
		lv = CHAR_getInt( petindex, CHAR_LV);
		if( NPC_ActionBigSmallLastCheck( petLv, lv, flg) == TRUE)	{
			return TRUE;
		}else	{
			return FALSE;
		}
	}
	return FALSE;                                             
}


BOOL NPC_ActionCheckTrans(int meindex,int talker,int trans,int flg)
{
	int myTrans;
	myTrans=CHAR_getInt(talker, CHAR_TRANSMIGRATION);
	if( NPC_ActionBigSmallLastCheck( trans, myTrans, flg ) == TRUE )	{
             return TRUE;
        }                   
	return FALSE;
}


#ifdef _VIP_SHOP
BOOL NPC_ActionVipPointCheck(int meindex,int talker,int point,int flg)
{
	int mypoint;
	mypoint=sasql_vippoint(CHAR_getUseID(talker),0,0);
	if(NPC_ActionBigSmallLastCheck( point, mypoint, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif
#ifdef _VIP_RIDE
BOOL NPC_ActionVipCheck(int meindex,int talker,int vip,int flg)
{
	if(NPC_ActionBigSmallLastCheck( CHAR_getInt(talker,CHAR_VIPRIDE), vip, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif
#ifdef _VIP_SHOP
BOOL NPC_ActionNewVipPointCheck(int meindex,int talker,int point,int flg)
{
	int mypoint;
	mypoint=sasql_vippoint(CHAR_getUseID(talker),0,0);
	if(NPC_ActionBigSmallLastCheck( point, mypoint, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif
BOOL NPC_ActionLevelCheck(int meindex,int talker,int level,int flg)
{
	int mylevel;
	mylevel=CHAR_getInt(talker,CHAR_LV);
	if(NPC_ActionBigSmallLastCheck( level, mylevel, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

#ifdef _ALL_ITEM
BOOL NPC_ActionAllNum1Check(int meindex,int talker,int npcallnum1,int flg)
{
	int allnum1 = sasql_allnum(1,0,0);
	if(NPC_ActionBigSmallLastCheck( npcallnum1, allnum1, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionAllNum2Check(int meindex,int talker,int npcallnum2,int flg)
{
	int allnum2 = sasql_allnum(2,0,0);
	if(NPC_ActionBigSmallLastCheck( npcallnum2, allnum2, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionAllNum3Check(int meindex,int talker,int npcallnum3,int flg)
{
	int allnum3 = sasql_allnum(3,0,0);
	if(NPC_ActionBigSmallLastCheck( npcallnum3, allnum3, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionAllNum4Check(int meindex,int talker,int npcallnum4,int flg)
{
	int allnum4 = sasql_allnum(4,0,0);
	if(NPC_ActionBigSmallLastCheck( npcallnum4, allnum4, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionAllNum5Check(int meindex,int talker,int npcallnum5,int flg)
{
	int allnum5 = sasql_allnum(5,0,0);
	if(NPC_ActionBigSmallLastCheck( npcallnum5, allnum5, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif

BOOL NPC_ActionFameCheck(int meindex,int talker,int fame,int flg)
{
	int myfame;
	myfame=CHAR_getInt(talker,CHAR_FAME)/100;
	if(NPC_ActionBigSmallLastCheck( fame, myfame, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionVippointCheck(int meindex,int talker,int point,int flg)
{
	int mypoint;
	mypoint=sasql_vippoint(CHAR_getChar(talker,CHAR_CDKEY),0,0);
	if(NPC_ActionBigSmallLastCheck( point, mypoint, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionAmpointCheck(int meindex,int talker,int ampoint,int flg)
{
	int myampoint;
	myampoint=sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);
	if(NPC_ActionBigSmallLastCheck( ampoint, myampoint, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#ifdef	_VIGOR_SYS
BOOL NPC_ActionVigorCheck(int meindex,int talker,int vigor,int flg)
{
	int myvigor;
	myvigor=CHAR_getInt(talker,CHAR_VIGOR);
	if(NPC_ActionBigSmallLastCheck( vigor, myvigor, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif

#ifndef _CAX_NPCTIME_OFF
BOOL NPC_ActionYearCheck(int meindex,int talker,int npcyear,int flg)
{
	NPC_ActionTime();
	if(NPC_ActionBigSmallLastCheck( npcyear, year, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionMonCheck(int meindex,int talker,int npcmon,int flg)
{
	NPC_ActionTime();
	if(NPC_ActionBigSmallLastCheck( npcmon, mon, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionDateCheck(int meindex,int talker,int npcdate,int flg)
{
	NPC_ActionTime();
	if(NPC_ActionBigSmallLastCheck( npcdate, date, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionDayCheck(int meindex,int talker,int npcday,int flg)
{
	NPC_ActionTime();
	if(NPC_ActionBigSmallLastCheck( npcday, day, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionHourCheck(int meindex,int talker,int npchour,int flg)
{
	NPC_ActionTime();
	if(NPC_ActionBigSmallLastCheck( npchour, hour, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionMinCheck(int meindex,int talker,int npcmin,int flg)
{
	NPC_ActionTime();
	if(NPC_ActionBigSmallLastCheck( npcmin, min, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}


BOOL NPC_ActionSecCheck(int meindex,int talker,int npcsec,int flg)
{
	NPC_ActionTime();
	if(NPC_ActionBigSmallLastCheck( npcsec, sec, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
BOOL NPC_ActionClassCheck(int meindex,int talker,int p_class,int flg)
{
	int myclass;
	myclass= CHAR_getInt( talker, PROFESSION_CLASS );

	if(NPC_ActionBigSmallLastCheck( p_class, myclass, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionSkillPointAndSkillCheck( int meindex,int talker,int sknum,int flg)
{
	int myskillpoint, myskillnum=0, myskilllevel;
	int mysknum=0, j;
	myskillpoint= CHAR_getInt( talker, PROFESSION_SKILL_POINT );
	myskilllevel= CHAR_getInt( talker, PROFESSION_LEVEL );

	for( j=0; j<PROFESSION_MAX_LEVEL; j++ ){
		if( CHAR_getCharSkill( talker, j) <= 0 ) continue;
		myskillnum++;
	}
	mysknum = myskilllevel - (myskillpoint + myskillnum);
	//andy_log
	print("mysknum%d sknum:%d \n", mysknum, sknum);
	return NPC_ActionBigSmallLastCheck( sknum, mysknum, flg);
}

BOOL NPC_ActionSkillNumCheck( int meindex, int talker, int sknum, int flg)
{
	int j, mynum=0;

	for( j=0; j<PROFESSION_MAX_LEVEL; j++ ){
		if( CHAR_getCharSkill( talker, j) <= 0 ) continue;
		mynum++;
	}
	return NPC_ActionBigSmallLastCheck( sknum, mynum, flg);
}

BOOL NPC_ActionSkillCheck( int meindex,int talker,int skillId,int flg)
{
	int sID, j;
		//andy_log
		print("NPC_ActionSkillCheck( skillId:%d ) \n", skillId);
	for( j=0; j<PROFESSION_MAX_LEVEL; j++ ){
		if( (sID = CHAR_getCharSkill( talker, j)) == 0 ) continue;

		//andy_log
		print("sID:%d skillId:%d flg:%d\n", sID, skillId, flg);
		if( NPC_ActionBigSmallLastCheck( skillId, sID, flg) == TRUE){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL NPC_ActionAddProfessionSkillPoint( int toindex, char *buf )
{
	char token[256];
	int skillpoint=0, addsp=0;
	addsp = atoi( buf);
	skillpoint = CHAR_getInt( toindex , PROFESSION_SKILL_POINT);
	CHAR_setInt( toindex , PROFESSION_SKILL_POINT, skillpoint + addsp );
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
	sprintf( token, "技能学习点数增加%d点。", addsp);
	CHAR_talkToCli( toindex, -1, token, CHAR_COLORYELLOW);
	return TRUE;
}

BOOL NPC_ActionCleanProfession( int toindex, char *buf)
{
	int k=1,i;
	char buf1[256], buf2[256];
	CHAR_HaveSkill *pSkil;

	k=1;
	while( getStringFromIndexWithDelim( buf , "," , k, buf1, sizeof( buf1)) !=FALSE ){
		k++;
		if( getStringFromIndexWithDelim( buf1 , "=" , 1, buf2, sizeof( buf2)) ==FALSE ) continue;

		if( !strcmp( buf2, "SKILL") ){
			int psID=-1;
			if( getStringFromIndexWithDelim( buf1 , "=" , 2, buf2, sizeof( buf2)) ==FALSE ) continue;
			if( strstr( buf2, "ALL") != NULL ){
			}else{
				psID = atoi( buf2);
			}
			for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
				if( psID != -1 && psID != CHAR_getIntPSkill( toindex, i, SKILL_IDENTITY) ) continue;
					pSkil = CHAR_getCharHaveSkill( toindex, i );
					SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
					pSkil->use = 0;
			}
		}else if( !strcmp( buf2, "CLASS") ){
			if( getStringFromIndexWithDelim( buf1 , "=" , 2, buf2, sizeof( buf2)) ==FALSE ) continue;
			if( strstr( buf2, "ALL") != NULL ){
				CHAR_setInt( toindex , PROFESSION_CLASS, 0);
#ifdef _CHANNEL_MODIFY
				if(CHAR_getInt(toindex,PROFESSION_CLASS) > 0){
					int i,pclass = CHAR_getInt(toindex,PROFESSION_CLASS) - 1;
					for(i=0;i<getFdnum();i++){
						if(*(piOccChannelMember + (pclass * getFdnum()) + i) == toindex){
							*(piOccChannelMember + (pclass * getFdnum()) + i) = -1;
							break;
						}
					}
				}
#endif
				CHAR_setInt( toindex , PROFESSION_LEVEL, 0);
				CHAR_setInt( toindex , PROFESSION_SKILL_POINT, 0);
				CHAR_setInt( toindex , ATTACHPILE, 0);
			}else{
				print("尚未设定参数!! (%s %d)\n", __FILE__, __LINE__);
			}
		}
	}
	CHAR_sendStatusString( toindex , "S");
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( toindex , CHAR_WORKOBJINDEX ));
	return TRUE;	                                                                                                                                                                      
}
#endif


BOOL NPC_ActionPartyCheck( int toindex, int nums, int flg)
{
	int partys=0;
	int leaderindex=-1;
	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
		partys = 0;
		leaderindex = toindex;
	}else if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ){
		leaderindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1);
	}

	if( CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ){
		int i;
		for( i=0; i<5; i++)	{
			int pindex = CHAR_getWorkInt( leaderindex, CHAR_WORKPARTYINDEX1+i);
			if( !CHAR_CHECKINDEX( pindex) ) continue;
				partys++;
		}
	}

	if(NPC_ActionBigSmallLastCheck( nums, partys, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionReItemCheck( int toindex, int nums, int flg)
{
	int remnants=0;
	remnants = CHAR_findSurplusItemBox( toindex);
	if(NPC_ActionBigSmallLastCheck( nums, remnants, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}
BOOL NPC_ActionRePetCheck( int toindex, int nums, int flg)
{
	int remnants=0;
	remnants = CHAR_findSurplusPetBox( toindex);
	if(NPC_ActionBigSmallLastCheck( nums, remnants, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionGoldCheck(int meindex,int talker,int Golds,int flg)
{
	int myGold;
	myGold=CHAR_getInt(talker,CHAR_GOLD);
	if(NPC_ActionBigSmallLastCheck( Golds, myGold, flg) == TRUE){
		return TRUE;
	}
	return FALSE;
}

BOOL NPC_ActionWarpManReduce(int meindex,int talker,char *buf)
{
	char buf2[512];
	char buf3[256];
	int i, itemindex, itemno, kosuu;
	int id=0;
#ifdef _ITEM_PILENUMS
	int nums = 0;
#else 
	int cnt =0;
#endif
	getStringFromIndexWithDelim(buf,"=",2,buf2,sizeof(buf2));
	getStringFromIndexWithDelim(buf2,"*",1,buf3,sizeof(buf3));
	itemno = atoi(buf3);
	getStringFromIndexWithDelim(buf2,"*",2,buf3,sizeof(buf3));
	kosuu = atoi(buf3);

	for( i=CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE;i++ ){
		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( itemno == id ){
#ifdef _ITEM_PILENUMS
				nums += ITEM_getInt( itemindex, ITEM_USEPILENUMS);//数量
#else
				cnt++;
				if(cnt==kosuu){
					return TRUE;
				}
#endif
			}
		}
	}

#ifdef _ITEM_PILENUMS
	if( nums >= kosuu ) return TRUE;
#endif

	return FALSE;

}

#ifdef _NPCCHANGE_PLAYERIMG	//andy
BOOL NPC_ActionChangePlayerBBI( int meindex, int charindex, char *Img)
{
	int cBBI;
	char buf1[256];
	memset( buf1, 0, sizeof( buf1));
	if( !CHAR_CHECKINDEX( charindex) )
		return FALSE;
	if( !CHAR_CHECKINDEX( meindex) )
		return FALSE;
#ifdef _FIXBUG_ATTACKBOW
	{
		int armtype;
		int itemindex = CHAR_getItemIndex( charindex, CHAR_ARM);
		if( ITEM_CHECKINDEX(itemindex) ){
			armtype = ITEM_getInt( itemindex, ITEM_TYPE );
			if( armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
				armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG)	{
				CHAR_talkToCli( charindex, -1, "使用远距离武器无法变身！", CHAR_COLORYELLOW );
				return FALSE;
			}
		}

	}
#endif
#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( charindex, CHAR_BECOMEPIG) > -1 ){//变成乌力了
		CHAR_talkToCli( charindex, -1, "无法变身，乌力化中不能变身！", CHAR_COLORYELLOW );
	    return FALSE;
	}
#endif
	if( CHAR_getInt( charindex, CHAR_RIDEPET) != -1 )	{//骑宠不换图
		CHAR_setInt( charindex, CHAR_RIDEPET, -1 );
		BATTLE_changeRideImage( charindex );
		CHAR_talkToCli( charindex, -1, "取消骑宠状态。", CHAR_COLORYELLOW);
		
		//change -----> 修改bug(落马後,宠物选项的状态依然为"骑乘",这里修正过来 
		CHAR_complianceParameter( charindex );
		CHAR_send_P_StatusString( charindex, CHAR_P_STRING_RIDEPET);
		
		//return FALSE;
	}
#ifdef _ITEM_METAMO
	CHAR_setWorkInt( charindex, CHAR_WORKITEMMETAMO, 0);
#endif

	if( Img == NULL || !strcmp( Img, "\0"))
		return FALSE;
//=======================
	CHAR_setWorkInt( charindex, CHAR_WORKNPCMETAMO, meindex);
	if( strstr( Img, "NPCBBI") != NULL )	{
		cBBI = CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER);
		sprintf( buf1, "你化身成了%s的样子。", CHAR_getChar( meindex, CHAR_NAME));
	}else if( strstr( Img, "MYBBI") != NULL )	{
		cBBI = CHAR_getInt( charindex, CHAR_BASEBASEIMAGENUMBER);
		CHAR_setWorkInt( charindex, CHAR_WORKNPCMETAMO, 0);
		sprintf( buf1, "变回了原来的样子。");
	}else if( strstr( Img, ",") != NULL )	{
		int imgindex=1, maxImg=0;
		while( getStringFromIndexWithDelim( Img, ",", imgindex, buf1, sizeof( buf1)) != FALSE )	{
			imgindex++;
			maxImg++;
		}
		if( (imgindex = (rand()%maxImg)) > maxImg) imgindex = 1;
		if( imgindex < 1 ) imgindex = 1;

		if( getStringFromIndexWithDelim( Img, ",", imgindex, buf1, sizeof( buf1)) == FALSE )
			return FALSE;
		cBBI = atoi( buf1);
		sprintf( buf1, "你样子改变了。");
	}else	{
		cBBI = atoi( Img);
		sprintf( buf1, "你样子改变了。");
	}
//=======================
	if( cBBI < 0 )	{
		return FALSE;
	}
	CHAR_talkToCli( charindex, -1, buf1, CHAR_COLORYELLOW);
	//CHAR_BASEBASEIMAGENUMBER	CHAR_BASEIMAGENUMBER
	CHAR_setInt( charindex, CHAR_BASEIMAGENUMBER, cBBI);
	CHAR_complianceParameter( charindex );
	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charindex , CHAR_WORKOBJINDEX ));
	CHAR_send_P_StatusString( charindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
	//CHAR_K_STRING_BASEIMAGENUMBER
	//CHAR_P_STRING_BASEBASEIMAGENUMBER
	return TRUE;
}
#endif

#ifdef _NPCCHANGE_PLAYERIMG
BOOL NPC_CheckPlayerBBI( int meindex, int charindex, int BBI, int flg)
{
	int MyBBI;
	if( !CHAR_CHECKINDEX( charindex))
		return FALSE;
	if( BBI < 0 )
		return FALSE;
	MyBBI = CHAR_getInt( charindex, CHAR_BASEIMAGENUMBER);
	return NPC_ActionBigSmallLastCheck( BBI, MyBBI,flg);
}
#endif

BOOL showString( int meindex, char *showstr,int flg)
{	//广播
	char buf1[256];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	int toindex = -1;
	int floor;
	int i;
	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("\n not found argstr ");
		return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, showstr,buf1, sizeof(buf1)) == NULL ) {
		strcpy( buf1, showstr);
	}
	toindex = -1;
	floor = CHAR_getInt( meindex , CHAR_FLOOR);
	while( toindex < 1000)	{
		toindex ++;
		if( !CHAR_CHECKINDEX( toindex) )
			continue;
		if( CHAR_getInt( toindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
			continue;
		if( CHAR_getInt( toindex , CHAR_FLOOR) != floor )
			continue;
		CHAR_talkToCli( toindex, meindex, buf1,  CHAR_COLORYELLOW);
		if( flg == 1 )	{
			CHAR_setWorkInt( toindex, CHAR_WORKSTAKEFLAG, FALSE );
			for( i=0; i<5; i++)	{
				CHAR_setWorkInt( toindex, i+CHAR_WORKSTAKETYPE1, 0);
			}
		}
	}
	return TRUE;
}

BOOL NPC_getTimeXYPoint( int meindex, int Mode)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[256], buf1[256], buf2[256];
	int fl=-1, x, y;
	int GraNo = -1;
	int points = 1;	//条件成立为1
	if( Mode <= 0 )	{
		points = 2;
	}
	memset( npcarg, 0, sizeof( npcarg));
	if( !CHAR_CHECKINDEX( meindex) ) return FALSE;
	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL ){
		print("WarpMan:GetArgStrErr");
		return FALSE;
	}
	memset( buf, 0, sizeof( buf));
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "TimeXYPoint", buf, sizeof( buf)) != NULL ){
		if( getStringFromIndexWithDelim(buf , "&" , points, buf1, sizeof( buf1)) != FALSE ){
			memset( buf2, 0, sizeof( buf2));
			if( getStringFromIndexWithDelim(buf1 , "," , 1, buf2, sizeof( buf2)) == FALSE ){
				return FALSE;
			}
			fl = atoi( buf2);
			if( getStringFromIndexWithDelim(buf1 , "," , 2, buf2, sizeof( buf2)) == FALSE ){
				return FALSE;
			}
			x = atoi( buf2);
			if( getStringFromIndexWithDelim(buf1 , "," , 3, buf2, sizeof( buf2)) == FALSE ){
				return FALSE;
			}
			y = atoi( buf2);
			if( MAP_IsValidCoordinate( fl, x, y ) == FALSE ){
				char filename[256];
				NPC_Util_CheckAssignArgFile( meindex, filename);
				return FALSE;
			}
			if( CHAR_getInt( meindex, CHAR_FLOOR) != fl ||
				CHAR_getInt( meindex, CHAR_X) != x ||
				CHAR_getInt( meindex, CHAR_Y) != y ){
				CHAR_warpToSpecificPoint( meindex, fl , x , y);
			}
		}
	}

	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "GraNo", buf, sizeof( buf)) != NULL ){
		if( getStringFromIndexWithDelim(buf , "&" , points, buf1, sizeof( buf1)) != FALSE ){
			GraNo = atoi( buf1);
			CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER, GraNo);
		}
	}
	return TRUE;
}

BOOL NPC_getTimeData( int meindex, char *npcarg, int *born, int *dead, int loopspeed)
{
	int i;
	char buf[256];
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NEWTIME", buf, sizeof( buf)) != NULL ){
		for(i=0 ; i<9 ; i++){
			if( strstr( buf, TimeTble[i].arg ) != NULL){
				*born = TimeTble[i].born;
				*dead = TimeTble[i].dead;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL NPC_CheckTimeDefine( char *timetype)
{
	LSTIME nowlstime;
	int born=-1, dead=-1, finds=0, i;

	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);

	for(i=0 ; i<9 ; i++){
		if( strstr( timetype, TimeTble[i].arg ) != NULL){
			born = TimeTble[i].born;
			dead = TimeTble[i].dead;
			finds = 1;
			break;
		}
	}
	if( finds == 0 ) return FALSE;

	if( born < dead ){
		if( (born < nowlstime.hour) && (dead > nowlstime.hour) ){
			return TRUE;
		}
	}else{
		if( (born < nowlstime.hour && 1024 > nowlstime.hour) ||
			( 0 < nowlstime.hour && dead > nowlstime.hour)){
			return TRUE;
		}
	}

	return FALSE;
}

void NPC_TimeDefineDo( int meindex, int born, int dead, int typeMode)
{
	LSTIME nowlstime;
	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
	if(born < dead){
		if( (born < nowlstime.hour) && (dead > nowlstime.hour) ){
			CHAR_setWorkInt(meindex,typeMode,0);		
		}else{
			CHAR_setWorkInt(meindex,typeMode,1);
		}
	}else{
		if( (born < nowlstime.hour && 1024 > nowlstime.hour) || ( 0 < nowlstime.hour && dead > nowlstime.hour)){
			CHAR_setWorkInt(meindex,typeMode,0);
		}else{
			CHAR_setWorkInt(meindex,typeMode,1);
		}
	}
	if( NPC_getTimeXYPoint( meindex, CHAR_getWorkInt( meindex, typeMode)) == FALSE )
		return;
}


void GetRand_WarpPoint( char *buf, int *fl, int *x, int *y )	
{
	char buff1[256],buff2[256];
	int i=0,point=1;
	struct {
		int m_fl;
		int m_x;
		int m_y;
	} RandPoint[]={
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
	};
		while( getStringFromIndexWithDelim( buf,";",point,buff1,sizeof( buff1)) != FALSE) {
			point++;
			getStringFromIndexWithDelim(buff1,",",1,buff2,sizeof(buff2));
			RandPoint[i].m_fl = atoi(buff2);
			getStringFromIndexWithDelim(buff1,",",2,buff2,sizeof(buff2));
			RandPoint[i].m_x = atoi(buff2);
			getStringFromIndexWithDelim(buff1,",",3,buff2,sizeof(buff2));
			RandPoint[i].m_y = atoi(buff2);
			i++;
			if( i >= arraysizeof( RandPoint) )
				break;
		}
		
		point = RAND( 0, i);
		if( RandPoint[point].m_fl <= 0 || point > arraysizeof( RandPoint) -1)	{
			point = 0;
		}
		*fl = RandPoint[point].m_fl;
		*x = RandPoint[point].m_x;
		*y = RandPoint[point].m_y;

	return;
}

#ifdef _EMENY_CHANCEMAN
int NPC_EmenyChanceCheck( int meindex, int charaindex, char *arg)
{
	char buf1[256];
	int sex=2, fl=-1,x=-1,y=-1;
	memset( buf1, 0, sizeof( buf1));

	if( NPC_Util_GetStrFromStrWithDelim( arg, "CHECKSEX", buf1, sizeof( buf1)) == NULL ) {
		return charaindex;
	}
	if( strstr( buf1, "male") != 0 ){
		sex = 1;
	}
	if( NPC_Util_GetStrFromStrWithDelim( arg, "NOBATTLEWARP", buf1, sizeof( buf1)) != NULL ) {
		char data[256];
		if( getStringFromIndexWithDelim( buf1,",", 1, data, sizeof(data)) == FALSE ) return charaindex;
		fl = atoi( data);
		if( getStringFromIndexWithDelim( buf1,",", 2, data, sizeof(data)) == FALSE ) return charaindex;
		x = atoi( data);
		if( getStringFromIndexWithDelim( buf1,",", 3, data, sizeof(data)) == FALSE ) return charaindex;
		y = atoi( data);
	}
	return NPC_DischargePartyForSex( meindex, charaindex, fl, x, y, sex);
}

int NPC_DischargePartyForSex( int meindex, int charaindex, int fl, int x, int y, int fsex)
{
	int Party[CHAR_PARTYMAX] = {-1,-1,-1,-1,-1};
	int masterindex =-1, i, j, playerNo;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER ){
		playerNo = CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		if( CHAR_getSexInt( playerNo ) == fsex )	{
			if( MAP_IsValidCoordinate( fl, x, y) != FALSE ){
				CHAR_warpToSpecificPoint( charaindex, fl, x, y);
				return -1;
			}
		}else	{
			return charaindex;
		}
	}
	CHAR_talkToCli( charaindex, -1, "团队重组！", CHAR_COLORYELLOW);

	for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
		int index = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+i);
		if( !CHAR_CHECKINDEX( index)) continue;
		Party[i] = index;//纪录所有人
	}
	CHAR_DischargePartyNoMsg( charaindex);//解散团队
	
	for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
		int index = Party[i];
		playerNo = CHAR_getInt( index, CHAR_BASEBASEIMAGENUMBER);
		if( CHAR_getSexInt( playerNo ) == fsex )	{
			if( MAP_IsValidCoordinate( fl, x, y) != FALSE ){
				CHAR_warpToSpecificPoint( index, fl, x, y);
			}
			Party[i] = -1;
		}
	}
	j = CHAR_WORKPARTYINDEX1;
	for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
		int index = Party[i];
		if( !CHAR_CHECKINDEX( index ))continue;
		if( !CHAR_CHECKINDEX( masterindex )){
			masterindex = index;
			CHAR_setWorkInt( masterindex, CHAR_WORKPARTYMODE, CHAR_PARTY_LEADER);
			CHAR_setWorkInt( masterindex, j++, masterindex);
		}else	{
			CHAR_setWorkInt( masterindex, j++, index);
			CHAR_setWorkInt( index, CHAR_WORKPARTYMODE, CHAR_PARTY_CLIENT);
			CHAR_setWorkInt( index, CHAR_WORKPARTYINDEX1, masterindex);
		}
	}
	return masterindex;
}
#endif

BOOL NPC_ActionDuelRankCheck(int meindex,int talker,int rank,int flg)
{
	int myrank;
	myrank=CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC);
	if( NPC_ActionBigSmallLastCheck( rank, myrank, flg)==TRUE){
		return TRUE;
	}
	
	return FALSE;
}

BOOL NPC_ActionDuelPointCheck(int meindex,int talker,int rank,int flg)
{
	int myduel;
	myduel = CHAR_getInt( talker, CHAR_DUELPOINT);
	if( NPC_ActionBigSmallLastCheck( rank, myduel, flg)==TRUE){
		return TRUE;
	}
	
	return FALSE;
}

BOOL NPC_ActionTreasureTypeCheck(int meindex,int level, int type, int flg)
{
	int mylevel;

//	if( CHAR_getWorkInt( meindex, NPC_TIME_EVENTMODE) == NPC_EVENTMODE_EVENT ){
	mylevel = (type == NPC_EVENTMODE_EVENT)?1:0;
	if( NPC_ActionBigSmallLastCheck( level, mylevel, flg)==TRUE)
		return TRUE;
	return FALSE;
}

BOOL NPC_ActionTreasureRandItemGet(int meidex,int talker,int rand_j,char *buf)
{
	char buff2[64];
	int randitem;
	int ret;
	int itemindex;
	char token[128];

	if(rand_j == 0) {
		print("Event:由於０的介入，出现错误。");
 		return FALSE;
 	}
 	
	//randitem = rand()%rand_j;
	//srand((int)time(0));
	randitem = RAND(1,rand_j);
	if(randitem == 0) randitem = rand_j;

	getStringFromIndexWithDelim(buf , "," , randitem, buff2, sizeof(buff2)) ;

	itemindex = ITEM_makeItemAndRegist( atoi( buff2));

	if(itemindex == -1) return FALSE;
	
	/*失奶  丞及馨笛(  涛失奶  丞  卞中木化仄引丹  */
	ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
		print( "npc_exchangeman.c: additem error itemindex[%d]\n", itemindex);
		ITEM_endExistItemsOne( itemindex);
		return FALSE;
	}

	if(itemindex != -1) {
		LogItem(
			CHAR_getChar( talker, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
			itemindex,
#else
			ITEM_getInt( itemindex, ITEM_ID),  /* 失奶  丞  寞 */
#endif
			"EventAddItem(任务需求所得到的道具)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
 			CHAR_getInt( talker,CHAR_Y ),
            ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
	}
					
	sprintf(token,"收下了%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

	CHAR_sendItemDataOne( talker, ret);
	return TRUE;

}

#ifdef  _NPC_ADDLEVELUP				// (不可开) ANDY 外部测试机用来增加玩家等级
extern  tagRidePetTable ridePetTable[296];
void NPC_ActionLevelAndTransUp( int meindex, int charindex, int level, int skillpoint, int exp, int ridepet)
{
	char szBuffer[256]="";
	if( !CHAR_CHECKINDEX( charindex) )
		return;
//int CHAR_LevelUpCheck( int charaindex , int toindex)
	if( exp > 0 ){
		int LevelUp=0;
		int myexp = CHAR_getInt( charindex, CHAR_EXP);
		myexp += exp;
		if( myexp > 300000000 ) myexp = 300000000;
		CHAR_setInt( charindex, CHAR_EXP, myexp);
		snprintf( szBuffer, sizeof(szBuffer),
			"(%s) 得到 EXP %d", CHAR_getUseName( charindex ), exp);
		CHAR_talkToCli( charindex, -1, szBuffer, CHAR_COLORYELLOW);
		LevelUp = CHAR_LevelUpCheck( charindex , -1);
		if( LevelUp > 0 ){
			snprintf( szBuffer, sizeof(szBuffer),
				"(%s) 升级至 %d",
				CHAR_getUseName( charindex ),
				CHAR_getInt( charindex, CHAR_LV )
			);
			CHAR_talkToCli( charindex, -1, szBuffer, CHAR_COLORYELLOW);
			CHAR_setInt( charindex, CHAR_SKILLUPPOINT,
				CHAR_getInt( charindex, CHAR_SKILLUPPOINT) +  LevelUp*3 );
			CHAR_complianceParameter( charindex );
		}
	}

	if( level > 0 ){
		CHAR_setInt( charindex, CHAR_LV, level);
		snprintf( szBuffer, sizeof(szBuffer),
			"(%s) 等级设定为 %d",
			CHAR_getUseName( charindex ),	CHAR_getInt( charindex, CHAR_LV )
		);
		CHAR_talkToCli( charindex, -1, szBuffer, CHAR_COLORYELLOW);
	}

	if( skillpoint > 0 ){
		int MySp = CHAR_getInt( charindex, CHAR_SKILLUPPOINT);
		MySp += skillpoint;
		CHAR_setInt( charindex, CHAR_SKILLUPPOINT, MySp);
	}
	if( ridepet >= 0 ){
		int basepet[2][4]={	{2066,2067,2068,2069},
							{2072,2073,2074,2075}};
		int petTemp[]={-1,-1,-1,-1,-1};
		char msgbuf[256];
		int petNum=0;
		int i,j=0;
		int k=0, petindex,enemyarray;
		petTemp[j++] = basepet[0][rand()%4];
		petTemp[j++] = basepet[1][rand()%4];

		for( i=0; i< arraysizeof(ridePetTable) ; i++ ){
			if( CHAR_getInt( charindex, CHAR_BASEBASEIMAGENUMBER) == ridePetTable[i].charNo ){
				petTemp[j]= ridePetTable[i].petId;
				j++;
				if( j >= arraysizeof( petTemp) )
					break;
			}
		}
		j=0;
		for( petNum=0; petNum<arraysizeof( petTemp); petNum++)	{
			enemyarray = ENEMY_getEnemyArrayFromId( petTemp[ petNum]); //白虎
			petindex = ENEMY_createPetFromEnemyIndex( charindex, enemyarray);
			if( petindex == -1 ) {
				continue;
			}
			j++;
			for( k = 1; k < 120; k ++ ){	//升级
				CHAR_PetLevelUp( petindex );
				CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
				CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
			}
			CHAR_complianceParameter( petindex );
			CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );
			snprintf( szBuffer, sizeof(szBuffer), "得到%s", CHAR_getUseName( petindex) );
			CHAR_talkToCli( charindex, -1, szBuffer, CHAR_COLORYELLOW);
		}
		for(j = 0; j < CHAR_MAXPETHAVE; j++){
			petindex = CHAR_getCharPet(charindex, j);
			if( !CHAR_CHECKINDEX( petindex) )
				continue;
			CHAR_complianceParameter( petindex );
			snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
			CHAR_sendStatusString( charindex, msgbuf );
			snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
			CHAR_sendStatusString( charindex, msgbuf );
		}
		CHAR_setInt( charindex, CHAR_LEARNRIDE, ridepet);
	}
	CHAR_send_P_StatusString(  charindex, CHAR_P_STRING_DUELPOINT|
		CHAR_P_STRING_TRANSMIGRATION| CHAR_P_STRING_RIDEPET|
		CHAR_P_STRING_BASEBASEIMAGENUMBER| CHAR_P_STRING_GOLD|
		CHAR_P_STRING_EXP| CHAR_P_STRING_LV| CHAR_P_STRING_HP|CHAR_P_STRING_LEARNRIDE);
	CHAR_Skillupsend( charindex);
}
#endif


#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
// 还原点数
int PROFESSION_RESTORE_POINT( int charaindex )
{
	int vital, str, tough, dex, MySp;
	int sum	= 0, sum2=0, fd=-1;
	char msg[256];
	//andy_log 修先判断可能中断的条件可减少做不必要的动作
	if( !CHAR_CHECKINDEX( charaindex ) ) return -1;
	if( (fd = getfdFromCharaIndex(charaindex) ) == -1 ) return -1;

	vital = CHAR_getInt( charaindex, CHAR_VITAL );		// 体
	str   = CHAR_getInt( charaindex, CHAR_STR );		// 攻
	tough = CHAR_getInt( charaindex, CHAR_TOUGH );		// 防
	dex	  = CHAR_getInt( charaindex, CHAR_DEX );		// 敏
	MySp = CHAR_getInt( charaindex, CHAR_SKILLUPPOINT);
	sum = vital + str + tough + dex;
	if( sum < 0 )	return -1;

	//andy_reEdit 2003.06.25 19:30
	vital = 20 * (vital / sum);
	str   = 20 * (str   / sum);
	tough = 20 * (tough / sum);
	dex	  = 20 * (dex   / sum);

/*	大点视为浮点数 浮点数会误判 1950 + 50 = 2000 但转换後/100 = 19 + 0 = 19
	vital = 2000 * vital / sum;
	str   = 2000 * str   / sum;
	tough = 2000 * tough / sum;
	dex	  = 2000 * dex   / sum;
*/	
	sum2 = vital + str + tough + dex;
    if( sum2 < 20)	vital = vital + (20 - sum2);
	CHAR_setInt( charaindex, CHAR_VITAL, vital *100);	// 体
	CHAR_setInt( charaindex, CHAR_STR,	 str *100);		// 攻
	CHAR_setInt( charaindex, CHAR_TOUGH, tough *100);	// 防
	CHAR_setInt( charaindex, CHAR_DEX,   dex *100);		// 敏
	
    sum /= 100;
	MySp = (sum + CHAR_getInt( charaindex, CHAR_SKILLUPPOINT )) - 20;
	CHAR_setInt( charaindex, CHAR_SKILLUPPOINT, MySp);

	lssproto_SKUP_send( fd, MySp );

	snprintf( msg, sizeof(msg), "还原点数得到 %d", MySp );
	CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);	

	CHAR_complianceParameter( charaindex );
	CHAR_send_P_StatusString(  charaindex, 
		CHAR_P_STRING_DUELPOINT|
		CHAR_P_STRING_VITAL|
		CHAR_P_STRING_STR|
		CHAR_P_STRING_TOUGH|
		CHAR_P_STRING_DEX
	);

	return 1;
}
#endif

#ifdef _NPC_ADDWARPMAN1
BOOL NPC_ActionPartyCountCheck( int toindex, int nums, int flg)
{
	int partys=0;
	int i;

	partys = 0;
	for( i=0 ; i < CHAR_PARTYMAX ;i++)
		if(CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) != -1)
			partys++;

	if(NPC_ActionBigSmallLastCheck( nums, partys, flg) == TRUE)
		return TRUE;

	return FALSE;
}
//检查男生人数
BOOL NPC_ActionManCountCheck( int toindex, int nums, int flg)
{
	int i,tempcount = 0;
	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
	    if( IsMale(toindex) == 1 )
		    tempcount++;
	}
	else{
	    for( i=0 ; i < CHAR_PARTYMAX ;i++)
	        if(CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) != -1)
			    if( IsMale( CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) ) == 1 )
				    tempcount++;
	}

	print("\n检查男生人数:%d",tempcount);
	if(NPC_ActionBigSmallLastCheck( nums, tempcount, flg) == TRUE)
		return TRUE;

	return FALSE;
}
//检查女生人数
BOOL NPC_ActionWomanCountCheck( int toindex, int nums, int flg)
{
	int i,tempcount = 0;
	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
	    if( IsFemale(toindex) == 1 )
		    tempcount++;
	}
    else{
	    for( i=0 ; i < CHAR_PARTYMAX ;i++)
	        if(CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) != -1)
		        if( IsFemale( CHAR_getWorkInt(toindex,CHAR_WORKPARTYINDEX1+i) ) == 1 )
				    tempcount++;
	}
    print("\n检查女生人数:%d",tempcount);
	if(NPC_ActionBigSmallLastCheck( nums, tempcount, flg) == TRUE)
		return TRUE;

	return FALSE;
}
#endif


#ifdef _ANGEL_SUMMON
BOOL NPC_ActionMissionAngelCheck(int meindex,int talker,int mission,int flg,int checkitem)
{
	int mindex;
	char nameinfo[64];
	int i, itemindex, itemid;

	mindex = checkIfAngel( talker);
	if( mindex <= -1 ) return FALSE;
	if( missiontable[mindex].mission != mission)	return FALSE;
	getMissionNameInfo( talker, nameinfo);
	if( strcmp( nameinfo, missiontable[mindex].angelinfo ))	return FALSE;
	if( missiontable[mindex].flag != flg )	return FALSE;

	if( checkitem ) {	// 有无信物
		for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
			itemindex = CHAR_getItemIndex( talker ,i );
			if( ITEM_CHECKINDEX( itemindex) )	{
				itemid=ITEM_getInt(itemindex ,ITEM_ID );
				if( itemid == ANGELITEM ) {
					if( !strcmp( ITEM_getChar( itemindex, ITEM_ANGELINFO), missiontable[mindex].angelinfo) 
						&& !strcmp( ITEM_getChar( itemindex, ITEM_HEROINFO), missiontable[mindex].heroinfo) )
						break;
				}
			}
		}
		if( i >= CHAR_MAXITEMHAVE )
			return FALSE;
	}
	return TRUE;
}

BOOL NPC_ActionMissionHeroCheck(int meindex,int talker,int mission,int flg,int checkitem)
{
	int mindex;
	char nameinfo[64];
	int i, itemindex, itemid;

	mindex = checkIfAngel( talker);
	if( mindex <= -1 ) return FALSE;
	if( missiontable[mindex].mission != mission)	return FALSE;
	getMissionNameInfo( talker, nameinfo);
	if( strcmp( nameinfo, missiontable[mindex].heroinfo ))	return FALSE;
	if( missiontable[mindex].flag != flg )	return FALSE;

	if( checkitem ) {	// 有无信物
		for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
			itemindex = CHAR_getItemIndex( talker ,i );
			if( ITEM_CHECKINDEX( itemindex) )	{
				itemid=ITEM_getInt(itemindex ,ITEM_ID );
				if( itemid == HEROITEM ) {
					if( !strcmp( ITEM_getChar( itemindex, ITEM_ANGELINFO), missiontable[mindex].angelinfo) 
						&& !strcmp( ITEM_getChar( itemindex, ITEM_HEROINFO), missiontable[mindex].heroinfo) )
						break;
				}
			}
		}
		if( i >= CHAR_MAXITEMHAVE )
			return FALSE;
	}

	return TRUE;
}


BOOL NPC_HeroCompleteCountCheck(int meindex,int talker,int count,int flg)
{

	int herocnt;
	herocnt = CHAR_getInt(talker,CHAR_HEROCNT);

	if(NPC_EventBigSmallLastCheck( count, herocnt, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}



#endif


BOOL NPC_ActionCheckSkup(int meindex,int talker,int skup,int flg)
{
	int mySkup;
	mySkup=CHAR_getInt(talker, CHAR_SKILLUPPOINT);
	if( NPC_ActionBigSmallLastCheck( skup, mySkup, flg ) == TRUE )	{
             return TRUE;
        }                   
	return FALSE;
}

BOOL NPC_ActionCheckSafe(int meindex,int talker,int safe,int flg)
{
	int mySafe;
	mySafe=CHAR_getWorkInt(talker, CHAR_WORKSAFEMODE);
	if( NPC_ActionBigSmallLastCheck( safe, mySafe, flg ) == TRUE )	{
             return TRUE;
        }                   
	return FALSE;
}

BOOL NPC_ActionWarpNpc(int meindex,char *buf)
{
	char token[32];
	char token2[32];
	char buf2[32];
	int floor,x,y,x1,x2,y1,y2;
	int ret;
	int cnt;
	int dcnt = 1;

	/*--蜇箕及伐□皿及赐  --*/
	cnt = CHAR_getWorkInt(meindex,CHAR_NPCWORKINT4);
	while(getStringFromIndexWithDelim(buf,",", dcnt,buf2,sizeof(buf2) )
	 !=FALSE ){
		dcnt++;
	}
	//cnt = RAND(1,dcnt);
	if(dcnt <= cnt) cnt=1;


	while(getStringFromIndexWithDelim(buf,",", cnt,buf2,sizeof(buf2)) !=FALSE )
	{
		cnt++;
	    ret=getStringFromIndexWithDelim(buf2,".", 1,token,sizeof(token));
        if( ret == FALSE ) {
        	 continue;
       	}
        floor = atoi( token );

        ret=getStringFromIndexWithDelim(buf2,".", 2,token,sizeof(token));
        if( ret == FALSE ){
          continue;
        }
         ret=getStringFromIndexWithDelim(token,"-", 1,token2,sizeof(token2));
        if( ret == FALSE ){
          x = atoi( token );
        }else{
        	x1=atoi(token2);
        	ret=getStringFromIndexWithDelim(token,"-", 2,token2,sizeof(token2));
        	if( ret == FALSE ){
        		x2=x1;
        	}else{
        		x2=atoi(token2);
        	}
        	x = RAND(x1,x2);
        }
        //x = atoi( token );

        ret=getStringFromIndexWithDelim(buf2,".", 3,token,sizeof(token));
        if( ret == FALSE ) {
         continue;
        }
        ret=getStringFromIndexWithDelim(token,"-", 1,token2,sizeof(token2));
        if( ret == FALSE ){
          y = atoi( token );
        }else{
        	y1=atoi(token2);
        	ret=getStringFromIndexWithDelim(token,"-", 2,token2,sizeof(token2));
        	if( ret == FALSE ){
        		y2=y1;
        	}else{
        		y2=atoi(token2);
        	}
        	y = RAND(y1,y2);
        }
        //y = atoi( token );
		//print("\nfloor=%d,x=%d,y=%d\n",floor,x,y);
		CHAR_setWorkInt(meindex,CHAR_NPCWORKINT4, cnt);
        CHAR_warpToSpecificPoint(meindex, floor, x, y);
		return TRUE;
	}
	
	return FALSE;

}

int NPC_ActionTime(void)
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep); /*取得当地时间*/
	year=1900+p->tm_year;
	mon=1+p->tm_mon;
	date=p->tm_mday;
	day=p->tm_wday;
	hour=p->tm_hour;
	min=p->tm_min;
	sec=p->tm_sec;
}
