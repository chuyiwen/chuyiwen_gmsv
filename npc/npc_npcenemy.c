#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_npcenemy.h"
#include "battle.h"
#include "enemy.h"
#include "readmap.h"
#include "encount.h"
#include "lssproto_serv.h"
#include "configfile.h"
#include "anim_tbl.h"
#include "handletime.h"
#include "npc_eventaction.h"
extern char hanzibuf[5000][8];
enum {
	CHAR_WORK_ENCOUNTTYPE	= CHAR_NPCWORKINT1,	/* 巨件市它件玄及酷   */
	CHAR_WORK_DIEACT		= CHAR_NPCWORKINT2,	/* 韶氏分凛升丹允月井 */
	CHAR_WORK_WARP_FLOOR	= CHAR_NPCWORKINT3,	/* 伐□皿允月白夫失*/
	CHAR_WORK_WARP_X		= CHAR_NPCWORKINT4,	/* 伐□皿允月X */
	CHAR_WORK_WARP_Y		= CHAR_NPCWORKINT5,	/* 伐□皿允月Y */
	CHAR_WORK_ONEBATTLE 	= CHAR_NPCWORKINT6,	/* 域勾仄井爵  仄卅中 */
	NPC_TIME_MODE			= CHAR_NPCWORKINT7, 
	CHAR_WORK_BASEIMGBAK	= CHAR_NPCWORKINT8, /* 手午及  飓  寞 */
	CHAR_WORK_DIETIME		= CHAR_NPCWORKINT9, /* 韶氏分凛棉 */
	CHAR_WORK_REVIVALTIME	= CHAR_NPCWORKINT10, /* 戏五忒月凛棉 */
	CHAR_WORK_BATTLETYPE	= CHAR_NPCWORKINT11,
#ifdef	_WARNPC_CTRL
	CHAR_WORK_BATTLEINGTIME = CHAR_NPCWORKINT12,
#endif
};

#define		NPC_ENEMY_REVIVALTIMA_DEFAULT	120

static int gymbody[] = {
	SPR_001em,SPR_011em,SPR_021em,SPR_031em,SPR_041em,SPR_051em,
	SPR_061em,SPR_071em,SPR_081em,SPR_091em,SPR_101em,SPR_111em,

	SPR_002em,SPR_012em,SPR_022em,SPR_032em,SPR_042em,SPR_052em,
	SPR_062em,SPR_072em,SPR_082em,SPR_092em,SPR_102em,SPR_112em,

	SPR_003em,SPR_013em,SPR_023em,SPR_033em,SPR_043em,SPR_053em,
	SPR_063em,SPR_073em,SPR_083em,SPR_093em,SPR_103em,SPR_113em,

	SPR_004em,SPR_014em,SPR_024em,SPR_034em,SPR_044em,SPR_054em,
	SPR_064em,SPR_074em,SPR_084em,SPR_094em,SPR_104em,SPR_114em,
};

#ifdef _NEW_WARPMAN
static BOOL NPC_NPCEnemy_CheckFree( int  meindex, int toindex, BOOL *Party);
BOOL NPC_WarpAllMsg(int meindex,int toindex );
BOOL NPCEnemy_CheckFree( int meindex, int talker, char *buf);
BOOL NPCEnemy_BSCheck(int meindex,int talker,char* buf);
BOOL NPCEnemy_FreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg, int temp);
BOOL NPCEnemy_WarpManReduce(int meindex,int talker,char *buf);
BOOL NPCEnemy_BigSmallLastCheck(int point1,int mypoint,int flg);
BOOL NPCEnemy_CheckTrans(int meindex,int talker,int trans,int flg);
BOOL NPCEnemy_LevelCheck(int meindex,int talker,int level,int flg);
BOOL NPCEnemy_CheckMyPet( int meindex, int talker, int petLv, int flg, int petid);
BOOL NPCEnemy_ItemCheck(int meindex,int talker,int itemNo,int flg);
#endif
#ifdef _NPC_REPLACEMENT
void Check_EnemyWarpMe( int meindex, char *args );
#endif
static int NPC_NPCEnemy_StealItem( char *argstr, int meindex, int charaindex);

BOOL NPC_NPCEnemyInit( int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];
	int		tmp, gym;
#ifdef	_NPCENEMY_1
	char token1[128];
#endif

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL )	{
		print("Can't GetArgStr !!");
		return FALSE;
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sktype", buf, sizeof( buf)) != NULL ){
		CHAR_setWorkInt( meindex, CHAR_WORK_BATTLETYPE, atoi( buf));
	}else{
		CHAR_setWorkInt( meindex, CHAR_WORK_BATTLETYPE, 0);
	}

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "enemyno", buf, sizeof( buf)) == NULL ){
		char filename[256];
		NPC_Util_CheckAssignArgFile( meindex, filename);
		print( "NPCEnemy: nothing parameter [enemyno]:\n%s\n", filename);
		return FALSE;
	}else {
		int		i;
		for( i = 0; i < NPC_ENEMY_ENEMYNUMBER; i ++ ) {
			int		curEnemy;
			char	data[128];
			int		ret;
			ret = getStringFromIndexWithDelim( buf,",", i + 1, data, sizeof(data));
			if( ret == FALSE)
				break;
			curEnemy = ENEMY_getEnemyArrayFromId( atoi( data));
			if( !ENEMY_CHECKINDEX( curEnemy) ) {
				char filename[256];
				NPC_Util_CheckAssignArgFile( meindex, filename);
				print( "NPCEnemy: invalid param [enemyno:%d data:%s curEnemy:%d]\nfile:%s\n",i+1, data, curEnemy, filename );
				return FALSE;
			}
		}
	}
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "steal", buf, sizeof( buf)) != NULL ) {
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "item", buf, sizeof( buf)) == NULL ) {
			print( "NPCEnemy:有指定steal而没有指定item，很奇怪\n");
			return FALSE;
		}
	}

	gym = NPC_Util_GetNumFromStrWithDelim( argstr, "gym");

	if( gym > 0 ){
		CHAR_setInt( meindex, CHAR_BASEBASEIMAGENUMBER,
			gymbody[RAND( 0, arraysizeof( gymbody ) - 1)] );
		CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER,
			CHAR_getInt( meindex, CHAR_BASEBASEIMAGENUMBER ) );
		CHAR_setInt( meindex, CHAR_LV, gym );
	}
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "dieact");
	if( tmp != 1 ) 
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_DIEACT, tmp);
	if( strstr( argstr, "NEWNPCENEMY") != 0 )	{
		CHAR_setWorkInt( meindex, CHAR_WORK_WARP_FLOOR, 0 );
		CHAR_setWorkInt( meindex, CHAR_WORK_WARP_Y, 0 );
		CHAR_setWorkInt( meindex, CHAR_WORK_WARP_X, 0 );
	}else	{
		if( CHAR_getWorkInt( meindex, CHAR_WORK_DIEACT) == 1 ) {
			char buff1[256];
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "warpfl", buff1, sizeof( buff1) ) == NULL) {
				print( "NPCENEMY: nothing warp_floor %s\n", buff1);
				return FALSE;
			}
			tmp = atoi( buff1);
			CHAR_setWorkInt( meindex, CHAR_WORK_WARP_FLOOR, tmp);
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "warpx", buff1, sizeof( buff1) ) == NULL) {
				print( "NPCENEMY: nothing warp_x\n");
				return FALSE;
			}
			tmp = atoi( buff1);
			CHAR_setWorkInt( meindex, CHAR_WORK_WARP_X, tmp);
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "warpy", buff1, sizeof( buff1) ) == NULL) {
				print( "NPCENEMY: nothing warp_y\n");
				return FALSE;
			}
			tmp = atoi( buff1);
			CHAR_setWorkInt( meindex, CHAR_WORK_WARP_Y, tmp);
			if( MAP_IsValidCoordinate( CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y) ) == FALSE ){
				char filename[256];
				NPC_Util_CheckAssignArgFile( meindex, filename);
				print( "NPCENEMY: %s: .\n %s.\nInvalid fl=%d,x=%d,y=%d\n", filename, argstr,
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y) );
				return FALSE;
			}
		}
	}
	//andy_end
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "entype");
	if( tmp != 1 && tmp != 2)
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE, tmp);

	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "onebattle");
	if( tmp != 1 )
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_ONEBATTLE, tmp);

	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "time");
	if( tmp == -1 )
		tmp = NPC_ENEMY_REVIVALTIMA_DEFAULT;
	CHAR_setWorkInt( meindex, CHAR_WORK_REVIVALTIME, tmp);
	

	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPENPCENEMY );
	CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_ENEMY);
	CHAR_setWorkInt( meindex, CHAR_WORK_BASEIMGBAK,
						CHAR_getInt( meindex, CHAR_BASEIMAGENUMBER));
	return TRUE;
}

