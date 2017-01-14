#include "version.h"
#include <stdio.h>

#include "configfile.h"
#include "readmap.h"
#include "map_deal.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "item.h"
#include "lssproto_serv.h"
#include "handletime.h"
#include "log.h"
#include "item_event.h"
#include "battle.h"
#include "petmail.h"
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

extern int itemLine;

#ifdef _DROPSTAKENEW
#define CASINOTOKENID				20121			// 游乐场彩券代号
#define MAXCASINOMAPNUM				150			// 游乐场地图上限
#define MAXACCUMULATION				1000000		// 积分上限
typedef struct tagCasinoMapTable
{
	int casinofl;				// 游乐场图层
	int casinox;				// 游乐场Ｘ座标
	int casinoy;				// 游乐场Ｙ座标
	int casinotype;			// 赌注类型
	int dropflag;				// 是否可下注 flag
	char casinoinfo[64];		// 赌注类型说明
	int accumulation;			// 积分
	int stakenum;				// 彩券数量
}CasinoMapTable;
CasinoMapTable casinomap[MAXCASINOMAPNUM] = 
{
	{ 7008, 13, 13, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 14, 13, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 15, 13, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 16, 13, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 17, 13, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 13, 14, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 14, 14, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 15, 14, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 16, 14, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 17, 14, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 13, 15, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 14, 15, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 15, 15, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 16, 15, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008, 17, 15, DICE_BIG, 1, "骰子－大", 1, 0},
	{ 7008,  8, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008,  9, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 10, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 11, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 12, 13, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008,  8, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008,  9, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 10, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 11, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 12, 14, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008,  8, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008,  9, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 10, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 11, 15, DICE_SMALL, 1, "骰子－小", 1, 0},
	{ 7008, 12, 15, DICE_SMALL, 1, "骰子－小", 1, 0},

	{ 7005, 22, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
	{ 7005, 23, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
	{ 7005, 24, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
	{ 7005, 25, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
	{ 7005, 26, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
	{ 7005, 27, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
	{ 7005, 28, 21, PET_RACE1, 1, "竞速场－宠物１", 2, 0},
	{ 7005, 22, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
	{ 7005, 23, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
	{ 7005, 24, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
	{ 7005, 25, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
	{ 7005, 26, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
	{ 7005, 27, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
	{ 7005, 28, 24, PET_RACE2, 1, "竞速场－宠物２", 2, 0},
	{ 7005, 22, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
	{ 7005, 23, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
	{ 7005, 24, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
	{ 7005, 25, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
	{ 7005, 26, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
	{ 7005, 27, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},
	{ 7005, 28, 27, PET_RACE3, 1, "竞速场－宠物３", 2, 0},

	{ 7003, 21, 15, ROULETTE1, 1, "轮盘红１", 1, 0}, { 7003, 17, 15, ROULETTE3, 1, "轮盘红３", 1, 0},
	{ 7003, 13, 15, ROULETTE5, 1, "轮盘红５", 1, 0}, { 7003, 21, 21, ROULETTE6, 1, "轮盘红６", 1, 0},
	{ 7003, 17, 21, ROULETTE8, 1, "轮盘红８", 1, 0}, { 7003, 15, 21, ROULETTE9, 1, "轮盘红９", 1, 0},
	{ 7003, 13, 21, ROULETTE10, 1, "轮盘红１０", 1, 0}, { 7003, 13, 18, ROULETTE13, 1, "轮盘红１３", 1, 0},
	{ 7003,  9, 21, ROULETTE17, 1, "轮盘红１７", 1, 0}, { 7003, 21, 18, ROULETTE18, 1, "轮盘红１８", 1, 0},

	{ 7003, 19, 15, ROULETTE22, 1, "轮盘绿２", 1, 0}, { 7003, 15, 15, ROULETTE24, 1, "轮盘绿４", 1, 0},
	{ 7003, 19, 21, ROULETTE27, 1, "轮盘绿７", 1, 0}, { 7003, 11, 15, ROULETTE31, 1, "轮盘绿１１", 1, 0},
	{ 7003,  9, 15, ROULETTE32, 1, "轮盘绿１２", 1, 0}, { 7003, 11, 18, ROULETTE34, 1, "轮盘绿１４", 1, 0},
	{ 7003,  9, 18, ROULETTE35, 1, "轮盘绿１５", 1, 0}, { 7003, 11, 21, ROULETTE36, 1, "轮盘绿１６", 1, 0},
	{ 7003, 19, 18, ROULETTE39, 1, "轮盘绿１９", 1, 0}, { 7003, 17, 18, ROULETTE40, 1, "轮盘绿２０", 1, 0},
	{ 7003, 17, 24, ROULETTE41, 1, "轮盘红", 1, 0}, { 7003, 16, 24, ROULETTE41, 1, "轮盘红", 1, 0},
	{ 7003, 17, 25, ROULETTE41, 1, "轮盘红", 1, 0}, { 7003, 16, 25, ROULETTE41, 1, "轮盘红", 1, 0},
	{ 7003, 14, 24, ROULETTE42, 1, "轮盘绿", 1, 0}, { 7003, 13, 24, ROULETTE42, 1, "轮盘绿", 1, 0},
	{ 7003, 14, 25, ROULETTE42, 1, "轮盘绿", 1, 0}, { 7003, 13, 25, ROULETTE42, 1, "轮盘绿", 1, 0},

    // WON ADD 修正地图上单双的错误
	{ 7003, 11, 24, ROULETTE43, 1, "轮盘单", 1, 0}, { 7003, 10, 24, ROULETTE43, 1, "轮盘单", 1, 0},
	{ 7003, 11, 25, ROULETTE43, 1, "轮盘单", 1, 0}, { 7003, 10, 25, ROULETTE43, 1, "轮盘单", 1, 0},
	{ 7003, 20, 24, ROULETTE44, 1, "轮盘双", 1, 0}, { 7003, 19, 24, ROULETTE44, 1, "轮盘双", 1, 0},
	{ 7003, 20, 25, ROULETTE44, 1, "轮盘双", 1, 0}, { 7003, 19, 25, ROULETTE44, 1, "轮盘双", 1, 0},



	{ 7003, 20, 15, (ROULETTE1<<16)+ROULETTE22, 1, "双号红１绿２", 1, 0},
	{ 7003, 18, 15, (ROULETTE3<<16)+ROULETTE22, 1, "双号红３绿２", 1, 0},
	{ 7003, 16, 15, (ROULETTE3<<16)+ROULETTE24, 1, "双号红３绿４", 1, 0},
	{ 7003, 14, 15, (ROULETTE5<<16)+ROULETTE24, 1, "双号红５绿４", 1, 0},
	{ 7003, 12, 15, (ROULETTE5<<16)+ROULETTE31, 1, "双号红５绿１１", 1, 0},
	{ 7003, 10, 15, (ROULETTE32<<16)+ROULETTE31, 1, "双号绿１２绿１１", 1, 0},

	{ 7003, 20, 18, (ROULETTE18<<16)+ROULETTE39, 1, "双号红１８绿１９", 1, 0},
	{ 7003, 18, 18, (ROULETTE40<<16)+ROULETTE39, 1, "双号绿２０绿１９", 1, 0},
	{ 7003, 16, 18, (ROULETTE40<<16)+ROULETTE45, 1, "双号绿２０黑００", 1, 0},
	{ 7003, 14, 18, (ROULETTE13<<16)+ROULETTE45, 1, "双号红１３黑００", 1, 0},
	{ 7003, 12, 18, (ROULETTE13<<16)+ROULETTE34, 1, "双号红１３绿１４", 1, 0},
	{ 7003, 10, 18, (ROULETTE35<<16)+ROULETTE34, 1, "双号绿１５绿１４", 1, 0},

	{ 7003, 20, 21, (ROULETTE6<<16) +ROULETTE27, 1, "双号红６绿７", 1, 0},
	{ 7003, 18, 21, (ROULETTE8<<16) +ROULETTE27, 1, "双号红８绿７", 1, 0},
	{ 7003, 16, 21, (ROULETTE8<<16) + ROULETTE9, 1, "双号红８红９", 1, 0},
	{ 7003, 14, 21, (ROULETTE10<<16)+ ROULETTE9, 1, "双号红１０红９", 1, 0},
	{ 7003, 12, 21, (ROULETTE10<<16)+ROULETTE36, 1, "双号红１０绿１６", 1, 0},
	{ 7003, 10, 21, (ROULETTE17<<16)+ROULETTE36, 1, "双号红１７绿１６", 1, 0},
	{ 7003,  8, 15, ROULETTE51, 1, "横列１", 1, 0},	//横列
	{ 7003,  8, 18, ROULETTE52, 1, "横列２", 1, 0},
	{ 7003,  8, 21, ROULETTE53, 1, "横列３", 1, 0},
	{ 7003,  6, 15, ROULETTE61, 1, "号码１－１０", 1, 0},	//1到10
	{ 7003,  5, 15, ROULETTE61, 1, "号码１－１０", 1, 0},
	{ 7003,  6, 16, ROULETTE61, 1, "号码１－１０", 1, 0},
	{ 7003,  5, 16, ROULETTE61, 1, "号码１－１０", 1, 0},
	{ 7003, 24, 15, ROULETTE62, 1, "号码１１－２０", 1, 0},	//10到11
	{ 7003, 23, 15, ROULETTE62, 1, "号码１１－２０", 1, 0},
	{ 7003, 24, 16, ROULETTE62, 1, "号码１１－２０", 1, 0},
	{ 7003, 23, 16, ROULETTE62, 1, "号码１１－２０", 1, 0},
	{ 7003, 15, 18, ROULETTE45, 1, "轮盘(００)", 1, 0},

};
#endif


static char CHAR_sendItemBuffer[STRINGBUFSIZ];
BOOL CHAR_sendItemDataOne( int charaindex, int haveitemindex)
{
	int		itemgrp[1];
	itemgrp[0] = haveitemindex;
	return CHAR_sendItemData( charaindex, itemgrp, 1);
}

BOOL CHAR_sendItemData( int charaindex, int *itemgroup, int num)
{
	int		i;
	int		strlength = 0;
	int     fd;
	
	if( num <= 0 || num > CHAR_MAXITEMHAVE) return FALSE;

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ) {
		return FALSE;
	}
	
	for( i = 0; i < num; i ++ ) {
		char    token[512];
		int     itemindex;
		if( !CHAR_CHECKITEMINDEX( charaindex, itemgroup[i])) {
			continue;
		}
		itemindex = CHAR_getItemIndex( charaindex, itemgroup[i] );
		if( itemindex >= 0 ) {
			snprintf( token ,sizeof(token),"%d|%s|" ,itemgroup[i], ITEM_makeItemStatusString( -1,itemindex ) );
			if(ITEM_getInt( itemindex, ITEM_CANBEPILE)!=1 && ITEM_getInt( itemindex, ITEM_USEPILENUMS)>1)
				{
					CHAR_setItemIndex( charaindex , itemgroup[i], -1);
					snprintf( token ,sizeof(token),"%s|" , ITEM_makeItemFalseStringWithNum( itemgroup[i]) );
				}
		}else {
			snprintf( token ,sizeof(token),"%s|" , ITEM_makeItemFalseStringWithNum( itemgroup[i]) );
		}
		strcpysafe( CHAR_sendItemBuffer + strlength, sizeof(CHAR_sendItemBuffer) - strlength, token );
		strlength += strlen( token );
		if( strlength >= arraysizeof(CHAR_sendItemBuffer) ) {
			break;
		}
	}
	dchop( CHAR_sendItemBuffer , "|" );
	fd = getfdFromCharaIndex( charaindex );
	if( fd != -1 ){
		lssproto_I_send( fd, CHAR_sendItemBuffer );
		return TRUE;
	}
	return FALSE;
}

static void CHAR_sendItemDetachEvent( int charaindex,int itemid )
{
	char    mesg[256];

	snprintf( mesg, sizeof( mesg), "卸下%s ", ITEM_getAppropriateName(itemid) );
	CHAR_talkToCli(charaindex,-1,mesg,CHAR_COLORWHITE);
#ifdef _ITEM_METAMO
	if( ITEM_getEquipPlace( charaindex, itemid) == CHAR_BODY 
			&& CHAR_getWorkInt( charaindex, CHAR_WORKITEMMETAMO) != 0 ) {
		CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, 0);
		CHAR_setWorkInt( charaindex, CHAR_WORKNPCMETAMO, 0 ); //与npc对话後的变身也要变回来
		CHAR_complianceParameter( charaindex);
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER);
		CHAR_talkToCli( charaindex, -1, "变身失效了！", CHAR_COLORWHITE);
	}
#endif
#ifdef _ITEM_RIDE
	if(CHAR_getInt( charaindex , CHAR_RIDEPET ) != -1 ){
		if( !strcmp( ITEM_getChar( itemid, ITEM_USEFUNC), "ITEM_RIDE") ) {
			char ridemetamo[12];
			char *itemarg = ITEM_getChar( itemid, ITEM_ARGUMENT);
			getStringFromIndexWithDelim( itemarg, "|", 2, ridemetamo, sizeof(ridemetamo));
			if(CHAR_getInt( charaindex , CHAR_BASEIMAGENUMBER )==atoi(ridemetamo)){
				CHAR_setInt( charaindex , CHAR_RIDEPET, -1 );
				CHAR_setInt( charaindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( charaindex , CHAR_BASEBASEIMAGENUMBER) );
				CHAR_complianceParameter( charaindex );
				CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX ));
				CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_RIDEPET);
			}
		}
	}
#endif
	{
		typedef void (*DETACHFUNC)(int,int);
		DETACHFUNC def;
		def=(DETACHFUNC)ITEM_getFunctionPointer(itemid,ITEM_DETACHFUNC);
		if( def )	{
			def( charaindex,itemid );
		}
#ifdef _ALLBLUES_LUA_1_2 
		else{
			RunItemDetachEvent(charaindex,itemid );
		}
#endif
	}
}

static void CHAR_sendItemAttachEvent( int charaindex,int itemid )
{
	char    mesg[256];
	snprintf( mesg, sizeof( mesg), "装备%s ",
			  ITEM_getAppropriateName(itemid) );
	CHAR_talkToCli(charaindex,-1,mesg,CHAR_COLORWHITE);
	{
		typedef void (*ATTACHFUNC)(int,int);
		ATTACHFUNC atf;
		atf=(ATTACHFUNC)ITEM_getFunctionPointer(itemid,ITEM_ATTACHFUNC);
		if( atf ){
			atf( charaindex,itemid );
		}
#ifdef _ALLBLUES_LUA_1_2 
		else{
			RunItemAttachEvent(charaindex, itemid );
		}
#endif
	}
}


int CHAR_findSurplusItemBox( int charaindex )
{
	int i, remnants=0, itemindex;
	for(i=CHAR_STARTITEMARRAY; i< CHAR_MAXITEMHAVE; i++){
		itemindex = CHAR_getItemIndex( charaindex , i );
		if( itemindex == -1 ) remnants++;
    }
	return remnants;
}


static int CHAR_findEmptyItemBoxFromChar( Char* ch )
{
	int     i;

	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE  ; i ++ ){
		int     itemindex;
		itemindex = ch->indexOfExistItems[i];
		if( itemindex == -1 )return i;
	}

	return -1;
}

static int CHAR_findEmptyPoolItemBoxFromChar( Char* ch )
{
	int     i;
	for( i=0; i<CHAR_MAXPOOLITEMHAVE; i++ ){
		int     itemindex;
		itemindex = ch->indexOfExistPoolItems[i];
		if( itemindex == -1 ) return i;
	}
	return -1;
}

int CHAR_findEmptyPoolItemBox( int index )
{
	Char*   ch;
	if( !CHAR_CHECKINDEX( index ) )return -1;
	ch = CHAR_getCharPointer(index);
	if( ch != NULL )
		return CHAR_findEmptyPoolItemBoxFromChar( ch );
	else
		return -1;
}

int CHAR_findEmptyItemBox( int index )
{
	Char*   ch;

	if( !CHAR_CHECKINDEX( index ) )return -1;
	ch = CHAR_getCharPointer(index);
	if( ch != NULL )
		return CHAR_findEmptyItemBoxFromChar( ch );
	else
		return -1;

}

#define LACKOFCLASS {CHAR_talkToCli(index,-1,"能力不足无法装备。",CHAR_COLORWHITE);}
#define CANNOTEQUIP {CHAR_talkToCli(index,-1,"无法装备该项物品。",CHAR_COLORWHITE);}
#define CANNOTEQUIP2 {CHAR_talkToCli(index,-1,"无法同时装备两项同种类的物品。",CHAR_COLORWHITE);}
#define CANNOTEXCHANGE {CHAR_talkToCli(index,-1,"无法交换此两项物品。",CHAR_COLORWHITE);}
#define CANNOTMOVE  {CHAR_talkToCli(index,-1,"无法移动该项物品。",CHAR_COLORWHITE);}

static BOOL CHAR_sendSIToCli( int charindex , int from , int to )
{
	int fd = getfdFromCharaIndex( charindex );
	if( fd != -1 ){
		lssproto_SI_send( fd , from, to  );
		return TRUE;
	}
	return FALSE;
}

static BOOL CHAR_moveItemFromItemBoxToEquip( int index, int fromindex,
											int toindex )
{
	int     fromid,toid;
	int     fromeqplace;
	BOOL	flg = FALSE;

	if( !CHAR_CHECKINDEX( index ) )return FALSE;

	fromid  = CHAR_getItemIndex(index,fromindex);
	toid    = CHAR_getItemIndex(index,toindex);
	if( !ITEM_CHECKINDEX( fromid) )
		return FALSE;

	if( CHAR_getInt( index, CHAR_TRANSMIGRATION ) <= 0 ){
	    if( ITEM_getInt(fromid,ITEM_LEVEL) > CHAR_getInt(index,CHAR_LV) ){
        	LACKOFCLASS;
	        return FALSE;
    	}
	}
#ifdef _ITEMSET4_TXT
	if( CHAR_getInt( index, CHAR_STR) < ITEM_getInt( fromid, ITEM_NEEDSTR) ) return FALSE;
	if( CHAR_getInt( index, CHAR_DEX) < ITEM_getInt( fromid, ITEM_NEEDDEX) ) return FALSE;
	if( CHAR_getInt( index, CHAR_TRANSMIGRATION) < ITEM_getInt( fromid, ITEM_NEEDTRANS) ) return FALSE;
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	if( ITEM_getInt( fromid, ITEM_NEEDPROFESSION) != 0 ){
		if( CHAR_getInt( index, PROFESSION_CLASS ) != ITEM_getInt( fromid, ITEM_NEEDPROFESSION) )
			return FALSE;
	}
#endif
#endif

#ifdef _FIXBUG_ATTACKBOW
	{
		if( CHAR_getWorkInt( index, CHAR_WORKITEMMETAMO) > 0 ||
			CHAR_getWorkInt( index, CHAR_WORKNPCMETAMO) > 0 
			|| CHAR_getWorkInt( index, CHAR_WORKFOXROUND ) != -1){ //若是变成小狐狸

			int armtype = ITEM_getInt( fromid, ITEM_TYPE );

			if( armtype == ITEM_BOW || armtype == ITEM_BOUNDTHROW ||
				armtype == ITEM_BREAKTHROW || armtype == ITEM_BOOMERANG)	{
				CHAR_talkToCli( index, -1, "变身无法使用远距离武器！", CHAR_COLORYELLOW );
				return FALSE;
			}
		}
	}
#endif

#ifdef _ANGEL_SUMMON
	//if( !strcmp( ITEM_getChar( fromid, ITEM_USEFUNC), "ITEM_AngelToken") ) {
	if( ITEM_getInt( fromid, ITEM_ID) == ANGELITEM ) {
		int mindex;
		char nameinfo[64];

		mindex = checkIfAngel( index);
		getMissionNameInfo( index, nameinfo);
		if( mindex < 0 ||
			strcmp( ITEM_getChar( fromid, ITEM_ANGELINFO), missiontable[mindex].angelinfo) ||
			strcmp( ITEM_getChar( fromid, ITEM_HEROINFO), missiontable[mindex].heroinfo) ||
			strcmp( ITEM_getChar( fromid, ITEM_ANGELINFO), nameinfo) ) {

			CHAR_talkToCli( index, -1, "这并不是属於你的信物，无法装备。", CHAR_COLORYELLOW );
			return FALSE;
		}
	}
#endif

	fromeqplace = ITEM_getEquipPlace( index, fromid );
	if( fromeqplace == -1 ){
		CANNOTEQUIP;
		return FALSE;
	}
	//if( BATTLE_GetWepon( charaindex ) != ITEM_BOW )

	if( fromeqplace == CHAR_DECORATION1 ) {
		int checkplace = ( toindex == CHAR_DECORATION1)
						? CHAR_DECORATION2:CHAR_DECORATION1;
		int	otheritemindex;

		if( toindex == CHAR_DECORATION1 || toindex == CHAR_DECORATION2 ) {
			otheritemindex = CHAR_getItemIndex( index, checkplace);
			if( ITEM_CHECKINDEX( otheritemindex )) {
				int otheritemtype = ITEM_getInt( otheritemindex, ITEM_TYPE);
				int myitemtype = ITEM_getInt( CHAR_getItemIndex( index, fromindex) , ITEM_TYPE);
				if( otheritemtype == myitemtype) {
					if( toid < 0 ) {
						CANNOTEQUIP2;
					}else {
						CANNOTEXCHANGE;
					}
					return FALSE;
				}
			}
			flg = TRUE;
		}
	}else {
		if( toindex == fromeqplace) flg = TRUE;
	}
	if( flg ){
		CHAR_setItemIndex(index,toindex,fromid);
		CHAR_setItemIndex(index,fromindex,toid);
		if( toid != -1 ) {
			CHAR_sendItemDetachEvent( index, toid );
		}
		CHAR_sendItemAttachEvent( index, fromid );
		CHAR_sendSIToCli( index, fromindex,toindex );
	}else {
		CANNOTEQUIP;
		return FALSE;
	}
	return TRUE;
}

static BOOL CHAR_moveItemFromEquipToItemBox(int index, int fromindex,
										  int toindex)
{
	int fromid,toid;

	if( !CHAR_CHECKINDEX( index )) return FALSE;

	fromid  = CHAR_getItemIndex(index,fromindex);
	toid    = CHAR_getItemIndex(index,toindex);
	if( fromid < 0 )
		return FALSE;
	else if( toid < 0 ){
		CHAR_setItemIndex(index,toindex,fromid);
		CHAR_setItemIndex(index,fromindex,-1);
		CHAR_sendItemDetachEvent( index,fromid );
		CHAR_sendSIToCli( index,fromindex,toindex);
		return TRUE;
	}

	return CHAR_moveItemFromItemBoxToEquip(index, toindex,fromindex);
}

#ifdef _ITEM_PILENUMS
int CHAR_getMyMaxPilenum( int charaindex)
{
	int maxpile;
	int transnum = CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
	if(CHAR_getInt(charaindex,CHAR_VIPRIDE)>1)
		{
			maxpile=100;
		}
	else
		{
			if(transnum <= 5)
				{
					maxpile = transnum
						+ (transnum/5) * 2 + 3;
				}
			else if(transnum > 5)
				{
					maxpile = 50;
				}
		}

/*
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	maxpile += CHAR_getInt( charaindex, ATTACHPILE );
#endif
*/

#ifdef _EQUIT_ADDPILE
	maxpile += CHAR_getWorkInt( charaindex, CHAR_WORKATTACHPILE);
	maxpile = (maxpile<0)?0:maxpile;
#endif
	return maxpile;
}

BOOL CHAR_PileItemFromItemBoxToItemBox( int charaindex, int fromindex, int toindex)
{
	int maxpile, fromid, toid;
	if( !CHAR_CHECKINDEX( charaindex) ) return FALSE;
	fromid = CHAR_getItemIndex( charaindex, fromindex);
	toid = CHAR_getItemIndex( charaindex, toindex);

	maxpile = CHAR_getMyMaxPilenum( charaindex);

	if( ITEM_CHECKINDEX( toid) && ITEM_CHECKINDEX( fromid) &&
		(ITEM_getInt( toid, ITEM_ID) == ITEM_getInt( fromid, ITEM_ID)) &&
		(ITEM_getInt( toid, ITEM_CANBEPILE) == 1) &&
		(ITEM_getInt( toid, ITEM_USEPILENUMS) < maxpile) &&
		(ITEM_getInt( fromid, ITEM_USEPILENUMS) < maxpile) ){
		int formpilenum , pilenum, defpilenum;

		pilenum = ITEM_getInt( toid, ITEM_USEPILENUMS);
		formpilenum = ITEM_getInt( fromid, ITEM_USEPILENUMS);
		defpilenum = ((maxpile-pilenum)>formpilenum)? formpilenum:(maxpile-pilenum);
		formpilenum = formpilenum - defpilenum;
		pilenum = pilenum + defpilenum;
		ITEM_setInt( toid, ITEM_USEPILENUMS, pilenum);
		ITEM_setInt( fromid, ITEM_USEPILENUMS, formpilenum);

		if( formpilenum <= 0 ){
			CHAR_setItemIndex( charaindex, fromindex ,-1);
			ITEM_endExistItemsOne( fromid);
		}
		return TRUE;
	}
	return FALSE;
}
#endif

static BOOL CHAR_moveItemFromItemBoxToItemBox( int index, int fromindex, int toindex)
{
	int     fromid, toid;

	if( !CHAR_CHECKINDEX( index ) ) return FALSE;
#ifdef _ITEM_PILENUMS
	if( CHAR_PileItemFromItemBoxToItemBox( index, fromindex, toindex) == TRUE ){
		CHAR_sendItemDataOne( index, toindex);
		CHAR_sendItemDataOne( index, fromindex);
		return TRUE;
	}
#endif
	fromid = CHAR_getItemIndex( index, fromindex);
	toid = CHAR_getItemIndex( index, toindex);
#ifdef	_ITEM_RESET
	char *itemarg = NULL;
	itemarg = ITEM_getChar(fromid,ITEM_ARGUMENT);
	if(strlen(itemarg)>0){
		int itemid;
		int playfd = getfdFromCharaIndex(index);
		if(strcmp(itemarg,"ITEMRESET")==0){
			itemid = ITEM_getInt(toid,ITEM_ID);
			int resetpoint = sasql_itemreset_query(itemid);
			if(resetpoint>=0){
				char resetmsg[256];
    		sprintf(resetmsg,"\n您确定需要还原【%s】么？\n\n还原此物品需要【%d】点\n\n确定的话请点击【OK】\n",ITEM_getChar(toid,ITEM_NAME),resetpoint);
				lssproto_WN_send( playfd, WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OKCANCEL,
            CHAR_WINDOWTYPE_RESETITEM,
            -1,
            resetmsg);
        CHAR_setWorkInt(index,CHAR_WORKFROMITEMINDEX,fromindex);
        CHAR_setWorkInt(index,CHAR_WORKTOITEMINDEX,toindex);
        return	FALSE;
			}else{
				if(itemid>=0){
					lssproto_WN_send( playfd, WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_CANCEL,
            -1,
            -1,
            "此道具不能还原！");
        }
			}
		}else if(strcmp(itemarg,"ITEMDEL")==0){
			itemid = ITEM_getInt(toid,ITEM_ID);
			int resetpoint = sasql_itemdel_query(itemid);
			if(resetpoint>=0){
				char resetmsg[256];
    		sprintf(resetmsg,"\n您确定需要回收【%s】么？\n\n此物品回收可得【%d】点\n\n确定的话请点击【OK】\n",ITEM_getChar(toid,ITEM_NAME),resetpoint);
				lssproto_WN_send( playfd, WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OKCANCEL,
            CHAR_WINDOWTYPE_DELITEM,
            -1,
            resetmsg);
        CHAR_setWorkInt(index,CHAR_WORKFROMITEMINDEX,fromindex);
        CHAR_setWorkInt(index,CHAR_WORKTOITEMINDEX,toindex);
        return	FALSE;
			}else{
				if(itemid>=0){
					lssproto_WN_send( playfd, WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_CANCEL,
            -1,
            -1,
            "此道具不能回收！");
        }
			}
		}
	}
#endif
	CHAR_setItemIndex(index,toindex, CHAR_setItemIndex( index, fromindex, toid));
	CHAR_sendSIToCli( index,fromindex,toindex);

	return TRUE;
}

void CHAR_moveEquipItem( int index, int fromindex, int toindex )
{
#define		MOVEITEM_NONE			0
#define		MOVEITEM_EQUIPTOITEM	1 << 0
#define		MOVEITEM_ITEMTOEQUIP	1 << 1
#define		MOVEITEM_ITEMTOITEM		1 << 2

	int     fromid;            /*  item配列でのインデックス    */


	unsigned int     moved_any = 0;
	while( 1) {
 		if( !CHAR_CHECKINDEX( index ) )break;
		if( CHAR_getFlg( index,CHAR_ISDIE ) )break;
		if( fromindex < 0 || fromindex >= CHAR_MAXITEMHAVE ||
			toindex < 0 || toindex >= CHAR_MAXITEMHAVE )
			break;
		
		fromid  = CHAR_getItemIndex(index,fromindex);
		if( fromid == -1 || fromid == -2 )break;
		if( fromindex == toindex )break;
		if( fromindex < CHAR_EQUIPPLACENUM ){
			if( toindex  < CHAR_EQUIPPLACENUM ){
				if( CHAR_getItemIndex(index,toindex) < 0 ){
					CANNOTMOVE;
				}else{
					CANNOTEXCHANGE;
				}

			}else {
				moved_any |= CHAR_moveItemFromEquipToItemBox(
					index, fromindex, toindex )
					? MOVEITEM_EQUIPTOITEM : MOVEITEM_NONE;
				if( moved_any & MOVEITEM_EQUIPTOITEM) {
					char	category[3];
					snprintf( category,sizeof( category),"J%d", fromindex);
					CHAR_sendStatusString( index , category);
				}
			}
		}else{
			if( toindex  < CHAR_EQUIPPLACENUM ) {
				moved_any |= CHAR_moveItemFromItemBoxToEquip(
					index, fromindex , toindex )
					? MOVEITEM_ITEMTOEQUIP : MOVEITEM_NONE;
				if( moved_any & MOVEITEM_ITEMTOEQUIP) {
					char	category[3];
					snprintf( category,sizeof( category),"J%d", toindex);
					CHAR_sendStatusString( index , category);
				}
			}else	{
				moved_any |= CHAR_moveItemFromItemBoxToItemBox(	index,fromindex,toindex )
					? MOVEITEM_ITEMTOITEM : MOVEITEM_NONE;
			}
		}
		CHAR_complianceParameter( index );
		CHAR_sendCToArroundCharacter(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX));
		break;
	}

	if( (moved_any & MOVEITEM_EQUIPTOITEM) || (moved_any & MOVEITEM_ITEMTOEQUIP)){
		int i;
		CHAR_send_P_StatusString( index,
								  CHAR_P_STRING_HP|CHAR_P_STRING_MAXHP|
								  CHAR_P_STRING_MP|CHAR_P_STRING_MAXMP|
								  CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
								  CHAR_P_STRING_QUICK|CHAR_P_STRING_CHARM|
								  CHAR_P_STRING_LUCK|CHAR_P_STRING_EARTH|
								  CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
								  CHAR_P_STRING_WIND
								  );
/*
		if( CHAR_getWorkInt( index,CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){
			for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
				int petindex = CHAR_getCharPet( index, i);
				if( CHAR_CHECKINDEX( petindex) ) {
					char	category[3];
					CHAR_complianceParameter( petindex );
					snprintf( category, sizeof( category), "K%d",i);
					CHAR_sendStatusString( index, category );
				}
			}
		}
*/
	}
	if( !moved_any ) {
		CHAR_sendSIToCli( index,-1,-1);
	}
}

int CHAR_addItemToChar( Char* ch, int itemid )
{
	int     itembox;
	itembox = CHAR_findEmptyItemBoxFromChar( ch );
	if( itembox == -1 )return -1;

	return ch->indexOfExistItems[itembox]=ITEM_makeItemAndRegist(itemid);
}

int CHAR_addItem( int charaindex, int itemid )
{
	Char*   ch;
	ch = CHAR_getCharPointer(charaindex);
	if( ch != NULL ){
		int ret = CHAR_addItemToChar( ch,itemid );
		if( ret == -1 )return ret;
		ITEM_setWorkInt(ret,ITEM_WORKCHARAINDEX,charaindex);
		ITEM_setWorkInt(ret,ITEM_WORKOBJINDEX,-1);
		return ret;
	}else
		return -1;
}


void CHAR_ItemUse( int charaindex, int to_charaindex, int haveitemindex )
{
	void (*usefunc)(int,int,int)=NULL;
	int itemindex;
	if( !CHAR_CHECKINDEX( charaindex ) ){
		print( "ANDY charaindex=%d err\n", charaindex);
		return;
	}
	if( CHAR_CHECKITEMINDEX( charaindex, haveitemindex) == FALSE ){
		print( "ANDY haveitemindex=%d err\n", haveitemindex);
		return;
	}
	itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( !ITEM_CHECKINDEX(itemindex) ){
//		print( "ANDY itemindex=%d err\n", itemindex);
		return;
	}
	int myviptype = CHAR_getInt(charaindex,CHAR_VIPRIDE);
	if(myviptype==0){
		if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP")!=NULL){
			CHAR_talkToCli( charaindex, -1, "会员等级不够，无法使用此装备。", CHAR_COLORYELLOW );
			return;
		}
	}else if(myviptype==1){
		if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP2]")!=NULL || strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL){
			CHAR_talkToCli( charaindex, -1, "会员等级不够，无法使用此装备。", CHAR_COLORYELLOW );
			return;
		}
	}else if(myviptype==2){
		if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL){
			CHAR_talkToCli( charaindex, -1, "会员等级不够，无法使用此装备。", CHAR_COLORYELLOW );
			return;
		}
	}
	if( CHAR_getFlg( charaindex, CHAR_ISDIE ) ){
		print( "ANDY charaindex=%d is CHAR_ISDIE err\n", charaindex);
		return;
	}
#ifdef _STREET_VENDOR
	// 摆摊中不可使用道具
	if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) > -1){
		print("StreetVendor use item charaindex=%d,name=%s\n",charaindex,CHAR_getChar(charaindex,CHAR_NAME));
		return;
	}
#endif
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ) {
		if( CHAR_getInt( to_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
			int action = ITEM_getInt( itemindex, ITEM_USEACTION);
			if( action != -1 ) {
				CHAR_sendAction( to_charaindex, action, TRUE);
			}
		}
	}
	//不等於料理 和 其他
	if( ITEM_getInt( itemindex,ITEM_TYPE) != ITEM_OTHER &&
		ITEM_getInt( itemindex,ITEM_TYPE) != ITEM_DISH ) {
		CHAR_EquipPlace ep;
		
		ep = ITEM_getEquipPlace( charaindex, itemindex );
		if( ep == -1 )return;
		if( ep == CHAR_DECORATION1 ) {
			int	flg = FALSE;
			int toitemindex1 = CHAR_getItemIndex( charaindex, ep);
			int	toitemindex2 = CHAR_getItemIndex( charaindex, CHAR_DECORATION2);
			int	searchindex = -1;
			int fromitemtype = ITEM_getInt( itemindex, ITEM_TYPE);

			if( !ITEM_CHECKINDEX( toitemindex1)) searchindex = toitemindex2;
			else if(!ITEM_CHECKINDEX( toitemindex2)) searchindex = toitemindex1;
			if( searchindex != -1 ) {
				if( fromitemtype != ITEM_getInt( searchindex, ITEM_TYPE)) {
					flg = TRUE;
					ep = ( searchindex == toitemindex1)
								? CHAR_DECORATION2:CHAR_DECORATION1;
				}
			}
			if( !flg) {
				if( fromitemtype != ITEM_getInt( toitemindex2, ITEM_TYPE)) {
					ep = CHAR_DECORATION1;
				}else if( fromitemtype != ITEM_getInt( toitemindex1, ITEM_TYPE)) {
					ep = CHAR_DECORATION2;
				}else {
					ep = CHAR_DECORATION1;
				}
			}
		}
		CHAR_moveEquipItem( charaindex, haveitemindex,ep );
		return;
	}

	usefunc = (void(*)(int,int,int))ITEM_getFunctionPointer( itemindex,ITEM_USEFUNC );
	if( usefunc ){
		{
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ), /* キャラ名 */
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
				ITEM_getInt( itemindex, ITEM_ID ),       /* アイテム番号 */
#endif
				"Use(使用道具)",
				CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
 	      		CHAR_getInt( charaindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
			);
		}
		usefunc(charaindex, to_charaindex, haveitemindex);
	}else{
#ifdef _ALLBLUES_LUA_1_2 
		if(RunItemUseEvent(itemindex, charaindex, to_charaindex, haveitemindex) == FALSE )
#endif
{
		char ansmsg[256];
		strcpysafe( ansmsg, sizeof(ansmsg),
					"什麽也没有发生。");
		CHAR_talkToCli( charaindex, -1, ansmsg, CHAR_COLORWHITE );
	}
	}
}

BOOL CHAR_DropItemFXY( int charaindex, int itemcharaindex, int fl,
							  int x, int y, int* objindex )
{
	int itemindex;
	itemindex = CHAR_getItemIndex(charaindex,itemcharaindex);
	if( ITEM_CHECKINDEX(itemindex) == FALSE )return -3;
	if( MAP_walkAbleFromPoint( fl,x,y, FALSE ) == FALSE ){
		return -2;
	}

	{
		OBJECT  object;
		for( object=MAP_getTopObj(fl,x,y) ; object; object = NEXT_OBJECT(object ) ){
			int objindex = GET_OBJINDEX(object);
			switch( OBJECT_getType(objindex) ){
			case OBJTYPE_NOUSE:
				break;
			case OBJTYPE_ITEM:
#ifdef _DROPSTAKENEW
				if(ITEM_getInt(itemindex, ITEM_ID) == CASINOTOKENID)
				{
					int i = 0;
					int dropflag = 0;
					ITEM_Item *itm;
					for(i = 0; i < arraysizeof(casinomap); i++){
						if(CHAR_getInt(charaindex, CHAR_FLOOR) == casinomap[i].casinofl){
							if(x == casinomap[i].casinox && y == casinomap[i].casinoy){
								dropflag = 1;
								break;
							}
						}
					}
					if(dropflag == 1){
						if( (itm = ITEM_getItemPointer( itemindex)) == NULL ) return -1;
							strcpysafe( itm->string[ITEM_WATCHFUNC].string,
								sizeof(itm->string[ITEM_WATCHFUNC].string), "ITEM_DeleteTimeWatched" );

						break;
					}else{

						return -1;
					}
				}
#endif
			case OBJTYPE_GOLD:
				{

				return -1;
				}
			case OBJTYPE_CHARA:
			{
				typedef BOOL (*ITEMPUTFUNC)(int,int);
				ITEMPUTFUNC ipfunc;
				ipfunc = (ITEMPUTFUNC)CHAR_getFunctionPointer(
					OBJECT_getIndex(objindex),CHAR_ITEMPUTFUNC) ;

				if( ipfunc && ipfunc(OBJECT_getIndex(objindex),
									 itemindex) == TRUE)
					return -3;
				break;
			}
			default:
				break;
			}
		}

		*objindex = CHAR_DropItemAbsolute( itemindex,fl,x,y, FALSE );

		if( *objindex == -1 )
			return -3;
		{
			char tmpbuf[256];
			sprintf(tmpbuf,"丢弃 %s (道具消失时间%d秒)。",ITEM_getChar( itemindex, ITEM_NAME),getItemdeletetime());
			CHAR_talkToCli( charaindex, -1, tmpbuf, CHAR_COLORYELLOW );
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ), /* キャラ名   */
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
				ITEM_getInt( itemindex, ITEM_ID ),  /* アイテム番号 */
#endif
				"Drop(丢出道具)",
			   	CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
 	      		CHAR_getInt( charaindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)

			);
		}
		return 0;
	}
}


