#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_fmlookwar1.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"

#ifdef	_FM_NPC_LOOK_WAR1
//特地从天堂高薪挖角来的"传送师"
enum {
	WINDOW_START=1,
	WINDOW_SELECT,
	WINDOW_LOOKSELECT,
};
enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
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
extern int afmwarnum;
extern int bfmwarnum;
extern int cfmwarnum;
extern int dfmwarnum;
extern int efmwarnum;
extern int ffmwarnum;

static void NPC_FmLookWarMan1_selectWindow( int meindex, int toindex, int num,int select);
BOOL FmLookWarMan1_Run( int meindex, int toindex, int select);

BOOL NPC_FmLookWarMan1Init( int meindex )
{
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	return TRUE;
}

void NPC_FmLookWarMan1Talked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}
// Syu ADD 天才传送师判断组队状态
	if( CHAR_getWorkInt ( talkerindex , CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
	{
		CHAR_talkToCli( talkerindex, meindex, "不允许组队！",CHAR_COLORYELLOW);
		return;
	}
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_FmLookWarMan1_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

void NPC_FmLookWarMan1WindowTalked( int meindex, int talkerindex, int seqno,
								int select, char *data)
{
	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
		return;
	switch( seqno)	{
	case NPC_FMLOOKWARMAN1_START:
		break;
	case NPC_FMLOOKWARMAN1_SELECT:
		if( select == WINDOW_BUTTONTYPE_CANCEL )
			return;
		NPC_FmLookWarMan1_selectWindow( meindex, talkerindex, WINDOW_SELECT, atoi( data));
		break;
	case NPC_FMLOOKWARMAN1_LOOK:
		if( select == WINDOW_BUTTONTYPE_CANCEL )
			return;
		else if( select == WINDOW_BUTTONTYPE_NEXT )
			NPC_FmLookWarMan1_selectWindow( meindex, talkerindex, WINDOW_SELECT, 99);
		else if( select == WINDOW_BUTTONTYPE_PREV )
			NPC_FmLookWarMan1_selectWindow( meindex, talkerindex, WINDOW_SELECT, 98);
		else
			NPC_FmLookWarMan1_selectWindow( meindex, talkerindex, WINDOW_LOOKSELECT, atoi( data));
		break;
	}

}

static void NPC_FmLookWarMan1_selectWindow( int meindex, int toindex, int num,int select)
{
	char token[512];

	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	int PREVTYPE;
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}

	memset( token, 0, sizeof( token));

	switch( num)	{
	case WINDOW_START:
		sprintf(token,"2\n我是远程观战员，提供身临其境的观战服务。\n"
									"下面请选择您需要观看的战斗：\n"
									"                伊甸园庄园族战\n"
									"                塔尔塔庄园族战\n"
									"                尼克斯庄园族战\n"
									"                弗烈顿庄园族战\n"
									"                亚伊欧庄园族战\n"
									"                瑞尔亚斯庄族战\n");
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_FMLOOKWARMAN1_SELECT;
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		CHAR_setWorkInt(toindex,CHAR_WORKLISTPAGE,0);
		CHAR_setWorkInt(toindex,CHAR_WORKLISTSELECT,0);
		break;
	case WINDOW_SELECT:
		PREVTYPE = 0;
		if(select == 99 || select == 98){
			if(select == 98) PREVTYPE =1;
			select = CHAR_getWorkInt(toindex,CHAR_WORKLISTSELECT);
		}
		if(select>=1 && select<=6){
			int fmwarnum = 0;
			if(select == 1){
				fmwarnum = afmwarnum;
			}else if(select == 2){
				fmwarnum = bfmwarnum;
			}else if(select == 3){
				fmwarnum = cfmwarnum;
			}else if(select == 4){
				fmwarnum = dfmwarnum;
			}else if(select == 5){
				fmwarnum = efmwarnum;
			}else if(select == 6){
				fmwarnum = ffmwarnum;
			}
			
			if(fmwarnum<1){
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return;
			}
			if(PREVTYPE==1)
				CHAR_setWorkInt(toindex,CHAR_WORKLISTPAGE,CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)-1);
			else
				CHAR_setWorkInt(toindex,CHAR_WORKLISTPAGE,CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)+1);
			if(CHAR_getWorkInt(toindex,CHAR_WORKLISTSELECT)==0)
				CHAR_setWorkInt(toindex,CHAR_WORKLISTSELECT,select);
			sprintf(token,SearchFmWar(toindex,select));
			if(strcmp(token,"err")==0){
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return;
			}
			CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_SELECT);
			if(fmwarnum>CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)*6){
				if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)-1>0)
					buttontype = WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
				else
					buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
			}else{
				if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)-1>0)
					buttontype = WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_CANCEL;
				else
					buttontype = WINDOW_BUTTONTYPE_CANCEL;
			}
			windowno = NPC_FMLOOKWARMAN1_LOOK;
			windowtype = WINDOW_MESSAGETYPE_SELECT;
		}else
			return;
		break;
	case WINDOW_LOOKSELECT:
		if(LookFmWarIndex(toindex,select)==-1){
			CHAR_talkToCli( toindex, meindex, "很遗憾，观战失败。",CHAR_COLORYELLOW);
			return;
		}
		break;
	default:
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}


void NPC_FmLookWarMan1Loop( int meindex)
{

}

BOOL FmLookWarMan1_Run( int meindex, int toindex, int select)
{
	if(select<1 || select>6) return FALSE;
	switch(select){
		case 1:
			if(SearchFmWarRandIndex(toindex,5042)==0){
				return TRUE;
			}else{
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return FALSE;
			}
			break;
		case 2:
			if(SearchFmWarRandIndex(toindex,6032)==0){
				return TRUE;
			}else{
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return FALSE;
			}
			break;
		case 3:
			if(SearchFmWarRandIndex(toindex,7032)==0){
				return TRUE;
			}else{
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return FALSE;
			}
			break;
		case 4:
			if(SearchFmWarRandIndex(toindex,8032)==0){
				return TRUE;
			}else{
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return FALSE;
			}
			break;
		case 5:
			if(SearchFmWarRandIndex(toindex,9032)==0){
				return TRUE;
			}else{
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return FALSE;
			}
			break;
		case 6:
			if(SearchFmWarRandIndex(toindex,10032)==0){
				return TRUE;
			}else{
				CHAR_talkToCli( toindex, meindex, "很遗憾，该地图目前无战斗可以观看。",CHAR_COLORYELLOW);
				return FALSE;
			}
			break;
		default:
			break;
	}
	return FALSE;
}
#endif