void NPC_NPCEnemyTalked( int meindex , int talkerindex , char *szMes ,
					 int color )
{
	char	npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[256];

	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
		return;
	}
	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof( npcarg)) == NULL )	{
		print("Can't GetArgStr !!");
		return;
	}
	if( NPC_Util_GetStrFromStrWithDelim( npcarg, "NEWTIME", buf, sizeof( buf)) != NULL ){
		if( CHAR_getWorkInt( meindex, NPC_TIME_MODE) <= 0 )	{
			if( NPC_Util_GetStrFromStrWithDelim( npcarg, "Time_Msg", buf, sizeof( buf) ) != NULL)	{
				CHAR_talkToCli( talkerindex, meindex, buf, CHAR_COLORYELLOW);
			}
			return;
		}
	}
	if( !NPC_Util_charIsInFrontOfChar( talkerindex, meindex, 1 ))
		return; 
	if( CHAR_getWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE) >= 1 ) {
		if(strstr(npcarg,"STARTRAND")!=NULL){
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep);
		timep = mktime(p);
		char *randtypetime = CHAR_getChar(talkerindex,CHAR_RANDTYPE);
		char tempbuff[64];
		int randtype = 0;
		int randtime = 0;
		int randwrong = 0;
		if(strlen(randtypetime)>2){
			if(getStringFromIndexWithDelim(randtypetime,"-", 3, tempbuff, sizeof(tempbuff)) != FALSE){
				if(getStringFromIndexWithDelim(randtypetime,"-", 1, tempbuff, sizeof(tempbuff))== FALSE)
					return;
				randwrong = atoi(tempbuff);
				if(getStringFromIndexWithDelim(randtypetime,"-", 2, tempbuff, sizeof(tempbuff))== FALSE)
					return;
				randtype = atoi(tempbuff);
				if(getStringFromIndexWithDelim(randtypetime,"-", 3, tempbuff, sizeof(tempbuff))== FALSE)
					return;
				randtime = atoi(tempbuff);
#ifdef _MM_NO_JIAOBEN
				if(timep - randtime < getEditBaseTime()*randwrong){
					char errbuf[256];
					sprintf(errbuf,"您还有%d秒才可以继续此操作！",getEditBaseTime()*randwrong-(timep - randtime));
					CHAR_talkToCli(talkerindex, -1, errbuf, CHAR_COLORRED);
					return;
				}
#endif
			}
		}
//		if(timep-CHAR_getWorkInt(talkerindex,CHAR_WORKRANDEDITBASETIME)>=getEditBaseTime())
//			CHAR_setWorkInt(talkerindex,CHAR_WORKRANDEDITBASETIME,timep);
//		else{
//			char errbuf[256];
//			sprintf(errbuf,"您还有%d秒才可以继续此操作！",getEditBaseTime()-(timep-CHAR_getWorkInt(talkerindex,CHAR_WORKRANDEDITBASETIME)));
//			CHAR_talkToCli(talkerindex, -1, errbuf, CHAR_COLORRED);
//			return;
//		}
		char jiabuf[][5]={"加","╋","╉","╈","╂","十","╃"};
		char jianbuf[][5]={"减","一","-","—","-","—","━"};
		char chengbuf[][5]={"乘","*","x","X","Ｘ","ｘ","＊"};
		char numbuf[][5]={"０","１","２","３","４","５","６","７","８","９"};
		char arg[255];
		int i,j,k,l,m;
		char buf[256];
		i = RAND(30,70);
		j = RAND(1,29);
		l = RAND(1,7);
		m = RAND(1,3);
		char *typebuf;
//		if(randtype == 0){
//			randtype = RAND(1,10);
//			if(randtype<=5){
//				randtype = RAND(1,10);
//				if(randtype<=3) randtype=1;
//				else randtype=2;
//			}else{
//				randtype = RAND(1,10);
//				if(randtype<=3) randtype=3;
//				else randtype=4;
//			}
//		}
		randtype = 4;
		{
//			sprintf(arg,"%s",sasql_rand_buf());
//			if(strcmp(arg,"err")==0) return;
			char tempbuf[64];
			char randquestion[64];
			char randrightanswer[64];
			char randwronganswer1[64];
			char randwronganswer2[64];
			char randwronganswer3[64];
			char randwronganswer4[64];
//			if(getStringFromIndexWithDelim(arg,"|", 1, tempbuf, sizeof(tempbuf)) == FALSE)
//				return;
//			strcpy(randquestion,tempbuf);
//			if(getStringFromIndexWithDelim(arg,"|", 2, tempbuf, sizeof(tempbuf)) == FALSE)
//				return;
//			strcpy(randrightanswer,tempbuf);
//			if(getStringFromIndexWithDelim(arg,"|", 3, tempbuf, sizeof(tempbuf)) == FALSE)
//				return;
//			strcpy(randwronganswer1,tempbuf);
//			if(getStringFromIndexWithDelim(arg,"|", 4, tempbuf, sizeof(tempbuf)) == FALSE)
//				return;
//			strcpy(randwronganswer2,tempbuf);
//			if(getStringFromIndexWithDelim(arg,"|", 5, tempbuf, sizeof(tempbuf)) == FALSE)
//				return;
//			strcpy(randwronganswer3,tempbuf);
//			if(getStringFromIndexWithDelim(arg,"|", 6, tempbuf, sizeof(tempbuf)) == FALSE)
//				return;
//			strcpy(randwronganswer4,tempbuf);
			memset(tempbuf, 0, sizeof(tempbuf));
			memset(randquestion, 0, sizeof(randquestion));
			memset(randrightanswer, 0, sizeof(randrightanswer));
			memset(randwronganswer1, 0, sizeof(randwronganswer1));
			memset(randwronganswer2, 0, sizeof(randwronganswer2));
			memset(randwronganswer3, 0, sizeof(randwronganswer3));
			memset(randwronganswer4, 0, sizeof(randwronganswer4));
			if(RAND(1,100)<=75){
					char randtmpbuf[4][10];
					sprintf(randtmpbuf[0],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randtmpbuf[1],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randtmpbuf[2],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randtmpbuf[3],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randquestion,"%s",str_hanzi_ganrao(randtmpbuf[0],randtmpbuf[1],randtmpbuf[2],randtmpbuf[3],0,0,0,0,0,0));
					strcat(randquestion,"（请找出相同词组）");
					int newrand1 = RAND(1,100)-1;
					int newrand2 = RAND(1,100)-1;
					int newrand3 = RAND(1,100)-1;
					int newrand4 = RAND(1,100)-1;
					int newrand5 = RAND(1,100)-1;
					sprintf(randrightanswer,"%s",str_hanzi_ganrao(randtmpbuf[0],randtmpbuf[1],randtmpbuf[2],randtmpbuf[3],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer1,"%s",str_hanzi_ganrao(randtmpbuf[0],randtmpbuf[1],randtmpbuf[3],randtmpbuf[2],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer2,"%s",str_hanzi_ganrao(randtmpbuf[3],randtmpbuf[1],randtmpbuf[2],randtmpbuf[0],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer3,"%s",str_hanzi_ganrao(randtmpbuf[1],randtmpbuf[0],randtmpbuf[2],randtmpbuf[3],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer4,"%s",str_hanzi_ganrao(randtmpbuf[2],randtmpbuf[3],randtmpbuf[0],randtmpbuf[1],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					
			}else{
				int tii;
				char timubuf[36][5] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9"};
				for(tii=0;tii<5;tii++){
					if(tii==0)
						strcpy(tempbuf,timubuf[RAND(1,36)-1]);
					else{
						int timunum = 0;
						while(1){
							timunum = RAND(1,36)-1;
							if(strstr(tempbuf,timubuf[timunum])==NULL)
								break;
						}
						strcat(tempbuf,timubuf[timunum]);
					}
				}
				sprintf(randwronganswer1,"[%c%c%c%c%c]",tempbuf[0],tempbuf[2],tempbuf[1],tempbuf[3],tempbuf[4]);
				sprintf(randwronganswer2,"[%c%c%c%c%c]",tempbuf[0],tempbuf[1],tempbuf[4],tempbuf[2],tempbuf[3]);
				sprintf(randwronganswer3,"[%c%c%c%c%c]",tempbuf[4],tempbuf[1],tempbuf[2],tempbuf[0],tempbuf[3]);
				sprintf(randwronganswer4,"[%c%c%c%c%c]",tempbuf[1],tempbuf[0],tempbuf[2],tempbuf[3],tempbuf[4]);
				sprintf(randquestion,"[%s]（请找出相同英文）",tempbuf);
				sprintf(randrightanswer,"[%s]",tempbuf);
				char* randstr = NULL;
				randstr = str_ganrao(randquestion);
				strcpy(randquestion,randstr);
				randstr = str_ganraoan(randrightanswer);
				strcpy(randrightanswer,randstr);
				randstr = str_ganraoan(randwronganswer1);
				strcpy(randwronganswer1,randstr);
				randstr = str_ganraoan(randwronganswer2);
				strcpy(randwronganswer2,randstr);
				randstr = str_ganraoan(randwronganswer3);
				strcpy(randwronganswer3,randstr);
				randstr = str_ganraoan(randwronganswer4);
				strcpy(randwronganswer4,randstr);
			}
				int n;
				int randnum1;
				m=1;
				char tempret[5][64];
				randnum1 = RAND(1,5);
				int rightnum,wrongnum1,wrongnum2,wrongnum3,wrongnum4;
				rightnum = RAND(getRandMin(),getRandMax());
				wrongnum1 = RAND(getRandMin(),getRandMax());
				wrongnum2 = RAND(getRandMin(),getRandMax());
				wrongnum3 = RAND(getRandMin(),getRandMax());
				wrongnum4 = RAND(getRandMin(),getRandMax());
				//char *tempbuff2 = "ABCDE";
				int qianhounum = RAND(1,100);
				for(n=1;n<=5;n++){
					if(n==randnum1){
						if(qianhounum<=50)
							sprintf(tempret[n-1],"选项( %d %s )",rightnum,randrightanswer);
						else
							sprintf(tempret[n-1],"选项( %s %d )",randrightanswer,rightnum);
					}else{
						if(m==1){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum1,randwronganswer1);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer1,wrongnum1);
							m = 2;
						}else if(m==2){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum2,randwronganswer2);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer2,wrongnum2);
							m = 3;
						}else if(m==3){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum3,randwronganswer3);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer3,wrongnum3);
							m = 4;
						}else if(m==4){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum4,randwronganswer4);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer4,wrongnum4);
							m = 5;
						}
					}
				}
			int windowtype = 0;
				int selecttype = RAND(1,100);
				if(selecttype<=30){
					sprintf(buf,"%d|%d",rightnum,meindex);
					windowtype = WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT;
				}else{
					sprintf(buf,"%d|%d",randnum1,meindex);
					windowtype = WINDOW_MESSAGETYPE_SELECT;
				}
			CHAR_setWorkChar(talkerindex, CHAR_WORKRANDMSG, buf);
			int fd = getfdFromCharaIndex( talkerindex );
			sprintf(buf,"%s\n%s\n%s\n%s\n%s\n%s\n输入正确答案括号内的5位数字或者点选答案",randquestion,tempret[0],tempret[1],tempret[2],tempret[3],tempret[4]);
			lssproto_WN_send( fd, windowtype, 
				WINDOW_BUTTONTYPE_OK,
				CHAR_WINDOWTYPE_NPCENEMY_RAND_MSG,
				-1,
				buf);
			CHAR_setWorkInt(talkerindex,CHAR_WORKLUANPCINT,0);
		}
		if(randwrong<=0){
			sprintf(tempbuff,"1-%d-%d",randtype,timep);
		}else{
			sprintf(tempbuff,"%d-%d-%d",randwrong*3,randtype,timep);
		}
		CHAR_setChar(talkerindex,CHAR_RANDTYPE,tempbuff);
		return;
	}
		NPC_NPCEnemy_Encount( meindex, talkerindex, 1);
	}
}

int NPC_NPCEnemy_Encount( int meindex, int charaindex, int mode)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[512];
	BOOL	flg = TRUE;
	int		battlemax = getBattlenum();
	int		i;
	if( CHAR_getInt( meindex, CHAR_BASEIMAGENUMBER) == 0 ) {
		return FALSE;
	}
	if( mode == 0 ) {
		if( CHAR_getWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE) == 1 ) {
			flg = FALSE;
		}
	}else if( mode == 1 ) {
		if( CHAR_getWorkInt( meindex, CHAR_WORK_ENCOUNTTYPE) == 0 ) {
			flg = FALSE;
		}
	}

	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

	if( !flg) {
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "deniedmsg", buf, sizeof( buf)) != NULL )	{
			CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
		}
		return FALSE;
	}
	flg = FALSE;
	while( 1 ) {
		char	buf[1024];
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "item", buf, sizeof( buf)) != NULL ) {
			char	data[128];
			BOOL	found = FALSE;

			for( i = 0; i < CHAR_MAXITEMHAVE; i ++ ) {
				int		j;
				int		itemid;
				int ret = getStringFromIndexWithDelim( buf,",", i + 1,data, sizeof(data));
				if( ret == FALSE)
					break;
				found = FALSE;
				itemid = atoi( data);
				for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
					int itemindex = CHAR_getItemIndex( charaindex, j);
					if( ITEM_CHECKINDEX( itemindex)) {
						if( ITEM_getInt( itemindex, ITEM_ID) == itemid ) {
							found = TRUE;
							break;
						}
					}
				}
				if( !found )
					break;
			}
			if( !found )
				break;
		}