#ifdef _ITEM_PILENUMS	//andy_edit 2003/04/01

void CHAR_SendDropItem_Stats( int charaindex, int itemindex, int itemcharaindex, int flg)
{
	if( flg == 1 ){
		CHAR_setItemIndex( charaindex, itemcharaindex , -1);
	}
	CHAR_sendItemDataOne( charaindex, itemcharaindex);

	if( CHAR_complianceParameter( charaindex ) ){
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX));
	}

	if( 0 <= itemcharaindex && itemcharaindex < CHAR_STARTITEMARRAY &&
		CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){	//如果丢弃物为装备
		if( ITEM_CHECKINDEX( itemindex) )
			CHAR_sendItemDetachEvent( charaindex, itemindex );

		CHAR_send_P_StatusString( charaindex,
								  CHAR_P_STRING_HP|CHAR_P_STRING_MAXHP|
								  CHAR_P_STRING_MP|CHAR_P_STRING_MAXMP|
								  CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
								  CHAR_P_STRING_QUICK|CHAR_P_STRING_CHARM|
								  CHAR_P_STRING_LUCK|CHAR_P_STRING_EARTH|
								  CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
								  CHAR_P_STRING_WIND );
	}
}
#ifdef _DROPSTAKENEW
void CHAR_DropStakeByDropItem( int charaindex, int itemcharaindex, int itemindex, int fl, int x, int y)
{
	int j, k, casinoflag = 0, dropflag;
	char tmpbuf[256];
	dropflag = CHAR_getWorkInt( charaindex, CHAR_WORKSTAKEFLAG);
	if( dropflag >= MAXSTAKENUM){
		snprintf( tmpbuf, sizeof( tmpbuf), "你已经下注五次了，无法再下注！");
		CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORYELLOW);
		return;
	}
	for (j = 0; j < arraysizeof( casinomap); j++){
		if( CHAR_getInt( charaindex, CHAR_FLOOR) != casinomap[j].casinofl ) continue;
		if( x != casinomap[j].casinox || y != casinomap[j].casinoy ) continue;
		if( casinomap[j].dropflag == 0 ){
			CHAR_talkToCli( charaindex, -1, "现在无法下注！", CHAR_COLORYELLOW);
			return;
		}else{
			int objindex;
			objindex = CHAR_DropItemAbsolute( itemindex, fl, x, y, FALSE );
			if( objindex == -1 ) return;
			ITEM_setInt( itemindex, ITEM_PUTTIME, NowTime.tv_sec+30*60);
			CHAR_sendWatchEvent( objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
			casinoflag = 1;
			snprintf(tmpbuf, sizeof(tmpbuf), "你在 %s 下注了一张彩券", casinomap[j].casinoinfo);
			for(k = 0; k < MAXSTAKENUM; k++){//下注设定
				if(CHAR_getWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + k) != 0) continue;
				CHAR_setWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + k, casinomap[j].casinotype);
				dropflag++;
				break;
			}
			CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM
			if (dropflag <= MAXSTAKENUM){ //做扣点的动作
				int nAcc = CHAR_getInt(charaindex, CHAR_GAMBLENUM);                                								
				nAcc -= casinomap[j].accumulation;
				CHAR_setInt(charaindex, CHAR_GAMBLENUM, nAcc);								
			}
#endif
			dropflag = ( dropflag >= MAXSTAKENUM )?MAXSTAKENUM:dropflag;
			CHAR_setWorkInt(charaindex, CHAR_WORKSTAKEFLAG, dropflag);
			break;
		}
	}

	if( casinoflag == 0) return;
		LogItem(CHAR_getChar(charaindex, CHAR_NAME),
				CHAR_getChar(charaindex, CHAR_CDKEY),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
				ITEM_getInt(itemindex, ITEM_ID),
#endif
				"StakeDrop(丢出彩券)",
				CHAR_getInt(charaindex,CHAR_FLOOR),
				CHAR_getInt(charaindex,CHAR_X),
 				CHAR_getInt(charaindex,CHAR_Y),
				ITEM_getChar(itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)
				);
	CHAR_SendDropItem_Stats( charaindex, itemindex, itemcharaindex, 1);

}
#endif

