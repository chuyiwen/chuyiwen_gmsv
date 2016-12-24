#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "npc_itemup.h"
#ifdef	_NPC_ITEMUP
enum {
	ITEMUP_START=1,
	ITEMUP_SELECT1,
	ITEMUP_SELECT2,
	ITEMUP_MESSAGE,
	ITEMUP_END,
	ITEMUP_ERROR,

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

static void NPC_ItemupMan_selectWindow( int meindex, int toindex, int num,int select);
BOOL ItemupMan_GetNeedStr( int meindex, int toindex, char *npcarg, char *token, int num );
BOOL ItemupMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token);
BOOL ItemupMan_GetItemStr( int meindex, int toindex, char *npcarg, char *token,int num);

BOOL NPC_ItemupManInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

   	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_ITEMCHANGENPC);

	//CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);

	return TRUE;
}
//CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,1);
void NPC_ItemupManTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if( NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if( NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}   
	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_ItemupMan_selectWindow( meindex, talkerindex, ITEMUP_START, -1);

}

void NPC_ItemupManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	int flg=-1;
	if( select == WINDOW_BUTTONTYPE_NO || select == WINDOW_BUTTONTYPE_CANCEL)
		return;
	switch( seqno)	{
	case NPC_ITEMUP_START:
		break;
	case NPC_ITEMUP_SELECT1:
		NPC_ItemupMan_selectWindow( meindex, talkerindex, ITEMUP_SELECT1, flg);
		break;
	case NPC_ITEMUP_SELECT2:
		flg = atoi( data);
		NPC_ItemupMan_selectWindow( meindex, talkerindex, ITEMUP_SELECT2, flg);
		break;
	case NPC_ITEMUP_MESSAGE:
		flg = atoi( data);
		CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, flg);
		NPC_ItemupMan_selectWindow( meindex, talkerindex, ITEMUP_MESSAGE, flg);
		break;
	case NPC_ITEMUP_END:
		flg = CHAR_getWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT);
		NPC_ItemupMan_selectWindow( meindex, talkerindex, ITEMUP_END , flg);
		break;
	}

}

static void NPC_ItemupMan_selectWindow( int meindex, int toindex, int num,int select)
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];
	char npcbuf[2048];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	windowtype = WINDOW_MESSAGETYPE_MESSAGE;
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
	case ITEMUP_START:
		if( NPC_Util_GetStrFromStrWithDelim( npcarg, "START_MSG", token, sizeof( token) ) == NULL) {
			print("TranserMan Get START_MSG ERROR !");
			return;
		}
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, ITEMUP_START);
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_ITEMUP_SELECT1;
		break;
	case ITEMUP_SELECT1:
		if( ItemupMan_GetMenuStr( meindex, toindex, npcarg, token) == FALSE )
			return;
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, ITEMUP_SELECT1);
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_ITEMUP_SELECT2;
		break;
	case ITEMUP_SELECT2:
		if( ItemupMan_GetItemStr( meindex, toindex, npcarg, token, select) == FALSE )
			return;
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, ITEMUP_SELECT2);
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_ITEMUP_MESSAGE;
		break;
	case ITEMUP_MESSAGE:
		if( ItemupMan_GetNeedStr( meindex, toindex, npcarg, token, select ) == FALSE )
			return;
		buttontype = WINDOW_BUTTONTYPE_YESNO;
		windowno = NPC_ITEMUP_END;
		break;
	case ITEMUP_END:
		if(getStringFromIndexWithDelim( npcarg,"]", CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE), npcbuf, sizeof( npcbuf)) == FALSE){
			return;
		}
		if( ActionNpc_CheckMenuFree( meindex, toindex, npcbuf, select ) == FALSE ){
			memset( token, 0, sizeof( token));
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "FAIL_MSG", token, sizeof( token) ) != NULL) {
				CHAR_talkToCli( toindex, meindex, token, CHAR_COLORYELLOW);
			}
		}
		return;
		break;
	case ITEMUP_ERROR:
		break;
	}

	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}