#ifdef _ADD_NOITEM_BATTLE
		if(NPC_Util_GetStrFromStrWithDelim(argstr,"noitem",buf,sizeof(buf)) != NULL){
			char data[128];
			BOOL found = FALSE;

			for(i=0;i<CHAR_MAXITEMHAVE;i++){
				int j;
				int itemid;
				int ret = getStringFromIndexWithDelim(buf,",",i + 1,data,sizeof(data));
				if(ret == FALSE) break;
				found = TRUE;
				itemid = atoi(data);
				for(j=0;j<CHAR_MAXITEMHAVE;j++){
					int itemindex = CHAR_getItemIndex(charaindex,j);
					if(ITEM_CHECKINDEX( itemindex)){
						if(ITEM_getInt(itemindex,ITEM_ID) == itemid){
							found = FALSE;
							break;
						}
					}
				}
				if(!found) break;
			}
			if(!found) break;
		}
#endif

		{
			char data[256];

			BOOL Evflg=TRUE;
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "B_evend", buf, sizeof( buf)) != NULL ) {
				int i=1, event=-1;;
				while( getStringFromIndexWithDelim( buf,",", i, data, sizeof( data)) ){
					i++;
					event=atoi( data);
					if( NPC_EventCheckFlg( charaindex, event) == FALSE )	{
						Evflg = FALSE;
						break;
					}
				}
			}
			if( Evflg == FALSE )
				break;
			i=1;
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "B_evnow", buf, sizeof( buf)) != NULL ) {
				int i=1, event=-1;;
				while( getStringFromIndexWithDelim( buf,",", i, data, sizeof( data)) ){
					i++;
					event=atoi( data);
					if( NPC_NowEventCheckFlg( charaindex, event) == FALSE )	{
						Evflg = FALSE;
						break;
					}
				}
			}
			if( Evflg == FALSE )
				break;
		}

		if( CHAR_getWorkInt( meindex, CHAR_WORK_ONEBATTLE ) == 1 ) {
			for( i = 0; i < battlemax; i ++ ) {
				if( BattleArray[i].use ) {
					if( BattleArray[i].createindex == meindex ) {
						break;
					}
				}
			}
			if( i != battlemax ) {
				if( NPC_Util_GetStrFromStrWithDelim( argstr, "alreadymsg", buf, sizeof( buf))!= NULL )
				{
					CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
				}
				return FALSE;
			}
		}

#ifdef	_NPCENEMY_1		
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "partynum", buf, sizeof( buf))!= NULL ) {
			if( CHAR_getWorkInt( charaindex, (atoi(buf)-1) + CHAR_WORKPARTYINDEX1) == -1 ) {
				char partymsg[256];
				sprintf(partymsg,"您的队伍人数少于%d人,无法继续！",atoi(buf));
				CHAR_talkToCliAndParty( charaindex, meindex ,partymsg, CHAR_COLORYELLOW );
				return FALSE; 
			}
		}
#endif
		flg = TRUE;
		break;
	}

	if( flg ) {
		/* 阂及凛反  骰允月 */
		if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_CLIENT) {
			char	config[32];
			snprintf( config, sizeof( config), "askbattlemsg1");
			/* 爵  互铵引月蟆卞撙  丢永本□斥 */
			
			
			if( NPC_Util_GetStrFromStrWithDelim( argstr, config,  buf, sizeof( buf))!= NULL )
			{
				int	len = strlen( buf);
				char	escapebuf[1024];
				int fd = getfdFromCharaIndex( charaindex);
				char	buf2[256];
				/* askbattlemsg? 互丐木壬  醒垫卞允月  "\n"戊□玉毛  木月)*/
				for( i = 2; i < 7; i ++ ) {
					snprintf( config, sizeof( config), "askbattlemsg%d", i);
					if( NPC_Util_GetStrFromStrWithDelim( argstr, config, buf2, sizeof( buf2))!= NULL )
					{
						strcpysafe( &buf[len], sizeof( buf)-len, "\n" );
						len++;
						strcpysafe( &buf[len], sizeof( buf)-len, buf2 );
						len += strlen( buf2);
						if( len >= sizeof( buf) ) {
							print( "buffer over\n");
							return FALSE;
						}
					}
					else {
						strcpysafe( &buf[len], sizeof( buf)-len, "\n" );
						len++;
						if( len >= sizeof( buf) ) {
							print( "buffer over\n");
							return FALSE;
						}
					}
				}

				if( fd != -1 ) {
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_YESNO,
									CHAR_WINDOWTYPE_NPCENEMY_START,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									makeEscapeString( buf, escapebuf,sizeof(escapebuf)));
				}
				/* 仇仇分仃    卞伉正□件 */
				return FALSE;
			}else {
				/* 爵  卞  日六月 */
				flg = NPC_NPCEnemy_BattleIn( meindex, charaindex);
			}
		}
	}
	if( !flg) {
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "deniedmsg", buf, sizeof( buf))!= NULL )
		{
			CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
		}
	}
	return flg;
}

int NPC_NPCEnemy_Dying( int battleindex, int meindex )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buf[64];
	int	i,toindex=-1;
	int tmp=-1;

	if( !CHAR_CHECKINDEX( meindex)) {
		return FALSE;
	}
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));

#ifdef _NEW_WARPMAN
	if( strstr( argstr, "NEWNPCENEMY") )    {
	}else	{
#endif
	/* 蝇    及陲太 */
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "endmsg", buf, sizeof( buf)) != NULL ) {
		for( i = 0; i < 5; i ++ ) {
			/* 爵  卞辅笛仄化中月蝈够卞丢永本□斥 */
			int toindex = BattleArray[battleindex].Side[0].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( toindex) )	{
				CHAR_talkToCli( toindex, meindex ,buf , CHAR_COLORYELLOW );
			}
		}

	}
#ifdef _NEW_WARPMAN
	}
#endif
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "steal", buf, sizeof( buf)) != NULL ) {
		print("\n steal TRUE !!");
		if( atoi( buf) == 1 ) {
			int charaindex = BattleArray[battleindex].Side[0].Entry[0].charaindex;
			NPC_NPCEnemy_StealItem( argstr, meindex, charaindex);
		}
		
	}
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "herobattlefield", buf, sizeof( buf)) != NULL ) {
			int charaindex = BattleArray[battleindex].Side[0].Entry[0].charaindex;
			CHAR_setWorkInt ( charaindex , CHAR_WORKHEROFLOOR , atoi ( buf ) ) ;
			// Syu ADD 
			if ( atoi ( buf ) > CHAR_getInt ( charaindex , CHAR_HEROFLOOR ) )
				CHAR_setInt ( charaindex , CHAR_HEROFLOOR , atoi( buf ) ) ; 
	}

#endif
	tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "dieact");
	if( tmp != 1 ) 
		tmp = 0;
	CHAR_setWorkInt( meindex, CHAR_WORK_DIEACT, tmp);

	if( CHAR_getWorkInt( meindex, CHAR_WORK_DIEACT) == 0 ) {
	    Char 	*ch;
		CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER, 0);
		print("bbi---->bi10");
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));

		CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_ALTERRATIVE);
		MAP_sendAroundMapdata( CHAR_getInt( meindex, CHAR_FLOOR),
								CHAR_getInt( meindex, CHAR_X),
								CHAR_getInt( meindex, CHAR_Y));
	    ch  = CHAR_getCharPointer( meindex);
	    if( ch != NULL ) {
			strcpysafe( ch->charfunctable[CHAR_LOOPFUNC].string,
						sizeof( ch->charfunctable[CHAR_LOOPFUNC]),
						"NPCEnemyLoop");
			CHAR_constructFunctable( meindex);
			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 5000);
			CHAR_setWorkInt( meindex, CHAR_WORK_DIETIME, NowTime.tv_sec);
#ifdef	_NPCENEMY_1
			char token1[512];
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "winmsg", token1, sizeof( token1)) != NULL ) {
				int     i;
    		int     playernum = CHAR_getPlayerMaxNum();
    		for( i = 0 ; i < playernum ; i++) {
        	if( CHAR_getCharUse(i) != FALSE ) {
						CHAR_talkToCli( i, -1, token1, CHAR_COLORRED);
    			}
				}
			}
			if( NPC_Util_GetStrFromStrWithDelim( argstr, "GetRandItem", buf, sizeof( buf))!= NULL ){
				int j=1, rand_cnt=1;
				char buf2[256];
				while( getStringFromIndexWithDelim( buf , "," , j, buf2, sizeof( buf2)) != FALSE ){
					j++;
					rand_cnt++;
				}
				for( i = 0; i < 5; i ++ ) {
					int toindex = BattleArray[battleindex].Side[0].Entry[i].charaindex;
					if( CHAR_CHECKINDEX( toindex) )	{
						NPC_ActionTreasureRandItemGet( meindex, toindex, rand_cnt, buf);
					}
				}
			}
#endif
		}else {
			print( "NPCENEMY:err\n");
		}
	}else {
		for( i = 0; i < 5; i ++ ) {
			toindex = BattleArray[battleindex].Side[0].Entry[i].charaindex;
			if( CHAR_CHECKINDEX( toindex) ) {
#ifdef _NEW_WARPMAN
				if( strstr( argstr, "NEWNPCENEMY") )    {
					BOOL Party=TRUE;
					if( NPC_NPCEnemy_CheckFree( meindex, toindex , &Party) == FALSE )       {
						continue;
					}
					if( Party == TRUE )	{
					}else	{
						if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER )	{
							if( NPC_WarpAllMsg( meindex, toindex ) == TRUE )	{
								return TRUE;
							}
						}
						continue;
					}
				}
				CHAR_DischargeParty( toindex, 0); // 拆队
				CHAR_warpToSpecificPoint( toindex,
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
					CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y)
					);

#else
				CHAR_warpToSpecificPoint( toindex,
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X),
								CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y)
								);
#endif
#ifdef _NPC_REPLACEMENT
				Check_EnemyWarpMe( meindex, argstr);
#endif
			}
		}
	}
	return TRUE;
}

void NPC_NPCEnemyLoop( int meindex )
{

	if( NowTime.tv_sec > CHAR_getWorkInt( meindex, CHAR_WORK_DIETIME) +
						 CHAR_getWorkInt( meindex, CHAR_WORK_REVIVALTIME)){
		Char	*ch;
		CHAR_setInt( meindex, CHAR_BASEIMAGENUMBER,	CHAR_getWorkInt( meindex, CHAR_WORK_BASEIMGBAK));
		print("bbi---->bi11");

		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX));
		CHAR_setWorkInt( meindex, CHAR_WORKEVENTTYPE, CHAR_EVENT_ENEMY);
#ifdef	_NPCENEMY_1
		char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
		NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
		char token1[1024];
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "npcwarp", token1, sizeof( token1)) != NULL ) {
			char token[32];
			char token2[32];
			char buf2[32];
			int floor,x,y,x1,x2,y1,y2;
			int ret;
			int cnt;
			int dcnt = 1;

			/*--蜇箕及伐□皿及赐  --*/
			//cnt = CHAR_getWorkInt(meindex,CHAR_NPCWORKINT4);
			while(getStringFromIndexWithDelim(token1,",", dcnt,buf2,sizeof(buf2) )
	 			!=FALSE ){
				dcnt++;
			}
			cnt = RAND(1,dcnt);
			//printf("\ndcnt=%d,cnt=%d\n",dcnt,cnt);

			if(getStringFromIndexWithDelim(token1,",", cnt,buf2,sizeof(buf2)) !=FALSE )
			{
				cnt++;
	    	ret=getStringFromIndexWithDelim(buf2,".", 1,token,sizeof(token));
       	if( ret == FALSE ) {
        	floor = CHAR_getInt(meindex,CHAR_FLOOR);
       	}
       	floor = atoi( token );

       	ret=getStringFromIndexWithDelim(buf2,".", 2,token,sizeof(token));
        if( ret == FALSE ){
          x = CHAR_getInt(meindex,CHAR_X);
        }else{
        	ret=getStringFromIndexWithDelim(token,"-", 1,token2,sizeof(token2));
        	if( ret == FALSE ){
          	x = atoi( token );
        	}else{
        		x1=atoi(token2);
        		ret=getStringFromIndexWithDelim(token,"-", 2,token2,sizeof(token2));
        		if( ret == FALSE ){
        			x2=x1;
        		}else{
        			x2=atoi(token2);
        		}
        		x = RAND(x1,x2);
        	}
        }
       	//x = atoi( token );

       	ret=getStringFromIndexWithDelim(buf2,".", 3,token,sizeof(token));
        if( ret == FALSE ) {
         y = CHAR_getInt(meindex,CHAR_Y);
        }else{
        	ret=getStringFromIndexWithDelim(token,"-", 1,token2,sizeof(token2));
        	if( ret == FALSE ){
          	y = atoi( token );
        	}else{
        		y1=atoi(token2);
        		ret=getStringFromIndexWithDelim(token,"-", 2,token2,sizeof(token2));
        		if( ret == FALSE ){
        			y2=y1;
        		}else{
        			y2=atoi(token2);
        		}
        		y = RAND(y1,y2);
        	}
        }
       	//y = atoi( token );

				//CHAR_setInt(meindex,CHAR_NPCWORKINT4, cnt);
        //CHAR_warpToSpecificPoint(meindex, floor, x, y);
       	//CHAR_setInt(meindex,CHAR_FLOOR,floor);
       	//CHAR_setInt(meindex,CHAR_X,x);
       	//CHAR_setInt(meindex,CHAR_Y,y);
       	CHAR_warpToSpecificPoint(meindex,floor,x,y);
			}
		}
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "allmsg", token1, sizeof( token1)) != NULL ) {
			int     i;
    	int     playernum = CHAR_getPlayerMaxNum();
    	for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
					CHAR_talkToCli( i, -1, token1, CHAR_COLORRED);
    		}
			}
		}
#endif
		MAP_sendAroundMapdata( CHAR_getInt( meindex, CHAR_FLOOR),
								CHAR_getInt( meindex, CHAR_X),
								CHAR_getInt( meindex, CHAR_Y));
	    ch  = CHAR_getCharPointer( meindex);
	    if( ch != NULL ) {
			strcpysafe( ch->charfunctable[CHAR_LOOPFUNC].string,
						sizeof( ch->charfunctable[CHAR_LOOPFUNC]),	"");
			CHAR_constructFunctable( meindex);
//			CHAR_setInt( meindex, CHAR_LOOPINTERVAL, 0);
		}
	}
}


BOOL NPC_NPCEnemy_BattleIn(  int meindex, int charaindex)
{
	int		gym;
	int		ret;
	char	buf[256];
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	memset( argstr, 0, sizeof( argstr));
	NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr));
	gym = NPC_Util_GetNumFromStrWithDelim( argstr, "gym");
	
