#include "version.h"
#include<string.h>
#include<stdlib.h>
#include"char.h"
#include"char_base.h"
#include "battle.h"
#include "pet_skill.h"
#include "battle_event.h"
#include "battle_command.h"
#include "lssproto_serv.h"
#include "magic.h"
#include "magic_base.h"
#include "handletime.h"

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#endif
int NowBattlerFd;
#if 1
int BATTLE_MpDown( int charaindex, int down )
{
	return 0;
}
#else
{
	int mp, battleindex;
	// 戦闘は対人戦か、???戦か
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return TRUE;
	// 対人戦じゃなかったら??
	if( BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P ){
		return TRUE;
	}

	//  ??を減らせるか？
	mp = CHAR_getInt( charaindex, CHAR_MP );
	if( mp < down ){
		// 減らせないので何もしないとする
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
		// コマンドOKの指定
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		return FALSE;
	}
	mp -= down;
	CHAR_setInt( charaindex, CHAR_MP, mp );
	return TRUE;
}
#endif


void BattleCommandDispach( int fd,	char *command )
{
	int endFlg = 0, battleindex;
	int EscapeFree = BATTLE_FLG_FREEDP;
	int charaindex = CONNECT_getCharaindex(fd);
	char szBuffer[256]="";
#ifdef _FIXWOLF	 // Syu ADD 修正狼人变身Bug
	int i , j , pindex;
	BATTLE_ENTRY	*pEntry;
#endif

	NowBattlerFd = fd;

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;

	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
		return;
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ||
		CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){

		return;
	}


	if( BattleArray[battleindex].flg & BATTLE_FLG_CHARALOST ){
		EscapeFree = 0;
	}

	if( strncmp( command, "U", 1 ) == 0 ){
		// shan 2001/12/25 begin
		EscapeFree = 0; 
		if (CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEWATCH )!=TRUE){
			CHAR_talkToCli( charaindex, -1,
                "华义大魔王使出定身法，定定定定定定把你定在战斗里。", CHAR_COLORYELLOW );
            CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD );
            CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
            //sprintf( szBuffer, "Command(%s)(防御)", CHAR_getUseName( charaindex ) );
            endFlg = 1;
			BATTLE_MpDown( charaindex, 2 ); 
		}else{
			BATTLE_WatchStop( charaindex );
		}
		// shan end
	}else
	if( strncmp( command, "E", 1 ) == 0 ){

		if( checkErrorStatus( charaindex) ) {
			BattleCommandDispach( fd, "N");
			return;
		}

		CHAR_setWorkInt( charaindex, CHAR_WORKDBATTLEESCAPE,1 );
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ESCAPE );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		//sprintf( szBuffer, "Command(%s)(逃跑)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

		BATTLE_MpDown( charaindex, 20 );

	}else
	if( strncmp( command, "H|", 2 ) == 0 ){
		int iNum = -1, At_SoubiIndex;
		if( sscanf( command+2, "%X", &iNum ) != 1
		|| ( iNum < 0 || iNum >= 20 )
		){
			iNum = -1;
		}

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, iNum );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );

		At_SoubiIndex =  CHAR_getItemIndex( charaindex, CHAR_ARM );
		if( ITEM_CHECKINDEX( At_SoubiIndex ) == FALSE ){

		}else{

			if( ITEM_getInt( At_SoubiIndex, ITEM_TYPE ) == ITEM_BOOMERANG ){
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_BOOMERANG );
#ifdef _PETSKILL_BECOMEFOX
				if( CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER)==101749
					|| CHAR_getWorkInt( charaindex, CHAR_WORKFOXROUND ) != -1 ){ //若是变成小狐狸不可以使用回力标
				    CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
				}
#endif
#ifdef _PETSKILL_BECOMEPIG
				if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > -1 ){//变成乌力了
				    CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
				}
