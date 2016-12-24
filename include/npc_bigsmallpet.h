0034:npc_blackmarket.h:00:1:14=48041e4b:16=47d21313:0032:npc_bodylan.h:0166:1:14=48041e4b:16=47d21313:#ifndef __NPC_BODYLAN_H__
#define __NPC_BODYLAN_H__

void NPC_BodyLanTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_BodyLanInit( int meindex );
void NPC_BodyLanWatch( int meindex , int lookedindex);
void NPC_BodyLanWindowTalked(
	int meindex,
	int talkerindex,
	int seqno,
	int select,
	char *data
);

#endif 
002E:npc_bus.h:016E:1:14=48041e4b:16=47d21313:#ifndef __NPC_BUS_H__
#define __NPC_BUS_H__

BOOL NPC_BusInit( int meindex );
void NPC_BusLoop( int meindex );
void NPC_BusTalked( int meindex , int talkerindex , char *szMes ,
                     int color );
BOOL NPC_BusCheckAllowItem( int meindex, int charaindex, BOOL pickupmode);
BOOL NPC_BusCheckJoinParty( int meindex, int charaindex, BOOL msgflg);

#endif

0030:npc_charm.h:0156:1:14=48041e4b:16=47d21313:#ifndef __NPC_CHARM_H__
#define __NPC_CHARM_H__

void NPC_CharmTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_CharmInit( int meindex );
void NPC_CharmWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_CharmLooked( int meindex , int lookedindex);

#endif 

0033:npc_checkman.h:0167:1:14=48041e4b:16=47d21313:#ifndef __NPC_CHECKMAN_H__
#define __NPC_CHECKMAN_H__

void NPC_CheckManTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_CheckManInit( int meindex );
void NPC_CheckManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_CheckManLooked( int meindex , int lookedindex);

#endif

0031:npc_Dengon.h:0146:1:14=48041e4b:16=47d21313:#ifndef __NPC_DENGON_H__
#define __NPC_DENGON_H__
BOOL NPC_DengonInit( int meindex );
//void NPC_DengonSpecialTalked( int index, int talker, char *msg, int color );
void NPC_DengonLooked( int meindex, int lookedindex );
void NPC_DengonWindowTalked( int index, int talker, 
								int seqno, int select, char *data);

#endif

002F:npc_door.h:0470:1:14=48041e4b:16=47d21313:#ifndef __NPC_DOOR_H__
#define __NPC_DOOR_H__

BOOL NPC_DoorInit(int meindex );
void NPC_DoorTalked( int meindex , int talkerindex , char *msg , int color );
void NPC_DoorPreAttacked(int meindex , int attacker , float *dm);
void NPC_DoorPostOver( int meindex , int movedindex );
void NPC_DoorOff( int meindex , int movedindex );
void NPC_DoorWatch( int meindex, int moveindex, CHAR_ACTION act,
                           int x, int y, int dir, int* opt, int optlen );
int NPC_DoorSearchByName( char *name );
int NPC_DoorSearchByPosition( int fl , int x, int y );
void NPC_DoorLooked( int meindex , int lookedindex );
BOOL NPC_DoorRefreshOverFlg( int meind );


/* 职及NPC井日丑综今木月迕［NPC棉皿夫玄戊伙卞勾中化反 npc_stepswitch.c毛心欠 */
int NPC_DoorSetPasswd( int meindex, char *pas );
int NPC_DoorGetPasswd( int meindex, char *out , int len );
void NPC_DoorFlip(int meindex , int doindex);
void NPC_DoorClose(int meindex , int doindex);
void NPC_DoorOpen(int meindex , int doindex );
void NPC_DoorAddSwitchCounter( int meindex , int i , int talker );
void NPC_DoorSetSwitchCounter( int meindex, int i , int doindex );

#endif





0031:npc_doorman.h:0CA:1:14=48041e4b:16=47d21313:#ifndef __NPC_DOORMAN_H__
#define __NPC_DOORMAN_H__

BOOL NPC_DoormanInit( int meindex );
void NPC_DoormanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

#endif 

0036:npc_duelranking.h:01AD:1:14=48041e4b:16=47d21313:#ifndef __NPC_DUELRANKING_H__
#define __NPC_DUELRANKING_H__

BOOL NPC_DuelrankingInit( int meindex );
void NPC_DuelrankingLooked( int meindex, int lookedindex );
void NPC_DuelrankingWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_Duelranking_PrintRanking( char *data, int count_start, int fdid, int msgid2);
void NPC_Duelranking_CallMyRanking( int count, int msgid, int msgid2);

#endif

0037:npc_eventaction.h:01505:1:14=48041e4b:16=47d21313:#ifndef _PRO_NPCFIX_H
#define _PRO_NPCFIX_H
//ANDY_Edit
BOOL ActionNpc_CheckMenuFree( int meindex, int toindex, char *npcarg, int nums);
BOOL ActionNpc_CheckFree( int meindex, int toindex, char *npcarg);
BOOL Action_RunDoEventAction( int meindex, int toindex, char *buf1);
BOOL ActionNpc_WarpPoint(int meindex,int talker,char *npcarg);
BOOL Action_PartyCheck( int meindex, int talker);
BOOL NPC_ActionAddGold( int talker, int Golds);
BOOL NPC_ActionDelGold( int talker,char *buf);
BOOL NPC_ActionDelFame( int talker,char *buf);
BOOL NPC_ActionAddFame( int talker,char *buf);
#ifdef _MOMENTUM_NPC
BOOL NPC_ActionMomentum( int talker,char *buf);
#endif
#ifdef _DP_NPC
BOOL NPC_ActionDP( int talker,char *buf);
#endif
#ifdef _VIP_SHOP
BOOL NPC_ActionDelVipPoint( int talker,char *buf);
#endif
#ifdef _TRANS_7_NPC
BOOL NPC_ActionTrans7( int talker, char *buf);
BOOL NPC_ActionTransNew7( int talker, char *buf);
BOOL NPC_ActionTransNewSeven( int talker, char *buf);
#endif
// WON ADD
BOOL NPC_ActionAddGold2( int talker,char *buf);
BOOL NPC_ActionDelPet( int toindex,char *buf);
#ifdef _NPC_NewDelPet
BOOL NPC_ActionNewDelPet( int talker,char *msg);
#endif
BOOL NPC_ActionAddPet( int talker, char *buf);
BOOL NPC_ActionDelItem( int talker,char *buf);
BOOL NPC_ActionAddItem( int talker, char *buf);
BOOL NPC_ActionSetEend( int talkerindex, char * buf);
BOOL NPC_ActionSetNow( int toindex, char * buf);
BOOL NPC_ActionClearEvent( int toindex, char * buf);

BOOL NPC_ActionCheckTrans(int meindex,int talker,int trans,int flg);
#ifdef _VIP_SHOP
BOOL NPC_ActionVipPointCheck(int meindex,int talker,int point,int flg);
#endif
#ifdef _VIP_RIDE
BOOL NPC_ActionVipCheck(int meindex,int talker,int vip,int flg);
#endif
#ifdef _VIP_SHOP
BOOL NPC_ActionNewVipPointCheck(int meindex,int talker,int point,int flg);
#endif
#ifdef _MOMENTUM_NPC
BOOL NPC_ActionMomentumCheck(int meindex,int talker,int Momentum,int flg);
#endif
#ifdef _DP_NPC
BOOL NPC_ActionDPCheck(int meindex,int talker,int DP,int flg);
#endif
BOOL NPC_ActionLevelCheck(int meindex,int talker,int level,int flg);

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
BOOL NPC_ActionClassCheck( int meindex,int talker,int p_class,int flg);

BOOL NPC_ActionSkillCheck( int meindex,int talker,int skillId,int flg);
BOOL NPC_ActionSkillNumCheck( int meindex,int talker,int sknum,int flg);

