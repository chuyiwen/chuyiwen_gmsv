#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_autopk.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "npc_healer.h"
#include "configfile.h"

#ifdef _GMRELOAD
	extern struct GMINFO gminfo[GMMAXNUM];
#endif
#ifdef _JOB_AUTOPK
	extern int jobautopk;
#endif
#ifdef _AUTO_PK

#define AWARDNO			  (1 << 0)
#define AWARDPET		  (1 << 1)
#define AWARDITEM    	(1 << 2)
#define AWARDALL    	(AWARDPET|AWARDITEM)

enum {
	WINDOW_START=1,
	WINDOW_SELECT,
	WINDOW_JOINPK,
	WINDOW_PKLIST,
	WINDOW_PKINFO,
	WINDOW_HISTORY,
	WINDOW_HISTORY_NEXT,
	WINDOW_HISTORY_PREV,
	WINDOW_END,
};

typedef struct tagautopk
{
	int winnum;
	char cdkey[CDKEYLEN];
	char name[CHARNAMELEN];
}Autopk;

typedef struct tagaward
{
	int awardpetid;
	int awarditemid;
  char awardpet[32];
  char awarditem[32];
	int draw;
}Award;

static int baward = FALSE;

Award award[5];

Autopk ship[5];

int autopklist;
static int autopktime=-1;
static int awardpetid=-1;
static int awarditemid=-1;
static char awardpet[32];
static char awarditem[32];
int autopkstart=0;
int autopkcnt=0;
int autopktype = 0;
enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
	NPC_WORK_TIMEFLAG = CHAR_NPCWORKINT9,
/*
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
*/
};
enum {
	NPC_AUTOPK_START,
	NPC_AUTOPK_SELECT,
	NPC_AUTOPK_JOINPK,
	NPC_AUTOPK_PKLIST,
	NPC_AUTOPK_PKAWARD,
	NPC_AUTOPK_HISTORY,
	NPC_AUTOPK_OK,
	NPC_AUTOPK_END,
};
#define STANDBYTIME 50

static void NPC_AutoPk_selectWindow( int meindex, int toindex, int num,int select);
BOOL AutoPk_GetHistoryStr( char *token, int page );
void AutoPk_PKNum( char *token );
BOOL AutoPk_AddItem( int charindex, int index );
BOOL AutoPk_AddPet( int charindex, int index );

BOOL NPC_AutoPkInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

//  	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TRANSERMANS);

	//CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	award[0].draw=0;
	award[1].draw=0;
	award[2].draw=0;
	award[3].draw=0;
	award[4].draw=0;

	return TRUE;
}
//CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,1);
void NPC_AutoPkTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}

	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_AutoPk_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