#ifdef _EMENY_CHANCEMAN
	{
		int masterindex=-1;
		masterindex = NPC_EmenyChanceCheck( meindex, charaindex, argstr);
		if( CHAR_CHECKINDEX( masterindex)){
			charaindex = masterindex;
		}else	{
			return TRUE;
		}
	}
#endif
#ifdef	_WARNPC_CTRL
	int sameip = NPC_Util_GetNumFromStrWithDelim( argstr, "sameip");
	if(sameip == -1)
		sameip = 0;
	CHAR_setWorkInt( charaindex, CHAR_WORKNPCBATTLESAMEIP, sameip);
	if(CHAR_getWorkInt(charaindex,CHAR_WORKNPCBATTLESAMEIP)>0){
		int partyi;
		unsigned ip=CONNECT_get_userip(getfdFromCharaIndex(charaindex));
		unsigned teseip = ip;
		unsigned toip;
		int a, b, c, d,ipcnt=1;
    char ipstr[32];              
		a=(teseip % 0x100); teseip=teseip / 0x100;
		b=(teseip % 0x100); teseip=teseip / 0x100;
		c=(teseip % 0x100); teseip=teseip / 0x100;
		d=(teseip % 0x100);
		sprintf(ipstr,"%d.%d.%d.%d",a,b,c,d);
		if(strcmp(ipstr,getNoAttIp(0))==0 || strcmp(ipstr,getNoAttIp(1))==0 || strcmp(ipstr,getNoAttIp(2))==0 || strcmp(ipstr,getNoAttIp(3))==0 || strcmp(ipstr,getNoAttIp(4))==0){
		}
		else{
			if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE ) {
			}else{
				for( partyi = 0; partyi < CHAR_PARTYMAX; partyi ++ ) {
					int index = CHAR_getWorkInt( charaindex, partyi + CHAR_WORKPARTYINDEX1);
					if( CHAR_CHECKINDEX(index)) {
						toip = CONNECT_get_userip(getfdFromCharaIndex(index));
						if(ip==toip){
							ipcnt++;
							if(ipcnt>sameip){
								CHAR_talkToCli( charaindex, -1, "同IP组队数已达上限，无法开战。", CHAR_COLORYELLOW);
								return FALSE;
							}
						}
					}
				}
			}
		}
	}
#endif
	if( gym > 0 ){
		ret = BATTLE_CreateVsEnemy( charaindex, 2, meindex);
	}else{
		ret = BATTLE_CreateVsEnemy( charaindex, 1, meindex);
	}
	if( ret == 0 ) {
#ifdef	_WARNPC_CTRL
		int tmp = NPC_Util_GetNumFromStrWithDelim( argstr, "noquckwar");
		if( tmp == -1 )
			tmp = 0;
		CHAR_setWorkInt( meindex, CHAR_WORK_BATTLEINGTIME, tmp);
		if(CHAR_getWorkInt(meindex,CHAR_WORK_BATTLEINGTIME)>0){
			CHAR_setWorkInt(charaindex,CHAR_WORKNPCENEMYBATTLETIME,1);
			CHAR_setWorkInt(charaindex,CHAR_WORKNPCENEMYTIME,CHAR_getWorkInt(meindex,CHAR_WORK_BATTLEINGTIME));
			if(CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
				int i;
				for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
					if( CHAR_getWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1) > -1 ) {
						CHAR_setWorkInt(CHAR_getWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1),CHAR_WORKNPCENEMYBATTLETIME,1);
						CHAR_setWorkInt(CHAR_getWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1),CHAR_WORKNPCENEMYTIME,CHAR_getWorkInt(meindex,CHAR_WORK_BATTLEINGTIME));
					}
				}
			}
		}
#endif
		BattleArray[CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX)].WinFunc	= NPC_NPCEnemy_Dying;
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "startmsg", buf, sizeof( buf))	!= NULL ){
			CHAR_talkToCliAndParty( charaindex, meindex ,buf , CHAR_COLORYELLOW );
		}
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "steal", buf, sizeof( buf))!= NULL ){
			if( atoi( buf) == 0 ) {
				NPC_NPCEnemy_StealItem( argstr, meindex, charaindex);
			}
		}
#ifdef	_NPCENEMY_1
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "help", buf, sizeof( buf))	!= NULL ){
		CHAR_setWorkInt(charaindex,CHAR_WORKBATTLEHELP,1);
		BattleArray[CHAR_getWorkInt( charaindex,
			CHAR_WORKBATTLEINDEX)].Side[
			CHAR_getWorkInt( charaindex,
			CHAR_WORKBATTLESIDE)].flg |= BSIDE_FLG_HELP_OK;
		int i;
		for( i = 0; i < 5; i ++ ) {
		int toindex = BattleArray[CHAR_getWorkInt(
					    charaindex, CHAR_WORKBATTLEINDEX)].Side[
						CHAR_getWorkInt( charaindex,
						CHAR_WORKBATTLESIDE)].Entry[i].charaindex;
		if( CHAR_CHECKINDEX( toindex)) {
			int tofd = getfdFromCharaIndex( toindex );
			if( tofd != -1 ) {
				CHAR_setWorkInt(toindex,CHAR_WORKBATTLEHELP,1);
				lssproto_HL_send( tofd, TRUE);
			}
			/* 丢永本□斥霜耨 */
			CHAR_sendBattleEffect( toindex, ON);
		}
		}
		char floorname[32];
		int floorx,floory;
		getStringFromIndexWithDelim( MAP_getfloorShowstring(CHAR_getInt(meindex,CHAR_FLOOR)),"|", 1,floorname, sizeof(floorname));
		floorx = CHAR_getInt(meindex,CHAR_X);
		floory = CHAR_getInt(meindex,CHAR_Y);
    	int     playernum = CHAR_getPlayerMaxNum();
    	char token1[256];
    	sprintf(token1,"勇士们，已有人在%s《%d,%d》找到炼狱使者，速来增援。",floorname,floorx,floory);
    	for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
					CHAR_talkToCli( i, -1, token1, CHAR_COLORRED);
    		}
			}
	}
#endif
	}
	return ( ret == 0 ) ?TRUE:FALSE ;
}

void NPC_NPCEnemyWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	if( seqno == CHAR_WINDOWTYPE_NPCENEMY_START) {
		if( select == WINDOW_BUTTONTYPE_YES) {
			NPC_NPCEnemy_BattleIn( meindex, talkerindex);
		}
	}
}

static int NPC_NPCEnemy_StealItem( char *argstr, int meindex, int charaindex)
{
	char	itembuf[1024];
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "item", itembuf, sizeof( itembuf))!= NULL )
	{
		char	data[128];
		int		delitemgroup[CHAR_MAXITEMHAVE];
		int		found = 0;
		int		i;
		for( i = 0; i < CHAR_MAXITEMHAVE; i ++ ) {
			int		j;
			int		itemid;
			int ret = getStringFromIndexWithDelim( itembuf,",", i + 1,data, sizeof(data));
			if( ret == FALSE) {
				break;
			}
			itemid = atoi( data);
			for( j = 0; j < CHAR_MAXITEMHAVE; j ++ ) {
				int itemindex = CHAR_getItemIndex( charaindex, j);
				if( ITEM_CHECKINDEX( itemindex)){
					if( ITEM_getInt( itemindex, ITEM_ID) == itemid ){
						CHAR_setItemIndex( charaindex, j, -1);
						ITEM_endExistItemsOne( itemindex);
						delitemgroup[found] = j;
						found ++;
						break;
					}
				}
			}
			if( !found ) break;
		}
		CHAR_sendItemData( charaindex, delitemgroup, found);
	}
	else {
		print( "NPCENEMY:奇怪\n");
		return FALSE;
	}
	return TRUE;
}

