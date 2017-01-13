#ifndef  __PET_H__
#define __PET_H__

/*
 * 矢永玄楮  及涩烂匹允［
 */
#include "version.h"
#include "common.h"

#define PETFEEDTIME	60*60	// 宠物蛋喂养时间(秒)
//#define PETFEEDTIME	5	// 宠物蛋喂养时间(秒)

int PET_DEBUG_initPetOne( int charaindex);
int PET_dropPet( int charaindex, int havepetindex);
int PET_dropPetFollow( int charaindex, int havepetindex, int fl, int x, int y);
int PET_dropPetFLXY( int charaindex, int havepetindex, int fl, int x, int y);
int PET_dropPetAbsolute( int petindex, int floor, int x, int y,BOOL net);
int PET_createPetFromCharaIndex( int charaindex, int enemyindex);
BOOL PET_SelectBattleEntryPet( int charaindex, int petarray);
int PET_initCharOneArray( Char *ch);
BOOL PET_isPutPoint( int fl,int x, int y);


BOOL PET_getBaseForAllocpoint( int toindex, int *work);
void PET_showEditBaseMsg( int charaindex, int toindex, int itemindex, int *work);

#ifdef _PET_EVOLUTION
BOOL PET_getBaseAndSkill( int charaindex, int baseindex, int *base, int *skill, int flg);
#endif
BOOL CHAR_DelPetForIndex( int charaindex, int petindex);

#endif

#ifdef _PET_AMOI__E
//typedef enum 
//{
//	PET_NOT_LIMIT = 0,//不限制
//	PET_NOT_DropTradeStreetVandor = 1,//不可丢弃不可交易
//	PET_NOT_Drop = 2,//不可丢弃
//	PET_NOT_Trade = 3,//不可交易
//	PET_NOT_StreetVandor = 4,//不可用此开店
//	PET_NOT_TradeAndStreetVandor = 5,//不可用此开店
//}PET_DUALTYPE;

#define PET_NOT_LIMIT        0x00000000
#define PET_NOT_DROP         0x00000010
#define PET_NOT_STREETVANDOR 0x00000100
#define PET_NOT_TRADE        0x00001000
#define PET_NOT_MAIL         0x00010000

#endif