BOOL CHAR_FindAroundUsabilitySpace( int charaindex, int itemindex, int *fl, int *x, int *y)
{
	int dropx, dropy, i;
	BOOL Find = FALSE;
	int myfl = CHAR_getInt( charaindex, CHAR_FLOOR);

	for( i  = 0 ; i < 8 ; i  ++ ){
		OBJECT  object;
		Find = FALSE;
		dropx = CHAR_getInt( charaindex, CHAR_X) + CHAR_getDX( CHAR_getInt( charaindex, CHAR_DIR) + i);
		dropy = CHAR_getInt( charaindex, CHAR_Y) + CHAR_getDY( CHAR_getInt( charaindex, CHAR_DIR) + i);
		if( MAP_walkAbleFromPoint( myfl, dropx, dropy, FALSE ) == FALSE ) continue;
		for( object = MAP_getTopObj( myfl, dropx, dropy) ; object ; object = NEXT_OBJECT(object ) ){
			int objindex = GET_OBJINDEX(object);
			if( !CHECKOBJECTUSE( objindex)) continue;
			if( OBJECT_getType( objindex) == OBJTYPE_CHARA ){
				typedef BOOL (*ITEMPUTFUNC)(int,int);
				ITEMPUTFUNC ipfunc;
				ipfunc = (ITEMPUTFUNC)CHAR_getFunctionPointer( OBJECT_getIndex(objindex),CHAR_ITEMPUTFUNC) ;
				if( ipfunc && ipfunc( OBJECT_getIndex( objindex), itemindex) == TRUE)
					return FALSE;
			}else{
				Find = TRUE;
				break;
			}
		}
		if( Find == FALSE ){
			*fl = myfl;
			*x  = dropx;
			*y  = dropy;
			return TRUE;
		}
	}
	return FALSE;
}

