0032:npctemplate.h:0DDE:1:14=48041e4b:16=47d21313:#ifndef __READNPCTEMPLATE_H__
#define __READNPCTEMPLATE_H__

#include "util.h"
#include "char_base.h"

#define NPC_TEMPLATEFILEMAGIC   "NPCTEMPLATE\n"

#undef EXTERN
#ifdef __NPCTEMPLATE__
#define EXTERN
#else
#define EXTERN extern
#endif /*__NPCTEMPLATE__*/



typedef struct tagNPC_haveItem
{
    int     itemnumber;
    int     haverate;
    int     havenum;
}NPC_haveItem;

typedef enum
{
    NPC_TEMPLATENAME,           /*    件皿伊□玄及  蟆  */
    NPC_TEMPLATECHARNAME,       /*    蟆    */

    NPC_TEMPLATEINITFUNC,       /*  CHAR_INITFUNC卞垫仁 */
    NPC_TEMPLATEWALKPREFUNC,    /*  CHAR_WALKPREFUNC    */
    NPC_TEMPLATEWALKPOSTFUNC,   /*  CHAR_WALKPOSTFUNC   */
    NPC_TEMPLATEPREOVERFUNC,    /*  CHAR_PREOVERFUNC    */
    NPC_TEMPLATEPOSTOVERFUNC,   /*  CHAR_POSTOVERFUNC   */
    NPC_TEMPLATEWATCHFUNC,      /*  CHAR_WATCHFUNC      */
    NPC_TEMPLATELOOPFUNC,       /*  CHAR_LOOPFUNC   */
    NPC_TEMPLATEDYINGFUNC,      /*  CHAR_DYINGFUNC  */
    NPC_TEMPLATETALKEDFUNC,     /*  CHAR_TALKEDFUNC */

    NPC_TEMPLATEPREATTACKEDFUNC,        /*  CHAR_PREATTACKEDFUNC    */
    NPC_TEMPLATEPOSTATTACKEDFUNC,       /*  CHAR_POSTATTACKEDFUNC    */

    NPC_TEMPLATEOFFFUNC,                /*  CHAR_OFFFUNC    */
    NPC_TEMPLATELOOKEDFUNC,            /*  CHAR_LOOKEDFUNC  */
    NPC_TEMPLATEITEMPUTFUNC,            /*  CHAR_ITEMPUTFUNC    */

    NPC_TEMPLATESPECIALTALKEDFUNC,    /*  CHAR_SPECIALTALKEDFUNC   */
    NPC_TEMPLATEWINDOWTALKEDFUNC,    /*  CHAR_WINDOWTALKEDFUNC   */
#ifdef _USER_CHARLOOPS
	NPC_TEMPLATELOOPFUNCTEMP1,		//CHAR_LOOPFUNCTEMP1,
	NPC_TEMPLATELOOPFUNCTEMP2,		//CHAR_LOOPFUNCTEMP2,
	NPC_TEMPLATEBATTLEPROPERTY,		//CHAR_BATTLEPROPERTY,
#endif
    NPC_TEMPLATECHARNUM,
}NPC_TEMPLATECHAR;

typedef enum
{
    NPC_TEMPLATEMAKEATNOBODY,           /* 簿手中卅中凛卞手综月井升丹井 */
    NPC_TEMPLATEMAKEATNOSEE,            /* 苇尹卅中赭匹综月井升丹井    */
    NPC_TEMPLATEIMAGENUMBER,            /*   飓  寞    */
    NPC_TEMPLATETYPE,                   /*     瑁户月凛卞瑁户月        */

    NPC_TEMPLATEMINHP,                  /* HP   */

    NPC_TEMPLATEMINMP,                  /* MP   */

    NPC_TEMPLATEMINSTR,                 /* STR  */

    NPC_TEMPLATEMINTOUGH,               /* TOUGH    */

    NPC_TEMPLATEISFLYING,               /*  氏匹月井升丹井  */

    NPC_TEMPLATEITEMNUM,                /*   切丹月失奶  丞及    醒    */

    NPC_TEMPLATELOOPFUNCTIME,           /*
                                         * 窒立伉  仍午卞伙□皿楮醒
                                         * 毛裟少井
                                         */
    NPC_TEMPLATEFUNCTIONINDEX,         /*
                                        * fucntionSet 及窒    及
                                        * 奶件犯永弁旦井
                                        */

    NPC_TEMPLATEINTNUM,
}NPC_TEMPLATEINT;

typedef struct tagNPC_Template
{
    STRING64    chardata[NPC_TEMPLATECHARNUM];
    int         intdata[NPC_TEMPLATEINTNUM];
    int         randomdata[NPC_TEMPLATEINTNUM]; /*  仿件母丞及  五今
                                                    互  匀化中月  */
    int         hash;
    NPC_haveItem*   haveitem;
}NPC_Template;


EXTERN NPC_Template*   NPC_template;
EXTERN int             NPC_templatenum;
EXTERN int             NPC_template_readindex;