#endif
			}
		}


		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, 1 );
		//sprintf( szBuffer, "Command(%s)(战斗)", CHAR_getUseName( charaindex ) );

		endFlg = 1;

		BATTLE_MpDown( charaindex, 5 );

	}else
	if( strncmp( command, "G", 1 ) == 0 ){

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_GUARD );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		sprintf( szBuffer, "Command(%s)(防御)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

		BATTLE_MpDown( charaindex, 2 );

	}else
	if( strncmp( command, "N", 1 ) == 0 ){

		EscapeFree = 0;

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
//		sprintf( szBuffer, "Command(%s)(server托管)", CHAR_getUseName( charaindex ) );
		//sprintf( szBuffer, "Command(%s)(待机)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

	}else
	if( strncmp( command, "T|", 2 ) == 0 ){
		int iNum = -1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		if( sscanf( command+2, "%X", &iNum ) != 1
		|| ( iNum < 0 || iNum >= 20 )
		){
			iNum = -1;
		}

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, iNum );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_CAPTURE );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		//sprintf( szBuffer, "Command(%s)(捕获)", CHAR_getUseName( charaindex ) );
		endFlg = 1;

		BATTLE_MpDown( charaindex, 20 );

	}else
	if( strncmp( command, "S|", 2 ) == 0 ){
		int iNum = -1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		// shan 2002/01/08 begin
		if( sscanf( command+2, "%d", &iNum ) != 1
        || ( iNum < -1 || iNum >= CHAR_MAXPETHAVE )
        || ( CHAR_getInt(charaindex,CHAR_RIDEPET)==iNum)
		){
			iNum = -1;
		}
		// shan end

#ifdef _STANDBYPET
#ifndef	_PET_STANDBY
		if( iNum >= 0) {
			if( ! (CHAR_getWorkInt( charaindex, CHAR_WORKSTANDBYPET) & ( 1 << iNum)) ) {
				//print("\n 改封包!叫出非待机宠!!:%s ", CHAR_getChar( charaindex, CHAR_CDKEY) );
				iNum = -1;
			}
		}
#endif		
#endif

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, iNum );
		if( iNum < 0 ){
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_PETIN );
			//sprintf( szBuffer, "Command(%s)(唤回宠物)", CHAR_getUseName( charaindex ) );
		}else{
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_PETOUT );
			//sprintf( szBuffer, "Command(%s)(叫出宠物)", CHAR_getUseName( charaindex ) );
		}

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

		endFlg = 1;

		BATTLE_MpDown( charaindex, 10 );

	}else
	if( strncmp( command, "W|", 2 ) == 0 ){
		int iNum = -1, petskillindex = -1, ToNo = -1, petnum, petindex;
		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) < 1 ){
			iNum = -1; ToNo = -1;
		}
		petnum = CHAR_getInt( charaindex, CHAR_DEFAULTPET );
		petindex = CHAR_getCharPet( charaindex, petnum );
		if( CHAR_CHECKINDEX( petindex ) == FALSE ){
		}else{

			if( iNum < 0 || iNum >= CHAR_MAXPETSKILLHAVE 
#ifdef _PETSKILLBUG  //Add By Syu 2002.0805
				|| (CHAR_getFlg( charaindex, CHAR_ISDIE ) == TRUE )||
				(CHAR_getInt( charaindex, CHAR_HP ) <= 0)
#endif
				|| checkErrorStatus( petindex)
				)
			{
				//sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex ) );
				CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
#ifdef _PETSKILLBUG //Add By Syu 2002.0805
			}else if ((CHAR_getInt(petindex, CHAR_TRANSMIGRATION)<1)
				&& (  iNum>=CHAR_getInt(petindex, CHAR_SLOT) ) ) 
				{
				//sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex ) );
				CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
#endif
			}else{
				petskillindex = PETSKILL_GetArray( petindex, iNum);
#ifdef _FIXWOLF	 // Syu ADD 修正狼人变身Bug
				if( (CHAR_getPetSkill( petindex , iNum )== 600) && (ToNo == -1))
				{
					for( j = 0; j < 2; j ++ )
					{
						pEntry = BattleArray[battleindex].Side[j].Entry;
						for( i = 0; i < BATTLE_ENTRY_MAX; i ++ )
						{
							pindex = pEntry[i].charaindex;
							if( pindex == petindex)
							{
								ToNo=i + ( j * 10 );
								break;
							}
						}
					}
				}
#endif
				if( petskillindex >= 0
				&& PETSKILL_Use( petindex, iNum, ToNo, NULL ) == TRUE
				){
					//sprintf( szBuffer, "Command(%s)(石版)", CHAR_getUseName( petindex ) );

					EscapeFree = 0;

				}else{
					//sprintf( szBuffer, "Command(%s)(自动)", CHAR_getUseName( petindex ) );
					CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
				}
			}
		}
		endFlg = 1;
		BATTLE_MpDown( charaindex, 10 );
	}else
	if( strncmp( command, "J|", 1 ) == 0 ){
		int iNum = -1, magicindex = -1, ToNo = -1;
		int valid = -1;
        int itemindex=-1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;
		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) != 2 ){
			iNum = -1; ToNo = -1;
		}
		if( iNum < CHAR_EQUIPPLACENUM && iNum >= CHAR_HEAD ){
			magicindex = MAGIC_GetArrayNo( charaindex, iNum );
			itemindex = CHAR_getItemIndex( charaindex, iNum );
			valid = MAGIC_isTargetValid( magicindex, ToNo);

			if ((valid <0 ) || ( magicindex < 0 ) || (itemindex < 0 )){
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			}else{
#ifdef _ITEM_ATTSKILLMAGIC
				char *magicarg="\0";
				int magic_array = -1;
				magic_array = MAGIC_getMagicArray( magicindex );
				magicarg = MAGIC_getChar( magic_array, MAGIC_OPTION );
				if( magicarg != "\0" && strstr( magicarg, "SKILL") != NULL )	{
					int mp = ITEM_getInt( itemindex, ITEM_MAGICUSEMP );
					if( MAGIC_AttSkill( charaindex, ToNo, magic_array, mp) == FALSE )	{
						CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
						CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
					}
				}else	{
#endif
					CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, ToNo );
					CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_JYUJYUTU );
					CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, magicindex );
					CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, iNum );
					CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
					//sprintf( szBuffer, "Command(%s)(咒术)", CHAR_getUseName( charaindex ) );