void NPC_AutoPkWindowTalked( int meindex, int talkerindex, int seqno,
								int select, char *data)
{
	char jiangbuf[256];
	memset(jiangbuf, 0, sizeof(jiangbuf));
	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
		return;
	switch( seqno)	{
	case NPC_AUTOPK_START:
		break;
	case NPC_AUTOPK_SELECT:
		NPC_AutoPk_selectWindow( meindex, talkerindex, WINDOW_SELECT, atoi( data));
		break;
	case NPC_AUTOPK_JOINPK:
		if(autopktime>2
#ifdef _JOB_AUTOPK
				&& CHAR_getInt( talkerindex, PROFESSION_CLASS ) == jobautopk
#endif
			){
			char buf1[128];
			char buf2[255];
			int i;
			if(CHAR_getInt(talkerindex,CHAR_LV)<getAutoPkMinLv()){
				sprintf(buf2,"您的等级低于%d，无法参加比赛。",getAutoPkMinLv());
				CHAR_talkToCli( talkerindex, -1, buf2, CHAR_COLORRED );
				return;
			}
			CHAR_DischargePartyNoMsg( talkerindex);//解散团队
			CHAR_warpToSpecificPoint( talkerindex, 20000, 15, 14 );
#ifdef _GMRELOAD
			for (i = 0; i < GMMAXNUM; i++){
				if (strcmp( CHAR_getChar( talkerindex, CHAR_CDKEY), gminfo[i].cdkey) == 0){
					CHAR_setInt(talkerindex,CHAR_AUTOPK,-1);
					break;
				}
			}
			if(i == GMMAXNUM)
#endif
			{
				CHAR_setInt(talkerindex,CHAR_AUTOPK,0);
#ifdef	_YUANGUSA
				sprintf(buf1,"[比赛快讯]：玩家「%s」踏空而来，各位选手全力应战哦。",CHAR_getChar( talkerindex, CHAR_NAME));
				sprintf(buf2,"[赛前风云]：玩家「%s」参加乱舞PK活动，比赛将在%d分钟后开始。",CHAR_getChar( talkerindex, CHAR_NAME),autopktime);
#else				
				sprintf(buf1,"玩家%s参加比赛！",CHAR_getChar( talkerindex, CHAR_NAME));
				sprintf(buf2,"玩家%s参加比赛,你还没参加吗？本次奖品丰富，你还等什么？快来吧！",CHAR_getChar( talkerindex, CHAR_NAME));
#endif				
				AutoPk_PKSystemTalk(buf1,buf2);
				sprintf(buf2,"[参与方式]：玛丽娜斯渔村[60.90]，要参加请尽快入场哦~~");
				int ii = 0;
				for(ii=0;ii<CHAR_getPlayerMaxNum();ii++){
					if(CHAR_CHECKINDEX(ii) == FALSE) continue;
					if(CHAR_getInt(ii,CHAR_FLOOR)!=20000)
						CHAR_talkToCli( ii, -1, buf2, CHAR_COLORRED );
				}
			}
		}
		break;
	case NPC_AUTOPK_PKLIST:
		NPC_AutoPk_selectWindow( meindex, talkerindex, WINDOW_PKLIST, atoi( data));
		break;
	case NPC_AUTOPK_PKAWARD:
		if(autopktype == 1) return;
		if(!strcmp(ship[0].cdkey, CHAR_getChar( talkerindex, CHAR_CDKEY))
					&& !strcmp(ship[0].name, CHAR_getChar( talkerindex, CHAR_NAME))){
//			if((award[0].draw&AWARDPET)!=AWARDPET){
//				if(AutoPk_AddPet( talkerindex, 0 )){
//					award[0].draw=award[0].draw|AWARDPET;
//				}
//			}

			if((award[0].draw)!=1){
				if(AutoPk_AddItem( talkerindex, 0 )){
					award[0].draw=1;
					sprintf(jiangbuf,"[布告天下]：本界乱舞冠军[%s]已领取奖品[%s]，并名留乱舞榜。",CHAR_getChar(talkerindex,CHAR_NAME),award[0].awarditem);
				}
			}
		}else if(!strcmp(ship[1].cdkey, CHAR_getChar( talkerindex, CHAR_CDKEY))
					&& !strcmp(ship[1].name, CHAR_getChar( talkerindex, CHAR_NAME))){
//			if((award[1].draw&AWARDPET)!=AWARDPET){
//				if(AutoPk_AddPet( talkerindex, 1 )){
//					award[1].draw=award[1].draw|AWARDPET;
//				}z
//			}
			if((award[1].draw)!=1){
				if(AutoPk_AddItem( talkerindex, 1 )){
					award[1].draw=1;
					sprintf(jiangbuf,"[布告天下]：本界乱舞亚军[%s]已领取奖品[%s]，并名留乱舞榜。",CHAR_getChar(talkerindex,CHAR_NAME),award[1].awarditem);
				}
			}
		}else if(!strcmp(ship[2].cdkey, CHAR_getChar( talkerindex, CHAR_CDKEY))
					&& !strcmp(ship[2].name, CHAR_getChar( talkerindex, CHAR_NAME))){
//			if((award[2].draw&AWARDPET)!=AWARDPET){
//				if(AutoPk_AddPet( talkerindex, 2 )){
//					award[2].draw=award[2].draw|AWARDPET;
//				}
//			}
			if((award[2].draw)!=1){
				if(AutoPk_AddItem( talkerindex, 2 )){
					award[2].draw=1;
					sprintf(jiangbuf,"[布告天下]：本界乱舞季军[%s]已领取奖品[%s]，并名留乱舞榜。",CHAR_getChar(talkerindex,CHAR_NAME),award[2].awarditem);
				}
			}
		}
		
		if(strlen(jiangbuf)>0){
			AutoPk_PKSystemTalk(jiangbuf,jiangbuf);
		}
//		if(!strcmp(ship[3].cdkey, CHAR_getChar( talkerindex, CHAR_CDKEY))
//					&& !strcmp(ship[3].name, CHAR_getChar( talkerindex, CHAR_NAME))){
//			if((award[3].draw&AWARDPET)!=AWARDPET)
//				if(AutoPk_AddPet( talkerindex, 3 ))
//					award[3].draw=award[3].draw|AWARDPET;
//			if((award[3].draw&AWARDITEM)!=AWARDITEM)
//				if(AutoPk_AddItem( talkerindex, 3 ))
//					award[3].draw=award[3].draw|AWARDITEM;
//		}else if(!strcmp(ship[4].cdkey, CHAR_getChar( talkerindex, CHAR_CDKEY))
//					&& !strcmp(ship[4].name, CHAR_getChar( talkerindex, CHAR_NAME))){
//			if((award[4].draw&AWARDPET)!=AWARDPET)
//				if(AutoPk_AddPet( talkerindex, 4 ))
//					award[4].draw=award[4].draw|AWARDPET;
//			if((award[4].draw&AWARDITEM)!=AWARDITEM)
//				if(AutoPk_AddItem( talkerindex, 4 ))
//					award[4].draw=award[4].draw|AWARDITEM;
//		}
		break;
	case NPC_AUTOPK_HISTORY:
		if( select == WINDOW_BUTTONTYPE_CANCEL )
			return;
		if( select == WINDOW_BUTTONTYPE_NEXT )
		{
				NPC_AutoPk_selectWindow( meindex, talkerindex, WINDOW_HISTORY_NEXT, atoi( data));
				return;
		}
		if( select == WINDOW_BUTTONTYPE_PREV )
		{
				NPC_AutoPk_selectWindow( meindex, talkerindex, WINDOW_HISTORY_PREV, atoi( data));
				return;
		}
		break;
	case NPC_AUTOPK_OK:
		break;
	case NPC_AUTOPK_END:
		NPC_AutoPk_selectWindow( meindex, talkerindex, WINDOW_END, atoi( data));
		break;
	}
}

static void NPC_AutoPk_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return;
	}
	memset( token, 0, sizeof( token));

	switch( num)	{
	case WINDOW_START:
		sprintf(token, "3\n　　　　　　★乱舞格斗ＰＫ赛管理员★\n\n"
											"　　　　　∨有什么需要我为您服务的呢∨\n"
											"　　　　　　　　《我要参加比赛》\n"
											"　　　　　　　　《查看场内选手》\n"
											"　　　　　　　　《领取比赛奖品》\n"
											"　　　　　　　　《历史比赛记录》\n"
											"　　　　　　　　《场外观看比赛》");
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_AUTOPK_SELECT;
		break;
	case WINDOW_SELECT:
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		if(select==1){
			if(autopktime>2){
				sprintf(token, "            ★乱舞格斗ＰＫ赛管理员★\n\n"
				               "当前离比赛开始时间还剩%d分钟，请做好一切准备。",autopktime);
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowno = NPC_AUTOPK_JOINPK;
			}else if(autopktime>0){
				sprintf(token, "　　　　　　★乱舞格斗ＰＫ赛管理员★\n\n"
				               "PK比赛将在%d分钟后开始!\n现在已无法进入比赛场地！\n\nPS：比赛最后两分钟无法进入场地，入场请提早。",autopktime);
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
			}else{
				sprintf(token, "            ★乱舞格斗ＰＫ赛管理员★\n\n"
				               "　　　　　　　目前还未举办ＰＫ比赛\n　　　　　　 请关注ＰＫ比赛最新消息\n　　　　　　 更多活动请关注官方论坛\n　　　　　　　◆ www.waei.cc ◆\n\n　　　　　　 PS：最后两分钟无法入场");
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
			}
		}else if(select==2){
			sprintf(token, "　　　　　　★乱舞格斗ＰＫ赛参赛名单★\n");
			strcat(token, "　「转生」　「等级」　　「玩家」\n");
			AutoPk_PKNum(token);
			buttontype = WINDOW_BUTTONTYPE_CANCEL;
			windowno = NPC_AUTOPK_PKLIST;
		}else if(select==3){
			AutoPk_GetAwardStr();
			char buff[64];
			sprintf(token, "　　　　　　★乱舞格斗ＰＫ赛奖品领取★\n　　　　　　获胜者在比赛后立刻领取奖励\n\n");;
//			if(strlen(award[0].awardpet)>0)
//				sprintf(buff, "　冠军　宠物：%s\n", award[0].awardpet);
//			else
//				sprintf(buff, "　冠军　宠物：无\n");
//			strcat(token, buff);
			if(strlen(award[0].awarditem)>0)
				sprintf(buff, "　　　　　「 冠 军 」　%s\n", award[0].awarditem);
			else
				sprintf(buff, "　　　　　「 冠 军 」　暂无\n");
			strcat(token, buff);
//			if(strlen(award[1].awardpet)>0)
//				sprintf(buff, "　亚军　宠物：%s\n", award[1].awardpet);
//			else
//				sprintf(buff, "　亚军　宠物：无\n");
//			strcat(token, buff);
			if(strlen(award[1].awarditem)>0)
				sprintf(buff, "　　　　　「 亚 军 」　%s\n", award[1].awarditem);
			else
				sprintf(buff, "　　　　　「 亚 军 」　暂无\n");
			strcat(token, buff);
//			if(strlen(award[2].awardpet)>0)
//				sprintf(buff, "　季军　宠物：%s\n", award[2].awardpet);
//			else
//				sprintf(buff, "　季军　宠物：无\n");
//			strcat(token, buff);
			if(strlen(award[2].awarditem)>0)
				sprintf(buff, "　　　　　「 季 军 」　%s\n", award[2].awarditem);
			else
				sprintf(buff, "　　　　　「 季 军 」　暂无\n");
			strcat(token, buff);
			strcat(token,"\n　比赛名次按击杀点数计算，杀敌要迅速哦！");
//			if(strlen(award[3].awardpet)>0)
//				sprintf(buff, "　优胜　宠物：%s\n", award[3].awardpet);
//			else
//				sprintf(buff, "　优胜　宠物：无\n");
//			strcat(token, buff);
//			if(strlen(award[3].awarditem)>0)
//				sprintf(buff, "　优胜　物品：%s\n", award[3].awarditem);
//			else
//				sprintf(buff, "　优胜　物品：无\n");
//			strcat(token, buff);
//			if(strlen(award[4].awardpet)>0)
//				sprintf(buff, "　优胜　宠物：%s\n", award[4].awardpet);
//			else
//				sprintf(buff, "　优胜　宠物：无\n");
//			strcat(token, buff);
//			if(strlen(award[4].awarditem)>0)
//				sprintf(buff, "　优胜　物品：%s\n", award[4].awarditem);
//			else
//				sprintf(buff, "　优胜　物品：无\n");
//			strcat(token, buff);
			if(baward && (!strcmp(ship[0].cdkey, CHAR_getChar( toindex, CHAR_CDKEY))
								&& !strcmp(ship[0].name, CHAR_getChar( toindex, CHAR_NAME))
								&& (award[0].draw&AWARDITEM)!=AWARDITEM)
								|| (!strcmp(ship[1].cdkey, CHAR_getChar( toindex, CHAR_CDKEY))
								&& !strcmp(ship[1].name, CHAR_getChar( toindex, CHAR_NAME))
								&& (award[1].draw&AWARDITEM)!=AWARDITEM)
								|| (!strcmp(ship[2].cdkey, CHAR_getChar( toindex, CHAR_CDKEY))
								&& !strcmp(ship[2].name, CHAR_getChar( toindex, CHAR_NAME))
								&& (award[2].draw&AWARDITEM)!=AWARDITEM) && autopktype == 0)
					buttontype = WINDOW_BUTTONTYPE_YESNO;
			else
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
			windowno = NPC_AUTOPK_PKAWARD;
		}else if(select==4){
			CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE, 1);
			if(AutoPk_GetHistoryStr(token,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)))
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
			else
				buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
				windowno = NPC_AUTOPK_HISTORY;
		}else if(select==5){
			CHAR_talkToCli( toindex, -1, "[友情提示]：比赛开始后，请到渔村医院<远程观战员>处，查看比赛哦。", CHAR_COLORRED );
			return;
		}
		break;
	case WINDOW_HISTORY_NEXT:
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)+1);
		if(AutoPk_GetHistoryStr(token,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)))
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_PREV;
		else
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
		windowno = NPC_AUTOPK_HISTORY;
		break;
	case WINDOW_HISTORY_PREV:
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)-1);
		if(CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)<1)CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE,1);
		if(AutoPk_GetHistoryStr(token,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)) || CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)==1)
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
		else
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
		windowno = NPC_AUTOPK_HISTORY;
		break;
	case WINDOW_END:
		return;
		break;
	}
	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}