INLINE int NPC_CHECKTEMPLATEINDEX(int index);

BOOL NPC_copyFunctionSetToChar( int id, Char* ch );

BOOL NPC_readNPCTemplateFiles( char* topdirectory ,int templatesize);
int NPC_templateGetTemplateIndex( char* templatename );

#endif
 /*__READNPCTEMPLATE_H__*/
002E:npcutil.h:0D63:1:14=48041e4b:16=47d21313:#ifndef _NPCUTIL_H_
#define _NPCUTIL_H_

BOOL NPC_Util_AddOneTitle( int charindex, int titleindex );
BOOL NPC_Util_HaveTitle( int charindex , int titleindex );
BOOL NPC_Util_Nearby( int x1 , int y1, int x2 , int y2 );
BOOL NPC_Util_CharNearby(int ind1,int ind2);
int NPC_Util_YN(char *input );
int NPC_Util_getDirFromTwoPoint( POINT* pstart, POINT* pend );
int NPC_Util_countHaveItem( int meindex , int itemid );
BOOL NPC_Util_isBackContact( int frontindex , int backindex );

void NPC_Util_AnnounceFloor( int floorid , char *msg );


BOOL NPC_Util_moveItemToChar( int charindex, int itemindex,BOOL net );
BOOL NPC_Util_createItemToChar( int charindex, int itemid , BOOL net);
int NPC_Util_CharDistance( int index1, int index2 );
int NPC_Util_SearchNear( int meindex, int maxlen, int type );
int NPC_Util_SearchNearPlayer( int meindex, int maxlen );
int NPC_Util_SearchNearEnemy( int meindex, int maxlen );
int NPC_Util_SuberiWalk( int index,	int	dir );
int NPC_Util_GetNumFromArg( int meindex, char* in);

int NPC_Util_GetDirCharToChar( int fromindex, int toindex, int mode);
int NPC_Util_WalkCharToChar( int fromindex, int toindex, int mode, int suberi);
BOOL NPC_Util_isFaceToFace( int index1, int index2, int distance );
BOOL NPC_Util_isFaceToChara( int index1, int index2, int distance );
BOOL NPC_Util_charIsInFrontOfChar( int index1, int index2, int distance );

int NPC_Util_SearchItemInChar( int charindex , int itemindex);
int NPC_Util_GiveAllItemToChar( int give , int take );
/*
int NPC_Util_ControlOtherNPC( CHAR_TYPE chartype ,
                              char *npcname,
                              char *command );
*/
void NPC_Util_NPCDelete( int srcindex );
BOOL NPC_Util_moveItemToMap( int itemindex , int fl , int x , int y,
                             BOOL net );
char *NPC_Util_GetArgStr( int index, char *argstr, int len);
int NPC_Util_GetNumFromStrWithDelim( char *srcstr, char* in);
char *NPC_Util_GetStrFromStrWithDelim( char *srcstr, char *srhstr,
										char *buf, int len);
inline double NPC_Util_sellRate( int seller );
inline double NPC_Util_buyRate( int buyer );
BOOL NPC_Util_IsVisiblePlayer( int meindex);
BOOL NPC_Util_WordInclude( char *text , char *word );
void NPC_Util_RandomToken(char *in, char *out, int outbufsize );

void cutDotsTail( char *s );
int NPC_Util_FrontItem( int meindex );
void NPC_Util_Boss2KingStart( int bossindex );
int NPC_Util_FrontChar( int meindex );
int *NPC_Util_getEnemy( int meindex, int charaindex);

void  NPC_NowEndEventSetFlgCls(int talker,int shiftbit);
void  NPC_EventSetFlg(int talker,int shiftbit);
BOOL NPC_EventCheckFlg(int point,int shiftbit);
void  NPC_NowEventSetFlg(int talker,int shiftbit);
void  NPC_NowEventSetFlgCls(int talker,int shiftbit);
BOOL NPC_NowEventCheckFlg(int point,int shiftbit);
void  NPC_EndEventSetFlgCls(int talker,int shiftbit);
char *NPC_Util_CheckAssignArgFile( int index, char *filename);

// CoolFish: Family Adv 2001/8/4
void AddFMAdv(int talker, int shiftbit);

// Robin 0817 family income
int addNpcFamilyTax( int meindex, int talkerindex, int income );

#define		NPC_ENEMY_ENEMYNUMBER		10		/* 请蜇衬    醒 */

/* 娄醒白央奶伙  心  心及ㄠ垫及  侬醒晓蜃 */
#define NPC_UTIL_GETARGSTR_LINEMAX	4096
/* 娄醒白央奶伙及  心  心田永白央扔奶术*/
#ifdef _NEWEVENT
//#define NPC_UTIL_GETARGSTR_BUFSIZE	1024*1200
#define NPC_UTIL_GETARGSTR_BUFSIZE	1024*32
#else
#define NPC_UTIL_GETARGSTR_BUFSIZE	1024*12
#endif
#endif



0031:npc_action.h:014C:1:14=48041e4b:16=47d21313: