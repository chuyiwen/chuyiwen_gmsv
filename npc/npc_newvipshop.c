#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_newvipshop.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"

//特地从天堂高薪挖角来的"传送师"
#ifdef _VIP_SHOP
enum {
	WINDOW_START=1,
	WINDOW_SELECT,
	WINDOW_PREV,
	WINDOW_NEXT,
	WINDOW_WARP,
	WINDOW_END,
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

#define STANDBYTIME 50

static void NPC_NewVipShop_selectWindow( int meindex, int toindex, int num,int select);
BOOL NewVipShop_GetMenuStr( int meindex, int toindex, char *npcarg, char *token, int index );
int NewVipShop_ShowMenulist( char *npcarg );

int page;

BOOL NPC_NewVipshopInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TRANSERMANS);

	//CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	return TRUE;
}
//CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,1);
void NPC_NewVipshopTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}

	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_NewVipShop_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

void NPC_NewVipshopWindowTalked( int meindex, int talkerindex, int seqno,
								int select, char *data)
{
	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
		return;
	switch( seqno)	{
	case NPC_TRANSERMAN_START:
		break;
	case NPC_TRANSERMAN_SELECT:
		NPC_NewVipShop_selectWindow( meindex, talkerindex, WINDOW_SELECT, atoi( data));
		break;
	case NPC_TRANSERMAN_WARP:
		if( select == WINDOW_BUTTONTYPE_CANCEL )
			return;
		if( select == WINDOW_BUTTONTYPE_NEXT )
		{
				NPC_NewVipShop_selectWindow( meindex, talkerindex, WINDOW_NEXT, atoi( data));
				return;
		}
		if( select == WINDOW_BUTTONTYPE_PREV )
		{
				NPC_NewVipShop_selectWindow( meindex, talkerindex, WINDOW_PREV, atoi( data));
				return;
		}
		NPC_NewVipShop_selectWindow( meindex, talkerindex, WINDOW_WARP, atoi( data));
		break;
	case NPC_TRANSERMAN_END:
		NPC_NewVipShop_selectWindow( meindex, talkerindex, WINDOW_END, atoi( data));
		break;
	}

}

static void NPC_NewVipShop_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[1024];
	char buf1[256];
	char buf2[256];
	char buf3[256];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	static int select1;
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;

	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("NewVipShop_MAN: GetArgStrErr!!");
		return;
	}
	memset( token, 0, sizeof( token));

	switch( num)	{
	case WINDOW_START:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "START_MSG", token, sizeof( token) ) == NULL) {
			print("NewVipShop Get START_MSG ERROR !");
			return;
		}
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_TRANSERMAN_SELECT;
		break;
	case WINDOW_SELECT:
		CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE, 0);
		page=NewVipShop_ShowMenulist(npcarg);
		if( NewVipShop_GetMenuStr( meindex, toindex, npcarg, token,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)) == FALSE )	{
			print( "NewVipShop GetMenu ERROR !!");
			return;
		}
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		if(page>1)
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
		else
			buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_TRANSERMAN_WARP;
		break;
	case WINDOW_NEXT:
		if(CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)<=page)
			CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE, CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)+1);
		if( NewVipShop_GetMenuStr( meindex, toindex, npcarg, token,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)) == FALSE )	{
			print( "NewVipShop GetMenu ERROR !!");
			return;
		}
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		if(CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)+1==page)
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_PREV;
		else
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
		windowno = NPC_TRANSERMAN_WARP;
		break;
	case WINDOW_PREV:
		if(CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)>0)
			CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE, CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)-1);
		if( NewVipShop_GetMenuStr( meindex, toindex, npcarg, token,CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)) == FALSE )	{
			print( "NewVipShop GetMenu ERROR !!");
			return;
		}
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		if(CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)==0)
			buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
		else
			buttontype = WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
		windowno = NPC_TRANSERMAN_WARP;
		break;
	case WINDOW_WARP:
		select1=CHAR_getWorkInt( toindex, CHAR_WORKLISTPAGE)*7+select;
		CHAR_setWorkInt( toindex, CHAR_WORKLISTSELECT , select1);
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "END_MSG", buf1, sizeof( buf1) ) == NULL) {
			print("NewVipShop Get START_MSG ERROR !");
			return;
		}
		if(getStringFromIndexWithDelim( npcarg,"}",select1, buf2, sizeof( buf2)) != FALSE)
			NPC_Util_GetStrFromStrWithDelim( buf2, "MenuStr", buf3, sizeof( buf3));
		sprintf( token, "\n%s\n\n        %s\n",buf1, buf3);
		
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_TRANSERMAN_END;
		break;
	case WINDOW_END:
		ActionNpc_CheckMenuFree( meindex, toindex, npcarg, CHAR_getWorkInt( toindex, CHAR_WORKLISTSELECT ));
		return;
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}


void NPC_NewVipshopLoop( int meindex)
{

}

BOOL NewVipShop_GetMenuStr( int meindex, int toindex, char *npcarg, char *token,int index )
{
	char buf1[1024], buf2[32], buf3[36];
	BOOL FINDS=FALSE;
	int talkNo=index*7+1;
	if( npcarg == NULL ) return FALSE;
	
	sprintf( token, "你当前的会员点数是:%d        %d/%d页\n",sasql_vippoint(CHAR_getUseID(toindex),0,0), index+1, page);
	while( getStringFromIndexWithDelim( npcarg,"}",talkNo, buf1, sizeof( buf1)) != FALSE )	{
		talkNo++;
		memset( buf2, 0, sizeof( buf2));
		if( NPC_Util_GetStrFromStrWithDelim( buf1, "MenuStr", buf2, sizeof( buf2)) == NULL  )
			continue;
		FINDS = TRUE;

		sprintf( buf3, "    %s", buf2);
		if(strlen(buf3)<10)
			strcat( buf3, "　　　\n");
		else
			strcat( buf3, "\n");
		strcat( token, buf3);
		if(talkNo>(index*7+7))return FINDS;
	}
	return FINDS;
}

int NewVipShop_ShowMenulist( char *npcarg )
{
	char buf1[1024];
	int talkNo=1;
	while( getStringFromIndexWithDelim( npcarg,"}",talkNo, buf1, sizeof( buf1)) != FALSE )	{
		talkNo++;
	}
	return (talkNo-3)/7+1;
}

#endif