void NPC_AutoPkLoop( void )
{
}

BOOL AutoPk_GetHistoryStr( char *token, int page )
{
	FILE* fp;
	fp = fopen("./data/autopk/autopkhistory.txt", "r");
	if (fp == NULL)
	{
		print("查看比赛历史文件打开错误\n");
		return FALSE;
	}
	typedef struct taghistory
	{
		char winnum[5];
		char name[CHARNAMELEN];
	}History;
	History history[5];
	
	char hstime[5],line[255],buf[255];
	int i;
	for(i=0;i<page;i++){
		if(fgets(line, sizeof(line), fp) == NULL)break;
		chop(line);
	}
	getStringFromIndexWithDelim(line,"|", 1, hstime, sizeof(hstime));
	getStringFromIndexWithDelim(line,"|", 2, history[0].winnum, sizeof(history[0].winnum));
	getStringFromIndexWithDelim(line,"|", 3, history[0].name, sizeof(history[0].name));
	if(strlen(history[0].name)==0)
		strcpy(history[0].name,"无");
	getStringFromIndexWithDelim(line,"|", 4, history[1].winnum, sizeof(history[1].winnum));
	getStringFromIndexWithDelim(line,"|", 5, history[1].name, sizeof(history[1].name));
	if(strlen(history[1].name)==0)
		strcpy(history[1].name,"无");
	getStringFromIndexWithDelim(line,"|", 6, history[2].winnum, sizeof(history[2].winnum));
	getStringFromIndexWithDelim(line,"|", 7, history[2].name, sizeof(history[2].name));
	if(strlen(history[2].name)==0)
		strcpy(history[2].name,"无");
	getStringFromIndexWithDelim(line,"|", 8, history[3].winnum, sizeof(history[3].winnum));
	getStringFromIndexWithDelim(line,"|", 9, history[3].name, sizeof(history[3].name));
	if(strlen(history[3].name)==0)
		strcpy(history[3].name,"无");
	getStringFromIndexWithDelim(line,"|", 10, history[4].winnum, sizeof(history[4].winnum));
	getStringFromIndexWithDelim(line,"|", 11, history[4].name, sizeof(history[4].name));
	if(strlen(history[4].name)==0)
		strcpy(history[4].name,"无");
	sprintf(token, "　　　　　　★乱舞格斗ＰＫ赛历史记录★\n"
	               "　　　　　　 第[%s]界乱舞格斗获奖名单\n\n",hstime);
	sprintf(buf,  "　　　　　「 冠 军 」　 %s\n"
								"　　　　　「 亚 军 」　 %s\n"
								"　　　　　「 季 军 」　 %s\n"
								"\n"
								"　　　　　　 更多活动请关注官方论坛"
								,history[0].name
								,history[1].name
								,history[2].name);
	strcat(token,buf);
	if(fgets(line, sizeof(line), fp) == NULL){
		fclose(fp);
		return TRUE;
	}else{
		fclose(fp);
		return FALSE;
	}
}

void AutoPk_SetHistoryStr( void )
{
	FILE* fp;
	char token[1024*16];
	char line[64];
	char time[4];
  fp=fopen("./data/autopk/autopkhistory.txt","r");
	if(fgets(line, sizeof(line), fp) == NULL)
		strcpy(time,"1");
	else
		getStringFromIndexWithDelim(line,"|",1,time,sizeof(time));
	sprintf(token, "%d|%d|%s|%d|%s|%d|%s|%d|%s|%d|%s|\n",atoi(time)+1 
									                      ,ship[0].winnum,ship[0].name
																				,ship[1].winnum,ship[1].name
																				,ship[2].winnum,ship[2].name
																				,ship[3].winnum,ship[3].name
																				,ship[4].winnum,ship[4].name);
	int i;
	strcat(token,line);
	for(i=1; i<20; i++){
		if(fgets(line, sizeof(line), fp) == NULL)break;
		strcat(token,line);
	}
	fclose(fp);
	
	fp=fopen("./data/autopk/autopkhistory.txt","w");
  fwrite(token, strlen(token), 1, fp);
  fclose(fp);
}

void AutoPk_PKNum( char *token )
{
	int playernum = CHAR_getPlayerMaxNum();
	int i,num=0,j=0;
	char buf[64];
	for(i=0;i<playernum;i++){
		if(CHAR_CHECKINDEX(i) == FALSE) continue;
		if(CHAR_getInt(i, CHAR_FLOOR) == 20000 )
		{
			if(++j<=5){
		 	sprintf(buf, "　 %3d      %5d     %s\n",
			                            CHAR_getInt( i, CHAR_TRANSMIGRATION),
			                            CHAR_getInt( i, CHAR_LV),
			                            CHAR_getChar( i, CHAR_NAME));
			strcat(token,buf);
			}
			num++;
		}
	}
	sprintf(buf, "　特别提示：当前参与比赛的人数为[%d]人",num);
	strcat(token,buf);
}

void AutoPk_PKSystemTalk( char *token1, char *token2 )
{
	int playernum = CHAR_getPlayerMaxNum();
	int i;
	for(i=0;i<playernum;i++){
		if(CHAR_CHECKINDEX(i) == FALSE) continue;
		if(CHAR_getInt(i, CHAR_FLOOR) == 20000 )
		 	CHAR_talkToCli( i, -1, token1, CHAR_COLORRED );
		else
			CHAR_talkToCli( i, -1, token2, CHAR_COLORRED );
	}
}

void AutoPk_PKSystemInfo()
{
	autopktime--;
	if(autopktime>0){
		char buf1[64];
		char buf2[128];
		char qfgonggao[256];
#ifdef	_YUANGUSA
		sprintf(qfgonggao,"[活动快讯]：乱舞格斗PK大赛将在[%d]分钟后进行，参加线路：%s",autopktime,getGameservername());
		system_all_talk(qfgonggao);
		sprintf(buf1, "距离参加今日乱舞PK大赛时间还剩%d分钟，请大家提前入场！",autopktime);
		if(awardpetid!=-1 && awarditemid==-1)
			sprintf(buf2,"距离参加今日乱舞PK大赛时间还剩%d分钟，请大家提前入场，本次比赛奖品为:编号:%d,宠物:%s",autopktime-2,awardpetid,awardpet);
		else if(awardpetid==-1 && awarditemid!=-1)
			sprintf(buf2,"距离参加今日乱舞PK大赛时间还剩%d分钟，请大家提前入场，本次比赛奖品为:编号:%d,物品:%s",autopktime-2,awarditemid,awarditem);
		else if(awardpetid!=-1 && awarditemid!=-1)
			sprintf(buf2,"距离参加今日乱舞PK大赛时间还剩%d分钟，请大家提前入场，本次比赛奖品为:编号:%d,宠物:%s和编号:%d,物品:%s",autopktime-2,awardpetid,awardpet,awarditemid,awarditem);
		else
			sprintf(buf2, "距离今日乱舞PK大赛时间还剩%d分钟，请大家提前入场！",autopktime);
#else
		sprintf(buf1, "距离截止参加PK比赛时间还剩%d分钟！",autopktime);
		if(awardpetid!=-1 && awarditemid==-1)
			sprintf(buf2,"距离截止参加PK比赛时间还剩%d分钟，本次比赛奖品为:编号:%d,宠物:%s",autopktime-2,awardpetid,awardpet);
		else if(awardpetid==-1 && awarditemid!=-1)
			sprintf(buf2,"距离截止参加PK比赛时间还剩%d分钟，本次比赛奖品为:编号:%d,物品:%s",autopktime-2,awarditemid,awarditem);
		else if(awardpetid!=-1 && awarditemid!=-1)
			sprintf(buf2,"距离截止参加PK比赛时间还剩%d分钟，本次比赛奖品为:编号:%d,宠物:%s和编号:%d,物品:%s",autopktime-2,awardpetid,awardpet,awarditemid,awarditem);
		else
			sprintf(buf2, "距离正式PK比赛时间还剩%d分钟！",autopktime);
#endif
		AutoPk_PKSystemTalk(buf1, buf2);
	}else if(autopktime==0){
		int i,charaindex,num=0;
		int playernum=CHAR_getPlayerMaxNum();
		for(i=0;i<playernum;i++){
			if(CHAR_CHECKINDEX(i) == FALSE) continue;
				if(CHAR_getInt(i, CHAR_FLOOR) == 20000 ){
					num++;
					charaindex=i;
					if(num>=getAutoPkMinNum())break;
				}
		}
		if(num>=getAutoPkMinNum()){
			AutoPk_PKSystemTalk("比赛正式开始咯！","比赛正式开始咯！");
			AutoPk_PKSystemStart();
			autopkstart=1;
			autopkcnt = 0;
			autopktime--;
			autopktype = 1;
		}else{
			i = 0;
			for(i=0;i<playernum;i++){
				if(CHAR_CHECKINDEX(i) == FALSE) continue;
				if(CHAR_getInt(i, CHAR_FLOOR) == 20000 ){
					int fl = 0, x = 0, y = 0;
					CHAR_getElderPosition(CHAR_getInt(i, CHAR_LASTTALKELDER), &fl, &x, &y);
					CHAR_warpToSpecificPoint(i, fl, x, y);
				}
			}
			AutoPk_PKSystemTalk("由于参加比赛人数过少，所以取消比赛！","由于参加比赛人数过少，所以取消比赛！");
		}
	}
}

void AutoPk_PKTimeSet( int pktime )
{
	autopktime=pktime;
	ship[0].winnum=0;
	strcpy(ship[0].cdkey, "\0");
	strcpy(ship[0].name, "\0");
	ship[1].winnum=0;
	strcpy(ship[1].cdkey, "\0");
	strcpy(ship[1].name, "\0");
	ship[2].winnum=0;
	strcpy(ship[2].cdkey, "\0");
	strcpy(ship[2].name, "\0");
	ship[3].winnum=0;
	strcpy(ship[3].cdkey, "\0");
	strcpy(ship[3].name, "\0");
	ship[4].winnum=0;
	strcpy(ship[4].cdkey, "\0");
	strcpy(ship[4].name, "\0");
	baward=FALSE;
}

void AutoPk_PKTimeSetOnly( int pktime )
{
	autopktime=pktime;
}

int AutoPk_PKTimeGet( void )
{
	return autopktime;
}

void AutoPk_AwardSet( int petid, char *pet, int itemid, char *item )
{
	if(petid>0){
		awardpetid=petid;
		strcpy(awardpet,pet);
	}else{
		awardpetid=-1;
	}
	if(itemid>0){
		awarditemid=itemid;
		strcpy(awarditem,item);
	}else{
		awarditemid=-1;
	}
}

