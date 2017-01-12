#include "version.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "init.h"
#include "net.h"
#include "handletime.h"
#include "char.h"
#include "map_warppoint.h"
#include "npcgen.h"
#include "log.h"
#include "configfile.h"
#include "battle.h"
#include "petmail.h"
#include "autil.h"
#include "family.h"
#include "trade.h"
#include "title.h"
#include "attestation.h"
#ifdef _ALLBLUES_LUA
#include "mylua/ablua.h"
#endif
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
#include "chatroom.h"
#endif

#include "sasql.h"

static void ShutdownProc( void);
void mainloop( void );
struct  tm tmNow, tmOld;
void family_proc();

#ifdef _CHANNEL_MODIFY
extern int InitOccChannel(void);
#endif

#ifdef _GMSV_DEBUG
extern char *DebugMainFunction;
#endif

#ifdef _ANGEL_SUMMON
#define ANGELTIMELIMIT	3*24*60*60 // 完成任务时限(秒)
int AngelReady =0;
int AngelTimelimit = ANGELTIMELIMIT;
time_t AngelNextTime;
void AngelReadyProc();
#endif
int yzflg=0;
//#include "genver.h"

void warplog_proc();


int main( int argc , char** argv, char** env )
{

	//if(attestation()<1)
	//	{
		//	print("无法通过正版验证！具体请联系购买者！\n");
	//		exit(0);
	//	}
	//if (strcmp(getlocaltime(), TimeLimitLocal)>0)exit(0);
#ifdef	_ONLINENUM_FIX
		system("ulimit -HSn 2048");
#endif

    /*  とりあえず時間を設定しておく    */
    setNewTime();

/*    if ( argc > 1 && 0==strcmp(argv[1],"-v"))
    {
        printf("%s",genver);
        exit(0);
    }
    else fprintf(stderr,"%s",genver);*/
		readconfigfile(getConfigfilename());
    EXITWITHEXITCODEIFFALSE( util_Init() , 1);

    LoadAnnounce();	// Arminius 7.12 loginannounce

		/* 初期設定 */
		memcpy( &tmOld, localtime( (time_t *)&NowTime.tv_sec), sizeof( tmNow ) );

		sasql_init();

    EXITWITHEXITCODEIFFALSE( init(argc , argv ,env ) , 1);
		sasql_online(NULL,NULL,NULL,NULL,NULL,3);
    LoadPetTalk();	// Arminius 8.14 pet talk

#ifdef _GAMBLE_BANK
	Load_GambleBankItems();
#endif

#ifdef _CFREE_petskill
	Load_PetSkillCodes();
#endif

#ifdef _ITEM_PILEFORTRADE
	TRADE_InitTradeList();
#endif
    mainloop();
    
    return 0;
}