BOOL NPC_ActionSkillPointAndSkillCheck( int meindex,int talker,int sknum,int flg);

BOOL NPC_ActionCheckSkup(int meindex,int talker,int skup,int flg);
BOOL NPC_ActionCheckSafe(int meindex,int talker,int safe,int flg);

BOOL NPC_ActionCleanProfession( int toindex, char *buf);
BOOL NPC_ActionAddProfessionSkillPoint( int toindex, char *buf );
#endif

BOOL NPC_ActionGoldCheck(int meindex,int talker,int Golds,int flg);
BOOL NPC_ActionPartyCheck( int toindex, int nums, int flg);
BOOL NPC_ActionReItemCheck( int toindex, int nums, int flg);
BOOL NPC_ActionRePetCheck( int toindex, int nums, int flg);
BOOL ActionCheckMyPet( int meindex, int talker, int petLv, int flg, int petid);
BOOL NPC_ActionBigSmallLastCheck(int point1,int mypoint,int flg);

BOOL NPC_ActionItemCheck(int meindex,int talker,int itemNo, int flg);
BOOL NPC_ActioneEquitCheck(int meindex,int talker,int itemNo, int flg);

BOOL NPC_ActionFreeIfCheck(int meindex,int talker, char* buf, char *opt, int kosuu,int flg, int temp);

BOOL NPC_ActionBigSmallCheck(int meindex,int talker,char* buf);
BOOL NPC_ActionPassCheck( int meindex, int talker, char *buf);
BOOL NPC_ActionWarpManReduce(int meindex,int talker,char *buf);
#ifdef _NPC_ActionFreeCmp 
BOOL NPC_ActionWarpManReduce2(int meindex,int talker,char *buf);
#endif
#ifdef _NPCCHANGE_PLAYERIMG	//andy
BOOL NPC_ActionChangePlayerBBI( int meindex, int charindex, char *Img);
BOOL NPC_CheckPlayerBBI( int meindex, int charindex, int BBI, int flg);
#endif

BOOL showString( int meindex, char *showstr,int flg);
BOOL NPC_getTimeXYPoint( int meindex, int Mode);
BOOL NPC_getTimeData( int meindex, char *npcarg, int *born, int *dead, int loopspeed);
void NPC_TimeDefineDo( int meindex, int born, int dead, int typeMode);
BOOL NPC_CheckTimeDefine( char *timetype);

void GetRand_WarpPoint( char *buf, int *fl, int *x, int *y );
#ifdef _EMENY_CHANCEMAN
int NPC_EmenyChanceCheck( int meindex, int charaindex, char *arg);
int NPC_DischargePartyForSex( int meindex, int charaindex, int fl, int x, int y, int fsex);
#endif

BOOL NPC_ActionDuelRankCheck(int meindex,int talker,int rank,int flg);
BOOL NPC_ActionDuelPointCheck(int meindex,int talker,int rank,int flg);

#ifdef _TREASURE_BOX
BOOL NPC_ActionTreasureTypeCheck( int meindex, int level, int type, int flg);
#endif
BOOL NPC_ActionTreasureRandItemGet(int meidex,int talker,int rand_j,char *buf);
void NPC_ActionLevelAndTransUp( int meindex, int charindex, int level,
							   int skillpoint, int exp, int ridepet);
#ifdef _ITEM_PILENUMS
BOOL NPC_ActionDoPileDelItem( int toindex, int itemID, int num);
BOOL NPC_DelItem( int toindex, int ti, int num);
#endif
void NPC_ActionDoPileClearItem( int toindex, int itemID);


#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
int PROFESSION_RESTORE_POINT( int charaindex );
#endif


#ifdef _NPC_ADDWARPMAN1
BOOL NPC_ActionPartyCountCheck( int toindex, int nums, int flg);
BOOL NPC_ActionManCountCheck( int toindex, int nums, int flg);//检查男生人数
BOOL NPC_ActionWomanCountCheck( int toindex, int nums, int flg);//检查女生人数
#endif