#ifdef _NEW_WARPMAN
static BOOL NPC_NPCEnemy_CheckFree( int  meindex, int toindex, BOOL *Party)
{
  char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
  char buf1[1024],buf2[256],buf3[256],buf4[256];
  char token[256];
  char temp1[16];
  int talkNo = 1;
  int point=1;

  int i=0,type=0;

  struct tagWarpPoint {
	int floor;
	int x;
	int y;
  }AllWarPoint[]={
	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
	{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
  };

  if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL)	{
	print("GetArgStrErr");
	return FALSE;
  }
  if( (strstr( npcarg,"NEWNPCENEMY") == NULL) && (strstr( npcarg,"OVER") == NULL) )	{
	return FALSE;
  }else	{
  }
	
  while( getStringFromIndexWithDelim( npcarg,"OVER",talkNo,buf1,sizeof( buf1)) != FALSE )	{
	
	talkNo++;
	sprintf(temp1,"NEWEVENT");
	if( strstr( buf1, temp1) != NULL)	{
	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "FREE", buf2, sizeof( buf2) ) == FALSE )	{
	  	continue;
	  }
		if( NPC_ActionPassCheck( meindex, toindex, buf2) == FALSE )	{
			continue;
		}

	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "endmsg", token, sizeof( token)) != NULL ) {
		CHAR_talkToCli( toindex, meindex ,token , CHAR_COLORYELLOW );
	  }
#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "herobattlefield", token, sizeof( token)) != NULL ) {
		  CHAR_setWorkInt ( toindex , CHAR_WORKHEROFLOOR , atoi ( token ) ) ;
		  if ( atoi ( token ) > CHAR_getInt ( toindex , CHAR_HEROFLOOR ) )
				CHAR_setInt ( toindex , CHAR_HEROFLOOR , atoi( token ) ) ; 

	  }
#endif

	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "WARP", buf2, sizeof( buf2) ) == FALSE ){
	  	continue;
	  }else	{//RAND	arraysizeof	AllWarPoint
		  	while( getStringFromIndexWithDelim( buf2, ";", point, buf3,sizeof( buf3)) != FALSE )	{
				point++;
				getStringFromIndexWithDelim( buf3, ",", 1, buf4, sizeof( buf4));
				AllWarPoint[i].floor = atoi( buf4);
				getStringFromIndexWithDelim( buf3, ",", 2, buf4, sizeof( buf4));
				AllWarPoint[i].x = atoi( buf4);
				getStringFromIndexWithDelim( buf3, ",", 3, buf4, sizeof( buf4));
				AllWarPoint[i].y = atoi( buf4);
				i++;
				if( i >= arraysizeof( AllWarPoint)  )
					  break;
			}
			type = RAND( 0, i-1);
			if( AllWarPoint[type].floor <= 0 || type > arraysizeof( AllWarPoint)-1)	{
				type = 0;
			}
		  CHAR_setWorkInt( meindex, CHAR_WORK_WARP_FLOOR, AllWarPoint[type].floor );
		  CHAR_setWorkInt( meindex, CHAR_WORK_WARP_X, AllWarPoint[type].x );
		  CHAR_setWorkInt( meindex, CHAR_WORK_WARP_Y, AllWarPoint[type].y );
		  
	  }
	  if( NPC_Util_GetStrFromStrWithDelim( buf1, "CHECKPARTY", buf4, sizeof( buf4)) != NULL ) {
		  if( strstr( buf4, "FALSE") != NULL )	{
				*Party = FALSE;
		  }
	  }
	  if( *Party == TRUE )	{
		  Action_RunDoEventAction( meindex, toindex, buf1);
	  }
	  return TRUE;
	}
  }
  return FALSE;  
}


BOOL NPCEnemy_CheckFree( int meindex, int talker, char *buf)
{
	char buff2[256];
	char buff3[128];
	int i=1,j=1;
	int loop=0;
	while( getStringFromIndexWithDelim(buf,",",i,buff2,sizeof(buff2)) !=FALSE )	{	
		i++;
		if(strstr(buff2,"&")!=NULL){
			j=1;
			loop=0;
			while( getStringFromIndexWithDelim(buff2,"&",j,buff3,sizeof(buff3)) !=FALSE )	{
				j++;
				if(NPCEnemy_BSCheck(meindex,talker,buff3)==FALSE)	{
					loop=1;
					break;
				}
			}
			if(loop==0) {
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}else{
			if( NPCEnemy_BSCheck( meindex, talker, buff2) == TRUE ){
				CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, TRUE );
				return TRUE;
			}
		}
	}
	CHAR_setWorkInt( talker, CHAR_WORKWARPCHECK, FALSE );
	return FALSE;
}

BOOL NPCEnemy_BSCheck(int meindex,int talker,char* buf)
{
	char buff2[128];
	int kosuu,temp=-1,flg=0;
	char buff1[128],buff3[128];
	if(strstr( buf, "-") != NULL)	{
		//buff3为抓宠物ID
		getStringFromIndexWithDelim( buf, "-", 2, buff3, sizeof(buff3));
		temp = atoi( buff3);
		getStringFromIndexWithDelim( buf, "-", 1, buff1, sizeof(buff1));
		strcpy( buf, buff1);
        }                         

	if(strstr( buf, "<") != NULL){
		getStringFromIndexWithDelim( buf, "<", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "<", 1, buff2, sizeof(buff2));

		if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, 1, temp)==TRUE){
			return TRUE;
		}
	}else if(strstr( buf, ">") != NULL){
		getStringFromIndexWithDelim( buf, ">", 2, buff2, sizeof(buff2));
		kosuu = atoi(buff2);
		getStringFromIndexWithDelim( buf, ">" ,1, buff2, sizeof(buff2));

		if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, 2, temp)==TRUE){
			return TRUE;
		}

	}else if(strstr( buf, "!" ) != NULL){
		getStringFromIndexWithDelim( buf, "!=", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "!=", 1, buff2, sizeof(buff2));
		if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, 0, temp) == TRUE){
			return FALSE;
		}else{
			return TRUE;
		}

	}else if(strstr( buf, "=") != NULL){
	
		getStringFromIndexWithDelim( buf, "=", 2, buff2, sizeof(buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "=", 1, buff2, sizeof(buff2));

		if( strstr( buf, "PET"))	{
			flg = 3;
		}
		if(strstr( buf, "*") != NULL){
			if( NPCEnemy_WarpManReduce( meindex, talker, buf)==TRUE){
				return TRUE;
			}
		}else if(NPCEnemy_FreeIfCheck( meindex, talker, buff2, kosuu, flg, temp)==TRUE){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL NPCEnemy_FreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg, int temp)
{
	if(strcmp(buf,"LV")==0){
		if(NPCEnemy_LevelCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	
	if( strcmp(buf,"TRANS") == 0 )	{
		if( NPCEnemy_CheckTrans( meindex, talker, kosuu, flg) == TRUE )
			return TRUE;
	}
	if( strcmp( buf, "PET") == 0 )	{
		if( temp > 0 )	{
			if( NPCEnemy_CheckMyPet( meindex, talker, kosuu, flg, temp) == TRUE )
				return TRUE;
		}else
			return FALSE;
	}	

	if(strcmp(buf,"ITEM")==0){
		if(NPCEnemy_ItemCheck(meindex,talker,kosuu,flg)==TRUE){
				return TRUE;
		}
	}
	if(strcmp(buf,"ENDEV")==0){
		if(NPC_EventCheckFlg( talker, kosuu)==TRUE){
				return TRUE;
		}
	}
	if(strcmp(buf,"NOWEV")==0){
		if(NPC_NowEventCheckFlg( talker, kosuu)==TRUE){
				return TRUE;
		}
	}
	return FALSE;

}

BOOL NPCEnemy_WarpManReduce(int meindex,int talker,char *buf)
{
	char buf2[512];
	char buf3[256];
	int id=0;
	int i;
	int itemindex;
	int itemno;
	int kosuu;
	int cnt=0;
	
	getStringFromIndexWithDelim(buf,"=",2,buf2,sizeof(buf2));
	getStringFromIndexWithDelim(buf2,"*",1,buf3,sizeof(buf3));
	itemno = atoi(buf3);
	getStringFromIndexWithDelim(buf2,"*",2,buf3,sizeof(buf3));
	kosuu = atoi(buf3);
	
	for( i=0 ; i < CHAR_MAXITEMHAVE;i++ ){
		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX(itemindex) ){
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if(itemno==id){
				cnt++;
				if(cnt==kosuu){
					return TRUE;
				}
			}
		}
	}

	return FALSE;

}

BOOL NPCEnemy_BigSmallLastCheck(int point1,int mypoint,int flg)
{
	if(flg==0){
		if(point1==mypoint) {
			return TRUE;
		}
		
	}else if(flg==1){
		if(mypoint < point1) {
			return TRUE;
		}
		
	}else if(flg==2){
		if(mypoint > point1) {
			return TRUE;
		}
	}

	return FALSE;

}

BOOL NPCEnemy_LevelCheck(int meindex,int talker,int level,int flg)
{
	int mylevel;
	mylevel=CHAR_getInt(talker,CHAR_LV);
	if(NPCEnemy_BigSmallLastCheck(level,mylevel,flg)==TRUE)	{
		return TRUE;
	}
	return FALSE;
}

BOOL NPCEnemy_CheckTrans(int meindex,int talker,int trans,int flg)
{
	int myTrans;
	myTrans=CHAR_getInt(talker, CHAR_TRANSMIGRATION);
	if( NPCEnemy_BigSmallLastCheck( trans, myTrans, flg ) == TRUE )	{
             return TRUE;
        }                   
	return FALSE;
}

BOOL NPCEnemy_CheckMyPet( int meindex, int talker, int petLv, int flg, int petid)
{
	int petsel,petindex=-1;	
	for(petsel=0 ; petsel < CHAR_MAXPETHAVE ; petsel++)     {
		petindex = CHAR_getCharPet( talker, petsel);
		if( petindex == -1  )
			continue;
	        if(CHAR_getInt( petindex, CHAR_PETID) !=  petid )
			continue;
		break;
	}
	if( petsel  == CHAR_MAXPETHAVE )	{
		return FALSE;
	}else	{	//找到条件宠
		switch( flg)	{
		case 0:
			if( CHAR_getInt( petindex, CHAR_LV) != petLv )
				return TRUE;
		break;
		case 1:
			if( CHAR_getInt( petindex, CHAR_LV) < petLv )
				return TRUE;
		break;
		case 2:
			if( CHAR_getInt( petindex, CHAR_LV) > petLv )
				return TRUE;
		break;
		case 3:
			if( CHAR_getInt( petindex, CHAR_LV) == petLv )
				return TRUE;
		break;
		}
		return FALSE;
	}
	return FALSE;                                             
}

BOOL NPCEnemy_ItemCheck(int meindex,int talker,int itemNo,int flg)
{
	int i;
	int itemindex=-1;
	int id;

	for( i=0;i<CHAR_MAXITEMHAVE;i++ ){

		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			
			if( NPCEnemy_BigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}
	return FALSE;

}

BOOL NPC_WarpAllMsg(int meindex,int toindex )
{
	int fl,x,y;
	int subindex=-1;
	int i;
	fl = CHAR_getWorkInt( meindex, CHAR_WORK_WARP_FLOOR );
	x = CHAR_getWorkInt( meindex, CHAR_WORK_WARP_X );
	y = CHAR_getWorkInt( meindex, CHAR_WORK_WARP_Y );

	if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE ) != 1 )
		return FALSE;

	for( i = 0; i < CHAR_PARTYMAX; i ++ ){
		subindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1+i );
		if( CHAR_CHECKINDEX( subindex ) == FALSE )continue;
		if( fl==0 && x==0 && y==0 )	{
		}else	{
			CHAR_warpToSpecificPoint( subindex, fl, x, y );
		}
	}

	return TRUE;
}
#endif



void NPC_NPCEnemyWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen )
{
	int born, dead;
	int meindex, index;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];

	if( OBJECT_getType( objindex) != OBJTYPE_CHARA) return;
	index = OBJECT_getIndex( objindex);
	if( !CHAR_CHECKINDEX( index) ) return;
	if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return;
    meindex = OBJECT_getIndex( meobjindex);
	if( !CHAR_CHECKINDEX( meindex) ) return;

	if( NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg)) == NULL ){
		print("WarpMan:GetArgStrErr");
		return;
	}
	if( !strstr( npcarg,"NEWNPCENEMY") )
		return;
	if( NPC_getTimeData( meindex, npcarg, &born, &dead, 200) == FALSE )
		return;
	
	NPC_TimeDefineDo( meindex, born, dead, NPC_TIME_MODE);

	CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ) );

}

#ifdef _NPC_REPLACEMENT
void Check_EnemyWarpMe( int meindex, char *args )
{

	char buf1[256];
	int objindex,index;
	int nfl, nx, ny;
	OBJECT obj;

	memset( buf1, 0, sizeof( buf1));
	if( NPC_Util_GetStrFromStrWithDelim( args, "REPLACEMENT", buf1, sizeof( buf1)) == NULL ) {
		return;
	}
	GetRand_WarpPoint( buf1, &nfl, &nx, &ny );

	if( MAP_IsValidCoordinate( nfl, nx, ny ) == FALSE ){
		char filename[256];
		NPC_Util_CheckAssignArgFile( meindex, filename);
		print( "NPCENEMY REPLACEMENT: %s .Invalid POINT( %d, %d, %d)\n", filename, nfl, nx, ny );
		return;
	}
	{
		int ofl, ox, oy;
		ofl = CHAR_getInt( meindex, CHAR_FLOOR );
		ox = CHAR_getInt( meindex, CHAR_X );
		oy = CHAR_getInt( meindex, CHAR_Y );

		for ( obj=MAP_getTopObj( nfl, nx, ny); obj; obj=NEXT_OBJECT( obj) ) {
			objindex = GET_OBJINDEX( obj);
			if (OBJECT_getType( objindex) != OBJTYPE_CHARA) continue;
			index = OBJECT_getIndex( objindex);
			if( CHAR_getInt( index, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER &&
				CHAR_getWorkInt( index, CHAR_WORKEVENTTYPE) != CHAR_TYPEPET )	{
				CHAR_warpToSpecificPoint( index, ofl, ox, oy);
				CHAR_sendCToArroundCharacter( objindex);
				break;
			}
		}
	}
	objindex = CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX);
	CHAR_warpToSpecificPoint( meindex, nfl, nx, ny);
	CHAR_sendCToArroundCharacter( objindex);
}
#endif
