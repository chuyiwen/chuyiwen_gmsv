#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   // shan
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "net.h"
#include "common.h"
#include "char_base.h"
#include "char_data.h"
#include "char.h"
#include "lssproto_serv.h"
#include "item.h"
#include "item_event.h"
#include "buf.h"
#include "object.h"
#include "map_deal.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "handletime.h"
#include "char_event.h"
#include "npccreate.h"
#include "addressbook.h"
#include "item_event.h"
#include "magic_base.h"
#include "magic.h"
#include "chatmagic.h"
#include "configfile.h"
#include "log.h"
#include "anim_tbl.h"
#include "encount.h"
#include "battle.h"
#include "pet_skill.h"
#include "util.h"
#include "enemy.h"
#include "npcutil.h"
#include "pet.h"
#include "family.h"
#include "defend.h"
#include "correct_bug.h"
#ifdef _JOBDAILY
#include "npc_checkman.h"
#endif

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业栏位
#include "profession_skill.h"
#endif
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
#include "chatroom.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
extern int channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];	
extern int familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

extern  tagRidePetTable ridePetTable[296];
extern	int BATTLE_getRidePet( int charaindex );

#ifdef _CHANNEL_MODIFY
extern int InitOccChannel(void);
#endif

#ifdef _ANGEL_SUMMON
extern int checkIfAngelByName( char* nameinfo);
extern char* getMissionNameInfo( int charaindex, char* nameinfo);
extern void CHAR_sendAngelMark( int objindex, int flag);
extern int checkIfOnlyAngel( int charaindex);
extern void selectAngel( int charaindex, int heroindex, int mission, int gm_cmd);
#endif
int CharaData( int sockfd, Char* ch );
extern void GOLD_DeleteTimeCheckOne( int objindex);
extern int autopkstart;
extern int playeronlinenum;
extern int autopkcnt;
// WON ADD 修正道具的设定问题
void fix_item_bug(int charaindex, int i);


/*====================キャラの作成に関する関数====================*/
/*------------------------------------------------------------
 * 作成する時のオプションの値を設定しようとする。
 * 引数
 *  ch      Char*       設定先のキャラのデータ
 *  option  char*       クライアントからくるデータ
 * 返り値
 *  成功    TRUE
 *  失敗    FALSE

 ------------------------------------------------------------*/
static BOOL CHAR_makeCharFromOptionAtCreate( Char* ch ,
		int	vital, int str, int tgh, int dex,
		int earth, int water, int fire, int wind)
{

	/*    有効性の検証を行う  */
	int		para[4];
	int		attr[4];
	int		i;
	int		parasum,attrsum;
	int		cnt;

	para[0] = vital;
	para[1] = str;
	para[2] = tgh;
	para[3] = dex;

	attr[0] = earth;
	attr[1] = water;
	attr[2] = fire;
	attr[3] = wind;

#define MAXPARAMETER 20
#define PKMAXPARAMETER 300
	parasum = 0;
	/*
	 * すべてのデータの中に - はあってはならないし
	 * 桁あふれの事を考えておいて最大の値も超えない
	 */

	for( i = 0; i < arraysizeof( para); i ++ ) {
		if( para[i] < 0 || para[i] > MAXPARAMETER )return FALSE;
		parasum += para[i];
	}
#ifdef _NEW_PLAYER_CF	//新手出生配置
	if( parasum > MAXPARAMETER ) return FALSE;
#else
	if( parasum != MAXPARAMETER ) return FALSE;
#endif

#undef MAXPARAMETER


#define MAXATTRIBUTE 10
	cnt = 0;
	attrsum = 0;
	/* 属性のチェックをする */
	for( i = 0; i < arraysizeof( para); i ++ ) {
		if( attr[i] < 0 || attr[i] > MAXATTRIBUTE )return FALSE;
		attrsum += attr[i];
		if( attr[i] > 0 ) cnt ++;
	}
	/* 合計値は10 */
	if( attrsum != MAXATTRIBUTE ) return FALSE;

	/*  2つより多く割り当てられていたら駄目  */
	if( cnt > 2 ) return FALSE;

	/* 逆属性は割り当てられない */
	if( attr[0] > 0 && attr[2] > 0 ) return FALSE;
	if( attr[1] > 0 && attr[3] > 0 ) return FALSE;

#undef MAXATTRIBUTE

	/*  コピー */
	ch->data[CHAR_VITAL] = vital*100;
	ch->data[CHAR_STR]   = str*100;
	ch->data[CHAR_TOUGH] = tgh*100;
	ch->data[CHAR_DEX]   = dex*100;

	ch->data[CHAR_EARTHAT] = attr[0] * 10;
	ch->data[CHAR_WATERAT] = attr[1] * 10;
	ch->data[CHAR_FIREAT]  = attr[2] * 10;
	ch->data[CHAR_WINDAT]  = attr[3] * 10;
#ifdef _NEWOPEN_MAXEXP
	ch->data[CHAR_OLDEXP] = 0;
#endif
	ch->data[CHAR_EXP] = 0;
	
	
#ifdef _NEW_PLAYER_CF	//新手出生配置
	ch->data[CHAR_TRANSMIGRATION] = getNewplayertrans();
	ch->data[CHAR_GOLD] = getNewplayergivegold();;
	ch->data[CHAR_LV] = getNewplayerlv();
#ifdef _VIP_SERVER
	ch->data[CHAR_AMPOINT] = getNewplayergivevip();
#endif
//	ch->data[CHAR_LEARNRIDE] = 200;
//	ch->data[CHAR_LASTTALKELDER] = (rand()%10)>5?35:36;//34;
//	ch->data[CHAR_SKILLUPPOINT] = 616;//600;
#ifdef _NEW_RIDEPETS
	//ch->data[CHAR_LOWRIDEPETS] = 0xffffffff;
#endif
#endif

#ifdef _PETSKILL_BECOMEPIG
    ch->data[CHAR_BECOMEPIG] = -1;
	ch->data[CHAR_BECOMEPIG_BBI] = 100250;
#endif

	ch->data[CHAR_LASTLEAVETIME] = 0;

	return TRUE;
}

#ifdef _HELP_NEWHAND
void CHAR_loginAddItemForNew( int charindex )
{
    int emptyitemindexinchara, itemindex;
	int	i;
	char	msgbuf[128];
	
	for( i = 0; i < 15; i ++ ) {
	    emptyitemindexinchara = CHAR_findEmptyItemBox( charindex );

		if( emptyitemindexinchara < 0 ){
			sprintf( msgbuf, "空间栏位不足。" );

			CHAR_talkToCli( charindex, -1, msgbuf, CHAR_COLORYELLOW);
			return;
		}
#ifdef _NEW_PLAYER_CF
		if(getNewplayergiveitem(i)==-1)continue;
		itemindex = ITEM_makeItemAndRegist( getNewplayergiveitem(i) );
#endif
	    if( itemindex != -1 ){
	        CHAR_setItemIndex( charindex, emptyitemindexinchara, itemindex );
	        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
	        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charindex);
	       	CHAR_sendItemDataOne( charindex, emptyitemindexinchara);
		}
	}
	print("Get New Hand Item");
}
#endif

void CHAR_createNewChar( int clifd, int dataplacenum, char* charname ,
						  int imgno,int faceimgno,
						  int vital,int str,int tgh,int dex,
						  int earth,int water,int fire,int wind,
						  int hometown , char *cdkey )
{

	Char    ch, *chwk;
	int		charaindex, petindex;
	int		enemyarray;
	char szKey[256];

#ifdef _NEW_PLAYER_CF	//新手出生配置
	int add_pet[]={0,0,0,0,0};
#endif

	memset(&ch,0,sizeof(Char));

	if( !CHAR_checkPlayerImageNumber( imgno)) {
		lssproto_CreateNewChar_send( clifd ,FAILED, "" );
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	if( !CHAR_checkFaceImageNumber( imgno, faceimgno)) {
		lssproto_CreateNewChar_send( clifd ,FAILED, "" );
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	CHAR_getDefaultChar(&ch,imgno);
	ch.data[CHAR_WALKINTERVAL] = getWalksendinterval();

	if( CHAR_getInitElderPosition( &ch ,hometown) == FALSE ){
		lssproto_CreateNewChar_send( clifd ,FAILED, "" );
        CONNECT_setState( clifd, NOTLOGIN );
		return;

	}else if( MAP_checkCoordinates( ch.data[CHAR_FLOOR], ch.data[CHAR_X],
									ch.data[CHAR_Y] ) == FALSE ){
		lssproto_CreateNewChar_send( clifd ,FAILED, "");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	strcpysafe( ch.string[CHAR_NAME].string ,
				sizeof(ch.string[CHAR_NAME].string),
				charname);

	if( CHAR_getNewImagenumberFromEquip( imgno,0) == -1 ){
		lssproto_CreateNewChar_send( clifd ,FAILED, "");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	ch.data[CHAR_BASEIMAGENUMBER] = imgno;
	ch.data[CHAR_BASEBASEIMAGENUMBER] = imgno;
	ch.data[CHAR_FACEIMAGENUMBER] = faceimgno;
	ch.data[CHAR_DATAPLACENUMBER] = dataplacenum;

#ifdef _FM_JOINLIMIT
	ch.data[CHAR_FMTIMELIMIT] = 0;
#endif

#ifdef _NEWSAVE
	ch.data[CHAR_SAVEINDEXNUMBER] = -1;
#endif
	ch.data[CHAR_HP] = 0x7fffffff;

	ch.data[CHAR_CHARM] = 60;
	ch.data[CHAR_MAXMP] = ch.data[CHAR_MP] = 100;
	strcpysafe( ch.string[CHAR_CDKEY].string,
                sizeof( ch.string[CHAR_CDKEY].string), cdkey );
  
        // Robin 0724
        ch.data[CHAR_RIDEPET] = -1;
#ifdef _NEW_PLAYER_CF
        ch.data[CHAR_LEARNRIDE] = getRidePetLevel();
#endif
#ifdef _NEW_RIDEPETS
		ch.data[CHAR_LOWRIDEPETS] = 0;
#endif

#ifdef _PERSONAL_FAME	// Arminius: 家族个人声望
	ch.data[CHAR_FAME] = 0;
#endif
#ifdef _NEW_MANOR_LAW
	ch.data[CHAR_MOMENTUM] = 0;	// 个人气势
#endif
#ifdef _TEACHER_SYSTEM
	ch.data[CHAR_TEACHER_FAME] = 0;	// 导师领导声望
#endif

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
	ch.data[PROFESSION_CLASS] = 0;
	ch.data[PROFESSION_LEVEL] = 0;
//	ch.data[PROFESSION_EXP] = 0;
	ch.data[PROFESSION_SKILL_POINT] = 0;
	ch.data[ATTACHPILE] = 0;
#endif

#ifdef _TEACHER_SYSTEM
	memset(ch.string[CHAR_TEACHER_ID].string,0,sizeof(STRING128));
	memset(ch.string[CHAR_TEACHER_NAME].string,0,sizeof(STRING128));
#endif

#ifdef _ITEM_SETLOVER
	memset(ch.string[CHAR_LOVERID].string,0,sizeof(STRING128));
	memset(ch.string[CHAR_LOVERNAME].string,0,sizeof(STRING128));
#endif

#ifdef _PETSKILL_BECOMEPIG
    ch.data[CHAR_BECOMEPIG] = -1;
	ch.data[CHAR_BECOMEPIG_BBI] = 100250;
#endif

	strcpysafe( ch.string[CHAR_FMNAME].string,
                sizeof( ch.string[CHAR_FMNAME].string), "\0" );
	ch.data[CHAR_FMINDEX] = -1;
#ifdef _OFFLINE_SYSTEM
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep); /*取得当地时间*/
	int mon=1+p->tm_mon;
	int date=p->tm_mday;
	char offbuf[64];
	sprintf(offbuf,"%d-%d-%d",mon,date,0);
	strcpysafe( ch.string[CHAR_OFFBUF].string,
                sizeof( ch.string[CHAR_OFFBUF].string), offbuf );
  //ch.data[CHAR_OFFTIME] = 18000;
  ch.data[CHAR_OFFTIME] = 1;
#endif
	if( CHAR_makeCharFromOptionAtCreate( &ch,
										vital,str,tgh,dex,
										earth,water,fire,wind ) == FALSE )
	{
		lssproto_CreateNewChar_send( clifd ,FAILED,
									 "option data is invalid\n");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
	{
		int mp=ch.data[CHAR_MAXMP];
		int str=ch.data[CHAR_STR];
		int tough=ch.data[CHAR_TOUGH];

		ch.data[CHAR_INITDATA] = 2*100*100*100*100 /*+ hp*100*100*100*/
			+ mp*100*100 + str* 100 + tough;
	}

	charaindex =  CHAR_initCharOneArray( &ch );
	if( charaindex == -1 ) {
		lssproto_CreateNewChar_send( clifd ,FAILED, "在线人数已满，无法建立人物");
        CONNECT_setState( clifd, NOTLOGIN );
		return;
	}
#ifdef _ALLBLUES_LUA_1_5
	FreeCharCreate(charaindex);
#endif
#ifdef _NEW_PLAYER_CF	//新手出生配置
		int petNum=0;
		int k=0;
		int mylevel,level;
		if(getNewplayergivepet(0)==-1){
			if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 1 )//玛丽娜丝
				setNewplayergivepet(0,2);
			else if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 2 )//加加
				setNewplayergivepet(0,3);
			else if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) == 3 )//卡鲁它那
				setNewplayergivepet(0,4);
			else 
				setNewplayergivepet(0,1);
		}
#ifdef _NEW_PLAYER_RIDE
		if(!strstr( getPlayerRide(), "不送配套骑宠"))
			CHAR_PlayerRide(charaindex);
#endif
		for( petNum=0; petNum<5; petNum++)	{
			if(getNewplayergivepet(petNum)==0)continue;
			enemyarray = ENEMY_getEnemyArrayFromId( getNewplayergivepet(petNum)); //白虎
			petindex = ENEMY_createPetFromEnemyIndex( charaindex, enemyarray);
			if( petindex == -1 ) {
				continue;
			}
			add_pet[petNum]=petindex;
			mylevel = CHAR_getInt( petindex, CHAR_LV);
			level = getNewplayerpetlv()-mylevel;
			if(level<1)level=1;
			for( k = 1; k < level; k ++ ){	//升级
				CHAR_PetLevelUp( petindex );
				CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
				CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) +1 );
			}
			CHAR_setInt( petindex , CHAR_VARIABLEAI , 10000 );
			CHAR_complianceParameter( petindex );
			CHAR_setInt( petindex , CHAR_HP , CHAR_getWorkInt( petindex , CHAR_WORKMAXHP ) );
		}
		setNewplayergivepet(0,-1);
#endif

#ifdef _HELP_NEWHAND
	CHAR_loginAddItemForNew(charaindex); 
#endif

#if 1
	if( CHAR_makeDBKey(  charaindex, szKey, sizeof( szKey ) ) != FALSE ){
		int dp;
		char info[512];
		snprintf( info, sizeof( info),
					"%d,%d,%d,%d,%d,%d",
					CHAR_getInt( charaindex, CHAR_LV),
					CHAR_getInt( charaindex, CHAR_DUELBATTLECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELLOSECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELSTWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELMAXSTWINCOUNT));
		dp = CHAR_getInt( charaindex, CHAR_DUELPOINT );
		saacproto_DBUpdateEntryInt_send(
			acfd,
			DB_DUELPOINT,
			szKey,
			(max(dp,0)),
			info,
      CONNECT_getFdid(clifd ),0 );
	}
#endif
    chwk = CHAR_getCharPointer( charaindex);

	CHAR_charSaveFromConnectAndChar( clifd,chwk ,FALSE );
	
	CHAR_endCharData(&ch);
	CHAR_endCharOneArray( charaindex);
#ifdef _NEW_PLAYER_CF	//新手出生配置
	{
		int k=0;
		for( k=0;k<arraysizeof( add_pet); k++)	{
			CHAR_endCharOneArray( add_pet[k]);
		}
	}
#endif
  CONNECT_setState( clifd, WHILECREATE );

}

static void CHAR_setCharFuncTable( Char *ch)
{
	int i;
	char *tmp[CHAR_FUNCTABLENUM] = {
		"",                 /*  CHAR_INITFUNC */
		"core_PreWalk",     /*  CHAR_WALKPREFUNC    */
		"core_PostWalk",    /*  CHAR_WALKPOSTFUNC   */
		"",                 /*  CHAR_PREOVERFUNC    */
		"",                 /*  CHAR_PREOVERFUNC    */
		"core_PlayerWatch", /*  CHAR_WATCHFUNC  */
#ifdef _FIX_CORE_LOOP
		"",        /*  CHAR_LOOPFUNC */
#else
		"core_Loop",        /*  CHAR_LOOPFUNC */
#endif
		"core_Dying",       /*  CHAR_DYINGFUNC */
		"core_PlayerTalked",/*  CHAR_TALKEDFUNC */
		"",                 /*  CHAR_PREATTACKEDFUNC    */
		"",                 /*  CHAR_POSTATTACKEDFUNC    */
		"",                 /*  CHAR_OFFFUNC    */
		"",                 /*  CHAR_LOOKEDFUNC */
		"",                 /*  CHAR_ITEMPUTFUNC    */
		"",                 /*  CHAR_SPECIALTALKEDFUNC    */
		"",                 /*  CHAR_WINDOWTALKEDFUNC    */
#ifdef _USER_CHARLOOPS
		"",					//	CHAR_LOOPFUNCTEMP1,
		"",					//	CHAR_LOOPFUNCTEMP2,
		"",					//CHAR_BATTLEPROPERTY,
#endif
	};
	for( i = 0; i < CHAR_FUNCTABLENUM; i ++ ) {
		strcpysafe( ch->charfunctable[i].string,
					sizeof( ch->charfunctable[i]),
					tmp[i]);
	}
}

void CHAR_loginAddItem( int charaindex )
{
/*
	int emptyindex, itemindex, id;
	int itemID[10]={	20128, 20130, 20171, 20176, 20296,
					20287, 20288, 20289, 20290, 20291};

	{
		int trn_num=0;
		trn_num=CHAR_getInt(charaindex,CHAR_TRANSMIGRATION);
		CHAR_setInt( charaindex, CHAR_MAXPOOLITEMHAVELIMIT, 10 + (trn_num * 4) )	;	
		CHAR_setInt( charaindex, CHAR_MAXPOOLPETHAVELIMIT , 5 + (trn_num * 2) );
	}
	while( 1){
		emptyindex = CHAR_findEmptyItemBox( charaindex );
		if( emptyindex == -1 ) break;
		id = rand()%10;
		itemindex = ITEM_makeItemAndRegist( itemID[id] );
		if( !ITEM_CHECKINDEX( itemindex) ) break;
		CHAR_setItemIndex( charaindex, emptyindex, itemindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charaindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);
		//andy_log
		print("@:%d.", emptyindex);
	}
	while( 1){
		emptyindex = CHAR_findEmptyPoolItemBox( charaindex );
		if( emptyindex == -1 ) break;
		id = rand()%10;
		itemindex = ITEM_makeItemAndRegist( itemID[id] );
		if( !ITEM_CHECKINDEX( itemindex) ) break;

		CHAR_setPoolItemIndex( charaindex, emptyindex, itemindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charaindex);
		ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);
		//andy_log
		print("&:%d.", emptyindex);
	}
*/
}

void CHAR_loginCheckUserItem( int charaindex)
{
	typedef void (*ATTACHFUNC)(int,int);
	int i, j, itemindex;
	int ID1,ID2;
	char* delbuf;
	char delitembuf[10];
	delbuf = getDelItem();
	int myviptype = CHAR_getInt(charaindex,CHAR_VIPRIDE); 
	for( i=0; i<CHAR_STARTITEMARRAY; i++){ //检查装备中道具
		int ti;
		itemindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex)){
			continue;
		}
		ITEM_reChangeItemName( itemindex);
#ifdef _ITEM_PILENUMS
		ITEM_reChangeItemToPile( itemindex);
#endif
		ITEM_setItemUniCode( itemindex);
		ID1 = ITEM_getInt( itemindex, ITEM_ID);
#ifdef	_YUANGUSA		
		if(ID1>=23228 && ID1<=23307){
			if(strstr(ITEM_getChar(itemindex,ITEM_EFFECTSTRING),"需更换")==NULL){
				ITEM_setChar(itemindex,ITEM_EFFECTSTRING,"[无属性，需更换]请到渔村村长家(13.5)找更换大师");
			}
		}
		if(((ID1>=3000 && ID1<=11719) || (ID1>=14001 && ID1<=18530)) && ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE)<1){
			int maxcrushe = ITEMTBL_getInt(ID1,ITEM_MAXDAMAGECRUSHE);
			ITEM_setInt(itemindex,ITEM_MAXDAMAGECRUSHE,maxcrushe);
			ITEM_setInt(itemindex,ITEM_DAMAGECRUSHE,maxcrushe);
		}
#endif
		if(myviptype==0){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP")!=NULL){
				CHAR_setItemIndex( charaindex , i, -1);
			}
		}else if(myviptype==1){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP2]")!=NULL || strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL){
				CHAR_setItemIndex( charaindex , i, -1);
			}
		}else if(myviptype==2){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL ){
				CHAR_setItemIndex( charaindex , i, -1);
			}
		}
#ifdef _JK_CF_DELPETITEM        //删除CF指定物品
		for( j=1;j<=30;j++ ) {
			if(getStringFromIndexWithDelim(delbuf,",", j, delitembuf, sizeof(delitembuf)) == FALSE)
			break;
			ID2 = atoi(delitembuf);
			if(ID2 == -1)break;
			if( ID1 == ID2){
				CHAR_setItemIndex( charaindex , i, -1);
				LogItem(CHAR_getChar( charaindex, CHAR_NAME ), //人物名
								CHAR_getChar( charaindex, CHAR_CDKEY ), //帐号
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
								itemindex,
#else
       					ITEM_getInt( itemindex, ITEM_ID ),
#endif
								"CF设置清除物品",
								CHAR_getInt( charaindex,CHAR_FLOOR),
								CHAR_getInt( charaindex,CHAR_X ),
 	      				CHAR_getInt( charaindex,CHAR_Y ),
            		ITEM_getChar( itemindex, ITEM_UNIQUECODE),
								ITEM_getChar( itemindex, ITEM_NAME),
								ITEM_getInt( itemindex, ITEM_ID)
								);
				CHAR_talkToCli( charaindex, -1, "系统清除你身上的道具", CHAR_COLORWHITE );
			}
		}
#endif
		
#ifdef _ANGEL_SUMMON
		if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM 
			|| ITEM_getInt( itemindex, ITEM_ID) == HEROITEM ) {
			int mindex, mission;
			// 检查信物所有人是否任务中
			mindex = checkIfAngelByName( ITEM_getChar( itemindex, ITEM_ANGELINFO));
			mission = atoi( ITEM_getChar( itemindex, ITEM_ANGELMISSION));
			if( mindex < 0 || mission != missiontable[mindex].mission ) {
				CHAR_setItemIndex( charaindex, i, -1 );
				ITEM_endExistItemsOne( itemindex);
				CHAR_sendItemDataOne( charaindex, i);
				continue;
			}

			if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM ) {
				char nameinfo[128];
				getMissionNameInfo( charaindex, nameinfo);
				// 检查使者信物装备中
				if( !strcmp( ITEM_getChar( itemindex, ITEM_ANGELINFO), nameinfo) ) {
					//CHAR_setWorkInt( charaindex, CHAR_WORKANGELMODE, TRUE);
					CHAR_sendAngelMark( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX), 1);
				}
			}
		}
#endif
		//Change fix 2004/07/05
		//与人物职业不合的道具拿下//////////////////
		if( ITEM_getInt( charaindex, ITEM_NEEDPROFESSION) != 0 ){
			if( CHAR_getInt( charaindex, PROFESSION_CLASS ) != ITEM_getInt( itemindex, ITEM_NEEDPROFESSION) 
				&& ITEM_getInt( itemindex, ITEM_NEEDPROFESSION) > 0 ){
				if( (ti = CHAR_findEmptyItemBox( charaindex )) != -1 ){
					CHAR_setItemIndex( charaindex , i, -1);
					CHAR_setItemIndex( charaindex , ti, itemindex);
				}
				continue;
			}
		}
		////////////////////////////////////////////


		if( (i == ITEM_getEquipPlace( charaindex, itemindex )) ||
			i == CHAR_DECORATION1 || i == CHAR_DECORATION2 ){
			char category[256];
			ATTACHFUNC atf;
			atf=(ATTACHFUNC)ITEM_getFunctionPointer(itemindex,ITEM_ATTACHFUNC);
			if( atf ){
				atf( charaindex, itemindex );
			}
#ifdef _ALLBLUES_LUA_1_2 
			else{
				RunItemAttachEvent(charaindex, itemindex );			
			}
#endif

			if( ITEM_canuseMagic( itemindex)) {
				snprintf( category, sizeof( category), "J%d",i);
				CHAR_sendStatusString( charaindex, category );
			}
		}else{
			if( (ti = CHAR_findEmptyItemBox( charaindex )) == -1 ) continue;
			CHAR_setItemIndex( charaindex , i, -1);
			CHAR_setItemIndex( charaindex , ti, itemindex);
		}
		
	}
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){ //检查携带道具
		itemindex = CHAR_getItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex)) continue;
		ITEM_reChangeItemName( itemindex);
#ifdef _ITEM_PILENUMS
		ITEM_reChangeItemToPile( itemindex);
#endif

		ID1 = ITEM_getInt( itemindex, ITEM_ID);
#ifdef	_YUANGUSA		
		if(ID1>=23228 && ID1<=23307){
			if(strstr(ITEM_getChar(itemindex,ITEM_EFFECTSTRING),"需更换")==NULL){
				ITEM_setChar(itemindex,ITEM_EFFECTSTRING,"[无属性，需更换]请到渔村村长家(13.5)找更换大师");
			}
		}
		if(((ID1>=3000 && ID1<=11719) || (ID1>=14001 && ID1<=18530)) && ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE)<1){
			int maxcrushe = ITEMTBL_getInt(ID1,ITEM_MAXDAMAGECRUSHE);
			ITEM_setInt(itemindex,ITEM_MAXDAMAGECRUSHE,maxcrushe);
			ITEM_setInt(itemindex,ITEM_DAMAGECRUSHE,maxcrushe);
		}
#endif
		if(myviptype==0){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP")!=NULL){
				CHAR_setItemIndex( charaindex , i, -1);
			}
		}else if(myviptype==1){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP2]")!=NULL || strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL){
				CHAR_setItemIndex( charaindex , i, -1);
			}
		}else if(myviptype==2){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL ){
				CHAR_setItemIndex( charaindex , i, -1);
			}
		}		
#ifdef _JK_CF_DELPETITEM      //删除CF指定物品
		for( j=1;j<=30;j++ ) {
			if(getStringFromIndexWithDelim(delbuf,",", j, delitembuf, sizeof(delitembuf)) == FALSE)
			break;
			ID2 = atoi(delitembuf);
			if(ID2 == -1)break;
			if( ID1 == ID2){
				CHAR_setItemIndex( charaindex , i, -1);
				LogItem(CHAR_getChar( charaindex, CHAR_NAME ), //人物名
								CHAR_getChar( charaindex, CHAR_CDKEY ), //帐号
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
								itemindex,
#else
       					ITEM_getInt( itemindex, ITEM_ID ),
#endif
								"CF设置清除物品",
								CHAR_getInt( charaindex,CHAR_FLOOR),
								CHAR_getInt( charaindex,CHAR_X ),
 	      				CHAR_getInt( charaindex,CHAR_Y ),
            		ITEM_getChar( itemindex, ITEM_UNIQUECODE),
								ITEM_getChar( itemindex, ITEM_NAME),
								ITEM_getInt( itemindex, ITEM_ID)
								);
				CHAR_talkToCli( charaindex, -1, "系统清除你身上的道具", CHAR_COLORWHITE );
			}
		}
#endif

#ifdef _ANGEL_SUMMON
		if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM 
			|| ITEM_getInt( itemindex, ITEM_ID) == HEROITEM ) {
			int mindex, mission;
			// 检查信物所有人是否任务中
			mindex = checkIfAngelByName( ITEM_getChar( itemindex, ITEM_ANGELINFO));
			mission = atoi( ITEM_getChar( itemindex, ITEM_ANGELMISSION));
			if( mindex < 0 || mission != missiontable[mindex].mission ) {
				CHAR_setItemIndex( charaindex, i, -1 );
				ITEM_endExistItemsOne( itemindex);
				CHAR_sendItemDataOne( charaindex, i);
				continue;
			}
		}
#endif
		ITEM_setItemUniCode( itemindex);

	}
	//检查寄放店道具
	for( i = 0 ; i < CHAR_MAXPOOLITEMHAVE ; i++ ){
		itemindex = CHAR_getPoolItemIndex( charaindex , i );
		if( !ITEM_CHECKINDEX( itemindex) ) continue;

		ITEM_reChangeItemName( itemindex);
#ifdef _ITEM_PILENUMS
		ITEM_reChangeItemToPile( itemindex);
#endif

		ID1 = ITEM_getInt( itemindex, ITEM_ID);
#ifdef	_YUANGUSA		
		if(ID1>=23228 && ID1<=23307){
			if(strstr(ITEM_getChar(itemindex,ITEM_EFFECTSTRING),"需更换")==NULL){
				ITEM_setChar(itemindex,ITEM_EFFECTSTRING,"[无属性，需更换]请到渔村村长家(13.5)找更换大师");
			}
		}
		if(((ID1>=3000 && ID1<=11719) || (ID1>=14001 && ID1<=18530)) && ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE)<1){
			int maxcrushe = ITEMTBL_getInt(ID1,ITEM_MAXDAMAGECRUSHE);
			ITEM_setInt(itemindex,ITEM_MAXDAMAGECRUSHE,maxcrushe);
			ITEM_setInt(itemindex,ITEM_DAMAGECRUSHE,maxcrushe);
		}
#endif
		if(myviptype==0){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP")!=NULL){
				CHAR_setPoolItemIndex( charaindex , i, -1);
			}
		}else if(myviptype==1){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP2]")!=NULL || strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL){
				CHAR_setPoolItemIndex( charaindex , i, -1);
			}
		}else if(myviptype==2){
			if(strstr(ITEM_getChar(itemindex,ITEM_NAME),"[VIP3]")!=NULL ){
				CHAR_setPoolItemIndex( charaindex , i, -1);
			}
		}		
#ifdef _JK_CF_DELPETITEM      //删除CF指定物品
		for( j=1;j<=30;j++ ) {
			if(getStringFromIndexWithDelim(delbuf,",", j, delitembuf, sizeof(delitembuf)) == FALSE)
			break;
			ID2 = atoi(delitembuf);
			if(ID2 == -1)break;
			if( ID1 == ID2){
				CHAR_setPoolItemIndex( charaindex , i, -1);
				LogItem(CHAR_getChar( charaindex, CHAR_NAME ), //人物名
								CHAR_getChar( charaindex, CHAR_CDKEY ), //帐号
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
								itemindex,
#else
       					ITEM_getInt( itemindex, ITEM_ID ),
#endif
								"CF设置清除物品",
								CHAR_getInt( charaindex,CHAR_FLOOR),
								CHAR_getInt( charaindex,CHAR_X ),
 	      				CHAR_getInt( charaindex,CHAR_Y ),
            		ITEM_getChar( itemindex, ITEM_UNIQUECODE),
								ITEM_getChar( itemindex, ITEM_NAME),
								ITEM_getInt( itemindex, ITEM_ID)
								);
				CHAR_talkToCli( charaindex, -1, "系统清除你寄放店里的道具", CHAR_COLORWHITE );
			}
		}
#endif

#ifdef _ANGEL_SUMMON
		if( ITEM_getInt( itemindex, ITEM_ID) == ANGELITEM 
			|| ITEM_getInt( itemindex, ITEM_ID) == HEROITEM ) {
			int mindex, mission;
			// 检查信物所有人是否任务中
			mindex = checkIfAngelByName( ITEM_getChar( itemindex, ITEM_ANGELINFO));
			mission = atoi( ITEM_getChar( itemindex, ITEM_ANGELMISSION));
			if( mindex < 0 || mission != missiontable[mindex].mission ) {
				CHAR_setPoolItemIndex( charaindex, i, -1 );
				ITEM_endExistItemsOne( itemindex);
				continue;
			}
		}
#endif
		
		ITEM_setItemUniCode( itemindex);
	}

	// WON ADD 修正道具的设定问题
	if(!NPC_EventCheckFlg( charaindex,134)){
		for (i = 0; i < CHAR_MAXITEMHAVE; i ++) {
			itemindex = CHAR_getItemIndex(charaindex, i);
			fix_item_bug(charaindex, itemindex);
		}	
		for (i = 0; i < CHAR_MAXPOOLITEMHAVE; i++) {
			itemindex = CHAR_getPoolItemIndex(charaindex, i);
			fix_item_bug(charaindex, itemindex);
		}
		NPC_EventSetFlg(charaindex, 134);
	}
}
void CHAR_LoginBesideSetWorkInt( int charaindex, int clifd)
{
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYMODE, CHAR_PARTY_NONE);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX1, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX2, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX3, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX4, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKPARTYINDEX5, -1);

	CHAR_setWorkInt( charaindex, CHAR_WORKTRADEMODE, CHAR_TRADE_FREE);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMINDEXI, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMCHANNEL, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMCHANNELQUICK, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMFLOOR, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMDP, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMPKFLAG, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMSETUPFLAG, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMMANINDEX, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFMCHARINDEX, -1);
	
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEFLAG, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET0_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET1_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET2_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET3_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORK_PET4_STAT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORKACTION, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, -1);
	CHAR_setWorkInt( charaindex, CHAR_WORKFD, clifd);


	if( CHAR_getInt( charaindex, CHAR_SILENT) > 100000000 )
		CHAR_setInt( charaindex, CHAR_SILENT, 0);
	CHAR_setWorkInt( charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec);
	CHAR_setWorkInt( charaindex, CHAR_WORKTALKCOUNT, 0 );
	CHAR_setWorkInt( charaindex, CHAR_WORKTALKTIME, (int)NowTime.tv_sec);
	CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, -1);
	CHAR_setWorkInt( charaindex, CHAR_LASTCHARSAVETIME, (int)NowTime.tv_sec);
#ifdef _STATUS_WATERWORD //水世界状态
	if( MAP_getMapFloorType( CHAR_getInt( charaindex, CHAR_FLOOR)) == 1 ){
		CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 1);
	}else{
		CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 0);
	}
#endif
#ifdef _NEWOPEN_MAXEXP
	if( CHAR_getInt( charaindex, CHAR_OLDEXP) > 0 || CHAR_getInt( charaindex, CHAR_OLDEXP) < 0 ){
		CHAR_ChangeExp( charaindex);
	}
#endif
#ifdef _PETSKILL_BECOMEFOX
    CHAR_setWorkInt( charaindex, CHAR_WORKFOXROUND, -1 );
#endif
#ifdef _MAP_TIME
	CHAR_setWorkInt(charaindex,CHAR_WORK_MAP_TIME,0);	
#endif
#ifdef _PETSKILL_LER
	CHAR_setWorkInt(charaindex,CHAR_WORK_RELIFE,0);
#endif
#ifdef	_NO_DROPITEM
	CHAR_setWorkInt(charaindex,CHAR_DROPITEMCHARAINDEX,-1);
#endif
}

void CHAR_CheckProfessionSkill( int charaindex)
{
	int i, Pclass, skpoint=0, mynum=0;
	CHAR_HaveSkill *pSkil;

	if( NPC_EventCheckFlg( charaindex, 155) ) return;

	Pclass = CHAR_getInt( charaindex, PROFESSION_CLASS );
	skpoint = CHAR_getInt( charaindex, PROFESSION_SKILL_POINT);
	if( Pclass == PROFESSION_CLASS_NONE )return;


	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
			
		// 技能ID
		int skillID = CHAR_getCharSkill( charaindex, i);

		if( skillID < 0 ) continue;
		
		switch( skillID ){
		case 63:
			mynum = CHAR_getInt( charaindex, CHAR_TOUGH);
			if( Pclass == PROFESSION_CLASS_FIGHTER ) mynum = mynum-25;
			if( Pclass == PROFESSION_CLASS_WIZARD )	mynum = mynum-10;
			if( Pclass == PROFESSION_CLASS_HUNTER ) mynum = mynum-15;
			if( mynum < 0 ) mynum = 0;
			CHAR_setInt( charaindex, CHAR_TOUGH, mynum);
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, skpoint+1 );		
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
			pSkil->use = 0;
			break;
		case 64:
			CHAR_setInt( charaindex, CHAR_MAXMP, 100);
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, skpoint+1 );
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
			pSkil->use = 0;
			break;
		case 65:
			CHAR_setInt( charaindex, ATTACHPILE, 0);
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, skpoint+1 );
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
			pSkil->use = 0;
			break;
		}
	}
	{
		int myskillpoint, myskillnum=0, myskilllevel;
		int mysknum=0;

		myskillpoint= CHAR_getInt( charaindex, PROFESSION_SKILL_POINT );
		myskilllevel= CHAR_getInt( charaindex, PROFESSION_LEVEL );

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			if( CHAR_getCharSkill( charaindex, i) <= 0 ) continue;
			myskillnum++;
		}
		mysknum = myskilllevel - (myskillpoint + myskillnum);

		if( mysknum > 0 ){
			CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, myskillpoint + mysknum );
		}else if( mysknum < 0 ){
			int defsknum = (myskillpoint + myskillnum)-myskilllevel;
			while( 1){
				if( defsknum<= 0 ||
					(myskillpoint= CHAR_getInt( charaindex, PROFESSION_SKILL_POINT )) <= 0 ) break;
				CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, myskillpoint-1);
				defsknum--;
			}
		}
	}

	NPC_EventSetFlg( charaindex, 155);
}

void CHAR_ResetPoolPetEgg( int charaindex)
{
	int levelup, vital, str, tgh, dex;
	int raise, petindex, i;


	if( NPC_EventCheckFlg( charaindex, 157) ) return;

	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		petindex = CHAR_getCharPoolPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;

		if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) != 1 ||
			CHAR_getInt( petindex, CHAR_FUSIONRAISE) <= 0 ) continue;

		raise = CHAR_getInt( petindex, CHAR_FUSIONRAISE);
		raise--;
		CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time(NULL)+PETFEEDTIME);
		if( raise < 1 ) raise = 1;
		if( raise >= 40 ) raise = 30;
		CHAR_setInt( petindex, CHAR_FUSIONRAISE, raise);

		levelup = CHAR_getInt( petindex, CHAR_ALLOCPOINT);
		vital = ((levelup>>24) & 0xFF) + 4;
		str   = ((levelup>>16) & 0xFF) + 4;
		tgh   = ((levelup>> 8) & 0xFF) + 4;
		dex   = ((levelup>> 0) & 0xFF) + 4;
		if( vital < 0 ) vital = 0;
		if( str < 0 ) str = 0;
		if( tgh < 0 ) tgh = 0;
		if( dex < 0 ) dex = 0;
		//扣属性
		levelup = (vital<<24) + (str<<16) + (tgh<<8) + (dex<<0);
		CHAR_setInt( petindex, CHAR_ALLOCPOINT, levelup);
	}
	NPC_EventSetFlg( charaindex, 157);
}

// Robin add 重设宠蛋喂养时间
void CHAR_ResetPetEggFusionTime(int charaindex)
{
	int i, petindex;
	int nowTime =0;
	//int leaveTime =0;
	//int lastFeedTime =0;
	//int anhour = PETFEEDTIME;

	//if( NPC_EventCheckFlg( charaindex, 157) ) return;
	
	nowTime = (int)time(NULL);
	//leaveTime = CHAR_getInt( charaindex, CHAR_LASTLEAVETIME);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
		petindex = CHAR_getCharPet( charaindex, i);
		if( !CHAR_CHECKINDEX( petindex) ) continue;

		if( CHAR_getInt( petindex, CHAR_FUSIONBEIT) != 1 ||
			CHAR_getInt( petindex, CHAR_FUSIONRAISE) <= 0 ) continue;

		//lastFeedTime = CHAR_getInt( petindex, CHAR_FUSIONTIMELIMIT);
		//if( leaveTime > 0) {
		//	CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, nowTime- (leaveTime-lastFeedTime));
		//}
		//else
		//	CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, nowTime);

		CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, nowTime);

	}

	//NPC_EventSetFlg( charaindex, 157);

}


BOOL CHAR_CheckProfessionEquit( int toindex)
{
	int i, j, itemindex, newindex;
	int FixItem[16]={
		2028,2029,2030,2031,2032,2033,2034,2035,
		2036,2037,2038,2039,2040,2041,2042,2043
	};
	CHAR_HaveSkill *pSkil;

	if(!NPC_EventCheckFlg( toindex, 149)){
		for (i = 0; i < CHAR_MAXITEMHAVE; i ++) {
			itemindex = CHAR_getItemIndex( toindex, i);
			if( !ITEM_CHECKINDEX( itemindex)) continue;

			for( j=0; j<16; j++){
				if( ITEM_getInt( itemindex, ITEM_ID) == FixItem[j] ){
					CHAR_setItemIndex( toindex, i, -1 );
					ITEM_endExistItemsOne( itemindex);
					newindex = ITEM_makeItemAndRegist( FixItem[j]);
					if( !ITEM_CHECKINDEX( newindex) ) break;
					CHAR_setItemIndex( toindex , i, newindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, toindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);

					CHAR_sendItemDataOne( toindex, i);

				}
			}
		}	

		for (i = 0; i < CHAR_MAXPOOLITEMHAVE; i++){
			int itemindex = CHAR_getPoolItemIndex(toindex, i);
			if( !ITEM_CHECKINDEX( itemindex)) continue;
			for( j=0; j<16; j++){
				if( ITEM_getInt( itemindex, ITEM_ID) == FixItem[j] ){
					CHAR_setPoolItemIndex( toindex, i, -1);
					ITEM_endExistItemsOne( itemindex);

					newindex = ITEM_makeItemAndRegist( FixItem[j]);
					if( !ITEM_CHECKINDEX( newindex) ) break;
					CHAR_setPoolItemIndex( toindex , i, newindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, toindex);
					ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1);

				}
			}
		}

		NPC_EventSetFlg( toindex, 149);
	}


	if( NPC_EventCheckFlg( toindex, 156) ) return TRUE;

	// WON ADD
	for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
		if( CHAR_getIntPSkill( toindex, i, SKILL_IDENTITY) > 0 ) continue;

		pSkil = CHAR_getCharHaveSkill( toindex, i );
		SKILL_makeSkillData( &pSkil->skill, 0, 0 );	
		pSkil->use = 0;
	}
	{
		int myskillpoint= CHAR_getInt( toindex, PROFESSION_SKILL_POINT );
		int myskilllevel= CHAR_getInt( toindex, PROFESSION_LEVEL );
		int myskillnum = 0, mysknum = 0;

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			if( CHAR_getCharSkill( toindex, i) <= 0 ) continue;
			myskillnum++;
		}

		mysknum = myskilllevel - (myskillpoint + myskillnum);

		if( mysknum > 0 ){
			CHAR_setInt( toindex, PROFESSION_SKILL_POINT, myskillpoint + mysknum );
		}
	}

	{
		Skill work1[PROFESSION_MAX_LEVEL];
		int cnt=0;
		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			work1[i].data[SKILL_IDENTITY]=-1;
			work1[i].data[SKILL_LEVEL]=0;
		}

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			if( CHAR_getIntPSkill( toindex, i, SKILL_IDENTITY) <= 0 ) continue;
			work1[cnt].data[SKILL_IDENTITY] = CHAR_getIntPSkill( toindex, i, SKILL_IDENTITY);
			work1[cnt].data[SKILL_LEVEL] = CHAR_getIntPSkill( toindex, i, SKILL_LEVEL);
			cnt++;
		}
		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			print("%d,", work1[i].data[SKILL_IDENTITY] );
		}

		for( i=0; i<PROFESSION_MAX_LEVEL; i++ ){
			pSkil = CHAR_getCharHaveSkill( toindex, i );
			if( pSkil == NULL  ) continue;
			pSkil->use = 0;
			if( work1[i].data[SKILL_IDENTITY] == -1 ) continue;
			SKILL_makeSkillData( &pSkil->skill, work1[i].data[SKILL_IDENTITY], work1[i].data[SKILL_LEVEL] );
			pSkil->use = 1;
		}
	}

	NPC_EventSetFlg( toindex, 156);

	return TRUE;
}

static void CHAR_setLuck( int charaindex);

#ifdef _CHANNEL_MODIFY
extern int *piOccChannelMember;
#endif
void CHAR_login( int clifd, char* data, int saveindex )
{
	int charaindex,objindex;
	Char    ch;
	int per;
//	char c_temp2[4096];
	if( CHAR_makeCharFromStringToArg( data , &ch ) == FALSE ){
		fprint ("制作人物错误！\n");
		goto MAKECHARDATAERROR;
	}
	CHAR_setCharFuncTable( &ch);
	charaindex =  CHAR_initCharOneArray( &ch );
	if( charaindex == -1 ){
		fprint ("制作人物错误！\n");
		CHAR_endCharData(&ch);
		goto MAKECHARDATAERROR;
	}
#ifdef _NEWSAVE
	CHAR_setInt( charaindex, CHAR_SAVEINDEXNUMBER, saveindex);
	print("存档装载索引:%d\n", CHAR_getInt( charaindex, CHAR_SAVEINDEXNUMBER) );
#endif

	char cdkey[16];
	char passwd[16];
	CONNECT_getCdkey( clifd, cdkey, sizeof( cdkey ));
	CONNECT_getPasswd( clifd, passwd, sizeof(passwd));

	if(strcmp(cdkey,CHAR_getChar( charaindex, CHAR_CDKEY))!=0){
//		print( "修正人物账号%s=>%s\n",CHAR_getChar( charaindex, CHAR_CDKEY),cdkey);
//		CHAR_setChar( charaindex, CHAR_CDKEY, cdkey);
			print( "账号%s与档案人物账号%s不相同",cdkey,CHAR_getChar( charaindex, CHAR_CDKEY));
			goto DELETECHARDATA;
	}
	if(CHAR_getInt(charaindex,CHAR_LOCK)==1 && strstr(cdkey,passwd)!=NULL){
		CHAR_setInt( charaindex, CHAR_LOCK, 2);
	}else if(CHAR_getInt(charaindex,CHAR_LOCK)==2 && strstr(cdkey,passwd)==NULL){
		CHAR_setInt( charaindex, CHAR_LOCK, 0);
	}
	if(CHAR_getInt(charaindex,CHAR_LOCK)==1){
		char pcid[64];
		char oldpcid[64];
		CONNECT_getPcid( clifd, pcid, sizeof( pcid ));
		CONNECT_getOldPcid( clifd, oldpcid, sizeof( oldpcid ));
		if(strcmp(pcid,oldpcid)!=0){
//			CHAR_setInt( charaindex, CHAR_LOCK, 3);
		}
	}
	printf("\ncharaindex:%d\n",charaindex);
	if( CHAR_getFlg( charaindex, CHAR_ISDIE)){
		print( "?data? ISDIE flg is standing.\n");
		CHAR_setFlg( charaindex, CHAR_ISDIE, FALSE);
	}
#ifdef _ALLBLUES_LUA_1_5
	FreeCharLogin(charaindex);
#endif	

#ifndef _FIX_UNNECESSARY
	// Arminius 7.9 airplane logout
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
		int oyaindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
		if( CHAR_CHECKINDEX( oyaindex)) {
			if (CHAR_getInt( oyaindex, CHAR_WHICHTYPE ) == CHAR_TYPEBUS) {
				if ((CHAR_getInt( oyaindex, CHAR_BASEIMAGENUMBER) !=100355) &&
					(CHAR_getInt( oyaindex, CHAR_BASEIMAGENUMBER) !=100461))	{

					if(CHAR_getWorkInt(oyaindex,CHAR_NPCWORKINT5)==1) {
					  if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER)>=0){
						int fl,x,y;
						CHAR_getElderPosition( CHAR_getInt( charaindex, CHAR_LASTTALKELDER), &fl, &x, &y );
						CHAR_setInt(charaindex,CHAR_FLOOR,fl);
						CHAR_setInt(charaindex,CHAR_X,x);
						CHAR_setInt(charaindex,CHAR_Y,y);
					  }
					}
				}
			}
		}
	}
#endif

// Nuke 20040420: CHECK MAX POINT

#if 1 
  {
  	if(CHAR_getInt(charaindex,CHAR_SUPER)==0)
		{
    int lv,vi,str,tou,dx,skup,trn,teq,quest,level,total,max;
    float table[]={437,490,521,550,578,620,700,800,900}; //各转最高点数(减10)
    //float table[]={620,660,700,740,780,820}; //各转最高点数(减10)
    lv = CHAR_getInt(charaindex,CHAR_LV);
  	vi = CHAR_getInt(charaindex,CHAR_VITAL);
		str = CHAR_getInt(charaindex,CHAR_STR);
		tou = CHAR_getInt(charaindex,CHAR_TOUGH);
		dx = CHAR_getInt(charaindex,CHAR_DEX);
    trn = CHAR_getInt(charaindex,CHAR_TRANSMIGRATION);
    teq = CHAR_getInt(charaindex,CHAR_TRANSEQUATION);
    skup = CHAR_getInt(charaindex,CHAR_SKILLUPPOINT);
    quest=(teq >> 16)& 0xFF;
    level=teq & 0xFFF;
    total=(vi+str+tou+dx)/100+skup;
//    max=(trn==0)?(lv-1)*3+20+10: // 0转447
//      (lv-1)*3+table[trn-1]/12.0+quest/4.0+(level-trn*85)/4.0+10+1+10+trn*10;
/*
		print("升级点数:%d\n",(lv-1)*getSkup());
		print("继承点术:%f\n",(table[trn-1]/12.0));
		print("历史任务:%f\n",quest/4.0);
		print("历史等级:%f\n",(level-trn*85)/4.0);
		print("历史等级:%d/%d\n",level,trn*85);
		print("转前祝福:%d\n",getTransPoint(trn));
		print("误差1点:%d\n",1);
		print("转後祝福:%d\n",getTransPoint(trn));
		print("转生红利:%d\n",trn*10);
*/
#ifdef _REVLEVEL
	if (strcmp(getRevLevel(),"是")==0){
		if(trn>=getChartrans()){
			if(lv>getMaxLevel()){
				lv=getMaxLevel();
				print("[%s:%s]lv:%d->%d",
								CHAR_getChar(charaindex,CHAR_CDKEY),
							  CHAR_getChar(charaindex,CHAR_NAME),
							  CHAR_getInt(charaindex,CHAR_LV),
								getMaxLevel());
			}
		}else if(lv>getYBLevel()){
			lv=getYBLevel();
			print("[%s:%s]lv:%d->%d",
							CHAR_getChar(charaindex,CHAR_CDKEY),
						  CHAR_getChar(charaindex,CHAR_NAME),
						  CHAR_getInt(charaindex,CHAR_LV),
							getMaxLevel());
		}
		CHAR_setInt(charaindex,CHAR_LV,lv);
	}
#endif
		float jxds=(level-trn*85)/4.0;
		if(jxds<0)jxds=0;
#ifdef _SKILLUPPOINT_CF
		max=(trn==0)?getTransPoint(trn)+(lv-1)*getSkup()+20:
			getTransPoint(trn)+(lv-1)*getSkup()+table[trn-1]/12.0+quest/4.0+jxds+getTransPoint(trn)+ trn*10+1;
//		max=getTransPoint(trn);
#else
    max=(trn==0)?(lv-1)*3+20+10: // 0转447
      (lv-1)*3+table[trn-1]/12.0+quest/4.0+(level-trn*85)/4.0+10+1+10+trn*10;
#endif
/* 1转以上=升级点数+继承点术+历史任务+历史等级+转前祝福+误差1点+转後祝福+转生红利 */
//	if (trn==6) max=max-20; /* 六转时没有转前祝福与转後祝福 */
#ifdef _REVLEVEL
  if (total>max && strcmp(getPoint(),"否")==0)
#else
	if (total>max )
#endif
  {
	  print("\n重调点数[%s:%s]:%d->%d ",
	  		CHAR_getChar(charaindex,CHAR_CDKEY),
			  CHAR_getChar(charaindex,CHAR_NAME),
	  		total,max);
    CHAR_setInt(charaindex,CHAR_VITAL,1000);
    CHAR_setInt(charaindex,CHAR_STR,0);
    CHAR_setInt(charaindex,CHAR_TOUGH,0);
    CHAR_setInt(charaindex,CHAR_DEX,0);
    CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,max-10);
  }
	// 补足六转点数不足之玩家
/*	if ((trn==6) && (total < max))
	{
   		print("\n补足点数[%s:%s]:%d->%d ",
		  CHAR_getChar(charaindex,CHAR_CDKEY),
		  CHAR_getChar(charaindex,CHAR_NAME),
		  total,max);
		CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,
			CHAR_getInt(charaindex,CHAR_SKILLUPPOINT)+(max-total));
	}*/
  }
#ifdef	_SUPERMAN_FIX
	else{
		if(getSuperManPoint()>-1){
			int superpoint;
			superpoint = (CHAR_getInt(charaindex,CHAR_VITAL)+CHAR_getInt(charaindex,CHAR_STR)+CHAR_getInt(charaindex,CHAR_TOUGH)+CHAR_getInt(charaindex,CHAR_DEX))/100 + CHAR_getInt(charaindex,CHAR_SKILLUPPOINT);
			if(superpoint < getSuperManPoint()){
    		CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,CHAR_getInt(charaindex,CHAR_SKILLUPPOINT)+(getSuperManPoint()-superpoint));
			}else if(superpoint > getSuperManPoint()){
				CHAR_setInt(charaindex,CHAR_VITAL,1000);
   			CHAR_setInt(charaindex,CHAR_STR,0);
    		CHAR_setInt(charaindex,CHAR_TOUGH,0);
    		CHAR_setInt(charaindex,CHAR_DEX,0);
    		CHAR_setInt(charaindex,CHAR_SKILLUPPOINT,getSuperManPoint()-10);
			}
		}
	}
#endif
	}
#endif

	{
		int EQ_BBI=-1, EQ_ARM=-1, EQ_NUM=-1, EQ_BI=-1;
		int CH_BI = CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER);

		EQ_BBI = CHAR_getInt( charaindex, CHAR_BASEBASEIMAGENUMBER);
		if(CH_BI != EQ_BBI){
			EQ_ARM = CHAR_getItemIndex(charaindex,CHAR_ARM);
			EQ_NUM=ITEM_FIST;
			if ( ITEM_CHECKINDEX( EQ_ARM))	{
				EQ_NUM=ITEM_getInt(EQ_ARM,ITEM_TYPE);
			}
	
			EQ_BI=CHAR_getNewImagenumberFromEquip( EQ_BBI, EQ_NUM);
	
			if( CHAR_getInt( charaindex, CHAR_RIDEPET) == -1 )	{	//非骑宠
				if( EQ_BI != CH_BI )	{
					print("\n 非骑宠形象 %d [%d=>%d]", charaindex,
						CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER),
						EQ_BI );
					CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, EQ_BI);
				}
			}
		}
	}
	{
	  int x,y;
	  if( CHAR_isAppearPosition( CHAR_getInt( charaindex, CHAR_FLOOR),&x, &y ))	{
	  	int flelder, xelder, yelder;
			CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
	  			&flelder, &xelder, &yelder);
			   CHAR_setInt(charaindex, CHAR_FLOOR, flelder);
			   CHAR_setInt(charaindex, CHAR_X, xelder);
			   CHAR_setInt(charaindex, CHAR_Y, yelder);
	  }
	}

#ifdef _FIX_GAMBLENUM		
	{		
        if( CHAR_getInt(charaindex, CHAR_GAMBLENUM) < 0){
			CHAR_setInt(charaindex, CHAR_GAMBLENUM, 0);
		}
	}
#endif

#ifdef _MAP_NOEXIT
	{
		int exfloor=-1,ex_X=-1,ex_Y=-1;
		int map_type=0;
		unsigned int point;
		point = MAP_getExFloor_XY( CHAR_getInt(charaindex, CHAR_FLOOR) , &map_type);
		if( point > 0 )	{
			if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) >= 0 )	{
				CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER),
					&exfloor, &ex_X, &ex_Y);
			}
			if( map_type >= 0 )	{	// map_type >= 0 优先回纪录点
				if( exfloor == map_type && exfloor >= 0 )	{//回纪录点
				}else	{
					exfloor = (point>>16)&0xffffff;
					ex_X = (point>>8)&0xff;
					ex_Y = (point>>0)&0xff;
				}
			}
			if( CHECKFLOORID( exfloor) )	{	//检查房间号
				CHAR_setInt(charaindex, CHAR_FLOOR, exfloor);
				CHAR_setInt(charaindex, CHAR_X, ex_X);
				CHAR_setInt(charaindex, CHAR_Y, ex_Y);
			}
		}
	}
#endif
	if( CHAR_getInt(charaindex,CHAR_X) < 0 || CHAR_getInt(charaindex,CHAR_Y) < 0 ){
		CHAR_setInt(charaindex,CHAR_X,1);
		CHAR_setInt(charaindex,CHAR_Y,1);
	}
#ifdef _MAP_TIME
	// 如果登入的地图是这些编号的话,回到入口
	if(CHAR_getInt(charaindex,CHAR_FLOOR) >= 30017 && CHAR_getInt(charaindex, CHAR_FLOOR) <= 30021){
		CHAR_setInt(charaindex,CHAR_FLOOR,30008);
		CHAR_setInt(charaindex,CHAR_X,39);
		CHAR_setInt(charaindex,CHAR_Y,38);
	}
#endif
//LUA整数储存 上线清零
   CHAR_setInt(charaindex,CHAR_LUASAVE_WORKINT_1,0);//上线清零
   CHAR_setInt(charaindex,CHAR_LUASAVE_WORKINT_2,0);//上线清零
   CHAR_setInt(charaindex,CHAR_LUASAVE_WORKINT_3,0);//上线清零
#ifdef _FM_METAMO
	if( CHAR_getInt( charaindex , CHAR_BASEIMAGENUMBER)>=100700 && CHAR_getInt( charaindex , CHAR_BASEIMAGENUMBER)<100819
		  && (CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) == FMMEMBER_NONE	|| CHAR_getInt( charaindex, CHAR_FMLEADERFLAG ) == FMMEMBER_APPLY)){
		switch( CHAR_getWorkInt( charaindex, CHAR_WORKFMFLOOR) ){
			case 1041:
			case 2031:
			case 3031:
			case 4031:
			case 5031:
			case 6031:
			case 7031:
			case 8031:
			case 9031:
			case 10031:
				break;
			default:
				CHAR_ReMetamo(charaindex);
		}
	}
#endif
#ifdef _AUTO_PK
	int exfloor=-1,ex_X=-1,ex_Y=-1;
	if( CHAR_getInt(charaindex, CHAR_FLOOR) == 20000 ){   
		CHAR_getElderPosition(CHAR_getInt(charaindex, CHAR_LASTTALKELDER), &exfloor, &ex_X, &ex_Y);
		CHAR_setInt(charaindex, CHAR_FLOOR, exfloor);
		CHAR_setInt(charaindex, CHAR_X, ex_X);
		CHAR_setInt(charaindex, CHAR_Y, ex_Y);	
	}
	CHAR_setInt(charaindex, CHAR_AUTOPK, 0);	
#endif
	if( CHAR_getInt(charaindex, CHAR_FLOOR) == 10032 ){   
		CHAR_setInt(charaindex, CHAR_FLOOR, 10030);
		CHAR_setInt(charaindex, CHAR_X, 52);
		CHAR_setInt(charaindex, CHAR_Y, 36);	
	}
	
	objindex = initObjectFromObjectMember(OBJTYPE_CHARA,charaindex,
										  CHAR_getInt(charaindex,CHAR_X),
										  CHAR_getInt(charaindex,CHAR_Y),
										  CHAR_getInt(charaindex,CHAR_FLOOR));
	if( objindex == -1 ){
		fprint ("始化对象错误！\n");
		goto DELETECHARDATA;
	}

	CHAR_setWorkInt( charaindex,CHAR_WORKOBJINDEX,objindex );
	CONNECT_setState( clifd, LOGIN );
	//print("\n人物登录结束1111111111111111\n");
	CONNECT_setCharaindex( clifd, charaindex );
	CHAR_LoginBesideSetWorkInt( charaindex, clifd);

	CHAR_complianceParameter( charaindex );
	//检查人物身上
	{
		int i,j;
//		char *petstring;
		int ID1,ID2;
		char* delbuf;
		char delpetbuf[10];
		delbuf = getDelPet();
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			int petindex = CHAR_getCharPet(charaindex,i);
			if( CHAR_CHECKINDEX( petindex) ) {
/*
		if(CHAR_getInt( petindex, CHAR_FUSIONBEIT )==1
			 && CHAR_getInt( petindex, CHAR_TRANSMIGRATION )<2)
			CHAR_setInt( petindex, CHAR_FUSIONBEIT, 0 );

		if(CHAR_getInt( petindex, CHAR_FUSIONBEIT )==1
			 && CHAR_getInt( petindex, CHAR_TRANSMIGRATION )>2){
			int vital=CHAR_getInt( petindex, CHAR_VITAL);
			int str=CHAR_getInt( petindex, CHAR_STR);
			int tough=CHAR_getInt( petindex, CHAR_TOUGH);
			int dex=CHAR_getInt( petindex, CHAR_DEX);
			int lv=CHAR_getInt( petindex, CHAR_LV);
			int LevelUpPoint = CHAR_getInt( petindex, CHAR_ALLOCPOINT );
			int work[4];
			float fRand, cvitl, cstr, ctough, cdex;
			float rnd=(rand()%5+95)/100.0;
			float Param[4] = { 0.0, 0.0, 0.0, 0.0 };
			work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
			work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
			work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
			work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
			struct _RankRandTbl{
				int min;
				int max;
			}RankRandTbl[] = {
				{ 450, 500 },
				{ 470, 520 },
				{ 490, 540 },
				{ 510, 560 },
				{ 530, 580 },
				{ 550, 600 },
			};
			int petrank = CHAR_getInt( petindex, CHAR_PETRANK );
			if( petrank < 0 || petrank > 5 ) petrank = 0;
				
			printf("\n.........................\n");
			printf("名：%s\n",CHAR_getChar( petindex, CHAR_NAME));
			printf("血：%d\n",vital/15);
			printf("攻：%d\n",str/80);
			printf("防：%d\n",tough/80);
			printf("敏：%d\n",dex/100);
			printf("成：%f\n",((str/80-20.0)/lv+(tough/80-15.0)/lv+(dex/100-15.0)/lv));
			printf(".........................\n");

			while(((str/80-20.0)/lv+(tough/80-15.0)/lv+(dex/100-15.0)/lv)>8.5){
				vital*=rnd;
				str*=rnd;
				tough*=rnd;
				dex*=rnd;
				rnd=(rand()%5+95)/100.0;
			}

			for(i=0;i<160;i++){
				fRand = (float)RAND( RankRandTbl[petrank].min, RankRandTbl[petrank].max ) * 0.01;
				cvitl += (float)work[3] * fRand + Param[0] * fRand;
				cstr += (float)work[0] * fRand + Param[1] * fRand;
				ctough += (float)work[1] * fRand + Param[2] * fRand;
				cdex += (float)work[2] * fRand + Param[3] * fRand;
			}
			rnd=(cstr/80/160.0+ctough/80/160.0+cdex/100/160.0);

			printf("血：%d\n",work[3]);
			printf("攻：%d\n",work[0]);
			printf("防：%d\n",work[1]);
			printf("敏：%d\n",work[2]);
			printf("成：%f\n",rnd);
			
			if(rnd>8.5){
				work[3]*=(8.5/rnd);
				work[0]*=(8.5/rnd);
				work[1]*=(8.5/rnd);
				work[2]*=(8.5/rnd);
			}
			LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
			CHAR_setInt( petindex, CHAR_ALLOCPOINT, LevelUpPoint);
			
			CHAR_setInt( petindex, CHAR_VITAL, vital);
			CHAR_setInt( petindex, CHAR_STR, str);
			CHAR_setInt( petindex, CHAR_TOUGH, tough);
			CHAR_setInt( petindex, CHAR_DEX, dex);
			
			CHAR_setInt( petindex, CHAR_FUSIONBEIT, 2 );
			CHAR_setInt( petindex, CHAR_TRANSMIGRATION,3 );
			printf("\n.........................\n");
			printf("名：%s\n",CHAR_getChar( petindex, CHAR_NAME));
			printf("血：%d\n",vital/15);
			printf("攻：%d\n",str/80);
			printf("防：%d\n",tough/80);
			printf("敏：%d\n",dex/100);
			printf("成：%f\n",((str/80-20.0)/lv+(tough/80-15.0)/lv+(dex/100-15.0)/lv));
			printf(".........................\n");
			printf("血：%d\n",work[3]);
			printf("攻：%d\n",work[0]);
			printf("防：%d\n",work[1]);
			printf("敏：%d\n",work[2]);
		}
*/	

#ifdef _UNIQUE_P_I			    
			    // CoolFish: 2001/10/11 Set Pet Unicode 
			    CHAR_setPetUniCode(petindex);
#endif
#ifdef _NEWOPEN_MAXEXP
				if( CHAR_getInt( petindex, CHAR_OLDEXP) > 0 || CHAR_getInt( charaindex, CHAR_OLDEXP) < 0 ){
					CHAR_ChangeExp( petindex);
				}
#endif
				ID1=CHAR_getInt( petindex, CHAR_PETID);
				/*
				if(CHAR_getInt(petindex,CHAR_BEATITUDE)>0 && strstr(CHAR_getChar(petindex,CHAR_NAME),"+")==NULL){
					char btmpname[128];
					sprintf(btmpname,"%s+",CHAR_getChar(petindex,CHAR_NAME));
					CHAR_setChar(petindex,CHAR_NAME,btmpname);
				}
				*/
//				petstring = CHAR_makePetStringFromPetIndex( petindex);
#ifdef _JK_CF_DELPETITEM           //删除CF指定宠物
				for( j=1;j<=30;j++ ) {
					if(getStringFromIndexWithDelim(delbuf,",", j, delpetbuf, sizeof(delpetbuf)) == FALSE)
						break;
					ID2 = atoi(delpetbuf);
					if(ID2 == -1)break;
					if( ID1 == ID2 ){
						CHAR_setCharPet( charaindex, i, -1);//清除不能当宠物的角色
						LogPet( CHAR_getChar( charaindex, CHAR_NAME ),
									CHAR_getChar( charaindex, CHAR_CDKEY ),
									CHAR_getChar( petindex, CHAR_NAME),
									CHAR_getInt( petindex, CHAR_LV),
									"login(清除CF设置的宠物)",
									CHAR_getInt( charaindex,CHAR_FLOOR),
									CHAR_getInt( charaindex,CHAR_X ),
									CHAR_getInt( charaindex,CHAR_Y ),
									CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
									);
						CHAR_talkToCli(charaindex,-1,"系统清除你身上的宠物",CHAR_COLORWHITE);
					}
				}
#endif
#ifdef _REEDIT_PETBBIS
				if( ID1 == 1133 ){
					CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, 101578);
					CHAR_setInt( petindex, CHAR_BASEBASEIMAGENUMBER, 101578);
					//print("PET:%s fix BBI\n", CHAR_getUseName(petindex));
				}
#endif
#ifdef	_YUANGUSA
				if( ID1 == 10247 ){
					CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, 130088);
					CHAR_setInt( petindex, CHAR_BASEBASEIMAGENUMBER, 130088);
					CHAR_setChar( petindex, CHAR_NAME, "至尊-红龙");
					//print("PET:%s fix BBI\n", CHAR_getUseName(petindex));
				}
#endif				
				// WON ADD 修正白虎七技
				if( ID1 == 777 )CHAR_setInt( petindex, CHAR_SLOT, 7);
				CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, charaindex);
				// 修正宠技最大值变-1  Robin 20040803
				if( CHAR_getInt( petindex, CHAR_SLOT) < 0 ) {
					int j;
					for( j=0; j<CHAR_MAXPETSKILLHAVE; j++) {
						if( CHAR_getPetSkill( petindex, j) >= 0)
							CHAR_setInt( petindex, CHAR_SLOT, j+1);
					}
					//print(" 宠技MAX变-1改6 ");
				}

				CHAR_complianceParameter( petindex );
#ifdef _FIX_GP_PET_SKILL	 // WON ADD 修正gp宠会落马术
				if( ID1 == 1133){
					int j;
					for( j=0; j<CHAR_MAXPETSKILLHAVE; j++){
						if( CHAR_getPetSkill( petindex, j) == 210 ){ // 落马术
							CHAR_setPetSkill( petindex, j, -1);
						}
					}
				}
#endif
			}
		}
	}
	{
		int i,j;
//		char *petstring;//ttom
//		char *c_ptr_index;
//		char c_temp[4096];
//		char c_temp1[4096];
		int ID1,ID2;
		char* delbuf;
		char delpetbuf[10];
		delbuf = getDelPet();
//		int tran_no;
		BOOL b_find=FALSE;
		//ttom
		for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
			int petindex = CHAR_getCharPoolPet(charaindex,i);
			if( CHAR_CHECKINDEX( petindex) ) {
#ifdef _UNIQUE_P_I			    
			    // CoolFish: 2001/10/11 Set Pet Unicode 
			    CHAR_setPetUniCode(petindex);
#endif
#ifdef _NEWOPEN_MAXEXP
				if( CHAR_getInt( petindex, CHAR_OLDEXP) > 0 ||
					CHAR_getInt( charaindex, CHAR_OLDEXP) < 0 ){
					CHAR_ChangeExp( petindex);
				}
#endif
				b_find=FALSE;
				ID1=CHAR_getInt( petindex, CHAR_PETID);
/*
				if(CHAR_getInt(petindex,CHAR_BEATITUDE)>0 && strstr(CHAR_getChar(petindex,CHAR_NAME),"+")==NULL){
					char btmpname[128];
					sprintf(btmpname,"%s+",CHAR_getChar(petindex,CHAR_NAME));
					CHAR_setChar(petindex,CHAR_NAME,btmpname);
				}
*/
#ifdef _JK_CF_DELPETITEM           //删除CF指定宠物
				for( j=1;j<=30;j++ ) {
					if(getStringFromIndexWithDelim(delbuf,",", j, delpetbuf, sizeof(delpetbuf)) == FALSE)
						break;
					ID2 = atoi(delpetbuf);
					if(ID2 == -1)break;
					if( ID1 == ID2 ){
						CHAR_setCharPoolPet( charaindex, i, -1);//清除不能当宠物的角色
						LogPet( CHAR_getChar( charaindex, CHAR_NAME ),
									CHAR_getChar( charaindex, CHAR_CDKEY ),
									CHAR_getChar( petindex, CHAR_NAME),
									CHAR_getInt( petindex, CHAR_LV),
									"login(清除CF设置的宠物)",
									CHAR_getInt( charaindex,CHAR_FLOOR),
									CHAR_getInt( charaindex,CHAR_X ),
									CHAR_getInt( charaindex,CHAR_Y ),
									CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
									);
						CHAR_talkToCli(charaindex,-1,"系统清除你宠物店里的宠物",CHAR_COLORWHITE);
					}
				}
#endif
#ifdef _REEDIT_PETBBIS
				if( ID1 == 1133 ){
					CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, 101578);
					CHAR_setInt( petindex, CHAR_BASEBASEIMAGENUMBER, 101578);
				}
#endif
#ifdef	_YUANGUSA
				if( ID1 == 10247 ){
					CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, 130088);
					CHAR_setInt( petindex, CHAR_BASEBASEIMAGENUMBER, 130088);
					CHAR_setChar( petindex, CHAR_NAME, "至尊-红龙");
					//print("PET:%s fix BBI\n", CHAR_getUseName(petindex));
				}
#endif				
/* 
			    tran_no=CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
			    petstring = CHAR_makePetStringFromPetIndex( petindex);
				c_ptr_index=strstr(petstring,"name:");
			    getStringFromIndexWithDelim(c_ptr_index,":",2,c_temp,sizeof(c_temp));
			    c_ptr_index=c_temp;
			    getStringFromIndexWithDelim(c_ptr_index,"|",1,c_temp1,sizeof(c_temp1));
                                                                                                                                                                                                                                                                                                       
			    if(b_find){//ttom
			          CHAR_setCharPoolPet( charaindex, i, -1);
			          print("\nKill pool pet  ID=%d name=%s",ID1,c_temp1);
			          sprintf(c_temp2,"Kill pool pet  ID=%d name=%s",ID1,c_temp1);
			          LogKill(CHAR_getChar( charaindex, CHAR_NAME ),
			                  CHAR_getChar( charaindex, CHAR_CDKEY ),
			                  c_temp2);
			    }else{//ttom
*/			    
						CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, charaindex);
						CHAR_complianceParameter( petindex );				    
//				}//ttom
			}
		}
	}

//	CHAR_ResetPoolPetEgg( charaindex);
//	CHAR_CheckProfessionEquit( charaindex);
	CHAR_CheckProfessionSkill( charaindex);
	/*{
		int i=0;
		int skidx[]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
					1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
					3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,3,3,3,3,3,3,3};
		//20040702 Change fix
		//检查是否有职业不该有的技能并删除
		for( i = CHAR_SKILLMAXHAVE-1; i >= 0; i -- ){
			CHAR_HaveSkill *pSkil;
			Skill*          skill;
			
			pSkil = CHAR_getCharHaveSkill( charaindex, i );
			skill = &pSkil->skill;
			if( pSkil == NULL )continue;
			if( pSkil->use == 0 )continue;	
			if( skidx[skill->data[SKILL_IDENTITY]] != CHAR_getInt( charaindex, PROFESSION_CLASS )
				&& skidx[skill->data[SKILL_IDENTITY]] != 4 ){//共通技不删
				SKILL_makeSkillData( &pSkil->skill, 0, 0 );
				pSkil->use = 0;
			}
		}
	}*/
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep);
	timep = mktime(p);
#ifdef _VIP_ANNOUNCE   //VIP上线系统自动广告
		
		if(CHAR_getInt( charaindex,CHAR_VIPRIDE )>0)
			{
//#ifdef	_YUANGUSA
//				if(CHAR_getInt(charaindex,CHAR_VIPRIDE)==1 && CHAR_getInt(charaindex,CHAR_VIPTIME)==0){
//					CHAR_setInt(charaindex,CHAR_VIPTIME,timep+60*60*24*30);
//				}
//#endif
				int viptype,viptime;
				viptime = CHAR_getInt( charaindex,CHAR_VIPTIME );
				viptype = CHAR_getInt( charaindex,CHAR_VIPRIDE );
				int vipmsgtype = getVipMsgType();
				if(viptime > 0)
					{
						viptime = viptime - timep;
					}
				if(viptime < 0)
					{
						CHAR_setInt( charaindex,CHAR_VIPRIDE,0 );
						CHAR_setInt( charaindex,CHAR_VIPTIME,0 );
						viptype = CHAR_getInt( charaindex,CHAR_VIPRIDE );
					}
				if(viptype>0 && vipmsgtype>0 && viptype>=vipmsgtype)
				{
					int i;
					int playernum = CHAR_getPlayerMaxNum();
					char VipMsg[256];
					char *MyName = CHAR_getChar( charaindex,CHAR_NAME );
					if(vipmsgtype == 1)
					{
#ifdef _TEST_SERVER
						if(viptype == 1)
							sprintf( VipMsg, "『%s』尊贵的会员【%s】上线了，大家热烈欢迎！", "一起石器服务端", MyName );
						else if(viptype == 2)
							sprintf( VipMsg, "『%s』尊贵的黄金会员【%s】上线了，大家热烈欢迎！", "一起石器服务端", MyName );
						else if(viptype == 3)
							sprintf( VipMsg, "『%s』尊贵的钻石会员【%s】上线了，大家热烈欢迎！", "一起石器服务端", MyName );
#else
						if(viptype == 1)
        			sprintf( VipMsg, "『%s』尊贵的会员【%s】上线了，大家热烈欢迎！", getGameservername(), MyName );
        		else if(viptype == 2)
        			sprintf( VipMsg, "『%s』尊贵的黄金会员【%s】上线了，大家热烈欢迎！", getGameservername(), MyName );
        		else if(viptype == 3)
        			sprintf( VipMsg, "『%s』尊贵的钻石会员【%s】上线了，大家热烈欢迎！", getGameservername(), MyName );
#endif
					}
					else if(vipmsgtype == 2)
					{
#ifdef _TEST_SERVER
						if(viptype == 2)
							sprintf( VipMsg, "『%s』尊贵的黄金会员【%s】上线了，大家热烈欢迎！", "一起石器服务端", MyName );
						else if(viptype == 3)
							sprintf( VipMsg, "『%s』尊贵的钻石会员【%s】上线了，大家热烈欢迎！", "一起石器服务端", MyName );
#else
        		if(viptype == 2)
        			sprintf( VipMsg, "『%s』尊贵的黄金会员【%s】上线了，大家热烈欢迎！", getGameservername(), MyName );
        		else if(viptype == 3)
        			sprintf( VipMsg, "『%s』尊贵的钻石会员【%s】上线了，大家热烈欢迎！", getGameservername(), MyName );
#endif
					}
					else if(vipmsgtype == 3)
					{
#ifdef _TEST_SERVER
						if(viptype == 3)
							sprintf( VipMsg, "『%s』尊贵的钻石会员【%s】上线了，大家热烈欢迎！", "一起石器服务端", MyName );
#else
        		if(viptype == 3)
        			sprintf( VipMsg, "『%s』尊贵的钻石会员【%s】上线了，大家热烈欢迎！", getGameservername(), MyName );
#endif
					}
					for( i = 0 ; i < playernum ; i++)
					{
						CHAR_talkToCli( i, -1, VipMsg, CHAR_COLORPURPLE);
  				}
  			}
  		}
#endif
#ifdef _OFFLINE_SYSTEM
	if(strlen(CHAR_getChar(charaindex,CHAR_OFFBUF))<1){
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep); /*取得当地时间*/
		int mon=1+p->tm_mon;
		int date=p->tm_mday;
		char offbuf[64];
		sprintf(offbuf,"%d-%d-%d",mon,date,0);
		CHAR_setChar(charaindex,CHAR_OFFBUF,offbuf);
		CHAR_setInt(charaindex,CHAR_OFFTIME,172800);
	}
#endif
//22222222222222222222222222222222

	CHAR_loginCheckUserItem( charaindex);
	CHAR_complianceParameter( charaindex );

	// Robin add 重设宠蛋  养时间
	CHAR_ResetPetEggFusionTime( charaindex);

#ifdef _PROFESSION_FIX_LEVEL		// WON ADD 修正职业经验值
	{
		int i=0;
		for( i = 0 ; i < CHAR_SKILLMAXHAVE ; i ++ ){
			CHAR_HaveSkill* hskill;

			hskill = CHAR_getCharHaveSkill( charaindex, i );

			if( hskill != NULL && hskill->use == TRUE ){
				int skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);
				if( skill_level < 10 ){
					SKILL_setInt( &hskill->skill, SKILL_LEVEL, 10* 100 );
				}
			}
		}
	}
#endif

#ifdef _CHANNEL_MODIFY
	if(CHAR_getInt(charaindex,PROFESSION_CLASS) > 0){
		int i,pclass = CHAR_getInt(charaindex,PROFESSION_CLASS) - 1,Empty = -1;
		// 若旧的职业频道记录有记录在就留着,若没有加入频道
		for(i=0;i<getFdnum();i++){
			if(*(piOccChannelMember + (pclass * getFdnum()) + i) == charaindex) break;
			else if(*(piOccChannelMember + (pclass * getFdnum()) + i) == -1 && Empty == -1) Empty = i;
		}
		// 找不到旧记录
		if(i == getFdnum()){
			if(Empty != -1) *(piOccChannelMember + (pclass * getFdnum()) + Empty) = charaindex;
		}
	}
#endif

#ifdef _STREET_VENDOR
	CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
	CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
	{
		int i;
		for(i=0;i<MAX_SELL_ITEM;i++) CHAR_clearStreetVendor(charaindex,i);
	}
#endif
#ifdef _NEW_MANOR_LAW
	if(CHAR_getInt(charaindex,CHAR_MOMENTUM) < 0) CHAR_setInt(charaindex,CHAR_MOMENTUM,0);
	if(CHAR_getInt(charaindex,CHAR_MOMENTUM) > MAX_PERSONAL_MOMENTUM) CHAR_setInt(charaindex,CHAR_MOMENTUM,MAX_PERSONAL_MOMENTUM);
#endif

#ifdef _PETSKILL_BECOMEPIG
	if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){
        CHAR_setInt( charaindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( charaindex, CHAR_BECOMEPIG_BBI ) );
	}
	//change fix 处理负很大的乌力时间
	else{
		CHAR_setInt( charaindex, CHAR_BECOMEPIG, -1 );
	}
#endif


	lssproto_CharLogin_send( clifd, SUCCESSFUL,"" );
	per = ENCOUNT_getEncountPercentMin( charaindex,
									CHAR_getInt( charaindex, CHAR_FLOOR),
									CHAR_getInt( charaindex, CHAR_X),
									CHAR_getInt( charaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, per);
	}
	per = ENCOUNT_getEncountPercentMax( charaindex,
									CHAR_getInt( charaindex, CHAR_FLOOR),
									CHAR_getInt( charaindex, CHAR_X),
									CHAR_getInt( charaindex, CHAR_Y));
	if( per != -1) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, per);
	}

	CHAR_setLuck( charaindex);
	{
		int i;
		char*   statuscat[]={"C","P","I","S","D","E","F"};
		for( i=0 ; i<arraysizeof(statuscat) ; i++ )
			CHAR_sendStatusString( charaindex, statuscat[i] );
	}
	{
		int		i;
		char	category[3];
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			snprintf( category, sizeof( category), "K%d",i);
			CHAR_sendStatusString( charaindex, category );
			snprintf( category, sizeof( category), "W%d",i);
			CHAR_sendStatusString( charaindex, category );
		}
	}
	{
		int petarray = CHAR_getInt( charaindex, CHAR_DEFAULTPET);
		if( CHAR_CHECKPETINDEX( petarray)) {
			if( !CHAR_CHECKINDEX( CHAR_getCharPet( charaindex, petarray))) {
				petarray = -1;
			}
		}else {
			petarray = -1;
		}
		lssproto_KS_send( clifd, petarray, TRUE);
	}
	{
		int flg = 0;
		CHAR_setFlg( charaindex, CHAR_ISSHOWBATTLEMSG, 0);
		CHAR_setFlg( charaindex, CHAR_ISDUEL, 0);
		CHAR_setFlg( charaindex, CHAR_ISTRADECARD, 0);
		if( CHAR_getFlg( charaindex, CHAR_ISPARTY)) flg |= CHAR_FS_PARTY;
		if( CHAR_getFlg( charaindex, CHAR_ISDUEL)) flg |= CHAR_FS_DUEL;
		if( CHAR_getFlg( charaindex, CHAR_ISPARTYCHAT)) flg |= CHAR_FS_PARTYCHAT;
		if( CHAR_getFlg( charaindex, CHAR_ISTRADECARD)) flg |= CHAR_FS_TRADECARD;
#ifdef _CHANNEL_MODIFY
		CHAR_setFlg(charaindex,CHAR_ISTELL,0);
		CHAR_setFlg(charaindex,CHAR_ISFM,0);
		CHAR_setFlg(charaindex,CHAR_ISOCC,0);
		CHAR_setFlg(charaindex,CHAR_ISCHAT,0);
		CHAR_setFlg(charaindex,CHAR_ISSAVE,0);
		if(getSaType()==95){
			CHAR_setFlg(charaindex,CHAR_ISWORLD,TRUE);
			flg |= CHAR_FS_WORLD;
		}
#endif
        // CoolFish: 2001/4/18
		CHAR_setFlg( charaindex, CHAR_ISTRADE, 0);
		lssproto_FS_send( clifd, flg);

	}

	MAP_sendArroundCharNeedFD( clifd, charaindex );
	CHAR_setInt( charaindex, CHAR_LOGINCOUNT, CHAR_getInt( charaindex, CHAR_LOGINCOUNT) + 1  );
	CHAR_sendCToArroundCharacter( objindex );
	CHAR_sendArroundCharaData( charaindex );
	CHAR_sendWatchEvent(objindex,CHAR_ACTSTAND,NULL,0,TRUE);
	ADDRESSBOOK_notifyLoginLogout(charaindex,1);
	CHAR_setWorkInt(charaindex,CHAR_WORKLASTATTACKCHARAINDEX,-1);
	if(CHAR_getInt(charaindex,CHAR_HP) <= 0 ){
		typedef int (*DYINGFUNC)(int);
		DYINGFUNC   dyingfunc=NULL;
		dyingfunc = (DYINGFUNC)CHAR_getFunctionPointer(charaindex, CHAR_DYINGFUNC );
		CHAR_setInt(charaindex,CHAR_HP,1);    // HP 设为 1
		//logHack( CHAR_getWorkInt(charaindex,CHAR_WORKFD), HACK_HP);
	}
	if( CHAR_getInt( charaindex,CHAR_SKILLUPPOINT ) >=1 )
		CHAR_Skillupsend(charaindex);
	CHAR_checkEffect( charaindex);
	// CoolFish: Family 2001/5/30
	if ((CHAR_getChar(charaindex, CHAR_FMNAME)!=NULL) &&
		(CHAR_getInt(charaindex, CHAR_FMINDEX) >= 0) &&
		(strcmp(CHAR_getChar(charaindex, CHAR_FMNAME), "") != 0))
	{
#ifdef _FMVER21
		int eventflag = 0;
		// 是否符合族长得资格
		if (CheckLeaderQ(charaindex) >= 0)
			eventflag = 1;
		if (CHAR_getInt(charaindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
			SetFMPetVarInit(charaindex);
	   	saacproto_ACFMCharLogin_send(
				 acfd,
	   	   CHAR_getChar(charaindex, CHAR_FMNAME),
	   	   CHAR_getInt(charaindex, CHAR_FMINDEX),
	   	   CHAR_getChar(charaindex, CHAR_NAME),
	   	   CHAR_getChar(charaindex, CHAR_CDKEY),
	   	   CHAR_getInt(charaindex, CHAR_LV), eventflag,
	   	   CONNECT_getFdid(clifd)
#ifdef _FM_MODIFY
				 ,getServernumber()
#endif
				 );
	}
#ifdef _FMVER21
	else
		SetFMVarInit(charaindex);
#endif	
	
#endif

	AnnounceToPlayerWN( clifd );

#ifdef _ANGEL_SUMMON
	{
		int mindex;
		mindex = checkIfOnlyAngel( charaindex);
		if( mindex != -1 ) {
			if( missiontable[mindex].flag == MISSION_WAIT_ANSWER ) {
				lssproto_WN_send( clifd, WINDOW_MESSAGETYPE_ANGELMESSAGE, 
				WINDOW_BUTTONTYPE_YESNO, 
				CHAR_WINDOWTYPE_ANGEL_ASK,
				-1,
				"目前魔族肆虐，精灵们需要你的帮忙，前往寻找勇者来消灭这些魔族，你是否愿意帮忙？" );
			}
		}
	}
#endif

	print("\n登陆人物名称:%s ", CHAR_getChar(charaindex, CHAR_NAME ) );
	
	
		unsigned long ip;
		char ipstr[512];
		ip=CONNECT_get_userip(clifd);
		sprintf(ipstr,"%d.%d.%d.%d",
        	((unsigned char *)&ip)[0],
        	((unsigned char *)&ip)[1],
        	((unsigned char *)&ip)[2],
        	((unsigned char *)&ip)[3]);
        	
#ifdef _RECORD_IP
	CHAR_setWorkInt(charaindex, CHAR_WORK_RECORD_IP, CONNECT_get_userip(clifd));
#endif

//    CHAR_setChar(charaindex,CHAR_CHARPLAYIP,ipstr);
    char pcid[64];
    memset( pcid, 0 , sizeof( pcid ) );
    CONNECT_getPcid( clifd, pcid, sizeof( pcid ));
		LogLogin(
			CHAR_getChar( charaindex, CHAR_CDKEY),
			CHAR_getChar( charaindex, CHAR_NAME),
			saveindex, ipstr,pcid
			);
	
#ifdef _VIGOR_SYS
	CHAR_setWorkInt(charaindex,CHAR_WORKVIGORTIME,timep);
#endif

#ifdef _TEACHER_SYSTEM
	// 学生上线通知导师
	if(strlen(CHAR_getChar(charaindex,CHAR_TEACHER_ID)) > 0 && strlen(CHAR_getChar(charaindex,CHAR_TEACHER_NAME)) > 0){
		int iPlayernum = CHAR_getPlayerMaxNum(),i;
		char szMsg[128];

		for(i=0;i<iPlayernum;i++){
			if(CHAR_getCharUse(i) == FALSE) continue;
			if(strcmp(CHAR_getChar(charaindex,CHAR_TEACHER_ID),CHAR_getChar(i,CHAR_CDKEY)) == 0 &&
				 strcmp(CHAR_getChar(charaindex,CHAR_TEACHER_NAME),CHAR_getChar(i,CHAR_NAME)) == 0){
				sprintf(szMsg,"学生 %s 已上线",CHAR_getChar(charaindex,CHAR_NAME));
				CHAR_talkToCli(i,-1,szMsg,CHAR_COLORYELLOW);
				break;
			}
		}
	}
#endif

#ifdef _ITEM_SETLOVER
	// 夫妻上线通知对方
		if(strlen( CHAR_getChar( charaindex, CHAR_LOVE)) > 0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERID))>0 &&
        strlen(CHAR_getChar(charaindex,CHAR_LOVERNAME))>0){
		int iPlayernum = CHAR_getPlayerMaxNum(),i;
		char szMsg[128];
		for(i=0;i<iPlayernum;i++){
			if(CHAR_getCharUse(i) == FALSE) continue;
			if(!strcmp( CHAR_getChar( i, CHAR_LOVE), "YES") &&
        	!strcmp(CHAR_getChar(charaindex,CHAR_LOVERID),CHAR_getChar(i,CHAR_CDKEY)) &&
        	!strcmp(CHAR_getChar(charaindex,CHAR_LOVERNAME),CHAR_getChar(i,CHAR_NAME))){
				sprintf(szMsg,"你的爱人 %s 上线了",CHAR_getChar(charaindex,CHAR_NAME));
				CHAR_talkToCli(i,-1,szMsg,CHAR_COLORYELLOW);
				break;
			}
		}
	}
#endif


#ifdef _SAFE_PASSWORD
if(getSafeMode()==1 || CHAR_getInt(charaindex,CHAR_LOCK)==1)
	{
		CHAR_setWorkInt(charaindex,CHAR_WORKSAFEMODE,1);
	}
#endif	
#ifdef _JOB_AUTOPK
	if(CHAR_getInt(charaindex,CHAR_JOBPKMODE)>0){
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep);
		timep = mktime(p);
		if(CHAR_getInt(charaindex,CHAR_JOBPKTIME)-timep<=0){
			CHAR_setInt(charaindex,CHAR_JOBPKTIME,0);
			CHAR_setInt(charaindex,CHAR_JOBPKMODE,0);
		}else{
			char jobbuf[256];
			if(CHAR_getInt(charaindex,CHAR_JOBPKMODE)==1){
				sprintf(jobbuf,"热烈欢迎远古白狼勇士『首席弟子』【%s】上线！",CHAR_getChar(charaindex,CHAR_NAME));
			}else if(CHAR_getInt(charaindex,CHAR_JOBPKMODE)==2){
				sprintf(jobbuf,"热烈欢迎远古闇灵法师『首席弟子』【%s】上线！",CHAR_getChar(charaindex,CHAR_NAME));
			}else{
				sprintf(jobbuf,"热烈欢迎远古追猎者『首席弟子』【%s】上线！",CHAR_getChar(charaindex,CHAR_NAME));
			}
			int i;
			int playernum = CHAR_getPlayerMaxNum();
			for( i = 0 ; i < playernum ; i++)
				{
					CHAR_talkToCli( i, -1, jobbuf, CHAR_COLORRED);
  			}
		}
	}
#endif
	{
		sasql_online(cdkey,CHAR_getChar(charaindex,CHAR_NAME),ipstr,CHAR_getInt(charaindex,CHAR_FAME),CHAR_getInt(charaindex,CHAR_VIGOR),1);
	}
	{
		if(CHAR_getInt(charaindex,CHAR_WALKINTERVAL) != getWalksendinterval())
			CHAR_setInt(charaindex,CHAR_WALKINTERVAL,getWalksendinterval());
	}
#ifdef _PET_ITEM
	if( CHAR_CHECKINDEX( CHAR_getCharPet( charaindex, 0) ) ){
		CHAR_sendPetItemData(charaindex,0);
	}
	if( CHAR_CHECKINDEX( CHAR_getCharPet( charaindex, 1) ) ){
		CHAR_sendPetItemData(charaindex,1);
	}
	if( CHAR_CHECKINDEX( CHAR_getCharPet( charaindex, 2) ) ){
		CHAR_sendPetItemData(charaindex,2);
	}
	if( CHAR_CHECKINDEX( CHAR_getCharPet( charaindex, 3) ) ){
		CHAR_sendPetItemData(charaindex,3);
	}
	if( CHAR_CHECKINDEX( CHAR_getCharPet( charaindex, 4) ) ){
		CHAR_sendPetItemData(charaindex,4);
	}
#endif
	if(CHAR_getInt(charaindex,CHAR_LV)>10 || CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)>0){
		sasql_online_ip(cdkey,ipstr);
	}
	
	CHAR_setWorkInt(charaindex,CHAR_WORKALLTALKTYPE,0);

	return;

DELETECHARDATA:
	CHAR_endCharOneArray( charaindex );
MAKECHARDATAERROR:
	{
		int i;
		char cdkey[CDKEYLEN];
		for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
			if( CHAR_CHECKINDEX(ch.unionTable.indexOfPet[i] ) ) {
				CHAR_endCharOneArray( ch.unionTable.indexOfPet[i] );
			}
		}
        CONNECT_getCdkey( clifd, cdkey, sizeof(cdkey ));
        saacproto_ACLock_send( acfd, cdkey, UNLOCK, CONNECT_getFdid(clifd) );
  }
  CONNECT_setState(clifd, WHILECANNOTLOGIN );
  CONNECT_setCharaindex( clifd, -1 );
	lssproto_CharLogin_send( clifd, FAILED, "在线人数已满，请尝试登陆其他线路，或稍后重试。");
}

BOOL CHAR_charSaveFromConnectAndChar( int fd, Char* ch, BOOL unlock )
{
	char*  chardata;
  char   cdkey[CDKEYLEN];
	
	chardata = CHAR_makeStringFromCharData( ch );
	if( chardata == "\0" )return FALSE;
//  CONNECT_getCdkey( fd, cdkey, sizeof(cdkey));
//
//  if(strcmp(cdkey,ch->string[CHAR_CDKEY].string)!=0){
//  	printf("连接:%s与账号:%s不符合，不给予存档",cdkey,ch->string[CHAR_CDKEY].string);
//  	return FALSE;
//  }
#ifdef _DBSAVE_CHAR
	else{
		if(ch->data[CHAR_SAVEINDEXNUMBER]!=-1)
			CharaData( fd, ch );
	}
#endif

#ifdef _NEWSAVE

//	print("saveindex_save:%d\n",ch->data[CHAR_SAVEINDEXNUMBER]);
	saacproto_ACCharSave_send( acfd, ch->string[CHAR_CDKEY].string,
							   ch->string[CHAR_NAME].string,
							   CHAR_makeOptionString( ch ),  chardata,
							   unlock, CONNECT_getFdid(fd), ch->data[CHAR_SAVEINDEXNUMBER] );
#else

	saacproto_ACCharSave_send( acfd, cdkey,
							   ch->string[CHAR_NAME].string,
							   CHAR_makeOptionString( ch ),  chardata,
							   unlock, CONNECT_getFdid(fd) );
#endif
	if(unlock) sasql_online(ch->string[CHAR_CDKEY].string,NULL,NULL,NULL,NULL,0);
	return TRUE;
}

BOOL CHAR_charSaveFromConnect( int charaindex , BOOL unlock)
{
	Char*   ch;
	int fd = getfdFromCharaIndex(charaindex);
	if( CHAR_CHECKINDEX( charaindex ) == FALSE)return FALSE;
	ch = CHAR_getCharPointer( charaindex );
	if( !ch )return FALSE;
#ifdef _CHAR_POOLITEM
	if( CHAR_SaveDepotItem( charaindex) == TRUE ){
		print("saveDepotItem:%d \n", charaindex);
	}
#endif
#ifdef _CHAR_POOLPET
	if( CHAR_SaveDepotPet( charaindex) == TRUE ){
		print("saveDepotPet:%d \n", charaindex);
	}
#endif

	return CHAR_charSaveFromConnectAndChar( fd, ch, unlock );
}


/*------------------------------------------------------------
 * ログアウトする時に持ったままログアウトできないアイテムを
 * 落す
 * 引数
 *  charaindex  int     キャラクターのインデックス
 * 返り値
 *  なし
 ------------------------------------------------------------*/
static void CHAR_dropItemAtLogout( int charaindex )
{
	int i;
	for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
		int     itemindex;
		itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_CHECKINDEX(itemindex) == FALSE )continue;
		if( ITEM_getInt(itemindex,ITEM_DROPATLOGOUT ) == TRUE ) {
			{
				LogItem(
					CHAR_getChar( charaindex, CHAR_NAME ),
					CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
					itemindex,
#else
		       		ITEM_getInt( itemindex, ITEM_ID ),
#endif
					"LogoutDel(登出後消失的道具)",
					CHAR_getInt( charaindex,CHAR_FLOOR),
					CHAR_getInt( charaindex,CHAR_X ),
 		      		CHAR_getInt( charaindex,CHAR_Y ),
					ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
				);
			}
			CHAR_setItemIndex( charaindex, i ,-1);
	    CHAR_sendItemDataOne( charaindex, i);
			ITEM_endExistItemsOne(itemindex);
		}
	}
}

BOOL _CHAR_logout( char *file, int line, int charindex, BOOL save )
{
	int clifd,battleindex;
#ifdef _STREET_VENDOR
	int toindex;
#endif
	BOOL	ret = TRUE;
	int	fmindexi, channel, i;
	clifd = getfdFromCharaIndex(charindex);
	if( !CHAR_CHECKINDEX( charindex)) {
//		print( "logout err : clifd[%d] charaindex[%d] %s:%d from %s:%d \n", 
//				clifd, charindex, __FILE__, __LINE__, file, line);
		ret = FALSE;
	}
	battleindex = CHAR_getWorkInt( charindex, CHAR_WORKBATTLEINDEX );
	if( battleindex >= 0 ){
		BATTLE_EscapeDpSend( battleindex, charindex );
		CHAR_setInt( charindex, CHAR_DUELPOINT,
			CHAR_getInt( charindex, CHAR_DUELPOINT )
			+ CHAR_getWorkInt( charindex, CHAR_WORKGETEXP ) );
		CHAR_setWorkInt( charindex, CHAR_WORKDBATTLEESCAPE,	1);
		BATTLE_Exit( charindex, battleindex );
	}

	CHAR_dropItemAtLogout( charindex );
	CHAR_DischargeParty( charindex, 0);
	if(CHAR_getChar(charindex, CHAR_FMNAME) != NULL )	{
		if(strlen(CHAR_getChar(charindex, CHAR_FMNAME)) > 0 &&
			strlen(CHAR_getChar(charindex, CHAR_FMNAME)) < sizeof( STRING32) )
		if( (CHAR_getInt(charindex, CHAR_FMINDEX) >= 0)	&&                
				(strcmp(CHAR_getChar(charindex, CHAR_FMNAME), "") != 0) )	{
	   		saacproto_ACFMCharLogout_send(acfd,
	   		   CHAR_getChar(charindex, CHAR_FMNAME),
	   		   CHAR_getInt(charindex, CHAR_FMINDEX),
	   		   CHAR_getChar(charindex, CHAR_NAME),
	   		   CHAR_getChar(charindex, CHAR_CDKEY),
	   		   CHAR_getInt(charindex, CHAR_LV),
	   		   CHAR_getWorkInt(charindex, CHAR_WORKFMINDEXI),
	   		   CONNECT_getFdid(clifd));
		}
	}

#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
	if ( CHAR_getWorkInt ( charindex , CHAR_WORKCHATROOMTYPE ) != 0 ) {
		ChatRoom_Leave ( charindex ) ;
	}
#endif
#ifdef _STREET_VENDOR
	// 若玩家是卖方且正在交易中
	if(CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR) == 2){
		// 取出和他交易的人
		toindex = CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR_WHO);
		if(toindex > -1 && CHAR_CHECKINDEX(toindex)){
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
			lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
			CHAR_talkToCli(toindex,-1,"店家取消交易",CHAR_COLORYELLOW);
		}
	}
	// 玩家是买方
	else if(CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR) == 3){
		// 取出卖方
		toindex = CHAR_getWorkInt(charindex,CHAR_WORKSTREETVENDOR_WHO);
		if(toindex > -1 && CHAR_CHECKINDEX(toindex)){
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,1);
		}
	}
#endif

	// Robin 0606 channel	
	fmindexi = CHAR_getWorkInt(charindex, CHAR_WORKFMINDEXI);
	channel = CHAR_getWorkInt(charindex, CHAR_WORKFMCHANNEL);
	i = 0;
	if( (fmindexi > 0) && (channel >= 0) && (fmindexi < FAMILY_MAXNUM) && (channel < FAMILY_MAXCHANNEL)) {
		while( i < FAMILY_MAXMEMBER ) {
		    if( channelMember[fmindexi][channel][i] == charindex ) {
		    	channelMember[fmindexi][channel][i] = -1;
		    	break;
		    }
		    i++;
		}
	}
#ifdef _CHANNEL_MODIFY
	if(CHAR_getInt(charindex,PROFESSION_CLASS) > 0){
		int i,pclass = CHAR_getInt(charindex,PROFESSION_CLASS) - 1;
		for(i=0;i<getFdnum();i++){
			if(*(piOccChannelMember + (pclass * getFdnum()) + i) == charindex){
				*(piOccChannelMember + (pclass * getFdnum()) + i) = -1;
				break;
			}
		}
	}
#endif
	// Robin 0707 petFollow
	CHAR_pickupFollowPet( charindex, -1 );
	for( i=0; i<FAMILY_MAXMEMBER; i++ ){
		if ((fmindexi > 0) && (fmindexi < FAMILY_MAXNUM)){
			if( familyMemberIndex[fmindexi][i] == charindex )
				familyMemberIndex[fmindexi][i] = -1;
		}
	}
	// Robin 0629 silent
	if( CHAR_getInt( charindex, CHAR_SILENT) > 0 ){
		int silent_t, login_t;
		login_t = CHAR_getWorkInt(charindex, CHAR_WORKLOGINTIME);
		silent_t = CHAR_getInt( charindex, CHAR_SILENT);
		silent_t = silent_t - ( (int)NowTime.tv_sec-login_t);
		if( silent_t<0 ) silent_t = 0;
		CHAR_setInt( charindex, CHAR_SILENT, silent_t);
	}



#ifdef _ITEM_ADDEXP2
	if( CHAR_getInt( charindex, CHAR_ADDEXPPOWER) > 0 ) {
		// 登入时智果效果自动减90秒
		CHAR_setInt( charindex, CHAR_ADDEXPTIME,
			CHAR_getInt( charindex, CHAR_ADDEXPTIME) - 90 );
		if( CHAR_getInt( charindex, CHAR_ADDEXPTIME) <= 0 ) {
			CHAR_setInt( charindex, CHAR_ADDEXPTIME, 0);
			CHAR_setInt( charindex, CHAR_ADDEXPPOWER, 0);
		}
	}
#endif

#ifdef _VIP_BATTLE_EXP
		if( CHAR_getInt( charindex, CHAR_NVIPTIME) <= 0 ) {
			CHAR_setInt( charindex, CHAR_NVIPTIME, 0);
			CHAR_setInt( charindex, CHAR_NVIPEXP, 0);
		}
#endif

#ifdef _TIME_TICKET
	if( check_TimeTicketMap( CHAR_getInt( charindex, CHAR_FLOOR)) ) {
		CHAR_warpToSpecificPoint( charindex, 2006, 20, 17);
	}
#endif

#ifdef _AUTO_PK
		if(CHAR_getInt(charindex,CHAR_FLOOR)==20000 && CHAR_getInt(charindex,CHAR_AUTOPK)!=-1 && autopkstart == 1){
	  	char buf[64];
	  	int i,num=0,winindex;
	  	int playernum = CHAR_getPlayerMaxNum();
#ifndef	_YUANGUSA
	  	sprintf(buf, "胆小鬼%s退出当前比赛！",CHAR_getChar(charindex,CHAR_NAME));
			AutoPk_PKSystemTalk( buf, buf );
#endif
			if(CHAR_getWorkInt( charindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
				if(AutoPk_PKTimeGet()<=0){
					for(i=0;i<playernum;i++){
						if(CHAR_CHECKINDEX(i) == FALSE ) continue;
						if(CHAR_getInt(i, CHAR_FLOOR) == 20000 ){
							if(CHAR_getInt(i,CHAR_AUTOPK)!=-1){
								if(i!=charindex)
									winindex=i;
								num++;
							}
						}
					}
#ifdef _NONUM_AUTOPK
						if(num==5){
							AutoPk_ChampionShipSet( CHAR_getChar(charindex,CHAR_CDKEY), 
																CHAR_getChar(charindex,CHAR_NAME), CHAR_getInt(charindex,CHAR_AUTOPK),4);
						}else if(num==4){
							AutoPk_ChampionShipSet( CHAR_getChar(charindex,CHAR_CDKEY), 
																CHAR_getChar(charindex,CHAR_NAME), CHAR_getInt(charindex,CHAR_AUTOPK),3);
						}else if(num==3){
							AutoPk_ChampionShipSet( CHAR_getChar(charindex,CHAR_CDKEY), 
																CHAR_getChar(charindex,CHAR_NAME), CHAR_getInt(charindex,CHAR_AUTOPK),2);
						}else if(num==2){
							int fl = 0, x = 0, y = 0;
							CHAR_getElderPosition(CHAR_getInt(winindex, CHAR_LASTTALKELDER), &fl, &x, &y);
							CHAR_warpToSpecificPoint(winindex, fl, x, y);
							AutoPk_ChampionShipSet( CHAR_getChar(winindex,CHAR_CDKEY), 
																CHAR_getChar(winindex,CHAR_NAME), CHAR_getInt(winindex,CHAR_AUTOPK),0);
							AutoPk_ChampionShipSet( CHAR_getChar(charindex,CHAR_CDKEY), 
																CHAR_getChar(charindex,CHAR_NAME), CHAR_getInt(charindex,CHAR_AUTOPK),1);
							AutoPk_GetChampionShip();
						}
#else
					if(num==2){
						int fl = 0, x = 0, y = 0;
						char pktoken1[256],pktoken2[256];
						CHAR_getElderPosition(CHAR_getInt(winindex, CHAR_LASTTALKELDER), &fl, &x, &y);
						CHAR_warpToSpecificPoint(winindex, fl, x, y);
						autopkcnt++;
						int autopkvigor = getAutoPkVigorPkNum()*CHAR_getInt(charindex,CHAR_AUTOPK)+getAutoPkVigorPkCnt()*autopkcnt;
						CHAR_setInt(charindex,CHAR_VIGOR,CHAR_getInt(charindex,CHAR_VIGOR)+autopkvigor);
						sprintf(pktoken1,"[比赛战果]：「%s」击杀点数 %d 生存点数 %d 获得活力%d",CHAR_getChar(charindex,CHAR_NAME),CHAR_getInt(charindex,CHAR_AUTOPK),autopkcnt,autopkvigor);
						autopkcnt++;
						CHAR_setInt(winindex,CHAR_AUTOPK,CHAR_getInt(winindex,CHAR_AUTOPK)+2);
						autopkvigor = getAutoPkVigorPkNum()*CHAR_getInt(winindex,CHAR_AUTOPK)+getAutoPkVigorPkCnt()*autopkcnt;
						CHAR_setInt(winindex,CHAR_VIGOR,CHAR_getInt(winindex,CHAR_VIGOR)+autopkvigor);
						sprintf(pktoken2,"[比赛战果]：「%s」击杀点数 %d 生存点数 %d 获得活力%d",CHAR_getChar(winindex,CHAR_NAME),CHAR_getInt(winindex,CHAR_AUTOPK),autopkcnt,autopkvigor);
						i=0;
						for(i=0;i<playernum;i++){
							if(CHAR_CHECKINDEX(i) == FALSE) continue;
							CHAR_talkToCli( i, -1, pktoken1, CHAR_COLORCYAN );
							CHAR_talkToCli( i, -1, pktoken2, CHAR_COLORCYAN );
						}
						AutoPk_ChampionShipSet( CHAR_getChar(winindex,CHAR_CDKEY), 
															CHAR_getChar(winindex,CHAR_NAME), CHAR_getInt(winindex,CHAR_AUTOPK),0);
						AutoPk_ChampionShipSet( CHAR_getChar(winindex,CHAR_CDKEY), 
															CHAR_getChar(winindex,CHAR_NAME), CHAR_getInt(winindex,CHAR_AUTOPK),1);
						AutoPk_ChampionShipSet( CHAR_getChar(charindex,CHAR_CDKEY), 
															CHAR_getChar(charindex,CHAR_NAME), 
															CHAR_getInt(charindex,CHAR_AUTOPK),2);
						AutoPk_GetChampionShip();
					}
#endif
				}
			}
		}
#endif

	// Robin add
	CHAR_setInt( charindex, CHAR_LASTLEAVETIME, (int)time(NULL));
#ifdef _VIGOR_SYS
#ifdef _OFFLINE_SYSTEM 
	if(CHAR_getWorkInt( charindex, CHAR_WORK_OFFLINE ) == 0)
#endif
	{
	if(CHAR_getInt(charindex,CHAR_VIPRIDE)>0 && getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION))>0){
		if(CHAR_getInt(charindex,CHAR_VIGOR)<getVigorMax()){    //小于活力最大值
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			timep = mktime(p);
			int curtime = timep-CHAR_getWorkInt(charindex,CHAR_WORKVIGORTIME);
			if(curtime>=getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION)) && getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION))>0 && CHAR_getWorkInt(charindex,CHAR_WORKVIGORTIME)>0 ){
				CHAR_setInt(charindex,CHAR_VIGOR,min(CHAR_getInt(charindex,CHAR_VIGOR)+(int)(curtime/getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION))*2),getVigorMax()));
				//CHAR_setWorkInt(charindex,CHAR_WORKVIGORTIME,CHAR_getWorkInt(charindex,CHAR_WORKVIGORTIME)+(int)(curtime/getVigorTime(1))*getVigorTime(1));
				CHAR_setWorkInt(charindex,CHAR_WORKVIGORTIME,timep);
			}
		}
	}else if(CHAR_getInt(charindex,CHAR_VIPRIDE)<=0 && getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION))>0){
		if(CHAR_getInt(charindex,CHAR_VIGOR)<getVigorMax()){    //小于活力最大值
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			timep = mktime(p);
			int curtime = timep-CHAR_getWorkInt(charindex,CHAR_WORKVIGORTIME);
			if(curtime>=getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION)) && getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION))>0 && CHAR_getWorkInt(charindex,CHAR_WORKVIGORTIME)>0){
				CHAR_setInt(charindex,CHAR_VIGOR,min(CHAR_getInt(charindex,CHAR_VIGOR)+(int)(curtime/getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION))),getVigorMax()));
				//CHAR_setWorkInt(charindex,CHAR_WORKVIGORTIME,CHAR_getWorkInt(charindex,CHAR_WORKVIGORTIME)+(int)(curtime/getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION)))*getVigorTime(CHAR_getInt(charindex,CHAR_TRANSMIGRATION)));
				CHAR_setWorkInt(charindex,CHAR_WORKVIGORTIME,timep);
			}
		}
	}
#ifdef _ONLINE_TIME
	if (CHAR_getInt(charindex,CHAR_TRANSMIGRATION)>=0)
		{
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			timep = mktime(p);
			int curtime = timep-CHAR_getWorkInt(charindex,CHAR_WORKVIGORTIME);
			CHAR_setInt(charindex,CHAR_ONLINE_TIME,CHAR_getInt(charindex,CHAR_ONLINE_TIME)+curtime);
#ifdef _REGISTRATION_TIME
		//	CHAR_setInt(charindex,CHAR_REGISTRATION_ONLINE_TIME,CHAR_getInt(charindex,CHAR_REGISTRATION_ONLINE_TIME)+curtime);
#endif
		}
#endif
	}
#endif
//1111111111111111111111111111111111111




	if(CHAR_getInt(charindex,CHAR_FMINDEX) != -1 && (strcmp(CHAR_getChar(charindex,CHAR_FMNAME),""))){
			char	tempbuf[256];
			sprintf(tempbuf,"%d",CHAR_getInt(charindex,CHAR_FAME));
			saacproto_ACFixFMData_send(acfd,
				CHAR_getChar(charindex,CHAR_FMNAME),
				CHAR_getInt(charindex,CHAR_FMINDEX),
				CHAR_getWorkInt(charindex,CHAR_WORKFMINDEXI),
				FM_FIX_FAME,tempbuf,"",
				CHAR_getWorkInt(charindex,CHAR_WORKFMCHARINDEX),
				CONNECT_getFdid(clifd));
	}
#ifdef _ALLBLUES_LUA_1_4
	RunCharLogOutEvent(charindex);
#endif
	if( save ){
		CHAR_charSaveFromConnect( charindex ,TRUE);
	}
#ifdef _OFFLINE_SYSTEM
	if(CHAR_getWorkInt( charindex, CHAR_WORK_OFFLINE )!=0)
			playeronlinenum--;
#endif

	ADDRESSBOOK_notifyLoginLogout(charindex,0);
	CHAR_CharaDeleteHavePet( charindex);
	CHAR_CharaDelete( charindex );

	return ret;
}
/*------------------------------------------------------------
 * watchイベントを送るかどうかチェックする
 * 引数
 *  objindex    int             イベントをおこしたキャラのオブジェクト
 *                                  インデックス
 *	index		int				イベントをおこしたキャラのインデックス
 *  recvindex   int				イベントを受けとるキャラのindex
 *	chac		int				アクションの種類
 * 返り値 		TRUE:	送っていいよ
 *				FALSE: 	駄目
 ------------------------------------------------------------*/
static BOOL CHAR_sendWatchEvent_sendCheck( int objindex, int index,
											int recvindex, int chac)
{
	/* 自分がNPCで相手もNPCの時抜ける */
	if( OBJECT_getType( objindex) == OBJTYPE_CHARA &&
		CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER &&
		CHAR_getInt(recvindex, CHAR_WHICHTYPE )
		!= CHAR_TYPEPLAYER )
	{
		return FALSE;
	}
	/* キャラ以外のwatch はチェック無しに真 */
	if( OBJECT_getType( objindex) != OBJTYPE_CHARA) {
		return TRUE;
	}
	/*
	 * 自分への歩きのＣＡは無しにする
	 */
	if( index == recvindex && chac == CHAR_ACTWALK ) {
		return FALSE;
	}

	if( CHAR_getInt( recvindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
		/* 戦闘中の人には無駄だから送信しない */
		if( CHAR_getWorkInt( recvindex,CHAR_WORKBATTLEMODE)
			!= BATTLE_CHARMODE_NONE)
		{
			return FALSE;
		}
		/* アクションじゃ無い時 */
		if( CHAR_getWorkInt( index, CHAR_WORKACTION) == -1 ) {
			if( OBJECT_getType(objindex) == OBJTYPE_CHARA) {
				/* 同じパーティだったら
				 * 親以外は他のメンバーに歩きを送信しない
				 */
				if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE )
						== CHAR_PARTY_CLIENT &&
					CHAR_getWorkInt( recvindex, CHAR_WORKPARTYMODE)
						!= CHAR_PARTY_NONE &&
					chac == CHAR_ACTWALK )
				{
					int found = FALSE;
					int loop;
					/* 子を調べる */
					for( loop = 1; loop < CHAR_PARTYMAX; loop ++ ) {
						int partycharaindex = CHAR_getPartyIndex( recvindex, loop);
						/*  動いたキャラが自分のパーティの子だった */
						if( partycharaindex == index ) {
							found = TRUE;
							break;
						}
					}
					if( found ) return FALSE;
				}
				/* 親は子のSTANDを送信しない */
				if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE)
					== CHAR_PARTY_CLIENT &&
					CHAR_getWorkInt( recvindex, CHAR_WORKPARTYMODE)
						== CHAR_PARTY_LEADER &&
					chac == CHAR_ACTSTAND)
				{
					int found = FALSE;
					int loop;
					/* 子を調べる */
					for( loop = 1; loop < CHAR_PARTYMAX; loop ++ ) {
						int partycharaindex
							= CHAR_getWorkInt( recvindex,
											CHAR_WORKPARTYINDEX1+loop);
						/* STANDしたキャラが自分のパーティの子だった */
						if( partycharaindex == index ) {
							found = TRUE;
							break;
						}
					}
					if( found ) return FALSE;
				}
			}
		}
	}
	return TRUE;
}

void CHAR_sendWatchEvent( int objindex, int chac, int* opt,
						  int optlen,BOOL myflg )
{
	int     i;
	int j;
	int     x=0,y=0,dir=0;
	int     floor;
	int     watchopt[32];
	int     watchoptlen=0;
	typedef void (*WATCHF)(int,int,CHAR_ACTION,int,int,int,int*,int);
	int		index;

	if( !CHECKOBJECTUSE(objindex) )return;
	index = OBJECT_getIndex( objindex);
	switch( OBJECT_getType(objindex) ){
	case OBJTYPE_CHARA:
		for( i=0; i<optlen ; i++ ) {
			watchopt[i] = opt[i];
		}
		watchoptlen = optlen;
		dir = CHAR_getInt(index,CHAR_DIR);
		break;
	case OBJTYPE_ITEM:
		for( i=0; i<optlen ; i++ ) {
			watchopt[i] = opt[i];
		}
		watchoptlen = optlen;
		dir = 0;
		break;
	case OBJTYPE_GOLD:
		watchopt[0] = index;
		watchoptlen = 1;
		dir = 0;
		break;
	default:
		return;
	}
	x = OBJECT_getX(objindex);
	y = OBJECT_getY(objindex);
	floor = OBJECT_getFloor(objindex);

	for( i=x-CHAR_DEFAULTSEESIZ/2 ; i<=x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j=y-CHAR_DEFAULTSEESIZ/2 ; j<=y+CHAR_DEFAULTSEESIZ/2 ; j ++ ){
			OBJECT object;
			for( object = MAP_getTopObj(floor,i,j) ; object ;
				 object = NEXT_OBJECT(object) )
			{
				int receiveobjindex = GET_OBJINDEX(object);
				int	recvindex = OBJECT_getIndex( receiveobjindex);
				WATCHF watchfunc=NULL;

				switch( OBJECT_getType(receiveobjindex) ){
				  case OBJTYPE_CHARA:
					/* 自分あてでは送信しないフラグが立っている時抜ける */
					if( index == recvindex && myflg == FALSE ) {
						break;
					}
					/* watch イベントを起こすかどうか他にチェックする */
					if( !CHAR_sendWatchEvent_sendCheck( objindex,
														index, recvindex ,
														chac
														))
					{
						break;
					}

					watchfunc =(WATCHF)CHAR_getFunctionPointer(
						OBJECT_getIndex(receiveobjindex),
						CHAR_WATCHFUNC);
					break;
				  case OBJTYPE_ITEM:
					watchfunc = (WATCHF)ITEM_getFunctionPointer(
						OBJECT_getIndex(receiveobjindex),
						ITEM_WATCHFUNC);
					break;
				  case OBJTYPE_NOUSE:
				  case OBJTYPE_GOLD:
#ifdef _DEL_DROP_GOLD
					GOLD_DeleteTimeCheckOne( receiveobjindex);
#endif
				  default:
					break;
				}

				if( watchfunc == NULL )continue;
				watchfunc( receiveobjindex, objindex, chac, x, y, dir,
						   watchopt, watchoptlen );
			}
		}
	}
}


/*====================スキル====================*/
/*------------------------------------------------------------
 * スキルアップできるよプロトコルをしゃべる。
 * 引数
 *  charaindex      int     キャラインデックス
 * 返り値
 *  成功    TRUE(1)
 *  失敗(実際に送らなかったらエラーも含まれる)    FALSE(0)
 ------------------------------------------------------------*/
BOOL CHAR_Skillupsend(int charaindex )
{
#if 1
	// ストーンエイジ用に変更したもの
	int point;
	int fd;
	if( !CHAR_CHECKINDEX(charaindex) )return FALSE;
	// いくつポイントアップできるか
	point = CHAR_getInt( charaindex, CHAR_SKILLUPPOINT );
	fd = getfdFromCharaIndex(charaindex);
	lssproto_SKUP_send(fd, point );
	return TRUE;
#else
	char    sendbuf[128];

	// こちらはLS2時のもの
	if(SKILL_getUpableSkillID(charaindex,sendbuf,sizeof(sendbuf))){
		int fd;
		fd = getfdFromCharaIndex(charaindex);
		if( fd != -1 && sendbuf[0] != '\0' ){
			lssproto_SKUP_send(fd,sendbuf);
			return TRUE;
		}
	}
#endif
	return FALSE;
}

/*------------------------------------------------------------
* スキルアップする
 * 引数
 *  charaindex          int         キャラインデックス
 *  skillid             int         スキルID
 * 返り値
 *  なし
 ------------------------------------------------------------*/
void CHAR_SkillUp(  int charaindex, int skillid )
{
	if( !CHAR_CHECKINDEX(charaindex) )return;

	/*死んでいたらできない  */
	if( CHAR_getFlg(charaindex,CHAR_ISDIE) )return;

	{ 
		int SkUpTbl[] = { CHAR_VITAL, CHAR_STR, CHAR_TOUGH, CHAR_DEX };
		int	SendTbl[] = { CHAR_P_STRING_VITAL|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF,
						  CHAR_P_STRING_STR|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK,
						  CHAR_P_STRING_TOUGH|CHAR_P_STRING_MAXHP|CHAR_P_STRING_DEF,
						  CHAR_P_STRING_DEX|CHAR_P_STRING_MAXHP|CHAR_P_STRING_ATK|CHAR_P_STRING_QUICK
						};
		int cnt;
		//   範囲チェック
		if( skillid < 0 || skillid >= 4 ){
			return;
		}
		// まだパラメータアップできるか？
		cnt = CHAR_getInt(charaindex,CHAR_SKILLUPPOINT);
		if( cnt <= 0 )return;

#ifdef _CHAR_PROFESSION			// WON ADD 人物职业栏位
		{
			int p_class = CHAR_getInt( charaindex, PROFESSION_CLASS );
			int fd=-1;

			switch( p_class ){
				case PROFESSION_CLASS_FIGHTER:
					// 勇士锁敏200
					if( SkUpTbl[skillid] == CHAR_DEX ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "你的能力值已达上限", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );								
							return;
						}
					}						
					break;
				case PROFESSION_CLASS_WIZARD:
					// 巫师锁攻200
					if( SkUpTbl[skillid] == CHAR_STR ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "你的能力值已达上限", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					// 巫师锁防200
					if( SkUpTbl[skillid] == CHAR_TOUGH ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "你的能力值已达上限", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					break;
				case PROFESSION_CLASS_HUNTER:
					// 猎人锁攻200
					if( SkUpTbl[skillid] == CHAR_STR ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "你的能力值已达上限", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					// 猎人锁防200
					if( SkUpTbl[skillid] == CHAR_TOUGH ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 200 * 100 ){
							CHAR_talkToCli(charaindex, -1, "你的能力值已达上限", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					// 猎人锁敏400
					if( SkUpTbl[skillid] == CHAR_DEX ){
						if( CHAR_getInt( charaindex, SkUpTbl[skillid] ) >= 400 * 100 ){
							CHAR_talkToCli(charaindex, -1, "你的能力值已达上限", CHAR_COLORYELLOW );
							fd = getfdFromCharaIndex(charaindex);
							if( fd != -1 )	lssproto_SKUP_send( fd, cnt );										
							return;
						}
					}
					break;
				default: break;
			}

			fd = getfdFromCharaIndex(charaindex);
			if( fd != -1 )	lssproto_SKUP_send( fd, cnt-1 );
		}
#endif

		CHAR_setInt( charaindex,CHAR_SKILLUPPOINT,cnt-1);
		CHAR_setInt( charaindex, SkUpTbl[skillid], CHAR_getInt( charaindex, SkUpTbl[skillid] ) + 1*100 );

		CHAR_complianceParameter(charaindex);
		CHAR_send_P_StatusString( charaindex, SendTbl[skillid]);

#ifdef _ANGEL_SUMMON // 加点时检查资格
		if( cnt-1 == 0 )
			//selectAngel( charaindex);
			selectAngel( charaindex, -1, -1 , FALSE);

#endif
	}
	CHAR_PartyUpdate( charaindex, CHAR_N_STRING_MAXHP );

}

INLINE void CHAR_getDXDY( int dir , int* dx, int* dy )
{
	static POINT CHAR_dxdy[8]=
	{
		{ 0,-1},    /*  */
		{ 1,-1},    /*    */
		{ 1, 0},    /*  */
		{ 1, 1},    /*    */
		{ 0, 1},    /*  */
		{-1, 1},    /*    */
		{-1, 0},    /*  */
		{-1,-1},    /*    */
	};
	VALIDATEDIR(dir);

	if( dx )*dx = CHAR_dxdy[dir].x;
	if( dy )*dy = CHAR_dxdy[dir].y;
}
INLINE int CHAR_getDX( int dir )
{
	int tmp;
	CHAR_getDXDY( dir,&tmp,NULL);
	return tmp;
}
INLINE int CHAR_getDY( int dir )
{
	int tmp;
	CHAR_getDXDY( dir,NULL,&tmp);
	return tmp;
}

/*------------------------------------------------------------
 * 同じマスのキャラクターを得る。
 * 引数
 *  objbuf      int*        object 配列での index を代入する
 *                              バッファ
 *  siz         int         objbuf の サイズ
 *  ff          int         フロア
 *  fx          int         x
 *  fy          int         y
 * 返り値
 *  何個キャラがいるか。
 ------------------------------------------------------------*/
int CHAR_getSameCoordinateObjects(int* objbuf, int siz,int ff, int fx, int fy)
{
	OBJECT  object;
	int     findobjnum=0;

	for( object = MAP_getTopObj(ff,fx,fy) ; object ;
		 object = NEXT_OBJECT(object ) ){
		int objindex = GET_OBJINDEX(object);

		if( siz <= findobjnum )break;
		if( OBJECT_getType(objindex) != OBJTYPE_NOUSE )
			objbuf[findobjnum++] = objindex;
	}
	return findobjnum;
}


/*====================キャラの情報を得る系の関数====================*/
/*オプションデータの文字列を保存する関数*/
static char    CHAR_optiondataString[STRINGBUFSIZ];
/*------------------------------------------------------------
 * オプション用の文字列を作る。
 *  level, showstring , 魂の数 , 画像番号、 何回ログインしたか。
 *  クラス。
 * 引数
 *  ch      Char*       キャラデータ
 * 返り値
 *  char*   static な領域を返すので次回の呼び出しの時には内容が
 *  変更されている。引数をそのまま保存しない事。
 ------------------------------------------------------------*/
char* CHAR_makeOptionString( Char* ch )
{
	char    escapeshowstring[256];
	char *showstr = MAP_getfloorShowstring(ch->data[CHAR_FLOOR]);
	if( ! showstr ){
		CHAR_optiondataString[0] = '\0';
		return CHAR_optiondataString;
	} else {

#if 0  /* 以下はLS2仕様である */
		snprintf( CHAR_optiondataString,
			  sizeof( CHAR_optiondataString ),
			  "%d|%s|1|%d|%d",
			  ch->data[CHAR_LV],
			  makeEscapeString( showstr ,
				  escapeshowstring,sizeof(escapeshowstring) ),
			  ch->data[CHAR_BASEIMAGENUMBER],
			  ch->data[CHAR_LOGINCOUNT]);
#endif

        /* 以下はStoneAge 仕様である。

           dataplace|faceimg|level|hp|str|def|dex|charm|dp|earth|water|fire|wind|login|Name|Place
           となる。
         */
        {
            snprintf( CHAR_optiondataString,
                      sizeof( CHAR_optiondataString ),
                      "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%s|%s",
                      ch->data[CHAR_DATAPLACENUMBER],
                      ch->data[CHAR_FACEIMAGENUMBER],
                      ch->data[CHAR_LV],
                      ch->workint[CHAR_WORKMAXHP],
                      ch->workint[CHAR_WORKATTACKPOWER],
                      ch->workint[CHAR_WORKDEFENCEPOWER],
                      ch->workint[CHAR_WORKQUICK],
                      ch->workint[CHAR_WORKFIXCHARM],
                      ch->data[CHAR_DUELPOINT],    /* dp, FIXME */
                      ch->workint[CHAR_WORKFIXEARTHAT],
                      ch->workint[CHAR_WORKFIXWATERAT],
                      ch->workint[CHAR_WORKFIXFIREAT],
                      ch->workint[CHAR_WORKFIXWINDAT],
                      ch->data[CHAR_LOGINCOUNT],
                      ch->string[CHAR_NAME].string,
                      makeEscapeString( showstr ,
                                        escapeshowstring,
                                        sizeof(escapeshowstring) )
                      );
        }
    }
	return CHAR_optiondataString;
}

static char CHAR_statusSendBuffer[STRINGBUFSIZ];

char* CHAR_makeStatusString( int index, char* category )
{
	char    c = tolower( category[0] );
	int     strlength=0;
	
	if( !CHAR_CHECKINDEX( index ) ) return "\0";

	if( category == "\0" ){
		CHAR_statusSendBuffer[0] = '\0';
		return CHAR_statusSendBuffer;
	}
	switch( c ){
	case 'p':
	{
		int i;
		int getCharDataArray[]= {
			CHAR_NAME,
			CHAR_OWNTITLE,
			//CHAR_FMNAME,
		};
		int exp = CHAR_getInt(index,CHAR_EXP );
		int maxexp = CHAR_GetLevelExp( index, CHAR_getInt( index, CHAR_LV ) + 1);
		int	attr[4];

		for( i = 0; i < 4; i ++ ) {
			attr[i] = CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT + i);
			if( attr[i] < 0 ) attr[i] = 0;
		}
		int mytrans = CHAR_getInt( index, CHAR_TRANSMIGRATION);
		//if(mytrans > 6) mytrans = 0;
		snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
			"P1|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
				 CHAR_getInt(index,CHAR_HP),
				 CHAR_getWorkInt(index,CHAR_WORKMAXHP),
				 CHAR_getInt(index,CHAR_MP),
				 CHAR_getWorkInt(index,CHAR_WORKMAXMP),
				 CHAR_getInt(index,CHAR_VITAL)/100,
				 CHAR_getInt(index,CHAR_STR)/100,
				 CHAR_getInt(index,CHAR_TOUGH)/100,
				 CHAR_getInt(index,CHAR_DEX)/100,
				 exp, maxexp,
				 CHAR_getInt(index,CHAR_LV),
				 CHAR_getWorkInt(index,CHAR_WORKATTACKPOWER),
				 CHAR_getWorkInt(index,CHAR_WORKDEFENCEPOWER),
				 CHAR_getWorkInt( index, CHAR_WORKQUICK),
				 CHAR_getWorkInt( index, CHAR_WORKFIXCHARM),
				 CHAR_getWorkInt( index, CHAR_WORKFIXLUCK),
				 attr[0],
				 attr[1],
				 attr[2],
				 attr[3],
				 CHAR_getInt(index,CHAR_GOLD),
				 CHAR_getInt(index,CHAR_INDEXOFEQTITLE),
				 CHAR_getInt(index,CHAR_DUELPOINT),
				 mytrans,
				 CHAR_getInt(index,CHAR_RIDEPET),
				 CHAR_getInt(index,CHAR_LEARNRIDE),
				 CHAR_getInt(index,CHAR_BASEBASEIMAGENUMBER)
#ifdef _NEW_RIDEPETS
				 ,CHAR_getInt( index, CHAR_LOWRIDEPETS)
#endif

				);
		strlength = strlen( CHAR_statusSendBuffer );
		for( i = 0 ; i < arraysizeof( getCharDataArray ) ; i ++ ){
			char    token[256];
			char    escapebuffer[128];
			snprintf( token,
					  sizeof( token ),
					  "%s" STATUSSENDDELIMITER,
					  makeEscapeString( CHAR_getChar(index, getCharDataArray[i]),
					  escapebuffer,sizeof(escapebuffer)
					));
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof( CHAR_statusSendBuffer ) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= sizeof( CHAR_statusSendBuffer ) )
				return CHAR_statusSendBuffer;
		}
		return CHAR_statusSendBuffer;
		break;
	}
#ifdef _NEW_RIDEPETS
	case 'x':
		{
		snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer), "X0|%d",
				CHAR_getInt( index, CHAR_LOWRIDEPETS));
		return CHAR_statusSendBuffer;
			break;
		}
#endif
	case 'f':
	{
		int big4fm =0;
#ifndef _NEW_RIDEPETS
		switch( CHAR_getWorkInt( index, CHAR_WORKFMFLOOR) )
		{
			case 1041:
				big4fm = 1;
				break;
			case 2031:
				big4fm = 2;
				break;
			case 3031:
				big4fm = 3;
				break;
			case 4031:
				big4fm = 4;
				break;
			default:
				big4fm = 0;
		}
#endif		
#ifdef _FIX_FMNAME_RULE	 // WON ADD 家族未成立，不显示名称
		if( CHAR_getWorkInt(index, CHAR_WORKFMSETUPFLAG)==1 ){
			snprintf( CHAR_statusSendBuffer,
				sizeof( CHAR_statusSendBuffer ),
				"F%s|%d|%d|%d|%d",	
				"(待审)",
				CHAR_getInt( index, CHAR_FMLEADERFLAG),
				CHAR_getWorkInt( index, CHAR_WORKFMCHANNEL),
				CHAR_getInt( index, CHAR_FMSPRITE),	big4fm );		
		}else
#endif	

		snprintf( CHAR_statusSendBuffer,
			sizeof( CHAR_statusSendBuffer ),
			"F%s|%d|%d|%d|%d",
			CHAR_getChar( index, CHAR_FMNAME),
			CHAR_getInt( index, CHAR_FMLEADERFLAG),
			CHAR_getWorkInt( index, CHAR_WORKFMCHANNEL),
			CHAR_getInt( index, CHAR_FMSPRITE),	big4fm );
		return CHAR_statusSendBuffer;
		break;			
	}
	case 'c':
	{
		int floor,xsiz,ysiz;
		floor = CHAR_getInt(index,CHAR_FLOOR);
		xsiz = MAP_getfloorX(floor);
		ysiz = MAP_getfloorY(floor);
		snprintf( CHAR_statusSendBuffer,
				  sizeof( CHAR_statusSendBuffer ),
				  "C%d|%d|%d|%d|%d",
				  CHAR_getInt(index,CHAR_FLOOR),
				  xsiz, ysiz,
				  CHAR_getInt(index,CHAR_X),
				  CHAR_getInt(index,CHAR_Y) );
		return CHAR_statusSendBuffer;
		break;
	}
	case 'i':
	{
		int     i;
		CHAR_statusSendBuffer[0] = 'I';
		strlength = 1;
		for( i = 0 ; i < CHAR_MAXITEMHAVE ; i ++ ){
			char    token[512];
			int     itemindex;
			itemindex = CHAR_getItemIndex( index, i );
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
						CHAR_talkToCli( index, -1, itemname, CHAR_COLORYELLOW );
						CHAR_setItemIndex( index, i ,-1);
						itemindex = -1;
					}
				}
//				char itembuf[512];
//				sprintf(itembuf,"%s",ITEM_getChar(itemindex,ITEM_EFFECTSTRING));
//				if(strstr(itembuf,"有效期")!=NULL){
//					char itemtempbuf[64];
//					int year=3000,mon=13,date=32,hour=25,min=70,sec=70;
//					if(getStringFromIndexWithDelim(itembuf," ", 1, itemtempbuf, sizeof(itemtempbuf)) != FALSE){
//						char itemtempbuf2[64];
//						char itemtempbuf3[64];
//						char itemtempbuf4[32];
//						if(getStringFromIndexWithDelim(itemtempbuf,"/", 1, itemtempbuf2, sizeof(itemtempbuf2)) != FALSE){
//							if(getStringFromIndexWithDelim(itemtempbuf2,":", 2, itemtempbuf3, sizeof(itemtempbuf3)) != FALSE){
//								if(getStringFromIndexWithDelim(itemtempbuf3,".", 1, itemtempbuf4, sizeof(itemtempbuf4)) != FALSE)
//									year = atoi(itemtempbuf4);
//								if(getStringFromIndexWithDelim(itemtempbuf3,".", 2, itemtempbuf4, sizeof(itemtempbuf4)) != FALSE)
//									mon = atoi(itemtempbuf4);
//								if(getStringFromIndexWithDelim(itemtempbuf3,".", 3, itemtempbuf4, sizeof(itemtempbuf4)) != FALSE)
//									date = atoi(itemtempbuf4);
//							}
//						}
//						if(getStringFromIndexWithDelim(itemtempbuf,"/", 2, itemtempbuf2, sizeof(itemtempbuf2)) != FALSE){
//							if(getStringFromIndexWithDelim(itemtempbuf2,":", 1, itemtempbuf3, sizeof(itemtempbuf3)) != FALSE)
//								hour = atoi(itemtempbuf3);
//							if(getStringFromIndexWithDelim(itemtempbuf2,":", 2, itemtempbuf3, sizeof(itemtempbuf3)) != FALSE)
//								min = atoi(itemtempbuf3);
//							if(getStringFromIndexWithDelim(itemtempbuf2,":", 3, itemtempbuf3, sizeof(itemtempbuf3)) != FALSE)
//								sec = atoi(itemtempbuf3);
//						}
//						time_t timep;
//						struct tm *p;
//						time(&timep);
//						p=localtime(&timep);
//						int itemtype = 0;
//						if(year<(1900+p->tm_year))
//							itemtype = 1;
//						else if(year==(1900+p->tm_year)){
//							if(mon<(1+p->tm_mon))
//								itemtype = 1;
//							else if(mon==(1+p->tm_mon)){
//								if(date<(p->tm_mday))
//									itemtype = 1;
//								else if(date==(p->tm_mday)){
//									if(hour<(p->tm_hour))
//										itemtype = 1;
//									else if(hour==(p->tm_hour)){
//										if(min<(p->tm_min))
//											itemtype = 1;
//										else if(min==(p->tm_min)){
//											if(sec<(p->tm_sec))
//												itemtype = 1;
//										}
//									}
//								}
//							}
//						}
//						if(itemtype>0){
//							CHAR_setItemIndex( index, i ,-1);
//							itemindex = -1;
//	    				//CHAR_sendItemDataOne( charaindex, i);
//							//ITEM_endExistItemsOne(itemindex);
//						}
//					}
//				}
			}
			if( itemindex >= 0 ) {
				snprintf( token ,sizeof(token),"%s|" ,ITEM_makeItemStatusString( -1, itemindex ) );
			}else {
				snprintf( token ,sizeof(token),"%s|" , ITEM_makeItemFalseString() );
			}
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof(CHAR_statusSendBuffer) - strlength, token );
			strlength += strlen( token );
			if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) return CHAR_statusSendBuffer;
		}
		dchop( CHAR_statusSendBuffer , "|" );
		return CHAR_statusSendBuffer;
		break;
	}
	case 's':
	{
		int  i;
		CHAR_statusSendBuffer[0] = 'S';
		strlength = 1;

		for( i = 0 ; i < CHAR_SKILLMAXHAVE ; i ++ ){
			char    token[256];
			CHAR_HaveSkill* hskill;

			hskill = CHAR_getCharHaveSkill( index, i );

			if( hskill != NULL && hskill->use == TRUE ){
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
				snprintf( token , sizeof(token), "%s|", SKILL_makeSkillStatusString(&hskill->skill, index, i) );
#else
				snprintf( token , sizeof(token), "%s|", SKILL_makeSkillStatusString(&hskill->skill) );
#endif
			}else{
				snprintf( token , sizeof(token), "%s|", SKILL_makeSkillFalseString() );
			}

			strcpysafe( CHAR_statusSendBuffer + strlength, sizeof(CHAR_statusSendBuffer)-strlength, token );

			strlength += strlen( token );
			if( strlength >= arraysizeof(CHAR_statusSendBuffer) )
				return CHAR_statusSendBuffer;
		}

		dchop( CHAR_statusSendBuffer , "|" );
		return CHAR_statusSendBuffer;
		break;
	}
	case 't':
	{
		int     i;
		CHAR_statusSendBuffer[0] = 'T';
		strlength = 1;
		for( i = 0 ; i < CHAR_TITLEMAXHAVE ; i ++ ){
			char    token[256];
			char    escape[256];
			snprintf( token , sizeof(token), "%s|" ,
					  makeEscapeString( TITLE_makeTitleStatusString(index,
																	i ),
										escape,sizeof(escape) ) );

			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof(CHAR_statusSendBuffer) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= arraysizeof(CHAR_statusSendBuffer) )
				return CHAR_statusSendBuffer;
		}
		dchop( CHAR_statusSendBuffer , "|" );
		return CHAR_statusSendBuffer;
		break;
	}
	case 'd':
		snprintf( CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				  "D%d|%ld",CHAR_getWorkInt(index,CHAR_WORKOBJINDEX),
				  NowTime.tv_sec );
		return CHAR_statusSendBuffer;
		break;
	case 'm':
		snprintf( CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				  "M%d|%d|%d",
				  CHAR_getInt( index,CHAR_HP ),
				  CHAR_getInt( index,CHAR_MP ),
				  CHAR_getInt( index,CHAR_EXP ) );
		return CHAR_statusSendBuffer;
		break;
	case 'e':
		return "\0";

	case 'n':
	{
		int num;

		int i, nindex = -1;
		int getCharDataArray[]= {
			CHAR_NAME
		};
		num = tolower( category[1] ) - '0';

		if( num < 0 || num >= CHAR_PARTYMAX ){
			print( "朋友模式失败 (%c)%d \n", num, num );
			break;
		}

		nindex = CHAR_getPartyIndex( index, num);

		if( nindex == -1 ){
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "N%d|0|", num );
			return  CHAR_statusSendBuffer;
		}else{
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "N%d|1|%d|%d|%d|%d|%d|",
				 num,
				 CHAR_getWorkInt( nindex, CHAR_WORKOBJINDEX),
				 CHAR_getInt( nindex, CHAR_LV),
				 CHAR_getWorkInt( nindex, CHAR_WORKMAXHP),
				 CHAR_getInt( nindex, CHAR_HP),
				 CHAR_getInt( nindex, CHAR_MP)

			);
		}
		strlength = strlen( CHAR_statusSendBuffer );

		for( i = 0 ; i < arraysizeof( getCharDataArray ) ; i ++ ){
			char    token[256];
			char    escapebuffer[128];
			snprintf( token,
					  sizeof( token ),
					  "%s" STATUSSENDDELIMITER,
					  makeEscapeString( CHAR_getChar(nindex,
													 getCharDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
						  ));
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof( CHAR_statusSendBuffer ) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= sizeof( CHAR_statusSendBuffer ) )
				return CHAR_statusSendBuffer;
		}

		return CHAR_statusSendBuffer;
		break;
	}
	case 'k':
	{
		int num;
		int i, pindex;
		int getCharDataArray[]= {
			CHAR_NAME,
			CHAR_USERPETNAME,
		};
		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "宠物模式失败 (%c)%d \n", num, num );
			break;
		}
		pindex = CHAR_getCharPet( index, num );
		if( pindex == -1 ){
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "K%d|0|", num );
			return  CHAR_statusSendBuffer;
		}else{
			int		attr[4];
			BOOL	changenameflg = FALSE;
            char    *mycdkey;
			char	*cdkey = NULL;
			for( i = 0; i < 4; i ++ ) {
				attr[i] = CHAR_getWorkInt( pindex, CHAR_WORKFIXEARTHAT + i);
				if( attr[i] < 0 ) attr[i] = 0;
			}
			mycdkey = CHAR_getChar( index, CHAR_CDKEY);
			{
				cdkey = CHAR_getChar( pindex, CHAR_CDKEY);
				if( cdkey == "\0" ) {
					print( "can't get CDKEY\n");
				}else {

					if( strlen( cdkey) == 0 ||
						strcmp( cdkey, mycdkey) == 0 ){
						changenameflg = TRUE;
					}
				}
			}
			
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
// Syu ADD 改变交易时宠物转生颜色
#ifdef _SHOW_FUSION
				 "K%d|1|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
#else
 				 "K%d|1|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
#endif
				 num,
				 CHAR_getInt(pindex,CHAR_BASEIMAGENUMBER),
				 CHAR_getInt(pindex,CHAR_HP),
				 CHAR_getWorkInt(pindex,CHAR_WORKMAXHP),
				 CHAR_getInt(pindex,CHAR_MP),
				 CHAR_getWorkInt(pindex,CHAR_WORKMAXMP),
				 CHAR_getInt(pindex,CHAR_EXP ),
				 CHAR_GetLevelExp( pindex,CHAR_getInt( pindex, CHAR_LV ) + 1),
				 CHAR_getInt(pindex,CHAR_LV),
				 CHAR_getWorkInt(pindex,  CHAR_WORKATTACKPOWER),
				 CHAR_getWorkInt(pindex,  CHAR_WORKDEFENCEPOWER),
				 CHAR_getWorkInt( pindex, CHAR_WORKQUICK),
				 CHAR_getWorkInt( pindex, CHAR_WORKFIXAI),
				 attr[0],
				 attr[1],
				 attr[2],
				 attr[3],
				 CHAR_getInt( pindex, CHAR_SLOT),
// Syu ADD 改变交易时宠物转生颜色
				 changenameflg,
				 CHAR_getInt( pindex,CHAR_TRANSMIGRATION)
#ifdef _SHOW_FUSION
				,CHAR_getInt ( pindex, CHAR_FUSIONBEIT)
#endif
			);
		
		}
		strlength = strlen( CHAR_statusSendBuffer );
		for( i = 0 ; i < arraysizeof( getCharDataArray ) ; i ++ ){
			char    token[256];
			char    escapebuffer[128];
			snprintf( token,
					  sizeof( token ),
					  "%s" STATUSSENDDELIMITER,
					  makeEscapeString( CHAR_getChar(pindex,
													 getCharDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
						  ));
			strcpysafe( CHAR_statusSendBuffer + strlength,
						sizeof( CHAR_statusSendBuffer ) - strlength,
						token );
			strlength += strlen( token );
			if( strlength >= sizeof( CHAR_statusSendBuffer ) )
				return CHAR_statusSendBuffer;
		}
		return CHAR_statusSendBuffer;
		break;
	}

	case 'j':
{

		int num;
		int i, itemindex;
		int getMagicDataArray[]= {
			MAGIC_NAME,
			MAGIC_COMMENT,
		};
		num = tolower( category[1] ) - '0';

		if( num < 0 || num >= CHAR_EQUIPPLACENUM ){
			break;
		}

		itemindex = CHAR_getItemIndex( index, num );
		if( !ITEM_CHECKINDEX( itemindex) ) {
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "J%d|0|", num );
			return  CHAR_statusSendBuffer;
		}else{
			int	magicindex = MAGIC_getMagicArray( ITEM_getInt( itemindex, ITEM_MAGICID));
			if( magicindex == -1 ) {
				snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
					 "J%d|0|", num );
				return  CHAR_statusSendBuffer;
			}
			int magicmp = ITEM_getInt( itemindex, ITEM_MAGICUSEMP);
			if(CHAR_getInt(index,CHAR_LV)<100 && CHAR_getInt(index,CHAR_TRANSMIGRATION)<1){ //CF中指定精灵在0转100级之前不费气
				char magictmpbuf[32];
				sprintf(magictmpbuf,",%d,",ITEM_getInt( itemindex, ITEM_MAGICID));
				if(strstr(getNoMpMagic(),magictmpbuf)!=NULL){
					magicmp = 0;
				}
			}
			snprintf(CHAR_statusSendBuffer,sizeof(CHAR_statusSendBuffer),
				 "J%d|1|%d|%d|%d|",
				 num,
				 magicmp,
				 MAGIC_getInt( magicindex, MAGIC_FIELD),
				 MAGIC_getInt( magicindex, MAGIC_TARGET));
			strlength = strlen( CHAR_statusSendBuffer );

			for( i = 0 ; i < arraysizeof( getMagicDataArray ) ; i ++ ){
				char    token[256];
				char    escapebuffer[128];
				snprintf( token,
						  sizeof( token ),
						  "%s" STATUSSENDDELIMITER,
						  makeEscapeString( MAGIC_getChar(magicindex,
														 getMagicDataArray[i]),
											escapebuffer,sizeof(escapebuffer)
							  ));
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof( CHAR_statusSendBuffer ) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= sizeof( CHAR_statusSendBuffer ) )
					return CHAR_statusSendBuffer;
			}
			return CHAR_statusSendBuffer;
		}
 }
		break;

	case 'w':
{
		int num;
		int i,petskillloop, pindex;
		char	tmp[16];
		int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "宠物特技资料失败 (%c)%d \n", num, num );
			break;
		}

		pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return "\0";
		}
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}
		for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				snprintf( token, sizeof( token),
					 "%d|%d|%d|",

					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 PETSKILL_getInt( petskillindex, PETSKILL_FIELD),
					 PETSKILL_getInt( petskillindex, PETSKILL_TARGET)
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	case 'y':
{
		int num = 0, f_num = 0;
		int i,petskillloop, pindex;
		char	tmp[16];
		int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "宠物特技资料失败 (%c)%d \n", num, num );
			break;
		}

		pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return "\0";
		}
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}

		if( CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION) > 0 )
			f_num = CHAR_getWorkInt( pindex, CHAR_WORKMODOBLIVION ) + 1;// 遗忘宠技数量

		for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				int field = -1, target = -1;
				int oblivion = CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION );
				int p_id = PETSKILL_getInt( petskillindex, PETSKILL_ID);
				int rand_num = RAND( 0, 100 );
						
				// 中了遗忘
				if( (oblivion > 0) && ( f_num > 0 ) && ( rand_num <= 60 ) && ( p_id != 1 ) ){
					field = PETSKILL_FIELD_MAP;
					target = PETSKILL_TARGET_NONE;
					f_num --;
				}else{
					field = PETSKILL_getInt( petskillindex, PETSKILL_FIELD);
					target = PETSKILL_getInt( petskillindex, PETSKILL_TARGET);
				}

				snprintf( token, sizeof( token),"%d|%d|%d|",
//					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 p_id,
					 field,	target
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}
#endif

#ifdef _PETSKILL_DAMAGETOHP
//宠物战斗中发送技能封包
    case 'o':
{
	    int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};
		int i,petskillloop, pindex=index;
        int num = 0;//, f_num = 0;
        char	tmp[16];

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "宠物特技资料失败 (%c)%d \n", num, num );
			break;
		}

        pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return "\0";
		}
#ifdef _WAN_FIX
		if(CHAR_getWorkInt(index,CHAR_WORKBATTLEINDEX)>-1){
			return "\0";
		}
#endif
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}

        for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				int field = -1, target = -1;
				//int oblivion = CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION );
				int p_id = PETSKILL_getInt( petskillindex, PETSKILL_ID);
			
				//生命大於50%且技能名称若为 浴血狂袭 技能不能选
				if( ( CHAR_getInt( pindex, CHAR_HP ) > CHAR_getWorkInt( pindex, CHAR_WORKMAXHP )*0.5 
					&& strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"浴血狂袭" ) == 0 )
					){
					field = PETSKILL_FIELD_MAP;
					target = PETSKILL_TARGET_NONE;
				}else{
					field = PETSKILL_getInt( petskillindex, PETSKILL_FIELD);
					target = PETSKILL_getInt( petskillindex, PETSKILL_TARGET);
				}

				snprintf( token, sizeof( token),"%d|%d|%d|",
//					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 p_id,
					 field,	target
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}
#endif

#ifdef _PETSKILL_BECOMEFOX		
    case 'a':
{
	    int getPetSkillDataArray[]= {
			PETSKILL_NAME,
			PETSKILL_COMMENT,
		};
		int i,petskillloop, pindex=index;
        int num = 0;//, f_num = 0;
        char	tmp[16];

		num = tolower( category[1] ) - '0';
		if( num < 0 || num >= CHAR_MAXPETHAVE ){
			print( "宠物特技资料失败 (%c)%d \n", num, num );
			break;
		}
		print( "宠物特技资料" );
        pindex = CHAR_getCharPet( index, num );
		if( !CHAR_CHECKINDEX(pindex)){
			return "\0";
		}
		snprintf( tmp, sizeof( tmp), "W%d|", num);
		strcpysafe( CHAR_statusSendBuffer, sizeof( CHAR_statusSendBuffer),
					tmp);
		strlength += strlen( tmp);
		if( strlength >= arraysizeof( CHAR_statusSendBuffer) ) {
			return CHAR_statusSendBuffer;
		}        
		
		for( petskillloop = 0; petskillloop < CHAR_MAXPETSKILLHAVE; petskillloop ++ ) {
			int skillid = CHAR_getPetSkill( pindex, petskillloop);
			int petskillindex =  PETSKILL_getPetskillArray( skillid );
			if( PETSKILL_CHECKINDEX( petskillindex) ) {
				char    token[256];
				int field = -1, target = -1;
				//int oblivion = CHAR_getWorkInt( pindex, CHAR_WORKOBLIVION );
				int p_id = PETSKILL_getInt( petskillindex, PETSKILL_ID);
			
				//技能名称若为 待机 攻击 防御 技能才能选
				print("\n技能名称:%s",PETSKILL_getChar( petskillindex, PETSKILL_NAME));
				if( strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"待机" ) == 0
					|| strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"攻击" ) == 0
					|| strcmp(PETSKILL_getChar( petskillindex, PETSKILL_NAME),"防御" ) == 0){
					field = PETSKILL_getInt( petskillindex, PETSKILL_FIELD);
					target = PETSKILL_getInt( petskillindex, PETSKILL_TARGET);
				}else{
					field = PETSKILL_FIELD_MAP;
					target = PETSKILL_TARGET_NONE;
				}

				snprintf( token, sizeof( token),"%d|%d|%d|",
//					 PETSKILL_getInt( petskillindex, PETSKILL_ID),
					 p_id,
					 field,	target
				);
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer)) {
					return CHAR_statusSendBuffer;
				}

				for( i = 0 ; i < arraysizeof( getPetSkillDataArray ) ; i ++ ){
					char    escapebuffer[128];
					snprintf( token,
							  sizeof( token ),
							  "%s" STATUSSENDDELIMITER,
							  makeEscapeString(
							  			PETSKILL_getChar(petskillindex,
										getPetSkillDataArray[i]),
										escapebuffer,sizeof(escapebuffer)
								  ));
					strcpysafe( CHAR_statusSendBuffer + strlength,
								sizeof( CHAR_statusSendBuffer ) - strlength,
								token );
					strlength += strlen( token );
					if( strlength >= sizeof( CHAR_statusSendBuffer )) {
						return CHAR_statusSendBuffer;
					}
				}
			}
			else {
				char    token[256];
				snprintf( token, sizeof( token),"|||||");
				strcpysafe( CHAR_statusSendBuffer + strlength,
							sizeof(CHAR_statusSendBuffer) - strlength,
							token );
				strlength += strlen( token );
				if( strlength >= arraysizeof(CHAR_statusSendBuffer) ) {
					return CHAR_statusSendBuffer;
				}
			}
		}
		return CHAR_statusSendBuffer;
}
#endif

	default:
		break;
	}

	CHAR_statusSendBuffer[0] = '\0';
	return CHAR_statusSendBuffer;
}

//BOOL CHAR_sendStatusString( int charaindex, char* category )
BOOL _CHAR_sendStatusString( int charaindex, char* category, char* file, int line )
{
	char*   string;

	//print("\nsend_S:%s:%d ", file, line);

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}
	string = CHAR_makeStatusString( charaindex, category );

	if( string != "\0" ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}

void CHAR_inputOwnTitle( int index ,char* name )
{
	if( !CHAR_CHECKINDEX( index ) ) return;

	if( CHAR_getFlg( index,CHAR_ISDIE ) )return;

	CHAR_setChar( index, CHAR_OWNTITLE ,name);

	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( index, CHAR_WORKOBJINDEX) );

	CHAR_send_P_StatusString( index, CHAR_P_STRING_OWNTITLE );
}


void CHAR_selectTitle( int index, int titleindex )
{
	if( !CHAR_CHECKINDEX( index ) )return;

	if( CHAR_getFlg( index,CHAR_ISDIE ) )return;

	if( titleindex == -1 ){
		CHAR_setInt( index, CHAR_INDEXOFEQTITLE, titleindex);
	}else if( titleindex < 0 || titleindex >= CHAR_TITLEMAXHAVE ) {
		return;
	}
	else{

		int title = CHAR_getCharHaveTitle( index,titleindex );

		if( TITLE_getTitleIndex( title) != -1 ){
			CHAR_setInt( index, CHAR_INDEXOFEQTITLE, titleindex);
		}
	}

	CHAR_sendStatusString( index, "T" );
	CHAR_send_P_StatusString( index, CHAR_P_STRING_TITLE );
}


void CHAR_deleteTitle( int index, int titleindex )
{
	int title;

	if( !CHAR_CHECKINDEX( index ) )return;
	/* 死んでいる時はだめ */
	if( CHAR_getFlg( index,CHAR_ISDIE ) )return;

	if( titleindex < 0 || titleindex >= CHAR_TITLEMAXHAVE )return;
	title = CHAR_getCharHaveTitle( index,titleindex );
	/*if( TITLE_CHECKTABLEINDEX( title ) == FALSE )*/
	if( TITLE_getTitleIndex( title) == -1 ) {
		return;
	}
	CHAR_setCharHaveTitle( index,titleindex,-1 );

	/*  自分が使っていたやつならば、それもなしにする    */
	if( CHAR_getInt(index, CHAR_INDEXOFEQTITLE) == titleindex )
		CHAR_setInt(index, CHAR_INDEXOFEQTITLE, -1 );

	/*
	 *ステータス情報を送る。
	 */
	CHAR_sendStatusString( index, "T" );
	CHAR_send_P_StatusString( index, CHAR_P_STRING_TITLE );
}

static void CHAR_initcharWorkInt( index )
{
	float hp;
	static struct substitutionTable{
		CHAR_WORKDATAINT    workindex;
		CHAR_DATAINT        intindex;
	}subtbl[]={
		{ CHAR_WORKMAXMP,		CHAR_MAXMP  },
		{ CHAR_WORKFIXFIREAT,   CHAR_FIREAT },
		{ CHAR_WORKFIXWATERAT,  CHAR_WATERAT},
		{ CHAR_WORKFIXEARTHAT,  CHAR_EARTHAT},
		{ CHAR_WORKFIXWINDAT,   CHAR_WINDAT },
		{ CHAR_WORKFIXCHARM,    CHAR_CHARM  },
		{ CHAR_WORKFIXLUCK,     CHAR_LUCK   },
		{ CHAR_WORKFIXLUCK,     CHAR_LUCK   },
		{ CHAR_WORKMODPOISON,   CHAR_POISON },
		{ CHAR_WORKMODPARALYSIS,CHAR_PARALYSIS },
		{ CHAR_WORKMODSLEEP,	CHAR_SLEEP },
		{ CHAR_WORKMODSTONE,	CHAR_STONE },
		{ CHAR_WORKMODDRUNK,	CHAR_DRUNK },
		{ CHAR_WORKMODCONFUSION, CHAR_CONFUSION }
	};

	int i;
	for( i=0 ; i<arraysizeof(subtbl) ; i++ )
		CHAR_setWorkInt(index,subtbl[i].workindex, 	CHAR_getInt(index,subtbl[i].intindex));

	CHAR_setWorkInt( index, CHAR_WORKMODWEAKEN, 0);
	CHAR_setWorkInt( index, CHAR_WORKMODDEEPPOISON, 0);
	CHAR_setWorkInt( index, CHAR_WORKMODBARRIER, 0);
	CHAR_setWorkInt( index, CHAR_WORKMODNOCAST, 0);

	for( i = 0; i < 4; i ++ ){
		int attr = CHAR_getWorkInt( index, CHAR_WORKFIXEARTHAT + i);
		if( attr > 0 ){
			CHAR_setWorkInt( index, CHAR_WORKFIXEARTHAT + (i + 2) % 4, attr * -1);
		}
	}

#ifdef _MAGIC_RESIST_EQUIT			// WON ADD 职业抗性装备
	{
		// 火冰电抗性
		//for( i=0; i<3; i++)
		//	CHAR_setWorkInt( index, CHAR_WORK_F_RESIST+i, CHAR_getInt( index, PROFESSION_FIRE_R+i ) );
	}
#endif


	CHAR_setWorkInt( index, CHAR_WORKFIXDEX,
		CHAR_getInt( index, CHAR_DEX) * 0.01 );
	CHAR_setWorkInt( index, CHAR_WORKFIXVITAL,
		CHAR_getInt( index, CHAR_VITAL) * 0.01 );

	CHAR_setWorkInt( index, CHAR_WORKFIXSTR,
		CHAR_getInt( index, CHAR_STR) * 0.01 * 1.0
//#ifdef _BATTLE_NEWPOWER
//		+ CHAR_getInt( index, CHAR_DEX) * 0.01 * 0.05
//		+ CHAR_getInt( index, CHAR_VITAL) * 0.01 * 0.05	); // 耐力
//#else
		+ CHAR_getInt( index, CHAR_TOUGH) * 0.01 * 0.1
		+ CHAR_getInt( index, CHAR_VITAL) * 0.01 * 0.1
		+ CHAR_getInt( index, CHAR_DEX) * 0.01 * 0.05 );
//#endif
	CHAR_setWorkInt( index, CHAR_WORKFIXTOUGH,
		CHAR_getInt( index, CHAR_TOUGH) * 0.01 * 1.0
//#ifdef _BATTLE_NEWPOWER
//		+ CHAR_getInt( index, CHAR_DEX) * 0.01 * 0.05
//		+ CHAR_getInt( index, CHAR_VITAL) * 0.01 * 0.05	);	// 力量
//#else
		+ CHAR_getInt(index,CHAR_STR) * 0.01 * 0.1
		+ CHAR_getInt(index,CHAR_VITAL) * 0.01 * 0.1
		+ CHAR_getInt(index,CHAR_DEX) * 0.01 * 0.05 );
//#endif
  
#ifdef _ITEMSET5_TXT
	{
		CHAR_setWorkInt( index, CHAR_WORKFIXARRANGE, 0);
		CHAR_setWorkInt( index, CHAR_WORKFIXSEQUENCE, 0);
		CHAR_setWorkInt( index, CHAR_WORKATTACHPILE, 0);
		CHAR_setWorkInt( index, CHAR_WORKHITRIGHT, 0);
	}
#endif



#ifdef _ITEMSET6_TXT
	{
		CHAR_setWorkInt( index, CHAR_WORKNEGLECTGUARD, 0);
	}
#endif


	hp = ( CHAR_getInt(index,CHAR_VITAL) * 4
		+    CHAR_getInt(index,CHAR_STR) * 1
		+    CHAR_getInt(index,CHAR_TOUGH) * 1
		+    CHAR_getInt(index,CHAR_DEX) * 1 ) * 0.01;

	 CHAR_setWorkInt( index, CHAR_WORKMAXHP, (int)hp );
	 CHAR_setWorkInt( index, CHAR_WORKMAXMP, CHAR_getInt( index, CHAR_MAXMP));
	
	int fmfloor = CHAR_getWorkInt( index, CHAR_WORKFMFLOOR);
#ifdef	_FM_WELFARE	
  if( fmfloor !=-1 && CHAR_getInt( index, CHAR_FMLEADERFLAG ) != FMMEMBER_NONE
					&& CHAR_getInt( index, CHAR_FMLEADERFLAG ) != FMMEMBER_APPLY && CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
		char* fmbuf=NULL;
		char* pszP=NULL;
		int fPer;
		switch(fmfloor){
			case 1041:
				fmbuf = getFmWelfare(0);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );
						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 2031:
				fmbuf = getFmWelfare(1);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 3031:
				fmbuf = getFmWelfare(2);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 4031:
				fmbuf = getFmWelfare(3);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 5031:
				fmbuf = getFmWelfare(4);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 6031:
				fmbuf = getFmWelfare(5);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 7031:
				fmbuf = getFmWelfare(6);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 8031:
				fmbuf = getFmWelfare(7);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 9031:
				fmbuf = getFmWelfare(8);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			case 10031:
				fmbuf = getFmWelfare(9);
				if( (pszP = strstr( fmbuf, "攻" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXSTR,CHAR_getWorkInt(index,CHAR_WORKFIXSTR)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "防" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXTOUGH,CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "敏" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKFIXDEX,CHAR_getWorkInt(index,CHAR_WORKFIXDEX)+fPer);
						break;
				}
				if( (pszP = strstr( fmbuf, "体" ) ) != NULL ){
						sscanf( pszP+2, "%d", &fPer );

						//设定修正值
						CHAR_setWorkInt(index,CHAR_WORKMAXHP,CHAR_getWorkInt(index,CHAR_WORKMAXHP)+fPer);
						break;
				}
				break;
			default:
				break;
		}
	}
#endif
#ifdef	_FMZUZHANG_MSG
	if(fmfloor>0 && CHAR_getInt(index, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER && CHAR_getWorkInt( index, CHAR_WORKLOGININTYPE) == 0){
		char floorbuf[10];
		sprintf(floorbuf,"%d",fmfloor);
		if(strstr(getFmZuzhang(),floorbuf)){
			char zuzhangmsg[256];
			int i,playernum;
			playernum = CHAR_getPlayerMaxNum();
			sprintf(zuzhangmsg,"尊贵的%s庄主【%s】上线啦，大家快去围观！",MAP_getFloorName(fmfloor-1),CHAR_getChar(index,CHAR_NAME));
			CHAR_setWorkInt( index, CHAR_WORKLOGININTYPE,1);
			for( i = 0 ; i < playernum ; i++)
			{
				CHAR_talkToCli( i, -1, zuzhangmsg, CHAR_COLORRED);
  		}
		}
	}
#endif
	
	CHAR_setWorkInt( index, CHAR_WORKATTACKPOWER,
		CHAR_getWorkInt(index,CHAR_WORKFIXSTR) );
	CHAR_setWorkInt( index, CHAR_WORKDEFENCEPOWER,
		CHAR_getWorkInt(index,CHAR_WORKFIXTOUGH) );
	CHAR_setWorkInt( index, CHAR_WORKQUICK,
		CHAR_getWorkInt( index, CHAR_WORKFIXDEX));
#ifdef _ADD_DEAMGEDEFC
	CHAR_setWorkInt( index, CHAR_WORKOTHERDMAGE, 0);
	CHAR_setWorkInt( index, CHAR_WORKOTHERDEFC, 0);
#endif

	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
		int		hostindex;
		CHAR_setWorkInt( index, CHAR_WORKFIXAI, 0);
		hostindex =  CHAR_getWorkInt( index, CHAR_WORKPLAYERINDEX);
		if( CHAR_CHECKINDEX( hostindex)) {
			int		ai;
			int 	modai = CHAR_getInt( index, CHAR_MODAI);
			double	R;
			if( modai <= 0 ) modai = 100;
			R = 1.10;
			ai =  ( (CHAR_getInt( hostindex, CHAR_LV)
				    * CHAR_getWorkInt( hostindex, CHAR_WORKFIXCHARM )
				    * R)
				   /
				   (CHAR_getInt( index, CHAR_LV)* modai) * 100 ) ;
			if( ai > 100 ) ai = 100;

			ai += CHAR_getInt( index, CHAR_VARIABLEAI) * 0.01;				
			if( CHAR_getInt( hostindex, CHAR_TRANSMIGRATION ) > 0 ){
				// shan   120 -> 140				
				ai += ( CHAR_GetLevel() - CHAR_getInt( hostindex, CHAR_LV ) ) / 2;
			}
			if( ai < 0 ) ai = 0;
			if( ai > 100 ) ai = 100;
			CHAR_setWorkInt( index, CHAR_WORKFIXAI, ai);
		}
	}
}

int _CHAR_complianceParameter( int index, char *FILE, int LINE)
{
	//print("\ncompPara:%s:%d", FILE, LINE);
	
	if( !CHAR_CHECKINDEX( index)) return 0;
//CAX		
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER 
		&& CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEENEMY 
		&& CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPET 
#ifdef _ALLBLUES_LUA 
		&& CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPELUANPC 
#endif
#ifdef _PLAYER_NPC 
		&& CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC 
		&& CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERPETNPC 
#endif
		){
		return 0;
	}
	CHAR_initcharWorkInt( index );
	ITEM_equipEffect( index );

	CHAR_setWorkInt(index, CHAR_WORKATTACKPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXSTR ) );
	CHAR_setWorkInt(index, CHAR_WORKDEFENCEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH ) );
	CHAR_setWorkInt(index, CHAR_WORKQUICK,
		CHAR_getWorkInt( index, CHAR_WORKFIXDEX ) );

#ifdef _ITEMSET5_TXT
	CHAR_setWorkInt(index, CHAR_WORKARRANGEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXARRANGE ) );
	CHAR_setWorkInt(index, CHAR_WORKSEQUENCEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXSEQUENCE ) );
#endif

	Other_DefcharWorkInt( index);
	CHAR_setInt( index, CHAR_HP, min( CHAR_getInt(index,CHAR_HP), CHAR_getWorkInt( index, CHAR_WORKMAXHP)));
	CHAR_setInt( index, CHAR_MP, min( CHAR_getInt(index,CHAR_MP), CHAR_getWorkInt( index, CHAR_WORKMAXMP)));

	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){
		if( CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW ) != -1 ){
			int petindex = CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW );
			if( !CHAR_CHECKINDEX( petindex) ){
				CHAR_setWorkInt( index, CHAR_WORKPETFOLLOW, -1);
			}else{
				if( CHAR_getInt( petindex, CHAR_WHICHTYPE) == CHAR_TYPEPET && 
					!CHAR_CHECKINDEX( CHAR_getWorkInt( petindex, CHAR_WORKPLAYERINDEX)) ){
						CHAR_setWorkInt( index, CHAR_WORKPETFOLLOW, petindex);
						CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, index);
					}
			}
		}
		
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
		{
			int i;
			
			for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
				int skillID = CHAR_getIntPSkill( index, i, SKILL_IDENTITY );
				
				if( skillID <= 0 ) continue;
				
				// 负重增加
				if( skillID == 65 ){
					PROFESSION_strong_back(index,NULL,65,NULL,NULL);
					break;
				}
			}
		}
#endif
	}

	if( CHAR_getFlg(index,CHAR_ISDIE) == FALSE ){
		int oldimagenumber = CHAR_getInt(index,CHAR_BASEIMAGENUMBER);
		int itemindex = CHAR_getItemIndex(index,CHAR_ARM);
		int category;
		int newimagenumber;
		int basebaseimagenumber = CHAR_getInt(index,CHAR_BASEBASEIMAGENUMBER);
		if( !ITEM_CHECKINDEX(itemindex) )
			category = ITEM_FIST;
		else
			category = ITEM_getInt(itemindex,ITEM_TYPE );

		newimagenumber = CHAR_getNewImagenumberFromEquip( basebaseimagenumber, category);
#ifdef _ITEM_METAMO
		if( CHAR_getWorkInt( index, CHAR_WORKITEMMETAMO) > NowTime.tv_sec 
#ifdef _NPCCHANGE_PLAYERIMG
			|| CHAR_getWorkInt( index, CHAR_WORKNPCMETAMO) > 0
#endif
#ifdef _PETSKILL_BECOMEPIG
			|| CHAR_getInt( index, CHAR_BECOMEPIG) > -1
#endif
			){
			newimagenumber = oldimagenumber; //不恢复成原本的图
		}
#endif
		// Arminius 8.9 airplane
		if (oldimagenumber==100259) newimagenumber=100259;
#ifdef _VARY_WOLF		
		if( (CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPET) &&
			(CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) && 			
			oldimagenumber == 101428 ){
			newimagenumber = 101428;
		}
#endif

#ifdef _ENEMY_FALLGROUND
		if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEENEMY )	{
			return 0;
		}else	{
#endif
			if(CHAR_getInt(index, CHAR_RIDEPET) == -1){
				if( newimagenumber == -1 ){
					CHAR_setInt(index,CHAR_BASEIMAGENUMBER, basebaseimagenumber);
				}else	{
					CHAR_setInt(index,CHAR_BASEIMAGENUMBER,newimagenumber);				
				}
			}

#ifdef _ENEMY_FALLGROUND
		}
#endif
#ifdef _NEW_RIDEPETS
		if( (CHAR_getWorkInt(index, CHAR_WORKITEMMETAMO)>NowTime.tv_sec)
			|| (CHAR_getWorkInt(index,CHAR_WORKNPCMETAMO)>0)){
			return 0;
		}
#endif
	}
	return 1;
}

int CHAR_complianceParameter2( int index)
{
	//print("\ncompPara:%s:%d", FILE, LINE);
	
	if( !CHAR_CHECKINDEX( index)) return 0;

	//CHAR_initcharWorkInt( index );
	//ITEM_equipEffect( index );

	CHAR_setWorkInt(index, CHAR_WORKATTACKPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXSTR ) );
	CHAR_setWorkInt(index, CHAR_WORKDEFENCEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXTOUGH ) );
	CHAR_setWorkInt(index, CHAR_WORKQUICK,
		CHAR_getWorkInt( index, CHAR_WORKFIXDEX ) );

#ifdef _ITEMSET5_TXT
	CHAR_setWorkInt(index, CHAR_WORKARRANGEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXARRANGE ) );
	CHAR_setWorkInt(index, CHAR_WORKSEQUENCEPOWER,
		CHAR_getWorkInt( index, CHAR_WORKFIXSEQUENCE ) );
#endif

	Other_DefcharWorkInt( index);
	CHAR_setInt( index, CHAR_HP, min( CHAR_getInt(index,CHAR_HP), CHAR_getWorkInt( index, CHAR_WORKMAXHP)));
	CHAR_setInt( index, CHAR_MP, min( CHAR_getInt(index,CHAR_MP), CHAR_getWorkInt( index, CHAR_WORKMAXMP)));

	if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ){
		if( CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW ) != -1 ){
			int petindex = CHAR_getWorkInt( index, CHAR_WORKPETFOLLOW );
			if( !CHAR_CHECKINDEX( petindex) ){
				CHAR_setWorkInt( index, CHAR_WORKPETFOLLOW, -1);
			}else{
				if( CHAR_getInt( petindex, CHAR_WHICHTYPE) == CHAR_TYPEPET && 
					!CHAR_CHECKINDEX( CHAR_getWorkInt( petindex, CHAR_WORKPLAYERINDEX)) ){
						CHAR_setWorkInt( index, CHAR_WORKPETFOLLOW, petindex);
						CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, index);
					}
			}
		}
		
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
		{
			int i;
			
			for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
				int skillID = CHAR_getIntPSkill( index, i, SKILL_IDENTITY );
				
				if( skillID <= 0 ) continue;
				
				// 负重增加
				if( skillID == 65 ){
					PROFESSION_strong_back(index,NULL,65,NULL,NULL);
					break;
				}
			}
		}
#endif
	}

	if( CHAR_getFlg(index,CHAR_ISDIE) == FALSE ){
		int oldimagenumber = CHAR_getInt(index,CHAR_BASEIMAGENUMBER);
		int itemindex = CHAR_getItemIndex(index,CHAR_ARM);
		int category;
		int newimagenumber;
		int basebaseimagenumber = CHAR_getInt(index,CHAR_BASEBASEIMAGENUMBER);
		if( !ITEM_CHECKINDEX(itemindex) )
			category = ITEM_FIST;
		else
			category = ITEM_getInt(itemindex,ITEM_TYPE );

		newimagenumber = CHAR_getNewImagenumberFromEquip( basebaseimagenumber, category);
#ifdef _ITEM_METAMO
		if( CHAR_getWorkInt( index, CHAR_WORKITEMMETAMO) > NowTime.tv_sec 
#ifdef _NPCCHANGE_PLAYERIMG
			|| CHAR_getWorkInt( index, CHAR_WORKNPCMETAMO) > 0
#endif
#ifdef _PETSKILL_BECOMEPIG
			|| CHAR_getInt( index, CHAR_BECOMEPIG) > -1
#endif
			){
			newimagenumber = oldimagenumber; //不恢复成原本的图
		}
#endif
		// Arminius 8.9 airplane
		if (oldimagenumber==100259) newimagenumber=100259;
#ifdef _VARY_WOLF		
		if( (CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEPET) &&
			(CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) && 			
			oldimagenumber == 101428 ){
			newimagenumber = 101428;
		}
#endif

#ifdef _ENEMY_FALLGROUND
		if( CHAR_getInt( index, CHAR_WHICHTYPE) == CHAR_TYPEENEMY )	{
			return 0;
		}else	{
#endif
			if(CHAR_getInt(index, CHAR_RIDEPET) == -1){
				if( newimagenumber == -1 ){
					CHAR_setInt(index,CHAR_BASEIMAGENUMBER, basebaseimagenumber);
				}else	{
					CHAR_setInt(index,CHAR_BASEIMAGENUMBER,newimagenumber);				
				}
			}

#ifdef _ENEMY_FALLGROUND
		}
#endif
#ifdef _NEW_RIDEPETS
		if( (CHAR_getWorkInt(index, CHAR_WORKITEMMETAMO)>NowTime.tv_sec)
			|| (CHAR_getWorkInt(index,CHAR_WORKNPCMETAMO)>0)){
			return 0;
		}
#endif
	}
	return 1;
}

/*======================================================================
  キャラのアイテムを扱う関数
  ======================================================================*/

/*====================見る====================*/
/*------------------------------------------------------------
 * 方向指定で、その方向のキャラクタに見るイベントを発生させる
 * 引数
 *  charaindex      int     キャラインデックス
 *  dir             int     方向
 * 返り値
 *  なし

 ------------------------------------------------------------*/
void CHAR_Look( int charaindex, int dir )
{
	int fl ,x,y;
	OBJECT  object;

	/*  インデックスのチェック  */
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;

	if( dir < 0 ) {
		dir = CHAR_getInt(charaindex,CHAR_DIR);
	}
	VALIDATEDIR(dir);
	if( CHAR_getInt( charaindex, CHAR_DIR) != dir ) {
		CHAR_setInt(charaindex,CHAR_DIR,dir);
		/* 方向変換したアクションを送信する  */
		CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX),
							CHAR_ACTTURN,NULL,0,TRUE);
	}

	fl = CHAR_getInt(charaindex,CHAR_FLOOR);
	CHAR_getCoordinationDir( CHAR_getInt(charaindex,CHAR_DIR),
							 CHAR_getInt(charaindex,CHAR_X),
							 CHAR_getInt(charaindex,CHAR_Y),
							 1, &x, &y );

	//CHAR_talkToCli( charaindex, -1, "调查四周环境。", CHAR_COLORWHITE );

	for( object=MAP_getTopObj(fl,x,y) ; object ;
		 object = NEXT_OBJECT(object) ){
		int objindex = GET_OBJINDEX(object);
		if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
			typedef void (*LOOKF)(int,int);
				LOOKF lf;
				lf = (LOOKF)CHAR_getFunctionPointer(
					OBJECT_getIndex(objindex),CHAR_LOOKEDFUNC);
				if( lf ) lf(OBJECT_getIndex(objindex),charaindex);
#ifdef _ALLBLUES_LUA
							else{
								RunCharTalkedEvent(OBJECT_getIndex(objindex),charaindex,"hi",1, -1);
							}
#endif
		}
	}
}

BOOL _CHAR_makeObjectCString( char *file, int line, int objindex,char* buf, int buflen )
{
	char    objindexbuf[64];
	if( CHECKOBJECT(objindex) == FALSE )    return FALSE;

	switch( OBJECT_getType(objindex) ){
	case OBJTYPE_NOUSE:
		return FALSE;
		break;
	case OBJTYPE_CHARA:
	{
		int     charaindex = OBJECT_getIndex(objindex);
		char    escapename[256];
		char    escapetitle[256];
		// shan add +1
		char    tmp[256];
		int namecolor;
#ifdef _TRANS_6
#ifdef _TRANS_7
		int namecolortbl[] = { CHAR_COLORWHITE, CHAR_COLORYELLOW, CHAR_COLORGREEN,
					CHAR_COLORCYAN, CHAR_COLORRED, CHAR_COLORPURPLE, CHAR_COLORBLUE2,CHAR_COLORGREEN2,CHAR_COLORBLACK};//转生後的颜色
#else
		int namecolortbl[] = { CHAR_COLORWHITE, CHAR_COLORYELLOW, CHAR_COLORGREEN,
					CHAR_COLORCYAN, CHAR_COLORRED, CHAR_COLORPURPLE, CHAR_COLORBLUE2};//转生後的颜色
#endif
#else
		int namecolortbl[] = { CHAR_COLORWHITE, CHAR_COLORYELLOW, CHAR_COLORGREEN,
					CHAR_COLORCYAN, CHAR_COLORRED, CHAR_COLORPURPLE};
#endif
		int ridepet = BATTLE_getRidePet( charaindex );
		char petname[64] = "";
		char escapepetname[256];
		int petlevel = 0;

		if( !CHAR_getFlg(charaindex,CHAR_ISVISIBLE) )return FALSE;
		namecolor = CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
#ifdef _TRANS_6
#ifdef _TRANS_7
		if( namecolor > 8 ) namecolor = 8;
#else
		if( namecolor > 6 ) namecolor = 6;
#endif
#else
		if( namecolor > 5 ) namecolor = 5;
#endif
		if( namecolor < 0 ) namecolor = 0;
		
		// shan add begin
#ifdef _NEW_NAME         //自定义称号
		char* szNewName = CHAR_getChar( charaindex, CHAR_NEWNAME);
#endif

#ifdef _SHOW_VIP_CF
		char VipName[32]="";
		if(getShowVip()!=0)
			if(CHAR_getInt( charaindex,CHAR_VIPRIDE )>0)
				sprintf(VipName, "VIP-");
#endif
#ifdef _ITEM_SETLOVER
		char LoveName[32]="";
		if(strlen( CHAR_getChar( charaindex, CHAR_LOVE)) > 0 &&
	      strlen(CHAR_getChar(charaindex,CHAR_LOVERID))>0 &&
	      strlen(CHAR_getChar(charaindex,CHAR_LOVERNAME))>0)
			sprintf(LoveName, "§♂%s♀",CHAR_getChar( charaindex, CHAR_LOVERNAME));
#endif

		if( CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI) >= 0 
				&& CHAR_getWorkInt(charaindex, CHAR_WORKFMINDEXI) < FAMILY_MAXNUM
				&& CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER		
				&& CHAR_getWorkInt(charaindex, CHAR_WORKFMSETUPFLAG)==1
//				||CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC)
#ifdef _PLAYER_NPC 
				||CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYERNPC
#endif	
        )
#ifdef _SHOW_VIP_CF
			if(getShowVip()==1)
       	sprintf(tmp, "%s%s%s",VipName,CHAR_getChar( charaindex, CHAR_FMNAME),LoveName);
      else
#endif
#ifdef _ITEM_SETLOVER
       	sprintf(tmp, "%s%s",CHAR_getChar( charaindex, CHAR_FMNAME),LoveName);
		else
				if(strlen(LoveName)>0)
 		    	sprintf(tmp, "%s",LoveName);
 		    else
 		    	strcpy(tmp, "");
#else
				sprintf(tmp, "%s",CHAR_getChar( charaindex, CHAR_FMNAME));
		else
 		    	strcpy(tmp, "");
#endif
#ifdef _SHOW_VIP_CF
		char Name[64];
		if(getShowVip()==2)
	    	sprintf(Name, "%s%s",VipName,makeEscapeString( CHAR_getChar( charaindex,CHAR_NAME ), escapename, sizeof(escapename)));
	  else
	    	sprintf(Name, "%s",CHAR_getChar( charaindex,CHAR_NAME ));
#endif

			char szNewTemp[512];
			if(szNewName!=0 && strlen(szNewName) > 0)
				sprintf( szNewTemp, "<%s>%s", szNewName, tmp);				
			else
				sprintf( szNewTemp, "%s", tmp);
				
			
   		// Robin 0730
   		if( ridepet >= 0 ){
   			if( ! strcmp( CHAR_getChar( ridepet, CHAR_USERPETNAME), "") )
	   			strcpy( petname, CHAR_getChar( ridepet, CHAR_NAME ));
	   		else
	   			strcpy( petname, CHAR_getChar( ridepet, CHAR_USERPETNAME ));
	   		petlevel = CHAR_getInt( ridepet, CHAR_LV);
	   	}
	   	
	   	int whichtype = CHAR_getInt( charaindex, CHAR_WHICHTYPE);
#ifdef _PLAYER_NPC
			if(whichtype == CHAR_TYPEPLAYERNPC){
				whichtype = CHAR_TYPEPLAYER;
			}else{
				
			}
#endif
/*
			if(whichtype == CHAR_TYPEPLAYERNPC){
				whichtype = CHAR_TYPEPLAYER;
			}else{
				
			}
*/			
#ifdef _OBJSEND_C
  #ifdef _CHAR_PROFESSION			// WON ADD 人物职业
    #ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
		  snprintf( buf, buflen, "%d|%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d|%d|%d|%d|%d",
    #else
		  snprintf( buf, buflen, "%d|%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d|%d|%d|%d",
    #endif
  #else
		snprintf( buf, buflen, "%d|%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d",
  #endif
			OBJTYPE_CHARA,
#else
		snprintf( buf, buflen, "%d|%s|%d|%d|%d|%d|%d|%d|%s|%s|%d|%d|%d|%s|%s|%d",
#endif
				  whichtype,
				  cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
				  OBJECT_getX(objindex), 
				  OBJECT_getY(objindex),
				  CHAR_getInt( charaindex, CHAR_DIR),
				  CHAR_getInt( charaindex,CHAR_BASEIMAGENUMBER ),
				  CHAR_getInt( charaindex,CHAR_LV ),
				  namecolortbl[namecolor],
//#ifdef _NEW_SHOW_VIP_LOVE
//					FMandName,
//#else
#ifdef _SHOW_VIP_CF
	   			Name,
#else
					makeEscapeString( CHAR_getChar( charaindex,CHAR_NAME ), escapename, sizeof(escapename)),
#endif
//#endif
				  makeEscapeString(CHAR_getChar(charaindex,CHAR_OWNTITLE), escapetitle,sizeof(escapetitle)),
				  CHAR_getFlg( charaindex,CHAR_ISOVERED ),
				  CHAR_getFlg( charaindex,CHAR_HAVEHEIGHT ),
				  CHAR_getInt( charaindex, CHAR_POPUPNAMECOLOR),
				  szNewTemp,
				  makeEscapeString( petname, escapepetname, sizeof(escapepetname)),
				  petlevel
#ifdef _CHAR_PROFESSION			// WON ADD 人物职业
				  ,CHAR_getInt( charaindex, PROFESSION_CLASS)	// 职业别
				  ,CHAR_getInt( charaindex, PROFESSION_LEVEL)	// 职业等级
//				  ,CHAR_getInt( charaindex, PROFESSION_EXP)		// 职业经验值
				  ,CHAR_getInt( charaindex, PROFESSION_SKILL_POINT)	// 技能点数
#endif
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
				  ,CHAR_getInt( charaindex, CHAR_HEROFLOOR )	// 英雄战场楼层
#endif

#ifdef _PETSKILL_BECOMEPIG
    //              ,CHAR_getInt(charaindex, CHAR_BECOMEPIG)
#endif
			);
		break;
	}
	case OBJTYPE_ITEM:
	{
		int     itemindex = OBJECT_getIndex(objindex);
		int     nameint;
		int     leaklevel;
		int		color;
		char bufbuf[256];
		char *temp;
		leaklevel = ITEM_getInt( itemindex,ITEM_LEAKLEVEL );

		nameint = ITEM_SECRETNAME;
//		if( leaklevel >=1 ) {
//			nameint = ITEM_SECRETNAME;
//		}else {
//			nameint = ITEM_NAME;
//		}
		color = CHAR_COLORWHITE;
		if (ITEM_getChar( itemindex, ITEM_CDKEY) == "\0"){
			return FALSE;
			break;
		}
		temp=ITEM_getChar( itemindex, ITEM_CDKEY);
		if( temp && strlen( temp ) ) {
			color = CHAR_COLORGREEN;
		}else {
			if( ITEM_getInt( itemindex, ITEM_MERGEFLG) ) {
				color = CHAR_COLORYELLOW;
			}
		}
#ifdef _OBJSEND_C
		snprintf( buf, buflen, "%d|%s|%d|%d|%d|%d|%s",
					OBJTYPE_ITEM,
#else
		snprintf( buf, buflen, "%s|%d|%d|%d|%d|%s",
#endif
				cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
				OBJECT_getX(objindex), OBJECT_getY(objindex),
				ITEM_getInt( itemindex,ITEM_BASEIMAGENUMBER ),
				color,
				makeEscapeString( ITEM_getChar( itemindex,nameint), bufbuf, sizeof(bufbuf))
		);
		break;
	}
	case OBJTYPE_GOLD:
#ifdef _OBJSEND_C
		snprintf( buf, buflen, "%d|%s|%d|%d|%d",
					OBJTYPE_GOLD,
#else
		snprintf( buf, buflen, "%s|%d|%d|%d",
#endif
				cnv10to62( objindex,objindexbuf, sizeof(objindexbuf)),
				OBJECT_getX(objindex),OBJECT_getY(objindex),
				OBJECT_getIndex(objindex) );
		break;
#ifdef _OBJSEND_C
#endif
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

void CHAR_sendCSpecifiedObjindex( int fd, int index)
{
	char    introduction[512];	
	if( CHECKOBJECTUSE( index )
		&& CHAR_makeObjectCString( index, introduction,
								   sizeof(introduction) ) ){
		lssproto_C_send( fd,introduction );
	}else{
		char    mesg[32];
		cnv10to62( index, mesg, sizeof( mesg));
		lssproto_C_send( fd, mesg );
	}
}

void CHAR_sendSpecifiedobjindexCToCharaindex(int charaindex,int objindex)
{
	int fd;
	fd = getfdFromCharaIndex(charaindex);
	if( fd == -1)return;
	CHAR_sendCSpecifiedObjindex(fd,objindex);
}

void CHAR_sendCToArroundCharacter( int objindex )
{
	char    introduction[512];
	int     x,y,fl;
	int     i,j;

	if( CHAR_makeObjectCString( objindex,introduction, sizeof(introduction) ) == FALSE ) return;

	x = OBJECT_getX(objindex);
	y = OBJECT_getY(objindex);
	fl =OBJECT_getFloor(objindex);

	for( i = x - CHAR_DEFAULTSEESIZ/2 ;
		 i <= x +CHAR_DEFAULTSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_DEFAULTSEESIZ/2;
			 j <= y + CHAR_DEFAULTSEESIZ/2; j ++ ){
			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j);
				 object;
				 object = NEXT_OBJECT(object) ){
				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex(objindexwk);
					if( CHAR_getInt( index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
						int fd;
						if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) continue;
						fd = getfdFromCharaIndex( index);
						if( fd == -1 )continue;
						lssproto_C_send( fd,introduction );
					}
				}
			}
		}
	}
}

void CHAR_sendCDArroundChar_Main( int fl, int x, int y, int objindex, BOOL mode )
{
#define CHAR_CDSEESIZ CHAR_DEFAULTSEESIZ+10

	int     i,j;
	char    cdmsg[64*2];
	cnv10to62( objindex,cdmsg, sizeof(cdmsg));
	for( i = x - CHAR_CDSEESIZ/2 ; i <= x +CHAR_CDSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_CDSEESIZ/2; j <= y + CHAR_CDSEESIZ/2; j ++ ){
			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j); object;
				 object = NEXT_OBJECT(object) ){

				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex( objindexwk);
					if( CHAR_getInt( index,CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER) {
						if( objindexwk != objindex ) {
							int fd;
							fd = getfdFromCharaIndex(index);
							if( fd == -1 )continue;
							if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE)
								== BATTLE_CHARMODE_NONE )
							{
								CONNECT_appendCDbuf( fd, cdmsg, strlen( cdmsg));
								CDflush( index);
							}
						}
					}
				}
			}
		}
	}
}

void CHAR_sendCDArroundChar( int fl, int x, int y, int objindex )
{
	CHAR_sendCDArroundChar_Main( fl,x,y,objindex,FALSE);
}


void CHAR_sendArroundCharaData( int charaindex )
{
	int fd;
	int     i,j,x, y,fl;
	char    introduction[512*2];
	char    c_msg[5][1024*6];
	int     strpos[5] = {0,0,0,0,0};
	char	cabuf[128*2];
	fd = getfdFromCharaIndex( charaindex );
	if( fd == -1 )return;
	memset(c_msg[0], 0, sizeof(c_msg[0]));
	memset(c_msg[1], 0, sizeof(c_msg[1]));
	memset(c_msg[2], 0, sizeof(c_msg[2]));
	memset(c_msg[3], 0, sizeof(c_msg[3]));
	memset(c_msg[4], 0, sizeof(c_msg[4]));
	c_msg[0][0] = '\0';
	c_msg[1][0] = '\0';
	c_msg[2][0] = '\0';
	c_msg[3][0] = '\0';
	c_msg[4][0] = '\0';
	fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	x = CHAR_getInt( charaindex, CHAR_X);
	y = CHAR_getInt( charaindex, CHAR_Y);
							
	for( i = x-CHAR_DEFAULTSEESIZ/2 ; i <= x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j = y-CHAR_DEFAULTSEESIZ/2 ; j <= y+CHAR_DEFAULTSEESIZ/2 ;	 j ++ )	{
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ; object ; object = NEXT_OBJECT(object ) ){
				int introlen;
				int objindex = GET_OBJINDEX(object);
				int c_index = OBJECT_getIndex( objindex);

				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA &&
					!CHAR_getFlg(OBJECT_getIndex(objindex),	CHAR_ISVISIBLE) ){
					if(!CHAR_CHECKINDEX(OBJECT_getIndex(objindex))){
	            printf("自动删除一个问题对象！");
	        		endObjectOne(objindex);
	        }
					continue;
				}
				if( OBJECT_getType( objindex) == OBJTYPE_CHARA) {
					if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
#ifdef _ALLBLUES_LUA
						|| CHAR_getInt( c_index, CHAR_WHICHTYPE ) >= CHAR_TYPELUANPC
#endif
           ){
			 	   		/* 見えたキャラが仲間歩きの親ならばリーダー表示CAを送る */
						if( CHAR_getWorkInt( c_index, CHAR_WORKPARTYMODE )== CHAR_PARTY_LEADER ){
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),	CHAR_ACTLEADER,1 ))	{
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
						// Robin 0611 trade
						/* 角色交易中 */
						if( CHAR_getWorkInt( c_index, CHAR_WORKTRADEMODE ) != CHAR_TRADE_FREE )	{
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),	CHAR_ACTTRADE,1 )){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}

#ifdef _ANGEL_SUMMON
						if( CHAR_getWorkInt( c_index, CHAR_WORKANGELMODE ) == TRUE )	{
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),	CHAR_ACTANGEL,1 )){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
#endif

#ifdef _MIND_ICON
						//print("\nshan--->(batlmode)->%d", CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE ));
						if(CHAR_getWorkInt( c_index, CHAR_MIND_NUM) &&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	== BATTLE_CHARMODE_NONE){
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
														CHAR_MIND, CHAR_getWorkInt( c_index, CHAR_MIND_NUM))){
								CONNECT_appendCAbuf( fd, cabuf, strlen(cabuf));
								//print("\nshan---->(1)cabuf-->%s", cabuf);
							}
						}
#endif

#ifdef _STREET_VENDOR
						if(CHAR_getWorkInt(c_index,CHAR_WORKSTREETVENDOR) == 1 && 
							 CHAR_getWorkInt(c_index,CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
							if(CHAR_makeCAOPTString(objindex,cabuf,sizeof(cabuf),
								 CHAR_STREETVENDOR_OPEN,CHAR_getWorkChar(c_index,CHAR_STREETVENDOR_NAME))){
								CONNECT_appendCAbuf(fd,cabuf,strlen(cabuf));
							}
						}
#endif

#ifdef _ITEM_CRACKER
						if(CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER)&&
							CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	== BATTLE_CHARMODE_NONE)
						{
							if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
														CHAR_ITEM_CRACKER, CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER)))
							{
								CONNECT_appendCAbuf( fd, cabuf, strlen(cabuf));
							}
						}
#endif						
						if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )	!= BATTLE_CHARMODE_NONE ){		
							if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEWATCH) == TRUE ) {
								if( CHAR_makeCAOPT1String( objindex, cabuf,	sizeof( cabuf),	CHAR_ACTBATTLEWATCH, 1))	{
									CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
								}
							}else {
								int	battleno = CHAR_getWorkInt( c_index, CHAR_WORKBATTLEINDEX);
								int	sideno   = CHAR_getWorkInt( c_index, CHAR_WORKBATTLESIDE);
								int	helpno = FALSE;
								if( BATTLE_CHECKINDEX( battleno ) == FALSE ){
								}else	{
									helpno = (BattleArray[ battleno].Side[
										CHAR_getWorkInt( c_index, CHAR_WORKBATTLESIDE)].flg & BSIDE_FLG_HELP_OK)?TRUE:FALSE;
								}
								if( CHAR_makeCAOPT3String( objindex, cabuf,	sizeof( cabuf),	CHAR_ACTBATTLE,
															battleno,sideno,helpno)){
									CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));

								}
							}
						}
						if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 )	{
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));

							}
						}
					}else if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPET )	{
						if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 ){
							if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
											CHAR_getWorkInt( c_index, CHAR_WORKACTION))){
								CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
							}
						}
					}
#ifdef _ADD_ACTION
					else {	//npc
			
						if( CHAR_CHECKINDEX( c_index) )	{
			 	   			if( CHAR_getWorkInt( c_index, CHAR_WORKACTION ) > 0 ) {
								if( CHAR_makeCADefaultString( objindex, cabuf, sizeof( cabuf),
												CHAR_getWorkInt( c_index, CHAR_WORKACTION))) {
									CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));

								}
							}
						}
					}
#endif
				}
				if( c_index == charaindex) continue;
				if( CHAR_makeObjectCString( objindex, introduction,	sizeof( introduction))){
					introlen = strlen( introduction);
					introduction[introlen] = ',';
					introduction[introlen+1] = '\0';
					if(strpos[0]+strlen( introduction)<=5120){
						strcpysafe( &c_msg[0][strpos[0]], sizeof( c_msg[0]) - strpos[0],
									introduction);
						strpos[0] += strlen( introduction);
					}else if(strpos[1]+strlen( introduction)<=5120){
						strcpysafe( &c_msg[1][strpos[1]], sizeof( c_msg[0]) - strpos[1],
									introduction);
						strpos[1] += strlen( introduction);
					}else if(strpos[2]+strlen( introduction)<=5120){
						strcpysafe( &c_msg[2][strpos[2]], sizeof( c_msg[2]) - strpos[2],
									introduction);
						strpos[2] += strlen( introduction);
					}else if(strpos[3]+strlen( introduction)<=5120){
						strcpysafe( &c_msg[3][strpos[3]], sizeof( c_msg[3]) - strpos[3],
									introduction);
						strpos[3] += strlen( introduction);
					}else if(strpos[4]+strlen( introduction)<=5120){
						strcpysafe( &c_msg[4][strpos[4]], sizeof( c_msg[4]) - strpos[4],
									introduction);
						strpos[4] += strlen( introduction);
					}
				}
			}
		}
	}
	dchop( c_msg[0], "," );
	dchop( c_msg[1], "," );
	dchop( c_msg[2], "," );
	dchop( c_msg[3], "," );
	dchop( c_msg[4], "," );
	if( strlen( c_msg[0]) == 0 ) return;
	//print("\nc_msg[0]=%s,strpos[0]=%d\n",c_msg[0],strpos[0]);
	lssproto_C_send( fd, c_msg[0]);
	if(strlen( c_msg[1]) > 0){
		//print("\nc_msg[1]=%s,strpos[1]=%d\n",c_msg[1],strpos[1]);
		lssproto_C_send( fd, c_msg[1]);
	}
	if(strlen( c_msg[2]) > 0){
		//print("\nc_msg[2]=%s,strpos[2]=%d\n",c_msg[2],strpos[2]);
		lssproto_C_send( fd, c_msg[2]);
	}
	if(strlen( c_msg[3]) > 0){
		//print("\nc_msg[3]=%s,strpos[3]=%d\n",c_msg[3],strpos[3]);
		lssproto_C_send( fd, c_msg[3]);
	}
	if(strlen( c_msg[4]) > 0){
		//print("\nc_msg[4]=%s,strpos[4]=%d\n",c_msg[4],strpos[4]);
		lssproto_C_send( fd, c_msg[4]);
	}//修复人多关闭外挂
}

BOOL _CHAR_warpToSpecificPoint( char *file, int line, int charaindex, int floor, int x, int y)
{
	int     objindex;
	int     per;
	objindex = CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX);
	if( !MAP_IsValidCoordinate( floor, x, y )) {
//		print( "error: invalid Coordinate fl[%d] x[%d] y[%d] %s:%d from %s:%d\n", 
//					floor, x, y, __FILE__, __LINE__, file, line);
		return FALSE;
	}
	CHAR_sendCDArroundChar_Main( OBJECT_getFloor(objindex),
							OBJECT_getX(objindex),
							OBJECT_getY(objindex),
							objindex, TRUE );

	CHAR_setInt( charaindex,CHAR_FLOOR,floor);
	CHAR_setInt( charaindex,CHAR_X,x);
	CHAR_setInt( charaindex,CHAR_Y,y);
	{
		int of,ox,oy;
		of = OBJECT_setFloor(objindex,floor);
		ox = OBJECT_setX(objindex,x);
		oy = OBJECT_setY(objindex,y);
		if( !MAP_objmove( objindex,of,ox,oy,floor,x,y ) ){
			fprint( "ERROR MAP_OBJMOVE objindex=%d\n",objindex );
			return FALSE; 
		}
	}
	per = ENCOUNT_getEncountPercentMin( charaindex, floor,x,y);
	if( per != -1 ) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MIN, per);
	}
	per = ENCOUNT_getEncountPercentMax( charaindex, floor,x,y);
	if( per != -1 ) {
		CHAR_setWorkInt( charaindex, CHAR_WORKENCOUNTPROBABILITY_MAX, per);
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_FIX, 0);
#endif

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		CAflush( charaindex );
		{
			int i;
			char*   statuscat[]={"C","E",};
			for( i=0 ; i<arraysizeof(statuscat) ; i++ )
				CHAR_sendStatusString( charaindex, statuscat[i] );

			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
				CHAR_sendStatusString( charaindex, "P" );

			}
		}
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
			CHAR_sendArroundCharaData( charaindex );
		}
	}

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPET ||
		( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
		  CHAR_getInt( charaindex, CHAR_MAILMODE) == CHAR_PETMAIL_NONE)){
		CHAR_sendCToArroundCharacter( objindex );
	}
	else {
		CHAR_sendPMEToArroundCharacterFLXY( charaindex,
										CHAR_getInt( charaindex, CHAR_FLOOR),
										CHAR_getInt( charaindex, CHAR_X),
										CHAR_getInt( charaindex, CHAR_Y),
										0,1,CHAR_getInt( charaindex, CHAR_PETMAILEFFECT)
										 );
	}

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ) {
			CHAR_sendLeader( objindex, 1);
		}
		MAP_sendArroundChar( charaindex );
		CHAR_setWorkChar(charaindex,CHAR_WORKWALKARRAY,"");
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) != CHAR_PARTY_CLIENT ){
			CHAR_setFlg( charaindex, CHAR_ISWARP, 1);
		}
		CHAR_checkEffect( charaindex);
		// Robin 0706 petFollow
		{
			int petindex = CHAR_getWorkInt( charaindex, CHAR_WORKPETFOLLOW );
			if( (petindex > -1) && (CHAR_CHECKINDEX( petindex )) )
          
			CHAR_warpToSpecificPoint( petindex, floor, x, y );
		}
#ifdef _STATUS_WATERWORD //水世界状态
		if( MAP_getMapFloorType( floor) == 1 ){
			CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 1);
		}else {
			CHAR_setWorkInt( charaindex, CHAR_WORKMAPFLOORTYPE, 0);
		}
#endif
#ifdef _MAP_TIME
		// 如果进入的地图是这些编号的话,设定倒数
		if(floor >= 30017 && floor <= 30021){
			if(CHAR_getWorkInt(charaindex,CHAR_WORK_MAP_TIME) <= 0) CHAR_setWorkInt(charaindex,CHAR_WORK_MAP_TIME,60);
		}
		// 如果进入正常的地图
		else CHAR_setWorkInt(charaindex,CHAR_WORK_MAP_TIME,0);
#endif

#ifdef _ANGEL_SUMMON
		if( CHAR_getWorkInt( charaindex, CHAR_WORKANGELMODE ) == TRUE ) {
			CHAR_sendAngelMark( objindex, 1);
		}
#endif

	}else if( OBJECT_getType( objindex) == OBJTYPE_CHARA ){
		MAP_sendArroundChar( charaindex );
	}
	
	return TRUE;
}

static void CHAR_walk_check( int charaindex )
{
	if( strlen( CHAR_getWorkChar( charaindex ,CHAR_WORKWALKARRAY ) ) != 0 ){
		struct timeval old;
		old.tv_sec  = CHAR_getWorkInt(charaindex,CHAR_WORKWALKSTARTSEC);
		old.tv_usec = CHAR_getWorkInt(charaindex,CHAR_WORKWALKSTARTMSEC);
		if( time_diff_us(NowTime,old) >= CHAR_getInt(charaindex,CHAR_WALKINTERVAL)*1000.0 )	{
			CHAR_walkcall( charaindex );
		}
	}
}

static BOOL CHAR_callLoop( int charaindex )
{
	unsigned int timediff_us;
	struct timeval old;
	int     loopinterval, iRet = FALSE;
	loopinterval = CHAR_getInt(charaindex,CHAR_LOOPINTERVAL);
	if( loopinterval <= 0 )return FALSE;

	//print("\n CHAR_callLoop:%d", charaindex);

	old.tv_sec  = CHAR_getWorkInt(charaindex,CHAR_WORKLOOPSTARTSEC);
	old.tv_usec = CHAR_getWorkInt(charaindex,CHAR_WORKLOOPSTARTMSEC);
	timediff_us = time_diff_us( NowTime , old );

	if( timediff_us >= loopinterval*500.0 ){
		int (*loopfunc)(int)=NULL;
		loopfunc =(int(*)(int))CHAR_getFunctionPointer( charaindex, CHAR_LOOPFUNC);
		if( loopfunc ){
			loopfunc(charaindex);
			iRet = TRUE;
		}
#ifdef _ALLBLUES_LUA
		else{
			RunCharLoopEvent(charaindex);
			iRet = TRUE;
		}
#endif
#ifdef _USER_CHARLOOPS
		loopfunc =(int(*)(int))CHAR_getFunctionPointer( charaindex, CHAR_LOOPFUNCTEMP1);
		if( loopfunc ){
			loopfunc(charaindex);
			iRet = TRUE;
		}
#endif
		if( iRet == FALSE ){
			CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 0);
		}

		CHAR_setWorkInt(charaindex,CHAR_WORKLOOPSTARTSEC, NowTime.tv_sec);
		CHAR_setWorkInt(charaindex,CHAR_WORKLOOPSTARTMSEC, NowTime.tv_usec);
	}
	return iRet;
}

int EnemyMoveNum = 10;
void CHAR_Loop( void )
{
	int charnum=CHAR_getCharNum();
	int playernum = CHAR_getPlayerMaxNum();
	int petnum = CHAR_getPetMaxNum();
	static int  charcnt = 0;
	int i, movecnt=0;
	

	if( charcnt == -1 ) {
		charcnt = CHAR_getCharNum();
	}

	// 玩家用的loop
	for( i=0 ; i< playernum ; i++ ){
		if( CHAR_getCharUse(i) == FALSE )continue;
		CHAR_walk_check( i );
		CHAR_callLoop( i );

#ifdef _TEACHER_SYSTEM
		// 有获得导师声望
		if(CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME) > 0){
			int iAddTFame = CHAR_getInt(i,CHAR_TEACHER_FAME) + CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME);
			int iAddFame = CHAR_getInt(i,CHAR_FAME) + CHAR_getWorkInt(i,CHAR_WORK_GET_TEACHER_FAME);

			// 加导师声望(导师声望最大值和个人声望最大值一样)
			if(iAddTFame > MAX_PERSONALFAME) iAddTFame = MAX_PERSONALFAME;
			else if(iAddTFame < 0) iAddTFame = 0;
			CHAR_setInt(i,CHAR_TEACHER_FAME,iAddTFame);
			// 加声望
			if(iAddFame > MAX_PERSONALFAME) iAddFame = MAX_PERSONALFAME;
			else if(iAddFame < 0) iAddFame = 0;
			CHAR_setInt(i,CHAR_FAME,iAddFame);
			// 清为 0
			CHAR_setWorkInt(i,CHAR_WORK_GET_TEACHER_FAME,0);
			// 若有家族,上传给ac保持二边资料同步
			if(CHAR_getInt(i,CHAR_FMLEADERFLAG) > 0 && CHAR_getInt(i,CHAR_FMLEADERFLAG) != FMMEMBER_APPLY){
				char tmpbuf1[16];

				sprintf(tmpbuf1,"%d",iAddFame);
				saacproto_ACFixFMData_send(acfd,
					CHAR_getChar(i,CHAR_FMNAME),
					CHAR_getInt(i,CHAR_FMINDEX),
					CHAR_getWorkInt(i,CHAR_WORKFMINDEXI),
					FM_FIX_FMFEED,
					"0",			// 只是为了同步资料,所以设为0,不动到家族的资料
					tmpbuf1,	// 同步个人声望资料
					CHAR_getWorkInt(i,CHAR_WORKFMCHARINDEX),
					CONNECT_getFdid(getfdFromCharaIndex(i))
				);
			}
		}
#endif
	}
#ifdef _FIX_CHAR_LOOP
	static int petcnt = 0;
	static int othercnt = 0;
	for( i = 0, movecnt = 0; i < CHAR_getPetMaxNum() ; i++, petcnt++ ){
		if( petcnt >= petnum )petcnt = playernum ;
		if( CHAR_getCharUse(petcnt) == FALSE )continue;
		if( CHAR_callLoop(petcnt) == TRUE ){
			if( ++movecnt >= EnemyMoveNum )break;
		}else{
			continue;
		}
	}
	for( i = 0, movecnt = 0; i < CHAR_getOthersMaxNum() ; i++, othercnt++ ){
		if( othercnt >= charnum )othercnt = petnum ;
		if( CHAR_getCharUse(othercnt) == FALSE )continue;
		if( CHAR_callLoop(othercnt) == TRUE ){
			if( ++movecnt >= 50 )break;
		}else{
			continue;
		}
	}
#else
	// 玩家以外的Object用的loop
	for( i = 0, movecnt = 0 ; i < (petnum/2) ; i++, charcnt++ ){
		if( charcnt >= charnum )charcnt = playernum ;
		if( CHAR_getCharUse(charcnt) == FALSE )continue;
		if( CHAR_callLoop(charcnt) == TRUE ){
			if( ++movecnt >= EnemyMoveNum )break;
		}else{
			continue;
		}
	}
#endif
}

char* CHAR_appendNameAndTitle( int charaindex, char* src, char* buf,
							   int buflen )
{
	if( buflen > 0 ){
		if( CHAR_CHECKINDEX(charaindex) == FALSE )
			snprintf( buf,buflen,"%s",src );
		else{
			int titleindex = CHAR_getInt(charaindex,CHAR_INDEXOFEQTITLE);
			char indexname[128];
			strcpy(indexname,CHAR_getChar(charaindex,CHAR_NAME));
#ifdef	_NO_DAOLUAN
	if(CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)<1){
		if(CHAR_getInt(charaindex,CHAR_LV)<=getTalkLv()){
			if(CHAR_getInt(charaindex,CHAR_WHICHTYPE)==CHAR_TYPEPLAYER){
				strcpy(indexname,getTalkName());
			}
		}
	}
#endif
			if( titleindex == -1 )
				snprintf( buf,buflen, "%s：%s",
						  indexname, src );
			else
				snprintf( buf,buflen, "%s(%s)：%s",
						  indexname,
						  TITLE_makeTitleStatusString(charaindex,
													  titleindex), src );
		}
	}
	return buf;
}

void CHAR_getCoordinationDir( int dir , int x, int y ,int c,
							  int *xout , int *yout )
{
	*xout = x + CHAR_getDX(dir) * c;
	*yout = y + CHAR_getDY(dir) * c;
}

BOOL CHAR_createCharacter( int type, int floor, int x, int y,int dir,
						   int* charaindex, int* objindex, BOOL seemap )
{
	Char   ch;
	Object  ob;
	if( !CHAR_getDefaultChar( &ch,type ) )return FALSE;

	ch.data[CHAR_FLOOR] = floor;
	ch.data[CHAR_X] = x;
	ch.data[CHAR_Y] = y;
	ch.data[CHAR_DIR] = dir;

	*charaindex = CHAR_initCharOneArray( &ch );
	if( *charaindex == -1 )return FALSE;

	ob.type     =   OBJTYPE_CHARA;
	ob.index    =   *charaindex;
	ob.floor    =   floor;
	ob.x        =   x;
	ob.y        =   y;

	if( seemap && MAP_walkAble( *charaindex, floor,x,y ) == FALSE ){
		CHAR_endCharOneArray( *charaindex );
		return FALSE;
	}

	*objindex = initObjectOne( &ob );
	if( *objindex == -1 ){
		CHAR_endCharOneArray(*charaindex);
		return FALSE;
	}
	CHAR_setWorkInt( *charaindex,CHAR_WORKOBJINDEX,*objindex);

	/*  ネットワークを行う  */

	CHAR_sendWatchEvent( *objindex,CHAR_ACTSTAND,NULL,0,TRUE);

	return TRUE;
}

void CHAR_ObjectDelete( int objindex )
{
	CHAR_sendCDArroundChar( OBJECT_getFloor(objindex),
							OBJECT_getX(objindex),
							OBJECT_getY(objindex),
							objindex );
#ifdef _MAP_WARPPOINT
	if( OBJECT_getType( objindex) == OBJTYPE_WARPPOINT ){
//		print("Delete OBJTYPE_WARPPOINT endObjectOne( %d) [%d,%d,%d] \n ", objindex,
//							OBJECT_getFloor(objindex), OBJECT_getX(objindex),
//							OBJECT_getY(objindex) );
	}
#endif
	endObjectOne(objindex);
}

/*------------------------------------------------------------
 * キャラを削除する。オブジェクトも削除する。
 * ネットワークのやり残しをなくすためのもの
 * 引数
 *  charaindex      int     キャラインデッックス
 * 返り値
 *  なし
 ------------------------------------------------------------*/
void CHAR_CharaDelete( int charaindex )
{
	/*  オブジェクトを削除  */
	CHAR_ObjectDelete( CHAR_getWorkInt( charaindex,CHAR_WORKOBJINDEX ) );
	/*  キャラの削除    */
	CHAR_endCharOneArray( charaindex );
}
/*------------------------------------------------------------
 * プレイヤーの持っているペットを消す
 ------------------------------------------------------------*/
void CHAR_CharaDeleteHavePet( int charaindex)
{
	int     i;
	int     pindex;

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
		pindex = CHAR_getCharPet( charaindex, i);
		if( CHAR_CHECKINDEX( pindex)) {
			CHAR_endCharOneArray( pindex );
		}
	}
	for( i = 0; i < CHAR_MAXPOOLPETHAVE; i ++ ) {
		pindex = CHAR_getCharPoolPet( charaindex, i);
		if( CHAR_CHECKINDEX( pindex)) {
			CHAR_endCharOneArray( pindex );
		}
	}
}
/*------------------------------------------------------------
 * 喜怒哀楽などを表現する。
 * mode		int			0: 自分にはCA送らない
 *						1: 自分にもCA送る
 ------------------------------------------------------------*/
int CHAR_sendAction( int charaindex, int action, int mode)
{
	/* 送られて来たアクションとCAを一致させるテーブル */
	static int table[] = {
		CHAR_ACTATTACK,CHAR_ACTDAMAGE,CHAR_ACTDOWN,CHAR_ACTACTIONSTAND,
		CHAR_ACTACTIONWALK,CHAR_ACTSIT,CHAR_ACTHAND,CHAR_ACTPLEASURE,
		CHAR_ACTANGRY,CHAR_ACTSAD,CHAR_ACTGUARD, CHAR_ACTNOD, CHAR_ACTTHROW
	};
	if( action < 0 || action >= arraysizeof( table)) return FALSE;

	/* 周りにアクションを送信する */
	CHAR_sendWatchEvent(
			CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
			table[action],
			NULL,0,mode);

	/* アクションの登録   */
	CHAR_setWorkInt( charaindex, CHAR_WORKACTION, table[action]);

	return TRUE;
}
/*------------------------------------------------------------
 * 自分が戦闘中であるという事を送信する。
 ------------------------------------------------------------*/
void CHAR_sendBattleEffect( int charaindex, int onoff)
{
	int		opt[3];

	if( onoff == 1)	{
		opt[0] = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX);
		opt[1] = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLESIDE);
		opt[2] = (BattleArray[CHAR_getWorkInt( charaindex,
				CHAR_WORKBATTLEINDEX)].Side[
				CHAR_getWorkInt( charaindex,
				CHAR_WORKBATTLESIDE)].flg & BSIDE_FLG_HELP_OK) ? TRUE:FALSE;
	}
	else {
		opt[0] = -1;
		opt[1] = -1;
		opt[2] = 0;
	}

	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ACTBATTLE,opt,arraysizeof(opt),TRUE);
}

/*------------------------------------------------------------
 * 交易中传送讯息      add code Shan 2001/06/30
 ------------------------------------------------------------*/
void CHAR_sendTradeEffect( int charaindex, int onoff)
{
	int opt[1];

	if( onoff == 1)	{
		opt[0] = 1;
	}
	else {
		opt[0] = -1;
	}
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ACTTRADE,opt,arraysizeof(opt),TRUE);
}

#ifdef _ANGEL_SUMMON
void CHAR_sendAngelEffect( int charaindex, int onoff)
{
	int opt[1];

	if( onoff == 1)	{
		opt[0] = 1;
	}
	else {
		opt[0] = -1;
	}
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ACTANGEL,opt,arraysizeof(opt),TRUE);
}
#endif

#ifdef _MIND_ICON
void CHAR_sendMindEffect( int charaindex, int onoff)
{
	int opt[1];

	if(onoff>0){
		opt[0] = onoff;
	}
	else{
		opt[0] = 0;
	}
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_MIND, opt, arraysizeof(opt), FALSE);
}
#endif
#ifdef _ITEM_CRACKER
void CHAR_sendCrackerEffect( int charaindex, int onoff)
{
	int opt[1];
		opt[0] = onoff;
	CHAR_sendWatchEvent( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
						CHAR_ITEM_CRACKER, opt, arraysizeof(opt), FALSE);
}
#endif

void CHAR_inputUserPetName( int index , int havepetindex, char* name )
{
	int		petindex;
	//char	category[3];
	char	*mycdkey=NULL, *cdkey = NULL;
	BOOL	changenameflg = FALSE;
	
	if( !CHAR_CHECKINDEX( index ) ) return;

	petindex = CHAR_getCharPet( index, havepetindex);
	if( !CHAR_CHECKINDEX( petindex)) return;

	if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){
		CHAR_talkToCli(index, -1, "家族守护兽无法修改名字！", CHAR_COLORYELLOW);
		return;
	}

	mycdkey = CHAR_getChar( index, CHAR_CDKEY);
	cdkey = CHAR_getChar( petindex, CHAR_CDKEY);
	if( cdkey == "\0" ) {
		print( "can't get CDKEY\n");
	}else {
		if( strlen( cdkey) == 0 || strcmp( cdkey, mycdkey) == 0 ) {
			changenameflg = TRUE;
		}
	}
	if( !changenameflg) {
		CHAR_talkToCli( index, -1, "无法为宠物命名。", CHAR_COLORYELLOW );
		return;
	}

	CHAR_setChar( petindex, CHAR_USERPETNAME ,name);

	if( strlen( name ) == 0 ||
		strcmp( CHAR_getChar( petindex, CHAR_NAME), name) == 0 ){
		CHAR_setChar( petindex, CHAR_CDKEY, "");
	}else {
		CHAR_setChar( petindex, CHAR_CDKEY, mycdkey);
	}
	CHAR_send_K_StatusString( index, havepetindex, CHAR_K_STRING_USERPETNAME);
}

static void CHAR_setLuck( int charaindex)
{
	LSTIME		nowlstime;
	LSTIME		mylstime;
	struct {
		int	prob;
		int	luck;
	}lucktbl[] = {
		{ 60, 1 },		/* 40% */
		{ 30, 2 },		/* 30% */
		{ 10, 3 },		/* 20% */
		{  3, 4 },		/* 7% */
		{  0, 5 }		/* 3% */
	};

	if( !CHAR_CHECKINDEX( charaindex)) return;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;

	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
	RealTimeToLSTime( CHAR_getInt( charaindex, CHAR_LASTTIMESETLUCK), &mylstime);

	/* LS時間で１日以上経っていればCHAR_LUCKを更新する */
	if( nowlstime.day != mylstime.day || nowlstime.year != mylstime.year ) {
		int		i,r;
		r = RAND( 0, 99);
		for( i = 0; i < arraysizeof( lucktbl); i ++ ) {
			if( r >= lucktbl[i].prob ) {
				CHAR_setInt( charaindex, CHAR_LUCK, lucktbl[i].luck);
				CHAR_setInt( charaindex, CHAR_LASTTIMESETLUCK, NowTime.tv_sec);
				return;
			}
		}
	}
}
void CHAR_JoinBattle_WindowResult( int charaindex, int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);

#if 1	// 修正利用参战重复加入战斗	Robin
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) {
		CHAR_talkToCli( charaindex, -1, " 重复加入战斗! ", CHAR_COLORRED);
		print("\n 改封包!!重复加入战斗!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
		//lssproto_EN_send( fd, FALSE, 0 );
		return;
	}
#endif
	if( select != WINDOW_BUTTONTYPE_CANCEL &&
	    CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ){
		while( 1 ) {
			int		selected;
			int		bcharaindex;
			if( fd == -1 ) break;
			selected  = atoi( data)-1;

			if( !CHAR_CHECKINDEX( CONNECT_getBattlecharaindex(fd,selected))) {
				break;
			}
        	if( CHAR_getWorkInt( CONNECT_getBattlecharaindex(fd,selected),
        						CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
				break;
			}
			if( BattleArray[ CHAR_getWorkInt(
                CONNECT_getBattlecharaindex( fd,selected),
							CHAR_WORKBATTLEINDEX)].type	!= BATTLE_TYPE_P_vs_E ){
				break;
			}
			bcharaindex = CONNECT_getBattlecharaindex( fd,selected);
			if( !(BattleArray[CHAR_getWorkInt( bcharaindex,
						CHAR_WORKBATTLEINDEX)].Side[
						CHAR_getWorkInt( bcharaindex,
						CHAR_WORKBATTLESIDE)].flg
				& BSIDE_FLG_HELP_OK)){
				break;
			}
			if(NPC_Util_CharDistance( charaindex,
                                      CONNECT_getBattlecharaindex(
                                          fd,selected) )> 1){
				break;
			}
			if( BATTLE_RescueEntry( charaindex,
                                    CONNECT_getBattlecharaindex(fd,selected) )){
	        	CHAR_talkToCli( charaindex, -1, "无法参战。",
	        									CHAR_COLORYELLOW);
			}else {
				ret = TRUE;
			}

			break;
		}
	}
	if( ret == FALSE ) {
		if( fd != -1 ) {
			lssproto_EN_send( fd, FALSE, 0 );
		}
	}
}
void CHAR_JoinDuel_WindowResult( int charaindex, int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);
	/* もう一度チェックする */
	if( select != WINDOW_BUTTONTYPE_CANCEL &&
	    CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT )
	{
		/* ネストがうっとおしいので１度だけループを使う */
		while( 1 ) {
			int		selected;
			int		enemyindex;

			if( fd == -1 ) break;
			selected  = atoi( data)-1;
			/*  有効なindexか */
			if( !CHAR_CHECKINDEX( CONNECT_getDuelcharaindex(fd,selected)) ) {
				break;
			}
        	/* 戦闘中かどうか */
        	if( CHAR_getWorkInt( CONNECT_getDuelcharaindex(fd,selected),
        						CHAR_WORKBATTLEMODE)
				!= BATTLE_CHARMODE_NONE)
			{
				break;
			}
			/* duel可か */
			if( !CHAR_getFlg( CONNECT_getDuelcharaindex(fd,selected),
                              CHAR_ISDUEL))
			{
				break;
			}
			/* １歩以内にいるか(ウィンドウで問い合わせしてる間に動ける為) */
			if(NPC_Util_CharDistance( charaindex,
									CONNECT_getDuelcharaindex(fd,selected) )
				> 1)
			{
				break;
			}
			//  目の前のキャラのインデックス
			enemyindex = CONNECT_getDuelcharaindex( fd,selected);
			print("\nenemyindex=%d\n",enemyindex);
			// 相手が親ならそのままエンカウントさせるが
			// 子供なら親を呼んでくる
			if( CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYMODE )
				== CHAR_PARTY_CLIENT )
			{
				enemyindex = CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYINDEX1 );
				// なぜか親がいない
				if( enemyindex < 0 ){
					 break;
				}
			}
			/* 相手がプレイヤーでない事もある */
			if( CHAR_getInt( enemyindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER){
				break;
			}
			ret = BATTLE_CreateVsPlayer( charaindex, enemyindex );
			if( ret != 0 ){
				CHAR_talkToCli( charaindex, -1,
						"遭遇失败！", CHAR_COLORYELLOW);
				ret = FALSE;
			}
			else {
				ret = TRUE;
			}
			break;
		}
	}
	if( ret == FALSE ) {
		/* 結果送信 */
		if( fd != -1 ) {
			lssproto_EN_send( fd, FALSE, 0 );
		}
	}
}
void CHAR_SelectCard_WindowResult( int charaindex, int select, char *data)
{
	if( select != WINDOW_BUTTONTYPE_CANCEL ){
		while( 1 ) {
			int		selected;
			int		fd;
			fd = getfdFromCharaIndex( charaindex);
			if( fd == -1 ) break;
			selected  = atoi( data)-1;
			if( !CHAR_CHECKINDEX(
                CONNECT_getTradecardcharaindex(fd,selected))) {
				break;
			}
	  		if( CHAR_getWorkInt( CONNECT_getTradecardcharaindex(fd,selected),
	  							CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE){
	  			break;
	  		}
	  		if(!CHAR_getFlg( CONNECT_getTradecardcharaindex(fd,selected),
	  						CHAR_ISTRADECARD)){
	  			break;
	  		}
			if( NPC_Util_CharDistance( charaindex,
									CONNECT_getTradecardcharaindex
                                       (fd,selected) ) > 1){
				break;
			}
			ADDRESSBOOK_addAddressBook( charaindex,
                                        CONNECT_getTradecardcharaindex(
                                            fd,selected));
			break;
		}
	}
}

void CHAR_JoinParty_WindowResult( int charaindex , int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);
	/* もう一度チェックする */
	if( select != WINDOW_BUTTONTYPE_CANCEL &&
	    /* 自分がパーティ組んでたら駄目    */
	    CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE )
	{
		/* ネストがうっとおしいので１度だけループを使う */
		while( 1 ) {
			int		parray;
			int		selected;
			int		toindex = -1;
			if( fd == -1 ) break;
			selected  = atoi( data)-1;

			/* 有効なindexか
			 * (重要：ウィンドウで問い合わせしてる間に
			 *  無効なindexになる可能性がある)
			 */
			if( !CHAR_CHECKINDEX(
                CONNECT_getJoinpartycharaindex(fd,selected))) {
				break;
			}
			/* 親がいたら引っ張り出す */
			if( CHAR_getWorkInt( CONNECT_getJoinpartycharaindex(
                fd,selected),	CHAR_WORKPARTYMODE)
			== CHAR_PARTY_NONE )
			{
				toindex = CONNECT_getJoinpartycharaindex( fd,selected );
			}
			else {
				toindex = CHAR_getPartyIndex(
						CONNECT_getJoinpartycharaindex(fd,selected), 0);
				if( !CHAR_CHECKINDEX( toindex) ) {
					print( " %s:%d err\n", __FILE__, __LINE__);
					break;
				}
			}
			/* １歩以内にいるか(ウィンドウで問い合わせしてる間に動ける為) */
			if( NPC_Util_CharDistance( charaindex, toindex ) > 1) {
				break;
			}
        	/* 戦闘中はだめ */
        	if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLEMODE)
				!= BATTLE_CHARMODE_NONE)
			{
				break;
			}
			/* 仲間許可モードか */
			if( !CHAR_getFlg( toindex, CHAR_ISPARTY)) {
				break;
			}

			/* 相手パーティの人数はＯＫか？ */
			parray = CHAR_getEmptyPartyArray( toindex) ;
			if( parray == -1 ) break;
				/*パーティに入らせる */
			CHAR_JoinParty_Main( charaindex, toindex);
			ret = TRUE;
			break;
		}
	}
	if( ret == FALSE ) {
		CHAR_talkToCli( charaindex, -1, "无法加入团队。",
						CHAR_COLORYELLOW);
	}
	/* 結果送信 */
	if( fd != -1 ) {
		lssproto_PR_send( fd, 1, ret);
	}
}
void CHAR_JoinBattleWatch_WindowResult( int charaindex, int select, char *data)
{
	int	ret = FALSE;
	int	fd = getfdFromCharaIndex( charaindex);
	/* もう一度チェックする */
	if( select != WINDOW_BUTTONTYPE_CANCEL ) {
		/* ネストがうっとおしいので１度だけループを使う */
		while( 1 ) {
			int		selected;
			if( fd == -1 ) break;
			selected  = atoi( data)-1;

			/* 有効なindexか
			 * (重要：ウィンドウで問い合わせしてる間に
			 *  無効なindexになる可能性がある)
			 */
			if( !CHAR_CHECKINDEX( CONNECT_getBattlecharaindex(fd,selected))) {
				break;
			}
        	/* 戦闘中かどうか */
        	if( CHAR_getWorkInt( CONNECT_getBattlecharaindex(fd,selected),
        						CHAR_WORKBATTLEMODE)
				== BATTLE_CHARMODE_NONE)
			{
				break;
			}
			/* １歩以内にいるか(ウィンドウで問い合わせしてる間に動ける為)*/
			if(NPC_Util_CharDistance( charaindex,
                                      CONNECT_getBattlecharaindex(
                                          fd,selected) )
				> 1)
			{
				break;
			}
			/* エントリーさせる */
			if( BATTLE_WatchEntry( charaindex,
                                    CONNECT_getBattlecharaindex(fd,selected)
					 ))
			{
	        	CHAR_talkToCli( charaindex, -1, "无法观战。",
	        									CHAR_COLORYELLOW);
			}
			else {
				ret = TRUE;
			}

			break;
		}
	}
	if( ret == FALSE ) {
		/* 結果送信 */
		if( fd != -1 ) {
			lssproto_EN_send( fd, FALSE, 0 );
		}
	}
}

#ifdef _FM_MODIFY
extern void NPC_FmDengonWindowTalked( int index, int talker, int seqno, int select, char *data);
#endif

void CHAR_processWindow(int charaindex, int seqno, int select,
						int objindex, char* data )
{
	if( CHECKOBJECTUSE(objindex)) {
		if( OBJECT_getType(objindex) == OBJTYPE_CHARA ) {
			typedef void (*WINDF)(int,int,int, int,char*);
			WINDF   windowtalkedfunc=NULL;
			windowtalkedfunc = (WINDF)CHAR_getFunctionPointer(
							OBJECT_getIndex(objindex),CHAR_WINDOWTALKEDFUNC);
			if( windowtalkedfunc ) {
				windowtalkedfunc( OBJECT_getIndex(objindex), charaindex,
								seqno, select, data);
			}
#ifdef _ALLBLUES_LUA
			else{
				RunCharWindowTalked( OBJECT_getIndex(objindex), charaindex,
								seqno, select, data);
			}
#endif
		}
	}else {
		if( seqno == CHAR_WINDOWTYPE_SELECTBATTLE) {
			CHAR_JoinBattle_WindowResult( charaindex, select, data);
		}else if( seqno == CHAR_WINDOWTYPE_SELECTDUEL) {
			CHAR_JoinDuel_WindowResult( charaindex, select, data);
		}else if( seqno == CHAR_WINDOWTYPE_SELECTTRADECARD) {
			CHAR_SelectCard_WindowResult( charaindex, select, data);
		}else{
			if( seqno == CHAR_WINDOWTYPE_SELECTPARTY) {
				CHAR_JoinParty_WindowResult( charaindex, select, data);
			}else {
				if( seqno == CHAR_WINDOWTYPE_SELECTBATTLEWATCH) {
					CHAR_JoinBattleWatch_WindowResult( charaindex, select, data);
				}else if( seqno >= CHAR_WINDOWTYPE_SELECTRENAMEITEM_PAGE1 && 
						 seqno <= CHAR_WINDOWTYPE_SELECTRENAMEITEM_RENAME_ATTENTION ){
					ITEM_useRenameItem_WindowResult( charaindex, seqno, select, data);
				}
#ifdef _PETSKILL_CANNEDFOOD
				else if( seqno == ITEM_WINDOWTYPE_SELECTPETSKILL_SELECT ){
					ITEM_usePetSkillCanned_WindowResult( charaindex, seqno, select, data);
				}
#endif
#ifdef _NULL_CHECK_ITEM
		else if( seqno == CHAR_WINDOWTYPE_NULL_CHECK ){
			int value=atoi(data);
			if(value<0){
				CHAR_talkToCli( charaindex, -1, "支票面值不能小于零,如有再犯,装清空你身上所有积分!", CHAR_COLORYELLOW );
				return;
			}
			
			if(CHAR_getInt( charaindex , CHAR_AMPOINT)<value){
				CHAR_talkToCli( charaindex, -1, "你的积分点不足制作这张支票！", CHAR_COLORYELLOW );
				return;
			}else{
				char buf[256];
				int haveitemindex;
				int itemindex = CHAR_getWorkInt(charaindex, CHAR_WORKITEMINDEX);
				for( haveitemindex = 0; haveitemindex < CHAR_MAXITEMHAVE; haveitemindex ++ ) {
					if( itemindex == CHAR_getItemIndex( charaindex, haveitemindex )){
						break;
					}
				}
				if(haveitemindex < CHAR_MAXITEMHAVE){
					CHAR_setInt( charaindex , CHAR_AMPOINT, CHAR_getInt( charaindex , CHAR_AMPOINT)-value);
#ifdef _AMPOINT_LOG
					if(value>0){
						LogAmPoint(CHAR_getChar( charaindex, CHAR_NAME ),
							         	CHAR_getChar( charaindex, CHAR_CDKEY ),
							          -value,
							   				CHAR_getInt( charaindex, CHAR_AMPOINT ),
							         	"(支票制作)",
							         	CHAR_getInt( charaindex,CHAR_FLOOR),
							         	CHAR_getInt( charaindex,CHAR_X ),
							         	CHAR_getInt( charaindex,CHAR_Y ));
					}
#endif									         	


					sprintf(buf, "%d", value);
					ITEM_setChar(itemindex, ITEM_ARGUMENT, buf);
					
					sprintf( buf, "积分支票面额【%8d.00】签发人: %s", value, CHAR_getChar(charaindex, CHAR_NAME));
					ITEM_setChar( itemindex, ITEM_EFFECTSTRING, buf);
					CHAR_sendItemDataOne( charaindex, haveitemindex);
					
					sprintf( buf, "私人积分支票内已注入%d积分点！", value);
					CHAR_talkToCli( charaindex, -1, buf, CHAR_COLORYELLOW);
				}else{
					CHAR_talkToCli( charaindex, -1, "制作失败，请不要在制作支票时乱移动你身上的物品！", CHAR_COLORYELLOW);
				}
			}
		}
#endif
			}
		}
#ifdef _FM_MODIFY
		if(seqno >= CHAR_WINDOWTYPE_FM_DENGON || seqno <= CHAR_WINDOWTYPE_FM_DPSELECT)
			NPC_FmDengonWindowTalked(0,charaindex,seqno,select,data);
#endif
	}
}

void CHAR_PartyUpdate( int charaindex, int senddata ){
	int i, ownerindex, mode, pindex, pno;
	mode = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE );
	if( mode == CHAR_PARTY_NONE )return;
	if( mode == CHAR_PARTY_LEADER ){
		ownerindex = charaindex;
	}else{
		ownerindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1 );
	}

	for( pno = 0; pno < CHAR_PARTYMAX; pno ++ ){
		pindex = CHAR_getWorkInt( ownerindex, CHAR_WORKPARTYINDEX1+pno );
		if( pindex == charaindex )break;
	}
	if( pno == CHAR_PARTYMAX )return;
	for( i = 0; i < CHAR_PARTYMAX; i ++ ){
		pindex = CHAR_getWorkInt( ownerindex, CHAR_WORKPARTYINDEX1+i );
		if( pindex == charaindex )continue;
		if( pindex < 0 )continue;
		CHAR_send_N_StatusString( pindex, pno, senddata );
	}
}

static int CHAR_getObjectByPosition( int myobjindex, int fl, int x, int y,
									  int *objbuf, int objbuflen)
{
	int     i,j;
	int		cnt = 0;

	for( i = x-CHAR_DEFAULTSEESIZ/2 ; i <= x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j = y-CHAR_DEFAULTSEESIZ/2 ; j <= y+CHAR_DEFAULTSEESIZ/2 ;
			 j ++ )
		{
			OBJECT  object;
			for( object = MAP_getTopObj(fl,i,j) ;
				 object ;
				 object = NEXT_OBJECT(object ) )
			{
				int objindex = GET_OBJINDEX(object);
				if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;
				if( OBJECT_getType( objindex) == OBJTYPE_CHARA &&
					!CHAR_getFlg( OBJECT_getIndex( objindex), CHAR_ISVISIBLE)) 
				{
					if(!CHAR_CHECKINDEX(OBJECT_getIndex(objindex))){
	            printf("自动删除一个问题对象！");
	        		endObjectOne(objindex);
	        }
					continue;
				}
				/* 自分はいいよ */
				if( objindex == myobjindex) continue;

				objbuf[cnt] = objindex;
				cnt ++;
				if( cnt >= objbuflen) return cnt;
			}
		}
	}
	return cnt;
}
static void CHAR_setMyPosition_sendData( int charaindex,int prev_x, int prev_y, int x, int y)
{
	int		CurrentObjCollection[CHAR_DEFAULTSEESIZ*CHAR_DEFAULTSEESIZ*10];
	int		PrevObjCollection[CHAR_DEFAULTSEESIZ*CHAR_DEFAULTSEESIZ*10];
	int		curcnt,prevcnt;
	int		objindex;
	int		fl;
	char    introduction[512];
	char    c_msg[1024*96];
	char	buf[64];
	int     strpos;
	int		fd;
	int		i,j;
	char	cabuf[128];
	char	myintroduction[512];
	char	cd_msg[128];

	fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	if( fd == -1 )return;

	objindex = CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX);
	fl = CHAR_getInt( charaindex, CHAR_FLOOR);

	curcnt = CHAR_getObjectByPosition( objindex, fl,x,y, CurrentObjCollection, sizeof(CurrentObjCollection));
	prevcnt = CHAR_getObjectByPosition( objindex, fl, prev_x, prev_y, PrevObjCollection, sizeof(PrevObjCollection));
	strpos = 0;
	c_msg[0] = '\0';
	if( !CHAR_makeObjectCString( objindex, myintroduction,sizeof( myintroduction))){
		myintroduction[0] = '\0';
	}

	for( i = 0 ; i < curcnt; i ++ ) {
		int	findflg = FALSE;
		for( j = 0; j < prevcnt; j ++ ) {
			if( CurrentObjCollection[i] == PrevObjCollection[j] ) {
				findflg = TRUE;
				break;
			}
		}
		if( !findflg ) {
			int introlen;
			int c_index = OBJECT_getIndex( CurrentObjCollection[i]);
			if( CHAR_makeObjectCString( CurrentObjCollection[i], introduction,
											sizeof( introduction)))
			{
				introlen = strlen( introduction);
				introduction[introlen] = ',';
				introduction[introlen+1] = '\0';

				strcpysafe( &c_msg[strpos], sizeof( c_msg) - strpos,
							introduction);
				strpos += strlen( introduction);
				if( strpos >= sizeof( c_msg)) break;
			}
			if( strlen( myintroduction) != 0 ) {
				if( OBJECT_getType( CurrentObjCollection[i] ) == OBJTYPE_CHARA ) {
					if( CHAR_getInt( c_index, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
						int		tofd = getfdFromCharaIndex( c_index );
						CDsend( tofd);
						lssproto_C_send( tofd, myintroduction);
					}
				}
			}
			if( OBJECT_getType( CurrentObjCollection[i]) == OBJTYPE_CHARA ) {
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKPARTYMODE )
						== CHAR_PARTY_LEADER ){
					if( CHAR_makeCAOPT1String( CurrentObjCollection[i], cabuf, sizeof( cabuf),
												CHAR_ACTLEADER,1 )){
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}
				
				// Robin 0611 trade
				/* 角色交易中 */
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKTRADEMODE ) != CHAR_TRADE_FREE )	{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf), CHAR_ACTTRADE,1 ))
					{
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}				

#ifdef _ANGEL_SUMMON
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKANGELMODE ) == TRUE )	{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf), CHAR_ACTANGEL,1 ))
					{
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}				
#endif

#ifdef _MIND_ICON
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE &&
					CHAR_getWorkInt( c_index, CHAR_MIND_NUM))
				{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
										CHAR_MIND, CHAR_getWorkInt( c_index, CHAR_MIND_NUM)))
					{
						CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
						//print("\nshan---->(2)cabuf-->%s", cabuf);
					}
				}				
#endif
#ifdef _STREET_VENDOR
				if(CHAR_getWorkInt(c_index,CHAR_WORKSTREETVENDOR) == 1 && 
					CHAR_getWorkInt(c_index,CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
					if(CHAR_makeCAOPTString(objindex,cabuf,sizeof(cabuf),
						CHAR_STREETVENDOR_OPEN,CHAR_getWorkChar(c_index,CHAR_STREETVENDOR_NAME)))
						CONNECT_appendCAbuf(fd,cabuf,strlen(cabuf));
				}
#endif
#ifdef _ITEM_CRACKER
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE &&
					CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER))
				{
					if( CHAR_makeCAOPT1String( objindex, cabuf, sizeof( cabuf),
										CHAR_ITEM_CRACKER, CHAR_getWorkInt( c_index, CHAR_WORKITEM_CRACKER)))
					{
						CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
					}
				}				
#endif				
				/* 自分が親で相手が戦闘中でないなら相手にリーダー表示CAを送る */
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE )
						== CHAR_PARTY_LEADER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE)
						== BATTLE_CHARMODE_NONE)
				{
					int		tofd = getfdFromCharaIndex( c_index );
					if( tofd != -1 ) {
						if( CHAR_makeCAOPT1String( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX),
													cabuf, sizeof( cabuf),
													CHAR_ACTLEADER,1 ))
						{
							CONNECT_appendCAbuf( tofd,cabuf,strlen(cabuf));
						}
					}
				}
				/* 見えたキャラが戦闘中なら戦闘アイコン表示ＣＡを送る */
#ifdef _ALLBLUES_LUA 
  			if( (CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER 
  					|| CHAR_getInt( c_index, CHAR_WHICHTYPE ) >= CHAR_TYPELUANPC)
  				&& CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )!= BATTLE_CHARMODE_NONE )
#else
				if( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER &&
					CHAR_getWorkInt( c_index, CHAR_WORKBATTLEMODE )
					!= BATTLE_CHARMODE_NONE )
#endif
				{
					/*  観戦CA */
					if( CHAR_getWorkInt( c_index, CHAR_WORKBATTLEWATCH) == TRUE ) {
						if( CHAR_makeCAOPT1String( CurrentObjCollection[i], cabuf,
													sizeof( cabuf),
													CHAR_ACTBATTLEWATCH,
													1))
						{
							CONNECT_appendCAbuf( fd, cabuf, strlen( cabuf));
						}
					}
					else {
						int	battleno = CHAR_getWorkInt( c_index,
															CHAR_WORKBATTLEINDEX);
						int	sideno   = CHAR_getWorkInt( c_index,
															CHAR_WORKBATTLESIDE);
						int	helpno = (BattleArray[CHAR_getWorkInt( c_index,
								CHAR_WORKBATTLEINDEX)].Side[
								CHAR_getWorkInt( c_index,
								CHAR_WORKBATTLESIDE)].flg & BSIDE_FLG_HELP_OK) ? TRUE:FALSE;
						/*  HelpNo = 1 は仮 */
						if( CHAR_makeCAOPT3String( CurrentObjCollection[i], cabuf, sizeof( cabuf),
													CHAR_ACTBATTLE,
													battleno,sideno,helpno
													))
						{
							CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
						}
					}
				}
	 	   		/* 見えたキャラが何かのアクションをしていれば送る。 */
				if( ( CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ||
					  CHAR_getInt( c_index, CHAR_WHICHTYPE ) == CHAR_TYPEPET ) &&
					CHAR_getWorkInt( c_index, CHAR_WORKACTION ) != -1 )
				{
					if( CHAR_makeCADefaultString( CurrentObjCollection[i], cabuf, sizeof( cabuf),
									CHAR_getWorkInt( c_index, CHAR_WORKACTION)))
					{
						CONNECT_appendCAbuf( fd,cabuf,strlen(cabuf));
					}
				}
			}
		}
	}
	dchop( c_msg, "," );
	if( strlen( c_msg) != 0 ) {
		lssproto_C_send( fd, c_msg);
	}

	/* 自分のCD作成 */
	cnv10to62( objindex,cd_msg, sizeof(cd_msg));

	for( i = 0 ; i < prevcnt; i ++ ) {
		int	findflg = FALSE;
		/* 前の位置にいるオブジェクトで，現在の座標にいない
		 * オブジェクトなら消すべきオブジェクトだ
		 */
		for( j = 0; j < curcnt; j ++ ) {
			if( PrevObjCollection[i] == CurrentObjCollection[j] ) {
				findflg = TRUE;
				break;
			}
		}
		if( !findflg ) {
			cnv10to62( PrevObjCollection[i], buf, sizeof( buf));
			CONNECT_appendCDbuf( fd, buf, strlen( buf));
			
			/* 相手にもＣＤ送る */
			if( OBJECT_getType( PrevObjCollection[i]) == OBJTYPE_CHARA ) {
				int tocharaindex = OBJECT_getIndex( PrevObjCollection[i]);
				if( CHAR_getInt( tocharaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
					int tofd = CHAR_getWorkInt( tocharaindex, CHAR_WORKFD);
					if( tofd != -1 ) {
						CONNECT_appendCDbuf( tofd, cd_msg, strlen( cd_msg));
						CDsend( tofd);
					}
				}
			}
		}
	}
	CDsend( fd);


}
BOOL CHAR_setMyPosition( int index, int x, int y, BOOL CAFlg)
{
    return CHAR_setMyPosition_main( index,x,y,-1,CAFlg);
}

BOOL CHAR_setMyPosition_main( int index, int x, int y, int setdir, BOOL CAFlg)
{
	int     objindex;
	int		prev_x,prev_y;
	int		fl;

	prev_x = CHAR_getInt( index, CHAR_X);
	prev_y = CHAR_getInt( index, CHAR_Y);
	if( prev_x == x && prev_y == y ) {
		return FALSE;
	}

	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT) {
		return FALSE;
	}

	if( CHAR_getFlg( index, CHAR_ISWARP)) return FALSE;

	objindex = CHAR_getWorkInt(index,CHAR_WORKOBJINDEX);
	fl = CHAR_getInt( index, CHAR_FLOOR);

	if( !MAP_IsValidCoordinate( fl, x, y )) {
		print( "error: invalid Coordinate fl[%d] x[%d] y[%d] %s:%d\n", 
					fl, x, y, __FILE__, __LINE__);
                CONNECT_setCloseRequest( CHAR_getWorkInt( index, CHAR_WORKFD), 1);
		return FALSE;
	}

	if( ABS( x - prev_x) > CHAR_DEFAULTSEESIZ/2 ||
		ABS( y - prev_y) > CHAR_DEFAULTSEESIZ/2 )
	{
		CHAR_sendCDArroundChar( fl,prev_x,prev_y,objindex );
	}

	if( ABS( x - prev_x) <= 1 && ABS( y - prev_y ) <= 1){
		POINT start,end;
		int	dir;
		start.x = CHAR_getInt( index, CHAR_X);
		start.y = CHAR_getInt( index, CHAR_Y);
		end.x = x;
		end.y = y;
		dir = NPC_Util_getDirFromTwoPoint( &start, &end);
		if( dir != -1 ) {
			CHAR_walk( index, dir, 0);
		}
	}
	else {
		CHAR_setInt( index, CHAR_X,x);
		CHAR_setInt( index, CHAR_Y,y);
        if( setdir >= 0 && setdir < 8) CHAR_setInt( index, CHAR_DIR, setdir);
		{
			int of,ox,oy;
			of = OBJECT_setFloor(objindex,fl);
			ox = OBJECT_setX(objindex,x);
			oy = OBJECT_setY(objindex,y);

			if( !MAP_objmove( objindex,of,ox,oy,fl,x,y ) ){
				fprint( "ERROR MAP_OBJMOVE objindex=%d\n",objindex );
				return FALSE;
			}
		}
		if( ABS( x - prev_x) > CHAR_DEFAULTSEESIZ/2 ||
			ABS( y - prev_y) > CHAR_DEFAULTSEESIZ/2 )
		{
			CHAR_sendWatchEvent(objindex,CHAR_ACTWARP,NULL,0,FALSE);
		}
		else if( CAFlg == TRUE ) {
			CHAR_sendWatchEvent(objindex,CHAR_ACTWARP,NULL,0,FALSE);
		}
	}
	
	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) {
		if( ABS( x - prev_x) <= 1 && ABS( y - prev_y ) <= 1){
			int		i;
			int prevparty_x = prev_x;
			int prevparty_y = prev_y;
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int toindex = CHAR_getWorkInt( index, i + CHAR_WORKPARTYINDEX1);
				if( CHAR_CHECKINDEX(toindex) ) {
					int client_x = prevparty_x;
					int client_y = prevparty_y;
					int	dir;
					POINT	start, end;

					prevparty_x = CHAR_getInt( toindex, CHAR_X);
					prevparty_y = CHAR_getInt( toindex, CHAR_Y);
					start.x = prevparty_x;
					start.y = prevparty_y;

					end.x = client_x;
					end.y = client_y;

					dir = NPC_Util_getDirFromTwoPoint( &start, &end);
					if( dir != - 1 ) {
						CHAR_walk( toindex, dir, 0);
					}
				}
			}
		}

		else if( ABS( x - prev_x) > CHAR_DEFAULTSEESIZ/2 ||
				ABS( y - prev_y) > CHAR_DEFAULTSEESIZ/2 )
		{
			int i;
			for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
				int toindex = CHAR_getWorkInt( index, i + CHAR_WORKPARTYINDEX1);
				if( CHAR_CHECKINDEX(toindex) ) {
					CHAR_warpToSpecificPoint( toindex,fl , x,y);
				}
			}
		}
	}

	if( ABS( x - prev_x) <= CHAR_DEFAULTSEESIZ/2 &&
		ABS( y - prev_y) <= CHAR_DEFAULTSEESIZ/2 )
	{
		CHAR_setMyPosition_sendData( index, prev_x, prev_y, x, y);
	}

	return TRUE;
}


static char CHAR_P_statusSendBuffer[STRINGBUFSIZ];



static char *CHAR_make_P_StatusString( int charaindex, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_P_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_P_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_P_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_P_STRING_MAXMP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_P_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_P_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_P_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_P_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_P_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_P_STRING_CHARM, 	CHAR_WORKFIXCHARM,		1 },
		{ CHAR_P_STRING_LUCK, 	CHAR_WORKFIXLUCK,		1 },
		{ CHAR_P_STRING_GOLD, 	CHAR_GOLD,				0 },
		{ CHAR_P_STRING_TITLE, 	CHAR_INDEXOFEQTITLE,	0 },
		{ CHAR_P_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_P_STRING_OWNTITLE, CHAR_OWNTITLE,			2 },
		{ CHAR_P_STRING_VITAL, 	CHAR_VITAL,				10 },
		{ CHAR_P_STRING_STR, 	CHAR_STR,				10 },
		{ CHAR_P_STRING_TOUGH, 	CHAR_TOUGH,				10 },
		{ CHAR_P_STRING_DEX, 	CHAR_DEX,				10 },
		{ CHAR_P_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11 },
		{ CHAR_P_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11 },
		{ CHAR_P_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11 },
		{ CHAR_P_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11 },
		{ CHAR_P_STRING_DUELPOINT, 	CHAR_DUELPOINT,		0 },
		{ CHAR_P_STRING_TRANSMIGRATION, CHAR_TRANSMIGRATION, 0},
		{ CHAR_P_STRING_RIDEPET, CHAR_RIDEPET, 0},
		{ CHAR_P_STRING_LEARNRIDE, CHAR_LEARNRIDE, 0},
		{ CHAR_P_STRING_BASEBASEIMAGENUMBER, CHAR_BASEBASEIMAGENUMBER, 0},
		{ CHAR_P_STRING_SKYWALKER, CHAR_WORKSKYWALKER, 1},
		{ CHAR_P_STRING_DEBUGMODE, CHAR_WORKDEBUGMODE, 1},
	};
	

	snprintf( CHAR_P_statusSendBuffer, sizeof( CHAR_P_statusSendBuffer),
				"P%s|", cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_P_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( charaindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( charaindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( charaindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}

					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_P_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( charaindex, CHAR_getInt( charaindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
			}
			if( !found) {
				print( "找不到指定的参数[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_P_statusSendBuffer[ strlength],
							sizeof( CHAR_P_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_P_statusSendBuffer ) ) {
					return CHAR_P_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_P_statusSendBuffer;
}

static char *CHAR_make_P_StatusString_Point( int charaindex, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	
	int vippoint = sasql_vippoint(CHAR_getChar(charaindex,CHAR_CDKEY),0,0);
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_P_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_P_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_P_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_P_STRING_MAXMP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_P_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_P_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_P_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_P_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_P_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_P_STRING_CHARM, 	CHAR_WORKFIXCHARM,		1 },
		{ CHAR_P_STRING_LUCK, 	CHAR_WORKFIXLUCK,		1 },
		{ CHAR_P_STRING_GOLD, 	vippoint,				3 },
		{ CHAR_P_STRING_TITLE, 	CHAR_INDEXOFEQTITLE,	0 },
		{ CHAR_P_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_P_STRING_OWNTITLE, CHAR_OWNTITLE,			2 },
		{ CHAR_P_STRING_VITAL, 	CHAR_VITAL,				10 },
		{ CHAR_P_STRING_STR, 	CHAR_STR,				10 },
		{ CHAR_P_STRING_TOUGH, 	CHAR_TOUGH,				10 },
		{ CHAR_P_STRING_DEX, 	CHAR_DEX,				10 },
		{ CHAR_P_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11 },
		{ CHAR_P_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11 },
		{ CHAR_P_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11 },
		{ CHAR_P_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11 },
		{ CHAR_P_STRING_DUELPOINT, 	CHAR_DUELPOINT,		0 },
		{ CHAR_P_STRING_TRANSMIGRATION, CHAR_TRANSMIGRATION, 0},
		{ CHAR_P_STRING_RIDEPET, CHAR_RIDEPET, 0},
		{ CHAR_P_STRING_LEARNRIDE, CHAR_LEARNRIDE, 0},
		{ CHAR_P_STRING_BASEBASEIMAGENUMBER, CHAR_BASEBASEIMAGENUMBER, 0},
		{ CHAR_P_STRING_SKYWALKER, CHAR_WORKSKYWALKER, 1},
		{ CHAR_P_STRING_DEBUGMODE, CHAR_WORKDEBUGMODE, 1},
	};

	snprintf( CHAR_P_statusSendBuffer, sizeof( CHAR_P_statusSendBuffer),
				"P%s|", cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_P_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 3 ) {
						ret = vippoint;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( charaindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( charaindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( charaindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}

					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_P_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( charaindex, CHAR_getInt( charaindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
			}
			if( !found) {
				print( "找不到指定的参数[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_P_statusSendBuffer[ strlength],
							sizeof( CHAR_P_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_P_statusSendBuffer ) ) {
					return CHAR_P_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_P_statusSendBuffer;
}

static char *CHAR_make_P_StatusString_Fame( int charaindex, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_P_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_P_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_P_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_P_STRING_MAXMP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_P_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_P_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_P_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_P_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_P_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_P_STRING_CHARM, 	CHAR_WORKFIXCHARM,		1 },
		{ CHAR_P_STRING_LUCK, 	CHAR_WORKFIXLUCK,		1 },
		{ CHAR_P_STRING_GOLD, 	CHAR_FAME,				12 },
		{ CHAR_P_STRING_TITLE, 	CHAR_INDEXOFEQTITLE,	0 },
		{ CHAR_P_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_P_STRING_OWNTITLE, CHAR_OWNTITLE,			2 },
		{ CHAR_P_STRING_VITAL, 	CHAR_VITAL,				10 },
		{ CHAR_P_STRING_STR, 	CHAR_STR,				10 },
		{ CHAR_P_STRING_TOUGH, 	CHAR_TOUGH,				10 },
		{ CHAR_P_STRING_DEX, 	CHAR_DEX,				10 },
		{ CHAR_P_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11 },
		{ CHAR_P_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11 },
		{ CHAR_P_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11 },
		{ CHAR_P_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11 },
		{ CHAR_P_STRING_DUELPOINT, 	CHAR_DUELPOINT,		0 },
		{ CHAR_P_STRING_TRANSMIGRATION, CHAR_TRANSMIGRATION, 0},
		{ CHAR_P_STRING_RIDEPET, CHAR_RIDEPET, 0},
		{ CHAR_P_STRING_LEARNRIDE, CHAR_LEARNRIDE, 0},
		{ CHAR_P_STRING_BASEBASEIMAGENUMBER, CHAR_BASEBASEIMAGENUMBER, 0},
		{ CHAR_P_STRING_SKYWALKER, CHAR_WORKSKYWALKER, 1},
		{ CHAR_P_STRING_DEBUGMODE, CHAR_WORKDEBUGMODE, 1},
	};
	
	snprintf( CHAR_P_statusSendBuffer, sizeof( CHAR_P_statusSendBuffer),
				"P%s|", cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_P_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( charaindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( charaindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 12 ) {
						ret = (int)(CHAR_getInt( charaindex, chk[j].intdataindex)/100);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( charaindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}

					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_P_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( charaindex, CHAR_getInt( charaindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
			}
			if( !found) {
				print( "找不到指定的参数[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_P_statusSendBuffer[ strlength],
							sizeof( CHAR_P_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_P_statusSendBuffer ) ) {
					return CHAR_P_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_P_statusSendBuffer;
}


static char *CHAR_make_P_StatusString_AMPOINT( int charaindex, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	int ampoint = sasql_ampoint(CHAR_getChar(charaindex,CHAR_CDKEY),0,0);
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_P_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_P_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_P_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_P_STRING_MAXMP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_P_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_P_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_P_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_P_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_P_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_P_STRING_CHARM, 	CHAR_WORKFIXCHARM,		1 },
		{ CHAR_P_STRING_LUCK, 	CHAR_WORKFIXLUCK,		1 },
		{ CHAR_P_STRING_GOLD, 	ampoint,				3 },
		{ CHAR_P_STRING_TITLE, 	CHAR_INDEXOFEQTITLE,	0 },
		{ CHAR_P_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_P_STRING_OWNTITLE, CHAR_OWNTITLE,			2 },
		{ CHAR_P_STRING_VITAL, 	CHAR_VITAL,				10 },
		{ CHAR_P_STRING_STR, 	CHAR_STR,				10 },
		{ CHAR_P_STRING_TOUGH, 	CHAR_TOUGH,				10 },
		{ CHAR_P_STRING_DEX, 	CHAR_DEX,				10 },
		{ CHAR_P_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11 },
		{ CHAR_P_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11 },
		{ CHAR_P_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11 },
		{ CHAR_P_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11 },
		{ CHAR_P_STRING_DUELPOINT, 	CHAR_DUELPOINT,		0 },
		{ CHAR_P_STRING_TRANSMIGRATION, CHAR_TRANSMIGRATION, 0},
		{ CHAR_P_STRING_RIDEPET, CHAR_RIDEPET, 0},
		{ CHAR_P_STRING_LEARNRIDE, CHAR_LEARNRIDE, 0},
		{ CHAR_P_STRING_BASEBASEIMAGENUMBER, CHAR_BASEBASEIMAGENUMBER, 0},
		{ CHAR_P_STRING_SKYWALKER, CHAR_WORKSKYWALKER, 1},
		{ CHAR_P_STRING_DEBUGMODE, CHAR_WORKDEBUGMODE, 1},
	};
	
	snprintf( CHAR_P_statusSendBuffer, sizeof( CHAR_P_statusSendBuffer),
				"P%s|", cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_P_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 3 ) {
						ret = ampoint;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( charaindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( charaindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( charaindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}

					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_P_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( charaindex, CHAR_getInt( charaindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
			}
			if( !found) {
				print( "找不到指定的参数[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_P_statusSendBuffer[ strlength],
							sizeof( CHAR_P_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_P_statusSendBuffer ) ) {
					return CHAR_P_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_P_statusSendBuffer;
}

#ifdef	_VIGOR_SYS
static char *CHAR_make_P_StatusString_VIGOR( int charaindex, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_P_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_P_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_P_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_P_STRING_MAXMP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_P_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_P_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_P_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_P_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_P_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_P_STRING_CHARM, 	CHAR_WORKFIXCHARM,		1 },
		{ CHAR_P_STRING_LUCK, 	CHAR_WORKFIXLUCK,		1 },
		{ CHAR_P_STRING_GOLD, 	CHAR_VIGOR,				0 },
		{ CHAR_P_STRING_TITLE, 	CHAR_INDEXOFEQTITLE,	0 },
		{ CHAR_P_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_P_STRING_OWNTITLE, CHAR_OWNTITLE,			2 },
		{ CHAR_P_STRING_VITAL, 	CHAR_VITAL,				10 },
		{ CHAR_P_STRING_STR, 	CHAR_STR,				10 },
		{ CHAR_P_STRING_TOUGH, 	CHAR_TOUGH,				10 },
		{ CHAR_P_STRING_DEX, 	CHAR_DEX,				10 },
		{ CHAR_P_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11 },
		{ CHAR_P_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11 },
		{ CHAR_P_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11 },
		{ CHAR_P_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11 },
		{ CHAR_P_STRING_DUELPOINT, 	CHAR_DUELPOINT,		0 },
		{ CHAR_P_STRING_TRANSMIGRATION, CHAR_TRANSMIGRATION, 0},
		{ CHAR_P_STRING_RIDEPET, CHAR_RIDEPET, 0},
		{ CHAR_P_STRING_LEARNRIDE, CHAR_LEARNRIDE, 0},
		{ CHAR_P_STRING_BASEBASEIMAGENUMBER, CHAR_BASEBASEIMAGENUMBER, 0},
		{ CHAR_P_STRING_SKYWALKER, CHAR_WORKSKYWALKER, 1},
		{ CHAR_P_STRING_DEBUGMODE, CHAR_WORKDEBUGMODE, 1},
	};
	
	snprintf( CHAR_P_statusSendBuffer, sizeof( CHAR_P_statusSendBuffer),
				"P%s|", cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_P_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( charaindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( charaindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( charaindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( charaindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}

					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_P_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( charaindex, CHAR_getInt( charaindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
			}
			if( !found) {
				print( "找不到指定的参数[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_P_statusSendBuffer[ strlength],
							sizeof( CHAR_P_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_P_statusSendBuffer ) ) {
					return CHAR_P_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_P_statusSendBuffer;
}
#endif
BOOL CHAR_send_P_StatusString( int charaindex, unsigned int indextable )
{
	char*   string;
	//if( indextable >= ( CHAR_P_STRING_BASEBASEIMAGENUMBER << 1)) {
	if( indextable >= ( CHAR_P_STRING_SKYWALKER << 1)) {
	//if( indextable >= ( CHAR_P_STRING_DEBUGMODE << 1)) {
		print( "send_P invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}

	string = CHAR_make_P_StatusString( charaindex, indextable );

	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CHAR_send_P_StatusString_Point( int charaindex, unsigned int indextable )
{
	char*   string;
	//if( indextable >= ( CHAR_P_STRING_BASEBASEIMAGENUMBER << 1)) {
	if( indextable >= ( CHAR_P_STRING_SKYWALKER << 1)) {
	//if( indextable >= ( CHAR_P_STRING_DEBUGMODE << 1)) {
		print( "send_P invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}

	string = CHAR_make_P_StatusString_Point( charaindex, indextable );

	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CHAR_send_P_StatusString_Fame( int charaindex, unsigned int indextable )
{
	char*   string;
	//if( indextable >= ( CHAR_P_STRING_BASEBASEIMAGENUMBER << 1)) {
	if( indextable >= ( CHAR_P_STRING_SKYWALKER << 1)) {
	//if( indextable >= ( CHAR_P_STRING_DEBUGMODE << 1)) {
		print( "send_P invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}

	string = CHAR_make_P_StatusString_Fame( charaindex, indextable );

	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CHAR_send_P_StatusString_AMPOINT( int charaindex, unsigned int indextable )
{
	char*   string;
	//if( indextable >= ( CHAR_P_STRING_BASEBASEIMAGENUMBER << 1)) {
	if( indextable >= ( CHAR_P_STRING_SKYWALKER << 1)) {
	//if( indextable >= ( CHAR_P_STRING_DEBUGMODE << 1)) {
		print( "send_P invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}

	string = CHAR_make_P_StatusString_AMPOINT( charaindex, indextable );

	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}
#ifdef	_VIGOR_SYS
BOOL CHAR_send_P_StatusString_VIGOR( int charaindex, unsigned int indextable )
{
	char*   string;
	//if( indextable >= ( CHAR_P_STRING_BASEBASEIMAGENUMBER << 1)) {
	if( indextable >= ( CHAR_P_STRING_SKYWALKER << 1)) {
	//if( indextable >= ( CHAR_P_STRING_DEBUGMODE << 1)) {
		print( "send_P invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}

	string = CHAR_make_P_StatusString_VIGOR( charaindex, indextable );

	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}
#endif
static char CHAR_N_statusSendBuffer[STRINGBUFSIZ];
/*------------------------------------------------------------
 * クライアントに送るＳ　Ｐの文字列を作成する。
 * 引数
 *  charaindex      int     キャラインデクッス
 *  indextable		int 	P を送るパラメータの集合体。
 *------------------------------------------------------------*/
static char *CHAR_make_N_StatusString( int charaindex, int num, unsigned int indextable )
{
	int		i,j;
	int 	nindex = -1;
	int		ret = 0;
	int		strlength;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_N_STRING_OBJINDEX, 	CHAR_WORKOBJINDEX,	1 },
		{ CHAR_N_STRING_LV,		CHAR_LV, 				0 },
		{ CHAR_N_STRING_MAXHP, 	CHAR_WORKMAXHP,			1 },
		{ CHAR_N_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_N_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_N_STRING_NAME, 	CHAR_NAME,				2 },


	};
	/*  仲間ステータスデータ    */
	/*  最大最小チェック */
	if( num < 0 || num >= CHAR_PARTYMAX ){
		print( "朋友模式失败 (%c)%d \n", num, num );
		return "\0";
	}
	/*  仲間のインデックスを取得   */
	nindex = CHAR_getPartyIndex( charaindex, num);

	if( nindex == -1 ){
		/*  最悪やけど snprintf する    */
		snprintf( CHAR_N_statusSendBuffer,sizeof(CHAR_N_statusSendBuffer),
			 "N%d|0|", num );
		return  CHAR_N_statusSendBuffer;
	}

	snprintf( CHAR_N_statusSendBuffer, sizeof( CHAR_N_statusSendBuffer),
				"N%d|%s|", num,cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_N_statusSendBuffer );

	/* indextableの中で立っているビットを順番に調べる */
	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			/*   立っているビットと，chkにヒットするか調べる */
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					/* 一致したので値を取得する */
					/* 数値系のデータ */
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( nindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( nindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( nindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( nindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					/* 文字列系のデータ*/
					else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( nindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}
					found = TRUE;
					break;
				}
			}
			/* 他のタイプのデータかもしれない */
			if( !found) {
				/* ここからは１つ１つ  */
			}
			if( !found) {
				print( "找不到指定的参数[%d]\n",indextable);
			}
			else {
				strcpysafe( &CHAR_N_statusSendBuffer[ strlength],
							sizeof( CHAR_N_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_N_statusSendBuffer ) ) {
					return CHAR_N_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_N_statusSendBuffer;
}
/*------------------------------------------------------------
 * S P ステータスを送る。チェックするので、上ではチェックしなくてよい。
 * 引数
 *  charaindex      int     キャラインデクッス
 *  num				int		仲間番号
 *  indextable		int 	P を送るパラメータの集合体。
 * 返り値
 *  送った。TRUE(1)
 *  送れなかった。FALSE(0)
 *------------------------------------------------------------*/
BOOL CHAR_send_N_StatusString( int charaindex, int num, unsigned int indextable )
{
	char*   string;

	if( indextable >= (CHAR_N_STRING_NAME << 1)) {
		print( "send_N invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}
	string = CHAR_make_N_StatusString( charaindex, num, indextable );
	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}

static char CHAR_K_statusSendBuffer[STRINGBUFSIZ];
static char *CHAR_make_K_StatusString( int charaindex, int num, unsigned int indextable )
{
	int		i,j;
	int		ret = 0;
	int		strlength;
	int 	pindex;
	char	buf[64];
	struct {
		unsigned int		kind;
		int		intdataindex;
		int		gettype;
	}chk[] = {
		{ CHAR_K_STRING_BASEIMAGENUMBER, 	CHAR_BASEIMAGENUMBER,	0 },
		{ CHAR_K_STRING_HP, 	CHAR_HP,				0 },
		{ CHAR_K_STRING_MAXHP,	CHAR_WORKMAXHP, 		1 },
		{ CHAR_K_STRING_MP, 	CHAR_MP,				0 },
		{ CHAR_K_STRING_MAXHP, 	CHAR_WORKMAXMP,			1 },
		{ CHAR_K_STRING_EXP, 	CHAR_EXP,				0 },
		{ CHAR_K_STRING_LV, 	CHAR_LV,				0 },
		{ CHAR_K_STRING_ATK, 	CHAR_WORKATTACKPOWER,	1 },
		{ CHAR_K_STRING_DEF, 	CHAR_WORKDEFENCEPOWER,	1 },
		{ CHAR_K_STRING_QUICK, 	CHAR_WORKQUICK,			1 },
		{ CHAR_K_STRING_AI, 	CHAR_WORKFIXAI,			1 },
		{ CHAR_K_STRING_EARTH, 	CHAR_WORKFIXEARTHAT,	11},
		{ CHAR_K_STRING_WATER, 	CHAR_WORKFIXWATERAT,	11},
		{ CHAR_K_STRING_FIRE, 	CHAR_WORKFIXFIREAT,		11},
		{ CHAR_K_STRING_WIND, 	CHAR_WORKFIXWINDAT,		11},
		{ CHAR_K_STRING_SLOT,	CHAR_SLOT,				0 },
		{ CHAR_K_STRING_NAME, 	CHAR_NAME,				2 },
		{ CHAR_K_STRING_USERPETNAME, CHAR_USERPETNAME,  2 },

	};
	/*  最大最小チェック */
	if( num < 0 || num >= CHAR_MAXPETHAVE ){
		print( "宠物模式失败 (%c)%d \n", num, num );
		return "\0";
	}
	/* ペットのインデックスを取得   */
	pindex = CHAR_getCharPet( charaindex, num );
	if( pindex == -1 ){
		/*    最悪やけど snprintf する    */
		snprintf(CHAR_K_statusSendBuffer,sizeof(CHAR_K_statusSendBuffer),
			 "K%d|0|", num );
		return  CHAR_K_statusSendBuffer;
	}

	snprintf( CHAR_K_statusSendBuffer, sizeof( CHAR_K_statusSendBuffer),
				"K%d|%s|", num, cnv10to62( indextable, buf,sizeof(buf)));
	strlength = strlen( CHAR_K_statusSendBuffer );

	for( i = 1; i < sizeof( int) * 8; i ++ ){
		if( indextable & (1 << i) ) {
			char	tmp[256];
			BOOL	found = FALSE;
			for( j = 0; j < arraysizeof( chk); j ++ ) {
				if( chk[j].kind ==  1 << i ) {
					if( chk[j].gettype == 0 ) {
						ret = CHAR_getInt( pindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 1 ) {
						ret = CHAR_getWorkInt( pindex,chk[j].intdataindex);
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 10 ) {
						ret = CHAR_getInt( pindex, chk[j].intdataindex)/100;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					else if( chk[j].gettype == 11 ) {
						ret = CHAR_getWorkInt( pindex, chk[j].intdataindex);
						if( ret < 0) ret = 0;
						snprintf( tmp, sizeof( tmp), "%d|", ret);
					}
					/*  文字列系のデータ */
					else if( chk[j].gettype == 2 ) {
						char    escapebuffer[128];
						char *a = CHAR_getChar( pindex, chk[j].intdataindex);

						snprintf( tmp,sizeof( tmp ),
								  "%s|",
								  makeEscapeString( a, escapebuffer,
								  					sizeof(escapebuffer)
								));
					}
					found = TRUE;
					break;
				}
			}
			if( !found) {
				if( indextable & CHAR_K_STRING_NEXTEXP ) {
					ret = CHAR_GetLevelExp( pindex, CHAR_getInt( pindex, CHAR_LV ) + 1);
					snprintf( tmp, sizeof( tmp), "%d|", ret);
					found = TRUE;
				}
				if( indextable & CHAR_K_STRING_CHANGENAMEFLG ) {
					BOOL	changenameflg = FALSE;
					char	*mycdkey, *cdkey = NULL;
					mycdkey = CHAR_getChar( charaindex, CHAR_CDKEY);
					{
						cdkey = CHAR_getChar( pindex, CHAR_CDKEY);
						if( cdkey == "\0" ) {
							print( "can't get CDKEY\n");
						}else {
							if( strlen( cdkey) == 0 ||
								strcmp( cdkey, mycdkey) == 0 ){
								changenameflg = TRUE;
							}
						}
					}
					snprintf( tmp, sizeof( tmp), "%d|", changenameflg);
					found = TRUE;
				}
			}
			if( !found) {
				print( "找不到指定的参数[%d]\n",indextable);
			}else {
				strcpysafe( &CHAR_K_statusSendBuffer[ strlength],
							sizeof( CHAR_K_statusSendBuffer) - strlength,
							tmp);

				strlength += strlen( tmp );
				if( strlength >= sizeof( CHAR_K_statusSendBuffer ) ) {
					return CHAR_K_statusSendBuffer;
				}
			}
		}
	}
	return CHAR_K_statusSendBuffer;
}
/*------------------------------------------------------------
 * S K ステータスを送る。チェックするので、上ではチェックしなくてよい。
 * 引数
 *  charaindex      int     キャラインデクッス
 *  indextable		int 	P を送るパラメータの集合体。
 * 返り値
 *  送った。TRUE(1)
 *  送れなかった。FALSE(0)
 *------------------------------------------------------------*/
BOOL CHAR_send_K_StatusString( int charaindex, int num, unsigned int indextable )
{
	char*   string;

	if( indextable >= (CHAR_K_STRING_USERPETNAME << 1)) {
		print( "send_K invalid index[%d]\n", indextable);
		return FALSE;
	}

	if( CHAR_getInt(charaindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER ){
		return FALSE;
	}
	string = CHAR_make_K_StatusString( charaindex, num, indextable );
	if( string != NULL ){
		int     fd;
		fd = getfdFromCharaIndex( charaindex );
		if( fd != -1 ){
			lssproto_S_send( fd, string );
			return TRUE;
		}
	}
	return FALSE;
}



//------------------------------------------------------------
// プレイヤーの魅力を加算
//------------------------------------------------------------
void CHAR_AddCharm( int charaindex, int iValue )
{
	int iWork;
	iWork = CHAR_getInt( charaindex, CHAR_CHARM ) + iValue;
	iWork = max( 0, iWork );	// 最小０
	iWork = min( 100, iWork );	// 最大１００
	CHAR_setInt( charaindex, CHAR_CHARM, iWork );
}

//------------------------------------------------------------
// ペットの忠誠度変化値を加算する
//------------------------------------------------------------
void CHAR_PetAddVariableAi( int petindex, int iValue )
{
	int iWork;
	iWork = CHAR_getInt( petindex, CHAR_VARIABLEAI ) + iValue;
	iWork = min( CHAR_MAXVARIABLEAI, iWork );
	iWork = max( CHAR_MINVARIABLEAI, iWork );
	CHAR_setInt( petindex, CHAR_VARIABLEAI, iWork );

}

//------------------------------------------------------------
// データベースに登録するキー（名前）を作成する。
//------------------------------------------------------------
int CHAR_makeDBKey( int charaindex, char *pszBuffer, int size ){

	char *cdkey;
	char buff[512];
	char escapebuf[1024];

	pszBuffer[0] = 0;	// 初期化
	cdkey = CHAR_getChar( charaindex, CHAR_CDKEY);
	if( cdkey == "\0" )return FALSE; //登録できない
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;

	// キーは cdkey_名前
	snprintf( buff, sizeof(buff),
		"%s_%s", cdkey, CHAR_getChar( charaindex, CHAR_NAME ) );
	makeEscapeString( buff, escapebuf, sizeof(escapebuf));
	strcpysafe( pszBuffer, size, escapebuf);

	return TRUE;
}


//------------------------------------------------------------
//
//  データベースの項目を更新
//
// DuelPoint の DBUpdateEntryを送る
//------------------------------------------------------------
BOOL CHAR_send_DpDBUpdate( int charaindex )
{
	int fd,dp;
	char szKey[256];
	if( CHAR_makeDBKey(  charaindex, szKey, sizeof( szKey ) ) == FALSE ){
		return FALSE;
	}
	dp = CHAR_getInt( charaindex, CHAR_DUELPOINT );
	fd = getfdFromCharaIndex( charaindex);
	if( fd != -1 ) {
		char info[512];
		//char escapebuff[1024];
		/* "等级｜几战｜几胜｜几败｜几连胜｜最高连胜数" */
		snprintf( info, sizeof( info),
					"%d,%d,%d,%d,%d,%d",
					CHAR_getInt( charaindex, CHAR_LV),
					CHAR_getInt( charaindex, CHAR_DUELBATTLECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELLOSECOUNT),
					CHAR_getInt( charaindex, CHAR_DUELSTWINCOUNT),
					CHAR_getInt( charaindex, CHAR_DUELMAXSTWINCOUNT));
		saacproto_DBUpdateEntryInt_send(
			acfd, DB_DUELPOINT, szKey, max(dp,0), info,
			CONNECT_getFdid( fd ) ,0 );
	}

	return TRUE;
}

char *CHAR_getUseName( int charaindex )
{
	char *pName=NULL;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
		pName = CHAR_getChar( charaindex, CHAR_USERPETNAME );
		if( pName[0] == 0 ){
			pName = CHAR_getChar( charaindex, CHAR_NAME );
		}
	}else{
//		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )
//			pName = CHAR_getChar( charaindex, CHAR_OWNTITLE );
//		if( pName == "\0" || strlen(pName) <= 0 )
			pName = CHAR_getChar( charaindex, CHAR_NAME );
	}
	return pName;
}

// Robin extend_of_CHAR_getUseName() 2001/02/15  
char *CHAR_getUseID( int charaindex )
{
	char *pName;
	// ペットだったらフリーネームにする
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
		pName = "NO_USER";
	}else{
		pName = CHAR_getChar( charaindex, CHAR_CDKEY );
	}
	return pName;
}

BOOL CHAR_send_DpDBUpdate_AddressBook( int charaindex, int mode )
{
	int fd;
	char szKey[256];
	if( CHAR_makeDBKey(  charaindex, szKey, sizeof( szKey ) ) == FALSE ){
		return FALSE;
	}

	fd = getfdFromCharaIndex( charaindex);
	if( fd != -1 ) {
		char msgbuf[512];
		int mytrans = CHAR_getInt( charaindex, CHAR_TRANSMIGRATION);
#ifdef	_ASSA_TOP		
		if(mytrans>6) mytrans = 0;
#endif			
		snprintf( msgbuf, sizeof( msgbuf),
					"%d|%d|%d|%d|%d|",
					(mode == 0 ) ? 0 : getServernumber(),
					CHAR_getInt( charaindex, CHAR_LV),
					CHAR_getInt( charaindex, CHAR_DUELPOINT),
					CHAR_getInt( charaindex, CHAR_FACEIMAGENUMBER),
					mytrans );

		saacproto_DBUpdateEntryString_send(
			acfd, DB_ADDRESSBOOK, szKey, msgbuf, CONNECT_getFdid( fd ),0 );

		saacproto_Broadcast_send( acfd, CHAR_getChar( charaindex, CHAR_CDKEY),
									CHAR_getChar( charaindex, CHAR_NAME), "param", 0);

	}
	return TRUE;
}

void CHAR_sendBattleWatch( int objindex, int onoff)
{
	int		opt[1];
	opt[0] = onoff;
	CHAR_sendWatchEvent( objindex,CHAR_ACTBATTLEWATCH,opt,1,TRUE);
}
/*------------------------------------------------------------
 * 指定された物を廻りの人にペットメールの演出をする。
 * 引数
 * charaindex        int     chara でのインデクッス
 * 返り値
 *  なし
 ------------------------------------------------------------*/
void CHAR_sendPMEToArroundCharacterFLXY( int petindex, int fl, int x, int y,
										int dir, int flg, int no )
{
	int     i,j;
	int		objindex;
	int		graphicsno;
	char	buff[2048];

	objindex = CHAR_getWorkInt( petindex, CHAR_WORKOBJINDEX);
	graphicsno = CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER);
	if( CHAR_makeObjectCString( objindex, buff, sizeof( buff)) == FALSE ) {
		buff[0] = '\0';
	}

	for( i = x - CHAR_DEFAULTSEESIZ/2 ;
		 i <= x +CHAR_DEFAULTSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_DEFAULTSEESIZ/2;
			 j <= y + CHAR_DEFAULTSEESIZ/2; j ++ ){

			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j);
				 object;
				 object = NEXT_OBJECT(object) ){
				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex(objindexwk);
					if( CHAR_getInt( index,CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
						int fd;
						if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE)
							!= BATTLE_CHARMODE_NONE) continue;
						fd = getfdFromCharaIndex( index);
						if( fd == -1 )continue;
						lssproto_PME_send( fd, objindex, graphicsno,
											x,y, dir,flg,no,buff
						 );
					}
				}
			}
		}
	}
}
/*------------------------------------------------------------
 * charaindex で指定された物を廻りの人にペットメールの演出をする。
 * 引数
 * charaindex        int     chara でのインデクッス
 * 返り値
 *  なし
 ------------------------------------------------------------*/
void CHAR_sendPMEToArroundCharacter( int charaindex, int petindex, int flg, int no )
{
    int dirx[9],diry[9];
    int		i;
    int		fl,x,y;
	int		dir;
    for( i  = 0 ; i < 7 ; i  ++ ){
        dirx[i+2] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
        diry[i+2] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
    }
    dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
    diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
    dirx[1] = 0;
    diry[1] = 0;

    fl = CHAR_getInt( charaindex,CHAR_FLOOR );
	dir = CHAR_getInt( charaindex, CHAR_DIR);

    /* 置く方向取得  */
    for( i = 0 ; i < 9 ; i ++ ){
        int x = CHAR_getInt( charaindex, CHAR_X) + dirx[i];
        int y = CHAR_getInt( charaindex, CHAR_Y) + diry[i];
        /* ペットを置ける場所か調べる */
        if( PET_isPutPoint( fl, x, y ) == TRUE ) {
            break;
        }
    }
    /* なければ足元*/
    if( i == 9 ) i = 1;

    x = CHAR_getInt(charaindex,CHAR_X)+dirx[i];
    y = CHAR_getInt(charaindex,CHAR_Y)+diry[i];

	CHAR_sendPMEToArroundCharacterFLXY( petindex,
										fl, x, y, dir, flg, no);
}
/*------------------------------------------------------------
 * SEを鳴らす。
 * 発信元を中心に，見える範囲分送信する
 * 引数
 * fl,x,y 発信元の座標
 * senumber ＳＥの番号。sndcnf.hを参照のこと。
 * sw 鳴らすか止めるか 0 : 止める 1 : 鳴らす
 ------------------------------------------------------------*/
void CHAR_sendSEoArroundCharacter( int fl, int x, int y, int senumber, int sw )
{
	int     i,j;

	for( i = x - CHAR_DEFAULTSEESIZ/2 ;
		 i <= x +CHAR_DEFAULTSEESIZ/2 ; i ++ ){
		for( j = y - CHAR_DEFAULTSEESIZ/2;
			 j <= y + CHAR_DEFAULTSEESIZ/2; j ++ ){

			OBJECT  object;
			for( object = MAP_getTopObj( fl, i,j);
				 object;
				 object = NEXT_OBJECT(object) ){
				int objindexwk = GET_OBJINDEX( object);
				if( OBJECT_getType(objindexwk) == OBJTYPE_CHARA ){
					int index = OBJECT_getIndex(objindexwk);
					if( CHAR_getInt( index,CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
						int fd;
						if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE)
							!= BATTLE_CHARMODE_NONE) continue;
						fd = getfdFromCharaIndex( index);
						if( fd == -1 )continue;
						lssproto_SE_send( fd,x, y, senumber, sw );
					}
				}
			}
		}
	}
}
/*====================演出設定====================*/


#ifndef _SEND_EFFECT	   	        // WON ADD AC送下雪、下雨等特效

#define		CHAR_EFFECT_SETTINGBUFFER	256
typedef struct tagCHAR_effectsetting
{
    int     floor;								//	フロア
	int		effect;								//	演出番号
	int		level;								//	演出のレベル。表現の強さ。
	int		sendflg;							//	表示エフェクトを送ったかどうか。
	char	month[CHAR_EFFECT_SETTINGBUFFER];	//	実行する月
	char	day[CHAR_EFFECT_SETTINGBUFFER];		//	実行する日
	char	hour[CHAR_EFFECT_SETTINGBUFFER];	//	実行する時間
	char	min[CHAR_EFFECT_SETTINGBUFFER];		//	実行する分
	char	expire[CHAR_EFFECT_SETTINGBUFFER];	//	実行している長さ。(秒）
	
}CHAR_effectsetting;

CHAR_effectsetting*    CHAR_effect;
int                     CHAR_effectnum;

#endif



/*------------------------------------------------------------
 * 演出設定の初期化をする。
 * 引数
 *  filename        char*       設定ファイル名
 * 返り値
 *  成功    TRUE(1)
 *  失敗    FALSE(0)
 *------------------------------------------------------------*/
BOOL CHAR_initEffectSetting( char* filename )
{
    FILE*   f;
    char    line[256];
    int     linenum=0;

    int effectreadlen=0;

    f = fopen(filename,"r");
    if( f == NULL ){
        errorprint;
        return FALSE;
    }

    CHAR_effectnum=0;

    /*  まず有効な行が何行あるかどうか調べる     */
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        CHAR_effectnum++;
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

    CHAR_effect = allocateMemory( sizeof(CHAR_effectsetting)
                                   * CHAR_effectnum );
    if( CHAR_effect == NULL ){
        fprint( "Can't allocate Memory %d\n" ,
                sizeof(CHAR_effectsetting)*CHAR_effectnum);
        fclose( f );
        return FALSE;
    }
    /* 初期化 */
{
    int     i;
    for( i = 0; i < CHAR_effectnum; i ++ ) {
        CHAR_effect[i].floor = 0;
        CHAR_effect[i].effect = 0;
        CHAR_effect[i].level = 0;
		CHAR_effect[i].sendflg = 0;
        CHAR_effect[i].month[0] = '\0';
        CHAR_effect[i].day[0] = '\0';
        CHAR_effect[i].hour[0] = '\0';
        CHAR_effect[i].min[0] = '\0';
        CHAR_effect[i].expire[0] = '\0';
		
    }
}

    /*  また読み直す    */
    linenum = 0;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;        /* comment */
        if( line[0] == '\n' )continue;       /* none    */
        chomp( line );

        /*  行を整形する    */
        /*  まず tab を " " に置き換える    */
        replaceString( line, '\t' , ' ' );
        /*   連続した " " をひとつにする */
        deleteSequentChar( line, " " );

        /*  ここまで整形して、先頭が スペースだったら */
        /*  読まない    */
        if( line[0] == ' ' )continue;

        {
            char    token[256];
            int     ret;

            /*  ひとつめのトークンを見る    */
            ret = getStringFromIndexWithDelim( line," ",1,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }
            CHAR_effect[effectreadlen].floor = atoi(token);

            /*  2つめのトークンを見る     */
            ret = getStringFromIndexWithDelim( line," ",2,token,
                                               sizeof(token));
            if( ret==FALSE ){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }

            CHAR_effect[effectreadlen].effect = atoi( token );

            /*   3つめのトークンを見る    */
            ret = getStringFromIndexWithDelim( line," ",3,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }

            CHAR_effect[effectreadlen].level = atoi( token );
            
            /*  4つめのトークンを見る    */
            ret = getStringFromIndexWithDelim( line," ",4,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].month, 
						sizeof( CHAR_effect[effectreadlen].month),
						token); 
            
            /*  5つめのトークンを見る     */
            ret = getStringFromIndexWithDelim( line," ",5,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].day, 
						sizeof( CHAR_effect[effectreadlen].day),
						token); 
            
            /*   6つめのトークンを見る    */
            ret = getStringFromIndexWithDelim( line," ",6,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].hour, 
						sizeof( CHAR_effect[effectreadlen].hour),
						token); 

            /*  7つめのトークンを見る   */
            ret = getStringFromIndexWithDelim( line," ",7,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].min, 
						sizeof( CHAR_effect[effectreadlen].min),
						token); 
            
            /*  8つめのトークンを見る    */
            ret = getStringFromIndexWithDelim( line," ",8,token,
                                               sizeof(token));
            if( ret ==FALSE){
                fprint("文件秩序错误:%s 第%d行\n",filename,linenum);
                continue;
            }
			strcpysafe( CHAR_effect[effectreadlen].expire, 
						sizeof( CHAR_effect[effectreadlen].expire),
						token); 

            effectreadlen ++;
        }
    }
    fclose(f);

    CHAR_effectnum = effectreadlen;

    print( "有效设置总数 %d...", CHAR_effectnum );
#ifdef DEBUG

    {
        int i;
        for( i=0; i <CHAR_effectnum ; i++ )
            print( "effect fl[%d] effect[%d] level[%d] month[%d] day[%d] hour[%d] min[%d] expire[%d]\n",
                   CHAR_effect[i].floor,
                   CHAR_effect[i].x,
                   CHAR_effect[i].y,
                   CHAR_effect[i].month,
                   CHAR_effect[i].day,
                   CHAR_effect[i].hour,
                   CHAR_effect[i].min,
                   CHAR_effect[i].expire
                    );
    }
#endif
    return TRUE;
}

//-------------------------------------------------------------------------
//	現在時刻が，指定の条件にマッチしているかを調べます。
//	合っていればTRUEを返す。
//-------------------------------------------------------------------------
static BOOL CHAR_checkEffectTime( int num)
{
	BOOL returnflg = FALSE;
	struct	tm	t;
	
	memcpy( &t, localtime( &NowTime.tv_sec), sizeof( struct tm));
	
	while( 1 ) {
		int	i;
		int ret;
		char token[256];
		
		//	"*"は，全てを指す。
		
		//月が一致しているか
		if( strcmp( CHAR_effect[num].month, "*" ) != 0 ) {
			BOOL flg = FALSE;
			for( i = 1; ; i ++) {
	            ret = getStringFromIndexWithDelim( CHAR_effect[num].month, 
	            									",", i, 
	            									token, sizeof( token));
	            if( ret == FALSE ) break;
				//	１つでも一致していればそれでいい
				if( t.tm_mon +1 == atoi( token)) {
					flg = TRUE;
					break;
				}
			}
			if( !flg ) break;
		}
		
		//   日が一致しているか
		if( strcmp( CHAR_effect[num].day, "*" ) != 0 ) {
			BOOL flg = FALSE;
			for( i = 1; ; i ++) {
	            ret = getStringFromIndexWithDelim( CHAR_effect[num].day, 
	            									",", i, 
	            									token, sizeof( token));
	            if( ret == FALSE ) break;
				//	１つでも一致していればそれでいい
				if( t.tm_mday == atoi( token)) {
					flg = TRUE;
					break;
				}
			}
			if( !flg ) break;
		}
		// 時間が一致しているか
		if( strcmp( CHAR_effect[num].hour, "*" ) != 0 ) {
			BOOL flg = FALSE;
			for( i = 1; ; i ++) {
	            ret = getStringFromIndexWithDelim( CHAR_effect[num].hour, 
	            									",", i, 
	            									token, sizeof( token));
	            if( ret == FALSE ) break;
				//	１つでも一致していればそれでいい
				if( t.tm_hour == atoi( token)) {
					flg = TRUE;
					break;
				}
			}
			if( !flg ) break;
		}
		// 分が一致しているか
		//	ちょっとだけ特殊処理
		if( strcmp( CHAR_effect[num].min, "*" ) != 0 ) {
			// 期間が＊ならもうOK
			if( strcmp( CHAR_effect[num].expire, "*" ) != 0 ) {
				BOOL flg = FALSE;
				for( i = 1; ; i ++) {
		            struct tm tm_work;
		            time_t tim;
		            ret = getStringFromIndexWithDelim( CHAR_effect[num].min, 
		            									",", i, 
		            									token, sizeof( token));
		            if( ret == FALSE ) break;
					//	１つでも一致していればそれでいい

					//	設定した分から，有効期間の範囲に入っているか？

					tm_work = t;
					tm_work.tm_sec = 0;
					tm_work.tm_min = atoi( token);
					tim = mktime( &tm_work);
					
					if( NowTime.tv_sec > tim && NowTime.tv_sec  < tim+atoi(CHAR_effect[num].expire)) {
						flg = TRUE;
						break;
					}
				}
				if( !flg ) break;
			}
		}
		// 分が指定無しだったらもうOK
		
		returnflg = TRUE;
		break;
	}
	return returnflg;
}
//-------------------------------------------------------------------------
//	キャラクターの位置と時間を元に，
//	エフェクトを送る必要があれば送る
//-------------------------------------------------------------------------
void CHAR_checkEffect( int charaindex)
{
	int	i;
	int fl = CHAR_getInt( charaindex, CHAR_FLOOR);
	int fd = getfdFromCharaIndex( charaindex);
	
	// エフェクト一旦消す
	lssproto_EF_send( fd, 0, 0, "");
	CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, 0);

	for( i = 0; i < CHAR_effectnum; i ++){
		//	フロアが一致した時だけ
		if( CHAR_effect[i].floor == fl) {
			// 条件を調べます。
			if( CHAR_checkEffectTime( i)){
				// エフェクトを送る
				lssproto_EF_send( fd, CHAR_effect[i].effect, CHAR_effect[i].level, "");
				if( CHAR_effect[i].level == 0 ) {
					CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, 
									CHAR_getWorkInt( charaindex, CHAR_WORKEFFECT) &
										~CHAR_effect[i].effect);
				}
				else {
					CHAR_setWorkInt( charaindex, CHAR_WORKEFFECT, 
									CHAR_getWorkInt( charaindex, CHAR_WORKEFFECT) | 
										CHAR_effect[i].effect);
				}
			}
		}
	}
}

void CHAR_checkEffectLoop( void)
{
	int	i,j;
	int     playernum = CHAR_getPlayerMaxNum();
	int		player;
	int		efnum = 0;

	struct {
		int floor;
		int effect;
		BOOL on;
		BOOL off;
	} ef[2048];

	memset( &ef, 0, sizeof( ef));

	for( i = 0; i < CHAR_effectnum; i ++){

		if( CHAR_checkEffectTime( i)){
			if( !CHAR_effect[i].sendflg) {

				for( player = 0 ; player < playernum; player ++ ) {
					if( CHAR_getInt( player, CHAR_FLOOR) == CHAR_effect[i].floor ) {
						int fd = getfdFromCharaIndex( player);
						lssproto_EF_send( fd, CHAR_effect[i].effect, CHAR_effect[i].level, "");
						if( CHAR_effect[i].level == 0 ) {
							CHAR_setWorkInt( player, CHAR_WORKEFFECT, 
											CHAR_getWorkInt( player, CHAR_WORKEFFECT) &
												~CHAR_effect[i].effect);
						}
						else {					
							CHAR_setWorkInt( player, CHAR_WORKEFFECT, 
											CHAR_getWorkInt( player, CHAR_WORKEFFECT) | 
												CHAR_effect[i].effect);
						}
					}
				}
				CHAR_effect[i].sendflg = TRUE;
				
			}

			for( j = 0; j < efnum; j ++ ) {
				if( ef[j].floor == CHAR_effect[i].floor && 
					ef[j].effect == CHAR_effect[i].effect) 
				{
					ef[j].on = TRUE;
					break;
				}
			}
			if( j == efnum ) {
				ef[j].floor = CHAR_effect[i].floor;
				ef[j].effect = CHAR_effect[i].effect;
				ef[j].on = TRUE;
				efnum++;
				if( efnum >= arraysizeof( ef)) {
					print( "err buffer over %s:%d\n", __FILE__, __LINE__);
					efnum --;
				}
			}
		}
		else {
			if( CHAR_effect[i].sendflg) {

				for( j = 0; j < efnum; j ++ ) {
					if( ef[j].floor == CHAR_effect[i].floor && 
						ef[j].effect == CHAR_effect[i].effect) 
					{
						ef[j].off = TRUE;
						break;
					}
				}
				if( j == efnum ) {
					ef[j].floor = CHAR_effect[i].floor;
					ef[j].effect = CHAR_effect[i].effect;
					ef[j].off = TRUE;
					efnum++;
					if( efnum >= arraysizeof( ef)) {
						print( "err buffer over %s:%d\n", __FILE__, __LINE__);
						efnum --;
					}
				}
				CHAR_effect[i].sendflg = FALSE;
			}
		}
	}

	for( i = 0; i < efnum; i ++ ) {
		if( ef[i].on == FALSE && ef[i].off == TRUE ) {

			for( player = 0 ; player < playernum; player ++ ) {
				if( CHAR_getInt( player, CHAR_FLOOR) == ef[i].floor ) {
					int fd = getfdFromCharaIndex( player);
					lssproto_EF_send( fd, 0, 0, "");
					CHAR_setWorkInt( player, CHAR_WORKEFFECT, 0);
				}
			}
		}
	}
}

float GetRecoveryRate( int charaindex ){
	float fBai = 1.0;

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		fBai = 1.0 + 0.00010 * CHAR_getInt( charaindex, CHAR_VITAL );
	}else{

		fBai = 1.0 + 0.00005 * CHAR_getInt( charaindex, CHAR_VITAL );
	}
	return fBai;

}


#define CHARDATASIZE (1024*256)
#define SPACESTRING "|"

static int makeSaveCharString( char *out , int outlen ,
                               char *nm , char *opt , char *info )
{
    char nmwork[256];
    char optwork[512];
    char infowork[CHARDATASIZE];
    char outwork[CHARDATASIZE];
    char *nmwork_p , *optwork_p , *infowork_p;
    int l;
    
    strncpy( nmwork , nm , sizeof( nmwork ));
    nmwork[strlen( nm)]=0;

    strncpy( optwork , opt , sizeof( optwork ));
    optwork[strlen(opt)]=0;

    strncpy( infowork , info , sizeof( infowork ));
    infowork[strlen(info)]=0;

    nmwork_p = makeEscapeString( nm , nmwork ,sizeof( nmwork ));
    optwork_p = makeEscapeString( opt , optwork , sizeof( optwork ));
    infowork_p = makeEscapeString( info , infowork , sizeof( infowork ));

    snprintf( outwork ,sizeof( outwork ) ,
              "%s" SPACESTRING "%s" SPACESTRING "%s" ,
              nmwork_p , optwork_p , infowork_p );

    l = strlen( outwork );
	outwork[ strlen( outwork)+1] = 0;
    if( l >= (outlen-1)){
        return -1;
    }

    memcpy( out , outwork , l + 1 );
    
    return 0;
}

#ifdef _STORECHAR
int storeCharaData( void ){
	int i, charamax;
	FILE	*fp;
	struct tm *pLtime;
	char szFileName[256], *chardata;
	char outbuff[CHARDATASIZE];
	Char*   ch;

	print("\n保存运行中的数据");
	
	pLtime = localtime( &NowTime.tv_sec );

	charamax = getPlayercharnum();

	for( i = 0; i < charamax; i ++ ){
		int hash, dir, j;
		char charId[32];
		char pathname[128];
		
		if( CHAR_getCharUse( i ) == FALSE )continue;
#ifdef _ALLBLUES_LUA_1_4
		RunCharLogOutEvent(i);
#endif
		print(".");
		
		strcpy( charId, CHAR_getChar( i, CHAR_CDKEY ) );
//		print("账号:%s", charId);
		
		hash = 0;
		for( j=0; j<strlen(charId); j++) {
			hash += (int)charId[j];
			hash = hash % 256;
		}

//		sprintf( pathname, "%s/char/0x%x", getStoredir(), hash);
		sprintf( pathname, "%s/0x%x", getStoredir(), hash);
//		print("文件路径:%s\n", pathname);
		dir = mkdir( pathname, -1);
		
		if( dir != 0 && errno != EEXIST )
			continue;
		//print("dir:%d\n", dir);

		sprintf( szFileName,
			"%s/%s.%d.char",
			pathname,
			CHAR_getChar( i, CHAR_CDKEY ),	// ID
			CHAR_getInt( i, CHAR_SAVEINDEXNUMBER )
		);
		
//		print("\n存储:%s\n", szFileName);

		fp = fopen( szFileName, "w" );
		if( fp == NULL )continue;

		ch = CHAR_getCharPointer( i );
		if( !ch )continue;

		chardata = CHAR_makeStringFromCharData( ch );

		if( makeSaveCharString( outbuff , sizeof( outbuff ),
			 CHAR_getChar( i, CHAR_NAME ),
			 CHAR_makeOptionString( ch ), 
			 chardata ) == 0 
		){

			fprintf( fp, outbuff );
		}else{
//			fprintf( fp, "本□皮撩  \n" );
		}

		fclose( fp );
#ifdef _CHAR_POOLITEM
	{
		if( !CHAR_CheckDepotItem( i) ) continue; //仓库未存在
		sprintf( szFileName, "%s/%s.item", pathname, CHAR_getChar( i, CHAR_CDKEY ));
		if( (fp=fopen( szFileName, "w" )) == NULL )continue;
//		print(" 存储:%s\n", szFileName);
		chardata = CHAR_makeDepotItemFromCharIndex( i);
		fprintf( fp, chardata );
		fclose( fp );
		CHAR_removeDepotItem( i);
	}
#endif

#ifdef _CHAR_POOLPET
	{
		if( !CHAR_CheckDepotPet( i) ) continue; //仓库未存在
		sprintf( szFileName, "%s/%s.pet", pathname, CHAR_getChar( i, CHAR_CDKEY ));
		if( (fp=fopen( szFileName, "w" )) == NULL )continue;
//		print(" 存储:%s\n", szFileName);
		chardata = CHAR_makeDepotPetFromCharIndex( i);
		fprintf( fp, chardata );
		fclose( fp );
		CHAR_removeDepotPet( i);
	}
#endif
	}

	/*
	if( execlp( getStorechar(), "" ) == -1 ) {
		print( " run %s error!:%d\n", getStorechar(), errno );
	}else {
		print( " run %s\n", getStorechar());
	}
	*/
	print("\n");
	return 0;
}

#else

int storeCharaData( void ){
	int i, charamax;
	FILE	*fp;
	struct tm *pLtime;
	char szFileName[256], *chardata;
	char outbuff[CHARDATASIZE];
	Char*   ch;

	print(" run_storeCharaData ");
	
	pLtime = localtime( &NowTime.tv_sec );
	charamax = getFdnum();

	for( i = 0; i < charamax; i ++ ){

		if( CHAR_getCharUse( i ) == FALSE )continue;

		sprintf( szFileName, 
			"%s/%s_%d%02d%02d_%02d%02d", 
			getStoredir(),
			CHAR_getChar( i, CHAR_CDKEY ),	// ID
			pLtime->tm_year+1900,
			pLtime->tm_mon+1,
			pLtime->tm_mday,
			pLtime->tm_hour,
			pLtime->tm_min
		);

		fp = fopen( szFileName, "w" );
		if( fp == NULL )continue;

		ch = CHAR_getCharPointer( i );
		if( !ch )continue;

		chardata = CHAR_makeStringFromCharData( ch );

		if( makeSaveCharString( outbuff , sizeof( outbuff ),
			 CHAR_getChar( i, CHAR_NAME ),
			 CHAR_makeOptionString( ch ), 
			 chardata ) == 0 
		){

			fprintf( fp, outbuff );
		}else{
			fprintf( fp, "セーブ失敗  \n" );
		}

		fclose( fp );
	}
	return 0;
}

#endif


#ifdef _FIX_METAMORIDE
BOOL CHAR_CHECKJOINENEMY( int index)
{
	int masterindex=-1;
	if( CHAR_getWorkInt( index, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT ) {
		masterindex = CHAR_getWorkInt( index, CHAR_WORKPARTYINDEX1);
		if( !CHAR_CHECKINDEX( masterindex) ) {
			print( "err masterindex:%d %s:%d !!\n", masterindex,__FILE__, __LINE__);
			return FALSE;
		}
		if( CHAR_getInt( masterindex, CHAR_WHICHTYPE) == CHAR_TYPEBUS) {
			return TRUE;	//JOIN ENEMY
		}
	}

	return FALSE;
}
#endif

 // WON ADD 修正道具的设定问题
void fix_item_bug(int charaindex, int itemindex)
{

	if (ITEM_CHECKINDEX(itemindex)){		
		switch( ITEM_getInt(itemindex,ITEM_ID) ){
			case 19700:	ITEM_setChar( itemindex, ITEM_ARGUMENT, "KPET:901_902_903_904"); break; // 鞭炮
			case 20242:	ITEM_setChar( itemindex, ITEM_ARGUMENT, "KPET:957_958_959_960"); break; // 雄黄酒
			case 1292:  ITEM_setInt( itemindex, ITEM_MAXDAMAGECRUSHE, 0); ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, 0); break; // 灵力铠
			case 19646: ITEM_setInt( itemindex, ITEM_MAXDAMAGECRUSHE, 0); ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, 0); break; // 灵力铠
			case 1478:	ITEM_setChar( itemindex, ITEM_ARGUMENT, "HP:30");	break;  // 洛克服
			case 20282: ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "攻+25 伤+25 敏-4 魅+5");	ITEM_setInt( itemindex, ITEM_MODIFYQUICK, -4 );		break;  // 铁枪1
			case 20283: ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "攻+50 伤+50 敏-10 魅+10");   ITEM_setInt( itemindex, ITEM_MODIFYQUICK, -10 );	break;  // 铁枪2
			case 20284: ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "攻+100 伤+100 敏-20 魅+15"); ITEM_setInt( itemindex, ITEM_MODIFYQUICK, -20 );	break;  // 铁枪3
			case 1210:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "攻 +4 防 -4 敏 +4 魅 +2");		break;  // 碧玉的石手环
			case 1203:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "攻 +2 防 +2 敏 -2 魅 +1");		break;  // 皮的手环(红)
			case 1204:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "攻 -2 防 +2 敏 +2 魅 +1");		break;  // 皮的手环(绿)
			case 1205:  ITEM_setChar( itemindex, ITEM_EFFECTSTRING, "攻 +2 防 -2 敏 +2 魅 +1");		break;  // 皮的手环(青)
			case 1483:  ITEM_setInt( itemindex, ITEM_MODIFYCHARM, 5);	break;  // 库沙达号角
			case 20147: ITEM_setChar(itemindex,ITEM_INGNAME0,"木");	ITEM_setChar(itemindex,ITEM_INGNAME1,"骨");	ITEM_setInt(itemindex,ITEM_INGVALUE0, 1125 );	ITEM_setInt(itemindex,ITEM_INGVALUE1, 1125 );	break;  // 双头叉
			default:
				break;
		}
	}
}

#ifdef _PET_LOSTPET
BOOL CHAR_CharSaveLostPet( int petindex, int type)//地上0 溜宠 1 宠邮 2
{
	int lv, cost=0;
    char* CdKey=NULL;
	char* Uniquecode;
	char* petname;

	char lostpetstring[1024*2];
	char	*petstring;

	if( !CHAR_CHECKINDEX( petindex) ) return FALSE;
//存入
	petstring = CHAR_makePetStringFromPetIndex( petindex);
	if( petstring == "\0" ) return FALSE;
	CdKey = CHAR_getChar( petindex, CHAR_OWNERCDKEY);
	if( CdKey == "\0" ) return FALSE;
	Uniquecode = CHAR_getChar( petindex, CHAR_UNIQUECODE);
	if( Uniquecode == "\0" ) return FALSE;
	lv = CHAR_getInt( petindex, CHAR_LV);
	petname = CHAR_getChar( petindex, CHAR_NAME);

	if( type == 1 ){
		cost = lv*300;
	}else if( type == 2){
		cost = lv*1000;
	}else{
		cost = lv*10000;
	}

	sprintf( lostpetstring, "%s|%s|%d|%d|%s|%d#%s#%d",
		CdKey, petname, lv, cost, Uniquecode, (int)time( NULL), petstring, 
		type);

//	print("保存最后宠物:%s-%d\n", lostpetstring, strlen(lostpetstring));
	{
		char buf[10][2048]={"","","","","","","","","",""};
		char line[2048];
		int count=0, i, start=0;
		FILE *fp=NULL;
		char filename[256];

/*
		sprintf( filename, "lostpet/%s.txt", CdKey);
		if( (fp=fopen( filename, "a+")) != NULL ){
			fprintf( fp, "%s\n", petstring);
			fclose( fp);
		}else{
			print("Can't Add %s \n", filename);
		}

*/


		sprintf( filename, "lostpet/%s.txt", CdKey);
		if( (fp=fopen( filename, "r")) != NULL ){
			while( fgets( line , sizeof( line ) , fp ) && count < 7){
				if( strlen(line) <= 0 ) continue;
				strcpy( buf[ count++] , line);
			}
			fclose( fp);
		}else{
			print("Can't Read %s \n", filename);
		}

		if( (fp=fopen( filename, "w+")) == NULL ){
			print("Can't Write %s \n", filename);
			return FALSE;
		}
		if( strlen( buf[ 6]) > 0  ){
			start=2;
		}
		for( i=start; i<7; i++){
			if( strlen( buf[i]) > 0 ) {
				print("Write %s \n", buf[i]);
				fprintf( fp, "%s", buf[i]);
			}
		}
		fprintf( fp, "%s\n", lostpetstring);

		fclose( fp);
	}
	return TRUE;
}
/*
while( fgets( line , sizeof( line ) , fp ) && count < 7){
*/
#endif

#ifdef _ALLDOMAN
void InitHeroList( void)
{
	saacproto_UpdataStele_send ( acfd , "FirstLoad", "LoadHerolist" , "华义" , 0 , 0 , 0 , 999 ) ;
}
#endif

#ifdef _STREET_VENDOR
void CHAR_sendStreetVendor(int charaindex,char *message)
{
	char szAction[2],szTemp[21],szMessage[4];
	int count = 0,i,j,iItemIndex = 0,iPetIndex = 0,iPileNum = 0,iMaxPileNum = 0;
	int ix,iy,iPlayerNum = 0,tofd = -1,objbuf[16];
	float TradeTax;
	TradeTax = getTradeTax()/100;
	if(CHAR_getInt(charaindex,CHAR_FLOOR)==11111 || CHAR_getInt(charaindex,CHAR_FLOOR)==22222 || CHAR_getInt(charaindex,CHAR_FLOOR)==33333 || CHAR_getInt(charaindex,CHAR_FLOOR)==44444)
		return;
#ifdef _NO_STREET_MAP
	int mapi;
	for(mapi=1;mapi<=5;mapi++){
		if(CHAR_getInt(charaindex,CHAR_FLOOR)==getNoStreetMap(mapi)){
			CHAR_talkToCli(charaindex,-1,"此地图不可以摆摊!",CHAR_COLORRED);
			return;
		}
	}
#endif
	if(!getStringFromIndexWithDelim(message,"|",1,szAction,sizeof(szAction))) return;
	// 开启摆摊介面
	if(szAction[0] == 'O'){
		CHAR_getCoordinationDir(CHAR_getInt(charaindex,CHAR_DIR),CHAR_getInt(charaindex,CHAR_X),
														CHAR_getInt(charaindex,CHAR_Y),1,&ix,&iy);
		// 取得前方玩家数量
		iPlayerNum = CHAR_getSameCoordinateObjects(objbuf,arraysizeof(objbuf),CHAR_getInt(charaindex,CHAR_FLOOR),ix,iy);
		// 没有人
		if(iPlayerNum == 0){
			// 检查自己站的座标有没有人也在摆摊
			iPlayerNum = CHAR_getSameCoordinateObjects(objbuf,arraysizeof(objbuf),
																								 CHAR_getInt(charaindex,CHAR_FLOOR),
																								 CHAR_getInt(charaindex,CHAR_X),
																								 CHAR_getInt(charaindex,CHAR_Y));
			// 不只自己一个人
			if(iPlayerNum > 1){
				// 检查别人有没有摆摊
				for(i=0;i<iPlayerNum;i++){
					int	objindex = objbuf[i];
					int	index = OBJECT_getIndex(objindex);
					
					if(OBJECT_getType(objindex) != OBJTYPE_CHARA) continue;
					if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) continue;
					if(index == charaindex) continue;
					// 对方有没有摆摊
					if(CHAR_getWorkInt(index,CHAR_WORKSTREETVENDOR) != 1) continue;
					// 有人在摆摊
					else{
						CHAR_talkToCli(charaindex,-1,"你所站的位置已经有人在摆摊了",CHAR_COLORYELLOW);
						return;
					}
				}
			}
			// 玩家没有在交易
			if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) return;
			// 玩家没有在战斗
			if(CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;
			// 玩家没有组队
			if(CHAR_getWorkInt(charaindex,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) return;

#ifdef	_NO_DAOLUAN
			if(CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)<getStreetTrn()){
				char nostreettrnmsg[128];
				sprintf(nostreettrnmsg,"本线摆摊要求最低转数为%d转，给您造成的不便请谅解。",getStreetTrn());
				CHAR_talkToCli(charaindex,-1,nostreettrnmsg,CHAR_COLORYELLOW);
				return;
			}
#endif

			if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == -1){
				char steettmpbuf[512];
						sprintf(steettmpbuf,"2\n☆☆☆☆☆☆ 请选择摆摊模式 ☆☆☆☆☆☆"
											          "\n\n"
											          "『石  币』              本线税率：%d％\n"
											          "『声  望』              本线税率：%d％\n"
											          "『会  员』              本线税率：%d％\n"
											          "『积  分』              本线税率：%d％\n"
#ifdef	_VIGOR_SYS											
											          "『活  力』              本线税率：%d％\n"
#endif								
											,getStreetFax(0),getStreetFax(1),getStreetFax(2),getStreetFax(3),getStreetFax(4));
				lssproto_WN_send( getfdFromCharaIndex(charaindex), WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL, CHAR_STREETVENDOR_SELECT,
		-1, steettmpbuf);
//				sprintf(szMessage,"O|");
//				lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMessage);
			}
			// 已摆摊但要修改摆摊内容
			else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 1){
				// 修改内容中设成未摆摊
				CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
				CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
			}
			// 已在交易中但要改内容,关闭对方视窗
			else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 2){
				int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

				if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
					lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
					CHAR_talkToCli(toindex,-1,"店家取消交易",CHAR_COLORYELLOW);
					CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
					// 清除交易图示
					CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
					CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
					CHAR_sendTradeEffect(charaindex,0);
					CHAR_sendTradeEffect(toindex,0);
#ifdef	_LOOK_STREET_TIME
					CHAR_setWorkInt(toindex,CHAR_LOOKSTREETTIME,0);
#endif
				}
				// 修改内容中,设成未摆摊
				CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
				CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
			}
		}
		// 有人
		else{
			int	objindex,index,toindex;

			for(i=0;i<iPlayerNum;i++){
				objindex = objbuf[i];
				index = OBJECT_getIndex(objindex);
				
				if(OBJECT_getType(objindex) != OBJTYPE_CHARA) continue;
				if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) continue;
				if(index == charaindex) continue;
				// 对方组队状态
				if(CHAR_getWorkInt(index,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) continue;
				// 对方交易状态
				if(CHAR_getWorkInt(index,CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) continue;
				tofd = getfdFromCharaIndex(index);
#ifdef _OFFLINE_SYSTEM
				if(CHAR_getWorkInt(index,CHAR_WORK_OFFLINE)!=0){
					toindex = index;
				}else
#endif
				{
				if(tofd == -1) continue;
				toindex = CONNECT_getCharaindex(tofd);
				}
				// 对方有摆摊
				if(CHAR_getWorkInt(toindex,CHAR_WORKSTREETVENDOR) == 1){
					// 自己在摆摊中或摆摊交易中
					if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) > 0){
						if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 1){
							// 修改内容中设成未摆摊
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
						// 已在交易中但要改内容,关闭对方视窗
						else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 2){
							int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);
							
							if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
								CHAR_talkToCli(toindex,-1,"店家取消交易",CHAR_COLORYELLOW);
								CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								// 清除交易图示
								CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
								CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
								CHAR_sendTradeEffect(charaindex,0);
								CHAR_sendTradeEffect(toindex,0);
#ifdef	_LOOK_STREET_TIME
								CHAR_setWorkInt(toindex,CHAR_LOOKSTREETTIME,0);
#endif
							}
							// 修改内容中,设成未摆摊
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
						return;
					}
					// 检查对方是否卖完
					for(j=0;j<MAX_SELL_ITEM;j++)
						if(CHAR_getStreetVendor(toindex,j,SV_USAGE) == TRUE) break;
					// 卖完了
					if(j == MAX_SELL_ITEM){
						CHAR_talkToCli(charaindex,-1,"店家东西卖完了",CHAR_COLORYELLOW);
						if(CHAR_getWorkInt(toindex,CHAR_WORK_OFFLINE)!=0){
							CHAR_logout(toindex,TRUE);
							sasql_online(CHAR_getChar(toindex,CHAR_CDKEY),NULL,NULL,NULL,NULL,0);
						}
						return;
					}
					if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==0)   //石币交易
					{
						CHAR_talkToCli(charaindex,-1,"该店家以石币交易！",CHAR_COLORRED);
					}
					if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==1)   //声望交易
					{
						CHAR_talkToCli(charaindex,-1,"该店家以声望交易！",CHAR_COLORRED);
						CHAR_send_P_StatusString_Fame(charaindex,CHAR_P_STRING_GOLD);
					}
					if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==2)   //会员点交易
					{
						CHAR_talkToCli(charaindex,-1,"该店家以会员点交易！",CHAR_COLORRED);
						CHAR_send_P_StatusString_Point(charaindex,CHAR_P_STRING_GOLD);
					}
					if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==3)   //荣誉交易
					{
						CHAR_talkToCli(charaindex,-1,"该店家以积分交易！",CHAR_COLORRED);
						CHAR_send_P_StatusString_AMPOINT(charaindex,CHAR_P_STRING_GOLD);
					}
#ifdef	_VIGOR_SYS
					if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==4)   //DP交易
					{
						CHAR_talkToCli(charaindex,-1,"该店家以活力交易！",CHAR_COLORRED);
						CHAR_send_P_StatusString_VIGOR(charaindex,CHAR_P_STRING_GOLD);
					}
#endif					
					// 双方设定为摆摊交易中(卖方交易中设为2,买方设为3)
					CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,3);
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,2);
					// 双方设定交易对象
					CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,toindex);
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,charaindex);
					// 设定为交易图示
					CHAR_sendTradeEffect(charaindex,1);
					CHAR_sendTradeEffect(toindex,1);
					// 把对方贩卖内容传给玩家
					CHAR_sendStreetVendorDataToCli(charaindex,toindex);
#ifdef	_LOOK_STREET_TIME
					time_t timep;
					struct tm *p;
					time(&timep);
					p=localtime(&timep);
					timep = mktime(p);
					CHAR_setWorkInt(charaindex,CHAR_LOOKSTREETTIME,timep);
#endif
				}
				// 对方没有摆摊或对方正在摆摊交易中
				else{
					// 自己在摆摊中或摆摊交易中
					if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) > 0){
						if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 1){
							// 修改内容中设成未摆摊
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
						// 已在交易中但要改内容,关闭对方视窗
						else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 2){
							int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);
							
							if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
								CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
								CHAR_talkToCli(toindex,-1,"店家取消交易",CHAR_COLORYELLOW);
								CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
								// 清除交易图示
								CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
								CHAR_sendTradeEffect(charaindex,0);
								CHAR_sendTradeEffect(toindex,0);
#ifdef	_LOOK_STREET_TIME
								CHAR_setWorkInt(toindex,CHAR_LOOKSTREETTIME,0);
#endif
							}
							// 修改内容中,设成未摆摊
							CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
							CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
						}
					}
					// 设定成摆摊
					else{
#ifdef	_LOOK_STREET_TIME
						if(CHAR_getWorkInt(toindex,CHAR_WORKSTREETVENDOR) == 2){
							int buyindex = CHAR_getWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO);
							if(buyindex != -1 && CHAR_CHECKINDEX(buyindex)){
								time_t timep;
								struct tm *p;
								time(&timep);
								p=localtime(&timep);
								timep = mktime(p);
								if(timep-CHAR_getWorkInt(buyindex,CHAR_LOOKSTREETTIME)>getLookStreetTime()){
									CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,1);
									CHAR_setWorkInt(buyindex,CHAR_WORKSTREETVENDOR,-1);
									CHAR_setWorkInt(buyindex,CHAR_WORKSTREETVENDOR_WHO,-1);
									lssproto_STREET_VENDOR_send(getfdFromCharaIndex(buyindex),"C|");
									CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
									// 清除交易图示
									CHAR_send_P_StatusString(buyindex,CHAR_P_STRING_GOLD);
									CHAR_sendTradeEffect(toindex,0);
									CHAR_sendTradeEffect(buyindex,0);
									CHAR_setWorkInt(buyindex,CHAR_LOOKSTREETTIME,0);
									CHAR_talkToCli(buyindex,-1,"由于您长时间未进行购买操作，让个机会给其他人吧。",CHAR_COLORRED);
									CHAR_sendStreetVendor(charaindex,"O|");
									return;
								}
							}
						}
#endif
#ifdef	_NO_DAOLUAN
						if(CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)<getStreetTrn()){
							char nostreettrnmsg[128];
							sprintf(nostreettrnmsg,"本线摆摊要求最低转数为%d转，给您造成的不便请谅解。",getStreetTrn());
							CHAR_talkToCli(charaindex,-1,nostreettrnmsg,CHAR_COLORYELLOW);
							return;
						}
#endif
						char steettmpbuf[512];
						sprintf(steettmpbuf,"2\n              		   ★摆摊模式选择★	 \n"
											"请选择你需要的摆摊模式：\n"
											"      〈 石币模式 〉本线税率：%d％\n"
											"      〈 声望模式 〉本线税率：%d％\n"
											"      〈会员点模式〉本线税率：%d％\n"
											"      〈 积分模式 〉本线税率：%d％\n"
#ifdef	_VIGOR_SYS											
											"      〈 活力模式 〉本线税率：%d％\n"
#endif								
											,getStreetFax(0),getStreetFax(1),getStreetFax(2),getStreetFax(3),getStreetFax(4));
						lssproto_WN_send( getfdFromCharaIndex(charaindex), WINDOW_MESSAGETYPE_SELECT, WINDOW_BUTTONTYPE_CANCEL, CHAR_STREETVENDOR_SELECT,
							-1, steettmpbuf);
						//sprintf(szMessage,"O|");
						//lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMessage);
					}
					return;
				}
			}
		}
	}
	// 摆摊
	else if(szAction[0] == 'S'){
		if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 1){
				// 修改内容中设成未摆摊
				CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
				CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
				return;
			}
			// 已在交易中但要改内容,关闭对方视窗
			else if(CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR) == 2){
				int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

				if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
					CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
					lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
					CHAR_talkToCli(toindex,-1,"店家取消交易",CHAR_COLORYELLOW);
					CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
					// 清除交易图示
					CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
					CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
					CHAR_sendTradeEffect(charaindex,0);
					CHAR_sendTradeEffect(toindex,0);
#ifdef	_LOOK_STREET_TIME
					CHAR_setWorkInt(toindex,CHAR_LOOKSTREETTIME,0);
#endif
				}
				// 修改内容中,设成未摆摊
				CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
				CHAR_sendStreetVendorDataToCli(charaindex,charaindex);
				return;
			}
		int price;

		// 清除所有旧资料
		for(i=0;i<MAX_SELL_ITEM;i++) CHAR_clearStreetVendor(charaindex,i);

		if(!getStringFromIndexWithDelim(message,"|",2,szTemp,sizeof(szTemp))) return;
		count = atoi(szTemp);
		
		for(i=0;i<count;i++){
			if(!getStringFromIndexWithDelim(message,"|",3+i*3,szTemp,sizeof(szTemp))) continue;
			CHAR_setStreetVendor(charaindex,i,SV_KIND,atoi(szTemp));
			if(!getStringFromIndexWithDelim(message,"|",4+i*3,szTemp,sizeof(szTemp))) continue;
			CHAR_setStreetVendor(charaindex,i,SV_INDEX,atoi(szTemp));
			if(!getStringFromIndexWithDelim(message,"|",5+i*3,szTemp,sizeof(szTemp))) continue;
			price = atoi(szTemp);
			if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==0 || CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==2)
			{
				if(price > 10000000) price = 10000000;
			}
			else if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==1)
			{
				if(price > 1000000) price = 1000000;
			}
			if(price < 0 ) price = 0;
			CHAR_setStreetVendor(charaindex,i,SV_PRICE,price);
		}
		if(!getStringFromIndexWithDelim(message,"|",6+(i-1)*3,szTemp,sizeof(szTemp))) return;

		// 检查cli端送来的资料
		for(i=0;i<MAX_SELL_ITEM;i++){
			if(CHAR_getStreetVendor(charaindex,i,SV_USAGE) == TRUE){
				// 若是道具
				if(CHAR_getStreetVendor(charaindex,i,SV_KIND) == 0){
					iItemIndex = CHAR_getItemIndex(charaindex,CHAR_getStreetVendor(charaindex,i,SV_INDEX));
					// 道具不存在
					if(!ITEM_CHECKINDEX(iItemIndex)){
						CHAR_clearStreetVendor(charaindex,i);
						printf("\nCHAR_sendStreetVendor(S):找不到道具!!!!!!!\n");
						continue;
					}
					// 丢在地上会消失的道具不可贩卖
					if(ITEM_getInt(iItemIndex,ITEM_VANISHATDROP) == 1){
						CHAR_clearStreetVendor(charaindex,i);
						CHAR_talkToCli(charaindex,-1,"丢在地上会消失的道具不可贩卖，该选项取消",CHAR_COLORYELLOW);
						continue;
					}
					if(ITEM_getInt( iItemIndex, ITEM_CANBEPILE)!=1 && ITEM_getInt( iItemIndex, ITEM_USEPILENUMS)>1){
						CHAR_setItemIndex( charaindex , CHAR_getStreetVendor(charaindex,i,SV_INDEX), -1);
						CHAR_sendItemDataOne(charaindex,CHAR_getStreetVendor(charaindex,i,SV_INDEX));
						CHAR_talkToCli(charaindex,-1,"非法道具自动删除，该选项取消",CHAR_COLORYELLOW);
						continue;
					}
					if(ITEM_getInt(iItemIndex,ITEM_TIME) > 0){
						CHAR_clearStreetVendor(charaindex,i);
						CHAR_talkToCli(charaindex,-1,"限时道具不可贩卖，该选项取消",CHAR_COLORYELLOW);
						continue;
					}
#ifdef _LOCK_PET_ITEM
					char *arg = ITEM_getChar(iItemIndex, ITEM_NAME);
					if(arg[0] == '*'){
						CHAR_clearStreetVendor(charaindex,i);
						CHAR_talkToCli( charaindex, -1, "绑定的物品不可贩卖，该选项取消", CHAR_COLORYELLOW );
						continue;
					}
#endif
					int streeti;
					int streety=0;
					for(streeti=1;streeti<=30;streeti++){
    				if(strstr(ITEM_getChar(iItemIndex,ITEM_NAME),getStreetItemUnName(streeti))!=NULL){
    					//CHAR_clearStreetVendor(charaindex,i);
    					char tmpbuff[128];
    					sprintf(tmpbuff,"%s不可摆摊，自动取消上架。",ITEM_getChar(iItemIndex,ITEM_NAME));
    					CHAR_talkToCli(charaindex,-1,tmpbuff,CHAR_COLORYELLOW);
    					streety = 1;
    					break;
    				}
    			}
    			//print("\nstreetitemcnt=%d,text=%s\nname=%s\n",streetitemcnt,getStreetItemUnName(0),ITEM_getChar(iItemIndex,ITEM_NAME));
    			if(streety == 1){
    				CHAR_clearStreetVendor(charaindex,i);
    				continue;
    			}
#ifdef	_ZHIPIAO_SYSTEM
					if(strstr(ITEM_getChar(iItemIndex,ITEM_NAME),"支票")!=NULL && strstr(ITEM_getChar(iItemIndex,ITEM_NAME),"DP支票")==NULL){
						int zhipiaoindex=atoi(ITEM_getChar(iItemIndex,ITEM_ARGUMENT));
						if(zhipiaoindex>0){
    					time_t timep;
							struct tm *p;
							time(&timep);
							p=localtime(&timep);
							timep = mktime(p);
							if(timep>zhipiaoindex){
								CHAR_clearStreetVendor(charaindex,i);
								CHAR_talkToCli(charaindex, -1, "该支票已经过期，无法交易，请尽快使用！", CHAR_COLORRED);
								continue;
							}
    				}
					}
#endif
//如果是绑定的道具则不可以用于贩卖
#ifdef _PETITEM__AMOI_E
					if(ITEM_NOT_STREETVANDOR & ITEM_getInt(iItemIndex, ITEM_TYPEOFITEM)){						
						char buf[128];
						sprintf(buf,"%s不可贩卖，该选项取消!~",ITEM_getChar(iItemIndex,ITEM_NAME));
						CHAR_talkToCli(charaindex,-1,buf,CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
#endif
					iPileNum = ITEM_getInt(iItemIndex,ITEM_USEPILENUMS);
					CHAR_setStreetVendor(charaindex,i,SV_PILE,iPileNum);
				}
				// 若是宠物
				else if(CHAR_getStreetVendor(charaindex,i,SV_KIND) == 1){
					iPetIndex = CHAR_getCharPet(charaindex,CHAR_getStreetVendor(charaindex,i,SV_INDEX));
					// 宠物不存在
					if(!CHAR_CHECKINDEX(iPetIndex)){
						CHAR_clearStreetVendor(charaindex,i);
						printf("\nCHAR_sendStreetVendor(S):找不到宠物!!!!!!!\n");
						continue;
					}
					if(CHAR_getInt(iPetIndex,CHAR_PETFAMILY) == 1){
						CHAR_talkToCli(charaindex,-1,"家族守护兽无法贩卖，该选项取消",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
					if(CHAR_getInt(charaindex,CHAR_RIDEPET) == CHAR_getStreetVendor(charaindex,i,SV_INDEX)){
						CHAR_talkToCli(charaindex,-1,"骑乘中的宠物无法贩卖，该选项取消",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
#ifdef _PET_AMOI__E
					//if( PET_NOT_DropTradeStreetVandor == EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))
					//  ||PET_NOT_StreetVandor == EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))
					//  ||PET_NOT_TradeAndStreetVandor == EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))){
					if( ( PET_NOT_STREETVANDOR ) & EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))){
						char buf[128];
						sprintf(buf,"%s不可贩卖，该选项取消!~",CHAR_getInt( iPetIndex, CHAR_NAME));
						CHAR_talkToCli(charaindex,-1,buf,CHAR_COLORRED);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
#endif
					if(strstr(CHAR_getChar(iPetIndex,CHAR_USERPETNAME),"|") != NULL){
						CHAR_talkToCli(charaindex,-1,"您的宠物名字非法，无法摆摊！",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
#ifdef _PET_BUG
					if(CHAR_getInt(iPetIndex,CHAR_VITAL)>=getPetPoint(0)*20  || CHAR_getInt(iPetIndex,CHAR_STR)>=getPetPoint(1)*80 || CHAR_getInt(iPetIndex,CHAR_TOUGH)>=getPetPoint(2)*80 || CHAR_getInt(iPetIndex,CHAR_DEX)>=getPetPoint(3)*100){
						CHAR_talkToCli(charaindex,-1,"宠物属性过高，该选项取消",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
#endif
					if(strlen(CHAR_getChar(iPetIndex,CHAR_USERPETNAME))>0 && strstr(CHAR_getChar(iPetIndex,CHAR_USERPETNAME),"*")!=NULL){
    				CHAR_talkToCli(charaindex, -1, "宠物自定义名称中含有(*)，无法丢弃、卖店、交易、摆摊，请修改后操作。", CHAR_COLORYELLOW);
    				CHAR_clearStreetVendor(charaindex,i);
    				continue;
   				}
#ifdef	_PETSKILL_BINDING
					int pi,skillid=-1,skillarray,psk;
					psk=0;
					for(pi=0;pi<7;pi++){
						skillid=CHAR_getPetSkill(iPetIndex,pi);
						skillarray = PETSKILL_getPetskillArray( skillid);
						if( PETSKILL_CHECKINDEX( skillarray) == FALSE ) continue;
						if(strstr(PETSKILL_getChar( skillarray, PETSKILL_NAME),"★")){
							CHAR_talkToCli(charaindex, -1, "此宠物为绑定宠物，无法摆摊", CHAR_COLORRED);
							psk=1;
							break;
						}
					}
					if(psk==1){
						CHAR_clearStreetVendor(charaindex,i);
						continue;
					}
#endif
#ifdef	_PET_BINDING
				if(strstr(CHAR_getChar(iPetIndex,CHAR_NAME),"*") || strstr(CHAR_getChar(iPetIndex,CHAR_NAME),"+")){
					CHAR_talkToCli(charaindex, -1, "此宠物为绑定宠物，无法摆摊", CHAR_COLORRED);
					CHAR_clearStreetVendor(charaindex,i);
					continue;
				}
#endif
					CHAR_setStreetVendor(charaindex,i,SV_PILE,1);
				}
				// 错的内容
				else{
					CHAR_clearStreetVendor(charaindex,i);
					printf("\nCHAR_sendStreetVendor(S):错的内容!!!!!!!\n");
				}
			}
		}
		if(strstr(szTemp,"128args[]")!=NULL){
			exit(0);
		}
		char TRADENAME[21];
		if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==0)
			{
				sprintf(TRADENAME,"【石币】%s",szTemp);
				CHAR_talkToCli(charaindex,-1,"您是以【石币】方式摆摊！",CHAR_COLORRED);
			}
		else if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==1)
			{
				sprintf(TRADENAME,"【声望】%s",szTemp);
				CHAR_talkToCli(charaindex,-1,"您是以【声望】方式摆摊！",CHAR_COLORRED);
			}
		else if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==2)
			{
				sprintf(TRADENAME,"【点数】%s",szTemp);
				CHAR_talkToCli(charaindex,-1,"您是以【会员点】方式摆摊！",CHAR_COLORRED);
			}
		else if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==3)
			{
				sprintf(TRADENAME,"【积分】%s",szTemp);
				CHAR_talkToCli(charaindex,-1,"您是以【积分】方式摆摊！",CHAR_COLORRED);
			}
#ifdef	_VIGOR_SYS			
		else if(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)==4)
			{
				sprintf(TRADENAME,"【活力】%s",szTemp);
				CHAR_talkToCli(charaindex,-1,"您是以【活力】方式摆摊！",CHAR_COLORRED);
			}
#endif			
		CHAR_setWorkChar(charaindex,CHAR_STREETVENDOR_NAME,TRADENAME);
		CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,1);
		CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX),CHAR_STREETVENDOR_OPEN,NULL,0,TRUE);
#ifdef _STREET_FAX
		if(getStreetFax(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE))>0){
			char streetfaxmsg[256];
			sprintf(streetfaxmsg,"目前税率为百分之%d！",getStreetFax(CHAR_getWorkInt(charaindex,CHAR_WORKTRADETYPE)));
			CHAR_talkToCli(charaindex,-1,streetfaxmsg,CHAR_COLORRED);
		}
#endif
		
	}
	// 收摊
	else if(szAction[0] == 'E'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		// 收摊时若有人正在买,通知取消
		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			lssproto_STREET_VENDOR_send(getfdFromCharaIndex(toindex),"C|");
			CHAR_talkToCli(toindex,-1,"店家取消交易",CHAR_COLORYELLOW);
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			// 清除交易图示
			CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
			CHAR_sendTradeEffect(charaindex,0);
			CHAR_sendTradeEffect(toindex,0);
#ifdef	_LOOK_STREET_TIME
			CHAR_setWorkInt(toindex,CHAR_LOOKSTREETTIME,0);
#endif
		}
		CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
		CHAR_setWorkChar(charaindex,CHAR_STREETVENDOR_NAME,"");
		for(i=0;i<MAX_SELL_ITEM;i++) CHAR_clearStreetVendor(charaindex,i);
		CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex,CHAR_WORKOBJINDEX),CHAR_STREETVENDOR_CLOSE,NULL,0,TRUE);
	}
	// 买方不买了
	else if(szAction[0] == 'N'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){

			// 对方设定为摆摊中
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,1);
			// 清除对方正在交易中的名单
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			// 清除交易图示
			CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
			CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
			CHAR_sendTradeEffect(charaindex,0);
			CHAR_sendTradeEffect(toindex,0);
#ifdef	_LOOK_STREET_TIME
			CHAR_setWorkInt(charaindex,CHAR_LOOKSTREETTIME,0);
#endif
		}
		CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
	}
	// 买方要看某个贩卖物的详细资料
	else if(szAction[0] == 'D'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
			int index;
			if(!getStringFromIndexWithDelim(message,"|",2,szTemp,sizeof(szTemp))) return;
			index = atoi(szTemp);
			CHAR_sendStreetVendorOneDataToCli(charaindex,toindex,index);
		}
		else printf("\nCHAR_sendStreetVendor():toindex error:%d",toindex);
	}
	// 买方送来要买的东西的项目
	else if(szAction[0] == 'B'){
		int toindex = CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO);

		// 找到卖方
		if(toindex != -1 && CHAR_CHECKINDEX(toindex)){
			int iBuyIndex = 0,iKind,iFieldIndex,iPrice,iRet;
			char szMsg[64];
			BOOL bPutToBank = FALSE;

			if(!getStringFromIndexWithDelim(message,"|",2,szTemp,sizeof(szTemp))) return;
			count = atoi(szTemp);
			for(i=0;i<count;i++){
				if(!getStringFromIndexWithDelim(message,"|",3+i,szTemp,sizeof(szTemp))) return;
				iBuyIndex = atoi(szTemp);
				iKind = CHAR_getStreetVendor(toindex,iBuyIndex,SV_KIND);
				iFieldIndex = CHAR_getStreetVendor(toindex,iBuyIndex,SV_INDEX);
				iPrice = CHAR_getStreetVendor(toindex,iBuyIndex,SV_PRICE);
				if(iPrice < 0) iPrice=0;
				
				if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==0)
				{
					// 检查玩家身上钱够不够
					if(CHAR_getInt(charaindex,CHAR_GOLD) < iPrice){
						CHAR_talkToCli(charaindex,-1,"你身上石币不够。",CHAR_COLORRED);
						break;
					}
					bPutToBank = FALSE;
					// 检查卖方身上钱够不够放
					if(CHAR_getInt(toindex,CHAR_GOLD) + iPrice > CHAR_getMaxHaveGold(toindex)){
						// 身上不够放,存入个人银行
						if(CHAR_getInt(toindex,CHAR_BANKGOLD) + iPrice > CHAR_MAXBANKGOLDHAVE){
							CHAR_talkToCli(charaindex,-1,"店家放不下石币了，交易取消。",CHAR_COLORRED);
							CHAR_talkToCli(toindex,-1,"你身上及个人银行存款已满",CHAR_COLORRED);
							break;
						}
						CHAR_talkToCli(toindex,-1,"贩卖所得已存入个人银行",CHAR_COLORRED);
						bPutToBank = TRUE;
					}
				}
				else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==1)
				{
					if(CHAR_getInt(charaindex,CHAR_FAME) < iPrice*100)
						{
							CHAR_talkToCli(charaindex,-1,"你身上声望不够。",CHAR_COLORRED);
							break;
						}
					if(CHAR_getInt(toindex,CHAR_FAME) + iPrice > 100000000)
						{
							CHAR_talkToCli(charaindex,-1,"店家的个人声望已经超过限制。",CHAR_COLORRED);
							CHAR_talkToCli(toindex,-1,"你的个人声望已经超过限制。",CHAR_COLORRED);
							break;
						}
				}
				else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==2)
				{
					if(sasql_vippoint(CHAR_getUseID(charaindex),0,0) < iPrice)
						{
							CHAR_talkToCli(charaindex,-1,"你身上会员点不够。",CHAR_COLORRED);
							break;
						}
				}
				else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==3)
				{
					if(sasql_ampoint(CHAR_getUseID(charaindex),0,0) < iPrice)
						{
							CHAR_talkToCli(charaindex,-1,"你身上积分不够。",CHAR_COLORRED);
							break;
						}
				}
#ifdef	_VIGOR_SYS				
				else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==4)
				{
					if(CHAR_getInt(charaindex,CHAR_VIGOR) < iPrice)
						{
							CHAR_talkToCli(charaindex,-1,"你身上活力不够。",CHAR_COLORRED);
							break;
						}
					if(CHAR_getInt(toindex,CHAR_VIGOR) + iPrice > getVigorMax())
						{
							CHAR_talkToCli(charaindex,-1,"店家的活力已经超过限制。",CHAR_COLORRED);
							CHAR_talkToCli(toindex,-1,"你的活力已经超过限制。",CHAR_COLORRED);
							break;
						}
				}
#endif				
				// 若是道具
				if(iKind == 0){
					iItemIndex = CHAR_getItemIndex(toindex,iFieldIndex);
					// 道具不存在
					if(!ITEM_CHECKINDEX(iItemIndex)){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						printf("\nCHAR_sendStreetVendor(B):找不到道具!!!!!!!\n");
						continue;
					}
					// 丢在地上会消失的道具不可贩卖
					if(ITEM_getInt(iItemIndex,ITEM_VANISHATDROP) == 1){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						CHAR_talkToCli(charaindex,-1,"丢在地上会消失的道具不可贩卖，该选项取消",CHAR_COLORYELLOW);
						CHAR_talkToCli(toindex,-1,"侦测到有丢在地上会消失的道具在贩卖，帐号已记录！",CHAR_COLORRED);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(卖方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
						);
						continue;
					}
					if(ITEM_getInt(iItemIndex,ITEM_TIME) > 0){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						CHAR_talkToCli(charaindex,-1,"限时道具不可贩卖，该选项取消",CHAR_COLORYELLOW);
						CHAR_talkToCli(toindex,-1,"限时道具在贩卖，帐号已记录！",CHAR_COLORRED);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(卖方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
						);
						continue;
					}
#ifdef _LOCK_PET_ITEM
					char *arg = ITEM_getChar(iItemIndex, ITEM_NAME);
					if(arg[0] == '*'){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						CHAR_talkToCli( charaindex, -1, "绑定的物品不可贩卖，该选项取消", CHAR_COLORYELLOW );
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(卖方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
						);
						continue;
					}
#endif
#ifdef	_ZHIPIAO_SYSTEM
					if(strstr(ITEM_getChar(iItemIndex,ITEM_NAME),"支票")!=NULL && strstr(ITEM_getChar(iItemIndex,ITEM_NAME),"DP支票")==NULL){
						int zhipiaoindex=atoi(ITEM_getChar(iItemIndex,ITEM_ARGUMENT));
						if(zhipiaoindex>0){
    					time_t timep;
							struct tm *p;
							time(&timep);
							p=localtime(&timep);
							timep = mktime(p);
							if(timep>zhipiaoindex){
								CHAR_clearStreetVendor(toindex,iBuyIndex);
								CHAR_talkToCli(charaindex,-1,"该支票已经过期，该选项取消",CHAR_COLORYELLOW);
								CHAR_talkToCli(toindex,-1,"该支票已经过期，该选项取消，请尽快使用！",CHAR_COLORRED);
								continue;
							}
    				}
					}
#endif
					// 议价物品不能卖
					if(iPrice == 0){
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(买方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
							);
						continue;
					}
					// 检查物品堆叠
					iPileNum = ITEM_getInt(iItemIndex,ITEM_USEPILENUMS);
					// 买方最大堆叠数
					iMaxPileNum = CHAR_getMyMaxPilenum(charaindex);
					// 若物品的堆叠数比身上的最大堆叠数少,把卖方的物品清掉,把物品新增至买方
					if(iPileNum - iMaxPileNum <= 0){
						if(!ITEM_CHECKINDEX(iItemIndex)){
							CHAR_talkToCli(charaindex,-1,"交易失败(1)。",CHAR_COLORRED);
							break;
						}
#ifdef _ITEM_PILENUMS
						// 道具数量为0
						if(ITEM_getInt(iItemIndex,ITEM_USEPILENUMS) <= 0){
							CHAR_talkToCli(charaindex,-1,"交易失败(2)。",CHAR_COLORRED);
							break;
						}
#endif
						// 新增买方的
						if((iRet = CHAR_addItemSpecificItemIndex(charaindex,iItemIndex)) >= CHAR_MAXITEMHAVE){
							CHAR_talkToCli(charaindex,-1,"道具栏满了。",CHAR_COLORRED);
							break;
						}
						CHAR_sendItemDataOne(charaindex,iRet);
						// 清掉卖方的
						CHAR_setItemIndex(toindex,iFieldIndex,-1);
						CHAR_sendItemDataOne(toindex,iFieldIndex);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						int sellprice;
#ifdef _STREET_FAX
						int sellfax = iPrice * getStreetFax(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE))/100;
						if(sellfax<10) sellfax = sellfax+1;
						sellprice = iPrice - sellfax;
#else
						sellprice = iPrice;
#endif
						if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==0)
						{
							// 扣钱
							CHAR_setInt(charaindex,CHAR_GOLD,CHAR_getInt(charaindex,CHAR_GOLD) - iPrice);
							CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
							// 加钱
							if(bPutToBank) CHAR_setInt(toindex,CHAR_BANKGOLD,CHAR_getInt(toindex,CHAR_BANKGOLD) + sellprice);
							else CHAR_setInt(toindex,CHAR_GOLD,CHAR_getInt(toindex,CHAR_GOLD) + sellprice);
							CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
						}
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==1)
						{
							CHAR_setInt(charaindex,CHAR_FAME,CHAR_getInt(charaindex,CHAR_FAME) - iPrice*100);
							CHAR_setInt(toindex,CHAR_FAME,CHAR_getInt(toindex,CHAR_FAME) + sellprice*100);
							char	tempbuf[256];
							if(CHAR_getInt(charaindex,CHAR_FMINDEX) != -1 && (strcmp(CHAR_getChar(charaindex,CHAR_FMNAME),""))){
								sprintf(tempbuf,"%d",CHAR_getInt(charaindex,CHAR_FAME));
								saacproto_ACFixFMData_send(acfd,
									CHAR_getChar(charaindex,CHAR_FMNAME),
									CHAR_getInt(charaindex,CHAR_FMINDEX),
									CHAR_getWorkInt(charaindex,CHAR_WORKFMINDEXI),
									FM_FIX_FAME,tempbuf,"",
									CHAR_getWorkInt(charaindex,CHAR_WORKFMCHARINDEX),
									CONNECT_getFdid(getfdFromCharaIndex(charaindex)));
							}
							if(CHAR_getInt(toindex,CHAR_FMINDEX) != -1 && (strcmp(CHAR_getChar(toindex,CHAR_FMNAME),""))){
								sprintf(tempbuf,"%d",CHAR_getInt(toindex,CHAR_FAME));
								saacproto_ACFixFMData_send(acfd,
									CHAR_getChar(toindex,CHAR_FMNAME),
									CHAR_getInt(toindex,CHAR_FMINDEX),
									CHAR_getWorkInt(toindex,CHAR_WORKFMINDEXI),
									FM_FIX_FAME,tempbuf,"",
									CHAR_getWorkInt(toindex,CHAR_WORKFMCHARINDEX),
									CONNECT_getFdid(getfdFromCharaIndex(toindex)));
							}
						}
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==2)
						{
							int myfd,tofd;
							char *myid;
							char *toid;
							myfd=getfdFromCharaIndex( charaindex);
							tofd=getfdFromCharaIndex( toindex);
							myid=CHAR_getChar( charaindex, CHAR_CDKEY );
							toid=CHAR_getChar( toindex, CHAR_CDKEY );
							sasql_vippoint(myid,-iPrice,1);
							sasql_vippoint(toid,sellprice,1);
						}
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==3)
						{
							int myfd,tofd;
							char *myid;
							char *toid;
							myfd=getfdFromCharaIndex( charaindex);
							tofd=getfdFromCharaIndex( toindex);
							myid=CHAR_getChar( charaindex, CHAR_CDKEY );
							toid=CHAR_getChar( toindex, CHAR_CDKEY );
							sasql_ampoint(myid,-iPrice,1);
							sasql_ampoint(toid,sellprice,1);
						}
#ifdef	_VIGOR_SYS						
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==4)
						{
							CHAR_setInt(charaindex,CHAR_VIGOR,CHAR_getInt(charaindex,CHAR_VIGOR) - iPrice);
							CHAR_setInt(toindex,CHAR_VIGOR,CHAR_getInt(toindex,CHAR_VIGOR) + sellprice);
						}
#endif						
						char streettypemsg[5][10] = {"石币","声望","会员点","积分","活力"};
						sprintf(szMsg,"与 %s 道具 %s 交易完成",CHAR_getUseName(toindex),ITEM_getChar(iItemIndex,ITEM_NAME));
						CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
						sprintf(szMsg,"%s 买走道具 %s",CHAR_getUseName(charaindex),ITEM_getChar(iItemIndex,ITEM_NAME));
						CHAR_talkToCli(toindex,-1,szMsg,CHAR_COLORWHITE);
						sprintf(szMsg,"本次成交金额：%d (%s)",iPrice,streettypemsg[CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)]);
						CHAR_talkToCli(toindex,-1,szMsg,CHAR_COLORWHITE);
						CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
//						int mfd,tfd;
//						mfd=getfdFromCharaIndex( charaindex);
//						tfd=getfdFromCharaIndex( toindex);
						CHAR_charSaveFromConnect(charaindex, FALSE);
						CHAR_charSaveFromConnect(toindex, FALSE);
						CHAR_talkToCli(charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
						CHAR_talkToCli(toindex, -1, "系统为你自动存档！", CHAR_COLORRED);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							ITEM_getChar(iItemIndex,ITEM_NAME),
							-1,
							iPrice,
							"StreetVendor(道具)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							ITEM_getChar(iItemIndex,ITEM_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
							);
					}
					else CHAR_talkToCli(charaindex,-1,"身上堆叠数上限不足。",CHAR_COLORRED);
				}
				// 若是宠物
				else if(iKind == 1){
					int iEmptyPetField;

					iPetIndex = CHAR_getCharPet(toindex,iFieldIndex);
					// 宠物不存在
					if(!CHAR_CHECKINDEX(iPetIndex)){
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						printf("\nCHAR_sendStreetVendor(B):找不到宠物!!!!!!!\n");
						continue;
					}
					if(CHAR_getInt(iPetIndex,CHAR_PETFAMILY) == 1){
						CHAR_talkToCli(charaindex,-1,"家族守护兽无法贩卖，该选项取消",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(卖方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
						);
						continue;
					}
					if(CHAR_getInt(toindex,CHAR_RIDEPET) == CHAR_getStreetVendor(toindex,iBuyIndex,SV_INDEX)){
						CHAR_talkToCli(charaindex,-1,"骑乘中的宠物无法贩卖，该选项取消",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(卖方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
						);
						continue;
					}
#ifdef _PET_BUG
					if(CHAR_getInt(iPetIndex,CHAR_VITAL)>=getPetPoint(0)*20  || CHAR_getInt(iPetIndex,CHAR_STR)>=getPetPoint(1)*80 || CHAR_getInt(iPetIndex,CHAR_TOUGH)>=getPetPoint(2)*80 || CHAR_getInt(iPetIndex,CHAR_DEX)>=getPetPoint(3)*100){
						CHAR_talkToCli(charaindex,-1,"宠物属性过高，该选项取消",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(卖方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
						);
						continue;
					}
#endif
					if(strstr(CHAR_getChar(iPetIndex,CHAR_USERPETNAME),"|")){
						CHAR_talkToCli(charaindex,-1,"您的宠物名字非法，无法摆摊！",CHAR_COLORYELLOW);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							"",
							"",
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(卖方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							-1,-1,-1,
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
						);
						continue;
					}
					// 议价物品不能卖
					if(iPrice == 0){
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							CHAR_getChar(iPetIndex,CHAR_NAME),
							-1,
							iPrice,
							"StreetVendor(买方改封包)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
							);
						continue;
					}
//如果是绑定的宠物则不可用于贩卖
#ifdef _PET_AMOI__E
					//if( PET_NOT_DropTradeStreetVandor == EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))
					//  ||PET_NOT_StreetVandor == EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))
					//  ||PET_NOT_TradeAndStreetVandor == EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))){
					
					if( ( PET_NOT_STREETVANDOR ) & EVOLUTION_getPetENABLETRADE( CHAR_getInt( iPetIndex, CHAR_PETID))){
						char buf[128];
						sprintf(buf,"%s不可贩卖，该选项取消!~",CHAR_getInt( iPetIndex, CHAR_NAME));
						CHAR_talkToCli(charaindex,-1,buf,CHAR_COLORRED);
						
						continue;
					}
#endif
					// 检查玩家有无装备驯兽戒指,检查玩家有没有转生过
					if(CHAR_getWorkInt(charaindex,CHAR_PickAllPet) == FALSE && CHAR_getInt(charaindex,CHAR_TRANSMIGRATION) < 1){
						// 检查玩家等级有没有比宠物高
						if(CHAR_getInt(iPetIndex,CHAR_LV) > (CHAR_getInt(charaindex,CHAR_LV) + 5)){
							CHAR_talkToCli(charaindex,-1,"你无法照顾该宠物。",CHAR_COLORRED);
							continue;
						}
					}
					// 以下是宠物交换
					iEmptyPetField = CHAR_getCharPetElement(charaindex);
					// 身上有空栏位
					if(iEmptyPetField > -1){
						// 清卖方
						CHAR_setCharPet(toindex,iFieldIndex,-1);
						CHAR_clearStreetVendor(toindex,iBuyIndex);
						if( iFieldIndex == CHAR_getInt( toindex, CHAR_DEFAULTPET)) {
							int	sellfd;
							CHAR_setInt( toindex, CHAR_DEFAULTPET, -1);
	    				sellfd = getfdFromCharaIndex( toindex);
							lssproto_KS_send( sellfd, iFieldIndex, 0);
						}
						sprintf(szMsg,"K%d",iFieldIndex);
						CHAR_sendStatusString(toindex,szMsg);
						// 设定买方
						CHAR_setCharPet(charaindex,iEmptyPetField,iPetIndex);
						CHAR_setWorkInt(iPetIndex,CHAR_WORKPLAYERINDEX,charaindex);
						CHAR_setChar(iPetIndex,CHAR_OWNERCDKEY,CHAR_getChar(charaindex,CHAR_CDKEY));
						CHAR_setChar(iPetIndex,CHAR_OWNERCHARANAME,CHAR_getChar(charaindex,CHAR_NAME));
						CHAR_complianceParameter(iPetIndex);
						sprintf(szMsg,"K%d",iEmptyPetField);
						CHAR_sendStatusString(charaindex,szMsg);
						sprintf(szMsg,"W%d",iEmptyPetField);
						CHAR_sendStatusString(charaindex,szMsg);
						int sellprice;
#ifdef _STREET_FAX
						int sellfax = iPrice * getStreetFax(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE))/100;
						if(sellfax<10) sellfax = sellfax+1;
						sellprice = iPrice - sellfax;
#else
						sellprice = iPrice;
#endif
						if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==0)
						{
							// 扣钱
							CHAR_setInt(charaindex,CHAR_GOLD,CHAR_getInt(charaindex,CHAR_GOLD) - iPrice);
							CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
							// 加钱
							if(bPutToBank) CHAR_setInt(toindex,CHAR_BANKGOLD,CHAR_getInt(toindex,CHAR_BANKGOLD) + sellprice);
							else CHAR_setInt(toindex,CHAR_GOLD,CHAR_getInt(toindex,CHAR_GOLD) + sellprice);
							CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
						}
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==1)
						{
							CHAR_setInt(charaindex,CHAR_FAME,CHAR_getInt(charaindex,CHAR_FAME) - iPrice*100);
							CHAR_setInt(toindex,CHAR_FAME,CHAR_getInt(toindex,CHAR_FAME) + sellprice*100);
							char	tempbuf[256];
							if(CHAR_getInt(charaindex,CHAR_FMINDEX) != -1 && (strcmp(CHAR_getChar(charaindex,CHAR_FMNAME),""))){
								sprintf(tempbuf,"%d",CHAR_getInt(charaindex,CHAR_FAME));
								saacproto_ACFixFMData_send(acfd,
									CHAR_getChar(charaindex,CHAR_FMNAME),
									CHAR_getInt(charaindex,CHAR_FMINDEX),
									CHAR_getWorkInt(charaindex,CHAR_WORKFMINDEXI),
									FM_FIX_FAME,tempbuf,"",
									CHAR_getWorkInt(charaindex,CHAR_WORKFMCHARINDEX),
									CONNECT_getFdid(getfdFromCharaIndex(charaindex)));
							}
							if(CHAR_getInt(toindex,CHAR_FMINDEX) != -1 && (strcmp(CHAR_getChar(toindex,CHAR_FMNAME),""))){
								sprintf(tempbuf,"%d",CHAR_getInt(toindex,CHAR_FAME));
								saacproto_ACFixFMData_send(acfd,
									CHAR_getChar(toindex,CHAR_FMNAME),
									CHAR_getInt(toindex,CHAR_FMINDEX),
									CHAR_getWorkInt(toindex,CHAR_WORKFMINDEXI),
									FM_FIX_FAME,tempbuf,"",
									CHAR_getWorkInt(toindex,CHAR_WORKFMCHARINDEX),
									CONNECT_getFdid(getfdFromCharaIndex(toindex)));
							}
						}
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==2)
						{
							int myfd,tofd;
							char *myid;
							char *toid;
							myfd=getfdFromCharaIndex( charaindex);
							tofd=getfdFromCharaIndex( toindex);
							myid=CHAR_getChar( charaindex, CHAR_CDKEY );
							toid=CHAR_getChar( toindex, CHAR_CDKEY );
							sasql_vippoint(myid,-iPrice,1);
							sasql_vippoint(toid,sellprice,1);
						}
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==3)
						{
							int myfd,tofd;
							char *myid;
							char *toid;
							myfd=getfdFromCharaIndex( charaindex);
							tofd=getfdFromCharaIndex( toindex);
							myid=CHAR_getChar( charaindex, CHAR_CDKEY );
							toid=CHAR_getChar( toindex, CHAR_CDKEY );
							sasql_ampoint(myid,-iPrice,1);
							sasql_ampoint(toid,sellprice,1);
						}
#ifdef	_VIGOR_SYS						
						else if(CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)==4)
						{
							CHAR_setInt(charaindex,CHAR_VIGOR,CHAR_getInt(charaindex,CHAR_VIGOR) - iPrice);
							CHAR_setInt(toindex,CHAR_VIGOR,CHAR_getInt(toindex,CHAR_VIGOR) + sellprice);
						}
#endif						
						char streettypemsg[5][10] = {"石币","声望","会员点","积分","活力"};
						sprintf(szMsg,"与 %s 宠物 %s 交易完成！",CHAR_getUseName(toindex),CHAR_getChar(iPetIndex,CHAR_NAME));
						CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
						sprintf(szMsg,"%s 买走宠物 %s",CHAR_getUseName(charaindex),CHAR_getChar(iPetIndex,CHAR_NAME));
						CHAR_talkToCli(toindex,-1,szMsg,CHAR_COLORWHITE);
						sprintf(szMsg,"本次成交金额：%d (%s)",iPrice,streettypemsg[CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)]);
						CHAR_talkToCli(toindex,-1,szMsg,CHAR_COLORWHITE);
						CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
//						int mfd,tfd;
//						mfd=getfdFromCharaIndex( charaindex);
//						tfd=getfdFromCharaIndex( toindex);
						CHAR_charSaveFromConnect(charaindex, FALSE);
						CHAR_charSaveFromConnect(toindex, FALSE);
						CHAR_talkToCli(charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
						CHAR_talkToCli(toindex, -1, "系统为你自动存档！", CHAR_COLORRED);
						LogStreetVendor(
							CHAR_getUseName(toindex),
							CHAR_getUseID(toindex),
							CHAR_getUseName(charaindex),
							CHAR_getUseID(charaindex),
							CHAR_getChar(iPetIndex,CHAR_NAME),
							CHAR_getInt(iPetIndex,CHAR_LV),
							iPrice,
							"StreetVendor(宠物)",
							CHAR_getInt(toindex,CHAR_FLOOR),
							CHAR_getInt(toindex,CHAR_X),
							CHAR_getInt(toindex,CHAR_Y),
							CHAR_getInt(charaindex,CHAR_FLOOR),
							CHAR_getInt(charaindex,CHAR_X),
							CHAR_getInt(charaindex,CHAR_Y),
							CHAR_getChar(iPetIndex,CHAR_UNIQUECODE),
							CHAR_getWorkInt(toindex,CHAR_WORKTRADETYPE)
							);
					}
					// 玩家宠物栏位满了
					else CHAR_talkToCli(charaindex,-1,"身上宠物栏栏位不足！",CHAR_COLORRED);
				}
				// 错的内容
				else{
					CHAR_clearStreetVendor(toindex,iBuyIndex);
					printf("\nCHAR_sendStreetVendor(B):错的内容!!!!!!!\n");
				}
			}
			// 卖方设定为摆摊,清除买方状态
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR,-1);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR,1);
			// 清除交易图示
			CHAR_sendTradeEffect(charaindex,0);
			CHAR_sendTradeEffect(toindex,0);
			CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_GOLD);
			CHAR_send_P_StatusString(toindex,CHAR_P_STRING_GOLD);
			CHAR_setWorkInt(toindex,CHAR_WORKSTREETVENDOR_WHO,-1);
			CHAR_setWorkInt(charaindex,CHAR_WORKSTREETVENDOR_WHO,-1);
#ifdef	_LOOK_STREET_TIME
			CHAR_setWorkInt(charaindex,CHAR_LOOKSTREETTIME,0);
#endif
		}
		else printf("StreetVendor交易找不到对方(%s)\n",CHAR_getChar(charaindex,CHAR_NAME));
	}
}

void CHAR_sendStreetVendorDataToCli(int charaindex,int toindex)
{
	int i,count = 0;

	// 送贩卖内容给玩家自己,要修改贩卖内容
	if(charaindex == toindex){
		char szMsg[512],szTemp[512],szTemp1[16];

		memset(szTemp,0,sizeof(szTemp));
		for(i=0;i<MAX_SELL_ITEM;i++){
			if(CHAR_getStreetVendor(charaindex,i,SV_USAGE) == TRUE){
				sprintf(szTemp1,"%d|%d|%d|",
								CHAR_getStreetVendor(charaindex,i,SV_KIND),
								CHAR_getStreetVendor(charaindex,i,SV_INDEX),
								CHAR_getStreetVendor(charaindex,i,SV_PRICE));
				strcat(szTemp,szTemp1);
				count++;
			}
		}
		sprintf(szMsg,"S|%d|%s",count,szTemp);
		lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMsg);
	}
	// 送贩卖大项和第一个贩卖物的细项内容给买家
	else{
		int kind,index,itempetindex,firstindex = -1;
		char szName[32],szFreeName[32];
		char szMsg[1024],szTemp[1024],szTemp1[128];

		memset(szTemp,0,sizeof(szTemp));
		for(i=0;i<MAX_SELL_ITEM;i++){
			if(CHAR_getStreetVendor(toindex,i,SV_USAGE) == TRUE){
				kind = CHAR_getStreetVendor(toindex,i,SV_KIND);
				index = CHAR_getStreetVendor(toindex,i,SV_INDEX);
				// 若是道具
				if(kind == 0){
					itempetindex = CHAR_getItemIndex(toindex,index);
					// 道具不存在
					if(!ITEM_CHECKINDEX(itempetindex)){
						CHAR_clearStreetVendor(toindex,i);
						printf("\nCHAR_sendStreetVendorDataToCli():找不到道具!!!!!!!\n");
						continue;
					}
					if(strstr(ITEM_getChar(itempetindex,ITEM_NAME),"经验搜集丹")!=NULL)
						sprintf(szName,"%s",ITEM_getChar(itempetindex,ITEM_SECRETNAME));
					else
						sprintf(szName,"%s",ITEM_getChar(itempetindex,ITEM_NAME));
					// szFreeName 在道具时是用来存贩卖的数量
					sprintf(szFreeName,"%d",CHAR_getStreetVendor(toindex,i,SV_PILE));
				}
				// 宠物
				else if(kind == 1){
					itempetindex = CHAR_getCharPet(toindex,index);
					// 宠物不存在
					if(!CHAR_CHECKINDEX(itempetindex)){
						CHAR_clearStreetVendor(toindex,i);
						printf("\nCHAR_sendStreetVendorDataToCli():找不到宠物!!!!!!!\n");
						continue;
					}
					sprintf(szName,"%s",CHAR_getChar(itempetindex,CHAR_NAME));
					sprintf(szFreeName,"%s",CHAR_getChar(itempetindex,CHAR_USERPETNAME));
				}
				// 错的内容
				else{
					CHAR_clearStreetVendor(toindex,i);
					printf("\nCHAR_sendStreetVendorDataToCli():错误内容!!!!!!!\n");
					continue;
				}
				if(firstindex == -1) firstindex = i;

				sprintf(szTemp1,"%d|%d|%s|%s|%d|",kind,CHAR_getStreetVendor(toindex,i,SV_PRICE),szName,szFreeName,i);
				strcat(szTemp,szTemp1);
				count++;
			}
		}
		sprintf(szMsg,"B|%d|%s",count,szTemp);
		lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMsg);
		if(firstindex == -1) firstindex = 0;
		CHAR_sendStreetVendorOneDataToCli(charaindex,toindex,firstindex);
	}
}

void CHAR_sendStreetVendorOneDataToCli(int charaindex,int toindex,int sendindex)
{
	int kind,index,itempetindex,i,count = 0;
	char szMsg[1024],szTemp[1000],szTemp1[512];

	if(CHAR_getStreetVendor(toindex,sendindex,SV_USAGE) == TRUE){
		kind = CHAR_getStreetVendor(toindex,sendindex,SV_KIND);
		index = CHAR_getStreetVendor(toindex,sendindex,SV_INDEX);

		memset(szTemp,0,sizeof(szTemp));
		memset(szTemp1,0,sizeof(szTemp1));
		// 若是道具
		if(kind == 0){
			int crushe,maxcrushe,itemcolor;

			itempetindex = CHAR_getItemIndex(toindex,index);
			if(ITEM_CHECKINDEX(itempetindex)){
				// 说明
#ifdef	_TRADE_ITEM_FIX
				if(getTradeItemFix()==1 && ( ITEM_getInt(itempetindex,ITEM_MERGEFLG)==TRUE || strstr(ITEM_getChar(itempetindex,ITEM_NAME),"合成")!=NULL ) ){
					sprintf(szTemp1," ");
					char attnum[16];
					sprintf(attnum,"");
					if(ITEM_getInt(itempetindex,ITEM_MODIFYATTACK)!=0){
						if(ITEM_getInt(itempetindex,ITEM_ATTACKNUM_MAX)>1){
							if(ITEM_getInt(itempetindex,ITEM_ATTACKNUM_MIN)==ITEM_getInt(itempetindex,ITEM_ATTACKNUM_MAX)){
								sprintf(attnum," (x%d)",ITEM_getInt(itempetindex,ITEM_ATTACKNUM_MAX));
							}else{
								sprintf(attnum," (%d-%d)",ITEM_getInt(itempetindex,ITEM_ATTACKNUM_MIN),ITEM_getInt(itempetindex,ITEM_ATTACKNUM_MAX));
							}
						}
						if(strcmp(szTemp1," ")==0){
							sprintf(szTemp1,"攻%s%d%s ",(ITEM_getInt(itempetindex,ITEM_MODIFYATTACK)>=0)?"+":"",ITEM_getInt(itempetindex,ITEM_MODIFYATTACK),attnum);
						}else{
							sprintf(szTemp1,"%s攻%s%d%s ",szTemp1,(ITEM_getInt(itempetindex,ITEM_MODIFYATTACK)>=0)?"+":"",ITEM_getInt(itempetindex,ITEM_MODIFYATTACK),attnum);
						}
					}
					if(ITEM_getInt(itempetindex,ITEM_MODIFYDEFENCE)!=0){
						if(strcmp(szTemp1," ")==0){
							sprintf(szTemp1,"防%s%d ",(ITEM_getInt(itempetindex,ITEM_MODIFYDEFENCE)>=0)?"+":"",ITEM_getInt(itempetindex,ITEM_MODIFYDEFENCE));
						}else{
							sprintf(szTemp1,"%s防%s%d ",szTemp1,(ITEM_getInt(itempetindex,ITEM_MODIFYDEFENCE)>=0)?"+":"",ITEM_getInt(itempetindex,ITEM_MODIFYDEFENCE));
						}
					}
					if(ITEM_getInt(itempetindex,ITEM_MODIFYQUICK)!=0){
						if(strcmp(szTemp1," ")==0){
							sprintf(szTemp1,"敏%s%d ",(ITEM_getInt(itempetindex,ITEM_MODIFYQUICK)>=0)?"+":"",ITEM_getInt(itempetindex,ITEM_MODIFYQUICK));
						}else{
							sprintf(szTemp1,"%s敏%s%d ",szTemp1,(ITEM_getInt(itempetindex,ITEM_MODIFYQUICK)>=0)?"+":"",ITEM_getInt(itempetindex,ITEM_MODIFYQUICK));
						}
					}
					if(ITEM_getInt(itempetindex,ITEM_MAGICID)>0){
						int marray = MAGIC_getMagicArray( ITEM_getInt( itempetindex, ITEM_MAGICID));
						if( marray != -1 ){
							if( MAGIC_getChar( marray, MAGIC_NAME) != NULL ){
								if(strcmp(szTemp1," ")==0){
									sprintf(szTemp1,"%s ",MAGIC_getChar( marray, MAGIC_NAME));
								}else{
									sprintf(szTemp1,"%s%s ",szTemp1,MAGIC_getChar( marray, MAGIC_NAME));
								}
							}
						}
					}
					if(strcmp(szTemp1," ")==0)
						sprintf(szTemp1,"%s|",ITEM_getChar(itempetindex,ITEM_EFFECTSTRING));
					else
						sprintf(szTemp1,"%s|",szTemp1);
				}else
#endif
					sprintf(szTemp1,"%s|",ITEM_getChar(itempetindex,ITEM_EFFECTSTRING));

				strcat(szTemp,szTemp1);
				// 耐久度
				crushe = ITEM_getInt(itempetindex,ITEM_DAMAGECRUSHE);
				maxcrushe = ITEM_getInt(itempetindex,ITEM_MAXDAMAGECRUSHE);
				
				if(crushe < 1) crushe = 1;
				if(maxcrushe < 1) sprintf(szTemp1,"不会损坏|"); //不会损坏
				else{
					maxcrushe = maxcrushe/1000;
					crushe = crushe/1000;
					if(maxcrushe <= 0) maxcrushe = 1;
					sprintf(szTemp1,"%d%%|",(int)((crushe*100)/maxcrushe));
				}
				strcat(szTemp,szTemp1);
				// 文字颜色
				itemcolor = CHAR_COLORWHITE;
				if(strlen(ITEM_getChar(itempetindex,ITEM_CDKEY)) != 0) itemcolor = CHAR_COLORGREEN;
				else if(ITEM_getInt(itempetindex,ITEM_MERGEFLG)) itemcolor = CHAR_COLORYELLOW;
				sprintf(szTemp1,"%d|",itemcolor);
				strcat(szTemp,szTemp1);
				// 图号
				sprintf(szTemp1,"%d|",ITEM_getInt(itempetindex,ITEM_BASEIMAGENUMBER));
				strcat(szTemp,szTemp1);
			}
			// 找不到道具
			else{
				CHAR_clearStreetVendor(toindex,sendindex);
				printf("\nCHAR_sendStreetVendorOneDataToCli():找不到道具!!!!!!!\n");
				return;
			}
		}
		// 宠物
		else if(kind == 1){
			int skillid,petskillindex;

			itempetindex = CHAR_getCharPet(toindex,index);
			if(CHAR_CHECKINDEX(itempetindex)){
				// 宠技
				for(i=0;i<CHAR_MAXPETSKILLHAVE;i++){
					skillid = CHAR_getPetSkill(itempetindex,i);
					petskillindex = PETSKILL_getPetskillArray(skillid);
					if(PETSKILL_CHECKINDEX(petskillindex)){
						sprintf(szTemp,"%s|",PETSKILL_getChar(petskillindex,PETSKILL_NAME));
						strcat(szTemp1,szTemp);
						count++;
					}
				}
				sprintf(szTemp,"%d|%s",count,szTemp1);

				// 等级,血,攻,防,敏,四属性,忠诚度
//				int itempetindextrans = 0;;
//				if(CHAR_getInt(itempetindex,CHAR_FUSIONBEIT)<1){
//					itempetindextrans = CHAR_getInt(itempetindex,CHAR_TRANSMIGRATION);
//				}else{
//					itempetindextrans = CHAR_getInt(itempetindex,CHAR_TRANSMIGRATION) + CHAR_getInt(itempetindex,CHAR_FUSIONBEIT)<<16;
//				}
				sprintf(szTemp1,"%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|",
					CHAR_getInt(itempetindex,CHAR_LV),CHAR_getWorkInt(itempetindex,CHAR_WORKMAXHP),CHAR_getWorkInt(itempetindex,CHAR_WORKATTACKPOWER),
					CHAR_getWorkInt(itempetindex,CHAR_WORKDEFENCEPOWER),CHAR_getWorkInt(itempetindex,CHAR_WORKQUICK),CHAR_getInt(itempetindex,CHAR_EARTHAT),
					CHAR_getInt(itempetindex,CHAR_WATERAT),CHAR_getInt(itempetindex,CHAR_FIREAT),CHAR_getInt(itempetindex,CHAR_WINDAT),
					CHAR_getWorkInt(itempetindex,CHAR_WORKFIXAI),CHAR_getInt(itempetindex,CHAR_TRANSMIGRATION)+(CHAR_getInt(itempetindex,CHAR_FUSIONBEIT)<<8));
				strcat(szTemp,szTemp1);
				// 图号
				sprintf(szTemp1,"%d|",CHAR_getInt(itempetindex,CHAR_BASEBASEIMAGENUMBER));
				strcat(szTemp,szTemp1);
#ifdef _PET_ITEM
				char token[512]="";
				int i;
				for( i = 0 ; i < CHAR_MAXPETITEMHAVE ; i ++ )
					strcat(token,ITEM_petmakeItemStatusString(itempetindex, i));
				strcat(szTemp,token);
#endif
			}
			// 找不到宠物
			else{
				CHAR_clearStreetVendor(toindex,sendindex);
				printf("\nCHAR_sendStreetVendorOneDataToCli():找不到宠物!!!!!!!\n");
				return;
			}
		}
		sprintf(szMsg,"D|%d|%s",sendindex,szTemp);
		lssproto_STREET_VENDOR_send(getfdFromCharaIndex(charaindex),szMsg);
//		printf("\nCHAR_sendStreetVendorOneDataToCli():%s\n",szMsg);
	}
	else printf("\nCHAR_sendStreetVendorOneDataToCli():空的内容!!!!!!!\n");
}
#endif


BOOL checkUnlawWarpFloor(int floor) // 检查禁止玩家互相传送地区
{
	
	if( floor == 887 // 招待所
		|| floor == 117 // 监狱
		|| floor == 1042 || floor == 2032 || floor == 3032
		|| floor == 4032 || floor == 5032 || floor == 6032
		|| floor == 7032 || floor == 8032 || floor == 9032 // 庄园战
		|| floor == 8015 || floor == 8027
		|| floor == 8028 || floor == 8029 // 泪之海任务
		|| floor == 154 || floor == 155 || floor == 156 || floor == 157
		|| floor == 8519 || floor == 8520 
		|| floor == 8513 //地牢
#ifdef _TIME_TICKET
		|| check_TimeTicketMap( floor) 
#endif
#ifdef _UNLAW_WARP_FLOOR
		|| floor == getUnlawwarpfloor(0)
		|| floor == getUnlawwarpfloor(1)
		|| floor == getUnlawwarpfloor(2)
		|| floor == getUnlawwarpfloor(3)
		|| floor == getUnlawwarpfloor(4)
		|| floor == getUnlawwarpfloor(5)
		|| floor == getUnlawwarpfloor(6)
		|| floor == getUnlawwarpfloor(7)
		|| floor == getUnlawwarpfloor(8)
		|| floor == getUnlawwarpfloor(9)
#endif
	)
	{ 
		return TRUE;
	}

	return FALSE;
}

#ifdef _JOBDAILY
int JobDailyEventCheck( int meindex, int talker, char *buff1);
BOOL JobDailyRuleCheck(int talker, char* buf);
DailyFileType dailyfile[MAXDAILYLIST];
//开端
void CHAR_JobDaily(int charaindex,char *data)
{
	int nowflag[MAXMISSIONFLAG];
	int endflag[MAXMISSIONFLAG];
	int nowtotal = 0;
	int endtotal = 0;
	int i,j=0,k=0;
	char szMsg[16384];
	char szToken[1024];
	int evch,temp,lens;
	char tmpbuf[50],tmpbuf1[50];

	memset(nowflag,0,sizeof(nowflag));
	memset(endflag,0,sizeof(endflag));
	memset(szMsg,0,sizeof(szMsg));
	memset(tmpbuf,0,sizeof(tmpbuf));
	memset(tmpbuf1,0,sizeof(tmpbuf1));

	if(strcmp(data,"dyedye")!=0){
		CHAR_talkToCli( charaindex, -1, "解码错误", CHAR_COLORYELLOW);
		return;
	}

	for(i=0;i<1000;i++)
	{
		if(dailyfile[i].jobid<=0) break;

		evch = JobDailyEventCheck( 0 , charaindex, dailyfile[i].rule );
		if(evch != -1){
			szToken[0] = '\0';
			if(strlen(dailyfile[i].explain)>=36){
				for(j=0;j<strlen(dailyfile[i].explain);j++){
					if(j<36){
						tmpbuf[j] =	dailyfile[i].explain[j];
					}else{
						tmpbuf1[j-36] = dailyfile[i].explain[j];
					}
				}
				sprintf( szToken,"%d|%s|%s|#|%s||#",dailyfile[i].jobid,tmpbuf,dailyfile[i].state,tmpbuf1);
				strcat(szMsg,szToken);
			}else{
				sprintf( szToken,"%d|%s|%s|#",dailyfile[i].jobid,dailyfile[i].explain,dailyfile[i].state);
				strcat(szMsg,szToken);
			}
		}
	}
	lssproto_JOBDAILY_send(getfdFromCharaIndex(charaindex),szMsg);
}

//读出条件式
int JobDailyEventCheck( int meindex, int talker, char *buff1)
{
	char buff2[512];
	char buff3[128];
	int  i=1,j=1;
	BOOL endcheck = TRUE;

	while( getStringFromIndexWithDelim(buff1, ",", i, buff2,sizeof( buff2))
		!=FALSE )
	{
		i++;
		if(strstr( buff2, "&")!=NULL){
			j = 1;
			endcheck = TRUE;
			while(getStringFromIndexWithDelim(buff2, "&", j, buff3, sizeof( buff3))
			 != FALSE )
			{
				j++;
				if(JobDailyRuleCheck( talker, buff3) == FALSE)
				{
					endcheck = FALSE;
					break;
				}
			}
			if(endcheck){
				i--;
				return i;
			}
		}else{
			if(JobDailyRuleCheck( talker, buff2) == TRUE){
				i--;
				return i;
			}
		}
	}
	return -1;
}

//判断任务旗标有无
BOOL JobDailyRuleCheck(int talker, char* buf)
{
	char buf1[20]="";
	char buf2[20]="";
	int  eventNo;

	getStringFromIndexWithDelim( buf, "=", 2, buf1, sizeof( buf1));
	eventNo = atoi(buf1);
	if(eventNo >= 512){
		print("任务日志的任务旗标设定有误%d\n",eventNo);
		return FALSE;
	}
	getStringFromIndexWithDelim( buf, "=", 1, buf2, sizeof( buf2));

	if(strcmp( buf2, "ENDEV") == 0) {
		if(NPC_EventCheckFlg( talker , eventNo) == TRUE){
			return TRUE;
		}
	}

	if(strcmp( buf2, "NOWEV") == 0) {
		if(NPC_EventCheckFlg( talker , eventNo) == TRUE){
			return FALSE;
		}else{
			if(NPC_NowEventCheckFlg( talker , eventNo) == TRUE){
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
#endif

#ifdef _TEACHER_SYSTEM
void CHAR_Teacher_system(int charaindex,char *data)
{
	char szAction[2];
	int i,ix,iy,iPlayerNum,objbuf[16];
	BOOL bHasTeacher = FALSE;
	char szMsg[1024];

	if(!getStringFromIndexWithDelim(data,"|",1,szAction,sizeof(szAction))) return;
	switch(szAction[0]){
		case 'P':
			{
				// 检查自己是否已经有导师了
				if(strlen(CHAR_getChar(charaindex,CHAR_TEACHER_ID)) > 0 && strlen(CHAR_getChar(charaindex,CHAR_TEACHER_NAME)) > 0) bHasTeacher = TRUE;
				// 检查正前方有没有人
				CHAR_getCoordinationDir(CHAR_getInt(charaindex,CHAR_DIR),CHAR_getInt(charaindex,CHAR_X), CHAR_getInt(charaindex,CHAR_Y),1,&ix,&iy);
				// 取得前方玩家数量
				iPlayerNum = CHAR_getSameCoordinateObjects(objbuf,arraysizeof(objbuf),CHAR_getInt(charaindex,CHAR_FLOOR),ix,iy);
				// 没有人
				if(iPlayerNum == 0){
					// 已有导师,显示导师资料
					if(bHasTeacher){
						// 向 ac 要资料
						saacproto_ACCheckCharacterOnLine_send(acfd,charaindex,CHAR_getChar(charaindex,CHAR_TEACHER_ID),
																									CHAR_getChar(charaindex,CHAR_TEACHER_NAME),
																									R_F_TEACHER_SYSTEM);
					}
					// 没有导师,显示说明
					else lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),"M|");
				}
				// 有人
				else{
					// 已有导师
					if(bHasTeacher){
						// 请玩家先取消原本的导师再进行找新导师动作
						CHAR_talkToCli(charaindex,-1,"请先取消原本的导师再找其他人作为你的导师",CHAR_COLORRED);
					}
					else{
						int	objindex,index;
						char szFindName[256];
						
						if(iPlayerNum == 1){
							// 询问是否要对方当你的导师
							objindex = objbuf[0];
							index = OBJECT_getIndex(objindex);
							
							if(OBJECT_getType(objindex) != OBJTYPE_CHARA) break;
							if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) break;
							if(index == charaindex) break;
							// 检查对方是否为自己的学生
							if(strcmp(CHAR_getChar(index,CHAR_TEACHER_ID),CHAR_getChar(charaindex,CHAR_CDKEY)) == 0 &&
								 strcmp(CHAR_getChar(index,CHAR_TEACHER_NAME),CHAR_getChar(charaindex,CHAR_NAME)) == 0){
								CHAR_talkToCli(charaindex,-1,"对方为你的学生，无法让对方成为你的导师",CHAR_COLORRED);
								break;
							}
							sprintf(szMsg,"C|%s|%d",CHAR_getChar(index,CHAR_NAME),index);
							lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),szMsg);
						}
						// 超过一人,询问要找谁当导师
						else{
							int iGetNum = 0;
							char szBuf[1024];

							memset(szBuf,0,sizeof(szBuf));
							for(i=0;i<iPlayerNum && i<5;i++){
								objindex = objbuf[i];
								index = OBJECT_getIndex(objindex);
								
								if(OBJECT_getType(objindex) != OBJTYPE_CHARA) continue;
								if(CHAR_getInt(index,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) continue;
								if(index == charaindex) continue;
								sprintf(szFindName,"%s|%d|",CHAR_getChar(index,CHAR_NAME),index);
								strcat(szBuf,szFindName);
								iGetNum++;
							}
							sprintf(szMsg,"A|%d|%s",iGetNum,szBuf);
							lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),szMsg);
						}
					}
				}
			}
			break;
		// 确定某人为导师
		case 'O':
			{
				char szBuf[16];
				int index;

				getStringFromIndexWithDelim(data,"|",2,szBuf,sizeof(szBuf));
				index = atoi(szBuf);
				if(!CHAR_CHECKINDEX(index)){
					printf("\nCHAR_Teacher_system:error index (%s:%d)\n",CHAR_getChar(charaindex,CHAR_NAME),index);
					sprintf(szMsg,"该玩家不存在(%d)",index);
					CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORRED);
					break;
				}
#ifdef _STU_SYS
				if((CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)>5)){
					CHAR_talkToCli(charaindex,-1,"已经超过拜师条件！",CHAR_COLORRED);
					break;
				}
#endif
				CHAR_setChar(charaindex,CHAR_TEACHER_ID,CHAR_getChar(index,CHAR_CDKEY));
				CHAR_setChar(charaindex,CHAR_TEACHER_NAME,CHAR_getChar(index,CHAR_NAME));
				sprintf(szMsg,"%s 已成为你的学生！",CHAR_getChar(charaindex,CHAR_NAME));
				CHAR_talkToCli(index,-1,szMsg,CHAR_COLORWHITE);
				sprintf(szMsg,"%s 已成为你的导师！",CHAR_getChar(index,CHAR_NAME));
				CHAR_talkToCli(charaindex,-1,szMsg,CHAR_COLORWHITE);
			}
			break;
		// 取消导师资格 已经出师
		case 'C':
			{
				if((CHAR_getInt(charaindex,CHAR_TRANSMIGRATION)==6 && CHAR_getInt(charaindex,CHAR_LV)==160)){
					int j,iPlayernum;
					iPlayernum = CHAR_getPlayerMaxNum();
					for(j=0;j<iPlayernum;j++){
						if(CHAR_getCharUse(j) == FALSE) continue;
						if(strcmp(CHAR_getChar(charaindex,CHAR_TEACHER_ID),CHAR_getChar(j,CHAR_CDKEY)) == 0 &&
				 			 strcmp(CHAR_getChar(charaindex,CHAR_TEACHER_NAME),CHAR_getChar(j,CHAR_NAME)) == 0){				 			 	
								CHAR_setChar(charaindex,CHAR_TEACHER_ID,"");
								CHAR_setChar(charaindex,CHAR_TEACHER_NAME,"");
#ifdef	_STU_SYS								
								CHAR_setInt(j,CHAR_STUNUM,CHAR_getInt(j,CHAR_STUNUM)+1);
								sprintf(szMsg,"你的学生<%s>已经成功出师，你目前为止成功使%d名学生学业有成，加油！",CHAR_getChar(charaindex,CHAR_NAME),CHAR_getInt(j,CHAR_STUNUM));
								CHAR_talkToCli(j,-1,szMsg,CHAR_COLORYELLOW);
								CHAR_talkToCli(charaindex,-1,"你已经成功出师！",CHAR_COLORYELLOW);
								return;
#endif								
						}
					}
				}
				int iCharm = CHAR_getInt(charaindex,CHAR_CHARM) - 30;

				CHAR_setChar(charaindex,CHAR_TEACHER_ID,"");
				CHAR_setChar(charaindex,CHAR_TEACHER_NAME,"");
				// 扣魅力值
				CHAR_setInt(charaindex,CHAR_CHARM,iCharm < 0 ? 0:iCharm);
				CHAR_setWorkInt(charaindex,CHAR_WORKFIXCHARM,iCharm < 0 ? 0:iCharm);
				CHAR_send_P_StatusString(charaindex,CHAR_P_STRING_CHARM);
			}
			break;
		default:printf("\nCHAR_Teacher_system:error command (%s)\n",&szAction[0]);
	}
}

void CHAR_Teacher_system_View(int charaindex,int iOnLine,char *data)
{
	char szMsg[1024];

	// V|导师姓名|在不在线上|所在星系
	sprintf(szMsg,"V|%s|%d|%s",CHAR_getChar(charaindex,CHAR_TEACHER_NAME),iOnLine,data);
	lssproto_TEACHER_SYSTEM_send(getfdFromCharaIndex(charaindex),szMsg);
}
#endif

#ifdef _TIME_TICKET

int check_TimeTicketMap(int floor)
{
	if( floor == 7025) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void check_TimeTicket()
{
	int	i;
	int tickettime;
	int	playernum = CHAR_getPlayerMaxNum();
	int nowtime = time(NULL);
	char msg[1024];

	for( i =0; i <playernum; i++) {
		
		if( !CHAR_CHECKINDEX(i) )	continue;

		if( check_TimeTicketMap( CHAR_getInt( i, CHAR_FLOOR)) == FALSE) {
			CHAR_setWorkInt( i, CHAR_WORKTICKETTIME, 0);
			CHAR_setWorkInt( i, CHAR_WORKTICKETTIMESTART, 0);
			continue;
		}
		
		tickettime = CHAR_getWorkInt( i, CHAR_WORKTICKETTIME);
		// 刚入场
		/*if( tickettime == 0 ) {
			//CHAR_talkToCli( i, -1, "请在１０秒内使用门票。", CHAR_COLORYELLOW);
			//CHAR_setWorkInt( i, CHAR_WORKTICKETTIME, nowtime+10);
		}
		else
		*/
		// 时限20秒以上
		if( tickettime > 0 && tickettime > nowtime+20 ) {
			continue;
		}
		// 时限不到20秒
		else if( tickettime < nowtime+20 && tickettime >= nowtime ) {
			sprintf( msg, "时间票剩余时间%d秒。", tickettime - nowtime);
			CHAR_talkToCli( i, -1, msg, CHAR_COLORYELLOW);
		}

		// 超过时限
		else if( tickettime < nowtime ) {
//			int floor, x, y;
			int totaltime;
			char msg[1024];
			if( CHAR_getWorkInt( i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE) {
				CHAR_talkToCli( i, -1, "时间已到，谢谢光顾。", CHAR_COLORYELLOW);
				if( CHAR_getWorkInt( i, CHAR_WORKTICKETTIMESTART) > 0 ) {
					totaltime = nowtime - CHAR_getWorkInt( i, CHAR_WORKTICKETTIMESTART);
					sprintf( msg, "总入场时间%d分%d秒。", totaltime/60, totaltime%60);
					CHAR_talkToCli( i, -1, msg, CHAR_COLORYELLOW);
				}
				CHAR_setWorkInt( i, CHAR_WORKTICKETTIME, 0);
				CHAR_setWorkInt( i, CHAR_WORKTICKETTIMESTART, 0);
				/*
				if( CHAR_getElderPosition( CHAR_getInt( i, CHAR_LASTTALKELDER), &floor, &x, &y ) != TRUE ) {
					CHAR_warpToSpecificPoint( i, 1006, 15, 15);
				}
				else {
					CHAR_warpToSpecificPoint( i, floor, x, y);
				}
				*/
				CHAR_warpToSpecificPoint( i, 2006, 20, 17);
			}
			else {
				CHAR_talkToCli( i, -1, "时间已到，将於战斗结束时离开。", CHAR_COLORYELLOW);
				// 移至 BATTLE_Exit() 处理
			}
		}
	}

}
#endif

int CharaData( int sockfd, Char* ch ){
	FILE	*fp;
	struct tm *pLtime;
	char szFileName[256], *chardata;
	char outbuff[CHARDATASIZE];
//	Char*   ch;
	int charaindex=CONNECT_getCharaindex(sockfd);
//	print("\n保存运行中的数据\n");
	
	pLtime = localtime( &NowTime.tv_sec );
	
	int hash, dir, j;
	char charId[32];
	char pathname[128];
		
//		strcpy( charId, CHAR_getChar( charaindex, CHAR_CDKEY ) );
	CONNECT_getCdkey( sockfd, charId, sizeof(charId));
	print("账号:%s", charId);
		
	hash = 0;
	for( j=0; j<strlen(charId); j++) {
		hash += (int)charId[j];
		hash = hash % 256;
	}

//		sprintf( pathname, "%s/char/0x%x", getStoredir(), hash);
	sprintf( pathname, "%s/0x%x", getStoredir(), hash);
//	print("文件路径:%s\n", pathname);
	dir = mkdir( pathname, -1);
		
	if( dir != 0 && errno != EEXIST )
		return;
	//print("dir:%d\n", dir);

	sprintf( szFileName,"%s/%s.%d.char",pathname,	charId,	// ID
		CHAR_getInt( charaindex, CHAR_SAVEINDEXNUMBER )
	);
		
	print("\n存储:%s\n", szFileName);

	fp = fopen( szFileName, "w" );
	if( fp == NULL )return;

//		ch = CHAR_getCharPointer( charaindex );
	if( !ch )return;

	chardata = CHAR_makeStringFromCharData( ch );

	if( makeSaveCharString( outbuff , sizeof( outbuff ),
		 CHAR_getChar( charaindex, CHAR_NAME ),
		 CHAR_makeOptionString( ch ), 
		 chardata ) == 0 
	){

		fprintf( fp, outbuff );
		chmod(pathname,0777);
	}else{
//			fprintf( fp, "セーブ失敗  \n" );
	}

	fclose( fp );
#ifdef _CHAR_POOLITEM
	if( CHAR_CheckDepotItem( charaindex ) ){
		sprintf( szFileName, "%s/%s.item", pathname, CHAR_getChar( charaindex, CHAR_CDKEY ));
		if( (fp=fopen( szFileName, "w" )) == NULL )return;
//		print(" 存储:%s\n", szFileName);
		chardata = CHAR_makeDepotItemFromCharIndex( charaindex );
		fprintf( fp, chardata );
		chmod(pathname,0777);
		fclose( fp );
		CHAR_removeDepotItem( charaindex );
	}
#endif

#ifdef _CHAR_POOLPET
	if( CHAR_CheckDepotPet( charaindex ) ){
		sprintf( szFileName, "%s/%s.pet", pathname, CHAR_getChar( charaindex, CHAR_CDKEY ));
		if( (fp=fopen( szFileName, "w" )) == NULL )return;
//		print(" 存储:%s\n", szFileName);
		chardata = CHAR_makeDepotPetFromCharIndex( charaindex );
		fprintf( fp, chardata );
		chmod(szFileName,0777);
		fclose( fp );
		CHAR_removeDepotPet( charaindex );
	}
#endif
	/*
	if( execlp( getStorechar(), "" ) == -1 ) {
		print( " run %s error!:%d\n", getStorechar(), errno );
	}else {
		print( " run %s\n", getStorechar());
	}
	*/
	
	return 0;
}