#endif
0036:npc_exchangeman.h:0235:1:14=48041e4b:16=47d21313:#ifndef __NPC_EXCHANGEMAN_H__
#define __NPC_EXCHANGEMAN_H__

void NPC_ExChangeManTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_ExChangeManInit( int meindex );
void NPC_ExChangeManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);



BOOL NPC_EventCheckFlg( int talker, int shiftbit);
BOOL NPC_NowEventCheckFlg( int talker, int shiftbit);

// Arminius 8.14 move from .c (for pet talk)
BOOL NPC_EventBigSmallCheck(int meindex,int talker,char* buf);
	
#endif


 /*__NPC_EXCHANGEMAN_H__*/


0034:npc_familyman.h:0187:1:14=48041e4b:16=47d21313:#ifndef __NPC_FAMILYMAN_H__
#define __NPC_FAMILYMAN_H__

void NPC_FamilymanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_FamilymanInit( int meindex );
void NPC_FamilymanLooked( int meindex, int lookedindex );
void NPC_FamilymanWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

/*__NPC_FAMILYMAN_H__*/
0033:npc_fmdengon.h:0111:1:14=48041e4b:16=47d21313:#ifndef __NPC_FMDENGON_H__
#define __NPC_FMDENGON_H__

BOOL NPC_FmDengonInit( int meindex );
void NPC_FmDengonLooked( int meindex, int lookedindex );
void NPC_FmDengonWindowTalked( int index, int talker, int seqno, int select, char *data);

#endif 

/*__NPC_FMDENGON_H__*/
0032:npc_fmhealer.h:0E4:1:14=48041e4b:16=47d21313:#ifndef __NPC_FMHEALER_H__
#define __NPC_FMHEALER_H__

void NPC_FmHealerTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_FmHealerInit( int meindex );

#endif 
/*__NPC_FMHEALER_H__*/
0033:npc_fmletter.h:0180:1:14=48041e4b:16=47d21313:#ifndef __NPC_FMLETTER_H__
#define __NPC_FMLETTER_H__

void NPC_FmLetterTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_FmLetterInit( int meindex );
void NPC_FmLetterLooked( int meindex, int lookedindex );
void NPC_FmLetterWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

/*__NPC_FMLETTER_H__*/
0036:npc_fmpkcallman.h:013B:1:14=48041e4b:16=47d21313:#ifndef __NPC_FMPKCALLMAN_H__
#define __NPC_FMPKCALLMAN_H__

void NPC_FMPKCallManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_FMPKCallManInit(int meindex);
void NPC_FMPKCallManWindowTalked(int meindex, int talkerindex, int seqno,
	int select, char *data);

#endif 
/*__NPC_FMPKCALLMAN_H__*/
0032:npc_fmpkman.h:0123:1:14=48041e4b:16=47d21313:#ifndef __NPC_FMPKMAN_H__
#define __NPC_FMPKMAN_H__

void NPC_FMPKManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_FMPKManInit(int meindex);
void NPC_FMPKManWindowTalked(int meindex, int talkerindex, int seqno,
	int select, char *data);

#endif 
/*__NPC_FMPKMAN_H__*/
0034:npc_fmwarpman.h:01A6:1:14=48041e4b:16=47d21313:#ifndef __NPC_FMWARPMAN_H__
#define __NPC_FMWARPMAN_H__

void NPC_FMWarpManTalked(int meindex, int talkerindex, char *msg, int color);
BOOL NPC_FMWarpManInit(int meindex);
void NPC_FMWarpManWindowTalked(int meindex, int talkerindex, int seqno,
	int select, char *data);
void NPC_FMWarpManLoop(int meindex);
void NPC_GetPKFMNum(int floor, int fmindex1, int fmindex2, int *num1, int *num2);

#endif 
/*__NPC_FMWARPMAN_H__*/
003B:npc_freepetskillshop.h:01C4:1:14=48041e4b:16=47d21313: