#include "version.h"
#include <string.h>
#include "char.h"
#include "char_base.h"
#include "battle.h"
#include "battle_event.h"
#include "battle_item.h"
#include "battle_magic.h"
#include "item_event.h"
#include "log.h"
#include "anim_tbl.h"
#include "npcutil.h"
#include "magic_base.h"
#include "lssproto_serv.h"


int BATTLE_ItemUseDelete(
	int charaindex,
	int haveitemindex
)
{
	int itemindex;

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if( ITEM_CHECKINDEX( itemindex ) == FALSE ) return 0;
	{
//		LogItem(
//			CHAR_getChar( charaindex, CHAR_NAME ),
//			CHAR_getChar( charaindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//			itemindex,
//#else
//       		ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//			"BattleUse(战斗中使用掉的道具)",
//	       	CHAR_getInt( charaindex,CHAR_FLOOR),
//			CHAR_getInt( charaindex,CHAR_X ),
//        	CHAR_getInt( charaindex,CHAR_Y ),
//			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//		);
	}
	CHAR_DelItemMess( charaindex, haveitemindex, 0);

	return 0;
}

#ifdef _IMPRECATE_ITEM
void ITEM_useImprecate( int charaindex, int toNo, int haveitemindex )
{
	int i;
	int battleindex, attackNo,itemindex;
	char *arg;
	char buf[256];

	struct tagImprecate {
		char fun[256];
		int intfun;
	};
	struct tagImprecate ImList[3] ={
		{"咒",BD_KIND_CURSE},{"恩",BD_KIND_BESTOW},{"祝",BD_KIND_WISHES} };

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	if( (attackNo = BATTLE_Index2No( battleindex, charaindex )) < 0 ){
		//print( "ANDY attackNo=%d\n", attackNo);
		return;
	}
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( arg == "\0" ){
		//print( "ANDY ITEM id:%d=>arg err\n", ITEM_getInt( itemindex, ITEM_ID));
		return;
	}

	for( i=0; i<3; i++)	{
		if( strstr( arg, ImList[i].fun ) != 0 )	{
			char buf1[256];
			int kind,powers, rounds, HealedEffect;

			if( NPC_Util_GetStrFromStrWithDelim( arg, ImList[i].fun, buf, sizeof( buf)) == NULL )continue;
			kind = ImList[i].intfun;
			if( getStringFromIndexWithDelim( buf,"x", 1, buf1, sizeof( buf1)) == FALSE ) continue;
			powers = atoi( buf1);
			if( getStringFromIndexWithDelim( buf,"x", 2, buf1, sizeof( buf1)) == FALSE ) continue;
			rounds = atoi( buf1);
			HealedEffect = SPR_hoshi;
			BATTLE_ImprecateRecovery(
				battleindex, attackNo, toNo, kind, powers,
				rounds, SPR_item3, HealedEffect );
			CHAR_setItemIndex(charaindex, haveitemindex ,-1);
			CHAR_sendItemDataOne( charaindex, haveitemindex);
			ITEM_endExistItemsOne( itemindex );
			break;
		}
	}
}
#endif

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery_Battle( int charaindex, int toNo, int haveitemindex )
{
	int battleindex, attackNo, itemindex;
	int turn=0, magicid, marray;
	char buf1[256];
	char *arg=NULL;

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return;
	if( (attackNo =  BATTLE_Index2No( battleindex, charaindex )) == -1 ) return;

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar( itemindex, ITEM_ARGUMENT );


	if( getStringFromIndexWithDelim( arg, ":", 2, buf1, sizeof(buf1)) ==FALSE ) return;
	turn = atoi( buf1);
	if( getStringFromIndexWithDelim( arg, ":", 1, buf1, sizeof(buf1)) ==FALSE ) return;

	if( strstr( buf1, "全" ) != NULL ){
	}else{
	}

	magicid = ITEM_getInt( itemindex, ITEM_MAGICID);
	marray = MAGIC_getMagicArray( magicid);
	if( marray == -1 ) return;


	if( IsBATTLING( charaindex ) == TRUE ){
		int i, status=-1;
		char *magicarg=NULL, *pszP=NULL;
#ifdef _CAX_ZHUANGYUAN_QTJ		
	int floor = CHAR_getInt( charaindex, CHAR_FLOOR );
	int fd;
	fd = getfdFromCharaIndex( charaindex);
	   if( floor == 1042
		  || floor == 2032
		  || floor == 3032
		  || floor == 4032
		  || floor == 5032
		  || floor == 6032
		  || floor == 7032
		  || floor == 8032
	    || floor == 9032
	    || floor == 10032
	    || floor == 2000
	    || floor == 2005){	
	    int battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );		
	    BATTLE_NoAction( battleindex, BATTLE_Index2No( battleindex, charaindex) );				
     CHAR_talkToCli(charaindex,-1,"此地图禁止使用军火(全体多次镜光守)",CHAR_COLORYELLOW);
       return;
  	}
#endif
#ifdef _PREVENT_TEAMATTACK //光镜守..不得使用敌方
		int battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
			//&& BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P 
			){
			if( BATTLE_CheckSameSide( charaindex, toNo) == 0 ){//不同边
				int battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
				BATTLE_NoAction( battleindex, BATTLE_Index2No( battleindex, charaindex) );
				CHAR_talkToCli( charaindex, -1, "光镜守..不得施予非玩家敌方。", CHAR_COLORYELLOW);
				return;
			}
		}
#endif
		
		magicarg = MAGIC_getChar( marray, MAGIC_OPTION );
		pszP = magicarg;
		for( ;status == -1 && pszP[0] != 0; pszP++ ){
			for( i = 1; i < BATTLE_MD_END; i ++ ){
				if( strncmp( pszP, aszMagicDef[i], 2 ) == 0 ){
					status = i;
					pszP +=2;
					break;
				}
			}
		}
		if( status == -1 ) return;

		BATTLE_MultiMagicDef( battleindex, attackNo, toNo,
			status, turn, MAGIC_EFFECT_USER, SPR_difence );

//		LogItem(
//			CHAR_getChar( charaindex, CHAR_NAME ),
//			CHAR_getChar( charaindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//			itemindex,
//#else
//       		ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//			"BattleUse(战斗中使用掉的道具)",
//	       	CHAR_getInt( charaindex,CHAR_FLOOR),
//			CHAR_getInt( charaindex,CHAR_X ),
//        	CHAR_getInt( charaindex,CHAR_Y ),
//			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//		);
	}

	CHAR_DelItemMess( charaindex, haveitemindex, 0);
}
#endif

#ifdef _ITEM_USEMAGIC
void ITEM_useMagic_Battle( int charaindex, int toNo, int haveitemindex )
{
	int itemindex,itemmaxuse;
    char *arg=NULL;
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar( itemindex, ITEM_ARGUMENT );

	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_JYUJYUTU );
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, atoi(arg) );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, 0 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );

	MAGIC_DirectUse(
				charaindex,
				CHAR_GETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3 ),
				CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM2 ),
				CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3 )
	);

	itemmaxuse = ITEM_getInt( itemindex, ITEM_DAMAGEBREAK);
	if( itemmaxuse > 0 )
	    ITEM_setInt( itemindex, ITEM_DAMAGEBREAK, itemmaxuse-1 );
    else{
//		LogItem(
//			CHAR_getChar( charaindex, CHAR_NAME ),
//			CHAR_getChar( charaindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//			itemindex,
//#else
//       		ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//			"BattleUse(战斗中使用掉的道具)",
//	       	CHAR_getInt( charaindex,CHAR_FLOOR),
//			CHAR_getInt( charaindex,CHAR_X ),
//        	CHAR_getInt( charaindex,CHAR_Y ),
//			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//		);

	    CHAR_DelItemMess( charaindex, haveitemindex, 0);
	}
}
#endif

void ITEM_useRecovery_Battle( int charaindex, int toNo, int haveitemindex )
{
	int power = 0, per = 0, HealedEffect=0;
	int battleindex, attackNo,itemindex, kind = BD_KIND_HP;
	char *p = NULL, *arg;
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	if( (p = strstr( arg, "体" )) != NULL )
	{
		kind = BD_KIND_HP;
	}
	else if( (p = strstr( arg, "气" )) != NULL )
	{
		kind = BD_KIND_MP;
	}
	else
#ifdef _ITEM_UNBECOMEPIG
    if( (p = strstr( arg, "解猪" )) != NULL ){
        kind = BD_KIND_UNBECOMEPIG;
		HealedEffect = 100608; //特效编号
	}
	else
#endif
#ifdef _ITEM_LVUPUP
	if( (p = strstr( arg, "LVUPUP" )) != NULL ){
	    return;
	}
	else
#endif
#ifdef _ITEM_PROPERTY
    if( (p = strstr( arg, "PROPERTY" )) != NULL ){
	    kind = BD_KIND_PROPERTY;
		HealedEffect = 100608; //特效编号
		if( strstr( arg, "+" ) )
			power = 1;
		else if( strstr( arg, "-" ) )
			power = 2;
	}
	else
#endif
#ifdef _ITEM_ADDPETEXP
    if( (p = strstr( arg, "GETEXP" )) != NULL ){
        return;
	}
	else
#endif
		return;

#ifdef _ITEM_UNBECOMEPIG
    if( HealedEffect != 100608 ){
#endif
	    if( sscanf( p+2, "%d", &power ) != 1 )
		    power = 0;
	    if( power <= 100 )
		    HealedEffect = SPR_heal;//SPR_hoshi
		else if( power <= 300 )
		    HealedEffect = SPR_heal2;
	    else
		    HealedEffect = SPR_heal3;
#ifdef _ITEM_UNBECOMEPIG
	}
#endif

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );
	if( attackNo < 0 )return;
	BATTLE_MultiRecovery( battleindex, attackNo, toNo,kind, power, per, SPR_item3, HealedEffect );
	{
//		LogItem(
//			CHAR_getChar( charaindex, CHAR_NAME ),
//			CHAR_getChar( charaindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//			itemindex,
//#else
//       		ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//			"BattleUse(战斗中使用掉的道具)",
//	       	CHAR_getInt( charaindex,CHAR_FLOOR),
//			CHAR_getInt( charaindex,CHAR_X ),
//        	CHAR_getInt( charaindex,CHAR_Y ),
//			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//		);
	}
	CHAR_DelItemMess( charaindex, haveitemindex, 0);
}

void ITEM_useStatusChange_Battle(
	int charaindex,
	int toNo,
	int haveitemindex
){
	int turn = 0, i;
	int battleindex, attackNo,itemindex, status = -1, Success = 15;
	int ReceveEffect;
	char *pszP = NULL, *arg;
	char szTurn[] = "turn";
	char szSuccess[] = "成";

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	pszP = arg;
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 0; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	if( status == -1 ) return ;
	if( ( pszP = strstr( arg, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	if( ( pszP = strstr( arg, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo = BATTLE_Index2No( battleindex, charaindex );

	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;
	}else{
		ReceveEffect = SPR_hoshi;
	}
	BATTLE_MultiStatusChange( battleindex, attackNo, toNo,
		status, turn, SPR_item3, ReceveEffect, Success );
	BATTLE_ItemUseDelete( charaindex, haveitemindex );
}

void ITEM_useStatusRecovery_Battle(
	int charaindex, 	// 使った人のインデックス
	int toNo, 			// 使われる人の番号
	int haveitemindex 	// 使う人のアイテムの欄番号
){
	int i;
	int battleindex, attackNo,itemindex, status = -1;
	int ReceveEffect;
	char *pszP = NULL, *arg;

	// アイテムがあるかどうか
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	// パラメータを取得
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	pszP = arg;
	// 効果を取得
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// 全快もあるので０から検索
		for( i = 0; i < BATTLE_ST_END; i ++ ){
			// 効果ピッタリか？
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 効果ないので失敗
	if( status == -1 ) return ;

	//------- ここから回復処理   -----------
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	ReceveEffect = SPR_tyusya;	//   直るときはこれ

	// 全員
	BATTLE_MultiStatusRecovery( battleindex, attackNo, toNo,
		status, SPR_item3, ReceveEffect );

	//  戦闘中にアイテムが消えた場合の共通処理
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}






void ITEM_useMagicDef_Battle(
	int charaindex,
	int toNo,
	int haveitemindex
)
{
	int turn = 0, i;
	int battleindex, attackNo,itemindex, status = -1;
	char *pszP = NULL, *arg;

	char szTurn[] = "turn";

    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	pszP = arg;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			if( strncmp( pszP, aszMagicDef[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return ;

	if( ( pszP = strstr( arg, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	BATTLE_MultiMagicDef( battleindex, attackNo, toNo,
		status, turn, SPR_item3, SPR_difence );

	BATTLE_ItemUseDelete( charaindex, haveitemindex );


}






//--------------------------------------------------------------
//  パラメータ変更アイテムを使った場合の処理
//--------------------------------------------------------------
// 戦闘中の場合
void ITEM_useParamChange_Battle(
	int charaindex, 	// 使った人のインデックス
	int toNo, 			// 使われる人の番号
	int haveitemindex 	// 使う人のアイテムの欄番号
)
{
	int i;
	int battleindex, attackNo,itemindex;
	int pow, par = 0;
	int kind = -1;
	char *pszP = NULL, *arg;

	// アイテムがあるかどうか
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	// パラメータを取得
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	pszP = arg;
	// 効果を取得
	for( ;kind == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			// 効果ピッタリか？
			if( strncmp( pszP, aszParamChange[i], 2 ) == 0 ){
				kind = i;
				pszP +=2;
				break;
			}
		}
	}
	// 効果ないので失敗
	if( kind == -1 ) return ;


	if( strstr( pszP, "%" ) ){	//  この場合は％計算
		par = 1;
	}

	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 何ポイントあげるか
		pow = 30;
	}

	//------- ここから処理   -----------
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	// 全員
	BATTLE_MultiParamChange( battleindex, attackNo, toNo,
		kind, pow, par, SPR_item3, SPR_hoshi );


	// 戦闘中にアイテムが消えた場合の共通処理
	BATTLE_ItemUseDelete( charaindex, haveitemindex );


}






//--------------------------------------------------------------
//  フィールド属性変更アイテムを使った場合の処理
//--------------------------------------------------------------
// 戦闘中の場合
void ITEM_useFieldChange_Battle(
	int charaindex, 	// 使った人のインデックス
	int toNo, 			// 使われる人の番号
	int haveitemindex 	// 使う人のアイテムの欄番号
)
{
	int itemindex;
	char *pArg;

	// アイテムがあるかどうか
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- ここから処理   -----------
	// パラメータを取得
	pArg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	// パラメータ無いので失敗
	if( pArg == "\0" )return ;

	BATTLE_FieldAttChange( charaindex, pArg );

	// 戦闘中にアイテムが消えた場合の共通処理
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}


//--------------------------------------------------------------
//  属性反転アイテムを使った場合の処理
//--------------------------------------------------------------
// 戦闘中の場合
void ITEM_useAttReverse_Battle(
	int charaindex, 	// 使った人のインデックス
	int toNo, 			// 使われる人の番号
	int haveitemindex 	// 使う人のアイテムの欄番号
)
{
	int itemindex, battleindex, attackNo;


	//  アイテムがあるかどうか
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- ここから処理 -----------
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	// 全員
	BATTLE_MultiAttReverse( battleindex, attackNo, toNo,
		SPR_item3, SPR_kyu );

	// 戦闘中にアイテムが消えた場合の共通処理
	BATTLE_ItemUseDelete( charaindex, haveitemindex );


}


//--------------------------------------------------------------
//  気絶から復活を使った場合の処理
//--------------------------------------------------------------
// 戦闘中の場合
void ITEM_useRessurect(
	int charaindex, 	// 使った人のインデックス
	int toNo, 			// 使われる人の番号
	int haveitemindex 	// 使う人のアイテムの欄番号
)
{
	int itemindex, battleindex, attackNo, par = 0, pow = 0, ReceveEffect;
	char *pszP = NULL;


	// アイテムがあるかどうか
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- ここから処理   -----------
	pszP = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	if( strstr( pszP, "%" ) ){	// この場合は％計算
		par = 1;
	}

	if( sscanf( pszP, "%d", &pow ) != 1 ){
		// 何ポイント回復するか？
		pow = 0;	// ０の場合は完全回復
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	if( pow <= 0 ){
		ReceveEffect = SPR_fukkatu3;
	}else
	if( pow <= 100 ){
		ReceveEffect = SPR_fukkatu1;
	}else
	if( pow <= 300 ){
		ReceveEffect = SPR_fukkatu2;
	}else{
		ReceveEffect = SPR_fukkatu3;
	}

	// 全員
	BATTLE_MultiRessurect( battleindex, attackNo, toNo,
		pow, par, SPR_item3, ReceveEffect );


	//  戦闘中にアイテムが消えた場合の共通処理
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}



//--------------------------------------------------------------
//   捕獲率修正を使った場合の処理
//--------------------------------------------------------------
// 戦闘中の場合
void ITEM_useCaptureUp_Battle(
	int charaindex, 	// 使った人のインデックス
	int toNo, 			// 使われる人の番号
	int haveitemindex 	// 使う人のアイテムの欄番号
)
{
	int itemindex, battleindex, attackNo, pow = 5, ReceveEffect;
	char *pArg;

	// アイテムがあるかどうか
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	//------- ここから処理   -----------
	// パラメータを取得
	pArg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	// パラメータ無いので失敗
	if( pArg == "\0" )return ;

	if( sscanf( pArg, "%d", &pow ) != 1 ){
		// 何ポイント回復するか？
		pow = 5;
	}

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	attackNo = BATTLE_Index2No( battleindex, charaindex );

	ReceveEffect = SPR_hoshi;

	// 全員
	BATTLE_MultiCaptureUp( battleindex, attackNo, toNo,
		pow, SPR_item3, ReceveEffect );

	//  戦闘中にアイテムが消えた場合の共通処理
	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}
#ifdef _ITEM_CRACKER
void ITEM_useCracker_Effect( charaindex, toindex, haveitemindex)
{
	int itemindex,x,y,tofd;

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	// 检查物品
	if(!ITEM_CHECKINDEX(itemindex)) return;

    x = CHAR_getInt( charaindex, CHAR_X);
    y = CHAR_getInt( charaindex, CHAR_Y);
	
	CHAR_setMyPosition( charaindex, x, y, TRUE);
	CHAR_setWorkInt( charaindex, CHAR_WORKITEM_CRACKER, 1);
	CHAR_sendCrackerEffect( charaindex, 101125);
	//to client
	tofd = getfdFromCharaIndex( charaindex );
	lssproto_IC_send(tofd, x, y);
	//清除道具
	BATTLE_ItemUseDelete(charaindex,haveitemindex);
	CHAR_talkToCli(charaindex,-1,"道具消失了。",CHAR_COLORYELLOW);

}
#endif
#ifdef _ITEM_REFRESH //vincent 解除异常状态道具
void ITEM_useRefresh_Effect( charaindex, toindex, haveitemindex)
{
	int i,itemindex,ReceveEffect;
	char  *arg;
//	char szBuffer[128]="";
	int status = -1,attackNo,index2;
	int battleindex;
	char *pszP;

//print("\nvincent--ITEM_useRefresh_Effect");
	// 检查物品
	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );
	pszP = arg;
	// 効果を取得
	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		// １から検索
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			// 効果ピッタリか？
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
//print("\nvincent-->status:%d",status);
	//  効果ないので失敗
	if( status == -1 ) return;

	// バトル番号
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	index2 = BATTLE_No2Index( battleindex, toindex);//被击中目标之index
	attackNo = BATTLE_Index2No( battleindex, charaindex );
//    defNo = BATTLE_Index2No( battleindex, index2 );
//print("\nvincent-->charaindex:%d,attackNo:%d,index2:%d,defNo:%d,",charaindex,attackNo,index2,defNo);

    /* キャラクターの所持品リストから消去 */
    CHAR_setItemIndex(charaindex, haveitemindex ,-1);
	CHAR_sendItemDataOne( charaindex, haveitemindex);/* アイテム更新 */
//	if( CHAR_getWorkInt( charaindex, StatusTbl[status] ) > 0 ){
//		ReceveEffect = SPR_tyusya;
//	}else{
//		ReceveEffect = SPR_hoshi;
//	}
	ReceveEffect = SPR_tyusya;//打针
	BATTLE_MultiStatusRecovery( battleindex, attackNo, toindex,
		status, MAGIC_EFFECT_USER, ReceveEffect );
	/* 消す */
	ITEM_endExistItemsOne( itemindex );
//////////////////////////
//	BATTLE_MultiList( battleindex, defNo, ToList );
//		 for( i = 0; ToList[i] != -1; i ++ ){
//		toindex = BATTLE_No2Index( battleindex, ToList[i] );
//
//			}

		

////////////////////////////
}
#endif
#ifdef _ITEM_ADDEXP	//vincent 经验提升
void ITEM_useAddexp_Effect( charaindex, toindex, haveitemindex)
{
	int itemindex,power,vtime;
	//,pindex
	char *p = NULL, *arg;
	char szBuffer[1024]="";

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);

	// 检查物品
	if(!ITEM_CHECKINDEX(itemindex)) return;
	arg = ITEM_getChar(itemindex, ITEM_ARGUMENT );

	if( (p = strstr( arg, "增" )) != NULL ){
        sscanf( p+2, "%d", &power );
	}
	if( (p = strstr( arg, "分" )) != NULL ){
		sscanf( p+2, "%d", &vtime );
	}
	if( p == NULL )return;
#ifdef CC_ADD_ADDEXPCHECKDEGREE
	{
	if ( power != (int)(CHAR_getInt( charaindex, CHAR_ADDEXPPOWER)) && 
		 0 != (int)(CHAR_getInt( charaindex, CHAR_ADDEXPPOWER))
		 )
	{			
			CHAR_talkToCli( charaindex, -1, "经验增加的效果不同,无法叠加使用", CHAR_COLORYELLOW);
			return;
	}
	}
	#endif
#ifdef _ITEM_ADDEXP2 // 智果时间可以累加
	{
		int point;
		point = CHAR_getInt( charaindex, CHAR_ADDEXPPOWER)
					* (int)(CHAR_getInt( charaindex, CHAR_ADDEXPTIME)/60);
#ifdef _CAX_ITEM_ADDEXP					
		if( point >= getexptime()* power) {
#else			
		if( point >= 7200000) {
#endif			
			CHAR_talkToCli( charaindex, -1, "智慧之果累计时间及效果已达上限。", CHAR_COLORYELLOW);
			return;
		}
		
		if(CHAR_getInt( charaindex, CHAR_ADDEXPPOWER)!=power){
			point = power * vtime;
#ifdef _CAX_ITEM_ADDEXP				
			point = min( point, getexptime()*power);
#else
      point = min( point, 7200000);
#endif	
			vtime = (int)(point / power);
		}else{
			point += (power * vtime);
#ifdef _CAX_ITEM_ADDEXP				
			point = min( point, getexptime()*power);
#else
      point = min( point, 7200000);
#endif	
			vtime = (int)(point / power);
		}
	}
#endif
	CHAR_setInt( charaindex, CHAR_ADDEXPPOWER, power);
	CHAR_setInt( charaindex, CHAR_ADDEXPTIME,vtime*60 );

	//清除道具
	BATTLE_ItemUseDelete(charaindex,haveitemindex);
	//sprintf(szBuffer, "学习经验的能力提升了%d％", power);
	sprintf(szBuffer, "学习经验的能力提升了%d％，时效剩余%d分钟。", power, vtime);
	CHAR_talkToCli(charaindex,-1,szBuffer,CHAR_COLORYELLOW);
	//int mfd=getfdFromCharaIndex( charaindex);
	CHAR_charSaveFromConnect(charaindex, FALSE);
	CHAR_talkToCli(charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
}
#endif
//Terry add 2001/12/24
#ifdef _ITEM_FIRECRACKER
void ITEM_useFirecracker_Battle( charaindex, toindex, haveitemindex)
{
	int itemindex, battleindex, masteridx=-1, index2;
	
	char szWork[128];
#ifdef _FIX_FIRECRACKER
	int petid=-1, i=1;
	BOOL FINDPET=FALSE;
	char *buff1;
	char token[256], buf1[256];
#else
	int PetEscape = 0;
#endif

	itemindex = CHAR_getItemIndex(charaindex,haveitemindex);
	// 检查物品
	if(!ITEM_CHECKINDEX(itemindex)) return;

	battleindex = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEINDEX);
#ifdef _FIX_FIRECRACKER
	index2 = BATTLE_No2Index( battleindex, toindex);
	if( !CHAR_CHECKINDEX( index2) ) return;
	{
		int attackNo = BATTLE_Index2No( battleindex, charaindex );
		int safeSide = 0;
		int MySide_start, MySide_end;
		if( attackNo >= 10 )
			safeSide = 1;

		MySide_start = safeSide*SIDE_OFFSET;
		MySide_end = ((safeSide*SIDE_OFFSET) + SIDE_OFFSET);

		if( (toindex >= MySide_start) && (toindex<MySide_end) ){	//同边
			return;
		}
	}

	if( BATTLE_Index2No( battleindex, charaindex ) == toindex ){
		BATTLE_ItemUseDelete(charaindex,haveitemindex);//用在自身
		return;
	}

	if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		petid = -1;//作用在人物
	}else if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEPET ) {
		masteridx = BATTLE_No2Index(battleindex,toindex-5);//作用在宠物

		if( !CHAR_CHECKINDEX( masteridx) ) return;

		petid = CHAR_getInt(index2,CHAR_PETID);//宠物id
	}else if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEENEMY ) {
		petid = CHAR_getInt(index2,CHAR_PETID);
	}

	if( (buff1 = ITEM_getChar( itemindex, ITEM_ARGUMENT)) == "\0" ) return;

	memset( token, 0, sizeof( token));
	if( NPC_Util_GetStrFromStrWithDelim( buff1, "KPET", token, sizeof( token) ) == NULL) {
		//print( "Can't get KPET: %s!!\n", buff1);
		return;
	}

	// won fix
	while( getStringFromIndexWithDelim( token,"_", i, buf1, sizeof( buf1)) != FALSE )	{
	//while( getStringFromIndexWithDelim( token,"|", i, buf1, sizeof( buf1)) != FALSE )	{
		i++;
		if( petid == atoi( buf1) ){//年兽判别
			FINDPET = TRUE;
			break;
		}
	}

	sprintf(szWork,"BB|a%X|w3|r%X|f0|d0|p0|FF|",BATTLE_Index2No(battleindex,charaindex),toindex);
	BATTLESTR_ADD(szWork);//丢出鞭炮的动画
//送讯息至玩家
	if( FINDPET == TRUE )	{//若为年兽
		char buf4[255];
		sprintf( buf4, "%s被吓跑了！", CHAR_getChar( index2, CHAR_NAME));

		BATTLE_Exit(index2,battleindex);//离开战斗
		if( CHAR_CHECKINDEX( masteridx) ){
			CHAR_setInt(masteridx,CHAR_DEFAULTPET,-1);//无参战宠
			CHAR_talkToCli( masteridx,-1, buf4, CHAR_COLORYELLOW);
		}

		sprintf(szWork,"BQ|e%X|",toindex);//逃跑动画
		BATTLESTR_ADD(szWork);
		CHAR_talkToCli( charaindex,-1, buf4, CHAR_COLORYELLOW);
	}else	{
		CHAR_talkToCli( charaindex, -1, "什麽事都没发生。", CHAR_COLORYELLOW);
	}
	BATTLE_ItemUseDelete( charaindex, haveitemindex);//删除鞭炮
#else

	index2 = BATTLE_No2Index(battleindex,toindex);
	// 被炸到的是否为宠物
	if(CHAR_getInt(index2,CHAR_WHICHTYPE) == CHAR_TYPEPET) {
		// 如果被炸的是年兽
		if(CHAR_getInt(index2,CHAR_PETID) >= 901 && CHAR_getInt(index2,CHAR_PETID) <= 904){
			// 取得目标的正後方的角色的index
			masteridx = BATTLE_No2Index(battleindex,toindex-5);
			// 如果对战的是玩家
			if(CHAR_getInt(masteridx,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) PetEscape = 1;
			else																									       print("\n年兽在遇敌时出现");
		}
	}
	
	// 如果不是炸自己
	if(BATTLE_Index2No(battleindex,charaindex) != toindex)
	{
		sprintf(szWork,"BB|a%X|w3|r%X|f0|d0|p0|FF|",BATTLE_Index2No(battleindex,charaindex),toindex);
		BATTLESTR_ADD(szWork);
		if(PetEscape)
		{
			BATTLE_Exit(index2,battleindex);
			CHAR_setInt(masteridx,CHAR_DEFAULTPET,-1);
			sprintf(szWork,"BQ|e%X|",toindex);
			BATTLESTR_ADD(szWork);
			CHAR_talkToCli(masteridx,-1,"年兽被吓跑了！",CHAR_COLORWHITE);
			CHAR_talkToCli(charaindex,-1,"年兽被吓跑了！",CHAR_COLORWHITE);
		}
	}
	CHAR_talkToCli(charaindex,-1,"什麽事都没发生。",CHAR_COLORWHITE);
	CHAR_talkToCli(charaindex,-1,"道具消失了。",CHAR_COLORWHITE);
	// 使用後道具消失
	BATTLE_ItemUseDelete(charaindex,haveitemindex);
#endif
}
#endif

//Terry end




//气绝状态回复耐力,复加复活(光,镜,守)精灵 参数同魔法(光,镜,守)精灵,但只能在战斗之使用,用完就没了
//参数 例如:耐力值 吸 turn 1
#ifdef  _FEV_ADD_NEW_ITEM			// FEV ADD 增加复活守精
void ITEM_ResAndDef( int charaindex, int toindex, int haveitemindex )
{
	int itemindex, battleindex, attackNo, par = 0, pow = 0, ReceveEffect;
	char *buffer = NULL;
	char *magicarg = NULL;
	char *magicarg2 = NULL;
	char *magicarg3 = NULL;
	char magicarg4[200];

    char *pszP = NULL; 
	char delim[] = " ";//协助分解字串的区隔字元

	int status = -1, i, turn = 3;
	char szTurn[] = "turn";

	// アイテムがあるかどうか
    itemindex = CHAR_getItemIndex( charaindex, haveitemindex);
    if(!ITEM_CHECKINDEX(itemindex)) return;

	buffer = ITEM_getChar(itemindex, ITEM_ARGUMENT );//整串字串
	pszP = strtok(buffer, delim);//第一个参数
    magicarg = strtok(NULL, delim);//第二个参数
	magicarg2 = strtok(NULL, delim);//第叁个参数
	magicarg3 = strtok(NULL, delim);//第四个参数
    sprintf(magicarg4,"%s %s %s",magicarg,magicarg2,magicarg3);
	magicarg = (char*)magicarg4;

	//光镜守..不得使用敌方
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
		//&& BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P 
		){
		if( BATTLE_CheckSameSide( charaindex, toindex) == 0 ){//不同边
			battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
			BATTLE_NoAction( battleindex, BATTLE_Index2No( battleindex, charaindex) );
			CHAR_talkToCli( charaindex, -1, "光镜守..不得施予非玩家敌方。", CHAR_COLORYELLOW);
			return;
		}
	}

	//在气绝状态回复耐力
	if( strstr( pszP, "%" ) ){
		par = 1;
	}
	
	if( sscanf( pszP, "%d", &pow ) != 1 ){
		pow = 0;
	}
      
	attackNo = BATTLE_Index2No( battleindex, charaindex );
   
	//选择代表的特效
	if( pow <= 0 ){
		ReceveEffect = SPR_fukkatu3;
	}else
	if( pow <= 100 ){
		ReceveEffect = SPR_fukkatu1;
	}else
	if( pow <= 300 ){
		ReceveEffect = SPR_fukkatu2;
	}else{
		ReceveEffect = SPR_fukkatu3;
	}

	BATTLE_MultiRessurect( battleindex, attackNo, toindex,
		pow, par, SPR_item3, ReceveEffect );
    

	//(光,镜,守)精灵

	// 効果を取得
	for( ;status == -1 && magicarg[0] != 0; magicarg++ ){
		for( i = 1; i < BATTLE_MD_END; i ++ ){
			// 効果ピッタリか？
			if( strncmp( magicarg, aszMagicDef[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}
	// 効果ないので失敗
	if( status == -1 ) return;

	// 何回変更させるか？
	if( ( magicarg = strstr( magicarg, szTurn ) ) != NULL){
		magicarg += sizeof( szTurn );
		sscanf( magicarg, "%d", &turn );
	}

	//print("次数:%d",turn);

	// バトル番号
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	attackNo =  BATTLE_Index2No( battleindex, charaindex );

	// 全員
	BATTLE_MultiMagicDef( battleindex, attackNo, toindex,
		status, turn, MAGIC_EFFECT_USER, SPR_difence );


	BATTLE_ItemUseDelete( charaindex, haveitemindex );

}

#endif 