void mainloop( void )
{	
    print("初始化NPC...");
    NPC_generateLoop( 1 );
    print("完成\n");
    print("初始化signal1...");
    signal(SIGUSR1,sigusr1);
    print("完成\n");
    print("初始化signal2...");
    signal(SIGUSR2,sigusr2);
    print("完成\n");
#ifdef _MAP_WARPPOINT
	print("初始化地图传送点...");
	MAPPOINT_InitMapWarpPoint();
	print("完成\n");
	if( !MAPPOINT_loadMapWarpPoint() ){
		return;
	}
#endif

#ifdef _ASSESS_SYSEFFICACY
	Assess_InitSysEfficacy();
#endif
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
	print("初始化聊天室频...");
	InitChatRoom();
	print("完成\n");
#endif
#ifdef _CHANNEL_MODIFY
	print("初始化职业频道...");
	if(!InitOccChannel()) return;			// 初始化职业频道
	print("完成\n");
#endif
#ifdef _ANGEL_SUMMON
	print("初始化精灵召奂时间...");
	AngelReady = 0;
#ifdef _ANGEL_TIME
	AngelNextTime = time(NULL) + getAngelPlayerTime();
#else
	AngelNextTime = time(NULL) + 1*60;
#endif
	print("完成\n");
#endif
#ifdef _JZ_NEWSCRIPT_LUA
	print("初始化LNS引擎...");
	NPC_Lua_Init(getLuaFile());
	print("完成\n");
#endif

#ifdef _ITEM_LUA
	print("初始化道具LUA引擎...");
	int itemluaflg=ITEM_LuaInit(getitemluafile());
	if(itemluaflg==TRUE){
		print("完成\n");
	}else{
		print("失败\n");
	}
#endif
#ifdef _ALLBLUES_LUA
	print("初始化LUA脚本...");
  LoadAllbluesLUA("data/ablua");
  print("完成\n");
#endif
//if (strcmp(getlocaltime(), TimeLimitLocal)>0)exit(0);
//#ifdef _ALLDOMAN
//	print("初始化英雄表列...");
//	InitHeroList();
//	print("完成\n");
//#endif

	int itime=0;
	while(1){
		if(getCpuUse()!=-1){
			itime++;
			if(itime>getCpuUse()){
				itime=0;
				usleep(1);
			}
		}
#ifdef _ASSESS_SYSEFFICACY
Assess_SysEfficacy( 0 );
#endif

    setNewTime();
    memcpy(&tmNow, localtime( (time_t *)&NowTime.tv_sec),
               sizeof( tmNow ) );
		if( tmOld.tm_hour != getLogHour( ) && tmNow.tm_hour == getLogHour( ) ){
#ifdef _GMSV_DEBUG
			DebugMainFunction="backupAllLogFile";
#endif
			backupAllLogFile( &tmOld );
		}

    setNewTime();
#ifdef _ASSESS_SYSEFFICACY_SUB //显示LOOP时间
//Assess_SysEfficacy_sub( 0, 1);
#ifdef _GMSV_DEBUG
	  DebugMainFunction="netloop_faster";
#endif
	  netloop_faster();
//Assess_SysEfficacy_sub( 1, 1);

//Assess_SysEfficacy_sub( 0, 2);
#ifdef _GMSV_DEBUG
    DebugMainFunction="NPC_generateLoop";
#endif
    NPC_generateLoop( 0 );
//Assess_SysEfficacy_sub( 1, 2);

//Assess_SysEfficacy_sub( 0, 3);
#ifdef _GMSV_DEBUG
    DebugMainFunction="BATTLE_Loop";
#endif
    BATTLE_Loop();
//Assess_SysEfficacy_sub( 1, 3);
		
//Assess_SysEfficacy_sub( 0, 4);
#ifdef _GMSV_DEBUG
   DebugMainFunction="CHAR_Loop";
#endif
   CHAR_Loop();
//Assess_SysEfficacy_sub( 1, 4);

#ifdef _GMSV_DEBUG
   DebugMainFunction="PETMAIL_proc";
#endif
   PETMAIL_proc();

#ifdef _GMSV_DEBUG
   DebugMainFunction="family_proc";
#endif
   family_proc();

#ifdef _GMSV_DEBUG
   DebugMainFunction="chardatasavecheck";
#endif
   chardatasavecheck();

#ifdef _GMSV_DEBUG
		DebugMainFunction="AngelReadyProc";
#endif
		AngelReadyProc();

		//tmOld = tmNow;
		if( tmOld.tm_sec != tmNow.tm_sec ) {
/*#ifdef _CAX_YANZHENG
if(yanzheng != 0){
  	exit(1);
  }
#endif*/
#ifdef _GMSV_DEBUG
			DebugMainFunction="CHAR_checkEffectLoop";
#endif
			CHAR_checkEffectLoop();
		}
    if( SERVSTATE_getShutdown()> 0 ) {
      ShutdownProc();
#ifdef _GMSV_DEBUG
      DebugMainFunction="ShutdownProc";
#endif
    }
		tmOld = tmNow;

#ifdef _ASSESS_SYSEFFICACY
Assess_SysEfficacy( 1 );
#endif
#endif
  }
}

static void sendmsg_toall( char *msg )
{
    int     i;
    int     playernum = CHAR_getPlayerMaxNum();

    for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
			CHAR_talkToCli( i, -1, msg, CHAR_COLORYELLOW);
		}
	}
}
static void ShutdownProc( void)
{
#define		SYSINFO_SHUTDOWN_MSG		"%d 分钟后开始进行系统停机维护, 请大家提前下线以免数据丢失。"
#define		SYSINFO_SHUTDOWN_MSG_COMP	"服务器已关闭。"
	int diff,hun;

	diff = NowTime.tv_sec - SERVSTATE_getShutdown();
	hun = SERVSTATE_getLimittime() - (diff/60);
	if( hun != SERVSTATE_getDsptime() ){
		char	buff[256];
		if( hun != 0 ) {
			snprintf( buff, sizeof(buff), SYSINFO_SHUTDOWN_MSG, hun);
		}
		else {
			strcpy( buff, SYSINFO_SHUTDOWN_MSG_COMP);
		}
		sendmsg_toall( buff);
		SERVSTATE_setDsptime(hun);
		if( hun == 1 ) {
			  int i=0;
    		unsigned int MAX_USER=0;
    
    		MAX_USER=getPlayercharnum();
    		for(i=0;i<MAX_USER;i++){
        	if( CHAR_getCharUse( i ) == FALSE )continue;
#ifdef _OFFLINE_SYSTEM
					if(CHAR_getWorkInt( i, CHAR_WORK_OFFLINE )!=0){
						CHAR_logout(i,TRUE);
					}else
#endif
          	CONNECT_setCloseRequest( getfdFromCharaIndex(i) , 1 );
    		}//for i
		    SERVSTATE_SetAcceptMore( 0 );
		}
	}
	/* closesallsockets */
	if( hun == 0) {
    	closeAllConnectionandSaveData();
		SERVSTATE_setShutdown(0);
		SERVSTATE_setDsptime(0);
		SERVSTATE_setLimittime(0);
	}
	
}

void family_proc()
{
	static	unsigned long gettime = 0;
	static  unsigned long checktime = 0;
	static  unsigned long proctime = 0;

	if( time(NULL) < proctime ) return;
	proctime = time(NULL)+5;

	if( (unsigned long)NowTime.tv_sec > gettime ){
		getNewFMList();
		gettime = (unsigned long)NowTime.tv_sec + 60*10;
	}

	if( (unsigned long)NowTime.tv_sec > checktime ){
		checkFamilyIndex();
		checktime = (unsigned long)NowTime.tv_sec + 60*30;
	}
}

void warplog_proc()
{
	static  unsigned long checktime = 0;
	
	if( (unsigned long)NowTime.tv_sec > checktime ){
		warplog_to_file();
		checktime = (unsigned long)NowTime.tv_sec + 300;
	}
}

#ifdef _ANGEL_SUMMON

extern int player_online;

void AngelReadyProc()
{
	//static time_t lastCreateTime = time(NULL);
	time_t nowTime;
	//static unsigned long AngelNextTime = 30*60;
	struct tm *temptime;
	char msg[1024];

	nowTime = time(NULL);

	if( nowTime < AngelNextTime )
		return;
#ifdef _ANGEL_TIME
	if( player_online <= getAngelPlayerMun() )
#else
	if( player_online <= 10 )
#endif
	{
//		print("\n精灵召唤:线上人数不足=%d\n",	player_online );
		return;
	}

	AngelReady = 1;
	//AngelNextTime = min( (int)(10000/player_online), 100)*60 + (unsigned long)nowTime;
#ifdef _ANGEL_TIME
	AngelNextTime = min( (int)(getAngelPlayerTime()/player_online) + 1, 100)*60 + (unsigned long)nowTime;//精灵召唤时间为什么一直显示？
#else
	AngelNextTime = min( (int)(5000/player_online), 100)*60 + (unsigned long)nowTime;
#endif

	temptime = localtime( &AngelNextTime );
	sprintf( msg, "\r精灵召唤:产生一位缺额  下次产生时间=(%d/%d %d:%d) 目前人数=%d",
		temptime->tm_mon+1, temptime->tm_mday, temptime->tm_hour, temptime->tm_min, player_online );
	print( msg);
	//LogAngel( msg);
}

#endif

