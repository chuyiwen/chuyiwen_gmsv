#ifndef  __PET_H__
#define __PET_H__

/*
 * ʸ������  ��ɬ��ƥ�ʣ�
 */
#include "version.h"
#include "common.h"

#define PETFEEDTIME	60*60	// ���ﵰι��ʱ��(��)
//#define PETFEEDTIME	5	// ���ﵰι��ʱ��(��)

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
//	PET_NOT_LIMIT = 0,//������
//	PET_NOT_DropTradeStreetVandor = 1,//���ɶ������ɽ���
//	PET_NOT_Drop = 2,//���ɶ���
//	PET_NOT_Trade = 3,//���ɽ���
//	PET_NOT_StreetVandor = 4,//�����ô˿���
//	PET_NOT_TradeAndStreetVandor = 5,//�����ô˿���
//}PET_DUALTYPE;

#define PET_NOT_LIMIT        0x00000000
#define PET_NOT_DROP         0x00000010
#define PET_NOT_STREETVANDOR 0x00000100
#define PET_NOT_TRADE        0x00001000
#define PET_NOT_MAIL         0x00010000

#endif