void CHAR_DropItem( int charaindex,  int itemcharaindex )
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKSAFEMODE)==0)
			{
				CHAR_SafePassword(charaindex);
				return;
			}
	int itemindex, fl, x, y, ret, objindex, beDropOne = 1;

	if( !CHAR_CHECKINDEX( charaindex ) ) return;
	int fd = getfdFromCharaIndex( charaindex);
  if( fd == - 1 ) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ){
		CHAR_talkToCli( charaindex, -1, "战斗状态中无法丢道具装备。", CHAR_COLORYELLOW );
		return;
	}
#ifdef _AVID_TRADETRYBUG  //丢道具装备
	if( CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE){
		CHAR_talkToCli( charaindex, -1, "交易状态中无法丢道具装备。", CHAR_COLORYELLOW );
		return;
	}
#endif

	itemindex = CHAR_getItemIndex( charaindex, itemcharaindex);
	if( !ITEM_CHECKINDEX( itemindex)) return;
#ifdef _LOCK_PET_ITEM
	char *arg = ITEM_getChar(itemindex, ITEM_NAME);
	if(arg[0] == '*'){
		CHAR_talkToCli( charaindex, -1, "绑定的物品不能丢弃！", CHAR_COLORYELLOW );
		return;
	}
#endif
#ifdef	_NO_DROPITEM		
	if( strstr(ITEM_getChar(itemindex,ITEM_EFFECTSTRING),"绑定")){
		if(CHAR_getWorkInt(charaindex,CHAR_DROPITEMCHARAINDEX)!=itemcharaindex){
			char dropmsg[256];
    	sprintf(dropmsg,"\n【%s】为绑定物品\n\n丢弃会直接消失！\n\n你确定要丢弃么？确定请按[OK]\n",ITEM_getChar(itemindex,ITEM_NAME));
    	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
    		WINDOW_BUTTONTYPE_OKCANCEL,
      	CHAR_WINDOWTYPE_DROPITEM,
      	-1,
      	dropmsg);
      CHAR_setWorkInt(charaindex,CHAR_DROPITEMCHARAINDEX,itemcharaindex);
      return;
    }else{
    	CHAR_setWorkInt(charaindex,CHAR_DROPITEMCHARAINDEX,-1);
    }
	}
#endif	
	if(ITEM_getInt(itemindex,ITEM_TIME)>0){
		if(CHAR_getWorkInt(charaindex,CHAR_DROPITEMCHARAINDEX)!=itemcharaindex){
			char dropmsg[256];
    	sprintf(dropmsg,"\n【%s】为限时物品\n\n丢弃会直接消失！\n\n你确定要丢弃么？确定请按[OK]\n",ITEM_getChar(itemindex,ITEM_NAME));
    	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
    		WINDOW_BUTTONTYPE_OKCANCEL,
      	CHAR_WINDOWTYPE_DROPITEM,
      	-1,
      	dropmsg);
      CHAR_setWorkInt(charaindex,CHAR_DROPITEMCHARAINDEX,itemcharaindex);
      return;
    }else{
    	CHAR_setWorkInt(charaindex,CHAR_DROPITEMCHARAINDEX,-1);
    }
	}
	if( ITEM_getInt( itemindex, ITEM_ID) == CASINOTOKENID &&
		( CHAR_getInt( charaindex, CHAR_FLOOR ) == 7008 ||
		  CHAR_getInt( charaindex, CHAR_FLOOR ) == 7005 ||
		  CHAR_getInt( charaindex, CHAR_FLOOR ) == 7003 ) ){

		int dropx, dropy, dropfl;
		dropfl = CHAR_getInt( charaindex, CHAR_FLOOR);
		dropx = CHAR_getInt( charaindex, CHAR_X) + CHAR_getDX( CHAR_getInt( charaindex, CHAR_DIR));
		dropy = CHAR_getInt( charaindex, CHAR_Y) + CHAR_getDY( CHAR_getInt( charaindex, CHAR_DIR));
		CHAR_DropStakeByDropItem( charaindex, itemcharaindex, itemindex, dropfl, dropx, dropy);
		return;
	}
#ifdef _PETITEM__AMOI_E
		//print ("\n%d,%d,%d\n",ITEM_NOT_DROP,ITEM_getInt(itemindex, ITEM_TYPEOFITEM),ITEM_NOT_DROP & ITEM_getInt(itemindex, ITEM_TYPEOFITEM));
		if(ITEM_NOT_DROP & ITEM_getInt(itemindex, ITEM_TYPEOFITEM)){
			char buf[256];
			sprintf( buf, "%s无法丢弃。", ITEM_getChar( itemindex, ITEM_NAME) );
			CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORRED);
			return ;
		}
#endif
	//找出周围空间
	if( CHAR_FindAroundUsabilitySpace( charaindex, itemindex, &fl, &x, &y) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "周围的地面已经满了。", CHAR_COLORYELLOW );
		return;
	}
	
#ifdef _ITEM_PILENUMS
	if( ITEM_getInt( itemindex, ITEM_CANBEPILE) == 1 &&
		ITEM_getInt( itemindex, ITEM_USEPILENUMS) > 1 ){
		int ret;
		int nums = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
		ret = ITEM_makeItemAndRegist( ITEM_getInt( itemindex, ITEM_ID) );
		if( !ITEM_CHECKINDEX( ret)) return;
		ITEM_setInt( itemindex, ITEM_USEPILENUMS, nums -1 );
		itemindex = ret;
		beDropOne = 0;
	}
#endif
	//找到空间
	ret = ITEM_eventDrop( itemindex, charaindex, itemcharaindex );
	if( ret == 1 || ITEM_getInt(itemindex,ITEM_TIME)>0){	//-1 物品不存在 1 消失 0 一般物品
		CHAR_SendDropItem_Stats( charaindex, itemindex, itemcharaindex, beDropOne);
		return;
	}else {
		objindex = CHAR_DropItemAbsolute( itemindex, fl, x, y, FALSE );
		if( objindex == -1 ) return;
		ITEM_setInt( itemindex, ITEM_PUTTIME, NowTime.tv_sec);
#ifdef	_PICKUP_ITEM_OTHER
		if(getPickupItem()==1){
			ITEM_setChar(itemindex,ITEM_FROMCDKEY,CHAR_getChar( charaindex, CHAR_CDKEY ));
		}
#endif
		{
			char tmpbuf[256];
			sprintf(tmpbuf,"丢弃 %s (道具消失时间%d秒)。",ITEM_getChar( itemindex, ITEM_NAME),getItemdeletetime());
			CHAR_talkToCli( charaindex, -1, tmpbuf, CHAR_COLORYELLOW );
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ), /* 平乓仿   */
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
				itemindex,
#else
				ITEM_getInt( itemindex, ITEM_ID ),  /* 失奶  丞  寞 */
#endif
				"Drop(丢出道具)",
		   		CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
 	    		CHAR_getInt( charaindex,CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
			);
		}
		CHAR_sendWatchEvent( objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
	}
	CHAR_SendDropItem_Stats( charaindex, itemindex, itemcharaindex, beDropOne);
}
#else
void CHAR_DropItem( int charaindex,  int itemcharaindex )
{
	int dirx[9],diry[9];
	int i, j;
	int floor, objindex=-1;
	int droped =0;
	int count_item =0, count_chara =0;
	int fl, x, y;
	int itemindex = CHAR_getItemIndex(charaindex,itemcharaindex);

	if( !CHAR_CHECKINDEX( charaindex ) ) return;
	if( !ITEM_CHECKINDEX( itemindex) ) return;
#ifdef _AVID_TRADETRYBUG  //丢道具装备
	if( CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE){
		CHAR_talkToCli( charaindex, -1, "交易状态中无法丢道具装备。", CHAR_COLORYELLOW );
		return;
	}
#endif

#ifdef _DROPCHECK2
	fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	x = CHAR_getInt( charaindex, CHAR_X);
	y = CHAR_getInt( charaindex, CHAR_Y);

	for( i = x-CHAR_DEFAULTSEESIZ/2 ; i <= x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j = y-CHAR_DEFAULTSEESIZ/2 ; j <= y+CHAR_DEFAULTSEESIZ/2 ; j ++ ){
		OBJECT  object;
		for( object = MAP_getTopObj(fl,i,j); object ; object = NEXT_OBJECT(object ) ) {
			int objindex = GET_OBJINDEX(object);
			if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
			if( OBJECT_getType(objindex) == OBJTYPE_ITEM || OBJECT_getType(objindex) == OBJTYPE_GOLD ) {
				count_item++;
			}
			if( OBJECT_getType(objindex) == OBJTYPE_CHARA ) {
				count_chara++;
			}
		}
	}

#ifdef _DROPSTAKENEW
		if(ITEM_getInt(itemindex, ITEM_ID) == CASINOTOKENID){
			int k = 0;
			int casinodropflag = 0;
			for(k = 0; k < arraysizeof(casinomap); k++){
				if(fl == casinomap[k].casinofl){
					if(x >= casinomap[k].casinox - 1 && x <= casinomap[k].casinox + 1){
						if(y >= casinomap[k].casinoy - 1 && y <= casinomap[k].casinoy + 1){
							casinodropflag = 1;
							break;
						}
					}
				}
			}
			if(casinodropflag == 0){
#endif
		if( count_item > 80 || count_chara > 80 ) {
			CHAR_talkToCli( charaindex, -1, "这里的物品已经太多了，不能再丢了。", CHAR_COLORYELLOW );
			return;
		}
#ifdef _DROPSTAKENEW
			}
		}
#endif
	}
#endif

	for( i  = 1 ; i < 8 ; i  ++ ){
		dirx[i] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR) + i);
		diry[i] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR) + i);
	}
	dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
	diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
	dirx[8] = 0;
	diry[8] = 0;
	floor = CHAR_getInt( charaindex,CHAR_FLOOR );
	{
		int     ret;
		if( 0 <= itemcharaindex && itemcharaindex < CHAR_STARTITEMARRAY ){	//如果丢弃物为装备
			CHAR_sendItemDetachEvent( charaindex, itemindex );
		}
		ret = ITEM_eventDrop( itemindex, charaindex, itemcharaindex );
		if( ret == 1 ){	//-1 物品不存在 1 消失 0 一般物品
			itemindex = -1;
			goto END;
		}else if( ret == -1 ){
			print( "%s:%d err\n", __FILE__,__LINE__);
			return;
		}else{
			;
		}
	}

	droped = 0;
	for( i = 0 ; i < 9 ; i ++ ){
		int x=CHAR_getInt(charaindex,CHAR_X)+dirx[i];
		int y=CHAR_getInt(charaindex,CHAR_Y)+diry[i];
#ifdef _DROPSTAKENEW
		// 判断物品是否为彩券
		if(ITEM_getInt(itemindex, ITEM_ID) == CASINOTOKENID){
			int j = 0, k = 0;
			int bdropstake = 0;
			int casinoflag = 0;
			for (j = 0; j < arraysizeof(casinomap); j++){
				if (CHAR_getInt(charaindex, CHAR_FLOOR) == casinomap[j].casinofl){
					if (x == casinomap[j].casinox && y == casinomap[j].casinoy){
						char tmpbuf[256];
						int dropflag = CHAR_getWorkInt(charaindex, CHAR_WORKSTAKEFLAG);
						casinoflag = 1;
						if(casinomap[j].dropflag == 0){
							CHAR_talkToCli(charaindex, -1, "现在无法下注！", CHAR_COLORYELLOW);
							return;
						}else{
							snprintf(tmpbuf, sizeof(tmpbuf), "你在 %s 下注了一张彩券", casinomap[j].casinoinfo);
							if(dropflag >= MAXSTAKENUM){
								snprintf(tmpbuf, sizeof(tmpbuf), "你已经下注五次了，无法再下注！");
								CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORYELLOW);
								return;
							}
							for(k = 0; k < MAXSTAKENUM; k++){
								if(CHAR_getWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + k) == 0){
									CHAR_setWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + k, casinomap[j].casinotype);
									casinomap[j].stakenum = casinomap[j].stakenum + 1;
									if(casinomap[j].stakenum >= 100000000)
										casinomap[j].stakenum = 100000000;
									dropflag++;
									if(casinomap[j].stakenum <= 1)
										bdropstake = 1;
									break;
								}
							}
						}
						CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORYELLOW);
#ifdef _FIX_GAMBLENUM		
						//做扣点的动作
						if (dropflag <= MAXSTAKENUM){
							int nAcc = CHAR_getInt(charaindex, CHAR_GAMBLENUM);                                								
							nAcc -= casinomap[j].accumulation;
							CHAR_setInt(charaindex, CHAR_GAMBLENUM, nAcc);								
						}
#endif
						if (dropflag >= MAXSTAKENUM){
							dropflag = 0;
							for (k = 0; k < MAXSTAKENUM ; k++){
								if (CHAR_getWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + k) > 0)
									dropflag++;
							}
							CHAR_setWorkInt(charaindex, CHAR_WORKSTAKEFLAG, dropflag);
						}else{
							CHAR_setWorkInt(charaindex, CHAR_WORKSTAKEFLAG, dropflag);
						}
						continue;
					}
				}
			}

			// 此地方的彩券数量超过一张以上
			if(bdropstake == 0 && casinoflag != 0){
				CHAR_setItemIndex(charaindex, itemcharaindex ,-1);
				CHAR_sendItemDataOne(charaindex, itemcharaindex);
				LogItem(CHAR_getChar(charaindex, CHAR_NAME),
						CHAR_getChar(charaindex, CHAR_CDKEY),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
						itemindex,
#else
						ITEM_getInt(itemindex, ITEM_ID),
#endif
						"StakeDrop(丢出彩券)",
						CHAR_getInt(charaindex,CHAR_FLOOR),
						CHAR_getInt(charaindex,CHAR_X),
 						CHAR_getInt(charaindex,CHAR_Y),
						ITEM_getChar(itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)

						);
				return;
			}
		}
#endif
		{
			int ret = CHAR_DropItemFXY( charaindex,itemcharaindex,
									CHAR_getInt(charaindex,CHAR_FLOOR), x, y,  &objindex );
			switch( ret ){
			case 0:
					droped = 1;
#ifdef _ITEM_ORNAMENTS	// WON FIX
			if( ITEM_getWorkInt( itemindex, ITEM_CANPICKUP) > 0 ){
				ITEM_setInt( itemindex, ITEM_PUTTIME, NowTime.tv_sec + 60*5);
			}else{
				ITEM_setInt( itemindex, ITEM_PUTTIME, NowTime.tv_sec);
			}
#else
			ITEM_setInt( itemindex, ITEM_PUTTIME, NowTime.tv_sec);
#endif
				goto END;
				break;
			case -1:
			case -2:
				break;
			case -3:
			default:
				return;
				break;
			}
#ifdef _DROPSTAKENEW
		}
#endif
	}
#ifdef _DROPCHECK	
	if( droped != 1 ) {
		CHAR_talkToCli( charaindex, -1, "周围的地面已经满了。", CHAR_COLORYELLOW );
		return;
	}
#endif
END:
	CHAR_sendWatchEvent( objindex, CHAR_ACTSTAND, NULL, 0, TRUE);
	CHAR_setItemIndex( charaindex, itemcharaindex ,-1);
{
	char	category[3];
	snprintf( category,sizeof( category),"J%d", itemcharaindex);
	CHAR_sendStatusString( charaindex , category);
}
	if( CHAR_complianceParameter( charaindex ) ){
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX) );
	}
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		CHAR_send_P_StatusString( charaindex,
								  CHAR_P_STRING_HP|CHAR_P_STRING_MAXHP|
								  CHAR_P_STRING_MP|CHAR_P_STRING_MAXMP|
								  CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
								  CHAR_P_STRING_QUICK|CHAR_P_STRING_CHARM|
								  CHAR_P_STRING_LUCK|CHAR_P_STRING_EARTH|
								  CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
								  CHAR_P_STRING_WIND
								);
        CHAR_sendItemDataOne( charaindex, itemcharaindex);
	}
	return;
}
#endif

int CHAR_DropItemAbsolute( int itemindex, int floor, int x, int y,
						   BOOL net)
{
	Object object;
	int objindex;

	if( !ITEM_CHECKINDEX( itemindex) )return FALSE;

	object.type = OBJTYPE_ITEM;
	object.index = itemindex;
	object.x = x;
	object.y = y;
	object.floor = floor;
	objindex = initObjectOne( &object );
	ITEM_setWorkInt( itemindex,ITEM_WORKOBJINDEX, objindex );
	ITEM_setWorkInt( itemindex,ITEM_WORKCHARAINDEX, -1 );
	if( net )
		CHAR_sendWatchEvent( objindex,CHAR_ACTSTAND,NULL,0,TRUE);

	return objindex;
}

int  CHAR_addItemSpecificItemIndex( int charaindex, int itemindex )
{
	int emptyindex;
	if( CHAR_CHECKINDEX( charaindex) == FALSE) return -1;
	if( ITEM_CHECKINDEX( itemindex) == FALSE) return -1;
	emptyindex = CHAR_findEmptyItemBox( charaindex );
	if( emptyindex == -1 ){
		return CHAR_MAXITEMHAVE;
	}else{
		CHAR_setItemIndex( charaindex, emptyindex, itemindex );
		ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
		ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charaindex);
		CHAR_sendItemDataOne( charaindex, emptyindex);
	}
	return emptyindex;
}

static int CHAR_PickUpItemFXY( int charaindex, int fl ,int x , int y ,
							   int* contents,int* objindex )
{
	OBJECT  object;

	for( object=MAP_getTopObj(fl,x,y) ; object ; object =  NEXT_OBJECT(object ) ){
		int index = GET_OBJINDEX(object);
		int objtype = OBJECT_getType(index);
		if( objtype == OBJTYPE_NOUSE )continue;
		else if( objtype == OBJTYPE_ITEM ){
			int ret;
			int itemindex = OBJECT_getIndex( index);
#ifdef _ITEM_ORNAMENTS
			if( ITEM_getWorkInt( itemindex, ITEM_CANPICKUP) > 0 ){
				return -1;
			}
#endif
#ifdef _DROPSTAKENEW
			// 判断物品是否为彩券
			if(ITEM_getInt(itemindex, ITEM_ID) == CASINOTOKENID){
				int i = 0;
				int bdropstake = 0, casinoflag = 0, emptyitemindexinchara = -1;
				for(i = 0; i < arraysizeof(casinomap); i++){
					if(fl == casinomap[i].casinofl){
						if(x == casinomap[i].casinox && y == casinomap[i].casinoy){
							int j = 0, stakeflag = 0;
							int pickupflag = 0;
							casinoflag = 1;
							if(casinomap[i].dropflag == 0){
								CHAR_talkToCli(charaindex, -1, "现在已经无法取回下注彩券了！", CHAR_COLORYELLOW);
								return -1;
							}
							stakeflag = CHAR_getWorkInt(charaindex, CHAR_WORKSTAKEFLAG);
							if(stakeflag == 0){
								CHAR_talkToCli(charaindex, -1, "您并没有下注，所以无法取回彩券！", CHAR_COLORYELLOW);
								return -1;
							}
							for(j = 0; j < MAXSTAKENUM; j++){
								if(CHAR_getWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + j) == casinomap[i].casinotype){
									emptyitemindexinchara = CHAR_findEmptyItemBox(charaindex);
								   if(emptyitemindexinchara < 0) return -2;
									CHAR_setWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + j, 0);
									casinomap[i].stakenum = casinomap[i].stakenum - 1;
									stakeflag--;
									pickupflag = 1;
#ifdef _FIX_GAMBLENUM
									//做加点的动作
									{
										int nAcc = CHAR_getInt(charaindex, CHAR_GAMBLENUM);										
										nAcc += casinomap[i].accumulation;
										CHAR_setInt(charaindex, CHAR_GAMBLENUM, nAcc);										
									}
#endif
									if(casinomap[i].stakenum <= 0){
										bdropstake = 1;
										casinomap[i].stakenum = 0;
									}
									break;
								}
							}
							if(stakeflag < 0){
								stakeflag = 0;
								for(j = 0; j < MAXSTAKENUM; j++){
									if(CHAR_getWorkInt(charaindex, CHAR_WORKSTAKETYPE1 + j) > 0)
										stakeflag++;
								}
								CHAR_setWorkInt(charaindex, CHAR_WORKSTAKEFLAG, stakeflag);
							}else
								CHAR_setWorkInt(charaindex, CHAR_WORKSTAKEFLAG, stakeflag);
							if(pickupflag == 0){
								CHAR_talkToCli(charaindex, -1, "您没有在此下注，所以无法取回彩券！", CHAR_COLORYELLOW);
								return -1;
							}
						}
					}
				}
				
				if(bdropstake == 0 && casinoflag != 0){// 此地方的彩券数量超过一张以上
					char tmpbuf[256];
					if(emptyitemindexinchara != -1){
						int itemid=-1, ret=-1;
						itemid = ITEM_makeItemAndRegist(CASINOTOKENID);
						if(itemid == -1) return -1;
						ret = CHAR_addItemSpecificItemIndex( charaindex, itemid);
						if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
							ITEM_endExistItemsOne( itemid );
							return -1;
						}
					}
					CHAR_sendItemDataOne(charaindex, emptyitemindexinchara);
					snprintf(tmpbuf, sizeof(tmpbuf), "拾回%s，已将下注取消！",
						ITEM_getChar(itemindex, ITEM_NAME));
					CHAR_talkToCli(charaindex, -1, tmpbuf, CHAR_COLORWHITE);
					// 记录捡回记录
					LogItem(CHAR_getChar(charaindex, CHAR_NAME),
							CHAR_getChar(charaindex, CHAR_CDKEY),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
							itemindex,
#else
							ITEM_getInt(itemindex, ITEM_ID),
#endif
							"StakePickUp(捡起彩券)",
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
 							CHAR_getInt(charaindex,CHAR_Y),
							ITEM_getChar(itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
							);
					return -1;
				}
			}
#endif
#ifdef	_PICKUP_ITEM_OTHER
			if(getPickupItem()==1){
				char pickupchar[16];
				sprintf(pickupchar,",%d,",ITEM_getInt(itemindex,ITEM_ID));
				if(ITEM_getChar(itemindex,ITEM_FROMCDKEY)!=NULL && strstr(getPickItemId(),pickupchar)==NULL){
					if(strcmp(CHAR_getChar(charaindex,CHAR_CDKEY),ITEM_getChar(itemindex,ITEM_FROMCDKEY))!=0){
						CHAR_talkToCli(charaindex, -1, "此道具不属于您所有，无法拾取。", CHAR_COLORWHITE);
						return -1;
					}
				}
			}
#endif
			ret = CHAR_addItemSpecificItemIndex( charaindex, itemindex );
			if(ret == CHAR_MAXITEMHAVE ){
				return -2;
			}else if( ret != -1 ) {
				typedef void (*PICKUPFUNC)( int, int);
				PICKUPFUNC pickupfunc = NULL;
				pickupfunc = (PICKUPFUNC)ITEM_getFunctionPointer(
									itemindex, ITEM_PICKUPFUNC);
				if( pickupfunc){
					pickupfunc( charaindex, itemindex);
				}
#ifdef _ALLBLUES_LUA_1_2 
				else{
					RunItemPickupEvent(charaindex, itemindex );
				}
#endif		
				*contents = OBJECT_getIndex(index);
				*objindex = index;
//	      CHAR_sendItemDataOne( charaindex, ret);
				return 0;
			}
		}else if( objtype == OBJTYPE_GOLD ){
			int MaxGold;
			long dropgoldamount = 0, newamountgold = 0, Minus = 0, newgold = 0;
			long mygold = (long) CHAR_getInt( charaindex, CHAR_GOLD );
			MaxGold = CHAR_getMaxHaveGold( charaindex);
			if ((mygold <0) || (mygold > MaxGold))return -2;
			dropgoldamount = OBJECT_getIndex(index);
			newamountgold  = (unsigned int) CHAR_getInt( charaindex,CHAR_GOLD ) + dropgoldamount;
			Minus = newamountgold - MaxGold;
			if( Minus > 0 ) {
				newgold = Minus;
				*contents = dropgoldamount - Minus;
			}else {
				newgold = 0;
				*contents = dropgoldamount;
			}
			OBJECT_setIndex( index, newgold);
			CHAR_setInt( charaindex, CHAR_GOLD, min( newamountgold, MaxGold ) );
			*objindex = index;
			return 1;
		}else if( objtype == OBJTYPE_CHARA ) {
			int pindex = OBJECT_getIndex( index);
			if( !CHAR_CHECKINDEX( pindex) ) continue;
			if( CHAR_getInt( pindex, CHAR_WHICHTYPE) == CHAR_TYPEPET ) {
				int havepetindex = CHAR_getCharPetElement( charaindex);
				if( havepetindex == -1 ) {
					if( CHAR_getWorkInt( pindex, CHAR_WORKPETFOLLOWMODE) != CHAR_PETFOLLOW_NOW )
					return -3;
				}
				if( CHAR_getWorkInt( charaindex, CHAR_PickAllPet) != TRUE )	{
					if( (CHAR_getInt( pindex, CHAR_LV) - CHAR_getInt( charaindex, CHAR_LV) > 5 )
						&& (CHAR_getInt( charaindex, CHAR_TRANSMIGRATION ) <= 0 ))	{
						return -4;
					}
				}
#ifdef _PET_UP
				if( getPetup() != 1 && CHAR_getInt(charaindex,CHAR_FLOOR)!=getPetup() )
#endif
				{
					if( !strcmp( CHAR_getChar( pindex, CHAR_OWNERCDKEY), "SYSTEM_WAYI" ) &&
						!strcmp( CHAR_getChar( pindex, CHAR_OWNERCHARANAME), "SYSTEM_WAYI" ) ){
					}else{
						if( strcmp( CHAR_getChar( pindex, CHAR_OWNERCDKEY), CHAR_getChar( charaindex, CHAR_CDKEY) ) ||
							strcmp( CHAR_getChar( pindex, CHAR_OWNERCHARANAME), CHAR_getChar( charaindex, CHAR_NAME) )){
							return -5;
						}
					}
				}
				if( CHAR_getWorkInt( pindex, CHAR_WORKPETFOLLOWMODE) == CHAR_PETFOLLOW_NOW ){
					CHAR_pickupFollowPet( charaindex, pindex );
					return -1;
				}				
				if( CHAR_getInt( pindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE){
					if( strcmp( CHAR_getChar( pindex, CHAR_OWNERCDKEY),
								CHAR_getChar( charaindex, CHAR_CDKEY)) 	!= 0 ||
						strcmp( CHAR_getChar( pindex, CHAR_OWNERCHARANAME),
						CHAR_getChar( charaindex, CHAR_NAME))!= 0 ){
						return -5;
					}else if( CHAR_getInt( pindex, CHAR_MAILMODE) != CHAR_PETMAIL_IDLE5){
						return -5;
					}
				}
				if( CHAR_getInt( pindex, CHAR_MAILMODE) != CHAR_PETMAIL_NONE){
					Char	*ch;
					CHAR_setInt( pindex, CHAR_MAILMODE, CHAR_PETMAIL_NONE);
					CHAR_setInt( pindex, CHAR_PETMAILIDLETIME, 0);
					PETMAIL_delPetMailTotalnums( 1); //手动捡起mail PET
				    ch  = CHAR_getCharPointer( pindex);
				    if( ch != NULL ) {
						strcpysafe( ch->charfunctable[CHAR_LOOPFUNC].string,
									sizeof( ch->charfunctable[CHAR_LOOPFUNC]),	"");
						CHAR_constructFunctable( pindex);
					}
				}
				CHAR_setWorkInt( pindex, CHAR_WORKPLAYERINDEX, charaindex);
				CHAR_setCharPet( charaindex, havepetindex, pindex);
#ifndef _FIX_COPY_MM_BUG			
#ifdef _PET_TALK
				if( CHAR_getInt( pindex, CHAR_PETID) != 718 )	{
#endif
#endif
					CHAR_setChar( pindex, CHAR_OWNERCDKEY,
						CHAR_getChar( charaindex, CHAR_CDKEY));
					CHAR_setChar( pindex, CHAR_OWNERCHARANAME,
						CHAR_getChar( charaindex, CHAR_NAME));
#ifndef _FIX_COPY_MM_BUG	
#ifdef _PET_TALK
				}
#endif		
#endif	
#ifdef _PET_ITEM
				CHAR_sendPetItemData( charaindex, havepetindex);
#endif		
				CHAR_complianceParameter( pindex);
{
				char category[3];
				snprintf( category,sizeof( category),"K%d",havepetindex);
				CHAR_sendStatusString( charaindex, category);
				snprintf( category,sizeof( category),"W%d",havepetindex);
				CHAR_sendStatusString( charaindex, category);
}
				CHAR_setInt( pindex, CHAR_PUTPETTIME, 0);
				*contents = pindex;
				*objindex = index;
				return 2;
			}
		}
	}
	return -1;
}

void CHAR_PickUpItem( int charaindex, int dir )
{
	if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) > 0)
		{
			return;
		}
	int i;
	int dirx[2],diry[2];
	int searchloopcount=0;

	if( !CHAR_CHECKINDEX(charaindex) )return;
	if( CHAR_getFlg( charaindex,CHAR_ISDIE ) )return;
	if( CHAR_getWorkInt( charaindex,CHAR_WORKBATTLEMODE ) )return;
	if( dir < 0 ){
		searchloopcount = 1;
		dirx[0] = 0;
		diry[0] = 0;
	}else{
		if( dir != CHAR_getInt( charaindex, CHAR_DIR)) {
			VALIDATEDIR(dir);
			CHAR_setInt(charaindex,CHAR_DIR,dir);
			CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX),
								CHAR_ACTSTAND,NULL,0,FALSE);
		}
		dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
		diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
		dirx[1] = 0;
		diry[1] = 0;
		searchloopcount=2;
	}
	for( i = 0 ; i < searchloopcount ; i ++ ){
		int contents,objindex;
		int fl = CHAR_getInt(charaindex,CHAR_FLOOR);
		
		int y  = CHAR_getInt(charaindex,CHAR_Y)+diry[i];
		int x  = CHAR_getInt(charaindex,CHAR_X)+dirx[i];
		int ret = CHAR_PickUpItemFXY( charaindex,fl,x,y,&contents, &objindex);
		switch( ret ){
		case 0:
			CHAR_ObjectDelete(objindex);
			{
				char    mesg[256];
#ifdef _DROPSTAKENEW
				int j = 0;
				int dropflag = 0;
				for(j = 0; j < arraysizeof(casinomap); j++)
				{
					if(CHAR_getInt(charaindex, CHAR_FLOOR) == casinomap[j].casinofl)
					{
						if(x == casinomap[j].casinox && y == casinomap[j].casinoy)
						{
							dropflag = 1;
							break;
						}
					}
				}
				if(ITEM_getInt(contents, ITEM_ID) == CASINOTOKENID && dropflag == 1)
					snprintf( mesg,sizeof(mesg), "拾回%s，已将下注取消！",
					ITEM_getAppropriateName(contents));
				else
#endif
				snprintf( mesg,sizeof(mesg), "拾获 %s",
						  ITEM_getAppropriateName(contents) );
				CHAR_talkToCli(charaindex,-1,mesg,CHAR_COLORWHITE);
			}

			{
				LogItem(
					CHAR_getChar( charaindex, CHAR_NAME ),
					CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
					contents,
#else
					ITEM_getInt(contents, ITEM_ID),
#endif
					"PickUp(捡起道具)",
					CHAR_getInt( charaindex,CHAR_FLOOR),
					CHAR_getInt( charaindex,CHAR_X ),
 	      			CHAR_getInt( charaindex,CHAR_Y ),
					ITEM_getChar( contents, ITEM_UNIQUECODE),
					ITEM_getChar( contents, ITEM_NAME),
					ITEM_getInt( contents, ITEM_ID)
				);
			}

			return;
			break;
		case 1:
			CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);

			if( OBJECT_getIndex( objindex) == 0 ) {
				CHAR_ObjectDelete(objindex);
			}else {
				CHAR_sendWatchEvent(objindex,CHAR_ACTSTAND,NULL,0,TRUE);
			}
			{
				char    mesg[256];
				if( contents == 0 ) {
					snprintf( mesg,sizeof(mesg), "无法再拾获Stone了。" );
				}else {
					snprintf( mesg,sizeof(mesg), "拾获 %d Stone",contents );
				}
				CHAR_talkToCli(charaindex,-1,mesg,CHAR_COLORWHITE);
			}
			if( contents != 0 ){
			     LogStone(
										-1,
			             CHAR_getChar( charaindex, CHAR_NAME ),
			             CHAR_getChar( charaindex, CHAR_CDKEY ),
			             contents,
						 CHAR_getInt( charaindex, CHAR_GOLD ),
			             "PickUp(拾获金钱)",
			             CHAR_getInt( charaindex,CHAR_FLOOR),
			             CHAR_getInt( charaindex,CHAR_X ),
			             CHAR_getInt( charaindex,CHAR_Y )
			             );
			}
			return;
			break;
		case 2:	//宠物
			CHAR_ObjectDelete(objindex);
			CHAR_setWorkInt( contents, CHAR_WORKOBJINDEX, -1);
			{
				char    mesg[256];
				char    *p = NULL;
				p = CHAR_getChar( contents, CHAR_USERPETNAME);
				if( strlen( p) == 0 ) {
					p = CHAR_getChar( contents, CHAR_NAME);
				}

				snprintf( mesg,sizeof(mesg), "拾获 %s", p);
				CHAR_talkToCli(charaindex,-1,mesg,CHAR_COLORWHITE);
			}
			// ペットを拾ったログ
			LogPet(
				CHAR_getChar( charaindex, CHAR_NAME ), /* キャラ名   */
				CHAR_getChar( charaindex, CHAR_CDKEY ),
				CHAR_getChar( contents, CHAR_NAME),
				CHAR_getInt( contents, CHAR_LV),
				"PickUp(捡宠)",
				CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
				CHAR_getInt( charaindex,CHAR_Y ),
				CHAR_getChar( contents, CHAR_UNIQUECODE)   // shan 2001/12/14
			);
			return;
			break;
		// Robin 0701 petFollow
		case 3:

			/*  まわりにこれを消せという   */
			CHAR_ObjectDelete(objindex);
			CHAR_setWorkInt( contents, CHAR_WORKOBJINDEX, -1);
			{
				char    mesg[256];
				char    *p = NULL;
				p = CHAR_getChar( contents, CHAR_USERPETNAME);
				if( strlen( p) == 0 ) {
					p = CHAR_getChar( contents, CHAR_NAME);
				}

				snprintf( mesg,sizeof(mesg), "拾回 %s", p);
				CHAR_talkToCli(charaindex,-1,mesg,CHAR_COLORWHITE);
			}
			// ペットを拾ったログ
			LogPet(
				CHAR_getChar( charaindex, CHAR_NAME ), /* キャラ名   */
				CHAR_getChar( charaindex, CHAR_CDKEY ),
				CHAR_getChar( contents, CHAR_NAME),
				CHAR_getInt( contents, CHAR_LV),
				"FollowPickUp(收回溜宠)",
				CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
				CHAR_getInt( charaindex,CHAR_Y ),
				CHAR_getChar( contents, CHAR_UNIQUECODE)   // shan 2001/12/14
			);

			return;
			break;		
		case -1:
			break;
		case -2:
			CHAR_talkToCli( charaindex,-1,"道具栏已满。", CHAR_COLORYELLOW);
			break;
		case -3:
			CHAR_talkToCli( charaindex,-1,"宠物数已满。", CHAR_COLORYELLOW);
			break;
		case -4:
			CHAR_talkToCli( charaindex,-1,"无法照顾该宠物。", CHAR_COLORYELLOW);
			break;
		case -5:
			CHAR_talkToCli( charaindex,-1,"无法拾获该宠物。", CHAR_COLORYELLOW);
			break;
		case -6:
			CHAR_talkToCli( charaindex,-1,"无法拾获该道具。", CHAR_COLORYELLOW);
			break;
		default:
			return;
		}
	}
	return;
}

/*------------------------------------------------------------
 * お金を座標指定で落そうとする。CHAR_DropMoneyからのみで使う事。
 * 引数
 *  charaindex  int     キャラインデックス
 *  itemindex   int     アイテムインデックス(キャラデータの中での)
 *  fl          int     フロア
 *  x           int     x
 *  y           int     y
 *  force       BOOL    この値が正だと無理矢理落す
 *  objindex    int*    置いたアイテムの Object でのインデクッス
 * 返り値
 *  失敗(そんなにお金を持っていない)    -1
 *  失敗(地形的にだめ)  -2
 *  それ以外の失敗  -3
 *  失敗(そこにはその量のお金は多すぎる)    -4
 *  すでに他のアイテムが落ちている  -5
 *  量が負で変。 -6
 *  成功    0
 ------------------------------------------------------------*/
static BOOL CHAR_DropMoneyFXY( int charaindex, int amount, int fl , int x,
							   int y, BOOL force, int* objindex )
{
	OBJECT  object;
	int MaxGold;
	int mygold = CHAR_getInt( charaindex, CHAR_GOLD );
	if( amount <= 0 )return -6;
	if( amount > mygold )
		return -1;

	MaxGold = CHAR_getMaxHaveGold( charaindex);
	if ((mygold <0) || (mygold > MaxGold))	return -1;
                                                                              	
	if( MAP_walkAbleFromPoint( fl,x,y,FALSE ) == FALSE )return -2;
	for( object=MAP_getTopObj(fl,x,y) ; object;
		 object = NEXT_OBJECT(object) ){
		int index = GET_OBJINDEX(object);
		int objtype = OBJECT_getType(index);

		if( objtype == OBJTYPE_NOUSE ) continue;
		else if( objtype == OBJTYPE_GOLD ){
			long tmpamount = OBJECT_getIndex(index) + amount;
			if( tmpamount  > MaxGold )
				return -4;
			OBJECT_setIndex(index,tmpamount);
			CHAR_setInt(charaindex,CHAR_GOLD,
						(unsigned int)CHAR_getInt(charaindex,
												  CHAR_GOLD )
						- amount );
			*objindex = index;
#ifdef _DEL_DROP_GOLD
			OBJECT_setTime( index, NowTime.tv_sec);
#endif
			if( amount >= 100 ){ /* お金を置いたログ */
			        LogStone(-1,CHAR_getChar( charaindex, CHAR_NAME ), /* キャラ名   */
			                 CHAR_getChar( charaindex, CHAR_CDKEY ), /* ユーザーID */
			                 amount,                                   /* 金額 */
							 CHAR_getInt( charaindex, CHAR_GOLD ),
			                 "Drop(放置金钱)",
			                 CHAR_getInt( charaindex,CHAR_FLOOR),
			                 CHAR_getInt( charaindex,CHAR_X ),
			                 CHAR_getInt( charaindex,CHAR_Y )
			                 );
			}
			return 0;
		}else if( objtype == OBJTYPE_ITEM ){
			if( force==TRUE )
				continue;
			else
				/* すでにアイテムがあるのでだめ */
				return -5;
		}
	}

	/*  ここまで到達するという事は新規登録する */
	{
		Object  one;
		one.type = OBJTYPE_GOLD;
		one.x = x;
		one.y = y;
		one.floor = fl;

		if( amount > MaxGold )
			return -4;

		one.index = amount;
		*objindex = initObjectOne( &one );
		/*  オブジェクト登録する    */
		if( *objindex == -1 )
			/*  登録失敗     */
			return -3;

		/*お金を減らす*/
		CHAR_setInt(charaindex,CHAR_GOLD,
					(unsigned int)CHAR_getInt(charaindex,CHAR_GOLD )
					- amount );
#ifdef _DEL_DROP_GOLD
		OBJECT_setTime( *objindex, NowTime.tv_sec);
#endif
		if( amount >= 100 ){ /*  お金を置いたログ */
		      LogStone(-1,CHAR_getChar( charaindex, CHAR_NAME ), /* キャラ名   */
		               CHAR_getChar( charaindex, CHAR_CDKEY ), /* ユーザーID */
		               amount,                                   /* 金額 */
					   CHAR_getInt( charaindex, CHAR_GOLD ),
		               "Drop(放置金钱)",
		               CHAR_getInt( charaindex,CHAR_FLOOR),
		               CHAR_getInt( charaindex,CHAR_X ),
		               CHAR_getInt( charaindex,CHAR_Y )
		               );
		}
		return 0;
	}
}

/*------------------------------------------------------------
 * お金を落す関数。目の前から落す。
 * 引数
 *  charaindex      int     キャラインデックス
 *  amount          int     お金の量
 *
 * 返り値
 *  なし
 ------------------------------------------------------------*/
void CHAR_DropMoney( int charaindex,  int amount )
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKSAFEMODE)==0)
		{
			CHAR_SafePassword(charaindex);
			return;
		}
	if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) > 0)
		{
			return;
		}
	int dirx[9],diry[9];
	int ret;
	int i;
	int objindex, MaxGold;

	if( !CHAR_CHECKINDEX( charaindex ) )return;

#ifdef _AVID_TRADETRYBUG //丢出石币
	if( CHAR_getWorkInt( charaindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING ){
		CHAR_talkToCli( charaindex, -1, "交易状态中无法丢出石币。", CHAR_COLORYELLOW );
		return;
	}
#endif

	MaxGold = CHAR_getMaxHaveGold( charaindex);
	if( CHAR_getFlg( charaindex,CHAR_ISDIE ) )return;

	//ANDY_ADD	12/28
	{
		int money=0;
		money = CHAR_getInt( charaindex, CHAR_GOLD );
		if( money < 0 )	{
			CHAR_setInt( charaindex, CHAR_GOLD, 0);
			return;
		}
		if( (amount > money) || (money > MaxGold))	{
			if( money > MaxGold )	{
				CHAR_setInt( charaindex, CHAR_GOLD, 0 );
				print("\n ERR: %s(stone%d) dropMoney%d > CHAR_MAXGOLDHAVE !!",
						CHAR_getChar(charaindex, CHAR_CDKEY), money, amount);
			}else	{
				print("\n ERR: %s dropMoney > CHAR_GOLD !!", CHAR_getChar(charaindex, CHAR_CDKEY));
			}
			CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
			return;
		}
	}
	//ANDY_END
	for( i  = 0 ; i < 7 ; i  ++ ){
		dirx[i+2] = CHAR_getDX( CHAR_getInt(charaindex,CHAR_DIR) + i+1 );
		diry[i+2] = CHAR_getDY( CHAR_getInt(charaindex,CHAR_DIR) + i+1 );
	}

	dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
	diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
	dirx[1] = 0;
	diry[1] = 0;

	for( i = 0 ; i < 9 ; i ++ ){
		int x = CHAR_getInt(charaindex,CHAR_X) + dirx[i];
		int y = CHAR_getInt(charaindex,CHAR_Y) + diry[i];

// CoolFish 2002/04/06 下注处不可丢置石币
#ifdef _DROPSTAKENEW
		{
			int j = 0;
			for(j = 0; j < arraysizeof(casinomap); j++)
			{
				if(CHAR_getInt(charaindex, CHAR_FLOOR) == casinomap[j].casinofl)
				{
					if (x == casinomap[j].casinox && y == casinomap[j].casinoy)
					{
						CHAR_talkToCli(charaindex, -1, "这里不能放置石币！", CHAR_COLORYELLOW);
						return;
					}
				}
			}
		}
#endif
		ret = CHAR_DropMoneyFXY( charaindex,amount,
								 CHAR_getInt(charaindex,CHAR_FLOOR),
								 x,y, FALSE, &objindex );
		switch( ret ){
		case 0:
			goto END;
			break;
		case -1:
			return;
			break;
		case -3:
			return;
			break;
		case -2:
		case -4:
		case -5:
		case -6:
			break;
		default:
			break;
		}
	}

	{
		int x = CHAR_getInt(charaindex,CHAR_X) + dirx[1];
		int y = CHAR_getInt(charaindex,CHAR_Y) + diry[1];

		ret = CHAR_DropMoneyFXY( charaindex,amount,
								 CHAR_getInt(charaindex,CHAR_FLOOR),
								 x,y, TRUE, &objindex );
		switch( ret ){
		case 0:
			goto END;
			break;
		case -1:
			break;
		case -3:
			return;
			break;
		case -2:
		case -4:
		case -5:
			break;
		default:
			break;
		}
		return;
	}

END:
	CHAR_sendWatchEvent(objindex,CHAR_ACTSTAND,NULL,0,TRUE );
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_GOLD);
	return;
}

static int CHAR_findEmptyItemBoxNoFromChar( Char* ch )
{
	int i, j=0;;
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE  ; i ++ ){
		int     itemindex;
		itemindex = ch->indexOfExistItems[i];
		if( itemindex == -1 )  j++;
	}
	return j;
}

int CHAR_findEmptyItemBoxNo( int index )
{
	Char*   ch;

	if( !CHAR_CHECKINDEX( index ) )return -1;
	ch = CHAR_getCharPointer(index);
	if( ch != NULL )
		return CHAR_findEmptyItemBoxNoFromChar( ch );
	else
		return -1;

}

// CoolFish: Trade 2001/4/4
int CHAR_findTotalEmptyItem( int index )
{
    int     i, totaleptitem = 0;
    Char*   ch;

    if( !CHAR_CHECKINDEX( index ) )return -1;
    ch = CHAR_getCharPointer(index);
    if( ch != NULL ){
        for (i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i++){
        	int itemindex = 0;
                itemindex = ch->indexOfExistItems[i];
                if (itemindex == -1) totaleptitem++;
        }
        return totaleptitem;
    }

	return -1;
}

// Robin 0707 petFollow
int CHAR_pickupFollowPet( int charaindex, int pickupindex )
{
	int petindex;
	int objindex;
	int havepetindex;
	char category[3];
	
	if( !CHAR_CHECKINDEX( charaindex ) )	return FALSE;
	
	if( pickupindex != -1 )
		petindex = pickupindex;
	else
		petindex = CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW );
		
	if( !CHAR_CHECKINDEX( petindex ) )
	{
		CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1 );
		return FALSE;
	}

	havepetindex = CHAR_getCharPetElement( charaindex);
	if( havepetindex == -1 ) {
		CHAR_talkToCli(charaindex,-1,"宠物栏已满！无法拾回！",CHAR_COLORYELLOW);
		return FALSE;
	}
	
	objindex = CHAR_getWorkInt( petindex, CHAR_WORKOBJINDEX );
	print(" objindex02:%d ", objindex);
	
	if( (pickupindex == -1) || (pickupindex == CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW )   ) )
		CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1 );
	
	/* CDKEY とキャラ名が一致しないときは拾えない */
	if( strcmp( CHAR_getChar( petindex, CHAR_OWNERCDKEY), CHAR_getChar( charaindex, CHAR_CDKEY)) != 0 ||
	    strcmp( CHAR_getChar( petindex, CHAR_OWNERCHARANAME), CHAR_getChar( charaindex, CHAR_NAME)) != 0 )
	{
		//CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1 );
		
		return FALSE;
	}

	
	//CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1 );
	
	CHAR_ObjectDelete(objindex);
	
	CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWMODE, CHAR_PETFOLLOW_NONE );
		
	CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, charaindex);
	CHAR_setCharPet( charaindex, havepetindex, petindex);

#ifndef _FIX_COPY_MM_BUG	
#ifdef _PET_TALK
	if( CHAR_getInt( petindex, CHAR_PETID) != 718 )	{
#endif
#endif
		CHAR_setChar( petindex, CHAR_OWNERCDKEY, CHAR_getChar( charaindex, CHAR_CDKEY));
		CHAR_setChar( petindex, CHAR_OWNERCHARANAME, CHAR_getChar( charaindex, CHAR_NAME));
#ifndef _FIX_COPY_MM_BUG	
#ifdef _PET_TALK
	}
#endif
#endif

	snprintf( category,sizeof( category),"K%d",havepetindex);
	CHAR_sendStatusString( charaindex, category);
	snprintf( category,sizeof( category),"W%d",havepetindex);
	CHAR_sendStatusString( charaindex, category);

	CHAR_setInt( petindex, CHAR_PUTPETTIME, 0);
	CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT, 0);

	
	CHAR_setWorkInt( petindex, CHAR_WORKOBJINDEX, -1);

	{
		char    mesg[256];
		char    *p = NULL;
		p = CHAR_getChar( petindex, CHAR_USERPETNAME);
		if( strlen( p) == 0 ) {
			p = CHAR_getChar( petindex, CHAR_NAME);
		}
		snprintf( mesg,sizeof(mesg), "拾回 %s", p);
		CHAR_talkToCli(charaindex,-1,mesg,CHAR_COLORYELLOW);
	}
	// ペットを拾ったログ
	LogPet(
		CHAR_getChar( charaindex, CHAR_NAME ), /*  キャラ名   */
		CHAR_getChar( charaindex, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"FollowPickUp(收回溜宠)",
		CHAR_getInt( charaindex,CHAR_FLOOR),
		CHAR_getInt( charaindex,CHAR_X ),
		CHAR_getInt( charaindex,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);

	return TRUE;
				
}
#ifdef _GAMBLE_ROULETTE
int NPC_MAPCLEANGOLD( int meindex , int floor)
{
	OBJECT  object;
	int x,y,fl;
	int End_type;
	fl = floor;

	for( End_type = 0; End_type < arraysizeof(casinomap) ; End_type++ )	{
		if( floor != casinomap[ End_type].casinofl )
			continue;
		x = casinomap[ End_type].casinox;
		y = casinomap[ End_type].casinoy;

		for( object=MAP_getTopObj(fl,x,y) ; object ; object =  NEXT_OBJECT(object ) )	{
			int index = GET_OBJINDEX(object);
			int objtype = OBJECT_getType(index);
			int itemindex = OBJECT_getIndex( index);

			if( objtype == OBJTYPE_NOUSE )continue;
			if( objtype == OBJTYPE_ITEM ){
				if(ITEM_getInt(itemindex, ITEM_ID) == CASINOTOKENID)	{
					ITEM_endExistItemsOne( itemindex);
					OBJECT_setIndex( index, 0);
					CHAR_ObjectDelete(index);	//清除
					print(" clean( %d )", index );
				}
				break;
			}
		}
		casinomap[End_type].stakenum = 0;
	}
	return 1;
}
#endif

#ifdef _DROPSTAKENEW
// casinotype : flg = 0 不分type，针对整层type做设定；扩充参数
// mapdropflag : TRUE = 可以下注;
int SetCasinoMap(int npcindex, int casinotype, int mapdropflag)
{
	int npcfloor = CHAR_getInt(npcindex, CHAR_FLOOR);
	int i = 0;
	if (casinotype < 0 || mapdropflag < 0)
		return	-1;
	for (i = 0; i < arraysizeof(casinomap); i++)
	{
		if (npcfloor == casinomap[i].casinofl)
		{
			if (casinotype != 0)
			{
				if (casinomap[i].casinotype == casinotype)
					casinomap[i].dropflag = mapdropflag;
			}
			else
				casinomap[i].dropflag = mapdropflag;
		}
	}
	return	1;
}

// 计算积分
int CasinoAccumulation(int charindex, int npcindex, int floor, int wincasinotype)
{
	int i = 0;
	int accumulation = CHAR_getInt(charindex, CHAR_GAMBLENUM);
	char tmpbuf[256];
	for (i = 0; i < MAXSTAKENUM; i++)
	{
		if (CHAR_getWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i) == wincasinotype)
		{
			int j = 0;
			for (j = 0; j < arraysizeof(casinomap); j++)
			{
				if (casinomap[j].casinotype == wincasinotype)
				{ 
#ifdef _FIX_GAMBLENUM
					// 加 原本积分及WIN积分
					accumulation = accumulation + casinomap[j].accumulation * 2;
#else
					accumulation += casinomap[j].accumulation;
#endif
					CHAR_setWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i, 0);
					break;
				}
			}
		}

		else if (CHAR_getWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i) != 0)
		{
			int j = 0;
			for (j = 0; j < arraysizeof(casinomap); j++)
			{
				if (casinomap[j].casinotype == CHAR_getWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i))
				{
#ifndef _FIX_GAMBLENUM
					accumulation -= casinomap[j].accumulation;
#endif
					CHAR_setWorkInt(charindex, CHAR_WORKSTAKETYPE1 + i, 0);
					break;
				}
			}
		}
	}
	CHAR_setWorkInt(charindex, CHAR_WORKSTAKEFLAG, 0);
	if (accumulation < 0)
		accumulation = 0;
	else if (accumulation > MAXACCUMULATION)
		accumulation = MAXACCUMULATION;
	CHAR_setInt(charindex, CHAR_GAMBLENUM, accumulation);
	snprintf(tmpbuf, sizeof(tmpbuf), "您现在总共有%4d点游乐场积分", accumulation);
	CHAR_talkToCli(charindex, npcindex, tmpbuf, CHAR_COLORWHITE);
	return 1;
}