#ifdef _NONUM_AUTOPK
void AutoPk_ChampionShipSet( char *cdkey, char *name,int winnum, int win )
{
	strcpy(ship[win].cdkey, cdkey);
	strcpy(ship[win].name, name);
	ship[win].winnum=winnum;
}
#else
void AutoPk_ChampionShipSet( char *cdkey, char *name, int winnum, int win )
{
	if(win==0){
		char token[64];
		if(winnum>ship[0].winnum){
			sprintf( token, "[乱舞战报]：%s目前排名第一，击杀点数：%d", name, winnum);
			AutoPk_PKSystemTalk(token,token);
			if(strstr(ship[0].cdkey,cdkey) && strstr(ship[0].name,name)){
				ship[0].winnum=winnum;
			}else if(strstr(ship[1].cdkey,cdkey) && strstr(ship[1].name,name)){
				ship[1].winnum=ship[0].winnum;
				strcpy(ship[1].cdkey, ship[0].cdkey);
				strcpy(ship[1].name, ship[0].name);
				ship[0].winnum=winnum;
				strcpy(ship[0].cdkey, cdkey);
				strcpy(ship[0].name, name);
			}else{
				ship[2].winnum=ship[1].winnum;
				strcpy(ship[2].cdkey, ship[1].cdkey);
				strcpy(ship[2].name, ship[1].name);
				ship[1].winnum=ship[0].winnum;
				strcpy(ship[1].cdkey, ship[0].cdkey);
				strcpy(ship[1].name, ship[0].name);
				ship[0].winnum=winnum;
				strcpy(ship[0].cdkey, cdkey);
				strcpy(ship[0].name, name);
			}
		}else if(winnum>ship[1].winnum){
			sprintf( token, "[乱舞战报]：%s目前排名第二，击杀点数：%d", name, winnum);
			AutoPk_PKSystemTalk(token,token);
			if(strstr(ship[1].cdkey,cdkey) && strstr(ship[1].name,name)){
				ship[1].winnum=winnum;
			}else{
				ship[2].winnum=ship[1].winnum;
				strcpy(ship[2].cdkey, ship[1].cdkey);
				strcpy(ship[2].name, ship[1].name);
				ship[1].winnum=winnum;
				strcpy(ship[1].cdkey, cdkey);
				strcpy(ship[1].name, name);
			}
		}else if(winnum>ship[2].winnum){
			sprintf( token, "[乱舞战报]：%s目前排名第三，击杀点数：%d", name, winnum);
			AutoPk_PKSystemTalk(token,token);
			ship[2].winnum=winnum;
			strcpy(ship[2].cdkey, cdkey);
			strcpy(ship[2].name, name);
		}
	}else if(win==1){
		ship[3].winnum=winnum;
		strcpy(ship[3].cdkey, cdkey);
		strcpy(ship[3].name, name);
	}else if(win==2){
		ship[4].winnum=winnum;
		strcpy(ship[4].cdkey, cdkey);
		strcpy(ship[4].name, name);
	}
}
#endif


void AutoPk_GetChampionShip( void )
{
	char token[64];
	if(strlen(ship[0].cdkey)>0){
		sprintf( token, "[布告天下]：本届乱舞格斗大赛冠军获奖者「%s」", ship[0].name);
		AutoPk_PKSystemTalk(token,token);
		AutoPk_PKTimeSetOnly(-1);
		autopkstart=0;
	}
	if(strlen(ship[1].cdkey)>0){
		sprintf( token, "[布告天下]：本届乱舞格斗大赛亚军获奖者「%s」", ship[1].name);
		AutoPk_PKSystemTalk(token,token);
	}
	if(strlen(ship[2].cdkey)>0){
		sprintf( token, "[布告天下]：本届乱舞格斗大赛季军获奖者「%s」", ship[2].name);
		AutoPk_PKSystemTalk(token,token);
	}
	if(strlen(ship[3].cdkey)>0 && strlen(ship[4].cdkey)>0){
		//sprintf( token, "恭喜%s和%s获得本次PK比赛第四和第五名！", ship[3].name,ship[4].name);
		//AutoPk_PKSystemTalk(token,token);
	}
	autopktype = 0;
	AutoPk_SetHistoryStr();
	
	
	
	baward = TRUE;
}

BOOL AutoPk_AddItem( int charindex, int index )
{
	char msgbuf[64];
	int	ret;
	int spaceNum=5,i,itemindex=-1;
  itemindex = CHAR_findEmptyItemBox( charindex );
	if( itemindex < 0 )	{
		CHAR_talkToCli( charindex, -1, "物品栏空间不足！！",  CHAR_COLORYELLOW);
		return FALSE;
	}

	itemindex = ITEM_makeItemAndRegist( award[index].awarditemid );
	if(itemindex == -1)
		return FALSE;
	ret = CHAR_addItemSpecificItemIndex( charindex, itemindex);
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
		ITEM_endExistItemsOne( itemindex );
		print ("\n ret error!!");
		return FALSE;
	}
	sprintf( msgbuf,"拿到%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( charindex, -1, msgbuf, CHAR_COLORYELLOW );
	CHAR_sendItemDataOne( charindex, ret);
	return TRUE;
}

BOOL AutoPk_AddPet( int charindex, int index)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	i,j;
	int petindex, petindex2;

	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( charindex, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "宠物已满！！");
		CHAR_talkToCli( charindex, -1, msgbuf,  CHAR_COLORYELLOW);
		return FALSE;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == award[index].awardpetid) {
				break;
			}
	}

	if( i == enemynum )
		return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( charindex, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( charindex, i ) == ret )
			break;
	}

	petindex2 = CHAR_getCharPet(charindex, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return FALSE;
	snprintf( msgbuf,sizeof( msgbuf), "拿到%s。", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( charindex, -1, msgbuf,  CHAR_COLORYELLOW);
	CHAR_setInt(petindex2,CHAR_VARIABLEAI,10000);
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
	return TRUE;
}

void NPC_AUTOPKHealerTalked( int meindex )
{
  if( CHAR_getInt( meindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )	return;

	if( (CHAR_getWorkInt( meindex, CHAR_WORKPARTYMODE) == 0)
	|| (CHAR_getWorkInt( meindex, CHAR_WORKPARTYMODE) == 2) )
	{

		NPC_HealerAllHeal( meindex);
		CHAR_talkToCli( meindex, -1,"恭喜你获胜，请等待本轮比赛结束，体力已全部回复。",CHAR_COLORYELLOW);
	}else{
		int i = 0;
		int otherindex;

		for( i=0 ; i < CHAR_PARTYMAX ; i++)
		{
			otherindex = CHAR_getWorkInt( meindex, CHAR_WORKPARTYINDEX1 + i);
			if(otherindex != -1){

				NPC_HealerAllHeal( otherindex);
				CHAR_talkToCli( otherindex, -1,"恭喜你获胜，体力已全部回复。",CHAR_COLORYELLOW);
			}
		}
	}
}

BOOL AutoPk_GetAwardStr( void )
{
	FILE* fp;
	int i=0;
	int j=0;
	int enemynum = ENEMY_getEnemyNum();
	fp = fopen("./data/autopk/autopkaward.txt", "r");
	if (fp == NULL)
	{
		print("查看比赛奖品文件打开错误\n");
		return FALSE;
	}
	for(i=0;i<5;i++){
		char	line[64],buf[32];
		if (fgets(line, sizeof(line), fp) == NULL)break;
		chop(line);
		getStringFromIndexWithDelim(line,"|", 1, buf, sizeof(buf));
		award[i].awardpetid=atoi(buf);
		if(award[i].awardpetid!=-1){
			for( j=0; j <enemynum ; j++ )
	        if(ENEMY_getInt( j, ENEMY_ID)==award[i].awardpetid)
	        	break;
	    if(j!=enemynum){
	    	strcpy(award[i].awardpet, ENEMY_getChar( j, ENEMY_NAME));
	    }else{
	    	award[i].awardpetid=-1;
	    	strcpy(award[i].awardpet, "\0");
	    }
    }
		getStringFromIndexWithDelim(line,"|", 2, buf, sizeof(buf));
		award[i].awarditemid=atoi(buf);
		if(award[i].awarditemid>0 && award[i].awarditemid <= getItemnum() ){
			strcpy(award[i].awarditem,ITEM_getNameFromNumber(award[i].awarditemid));
			}
		else{
			strcpy(award[i].awarditem, "\0");
			award[i].draw=AWARDNO;
			}
	}
	fclose(fp);
	return TRUE;
}

void AutoPk_SetAwardStr( char *token )
{
	FILE* fp;
	int i=0;
	fp = fopen("./data/autopk/autopkaward.txt", "w");
  fwrite(token, strlen(token), 1, fp);
	fclose(fp);
}

void AutoPk_PKSystemStart( void )
{
	int i,charaindex,num=0;
	int pkindex[1000],pktype[1000];
	int playernum=CHAR_getPlayerMaxNum();
	for(i=0;i<playernum;i++){
		if(CHAR_CHECKINDEX(i) == FALSE) continue;
			if(CHAR_getInt(i, CHAR_FLOOR) == 20000 && CHAR_getWorkInt( i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){
				pkindex[num]=i;
				pktype[num]=-1;
				num++;
			}
	}
	if(num>=2){
		if(num%2==0){
			int j = num-1;
			i=0;
			for(i=0;i<num/2;i++){
				BATTLE_CreateVsPlayer(pkindex[i],pkindex[j]);
				j--;
			}
		}else{
			int j,randi,fornum;
			fornum = num/2;
			randi=RAND(1,num);
			randi = randi - 1;
			if(randi != num - 1){
				pkindex[randi] = pkindex[num-1];
			}
			j = num-2;
			i=0;
			for(i=0;i<fornum;i++){
				BATTLE_CreateVsPlayer(pkindex[i],pkindex[j]);
				j--;
			}
		}
	}
}
#endif