#ifdef _ITEM_ATTSKILLMAGIC
				}
#endif
			}
		}else{
			EscapeFree = 0;
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			//sprintf( szBuffer, "Command(%s)(待机)", CHAR_getUseName( charaindex ) );
		}
		endFlg = 1;
	}else
	if( strncmp( command, "I|", 1 ) == 0 ){
		int iNum = -1, ToNo = -1;
		int valid = 0, itemindex = -1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) != 2 ){
			iNum = -1; ToNo = -1;
		}
		itemindex = CHAR_getItemIndex( charaindex, iNum );
		if ( !ITEM_CHECKINDEX( itemindex) ) valid=-1;
		if (valid >= 0) valid = ITEM_isTargetValid( charaindex, itemindex, ToNo );
		if (valid < 0 ) {
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		} else {
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, ToNo );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ITEM );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM3, iNum );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			//sprintf( szBuffer, "Command(%s)(道具)", CHAR_getUseName( charaindex ) );
			endFlg = 1;
		}
	}else
	if( strncmp( command, "@", 1 ) == 0 ){
		//sprintf( szBuffer, "Command(%s)(通用)", CHAR_getUseName( charaindex ) );

		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		endFlg = 1;

		CHAR_setInt( charaindex, CHAR_MP,
			min( 100, CHAR_getInt( charaindex, CHAR_MP ) + 30 ) );

	}else
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( strncmp( command, "P", 1 ) == 0 ){
		int iNum=-1, ToNo=-1;
		int skillindex=-1;
		int char_pskill=-1, profession_skill=-1, Pskillid=-1;

		if( checkErrorStatus( charaindex) )	{
			BattleCommandDispach( fd, "N");
			return;
		}

		EscapeFree = 0;

		if( sscanf( command+2, "%X|%X", &iNum, &ToNo ) < 1 ){
			iNum = -1; ToNo = -1;
		}

		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE ){
//			print("\n\n 非法封包(19001)(%s)(%s)\n\n", CHAR_getChar(charaindex, CHAR_CDKEY), CHAR_getChar(charaindex, CHAR_NAME) );
#ifdef _PROSKILL_ERR_KICK
			CONNECT_setCloseRequest( fd , 1 );
#endif
			return;
		}
#ifndef _PROSKILL_OPTIMUM	// Robin fix cancel 此处略过职业检查, 改在 PROFESSION_SKILL_Use 中检查
		// 人物的职业
		char_pskill = CHAR_getInt( charaindex, PROFESSION_CLASS );
		// 技能的职业
		skillindex = PROFESSION_SKILL_GetArray( charaindex, iNum);
		Pskillid = PROFESSION_SKILL_getskillArray( skillindex);
		profession_skill = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);

		if( (char_pskill > 0) && (char_pskill == profession_skill) ){
#else
		if( 1 ) {
#endif
			if( PROFESSION_SKILL_Use( charaindex, iNum, ToNo, NULL ) == 1 ){
				//sprintf( szBuffer, "Command(%s)(职业技能)", CHAR_getUseName( charaindex ) );
				endFlg = 1;
			}else{
//				print("\n职业技能失败!!\n");
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
				//sprintf( szBuffer, "Command(%s)(待机)", CHAR_getUseName( charaindex ) );
#ifdef _PROSKILL_ERR_KICK
			CONNECT_setCloseRequest( fd , 1 );
#endif
			}
		}else{
//			print("\n改封包??职业技能ID不正确:%s:%d:%d \n",	CHAR_getChar( charaindex, CHAR_CDKEY), char_pskill, profession_skill);
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_WAIT );
			CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
			//sprintf( szBuffer, "Command(%s)(待机)", CHAR_getUseName( charaindex ) );
#ifdef _PROSKILL_ERR_KICK
			CONNECT_setCloseRequest_debug( fd , 1 );
#endif
		}

		EscapeFree = 0;		
	}else
#endif

	{
		//sprintf( szBuffer, "Command(%s)(失败)", CHAR_getUseName( charaindex ) );
		endFlg = 2;
	}


	if( endFlg ){
		BATTLE_ActSettingSend( battleindex );
		
		if( endFlg==1 ){
			int fd;
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE)==CHAR_TYPEPLAYER ){
				fd = CHAR_getWorkInt( charaindex, CHAR_WORKFD);	
				CONNECT_SetBattleRecvTime( fd, &NowTime);				
			}
		}
	}

	{
		//if( BATTLE_CHECKINDEX( battleindex ) == TRUE ){
		//	BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORWHITE ) ;
		//}		
	}

	if( BattleArray[battleindex].turn == 0 ){
		BattleArray[battleindex].flg |= EscapeFree;
	}else{
		BattleArray[battleindex].flg &= EscapeFree;
	}
}


void BATTLE_ActSettingSend( int battleindex )
{
	int pindex, k = 0, i, j, charaindex;
	int endBit = 0;
	char szBA[256];
	BATTLE_ENTRY	*pEntry;
	BATTLE	*pBattle;

	for( j = 0; j < 2; j ++ ){
		pEntry = BattleArray[battleindex].Side[j].Entry;
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			pindex = pEntry[i].charaindex;
			// コマンド入力できていたら
			if( CHAR_getWorkInt( pindex, CHAR_WORKBATTLEMODE )
				== BATTLE_CHARMODE_C_OK
			){
				endBit |= 1 << k;	// ビット立てる
			}else
			// 敵だったら絶対動かす
			if( CHAR_getInt( pindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY || 
				  CHAR_getInt( pindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYERNPC || 
				  CHAR_getInt( pindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYERPETNPC )
			{
				endBit |= 1 << k;	//  ビット立てる
			}
			k ++;
		}
	}
	sprintf( szBA, "BA|%X|%X|", endBit, BattleArray[battleindex].turn );
//	print( "%s\n", szBA );
	//さらに全員ループして送る
	for( j = 0; j < 2; j ++ ){
		pEntry = BattleArray[battleindex].Side[j].Entry;
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			pindex = pEntry[i].charaindex;
			if( CHAR_CHECKINDEX( pindex ) == FALSE )continue;
			// 参戦したてのキャラには送らない
			if( CHAR_getWorkInt( pindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_RESCUE )continue;
			// プレイヤーだったら
			if( CHAR_getInt( pindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
			){	// コマンド送る
				BATTLE_CommandSend( pindex, szBA );
			}
		}
	}


	//  観戦している人にも送る
	pBattle = BattleArray[battleindex].pNext;
	for( ; pBattle ; pBattle = pBattle->pNext ){
		
		// このアドレスが正しいかチェック
		if( BATTLE_CHECKADDRESS( pBattle ) == FALSE ){
			//fprint( "err:观战battle address错误(%p)\n", pBattle );
			break;
		}
		// 全員に送信
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			charaindex = pBattle->Side[0].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			// コマンド送る
			BATTLE_CommandSend( pindex, szBA );
		}
	}
}



BOOL BATTLE_IsHide( int charaindex ){
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_EARTHROUND0 
		){
		return TRUE;
	}
	return FALSE;
}





//*******************************************************
//
//  バトルのコマンドをクライアントに送る
//
BOOL BATTLE_CommandSend( int charaindex, char *pszCommand)
//
//********************************************************
{
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;
	if(	getfdFromCharaIndex( charaindex ) < 0 )return FALSE;
	lssproto_B_send( getfdFromCharaIndex( charaindex ), pszCommand );	
	return TRUE;
}


BOOL BATTLE_MakeCharaString(
	int battleindex,
	char *pszCommand,
	int	size
)
{
	int i, j, iOffset, charaindex, flg;
	BATTLE_ENTRY	*pEntry;
	char szBuffer[4096]="", *pszTop, *pszLast;
	char szEscapeName[128], szEscapeTitle[128];
	char szEscapePetName[128];
	int rideflg = 0, petindex = -1, petlevel, pethp, petmaxhp;
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return FALSE;
	pszTop = pszCommand;
	pszLast = pszCommand+size-1;
#if 1
	sprintf( szBuffer, "BC|%X|", BattleArray[battleindex].field_att  );
#else
	sprintf( szBuffer, "BC|" );
#endif
	STRCPY_TAIL( pszTop, pszLast, szBuffer );

	for( j = 0; j < 2; j ++ ){
		if( j == 1 ){
			iOffset = SIDE_OFFSET;	// 向こう側サイドの場合はプラスする
		}else{
			iOffset = 0;
		}
		pEntry = BattleArray[battleindex].Side[j].Entry;
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			charaindex = pEntry[i].charaindex;
			if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLG )
				& CHAR_BATTLEFLG_ULTIMATE )continue;
			flg = 0;
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
				== BATTLE_CHARMODE_INIT ) flg |= BC_FLG_NEW;
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
				== BATTLE_CHARMODE_RESCUE ) flg |= BC_FLG_NEW;
			if( CHAR_getFlg( charaindex, CHAR_ISDIE ) == TRUE ){
				flg |= BC_FLG_DEAD;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKPOISON ) > 0 ){
				flg |= BC_FLG_POISON;
			}else
#ifdef _MAGIC_WEAKEN      //   虚弱
			if( CHAR_getWorkInt( charaindex, CHAR_WORKWEAKEN ) > 0 ){
				flg |= BC_FLG_WEAKEN;
			}else 
#endif
#ifdef _MAGIC_DEEPPOISON  //   剧毒
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDEEPPOISON ) > 0 ){
				flg |= BC_FLG_DEEPPOISON;
			}else
#endif
#ifdef _MAGIC_NOCAST     //   沉默
			if( CHAR_getWorkInt( charaindex, CHAR_WORKNOCAST ) > 0 ){
				flg |= BC_FLG_NOCAST;
			}else
#endif
#ifdef _MAGIC_BARRIER	  //   魔障
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBARRIER ) > 0 ){
				flg |= BC_FLG_BARRIER;
			}else 
#endif
			if( CHAR_getWorkInt( charaindex, CHAR_WORKPARALYSIS ) > 0 ){
				flg |= BC_FLG_PARALYSIS;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSLEEP ) > 0 ){
				flg |= BC_FLG_SLEEP;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSTONE ) > 0 ){
				flg |= BC_FLG_STONE;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDRUNK ) > 0 ){
				flg |= BC_FLG_DRUNK;
			}else
			if( CHAR_getWorkInt( charaindex, CHAR_WORKCONFUSION ) > 0 ){
				flg |= BC_FLG_CONFUSION;
			}
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
				flg |= BC_FLG_PLAYER;
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_REVERSE ){
				flg |= BC_FLG_REVERSE;
			}
			if( BATTLE_IsHide( charaindex ) == TRUE ){
				flg |= BC_FLG_HIDE;
			}
#ifdef _PET_SKILL_SARS			// WON ADD 毒煞蔓延
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSARS ) > 0 ){
				flg |= BC_FLG_SARS;
			}
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDIZZY ) > 0 ){
				flg |= BC_FLG_DIZZY;			// 晕眩
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKENTWINE ) > 0 ){
				flg |= BC_FLG_ENTWINE;			// 树根缠绕
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKDRAGNET ) > 0 ){
				flg |= BC_FLG_DRAGNET;			// 天罗地网
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKICECRACK ) > 0 ){
			//	flg |= BC_FLG_ICECRACK;			// 冰爆术
			}			
			if( CHAR_getWorkInt( charaindex, CHAR_WORKOBLIVION ) > 0 ){
				flg |= BC_FLG_OBLIVION;			// 遗忘
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKICEARROW ) > 0 ){
				flg |= BC_FLG_ICEARROW;			// 冰箭
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBLOODWORMS ) > 0 ){
				flg |= BC_FLG_BLOODWORMS;		// 嗜血蛊
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKSIGN ) > 0 ){
				flg |= BC_FLG_SIGN;				// 一针见血
			}		
			if( CHAR_getWorkInt( charaindex, CHAR_WORKINSTIGATE ) > 0 ){
				flg |= BC_FLG_CARY;				// 挑拨
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORK_F_ENCLOSE ) > 0 ){
				flg |= BC_FLG_F_ENCLOSE;		// 火附体
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORK_I_ENCLOSE ) > 0 ){
				flg |= BC_FLG_I_ENCLOSE;		// 冰附体
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORK_T_ENCLOSE ) > 0 ){
				flg |= BC_FLG_T_ENCLOSE;		// 雷附体
			}
#endif
#ifdef _PROFESSION_ADDSKILL
			if( CHAR_getWorkInt( charaindex, CHAR_WORKANNEX ) > 0 ){
				flg |= BC_FLG_T_ENCLOSE;		// 附身术
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKWATER ) > 0 ){
				flg |= BC_FLG_WATER;		// 水附体
			}
			if( CHAR_getWorkInt( charaindex, CHAR_WORKFEAR ) > 0 ){
				flg |= BC_FLG_FEAR;		// 恐惧
			}
#endif
#ifdef _PETSKILL_LER
			if(CHAR_getWorkInt(charaindex,CHAR_WORK_RELIFE) > 0){
				flg |= BC_FLG_CHANGE;		// 雷尔变身
				CHAR_setWorkInt(charaindex,CHAR_WORK_RELIFE,0);
			}
#endif
			makeEscapeString( CHAR_getUseName( charaindex ),
				szEscapeName, 
				sizeof( szEscapeName ) );
			makeEscapeString( BATTLE_CharTitle( charaindex ),
				szEscapeTitle, 
				sizeof( szEscapeTitle ) );
 // Robin 0728 ride Pet
			petindex = BATTLE_getRidePet( charaindex );
			if( petindex != -1 ){
				rideflg = 1;
				makeEscapeString( CHAR_getUseName( petindex ),
					szEscapePetName, 
					sizeof( szEscapePetName ) );
				petlevel = CHAR_getInt( petindex, CHAR_LV);
				pethp = CHAR_getInt( petindex, CHAR_HP);
				petmaxhp = CHAR_getWorkInt( petindex, CHAR_WORKMAXHP);
				
			}else {
				if( CHAR_getWorkInt( charaindex, CHAR_WORKPETFALL ) == 1 ){
					rideflg = -1;
#ifdef _PETSKILL_BECOMEFOX // 若中了媚惑术而落马时将讯息传给 Client
					if( CHAR_getWorkInt( charaindex, CHAR_WORKFOXROUND ) != -1 )
						rideflg = -2; //client用来判断显示落马的文字 (对应的client变数为p_party[ BattleMyNo ]->onRide)
#endif
#ifdef _PETSKILL_BECOMEPIG // 若中了乌力化而落马时将讯息传给 Client
					if( CHAR_getInt( charaindex, CHAR_BECOMEPIG) > 120 )//变成乌力了
						rideflg = -3; //client用来判断显示落马的文字 (对应的client变数为p_party[ BattleMyNo ]->onRide)
#endif
					 CHAR_setWorkInt( charaindex, CHAR_WORKPETFALL, 0 );
				}
				else
					rideflg = 0;
				szEscapePetName[0] = 0;
				petlevel = 0;
				pethp = 0;
				petmaxhp = 0;
			}

			sprintf( szBuffer, "%X|%s|%s|%X|%X|%X|%X|%X|%X|%s|%X|%X|%X|",
				pEntry[i].bid,
				szEscapeName,
				szEscapeTitle,
				CHAR_getInt( charaindex, CHAR_BASEIMAGENUMBER ),
				CHAR_getInt( charaindex, CHAR_LV ),
				max( CHAR_getInt( charaindex, CHAR_HP ),0),
				CHAR_getWorkInt( charaindex, CHAR_WORKMAXHP ),
				flg,
				rideflg,
				szEscapePetName,
				petlevel,
				pethp,
				petmaxhp
			);
			STRCPY_TAIL( pszTop, pszLast, szBuffer );
			if( pszTop >= pszLast )return FALSE;// 超えたら失敗
		}
	}

	return TRUE;
}

void BATTLE_BpSendToWatch(
	BATTLE *pBattle, 	// 観戦バトルタスクのポインタ
	char *pszBcString 	//  BC文字列
)
{

	char szBp[256];
	int flg = 0, i, charaindex;

//	for( ; pBattle ; pBattle = pBattle->pNext ){
	if( pBattle == NULL )return;
	// このアドレスが正しいかチェック
	if( BATTLE_CHECKADDRESS( pBattle ) == FALSE ){
		//fprint( "err:观战battle address错误(%p)\n", pBattle );
		return;
	}

	//  全員に送信
	for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
		charaindex = pBattle->Side[0].Entry[i].charaindex;
		if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) 
			!= CHAR_TYPEPLAYER ) continue;
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
			== BATTLE_CHARMODE_WATCHINIT){
			flg |= BP_FLG_JOIN;	// 新規参加
		}
		flg |= BP_FLG_PLAYER_MENU_OFF;	// プレイヤーメニュー出さない
		// プレイヤー番号送る
		sprintf( szBp, "BP|%X|%X|%X", 20, flg, 0 );
		BATTLE_CommandSend( charaindex, szBp );
		// 全員共通情報送る
		BATTLE_CommandSend( charaindex, pszBcString );
	}
	// コマンド待ちへ
	pBattle->mode = BATTLE_MODE_WATCHPRE;
//	}
}

void BATTLE_CharSendAll( int battleindex )
{
	int i, j, charaindex, flg, pet, At_SoubiIndex, pindex;
	char szBp[256];
	BATTLE	*pBattle;

	BATTLE_MakeCharaString(
		battleindex,
		szAllBattleString,
		sizeof(szAllBattleString)
	);

	for( j = 0; j < 2; j ++ ){
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			charaindex = BattleArray[battleindex].Side[j].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( charaindex ) == FALSE )continue;
			if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			flg = 0;
			if( BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE ){
				flg |= BP_FLG_ENEMY_SURPRISAL;
			}else
				if( BattleArray[battleindex].Side[1-j].flg & BSIDE_FLG_SURPRISE ){
					flg |= BP_FLG_PLAYER_SURPRISAL;
				}
				if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE )
					== BATTLE_CHARMODE_RESCUE){
					flg |= BP_FLG_JOIN;		// 参戦
				}
			At_SoubiIndex = CHAR_getItemIndex( charaindex, CHAR_ARM );
			if( ITEM_CHECKINDEX( At_SoubiIndex ) == FALSE ){
			}else{
				if( ITEM_getInt( At_SoubiIndex, ITEM_TYPE ) == ITEM_BOOMERANG ){
					flg |= BP_FLG_BOOMERANG;
				}
			}

			pet = CHAR_getInt( charaindex, CHAR_DEFAULTPET );
			if( pet >= 0 ){
				snprintf( szBp, sizeof( szBp ), "K%d", pet );
				CHAR_sendStatusString( charaindex, szBp );
				
				pindex = CHAR_getCharPet( charaindex, pet );

#ifdef _PETSKILL_DAMAGETOHP
	{

		char msg[32]={0};
		//print("\n宠物id:%d",CHAR_getInt( pindex, CHAR_PETID));
		//print("\n宠物名:%s",CHAR_getChar( pindex, CHAR_NAME));
		//if( CHAR_getInt( pindex, CHAR_PETID) == 777 ){//宠物ID 水双头狼
		//	  || CHAR_getInt( pindex, CHAR_PETID) == 146 ) //火双头狼
		//	&& CHAR_getInt( pindex, CHAR_HP) ){
		    sprintf( msg, "o%d", pet );
		    CHAR_sendStatusString( charaindex, msg );
			
		//}
	} 
#endif

#ifdef _PETSKILL_BECOMEFOX // 限制中了媚惑术的宠物的技能
		    //if( CHAR_getInt( pindex, CHAR_BASEIMAGENUMBER) == 101749 ){
	        if( CHAR_getWorkInt( pindex, CHAR_WORKFOXROUND ) != -1 ){ //若是变成小狐狸
			    char msg[32]={0};
				sprintf( msg, "a%d", pet );
		        CHAR_sendStatusString( charaindex, msg );
				
			}
#endif

				if( CHAR_CHECKINDEX( pindex ) == TRUE ){
					if( BATTLE_IsCharge( pindex  ) == TRUE){
						flg |= BP_FLG_PET_MENU_OFF;
					}
					if( BATTLE_CanMoveCheck( pindex ) == FALSE ){
						flg |= BP_FLG_PET_MENU_OFF;
					}
					if( CHAR_getFlg( pindex, CHAR_ISDIE ) == TRUE ){
						flg |= BP_FLG_PET_MENU_OFF;
					}
				}
			}else{
				flg |= BP_FLG_PET_MENU_OFF;
			}

			if( BATTLE_CanMoveCheck( charaindex ) == FALSE
				|| CHAR_getFlg( charaindex, CHAR_ISDIE ) == TRUE
			){
				flg |= BP_FLG_PLAYER_MENU_OFF;
			}

			if( BattleArray[battleindex].Side[j].flg & BSIDE_FLG_SURPRISE ){
				flg |= BP_FLG_PLAYER_MENU_OFF;
				flg |= BP_FLG_PET_MENU_OFF;
			}
			sprintf( szBp, "BP|%X|%X|%X",
				i + SIDE_OFFSET * j,
				flg, CHAR_getInt( charaindex, CHAR_MP )
			);
			BATTLE_CommandSend( charaindex, szBp );
			BATTLE_CommandSend( charaindex, szAllBattleString );
		}
	}
	pBattle = BattleArray[battleindex].pNext;
	for( ; pBattle ; pBattle = pBattle->pNext ){
		// このアドレスが正しいかチェック
		if( BATTLE_CHECKADDRESS( pBattle ) == FALSE ){
			//fprint( "err:观战battle address错误(%p)\n", pBattle );
			break;
		}

		BATTLE_BpSendToWatch( pBattle, szAllBattleString );

	}



}