int CasinoPay(int npcindex, int wincasinotype)
{
	int npcfloor = CHAR_getInt(npcindex, CHAR_FLOOR);
	int i = 0;
	int playernum = CHAR_getPlayerMaxNum();
	if (wincasinotype < 0) return FALSE;
	for (i = 0; i < playernum; i++)
	{
		if (CHAR_getCharUse(i) == FALSE) continue;
		if (!CHAR_CHECKINDEX(i))	continue;
		if (CHAR_getInt(i, CHAR_FLOOR) == npcfloor)
		{
			// 查询玩家是否有下注
			if (CHAR_getWorkInt(i, CHAR_WORKSTAKEFLAG) == 0)
				continue;
			// 计算积分
			CasinoAccumulation(i, npcindex, npcfloor, wincasinotype);
		}
	}
	// 清除此层地面彩券
	for (i = 0; i < arraysizeof(casinomap); i++)
	{
		OBJECT object;
		if (casinomap[i].casinofl != npcfloor) continue;
		for (object = MAP_getTopObj(casinomap[i].casinofl, casinomap[i].casinox,
			casinomap[i].casinoy); object; object = NEXT_OBJECT(object))
		{
			int index = GET_OBJINDEX(object);
			int objtype = OBJECT_getType(index);
			int itemindex = OBJECT_getIndex( index);
			if (objtype == OBJTYPE_NOUSE) continue;
			else if (objtype == OBJTYPE_ITEM)
			{
				if(ITEM_getInt(itemindex, ITEM_ID) == CASINOTOKENID)
				{
					OBJECT_setIndex(index, 0);
					CHAR_ObjectDelete(index);
				}
			}
		}
		casinomap[i].stakenum = 0;
	}
	return TRUE;
}
#endif

#ifdef _PET_ITEM
void CHAR_sendPetItemData( int charaindex, int petid)
{
		int     i;
		char CHAR_statusSendBuffer[1024];
		int strlength = 2;
		int petindex = CHAR_getCharPet( charaindex, petid);
		int fd = getfdFromCharaIndex( charaindex );
		snprintf( CHAR_statusSendBuffer ,sizeof(CHAR_statusSendBuffer),"B%d" , petid);
		for( i = 0 ; i < CHAR_MAXPETITEMHAVE ; i ++ ){
			char    token[512];
			int     itemindex;
			itemindex = CHAR_getItemIndex( petindex, i );
			if( itemindex >= 0 ) {
				snprintf( token ,sizeof(token),"%s|" ,ITEM_makeItemStatusString( -1, itemindex ) );
			}else {
				snprintf( token ,sizeof(token),"%s|" , ITEM_makeItemFalseString() );
			}
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof(CHAR_statusSendBuffer) - strlength, token );
			strlength += strlen( token );
			if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) lssproto_S_send(fd, CHAR_statusSendBuffer);
		}
		dchop( CHAR_statusSendBuffer , "|" );
		lssproto_S_send(fd, CHAR_statusSendBuffer);
}

void CHAR_movePetItem( int charaindex, int petid, int fromindex, int toindex )
{
	if( !CHAR_CHECKINDEX( charaindex ) )return;
	if( CHAR_getFlg( charaindex,CHAR_ISDIE ) )return;
	int petindex = CHAR_getCharPet( charaindex, petid);
	char token[512];
	if(toindex == -1 && fromindex >= CHAR_EQUIPPLACENUM && fromindex < CHAR_MAXITEMHAVE){
		int itemindex = CHAR_getItemIndex( charaindex, fromindex );
		toindex=ITEM_getInt( itemindex, ITEM_TYPE)-ITEM_PET_HEAD;
	}else if(toindex == -2){
		if(fromindex<CHAR_PETITEMNUM)
			CHAR_talkToCli( charaindex, -1, "无法丢弃宠物身上的物品。", CHAR_COLORYELLOW );
		else
			CHAR_DropItem( charaindex, fromindex );
	}
#ifdef _PETITEM__AMOI_E
	{		
		char* allowpetitemtype =NULL;
		char* forbitpetitemtype =NULL;
		int INT_petfesion = -1;
		
		int itemindex = CHAR_getItemIndex( charaindex, fromindex );
		
		INT_petfesion = EVOLUTION_getPetFusionCode( CHAR_getInt( petindex, CHAR_PETID));

		if( CHAR_getInt( charaindex, CHAR_RIDEPET ) == petid) {
			CHAR_talkToCli( charaindex, -1, "骑乘中的宠物无法装备物品!", CHAR_COLORYELLOW );
			return;
	    }	
		if(INT_petfesion == -1){
			if ( toindex>=0 && toindex<  CHAR_MAXPETITEMHAVE){//如果toindex 是到宠身上则
				//CHAR_talkToCli( charaindex, -1, "无法装备该项物品。", CHAR_COLORWHITE );
				//return;
				INT_petfesion = 28;// 
			}
			//增加能脱下的模块
			/*
			int     itemindexi;//
			itemindexi = CHAR_getItemIndex( charaindex , toindex );//取出装备的内在索引
			if(itemindexi >= 0){
				CHAR_talkToCli( charaindex, -1, "无法装备该项物品。", CHAR_COLORWHITE );
				return;
			}*/
			
		}		
		//print("\nINT_petfesion %d = \n", INT_petfesion );	
		allowpetitemtype = ITEM_getChar(itemindex,ITEM_PETEQUITALLOW);//允许装备
		forbitpetitemtype= ITEM_getChar(itemindex,ITEM_PETEQUITFORBIT);//不允许装备
		//print("\allowpetitemtype len %d = \n", arraysizeof(allowpetitemtype) );
		//print("\forbitpetitemtype len %d = \n", arraysizeof(forbitpetitemtype) );
		{	//允许装备
			int tokenNo = 1 ;
			char tokenBuf[3];
			BOOL ISCATFLG = FALSE;
			BOOL UseFisrtTokenFlg  = TRUE;
			BOOL UseSecondTokenFlg  = TRUE;
			BOOL FindFisrtTokenFlg = FALSE;
			
			tokenNo = 1 ;

			if(!(atoi(allowpetitemtype)== 0 && atoi(forbitpetitemtype)== 0 ) ){
				while(	getStringFromIndexWithDelim(allowpetitemtype," ",tokenNo,tokenBuf,sizeof(tokenBuf))){
					//print("\n tokenBuf %d = \n",atoi(tokenBuf));
					if(-1            == atoi(tokenBuf)) {
						UseFisrtTokenFlg = FALSE;
						break;
					}
					if(INT_petfesion == atoi(tokenBuf)) {
						FindFisrtTokenFlg = TRUE;
						break;
					}
					tokenNo++;
				}
				if(UseFisrtTokenFlg){//如果是使用限定表
					if(!FindFisrtTokenFlg){//如果发现可以使用
						//if(!getStringFromIndexWithDelim(allowpetitemtype," ",tokenNo,tokenBuf,sizeof(tokenBuf))){
							CHAR_talkToCli( charaindex, -1, "无法装备该项物品。", CHAR_COLORWHITE );
							return;
						//}
					}
				}else{//如果是使用排除表
					FindFisrtTokenFlg = FALSE;
					tokenNo = 1;
					while(	getStringFromIndexWithDelim(forbitpetitemtype," ",tokenNo,tokenBuf,sizeof(tokenBuf))){
						if(-1            == atoi(tokenBuf)) {
							UseSecondTokenFlg = FALSE;
							break;
						}
						if(INT_petfesion == atoi(tokenBuf)) {
							FindFisrtTokenFlg = TRUE;
							break;
						}
						tokenNo++;
				 	}
					if(UseSecondTokenFlg){
						if(FindFisrtTokenFlg){//如果发现不可以使用
							CHAR_talkToCli( charaindex, -1, "无法装备该项物品3。", CHAR_COLORWHITE );
							return;
						}
					}
				}
			}		
		}
	}
#endif
	if( (fromindex >=0 && fromindex < CHAR_PETITEMNUM && toindex >= CHAR_EQUIPPLACENUM && toindex < CHAR_MAXITEMHAVE) 
		|| (toindex >=0 && toindex < CHAR_PETITEMNUM && fromindex >= CHAR_EQUIPPLACENUM && fromindex < CHAR_MAXITEMHAVE)){
		if(fromindex >= CHAR_EQUIPPLACENUM){
			int charitemindex = CHAR_getItemIndex( charaindex, fromindex );
			int petitemindex = CHAR_getItemIndex( petindex, toindex );
			int type=ITEM_getInt( charitemindex, ITEM_TYPE);
			if(CHAR_getInt(petindex, CHAR_VARIABLEAI) < 100){
				CHAR_talkToCli( charaindex, -1, "无法训服宠物装备该物品。", CHAR_COLORRED );
				return;
			}
			if(type<ITEM_PET_HEAD || type>ITEM_PET_FEET || (type-ITEM_PET_HEAD) != toindex){
				CHAR_talkToCli( charaindex, -1, "无法装备该项物品。", CHAR_COLORWHITE );
				return;
			}
			if( CHAR_getInt( petindex, CHAR_LV) < ITEM_getInt( charitemindex, ITEM_LEVEL)){
				CHAR_talkToCli( charaindex, -1, "宠物等级尚未到达装备该项物品！", CHAR_COLORYELLOW );
				return;
			}
			if( ITEM_CHECKINDEX( petitemindex)){
				sprintf( token, "宠物%s 卸下%s", CHAR_getChar( petindex, CHAR_NAME), ITEM_getChar( petitemindex, ITEM_NAME));
				CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE );
			}
			sprintf( token, "宠物%s 装备%s", CHAR_getChar( petindex, CHAR_NAME), ITEM_getChar( charitemindex, ITEM_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE );
			CHAR_setItemIndex( charaindex, fromindex, petitemindex );
			CHAR_setItemIndex( petindex, toindex, charitemindex );
			CHAR_sendItemDataOne( charaindex, fromindex);
			CHAR_sendPetItemData( charaindex, petid);
		}else{
			int charitemindex = CHAR_getItemIndex( charaindex, toindex );
			int petitemindex = CHAR_getItemIndex( petindex, fromindex );
			int type=ITEM_getInt( charitemindex, ITEM_TYPE);
			sprintf( token, "宠物%s 卸下%s", CHAR_getChar( petindex, CHAR_NAME), ITEM_getChar( petitemindex, ITEM_NAME));
			CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE );
			if( ITEM_CHECKINDEX( charitemindex)){
				if(CHAR_getInt(petindex, CHAR_VARIABLEAI) < 100){
					CHAR_talkToCli( charaindex, -1, "无法训服宠物装备该物品。", CHAR_COLORRED );
					return;
				}
				if(type<ITEM_PET_HEAD || type>ITEM_PET_FEET || (type-ITEM_PET_HEAD) != toindex){
					CHAR_talkToCli( charaindex, -1, "无法装备该项物品。", CHAR_COLORWHITE );
					return;
				}
				if( CHAR_getInt( petindex, CHAR_LV) < ITEM_getInt( charitemindex, ITEM_LEVEL)){
					CHAR_talkToCli( charaindex, -1, "宠物等级尚未到达装备该项物品！", CHAR_COLORYELLOW );
					return;
				}
				sprintf( token, "宠物%s 装备%s", CHAR_getChar( petindex, CHAR_NAME), ITEM_getChar( charitemindex, ITEM_NAME));
				CHAR_talkToCli( charaindex, -1, token, CHAR_COLORWHITE );
			}
			CHAR_setItemIndex( charaindex, toindex, petitemindex );
			CHAR_setItemIndex( petindex, fromindex, charitemindex );
			CHAR_sendItemDataOne( charaindex, toindex);
			CHAR_sendPetItemData( charaindex, petid);
		}
	}
	CHAR_complianceParameter( petindex );
	snprintf( token, sizeof( token ), "K%d", petid );
	CHAR_sendStatusString( charaindex, token );
}

void CHAR_sendPetItemEmpty( int charaindex, int petid)
{
	int fd = getfdFromCharaIndex( charaindex );
	char CHAR_statusSendBuffer[16];
	sprintf(CHAR_statusSendBuffer,"B%d|",petid);
	lssproto_S_send(fd, CHAR_statusSendBuffer);
}

#endif

void CHAR_CheckItemTime( int charaindex ){
	int i;
	for( i = 0 ; i < CHAR_MAXITEMHAVE ; i ++ ){
		int itemindex = CHAR_getItemIndex( charaindex, i );
		if( itemindex >= 0 ) {
			int itemtime = 0;
			itemtime = ITEM_getInt(itemindex,ITEM_TIME);
			if(itemtime>0){
				time_t timep;
				struct tm *p;
				time(&timep);
				p=localtime(&timep);
				timep = mktime(p);
				if(itemtime<=timep){
					char itemname[128];
					sprintf(itemname,"您的道具:%s，由于时效已到，系统已收回。",ITEM_getChar(itemindex,ITEM_NAME));
					CHAR_talkToCli( charaindex, -1, itemname, CHAR_COLORYELLOW );
					CHAR_setItemIndex( charaindex, i ,-1);
					CHAR_sendItemDataOne( charaindex, i);
					ITEM_endExistItemsOne(itemindex);
				}
			}
		}
	}
}