void NPC_ItemupManLoop( int meindex)
{
	
}

BOOL ItemupMan_GetItemStr( int meindex, int toindex, char *npcarg, char *token, int num)
{
	char buf1[1024], buf2[256], buf3[256],buf0[2048];
	char NullName[256] = {"未知的物品"};
	BOOL FINDS=FALSE;
	int talkNo=1;
	int addID;
	if( npcarg == NULL ) return FALSE;
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "ItemHead", buf1, sizeof( buf1) ) == NULL){
		print("ItemchangeMan Get NeedHead ERROR!\n%s", npcarg);
		return FALSE;
	}
	sprintf( token, "%s\n", buf1);
	
	if(getStringFromIndexWithDelim( npcarg,"]", num, buf0, sizeof( buf0)) == FALSE){
		return FALSE;
	}
	CHAR_setWorkInt( toindex, CHAR_WORKLISTPAGE, num);
	while( getStringFromIndexWithDelim( buf0,"}", talkNo, buf1, sizeof( buf1)) != FALSE )	{
		char *ItemName;
		talkNo++;

		if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHANGEITEM", buf2, sizeof( buf2)) != NULL  ){
			addID = atoi( buf2);
			if( (ItemName = ITEM_getNameFromNumber( addID)) == "\0" ){
				sprintf( buf3, "   %s\n", NullName);
			}else{
				sprintf( buf3, "   %s\n", ItemName);
			}
		//	continue;
		}else	{
			memset( buf3, 0, sizeof( buf3));
			if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHANGEMSG", buf3, sizeof( buf3)) == NULL  )
				continue;
			strcat( buf3, "\n");
		}
		FINDS = TRUE;
		strcat( token, buf3);
	}

	return FINDS;
}

BOOL ItemupMan_GetNeedStr( int meindex, int toindex, char *npcarg, char *token, int num )
{
	char buf1[1024], buf2[256], buf3[256],buf0[2048],buf4[256];
	char NullName[256] = {"未知的物品"};
	int talkNo=1, addID, i=0,itemnum=1;
	BOOL FINDS = FALSE;
	if( npcarg == NULL ) return FALSE;
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NeedHead", buf1, sizeof( buf1) ) == NULL){
		print("ItemchangeMan Get NeedHead ERROR!\n%s", npcarg);
		return FALSE;
	}
	sprintf( token, "%s\n", buf1);
	if(getStringFromIndexWithDelim( npcarg,"]", CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE), buf0, sizeof( buf0)) == FALSE){
		return FALSE;
	}
	if( getStringFromIndexWithDelim( buf0,"}", num, buf1, sizeof( buf1)) == FALSE )	{
		printf("Get NeedMess error !!\n");
		return FALSE;
	}

	if( NPC_Util_GetStrFromStrWithDelim( buf1, "NeedMsg", buf2, sizeof( buf2) ) != NULL){
		strcat( token,buf2);
		FINDS = TRUE;
	}

	return FINDS;
}

BOOL ItemupMan_GetMenuStr( int meindex, int toindex, char *npcarg, char *token)
{
	char buf1[1024],buf2[256];;
	BOOL FINDS=FALSE;
	int talkNo=1;
	char *MenuName;
	if( npcarg == NULL ) return FALSE;
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "MenuHead", buf1, sizeof( buf1) ) == NULL){
		print("ItemchangeMan Get MenuHead ERROR!");
		return FALSE;
	}

	sprintf( token, "%s\n", buf1);
	
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "MenuName", buf1, sizeof( buf1) ) == NULL){
		print("ItemchangeMan Get MenuHead ERROR!");
		return FALSE;
	}

	while( getStringFromIndexWithDelim( buf1,",", talkNo, buf2, sizeof( buf2)) != FALSE )	{
		talkNo++;
		MenuName = buf2;
		FINDS=TRUE;
		strcat( token, MenuName);
		strcat( token, "\n");
	}

	return FINDS;
}
#endif