void BattleEncountOut( int charaindex)
{

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEINDEX, -1 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_MODE_NONE );

    CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX ) );
    CHAR_sendArroundCharaData( charaindex );

	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEWATCH ) == 0 ){
		CHAR_sendBattleEffect( charaindex, OFF);
	}else{
		CHAR_sendBattleWatch( CHAR_getWorkInt( charaindex, CHAR_WORKOBJINDEX ), OFF);
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEWATCH, 0 );
	}
   
	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_CLIENT ){
		int pmode/*, battleindex*/;
		int pindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1 );
		pmode = CHAR_getWorkInt( pindex, CHAR_WORKBATTLEMODE );
		if( pmode > 0 && pmode != BATTLE_CHARMODE_FINAL ){
				BATTLE_RescueParentTry( charaindex, pindex );
//				print( "较慢参加战斗(%s)\n", CHAR_getUseName( charaindex ) );
		}
	}

}


//**************************************************
//
// デフォルトのコマンドを入れるペット用
//
BOOL	BATTLE_PetDefaultCommand( int petindex )
//
//**************************************************
{
	if( CHAR_CHECKINDEX( petindex ) == FALSE )return FALSE;
	// 相手の指定
	CHAR_setWorkInt( petindex, CHAR_WORKBATTLECOM2, -1 );
	// 通常攻撃の指定
	CHAR_setWorkInt( petindex, CHAR_WORKBATTLECOM1, BATTLE_COM_ATTACK );
	// コマンドOKの指定
	CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	return TRUE;
}

// Robin fix 检查石化昏睡不可战斗
int checkErrorStatus( int charaindex)
{
	//if( BATTLE_CanMoveCheck( charaindex) == FALSE) {

	if( 
		CHAR_getWorkInt( charaindex, CHAR_WORKPARALYSIS ) > 0	// 麻痹
		|| CHAR_getWorkInt( charaindex, CHAR_WORKSTONE ) > 0	// 石化
		|| CHAR_getWorkInt( charaindex, CHAR_WORKSLEEP ) > 0	// 睡眠
		//|| CHAR_getWorkInt( charaindex, CHAR_WORKBARRIER ) > 0	// 魔障
		|| CHAR_getWorkInt( charaindex, CHAR_WORKDIZZY ) > 0	// 晕眩
		|| CHAR_getWorkInt( charaindex, CHAR_WORKDRAGNET ) > 0	// 天罗地网
		//|| CHAR_getWorkInt( charaindex, CHAR_WORK_T_ENCLOSE ) > 0 // 雷附体
#ifdef _PROFESSION_ADDSKILL
//		|| CHAR_getWorkInt( charaindex, CHAR_DOOMTIME ) > 0 //世界末日集气
#endif
	)
	{

//		char cdkey[128];
//		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER)
//			strcpysafe( cdkey,sizeof(cdkey), CHAR_getChar( charaindex, CHAR_CDKEY));
//		else
//			strcpysafe( cdkey,sizeof(cdkey), CHAR_getChar( charaindex, CHAR_OWNERCDKEY) );
	
//		print("\n 改封包!不可战斗的状态!!:%s ", cdkey );

		return 1;
	}
	return 0;
}

