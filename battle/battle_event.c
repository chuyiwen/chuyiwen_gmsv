#include "version.h"
#include <string.h>
#include <math.h>
#include "char.h"
#include "char_event.h"
#include "battle.h"
#include "battle_event.h"
#include "configfile.h"
#include "pet.h"
#include "log.h"
#include "lssproto_serv.h"
#include "anim_tbl.h"
#include "battle_magic.h"
#include "util.h"
#include "enemy.h"
#include "pet_skill.h"
#include "char_base.h"
#include "item_event.h"

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#include "item.h"
#include "skill.h"
#endif
#ifdef _PETSKILL_LER
#include "magic_base.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
#define DEFENSE_RATE	(0.5)
#define DAMAGE_RATE		(2.0)
#define CRITICAL_RATE	(1.0)
#define KAWASHI_MAX_RATE (75)

#define AJ_SAME	(1.0)

#define AJ_UP	(1.5)
#define AJ_DOWN	(0.6)

#define ATTR_MAX 100
#define D_ATTR (1.0/(ATTR_MAX*ATTR_MAX))

#define D_16	(1.0/16)
#define D_8		(1.0/8)

float gKawashiPara = 0.02;
float gCounterPara = 0.08;
float gCriticalPara = 0.09;

float gBattleDamageModyfy;
int   gBattleDuckModyfy;
int	 gBattleStausChange;
int	 gBattleStausTurn;
float gDuckPer;
int gCriper;

int	gBattleBadStatusTbl[BATTLE_ENTRY_MAX*2];

#ifdef _OTHER_MAGICSTAUTS
#ifdef _MAGICSTAUTS_RESIST
char MagicStatus[MAXSTATUSTYPE][36] = { "NULL","魔抗","铁壁","火抗","电抗","冰抗"};
int MagicTbl[] = { -1, CHAR_DEFMAGICSTATUS, CHAR_MAGICSUPERWALL, CHAR_MAGICFIRE, CHAR_MAGICTHUNDER, CHAR_MAGICICE };
#else
char MagicStatus[MAXSTATUSTYPE][36] = { "NULL","魔抗","铁壁"};
int MagicTbl[] = { -1, CHAR_DEFMAGICSTATUS, CHAR_MAGICSUPERWALL };
#endif //_MAGICSTAUTS_RESIST
#endif

char *aszStatus[] = { "全", "毒", "麻", "眠", "石", "醉", "乱", "虚", "剧", "障", "默" 
#ifdef _PET_SKILL_SARS			// WON ADD 毒煞蔓延
					  ,"煞"
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
					  ,"晕","缠","罗","爆","忘","箭","蛊","针","挑","烧","霜","电","专","火","冰","雷","炎","冻","击"
#endif
#ifdef _PROFESSION_ADDSKILL
					  ,"抗"
#endif
};

char *aszStatusFull[] = { "全快", "毒", "麻痹", "睡眠", "石化", "酒醉", "混乱", "虚弱", "剧毒", "魔障", "沉默" 
#ifdef _PET_SKILL_SARS			// WON ADD 毒煞蔓延
					  ,"毒煞"
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
					  ,"晕眩","缠绕","天罗","冰爆","遗忘","冰箭","嗜血蛊","一针见血","挑拨","火附体",
					  "冰附体","雷附体","专注战斗","火抗","冰抗","雷抗","火附","冰附","雷附"
#endif
};

int StatusTbl[] = { -1, CHAR_WORKPOISON,	CHAR_WORKPARALYSIS,
						CHAR_WORKSLEEP,		CHAR_WORKSTONE,
						CHAR_WORKDRUNK,		CHAR_WORKCONFUSION,
						CHAR_WORKWEAKEN,	CHAR_WORKDEEPPOISON,
						CHAR_WORKBARRIER,   CHAR_WORKNOCAST
#ifdef _PET_SKILL_SARS			// WON ADD 毒煞蔓延
						,CHAR_WORKSARS
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
						,CHAR_WORKDIZZY,		CHAR_WORKENTWINE
						,CHAR_WORKDRAGNET,		CHAR_WORKICECRACK
						,CHAR_WORKOBLIVION,		CHAR_WORKICEARROW
						,CHAR_WORKBLOODWORMS,	CHAR_WORKSIGN
						,CHAR_WORKINSTIGATE,	CHAR_WORK_F_ENCLOSE
						,CHAR_WORK_I_ENCLOSE,	CHAR_WORK_T_ENCLOSE
						,CHAR_WORK_FOCUS,		CHAR_WORKRESIST_F
						,CHAR_WORKRESIST_I,		CHAR_WORKRESIST_T
						,CHAR_WORK_F_ENCLOSE_2,	CHAR_WORK_I_ENCLOSE_2
						,CHAR_WORK_T_ENCLOSE_2
#endif
#ifdef _PROFESSION_ADDSKILL
						,CHAR_WORKERSIST_F_I_T, CHAR_WORKWATER//水附体
						,CHAR_WORKANNEX//附身
						,CHAR_WORKFEAR//恐惧
						,CHAR_WORKICECRACK2
						,CHAR_WORKICECRACK3
						,CHAR_WORKICECRACK4
						,CHAR_WORKICECRACK5
						,CHAR_WORKICECRACK6
						,CHAR_WORKICECRACK7
						,CHAR_WORKICECRACK8
						,CHAR_WORKICECRACK9
						,CHAR_WORKICECRACK10
#endif
					};

int RegTbl[] = { -1, CHAR_WORKMODPOISON,	CHAR_WORKMODPARALYSIS,
					 CHAR_WORKMODSLEEP,		CHAR_WORKMODSTONE,
					 CHAR_WORKMODDRUNK,		CHAR_WORKMODCONFUSION,
					 CHAR_WORKMODWEAKEN,	CHAR_WORKMODDEEPPOISON,
					 CHAR_WORKMODBARRIER,   CHAR_WORKMODNOCAST  
#ifdef _PET_SKILL_SARS			// WON ADD 毒煞蔓延
					 ,CHAR_WORKMODSARS
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
					 ,CHAR_WORKMODDIZZY,		CHAR_WORKMODENTWINE
					 ,CHAR_WORKMODDRAGNET,		CHAR_WORKMODICECRACK
					 ,CHAR_WORKMODOBLIVION,		CHAR_WORKMODICEARROW
					 ,CHAR_WORKMODBLOODWORMS,	CHAR_WORKMODSIGN
					 ,CHAR_WORKMODINSTIGATE,	CHAR_WORKMOD_F_ENCLOSE 
					 ,CHAR_WORKMOD_I_ENCLOSE,	CHAR_WORKMOD_T_ENCLOSE
					 ,CHAR_WORKMOD_FOCUS,		CHAR_WORKMODRESIST_F	
					 ,CHAR_WORKMODRESIST_I,		CHAR_WORKMODRESIST_T
					 ,CHAR_WORKMOD_F_ENCLOSE_2,	CHAR_WORKMOD_I_ENCLOSE_2
					 ,CHAR_WORKMOD_T_ENCLOSE_2					 
#endif
					};

char *aszMagicDef[] = { "常", "吸", "反", "无" 
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
						,"陷"
#endif
#ifdef _PETSKILL_ACUPUNCTURE
                        ,"刺"
#endif
};



char *aszMagicDefFull[] = { "通常", "吸收", "反射", "无效化" 
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
						,"陷阱"
#endif
#ifdef _PETSKILL_ACUPUNCTURE
						,"针刺"
#endif
};


int MagicDefTbl[] = { -1, CHAR_WORKDAMAGEABSROB, CHAR_WORKDAMAGEREFLEC, CHAR_WORKDAMAGEVANISH 
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
						,CHAR_WORKTRAP
#endif
#ifdef _PETSKILL_ACUPUNCTURE
                        ,CHAR_WORKACUPUNCTURE
#endif
};

char *aszParamChange[] = { "无", "攻", "防", "早", "魅", "捕" };
char *aszParamChangeFull[] = { "无", "攻击力", "防御力", "速度", "魅力", "捕获率" };
int aParamChangeTbl[] = { -1, CHAR_WORKMODATTACK, CHAR_WORKMODDEFENCE,
	CHAR_WORKMODQUICK, CHAR_WORKMODCHARM, CHAR_WORKMODCAPTURE };

// Robin 0727 Ride Pet
// 取得骑宠的index
int BATTLE_getRidePet( int charaindex )
{
	int petindex = -1;
	
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER && CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYERNPC )
		return -1;
	else
	{
		int rideIndex = CHAR_getInt( charaindex, CHAR_RIDEPET );
		if( rideIndex == -1 )
			return -1;
		petindex = CHAR_getCharPet( charaindex, rideIndex );
		if( !CHAR_CHECKINDEX( petindex ) )	return -1;
	
	}
	
	return petindex;

}

// 取得骑宠时的三围  正常版
float BATTLE_adjustRidePet3A( int charaindex, int petindex, int workindex, int action )
{
	float	ret = CHAR_getWorkInt( charaindex, workindex );
	BOOL	throwweapon = BATTLE_IsThrowWepon(CHAR_getItemIndex( charaindex, CHAR_ARM ) );
	
	switch( workindex )	{
	case CHAR_WORKATTACKPOWER : // 攻击力
		if( throwweapon ) // 投掷
			ret = CHAR_getWorkInt( charaindex, CHAR_WORKATTACKPOWER )
#ifdef _BATTLE_NEWPOWER //andy 1/24 reEdit
			+ CHAR_getWorkInt( petindex, CHAR_WORKATTACKPOWER )*0.4;
#else
			+ CHAR_getWorkInt( petindex, CHAR_WORKATTACKPOWER )*0.2;
#endif
		else // 近战
			ret = CHAR_getWorkInt( charaindex, CHAR_WORKATTACKPOWER )*0.8
				+ CHAR_getWorkInt( petindex, CHAR_WORKATTACKPOWER )*0.8;
		break;
	case CHAR_WORKDEFENCEPOWER : // 防御力
		ret = CHAR_getWorkInt( charaindex, CHAR_WORKDEFENCEPOWER )*0.7
			+ CHAR_getWorkInt( petindex, CHAR_WORKDEFENCEPOWER )*0.7;
		break;
	case CHAR_WORKQUICK : // 敏捷力
		if( action == ATTACKSIDE )	{
			if( throwweapon )	{
				ret = CHAR_getWorkInt( charaindex, CHAR_WORKQUICK )*0.8
					+ CHAR_getWorkInt( petindex, CHAR_WORKQUICK )*0.2;
			}else	{
#ifdef _BATTLE_NEWPOWER //andy 1/24 reEdit
				ret = CHAR_getWorkInt( charaindex, CHAR_WORKQUICK )*0.2
					+ CHAR_getWorkInt( petindex, CHAR_WORKQUICK )*0.8;

#else
				ret = CHAR_getWorkInt( charaindex, CHAR_WORKQUICK )*0.5
					+ CHAR_getWorkInt( petindex, CHAR_WORKQUICK )*0.5;
#endif
			}
		}else
			if( action == DEFFENCESIDE ){
			ret = CHAR_getWorkInt( charaindex, CHAR_WORKQUICK )*0.1
				+ CHAR_getWorkInt( petindex, CHAR_WORKQUICK )*0.9;
		}
		break;
	default :
		ret = CHAR_getWorkInt( charaindex, workindex );
	}
	
	//print(" adj3A_Now:%d:%d=%f ", charaindex, workindex, ret);

	return	ret;
}

// Robin 0727 ride Pet 落马改图
void BATTLE_changeRideImage( int index )
{
		int itemindex = CHAR_getItemIndex(index,CHAR_ARM);
		int category;
		int newimagenumber;
		int basebaseimagenumber
			= CHAR_getInt(index,CHAR_BASEBASEIMAGENUMBER);
		if( !ITEM_CHECKINDEX(itemindex) )	{
			category = ITEM_FIST;
		}else	{
			category = ITEM_getInt(itemindex,ITEM_TYPE );
		}
		newimagenumber = CHAR_getNewImagenumberFromEquip(
			basebaseimagenumber, category);
		if( newimagenumber == -1)	{
			CHAR_setInt( index, CHAR_BASEIMAGENUMBER, basebaseimagenumber);
		}else	{
			CHAR_setInt( index, CHAR_BASEIMAGENUMBER, newimagenumber);
		}
}

void  BATTLE_talkToCli(
	int charaindex,
	char *pszBuffer,
	int color
)
{
	if( getBattleDebugMsg( ) != 0
		&& CHAR_getFlg( charaindex, CHAR_ISSHOWBATTLEMSG )
	){
		CHAR_talkToCli( charaindex, -1, pszBuffer,  color );
	}
}


int  BATTLE_BroadCastBase(
	int battleindex,
	char *pszBuffer,
	int color,
	int flg
)
{
	int i, j, pindex;
	BATTLE_ENTRY	*pEntry;

	if( flg != 1 && getBattleDebugMsg( ) == 0 )return 0;

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return BATTLE_ERR_BATTLEINDEX;

	for( j = 0; j < 2; j ++ ){
		pEntry = BattleArray[battleindex].Side[j].Entry;
		for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
			pindex = pEntry[i].charaindex;
			if( CHAR_CHECKINDEX( pindex ) == FALSE )continue;
			if( CHAR_getInt( pindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )continue;
			if( CHAR_getFlg( pindex, CHAR_ISSHOWBATTLEMSG ) == FALSE ) continue;
			CHAR_talkToCli( pindex, -1, pszBuffer,  color );
		}
	}

	return 0;
}

// Nuke 20040112 fix for performance
/*int  BATTLE_BroadCast(
	int battleindex,
	char *pszBuffer,
	int color
)
{
	return BATTLE_BroadCastBase( battleindex, pszBuffer, color, 0 );

}

int  BATTLE_BroadCastOn(
	int battleindex,
	char *pszBuffer,
	int color
)
{
	return BATTLE_BroadCastBase( battleindex, pszBuffer, color, 1 );
}
*/

#ifdef _TAKE_ITEMDAMAGE
int BATTLE_ItemCrushCheck( int charaindex , int flg)
{//flg def 1 att 2
	int EquipTbl[CHAR_EQUIPPLACENUM], itemindex;
	int i, rndwork;
	int Crushs=0;
	//char szBuffer[256];

	memset( EquipTbl, -1, sizeof( EquipTbl));
	if( flg == 2 )	{
		itemindex = CHAR_getItemIndex( charaindex, CHAR_ARM);
		if( ITEM_CHECKINDEX( itemindex) == TRUE )	{
				return CHAR_ARM;
		}
		return -1;
	}else	{
		int equipnum;
#ifdef _TAKE_ITEMDAMAGE_FOR_PET
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET){
			equipnum = CHAR_PETITEMNUM;
		}else
#endif
			equipnum = CHAR_EQUIPPLACENUM;
#ifdef _TAKE_ITEMDAMAGE_FIX
		rndwork = rand() % equipnum;
#else
		Crushs = rand()%100;
		if( Crushs < 50 )	{
			rndwork = CHAR_BODY;
		}else if( Crushs >= 50 && Crushs < 67 )	{
			rndwork = CHAR_HEAD;
		}else if( Crushs >= 67 && Crushs < 84 )	{
			rndwork = CHAR_DECORATION1;
		}else	{
			rndwork = CHAR_DECORATION2;
		}
#endif
		for( i = 0; i < equipnum; i ++ ){//CHAR_ARM
			itemindex = CHAR_getItemIndex( charaindex, rndwork);
			if( ITEM_CHECKINDEX( itemindex) == TRUE && rndwork != CHAR_ARM ){
					return rndwork;
			}else {
				rndwork++;
				rndwork=rndwork%5;
			}
		}
	}
	
	return -1;
}
#else
int BATTLE_ItemCrushCheck( int charaindex )
{
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ){
		return FALSE;
	}
	if( RAND( 1, gItemCrushRate ) < CHAR_getInt( charaindex, CHAR_LV ) ){
		return TRUE;
	}else{
		return FALSE;
	}
}
#endif

#define BREAK_NAME_WEPON 		"武器"
#define BREAK_NAME_AROMER 		"防具"
#define BREAK_BRACELET			"护腕"
#define BREAK_MUSIC 			"乐器"
#define BREAK_NECKLACE 			"头饰"
#define BREAK_RING 				"戒指"
#define BREAK_BELT 				"皮带"
#define BREAK_EARRING 			"耳饰"
#define BREAK_NOSERING 			"鼻环"
#define BREAK_AMULET 			"护身符"
#define BREAK_OTHER 			"道具"

#ifdef _TAKE_ITEMDAMAGE
static char *aszCrushTbl[] = { "完好", "受损", "毁坏" , "不堪使用" , "碎片" };
#else
static char *aszCrushTbl[] = { "受损", "毁坏" };
#endif


#ifdef _TAKE_ITEMDAMAGE
int BATTLE_ItemCrush( int charaindex, int ItemEquip, int Damages, int flg)
{
	int itemindex, breadnums;
	int crushlevel, crushenum, maxcrushenum;
	char szBuffer[256]="";
	int battleindex, bid;

	itemindex = CHAR_getItemIndex( charaindex, ItemEquip );
	if( ITEM_CHECKINDEX( itemindex ) == FALSE )
		return -1;
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

	bid = BATTLE_Index2No( battleindex, charaindex );

	crushlevel = ITEM_getInt( itemindex, ITEM_CRUSHLEVEL);
	if( crushlevel < 0 ) crushlevel=0;

	crushenum = ITEM_getInt( itemindex, ITEM_DAMAGECRUSHE);
	maxcrushenum = ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE);
	if( maxcrushenum < 1 ) return -1;
	if( ItemEquip == CHAR_ARM )	{
		if( (breadnums = ((Damages-50)/40)) <= 0 )
			breadnums = 1;
	}else	{
		if( Damages >= 200 ) breadnums = 1;
		if( (breadnums = ((200 - Damages)/20)) < 0 )
			breadnums = 1;
	}
	crushenum = crushenum - RAND( breadnums, (breadnums*1.4) );
	ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, crushenum);

	if( crushenum <= 0 ){	//损坏消失
		crushenum = 0;
		sprintf(szBuffer, "%s因过度损坏而消失。\n", ITEM_getChar( itemindex, ITEM_NAME));
		CHAR_talkToCli( charaindex, -1, szBuffer, CHAR_COLORYELLOW);

//		LogItem(
//			CHAR_getChar( charaindex, CHAR_NAME ), 
//			CHAR_getChar( charaindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//			itemindex,
//#else
//       		ITEM_getInt( itemindex, ITEM_ID ),  
//#endif
//			"因过度损坏而消失",
//			CHAR_getInt( charaindex,CHAR_FLOOR),
//			CHAR_getInt( charaindex,CHAR_X ),
// 	      	CHAR_getInt( charaindex,CHAR_Y ),
//	        ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//		);
		CHAR_DelItem( charaindex, ItemEquip);
		crushlevel = 4;
	}else	{	//损坏扣值
		int defs, level=0;


		defs = (crushenum*100)/maxcrushenum;

		if( defs >= 70 ){
			level = 0;
		}else if( defs >= 50 && defs < 70 )	{
			level = 1;
		}else if( defs >= 30 && defs < 50 )	{
			level = 2;
		}else if( defs < 30 )	{
			level = 3;
		}

		if( level != crushlevel && defs < 70 )	{
			ITEM_setInt( itemindex, ITEM_CRUSHLEVEL, level);
//			for( i = 0; i < arraysizeof( ItemBreakPos ); i ++ ){
//				work = ITEM_getInt( itemindex, ItemBreakPos[i] );
//				if( work <= 0 )continue;
//				work *=0.95;
//				ITEM_setInt( itemindex, ItemBreakPos[i], work );
//			}
			sprintf( szBuffer, "%s是%s的。",
			ITEM_getChar( itemindex, ITEM_NAME ), aszCrushTbl[level] );
			CHAR_talkToCli( charaindex, -1, szBuffer, CHAR_COLORRED);
			//更改道具说明
//			buf1 = ITEM_getChar( itemindex, ITEM_SECRETNAME);
//			if( strstr( buf1, "(") != 0 )	{
//				char buf5[256];
//				if( getStringFromIndexWithDelim( buf1,"(",1, buf5, sizeof( buf5)) != FALSE )	{
//					sprintf( buf1, buf5);
//				}
//			}
//			sprintf( buf2, "%s(%s)", buf1, aszCrushTbl[level]);
//			ITEM_setChar( itemindex, ITEM_SECRETNAME, buf2);
		}
	}

   	CHAR_sendItemDataOne( charaindex, ItemEquip );
#ifdef _ATTREVERSE_FIX
	if(getAttReverse()==1){
  	//if(CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_REVERSE){
  	if(CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEREVERSE)==1){
  		CHAR_complianceParameter2(charaindex);
  	}else{
#endif
  	CHAR_complianceParameter( charaindex );
#ifdef _ATTREVERSE_FIX
  	}
  }else{
  	CHAR_complianceParameter( charaindex );
  }
#endif
	CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_ATK | CHAR_P_STRING_DEF 
		| CHAR_P_STRING_QUICK | CHAR_P_STRING_CHARM );

	return TRUE;
}
#else
int BATTLE_ItemCrush( int charaindex )
{
	int EquipTbl[CHAR_EQUIPPLACENUM], i, itemindex, rndwork, type;
	int work, crushlevel, j, ItemEquip;
	char szBuffer[256]="";

	int	ItemBreakPos[] = {
	    ITEM_ATTACKNUM_MIN,
    	ITEM_ATTACKNUM_MAX,
	    ITEM_MODIFYATTACK,
    	ITEM_MODIFYDEFENCE,
	    ITEM_MODIFYQUICK,
    	ITEM_MODIFYHP,
	    ITEM_MODIFYMP,
    	ITEM_MODIFYLUCK,
	    ITEM_MODIFYCHARM,
    	ITEM_MODIFYAVOID,
	};

	for( j = 0, i = 0; i < CHAR_EQUIPPLACENUM; i ++ ){
		itemindex = CHAR_getItemIndex( charaindex, i );
		if( ITEM_CHECKINDEX( itemindex ) == TRUE
		&& ITEM_getInt( itemindex, ITEM_MERGEFLG ) == 1	){
			EquipTbl[j] = i;
			j ++;
		}
	}
	if( j == 0 )return FALSE;
	rndwork = RAND( 0, j-1 );
	ItemEquip = EquipTbl[rndwork];
	itemindex = CHAR_getItemIndex( charaindex, ItemEquip );
	if( ITEM_CHECKINDEX( itemindex ) == FALSE )return FALSE;
	crushlevel = ITEM_getInt( itemindex, ITEM_CRUSHLEVEL );
	if( crushlevel > 1 ) return FALSE;
	{
		if( crushlevel == 1 ){
			sprintf( szBuffer, "CrushLv2(毁坏道具Lv2)" );
		}else{
			sprintf( szBuffer, "CrushLv1(毁坏道具Lv1)" );
		}
//		LogItem(
//			CHAR_getChar( charaindex, CHAR_NAME ),
//			CHAR_getChar( charaindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//			itemindex,
//#else
//   			ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//			szBuffer,
//	       	CHAR_getInt( charaindex,CHAR_FLOOR),
//			CHAR_getInt( charaindex,CHAR_X ),
//        	CHAR_getInt( charaindex,CHAR_Y ),
//			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//
//		);
	}

	/*{ int battleindex, bid;
		//sprintf( szBuffer, "(%s)的(%s)是(%s)\n",
		//	CHAR_getUseName( charaindex ),
		//	ITEM_getChar( itemindex, ITEM_NAME ),
		//	aszCrushTbl[crushlevel] );

		battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );

		bid = BATTLE_Index2No( battleindex, charaindex );
		//BATTLE_BroadCast( battleindex, szBuffer,
			(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	}*/

	type = ITEM_getInt( itemindex, ITEM_TYPE );
	szBuffer[0] = 0;
	switch( type ){
	case ITEM_FIST:
	case ITEM_AXE:
	case ITEM_CLUB:
	case ITEM_SPEAR:
	case ITEM_BOW:
	case ITEM_BOOMERANG:
	case ITEM_BOUNDTHROW:
	case ITEM_BREAKTHROW:
		sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_NAME_WEPON );
		break;

	case ITEM_HELM:
	case ITEM_ARMOUR:
		sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_NAME_AROMER );
		break;

	case ITEM_BRACELET:	sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_BRACELET );break;
	case ITEM_MUSIC:	sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_MUSIC );break;
	case ITEM_NECKLACE:	sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_NECKLACE );break;
	case ITEM_RING:		sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_RING );break;
	case ITEM_BELT:		sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_BELT );break;
	case ITEM_EARRING:	sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_EARRING );break;
	case ITEM_NOSERING:	sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_NOSERING );break;
	case ITEM_AMULET:	sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_AMULET );break;
	case ITEM_OTHER:	sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_OTHER );break;
	default:
		sprintf( szBuffer, "%s%s", aszCrushTbl[crushlevel], BREAK_OTHER );break;
		break;
	}
	ITEM_setChar( itemindex, ITEM_SECRETNAME, szBuffer );

	for( i = 0; i < arraysizeof( ItemBreakPos ); i ++ ){
		work = ITEM_getInt( itemindex, ItemBreakPos[i] );
		if( work <= 0 )continue;
		if( crushlevel == 1 ){
			work = 1;
		}else{
			work /= 2;
			if( work < 1 ) work = 1;
		}
		ITEM_setInt( itemindex, ItemBreakPos[i], work );
	}
   	CHAR_sendItemDataOne( charaindex, ItemEquip );
	ITEM_setInt( itemindex, ITEM_CRUSHLEVEL, crushlevel + 1 );
	CHAR_complianceParameter( charaindex );
	CHAR_send_P_StatusString( charaindex,
		CHAR_P_STRING_ATK
		| CHAR_P_STRING_DEF
		| CHAR_P_STRING_QUICK
		| CHAR_P_STRING_CHARM );

	return TRUE;
}
#endif

#ifdef _TAKE_ITEMDAMAGE
static int BATTLE_ItemCrushSeq( int attackindex, int charaindex, int Damages)
{
	int iRet = FALSE;
	int crushindex=-1, crushpower;
	int flg=0;//flg def 1 att 2

	flg=1;
	if( ( crushindex = BATTLE_ItemCrushCheck( charaindex, flg) ) >= 0 ) {
		if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER 
#ifdef _TAKE_ITEMDAMAGE_FOR_PET
			|| CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPET
#endif
			){
			crushpower = CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER);
			BATTLE_ItemCrush( charaindex, crushindex, Damages, flg);
		}
	}
	flg=2;
	if( ( crushindex = BATTLE_ItemCrushCheck( attackindex, flg) ) >= 0 )	{
		if( CHAR_getInt( attackindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER
#ifdef _TAKE_ITEMDAMAGE_FOR_PET
			|| CHAR_getInt( attackindex, CHAR_WHICHTYPE) == CHAR_TYPEPET
#endif
			 ){
			crushpower = CHAR_getWorkInt( charaindex, CHAR_WORKDEFENCEPOWER);
			BATTLE_ItemCrush( attackindex, crushindex, Damages, flg);
		}
	}
	
	return iRet;
}

#else

static int BATTLE_ItemCrushSeq( int charaindex )
{
	char szWork[256];
	int iRet = FALSE;

	if( BATTLE_ItemCrushCheck( charaindex ) == TRUE ){
		if( BATTLE_ItemCrush( charaindex ) == TRUE ){
			sprintf( szWork, "BK|%s的\t装备受到损伤。",
				CHAR_getUseName( charaindex ) );
			strcatsafe( szBadStatusString,sizeof(szBadStatusString), szWork );
			iRet = TRUE;
		}
	}
	return iRet;
}
#endif

#ifdef _EQUIT_ARRANGE
static BOOL BATTLE_ArrangeCheck( int attackindex, int defindex)
{
	int DEF_ARR;
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD ){
		return FALSE;
	}
	if( BATTLE_GetDamageReact( defindex ) > 0 ){
		return FALSE;
	}
	if( BATTLE_CanMoveCheck( defindex ) == FALSE ){
		return FALSE;
	}
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_NODUCK ){
		return FALSE;
	}
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
		return FALSE;
	}
	if( (DEF_ARR = CHAR_getWorkInt( defindex, CHAR_WORKARRANGEPOWER )) > 0 ){
		int per = (DEF_ARR > 700)?700:DEF_ARR;
		if( RAND( 1, 1000 ) <= per ){
			return TRUE;
		}
	}

	return FALSE;
}
#endif

//回避判定
static BOOL BATTLE_DuckCheck(
	int attackindex,
	int defindex
)
{
	int flg = FALSE;

	int  Df_Dex, At_Dex, Df_Luck = 0;
	float per, Work, wari, Big, Small;

	gDuckPer = 0;
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD ){
		return FALSE;
	}
	if( BATTLE_GetDamageReact( defindex ) > 0 ){
		return FALSE;
	}
	if( BATTLE_CanMoveCheck( defindex ) == FALSE ){
#ifdef _PROFESSION_ADDSKILL
		if( CHAR_getWorkInt( defindex, CHAR_DOOMTIME ) <= 0 )//集气时可闪避(并且没有处於天罗地网和盾击的状态)
			return FALSE;
		else{
			if( CHAR_getWorkInt( defindex, CHAR_WORKDRAGNET ) == 0 
				&& CHAR_getWorkInt( defindex, CHAR_WORKDIZZY ) == 0 ){
			}
			else{
				return FALSE;
			}
		}
#else
		return FALSE;
#endif
	}

	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_NODUCK ){
		return FALSE;
	}
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
		return FALSE;
	}

#ifdef _PETSKILL_SETDUCK
	if( BATTLE_CheckMySkillDuck( defindex ) == TRUE ){
		return TRUE;
	}
#endif

	At_Dex = CHAR_getWorkInt( attackindex, CHAR_WORKFIXDEX );
	Df_Dex = CHAR_getWorkInt( defindex, CHAR_WORKFIXDEX );
	if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		Df_Luck = CHAR_getWorkInt( defindex, CHAR_WORKFIXLUCK );
	}
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY
	&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET	){
		At_Dex *= 0.8;
	}else
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) != CHAR_TYPEENEMY
	&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET
	){
		Df_Dex *= 0.8;
	}else
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER
	&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER	){
		At_Dex *= 0.6;
	}else
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
	&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER	){
		Df_Dex *= 0.6;
	}

	if( Df_Dex >= At_Dex ){
		Big = Df_Dex;
		Small = At_Dex;
		wari = 1.0;
	}else{
		Big = At_Dex;
		Small = Df_Dex;
		if( Big <= 0 ){
			wari = 0.0;
		}else{
			wari = Small / Big;
		}
	}
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_JYUJYUTU	){
		gKawashiPara = 0.027;
	}else{
		gKawashiPara = 0.02;
	}
	Work = ( Big - Small ) / gKawashiPara;
	if( Work <= 0 ) Work = 0;
	per = (float)( sqrt( (double)Work ) );
	per *= wari;
	per += Df_Luck;

	per += gBattleDuckModyfy;
	if( CHAR_getWorkInt( attackindex, CHAR_WORKDRUNK )  > 0 ){
		per += RAND( 20, 30 );
	}
	if( gWeponType == ITEM_BOW ){
		per += 20;
	}
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_NOGUARD ){
		per += CHAR_GETWORKINT_HIGH( defindex, CHAR_WORKBATTLECOM3 );
	}

	if( gWeponType == ITEM_BOW ){
		per += 20;
	}

	per *= 100;
	if( per > KAWASHI_MAX_RATE*100 ) per = KAWASHI_MAX_RATE*100; 
	if( per <= 0 ) per = 1;

	gDuckPer = per * 0.01;
#ifdef _EQUIT_HITRIGHT
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		int AddHit = CHAR_getWorkInt( attackindex, CHAR_WORKHITRIGHT);
		per -= RAND( AddHit*0.8, AddHit*1.2);
		per = ( per < 0 )?0:per;
	}
#endif


#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	per = BATTLE_check_profession_duck( defindex, per );
#endif
	
#ifdef _PROFESSION_ADDSKILL
	if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_CHAOS ){ //混乱攻击时命中降低
		per += (per*0.4);	
	}
#endif

	if( RAND( 1, 10000 ) <= per ){
		flg = TRUE;
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
		// 回避技能升级
		if( (CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER) ){
			PROFESSION_SKILL_LVEVEL_UP( defindex, "PROFESSION_AVOID" );
		} 
#endif
	}else{
		flg = FALSE;
	}
	return flg;
}

static int	BATTLE_AttrCalc(
		int My_Fire,
		int My_Water,
		int My_Earth,
		int My_Wind,
		int My_None,
		int Vs_Fire,
		int Vs_Water,
		int Vs_Earth,
		int Vs_Wind,
		int Vs_None
)
{
	int iRet = 0;
	My_Fire = My_Fire * Vs_None * AJ_UP
			+ My_Fire * Vs_Fire * AJ_SAME
			+ My_Fire * Vs_Water * AJ_DOWN
			+ My_Fire * Vs_Earth * AJ_SAME
			+ My_Fire * Vs_Wind * AJ_UP;
	My_Water = My_Water * Vs_None * AJ_UP
			+ My_Water * Vs_Fire * AJ_UP
			+ My_Water * Vs_Water * AJ_SAME
			+ My_Water * Vs_Earth * AJ_DOWN
			+ My_Water * Vs_Wind * AJ_SAME;

	My_Earth = My_Earth * Vs_None * AJ_UP
			+ My_Earth  * Vs_Fire * AJ_SAME
			+ My_Earth * Vs_Water * AJ_UP
			+ My_Earth * Vs_Earth * AJ_SAME
			+ My_Earth * Vs_Wind * AJ_DOWN;

	My_Wind = My_Wind * Vs_None * AJ_UP
			+ My_Wind  * Vs_Fire * AJ_DOWN
			+ My_Wind * Vs_Water * AJ_SAME
			+ My_Wind * Vs_Earth * AJ_UP
			+ My_Wind * Vs_Wind * AJ_SAME;

	My_None = My_None * Vs_None * AJ_SAME
			+ My_None * Vs_Fire * AJ_DOWN
			+ My_None * Vs_Water * AJ_DOWN
			+ My_None * Vs_Earth * AJ_DOWN
			+ My_None * Vs_Wind * AJ_DOWN;

	iRet = (My_Fire + My_Water + My_Earth + My_Wind + My_None) ;
	return (iRet * D_ATTR);
}

static void BATTLE_GetAttr( int charaindex, int *T_pow )
{
#ifdef _BATTLE_NEWPOWER
#else
	int petindex = BATTLE_getRidePet( charaindex );    
    if( petindex == -1 ){
#endif
		T_pow[0] = CHAR_getWorkInt( charaindex, CHAR_WORKFIXEARTHAT );
		T_pow[1] = CHAR_getWorkInt( charaindex, CHAR_WORKFIXWATERAT );
		T_pow[2] = CHAR_getWorkInt( charaindex, CHAR_WORKFIXFIREAT );
		T_pow[3] = CHAR_getWorkInt( charaindex, CHAR_WORKFIXWINDAT );
/*
		*pAt_Fire = CHAR_getWorkInt( charaindex, CHAR_WORKFIXFIREAT );
		*pAt_Water = CHAR_getWorkInt( charaindex, CHAR_WORKFIXWATERAT );
		*pAt_Earth = CHAR_getWorkInt( charaindex, CHAR_WORKFIXEARTHAT );
		*pAt_Wind = CHAR_getWorkInt( charaindex, CHAR_WORKFIXWINDAT );
*/
#ifdef _BATTLE_NEWPOWER
#else
	}else	{    // Robin 0727 when Ride Pet
#endif
#ifdef _BATTLE_NEWPOWER
#else
		T_pow[0] = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXEARTHAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXEARTHAT )) /2;
		T_pow[1] = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXWATERAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXWATERAT )) /2;
		T_pow[2] = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXFIREAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXFIREAT )) /2;
		T_pow[3] = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXWINDAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXWINDAT )) /2;
/*
		*pAt_Fire = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXFIREAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXFIREAT )) /2;
		*pAt_Water = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXWATERAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXWATERAT )) /2;
		*pAt_Earth = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXEARTHAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXEARTHAT )) /2;
		*pAt_Wind = ( CHAR_getWorkInt( charaindex, CHAR_WORKFIXWINDAT )
			+ CHAR_getWorkInt( petindex, CHAR_WORKFIXWINDAT )) /2;
*/
	}
#endif
	//print("\n地1=%d,水1=%d,火1=%d,风1=%d\n",T_pow[0],T_pow[1],T_pow[2],T_pow[3]);
#ifdef _PROFESSION_ADDSKILL 
		//加上结界
        //T_pow[0] += T_pow[0]*( (CHAR_getWorkInt( charaindex, CHAR_WORKFIXEARTHAT_BOUNDARY )>>16&0x0000ffff) /100 );
		//T_pow[1] += T_pow[1]*( (CHAR_getWorkInt( charaindex, CHAR_WORKFIXWATERAT_BOUNDARY )>>16&0x0000ffff) /100 );
		//T_pow[2] += T_pow[2]*( (CHAR_getWorkInt( charaindex, CHAR_WORKFIXFIREAT_BOUNDARY )>>16&0x0000ffff) /100 );
		//T_pow[3] += T_pow[3]*( (CHAR_getWorkInt( charaindex, CHAR_WORKFIXWINDAT_BOUNDARY )>>16&0x0000ffff) /100 );
#endif	
#ifdef _SUIT_TWFWENDUM
	T_pow[0] = (T_pow[0]+CHAR_getWorkInt( charaindex, CHAR_WORK_EA ))>100?100:(T_pow[0]+CHAR_getWorkInt( charaindex, CHAR_WORK_EA ));
	T_pow[1] = (T_pow[1]+CHAR_getWorkInt( charaindex, CHAR_WORK_WR ))>100?100:(T_pow[1]+CHAR_getWorkInt( charaindex, CHAR_WORK_EA ));
	T_pow[2] = (T_pow[2]+CHAR_getWorkInt( charaindex, CHAR_WORK_FI ))>100?100:(T_pow[2]+CHAR_getWorkInt( charaindex, CHAR_WORK_EA ));
	T_pow[3] = (T_pow[3]+CHAR_getWorkInt( charaindex, CHAR_WORK_WI ))>100?100:(T_pow[3]+CHAR_getWorkInt( charaindex, CHAR_WORK_EA ));
#endif
	//print("\n地2=%d,水2=%d,火2=%d,风2=%d\n",T_pow[0],T_pow[1],T_pow[2],T_pow[3]);
	{
		int i, renum=ATTR_MAX;
		for( i=0; i<4; i++)	{
			if( T_pow[ i] < 0 ) T_pow[ i] = 0;
			renum -= T_pow[ i];
		}
		if( renum < 0 ) renum = 0;
		T_pow[ 4] = renum;
	}
	//print("\n地2=%d,水2=%d,火2=%d,风2=%d\n",T_pow[0],T_pow[1],T_pow[2],T_pow[3]);
/*
	if( *pAt_Fire < 0 ) *pAt_Fire = 0;
	if( *pAt_Water < 0 ) *pAt_Water = 0;
	if( *pAt_Earth < 0 ) *pAt_Earth = 0;
	if( *pAt_Wind < 0 ) *pAt_Wind = 0;
	*pAt_None = ATTR_MAX - *pAt_Fire - *pAt_Water - *pAt_Earth - *pAt_Wind;
	if( *pAt_None < 0 )*pAt_None = 0;
*/
}


float BATTLE_FieldAttAdjust(
	int battleindex,
	int pAt_Fire,
	int pAt_Water,
	int pAt_Earth,
	int pAt_Wind
)
{
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return 0.0;
#define AJ_BOTTOM (0.5)
#define AJ_PLUS   (0.5)
	int att;
	float power, a_pow;
	att = BattleArray[battleindex].field_att;
	a_pow = (float)(BattleArray[battleindex].att_pow);
	//print("\natt=%d\n",att);
	switch( att ){
	case BATTLE_ATTR_EARTH:
		power = AJ_BOTTOM + (pAt_Earth) * a_pow * 0.01 * 0.01 * AJ_PLUS;
		break;
	case BATTLE_ATTR_WATER:
		power = AJ_BOTTOM + (pAt_Water) * a_pow * 0.01 * 0.01 * AJ_PLUS ;
		break;
	case BATTLE_ATTR_FIRE:
		power = AJ_BOTTOM + (pAt_Fire) * a_pow * 0.01 * 0.01 * AJ_PLUS ;
		break;
	case BATTLE_ATTR_WIND:
		power = AJ_BOTTOM + (pAt_Wind) * a_pow * 0.01 * 0.01 * AJ_PLUS ;
		break;
	default:
		power = AJ_BOTTOM;
		break;
	}
	return power;
}

static int BATTLE_AttrAdjust(
	int attackindex,
	int defindex,
	int damage
)
{
	int At_pow[5]={0,0,0,0,0}; //地水火风
	int Dt_pow[5]={0,0,0,0,0};
	int i;
	float At_FieldPow, Df_FieldPow;
#ifdef _BATTLE_PROPERTY
	int (*loopfunc)(int,int,int*,int*,int)=NULL;
#endif

	BATTLE_GetAttr( attackindex, At_pow);
	BATTLE_GetAttr( defindex, Dt_pow);
	//print("\n地1=%d,水1=%d,火1=%d,风1=%d\n",At_pow[0],At_pow[1],At_pow[2],At_pow[3]);
	//print("\n地2=%d,水2=%d,火2=%d,风2=%d\n",Dt_pow[0],Dt_pow[1],Dt_pow[2],Dt_pow[3]);
#ifdef _PSKILL_MDFYATTACK
	if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1) == BATTLE_COM_S_MDFYATTACK )	{
		int MKind=-1, MODS=0;
		MKind = CHAR_GETWORKINT_LOW( attackindex, CHAR_WORKBATTLECOM4);
		MODS = CHAR_GETWORKINT_HIGH( attackindex, CHAR_WORKBATTLECOM4);
		
		for( i=0; i<5; i++)	{
			At_pow[ i] = 0;
		}

		if( MKind >= 0 && MKind <= 4 ){
			At_pow[ MKind] = MODS;
			At_pow[ 4] = 0;
		}
	}
#endif
#ifdef _BATTLE_PROPERTY
	loopfunc =(int(*)(int,int,int*,int*,int))CHAR_getFunctionPointer( attackindex, CHAR_BATTLEPROPERTY);
	if( loopfunc ){
		loopfunc( attackindex, defindex, &damage, At_pow, 5);
	}
#endif
#ifdef _BATTLE_PROPERTY
	loopfunc = NULL;
	loopfunc =(int(*)(int,int,int*,int*,int))CHAR_getFunctionPointer( defindex, CHAR_BATTLEPROPERTY);
	if( loopfunc ){
		loopfunc( defindex, attackindex, &damage, Dt_pow, 5);
	}
#endif
	//print("\n地1=%d,水1=%d,火1=%d,风1=%d\n",At_pow[0],At_pow[1],At_pow[2],At_pow[3]);
	//print("\n地2=%d,水2=%d,火2=%d,风2=%d\n",Dt_pow[0],Dt_pow[1],Dt_pow[2],Dt_pow[3]);
	At_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX ), At_pow[ 2], At_pow[ 1], At_pow[ 0], At_pow[ 3] );
	Df_FieldPow = BATTLE_FieldAttAdjust(
		CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX ), Dt_pow[ 2], Dt_pow[ 1], Dt_pow[ 0], Dt_pow[ 3] );
	//print("\nAt_FieldPow=%f,Df_FieldPow=%f\n",At_FieldPow,Df_FieldPow);
	for( i=0; i<5; i++)	{
		At_pow[ i] *= damage;
	}
/*
	At_Fire *= damage;	At_Water *= damage;	At_pow[0] *= damage;
	At_Wind *= damage;	At_none *= damage;
*/
	//print("\ndamage1=%d,",damage);
	damage = BATTLE_AttrCalc(//火2水1第0封3
		At_pow[ 2], At_pow[ 1], At_pow[ 0], At_pow[ 3], At_pow[ 4],
		Dt_pow[ 2], Dt_pow[ 1], Dt_pow[ 0], Dt_pow[ 3], Dt_pow[ 4] );
	damage *= (At_FieldPow / Df_FieldPow);
	//print("damage2=%d\n",damage);
	return damage;
}

int BATTLE_GuardAdjust( int damage )
{
	int Rand = RAND( 1, 100 );
	if( Rand <= 25 ){
		damage *= 0.00;
	}else{
		if( Rand <= 50 ){
			damage *= 0.10;
		}else{
			if( Rand <= 70 ){
				damage *= 0.20;
			}else{
				if( Rand <= 85 ){
					damage *= 0.30;
				}else{
					if( Rand <= 95 ){
						damage *= 0.40;
					}else{
						damage *= 0.50;
					}
				}
			}
		}
	}
	return damage;
}

//计算伤害值
int BATTLE_DamageCalc( int attackindex, int defindex )
{
	float attack, defense;
	int damage = 0;
	int attackpet = BATTLE_getRidePet( attackindex );
	int defpet = BATTLE_getRidePet( defindex );
	
	// Robin 0727 Ride Pet
	if( attackpet == -1 )	{
		attack = CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER );
	}else	{
		attack = BATTLE_adjustRidePet3A( attackindex, attackpet, CHAR_WORKATTACKPOWER, ATTACKSIDE );
	}
		if( defpet == -1 )	{
#ifdef _BATTLE_NEWPOWER
			defense = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ) * 0.70;
#else
			defense = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ) * 0.45;
			defense += CHAR_getWorkInt( defindex, CHAR_WORKQUICK ) * 0.2;
			defense += CHAR_getWorkInt( defindex, CHAR_WORKFIXVITAL ) * 0.1;
#endif
		}else	{
#ifdef _BATTLE_NEWPOWER
			defense = BATTLE_adjustRidePet3A( defindex, defpet, CHAR_WORKDEFENCEPOWER, DEFFENCESIDE ) * 0.70;
#else
			defense = BATTLE_adjustRidePet3A( defindex, defpet, CHAR_WORKDEFENCEPOWER, DEFFENCESIDE ) * 0.45;
			defense += BATTLE_adjustRidePet3A( defindex, defpet, CHAR_WORKQUICK, DEFFENCESIDE ) * 0.20;
			defense += CHAR_getWorkInt( defindex, CHAR_WORKFIXVITAL ) * 0.05;	
#endif
		}

#ifdef _MAGIC_SUPERWALL //铁壁防御
		if( CHAR_getWorkInt( defindex, CHAR_MAGICSUPERWALL ) > 0 ){
			float def = (float)(CHAR_getWorkInt( defindex, CHAR_OTHERSTATUSNUMS ));
			def = (def + rand()%20)/100;
			defense += defense * def;
		}
#endif
#ifdef _NPCENEMY_ADDPOWER //修改怪物的能力值
		if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )	{
			defense += (defense*(rand()%10)+2)/100;
		}
		if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )	{
			attack += (attack*(rand()%10)+2)/100;
		}
#endif
	if( CHAR_getWorkInt( defindex, CHAR_WORKSTONE ) > 0 ) defense *= 2.0;

#ifdef _PETSKILL_REGRET
	//无装备防御
	if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_REGRET
		|| CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_REGRET2 )
		defense = CHAR_getWorkInt(defindex,CHAR_WORKFIXTOUGH);
#endif

#ifdef _EQUIT_NEGLECTGUARD //忽视目标防御力%
	if( CHAR_getWorkInt(  attackindex, CHAR_WORKNEGLECTGUARD) > 1 ) {
		float defp = (float)CHAR_getWorkInt(  attackindex, CHAR_WORKNEGLECTGUARD);
		defp = 1 - (defp/100);
		defense = defense*defp;
	}
#endif

	if( defense <= attack && attack < (defense * 8.0/7.0)   ){
		damage = (int)(RAND( 0, attack * D_16 ));
	}else
	if( defense > attack ){
		damage = (int)(RAND( 0, 1 ));
	}else
	if( attack >= (defense * 8/7)   ){
		float K0;
		K0 = RAND( 0, attack*D_8 ) - attack*D_16;
		damage = (int)(( (attack - defense )*DAMAGE_RATE ) + K0);
	}

	//四属性
	damage = BATTLE_AttrAdjust( attackindex, defindex, damage );

#ifdef _PROFESSION_ADDSKILL 
	{
		//四属结界
		if( ((CHAR_getWorkInt( defindex, CHAR_WORKFIXEARTHAT_BOUNDARY )>>16) & 0x0000ffff) > 0 ){
			if( CHAR_getWorkInt( attackindex, CHAR_WORKFIXEARTHAT ) > 0 )
				damage -= damage*(float)((CHAR_getWorkInt( attackindex, CHAR_WORKFIXEARTHAT )/200.0)); 
		}else if( ((CHAR_getWorkInt( defindex, CHAR_WORKFIXWATERAT_BOUNDARY )>>16) & 0x0000ffff) > 0 ){
			if( CHAR_getWorkInt( attackindex, CHAR_WORKFIXWATERAT ) > 0 )
				damage -= damage*(float)((CHAR_getWorkInt( attackindex, CHAR_WORKFIXWATERAT )/200.0));
		}else if( ((CHAR_getWorkInt( defindex, CHAR_WORKFIXFIREAT_BOUNDARY )>>16) & 0x0000ffff) > 0 ){
			if( CHAR_getWorkInt( attackindex, CHAR_WORKFIXFIREAT ) > 0 )
				damage -= damage*(float)((CHAR_getWorkInt( attackindex, CHAR_WORKFIXFIREAT )/200.0));
		}else if( ((CHAR_getWorkInt( defindex, CHAR_WORKFIXWINDAT_BOUNDARY )>>16) & 0x0000ffff) > 0 ){
			if( CHAR_getWorkInt( attackindex, CHAR_WORKFIXWINDAT ) > 0 )
				damage -= damage*(float)((CHAR_getWorkInt( attackindex, CHAR_WORKFIXWINDAT )/200.0));
		}
		//T_pow[1] += T_pow[1]*( (CHAR_getWorkInt( charaindex, CHAR_WORKFIXWATERAT_BOUNDARY )>>16&0x0000ffff) /100 );
		//T_pow[2] += T_pow[2]*( (CHAR_getWorkInt( charaindex, CHAR_WORKFIXFIREAT_BOUNDARY )>>16&0x0000ffff) /100 );
		//T_pow[3] += T_pow[3]*( (CHAR_getWorkInt( charaindex, CHAR_WORKFIXWINDAT_BOUNDARY )>>16&0x0000ffff) /100 );
	}
#endif

#ifdef _ADD_DEAMGEDEFC //额外伤害 & 附加攻击
	{
		int apower, dpower, otherpower;
		apower = CHAR_getWorkInt( attackindex, CHAR_WORKOTHERDMAGE);
		dpower = CHAR_getWorkInt( defindex, CHAR_WORKOTHERDEFC);

		otherpower = RAND( apower*0.3, apower) - RAND( dpower*0.3, dpower);
		if( otherpower != 0 )	{
			damage += otherpower;
		}
	}
	if( damage < 0 )
		damage = 0;
#endif
#ifdef _DAMMAGE_CALC
	return damage * getDamageCalc() / 100;
#else
	return damage;
#endif
}

static int BATTLE_CriticalCheckPlayer( int attackindex, int defindex )
{
	int At_Dex, At_Luck = 0, At_Kiryoku = 0, At_Soubi = 0;
	int At_SoubiIndex, Df_Dex, root = 1;
	float per, Work, Big, Small, wari, divpara = gCriticalPara;

	// WON REM 
	//if( gWeponType == ITEM_BOW )return 0;

	At_Dex = CHAR_getWorkInt( attackindex, CHAR_WORKFIXDEX );
	Df_Dex = CHAR_getWorkInt( defindex, CHAR_WORKFIXDEX );

	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		At_Luck = CHAR_getWorkInt( attackindex, CHAR_WORKFIXLUCK );
	}
	At_SoubiIndex = CHAR_getItemIndex( attackindex, CHAR_ARM );
	At_Kiryoku = CHAR_getInt( attackindex, CHAR_MP );

	if( ITEM_CHECKINDEX( At_SoubiIndex ) == TRUE ){
		At_Soubi = ITEM_getInt( At_SoubiIndex, ITEM_CRITICAL );
	}
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET
	&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY){
		Df_Dex *= 0.8;
	}else
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY
	&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET){
		divpara = 10.0;
		root = 0;
	}else
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER
	&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
		divpara = 10.0;
		root = 0;
	}else
		if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
		&&  CHAR_getInt( defindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER){
			Df_Dex *= 0.6;
		}

	if( At_Dex >= Df_Dex ){
		Big = At_Dex;
		Small = Df_Dex;
		wari = 1.0;
	}else{
		Big = Df_Dex;
		Small = At_Dex;
		if( Big <= 0 ){
			wari = 0.0;
		}else{
			wari = Small / Big;
		}
	}
	Work = ( Big - Small ) / divpara;
	if( Work <= 0 ) Work = 0;

	if( root == 1 ){
		per = (float)( sqrt( (double)Work ) ) + At_Soubi * 0.5;
	}else{
		per = (float)Work + At_Soubi * 0.5;
	}
	per *= wari;
	per += At_Luck ;
	per *= 100;
	if( per < 0 ) per = 1;
	if( per > 10000 ) per = 10000;
#ifdef _PETSKILL_LER
	// 雷尔不能被打飞
	if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) per = 0;
#endif
	return (int)per;
}

static int BATTLE_CounterCalc( int attackindex, int defindex )
{
	int Df_Dex, At_Dex, Work, root = 1;
	float per, Big, Small, wari, divpara = gCounterPara;

	At_Dex = CHAR_getWorkInt( attackindex, CHAR_WORKFIXDEX );
	Df_Dex = CHAR_getWorkInt( defindex, CHAR_WORKFIXDEX );

	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY
		&& CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET
	){
		divpara = 10;
		root = 0;
	}else if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET
		&& CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY
	){
		Df_Dex *= 0.8;
	}else if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER
		&& CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
	){
		divpara = 10;
		root = 0;
	}else if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER
		&& CHAR_getInt( defindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER
	){
		Df_Dex *= 0.6;
	}

	if( At_Dex >= Df_Dex ){
		Big = At_Dex;
		Small = Df_Dex;
		wari = 1.0;
	}else{
		Big = Df_Dex;
		Small = At_Dex;
		if( Big <= 0 ){
			wari = 0.0;
		}else{
			wari = Small / Big;
		}
	}
	Work = ( Big - Small ) / divpara;
	if( Work <= 0 ) Work = 0;
	if( root == 1 ){
		per = (float)( (double)sqrt( Work ) );
	}else{
		per = (float)( Work);
	}
	per *= wari;

	return per;
}

static int BATTLE_CriticalCheck( int attackindex, int defindex )
{
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		return BATTLE_CriticalCheckPlayer( attackindex, defindex );
	}else{
		return BATTLE_CriticalCheckPlayer( attackindex, defindex );
	}
}

static int BATTLE_CriDamageCalc( int attackindex, int defindex )
{
	int damage = 0;
	damage = BATTLE_DamageCalc( attackindex, defindex );

	damage += CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER )
		* (float)CHAR_getInt( attackindex, CHAR_LV ) 
		/ (float)CHAR_getInt( defindex, CHAR_LV ) 
		* 0.5;
	return damage;
}

static int BATTLE_GuardianCheck( int attackindex, int defindex )
{
	int GuardianIndex, Guardian, battleindex;
	int DefSide = 0, i, DefNo;
	//char szBuffer[256]="";

	battleindex = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX );

	DefNo = BATTLE_Index2No( battleindex, defindex );
	i = DefNo;
	if( i >= SIDE_OFFSET ){
		i -= SIDE_OFFSET;
		DefSide = 1;
	}

	if( BattleArray[battleindex].Side[DefSide].Entry[i].guardian == -1 ){
		return -1;
	}

	Guardian = BattleArray[battleindex].Side[DefSide].Entry[i].guardian;
// Terry add fix 使用舍已为人技能时,若攻击使用此技能的人client会当
	if(Guardian == DefNo) return -1;
// end
	GuardianIndex = BATTLE_No2Index( battleindex, Guardian );

	if( CHAR_CHECKINDEX( GuardianIndex ) == FALSE )return -1;

	if( CHAR_getFlg( GuardianIndex, CHAR_ISDIE ) == TRUE )return -1;

	if( CHAR_getWorkInt( GuardianIndex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_GUARDIAN ){
	}else{
		return -1;
	}

	if( CHAR_getWorkInt( GuardianIndex, CHAR_WORKSLEEP ) > 0
	|| CHAR_getWorkInt( GuardianIndex, CHAR_WORKCONFUSION ) > 0
	|| CHAR_getWorkInt( GuardianIndex, CHAR_WORKPARALYSIS ) > 0
	|| CHAR_getWorkInt( GuardianIndex, CHAR_WORKSTONE ) > 0
	|| CHAR_getWorkInt( GuardianIndex, CHAR_WORKBARRIER ) > 0
	|| GuardianIndex == attackindex
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	|| CHAR_getWorkInt( GuardianIndex, CHAR_WORKDIZZY ) > 0 	// 晕眩				
	|| CHAR_getWorkInt( GuardianIndex, CHAR_WORKDRAGNET ) > 0 	// 天罗地网
	|| CHAR_getWorkInt( GuardianIndex, CHAR_WORKINSTIGATE ) > 0 // 挑拨	
#endif
#ifdef _PROFESSION_ADDSKILL
	|| CHAR_getWorkInt( GuardianIndex, CHAR_DOOMTIME ) > 0 //世界末日集气
#endif
	){
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)想要守住(%s)可是没成功\。",
		//	CHAR_getUseName( GuardianIndex ),
		//	CHAR_getUseName( defindex )
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(DefNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		return -1;
	}

	if( BATTLE_IsThrowWepon(
		CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == TRUE
	){
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)想要守住(%s)可是没成功\。",
		//	CHAR_getUseName( GuardianIndex ),
		//	CHAR_getUseName( defindex )
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(DefNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		return -1;
	}

	//snprintf( szBuffer, sizeof(szBuffer),
	//	"(%s)护住了(%s)。",
	//	CHAR_getUseName( GuardianIndex ),
	//	CHAR_getUseName( defindex )
	//);
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(DefNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	return Guardian;
}


static int BATTLE_DamageWakeUp( int battleindex, int defindex )
{
	int bid;
	char szBuffer[256]="";
	CHAR_setInt( defindex, CHAR_DAMAGECOUNT,
		CHAR_getInt( defindex, CHAR_DAMAGECOUNT ) +1 );

	if( CHAR_getWorkInt( defindex, CHAR_WORKSLEEP ) > 0 ){
		CHAR_setWorkInt( defindex, CHAR_WORKSLEEP, 0 );
		bid = BATTLE_Index2No( battleindex, defindex );
		//sprintf( szBuffer, "(%s)被击中跳了起来。\n", 
		//	CHAR_getUseName( defindex ) );
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(bid >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		sprintf( szBuffer, "BM|%X|%X|", bid, 0 );
		strcatsafe( szBadStatusString,sizeof(szBadStatusString), szBuffer );
	}
	return 0;
}

static int BATTLE_AttackSeq( int attackindex, int defindex, int *pDamage, int *pGuardian,	int opt )
{
	int iRet = 0, perCri = 0;
	int GuardianIndex = -1,battleindex;

	battleindex = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX );
	
	// 主人打宠
	if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET 
		&& BattleArray[battleindex].norisk == 0
		&& BattleArray[battleindex].type == BATTLE_TYPE_P_vs_E){
		if( CHAR_getWorkInt( defindex, CHAR_WORKPLAYERINDEX ) == attackindex ){
			CHAR_PetAddVariableAi( defindex, AI_FIX_SEKKAN );	// 修正忠诚度
		}
	}
	
	// 闪避
	if( opt != BATTLE_COM_COMBO ){
		if( BATTLE_DuckCheck( attackindex, defindex ) == TRUE ){
			return BATTLE_RET_DODGE;
		}
#ifdef _SUIT_ADDPART3
		if( CHAR_getWorkInt( defindex, CHAR_WORKDUCKPOWER) > 0 ){
			if( rand()%100 < CHAR_getWorkInt( defindex, CHAR_WORKDUCKPOWER) )
				return BATTLE_RET_DODGE;
		}
#endif
	}
	
	// 检查被攻击方有无被别人用忠犬保护
	if( *pGuardian == -1 ){
		*pGuardian = BATTLE_GuardianCheck( attackindex, defindex );
		if( *pGuardian != -1 ){
			GuardianIndex = BATTLE_No2Index( battleindex, *pGuardian );
			defindex = GuardianIndex;
		}
	}
	
	// 会心一击
	perCri = BATTLE_CriticalCheck( attackindex, defindex );
	
#ifdef _PETSKILL_DAMAGETOHP
	if( opt == BATTLE_COM_S_DAMAGETOHP2 ){ //暗月狂狼 (会心一击上升30%)  && CHAR_getInt( attackindex, CHAR_HP) <= CHAR_getWorkInt( attackindex, CHAR_MAXMP)*0.5
		//print("\n1会心一击:%d",perCri);                 
		//print("\n1攻:%d", CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER));
		//print("\n1敏:%d", CHAR_getWorkInt( attackindex, CHAR_WORKQUICK));
		perCri = perCri + (perCri*0.3); //会心一击上升30%  
		CHAR_setWorkInt( attackindex, CHAR_WORKATTACKPOWER, CHAR_getWorkInt( attackindex,CHAR_WORKFIXSTR)+CHAR_getWorkInt( attackindex,CHAR_WORKFIXSTR)*0.2);//攻上升20%
		CHAR_setWorkInt( attackindex, CHAR_WORKQUICK, CHAR_getWorkInt( attackindex, CHAR_WORKFIXDEX)+CHAR_getWorkInt( attackindex, CHAR_WORKFIXDEX) *0.2 );//敏上升20%
		//print("\n2会心一击:%d",perCri);
		//print("\n2攻:%d", CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER));
		//print("\n2敏:%d", CHAR_getWorkInt( attackindex, CHAR_WORKQUICK));
	}
#endif
	
	gCriper = perCri;
	if( RAND( 1, 10000 ) < perCri ){
		// WON ADD
		int gWeponType = BATTLE_GetWepon( attackindex );
		if( gWeponType != ITEM_BOW ){
			(*pDamage) = BATTLE_CriDamageCalc( attackindex, defindex );
		}else{
			(*pDamage) = BATTLE_DamageCalc( attackindex, defindex );
		}
		iRet = BATTLE_RET_CRITICAL;
		if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
			// 勇士武器专精技能升级
			PROFESSION_SKILL_WEAPON_FOCUS_LVEVEL_UP( attackindex, "PROFESSION_WEAPON_FOCUS" );
			// 勇士二刀流技能升级
			PROFESSION_SKILL_DUAL_WEAPON_LVEVEL_UP( attackindex, "PROFESSION_DUAL_WEAPON" );
		}
	}else{
		(*pDamage) = BATTLE_DamageCalc( attackindex, defindex );
		iRet = BATTLE_RET_NORMAL;
	}
	
	if( opt == BATTLE_COM_S_GBREAK){
		;;
	}else
#ifdef _SKILL_GUARDBREAK2//破除防御2 vincent add 2002/05/20
  if( opt == BATTLE_COM_S_GBREAK2){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD){
			(*pDamage) = (*pDamage)*1.3;
		}else{
			(*pDamage) = (*pDamage)*0.7;
		}
	}else
#endif

#ifdef _PETSKILL_REGRET
	if( opt == BATTLE_COM_S_REGRET2 ){// 人物伤害减少
		(*pDamage) = (*pDamage)*0.8;
	}else
#endif			
#ifdef _SONIC_ATTACK				// WON ADD 音波攻击
	if( opt == BATTLE_COM_S_SONIC2 ){// 人物伤害减半
		(*pDamage) = (*pDamage)*0.5;
	}else
#endif
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
		&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0 ){
		(*pDamage) = BATTLE_GuardAdjust( (*pDamage) );
	}
	if( (*pDamage) < 1 ) (*pDamage) = RAND( 0, 1 );
				
#ifdef _PREVENT_TEAMATTACK	// 防止同队互打
	if (CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ||
		CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ) {
		int myside = CHAR_getWorkInt( defindex, CHAR_WORKBATTLESIDE);//attackindex
		if( myside == CHAR_getWorkInt( attackindex, CHAR_WORKBATTLESIDE)){
			if( CHAR_getWorkInt( attackindex, CHAR_WORKCONFUSION) > 0 
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
					|| CHAR_getWorkInt( attackindex, CHAR_WORKINSTIGATE) > 0	// 挑拨	
#endif
#ifdef _PROFESSION_ADDSKILL
					|| CHAR_getWorkInt( attackindex, CHAR_WORKANNEX) > 0 //附身
#endif
				){					//攻击方异常混乱
#ifdef _PETSKILL_TEMPTATION
			}else if( CHAR_getWorkInt( attackindex, CHAR_WORKTEMPTATION ) > 0 ){//水漾诱惑							
#endif
			}else if( CHAR_getInt( attackindex, CHAR_WHICHTYPE) == CHAR_TYPEPET &&
				CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEFLG) & CHAR_BATTLEFLG_AIBAD ){	//nono宠
			}else if( CHAR_getInt( defindex, CHAR_WHICHTYPE) == CHAR_TYPEPET && 
				CHAR_getWorkInt( defindex, CHAR_WORKPLAYERINDEX) == attackindex ){		// 自已宠物
			}else {
				int myhp, mymaxhp;
				mymaxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP);
				myhp = CHAR_getInt( defindex, CHAR_HP ) - *pDamage;
				mymaxhp = mymaxhp *0.40;
				if( mymaxhp >= myhp || mymaxhp < 40 ){
					*pDamage = 0;
				}else {
					*pDamage = 1;
				}
			}
		}
	}
#endif
				
#ifdef _EQUIT_ARRANGE
	if( BATTLE_ArrangeCheck( attackindex, defindex) == TRUE ){
		*pDamage *= 0.1;
		iRet = BATTLE_RET_ARRANGE;
		
		// 勇士格挡技能增加
		if( (CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER) )
			PROFESSION_SKILL_LVEVEL_UP( defindex, "PROFESSION_DEFLECT" );
		}
#endif
			
	if( (*pDamage) == 0 ){
		iRet = BATTLE_RET_MISS;
		if( GuardianIndex != -1 ){
			iRet = BATTLE_RET_NORMAL;
			*pDamage = 1;
		}else if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD 
			&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0 ){
			iRet = BATTLE_RET_ALLGUARD;
		}
	}
			
	(*pDamage) *= gBattleDamageModyfy;
	return iRet;
}

#ifdef _PETSKILL_SETDUCK
BOOL BATTLE_CheckMySkillDuck( int charaindex )
{
	int SDuck=0,rad=0;
	if( !CHAR_CHECKINDEX( charaindex) )
		return FALSE;
	if( (SDuck=CHAR_getWorkInt( charaindex, CHAR_MYSKILLDUCK)) > 0 ){
		int duck1 = CHAR_getWorkInt( charaindex, CHAR_MYSKILLDUCKPOWER);
		if( (rad = (rand()%100)) > duck1 ){
			return FALSE;
		}else	{//回避
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
int BATTLE_check_profession_duck( int charaindex, int per )
{

	int SDuck=0;

	if( (SDuck=CHAR_getWorkInt( charaindex, CHAR_WORK_P_DUCK)) > 0 ){
		int duck1 = CHAR_getWorkInt( charaindex, CHAR_WORKMOD_P_DUCK);		
		per = per * ( 100 + duck1 ) / 100;
	}
		
	return per;
}
#endif


int BATTLE_GetDamageReact( int charaindex )
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKDAMAGEVANISH ) > 0 ){
		return BATTLE_MD_VANISH;
	}
	if( CHAR_getWorkInt( charaindex, CHAR_WORKDAMAGEABSROB ) > 0 ){
		return BATTLE_MD_ABSROB;
	}
	if( CHAR_getWorkInt( charaindex, CHAR_WORKDAMAGEREFLEC ) > 0 ){
		return BATTLE_MD_REFLEC;
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( CHAR_getWorkInt( charaindex, CHAR_WORKTRAP ) > 0 ){	//陷阱	
		return BATTLE_MD_TRAP;
	}
#endif
#ifdef _PETSKILL_ACUPUNCTURE
	if( CHAR_getWorkInt( charaindex, CHAR_WORKACUPUNCTURE ) > 0 ){	//针刺外皮
	    //print("\n针刺:BATTLE_GetDamageReact");
	 	return BATTLE_MD_ACUPUNCTURE;
	}
#endif
	return 0;
}

int BATTLE_DamageSub( int attackindex, int defindex, int *pDamage, int *pPetDamage, int *pRefrect )
{

	int hp = 0, addpoint = 0, maxhp, IsUltimate = 0, react, work, battleindex;
	int damage, playerdamage, petdamage;
	int attackpet, defpet;
	int pethp = 0, petmaxhp = 0;
	int defquick = 0, petquick = 0;
	
	damage = *pDamage;
	petdamage = *pPetDamage;
	playerdamage = damage;
	if( damage <= 0 )return 0;

#ifdef _PETSKILL_SHOWMERCY   
	if( CHAR_getInt( defindex, CHAR_HP ) - damage <= 0 
		&& CHAR_getWorkInt(attackindex, CHAR_WORKBATTLECOM1) == BATTLE_COM_S_SHOWMERCY ){
	    damage = CHAR_getInt( defindex, CHAR_HP ) - 1;
		*pDamage = damage;
		playerdamage = damage;
	}
#endif

	attackpet = BATTLE_getRidePet( attackindex );
	defpet = BATTLE_getRidePet( defindex );
	if( defpet != -1 ) {
		defquick = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER );
		petquick = CHAR_getWorkInt( defpet, CHAR_WORKDEFENCEPOWER );
	}

	if( *pRefrect != -1 ){
		react = BATTLE_GetDamageReact( defindex );
	}else{
		react = BATTLE_MD_NONE;	
	}
	battleindex = CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX );

	if( react == BATTLE_MD_REFLEC ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			*pRefrect = BATTLE_MD_REFLEC;
		}else{
			*pRefrect = BATTLE_MD_NONE;
		}
	}else{
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE )
			*pRefrect = BATTLE_MD_TRAP;
		else
			*pRefrect = BATTLE_MD_NONE;
	}
#endif

#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ){
        if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			*pRefrect = BATTLE_MD_ACUPUNCTURE;
		}else{
			*pRefrect = BATTLE_MD_NONE;
		}
	}
#endif

	if( react == BATTLE_MD_ABSROB ){
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet == -1 ) {
			hp += damage;
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			playerdamage = (damage*petquick)/(defquick+petquick) +1 ;
			petdamage = damage - playerdamage +1 ;
			hp += playerdamage;
			pethp += petdamage;
			
		}
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEABSROB ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEABSROB, max( work, 0 ) );
		*pRefrect = BATTLE_MD_ABSROB;
	}else
	if( *pRefrect == BATTLE_MD_REFLEC ){
#ifdef _PETSKILL_BATTLE_MODEL
		if(CHAR_getWorkInt(defindex,CHAR_NPCWORKINT1) == BATTLE_COM_S_BATTLE_MODEL){
			work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEREFLEC ) - 1;
			CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEREFLEC, max( work, 0 ) );
			hp = CHAR_getInt(defindex,CHAR_HP);
			pethp = CHAR_getInt(defpet,CHAR_HP);
		}
		else
#endif
		{
			hp = CHAR_getInt( attackindex, CHAR_HP );
			if( attackpet == -1 ) {
				hp -= damage;
			}else {
				pethp = CHAR_getInt( attackpet, CHAR_HP );
				defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
				petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
				if( pethp > 0 ) {
					playerdamage = (damage*petquick)/(defquick+petquick) +1;
					petdamage = damage - playerdamage +1 ;
					hp -= playerdamage;
					pethp -= petdamage;
				}else
					hp -= damage;
			}
			work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEREFLEC ) - 1;
			CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEREFLEC, max( work, 0 ) );
			defindex = attackindex;
			defpet = attackpet;
		}
	}else
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( *pRefrect == BATTLE_MD_TRAP ){		//陷阱	
#ifdef _PETSKILL_BATTLE_MODEL
		if(CHAR_getWorkInt(defindex,CHAR_NPCWORKINT1) == BATTLE_COM_S_BATTLE_MODEL){
			CHAR_setWorkInt( defindex, CHAR_WORKTRAP, 0 );
			CHAR_setWorkInt( defindex, CHAR_WORKMODTRAP, 0 );
			hp = CHAR_getInt(defindex,CHAR_HP);
			pethp = CHAR_getInt(defpet,CHAR_HP);
		}
		else
#endif
		{
			int value=0;
			value = CHAR_getWorkInt( defindex, CHAR_WORKMODTRAP );
			damage = value;
			
			playerdamage = damage;
			
			hp = CHAR_getInt( attackindex, CHAR_HP );
			if( attackpet == -1 ) {
				hp -= damage;
			}else {
				pethp = CHAR_getInt( attackpet, CHAR_HP );
				defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
				petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
				if( pethp > 0 ) {
					playerdamage = (damage*petquick)/(defquick+petquick) +1;
					petdamage = damage - playerdamage +1 ;
					hp -= playerdamage;
					pethp -= petdamage;
				}else
					hp -= damage;
			}
			
			CHAR_setWorkInt( defindex, CHAR_WORKTRAP, 0 );
			CHAR_setWorkInt( defindex, CHAR_WORKMODTRAP, 0 );
			
			defindex = attackindex;
			defpet = attackpet;	
		}
	}else
#endif
#ifdef _PETSKILL_ACUPUNCTURE
	if( *pRefrect == BATTLE_MD_ACUPUNCTURE ){ //针刺外皮
		
		if( damage%2 != 0 )
			damage+=1;
        playerdamage = damage;

		//先扣被攻击方的血,因为之後的动作都是对攻击方做的
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= damage;
			if(hp<0)hp=0;
			maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	        CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			defquick = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( defpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				if(playerdamage%2 != 0) playerdamage+=1;
				if(petdamage%2 != 0) petdamage+=1;
				hp -= playerdamage;
				pethp -= petdamage;
				if(hp<0)hp=0;
				maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	            CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
				if( pethp < 0 )	pethp = 0;
		        petmaxhp = CHAR_getWorkInt( defpet, CHAR_WORKMAXHP );
		        CHAR_setInt( defpet, CHAR_HP, min( pethp, petmaxhp ) );
			}else{
				hp -= damage;
				if(hp<0)hp=0;
				maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	            CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
			}
		}

		//Change 2003/08/22
		if( (damage) >= CHAR_getWorkInt( defindex, CHAR_WORKMAXHP ) * 1.2 + 20 ){
            int DefSide=0,i;
			int defNo = BATTLE_Index2No( battleindex, defindex );
			CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, 0 );
            
			//死亡的宠物改为休息
			if( defNo >= SIDE_OFFSET ){
		       i = defNo - SIDE_OFFSET;
		       DefSide = 1;
			}else{
		       i = defNo;
			}
        	i = defNo-DefSide*SIDE_OFFSET;  
		    BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
		}

		//针刺外皮技能回合数设为0
		CHAR_setWorkInt( defindex, CHAR_WORKACUPUNCTURE, 0);
		//if( CHAR_getWorkInt( defindex, CHAR_WORKSLEEP ) > 0 )//检查有无昏睡
	    //    CHAR_setWorkInt( defindex, CHAR_WORKSLEEP, 0);

#ifdef _PETSKILL_BATTLE_MODEL
		if(CHAR_getWorkInt(defindex,CHAR_NPCWORKINT1) != BATTLE_COM_S_BATTLE_MODEL)
#endif
		{
			//扣攻击方的血
			hp = CHAR_getInt( attackindex, CHAR_HP );
			if( attackpet == -1 ) {
				hp -= damage/2;
			}else {
				pethp = CHAR_getInt( attackpet, CHAR_HP );
				defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
				petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
				if( pethp > 0 ) {
					playerdamage = (damage*petquick)/(defquick+petquick) +1;
					petdamage = damage - playerdamage +1 ;
					if(playerdamage%2 != 0) playerdamage+=1;
					if(petdamage%2 != 0) petdamage+=1;
					hp -= playerdamage/2;
					pethp -= petdamage/2;
				}else
					hp -= damage/2;
			}
			playerdamage /= 2;
       
			defindex = attackindex;
			defpet = attackpet;
		}
	}else
#endif
	if( react == BATTLE_MD_VANISH ){
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet != -1 )
			pethp = CHAR_getInt( defpet, CHAR_HP );
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEVANISH ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEVANISH, max( work, 0 ) );
		*pRefrect = BATTLE_MD_VANISH;
		//andy_log
		//print("*pRefrect = BATTLE_MD_VANISH !!\n");
	}else{
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet == -1 ) {
			hp -= damage;
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			if( pethp > 0 ) {
				//andy_edit 2002/07/31
				defquick = defquick>0?defquick:1;
				petquick = petquick>0?petquick:1;
				playerdamage = (damage*petquick)/(defquick+petquick)+ 1;
				//playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1;
				hp -= playerdamage;
				pethp -= petdamage;
				//print(" def_damage:%d ridepet_damage:%d ", (damage*petquick)/(defquick+petquick), damage - playerdamage);
			}else
				hp -= damage;
		}
		*pRefrect = 0;
	}

	if( hp < 0 ){
		addpoint = -hp;
		hp = 0;
	}


	maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
	if( defpet != -1 ) {
		if( pethp < 0 )	pethp = 0;
		petmaxhp = CHAR_getWorkInt( defpet, CHAR_WORKMAXHP );
		CHAR_setInt( defpet, CHAR_HP, min( pethp, petmaxhp ) );
		// Robin 0804
		if( pethp <= 0 ) {
			CHAR_setInt( defindex, CHAR_RIDEPET, -1 );
			BATTLE_changeRideImage( defindex );
			CHAR_setWorkInt( defindex, CHAR_WORKPETFALL, 1 );
			//print(" defpet_die! ");
		}
	}
	
	if( (damage) >= maxhp * 1.2 + 20 ){
		IsUltimate = 2;
	}else{
		if( addpoint > 0 ){
			addpoint += CHAR_getWorkInt( defindex, CHAR_WORKULTIMATE );
			CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, addpoint );
			if( addpoint >= maxhp * 1.2 + 20 )IsUltimate = 1;
		}
	}

	if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P ){
	}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
	if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) IsUltimate = 0;
#endif

	if( IsUltimate ){
		CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, 0 );
	}
	if( playerdamage < 0 ) playerdamage = 0;
	if( petdamage < 0 ) petdamage = 0;
	*pDamage = playerdamage;
	*pPetDamage = petdamage;

	return IsUltimate;
}

#ifdef _PETSKILL_FIREKILL //火线猎杀专用
int BATTLE_DamageSub_FIREKILL( int attackindex, int defindex, int *pDamage, int *pPetDamage, int *pRefrect )
{

	int hp, addpoint = 0, maxhp, IsUltimate = 0, react, work, battleindex;
	int damage, playerdamage, petdamage;
	int attackpet, defpet;
	int pethp = 0, petmaxhp = 0;
	int defquick = 0, petquick = 0;
	damage = *pDamage;
	petdamage = *pPetDamage;
	playerdamage = damage;
	if( damage <= 0 )return 0;

	attackpet = BATTLE_getRidePet( attackindex );
	defpet = BATTLE_getRidePet( defindex );
	if( defpet != -1 ) {
		defquick = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER );
		petquick = CHAR_getWorkInt( defpet, CHAR_WORKDEFENCEPOWER );
	}

	if( *pRefrect != -1 ){
		react = BATTLE_GetDamageReact( defindex );
	}else{
		react = BATTLE_MD_NONE;	
	}

	react = BATTLE_MD_NONE;	

	battleindex = CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX );

	if( react == BATTLE_MD_REFLEC ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			*pRefrect = BATTLE_MD_REFLEC;
		}else{
			*pRefrect = BATTLE_MD_NONE;
		}
	}else{
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE )
			*pRefrect = BATTLE_MD_TRAP;
		else
			*pRefrect = BATTLE_MD_NONE;
	}
#endif

#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ){
        if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			*pRefrect = BATTLE_MD_ACUPUNCTURE;
		}else{
			*pRefrect = BATTLE_MD_NONE;
		}
	}
#endif

	if( react == BATTLE_MD_ABSROB ){
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet == -1 ) {
			hp += damage;
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			playerdamage = (damage*petquick)/(defquick+petquick) +1 ;
			petdamage = damage - playerdamage +1 ;
			hp += playerdamage;
			pethp += petdamage;
			
		}
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEABSROB ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEABSROB, max( work, 0 ) );
		*pRefrect = BATTLE_MD_ABSROB;
	}else
	if( *pRefrect == BATTLE_MD_REFLEC ){
		hp = CHAR_getInt( attackindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= damage;
		}else {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				hp -= playerdamage;
				pethp -= petdamage;
			}else
				hp -= damage;
		}
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEREFLEC ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEREFLEC, max( work, 0 ) );
		defindex = attackindex;
		defpet = attackpet;

	}else
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( *pRefrect == BATTLE_MD_TRAP ){		//陷阱	
		int value=0;
		value = CHAR_getWorkInt( defindex, CHAR_WORKMODTRAP );
		damage = value;

		playerdamage = damage;

		hp = CHAR_getInt( attackindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= damage;
		}else {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				hp -= playerdamage;
				pethp -= petdamage;
			}else
				hp -= damage;
		}

		CHAR_setWorkInt( defindex, CHAR_WORKTRAP, 0 );
		CHAR_setWorkInt( defindex, CHAR_WORKMODTRAP, 0 );

		defindex = attackindex;
		defpet = attackpet;	

	}else
#endif
#ifdef _PETSKILL_ACUPUNCTURE
	if( *pRefrect == BATTLE_MD_ACUPUNCTURE ){ //针刺外皮
		
		if( damage%2 != 0 )
			damage+=1;
        playerdamage = damage;

		//先扣被攻击方的血,因为之後的动作都是对攻击方做的
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= damage;
			if(hp<0)hp=0;
			maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	        CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			defquick = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( defpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				if(playerdamage%2 != 0) playerdamage+=1;
				if(petdamage%2 != 0) petdamage+=1;
				hp -= playerdamage;
				pethp -= petdamage;
				if(hp<0)hp=0;
				maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	            CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
				if( pethp < 0 )	pethp = 0;
		        petmaxhp = CHAR_getWorkInt( defpet, CHAR_WORKMAXHP );
		        CHAR_setInt( defpet, CHAR_HP, min( pethp, petmaxhp ) );
			}else{
				hp -= damage;
				if(hp<0)hp=0;
				maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	            CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
			}
		}

		//Change 2003/08/22
		if( (damage) >= CHAR_getWorkInt( defindex, CHAR_WORKMAXHP ) * 1.2 + 20 ){
            int DefSide=0,i;
			int defNo = BATTLE_Index2No( battleindex, defindex );
			CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, 0 );
            
			//死亡的宠物改为休息
			if( defNo >= SIDE_OFFSET ){
		       i = defNo - SIDE_OFFSET;
		       DefSide = 1;
			}else{
		       i = defNo;
			}
        	i = defNo-DefSide*SIDE_OFFSET;  
		    BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
		}

		//针刺外皮技能回合数设为0
		CHAR_setWorkInt( defindex, CHAR_WORKACUPUNCTURE, 0);
		//if( CHAR_getWorkInt( defindex, CHAR_WORKSLEEP ) > 0 )//检查有无昏睡
	    //    CHAR_setWorkInt( defindex, CHAR_WORKSLEEP, 0);

        //扣攻击方的血
		hp = CHAR_getInt( attackindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= damage/2;
		}else {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				if(playerdamage%2 != 0) playerdamage+=1;
				if(petdamage%2 != 0) petdamage+=1;
				hp -= playerdamage/2;
				pethp -= petdamage/2;
			}else
				hp -= damage/2;
		}
		
		playerdamage /= 2;
       
		defindex = attackindex;
		defpet = attackpet;
	
	}else
#endif
	if( react == BATTLE_MD_VANISH ){
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet != -1 )
			pethp = CHAR_getInt( defpet, CHAR_HP );
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEVANISH ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEVANISH, max( work, 0 ) );
		*pRefrect = BATTLE_MD_VANISH;
		//andy_log
		//print("*pRefrect = BATTLE_MD_VANISH !!\n");
	}else{
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet == -1 ) {
			hp -= damage;
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			if( pethp > 0 ) {
				//andy_edit 2002/07/31
				defquick = defquick>0?defquick:1;
				petquick = petquick>0?petquick:1;
				playerdamage = (damage*petquick)/(defquick+petquick)+ 1;
				//playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1;
				hp -= playerdamage;
				pethp -= petdamage;
				//print(" def_damage:%d ridepet_damage:%d ", (damage*petquick)/(defquick+petquick), damage - playerdamage);
			}else
				hp -= damage;
		}
		*pRefrect = 0;
	}
	if( hp < 0 ){
		addpoint = -hp;
		hp = 0;
	}

	maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
	if( defpet != -1 ) {
		if( pethp < 0 )	pethp = 0;
		petmaxhp = CHAR_getWorkInt( defpet, CHAR_WORKMAXHP );
		CHAR_setInt( defpet, CHAR_HP, min( pethp, petmaxhp ) );
		// Robin 0804
		if( pethp <= 0 ) {
			CHAR_setInt( defindex, CHAR_RIDEPET, -1 );
			BATTLE_changeRideImage( defindex );
			CHAR_setWorkInt( defindex, CHAR_WORKPETFALL, 1 );
			//print(" defpet_die! ");
		}
	}
	
	if( (damage) >= maxhp * 1.2 + 20 ){
		IsUltimate = 2;
	}else{
		if( addpoint > 0 ){
			addpoint += CHAR_getWorkInt( defindex, CHAR_WORKULTIMATE );
			CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, addpoint );
			if( addpoint >= maxhp * 1.2 + 20 )IsUltimate = 1;
		}
	}

	if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P ){
	}

#ifdef _PETSKILL_LER
// 雷尔不能被打飞
	if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) IsUltimate = 0;
#endif

	if( IsUltimate ){
		CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, 0 );
	}
	if( playerdamage < 0 ) playerdamage = 0;
	if( petdamage < 0 ) petdamage = 0;
	*pDamage = playerdamage;
	*pPetDamage = petdamage;

	return IsUltimate;
}
#endif


// Robin 0910 RidePetDebug
int BATTLE_DamageSub2( int attackindex, int defindex, int *pDamage, int *pPetDamage, int *pRefrect )
{

	int hp, addpoint = 0, maxhp, IsUltimate = 0, react, work, battleindex;
	int damage, playerdamage, petdamage;
	int attackpet, defpet;
	int pethp = 0, petmaxhp = 0;

	damage = *pDamage;
	petdamage = *pPetDamage;
	playerdamage = *pDamage;

	if( damage <= 0 )return 0;
	
	attackpet = BATTLE_getRidePet( attackindex );
	defpet = BATTLE_getRidePet( defindex );

	if( *pRefrect != -1 ){
		react = BATTLE_GetDamageReact( defindex );
	}else{
		react = BATTLE_MD_NONE;
	}

	if( react == BATTLE_MD_REFLEC ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			*pRefrect = BATTLE_MD_REFLEC;
		}else{
			*pRefrect = BATTLE_MD_NONE;
		}
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE )
			*pRefrect = BATTLE_MD_TRAP;
		else
			*pRefrect = BATTLE_MD_NONE;
	}
#endif

	if( react == BATTLE_MD_ABSROB ){

		hp = CHAR_getInt( defindex, CHAR_HP );
		hp += playerdamage;
		if( defpet != -1 ) {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			pethp += petdamage;
		}

		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEABSROB ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEABSROB, max( work, 0 ) );

		*pRefrect = BATTLE_MD_ABSROB;

	}else
	if( *pRefrect == BATTLE_MD_REFLEC ){

		hp = CHAR_getInt( attackindex, CHAR_HP );
		hp -= playerdamage;
		if( attackpet != -1 ) {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			if( pethp > 0 ) {
				pethp -= petdamage;
			}
		}

		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEREFLEC ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEREFLEC, max( work, 0 ) );

		defindex = attackindex;
		defpet = attackpet;

	}else
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( *pRefrect == BATTLE_MD_TRAP ){	//陷阱	

		hp = CHAR_getInt( attackindex, CHAR_HP );
		hp -= playerdamage;
		if( attackpet != -1 ) {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			if( pethp > 0 ) {
				pethp -= petdamage;
			}
		}

		CHAR_setWorkInt( defindex, CHAR_WORKTRAP, 0 );
		CHAR_setWorkInt( defindex, CHAR_WORKMODTRAP, 0 );

		defindex = attackindex;
		defpet = attackpet;

	}else
#endif
	if( react == BATTLE_MD_VANISH ){
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet != -1 )
			pethp = CHAR_getInt( defpet, CHAR_HP );

		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEVANISH ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEVANISH, max( work, 0 ) );

		*pRefrect = BATTLE_MD_VANISH;

	}else{
		hp = CHAR_getInt( defindex, CHAR_HP );
		hp -= playerdamage;
		if( defpet != -1 ){
			pethp = CHAR_getInt( defpet, CHAR_HP );
			if( pethp > 0 ) {
				pethp -= petdamage;
				//print(" def_damage:%d ridepet_damage:%d ", (damage*petquick)/(defquick+petquick), damage - playerdamage);
			}
		}
		*pRefrect = 0;
	}

	if( hp < 0 ){
		addpoint = -hp;
		hp = 0;
	}

	maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );

	CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
	
	
	if( defpet != -1 ) {
		if( pethp < 0 )	pethp = 0;
		petmaxhp = CHAR_getWorkInt( defpet, CHAR_WORKMAXHP );
		CHAR_setInt( defpet, CHAR_HP, min( pethp, petmaxhp ) );
		
		// Robin 0804
		if( pethp <= 0 ) {
			CHAR_setInt( defindex, CHAR_RIDEPET, -1 );
			BATTLE_changeRideImage( defindex );
			CHAR_setWorkInt( defindex, CHAR_WORKPETFALL, 1 );
			//print(" defpet_die! ");
		}
	}
	
	if( (damage) >= maxhp * 1.2 + 20 ){
		IsUltimate = 2;
	}else{
		if( addpoint > 0 ){
			addpoint += CHAR_getWorkInt( defindex, CHAR_WORKULTIMATE );
			CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, addpoint );
			if( addpoint >= maxhp * 1.2 + 20 )IsUltimate = 1;
		}
	}

	battleindex = CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX );
	if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P ){
	}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
	if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) IsUltimate = 0;
#endif

	if( IsUltimate ){
		CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, 0 );
	}

	//*pDamage = damage;
	*pDamage = playerdamage;
	*pPetDamage = petdamage;

	return IsUltimate;
}



void BATTLE_DamageSubCale(
	int attackindex,
	int defindex,
	int *pDamage,
	int *pPetDamage
)
{
	int pethp;
	int damage, playerdamage, petdamage;
	int attackpet, defpet;
	int defquick = 0, petquick = 0;

	damage = *pDamage;
	petdamage = *pPetDamage;
	playerdamage = damage;

	attackpet = BATTLE_getRidePet( attackindex );
	defpet = BATTLE_getRidePet( defindex );

	defquick = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER );
	defquick = ( defquick < 1)?1:defquick;
	if( defpet != -1 ) {
		petquick = CHAR_getWorkInt( defpet, CHAR_WORKDEFENCEPOWER );
		petquick = ( petquick < 1)?1:petquick;

		pethp = CHAR_getInt( defpet, CHAR_HP );
		if( pethp > 0 ) {
			playerdamage = (damage*petquick)/(defquick+petquick);
			petdamage = damage - playerdamage;
			if( damage > 0 && playerdamage < 1 )
				playerdamage = 1;
		}
	}

	*pDamage = playerdamage;
	*pPetDamage = petdamage;

}

int BATTLE_Attack( int battleindex, int attackNo, int defNo )
{
	char szBuffer[512]="";
	char szCommand[1024];
	int damage = 0, petdamage = 0, attackindex, toindex, ultimate = 0,
		defindex, react = 0, statusDefNo, opt = 0;
	int flg = 0, iWork, par, perStatus, i, DefSide = 0, Guardian = -1;
	BOOL iRet = TRUE;
	int suitpoison=30;//基本中毒%

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = toindex = BATTLE_No2Index( battleindex, defNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )	return FALSE;
	if( CHAR_CHECKINDEX( defindex ) == FALSE )		return FALSE;


#ifdef _PETSKILL_RETRACE
    Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = 0;
#endif

	if( gWeponType == ITEM_BOW ){
		if( toindex < 0	|| CHAR_getInt( toindex, CHAR_HP ) <= 0 ){
			snprintf( szCommand, sizeof( szCommand ), "r%X|f0|d0|", defNo );
			BATTLESTR_ADD( szCommand );
			return FALSE;
		}
	}

	if( CHAR_getInt( toindex, CHAR_HP ) <= 0 )	return FALSE;

	if( CHAR_getInt( attackindex, CHAR_HP ) <= 0 )	return FALSE;

	// 光、镜、守
	if( BATTLE_GetDamageReact( attackindex ) > 0 )		iRet = FALSE;
	else if( BATTLE_GetDamageReact( defindex ) > 0 )	iRet = FALSE;

	// 攻击模式
	iWork = BATTLE_AttackSeq( attackindex, toindex, &damage, &Guardian, opt );

	if( Guardian >= 0 )
		defindex = BATTLE_No2Index( battleindex, Guardian );

	if( gDamageDiv != 0.0 && damage > 0 ){
		damage /= gDamageDiv;
		if( damage <= 0 )	damage = 1;
	}

	par = BATTLE_AttrAdjust( attackindex, defindex, 100 );
	react = 0;

	ultimate = BATTLE_DamageSub( attackindex, defindex, &damage, &petdamage, &react );

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP ){
		defindex = attackindex;
		statusDefNo = attackNo;
	}
#endif
#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ){
        defindex = attackindex;
		statusDefNo = attackNo;
	}
#endif
	if( react == BATTLE_MD_REFLEC ){
		defindex = attackindex;
		statusDefNo = attackNo;
	}else{
		if( 0 <= Guardian && Guardian <= 19 ){
			statusDefNo = Guardian;
		}else{
			statusDefNo = defNo;
		}
	}

#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ){ //针刺外皮时会无法正常解除被打方的状态,所以把index换回来
	    defindex = toindex = BATTLE_No2Index( battleindex, defNo );
        statusDefNo = defNo;
	}
#endif

	//昏睡时,在这里解除状态(也有其它的状态)
	if( damage > 0 && ( react != BATTLE_MD_ABSROB ) && ( react != BATTLE_MD_VANISH ) ){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}

#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ){
        defindex = attackindex;
		statusDefNo = attackNo;
	}
#endif

	switch( iWork ){
		case BATTLE_RET_ALLGUARD:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)朝向(%s)攻击,轻易地被躲开。",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex )
			//);
			flg |= BCF_NORMAL;
#ifdef _PETSKILL_RETRACE
            Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = BATTLE_RET_ALLGUARD;
#endif
			break;
		case BATTLE_RET_MISS:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)朝向(%s)攻击,没击中。",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex )
			//);
#ifdef _PETSKILL_RETRACE
            Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = BATTLE_RET_MISS;
#endif
			break;
		case BATTLE_RET_DODGE:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)攻击了(%s)躲开了(%.2f%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	gDuckPer
			//);
			flg |= BCF_DODGE;
#ifdef _PETSKILL_RETRACE
            Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = BATTLE_RET_DODGE;
#endif
			break;
		case BATTLE_RET_NORMAL:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)受到(%s)的攻击(%d)遭受损伤(%d:%d:%d%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	damage,
			//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
			//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
			//	par
			//);
			//andy_edit
			flg |= BATTLE_getReactFlg( defindex, react);
			flg |= BCF_NORMAL;
			break;
		case BATTLE_RET_CRITICAL:
			//snprintf( szBuffer, sizeof(szBuffer),
			//"(%s)向(%s)CRITICAL(%d%%)(%d)不可以(%d:%d:%d%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	(int)(gCriper*0.01),
			//	damage,
			//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
			//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
			//	par
			//);
			//andy_edit
			flg |= BATTLE_getReactFlg( defindex, react);
			flg |= BCF_KAISHIN;
			iRet = FALSE;
			break;
#ifdef _EQUIT_ARRANGE
		case BATTLE_RET_ARRANGE:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)攻击了(%s)挡掉了(%.2f%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	gDuckPer
			//);
			flg |= BCF_B_ARRANGE;
#ifdef _PETSKILL_RETRACE
            Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = BATTLE_RET_ARRANGE;
#endif
			break;
#endif
	}


	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
		&& CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0){
		strncat( szBuffer,  "(GUARD)", sizeof( szBuffer ) );
		flg |= BCF_GUARD;
		iRet = FALSE;
	}

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
			ultimate = 1;
		}else if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER &&  flg & BCF_KAISHIN ){
			if( RAND( 1, 100 ) < 50 ){
				ultimate = 1;
			}
		}

#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) ultimate = 0;
#endif
		iRet = FALSE;
		flg |= BCF_DEATH;

		if( ultimate == 1 ){
			flg |= BCF_ULTIMATE_1;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
		if( ultimate == 2 ){
			flg |= BCF_ULTIMATE_2;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
	}

	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;


#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ){ //针刺外皮不会反射特殊状态,所以要改回来
	    defindex = toindex = BATTLE_No2Index( battleindex, defNo );
        statusDefNo = defNo;
	}
	//注意:之後要不要再改回原本反射.......????
#endif
	
#ifdef _SUIT_ADDPART4
	if( gBattleStausChange == -1 && CHAR_getWorkInt( attackindex, CHAR_SUITPOISON ) > 0 )//带毒装备
		gBattleStausChange = 1,gBattleStausTurn = 3, suitpoison=CHAR_getWorkInt( attackindex, CHAR_SUITPOISON );
#endif

	if( damage > 0 && gBattleStausChange >= 0 ){
		if( BATTLE_StatusAttackCheck(
			attackindex,
			defindex,
			gBattleStausChange,
			suitpoison,
			40,
			2.0,
			&perStatus
			) == TRUE
		){
			CHAR_setWorkInt( defindex, StatusTbl[gBattleStausChange],
				gBattleStausTurn + 1 );

			if( gBattleStausChange == BATTLE_ST_DRUNK ){
				CHAR_setWorkInt( defindex, CHAR_WORKDRUNK,
					CHAR_getWorkInt( defindex, CHAR_WORKDRUNK ) / 2 );
			}

#ifdef _PET_SKILL_SARS				// WON ADD 毒煞蔓延
			if( gBattleStausChange == BATTLE_ST_SARS ){
				CHAR_setWorkInt( defindex, CHAR_WORKMODSARS, 1);
			}
#endif

			if( gBattleStausChange == BATTLE_ST_PARALYSIS
			||  gBattleStausChange == BATTLE_ST_SLEEP
			||  gBattleStausChange == BATTLE_ST_STONE
			||  gBattleStausChange == BATTLE_ST_BARRIER
			){
				CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
			}

			//snprintf( szBuffer, sizeof(szBuffer),
			//"(%s)变成(%s)(%d%%)",
			//	CHAR_getUseName( defindex ),
			//	aszStatusFull[gBattleStausChange],
			//	perStatus
			//);
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

			BATTLE_BadStatusString( statusDefNo, gBattleStausChange );
		}else{
			//snprintf( szBuffer, sizeof(szBuffer),
			//"(%s)无法成功\(%d%%)",
			//	aszStatusFull[gBattleStausChange],
			//	perStatus );
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		}
	}
#ifdef _SHOOTCHESTNUT	// Syu ADD 宠技：丢栗子
	if ( ( damage > 0 ) && ( CHAR_getWorkInt( attackindex , CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_ATTSHOOT ) ) {
		if ( RAND ( 1 , 5 ) > 4 ) {
			CHAR_setWorkInt( defindex, CHAR_WORKSLEEP , 3 ); //设定昏睡
			CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );//因为设定了昏睡,所以设定无动作
		}
	}
#endif
#ifdef	_PSKILL_GYRATE
	if ( ( damage > 0 ) && ( CHAR_getWorkInt( attackindex , CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_GYRATE ) && (CHAR_getWorkInt( attackindex , CHAR_WORKBATTLECOM5 ) >0) ) {
		if ( RAND ( 1 , 100 ) <= CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM7) ) {
			int iEffect = CHAR_getWorkInt( attackindex , CHAR_WORKBATTLECOM5 );
			CHAR_setWorkInt( defindex, StatusTbl[iEffect] , CHAR_getWorkInt( attackindex , CHAR_WORKBATTLECOM6 ) ); //设定昏睡
			CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );//因为设定了昏睡,所以设定无动作
		}
	}
#endif	
	if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
		if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
			flg |= BCF_CRUSH;
		}
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能	
	if( damage > 0 && (CHAR_getInt(attackindex, CHAR_WHICHTYPE)== CHAR_TYPEPLAYER) ){
		int i;
		// 检查火、冰、雷附体
		for( i=0; i<3; i++ ){
			int turn = 0, status = -1;
			
			turn = CHAR_getWorkInt( attackindex, CHAR_WORK_F_ENCLOSE_2+i );
			
			if( turn > 0 ){
				int skill_type = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 );
				int perStatus = 0, j = 0;				
				int skill_level = CHAR_getWorkInt( attackindex, CHAR_WORKMOD_F_ENCLOSE_2+i );
				char pszP[3][10] = { "烧","霜","电" };
				int img1 = 101697, img2 = 101698 + i;
				
				// 状态
				for( j = 1; j < BATTLE_ST_END; j ++ ){
					if( strncmp( pszP[i], aszStatus[j], 2 ) == 0 ){
						status = j;
						break;
					}
				}	

				// 命中率
				perStatus = 20 + skill_level * 2;

				// 回合数
				if( skill_level >= 10 )		turn = 3;
				else if( skill_level >= 5 ) turn = 2;
				else						turn = 1;

				// 雷附体一回合数
				if( i == 2 )	turn = 1;

				// 火、冰、雷附体附加攻击
// Terry add fix 忠犬bug
				if(Guardian >= 0) iRet = BATTLE_PROFESSION_RANG_ATTACK_DAMAGE(attackindex, attackNo, Guardian, skill_type, status, turn, perStatus, 0, img1, img2 );
				else iRet = BATTLE_PROFESSION_RANG_ATTACK_DAMAGE(attackindex, attackNo, defNo, skill_type, status, turn, perStatus, 0, img1, img2 );
				
				break;
			}
		}
	}
#endif


	if( 0 <= Guardian && Guardian <= 19 ){
		iRet = FALSE;
		flg |= BCF_GUARDIAN;
		snprintf( szCommand, sizeof( szCommand ),
			"r%X|f%X|d%X|p%X|g%X|",
			defNo,
			flg,
			damage,
			petdamage,
			Guardian );
	}else{	
			snprintf( szCommand, sizeof( szCommand ),
			    "r%X|f%X|d%X|p%X|",
			    defNo,
			    flg,
			    damage,
			    petdamage );
	}

	BATTLESTR_ADD( szCommand );

	defNo = BATTLE_Index2No( battleindex, defindex );

	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}else{
		i = defNo;
	}

	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}
	return iRet;

}

#ifdef _PETSKILL_FIREKILL
int BATTLE_Attack_FIREKILL( int battleindex, int attackNo, int defNo )
{
	char szBuffer[512]="";
	char szCommand[1024];
	int damage = 0, petdamage = 0, attackindex, toindex, ultimate = 0,
		defindex, react = 0, statusDefNo, opt = 0;
	int flg = 0, iWork, par, perStatus, i, DefSide = 0, Guardian = -1;
	BOOL iRet = TRUE;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = toindex = BATTLE_No2Index( battleindex, defNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )	return FALSE;
	if( CHAR_CHECKINDEX( defindex ) == FALSE )		return FALSE;


#ifdef _PETSKILL_RETRACE
    Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = 0;
#endif

	if( gWeponType == ITEM_BOW ){
		if( toindex < 0	|| CHAR_getInt( toindex, CHAR_HP ) <= 0 ){
			snprintf( szCommand, sizeof( szCommand ), "r%X|f0|d0|", defNo );
			BATTLESTR_ADD( szCommand );
			return FALSE;
		}
	}

	if( CHAR_getInt( toindex, CHAR_HP ) <= 0 )	return FALSE;

	if( CHAR_getInt( attackindex, CHAR_HP ) <= 0 )	return FALSE;

	// 光、镜、守
//	if( BATTLE_GetDamageReact( attackindex ) > 0 )		iRet = FALSE;
//	else if( BATTLE_GetDamageReact( defindex ) > 0 )	iRet = FALSE;

	// 攻击模式
	iWork = BATTLE_AttackSeq( attackindex, toindex, &damage, &Guardian, opt );

	if( Guardian >= 0 )
		defindex = BATTLE_No2Index( battleindex, Guardian );

	if( gDamageDiv != 0.0 && damage > 0 ){
		damage /= gDamageDiv;
		if( damage <= 0 )	damage = 1;
	}

	par = BATTLE_AttrAdjust( attackindex, defindex, 100 );
	react = 0;

	ultimate = BATTLE_DamageSub_FIREKILL( attackindex, defindex, &damage, &petdamage, &react );

//#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
//	if( react == BATTLE_MD_TRAP ){
//		defindex = attackindex;
//		statusDefNo = attackNo;
//	}
//#endif
//#ifdef _PETSKILL_ACUPUNCTURE
//    if( react == BATTLE_MD_ACUPUNCTURE ){
//        defindex = attackindex;
//		statusDefNo = attackNo;
//	}
//#endif
//	if( react == BATTLE_MD_REFLEC ){
//		defindex = attackindex;
//		statusDefNo = attackNo;
//	}else{
		if( 0 <= Guardian && Guardian <= 19 ){
			statusDefNo = Guardian;
		}else{
			statusDefNo = defNo;
		}
//	}

//#ifdef _PETSKILL_ACUPUNCTURE
//    if( react == BATTLE_MD_ACUPUNCTURE ){ //针刺外皮时会无法正常解除被打方的状态,所以把index换回来
//	    defindex = toindex = BATTLE_No2Index( battleindex, defNo );
//        statusDefNo = defNo;
//	}
//#endif

	//昏睡时,在这里解除状态(也有其它的状态)
	if( damage > 0 && ( react != BATTLE_MD_ABSROB ) && ( react != BATTLE_MD_VANISH ) ){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}

//#ifdef _PETSKILL_ACUPUNCTURE
//    if( react == BATTLE_MD_ACUPUNCTURE ){
//        defindex = attackindex;
//		statusDefNo = attackNo;
//	}
//#endif

	switch( iWork ){
		case BATTLE_RET_ALLGUARD:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)朝向(%s)攻击,轻易地被躲开。",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex )
			//);
			flg |= BCF_NORMAL;
			break;
		case BATTLE_RET_MISS:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)朝向(%s)攻击,没击中。",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex )
			//);
			break;
		case BATTLE_RET_DODGE:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)攻击了(%s)躲开了(%.2f%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	gDuckPer
			//);
			flg |= BCF_DODGE;
#ifdef _PETSKILL_RETRACE
            Battle_Attack_ReturnData_x.Battle_Attack_ReturnData = BATTLE_RET_DODGE;
#endif
			break;
		case BATTLE_RET_NORMAL:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)受到(%s)的攻击(%d)遭受损伤(%d:%d:%d%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	damage,
			//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
			//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
			//	par
			//);
			//andy_edit
			flg |= BATTLE_getReactFlg( defindex, react);
			flg |= BCF_NORMAL;
			break;
		case BATTLE_RET_CRITICAL:
			//snprintf( szBuffer, sizeof(szBuffer),
			//"(%s)向(%s)CRITICAL(%d%%)(%d)不可以(%d:%d:%d%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	(int)(gCriper*0.01),
			//	damage,
			//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
			//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
			//	par
			//);
			//andy_edit
			flg |= BATTLE_getReactFlg( defindex, react);
			flg |= BCF_KAISHIN;
			iRet = FALSE;
			break;
#ifdef _EQUIT_ARRANGE
		case BATTLE_RET_ARRANGE:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)攻击了(%s)挡掉了(%.2f%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	gDuckPer
			//);
			flg |= BCF_B_ARRANGE;
			break;
#endif
	}


	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
		&& CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0){
		strncat( szBuffer,  "(GUARD)", sizeof( szBuffer ) );
		flg |= BCF_GUARD;
		iRet = FALSE;
	}

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
			ultimate = 1;
		}else if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER &&  flg & BCF_KAISHIN ){
			if( RAND( 1, 100 ) < 50 ){
				ultimate = 1;
			}
		}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) ultimate = 0;
#endif
		iRet = FALSE;
		flg |= BCF_DEATH;

		if( ultimate == 1 ){
			flg |= BCF_ULTIMATE_1;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
		if( ultimate == 2 ){
			flg |= BCF_ULTIMATE_2;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
	}

	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;


//#ifdef _PETSKILL_ACUPUNCTURE
//    if( react == BATTLE_MD_ACUPUNCTURE ){ //针刺外皮不会反射特殊状态,所以要改回来
//	    defindex = toindex = BATTLE_No2Index( battleindex, defNo );
//       statusDefNo = defNo;
//	}
//	//注意:之後要不要再改回原本反射.......????
//#endif
	

	if( damage > 0 && gBattleStausChange >= 0 ){
		if( BATTLE_StatusAttackCheck(
			attackindex,
			defindex,
			gBattleStausChange,
			30,
			40,
			2.0,
			&perStatus
			) == TRUE
		){
			CHAR_setWorkInt( defindex, StatusTbl[gBattleStausChange],
				gBattleStausTurn + 1 );

			if( gBattleStausChange == BATTLE_ST_DRUNK ){
				CHAR_setWorkInt( defindex, CHAR_WORKDRUNK,
					CHAR_getWorkInt( defindex, CHAR_WORKDRUNK ) / 2 );
			}

#ifdef _PET_SKILL_SARS				// WON ADD 毒煞蔓延
			if( gBattleStausChange == BATTLE_ST_SARS ){
				CHAR_setWorkInt( defindex, CHAR_WORKMODSARS, 1);
			}
#endif

			if( gBattleStausChange == BATTLE_ST_PARALYSIS
			||  gBattleStausChange == BATTLE_ST_SLEEP
			||  gBattleStausChange == BATTLE_ST_STONE
			||  gBattleStausChange == BATTLE_ST_BARRIER
			){
				CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
			}

			//snprintf( szBuffer, sizeof(szBuffer),
			//"(%s)变成(%s)(%d%%)",
			//	CHAR_getUseName( defindex ),
			//	aszStatusFull[gBattleStausChange],
			//	perStatus
			//);
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

			BATTLE_BadStatusString( statusDefNo, gBattleStausChange );
		}else{
			//snprintf( szBuffer, sizeof(szBuffer),
			//"(%s)无法成功\(%d%%)",
			//	aszStatusFull[gBattleStausChange],
			//	perStatus );
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		}
	}
#ifdef _SHOOTCHESTNUT	// Syu ADD 宠技：丢栗子
	if ( ( damage > 0 ) && ( CHAR_getWorkInt( attackindex , CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_ATTSHOOT ) ) {
		if ( RAND ( 1 , 5 ) > 4 ) {
			CHAR_setWorkInt( defindex, CHAR_WORKSLEEP , 3 ); //设定昏睡
			CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );//因为设定了昏睡,所以设定无动作
		}
	}
#endif
	if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
		if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
			flg |= BCF_CRUSH;
		}
	}

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能	
	if( damage > 0 && (CHAR_getInt(attackindex, CHAR_WHICHTYPE)== CHAR_TYPEPLAYER) ){
		int i;
		// 检查火、冰、雷附体
		for( i=0; i<3; i++ ){
			int turn = 0, status = -1;
			
			turn = CHAR_getWorkInt( attackindex, CHAR_WORK_F_ENCLOSE_2+i );
			
			if( turn > 0 ){
				int skill_type = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 );
				int perStatus = 0, j = 0;				
				int skill_level = CHAR_getWorkInt( attackindex, CHAR_WORKMOD_F_ENCLOSE_2+i );
				char pszP[3][10] = { "烧","霜","电" };
				int img1 = 101697, img2 = 101698 + i;
				
				// 状态
				for( j = 1; j < BATTLE_ST_END; j ++ ){
					if( strncmp( pszP[i], aszStatus[j], 2 ) == 0 ){
						status = j;
						break;
					}
				}	

				// 命中率
				perStatus = 20 + skill_level * 2;

				// 回合数
				if( skill_level >= 10 )		turn = 3;
				else if( skill_level >= 5 ) turn = 2;
				else						turn = 1;

				// 雷附体一回合数
				if( i == 2 )	turn = 1;

				// 火、冰、雷附体附加攻击
// Terry add fix 忠犬bug
				if(Guardian >= 0) iRet = BATTLE_PROFESSION_RANG_ATTACK_DAMAGE(attackindex, attackNo, Guardian, skill_type, status, turn, perStatus, 0, img1, img2 );
				else iRet = BATTLE_PROFESSION_RANG_ATTACK_DAMAGE(attackindex, attackNo, defNo, skill_type, status, turn, perStatus, 0, img1, img2 );
				
				break;
			}
		}
	}
#endif

	if( 0 <= Guardian && Guardian <= 19 ){
		iRet = FALSE;
		flg |= BCF_GUARDIAN;
		snprintf( szCommand, sizeof( szCommand ),
			"r%X|f%X|d%X|p%X|g%X|",
			defNo,
			flg,
			damage,
			petdamage,
			Guardian );
	}else{	
			snprintf( szCommand, sizeof( szCommand ),
			    "r%X|f%X|d%X|p%X|",
			    defNo,
			    flg,
			    damage,
			    petdamage );
	}

	BATTLESTR_ADD( szCommand );

	defNo = BATTLE_Index2No( battleindex, defindex );

	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}else{
		i = defNo;
	}

	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}
	return iRet;

}

#endif

	
enum {
	BATTLE_C_NONE = 0,
	BATTLE_C_CLAW,
	BATTLE_C_AXE,
	BATTLE_C_CLUB,
	BATTLE_C_SPEAR,
	BATTLE_C_BOW,
	BATTLE_C_THROU,
	BATTLE_C_OTHER,
	BATTLE_C_MAX
};

static int CounterTbl[] = {
	 10, 9, 8, 8, 5, 0, 0, 0 ,
	 10, 9, 7, 7, 6, 0, 0, 0 ,
	  9, 8,10,10, 7, 0, 0, 0 ,
	  8, 8,10,10, 7, 0, 0, 0 ,
	  6, 6, 8, 8, 9, 0, 0, 0 ,
	  0, 0, 0, 0, 0, 0, 0, 0 ,
	  0, 0, 0, 0, 0, 0, 0, 0 ,
};

static int BATTLE_ItemType2ItemMap( int type ){
	int iRet = BATTLE_C_NONE;
	if( type == ITEM_FIST ) iRet = BATTLE_C_CLAW;
	if( type == ITEM_AXE ) iRet = BATTLE_C_AXE;
	if( type == ITEM_CLUB ) iRet = BATTLE_C_CLUB;
	if( type == ITEM_BOW ) iRet = BATTLE_C_BOW;
	if( type == ITEM_BOOMERANG ) iRet = BATTLE_C_THROU;
	if( type == ITEM_BOUNDTHROW ) iRet = BATTLE_C_THROU;
	if( type == ITEM_BREAKTHROW ) iRet = BATTLE_C_THROU;
	return iRet;
}

static BOOL BATTLE_CounterCheckPlayer( int attackindex, int defindex, int *pPar )
{
	int
		At_SoubiIndex, At_SoubiType,
		Df_SoubiIndex, Df_SoubiType,
		At_Soubi, CriPer,
		At_Luck = 0,
		flg = FALSE;

	float per;

	CriPer = BATTLE_CounterCalc( attackindex, defindex );

	At_SoubiIndex = CHAR_getItemIndex( attackindex, CHAR_ARM );

	if( BATTLE_IsThrowWepon( At_SoubiIndex ) == TRUE ){
		*pPar = 0.0;
		return FALSE;
	}

	if( ITEM_CHECKINDEX( At_SoubiIndex ) == FALSE ){
		At_SoubiType = ITEM_FIST;
	}else{
		At_SoubiType = ITEM_getInt( At_SoubiIndex, ITEM_TYPE );
	}

	At_SoubiIndex = BATTLE_ItemType2ItemMap( At_SoubiType );

	Df_SoubiIndex = CHAR_getItemIndex( defindex, CHAR_ARM );

	if( BATTLE_IsThrowWepon( Df_SoubiIndex ) == TRUE ){
		*pPar = 0.0;
		return FALSE;
	}

	if( ITEM_CHECKINDEX( Df_SoubiIndex ) == FALSE ){
		Df_SoubiType = ITEM_FIST;
	}else{
		Df_SoubiType = ITEM_getInt( Df_SoubiIndex, ITEM_TYPE );
	}

	Df_SoubiIndex = BATTLE_ItemType2ItemMap( Df_SoubiType );

	At_Soubi = CounterTbl[BATTLE_C_MAX * At_SoubiIndex + Df_SoubiIndex];

	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		At_Luck = CHAR_getWorkInt( attackindex, CHAR_WORKFIXLUCK );
	}

#ifdef _SUIT_ADDENDUM
	per = CriPer * At_Soubi * 0.1 + At_Luck + CHAR_getWorkInt( attackindex, CHAR_WORKCOUNTER );
#else
	per = CriPer * At_Soubi * 0.1 + At_Luck;
#endif

	*pPar = per;

	per *= 100;
	if( per <= 0 ){
		per = 1;
		*pPar = 0;
	}

	if( RAND( 1, 10000 ) < per ){
		flg = TRUE;
	}else{
		flg = FALSE;
	}
	return flg;
}


static BOOL BATTLE_CounterCheckPet( int attackindex, int defindex, int *pPer )
{
	int	At_SoubiIndex;
	int flg = FALSE, Df_SoubiIndex, iWork;

	float per;

	At_SoubiIndex = CHAR_getItemIndex( attackindex, CHAR_ARM );

	if( BATTLE_IsThrowWepon( At_SoubiIndex ) == TRUE ){
		*pPer = 0.0;
		return FALSE;
	}


	Df_SoubiIndex = CHAR_getItemIndex( defindex, CHAR_ARM );

	if( BATTLE_IsThrowWepon( Df_SoubiIndex ) == TRUE ){
		*pPer = 0.0;
		return FALSE;
	}

	per = BATTLE_CounterCalc( attackindex, defindex );

	if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_NOGUARD ){
		iWork = CHAR_GETWORKINT_LOW( attackindex, CHAR_WORKBATTLECOM3 );
		iWork = iWork >> 8;
		if( iWork > 127 )iWork *= -1;
		per += iWork;
	}

	if( per > 100 ) per = 100;
	*pPer = per;
	per *= 100;

	if( per <= 0 ){
		per = 1;
		*pPer = per;
	}

	if( RAND( 1, 10000 ) <= per ){
		flg = TRUE;
	}else{
		flg = FALSE;
	}
	return flg;
}

static BOOL BATTLE_CounterCheck( int attackindex, int defindex, int *pPar )
{
#ifdef _SHOOTCHESTNUT	// Syu ADD 宠技：丢栗子
	if ( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_ATTSHOOT || 
		CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_ATTSHOOT
		) {
		return FALSE;
	}
#endif
	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
		return BATTLE_CounterCheckPlayer( attackindex, defindex, pPar );
	}else{
		return BATTLE_CounterCheckPet( attackindex, defindex, pPar );
	}
}



//反击判定
BOOL BATTLE_Counter( int battleindex, int attackNo, int defNo )
{

	char szBuffer[512]="";
	char szCommand[1024];
	int damage = 0, petdamage = 0, attackindex, defindex, Guardian = -2, react = 0, ultimate = 0;
	int flg = 0, iWork, par, DefSide = 0, i, per = 0;
	BOOL	iRet = TRUE;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = BATTLE_No2Index( battleindex, defNo );

#ifdef _SHOOTCHESTNUT	// Syu ADD 宠技：丢栗子
	if ( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_ATTSHOOT ) {
		return FALSE;
	}
#endif

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 )		return FALSE;
	if( CHAR_getInt( attackindex, CHAR_HP ) <= 0 )	return FALSE;

	if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_ATTACK
		|| CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_NOGUARD){
		;;
	}else{
		return FALSE;
	}

	// nono宠
	if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO )	return FALSE;

	// 反击条件
	if( BATTLE_CounterCheck( attackindex, defindex, &per ) == FALSE )	return FALSE;

	// 光、镜、守
	if( BATTLE_GetDamageReact( attackindex ) > 0 )
		iRet = FALSE;
	else if( BATTLE_GetDamageReact( defindex ) > 0 )
		iRet = FALSE;

	gWeponType = BATTLE_GetWepon( attackindex );

	gDamageDiv = 1.0;

	iWork = BATTLE_AttackSeq( attackindex, defindex, &damage, &Guardian, -1 );

	if( damage <= 0 ){
		damage = 0;
	}else{
		damage *= 0.75;
		if( damage < 1 ) damage = 1;
	}

	react = 0;
	ultimate = BATTLE_DamageSub( attackindex, defindex, &damage, &petdamage, &react );
	if( react == BATTLE_MD_REFLEC )		defindex = attackindex;

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP )	defindex = attackindex;
#endif

#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ) defindex = attackindex;
#endif

	if( damage > 0 && ( react != BATTLE_MD_ABSROB )	&& ( react != BATTLE_MD_VANISH )){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}
	par = BATTLE_AttrAdjust( attackindex, defindex, 100 );
	switch( iWork ){
	case BATTLE_RET_MISS:
/*
		snprintf( szBuffer, sizeof(szBuffer),
			"(%s)朝向(%s)反击,没击中。",
			CHAR_getUseName( attackindex ),
			CHAR_getUseName( defindex )
		);
*/
		return FALSE;

	case BATTLE_RET_DODGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)意外攻击(%d%%)!但是(%s)闪开了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	per,
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_DODGE;
		break;

	case BATTLE_RET_NORMAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//"(%s)的反击!(%d%%)(%d)损伤 (%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	per,
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		//andy_edit
		flg |= BATTLE_getReactFlg( defindex, react);
		break;

	case BATTLE_RET_CRITICAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//"(%s)的CRITICAL！(%d%%)(%d)损伤 (%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	per,
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		flg |= BCF_KAISHIN;
		iRet = FALSE;
		//andy_edit
		flg |= BATTLE_getReactFlg( defindex, react);
		break;
#ifdef _EQUIT_ARRANGE
	case BATTLE_RET_ARRANGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)攻击了(%s)挡掉了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_B_ARRANGE;
		break;
#endif
	}

	flg |= BCF_COUNTER;

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
			ultimate = 1;
		}else
		if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER
		&&  flg & BCF_KAISHIN
		){
			if( RAND( 1, 100 ) < 50 ){
				ultimate = 1;
			}
		}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) ultimate = 0;
#endif
		iRet = FALSE;
		flg |= BCF_DEATH;

		if( ultimate == 1 ){
			flg |= BCF_ULTIMATE_1;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
		if( ultimate == 2 ){
			flg |= BCF_ULTIMATE_2;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
	}

	if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
		if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
			flg |= BCF_CRUSH;
		}
	}

#ifdef _NOTRIDE_
	snprintf( szCommand, sizeof( szCommand ),
		"r%X|f%X|counter%X|",
		defNo,
		flg,
		damage );
#else
	snprintf( szCommand, sizeof( szCommand ),
		"r%X|f%X|counter%X|p%X|",
		defNo,
		flg,
		damage,
		petdamage
		);
#endif
	//print(" CounterAttack!:%s ", szCommand );

	BATTLESTR_ADD( szCommand );


	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}

	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}

	return iRet;
}


static BOOL BATTLE_CaptureCheck(
	int attackindex, 
	int defindex, 
	float *pPer
)
{
	float
		Df_MaxHp = 0,
		Df_HpPer = 0,
		At_Charm = 0,
		At_Level = 0,
		At_Dex = 0,
		At_Luck = 0,
		Df_Level = 0,
		Df_Dex = 0,
		Df_Ge = 30;
	float WorkGet;

	*pPer = 0;

	if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) != CHAR_TYPEENEMY ){
		return FALSE;
	}

	if( CHAR_getWorkInt( defindex, CHAR_WORK_PETFLG ) == 0 ){
		return FALSE;
	}

	if( CHAR_getWorkInt( attackindex, CHAR_PickAllPet) != TRUE )	{
		if( CHAR_getInt( attackindex, CHAR_LV ) + 5 < CHAR_getInt( defindex, CHAR_LV )){
			return FALSE;
		}
	}

	At_Charm = CHAR_getWorkInt( attackindex, CHAR_WORKFIXCHARM );
	At_Level = CHAR_getInt( attackindex, CHAR_LV );
	Df_Level = CHAR_getInt( defindex, CHAR_LV );
	At_Dex = CHAR_getWorkInt( attackindex, CHAR_WORKFIXDEX );
	Df_Dex = CHAR_getWorkInt( defindex, CHAR_WORKFIXDEX );
	Df_Ge = CHAR_getWorkInt( defindex, CHAR_WORKMODCAPTUREDEFAULT );
	At_Luck = CHAR_getWorkInt( attackindex, CHAR_WORKFIXLUCK );
	Df_HpPer = CHAR_getInt( defindex, CHAR_HP );
	Df_MaxHp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	if( Df_MaxHp <= 0 ) Df_MaxHp = 1;

	Df_HpPer = 10 - ( Df_HpPer * Df_HpPer ) / Df_MaxHp;
	Df_Level = ( At_Level/2 - Df_Level/2 );
	Df_Dex = At_Dex / 15 - Df_Dex / 15;

	WorkGet = ( Df_HpPer + Df_Level + Df_Dex + ( Df_Ge + At_Luck )  ) * At_Charm / 50;

	WorkGet += CHAR_getWorkInt( attackindex, CHAR_WORKMODCAPTURE );

	if( CHAR_getWorkInt( defindex, CHAR_WORKSLEEP ) > 0 ){
		WorkGet += 15;
	}

	if( WorkGet > 99 ) WorkGet = 99;

	*pPer = WorkGet;

	if( RAND( 1,100 ) < WorkGet ){
		return TRUE;
	}else{
		return FALSE;
	}

}
#ifdef _CAPTURE_FREES
#define MAXCAPTRUEFREE 15

typedef	struct _NeedItemEnemy{
		int EnemyId;
		int ItemId[MAXCAPTRUEFREE];
}NEEDITEMENEMY;
NEEDITEMENEMY	NeedEnemy[128] = {
	{ 524,	{ 2456, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
	//伊甸任务
	{ 961,	{20219, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
	{ 953,	{20223, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
	{ 962,	{20222, -1, -1, -1, -1, -1, -1, -1, -1, -1} },

	{ 777,	{20253, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
	{ 796,	{20247, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
	{ 812,	{20259, -1, -1, -1, -1, -1, -1, -1, -1, -1} },

	{ 1105,	{1690, 1691, 1692, -1, -1, -1, -1, -1, -1, -1} },
	{ 8,	{1810, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
#ifdef _WOLF_TAKE_AXE				// WON ADD 抓双头狼的限制
	{ 145,	{2236, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
	{ 146,	{2236, -1, -1, -1, -1, -1, -1, -1, -1, -1} },
#endif
};

#ifdef _NEED_ITEM_ENEMY
int need_item_eneny_init()
{
	FILE* fp;
	int i;
	int num=0;
	char line[128], buf[16];
	fp = fopen("data/needitemeneny.txt", "r");
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	while(1){
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);
		if(line[0]=='#')continue;
			
		getStringFromIndexWithDelim(line,"|", 1, buf, sizeof(buf));
		NeedEnemy[num].EnemyId=atoi(buf);
		for(i=0;i<MAXCAPTRUEFREE;i++){
			getStringFromIndexWithDelim(line,"|", i+2, buf, sizeof(buf));
			NeedEnemy[num].ItemId[i]=atoi(buf);
		}
		num++;
		if(num>=128)break;
	}
	fclose(fp);
	return TRUE;
}
#endif

#else
typedef	struct _NeedItemEnemy{
		int EnemyId;
		int ItemId;
}NEEDITEMENEMY;
NEEDITEMENEMY	NeedEnemy[] = {
	{ 524, 2456 },
	//伊甸任务
	{ 961, 20219},
	{ 953, 20223},
	{ 962, 20222},

	{ 777, 20253},
	{ 796, 20247},
	{ 812, 20259},

};
#endif

static int IsNeedCaptureItem( int charaindex )
{
	int i=0;
	for( i = 0; i < arraysizeof( NeedEnemy ); i ++ ){
		if( NeedEnemy[i].EnemyId != CHAR_getInt( charaindex, CHAR_PETID ) )continue;
#ifdef _CAPTURE_FREES
		return i;
#else
		return NeedEnemy[i].ItemId;
#endif
	}
	return -1;
}

BOOL BATTLE_CaptureItemCheck( int attackindex, int defindex )
{
#ifdef _CAPTURE_FREES
	int i;
#endif
	int j, ti, itemindex;

	ti = IsNeedCaptureItem( defindex );
	if( ti < 0 ) return TRUE;

#ifdef _CAPTURE_FREES
	for( i=0; i<MAXCAPTRUEFREE; i++ ){
		if( NeedEnemy[ ti].ItemId[i] == -1 ) break;
		for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ){
			itemindex =CHAR_getItemIndex( attackindex, j );
			if( ITEM_CHECKINDEX( itemindex ) == FALSE )continue;
			if( ITEM_getInt( itemindex, ITEM_ID ) != NeedEnemy[ ti].ItemId[i] ) continue;
				break;
		}
		//没找到
		if( j>= CHAR_MAXITEMHAVE ) return FALSE;
	}
	return TRUE;
#else
	for( j = CHAR_STARTITEMARRAY; j < CHAR_MAXITEMHAVE; j ++ ){
		itemindex =CHAR_getItemIndex( attackindex, j );
		if( ITEM_CHECKINDEX( itemindex ) == FALSE )continue;
		if( ITEM_getInt( itemindex, ITEM_ID ) == ti ){
			return TRUE;
		}
	}
	return FALSE;
#endif
	
}

#ifdef _CAPTURE_FREES
BOOL BATTLE_CaptureItemDelAll( int attackindex, int defindex )
{
	int i, j, itemindex=-1,ti;

	ti = IsNeedCaptureItem( defindex );
	if( ti < 0 ) return TRUE;
	for( i=0; i<MAXCAPTRUEFREE; i++ ){
		if( NeedEnemy[ ti].ItemId[i] == -1 ) break;
		for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ){
			itemindex =CHAR_getItemIndex( attackindex, j );
			if( ITEM_CHECKINDEX( itemindex ) == FALSE )continue;
			if( ITEM_getInt( itemindex, ITEM_ID ) == NeedEnemy[ ti].ItemId[i] ){
//				LogItem(
//					CHAR_getChar( attackindex, CHAR_NAME ),
//					CHAR_getChar( attackindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//						itemindex,
//#else
//					ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//					"PetEaten(使用条件道具捕获宠物)",
//					CHAR_getInt( attackindex,CHAR_FLOOR),
//					CHAR_getInt( attackindex,CHAR_X ),
//					CHAR_getInt( attackindex,CHAR_Y ),
//					ITEM_getChar(itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//				);
				{
					typedef void (*DETACHFUNC)(int,int);
					DETACHFUNC def;
					def = (DETACHFUNC)ITEM_getFunctionPointer( itemindex, ITEM_DETACHFUNC);
					if( def )	{
						def( attackindex, itemindex );
					}
	//				}
#ifdef _ALLBLUES_LUA_1_2 
					else{
						RunItemDetachEvent(attackindex, itemindex );
					}
#endif
				}
				CHAR_DelItem( attackindex, j);
				CHAR_complianceParameter( attackindex);
				break;//Change fix 抓到一只宠只删除一个道具(最後还是决定全删)
			}
		}
	}
	return TRUE;
}

#endif

BOOL BATTLE_Capture(
	int battleindex,
	int attackNo,
	int defNo
)
{
	char szBuffer[512]="";
	int attackindex, defindex, flg = 1, pindex;
#ifndef _CAPTURE_FREES
	int i, needItemId, itemindex;
#endif
	float per;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = BATTLE_No2Index( battleindex, defNo );

	szBuffer[0] = 0;

	if( BATTLE_CaptureItemCheck( attackindex, defindex ) == FALSE
#ifdef _ALLBLUES_LUA_1_8
		|| CaptureCheckFunction( attackindex, defindex ) == FALSE 
#endif
    ){
		flg = 0;
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)没有捕获(%s)的必要道具。",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex )
		//);
	}else
		if( BATTLE_CaptureCheck( attackindex, defindex, &per ) == FALSE ){
			//snprintf( szBuffer, sizeof(szBuffer), "(%s)无法捕获(%s)(%.2f%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	per
			//);
			flg = 0;
		}
	CHAR_setWorkInt( attackindex, CHAR_WORKMODCAPTURE, 0 );
	if( flg == 1 ){
		pindex = PET_createPetFromCharaIndex( attackindex, defindex );
		if( pindex == -1 ){
			//snprintf( szBuffer, sizeof(szBuffer), "(%s)呈现无法捕获的状态。",
			//	CHAR_getUseName( attackindex ) );
			flg = 0;
		}else{
			int ai;
			CHAR_setInt( pindex, CHAR_PETGETLV, CHAR_getInt( pindex, CHAR_LV ) );
			//snprintf( szBuffer, sizeof(szBuffer), "(%s)捕获了(%s)(%.2f%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	per
			//);
			flg = 1;

//			LogPet(
//				CHAR_getUseName( attackindex ),
//				CHAR_getUseID( attackindex ),
//				CHAR_getChar( defindex, CHAR_NAME ),
//				CHAR_getInt( defindex, CHAR_LV ),
//				"Get(捕获宠物)",
//				CHAR_getInt( attackindex, CHAR_FLOOR ),
//				CHAR_getInt( attackindex, CHAR_X ),
//				CHAR_getInt( attackindex, CHAR_Y ),
//				CHAR_getChar( defindex, CHAR_UNIQUECODE)   // shan 2001/12/14
//			);
#ifdef _ALLBLUES_LUA_1_8
		 CaptureOkFunction( attackindex, defindex );
#endif
#ifdef _CAPTURE_FREES
			BATTLE_CaptureItemDelAll( attackindex, defindex );
#else
			needItemId = IsNeedCaptureItem( defindex );
			if( needItemId >= 0 ){
				for( i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i ++ ) {
				    itemindex = CHAR_getItemIndex( attackindex, i );
				    if( ITEM_CHECKINDEX( itemindex ) == FALSE )continue;
					if( ITEM_getInt( itemindex, ITEM_ID ) == needItemId ){
						{
							char token[256];
//							LogItem(
//								CHAR_getChar( attackindex, CHAR_NAME ),
//								CHAR_getChar( attackindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//								itemindex,
//#else
//					   			ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//								"PetEaten(使用料理捕获宠物)",
//						       	CHAR_getInt( attackindex,CHAR_FLOOR),
//								CHAR_getInt( attackindex,CHAR_X ),
//					        	CHAR_getInt( attackindex,CHAR_Y ),
//								ITEM_getChar(itemindex, ITEM_UNIQUECODE),
//								ITEM_getChar( itemindex, ITEM_NAME),
//								ITEM_getInt( itemindex, ITEM_ID)
//							);
						}
						CHAR_DelItem( attackindex, i);
					}
				}
			}
#endif
			CHAR_setInt( attackindex, CHAR_GETPETCOUNT,
				CHAR_getInt( attackindex, CHAR_GETPETCOUNT ) + 1 );

			BATTLE_Exit( defindex, battleindex );
			CHAR_complianceParameter( pindex );
			CHAR_setInt( pindex, CHAR_VARIABLEAI, 0 );
			ai = CHAR_DEFAULTMAXAI - CHAR_getWorkInt( pindex, CHAR_WORKFIXAI );
			if( ai < 0 ){
				CHAR_PetAddVariableAi( pindex, ai * 100 );
				CHAR_setWorkInt( pindex, CHAR_WORKFIXAI,
					CHAR_getWorkInt( pindex, CHAR_WORKFIXAI ) + ai );
			}
		}
	}
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	{
		char szCommand[1024];
		snprintf( szCommand, sizeof( szCommand ),
			"BT|a%X|r%X|f%X|",
			attackNo,
			defNo,
			flg );
		BATTLESTR_ADD( szCommand );
	}
	return ( flg )?(TRUE):(FALSE);
}

void BATTLE_Guard(
	int battleindex,
	int attackNo
)
{
	char szCommand[128];
	//int attackindex;
	sprintf( szCommand, "bg|%x|", attackNo );
	BATTLESTR_ADD( szCommand );
	/*attackindex = BATTLE_No2Index( battleindex, attackNo );
	{
		char szBuffer[256]="";
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)保住了自身安全。",
		//	CHAR_getUseName( attackindex )
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	}*/

}

BOOL BATTLE_EscapeCheck( int attackindex, int attackNo, int *pPar )
{
	int Esc = 100;

	int myside, otherside,  battleindex;
	int mylevel, enemylevel = 0, enemycnt = 0, i, enemyindex, luck,
	escape_cnt, rare, SubNo;
	BATTLE_ENTRY	*pEntry;

	myside = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLESIDE );
	otherside = 1 - myside;

	battleindex = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX );

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE )return TRUE;

	if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P )return TRUE;

	if( attackNo >= SIDE_OFFSET ){
		SubNo = attackNo - SIDE_OFFSET;
	}else{
		SubNo = attackNo;
	}

	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY ){
		rare = CHAR_getInt( attackindex, CHAR_RARE );
		switch( rare ){
			case 0: luck = 1; break;
			case 1: luck = 3; break;
			default:luck = 5; break;
		}
	}else{
		luck = min( 5, CHAR_getWorkInt( attackindex, CHAR_WORKFIXLUCK ) );
		luck = max( 1, luck );
	}

	mylevel = CHAR_getInt( attackindex, CHAR_LV );
	pEntry = BattleArray[battleindex].Side[otherside].Entry;

	escape_cnt = BattleArray[battleindex].Side[myside].Entry[SubNo].escape+1;

	for( i = 0; i < BATTLE_ENTRY_MAX; i ++ ){
		enemyindex = pEntry[i].charaindex;
		if( CHAR_CHECKINDEX( enemyindex ) == FALSE )continue;

		if( CHAR_getWorkInt( enemyindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO )
			enemylevel -= 100;

		enemylevel += CHAR_getInt( enemyindex, CHAR_LV );

		enemycnt ++;
	}

	if( enemycnt <= 0 ){
		enemylevel = 0;
		Esc = 100;
	}else{
		enemylevel /= enemycnt;
		if( luck >= 5 ){
			Esc = 95 * escape_cnt;
		}else
		if( luck >= 4 ){
			Esc = ( 60 * escape_cnt ) - 2*( enemylevel - mylevel );
		}else
		if( luck >= 3 ){
			Esc = ( 50  * escape_cnt ) - 2*( enemylevel - mylevel );
		}else
		if( luck >= 2 ){
			Esc = ( 40  * escape_cnt ) - 2*( enemylevel - mylevel );
		}else
		if( luck >= 1 ){
			Esc = ( 30  * escape_cnt ) - 2*( enemylevel - mylevel );
		}else{
			Esc = 95 * escape_cnt;
		}
	}

	if( Esc < 1 )Esc = 1;

	*pPar = Esc;

	if( RAND( 1,100 ) < Esc ){
		return TRUE;
	}else{
		return FALSE;
	}
}


BOOL BATTLE_Escape( int battleindex, int attackNo, int flag)
{
	//char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, flg = FALSE, side, SubNo, iPar;

	sprintf( szCommand, "BE|e%X|", attackNo );
	BATTLESTR_ADD( szCommand );

	attackindex = BATTLE_No2Index( battleindex, attackNo );

	if( !CHAR_CHECKINDEX( attackindex ) ){
		return FALSE;
	}

	if( attackNo >= SIDE_OFFSET ){
		side = 1; SubNo = attackNo - SIDE_OFFSET;
	}else{
		side = 0; SubNo = attackNo;
	}

	BattleArray[battleindex].Side[side].Entry[SubNo].escape ++;

	if( BATTLE_EscapeCheck( attackindex, attackNo, &iPar ) == FALSE ){
		flg = 0;
	}else{
		flg = 1;
	}
	
	if( flg == 1 || flag == 1 ){
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)逃跑了(%d%%)",
		//	CHAR_getUseName( attackindex ), iPar
		//);
		//if( getBattleDebugMsg( ) != 0 ){
		//	BATTLE_BroadCast( battleindex, szBuffer,
		//		(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		//}

		if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P
		&& CHAR_getInt( attackindex, CHAR_HP ) > 0	){
			BATTLE_EscapeDpSend( battleindex, attackindex );
			BATTLE_GetDuelPoint( battleindex, side, SubNo );
		}
#ifdef _ALLBLUES_LUA_1_4
		BattleEscape(battleindex, attackindex);
#endif
		CHAR_setWorkInt( attackindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_FINAL );
		CHAR_DischargePartyNoMsg( attackindex );
		BATTLE_Exit( attackindex, battleindex );
	}else{
		//if( getBattleDebugMsg( ) != 0 ){
			//snprintf( szBuffer, sizeof(szBuffer), "(%s)差点跑了(%d%%)",
			//	CHAR_getUseName( attackindex ), iPar
			//);
			//BATTLE_BroadCast( battleindex, szBuffer,
			//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
		//}
	}
	snprintf( szCommand, sizeof( szCommand ), "f%X|", flg );
	BATTLESTR_ADD( szCommand );

	return ( flg )?(TRUE):(FALSE);
}


int BATTLE_SurpriseCheck( int battleindex )
{
	int iRet = 0, Rnd, charaindex, luck, a = 0, b = 7;

	if( BattleArray[battleindex].Side[0].type == BATTLE_S_TYPE_PLAYER
	&&  BattleArray[battleindex].Side[1].type == BATTLE_S_TYPE_PLAYER
	){
		return 0;
	}

	charaindex = BattleArray[battleindex].Side[0].Entry[0].charaindex;

	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return 0;

	if( BattleArray[battleindex].WinFunc != NULL )return 0;

	luck = CHAR_getWorkInt( charaindex, CHAR_WORKFIXLUCK );

	switch( luck ){
	case 5: a = 20; b = 0; break;
	case 4: a = 15; b = 2; break;
	case 3: a = 10; b = 3; break;
	case 2: a =  5; b = 5; break;
	default: a =  0; b = 7; break;
	}

	Rnd = RAND( 1, 100 );

	if( Rnd <= a ){
		iRet = 1;
	}else
	if( Rnd < a + b ){
		iRet = 2;
	}

	return iRet;
}


int BATTLE_NoAction( int battleindex, int attackNo )
{
	//char szBuffer[512]="";
	char szCommand[512];
	int attackindex ;

	attackindex = BATTLE_No2Index( battleindex, attackNo );

	sprintf( szCommand, "bn|%x|", attackNo );
	BATTLESTR_ADD( szCommand );

	//snprintf( szBuffer, sizeof(szBuffer), "(%s)什麽也不能做。",
	//	CHAR_getUseName( attackindex )
	//);
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	return 0;
}


int BATTLE_PetIn( int battleindex, int attackNo )
{
	//char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, petindex ;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	petindex = CHAR_getInt( attackindex, CHAR_DEFAULTPET );
	petindex = CHAR_getCharPet( attackindex, petindex );
#ifdef _FIXWOLF	 // Syu ADD 修正狼人变身Bug
	if ( CHAR_getInt ( petindex , CHAR_BASEIMAGENUMBER) == 101428 )
	{
		CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( petindex , CHAR_BASEBASEIMAGENUMBER));
		CHAR_setWorkInt( petindex, CHAR_WORKATTACKPOWER, CHAR_getWorkInt( petindex , CHAR_WORKFIXSTR));
		CHAR_setWorkInt( petindex, CHAR_WORKQUICK, CHAR_getWorkInt( petindex , CHAR_WORKFIXDEX));
    }
#endif
#ifdef _PETSKILL_BECOMEFOX // 宠物中媚惑术收回後再放出来时要恢复正常状态
    if ( CHAR_getInt ( petindex , CHAR_WORKFOXROUND) != -1 
		|| CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER) == 101749 ){ //若是变身为狸
		CHAR_setInt( petindex, CHAR_BASEIMAGENUMBER, CHAR_getInt( petindex , CHAR_BASEBASEIMAGENUMBER));
		CHAR_setWorkInt( petindex, CHAR_WORKATTACKPOWER, CHAR_getWorkInt( petindex , CHAR_WORKFIXSTR));
		CHAR_setWorkInt( petindex, CHAR_WORKQUICK, CHAR_getWorkInt( petindex , CHAR_WORKFIXDEX));
	  CHAR_setWorkInt( petindex, CHAR_WORKFOXROUND, -1 );
	}
#endif
	if( CHAR_getWorkInt( petindex, CHAR_WORKBATTLEFLG )
	& CHAR_BATTLEFLG_NORETURN
	){
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)无法回复成(%s)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( petindex )
		//);
		return 0;
	}

	BATTLE_PetDefaultExit( attackindex, battleindex );
	CHAR_setInt( attackindex, CHAR_DEFAULTPET, -1 );

	//snprintf( szBuffer, sizeof(szBuffer), "(%s)回复成(%s)",
	//	CHAR_getUseName( attackindex ),
	//	CHAR_getUseName( petindex )
	//);
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	sprintf( szCommand, "BS|s%X|f0|", attackNo );
	BATTLESTR_ADD( szCommand );

	return 0;

}

int BATTLE_PetOut( int battleindex, int attackNo )
{
	//char szBuffer[512]="";
	char szCommand[1024], szEscapeName[128];
	int attackindex, petindex, petNo, flg = 0,iRet ;

	szCommand[0] = 0;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	petNo = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM2 );
#ifdef _PET_BUG
    petindex = CHAR_getCharPet( attackindex, petNo);
    if(CHAR_getInt(petindex,CHAR_VITAL)>=getPetPoint(0)*20 || CHAR_getInt(petindex,CHAR_STR)>=getPetPoint(1)*80 || CHAR_getInt(petindex,CHAR_TOUGH)>=getPetPoint(2)*80 || CHAR_getInt(petindex,CHAR_DEX)>=getPetPoint(3)*100)
    	CHAR_setInt( attackindex, CHAR_DEFAULTPET, -1 );
    else
#endif   	
	CHAR_setInt( attackindex, CHAR_DEFAULTPET, petNo );

	iRet = BATTLE_PetDefaultEntry( attackindex, battleindex,
		CHAR_getWorkInt( attackindex, CHAR_WORKBATTLESIDE ) );

	if( iRet < 0 ){
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)呼唤宠物,可是没有出现其踪迹。",
		//	CHAR_getUseName( attackindex )
		//);
	}else
	if( CHAR_getInt( attackindex, CHAR_DEFAULTPET ) < 0 ){
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)呼唤宠物,可是没有出现其踪迹。",
		//	CHAR_getUseName( attackindex )
		//);
	}else{
		flg = 1;
		petNo = CHAR_getInt( attackindex, CHAR_DEFAULTPET );
		petindex = CHAR_getCharPet( attackindex, petNo );

		CHAR_setWorkInt( petindex, CHAR_WORKBATTLEMODE,
			BATTLE_CHARMODE_C_OK );

		//snprintf( szBuffer, sizeof(szBuffer), "(%s)呼唤(%s)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( petindex )
		//);

		makeEscapeString( CHAR_getUseName( petindex ),
			szEscapeName, 
			sizeof( szEscapeName ) );

		sprintf( szCommand, "BS|s%X|f1|g%X|l%X|h%X|%s|m%X|",
			attackNo,
			CHAR_getInt( petindex, CHAR_BASEIMAGENUMBER),
			CHAR_getInt( petindex, CHAR_LV),
			CHAR_getInt( petindex, CHAR_HP),
			szEscapeName,
			CHAR_getWorkInt( petindex, CHAR_WORKMAXHP)
		);
		BATTLESTR_ADD( szCommand );
	}

	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	return ( flg )?(TRUE):(FALSE);

}


int BATTLE_Magic( int battleindex, int attackNo )
{
	int attackindex, defindex, defNo;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defNo = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM2 );
	defindex = BATTLE_No2Index( battleindex, defNo );

	return 0;
}


//判别攻击是否命中
int BATTLE_S_GBreak( int battleindex, int attackNo, int defNo )
{
	char szBuffer[512]="";
	char szCommand[1024];
	int damage = 0, petdamage = 0, attackindex, defindex, ultimate = 0;
	int flg = 0, iWork, par, i, DefSide = 0,
		react = 0,
		Guardian = -1;
	BOOL iRet = FALSE;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = BATTLE_No2Index( battleindex, defNo );

	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}

	iWork = BATTLE_AttackSeq( attackindex, defindex, &damage,
		&Guardian, BATTLE_COM_S_GBREAK );

	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
	&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0
	){
		react = 0;
		ultimate = BATTLE_DamageSub( attackindex, defindex, &damage, &petdamage, &react );
		if( react == BATTLE_MD_REFLEC )defindex = attackindex;
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
		if( react == BATTLE_MD_TRAP )	defindex = attackindex;
#endif
#ifdef _PETSKILL_ACUPUNCTURE
        if( react == BATTLE_MD_ACUPUNCTURE ) defindex = attackindex;
#endif

	}else{
		//若敌非防御则damage为0
		damage = 0;
		iWork = BATTLE_RET_MISS;
	}

	if( damage > 0 
	&& ( react != BATTLE_MD_ABSROB )
	&& ( react != BATTLE_MD_VANISH )
	){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}

	par = BATTLE_AttrAdjust( attackindex, defindex, 100 );
	switch( iWork ){

	case BATTLE_RET_ALLGUARD:
	case BATTLE_RET_MISS:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)朝向(%s)发动绝技,没击中。",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex )
		//);
		iRet = TRUE;
		break;

	case BATTLE_RET_DODGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)展开防御攻击(%s)躲开了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_DODGE;
		iRet = TRUE;
		break;

	case BATTLE_RET_NORMAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)受到(%s)的绝技攻击(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		//andy_edit
		flg |= BATTLE_getReactFlg( defindex, react);
		flg |= BCF_NORMAL;
		iRet = TRUE;
		break;

	case BATTLE_RET_CRITICAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//"(%s)受到(%s)的绝技CRITICAL(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		flg |= BCF_KAISHIN;
		iRet = FALSE;
		//andy_edit
		flg |= BATTLE_getReactFlg( defindex, react);
		break;
#ifdef _EQUIT_ARRANGE
	case BATTLE_RET_ARRANGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)攻击了(%s)挡掉了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_B_ARRANGE;
		break;
#endif
	}

	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
	&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0 
	){
		strncat( szBuffer,  "(GUARD)", sizeof( szBuffer ) );
		iRet = FALSE;
	}

	flg |= BCF_GBREAK;

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
			ultimate = 1;
		}else
		if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY
		&&  flg & BCF_KAISHIN
		){
			if( RAND( 1, 100 ) < 50 ){
				ultimate = 1;
			}
		}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) ultimate = 0;
#endif
		iRet = FALSE;
		flg |= BCF_DEATH;

		if( ultimate == 1 ){
			flg |= BCF_ULTIMATE_1;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
		if( ultimate == 2 ){
			flg |= BCF_ULTIMATE_2;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
	}

	if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
		if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
			flg |= BCF_CRUSH;
		}
	}

#ifdef _NOTRIDE_
	snprintf( szCommand, sizeof( szCommand ),
		"BH|a%X|r%X|f%X|d%X|FF|",
		attackNo,
		defNo,
		flg,
		damage );
#else
	snprintf( szCommand, sizeof( szCommand ),
		"BH|a%X|r%X|f%X|d%X|p%X|FF|",
		attackNo,
		defNo,
		flg,
		damage,
		petdamage
		);
#endif

	BATTLESTR_ADD( szCommand );

	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}

	return iRet;
}


#ifdef _SKILL_SACRIFICE//救援 vincent add 2002/05/30
int BATTLE_S_Sacrifice( int battleindex, int attackNo, int defNo )
{
	int attackindex, defindex = 0;
	int DefSide ,Damage= 0;
	BOOL iRet = FALSE;
	char szCommand[256];
	int ToList[SIDE_OFFSET*2+1];

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = BATTLE_No2Index( battleindex, defNo );
	if( defNo >= SIDE_OFFSET ){
		DefSide = 1;
	}

	//修正HP
	CHAR_setInt(attackindex,CHAR_HP,(CHAR_getInt(attackindex,CHAR_HP)*0.5));
	CHAR_setInt(defindex,CHAR_HP,min(CHAR_getInt(attackindex,CHAR_HP)+CHAR_getInt(defindex,CHAR_HP),CHAR_getWorkInt(defindex,CHAR_WORKMAXHP)));
	Damage=CHAR_getInt(attackindex,CHAR_HP);

	//魔法动画
	BATTLE_MultiList( battleindex, defNo, ToList );
    //秀hp改变数值
	BATTLE_MagicEffect(battleindex, attackNo, ToList, MAGIC_EFFECT_USER, SPR_heal2 );
	snprintf( szCommand, sizeof(szCommand),"BD|r%X|%X|%X|d%X|p%X|", attackNo, BD_KIND_HP, 0, Damage, 0 );
    BATTLESTR_ADD( szCommand );
	snprintf( szCommand, sizeof(szCommand),"BD|r%X|%X|%X|d%X|p%X|", ToList[0], BD_KIND_HP, 1, Damage, 0 );
	BATTLESTR_ADD( szCommand );
	return iRet;
}
#endif


#ifdef _SKILL_REFRESH   
int BATTLE_S_Refresh( int battleindex, int attackNo, int defNo, int marray )
{
    BOOL iRet = FALSE;
	char *magicarg;
	int status = -1, i,charaindex;
	int  ReceveEffect= 0;
	char *pszP;
    magicarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	pszP = magicarg;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 0; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return iRet;
    charaindex = BATTLE_No2Index( battleindex, attackNo );
	if( CHAR_getWorkInt( charaindex, StatusTbl[status] ) > 0  ){
		ReceveEffect = SPR_tyusya;
             iRet = TRUE;
	}else{
		ReceveEffect = SPR_hoshi;
	}

	BATTLE_MultiStatusRecovery( battleindex, attackNo, defNo,
		status, MAGIC_EFFECT_USER, ReceveEffect );
	return iRet;
}
#endif

#ifdef _SKILL_ROAR //vincent宠技:大吼 
int BATTLE_S_Roar( int battleindex, int attackNo, int defNo, int marray )
{
	int  masteridx=-1, index2, charaindex;
    BOOL iRet = FALSE;
	char buf1[256];
	char szWork[128];
	int petid=-1, i=1;
	BOOL FINDPET=FALSE;

	char *magicarg;
	int flg=0;

    charaindex = BATTLE_No2Index( battleindex, attackNo );

	index2 = BATTLE_No2Index( battleindex, defNo);//被击中目标之index

	if( !CHAR_CHECKINDEX( index2) ) return iRet;
	if( BATTLE_Index2No( battleindex, charaindex ) == defNo ){//用在自身
		return iRet;
	}

	if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
		petid = -1;//作用在人物
	}else if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEPET ) {
		masteridx = BATTLE_No2Index(battleindex,defNo-5);//作用在宠物
		if( !CHAR_CHECKINDEX( masteridx) ) return iRet;
		petid = CHAR_getInt(index2,CHAR_PETID);//宠物id
	}else if( CHAR_getInt( index2, CHAR_WHICHTYPE) == CHAR_TYPEENEMY ) {
		petid = CHAR_getInt(index2,CHAR_PETID);
	}

	magicarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	while( getStringFromIndexWithDelim( magicarg,"|", i, buf1, sizeof( buf1)) != FALSE )	{
		if( petid == atoi( buf1) ){//年兽判别
			FINDPET = TRUE;
			break;
		}
		i++;
	}

//送讯息至玩家
	if( FINDPET == TRUE )	{//若为年兽
		char buf4[255];
		sprintf( buf4, "%s被吼声吓跑了！", CHAR_getChar( index2, CHAR_NAME));
		BATTLE_Exit(index2,battleindex);//离开战斗
		if( CHAR_CHECKINDEX( masteridx) ){
			CHAR_setInt(masteridx,CHAR_DEFAULTPET,-1);//无参战宠
			CHAR_talkToCli( masteridx,-1, buf4, CHAR_COLORYELLOW);
		}

		//前攻动画
	    flg |= BCF_ROAR;
	    sprintf( szWork, "BH|a%X|", attackNo );
	    BATTLESTR_ADD( szWork );
		snprintf( szWork, sizeof( szWork ), "r%X|f%X|d%X|p%X|", defNo, flg, 0, 0 );

		BATTLESTR_ADD(szWork);
		BATTLESTR_ADD( "FF|" );
		sprintf(szWork,"BQ|e%X|",defNo);//逃跑动画
		BATTLESTR_ADD(szWork);
		CHAR_talkToCli( CHAR_getWorkInt(charaindex, CHAR_WORKPLAYERINDEX),-1, buf4, CHAR_COLORYELLOW);
	}

	return iRet;
}
#endif

#ifdef _SKILL_GUARDBREAK2//破除防御2 vincent add 2002/05/20
int BATTLE_S_GBreak2( int battleindex, int attackNo, int defNo )
{
	char szBuffer[512]="";
	char szCommand[1024];
	int damage = 0, petdamage = 0, attackindex, defindex, ultimate = 0;
	int flg = 0, iWork, par, i, DefSide = 0,
		react = 0,
		Guardian = -1;
	BOOL iRet = FALSE;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = BATTLE_No2Index( battleindex, defNo );

	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}

	iWork = BATTLE_AttackSeq( attackindex, defindex, &damage,
		&Guardian, BATTLE_COM_S_GBREAK2 );
		react = 0;
		ultimate = BATTLE_DamageSub( attackindex, defindex, &damage, &petdamage, &react );
		if( react == BATTLE_MD_REFLEC )defindex = attackindex;

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
		if( react == BATTLE_MD_TRAP )	defindex = attackindex;
#endif
#ifdef _PETSKILL_ACUPUNCTURE
        if( react == BATTLE_MD_ACUPUNCTURE ) defindex = attackindex;
#endif


	if( damage > 0 
	&& ( react != BATTLE_MD_ABSROB )
	&& ( react != BATTLE_MD_VANISH )
	){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}

	par = BATTLE_AttrAdjust( attackindex, defindex, 100 );
	switch( iWork ){

	case BATTLE_RET_ALLGUARD:
	case BATTLE_RET_MISS:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)朝向(%s)发动绝技,没击中。",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex )
		//);
		iRet = TRUE;
		break;

	case BATTLE_RET_DODGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)展开防御攻击(%s)躲开了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_DODGE;
		iRet = TRUE;
		break;

	case BATTLE_RET_NORMAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)受到(%s)的绝技攻击(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		//andy_edit
		flg |= BATTLE_getReactFlg( defindex, react);
		flg |= BCF_NORMAL;
		iRet = TRUE;
		break;

	case BATTLE_RET_CRITICAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//"(%s)受到(%s)的绝技CRITICAL(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		flg |= BCF_KAISHIN;
		iRet = FALSE;
		//andy_edit
		flg |= BATTLE_getReactFlg( defindex, react);
		break;
#ifdef _EQUIT_ARRANGE
	case BATTLE_RET_ARRANGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)攻击了(%s)挡掉了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_B_ARRANGE;
		break;
#endif
	}

	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
	&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0
	){
		strncat( szBuffer,  "(GUARD)", sizeof( szBuffer ) );
		flg |= BCF_GUARD;
        flg |= BCF_GBREAK;
		iRet = FALSE;
	}

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
			ultimate = 1;
		}else if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY && flg & BCF_KAISHIN ){
			if( RAND( 1, 100 ) < 50 ){
				ultimate = 1;
			}
		}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) ultimate = 0;
#endif
		iRet = FALSE;
		flg |= BCF_DEATH;

		if( ultimate == 1 ){
			flg |= BCF_ULTIMATE_1;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
		if( ultimate == 2 ){
			flg |= BCF_ULTIMATE_2;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
	}

	if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
		if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
			flg |= BCF_CRUSH;
		}
	}

#ifdef _NOTRIDE_
	snprintf( szCommand, sizeof( szCommand ),
		"BH|a%X|r%X|f%X|d%X|FF|",
		attackNo,
		defNo,
		flg,
		damage );
#else
	snprintf( szCommand, sizeof( szCommand ),
		"BH|a%X|r%X|f%X|d%X|p%X|FF|",
		attackNo,
		defNo,
		flg,
		damage,
		petdamage
		);
#endif

	BATTLESTR_ADD( szCommand );

	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}

	return iRet;

}
#endif


int BATTLE_Charge( int battleindex, int attackNo )
{
	//char szBuffer[512]="";
	int attackindex, iWork, N, pow;

	attackindex = BATTLE_No2Index( battleindex, attackNo );

	iWork = CHAR_GETWORKINT_LOW( attackindex, CHAR_WORKBATTLECOM3 );
	
	if( iWork <= 0 ){

		N = CHAR_GETWORKINT_HIGH( attackindex, CHAR_WORKBATTLECOM3 );
		pow = CHAR_getWorkInt( attackindex, CHAR_WORKFIXSTR );
		pow +=  pow * N * 0.01;
		CHAR_setWorkInt( attackindex, CHAR_WORKATTACKPOWER,
			pow + CHAR_getWorkInt( attackindex, CHAR_WORKMODATTACK ) );

		CHAR_setWorkInt( attackindex, CHAR_WORKBATTLECOM1, BATTLE_COM_S_CHARGE_OK );
		return TRUE;
	}else{

		CHAR_SETWORKINT_LOW( attackindex, CHAR_WORKBATTLECOM3, iWork - 1);
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)保持体力中。",
		//	CHAR_getUseName( attackindex )
		//);
		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		BATTLE_NoAction( battleindex, attackNo );

		return FALSE;
	}
}

//状态攻击命中判定
int BATTLE_StatusAttackCheck(
	int attackindex,
	int defindex,
	int status,
	int PerOffset,
	int Range,
	float Bai,
	int *pPer
)
{

	int Df_Reg = 0, level = 0, per = 0, i ;
	int battleindex;
	if( status >= BATTLE_ST_END || status <= 0 )return FALSE;
	for( i = 1; i < BATTLE_ST_END; i++ ){//若已有异常状态则return
    	if( CHAR_getWorkInt( defindex, StatusTbl[i] ) > 0 )return FALSE;
	}

	if( status == BATTLE_ST_PARALYSIS){
		per = 20;
		per -= CHAR_getWorkInt( defindex, RegTbl[status] );
		*pPer = per;
	}else{
		float templP = 0.0;
		float fVitalP = 0.0;
		fVitalP = CHAR_getInt( defindex, CHAR_VITAL )
				+ CHAR_getInt( defindex, CHAR_STR )
				+ CHAR_getInt( defindex, CHAR_TOUGH )
				+ CHAR_getInt( defindex, CHAR_DEX );

		templP = (float)CHAR_getInt( defindex, CHAR_VITAL ) / (fVitalP);

#ifdef _PET_SKILL_SARS //andy_add 2003/06/20
		if( status == BATTLE_ST_SARS ){
			templP = (1 - templP)*0.9;
		}
#endif

		fVitalP = templP / 0.25;

		if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
			fVitalP *= 10.0;
		}else{
			if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				fVitalP *= 10.0;
			}else{
				if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY ){
					fVitalP *= 10.0;
				}else{
					fVitalP *= 10.0;
				}
			}
		}

		if( status >= arraysizeof( RegTbl) || status < 0 )	{
			Df_Reg = 0;
		}else	{
			Df_Reg = CHAR_getWorkInt( defindex, RegTbl[status] );
		}

		battleindex = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEINDEX );
		if( BattleArray[battleindex].type != BATTLE_TYPE_P_vs_P ){
			level = CHAR_getInt( attackindex, CHAR_LV ) 
				- CHAR_getInt( defindex, CHAR_LV );
			level *= Bai;
		}else{
			level = 0;
		}
		if( level > Range )level = Range;
		if( level < -Range )level = -Range;

#ifdef _SUIT_ADDENDUM
        per = PerOffset + level + CHAR_getWorkInt( attackindex, CHAR_WORKFIXLUCK )
			- Df_Reg - fVitalP - CHAR_getWorkInt( defindex, CHAR_WORKRESIST );
#else
		per = PerOffset + level + CHAR_getWorkInt( attackindex, CHAR_WORKFIXLUCK )
			- Df_Reg - fVitalP;
#endif //_SUIT_ADDENDUM
#ifdef _EQUIT_RESIST		
		if( status == CHAR_WORKWEAKEN )
			per -= CHAR_getWorkInt( defindex, CHAR_WORKEQUITWEAKEN );
		else if( status == CHAR_WORKBARRIER )
			per -= CHAR_getWorkInt( defindex, CHAR_WORKEQUITBARRIER );
		else if( status == CHAR_WORKNOCAST )
			per -= CHAR_getWorkInt( defindex, CHAR_WORKEQUITNOCAST );
#endif //_EQUIT_RESIST
#ifdef _SUIT_ADDPART3
       if( status == CHAR_WORKWEAKEN )
			per -= CHAR_getWorkInt( defindex, CHAR_WORKRENOCAST );
#endif
		if( per > 80 )per = 80;
		*pPer = per;
	}

	if( RAND( 1, 100 ) < per ){
		return TRUE;
	}else{
		 return FALSE;
	}
}

//职业技能状态攻击命中判定
int PROFESSION_BATTLE_StatusAttackCheck( int charaindex, int toindex, int status, int Success )
{
	int rand_num = RAND( 1, 100 );
	int i;

	if( status >= BATTLE_ST_END || status <= 0 )	return 0;
	if( CHAR_getInt( toindex , CHAR_HP) <= 0 )	return 0; 
	if( CHAR_getFlg( toindex, CHAR_ISDIE ) == TRUE )	return 0; 	

#ifdef _PROFESSION_ADDSKILL
	if( status == BATTLE_ST_RESIST_F_I_T ){//自然威能
	    if( CHAR_getWorkInt( toindex, StatusTbl[BATTLE_ST_RESIST_F] ) > 0 ) return 0;
        if( CHAR_getWorkInt( toindex, StatusTbl[BATTLE_ST_RESIST_I] ) > 0 ) return 0;
        if( CHAR_getWorkInt( toindex, StatusTbl[BATTLE_ST_RESIST_T] ) > 0 ) return 0;
	    return 1;
	}
	if( status == CHAR_WORKICECRACK
		|| status == CHAR_WORKICECRACK2
		|| status == CHAR_WORKICECRACK3
		|| status == CHAR_WORKICECRACK4
		|| status == CHAR_WORKICECRACK5
		|| status == CHAR_WORKICECRACK6
		|| status == CHAR_WORKICECRACK7
		|| status == CHAR_WORKICECRACK8
		|| status == CHAR_WORKICECRACK9
		|| status == CHAR_WORKICECRACK10){
		return 1;
	}
#endif

	for( i = 1; i < BATTLE_ST_END; i++ ){//若已有异常状态则return
    	if( CHAR_getWorkInt( toindex, StatusTbl[i] ) > 0 )	return 0;
	}

	if( rand_num < Success ){
		return 1;
	}else{
		return 0;
	}
}



int BATTLE_Combo( int battleindex, int *pAttackList, int defNo )
{
	char szBuffer[512]="";
	char szCommand[1024];
	int damage = 0, petdamage = 0,
		attackindex,
		toindex,
		defindex,
		ultimate = 0,
		AllDamage = 0,
		AllPetDamage = 0,
		react = 0,
		attackNo;
	int flg = 0, iWork, par, i, DefSide = 0,
		Guardian = -2;
	BOOL iRet = FALSE;

	defindex = toindex = BATTLE_No2Index( battleindex, defNo );

	//BATTLE_BroadCast( battleindex, "发动必杀技！！",
	//	(pAttackList[0] >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	for( i = 0; pAttackList[i] != -1 && i < BATTLE_ENTRY_MAX; i ++ ){
		attackNo = pAttackList[i];

		defindex = toindex;

		flg = 0;

		attackindex = BATTLE_No2Index( battleindex, attackNo );

		if( CHAR_getInt( toindex, CHAR_HP ) <= 0 ){			
			return FALSE;
		}

		if( CHAR_getInt( attackindex, CHAR_HP ) <= 0 ){
			return FALSE;
		}

		Guardian = -2;
		iWork = BATTLE_AttackSeq( attackindex, toindex, &damage,
			&Guardian, BATTLE_COM_COMBO );
		Guardian = -1;
		if( damage <= 0 )damage = 1;

		par = BATTLE_AttrAdjust( attackindex, defindex, 100 );

		react = BATTLE_GetDamageReact( defindex );
		if( ( react == BATTLE_MD_REFLEC
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
			 || react == BATTLE_MD_TRAP
#endif
#ifdef _PETSKILL_ACUPUNCTURE
             || react == BATTLE_MD_ACUPUNCTURE
#endif
			 ) && BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			
			int refrect = 0;
			BATTLE_DamageSub( attackindex,
					defindex, &damage, &petdamage, &refrect );
		}else
		if( react == BATTLE_MD_ABSROB
		|| react == BATTLE_MD_VANISH
		){
			int refrect = 0;
			BATTLE_DamageSub( attackindex,
					defindex, &damage, &petdamage, &refrect );
		}else{

			BATTLE_DamageSubCale( attackindex,
					defindex, &damage, &petdamage );
			AllDamage += damage;
			AllPetDamage += petdamage;
		}

		if( pAttackList[i+1] == -1 ){
			int refrect = -1;
			ultimate = BATTLE_DamageSub2( attackindex,
					defindex, &AllDamage, &AllPetDamage, &refrect );
		}		
		if( react == BATTLE_MD_REFLEC ){
			defindex = attackindex;
		}
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
		if( react == BATTLE_MD_TRAP )	defindex = attackindex;
#endif
#ifdef _PETSKILL_ACUPUNCTURE
        if( react == BATTLE_MD_ACUPUNCTURE ) defindex = attackindex;
#endif
		if( damage > 0 && ( react != BATTLE_MD_ABSROB )&& ( react != BATTLE_MD_VANISH )){
			BATTLE_DamageWakeUp( battleindex, defindex );
		}

		szBuffer[0] = 0;
		switch( iWork ){

		case BATTLE_RET_MISS:
		case BATTLE_RET_ALLGUARD:
		case BATTLE_RET_NORMAL:
			//snprintf( szBuffer, sizeof(szBuffer),
			//	"(%s)受到(%s)的攻击(%d)遭受损伤(%d:%d:%d%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	damage,
			//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
			//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
			//	par
			//);
			flg |= BCF_NORMAL;
			iRet = TRUE;
			//andy_edit
			flg |= BATTLE_getReactFlg( defindex, react);
			break;

		case BATTLE_RET_CRITICAL:
			//snprintf( szBuffer, sizeof(szBuffer),
			//"(%s)向(%s)CRITICAL(%d%%)(%d)不可以(%d:%d:%d%%)",
			//	CHAR_getUseName( attackindex ),
			//	CHAR_getUseName( defindex ),
			//	(int)(gCriper*0.01),
			//	damage,
			//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
			//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
			//	par
			//);
			flg |= BCF_KAISHIN;	
			iRet = FALSE;
			//andy_edit
			flg |= BATTLE_getReactFlg( defindex, react);
			break;

		}

		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
		&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0
		){
			strncat( szBuffer,  "(GUARD)", sizeof( szBuffer ) );
			flg |= BCF_GUARD;
			iRet = FALSE;
		}

		if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){

			if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
				ultimate = 1;
			}else

			if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY
			&&  flg & BCF_KAISHIN
			){
				if( RAND( 1, 100 ) < 50 ){
					ultimate = 1;
				}
			}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) ultimate = 0;
#endif
			iRet = FALSE;
			flg |= BCF_DEATH;

			if( ultimate == 1 ){
				flg |= BCF_ULTIMATE_1;
				strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
			}
			if( ultimate == 2 ){
				flg |= BCF_ULTIMATE_2;
				strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
			}
		}
		
		if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
			if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
				flg |= BCF_CRUSH;
			}
		}
#ifdef _NOTRIDE_
		if( 0 <= Guardian && Guardian <= 19 ){
			iRet = FALSE;
			flg |= BCF_GUARDIAN;
			snprintf( szCommand, sizeof( szCommand ),
				"a%X|f%X|d%X|g%X|",
				attackNo,
				flg,
				damage,
				Guardian );
		}else{
			snprintf( szCommand, sizeof( szCommand ),
				"a%X|f%X|d%X|",
				attackNo,
				flg,
				damage );
		}
#else
		if( 0 <= Guardian && Guardian <= 19 ){
			iRet = FALSE;
			flg |= BCF_GUARDIAN;
			snprintf( szCommand, sizeof( szCommand ),
				"a%X|f%X|d%X|p%X|g%X|",
				attackNo,
				flg,
				damage,
				petdamage,
				Guardian );
		}else{
			snprintf( szCommand, sizeof( szCommand ),
				"a%X|f%X|d%X|p%X|",
				attackNo,
				flg,
				damage,
				petdamage );
		}

#endif
		BATTLESTR_ADD( szCommand );

		//BATTLE_BroadCast( battleindex, szBuffer,
		//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

		defNo = BATTLE_Index2No( battleindex, defindex );

		if( defNo >= SIDE_OFFSET ){
			iWork = defNo - SIDE_OFFSET;
			DefSide = 1;
		}else{
			iWork = defNo;
		}

		iWork = defNo-DefSide*SIDE_OFFSET;
		if( ultimate > 0 ){
			BattleArray[battleindex].Side[DefSide].Entry[iWork].flg |= BENT_FLG_ULTIMATE;
		}

	}

	return iRet;
}



int BATTLE_EarthRoundHide( int battleindex, int attackNo )
{
	//char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, flg = 0 ;

	szCommand[0] = 0;

	attackindex = BATTLE_No2Index( battleindex, attackNo );

	sprintf( szCommand, "BF|%X|", attackNo );
	BATTLESTR_ADD( szCommand );

	//snprintf( szBuffer, sizeof(szBuffer),
	//	"(%s)绕到背後去。",
	//	CHAR_getUseName( attackindex )
	//);

	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	CHAR_setFlg( attackindex, CHAR_ISATTACKED, 0 );

	CHAR_setWorkInt( attackindex, CHAR_WORKBATTLECOM1,
		BATTLE_COM_S_EARTHROUND0 );

	return ( flg )?(TRUE):(FALSE);

}



BOOL BATTLE_LostEscape( int battleindex, int attackNo )
{
	char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, flg = FALSE, side, SubNo,
	oyaindex, pno;

	attackindex = BATTLE_No2Index( battleindex, attackNo );

	if( attackNo >= SIDE_OFFSET ){
		side = 1; SubNo = attackNo - SIDE_OFFSET;
	}else{
		side = 0; SubNo = attackNo;
	}

	oyaindex = CHAR_getWorkInt( attackindex, CHAR_WORKPLAYERINDEX );

	pno = CHAR_getInt( oyaindex, CHAR_DEFAULTPET );
	if( pno < 0 )return FALSE;

//	print(" nono宠改休息 ");

	BATTLE_Exit( attackindex, battleindex );

	CHAR_setInt( oyaindex, CHAR_DEFAULTPET, -1 );

	CHAR_AddCharm( oyaindex, CH_FIX_PETESCAPE );

//			LogPet(
//				CHAR_getUseName( oyaindex ),
//				CHAR_getUseID( oyaindex ),
//				CHAR_getChar( attackindex, CHAR_NAME ),
//				CHAR_getInt( attackindex, CHAR_LV ),
//				
//				"EscapeLost(战斗逃走并回收)",
//				
//				CHAR_getInt( oyaindex, CHAR_FLOOR ),
//				CHAR_getInt( oyaindex, CHAR_X ),
//				CHAR_getInt( oyaindex, CHAR_Y ),
//				CHAR_getChar( attackindex, CHAR_UNIQUECODE)   // shan 2001/12/14
//			);

	//snprintf( szBuffer, sizeof(szBuffer), "(%s)走掉了。",
	//	CHAR_getUseName( attackindex )
	//);
	//if( getBattleDebugMsg( ) != 0 ){
	//	BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	//}

	sprintf( szCommand, "BQ|e%X|", attackNo );
	BATTLESTR_ADD( szCommand );

	snprintf( szBuffer, sizeof( szBuffer ), "K%d", pno );

	CHAR_sendStatusString( oyaindex, szBuffer );
	


	return ( flg )?(TRUE):(FALSE);


}

BOOL BATTLE_Abduct(
	int battleindex,
	int attackNo,
	int defNo,
	int array
)
{
	//char szBuffer[512]="";
	char szCommand[1024];
#ifdef _BATTLE_ABDUCTII
	char *pszP=NULL;
#endif
	int attackindex, defindex, per, flg, attackoyaindex, defoyaindex = -1;
	int attacklevel, deflevel;
	int Attacktype, Deftype;
	BOOL iRet = FALSE;

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	defindex = BATTLE_No2Index( battleindex, defNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )	return iRet;
	if( CHAR_CHECKINDEX( defindex ) == FALSE )	return iRet;
#ifdef _BATTLE_ABDUCTII
	// 取得宠物技能命令
	pszP = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszP == "\0" ) return iRet;
#endif

	//ANDY_ReEdit
	Attacktype = CHAR_getInt( attackindex, CHAR_WHICHTYPE);
	if( Attacktype == CHAR_TYPEENEMY )	{
		attackoyaindex = -1;
	}else if( Attacktype == CHAR_TYPEPET )	{
		attackoyaindex = CHAR_getWorkInt( attackindex, CHAR_WORKPLAYERINDEX );
		if( CHAR_CHECKINDEX( attackoyaindex ) == FALSE ) return iRet;
	}else	{
		return iRet;
	}
	//ANDY_ReEdit
	Deftype = CHAR_getInt( defindex, CHAR_WHICHTYPE );
	if( Deftype == CHAR_TYPEPET ){
		defoyaindex = CHAR_getWorkInt( defindex, CHAR_WORKPLAYERINDEX );
		if( CHAR_CHECKINDEX( defoyaindex ) == FALSE ) return iRet;
	}else	{
		defoyaindex = -1;
	}

	if( Deftype != CHAR_TYPEPLAYER ){
#ifdef _BATTLE_ABDUCTII
		int AiPer=0;
		// 取得宠物技能命令
		AiPer = atoi( pszP);
		if( AiPer <= 0 || Deftype != CHAR_TYPEPET ){
#endif
			attacklevel = CHAR_getInt( attackindex, CHAR_LV );
			deflevel = CHAR_getInt( defindex, CHAR_LV );
			per = ( deflevel - attacklevel ) * 0.6 + 30;
			per = max( per, 50 );
#ifdef _BATTLE_ABDUCTII
		}else {
			per = 0;
			if( CHAR_getWorkInt( defindex, CHAR_WORKFIXAI) < AiPer ){
				per = 200;
			}
		}
#endif
	}else{
		per = 0;
		return iRet;
	}
	if( BattleArray[battleindex].WinFunc != NULL ){
		per = 0;
	}
#ifdef _ALLBLUES_LUA
	else if( CHAR_getInt( BattleArray[battleindex].createindex, CHAR_WHICHTYPE ) == CHAR_TYPELUANPC ){
		per = 0;
	}
#endif
	iRet = TRUE;
	if( RAND( 1, 100 ) < per ){
		flg = 1;
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)带着(%s)离开该处(%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	per
		//);
		if( Deftype == CHAR_TYPEPET ){
			BATTLE_PetDefaultExit( defoyaindex, battleindex );
			CHAR_setInt( defoyaindex, CHAR_DEFAULTPET, -1 );
		}else if( Deftype == CHAR_TYPEENEMY )	{
			BATTLE_Exit( defindex, battleindex );
		}
	}else{
		flg = 0;
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)无法带离(%s)一个人默默地离开(%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	per
		//);
	}

	//if( getBattleDebugMsg( ) != 0 ){
	//	BATTLE_BroadCast( battleindex, szBuffer,
	//		(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	//}
	sprintf( szCommand, "B!|a%X|d%X|f%X|", attackNo, defNo, flg );
	BATTLESTR_ADD( szCommand );

	//ANDY_ReEdit
	if( Attacktype == CHAR_TYPEPET )	{
		BATTLE_PetDefaultExit( attackoyaindex, battleindex );
		CHAR_setInt( attackoyaindex, CHAR_DEFAULTPET, -1 );
	}else	{
		BATTLE_Exit( attackindex, battleindex);
	}
	return iRet;
}
#ifdef _BATTLESTEAL_FIX
void BATTLE_StealMoney( int battleindex, int attackNo, int defNo)
{
	char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, defindex, per;
	int S_FLG = 0, GOLD = 0;
	int LV=0;
	char buf1[256];
	int masterindex;
	int safeSide=0;

	if( BATTLE_CHECKINDEX( battleindex) == FALSE ) return;
	attackindex = BATTLE_No2Index( battleindex, attackNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )	return;
	defindex = BATTLE_No2Index( battleindex, defNo );
	if( CHAR_CHECKINDEX( defindex ) == FALSE )		return;
	masterindex = CHAR_getWorkInt( attackindex, CHAR_WORKPLAYERINDEX );
	if( !CHAR_CHECKINDEX( masterindex ) ) return;
	LV = CHAR_getInt( defindex, CHAR_LV );
	per = 0;
	if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )	{
		if( attackNo > 10 )	{
			safeSide = 1;
		}
		if( defNo>=safeSide*SIDE_OFFSET && defNo<((safeSide*SIDE_OFFSET)+SIDE_OFFSET)){//同边
			per = 0;
		}else	{	//不同边
			per = 50;
			per = (((per+LV)/4)+10)>>1;
		}
	}else if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )	{
		per=5;
	}
	S_FLG = 0;

	

	if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET )	{
		if( CHAR_getInt( masterindex, CHAR_GOLD) >= CHAR_getMaxHaveGold( masterindex) )
			per = 0;
	}

	if( RAND( 1, 100 ) < per ){
		S_FLG = 1;
		if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )	{
			GOLD = CHAR_getInt( defindex, CHAR_GOLD );
			GOLD = (int)( (float)GOLD * RAND( 1, 15 ) * 0.01 );
			if( (CHAR_getInt( masterindex, CHAR_GOLD) + GOLD) >= CHAR_getMaxHaveGold( masterindex) )	{
				GOLD = CHAR_getMaxHaveGold( masterindex) - CHAR_getInt( masterindex, CHAR_GOLD);
			}
		}else	{
			GOLD = RAND( 10, 100);
		}
		if( GOLD <= 0 ){
			S_FLG = 0;
		}else{
			//snprintf( szBuffer, sizeof(szBuffer),
			//		"(%s)从(%s)获得了(%d)STONE(%d%%)",
			//		CHAR_getUseName( attackindex ),
			//		CHAR_getUseName( defindex ),
			//		GOLD,
			//		per
			//	);
			if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )	{
				CHAR_setInt( defindex, CHAR_GOLD, CHAR_getInt( defindex, CHAR_GOLD ) - GOLD );
				CHAR_send_P_StatusString( defindex, CHAR_P_STRING_GOLD );
			}
		}
	}else{
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)没从(%s)获得东西(%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	per
		//);
	}
	sprintf( buf1, "被偷了一些石币");
	if( S_FLG)	{
		sprintf( szBuffer, "BK|%s%s。|", CHAR_getUseName( defindex ), buf1 );
		strcatsafe( szBadStatusString,sizeof(szBadStatusString), szBuffer );
		if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
			int attackoyaindex = CHAR_getWorkInt( attackindex, CHAR_WORKPLAYERINDEX );
			if( CHAR_CHECKINDEX( attackoyaindex ) == TRUE ){
				BATTLE_PetDefaultExit( attackoyaindex, battleindex );
				CHAR_setInt( attackoyaindex, CHAR_DEFAULTPET, -1 );
				{
					int playergold=CHAR_getInt( attackoyaindex, CHAR_GOLD)+GOLD;
					if( playergold > CHAR_getMaxHaveGold( attackoyaindex) )	playergold = CHAR_getMaxHaveGold( attackoyaindex);
					CHAR_setInt( attackoyaindex, CHAR_GOLD , playergold);
					CHAR_send_P_StatusString( attackoyaindex, CHAR_P_STRING_GOLD );
				}
				lssproto_KS_send( getfdFromCharaIndex( attackoyaindex ),
					CHAR_getInt( attackoyaindex, CHAR_DEFAULTPET ), 1 );
			}
			BATTLE_Exit( attackindex, battleindex );
		}else{
			BATTLE_Exit( attackindex, battleindex );
		}
	}
	//if( getBattleDebugMsg( ) != 0 )	{
	//	BATTLE_BroadCast( battleindex, szBuffer,
	//		(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	//}
	sprintf( szCommand, "B#|a%X|d%X|f%X|", attackNo, defNo, S_FLG );
	BATTLESTR_ADD( szCommand );
}
#endif

void BATTLE_Steal( int battleindex, int attackNo, int defNo )
{
	char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, defindex, per, flg, i, j;
	int enemytype, stealmode = 0, gold, itemindex;
	int itemtbl[CHAR_MAXITEMNUM];

	attackindex = BATTLE_No2Index( battleindex, attackNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )return;
	defindex = BATTLE_No2Index( battleindex, defNo );
	if( CHAR_CHECKINDEX( defindex ) == FALSE )return;
	enemytype = CHAR_getInt( defindex, CHAR_WHICHTYPE );
	if( enemytype == CHAR_TYPEPLAYER ){
		per = 50;
	}else{
		per = 0;
	}

	if( RAND( 1, 100 ) < per ){
		flg = 1;
		if( RAND( 1, 100 ) < 50 ){
			stealmode = 0;
			gold = CHAR_getInt( defindex, CHAR_GOLD );
			gold = (int)( (float)gold * RAND( 8, 12 ) * 0.01 );
			if( gold <= 0 ){
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)无法获得(%s)的STONE(%d%%)",
				//	CHAR_getUseName( attackindex ),
				//	CHAR_getUseName( defindex ),
				//	per
				//);
				flg = 0;
			}else{
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)从(%s)获得了(%d)STONE(%d%%)",
				//	CHAR_getUseName( attackindex ),
				//	CHAR_getUseName( defindex ),
				//	gold,
				//	per
				//);
				CHAR_setInt( defindex, CHAR_GOLD,
					CHAR_getInt( defindex, CHAR_GOLD ) - gold );
				CHAR_send_P_StatusString( defindex, CHAR_P_STRING_GOLD );
			}
		}else{
			stealmode = 1;
			for( j = 0, i = CHAR_STARTITEMARRAY; i < CHAR_MAXITEMHAVE; i ++ ){
				itemindex = CHAR_getItemIndex( defindex, i );
				if( ITEM_CHECKINDEX( itemindex ) == TRUE ){
					itemtbl[j++] = i;
				}
			}
			if( j <= 0 ){
				itemindex = -1;
			}else{
				itemindex = -1;
				i = RAND( 0, j-1 );
				if( i >= 0 ){
					if( 0 <= itemtbl[i] && itemtbl[i] < CHAR_STARTITEMARRAY ){
						fprint( "err:要盗取装备品(%d)\n", itemtbl[i]  );
					}else{
						itemindex = CHAR_getItemIndex( defindex, itemtbl[i] );
					}
				}
			}
			if( itemindex == - 1 ){
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)无法获得(%s)的道具(%d%%)",
				//	CHAR_getUseName( attackindex ),
				//	CHAR_getUseName( defindex ),
				//	per
				//);
				flg = 0;
			}else{
				//snprintf( szBuffer, sizeof(szBuffer),
				//	"(%s)从(%s)获得了(%s)(%d%%)",
				//	CHAR_getUseName( attackindex ),
				//	CHAR_getUseName( defindex ),
				//	ITEM_getAppropriateName( itemindex ),
				//	per
				//);
				CHAR_setItemIndex( defindex, itemtbl[i], -1 );
			    CHAR_sendItemDataOne( defindex, itemtbl[i] );
				{
//					LogItem(
//						CHAR_getChar( defindex, CHAR_NAME ), 
//						CHAR_getChar( defindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//						itemindex,
//#else
//		       			ITEM_getInt( itemindex, ITEM_ID ),
//#endif
//						"Stealed(偷窃的道具)",
//				       	CHAR_getInt( defindex,CHAR_FLOOR),
//						CHAR_getInt( defindex,CHAR_X ),
//			        	CHAR_getInt( defindex,CHAR_Y ),
//						ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//						ITEM_getChar( itemindex, ITEM_NAME),
//						ITEM_getInt( itemindex, ITEM_ID)
//					);
				}
				ITEM_endExistItemsOne( itemindex );
			}
		}
	}else{
		flg = 0;
		//snprintf( szBuffer, sizeof(szBuffer), "(%s)没从(%s)获得东西(%d%%)",
		//	CHAR_getUseName( attackindex ),	CHAR_getUseName( defindex ), per );
	}
	if( flg == 1 ){
		sprintf( szBuffer, "BK|%s被偷了一些东西。|",
			CHAR_getUseName( defindex ) );
		strcatsafe( szBadStatusString,sizeof(strcatsafe), szBuffer );
		if( CHAR_getInt( attackindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
			int attackoyaindex = CHAR_getWorkInt( attackindex, CHAR_WORKPLAYERINDEX );
			if( CHAR_CHECKINDEX( attackoyaindex ) == TRUE ){
				BATTLE_PetDefaultExit( attackoyaindex, battleindex );
				CHAR_setInt( attackoyaindex, CHAR_DEFAULTPET, -1 );
			}else{
				BATTLE_Exit( attackindex, battleindex );
			}
		}else{
			BATTLE_Exit( attackindex, battleindex );
		}
	}
	//if( getBattleDebugMsg( ) != 0 ){
	//	BATTLE_BroadCast( battleindex, szBuffer,
	//		(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	//}
	sprintf( szCommand, "B#|a%X|d%X|f%X|", attackNo, defNo, flg );
	BATTLESTR_ADD( szCommand );
}

#ifdef _PSKILL_FALLGROUND	//落马术
int BATTLE_S_FallGround( int battleindex, int attackNo, int defNo, int skill_type)
{
	char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, defindex;
	int damage=0, petdamage=0, i=0, iWork, Guardian=-1;
	int flg = 0, DefSide=0,par,ultimate=0, react=0;
	BOOL iRet = FALSE;
	attackindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	defindex = BATTLE_No2Index( battleindex, defNo ); //守方index


	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}
	react = 0;
	react = BATTLE_GetDamageReact( defindex);//有光镜守
	//取得攻击後状态
	iWork = BATTLE_AttackSeq( attackindex, defindex, &damage,
		&Guardian, BATTLE_COM_S_FALLRIDE );

	
	ultimate = BATTLE_DamageSub( attackindex, defindex, &damage, &petdamage, &react );
	if( react == BATTLE_MD_REFLEC )		defindex = attackindex;

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP )	defindex = attackindex;
#endif
#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ) defindex = attackindex;
#endif
	if( damage > 0 && ( react != BATTLE_MD_ABSROB )&& ( react != BATTLE_MD_VANISH )){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}
	par = BATTLE_AttrAdjust( attackindex, defindex, 100 );

	

	switch( iWork ){
	case BATTLE_RET_ALLGUARD:
	case BATTLE_RET_MISS:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)朝向(%s)发动绝技,没击中。",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex )
		//);
		iRet = TRUE;
		break;
	case BATTLE_RET_DODGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)展开防御攻击(%s)躲开了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_DODGE;
		iRet = TRUE;
		break;
	case BATTLE_RET_NORMAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)受到(%s)的绝技攻击(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		//andy_edit
		flg |= BATTLE_getReactFlg( defindex, react);
		flg |= BCF_NORMAL;
		iRet = TRUE;
		break;
	case BATTLE_RET_CRITICAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//"(%s)受到(%s)的绝技CRITICAL(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		flg |= BCF_KAISHIN;
		iRet = FALSE;
		flg |= BATTLE_getReactFlg( defindex, react);
		break;
#ifdef _EQUIT_ARRANGE
	case BATTLE_RET_ARRANGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)攻击了(%s)挡掉了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		flg |= BCF_B_ARRANGE;
		break;
#endif
	}

	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
	&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0 ){
		strncat( szBuffer,  "(GUARD)", sizeof( szBuffer ) );
		iRet = FALSE;
	}
	
	if( skill_type == BATTLE_COM_S_FALLRIDE && damage > 0 && react == 0
#ifdef _PREVENT_TEAMATTACK
		&& BATTLE_CheckSameSide( attackindex, defNo) == 0
#endif
		){	//落马
		int fallflg = RAND( 0, 100);
		if( 
#ifdef _EQUIT_RESIST
			fallflg > 50 + CHAR_getWorkInt( defindex, CHAR_WORKEQUITFALLRIDE )
#else
			fallflg > 50 
#endif
			){
			if( CHAR_getInt( defindex , CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )	{
#ifdef _FIXPETFALL //Syu ADD 修正落马术
				if( CHAR_getInt( defindex, CHAR_RIDEPET) >= 0 ) {
#else
				if( CHAR_getInt( defindex, CHAR_RIDEPET) > 0 )	{
#endif
						flg |= BCF_FALL;
						CHAR_setInt( defindex, CHAR_RIDEPET, -1 );
						BATTLE_changeRideImage( defindex );
						CHAR_setWorkInt( defindex, CHAR_WORKPETFALL, 1 );
				}
			}
#ifdef _ENEMY_FALLGROUND
			else if( CHAR_getInt( defindex , CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )	{
				if( CHAR_getInt( defindex, CHAR_RIDEPET) > 0 )	{
					CHAR_setInt( defindex, CHAR_RIDEPET, -1 );
					BATTLE_changeRideImage( defindex );
					CHAR_setWorkInt( defindex, CHAR_WORKPETFALL, 1 );

					CHAR_setInt( defindex, CHAR_STR, CHAR_getInt( defindex, CHAR_STR)*0.7);
					CHAR_setInt( defindex, CHAR_TOUGH, CHAR_getInt( defindex, CHAR_TOUGH)*0.7);
					CHAR_setInt( defindex, CHAR_VITAL, CHAR_getInt( defindex, CHAR_VITAL)*0.7);
					CHAR_complianceParameter( defindex );
					if( CHAR_getInt( defindex , CHAR_HP) > CHAR_getWorkInt( defindex , CHAR_WORKMAXHP ) )	{
						CHAR_setInt( defindex , CHAR_HP , CHAR_getWorkInt( defindex , CHAR_WORKMAXHP ) );
					}
				}
			}
#endif
		}
	}
	
	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
			ultimate = 1;
		}else
		if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY &&  flg & BCF_KAISHIN ){
			if( RAND( 1, 100 ) < 50 ){
				ultimate = 1;
			}
		}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) ultimate = 0;
#endif
		iRet = FALSE;
		flg |= BCF_DEATH;
		if( ultimate == 1 ){
			flg |= BCF_ULTIMATE_1;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
		if( ultimate == 2 ){
			flg |= BCF_ULTIMATE_2;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
	}
	if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
		if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
			flg |= BCF_CRUSH;
		}
	}	
	snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
		attackNo, defNo, flg, damage, petdamage );

	BATTLESTR_ADD( szCommand );
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}

	return iRet;
}
#endif

#ifdef _PRO_BATTLEENEMYSKILL
int BATTLE_E_ENEMYREFILE( int battleindex, int attackNo, int defNo, int skill_type )
{
	int ReceveEffect;
	BOOL iRet = FALSE;
	int attackindex, defindex, pow, toNo;

	toNo=-1;
	attackindex = BATTLE_No2Index( battleindex, attackNo);

	if( !CHAR_CHECKINDEX( attackindex) ) return iRet;
	if( CHAR_getInt( attackindex , CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )	{
		int k=0, j=0, index;
		int ToNoList[10];
		int l=0;
		memset( ToNoList,-1, sizeof( ToNoList));

		for( k=10;k<20;k++)	{
			index = BATTLE_getBattleDieIndex( battleindex, k);
			if( !CHAR_CHECKINDEX( index) ) continue;
			if( CHAR_getFlg( index, CHAR_ISDIE ) == TRUE )	{
				ToNoList[j] = k;
				j++;
			}
		}
		if( j == 0 )	return iRet;
		toNo = ToNoList[ l=RAND( 0, j-1)];
		if( toNo < 0 || toNo > 20 )
			return iRet;
	}else if( CHAR_getInt( attackindex , CHAR_WHICHTYPE ) == CHAR_TYPEPET )	{
		toNo = defNo;
	}else	{
		return iRet;
	}
	if( !BATTLE_CHECKNO( toNo) ) return iRet;
	ReceveEffect = SPR_fukkatu1;
	defindex = BATTLE_No2Index( battleindex, toNo);
	if( !CHAR_CHECKINDEX( defindex) ) return iRet;
	pow = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP)/2;
	BATTLE_MultiRessurect( battleindex, attackNo, toNo,
		pow, 0, SPR_item3, ReceveEffect );
	iRet = TRUE;

	return iRet;
}

int BATTLE_E_ENEMYREHP( int battleindex, int attackNo, int defNo, int skill_type)
{
	BOOL iRet=FALSE;
	int attackindex, defindex;
	int power = 0, per = 0, HealedEffect, toNo=-1;
	int kind;

	attackindex = BATTLE_No2Index( battleindex, attackNo);
	if( CHAR_CHECKINDEX( attackindex ) == FALSE ) return iRet;
	if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_INIT ||
		!CHAR_getWorkInt( attackindex, CHAR_WORKBATTLEMODE ) ){
		return iRet;
	}

	if( CHAR_getInt( attackindex , CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )	{
		int k=0, j=0, l, index;
		int ToNoList[10];
		memset( ToNoList,-1, sizeof( ToNoList));
		for( k=10;k<20;k++)	{
			index = BATTLE_No2Index( battleindex, k);
			if( !CHAR_CHECKINDEX( index) ) continue;
			if( CHAR_getInt( index, CHAR_HP) <= 0 ) continue;
			if( CHAR_getInt( index, CHAR_HP) < (CHAR_getWorkInt( index, CHAR_WORKMAXHP) *2/3))	{
				ToNoList[j] = k;
				j++;
			}
		}
		if( j == 0 )	return iRet;
		toNo = ToNoList[ l=RAND( 0, j-1)];
		if( toNo < 0 || toNo > 20 )
			return iRet;

	}else if( CHAR_getInt( attackindex , CHAR_WHICHTYPE ) == CHAR_TYPEPET )	{
		toNo = defNo;
	}else	{
		return iRet;
	}
	defindex = BATTLE_No2Index( battleindex, toNo);
	if( CHAR_CHECKINDEX( defindex ) == FALSE ) return iRet;

	kind = 0;
	power = RAND( 100, CHAR_getWorkInt( defindex, CHAR_WORKMAXHP));
	HealedEffect = SPR_heal2;
	BATTLE_MultiRecovery( battleindex, attackNo, toNo,kind, power, per, SPR_item3, HealedEffect );

	iRet = TRUE;
	return iRet;
}

int BATTLE_E_ENEMYHELP( int battleindex, int attackNo, int defNo, int skill_type )
{
	BOOL iRet = FALSE;
	int attackindex, enindex, array;
	int Side=-1, LV, i, enemynum, PetID;
	//char szBuffer[256]="";
	//print("\n BATTLE_E_ENEMYHELP()");

	attackindex = BATTLE_No2Index( battleindex, attackNo);
	if( CHAR_CHECKINDEX( attackindex ) == FALSE ) return iRet;
	if( CHAR_getInt( attackindex , CHAR_WHICHTYPE ) != CHAR_TYPEENEMY ||
		BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P		||
		CHAR_getInt( attackindex , CHAR_HP) <= 0 )	{
		return iRet;
	}

	if( CHAR_getInt( attackindex , CHAR_WHICHTYPE ) == CHAR_TYPEENEMY )	{
		int k=0, index;
		int ToNoList[10];
		memset( ToNoList,-1, sizeof( ToNoList));
		for( k=10;k<20;k++)	{
			index = BATTLE_No2Index( battleindex, k);
			if( !CHAR_CHECKINDEX( index) )	{
				break;
			}
		}
		if( k == 20 )	return iRet;
	}
	LV = CHAR_getInt( attackindex, CHAR_LV );

	PetID = CHAR_getInt( attackindex, CHAR_PETID );
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		//if( ENEMY_getInt( i, ENEMY_ID ) == PetID )
		if( ENEMY_getInt( i, ENEMY_TEMPNO ) == PetID )
			break;
	}
	if( i == enemynum )
		return iRet;
	array = i;

	Side = CHAR_getWorkInt( attackindex, CHAR_WORKBATTLESIDE );
	enindex = ENEMY_createEnemy( array, RAND( (LV*0.8), LV*1.2));
	if( !CHAR_CHECKINDEX( enindex ) ) return iRet;
	if( BATTLE_NewEntry( enindex, battleindex, Side) )	{
		//CHAR_setWorkInt( enindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE );	
		CHAR_setWorkInt( enindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
		//snprintf( szBuffer, sizeof( szBuffer ), "(%s)加入作战。",
		//	CHAR_getUseName( enindex ) );
		//BATTLE_BroadCast( battleindex, szBuffer, CHAR_COLORYELLOW );
	}else	{

	}
	iRet = TRUE;
	return iRet;

}

#endif

int BATTLE_DefineAttack( int attackindex, int defindex, int iWork,
						char *szBuffer, int damage, int react, int *flg)
{
	int iRet;
	int par;
	iRet = TRUE;
	par = BATTLE_AttrAdjust( attackindex, defindex, 100 );
	switch( iWork ){
	case BATTLE_RET_ALLGUARD:
	case BATTLE_RET_MISS:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)朝向(%s)发动绝技,没击中。",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex )
		//);
		iRet = TRUE;
		break;
	case BATTLE_RET_DODGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)展开防御攻击(%s)躲开了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		*flg |= BCF_DODGE;
		iRet = TRUE;
		break;
	case BATTLE_RET_NORMAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)受到(%s)的绝技攻击(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);

		*flg |= BATTLE_getReactFlg( defindex, react);
		*flg |= BCF_NORMAL;
		iRet = TRUE;
		break;
	case BATTLE_RET_CRITICAL:
		//snprintf( szBuffer, sizeof(szBuffer),
		//"(%s)受到(%s)的绝技CRITICAL(%d)损伤(%d:%d:%d%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	damage,
		//	CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER ),
		//	CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER ),
		//	par
		//);
		*flg |= BCF_KAISHIN;
		iRet = FALSE;
		*flg |= BATTLE_getReactFlg( defindex, react);
		break;
#ifdef _EQUIT_ARRANGE
	case BATTLE_RET_ARRANGE:
		//snprintf( szBuffer, sizeof(szBuffer),
		//	"(%s)攻击了(%s)挡掉了(%.2f%%)",
		//	CHAR_getUseName( attackindex ),
		//	CHAR_getUseName( defindex ),
		//	gDuckPer
		//);
		*flg |= BCF_B_ARRANGE;
		break;
#endif
	}
	if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_GUARD
	&&  CHAR_getWorkInt( defindex, CHAR_WORKCONFUSION ) <= 0 ){
		//strncat( szBuffer,  "(GUARD)", sizeof( szBuffer ) );
		iRet = FALSE;
	}
	
	return iRet;
}

int BATTLE_DefDieType( int defindex, int iRet, int *ultimate, int *flg, char *szBuffer)
{

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO ){
			*ultimate = 1;
		}else
		if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEENEMY &&  *flg & BCF_KAISHIN ){
			if( RAND( 1, 100 ) < 50 ){
				*ultimate = 1;
			}
		}

#ifdef _PETSKILL_LER
// 雷尔不能被打飞
		if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) *ultimate = 0;
#endif
		iRet = FALSE;
		*flg |= BCF_DEATH;
		if( *ultimate == 1 ){
			*flg |= BCF_ULTIMATE_1;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
		if( *ultimate == 2 ){
			*flg |= BCF_ULTIMATE_2;
			strcatsafe( szBuffer,sizeof(szBuffer), "ultimate?" );
		}
	}
	return iRet;
}

#ifdef _SKILL_DAMAGETOHP
int BATTLE_S_DamageToHp( int battleindex, int attackindex, int defindex, int Damage, int skill)
{
	char *pszOption;
	char buf1[256];
	int A_HP=0, pHP,defi=0;
	float def=0.00;
	if( Damage < 1 )	return A_HP;
	if( BATTLE_GetDamageReact( defindex) > 0 )//有光镜守
		return A_HP;

	pszOption = PETSKILL_getChar( skill, PETSKILL_OPTION );
	if( pszOption == "\0" )
		return A_HP;
	memset( buf1, 0, sizeof( buf1));
	if( getStringFromIndexWithDelim( pszOption, "|",2, buf1,sizeof( buf1)) == FALSE )
		return A_HP;

	defi = atoi( buf1);
	def = ((float)defi)/100;
	A_HP = (int)(Damage*def);
	if( (A_HP+CHAR_getInt( attackindex, CHAR_HP)) > CHAR_getWorkInt( attackindex, CHAR_WORKMAXHP ) ){
		A_HP = CHAR_getWorkInt( attackindex, CHAR_WORKMAXHP )-CHAR_getInt( attackindex, CHAR_HP);
	}

	pHP = CHAR_getInt( attackindex, CHAR_HP) + A_HP;
	CHAR_setInt( attackindex, CHAR_HP, min( pHP, CHAR_getWorkInt( attackindex, CHAR_WORKMAXHP )));
	CHAR_send_P_StatusString(  attackindex, CHAR_P_STRING_HP);
	return A_HP;
}
#endif

#ifdef _PETSKILL_DAMAGETOHP
int BATTLE_S_DamageToHp2( int battleindex, int attackindex, int defindex, int Damage, int skill)
{
	char *pszOption;
	char buf1[256];
	int A_HP=0, pHP,defi=0;
	float def=0.00;
	if( Damage < 1 )	return A_HP;
	if( BATTLE_GetDamageReact( defindex) > 0 )//有光镜守
		return A_HP;

	pszOption = PETSKILL_getChar( skill, PETSKILL_OPTION );
	if( pszOption == "\0" )
		return A_HP;
	memset( buf1, 0, sizeof( buf1));
	sprintf(buf1,"%s",pszOption);
	//if( getStringFromIndexWithDelim( pszOption, "|",2, buf1,sizeof( buf1)) == FALSE )
	//	return A_HP;

	defi = atoi( buf1);
	def = ((float)defi)/100;
	A_HP = (int)(Damage*def);
	if( (A_HP+CHAR_getInt( attackindex, CHAR_HP)) > CHAR_getWorkInt( attackindex, CHAR_WORKMAXHP ) ){
		A_HP = CHAR_getWorkInt( attackindex, CHAR_WORKMAXHP )-CHAR_getInt( attackindex, CHAR_HP);
	}
	//print("\n伤:%d 回血:%d",Damage,A_HP);
	pHP = CHAR_getInt( attackindex, CHAR_HP) + A_HP;
	CHAR_setInt( attackindex, CHAR_HP, min( pHP, CHAR_getWorkInt( attackindex, CHAR_WORKMAXHP )));
	CHAR_send_P_StatusString(  attackindex, CHAR_P_STRING_HP);
	return A_HP;
}
#endif

#ifdef _Skill_MPDAMAGE	//BATTLE_COM_S_MPDAMAGE
int BATTLE_S_MpDamage( int battleindex, int attackindex, int defindex, int damage, int skill)
{
	char *pszOption;
	char buf1[256];
	int D_MP=0,defi=0;
	float def=0.00;
	if( damage < 1 )	return D_MP;
	if( BATTLE_GetDamageReact( defindex) > 0 )//有光镜守
		return D_MP;
	if( CHAR_getInt( defindex, CHAR_WHICHTYPE) == CHAR_TYPEENEMY || 
		CHAR_getInt( defindex, CHAR_WHICHTYPE) == CHAR_TYPEPET )
		return D_MP;
	if( CHAR_getInt( defindex, CHAR_MP) <= 0 ) return D_MP;

	pszOption = PETSKILL_getChar( skill, PETSKILL_OPTION );
	if( pszOption == "\0" ) return D_MP;
	memset( buf1, 0, sizeof( buf1));
	if( getStringFromIndexWithDelim( pszOption, "|",2, buf1,sizeof( buf1)) == FALSE )
		return D_MP;
	defi = atoi( buf1);
	def = ((float)defi)/100;
	D_MP = (int)(CHAR_getInt( defindex, CHAR_MP)*def);
	CHAR_setInt( defindex, CHAR_MP, ( CHAR_getInt( defindex, CHAR_MP) - D_MP) );
	CHAR_send_P_StatusString( defindex, CHAR_P_STRING_MP);
	return D_MP;
}
#endif
#ifdef _SKILL_TOOTH
void BATTLE_S_ToothCrushe( int battleindex, int attackindex, int defindex, int damage, int skill)
{
	int crushindex, itemindex;	//aszCrushTbl
	//char *strCrushTbl[] = { "完好", "受损", "毁坏" , "不堪使用" , "碎片" };
	if( CHAR_getInt( defindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _TAKE_ITEMDAMAGE_FOR_PET
			&& CHAR_getInt( defindex, CHAR_WHICHTYPE) == CHAR_TYPEPET
#endif
    )
		return;
#ifdef _TAKE_ITEMDAMAGE
	if( ( crushindex = BATTLE_ItemCrushCheck( defindex,  1) ) >= 0 ) {
		int crushenum, maxcrushenum, level=0;
		float def=0.00;
		itemindex = CHAR_getItemIndex( defindex, crushindex );
		if( ITEM_CHECKINDEX(itemindex) == FALSE ) return;
		crushenum = ITEM_getInt( itemindex, ITEM_DAMAGECRUSHE);
		maxcrushenum = ITEM_getInt( itemindex, ITEM_MAXDAMAGECRUSHE);
		if( maxcrushenum < 1 ) return;
		def = ((float)crushenum/maxcrushenum);
		if( def >= 1 )	{
			level = 1;
			def = 0.70;
		}else if( def >= 0.50 ){
			level = 2;
			def = 0.30;
		}else if( def >= 0.30 ){
			level = 3;
			def = 0.10;
		}else{
			def = 0.00;
		}
		crushenum = (int)((float)maxcrushenum*def);
		ITEM_setInt( itemindex, ITEM_DAMAGECRUSHE, crushenum);

		if( crushenum <= 0 )	{
			char buf2[256];
			sprintf(buf2, "%s因过度损坏而消失。\n", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( defindex, -1, buf2, CHAR_COLORYELLOW);

//			LogItem(
//				CHAR_getChar( defindex, CHAR_NAME ), 
//				CHAR_getChar( defindex, CHAR_CDKEY ),
//#ifdef _add_item_log_name  // WON ADD 在item的log中增加item名称
//				itemindex,
//#else
//       			ITEM_getInt( itemindex, ITEM_ID ),  
//#endif
//				"因过度损坏而消失",
//				CHAR_getInt( defindex,CHAR_FLOOR),
//				CHAR_getInt( defindex,CHAR_X ),
// 				CHAR_getInt( defindex,CHAR_Y ),
//				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
//					ITEM_getChar( itemindex, ITEM_NAME),
//					ITEM_getInt( itemindex, ITEM_ID)
//			);

			CHAR_setItemIndex( defindex, crushindex ,-1);
			ITEM_endExistItemsOne( itemindex);

		}else	{
			char buf2[256];
			sprintf( buf2, "%s是%s的。", ITEM_getChar( itemindex, ITEM_NAME ), aszCrushTbl[level] );
			CHAR_talkToCli( defindex, -1, buf2, CHAR_COLORYELLOW);
			//更改道具说明
//			buf1 = ITEM_getChar( itemindex, ITEM_SECRETNAME);
//			if( strstr( buf1, "(") != 0 )	{
//				char buf5[256];
//				if( getStringFromIndexWithDelim( buf1,"(",1, buf5, sizeof( buf5)) != FALSE )	{
//					sprintf( buf1, buf5);
//				}
//			}
//			sprintf( buf2, "%s(%s)", buf1, aszCrushTbl[level]);
//			ITEM_setChar( itemindex, ITEM_SECRETNAME, buf2);
			ITEM_setInt( itemindex, ITEM_CRUSHLEVEL, level);
		}
   		CHAR_sendItemDataOne( defindex, crushindex );
		CHAR_complianceParameter( defindex );
		CHAR_send_P_StatusString( defindex, CHAR_P_STRING_ATK | CHAR_P_STRING_DEF 
			| CHAR_P_STRING_QUICK | CHAR_P_STRING_CHARM );

	}
#endif
}
#endif

#ifdef _PSKILL_MODIFY
void BATTLE_S_Modifyattack( int battleindex, int attackindex, int defindex, int *damage, int skill)
{
	int i;
	char *pszOption=NULL;
	int array = skill;
	char buf1[256], buf2[256];
	float def=0.00;
	struct tagModKind{
		char szModKind[36];
		int Kind;
	}KModKind[5] = {
		{ "EA", CHAR_EARTHAT}, { "WA", CHAR_WATERAT},
		{ "FI", CHAR_FIREAT} , { "WI", CHAR_WINDAT},
		{ "ALL",100}
	};


	pszOption = PETSKILL_getChar( array, PETSKILL_OPTION );
	if( pszOption == "\0" ) return;
	if( getStringFromIndexWithDelim( pszOption, "|", 1, buf1, sizeof( buf1)) == FALSE )
		return;
	
	if( getStringFromIndexWithDelim( pszOption, "|", 2, buf2, sizeof( buf1)) == FALSE )
		return;
	def = ((float)(atoi( buf2))/100);

	for( i=0; i< 4; i++)	{
		int ModNum=0;
		if( strcmp( KModKind[i].szModKind , buf1)) continue;
		if( (ModNum = CHAR_getInt( defindex, KModKind[i].Kind )) > 0 )	{
			def += (float)((rand()%(ModNum+5))/100);
			*damage += *damage * def;
			break;
		}
	}
}
#endif

#ifdef _PETSKILL_PROPERTY
int BATTLE_S_PetSkillProperty( int battleindex, int attackNo, int skill_type, int skill)
{
	int attackindex;
	BOOL iRet = FALSE;
	char *pszP=NULL;

	pszP = PETSKILL_getChar( skill, PETSKILL_OPTION );
	if( pszP == "\0" ) return iRet;


	attackindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	if( !CHAR_CHECKINDEX( attackindex)) return iRet;
	switch( skill_type){
	case BATTLE_COM_S_PROPERTYSKILL:
		{
			Char 	*ch;
			ch  = CHAR_getCharPointer( attackindex);
			if( ch == NULL ) return iRet;
			strcpysafe( ch->charfunctable[CHAR_BATTLEPROPERTY].string,
				sizeof( ch->charfunctable[CHAR_BATTLEPROPERTY]), pszP);//战斗
			CHAR_constructFunctable( attackindex);
			iRet = TRUE;
		}
		break;
	}
	return iRet;
}
#endif

int BATTLE_S_AttackDamage( int battleindex, int attackNo, int defNo, int skill_type, int skill)
{
	char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, defindex;
	int damage=0, petdamage=0, i=0, iWork, Guardian=-1,react;
	int flg = 0, DefSide=0,ultimate=0;
	int otherdamage=0;
	BOOL iRet = FALSE;
	int ReactType = 0;

	char *pszP=NULL;
	pszP = PETSKILL_getChar( skill, PETSKILL_OPTION );
	if( pszP == "\0" ) return iRet;

	react = 0;
	attackindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	defindex = BATTLE_No2Index( battleindex, defNo ); //守方index
	if( (ReactType=BATTLE_GetDamageReact( defindex)) > 0 )	{	//有光镜守
		react = ReactType;
#ifdef _BATTLE_LIGHTTAKE
		if( skill_type == BATTLE_COM_S_LIGHTTAKE ){
			int Statustype = -2;
			if( strstr( pszP, "VANISH") != 0 ) Statustype = BATTLE_MD_VANISH;
			else if( strstr( pszP, "ABSROB") != 0 ) Statustype = BATTLE_MD_ABSROB;
			else if( strstr( pszP, "REFLEC") != 0 ) Statustype = BATTLE_MD_REFLEC;
			if( ReactType == Statustype ){
				react=0;
			}else{
				skill_type=-1;
			}
		}else{
			skill_type=-1;
		}
#else
		skill_type=-1;
#endif
	}

	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}

	//取得攻击後状态
	iWork = BATTLE_AttackSeq( attackindex, defindex, &damage, &Guardian, skill_type );

	switch( skill_type){
#ifdef _PSKILL_MODIFY
	case BATTLE_COM_S_MODIFYATT:
		if( damage > 0 ){
			BATTLE_S_Modifyattack( battleindex, attackindex, defindex, &damage, skill);
		}
		break;
#endif
#ifdef _PETSKILL_TEAR
	case BATTLE_COM_S_PETSKILLTEAR:
#ifdef _PREVENT_TEAMATTACK
		if( BATTLE_CheckSameSide( attackindex, defNo) == 1 ){
		}else
#endif
		if( iWork != BATTLE_RET_DODGE && react == 0 ){
			int userhp=0, petindex;
			float def=0;
			userhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP) - CHAR_getInt( defindex, CHAR_HP);
			if( CHAR_getInt( defindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER &&
				(petindex = BATTLE_getRidePet( defindex )) != -1 ){
				userhp += CHAR_getWorkInt( petindex, CHAR_WORKMAXHP) - CHAR_getInt( petindex, CHAR_HP);
			}
			def = ((float)atoi( pszP))/100;
			userhp = userhp * def;
			if( userhp <= 0 ) damage = 0;
			else damage += userhp;
		}
		break;
#endif
	}
	ultimate = BATTLE_DamageSub( attackindex, defindex, &damage, &petdamage, &react);

	if( react == BATTLE_MD_REFLEC )		defindex = attackindex;

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP )	defindex = attackindex;
#endif

#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE )	defindex = attackindex;
#endif

	if( damage > 0 && ( react != BATTLE_MD_ABSROB )&& ( react != BATTLE_MD_VANISH )){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}
    iRet = BATTLE_DefineAttack( attackindex, defindex, iWork, szBuffer, damage, react, &flg);
	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 )
		iRet = BATTLE_DefDieType( defindex, iRet, &ultimate, &flg, szBuffer);
#ifdef _PETSKILL_ANTINTER
		//强制将死掉的宠打飞
		if( CHAR_getWorkInt( attackindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_ANTINTER 
			&& CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				ultimate = 2;	
				flg &= (!BCF_ULTIMATE_1);
				flg |= BCF_DEATH;
				flg |= BCF_ULTIMATE_2;
				CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, 0 );
				damage=1;//因後面有判断若 damage<=0 则 skill_type=-1 所以这里damage设为1
		}
#endif

	if( damage > 0 ){
#ifdef _TAKE_ITEMDAMAGE
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE ){
#else
		if( BATTLE_ItemCrushSeq( defindex ) == TRUE ){
#endif
			flg |= BCF_CRUSH;
		}
	}

	if( damage <= 0 )	{
#ifdef _SONIC_ATTACK				// WON ADD 音波攻击
		if( skill_type != BATTLE_COM_S_SONIC && skill_type != BATTLE_COM_S_SONIC2 
#ifdef _PETSKILL_REGRET
			&& skill_type != BATTLE_COM_S_REGRET && skill_type != BATTLE_COM_S_REGRET2 
#endif
			)
#endif
			skill_type=-1;
	}




	memset( szCommand, 0, sizeof( szCommand));
	switch( skill_type)	{
	case -1:
		snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
							attackNo, defNo, flg, damage, petdamage );
		BATTLESTR_ADD( szCommand );
		break;

#ifdef _PETSKILL_TEAR
	case BATTLE_COM_S_PETSKILLTEAR:
		snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
							attackNo, defNo, flg, damage, petdamage );
		BATTLESTR_ADD( szCommand );
		break;
#endif

#ifdef _SKILL_DAMAGETOHP
	case BATTLE_COM_S_DAMAGETOHP:
		otherdamage = BATTLE_S_DamageToHp( battleindex, attackindex, defindex,damage+petdamage, skill);
		snprintf( szCommand, sizeof( szCommand ), "Bh|a%X|r%X|f%X|d%X|p%X|j%X|FF|",
							attackNo, defNo, flg, damage, petdamage, otherdamage);
		BATTLESTR_ADD( szCommand );
		break;
#endif

#ifdef _PETSKILL_DAMAGETOHP
	case BATTLE_COM_S_DAMAGETOHP2:
		otherdamage = BATTLE_S_DamageToHp2( battleindex, attackindex, defindex,damage+petdamage, skill);
		snprintf( szCommand, sizeof( szCommand ), "Bh|a%X|r%X|f%X|d%X|p%X|j%X|FF|",
							attackNo, defNo, flg, damage, petdamage, otherdamage);
		BATTLESTR_ADD( szCommand );
		break;
#endif

#ifdef _Skill_MPDAMAGE
	case BATTLE_COM_S_MPDAMAGE:
		otherdamage = BATTLE_S_MpDamage( battleindex, attackindex, defindex, damage, skill);
		snprintf( szCommand, sizeof( szCommand ), "Bp|a%X|r%X|f%X|d%X|p%X|j%X|FF|",
							attackNo, defNo, flg, damage, petdamage, otherdamage);
		BATTLESTR_ADD( szCommand );
		break;
#endif

#ifdef _SKILL_TOOTH
	case BATTLE_COM_S_TOOTHCRUSHE:	//囓齿术 tooth
		flg |= BCF_TOOTH;
		BATTLE_S_ToothCrushe( battleindex, attackindex, defindex, damage, skill);
		snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
							attackNo, defNo, flg, damage, petdamage );
		BATTLESTR_ADD( szCommand );
		break;
#endif

#ifdef _PSKILL_MDFYATTACK
	case BATTLE_COM_S_MDFYATTACK:
		flg |= BCF_MODIFY;
		snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|g%X|FF|",
					attackNo, defNo, flg, damage, petdamage, skill);
		BATTLESTR_ADD( szCommand );
		break;
#endif
#ifdef _PSKILL_MODIFY
	case BATTLE_COM_S_MODIFYATT:
		flg |= BCF_ATTDOUBLE;
		snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|g%X|FF|",
							attackNo, defNo, flg, damage, petdamage, skill);
		BATTLESTR_ADD( szCommand );
		break;
#endif

#ifdef _PETSKILL_TIMID
	case BATTLE_COM_S_TIMID:
		{
			int timid = rand()%100;
			snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
								attackNo, defNo, flg, damage, petdamage );
			BATTLESTR_ADD( szCommand );
			if( timid < 15 && damage > 1 ){//怯战
				BATTLE_NoAction( battleindex, defNo );
				sprintf( szCommand, "BE|e%X|", defNo );
				BATTLESTR_ADD( szCommand );
				snprintf( szCommand, sizeof( szCommand ), "f%X|", 1);
				BATTLESTR_ADD( szCommand );

				if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
					int defkoyaindex = CHAR_getWorkInt( defindex, CHAR_WORKPLAYERINDEX );
#ifdef _PETS_SELECTCON
					lssproto_PETS_send( getfdFromCharaIndex(defkoyaindex),
						CHAR_getInt( defkoyaindex, CHAR_DEFAULTPET), FALSE);
					CHAR_setWorkInt( defkoyaindex,
						CHAR_WORK_PET0_STAT+CHAR_getInt( defkoyaindex, CHAR_DEFAULTPET), FALSE);
#endif
					BATTLE_PetDefaultExit( defkoyaindex, battleindex );
					CHAR_setInt( defkoyaindex, CHAR_DEFAULTPET, -1 );

				}else {
					BATTLE_Exit( defindex, battleindex);
					CHAR_DischargePartyNoMsg( defindex);//解散团队
				}
			}
		}
		break;
#endif
#ifdef _PETSKILL_2TIMID
	case BATTLE_COM_S_2TIMID:
		{
			int timid=0;
			char *timidc=NULL;
			pszP = PETSKILL_getChar( skill, PETSKILL_OPTION );
			if( ( timidc = strstr( pszP, "命%" ) ) != NULL )
				sscanf( timidc+3,"%d", &timid );
			snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
								attackNo, defNo, flg, damage, petdamage );
			BATTLESTR_ADD( szCommand );
			if( rand()%100 < timid && damage > 1 ){
				if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
					int defkoyaindex = CHAR_getWorkInt( defindex, CHAR_WORKPLAYERINDEX );
					
					snprintf( szCommand, sizeof( szCommand ), "K%d", CHAR_getInt( defkoyaindex, CHAR_DEFAULTPET ) );
					CHAR_sendStatusString( defkoyaindex, szCommand );
				
					BATTLE_PetIn( battleindex, defNo-5 );
					lssproto_KS_send( getfdFromCharaIndex( defkoyaindex ),	CHAR_getInt( defkoyaindex, CHAR_DEFAULTPET ), 1 );

					/*BATTLE_NoAction( battleindex, defNo );
					sprintf( szCommand, "BE|e%X|", defNo );
					BATTLESTR_ADD( szCommand );
					snprintf( szCommand, sizeof( szCommand ), "f%X|", 1);
					BATTLESTR_ADD( szCommand );

					BATTLE_PetDefaultExit( defkoyaindex, battleindex );
					CHAR_setInt( defkoyaindex, CHAR_DEFAULTPET, -1 );

					print("\ndefNo:%d",defNo);*/
				}
			}
		}
		break;
#endif

#ifdef _PETSKILL_ANTINTER
		case BATTLE_COM_S_ANTINTER:
		{
			pszP = PETSKILL_getChar( skill, PETSKILL_OPTION );
			//flg &= !(BCF_ULTIMATE_1 | BCF_ULTIMATE_2);	
			snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
								attackNo, defNo, flg |= BCF_ANTINTER , 0, 0 );
			BATTLESTR_ADD( szCommand );
			if( CHAR_getInt( defindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET ){
				int defkoyaindex = CHAR_getWorkInt( defindex, CHAR_WORKPLAYERINDEX );
				BATTLE_PetDefaultExit( defkoyaindex, battleindex );//宠物离开战场
				CHAR_setInt( defkoyaindex, CHAR_DEFAULTPET, -1 );//设定无参战宠物
			}
		}
		break;
#endif

#ifdef _BATTLE_LIGHTTAKE
		case BATTLE_COM_S_LIGHTTAKE:
			{
				int Typenum = 0;
				if( strstr( pszP, "VANISH") != 0 ){
					if( ReactType == BATTLE_MD_VANISH ){
						Typenum = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEVANISH );
						CHAR_setWorkInt( attackindex, CHAR_WORKDAMAGEVANISH, Typenum);
					}
				}else if( strstr( pszP, "ABSROB") != 0 ){
					if( ReactType == BATTLE_MD_ABSROB ){
						Typenum = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEABSROB );
						CHAR_setWorkInt( attackindex, CHAR_WORKDAMAGEABSROB, Typenum);
					}
				}else if( strstr( pszP, "REFLEC") != 0 ){
					if( ReactType == BATTLE_MD_REFLEC ){ 
						Typenum = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEREFLEC );
						CHAR_setWorkInt( attackindex, CHAR_WORKDAMAGEREFLEC, Typenum);
					}
				}
				snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
									attackNo, defNo, flg, damage, petdamage );
				BATTLESTR_ADD( szCommand );
			}
			break;
#endif

#ifdef _SONIC_ATTACK				// WON ADD 音波攻击
		// 宠物伤害
		case BATTLE_COM_S_SONIC:
			{
				int img2=0;
				if( defNo >= 10 ) img2 = 101703;
				else			  img2 = 101704;

				flg |= BCF_B_SKILLACT; //击中後秀图	

				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|e%X|s%X|h%X|FF|",
									attackNo, defNo, flg, damage, 0, 0, img2 );
				BATTLESTR_ADD( szCommand );

				break;	
			}
		// 人物伤害
		case BATTLE_COM_S_SONIC2:
			{
				sprintf( szCommand, "BD|r%X|0|0|d%X|p%X|", defNo, damage, petdamage );
				BATTLESTR_ADD( szCommand );

				break;	
			}
#endif
#ifdef _PETSKILL_REGRET	
		// 宠物伤害
		case BATTLE_COM_S_REGRET:
			{
				int img2=0,Success=0;
				char *psz = NULL;
				if( (psz = strstr( pszP, "命%" ) ) != NULL )	sscanf( psz+3, "%d", &Success );		
				if( (PROFESSION_BATTLE_StatusAttackCheck( attackindex, defindex, 12, Success ) == 0 )
					|| (CHAR_getInt( defindex, CHAR_HP ) <= 0 ) ){
									
				}else{
					CHAR_setWorkInt( defindex, StatusTbl[12], 2 );//晕眩1回合
					CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
					BATTLE_BadStatusString( defNo, 12 );
				}

				if( defNo >= 10 ) img2 = 101418;//101703;
				else			  img2 = 101418;//101704;

				flg |= BCF_B_SKILLACT; //击中後秀图	
				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|e%X|s%X|h%X|FF|",
									attackNo, defNo, flg, damage, 0, 0, img2 );
				BATTLESTR_ADD( szCommand );
				break;	
			}
		// 人物伤害
		case BATTLE_COM_S_REGRET2:
			{
				int Success;
				char *psz = NULL;
				if( (psz = strstr( pszP, "命%" ) ) != NULL )	sscanf( psz+3, "%d", &Success );		
				if( (PROFESSION_BATTLE_StatusAttackCheck( attackindex, defindex, 12, Success ) == 0 )
					|| (CHAR_getInt( defindex, CHAR_HP ) <= 0 ) ){
									
				}else{
					CHAR_setWorkInt( defindex, StatusTbl[12], 2 );//晕眩1回合
					CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
					BATTLE_BadStatusString( defNo, 12 );
				}

				sprintf( szCommand, "BD|r%X|0|0|d%X|p%X|", defNo, damage, petdamage );
				BATTLESTR_ADD( szCommand );
				break;	
			}
#endif

	default:
		snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|",
							attackNo, defNo, flg, damage, petdamage );
		BATTLESTR_ADD( szCommand );
		break;
   }  
	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}
	return iRet;
}

#ifdef _MAGIC_SUPERWALL
int	PETSKILL_MagicStatusChange_Battle( int battleindex, int attackNo, int toNo, int marray)
{
	char *magicarg;
	int status = -1, i, turn = 3, nums=0;
	int ReceveEffect, charaindex=-1;
	char buf1[256];
	charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	if( !CHAR_CHECKINDEX( charaindex)) return FALSE;
	magicarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	if( getStringFromIndexWithDelim( magicarg,"|",1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	for( i=0; i<MAXSTATUSTYPE; i++)	{
		if( strcmp( buf1, MagicStatus[i])) continue;
		status = i;
		break;
	}
	if( i == MAXSTATUSTYPE ) return FALSE;
	if( getStringFromIndexWithDelim( magicarg,"|",2, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	turn = atoi( buf1);
	if( getStringFromIndexWithDelim( magicarg,"|",3, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	nums = atoi( buf1);
	if( getStringFromIndexWithDelim( magicarg,"|",4, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	if( strstr( buf1, "单") != 0 )	{
		if( toNo >= 20 )
			return FALSE;
	}
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	//attackNo = BATTLE_Index2No( battleindex, charaindex );
/*  //动画图号决定
	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;
	}else{
		ReceveEffect = SPR_hoshi;
	}
*/
	ReceveEffect = SPR_hoshi;
	BATTLE_MultiMagicStatusChange(
		battleindex, attackNo, toNo, status,
		turn, MAGIC_EFFECT_USER, ReceveEffect, nums );

	return TRUE;
}
#endif

#ifdef _PETSKILL_SETDUCK
int	PETSKILL_SetDuckChange_Battle( int battleindex, int attackNo, int toNo, int marray)
{
	char *skillarg=NULL;
	int turn = 3, nums=0;
	int charaindex=-1;
	char buf1[256];

	charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	if( !CHAR_CHECKINDEX( charaindex))	return FALSE;
	if( BATTLE_No2Index( battleindex, toNo ) != charaindex ) return FALSE;

	skillarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	if( skillarg == "\0" ) return FALSE;
	if( getStringFromIndexWithDelim( skillarg,"|",1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	turn = atoi( buf1);
	if( getStringFromIndexWithDelim( skillarg,"|",2, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	nums = atoi( buf1);

	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	{
		int ToList[SIDE_OFFSET*2+1];
		char szBuffer[256]="";
		memset( szBuffer, 0, sizeof( szBuffer));
		memset( ToList, -1, sizeof( ToList));
		BATTLE_MultiList( battleindex, toNo, ToList );
		BATTLE_MagicEffect( battleindex, attackNo, ToList, MAGIC_EFFECT_USER, SPR_hoshi );
		if( CHAR_getWorkInt( charaindex, CHAR_MYSKILLDUCK) > 0){
		}else	{
			CHAR_setWorkInt( charaindex, CHAR_MYSKILLDUCK, turn);
			CHAR_setWorkInt( charaindex, CHAR_MYSKILLDUCKPOWER, nums );
			sprintf( szBuffer, "BD|r%X|0|3|%X|", toNo, nums);
			BATTLESTR_ADD( szBuffer );
		}
	}
	return TRUE;
}
#endif

#ifdef _MAGICPET_SKILL
int PETSKILL_SetMagicPet_Battle( int battleindex, int attackNo, int toNo, int marray)
{
	char *skillarg=NULL;
	int turn = 3, nums=0;
	int charaindex=-1;
	char buf1[256];

	charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	if( !CHAR_CHECKINDEX( charaindex))	return FALSE;
	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return FALSE;

	skillarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	if( skillarg == "\0" ) return FALSE;
	if( getStringFromIndexWithDelim( skillarg,"|",1, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	turn = atoi( buf1);
	if( getStringFromIndexWithDelim( skillarg,"|",2, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	nums = atoi( buf1);
	if( getStringFromIndexWithDelim( skillarg,"|",3, buf1, sizeof( buf1)) == FALSE )
		return FALSE;
	if( strstr( buf1, "HP") != 0 )	{
		BATTLE_MultiRecovery( battleindex, attackNo, toNo,
			BD_KIND_HP, nums,  0, MAGIC_EFFECT_USER, SPR_heal3 );
	}else{
		int ToList[SIDE_OFFSET*2+1];
		char szBuffer[256]="";
		int todef=0, i;	

		memset( szBuffer, 0, sizeof( szBuffer));
		memset( ToList, -1, sizeof( ToList));
		BATTLE_MultiList( battleindex, toNo, ToList );
		BATTLE_MagicEffect( battleindex, attackNo, ToList, MAGIC_EFFECT_USER, SPR_hoshi );

		for( i = 0; ToList[i] != -1; i ++ ){
			int bid;
			int toindex = BATTLE_No2Index( battleindex, ToList[i] );
			if( !CHAR_CHECKINDEX( toindex ))continue;

			if( CHAR_getWorkInt( toindex, CHAR_MYSKILLDUCK) > 0 || 
				CHAR_getWorkInt( toindex, CHAR_MYSKILLSTR) > 0 ||
				CHAR_getWorkInt( toindex, CHAR_MYSKILLTGH) > 0 ||
				CHAR_getWorkInt( toindex, CHAR_MYSKILLDEX) > 0 ) continue;

			if( strstr( buf1, "STR") != 0 )	{
				CHAR_setWorkInt( toindex, CHAR_MYSKILLSTR, turn);
				CHAR_setWorkInt( toindex, CHAR_MYSKILLSTRPOWER, nums );
				todef = 4;
			}else if( strstr( buf1, "TGH") != 0 ){
				CHAR_setWorkInt( toindex, CHAR_MYSKILLTGH, turn);
				CHAR_setWorkInt( toindex, CHAR_MYSKILLTGHPOWER, nums );
				todef = 5;
			}else if( strstr( buf1, "DEX") != 0 ){
				CHAR_setWorkInt( toindex, CHAR_MYSKILLDEX, turn);
				CHAR_setWorkInt( toindex, CHAR_MYSKILLDEXPOWER, nums );
				todef = 6;
			}
			bid = BATTLE_Index2No( battleindex, toindex );
			sprintf( szBuffer, "BD|r%X|0|%d|%X|", bid, todef, nums);
			BATTLESTR_ADD( szBuffer );
		}
	}
	return TRUE;
}
#endif

#ifdef _SKILL_WEAKEN  //vincent宠技:虚弱   
int BATTLE_S_Weaken(
	int battleindex, 
	int attackNo, 	
	int defNo,
	int marray
)
{ 
	BOOL iRet = FALSE;
	char *magicarg;
	int status = -1, i,  turn = 3;
	int  ReceveEffect, Success = 0;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
    magicarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	if( magicarg == "\0" )	{
		//print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return FALSE;

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}

	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;
	}else{
		ReceveEffect = SPR_hoshi;
	}
    BATTLE_MultiParamChangeTurn( battleindex, attackNo, defNo, status, 
							MAGIC_EFFECT_USER, ReceveEffect, turn, Success);

	return iRet;
}
#endif

#ifdef _PETSKILL_TEMPTATION
BOOL BATTLE_S_Temptation(	int battleindex,int attackNo,int defNo,	int skill )
{
	BOOL iRet = FALSE;
	char *magicarg;
	char szWork[256];
	int  attackindex,defindex,defside,flg=0,img2,img1;
	int  target;

	magicarg = PETSKILL_getChar( skill , PETSKILL_OPTION );
	if( magicarg == "\0" )	{
		//print("\n magicarg == NULL ");
		return iRet;
	}
	attackindex = BATTLE_No2Index( battleindex, attackNo );	
	defindex	= BATTLE_No2Index( battleindex, defNo );
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )return iRet;
	if( CHAR_CHECKINDEX( defindex ) == FALSE )return iRet;
	if( CHAR_getInt( defindex, CHAR_WHICHTYPE) != CHAR_TYPEPET ) return iRet;
	if( CHAR_getWorkInt( defindex , CHAR_WORKFIXAI ) <= atoi(magicarg)){
		//技能攻击
		CHAR_setWorkInt( defindex, CHAR_WORKTEMPTATION , 1 ); //用来解除打自己方1滴血的限制
		defside = CHAR_getWorkInt( defindex, CHAR_WORKBATTLESIDE);
		target = CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM2 );

		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_ATTACK ){
			CHAR_setWorkInt( defindex , CHAR_WORKBATTLECOM2,
				BATTLE_DefaultAttacker(battleindex,defside));		
		}else{
			CHAR_setWorkInt( defindex , CHAR_WORKBATTLECOM2,
				BATTLE_DefaultAttacker(battleindex,defside));		
		}
		img1 = 101505;	//珠珠动画101637; //准备期动画
		img2 = 101502;	//珠珠动画101645; //击中後动画
		flg |= BCF_NO_DAMAGE;
		flg |= BCF_B_SKILLACT; //击中後秀图 
		snprintf( szWork, sizeof( szWork ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|FF|",
							attackNo, defNo, flg, 0, 0, 1 , img1 , img2 );
		BATTLESTR_ADD( szWork );
		iRet = TRUE;
	}
	return iRet;
	
}
#endif

#ifdef _SKILL_DEEPPOISON  //vincent宠技:剧毒  
int BATTLE_S_Deeppoison( int battleindex, int attackNo, int defNo, int marray )
{   
	BOOL iRet = FALSE;
	char *magicarg;
	int status = -1, i,  turn = 3;
	int  ReceveEffect, Success = 0;
	char *pszP;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
    magicarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	if( magicarg == "\0" )	{
		//print("\n magicarg == NULL ");
		return FALSE;
	}
	pszP = magicarg;

	for( ;status == -1 && pszP[0] != 0; pszP++ ){
		for( i = 1; i < BATTLE_ST_END; i ++ ){
			if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
				status = i;
				pszP +=2;
				break;
			}
		}
	}

	if( status == -1 ) return FALSE;

	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}

	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}

	if( status == BATTLE_ST_NONE ){
		ReceveEffect = SPR_tyusya;
	}else{
		ReceveEffect = SPR_hoshi;
	}

	BATTLE_MultiStatusChange( battleindex, attackNo, defNo,
		status, turn+2, MAGIC_EFFECT_USER, ReceveEffect, Success );

	return iRet;
}
#endif

#ifdef _SKILL_BARRIER  //vincent宠技:魔障      
int BATTLE_S_Barrier(
	int battleindex, 
	int attackNo, 	
	int defNo ,
	int marray
)
{   
	BOOL iRet = FALSE;
	int battlemode;
	int i,turn,perStatus,charaindex,toindex;
	int ToList[SIDE_OFFSET*2+1];
	char *magicarg;
	int Success = 0;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
	char *pszP;
    charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	//check index
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

    magicarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	if( magicarg == "\0" )	{
		//print("\n magicarg == NULL ");
		return FALSE;
	}

	pszP = magicarg;

    //读取作用回合数
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	//命中率
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}

	BATTLE_MultiList( battleindex, defNo, ToList );
	
	//施魔法动画效果
	BATTLE_MagicEffect(battleindex, attackNo, ToList, MAGIC_EFFECT_USER, SPR_hoshi);

	//check是否战斗中
	if( IsBATTLING( charaindex ) == TRUE )
	{

		 for( i = 0; ToList[i] != -1; i ++ )
		{
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		    if( BATTLE_StatusAttackCheck( charaindex,toindex, BATTLE_ST_BARRIER, Success, 30, 1.0, &perStatus ) == TRUE )
			{
		       CHAR_setWorkInt( toindex, CHAR_WORKBARRIER, turn+1);
			}
		}
		
	}
	return iRet;
}
#endif

#ifdef _SKILL_NOCAST  //vincent宠技:沉默  
int BATTLE_S_Nocast(
	int battleindex, 
	int attackNo, 	
	int defNo ,
	int marray
)
{   
	BOOL iRet = FALSE;
	int battlemode;
	int i,turn,perStatus,charaindex,toindex;
	int ToList[SIDE_OFFSET*2+1];
	char *magicarg;
	int Success = 0;
	char szTurn[] = "turn";
	char szSuccess[] = "成";
	char *pszP;
    charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	//check index
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return FALSE;

	battlemode = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE );

	if( battlemode == BATTLE_CHARMODE_INIT ){
		return FALSE;
	}

    magicarg = PETSKILL_getChar( marray, PETSKILL_OPTION );
	if( magicarg == "\0" )	{
		//print("\n magicarg == NULL ");
		return FALSE;
	}

	pszP = magicarg;

    //读取作用回合数
	if( ( pszP = strstr( pszP, szTurn ) ) != NULL){
		pszP += sizeof( szTurn );
		sscanf( pszP, "%d", &turn );
	}
	//命中率
	if( ( pszP = strstr( pszP, szSuccess ) ) != NULL){
		pszP += sizeof( szSuccess );
		sscanf( pszP, "%d", &Success );
	}

	BATTLE_MultiList( battleindex, defNo, ToList );
	
	//施魔法动画效果
	BATTLE_MagicEffect(battleindex, attackNo, ToList, MAGIC_EFFECT_USER, SPR_hoshi);

	//check是否战斗中
	if( IsBATTLING( charaindex ) == TRUE ){

		 for( i = 0; ToList[i] != -1; i ++ ){
		toindex = BATTLE_No2Index( battleindex, ToList[i] );
		    if( BATTLE_StatusAttackCheck( charaindex,toindex, BATTLE_ST_NOCAST, Success, 30, 1.0, &perStatus ) == TRUE 
				&& CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET){
			   //无法展开咒术页
	           lssproto_NC_send( getfdFromCharaIndex( toindex ), 1);
		       CHAR_setWorkInt( toindex, CHAR_WORKNOCAST, turn );
		       BATTLE_BadStatusString( ToList[i], BATTLE_ST_NOCAST  );
			}
		}
		
	}
	return iRet;
}
#endif

int BATTLE_getReactFlg( int index, int react)
{
	int flg=0;
	if( react == BATTLE_MD_ABSROB )flg |= BCF_ABSORB;
	if( react == BATTLE_MD_REFLEC )flg |= BCF_REFRECT;
	if( react == BATTLE_MD_VANISH )flg |= BCF_VANISH;
#ifdef  _MAGIC_SUPERWALL
	if( CHAR_getWorkInt( index, CHAR_MAGICSUPERWALL ) > 0 ){
		flg |= BCF_SUPERWALL;
	}
#endif
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
	if( react == BATTLE_MD_TRAP )	flg |= BCF_TRAP;
#endif
#ifdef _PETSKILL_ACUPUNCTURE 
	if( react == BATTLE_MD_ACUPUNCTURE )  flg |= BCF_ACUPUNCTURE;
#endif
	return flg;
}

#ifdef _PREVENT_TEAMATTACK
int BATTLE_CheckSameSide( int charaindex, int toNo)
{
	int ToList[SIDE_OFFSET*2+1];
	int toindex, i, battleindex;
	int MySide;
	battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	MySide = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLESIDE);

//toindex = BATTLE_No2Index( battleindex, ToList[i] );
	if( toNo < 20 ){//单一目标
		toindex = BATTLE_No2Index( battleindex, toNo);
		if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLESIDE) == MySide ) return 1;//同边
	}else if( toNo){

		BATTLE_MultiList( battleindex, toNo, ToList );
		for( i = 0; ToList[i] != -1; i ++ ){
			toindex = BATTLE_No2Index( battleindex, ToList[i] );
			if( !CHAR_CHECKINDEX( toindex) ) continue;

			if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLESIDE) == MySide )	return 1;//同边
		}
	}
	return 0;
}
#endif


#ifdef _USER_CHARLOOPS
int CHAR_BattleStayLoop( int charaindex)
{
#ifdef _OFFLINE_SYSTEM 
	if(CHAR_getWorkInt( charaindex, CHAR_WORK_OFFLINE ) == 1){
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE){
			CHAR_setWorkChar( charaindex, CHAR_WORKWALKARRAY,"");
			CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, getOfflineBattleTime());
			BATTLE_CreateVsEnemy( charaindex,0, -1);
		}
		return 1;
	}
#endif
	int fd = getfdFromCharaIndex( charaindex);
	if( getStayEncount( fd) ) {
		if( CONNECT_getBDTime( fd) < time( NULL) )	{
			if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE){
				if(getNoStayBattle()==1){
					CHAR_talkToCli(charaindex, -1, "本线为商业娱乐线路，原地遇敌异常缓慢，请到其他线路尝试。", CHAR_COLORYELLOW);
				}
				lssproto_EN_recv( fd, CHAR_getInt( charaindex,CHAR_X), CHAR_getInt( charaindex,CHAR_Y));
				CONNECT_setBDTime( fd, (int)time( NULL));
			}
		}
	}else{
		Char 	*ch;
		ch  = CHAR_getCharPointer( charaindex);
		if( ch == NULL ) return 0;
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
			sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "");//战斗
		CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 0);
		CHAR_constructFunctable( charaindex);
	}

	return 1;
}
#endif

#ifdef _PETSKILL_PROPERTY
int PET_PetskillPropertyEvent( int Myindex, int defindex, int *damage, int *T_Pow, int size)
{
	int My_Pow[5]={0,0,0,0,0};
	int i, totals=100;
	if( !CHAR_CHECKINDEX( Myindex)) return 0;
	if( !CHAR_CHECKINDEX( defindex)) return 0;
	BATTLE_GetAttr( defindex, My_Pow);

	//被攻击方转变属性
	for( i=0; i<4; i++)	{
		int nums = (i+3)%4;
		if( My_Pow[i] != 0 ){
			T_Pow[ nums] = My_Pow[i];
			totals -= My_Pow[i];
		}else{
			T_Pow[ nums] = 0;
		}
	}
	T_Pow[ 4] = totals;
	return 1;
}
#endif



#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能

// 直接攻击函式
int battle_profession_attack_fun(int battleindex, int attackNo, int defNo, int charaindex)
{
	char szBuffer[512]="";
	char szCommand[1024];
	int attackindex, defindex=-1;
	int damage=0, petdamage=0, i=0, iWork, Guardian=-1,react;
	int flg = 0, DefSide=0,ultimate=0;
	BOOL iRet = FALSE;
	int ReactType=0, skill_type=-1, skill=-1;
	char *pszP = NULL;
	char *pszOption = NULL;
	int effect=0, img1=0, img2=0, atk_num=0, skill_level;	

	skill = CHAR_GETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3);
	skill_type=CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1 );

	//攻方index
	attackindex = BATTLE_No2Index( battleindex, attackNo );		
	if( CHAR_CHECKINDEX( attackindex ) == FALSE )	return iRet;

	//守方index
	defindex = BATTLE_No2Index( battleindex, defNo );
	if( CHAR_CHECKINDEX( defindex ) == FALSE ){
		return iRet;	
	}

	// 屍体掠夺
	if( (CHAR_getInt( defindex, CHAR_HP ) <= 0) && (skill_type != BATTLE_COM_S_PLUNDER) ){
		return iRet;
	}
	
	// 非战斗使用技能
	if( !PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_USE_FLAG) ) 	return iRet;

	// 取得技能参数
	pszOption = PROFESSION_SKILL_getChar( skill, PROFESSION_SKILL_OPTION);
	if( pszOption == "\0" ) return iRet;

	// 技能等级
	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

	// 动画方式 0原地秀动画 1人物移动秀动画
	if( (pszP = strstr( pszOption, "效%" ) ) != NULL )	sscanf( pszP+3, "%d", &effect );

	// 准备期的动画
	img1 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_1 );

	// 击中後的动画
	img2 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_2 );

	// 目标地球一周，不动作
	if( defNo >= 0 && defNo <= 19 ){
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_EARTHROUND0 ){
			return iRet;
		}
	}

	// 取出各技能参数
	switch( skill_type){		
		case BATTLE_COM_S_BRUST:		// 爆击
			{	
				int str=0, old_str=0;
				
				// 增加攻击力
				old_str = CHAR_getWorkInt( charaindex, CHAR_WORKFIXSTR);
				str =  old_str * ( skill_level * 3 + 100 ) / 100;
			
				CHAR_setWorkInt( charaindex, CHAR_WORKFIXSTR, str );

				break;
			}
		case BATTLE_COM_S_CHAOS:		// 混乱攻击
			{
				int atk = 0;
				// 减30%攻击力
				atk = CHAR_getWorkInt( charaindex, CHAR_WORKATTACKPOWER );
				atk = atk * 70 / 100;
				CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER, atk );

				if( skill_level >= 10 ) 		atk_num = 5;
				else if( skill_level >= 5 ) 	atk_num = 4;
				else							atk_num = 3;

				break;
			}
		case BATTLE_COM_S_CHAIN_ATK:	// 连环攻击
			{
				int rand_num = RAND(1, 100);
				int hit; 
				
				if( skill_level % 10 != 0 )
				skill_level += 1;
				hit = skill_level * 5 + 15;

				if( rand_num <= hit ) 									
					atk_num=2;	// 攻击二次
				else 
					atk_num=0;

				break;
			}
		case BATTLE_COM_S_ENRAGE_PET:	// 激怒宠物
			{
				// 判断是否同队
				if( BATTLE_CheckSameSide( charaindex, defNo) != 1 )		return iRet;
				
				defNo = attackNo;

				// 攻击力 1
				CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER, 0 );

				break;
			}
		case BATTLE_COM_S_CHAIN_ATK_2:	// 双重攻击
			{
				int atkpower = 0;
				int absrob_num  = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEABSROB );	// 光
				//int reflec_num  = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEREFLEC );	// 镜
				int evanish_num = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEVANISH );	// 守
				int trap_num	= CHAR_getWorkInt( defindex, CHAR_WORKTRAP );			// 陷阱					

				if( absrob_num > 0 )	CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEABSROB, max(absrob_num - 1, 0)  );
				//if( reflec_num > 0 )	CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEREFLEC, max(reflec_num - 1, 0)  );//让镜有效,这行要拿掉
				if( evanish_num > 0 )	CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEVANISH, max(evanish_num - 1, 0) );
				if( trap_num > 0 )		CHAR_setWorkInt( defindex, CHAR_WORKTRAP,		  0   );

				// 第一下攻击力 0
				CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER, 0 );

				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|",
									attackNo, defNo, BCF_NO_DAMAGE, 0, 0, effect, img1, img2 );
				BATTLESTR_ADD( szCommand );

				// 增加攻击力
				atkpower =(int)( CHAR_getWorkInt(charaindex,CHAR_WORKFIXSTR) * (skill_level * 2 + 100) /100 );

				CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER, atkpower );

				if( (CHAR_getInt( charaindex, CHAR_HP ) > 0 ) &&
					(CHAR_getInt( defindex, CHAR_HP )   > 0 ) ){
					BATTLE_Attack( battleindex, attackNo, defNo );
				}

				snprintf( szCommand, sizeof( szCommand ), "FF|");		
				BATTLESTR_ADD( szCommand );

				return iRet;
			}
		case BATTLE_COM_S_DEAD_ATTACK:	// 濒死攻击
			{
				int hit=0, hp=0, old_hp=0, rate=0;

				rate = skill_level * 2 + 10 ;					
				old_hp = CHAR_getInt(charaindex, CHAR_HP);
				hp = old_hp * rate / 100;

				if( old_hp <= 10 ){
					CHAR_talkToCli( charaindex, -1, "HP不足，无法使用技能", CHAR_COLORYELLOW);
					return iRet;
				}

				hit = skill_level * 2 + 80;

				CHAR_setWorkInt(charaindex,CHAR_WORKHITRIGHT, CHAR_getWorkInt(charaindex,CHAR_WORKHITRIGHT) + hit );
				CHAR_setWorkInt( charaindex, CHAR_MYSKILLHIT, 1);
				CHAR_setWorkInt( charaindex, CHAR_MYSKILLHIT_NUM, hit);
				CHAR_talkToCli( charaindex, -1, "命中率上升", CHAR_COLORYELLOW);
				
				sprintf( szCommand, "BD|r%X|0|1|%X|", attackNo, (-1) * (old_hp - hp) );
				BATTLESTR_ADD( szCommand );			

				CHAR_setInt(charaindex, CHAR_HP, hp);
				break;
			}
		case BATTLE_COM_S_ATTACK_WEAK:		// 弱点攻击
			{
				float str=0.0, dex=0.0;
				int whichtype;

				whichtype = CHAR_getInt( defindex, CHAR_WHICHTYPE);
	
				// 目标为宠物时升攻
				if( (whichtype == CHAR_TYPEPET) || (whichtype == CHAR_TYPEENEMY) ){
					str = skill_level * 2 +10 + 100;
					CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER, CHAR_getWorkInt(charaindex,CHAR_WORKATTACKPOWER) * (str/100) );
				}

				// 降敏
				dex = 100 - ( skill_level + 10 );
				CHAR_setWorkInt(charaindex,CHAR_WORKQUICK, CHAR_getWorkInt(charaindex,CHAR_WORKFIXDEX) * (dex/100) );
				sprintf( szCommand, "BD|r%X|0|6|%X|", attackNo, (-1) * (skill_level + 10) );
				BATTLESTR_ADD( szCommand );

				break;
			}
		case BATTLE_COM_S_PLUNDER:			// 屍体掠夺
			{	
				char pBidList[BATTLE_ENTRY_MAX*2+1];
				int item, start = 0;
				memset( pBidList, -1, sizeof(pBidList) );
				pBidList[0] = defNo; 
				
				// 掠夺的目标
				defNo = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM2 );
				defindex = BATTLE_No2Index( battleindex, defNo );
				if( CHAR_getInt( defindex, CHAR_WHICHTYPE) != CHAR_TYPEENEMY ) return 1;

				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|FF|",
									attackNo, defNo, flg, 0, 0, effect, img1, img2 );
				BATTLESTR_ADD( szCommand );

				if( defNo < 10 ) start = 0;
				else			 start = 10;

				for( i = start; i < start+10; i++ ){
					for(item=CHAR_STARTITEMARRAY; item < CHAR_STARTITEMARRAY+10; item++){
						int enemy_index = -1;
						int itemindex = -1;
					
						enemy_index = BATTLE_No2Index( battleindex, i );
						if( !CHAR_CHECKINDEX(enemy_index) ) break;

						itemindex = CHAR_getItemIndex( enemy_index, item );

						if(ITEM_CHECKINDEX( itemindex ) == TRUE ){
							char msg[64], *item_name;

							CHAR_AddPileItem( charaindex, itemindex);

							item_name = ITEM_getChar( itemindex, ITEM_NAME);						

							sprintf( msg, "得到%s", item_name );
							CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);

							CHAR_setItemIndex( enemy_index, item, -1 );
							
							BATTLE_Exit( defindex, battleindex );

							return 1;
						}
					}
				}
				
				BATTLE_Exit( defindex, battleindex );

				return 1;
			}
		default:
			break;
	}


	react = 0;
	if( (ReactType=BATTLE_GetDamageReact( defindex)) > 0 )	{	//有光镜守
		react = ReactType;	
		if( skill_type != BATTLE_COM_S_CHAIN_ATK ){		
			react=0;
//			skill_type=-1;
		}
	}


	if( defNo >= SIDE_OFFSET ){
		i = defNo - SIDE_OFFSET;
		DefSide = 1;
	}

	//取得攻击後状态
	iWork = BATTLE_AttackSeq( attackindex, defindex, &damage, &Guardian, skill_type );
	if( skill_type == BATTLE_COM_S_ENRAGE_PET ){//让激怒宠物技能不要真的打死宠物
		if( CHAR_getInt( defindex, CHAR_HP ) <= damage ){
			damage = 0;
		}
	}
	if( skill_type == BATTLE_COM_S_CAVALRY)		// 座骑攻击
		ultimate = BATTLE_PROFESSION_ATK_PET_DamageSub( attackindex, defindex, &damage, &petdamage, &react, skill_level );
	else
		ultimate = BATTLE_DamageSub( attackindex, defindex, &damage, &petdamage, &react);

	if( react == BATTLE_MD_REFLEC )		defindex = attackindex;
	if( react == BATTLE_MD_TRAP )		defindex = attackindex;
#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ) defindex = attackindex;
#endif
	if( damage > 0 && ( react != BATTLE_MD_ABSROB )&& ( react != BATTLE_MD_VANISH )){
		BATTLE_DamageWakeUp( battleindex, defindex );
	}

    iRet = BATTLE_DefineAttack( attackindex, defindex, iWork, szBuffer, damage, react, &flg);

	// 将 NORMAL 及 CRITICAL 的 flg 改成 职业的flg
	if( img2 != 0  ){
		if( flg == BCF_NORMAL || flg == BCF_KAISHIN ){
			if( (pszP = strstr( pszOption, "前" ) ) != NULL )
				flg |= BCF_F_SKILLACT; //击中前秀图
			else
				flg |= BCF_B_SKILLACT; //击中後秀
		}
	}

	if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ){
		iRet = BATTLE_DefDieType( defindex, iRet, &ultimate, &flg, szBuffer);
	}

	if( damage > 0 ){
		if( BATTLE_ItemCrushSeq( attackindex, defindex, damage) == TRUE )
			flg |= BCF_CRUSH;
	}


	memset( szCommand, 0, sizeof( szCommand));
	switch( skill_type){
		case BATTLE_COM_S_BRUST:		// 爆击
		case BATTLE_COM_S_DEAD_ATTACK:	// 濒死攻击
		case BATTLE_COM_S_ATTACK_WEAK:	// 弱点攻击
		case BATTLE_COM_S_CAVALRY:		// 座骑攻击
			{
				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|FF|",
									attackNo, defNo, flg, damage, petdamage, effect, img1, img2 );
				BATTLESTR_ADD( szCommand );
				break;
			}
		case BATTLE_COM_S_ENRAGE_PET:	// 激怒宠物
			{
				int str = 0;
				int turn = 0;
				int pindex = -1;
			

				flg |= BCF_NO_DAMAGE;
				flg |= BCF_B_SKILLACT; //击中後秀图	

				defNo = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM2 );	

				pindex = BATTLE_No2Index( battleindex, defNo );
			
				// 检查是否为宠物
				if( CHAR_getInt( pindex, CHAR_WHICHTYPE) != CHAR_TYPEPET ) break;

				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|FF|",
									attackNo, defNo, flg, 0, 0, effect, img1, img2 );
				BATTLESTR_ADD( szCommand );

				// 加攻
				str = skill_level * 2 + 10;
				sprintf( szCommand, "BD|r%X|0|4|%X|", defNo, str);
				BATTLESTR_ADD( szCommand );
				CHAR_setWorkInt( pindex, CHAR_MYSKILLSTRPOWER, str);		

				if( skill_level >= 10 )		turn=5;
				else if( skill_level >= 5 )	turn=4;
				else						turn=3;

				CHAR_setWorkInt( pindex, CHAR_MYSKILLSTR, turn );

				break;
			}
		case BATTLE_COM_S_CHAOS:		// 混乱攻击
			{
				int i=0, j=0, k=0, f_num=0;
				int temp[10], chose_temp[10];
				int defNo_index  = BATTLE_No2Index( battleindex, defNo );

				memset( temp, -1, sizeof( temp ) );
				memset( chose_temp, -1, sizeof( chose_temp ) );

				if( CHAR_getWorkInt( defNo_index, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_EARTHROUND0 ){
					break;
				}

				if( defNo >= 10 )	f_num = 10;
				else f_num = 0;	

				// 减30%攻击力
				//CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER, CHAR_getWorkInt( charaindex, CHAR_WORKATTACKPOWER ) * 70 / 100 );

				//传给client减攻击力效果
				sprintf( szCommand, "BD|r%X|0|4|%X|", attackNo, -30 );
				BATTLESTR_ADD( szCommand );

				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|",
									attackNo, defNo, flg, damage, petdamage, effect, img1, img2 );
				BATTLESTR_ADD( szCommand );

				// 取出战场上存活的人
				for( i=f_num; i<f_num+10; i++ ){
					if(BATTLE_TargetCheck(battleindex, i) != FALSE){
						if( CHAR_getWorkInt( BATTLE_No2Index( battleindex, i ), CHAR_WHICHTYPE) == CHAR_TYPEPET 
#ifdef _PROFESSION_ADDSKILL
							&& !BATTLE_BattleUltimate(battleindex, i-5 ) ){
#else
							){
#endif
							temp[j] = i;
							j++;
						}
						else{
							temp[j] = i;
							j++;
						}
					}
				}
	
				if( j == 0 ){
					snprintf( szCommand, sizeof( szCommand ), "FF|");		
					BATTLESTR_ADD( szCommand );
					break;
				}

				// 乱数取攻击的目标
				for( i=0; i<atk_num-1; i++){
					chose_temp[i] = temp[ RAND(0,j-1)];
				}

				k=0;
				j=0;
				// 攻击中选的人
				while( atk_num-1 > 0 ){
					int atk_to_index = -1;
					atk_to_index = BATTLE_No2Index( battleindex, chose_temp[k] );

					if( ( atk_to_index >= 0 ) &&
						( BATTLE_TargetCheck(battleindex, chose_temp[k] ) != FALSE ) &&
						( CHAR_getWorkInt( atk_to_index, CHAR_WORKBATTLECOM1 ) != BATTLE_COM_S_EARTHROUND0 )
					){
						BATTLE_Attack( battleindex, attackNo, chose_temp[k] );
						k++;
						atk_num --;
					}else{

						memset( temp, -1, sizeof( temp ) );
						memset( chose_temp, -1, sizeof( chose_temp ) );
						j = 0; k = 0;

						// 取出战场上存活的人
						for( i=f_num; i<f_num+10; i++ ){
							if(BATTLE_TargetCheck(battleindex, i) != FALSE){
								if( CHAR_getWorkInt( BATTLE_No2Index( battleindex, i ), CHAR_WHICHTYPE) == CHAR_TYPEPET 
#ifdef _PROFESSION_ADDSKILL				
									&& !BATTLE_BattleUltimate(battleindex, i-5 ) ){
#else
								   ){	
#endif
									temp[j] = i;
									j++;
								}
								else{
									temp[j] = i;
									j++;
								}
							}
						}
						if( j == 0 ) break;

						// 乱数取攻击的目标
						for( i=0; i<atk_num-1; i++){
							chose_temp[i] = temp[ RAND(0,j-1)];
						}

						k=0;
					}
				}

				snprintf( szCommand, sizeof( szCommand ), "FF|");		
				BATTLESTR_ADD( szCommand );

				break;
			}
		case BATTLE_COM_S_CHAIN_ATK:	// 连环攻击
			{
				snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|",
									attackNo, defNo, flg, damage, petdamage, effect, img1, img2 );

				BATTLESTR_ADD( szCommand );

				if( atk_num == 2 &&
				    (CHAR_getInt( charaindex, CHAR_HP ) > 0 ) &&
					(CHAR_getInt( defindex, CHAR_HP )   > 0 ) ){
						BATTLE_Attack( battleindex, attackNo, defNo );
				}

				snprintf( szCommand, sizeof( szCommand ), "FF|");		
				BATTLESTR_ADD( szCommand );

				break;
			}
    }

	//BATTLE_BroadCast( battleindex, szBuffer,
	//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;

	i = defNo-DefSide*SIDE_OFFSET;
	if( ultimate > 0 ){
		BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
	}
	return iRet;
}


// 魔法攻击函式
int battle_profession_attack_magic_fun(int battleindex, int attackNo, int defNo, int charaindex)
{
	int attr=-1, attIdx=0, skill=-1, toindex;

	// 取技能ID
	skill = CHAR_GETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3);

	// 攻击目标
	// Robin fix
 	//toindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM2 );
	toindex = defNo;
	

	// 攻击范围 0单体 1单排 2一方全体
	if( toindex < 20 ){					// 单体
		attIdx = 0;
	}else if( toindex>=23 && toindex<=26 ){ // 单排
		attIdx = 1;
	}else if( toindex==20 || toindex==21 ){	// 一方全体
		attIdx = 2;
	}

	// Robin fix 增加战场属性优势
	attr = BattleArray[battleindex].field_att -1;

	PROFESSION_MAGIC_ATTAIC( battleindex, attackNo, toindex, attIdx, attr, skill);

	return TRUE;
}

// 提升自已能力系函式
int battle_profession_assist_fun(int battleindex, int attackNo, int defNo, int charaindex)
{
	int i, skill, skill_type, skill_level, img1, img2, turn=1;
	char *pszP = NULL, *pszOption = NULL, szCommand[256];
	BOOL iRet = FALSE;	
	int ToList[SIDE_OFFSET*2+1];

	skill = CHAR_GETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3);
	skill_type = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1 );

	// 非战斗使用技能
	if( !PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_USE_FLAG) ) 	return iRet;

	// 技能的参数
	pszOption = PROFESSION_SKILL_getChar( skill, PROFESSION_SKILL_OPTION);
	if( pszOption == "\0" ) return iRet;

	// 技能等级
	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);

	// 准备期的动画
	img1 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_1 );

	// 击中後的动画
	img2 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_2 );
	
	// 使用回合数
	if( (pszP = strstr( pszOption, "回%" ) ) != NULL )	sscanf( pszP+3, "%d", &turn );
	if(turn < 1)	turn=1;

	switch(skill_type){
		case BATTLE_COM_S_TRANSPOSE:	// 移形换位
			{
				int defNo2 = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLECOM2);
				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

				memset( ToList, -1, sizeof( ToList));
				BATTLE_MultiList( battleindex, defNo2, ToList );
				BATTLE_MagicEffect( battleindex, attackNo, ToList, img1, img2 );

				for( i = 0; ToList[i] != -1; i ++ ){
					int avoid=0;
					int toindex = BATTLE_No2Index( battleindex, ToList[i] );
					if( !CHAR_CHECKINDEX( toindex ))continue;
					if( charaindex != toindex ) continue;

					// 回避率				
					if( skill_level >= 10 )			avoid = 70;
					else if( skill_level >= 9 )		avoid = 60;
					else if( skill_level >= 8 )		avoid = 50;
					else if( skill_level >= 6 )		avoid = 45;
					else if( skill_level >= 5 )		avoid = 30;
					else if( skill_level >= 3 )		avoid = 25;
					else							avoid = 10;

					if( skill_level >= 10 )		turn = 5;
					else if( skill_level >= 6 ) turn = 4;
					else						turn = 3;

					if( CHAR_getWorkInt( toindex, CHAR_MYSKILLDUCK) <= 0){
						CHAR_setWorkInt( toindex, CHAR_MYSKILLDUCK, turn+1);
						CHAR_setWorkInt( toindex, CHAR_MYSKILLDUCKPOWER, avoid );
						sprintf( szCommand, "BD|r%X|0|3|%X|", ToList[i], avoid);
						BATTLESTR_ADD( szCommand );
						iRet = TRUE;
					}
				}
				break;
			}
		case BATTLE_COM_S_SCAPEGOAT:		// 舍已为友
			{
				int tgh=0, old_tgh=0, flg=0, rate=0;
				int pos=0, side=0, ownerpos=0;
				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

				pos = BATTLE_Index2No( battleindex, charaindex );
				side = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLESIDE );

				if( skill_level >= 10 ){		// 我方全体
					for(i=0; i<10; i++){
						if(pos != i )
							BattleArray[battleindex].Side[side].Entry[i].guardian = pos;
					}
				}else if( skill_level >= 5 ){	// 我方所有宠物
					for(i=5; i<10; i++){
						if(pos != i )
							BattleArray[battleindex].Side[side].Entry[i].guardian = pos;
					}
				}else{							// 人物之宠物
					ownerpos = pos + 5;	
					if( ownerpos >= 10 ) ownerpos -= 10;
					if( ownerpos < 0 || ownerpos > 19 ){
					}else{
						BattleArray[battleindex].Side[side].Entry[ownerpos].guardian = pos;
					}
				}

				rate = skill_level * 2;
		
				tgh = 30 - rate;
				sprintf( szCommand, "BD|r%X|0|5|%X|", attackNo, (-1) * tgh );
				BATTLESTR_ADD( szCommand );

				old_tgh = CHAR_getWorkInt(charaindex,CHAR_WORKFIXTOUGH);
				tgh = (int)( old_tgh * (100-tgh) / 100 );
				CHAR_setWorkInt( charaindex, CHAR_WORKFIXTOUGH, tgh);

				flg = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLG );
				flg |= CHAR_BATTLEFLG_GUARDIAN;
				CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEFLG, flg );

				iRet = TRUE;
			
				break;
			}
		case BATTLE_COM_S_ENRAGE:			// 激化攻击
			{
				int str=0, tgh=0;
				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

				memset( ToList, -1, sizeof( ToList));
				BATTLE_MultiList( battleindex, attackNo, ToList );
				BATTLE_MagicEffect( battleindex, attackNo, ToList, img1, img2 );

				// 减防
				tgh = skill_level * 2 + 10;
				sprintf( szCommand, "BD|r%X|0|5|%X|", attackNo, (-1) * tgh );
				BATTLESTR_ADD( szCommand );

				CHAR_setWorkInt(charaindex,CHAR_MYSKILLTGHPOWER, (-1) * tgh);

				// 加攻
				str = skill_level * 2 + 20;
				sprintf( szCommand, "BD|r%X|0|4|%X|", attackNo, str);
				BATTLESTR_ADD( szCommand );

				CHAR_setWorkInt( charaindex, CHAR_MYSKILLSTRPOWER, str);		

				if( skill_level >= 10 )		turn=5;
				else if( skill_level >= 5 )	turn=4;
				else						turn=3;

				CHAR_setWorkInt( charaindex, CHAR_MYSKILLSTR, turn );
				CHAR_setWorkInt( charaindex, CHAR_MYSKILLTGH, turn );	
				
				iRet = TRUE;
				break;
			}
		case BATTLE_COM_S_COLLECT:			// 能量聚集
			{
				int dex=0, tgh=0;
				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );
			
				memset( ToList, -1, sizeof( ToList));
				BATTLE_MultiList( battleindex, attackNo, ToList );
				BATTLE_MagicEffect( battleindex, attackNo, ToList, img1, img2 );

				// 减敏
				dex = (skill_level * 2) + 10;
				sprintf( szCommand, "BD|r%X|0|6|%X|", attackNo, (-1) * dex );
				BATTLESTR_ADD( szCommand );
				CHAR_setWorkInt(charaindex,CHAR_MYSKILLDEXPOWER, dex);

				// 加防
				tgh = (skill_level * 2) + 20;
				sprintf( szCommand, "BD|r%X|0|5|%X|", attackNo, tgh);
				BATTLESTR_ADD( szCommand );
				CHAR_setWorkInt( charaindex, CHAR_MYSKILLTGHPOWER, tgh);		

				if( skill_level >= 10 )		turn=5;
				else if( skill_level >= 5 )	turn=4;
				else						turn=3;

				CHAR_setWorkInt( charaindex, CHAR_MYSKILLDEX, turn );
				CHAR_setWorkInt( charaindex, CHAR_MYSKILLTGH, turn );
				
				iRet = TRUE;
				break;
			}
		case BATTLE_COM_S_FOCUS:			// 专注战斗
			{
				int ToList[SIDE_OFFSET*2+1];
				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

				memset( ToList, -1, sizeof( ToList));
				BATTLE_MultiList( battleindex, attackNo, ToList );
				BATTLE_MagicEffect( battleindex, attackNo, ToList, img1, img2 );

				CHAR_setWorkInt( charaindex, CHAR_MYSKILLHIT, 2);
				CHAR_setWorkInt( charaindex, CHAR_MYSKILLHIT_NUM, 100);
				
				CHAR_talkToCli( charaindex, -1, "命中率上升", CHAR_COLORYELLOW);		

				iRet = TRUE;
				break;
			}
		case BATTLE_COM_S_TRAP:				// 陷阱			
			{
				int value;
				int ToList[SIDE_OFFSET*2+1];			
				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_M( skill_level );

				memset( ToList, -1, sizeof(ToList) );
				ToList[0] = attackNo;

				value = skill_level * 30 + 100;
				BATTLE_MagicEffect( battleindex, attackNo, ToList, img1, img2 );

				if( skill_level >= 10 )		turn=3;
				else if( skill_level >= 5 )	turn=2;
				else						turn=1;
				
				CHAR_setWorkInt( charaindex, CHAR_WORKTRAP, turn);
				CHAR_setWorkInt( charaindex, CHAR_WORKMODTRAP, value);				
				
				iRet = TRUE;
				break;
			}
		case BATTLE_COM_S_DOCILE:			// 驯伏宠物	
			{
				int rate = 0;
				skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );
				
				rate  = skill_level * 2 + 10;

				BATTLE_MultiCaptureUp( battleindex, attackNo, defNo, rate, img1, img2 );
				
				iRet = TRUE;
				break;
			}
#ifdef _PROFESSION_ADDSKILL
        case BATTLE_COM_S_CALL_NATURE:      // 号召自然
			{
				int ridepet=0,count=0,addhp=0,toindex=-1;
				int ToList[SIDE_OFFSET*2+1];
				BATTLE_MultiList( battleindex, defNo, ToList );
			
                if( defNo == 20 || defNo == 25 || defNo == 26 )//右方
					img1 = 101772;
                for( i = 0; ToList[i] != -1; i ++ ){
		            toindex = BATTLE_No2Index( battleindex, ToList[i] );
             		if( CHAR_CHECKINDEX( toindex ) == FALSE )	continue;
            		ridepet = BATTLE_getRidePet( toindex );
        		    if( ridepet == -1 )
						++count;
					else
						count += 2;
				}
				if( skill_level >= 100 ) addhp = 5000/count;
				else if( skill_level > 95 ) addhp = 4500/count;
				else if( skill_level > 90 ) addhp = 4000/count;
				else if( skill_level > 85 ) addhp = 3500/count;
				else if( skill_level > 80 ) addhp = 3000/count;
				else if( skill_level > 60 ) addhp = 2500/count;
				else if( skill_level > 40 ) addhp = 2000/count;
				else if( skill_level > 20 ) addhp = 1000/count;
                else addhp = 500/count;
                if( addhp <= 100 )
		            img2 = SPR_heal;
		        else if( addhp <= 300 )
		            img2 = SPR_heal2;
	            else
		            img2 = SPR_heal3;
                BATTLE_MagicEffect( battleindex, attackNo, ToList, img1, img2 );

				for( i = 0; ToList[i] != -1; i ++ ){
		            toindex = BATTLE_No2Index( battleindex, ToList[i] );
             		if( CHAR_CHECKINDEX( toindex ) == FALSE )	continue;
            		ridepet = BATTLE_getRidePet( toindex );
					if( ridepet == -1 ){
				        CHAR_setInt( toindex, CHAR_HP,
				            min( addhp+CHAR_getInt( toindex, CHAR_HP ), CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
					}
					else{
                        CHAR_setInt( toindex, CHAR_HP,
				            min( addhp+CHAR_getInt( toindex, CHAR_HP ), CHAR_getWorkInt( toindex, CHAR_WORKMAXHP ) ) );
                        CHAR_setInt( ridepet, CHAR_HP,
				            min( addhp+CHAR_getInt( ridepet, CHAR_HP ), CHAR_getWorkInt( ridepet, CHAR_WORKMAXHP ) ) );
					}
					snprintf( szCommand, sizeof(szCommand),"BD|r%X|%X|%X|d%X|p%X|", ToList[i], BD_KIND_HP, 1, addhp, ridepet?addhp:0 );
				    BATTLESTR_ADD( szCommand );
			

					if( BattleArray[battleindex].norisk == 0
			            &&  CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPET)
					{
			            int flg = CHAR_getWorkInt( toindex, CHAR_WORKBATTLEFLG );
			            if(  flg & CHAR_BATTLEFLG_RECOVERY )
						{}
			            else
						{
				            CHAR_PetAddVariableAi( toindex, AI_FIX_PETRECOVERY );
				            CHAR_setWorkInt( toindex, CHAR_WORKBATTLEFLG, flg | CHAR_BATTLEFLG_RECOVERY );
						}
					}
				}
				
				iRet = TRUE;
				break;
			}
#endif
		default:
			break;
	}


	return iRet;
}

#ifdef _PROFESSION_ADDSKILL
extern unsigned int GET_PROFESSION_magic_array(int idx);
#endif

// 击中改变状态系函式
extern void BATTLE_TargetListSet( int charaindex, int attackNo,	int *pList);
extern int BoomerangVsTbl[4][5];
int battle_profession_status_chang_fun(int battleindex, int attackNo, int defNo, int charaindex)
{
	int i, skill, skill_type, skill_level, img1=0, img2=0, turn=1, j;
	char *pszP = NULL, *pszOption = NULL, szCommand[512];
	float rate=0.0;
	BOOL iRet = FALSE;	
	int defindex;
	int damage=0, petdamage=0, iWork, Guardian=-1,react;
	int flg=0, DefSide=0,ultimate=0;
	int ReactType=0, effect=0;
	
	skill = CHAR_GETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3 );
	skill_type=CHAR_getWorkInt( charaindex, CHAR_WORKBATTLECOM1 );

	// 非战斗使用技能
	if( !PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_USE_FLAG) ) 	return iRet;

	// 技能的参数
	pszOption = PROFESSION_SKILL_getChar( skill, PROFESSION_SKILL_OPTION);
	if( pszOption == "\0" ) return iRet;

	// 技能等级
	skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
	skill_level = PROFESSION_CHANGE_SKILL_LEVEL_A( skill_level );

	// 技能等级 rate
	if( (pszP = strstr( pszOption, "倍%" ) ) != NULL )	sscanf( pszP+3, "%f", &rate );

	// 准备期的动画
	img1 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_1 );

	// 击中後的动画
	img2 = PROFESSION_SKILL_getInt( skill, PROFESSION_SKILL_IMG_2 );

	// 使用回合数
	if( (pszP = strstr( pszOption, "回%" ) ) != NULL )	sscanf( pszP+3, "%d", &turn );
	if(turn < 1)	turn=1;

	// 动画方式 0原地秀动画 1人物移动秀动画
	if( (pszP = strstr( pszOption, "效%" ) ) != NULL )	sscanf( pszP+3, "%d", &effect );
	
	// 目标地球一周，不动作
	if( defNo >= 0 && defNo <= 19 ){
		defindex = BATTLE_No2Index( battleindex, defNo );
		if( CHAR_CHECKINDEX( defindex ) == FALSE )	return iRet;
		if( CHAR_getWorkInt( defindex, CHAR_WORKBATTLECOM1 ) == BATTLE_COM_S_EARTHROUND0 ){
			return iRet;	// Miss
		}
	}

	switch(skill_type){	
		// 不移动型
		case BATTLE_COM_S_ENTWINE:			// 树根缠绕
		case BATTLE_COM_S_DRAGNET:			// 天罗地网
		case BATTLE_COM_S_INSTIGATE:		// 挑拨
		case BATTLE_COM_S_OBLIVION:			// 遗忘
		case BATTLE_COM_S_RESIST_FIRE:		// 火抗性提升
		case BATTLE_COM_S_RESIST_ICE:		// 冰抗性提升
		case BATTLE_COM_S_RESIST_THUNDER:	// 雷抗性提升
#ifdef _PROFESSION_ADDSKILL
		case BATTLE_COM_S_RESIST_F_I_T:     // 自然威能
#endif
		case BATTLE_COM_S_FIRE_ENCLOSE:		// 火附体
		case BATTLE_COM_S_ICE_ENCLOSE:		// 冰附体
		case BATTLE_COM_S_THUNDER_ENCLOSE:	// 雷附体
			{
				int perStatus=0, toindex=-1, status=-1, Success=0, dex=0, old_dex;
				//char szBuffer[256]="";
				int ToList[SIDE_OFFSET*2+1];
				int defNo2 = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLECOM2);
				
				// 使用对象
				if( skill_type == BATTLE_COM_S_RESIST_FIRE ||		// 火抗性提升
					skill_type == BATTLE_COM_S_RESIST_ICE ||		// 冰抗性提升
					skill_type == BATTLE_COM_S_RESIST_THUNDER 	// 雷抗性提升
#ifdef _PROFESSION_ADDSKILL
					|| skill_type == BATTLE_COM_S_RESIST_F_I_T  // 自然威能
#endif
				){
#ifdef _PROFESSION_ADDSKILL
					defNo2 = BATTLE_Index2No( battleindex, charaindex );
#else
					if( defNo2 < 10 ){
						if( skill_level >= 10 )		defNo2 = 20;
						else if( skill_level >= 5 ) defNo2 = 25;						
					}else{
						if( skill_level >= 10 )		defNo2 = 21;
						else if( skill_level >= 5 ) defNo2 = 24;						
					}
#endif
				}

				memset( ToList, -1, sizeof( ToList));
				BATTLE_MultiList( battleindex, defNo2, ToList );

				BATTLE_MagicEffect( battleindex, attackNo, ToList, img1, img2 );

				charaindex = BATTLE_No2Index( battleindex, attackNo );
				if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return iRet;

				// 改变状态
				if( ((pszP = strstr( pszOption, "缠" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "挑" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "忘" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "火" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "冰" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "雷" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "罗" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "击" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "冻" ) ) != NULL ) ||
					((pszP = strstr( pszOption, "炎" ) ) != NULL ) 
#ifdef _PROFESSION_ADDSKILL
					|| ((pszP = strstr( pszOption, "抗" ) ) != NULL )
#endif
					){					
					for( i = 1; i < BATTLE_ST_END; i ++ ){
						if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
							status = i;
							break;
						}
					}	
				}
#ifdef _BATTLE_NO_WANG
				if(CHAR_getInt( defindex, CHAR_PETID) == 1165 || CHAR_getInt( defindex, CHAR_PETID) == 1166 || CHAR_getInt( defindex, CHAR_PETID) == 964)
					{
						Success = 0;
					}
				else
					{
#endif
						// 成功率
						if( (pszP = strstr( pszOption, "成%" ) ) != NULL )	sscanf( pszP+3, "%d", &Success );
						Success = Success + skill_level * 5;
#ifdef _BATTLE_NO_WANG
						}
#endif
				if( skill_type == BATTLE_COM_S_DRAGNET ){
					int DRAGNET=0,dragnet_idx=-1;
					int PLAYER=0;
					//找出攻方是那一边的
					if( attackNo > 9 ){//左边
						for(i=0;i<10;i++){//计算右边有多少人中天罗
							dragnet_idx = BATTLE_No2Index( battleindex, i );
							if( CHAR_CHECKINDEX( dragnet_idx ) ){
								PLAYER++;
								if( CHAR_getWorkInt( dragnet_idx, CHAR_WORKDRAGNET ) > 0 ) {
									++DRAGNET;
								}
							}
						}
					}
					else{//右
						for(i=10;i<20;i++){//计算左边有多少人中天罗
							dragnet_idx = BATTLE_No2Index( battleindex, i );
							if( CHAR_CHECKINDEX( dragnet_idx ) ){
								PLAYER++;
								if( CHAR_getWorkInt( BATTLE_No2Index( battleindex, i ), CHAR_WORKDRAGNET ) > 0 ) {
									++DRAGNET;
								}
							}
						}
					}
					Success *= (float)((PLAYER-DRAGNET)/10.0);
//					if( DRAGNET == 1 ){
//						Success *= 0.64;
//					}else if( DRAGNET > 1 ){
//						Success *= 0.4;
//					}
				}

				perStatus = Success;				

				// 回合数	
				if( (skill_type == BATTLE_COM_S_INSTIGATE) && (skill_level == 10) ){		// 挑拨
					turn = 4;
				}else if( (skill_type == BATTLE_COM_S_RESIST_FIRE)    ||	// 火抗性提升
						  (skill_type == BATTLE_COM_S_RESIST_ICE)     ||	// 冰抗性提升
						  (skill_type == BATTLE_COM_S_RESIST_THUNDER) ){	// 雷抗性提升
					
					if( skill_level >= 10 )		turn = 5;
					else if( skill_level >= 5 ) turn = 4; 
					else						turn = 3;
				}else if( skill_type == BATTLE_COM_S_OBLIVION ){			// 遗忘
					if( skill_level >= 10 ){		turn = 4;
					}else if( skill_level >= 5 ){	turn = 3; 
					}else{							turn = 2;	}
				}
#ifdef _PROFESSION_ADDSKILL
				else if( skill_type == BATTLE_COM_S_RESIST_F_I_T ){     // 自然威能
					skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
					if( skill_level >= 100 )		turn = 5;
					else if( skill_level > 80 )	turn = 4; 
					else turn = 3;
				}
#endif

				switch( skill_type ){
					case BATTLE_COM_S_RESIST_FIRE:
							CHAR_talkToCli( charaindex, -1 , "火抗性提升" , CHAR_COLORYELLOW );	break;
					case BATTLE_COM_S_RESIST_ICE:
							CHAR_talkToCli( charaindex, -1 , "冰抗性提升" , CHAR_COLORYELLOW ); break;
					case BATTLE_COM_S_RESIST_THUNDER:
							CHAR_talkToCli( charaindex, -1 , "雷抗性提升" , CHAR_COLORYELLOW ); break;
#ifdef _PROFESSION_ADDSKILL
                    case BATTLE_COM_S_RESIST_F_I_T:
							CHAR_talkToCli( charaindex, -1 , "火冰雷抗性提升" , CHAR_COLORYELLOW ); break;
#endif
				}		

				for( i = 0; ToList[i] != -1; i ++ ){
					toindex = BATTLE_No2Index( battleindex, ToList[i] );
					if( CHAR_CHECKINDEX( toindex ) == FALSE )	return iRet;

					// 挑拨不可施於玩家人物身上
					if( skill_type == BATTLE_COM_S_INSTIGATE ){
						if( CHAR_getInt( toindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ){
							CHAR_talkToCli( charaindex, -1, "不可施於人物身上", CHAR_COLORYELLOW );
							break;
						}
					}

					// 状态命中检定
					if( (PROFESSION_BATTLE_StatusAttackCheck( charaindex, toindex, status, Success ) == 0 )
						|| (CHAR_getInt( toindex, CHAR_HP ) <= 0 ) ){

						//snprintf( szBuffer, sizeof(szBuffer), "(%s)无法将(%s)做成(%s)(%d%%)",
						//	CHAR_getUseName( charaindex ), CHAR_getUseName( toindex ),
						//	aszStatusFull[status], perStatus );

						//BATTLE_BroadCast( battleindex, szBuffer,
						//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
					}else{
#ifdef _PROFESSION_ADDSKILL
						if( status == BATTLE_ST_RESIST_F_I_T ){//自然威能
						    CHAR_setWorkInt( toindex, StatusTbl[BATTLE_ST_RESIST_F], turn + 1 );
                            CHAR_setWorkInt( toindex, StatusTbl[BATTLE_ST_RESIST_I], turn + 1 );
                            CHAR_setWorkInt( toindex, StatusTbl[BATTLE_ST_RESIST_T], turn + 1 );
						}
						else
#endif
						    CHAR_setWorkInt( toindex, StatusTbl[status], turn + 1 );

						if( status == BATTLE_ST_PARALYSIS
							||  status == BATTLE_ST_SLEEP
							||  status == BATTLE_ST_STONE
							||  status == BATTLE_ST_BARRIER	
							||  status == BATTLE_ST_DIZZY
							||  status == BATTLE_ST_ENTWINE
							||  status == BATTLE_ST_DRAGNET
							||  status == BATTLE_ST_ICECRACK
							||  status == BATTLE_ST_ICEARROW ){
							CHAR_setWorkInt( toindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
						}
						if( status == BATTLE_ST_DRAGNET ){//天罗地网
							CHAR_setWorkInt( toindex, CHAR_DOOMTIME, 0 );//世界末日集气
							CHAR_setWorkInt( toindex, CHAR_WORK_com1, 0 );
							CHAR_setWorkInt( toindex, CHAR_WORK_toNo, 0 );
							CHAR_setWorkInt( toindex, CHAR_WORK_mode, 0 );
							CHAR_setWorkInt( toindex, CHAR_WORK_skill_level, 0 );
							CHAR_setWorkInt( toindex, CHAR_WORK_array, 0 );
						}
						if( skill_type == BATTLE_COM_S_FIRE_ENCLOSE ){			// 火附体
							CHAR_setWorkInt( toindex, CHAR_WORKMOD_F_ENCLOSE_2, skill_level );
							// 加火熟练度经验值
							PROFESSION_SKILL_LVEVEL_UP( charaindex, "PROFESSION_FIRE_PRACTICE" );
						}else if(skill_type == BATTLE_COM_S_ICE_ENCLOSE ){		// 冰附体
							CHAR_setWorkInt( toindex, CHAR_WORKMOD_I_ENCLOSE_2, skill_level );
							// 加冰熟练度经验值
							PROFESSION_SKILL_LVEVEL_UP( charaindex, "PROFESSION_ICE_PRACTICE" );
						}else if(skill_type == BATTLE_COM_S_THUNDER_ENCLOSE ){	// 雷附体
							CHAR_setWorkInt( toindex, CHAR_WORKMOD_T_ENCLOSE_2, skill_level );
							// 加雷熟练度经验值	
							PROFESSION_SKILL_LVEVEL_UP( charaindex, "PROFESSION_THUNDER_PRACTICE" );
						}

						// 遗忘 宠物遗忘技能
						if( skill_type == BATTLE_COM_S_OBLIVION ){
							int toNo = -1, toNoindex = -1, pet_no = -1, num = 0;
							char msg[20];

							memset( msg, -1, sizeof(msg) );

							// 主人index
							toNo = ToList[i] - 5;
							toNoindex = BATTLE_No2Index( battleindex, toNo );
							
							// 宠物编号
							pet_no = CHAR_getInt( toNoindex, CHAR_DEFAULTPET );
							
							// 遗忘宠技数量
							num = skill_level / 2;
							if( num < 1 ) num = 1;
							CHAR_setWorkInt( toindex, CHAR_WORKMODOBLIVION, num );

							// 传给 client 遗忘技能
							if( toNoindex != -1 ){
								sprintf( msg, "W%d", pet_no );
								CHAR_sendStatusString( toNoindex, msg );
								
								sprintf( msg, "Y%d", pet_no );
								CHAR_sendStatusString( toNoindex, msg );
								
							}
						}

						// 挑拨 降属性
						if(skill_type == BATTLE_COM_S_INSTIGATE ){
							int rate=0;
							rate = skill_level * 1 + 10;
							CHAR_setWorkInt( toindex, CHAR_WORKMODINSTIGATE, rate);

							sprintf( szCommand, "BD|r%X|0|4|%X|", ToList[i], (-1) * rate);	
							BATTLESTR_ADD( szCommand );
							sprintf( szCommand, "BD|r%X|0|5|%X|", ToList[i], (-1) * rate);		
							BATTLESTR_ADD( szCommand );
							sprintf( szCommand, "BD|r%X|0|6|%X|", ToList[i], (-1) * rate );
							BATTLESTR_ADD( szCommand );

						}

						// 树根缠绕 降敏
						if(skill_type == BATTLE_COM_S_ENTWINE ){
							if( (pszP = strstr( pszOption, "敏%" ) ) != NULL )	sscanf( pszP+3, "%d", &dex );
							dex = skill_level * 4 + dex;
							old_dex = CHAR_getWorkInt( toindex, CHAR_WORKFIXDEX);
							old_dex = old_dex * (100 - dex) / 100;			
							CHAR_setWorkInt( toindex, CHAR_WORKFIXDEX, old_dex );
							sprintf( szCommand, "BD|r%X|0|6|%X|", ToList[i], (-1) * dex );
							BATTLESTR_ADD( szCommand );
						}

						// 火抗性提升
						if(skill_type == BATTLE_COM_S_RESIST_FIRE ){
							int old_value =  CHAR_getWorkInt( toindex, CHAR_WORK_F_RESIST );
							int up_value = skill_level + 10;
							CHAR_setWorkInt( toindex, CHAR_WORKMODRESIST_F, up_value );
							CHAR_setWorkInt( toindex, CHAR_WORK_F_RESIST,  old_value + up_value );
						
						}else
						// 冰抗性提升
						if(skill_type == BATTLE_COM_S_RESIST_ICE ){
							int old_value =  CHAR_getWorkInt( toindex, CHAR_WORK_I_RESIST );
							int up_value = skill_level + 10;
							CHAR_setWorkInt( toindex, CHAR_WORKMODRESIST_I, up_value );
							CHAR_setWorkInt( toindex, CHAR_WORK_I_RESIST,  old_value + up_value );
					
						}else 
						// 雷抗性提升
						if(skill_type == BATTLE_COM_S_RESIST_THUNDER ){
							int old_value =  CHAR_getWorkInt( toindex, CHAR_WORK_T_RESIST );
							int up_value = skill_level + 10;
							CHAR_setWorkInt( toindex, CHAR_WORKMODRESIST_T, up_value );
							CHAR_setWorkInt( toindex, CHAR_WORK_T_RESIST,  old_value + up_value );
				
						}
#ifdef _PROFESSION_ADDSKILL
                        else 
						// 自然威能
						if(skill_type == BATTLE_COM_S_RESIST_F_I_T ){
							int old_value =  CHAR_getWorkInt( toindex, CHAR_WORK_F_RESIST );
							int up_value = 2; //抗性值

							if( skill_level >= 10 )		up_value = 20;
				            else if( skill_level > 8 )	up_value = 18; 
			                else if( skill_level > 7 )	up_value = 16;
				            else if( skill_level > 6 )	up_value = 14; 
							else if( skill_level > 5 )  up_value = 12;
							else if( skill_level > 4 )  up_value = 10;
							else if( skill_level > 3 )  up_value = 8;
							else if( skill_level > 2 )  up_value = 6;
							else if( skill_level > 1 )  up_value = 4;
				            else up_value = 2;

							CHAR_setWorkInt( toindex, CHAR_WORKMODRESIST_F, up_value );
							CHAR_setWorkInt( toindex, CHAR_WORK_F_RESIST,  old_value + up_value );
							
							old_value =  CHAR_getWorkInt( toindex, CHAR_WORK_I_RESIST );
							CHAR_setWorkInt( toindex, CHAR_WORKMODRESIST_I, up_value );
							CHAR_setWorkInt( toindex, CHAR_WORK_I_RESIST,  old_value + up_value );
							
							old_value =  CHAR_getWorkInt( toindex, CHAR_WORK_T_RESIST );
							CHAR_setWorkInt( toindex, CHAR_WORKMODRESIST_T, up_value );
							CHAR_setWorkInt( toindex, CHAR_WORK_T_RESIST,  old_value + up_value );
						
						}
#endif
						//snprintf( szBuffer, sizeof(szBuffer),
						//	"(%s)将(%s)做成(%s)(%d%%)", CHAR_getUseName( charaindex ),
						//	CHAR_getUseName( toindex ), aszStatusFull[status], perStatus );

						//BATTLE_BroadCast( battleindex, szBuffer,
						//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
						BATTLE_BadStatusString( ToList[i], status );
						iRet = TRUE;
					}
				}
				break;
			}
		// 移动型
		case BATTLE_COM_S_SHIELD_ATTACK:	// 盾击
			{
				int itmid=-1;
				int perStatus, status=-1, Success=-1;
				char szBuffer[256]="";
				int ToList[SIDE_OFFSET*2+1];
				int defNo2 = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLECOM2);

				itmid = CHAR_getItemIndex( charaindex, CHAR_EQSHIELD );				

				if( ( itmid < 0 ) || ITEM_WSHIELD != ITEM_getInt( itmid , ITEM_TYPE ) ){
					CHAR_talkToCli( charaindex, -1, "未装备盾牌，无法使用此技能", CHAR_COLORYELLOW);
					iRet = FALSE;	
					break;
				} 				

				// 攻击力减半
				if( skill_level != 10 )
					CHAR_setWorkInt(charaindex,CHAR_WORKATTACKPOWER, (int)(CHAR_getWorkInt(charaindex,CHAR_WORKATTACKPOWER) * 0.5 ) );
				
				memset( ToList, -1, sizeof( ToList));
				BATTLE_MultiList( battleindex, defNo2, ToList );

				// 改变状态
				if( (pszP = strstr( pszOption, "晕" ) ) != NULL ){
					for( i = 1; i < BATTLE_ST_END; i ++ ){
						if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
							status = i;
							break;
						}
					}	
				}

				// 成功率
				if( (pszP = strstr( pszOption, "成%" ) ) != NULL )	sscanf( pszP+3, "%d", &Success );		
				Success += skill_level * 4;
				perStatus = Success;

				if( defNo2 >= SIDE_OFFSET ){
					i = defNo2 - SIDE_OFFSET;
					DefSide = 1;
				}
				
				for( i = 0; ToList[i] != -1; i ++ ){
					int hit = -1;
					react = 0;

					defindex = BATTLE_No2Index( battleindex, ToList[i] );		//守方index
					if( CHAR_CHECKINDEX( defindex ) == FALSE )	continue;

					if( (ReactType=BATTLE_GetDamageReact( defindex)) > 0 )	{	//有光镜守
						react = ReactType;
						skill_type=-1;
					}

					//取得攻击後状态
					iWork = BATTLE_AttackSeq( charaindex, defindex, &damage, &Guardian, skill_type );

					ultimate = BATTLE_DamageSub( charaindex, defindex, &damage, &petdamage, &react);

					if( damage > 0 && ( react != BATTLE_MD_ABSROB )&& ( react != BATTLE_MD_VANISH )){
						BATTLE_DamageWakeUp( battleindex, defindex );
					}

					iRet = BATTLE_DefineAttack( charaindex, defindex, iWork, szBuffer, damage, react, &flg);

					// 将 NORMAL 及 CRITICAL 的 flg 改成 职业的flg
					if( img2 != 0  ){
						if( flg == BCF_NORMAL || flg == BCF_KAISHIN ){
							hit=1;
							if( (pszP = strstr( pszOption, "前" ) ) != NULL ){
								flg |= BCF_F_SKILLACT; //击中前秀图
							}else{
								flg |= BCF_B_SKILLACT; //击中後秀图		
							}
						}
					}

					if( CHAR_getInt( defindex, CHAR_HP ) <= 0 )
						iRet = BATTLE_DefDieType( defindex, iRet, &ultimate, &flg, szBuffer);

					if( damage > 0 ){
						if( BATTLE_ItemCrushSeq( charaindex, defindex, damage) == TRUE ){
							flg |= BCF_CRUSH;
						}
					}

					if( hit == 1 ){
						if( (PROFESSION_BATTLE_StatusAttackCheck( charaindex, defindex, status, Success ) == 0 )
							|| (CHAR_getInt( defindex, CHAR_HP ) <= 0 ) ){

							//snprintf( szBuffer, sizeof(szBuffer), "(%s)无法将(%s)做成(%s)(%d%%)", 
							//	CHAR_getUseName( charaindex ), CHAR_getUseName( defindex ), aszStatusFull[status], perStatus );

							//BATTLE_BroadCast( battleindex, szBuffer,
							//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;							
						}else{
							CHAR_setWorkInt( defindex, StatusTbl[status], turn + 1 );

							if( status == BATTLE_ST_PARALYSIS
								||  status == BATTLE_ST_SLEEP
								||  status == BATTLE_ST_STONE
								||  status == BATTLE_ST_BARRIER	
								||  status == BATTLE_ST_DIZZY
								||  status == BATTLE_ST_ENTWINE
								||  status == BATTLE_ST_DRAGNET
								||  status == BATTLE_ST_ICECRACK
								||  status == BATTLE_ST_ICEARROW
								){
								CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
							}
								
							//snprintf( szBuffer, sizeof(szBuffer),
							//	"(%s)将(%s)做成(%s)(%d%%)",
							//	CHAR_getUseName( charaindex ),
							//	CHAR_getUseName( defindex ),
							//	aszStatusFull[status],
							//	perStatus
							//);
							
							BATTLE_BadStatusString( ToList[i], status );
						}
					}

					snprintf( szCommand, sizeof( szCommand ), "B+|a%X|r%X|f%X|d%X|p%X|e%X|s%X|h%X|FF|",
													attackNo, ToList[i], flg, damage, petdamage, effect, img1, img2 );
					BATTLESTR_ADD( szCommand );

					//BATTLE_BroadCast( battleindex, szBuffer,
					//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
								
					i = defNo2-DefSide*SIDE_OFFSET;
					if( ultimate > 0 ){
						BattleArray[battleindex].Side[DefSide].Entry[i].flg |= BENT_FLG_ULTIMATE;
					}

				 	iRet = TRUE;
				}
			  	break;				

			}
		case BATTLE_COM_S_TOXIN_WEAPON:		// 毒素武器
			{
				int perStatus, status=-1, Success=-1;
				int defNo2 = CHAR_getWorkInt(charaindex,CHAR_WORKBATTLECOM2);

				charaindex = BATTLE_No2Index( battleindex, attackNo );
				if( CHAR_CHECKINDEX( charaindex ) == FALSE )	return iRet;

				// 改变状态
				if( ((pszP = strstr( pszOption, "毒" ) ) != NULL ) ){

					for( i = 1; i < BATTLE_ST_END; i ++ ){
						if( strncmp( pszP, aszStatus[i], 2 ) == 0 ){
							status = i;
							break;
						}
					}	
				}
				
				// 成功率
				if( (pszP = strstr( pszOption, "成%" ) ) != NULL )	sscanf( pszP+3, "%d", &Success );
				Success += skill_level * 2;
				perStatus = Success;
	
				if( defNo2 >= SIDE_OFFSET ){
					i = defNo2 - SIDE_OFFSET;
					DefSide = 1;
				}
				
				{
					int gWeponType = BATTLE_GetWepon( charaindex );
					char szCommand[512];
					int i = 0, ReactType = 0, iRet = -1;
					int ToList[SIDE_OFFSET*2+1];
					BOOL bChange = FALSE;

					memset( ToList, -1, sizeof( ToList));
					BATTLE_MultiList( battleindex, defNo, ToList );

					memset( szCommand, -1, sizeof(szCommand) );

					// 有无变身
#ifdef _PETSKILL_BECOMEPIG
					if(CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) > 0
						|| CHAR_getWorkInt(charaindex, CHAR_BECOMEPIG) > 0) bChange = TRUE;
#else
					if(CHAR_getWorkInt(charaindex, CHAR_WORKFOXROUND) > 0) bChange = TRUE;
#endif
					if(!bChange){
						// 武器为远程武器
						if( gWeponType == ITEM_BOW ){
							sprintf( szCommand, "BB|a%X|w0|", attackNo );
							BATTLESTR_ADD( szCommand );
							memset( ToList, -1, sizeof( ToList));
							BATTLE_TargetListSet(charaindex,attackNo,ToList);
						}else if( gWeponType == ITEM_BOUNDTHROW ){
							sprintf( szCommand, "BB|a%X|w1|", attackNo );
							BATTLESTR_ADD( szCommand );
						}else if( gWeponType == ITEM_BREAKTHROW ){
							sprintf( szCommand, "BB|a%X|w2|", attackNo );					
							BATTLESTR_ADD( szCommand );
						}else if( gWeponType == ITEM_BOOMERANG){
							sprintf( szCommand, "BO|a%X|", attackNo );					
							BATTLESTR_ADD( szCommand );
							memset(ToList,-1,sizeof(ToList));
							for(i=0;i<5;i++) ToList[i] = BoomerangVsTbl[defNo/5][i];
							gBattleDamageModyfy = 0.3;
						}
					}
					
					for( i = 0; ToList[i] != -1; i ++ ){
						int defindex = -1, j = -1;
						int react = 0;
						char szBuffer[256] = "";
						
						damage = 0;
						petdamage = 0;
						flg = 0;

						//memset( szBuffer, 0, sizeof( szBuffer ) );

						defindex = BATTLE_No2Index( battleindex, ToList[i] );		//守方index
						if( CHAR_CHECKINDEX( defindex ) == FALSE )	continue;
						if( CHAR_getInt( defindex, CHAR_HP ) <= 0 ) continue;
	
						if( (ReactType=BATTLE_GetDamageReact( defindex)) > 0 ){	//有光镜守
							react = ReactType;
							skill_type=-1;
						}

						//取得攻击後状态
						iWork = BATTLE_AttackSeq( charaindex, defindex, &damage, &Guardian, skill_type );
						if(Guardian >= 0) defindex = BATTLE_No2Index( battleindex, Guardian );
						
						iRet = BATTLE_DefineAttack( charaindex, defindex, iWork, szBuffer, damage, react, &flg);

						if(damage > 0){
							ultimate = BATTLE_DamageSub( charaindex, defindex, &damage, &petdamage, &react);

							if((react != BATTLE_MD_ABSROB ) && ( react != BATTLE_MD_VANISH ) ){
								BATTLE_DamageWakeUp( battleindex, defindex );
							}
										
							if( CHAR_getInt( defindex, CHAR_HP ) <= 0 )
								iRet = BATTLE_DefDieType( defindex, iRet, &ultimate, &flg, szBuffer);

							if( BATTLE_ItemCrushSeq( charaindex, defindex, damage) == TRUE ){
								flg |= BCF_CRUSH;
							}

							if( (PROFESSION_BATTLE_StatusAttackCheck( charaindex, defindex, status, perStatus ) == 0 )
								|| (CHAR_getInt( defindex, CHAR_HP ) <= 0 ) ){

								//snprintf( szBuffer, sizeof(szBuffer), "(%s)无法将(%s)做成(%s)(%d%%)", 
								//	CHAR_getUseName( charaindex ), CHAR_getUseName( defindex ), aszStatusFull[status], perStatus );
								//BATTLE_BroadCast( battleindex, szBuffer,
								//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;							
							}else{
								CHAR_setWorkInt( defindex, StatusTbl[status], turn + 1 );

								if( status == BATTLE_ST_PARALYSIS
									||  status == BATTLE_ST_SLEEP
									||  status == BATTLE_ST_STONE
									||  status == BATTLE_ST_BARRIER	
									||  status == BATTLE_ST_DIZZY
									||  status == BATTLE_ST_ENTWINE
									||  status == BATTLE_ST_DRAGNET
									||  status == BATTLE_ST_ICECRACK
									||  status == BATTLE_ST_ICEARROW
								){
									CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
								}
						
								//snprintf( szBuffer, sizeof(szBuffer),
								//	"(%s)将(%s)做成(%s)(%d%%)",
								//	CHAR_getUseName( charaindex ),
								//	CHAR_getUseName( defindex ),
								//	aszStatusFull[status],
								//	perStatus
								//);
												
								BATTLE_BadStatusString( ToList[i], status );
							}
						}
						
						// 武器为远程武器时

						if(((gWeponType == ITEM_BOW)
							|| (gWeponType == ITEM_BOUNDTHROW)
							|| (gWeponType == ITEM_BREAKTHROW)
							|| (gWeponType == ITEM_BOOMERANG)) && !bChange){
							snprintf( szCommand, sizeof( szCommand ), "r%X|f%X|d%X|p%X|", ToList[i], flg, damage, petdamage );
							BATTLESTR_ADD( szCommand );
						}else{
							snprintf( szCommand, sizeof( szCommand ), "BH|a%X|r%X|f%X|d%X|p%X|FF|", attackNo, ToList[i], flg, damage, petdamage);
							BATTLESTR_ADD( szCommand );
						}

						//BATTLE_BroadCast( battleindex, szBuffer,
						//	(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
								
						j = ToList[i]-DefSide*SIDE_OFFSET;
						if( ultimate > 0 ){
							BattleArray[battleindex].Side[DefSide].Entry[j].flg |= BENT_FLG_ULTIMATE;
						}

						iRet = TRUE;
					}

					// 武器为远程武器时
					if((( gWeponType == ITEM_BOW ) || ( gWeponType == ITEM_BOUNDTHROW ) || ( gWeponType == ITEM_BREAKTHROW ) || ( gWeponType == ITEM_BOOMERANG )) && !bChange){
						snprintf( szCommand, sizeof( szCommand ), "FF|" );
						BATTLESTR_ADD( szCommand );
					}
				}
				if(gWeponType == ITEM_BOOMERANG) gBattleDamageModyfy = 1.0;
		  	break;
			}
#ifdef _PROFESSION_ADDSKILL
		case BATTLE_COM_S_BOUNDARY:         // 四属性结界
            {
                int ToList[SIDE_OFFSET*2+1],status=-1,power=20,toindex=-1,loop=1;
				int defNo2 = defNo;//CHAR_getWorkInt(charaindex,CHAR_WORKBATTLECOM2);
                int rand_boundary,rand_boundary2 = RAND( 1, 100 );

                if( skill_level >= 10 )		turn = 5;
				else if( skill_level > 9 )	turn = 4; 
				else if( skill_level > 8 )	turn = 3;
				else if( skill_level > 4 )	turn = 2; 
				else turn = 1;
				
				skill_level = CHAR_GETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3);
                
				if( skill_level >= 100 ) power = 100;
				else if( skill_level > 95 ) power = 90;
				else if( skill_level > 90 ) power = 80;
				else if( skill_level > 85 ) power = 70;
				else if( skill_level > 80 ) power = 60;
				else if( skill_level > 60 ) power = 50;
				else if( skill_level > 40 ) power = 40;
				else if( skill_level > 20 ) power = 30;
				else power = 20;

				if( ((pszP = strstr( pszOption, "地结界" ) ) != NULL ) )
				    status = CHAR_WORKFIXEARTHAT_BOUNDARY;
				else if( ((pszP = strstr( pszOption, "水结界" ) ) != NULL ) )
				    status = CHAR_WORKFIXWATERAT_BOUNDARY;
				else if( ((pszP = strstr( pszOption, "火结界" ) ) != NULL ) )
				    status = CHAR_WORKFIXFIREAT_BOUNDARY;
				else if( ((pszP = strstr( pszOption, "风结界" ) ) != NULL ) ){
                    status = CHAR_WORKFIXWINDAT_BOUNDARY;
				}
				else{//破结界
                    status = CHAR_WORKFIXEARTHAT_BOUNDARY;
					
					if( skill_level >= 100 )		rand_boundary = 100;
				    else if( skill_level > 80 )	rand_boundary = 80; 
				    else if( skill_level > 40 )	rand_boundary = 70;
				    else if( skill_level > 20 )	rand_boundary = 60;
				    else rand_boundary = 50;
					if( rand_boundary2 <= rand_boundary ){
                        loop = 4;
					    power=0;
					    turn=0;
					}
					else
						loop = 0;
				}

				if( ((pszP = strstr( pszOption, "破结界" ) ) != NULL ) ){
					if( defNo2 < 10 )defNo2 = 20;	else defNo2 = 21;
				}

				memset( ToList, -1, sizeof( ToList));
				BATTLE_MultiList( battleindex, defNo2, ToList );

				if( ((pszP = strstr( pszOption, "破结界" ) ) == NULL ) ){//先把所有结界清除
					for( j=CHAR_WORKFIXEARTHAT_BOUNDARY; j<CHAR_WORKFIXEARTHAT_BOUNDARY+4; j++ ){
						for( i = 0; ToList[i] != -1; i ++ ){
							toindex = BATTLE_No2Index( battleindex, ToList[i] );
							if( CHAR_CHECKINDEX( toindex ) == FALSE )	return iRet;
							CHAR_setWorkInt( toindex, j, 0 );
						}
					}
				}
				for( j=status; j<status+loop; j++ ){
				    for( i = 0; ToList[i] != -1; i ++ ){
					    toindex = BATTLE_No2Index( battleindex, ToList[i] );
					    if( CHAR_CHECKINDEX( toindex ) == FALSE )	return iRet;
                        CHAR_setWorkInt( toindex, j, MAKE2VALUE(power,turn) );
					}
				}

				// 将魔法参数代入动画
	            analysis_profession_parameter( 2, CHAR_GETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3), defNo2, charaindex );
	            PROFESSION_MAGIC_ATTAIC_Effect( battleindex, attackNo, ToList, 2);
                sprintf( szCommand , "%X|" , 0x5711438 );
	            BATTLESTR_ADD( szCommand );

				if( power == 0 ){
                    if( defNo2 == 20 || defNo2 == 25 || defNo2 == 26 )//右方
				        snprintf( szCommand, sizeof( szCommand ), "Ba|%X|%X|", 0, 1 );
					else
                        snprintf( szCommand, sizeof( szCommand ), "Ba|%X|%X|", 0, 0 );
				}
				else
				    snprintf( szCommand, sizeof( szCommand ), "Ba|%X|%X|", 0, GET_PROFESSION_magic_array(2)/*status-(CHAR_WORKFIXEARTHAT_BOUNDARY-1)*/ );
				BATTLESTR_ADD( szCommand );
				BATTLESTR_ADD( "FF|" );
			break;
			}
#endif
		default:
			break;
	}

	return iRet;
}


int BATTLE_PROFESSION_ATK_PET_DamageSub( int attackindex, int defindex, int *pDamage, int *pPetDamage, int *pRefrect, int skill_level )
{

	int hp, addpoint = 0, maxhp, IsUltimate = 0, react, work, battleindex;
	int damage, playerdamage, petdamage;
	int attackpet, defpet;
	int pethp = 0, petmaxhp = 0;
	int defquick = 0, petquick = 0;
	damage = *pDamage;
	petdamage = *pPetDamage;
	playerdamage = damage;
	if( damage <= 0 )return 0;

	attackpet = BATTLE_getRidePet( attackindex );
	defpet = BATTLE_getRidePet( defindex );
	if( defpet != -1 ) {
		defquick = CHAR_getWorkInt( defindex, CHAR_WORKDEFENCEPOWER );
		petquick = CHAR_getWorkInt( defpet, CHAR_WORKDEFENCEPOWER );
	}

	if( *pRefrect != -1 ){
		react = BATTLE_GetDamageReact( defindex );
	}else{
		react = BATTLE_MD_NONE;	
	}
	battleindex = CHAR_getWorkInt( defindex, CHAR_WORKBATTLEINDEX );
	if( react == BATTLE_MD_REFLEC ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			*pRefrect = BATTLE_MD_REFLEC;
		}else{
			*pRefrect = BATTLE_MD_NONE;
		}
	}else{
	}

	if( react == BATTLE_MD_TRAP ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE )
			*pRefrect = BATTLE_MD_TRAP;
		else
			*pRefrect = BATTLE_MD_NONE;
	}
#ifdef _PETSKILL_ACUPUNCTURE
    if( react == BATTLE_MD_ACUPUNCTURE ){
		if( BATTLE_IsThrowWepon( CHAR_getItemIndex( attackindex, CHAR_ARM ) ) == FALSE ){
			*pRefrect = BATTLE_MD_ACUPUNCTURE;
		}else{
			*pRefrect = BATTLE_MD_NONE;
		}
	}
#endif
	if( react == BATTLE_MD_ABSROB ){
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet == -1 ) {
			hp += damage;
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			playerdamage = (damage*petquick)/(defquick+petquick) +1 ;
			petdamage = damage - playerdamage +1 ;
			hp += playerdamage;
			pethp += petdamage;
			
		}
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEABSROB ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEABSROB, max( work, 0 ) );
		*pRefrect = BATTLE_MD_ABSROB;
	}else
	if( *pRefrect == BATTLE_MD_REFLEC ){
		hp = CHAR_getInt( attackindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= damage;
		}else {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				hp -= playerdamage;
				pethp -= petdamage;
			}else
				hp -= damage;
		}
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEREFLEC ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEREFLEC, max( work, 0 ) );
		defindex = attackindex;
		defpet = attackpet;

	}else
	if( *pRefrect == BATTLE_MD_TRAP ){		//陷阱	
		int value=0;
		value = CHAR_getWorkInt( defindex, CHAR_WORKMODTRAP );
		damage = value;

		hp = CHAR_getInt( attackindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= damage;
			playerdamage = hp;
		}else {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				hp -= playerdamage;
				pethp -= petdamage;
			}else
				hp -= damage;
		}

		CHAR_setWorkInt( defindex, CHAR_WORKTRAP, 0 );
		CHAR_setWorkInt( defindex, CHAR_WORKMODTRAP, 0 );
		defindex = attackindex;
		defpet = attackpet;	

	}else
#ifdef _PETSKILL_ACUPUNCTURE
/*		if( *pRefrect == BATTLE_MD_ACUPUNCTURE ){
		hp = CHAR_getInt( attackindex, CHAR_HP );
		if( attackpet == -1 ) {
			hp -= ( damage/2 ? damage/2:1 );
		}else {
			pethp = CHAR_getInt( attackpet, CHAR_HP );
			defquick = CHAR_getWorkInt( attackindex, CHAR_WORKDEFENCEPOWER );
			petquick = CHAR_getWorkInt( attackpet, CHAR_WORKDEFENCEPOWER );
			if( pethp > 0 ) {
				playerdamage = (damage*petquick)/(defquick+petquick) +1;
				petdamage = damage - playerdamage +1 ;
				hp -= ( playerdamage/2 ? playerdamage/2:1 );
				pethp -= ( petdamage/2 ? petdamage/2:1 );
			}else
				hp -= ( damage/2 ? damage/2:1 );
		}
		//work = CHAR_getWorkInt( defindex, CHAR_WORKACUPUNCTURE ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKACUPUNCTURE, 0);//max( work, 0 ) );
		defindex = attackindex;
		defpet = attackpet;
	}*/
#endif
	if( react == BATTLE_MD_VANISH ){
		hp = CHAR_getInt( defindex, CHAR_HP );
		if( defpet != -1 )
			pethp = CHAR_getInt( defpet, CHAR_HP );
		work = CHAR_getWorkInt( defindex, CHAR_WORKDAMAGEVANISH ) - 1;
		CHAR_setWorkInt( defindex, CHAR_WORKDAMAGEVANISH, max( work, 0 ) );
		*pRefrect = BATTLE_MD_VANISH;
	}else{
		hp = CHAR_getInt( defindex, CHAR_HP );

		if( defpet == -1 ) {
			hp -= (damage * 0.7);
		}else {
			pethp = CHAR_getInt( defpet, CHAR_HP );
			if( pethp > 0 ) {
				int rate=0;
				rate = skill_level * 2 + 60;

				petdamage = damage * rate / 100;				
				playerdamage = damage - petdamage;

				hp -= playerdamage;
				pethp -= petdamage;
	
			}else
				hp -= damage;
		}
		*pRefrect = 0;
	}
	if( hp < 0 ){
		addpoint = -hp;
		hp = 0;
	}
	maxhp = CHAR_getWorkInt( defindex, CHAR_WORKMAXHP );
	CHAR_setInt( defindex, CHAR_HP, min( hp, maxhp ) );
	if( defpet != -1 ) {
		if( pethp < 0 )	pethp = 0;
		petmaxhp = CHAR_getWorkInt( defpet, CHAR_WORKMAXHP );
		CHAR_setInt( defpet, CHAR_HP, min( pethp, petmaxhp ) );
		// Robin 0804
		if( pethp <= 0 ) {
			CHAR_setInt( defindex, CHAR_RIDEPET, -1 );
			BATTLE_changeRideImage( defindex );
			CHAR_setWorkInt( defindex, CHAR_WORKPETFALL, 1 );
			//print(" defpet_die! ");
		}
	}
	
	if( (damage) >= maxhp * 1.2 + 20 ){
		IsUltimate = 2;
	}else{
		if( addpoint > 0 ){
			addpoint += CHAR_getWorkInt( defindex, CHAR_WORKULTIMATE );
			CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, addpoint );
			if( addpoint >= maxhp * 1.2 + 20 )IsUltimate = 1;
		}
	}

	if( BattleArray[battleindex].type == BATTLE_TYPE_P_vs_P ){
	}
#ifdef _PETSKILL_LER
// 雷尔不能被打飞
	if(CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
		 CHAR_getInt(defindex,CHAR_BASEBASEIMAGENUMBER) == 101814) IsUltimate = 0;
#endif

	if( IsUltimate ){
		CHAR_setWorkInt( defindex, CHAR_WORKULTIMATE, 0 );
	}
	if( playerdamage < 0 ) playerdamage = 0;
	if( petdamage < 0 ) petdamage = 0;
	*pDamage = playerdamage;
	*pPetDamage = petdamage;
	return IsUltimate;
}


// 取得回旋攻击伤害
int BATTLE_PROFESSION_CONVOLUTE_GET_DAMAGE( int attackindex, int defindex, int skill_level ) 
{
	int damage = 0, old_atk = 0, atk = 0;
	int rand_num = RAND(1, 10000 );
	// 会心一击
	int perCri = BATTLE_CriticalCheck( attackindex, defindex );

	old_atk = CHAR_getWorkInt( attackindex, CHAR_WORKATTACKPOWER );
	atk = old_atk * (100 -( 50 - skill_level )) / 100;	
	CHAR_setWorkInt( attackindex, CHAR_WORKATTACKPOWER, atk );

	if( rand_num < perCri ){
		damage = BATTLE_CriDamageCalc( attackindex, defindex );
	}else{
		//profession fix
		if( BATTLE_DuckCheck( attackindex, defindex ) )
			return 0;
		damage = BATTLE_DamageCalc( attackindex, defindex );
	}

	return damage;
}

// 取得贯穿攻击伤害
int BATTLE_PROFESSION_THROUGH_ATTACK_GET_DAMAGE( int attackindex, int defindex ) 
{
	int damage = 0;
	int rand_num = RAND(1, 10000 );
	// 会心一击
	int perCri = BATTLE_CriticalCheck( attackindex, defindex );

	if( rand_num < perCri ){
		damage = BATTLE_CriDamageCalc( attackindex, defindex );
	}else{
		//profession fix
		if( BATTLE_DuckCheck( attackindex, defindex ) )
			return 0;
		damage = BATTLE_DamageCalc( attackindex, defindex );
	}

	return damage;
}

// 火、冰、雷附体附加攻击
int BATTLE_PROFESSION_RANG_ATTACK_DAMAGE( int charaindex, int attackNo, int defNo, int skill_type, int status, int turn, int perStatus, int effect, int img1, int img2 )
{
	int ReactType = 0, iRet = -1,defindex = -1,react = 0;
	int battleindex = CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX );
	//char szBuffer[256]="";

	//memset( szBuffer, 0, sizeof( szBuffer ) );

	defindex = BATTLE_No2Index( battleindex, defNo);		//守方index					
	if( defindex == -1 ) return iRet;

	if( (ReactType=BATTLE_GetDamageReact( defindex)) > 0 ){	//有光镜守
		react = ReactType;
		skill_type=-1;
	}

	if( img1 != 0 ){
		if( (PROFESSION_BATTLE_StatusAttackCheck( charaindex, defindex, status, perStatus ) == 0 )
				|| (CHAR_getInt( defindex, CHAR_HP ) <= 0 ) ){
				//snprintf( szBuffer, sizeof(szBuffer), "(%s)无法将(%s)做成(%s)(%d%%)", 
				//					CHAR_getUseName( charaindex ), CHAR_getUseName( defindex ), aszStatusFull[status], perStatus );
				//BATTLE_BroadCast( battleindex, szBuffer,(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;							
		}
		else{
			CHAR_setWorkInt( defindex, StatusTbl[status], turn + 1 );

			if( status == BATTLE_ST_PARALYSIS
					||  status == BATTLE_ST_SLEEP
					||  status == BATTLE_ST_STONE
					||  status == BATTLE_ST_BARRIER	
					||  status == BATTLE_ST_DIZZY
					||  status == BATTLE_ST_ENTWINE
					||  status == BATTLE_ST_DRAGNET
					||  status == BATTLE_ST_ICECRACK
					||  status == BATTLE_ST_ICEARROW
				) CHAR_setWorkInt( defindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
		
			//snprintf( szBuffer, sizeof(szBuffer),
			//		"(%s)将(%s)做成(%s)(%d%%)",
			//		CHAR_getUseName( charaindex ),
			//		CHAR_getUseName( defindex ),
			//		aszStatusFull[status],
			//		perStatus
			//		);
								
			BATTLE_BadStatusString( defNo, status );
			iRet = TRUE;
		}
		//BATTLE_BroadCast( battleindex, szBuffer,(attackNo >= 10)? CHAR_COLORGRAY : CHAR_COLORPURPLE ) ;
	}

	return iRet;
}

#endif

#ifdef _PETSKILL_LER
extern ToCallMagic PROFESSION_magic[3];
// 雷尔技 - 群蝠四窜
void BATTLE_BatFly( int battleindex, int attackNo,int myside)
{
	int ToList[SIDE_OFFSET*2+1],i,toindex;
	int petidx,addhp = 0,charhp,pethp,charaindex;
	char szCommand[1024];

	charaindex = BATTLE_No2Index(battleindex,attackNo);
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return;

	memset(ToList,-1,sizeof(ToList));
	if(myside) BATTLE_MultiList(battleindex,TARGET_SIDE_0,ToList);	// 打右上
	else BATTLE_MultiList(battleindex,TARGET_SIDE_1,ToList);				// 打左下
	
	PROFESSION_magic[2].uiSpriteNum = myside ? 101806:101807;
	PROFESSION_magic[2].siSx = 320;
	PROFESSION_magic[2].siSy = 240;
	PROFESSION_magic[2].uiPrevMagicNum = myside ? 101808:101809;		// 前置动画
	PROFESSION_magic[2].uiPostMagicNum = 0xffffffff;								// 无後置动画
	PROFESSION_magic[2].uiShowType = 1;
	PROFESSION_magic[2].uiShowBehindChar = 1;
	PROFESSION_magic[2].siPrevMagicSx = 0;
	PROFESSION_magic[2].siPrevMagicSy = 0;
	PROFESSION_MAGIC_ATTAIC_Effect(battleindex,attackNo,ToList,2);
	sprintf(szCommand,"%X|",0x5711438);
	BATTLESTR_ADD(szCommand);

	for(i=0;i<SIDE_OFFSET;i++){
		toindex = BATTLE_No2Index(battleindex,ToList[i]);
		petidx = BATTLE_getRidePet(toindex);
		if(!CHAR_CHECKINDEX(toindex))	continue;
		charhp = CHAR_getInt(toindex,CHAR_HP);
		// 没骑宠物
		if( -1 == petidx || CHAR_getInt(petidx,CHAR_HP) <= 0){
			// 剩下不到10滴血
			if((charhp/10) == 0){
				CHAR_setInt(toindex,CHAR_HP,charhp - 1);
				charhp = 1;
			}
			// 扣10%的HP
			else{
				CHAR_setInt(toindex,CHAR_HP,charhp - (charhp/10));
				charhp /= 10;
			}
			addhp += charhp;
			sprintf(szCommand,"BD|r%X|%X|0|d%X|p0|",ToList[i],BD_KIND_HP,charhp);
		}
		else{
			// 剩下不到20滴血
			if((charhp/20) == 0){
				CHAR_setInt(toindex,CHAR_HP,charhp - 1);
				charhp = 1;
			}
			// 人和宠各扣5%HP
			else{
				CHAR_setInt(toindex,CHAR_HP,charhp - (charhp/20));
				charhp /= 20;
			}
			addhp += charhp;
			pethp = CHAR_getInt(petidx,CHAR_HP);
			// 剩下不到20滴血
			if((pethp/20) == 0){
				CHAR_setInt(petidx,CHAR_HP,pethp - 1);
				pethp = 1;
				if(CHAR_getInt(petidx,CHAR_HP) <= 0){
					CHAR_setInt(toindex,CHAR_RIDEPET,-1);
					BATTLE_changeRideImage(toindex);
					CHAR_setWorkInt(toindex,CHAR_WORKPETFALL,1);
				}
			}
			else{
				CHAR_setInt(petidx,CHAR_HP,pethp - (pethp/20));
				pethp /= 20;
			}
			addhp += pethp;
			sprintf(szCommand,"BD|r%X|%X|0|d%X|p%X|",ToList[i],BD_KIND_HP,charhp,pethp);
		}
		BATTLESTR_ADD(szCommand);
	}
	if((CHAR_getInt(charaindex,CHAR_HP) + addhp) > CHAR_getWorkInt(charaindex,CHAR_WORKMAXHP)){
		CHAR_setInt(charaindex,CHAR_HP,CHAR_getWorkInt(charaindex,CHAR_WORKMAXHP));
		addhp = 0;
	}
	else CHAR_setInt(charaindex,CHAR_HP,CHAR_getInt(charaindex,CHAR_HP) + addhp);
	sprintf(szCommand,"BD|r%X|%X|1|d%X|p0|",attackNo,BD_KIND_HP,addhp);
	BATTLESTR_ADD(szCommand);
}

// 雷尔技 - 分身地裂
void BATTLE_DivideAttack( int battleindex, int attackNo,int myside)
{
	int ToList[SIDE_OFFSET*2+1],i,toindex;
	int petidx,charhp,charmp,pethp,charaindex;
	char szCommand[1024];

	charaindex = BATTLE_No2Index(battleindex,attackNo);
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return;

	memset(ToList,-1,sizeof(ToList));
	if(myside) BATTLE_MultiList(battleindex,TARGET_SIDE_0,ToList);	// 打左上
	else BATTLE_MultiList(battleindex,TARGET_SIDE_1,ToList);				// 打右下
	
	PROFESSION_magic[2].uiSpriteNum = myside ? 101798:101800;
	PROFESSION_magic[2].siSx = 320;
	PROFESSION_magic[2].siSy = 240;
	PROFESSION_magic[2].uiPrevMagicNum = myside ? 101808:101809;		// 前置动画
	PROFESSION_magic[2].uiPostMagicNum = 0xffffffff;								// 无後置动画
	PROFESSION_magic[2].uiShowType = 1;
	PROFESSION_magic[2].uiShowBehindChar = 0;
	PROFESSION_magic[2].siPrevMagicSx = 0;
	PROFESSION_magic[2].siPrevMagicSy = 0;
	PROFESSION_MAGIC_ATTAIC_Effect(battleindex,attackNo,ToList,2);
	sprintf(szCommand,"%X|",0x5711438);
	BATTLESTR_ADD(szCommand);

	// 先完成扣MP的动作
	for(i=0;i<SIDE_OFFSET;i++){
		toindex = BATTLE_No2Index(battleindex,ToList[i]);
		if(!CHAR_CHECKINDEX(toindex))	continue;
		// 若是玩家
		if(CHAR_getInt(toindex,CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
			charmp = CHAR_getInt(toindex,CHAR_MP);
			// 人扣1/2MP
			CHAR_setInt(toindex,CHAR_MP,charmp - (charmp>>1));
			charmp >>= 1;
			sprintf(szCommand,"BD|r%X|%X|0|d%X|",ToList[i],BD_KIND_MP,charmp);
			BATTLESTR_ADD(szCommand);
		}
	}
	for(i=0;i<SIDE_OFFSET;i++){
		toindex = BATTLE_No2Index(battleindex,ToList[i]);
		petidx = BATTLE_getRidePet(toindex);
		if(!CHAR_CHECKINDEX(toindex))	continue;

		charhp = CHAR_getInt(toindex,CHAR_HP);
		// 没骑宠物
		if( -1 == petidx || CHAR_getInt(petidx,CHAR_HP) <= 0){
			// 扣20%HP
			if((charhp/5) == 0){
				CHAR_setInt(toindex,CHAR_HP,charhp - 1);
				charhp = 1;
			}
			else{
				CHAR_setInt(toindex,CHAR_HP,charhp - (charhp/5));
				charhp /= 5;
			}
			sprintf(szCommand,"BD|r%X|%X|0|d%X|p0|",ToList[i],BD_KIND_HP,charhp);
		}
		else{
			// 人和宠各扣10%HP
			if((charhp/10) == 0){
				CHAR_setInt(toindex,CHAR_HP,charhp - 1);
				charhp = 1;
			}
			else{
				CHAR_setInt(toindex,CHAR_HP,charhp - (charhp/10));
				charhp /= 10;
			}
			pethp = CHAR_getInt(petidx,CHAR_HP);
			if((pethp/10) == 0){
				CHAR_setInt(petidx,CHAR_HP,pethp - 1);
				pethp = 1;
				if(CHAR_getInt(petidx,CHAR_HP) <= 0){
					CHAR_setInt(toindex,CHAR_RIDEPET,-1);
					BATTLE_changeRideImage(toindex);
					CHAR_setWorkInt(toindex,CHAR_WORKPETFALL,1);
				}
			}
			else{
				CHAR_setInt(petidx,CHAR_HP,pethp - (pethp/10));
				pethp /= 10;
			}
			sprintf(szCommand,"BD|r%X|%X|0|d%X|p%X|",ToList[i],BD_KIND_HP,charhp,pethp);
		}
		BATTLESTR_ADD(szCommand);
	}
}

// 雷尔死亡,变身
void BATTLE_LerChange(int battleindex,int charaindex,int no)
{
	int array = -1,newindex = -1;
	BATTLE *pBattle;
	BATTLE_ENTRY *pEntry;
	
	BATTLE_Exit(charaindex,battleindex);
	if(CHAR_getInt(charaindex,CHAR_BASEBASEIMAGENUMBER) == 101813){
		array = ENEMY_getEnemyArrayFromId(2534);
		if(array == -1) return;
		newindex = ENEMY_createEnemy(array,200);
		if(newindex == -1) return;
		CHAR_setWorkInt(newindex,CHAR_WORK_RELIFE,101810);
		CHAR_setWorkInt(newindex,CHAR_WORKBATTLESIDE,no > 10 ? 1:0);
	}
	else if(CHAR_getInt(charaindex,CHAR_BASEBASEIMAGENUMBER) == 101814){
		array = ENEMY_getEnemyArrayFromId(2510);
		if(array == -1) return;
		newindex = ENEMY_createEnemy(array,300);
		if(newindex == -1) return;
		CHAR_setWorkInt(newindex,CHAR_WORK_RELIFE,101811);
		CHAR_setWorkInt(newindex,CHAR_WORKBATTLESIDE,no > 10 ? 1:0);
	}
	pBattle = &BattleArray[battleindex];
	pEntry = pBattle->Side[no > 10 ? 1:0].Entry;
	pEntry[no > 10 ? no-10:no].charaindex = newindex;
}
#endif

#ifdef _PETSKILL_BATTLE_MODEL
typedef struct _tsAttackObject{
	int index;					// 攻击物件编号
	int target;					// 目标编号
	int actionNumber;		// 攻击物件图号
}AttackObject;
// BATTLE_BattleModel 用的攻击函式
void BATTLE_BattleModel_ATTACK(
	int battleindex,
	int charaindex,
	AttackObject *pAAttackObject,
	int iEffect,
	int iTurn,
	int iEffectHit,
	int iType)
{
	int i,iDefindex,iPetindex,iToindex,iDamage,iPetDamage,iGuardian = -1,iDefState,iUltimate = 0,iTemp = 0,iTemp2 = 0,iFlg = 0;
	char szCommand[1024];

	// 检查目标存不存在或有没有死亡
	if(!BATTLE_TargetCheck(battleindex,pAAttackObject->target)) return;
	iToindex = iDefindex = BATTLE_No2Index(battleindex,pAAttackObject->target);
	// 若是宠物,检查主人在不在场上
	if(CHAR_getInt(iToindex,CHAR_WHICHTYPE ) == CHAR_TYPEPET){
		if(pAAttackObject->target + 5 >= SIDE_OFFSET) iFlg = 1;
		i = pAAttackObject->target + 5 - iFlg * SIDE_OFFSET;
		if(BattleArray[battleindex].Side[iFlg].Entry[i].flg & BENT_FLG_ULTIMATE) return;
	}
	iPetindex = BATTLE_getRidePet(iToindex);
	// 计算伤害
	iDefState = BATTLE_AttackSeq(charaindex,iToindex,&iDamage,&iGuardian,-1);
	// iType & 0x00000004 为 true 表示为物理攻击
	if(iType & 0x00000004){
		// 若忠犬没死亡,设定忠犬
		if(BATTLE_TargetCheck(battleindex,iGuardian)){
			// 若有忠犬,把被攻击者设定为使用忠犬技能的角色,否则被攻击者为原目标
			if(iGuardian >= 0) iDefindex = BATTLE_No2Index(battleindex,iGuardian);
		}
		else iGuardian = -1;
	}
	// 设定 CHAR_NPCWORKINT1 为 BATTLE_COM_S_BATTLE_MODEL 在 BATTLE_DamageSub 会用到
	iTemp2 = CHAR_getWorkInt(iDefindex,CHAR_NPCWORKINT1);
	CHAR_setWorkInt(iDefindex,CHAR_NPCWORKINT1,BATTLE_COM_S_BATTLE_MODEL);
	// 在这之後的 iDefindex 才是真正会受伤的目标
	if(gDamageDiv != 0.0 && iDamage > 0){
		iDamage /= gDamageDiv;
		if(iDamage <= 0) iDamage = 1;
	}
	// 若没有下列任一项,进行扣血
	if(!(iDefState == BATTLE_RET_DODGE)){
		// 扣血
		// 物理攻击
		if(iType & 0x00000004) iUltimate = BATTLE_DamageSub(charaindex,iDefindex,&iDamage,&iPetDamage,&iTemp);
		// 非物理攻击
		else{
			iTemp = -1;
			iUltimate = BATTLE_DamageSub(charaindex,iDefindex,&iDamage,&iPetDamage,&iTemp);
		}
	}
	iFlg = 0;
	// 回传状态检查
	switch(iDefState){
		case BATTLE_RET_ALLGUARD:iFlg |= BCF_GUARD;break;
		case BATTLE_RET_MISS:iDamage = 0;break;
		case BATTLE_RET_DODGE:iDamage = 0;iFlg |= BCF_DODGE;break;
		case BATTLE_RET_NORMAL:
			iFlg |= BATTLE_getReactFlg(iDefindex,iTemp);
			iFlg |= BCF_NORMAL;
			break;
		case BATTLE_RET_CRITICAL:
			iFlg |= BATTLE_getReactFlg(iDefindex,iTemp);
			iFlg |= BCF_KAISHIN;
			break;
#ifdef _EQUIT_ARRANGE
		case BATTLE_RET_ARRANGE:iFlg |= BCF_B_ARRANGE;break;
#endif
	}

	//昏睡时,在这里解除状态(也有其它的状态)
	if(iDamage > 0 && (iTemp != BATTLE_MD_ABSROB) && (iTemp != BATTLE_MD_VANISH)) BATTLE_DamageWakeUp(battleindex,iDefindex);

	// 目标是否死亡
	if(CHAR_getInt(iDefindex,CHAR_HP) <= 0){
		if(CHAR_getWorkInt(iDefindex,CHAR_WORKBATTLEFLG ) & CHAR_BATTLEFLG_ABIO) iUltimate = 1;
		else if(CHAR_getInt(iDefindex,CHAR_WHICHTYPE) != CHAR_TYPEPLAYER && (iFlg & BCF_KAISHIN)){
			if(RAND(1,100) < 50) iUltimate = 1;
		}
#ifdef _PETSKILL_LER
		// 雷尔不能被打飞
		if(CHAR_getInt(iDefindex,CHAR_BASEBASEIMAGENUMBER) == 101813 ||
			CHAR_getInt(iDefindex,CHAR_BASEBASEIMAGENUMBER) == 101814) iUltimate = 0;
#endif
		iFlg |= BCF_DEATH;
		if(iUltimate == 1) iFlg |= BCF_ULTIMATE_1;
		if(iUltimate == 2) iFlg |= BCF_ULTIMATE_2;
	}
	else{
#ifdef _TAKE_ITEMDAMAGE
		if(BATTLE_ItemCrushSeq(charaindex,iDefindex,iDamage) == TRUE)
#else
		if(BATTLE_ItemCrushSeq(iDefindex) == TRUE)
#endif
			iFlg |= BCF_CRUSH;
		// 状态命中判定
		if(iDamage > 0 && BATTLE_StatusAttackCheck(charaindex,iDefindex,iEffect,iEffectHit,30,1.0,&iTemp)){
			CHAR_setWorkInt(iDefindex,StatusTbl[iEffect],iTurn);
			if(iEffect == BATTLE_ST_DRUNK) CHAR_setWorkInt(iDefindex,CHAR_WORKDRUNK,CHAR_getWorkInt(iDefindex,CHAR_WORKDRUNK) >> 1);
			// 中以下状态的目标该回合之後不能行动
			if(iEffect == BATTLE_ST_PARALYSIS || iEffect == BATTLE_ST_SLEEP || iEffect == BATTLE_ST_STONE || iEffect == BATTLE_ST_BARRIER)
				CHAR_setWorkInt(iDefindex,CHAR_WORKBATTLECOM1,BATTLE_COM_NONE);
			// 回传client那一个目标要执行状态改变
			if(iGuardian >= 0) BATTLE_BadStatusString(iGuardian,iEffect);	// iGuardian 大於等於 0 表示有目标使用了忠犬,该目标状态改变
			else BATTLE_BadStatusString(pAAttackObject->target,iEffect);	// 否则原目标状态改变
		}
	}

	if(iGuardian >= 0){
		iFlg |= BCF_GUARDIAN;
		snprintf(szCommand,sizeof(szCommand),
			"r%X|i%X|f%X|d%X|p%X|a%X|g%X|",
			pAAttackObject->target,
			pAAttackObject->index,
			iFlg,
			iDamage,
			iPetDamage,
			pAAttackObject->actionNumber,
			iGuardian);
	}else
		snprintf(szCommand,sizeof(szCommand),
			"r%X|i%X|f%X|d%X|p%X|a%X|",
			pAAttackObject->target,
			pAAttackObject->index,
			iFlg,
			iDamage,
			iPetDamage,
			pAAttackObject->actionNumber);
	
	// 把命令送入传给client的buffer
	BATTLESTR_ADD(szCommand);
	
	iTemp = BATTLE_Index2No(battleindex,iDefindex);
	
	// iFlg 已没作用,拿来作别的用途
	iFlg = 0;
	if(iTemp >= SIDE_OFFSET) iFlg = 1;
	
	i = iTemp - iFlg * SIDE_OFFSET;
	if(iUltimate > 0) BattleArray[battleindex].Side[iFlg].Entry[i].flg |= BENT_FLG_ULTIMATE;
	CHAR_setWorkInt(iDefindex,CHAR_NPCWORKINT1,iTemp2);
}

void BATTLE_BattleModel(int battleindex,int attackNo,int myside)
{
	int iToList[SIDE_OFFSET*2+1],i,i0,i1,i2,iEffect = -1,iTurn = -1,iEffectHit = 0,iType,iObjectNum,charaindex;
	int iActionNumber[4] = {-1,-1,-1,-1},iActionAmount = 0;
	char szData[32],szData2[32],*pszOption = NULL;
	AttackObject AAttackObject[10];

	charaindex = BATTLE_No2Index(battleindex,attackNo);
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return;

	pszOption = PETSKILL_getChar(CHAR_getWorkInt(charaindex,CHAR_WORKBATTLECOM3),PETSKILL_OPTION);

	if(pszOption == "\0"){
//		printf("BATTLE_BattleModel: read PETSKILL_OPTION error!!(array:%d,file:%s,line:%d)\n",
//						CHAR_getWorkInt(charaindex,CHAR_WORKBATTLECOM3),__FILE__,__LINE__);
		return;
	}

	// 取得效果
	if(getStringFromIndexWithDelim(pszOption,"|",3,szData,sizeof(szData)) != FALSE){
		for(i=1;i<BATTLE_ST_END;i++){
			if(strncmp(szData,aszStatus[i],2) == 0){
				iEffect = i;
				break;
			}
		}	
	}
	// 有效果
	if(iEffect != -1){
		// 取得回合数
		if(getStringFromIndexWithDelim(pszOption,"|",4,szData,sizeof(szData)) == FALSE)
			// 回合数取得失败
			printf("BATTLE_BattleModel: has setting effect but no turn number!!(file:%s,line:%d)\n",__FILE__,__LINE__);
		else iTurn = atoi(szData);
		// 有回合数
		if(iTurn != -1){
			// 取得命中机率
			if(getStringFromIndexWithDelim(pszOption,"|",5,szData,sizeof(szData)) == FALSE)
				// 命中机率取得失败
				printf("BATTLE_BattleModel: has setting turn number but no hit probability!!(file:%s,line:%d)\n",__FILE__,__LINE__);
			else iEffectHit = atoi(szData);
		}
	}

	// 取得图号(至少要有一个)
	if(getStringFromIndexWithDelim(pszOption,"|",7,szData,sizeof(szData)) == FALSE){
		// 图号取得失败,不动作
		//printf("BATTLE_BattleModel: no picture number!!(file:%s,line:%d)\n",__FILE__,__LINE__);
		BATTLE_NoAction(battleindex,attackNo);
		return;
	}
	else{
		for(i=0;i<4;i++){
			// 取得图号
			if(getStringFromIndexWithDelim(szData," ",i+1,szData2,sizeof(szData2)) != FALSE) iActionNumber[i] = atoi(szData2);
			else break;
		}
		iActionAmount = i;
	}

	memset(iToList,-1,sizeof(iToList));
	memset(AAttackObject,-1,sizeof(AAttackObject));
	if(myside) BATTLE_MultiList(battleindex,TARGET_SIDE_0,iToList);	// 打左上
	else BATTLE_MultiList(battleindex,TARGET_SIDE_1,iToList);				// 打右下

	iType = CHAR_GETWORKINT_LOW(charaindex,CHAR_WORKBATTLECOM2);				// 取得攻击类型
	iObjectNum = CHAR_GETWORKINT_HIGH(charaindex,CHAR_WORKBATTLECOM2);	// 取得攻击物件数量

	i1 = 0;
	// 初始化 AAttackObject
	for(i=0;i<iObjectNum;i++){
		AAttackObject[i].index = i;
		AAttackObject[i].actionNumber = iActionNumber[i1];
		if(++i1 >= iActionAmount) i1 = 0;
	}

	// 检查场上目标剩几只
	for(i=0;i<SIDE_OFFSET;i++){
		if(iToList[i] != -1) continue;
		else break;
	}

	sprintf(szData,"Bb|a%X|",attackNo);
	BATTLESTR_ADD(szData);
	// 判断目标数量和攻击物件数量
	// 当攻击物件数量大於等於在场敌方数量，则攻击所有目标，有的目标可能受到二次以上的攻击
	if(iObjectNum >= i){
		i0 = i;		// 记录场上目标只数

		for(i=0;i<iObjectNum;i++){
			// 若 iToList[i] 为 -1 表示已打完所有的目标
			if(iToList[i] == -1) break;
			// 设定攻击目标内容
			if(AAttackObject[i].index != -1){
				AAttackObject[i].target = iToList[i];
				BATTLE_BattleModel_ATTACK(battleindex,charaindex,&AAttackObject[i],iEffect,iTurn,iEffectHit,iType);
			}
		}
		// 还有攻击物件没有目标可以攻击,进行随机目标攻击
		for(;i<iObjectNum;i++){
			// 设定攻击目标内容
			if(AAttackObject[i].index != -1){
				AAttackObject[i].target = iToList[RAND(0,i0-1)];
				BATTLE_BattleModel_ATTACK(battleindex,charaindex,&AAttackObject[i],iEffect,iTurn,iEffectHit,iType);
			}
		}
	}
	// 当攻击物件数量小於在场目标数量，则以攻击类型决定攻击方式
	else{
		i0 = i;		// 记录场上目标只数
		// 型态 2 :每个攻击物件打完该攻击目标後便不再攻击其它目标,每个目标只会受一次攻击
		// 当iType & 0x00000001 为 true 之外的值都以型态 2 处理
		for(i=0;i<iObjectNum;i++){
			// 设定攻击目标内容
			if(AAttackObject[i].index != -1){
				AAttackObject[i].target = iToList[i];
				BATTLE_BattleModel_ATTACK(battleindex,charaindex,&AAttackObject[i],iEffect,iTurn,iEffectHit,iType);
			}
		}
		// 型态 1 :每个目标都要打到,会有一个攻击物件打二个目标以上的情形,所以若为型态 1 的话,除了型态 2 的攻击处理要执外,要再作以下处理
		if(iType & 0x00000001){
			// 处理还没有被打的目标
			// i0 - iObjectNum 为剩下没打的目标数量
			for(i1=0;i1<i0 - iObjectNum;i1++){	// 计算剩几个目标没打
				for(i2=0;i2<iObjectNum;i2++){			// 攻击物件计数
					// 若 iToList[i] 为 -1 表示已打完所有的目标
					if(iToList[i] == -1) break;			// 这里的 i 保留型态 2 打完後的值
					// 设定攻击目标内容
					if(AAttackObject[i2].index != -1){
						AAttackObject[i2].target = iToList[i];
						BATTLE_BattleModel_ATTACK(battleindex,charaindex,&AAttackObject[i2],iEffect,iTurn,iEffectHit,iType);
						i++;	// 换下一个目标
					}
				}
			}
		}
	}
	BATTLESTR_ADD("FF|");
}
#endif

#ifdef _OFFLINE_SYSTEM
int	OFFLINE_MultiRessurect( int battleindex, int attackNo, int toNo, int HP, int flg)
{

	int charaindex=-1;

	if( BATTLE_CHECKINDEX( battleindex ) == FALSE ) return FALSE;
	charaindex = BATTLE_No2Index( battleindex, attackNo );	//攻方index
	if( !CHAR_CHECKINDEX( charaindex))	return FALSE;

	if( flg == 0 ){
		BATTLE_MultiRecovery( battleindex, attackNo, toNo,
				BD_KIND_HP, HP,  1, MAGIC_EFFECT_USER, SPR_heal3 );
	}else if( flg == 1 ){
		BATTLE_MultiRecovery( battleindex, attackNo, toNo,
				BD_KIND_MP, HP,  1, MAGIC_EFFECT_USER, SPR_heal3 );
	}else if( flg == 2 ){
	  BATTLE_MultiRessurect( battleindex, attackNo, toNo, HP,  1, MAGIC_EFFECT_USER, SPR_fukkatu3 );
	}
	return TRUE;
}
#endif
