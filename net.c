#define __NET_C__
#include "version.h"
//#define __FD_SETSIZE 2048
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
//ttom+1

#include <sys/timeb.h>
#define _SOCKET_NONBLOCK
#ifdef _SOCKET_NONBLOCK
#include <fcntl.h>
#endif
#include "net.h"
#include "buf.h"
#include "link.h"
#include "common.h"
#include "msignal.h"
#include "configfile.h"
#include "util.h"
#include "saacproto_cli.h"
#include "lssproto_serv.h"
#include "char.h"
#include "handletime.h"
#include "log.h"
#include "object.h"
#include "title.h"
#include "item_event.h"
#include "enemy.h"
// Arminius 7.31 cursed stone
#include "battle.h"
#include "version.h"
#include "pet_event.h"
#include "char_talk.h"
#include "petmail.h"
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#include "attestation.h"


#define MIN(x,y)     ( ( (x) < (y) ) ? (x) : (y) )

#ifdef _NEW_SERVER_
BOOL bNewServer = TRUE;
#else
BOOL bNewServer = FALSE;
#endif

// Nuke +1 0901: For state monitor
int StateTable[ WHILESAVEWAIT + 1 ];

int ITEM_getRatio();
int CHAR_players();
#define CONO_CHECK_LOGIN 0x001
#define CONO_CHECK_ITEM 0x010
#define CONO_CHECK_PET 0x100
int cono_check = 0x111;
extern int questionstarttime;
extern int questionman;
extern char answer[64];
int AC_WBSIZE = ( 1024*64*16 );
//ttom+1 for the performatce
static unsigned int MAX_item_use = 0;
int i_shutdown_time = 0; //ttom
BOOL b_first_shutdown = FALSE; //ttom

int mfdfulll = 0;
int yzcnt = 0;
unsigned int atttime=0;
#ifdef _NO_ATTACK
static unsigned long useripnum = 100000; 
static unsigned long* userip = 0;
static unsigned long* userip_count_data = 0;
static unsigned long useripcount = 0;
static unsigned long* useriptime = 0;
#endif
#ifdef	_PK_LUCK_MAN
int pkluckhour = -1;
int pkluckmin = -1;
int pklucknum = 0;
#endif
extern int autopkstart;
static int autopkend=0;
static int autopkone = 0;
extern char *DebugMainFunction;
/*------------------------------------------------------------
* �ӡ��Ｐ����
------------------------------------------------------------*/
typedef struct tag_serverState
{
	BOOL            acceptmore;     /*  1�������գ�accept ����ؤ��
									�н���ƥ��close ���� */
	unsigned int    fdid;           /*  fd ��ɧԻ�� */
	unsigned int    closeallsocketnum;  /*   closeallsocket   ����Ի��
										��*/
	int				shutdown;		/*  �ӡ���ëshutdown����ƹ���� 
									*	0:ɧ�� ��ľ��½:����������������ƹ����
									* ƹ�����ئ�������޻�  �Ȼ��£�
									*/
	int				dsptime;		/* shutdown ƹ���񼰷�����  ��*/
	int				limittime;		/* ��ľ�� */
}ServerState;

#ifdef	_CAX_LNS_NLSUOXU
typedef struct tagFuncKk
{
	char    *FileName;       /*�ű�·��*/
	char    *FuncName;       /*ִ�к���*/
	unsigned int     EspTime;
	unsigned int     MespTime;
	int     ID;
	unsigned int     GetTime;
}FuncKk;

FuncKk TimerLua[50];
void SetTimerLua(void)//��ʼ��LUA��ʱ��
{
	int i;
	for( i = 0 ; i < arraysizeof( TimerLua ) ; i ++ ){
		TimerLua[i].FileName = NULL;
		TimerLua[i].FuncName = NULL;
		TimerLua[i].EspTime = -1;
		TimerLua[i].MespTime=-1;
		TimerLua[i].ID = -1;
		TimerLua[i].GetTime = -1;
	}
}

int SetTimer_net(char *FileName,char *FuncName,unsigned int EspTime)
{       
	int i;
	for( i = 0 ; i < arraysizeof( TimerLua ) ; i ++ ){
		if (TimerLua[i].ID == -1) break;
	}
	TimerLua[i].FileName = FileName;
	TimerLua[i].FuncName = FuncName;
	TimerLua[i].EspTime = NowTime.tv_sec;
	TimerLua[i].MespTime= NowTime.tv_usec;
	TimerLua[i].ID = i+1;
	TimerLua[i].GetTime = EspTime*1024;
	return i+1;
}
#endif

typedef struct tagCONNECT
{
	BOOL use;

	char *rb;
	int rbuse;
	char *wb;
	int wbuse;
	int check_rb_oneline_b;
	int check_rb_time;
	pthread_mutex_t mutex;

	struct sockaddr_in sin; /* �����Ƽ�ʧ������ */
	ConnectType ctype;       /* ��������������˒� */

	char cdkey[ CDKEYLEN ];    /* CDKEY */
	char passwd[ PASSWDLEN ];  /* �ɵ������� */
	LoginType state;        /* �ػ�������̼����� */
	int nstatecount;
	char charname[ CHARNAMELEN ];  /* ����̼��w��ƽ�ҷo */
	int charaindex;     /* char?�d�߼��̼������͵��z
						* ����̼�ԉ�����ɬ�ý�ľ�©z-1�������ɻ���
						* ?�d��ئ���ݩz
						*/
	char CAbuf[ 2048 ];         /*  CA() ë�������¿м��������� */
	int CAbufsiz;       /*  CAbuf ��������  */

	struct timeval lastCAsendtime;     /*��ԉ��CAë˪�������� */

	char CDbuf[ 2048 ];         /*  CD() ë�������¿м��������� */
	int CDbufsiz;       /*  CDbuf ��������  */

	struct timeval lastCDsendtime;     /*��ԉ��CDë˪�������� */

	struct timeval lastCharSaveTime; /* ��ԉ��ƽ�ҷ·�����ë����Ƥ�������� */

	struct timeval lastprocesstime;    /* ��ԉ����������ë�ʆA��������*/

	struct timeval lastreadtime;       /* ��ԉ��read�������ީz���練�л���*/

	// Nuke start 08/27 : For acceleration avoidance
	// WALK_TOLERANCE: Permit n W messages in a second (3: is the most restricted)
#define WALK_TOLERANCE 4
#define WALK_SPOOL 5
#define WALK_RESTORE 100
	unsigned int Walktime;
	unsigned int lastWalktime;
	unsigned int Walkcount;
	int Walkspool;      // For walk burst after release key F10
	int Walkrestore;
	// B3_TOLERANCE: Time distance between recently 3 B message (8: is the latgest)
	// BEO_TOLERANCE: Time distance between the lastmost B and EO (5: is the largest)
#define B3_TOLERANCE 5
#define BEO_TOLERANCE 3
#define BEO_SPOOL 10
#define BEO_RESTORE 100
	unsigned int Btime;
	unsigned int lastBtime;
	unsigned int lastlastBtime;
	unsigned int EOtime;

#ifdef _BATTLE_TIMESPEED
	// unsigned int  DefBtime;
	int BDTime;
	int CBTime;
#endif

#ifdef _TYPE_TOXICATION
	int toxication;
#endif

#ifdef _ITEM_ADDEXP //vincent ������N
	int EDTime;
#endif
	//    unsigned int      BEO;
	int BEOspool;
	int BEOrestore;
	// Nuke 0219: Avoid cheating
	int die;
	// Nuke end
	// Nuke 0310
	int credit;
	int fcold;
	// Nuke 0406: New Flow Control
	int nu;
	int nu_decrease;
	int ke;
	// Nuke 1213: Flow Control 2
	int packetin;

	// Nuke 0624: Avoid Null Connection
	unsigned int cotime;
	// Nuke 0626: For no enemy
	int noenemy;
	// Arminius 7.2: Ra's amulet
	int eqnoenemy;
#ifdef _Item_MoonAct
	int eqrandenemy;
#endif

#ifdef _CHIKULA_STONE
	int chistone;
#endif
	// Arminius 7.31: cursed stone
	int stayencount;

	int battlecharaindex[ CONNECT_WINDOWBUFSIZE ];
	int duelcharaindex[ CONNECT_WINDOWBUFSIZE ];
	int tradecardcharaindex[ CONNECT_WINDOWBUFSIZE ];
	int joinpartycharaindex[ CONNECT_WINDOWBUFSIZE ];

	// CoolFish: Trade 2001/4/18
	int tradecharaindex[ CONNECT_WINDOWBUFSIZE ];
	int errornum;
	int rightnum;
	int fdid;

	int close_request; //the second have this

	int appendwb_overflow_flag;  /* 1��ƥ��appendWb����?������1������ */
	//ttom+1 avoidance the watch the battle be kept out
	BOOL in_watch_mode;
	BOOL b_shut_up; //for avoid the user wash the screen
	BOOL b_pass;      //for avoid the unlimited area

	struct timeval Wtime;

	struct timeval WLtime;
	BOOL b_first_warp;
	int state_trans;

	// CoolFish: Trade 2001/4/18
	char TradeTmp[ 256 ];

#ifdef _ITEM_PILEFORTRADE
	int tradelist;
#endif
	// Shan Recvdata Time

	struct timeval lastrecvtime;      // 'FM' Stream Control time

	struct timeval lastrecvtime_d;    // DENGON Talk Control time

	// Arminius: 6.22 encounter
	int CEP; // Current Encounter Probability
	// Arminius 7.12 login announce
	int announced;

	// shan battle delay time 2001/12/26

	struct timeval battle_recvtime;
#ifdef _NO_WARP
	// shan hjj add Begin 
	int seqno;
	int selectbutton;
	// shan End
#endif
	unsigned int starttime;
	unsigned int connecttime;
	char pcid[64];
	char oldpcid[64];
	char defaultkey[32];
	BOOL confirm_key;    // shan  trade(DoubleCheck)
#ifdef _CAX_NEWMAC_LOGIN	
	char mac[13];
#endif
}
CONNECT;

CONNECT *Connect;     /*���ͥ�������ݤ���*/


/* �ؿ�����Ƭ�ˤĤ��Ƥ狼��褦�ˤ�������Υޥ��� */
#define SINGLETHREAD
#define MUTLITHREAD
#define ANYTHREAD

ServerState servstate;

pthread_mutex_t MTIO_servstate_m;  
#define SERVSTATE_LOCK() pthread_mutex_lock( &MTIO_servstate_m );
#define SERVSTATE_UNLOCK() pthread_mutex_unlock( &MTIO_servstate_m );
#define CONNECT_LOCK_ARG2(i,j) pthread_mutex_lock( &Connect[i].mutex );
#define CONNECT_UNLOCK_ARG2(i,j) pthread_mutex_unlock( &Connect[i].mutex );
#define CONNECT_LOCK(i) pthread_mutex_lock( &Connect[i].mutex );
#define CONNECT_UNLOCK(i) pthread_mutex_unlock( &Connect[i].mutex );
/*
#define SERVSTATE_LOCK()
#define SERVSTATE_UNLOCK()
#define CONNECT_LOCK_ARG2(i,j)
#define CONNECT_UNLOCK_ARG2(i,j)
#define CONNECT_LOCK(i)
#define CONNECT_UNLOCK(i)
*/
char*  encrypt_key(char*   ip)//���ܺ��� 
{       
	int   b[]={3,7,8,2,9,5,4}; 
	int   i=0; 
	while(*ip!=NULL) 
	{
		*ip+=b[i%7]; 
		if(*ip> 122) 
			*ip=*ip%122+32; 
		i++,ip++; 
	}
	return ip;
}

char*  decrypt_key(char*   id)//���ܺ��� 
{ 
	int   b[]={3,7,8,2,9,5,4}; 
	int   i=0; 
	while(*id!=NULL) 
	{
		*id-=b[i%7]; 
		if(*id <32) 
			*id=*id-32+122; 
		i++,id++; 
	}
	return id;
}
/*------------------------------------------------------------
* servstateë��������©z
* ¦�ѩy߯Ի��
*  ئ��
------------------------------------------------------------*/
ANYTHREAD static void SERVSTATE_initserverState( void )
{
	SERVSTATE_LOCK();
	servstate.acceptmore = TRUE;
	servstate.fdid = 0;
	servstate.closeallsocketnum = -1;
	servstate.shutdown = 0;
	servstate.limittime = 0;
	servstate.dsptime = 0;
	SERVSTATE_UNLOCK();
}

ANYTHREAD int SERVSTATE_SetAcceptMore( int nvalue )
{
	BOOL buf;
	SERVSTATE_LOCK();
	buf = servstate.acceptmore;
	servstate.acceptmore = nvalue;
	SERVSTATE_UNLOCK();
	return buf;
}
ANYTHREAD static int SERVSTATE_incrementFdid( void )
{
	int ret;
	SERVSTATE_LOCK();
	ret = servstate.fdid++;
	SERVSTATE_UNLOCK();
	return ret;
}
ANYTHREAD static void SERVSTATE_setCloseallsocketnum( int a )
{
	SERVSTATE_LOCK();
	servstate.closeallsocketnum = a;
	SERVSTATE_UNLOCK();
}
ANYTHREAD static void SERVSTATE_incrementCloseallsocketnum(void)
{
	SERVSTATE_LOCK();
	servstate.closeallsocketnum ++;
	SERVSTATE_UNLOCK();
}
ANYTHREAD void SERVSTATE_decrementCloseallsocketnum(void)
{
	SERVSTATE_LOCK();
	servstate.closeallsocketnum --;
	SERVSTATE_UNLOCK();
}
ANYTHREAD int SERVSTATE_getCloseallsocketnum( void )
{
	int a;
	SERVSTATE_LOCK();
	a = servstate.closeallsocketnum;
	SERVSTATE_UNLOCK();
	return a;
}

ANYTHREAD static int SERVSTATE_getAcceptmore(void)
{
	int a;
	SERVSTATE_LOCK();
	a = servstate.acceptmore;
	SERVSTATE_UNLOCK();
	return a;
}
ANYTHREAD int SERVSTATE_getShutdown(void)
{
	int a;
	SERVSTATE_LOCK();
	a = servstate.shutdown;
	SERVSTATE_UNLOCK();
	return a;
}
ANYTHREAD void SERVSTATE_setShutdown(int a)
{
	SERVSTATE_LOCK();
	servstate.shutdown = a;
	SERVSTATE_UNLOCK();
}
ANYTHREAD int SERVSTATE_getLimittime(void)
{
	int a;
	SERVSTATE_LOCK();
	a = servstate.limittime;
	SERVSTATE_UNLOCK();
	return a;
}
ANYTHREAD void SERVSTATE_setLimittime(int a)
{
	SERVSTATE_LOCK();
	servstate.limittime = a;
	SERVSTATE_UNLOCK();
}
ANYTHREAD int SERVSTATE_getDsptime(void)
{
	int a;
	SERVSTATE_LOCK();
	a = servstate.dsptime;
	SERVSTATE_UNLOCK();
	return a;
}
ANYTHREAD void SERVSTATE_setDsptime(int a)
{
	SERVSTATE_LOCK();
	servstate.dsptime = a;
	SERVSTATE_UNLOCK();
}

static int appendWB( int fd, char *buf, int size )
{
	if ( fd != acfd ) {
		if ( Connect[ fd ].wbuse + size >= WBSIZE ) {
			//print( "appendWB:err buffer over[%d]:%s \n",
				//Connect[ fd ].wbuse + size, Connect[ fd ].cdkey );
			return -1;
		}
	}else {
		if ( Connect[ fd ].wbuse + size > AC_WBSIZE ) {
			FILE * fp = NULL;
			//print( "appendWB:err buffer over[%d+%d]:(SAAC) \n", Connect[ fd ].wbuse, size );

			if ( ( fp = fopen( "appendWBerr.log", "a+" ) ) == NULL ) return -1;

			fprintf( fp, "(SAAC) appendWB:err buffer over[%d+%d/%d]:\n", Connect[ fd ].wbuse, size, AC_WBSIZE );

			fclose( fp );

			return -1;
		}
	}



	memcpy( Connect[ fd ].wb + Connect[ fd ].wbuse ,
		buf, size );
	Connect[ fd ].wbuse += size;
	return size;
}
static int appendRB( int fd, char *buf, int size )
{
	if ( fd != acfd ) {
		if ( Connect[ fd ].rbuse + size > RBSIZE ) {
			//print( "appendRB:OTHER(%d) err buffer over \n", fd );

			return -1;
		}
	}else {
		if ( strlen( buf ) > size ) {
			//print( "appendRB AC buffer len err : %d/%d=\n(%s)!!\n", strlen( buf ), size, buf );
		}

		if ( Connect[ fd ].rbuse + size > AC_RBSIZE ) {
			//print( "appendRB AC err buffer over:\n(%s)\n len:%d - rbuse:%d \n",
				//buf, strlen( buf ), Connect[ fd ].rbuse );
			return -1;
		}
	}

	memcpy( Connect[ fd ].rb + Connect[ fd ].rbuse , buf, size );
	Connect[ fd ].rbuse += size;
	return size;
}

static int shiftWB( int fd, int len )
{
	if ( Connect[ fd ].wbuse < len ) {
		//print( "shiftWB: err\n" );
		return -1;
	}

	memmove( Connect[ fd ].wb, Connect[ fd ].wb + len, Connect[ fd ].wbuse - len );
	Connect[ fd ].wbuse -= len;

	if ( Connect[ fd ].wbuse < 0 ) {
		//print( "shiftWB:wbuse err\n" );
		Connect[ fd ].wbuse = 0;
	}

	return len;
}

static int shiftRB( int fd, int len )
{
	if ( Connect[ fd ].rbuse < len ) {
		//print( "shiftRB: err\n" );
		return -1;
	}

	memmove( Connect[ fd ].rb, Connect[ fd ].rb + len, Connect[ fd ].rbuse - len );
	Connect[ fd ].rbuse -= len;

	if ( Connect[ fd ].rbuse < 0 ) {
		//print( "shiftRB:rbuse err\n" );
		Connect[ fd ].rbuse = 0;
	}

	return len;
}

SINGLETHREAD int lsrpcClientWriteFunc( int fd , char* buf , int size )
{
	int r;

	if ( Connect[ fd ].use == FALSE ) {
		return FALSE;
	}

	if ( Connect[ fd ].appendwb_overflow_flag ) {
		//print( "lsrpcClientWriteFunc: buffer overflow fd:%d\n" , fd );
		return -1;
	}

	r = appendWB( fd, buf , size );

	// Nuke *1 0907: Ignore acfd from WB error

	if ( ( r < 0 ) && ( fd != acfd ) ) {
		Connect[ fd ].appendwb_overflow_flag = 1;
		//print("����11111");
		CONNECT_endOne_debug( fd );
		// Nuke + 1 0901: Why close
		//  print("closed in lsrpcClientWriteFunc");
	}

	return r;
}

static int logRBuseErr = 0;
SINGLETHREAD BOOL GetOneLine_fix( int fd, char *buf, int max )
{
	int i;

	if ( Connect[ fd ].rbuse == 0 ) return FALSE;

	if ( Connect[ fd ].check_rb_oneline_b == 0 &&
		Connect[ fd ].check_rb_oneline_b == Connect[ fd ].rbuse ) {
			return FALSE;
	}


	for ( i = 0; i < Connect[ fd ].rbuse && i < ( max - 1 ); i ++ ) {
		if ( Connect[ fd ].rb[ i ] == '\n' ) {
			memcpy( buf, Connect[ fd ].rb, i + 1 );
			buf[ i + 1 ] = '\0';
			shiftRB( fd, i + 1 );

			//--------
			/*
			//andy_log
			if( strstr( Connect[fd].rb , "ACCharLoad") != NULL &&
			Connect[fd].check_rb_oneline_b != 0 )//Connect[fd].rb
			LogAcMess( fd, "GetOne", Connect[fd].rb );
			*/
			//--------
			logRBuseErr = 0;
			Connect[ fd ].check_rb_oneline_b = 0;
			Connect[ fd ].check_rb_time = 0;
			return TRUE;
		}
	}

	//print("rbuse lens: %d!!\n", Connect[fd].rbuse);
	logRBuseErr++;

	//--------
	//andy_log
	if ( fd == acfd && strstr( Connect[ fd ].rb , "ACCharLoad" ) != NULL &&
		logRBuseErr >= 50 ) { //Connect[fd].rb
			char buf[ AC_RBSIZE ];
			memcpy( buf, Connect[ fd ].rb, Connect[ fd ].rbuse + 1 );
			buf[ Connect[ fd ].rbuse + 1 ] = 0;
			LogAcMess( fd, "RBUFFER", buf );
			logRBuseErr = 0;
	}

	//--------
	Connect[ fd ].check_rb_oneline_b = Connect[ fd ].rbuse;

	return FALSE;
}


char *getRandStr(int num){
	char str[num+1];
 	int i;
 	struct timeval tv;
	struct timezone tz;
	gettimeofday (&tv , &tz);
	srand(tv.tv_usec);
 	for(i = 0; i < num; i++)
 	{
 	 int a = 0;
  	a = ( rand() %2) ? ( rand() %26 + 65) : ( rand()% 10 + 48);
  	str[i] = a;
 	}
 	str[num] = '\0';
 	return str;
}

ANYTHREAD BOOL initConnectOne( int sockfd, struct sockaddr_in* sin ,int len )
{
	CONNECT_LOCK( sockfd );

	Connect[ sockfd ].use = TRUE;
	Connect[ sockfd ].ctype = NOTDETECTED;
	Connect[ sockfd ].wbuse = Connect[ sockfd ].rbuse = 0;
	Connect[ sockfd ].check_rb_oneline_b = 0;
	Connect[ sockfd ].check_rb_time = 0;

	memset( Connect[ sockfd ].cdkey , 0 , sizeof( Connect[ sockfd ].cdkey ) );
	memset( Connect[ sockfd ].passwd, 0 , sizeof( Connect[ sockfd ].passwd ) );

	Connect[ sockfd ].state = NOTLOGIN;
	Connect[ sockfd ].nstatecount = 0;
	memset( Connect[ sockfd ].charname, 0, sizeof( Connect[ sockfd ].charname ) );
	Connect[ sockfd ].charaindex = -1;

	Connect[ sockfd ].CAbufsiz = 0;
	Connect[ sockfd ].CDbufsiz = 0;
	Connect[ sockfd ].rbuse = 0;
	Connect[ sockfd ].wbuse = 0;
	Connect[ sockfd ].check_rb_oneline_b = 0;
	Connect[ sockfd ].check_rb_time = 0;

	Connect[ sockfd ].close_request = 0;      /* �Ӭۢ�ư׷º� */
	// Nuke 08/27 For acceleration avoidance
	Connect[ sockfd ].Walktime = 0;
	Connect[ sockfd ].lastWalktime = 0;
	Connect[ sockfd ].Walkcount = 0;
	Connect[ sockfd ].Walkspool = WALK_SPOOL;
	Connect[ sockfd ].Walkrestore = WALK_RESTORE;
	Connect[ sockfd ].Btime = 0;
	Connect[ sockfd ].lastBtime = 0;
	Connect[ sockfd ].lastlastBtime = 0;
	Connect[ sockfd ].EOtime = 0;
	Connect[ sockfd ].nu_decrease = 0;
#ifdef _BATTLE_TIMESPEED
	// Connect[sockfd].DefBtime = 0;
	Connect[ sockfd ].BDTime = 0;
	Connect[ sockfd ].CBTime = 0;
#endif
#ifdef _TYPE_TOXICATION
	Connect[ sockfd ].toxication = 0;
#endif
#ifdef _ITEM_ADDEXP	//vincent ��������
	Connect[ sockfd ].EDTime = 0;
#endif
	//      Connect[sockfd].BEO = 0;
	Connect[ sockfd ].BEOspool = BEO_SPOOL;
	Connect[ sockfd ].BEOrestore = BEO_RESTORE;
	//ttom
	Connect[ sockfd ].b_shut_up = FALSE;
	Connect[ sockfd ].Wtime.tv_sec = 0; //
	Connect[ sockfd ].Wtime.tv_usec = 0; //
	Connect[ sockfd ].WLtime.tv_sec = 0; //
	Connect[ sockfd ].WLtime.tv_usec = 0; //
	Connect[ sockfd ].b_first_warp = FALSE;
	Connect[ sockfd ].state_trans = 0; //avoid the trans
	// Nuke
	Connect[ sockfd ].die = 0;
	Connect[ sockfd ].credit = 3;
	Connect[ sockfd ].fcold = 0;
	// Nuke 0406: New Flow Control
	Connect[ sockfd ].nu = 30;
	Connect[ sockfd ].ke = 10;
	// Nuke 1213: Flow Control 2
	Connect[ sockfd ].packetin = 30; // if 10x10 seconds no packet, drop the line

	// Nuke 0624: Avoid Useless Connection
	Connect[ sockfd ].cotime = 0;
	// Nuke 0626: For no enemy
	Connect[ sockfd ].noenemy = 0;
	// Arminius 7.2: Ra's amulet
	Connect[ sockfd ].eqnoenemy = 0;

#ifdef _Item_MoonAct
	Connect[ sockfd ].eqrandenemy = 0;
#endif
#ifdef _CHIKULA_STONE
	Connect[ sockfd ].chistone = 0;
#endif
	// Arminius 7.31: cursed stone
	Connect[ sockfd ].stayencount = 0;

	// CoolFish: Init Trade 2001/4/18
	memset( &Connect[ sockfd ].TradeTmp, 0, sizeof( Connect[ sockfd ].TradeTmp ) );
#ifdef _ITEM_PILEFORTRADE
	Connect[ sockfd ].tradelist = -1;
#endif
	// Arminius 6.22 Encounter
	Connect[ sockfd ].CEP = 0;

	// Arminius 7.12 login announce
	Connect[ sockfd ].announced = 0;

#ifdef _NO_WARP
	// shan hjj add Begin
	Connect[sockfd].seqno=-1;
	Connect[sockfd].selectbutton=1;
	// shan End
#endif
	// ��¼��ʼʱ��
	time_t curtime;
	struct tm *p;
	time(&curtime);
	p=localtime(&curtime);
	curtime = mktime(p);
	Connect[ sockfd ].starttime = curtime;
	
	memset( Connect[ sockfd ].pcid, 0 , sizeof( Connect[ sockfd ].pcid ) );
  memset( Connect[ sockfd ].oldpcid, 0 , sizeof( Connect[ sockfd ].oldpcid ) );
	Connect[ sockfd ].confirm_key = FALSE;   // shan trade(DoubleCheck)

	if ( sin != NULL ) memcpy( &Connect[ sockfd ].sin , sin , len );

	memset( &Connect[ sockfd ].lastprocesstime, 0 ,
		sizeof( Connect[ sockfd ].lastprocesstime ) );

	memcpy( &Connect[ sockfd ].lastCAsendtime, &NowTime ,
		sizeof( Connect[ sockfd ].lastCAsendtime ) );

	memcpy( &Connect[ sockfd ].lastCDsendtime, &NowTime ,
		sizeof( Connect[ sockfd ].lastCDsendtime ) );

	memcpy( &Connect[ sockfd ].lastCharSaveTime, &NowTime ,
		sizeof( Connect[ sockfd ].lastCharSaveTime ) );

	// Shan Add
	memcpy( &Connect[ sockfd ].lastrecvtime, &NowTime ,
		sizeof( Connect[ sockfd ].lastrecvtime ) );

	memcpy( &Connect[ sockfd ].lastrecvtime_d, &NowTime ,
		sizeof( Connect[ sockfd ].lastrecvtime_d ) );

	memcpy( &Connect[ sockfd ].battle_recvtime, &NowTime ,
		sizeof( Connect[ sockfd ].battle_recvtime ) );

	memcpy( &Connect[ sockfd ].lastreadtime , &NowTime,

		sizeof( struct timeval ) );

	Connect[ sockfd ].lastreadtime.tv_sec -= DEBUG_ADJUSTTIME;

	Connect[ sockfd ].errornum = 0;
	
	Connect[ sockfd ].rightnum = 0;

	Connect[ sockfd ].fdid = SERVSTATE_incrementFdid();

	CONNECT_UNLOCK( sockfd );

	Connect[ sockfd ].appendwb_overflow_flag = 0;
	
	Connect[sockfd].connecttime = time(NULL);
	
	memset( Connect[ sockfd ].defaultkey , 0 , sizeof( Connect[ sockfd ].defaultkey ) );
	strcpy(Connect[ sockfd ].defaultkey,getRandStr(10));

	return TRUE;
}

ANYTHREAD BOOL _CONNECT_endOne( char *file, int fromline, int sockfd , int line )
{
	if(errno==113 || errno==104){
		return;
	}
	CONNECT_LOCK_ARG2(sockfd,line);

	if( Connect[sockfd].use == FALSE ){
		CONNECT_UNLOCK_ARG2(sockfd,line);
		//andy_log
		//print("�����ѹر�[%d]!!\n", sockfd );
		return TRUE;
	}
	Connect[sockfd].use = FALSE;
	if( Connect[sockfd].ctype == CLI ){
		CONNECT_UNLOCK_ARG2( sockfd,line );
		if(Connect[sockfd].charaindex >= 0){
#ifdef _OFFLINE_SYSTEM
		if(CHAR_getWorkInt( Connect[sockfd].charaindex, CHAR_WORK_OFFLINE ) != 0){
			CHAR_setWorkInt( Connect[sockfd].charaindex, CHAR_WORKFD, -1);
		}else
#endif
	{
		if( !CHAR_logout( Connect[sockfd].charaindex,TRUE )) {
			//print( "err %s:%d from %s:%d \n", __FILE__, __LINE__, file, fromline);
		}
		//print("\ndrop line from %s:%d,__FD_SETSIZE=%d\n",file,fromline,__FD_SETSIZE);
		sasql_online(Connect[sockfd].cdkey,NULL,NULL,NULL,NULL,0);
		print( "�˳��˺�=%s \n", Connect[sockfd].cdkey );
	}
		Connect[sockfd].charaindex=-1;
	}else{
		if(strlen(Connect[sockfd].cdkey)>0)
			sasql_online(Connect[sockfd].cdkey,NULL,NULL,NULL,NULL,0);
	}
		CONNECT_LOCK_ARG2( sockfd ,line);
	}
#ifdef _NO_ATTACK
	else{
		if(strlen(Connect[sockfd].cdkey)<1){
			// ����ʱ���
			time_t curtime;
			struct tm *p;
			time(&curtime);
			p=localtime(&curtime);
			curtime = mktime(p);
			if( curtime - Connect[sockfd].starttime <= getAttTime() ){
				unsigned long tmpip=CONNECT_get_userip(sockfd);
				unsigned long i = 0;
				int ipa,ipb,ipc,ipd;
				char ip[32];
				ipa=(tmpip % 0x100); tmpip=tmpip / 0x100;
				ipb=(tmpip % 0x100); tmpip=tmpip / 0x100;
				ipc=(tmpip % 0x100); tmpip=tmpip / 0x100;
				ipd=(tmpip % 0x100);
				sprintf(ip,"%d.%d.%d.%d",ipa,ipb,ipc,ipd);
				for(; i < useripcount; ++i ){
					if( userip[i] == tmpip ){
						if(userip_count_data[i]>=getAttCnt()){
							Connect[sockfd].wbuse = 0;
							Connect[sockfd].rbuse = 0;
							Connect[sockfd].CAbufsiz = 0;
							Connect[sockfd].CDbufsiz = 0;
							CONNECT_UNLOCK_ARG2(sockfd,line);

							print( "�Ƿ����=%d �Ƿ�IP=%s \n", sockfd, ip );
							close(sockfd);
#ifdef _CAX_OFF_IPTABLES
							if(strcmp(ip,"0.0.0.0")!=0){
							char systemstr[256];
							sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
							system(systemstr);
							}
#endif
							userip_count_data[i] = 1;
							useriptime[i] = curtime;
							return TRUE;
						}
						if(curtime-useriptime[i]<=getAttSafeTime()){
							++userip_count_data[i];
							break;
						}else{
							useriptime[i]=curtime;
							break;
						}
					}
				}
				if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
				if(i>= useripcount && useripcount<useripnum){
					userip[useripcount] = tmpip;
					userip_count_data[useripcount] = 1;
					useriptime[useripcount] = curtime;
					useripcount++;
				}
				}
			}
		}
	}
#endif
	
	Connect[sockfd].wbuse = 0;
	Connect[sockfd].rbuse = 0;
	Connect[sockfd].CAbufsiz = 0;
	Connect[sockfd].CDbufsiz = 0;
	CONNECT_UNLOCK_ARG2(sockfd,line);
	//print( "���=%d \n", sockfd );
	close(sockfd);
	return TRUE;
}
SINGLETHREAD BOOL initConnect( int size )
{
	int i, j;
	ConnectLen = size;
	Connect = calloc( 1, sizeof( CONNECT ) * size );

	if ( Connect == NULL ) return FALSE;

	for ( i = 0 ; i < size ; i ++ ) {
		memset( &Connect[ i ] , 0 , sizeof( CONNECT ) );
		Connect[ i ].charaindex = -1;
		Connect[ i ].rb = calloc( 1, RBSIZE );

		if ( Connect[ i ].rb == NULL ) {
			fprint( "calloc err\n" );

			for ( j = 0; j < i ; j ++ ) {
				free( Connect[ j ].rb );
				free( Connect[ j ].wb );
			}

			return FALSE;
		}

		memset( Connect[ i ].rb, 0, RBSIZE );
		Connect[ i ].wb = calloc( 1, WBSIZE );

		if ( Connect[ i ].wb == NULL ) {
			fprint( "calloc err\n" );

			for ( j = 0; j < i ; j ++ ) {
				free( Connect[ j ].rb );
				free( Connect[ j ].wb );
			}

			free( Connect[ j ].rb );
			return FALSE;
		}

		memset( Connect[ i ].wb, 0, WBSIZE );

	}

	print( "ԤԼ %d ����...���� %.2f MB �ռ�...", size, (sizeof( CONNECT ) * size + RBSIZE * size + WBSIZE * size) / 1024.0 / 1024.0 );

	SERVSTATE_initserverState( );

	//ttom for the performance of gmsv
	MAX_item_use = getItemnum() * 0.98;
#ifdef _NO_ATTACK
	if( userip == 0 ) userip = malloc(sizeof(unsigned long)*useripnum);
	if( userip_count_data == 0 ) userip_count_data = malloc(sizeof(unsigned long)*useripnum);
	if( useriptime == 0 ) useriptime = malloc(sizeof(unsigned long)*useripnum);
#endif
	return TRUE;
}
BOOL CONNECT_acfdInitRB( int fd )
{
	if ( fd != acfd ) return FALSE;

	Connect[ fd ].rb = realloc( Connect[ acfd ].rb, AC_RBSIZE );

	if ( Connect[ acfd ].rb == NULL ) {
		fprint( "realloc err\n" );
		return FALSE;
	}

	memset( Connect[ acfd ].rb, 0, AC_RBSIZE );
	return TRUE;
}
BOOL CONNECT_acfdInitWB( int fd )
{
	if ( fd != acfd ) return FALSE;

	Connect[ fd ].wb = realloc( Connect[ acfd ].wb, AC_WBSIZE );

	if ( Connect[ acfd ].wb == NULL ) {
		fprint( "realloc err\n" );
		return FALSE;
	}

	memset( Connect[ acfd ].wb, 0, AC_WBSIZE );
	return TRUE;
}

ANYTHREAD void endConnect( void )
{
	int i;

	for ( i = 0 ; i < ConnectLen ; i ++ ) {
		int lco;
		lco = close( i );

		if ( lco == 0 ) {
			//print("����22222");
			CONNECT_endOne_debug( i );
		}

		free( Connect[ i ].rb );
		free( Connect[ i ].wb );
	}

	free( Connect );
}

ANYTHREAD BOOL CONNECT_appendCAbuf( int fd , char* data, int size )
{
	CONNECT_LOCK( fd );
	/*  �Ǹ�Υǥ�ߥ��� ',' ��ʬ¿�����ݤ��ʤ�����������   */
	if ( ( Connect[ fd ].CAbufsiz + size ) >= sizeof( Connect[ fd ].CAbuf ) ) {
		CONNECT_UNLOCK( fd );
		return FALSE;
	}

	memcpy( Connect[ fd ].CAbuf + Connect[ fd ].CAbufsiz , data , size );
	Connect[ fd ].CAbuf[ Connect[ fd ].CAbufsiz + size ] = ',';
	Connect[ fd ].CAbufsiz += ( size + 1 );
	CONNECT_UNLOCK( fd );
	return TRUE;
}

ANYTHREAD static int CONNECT_getCAbuf( int fd, char *out, int outmax,
									  int *outlen )
{
	CONNECT_LOCK( fd );

	if ( Connect[ fd ].use == TRUE ) {
		int cplen = MIN( outmax, Connect[ fd ].CAbufsiz );
		memcpy( out, Connect[ fd ].CAbuf , cplen );
		*outlen = cplen;
		CONNECT_UNLOCK( fd );
		return 0;
	} else {
		CONNECT_UNLOCK( fd );
		return -1;
	}
}
ANYTHREAD static int CONNECT_getCDbuf( int fd, char *out, int outmax,
									  int *outlen )
{
	if(fd<0) return 0;
	CONNECT_LOCK( fd );

	if ( Connect[ fd ].use == TRUE ) {
		int cplen = MIN( outmax, Connect[ fd ].CDbufsiz );
		memcpy( out, Connect[ fd ].CDbuf, cplen );
		*outlen = cplen;
		CONNECT_UNLOCK( fd );
		return 0;
	} else {
		CONNECT_UNLOCK( fd );
		return 0;
	}
}

ANYTHREAD static int CONNECT_setCAbufsiz( int fd, int len )
{
	CONNECT_LOCK( fd );

	if ( Connect[ fd ].use == TRUE ) {
		Connect[ fd ].CAbufsiz = len;
		CONNECT_UNLOCK( fd );
		return 0;
	} else {
		CONNECT_UNLOCK( fd );
		return -1;
	}
}
ANYTHREAD static int CONNECT_setCDbufsiz( int fd, int len )
{
	CONNECT_LOCK( fd );

	if ( Connect[ fd ].use == TRUE ) {
		Connect[ fd ].CDbufsiz = len;
		CONNECT_UNLOCK( fd );
		return 0;
	} else {
		CONNECT_UNLOCK( fd );
		return -1;
	}
}

ANYTHREAD static void CONNECT_setLastCAsendtime( int fd, struct timeval *t)
{
	CONNECT_LOCK( fd );
	Connect[ fd ].lastCAsendtime = *t;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD static void CONNECT_getLastCAsendtime( int fd, struct timeval *t )
{
	CONNECT_LOCK( fd );
	*t = Connect[ fd ].lastCAsendtime;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD static void CONNECT_setLastCDsendtime( int fd, struct timeval *t )
{
	CONNECT_LOCK( fd );
	Connect[ fd ].lastCDsendtime = *t;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD static void CONNECT_getLastCDsendtime( int fd, struct timeval *t )
{
	CONNECT_LOCK( fd );
	*t = Connect[ fd ].lastCDsendtime;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD int CONNECT_getUse_debug( int fd, int i )
{
	int a;
	CONNECT_LOCK_ARG2( fd, i );
	a = Connect[ fd ].use;
	CONNECT_UNLOCK_ARG2( fd, i );
	return a;

}

ANYTHREAD int CONNECT_getUse( int fd )
{
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].use;
	CONNECT_UNLOCK( fd );
	return a;
}
void CONNECT_setUse( int fd , int a)
//ANYTHREAD static void CONNECT_setUse( int fd , int a)
{
	CONNECT_LOCK( fd );
	Connect[ fd ].use = a;
	CONNECT_UNLOCK( fd );
}

void CONNECT_setStarttime( int fd , int a)
//ANYTHREAD static void CONNECT_setUse( int fd , int a)
{
	CONNECT_LOCK( fd );
	Connect[ fd ].starttime = a;
	CONNECT_UNLOCK( fd );
}

ANYTHREAD void CONNECT_checkStatecount( int a )
{
	int i;
	int count = 0;

	for ( i = 0; i < ConnectLen; i++ ) {
		if ( Connect[ i ].use == FALSE || Connect[ i ].state != a ) continue;

		if ( Connect[ i ].nstatecount <= 0 ) {
			Connect[ i ].nstatecount = ( int ) time( NULL ) + 60;
		}else{
			if ( Connect[ i ].nstatecount < ( int ) time( NULL ) ) {
				//print("����33333");
				CONNECT_endOne_debug( i );
				count++;
			}
		}
	}

	{
		memset( StateTable, 0, sizeof( StateTable ) );

		for ( i = 0; i < ConnectLen; i++ )
			if ( Connect[ i ].use == TRUE )
				StateTable[ Connect[ i ].state ] ++;
	}
}

ANYTHREAD int CONNECT_checkStateSomeOne( int a, int maxcount)
{
	char temp[ 80 ], buffer[ 1024 ];
	int i, ret = 1;

	if ( StateTable[ a ] >= maxcount ) ret = -1;

	buffer[ 0 ] = 0;

	for ( i = 0; i <= WHILESAVEWAIT; i++ ) {
		sprintf( temp, "%4d", StateTable[ i ] );
		strcat( buffer, temp );
	}

	//print( "\nNOW{{%s}}", buffer );
	return ret;
}

ANYTHREAD void CONNECT_setState( int fd , int a)
{
	CONNECT_LOCK( fd );
	Connect[ fd ].state = a;
	Connect[ fd ].nstatecount = 0;

	// Nuke start 0829: For debugging
	{
		char temp[80],buffer[128];
		int i;
		memset( StateTable, 0, sizeof( StateTable ) );

		for ( i = 0; i < ConnectLen; i++ )
			if ( Connect[ i ].use == TRUE )
				StateTable[ Connect[ i ].state ] ++;

		buffer[ 0 ] = 0;

		for ( i = 0; i <= WHILESAVEWAIT; i++ ) {
			sprintf( temp, "%4d", StateTable[ i ] );
			strcat( buffer, temp );
		}
		//		print( "\nFILE:%s,LINE:%d", file,fromline );
		//print( "\n{{%s}}", buffer );
	}
	// Nuke end

	CONNECT_UNLOCK( fd );
}

ANYTHREAD int CONNECT_getState( int fd )
{
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].state;
	CONNECT_UNLOCK( fd );
	return a;
}
ANYTHREAD void CONNECT_incrementErrornum(int fd )
{
	CONNECT_LOCK( fd );
	Connect[ fd ].errornum ++;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD void CONNECT_setCharaindex( int fd, int a )
{
	CONNECT_LOCK( fd );
	Connect[ fd ].charaindex = a;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD int CONNECT_getCharaindex( int fd )
{
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].charaindex;
	CONNECT_UNLOCK( fd );
	return a;
}
ANYTHREAD void CONNECT_getCdkey( int fd , char *out, int outlen )
{
	CONNECT_LOCK( fd );
	strcpysafe( out, outlen, Connect[ fd ].cdkey );
	CONNECT_UNLOCK( fd );
}

ANYTHREAD void CONNECT_setCdkey( int sockfd, char *cd )
{
	CONNECT_LOCK( sockfd );
	snprintf( Connect[ sockfd ].cdkey, sizeof( Connect[ sockfd ].cdkey ), "%s",
		cd );
	CONNECT_UNLOCK( sockfd );
}

ANYTHREAD void CONNECT_getPcid( int fd , char *out, int outlen )
{
	CONNECT_LOCK( fd );
	strcpysafe( out, outlen, Connect[ fd ].pcid );
	CONNECT_UNLOCK( fd );
}

ANYTHREAD void CONNECT_setPcid( int sockfd, char *pcid )
{
	CONNECT_LOCK( sockfd );
	snprintf( Connect[ sockfd ].pcid, sizeof( Connect[ sockfd ].pcid ), "%s",
		pcid );
	CONNECT_UNLOCK( sockfd );
}

#ifdef _CAX_NEWMAC_LOGIN
ANYTHREAD void CONNECT_getMAC( int fd , char *out, int outlen )
{
	CONNECT_LOCK( fd );
	strcpysafe( out, outlen, Connect[ fd ].mac );
	CONNECT_UNLOCK( fd );
}

ANYTHREAD void CONNECT_setMAC( int sockfd, char *mac )
{
	CONNECT_LOCK( sockfd );
	snprintf( Connect[ sockfd ].mac, sizeof( Connect[ sockfd ].mac ), "%s", mac );
	CONNECT_UNLOCK( sockfd );
}
#endif 

ANYTHREAD void CONNECT_getOldPcid( int fd , char *out, int outlen )
{
	CONNECT_LOCK( fd );
	strcpysafe( out, outlen, Connect[ fd ].oldpcid );
	CONNECT_UNLOCK( fd );
}

ANYTHREAD void CONNECT_setOldPcid( int sockfd, char *pcid )
{
	CONNECT_LOCK( sockfd );
	snprintf( Connect[ sockfd ].oldpcid, sizeof( Connect[ sockfd ].oldpcid ), "%s",
		pcid );
	CONNECT_UNLOCK( sockfd );
}

ANYTHREAD void CONNECT_getDefaultKey( int fd , char *out, int outlen )
{
	CONNECT_LOCK( fd );
	strcpysafe( out, outlen, Connect[ fd ].defaultkey );
	CONNECT_UNLOCK( fd );
}

ANYTHREAD void CONNECT_setDefaultKey( int sockfd, char *defaultkey )
{
	CONNECT_LOCK( sockfd );
	snprintf( Connect[ sockfd ].defaultkey, sizeof( Connect[ sockfd ].defaultkey ), "%s",
		defaultkey );
	CONNECT_UNLOCK( sockfd );
}

ANYTHREAD void CONNECT_getPasswd( int fd , char *out, int outlen )
{
	CONNECT_LOCK( fd );
	strcpysafe( out, outlen, Connect[ fd ].passwd );
	CONNECT_UNLOCK( fd );
}
ANYTHREAD void CONNECT_setPasswd( int fd, char *in )
{
	CONNECT_LOCK( fd );
	strcpysafe( Connect[ fd ].passwd, sizeof( Connect[ fd ].passwd ), in );
	CONNECT_UNLOCK( fd );
}
ANYTHREAD int CONNECT_getCtype( int fd )
{
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].ctype;
	CONNECT_UNLOCK( fd );
	return a;
}
ANYTHREAD void CONNECT_setCtype( int fd , int a )
{
	CONNECT_LOCK( fd );
	Connect[ fd ].ctype = a;
	CONNECT_UNLOCK( fd );
}

ANYTHREAD void CONNECT_getCharname( int fd , char *out, int outlen )
{
	CONNECT_LOCK( fd );
	strcpysafe( out, outlen, Connect[ fd ].charname );
	CONNECT_UNLOCK( fd );
}
ANYTHREAD void CONNECT_setCharname( int fd, char *in )
{
	CONNECT_LOCK( fd );
	strcpysafe( Connect[ fd ].charname, sizeof( Connect[ fd ].charname ),
		in );
	CONNECT_UNLOCK( fd );
}

ANYTHREAD int CONNECT_getFdid( int fd )
{
	if( fd < 0 ||  fd >= ConnectLen ){
		return -1;
	}
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].fdid;
	CONNECT_UNLOCK( fd );
	return a;
}
ANYTHREAD void CONNECT_setDuelcharaindex( int fd, int i , int a )
{
	CONNECT_LOCK( fd );
	Connect[ fd ].duelcharaindex[ i ] = a;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD int CONNECT_getDuelcharaindex( int fd, int i )
{
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].duelcharaindex[ i ];
	CONNECT_UNLOCK( fd );
	return a;
}
ANYTHREAD void CONNECT_setBattlecharaindex( int fd, int i , int a )
{
	CONNECT_LOCK( fd );
	Connect[ fd ].battlecharaindex[ i ] = a;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD int CONNECT_getBattlecharaindex( int fd, int i )
{
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].battlecharaindex[ i ];
	CONNECT_UNLOCK( fd );
	return a;
}
ANYTHREAD void CONNECT_setJoinpartycharaindex( int fd, int i , int a)
{
	CONNECT_LOCK( fd );
	Connect[ fd ].joinpartycharaindex[ i ] = a;
	CONNECT_UNLOCK( fd );
}
ANYTHREAD int CONNECT_getJoinpartycharaindex( int fd, int i )
{
	int a;
	CONNECT_LOCK( fd );
	a = Connect[ fd ].joinpartycharaindex[ i ];
	CONNECT_UNLOCK( fd );
	return a;
}

// CoolFish: Trade 2001/4/18
ANYTHREAD void CONNECT_setTradecharaindex( int fd, int i , int a )
{
	CONNECT_LOCK( fd );
	Connect[ fd ].tradecharaindex[ i ] = a;
	CONNECT_UNLOCK( fd );
}

// Shan Begin
ANYTHREAD void CONNECT_setLastrecvtime( int fd, struct timeval *a )
{
	CONNECT_LOCK(fd);
	Connect[fd].lastrecvtime = *a;
	CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getLastrecvtime( int fd, struct timeval *a )
{
	CONNECT_LOCK(fd);
	*a = Connect[fd].lastrecvtime;
	CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setLastrecvtime_D( int fd, struct timeval *a )
{
	CONNECT_LOCK(fd);
	Connect[fd].lastrecvtime_d = *a;
	CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getLastrecvtime_D( int fd, struct timeval *a )
{
	CONNECT_LOCK(fd);
	*a = Connect[fd].lastrecvtime_d;
	CONNECT_UNLOCK(fd);
}
// 2001/12/26
ANYTHREAD void CONNECT_SetBattleRecvTime( int fd, struct timeval *a )
{
	CONNECT_LOCK(fd);
	Connect[fd].battle_recvtime = *a;
	CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_GetBattleRecvTime( int fd, struct timeval *a )
{
	CONNECT_LOCK(fd);
	*a = Connect[fd].battle_recvtime;
	CONNECT_UNLOCK(fd);
}
// Shan End


#ifdef _ITEM_PILEFORTRADE
ANYTHREAD void CONNECT_setTradeList( int fd, int num)
{
	Connect[fd].tradelist = num;
}
ANYTHREAD int CONNECT_getTradeList(int fd)
{
	return Connect[fd].tradelist;
}
#endif

ANYTHREAD void CONNECT_setTradeTmp(int fd, char* a)
{
	CONNECT_LOCK(fd);
	strcpysafe( Connect[fd].TradeTmp, sizeof(Connect[fd].TradeTmp), a);

	CONNECT_UNLOCK(fd);
}
ANYTHREAD void CONNECT_getTradeTmp(int fd, char *trademsg, int trademsglen)
{
	CONNECT_LOCK(fd);
	strcpysafe(trademsg, trademsglen, Connect[fd].TradeTmp);
	CONNECT_UNLOCK(fd);
}

ANYTHREAD void CONNECT_setTradecardcharaindex( int fd, int i , int a )
{
	CONNECT_LOCK(fd);
	Connect[fd].joinpartycharaindex[i] = a;
	CONNECT_UNLOCK(fd);
}
ANYTHREAD int CONNECT_getTradecardcharaindex( int fd, int i )
{
	int a;
	CONNECT_LOCK(fd);
	a = Connect[fd].joinpartycharaindex[i];
	CONNECT_UNLOCK(fd);
	return a;
}

ANYTHREAD void CONNECT_setCloseRequest( int fd, int count)
{
	//		print("\n�ر���������Ϊ FILE:%s,LINE:%d ", file, line);
	CONNECT_LOCK(fd);
	Connect[fd].close_request = count;
	// Nuke
	//    print("\n�ر���������Ϊ %d ",fd);
	CONNECT_UNLOCK(fd);
}


/*------------------------------------------------------------
* CAcheck �ʤɤ˻Ȥ���ؿ����ºݤ����롣
* ����
*  fd      int     �ե�����ǥ�������ץ�
* �֤���
*  �ʤ�
------------------------------------------------------------*/
ANYTHREAD void CAsend( int fd )
{
	char buf[sizeof(Connect[0].CAbuf)];
	int bufuse=0;

	if( CONNECT_getCAbuf( fd, buf, sizeof(buf), &bufuse ) < 0 )return;
	if( bufuse == 0 )return;

	//print("\nshan--->(CAsend)->%s fd->%d", buf, fd);

	/*�Ǹ�Υǥ�ߥ� ',' ��'\0' �Ȥ�����*/
	buf[bufuse-1] = '\0';
	
	lssproto_CA_send( fd , buf );

	CONNECT_setCAbufsiz( fd, 0 );	
}


/*------------------------------------------------------------
* CA�����롣
* ����
* �֤���
*  �ʤ�
------------------------------------------------------------*/
ANYTHREAD void CAcheck( void )
{
	int     i;
#ifndef _WAN_FIX
	unsigned int interval_us = getCAsendinterval_ms()*1000;
#endif
	/* Connect�������з���֧�¾��պ����Ϸ�ئ�� */
	for( i = 0; i < ConnectLen; i ++) {
#ifndef _WAN_FIX
		struct timeval t;
#endif
		if( !CONNECT_getUse_debug(i,1008) )continue;
#ifndef _WAN_FIX
		CONNECT_getLastCAsendtime( i, &t );
		if( time_diff_us( NowTime, t ) > interval_us ){
#endif
			CAsend( i);
#ifndef _WAN_FIX
			CONNECT_setLastCAsendtime( i, &NowTime );
		}
#endif
	}
}
ANYTHREAD void CAflush( int charaindex )
{
	int i;
	i = getfdFromCharaIndex( charaindex);
	if( i == -1 )return;
	CAsend(i);
}


/*------------------------------------------------------------
* CDbuf ���ɲä��롣
* ����
*  fd      int     �ե�����ǥ�������ץ�
*  data    char*   �ǡ���
*  size    int     �ǡ����Υ�����
* �֤���
*  ����    TRUE(1)
*  ����    FALSE(0)
------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_appendCDbuf( int fd , char* data, int size )
{
	CONNECT_LOCK(fd);

	if( ( Connect[fd].CDbufsiz + size ) >= sizeof( Connect[fd].CDbuf )){
		CONNECT_UNLOCK(fd);
		return FALSE;
	}
	memcpy( Connect[fd].CDbuf + Connect[fd].CDbufsiz , data, size );
	Connect[fd].CDbuf[Connect[fd].CDbufsiz+size] = ',';
	Connect[fd].CDbufsiz += ( size + 1 );
	CONNECT_UNLOCK(fd);
	return TRUE;
}


/*------------------------------------------------------------
* CDcheck �ʤɤ˻Ȥ���ؿ����ºݤ����롣
* ����
*  fd      int     �ե�����ǥ�������ץ�
* �֤���
*  �ʤ�
------------------------------------------------------------*/
ANYTHREAD void CDsend( int fd )
{
	char buf[sizeof(Connect[0].CAbuf )];
	int bufuse=0;
	if( CONNECT_getCDbuf( fd, buf, sizeof(buf), &bufuse ) < 0 ) return;
	if( bufuse == 0 ) return;
	buf[bufuse-1] = '\0';
	lssproto_CD_send(fd, buf );
	CONNECT_setCDbufsiz(fd,0);
}


/*------------------------------------------------------------
* CD�����롣
* ����
* �֤���
*  �ʤ�
------------------------------------------------------------*/
ANYTHREAD void CDcheck( void )
{
	int     i;
#ifndef _WAN_FIX
	unsigned int interval_us = getCDsendinterval_ms()*1000;
#endif
	for(i=0; i<ConnectLen; i++ ){
#ifndef _WAN_FIX
		struct timeval t;
#endif
		if( !CONNECT_getUse_debug(i,1082) ) continue;
#ifndef _WAN_FIX
		CONNECT_getLastCDsendtime( i, &t );
		if( time_diff_us( NowTime, t ) > interval_us ){
#endif
			CDsend( i);
#ifndef _WAN_FIX
			CONNECT_setLastCDsendtime( i , &NowTime );
		}
#endif
	}
}

ANYTHREAD void CDflush( int charaindex )
{
	int i;
	i = getfdFromCharaIndex( charaindex);
	if( i == -1 )return;
	CDsend(i);
}

void chardatasavecheck( void )
{
    int i;
    int interval = getCharSavesendinterval();
    static struct timeval chardatasavecheck_store;
    if( NowTime.tv_sec > (chardatasavecheck_store.tv_sec +10)){
        chardatasavecheck_store = NowTime;
        
        
#ifdef _FIX_SAVE_CHAR
        for(i=0;i<CHAR_getPlayerMaxNum();i++){
					if(CHAR_CHECKINDEX(i) == FALSE) continue;
					int tv_sec = NowTime.tv_sec;
					if(CHAR_getWorkInt( i, CHAR_WORK_SAVE_CHAR) == 0){
						CHAR_setWorkInt( i, CHAR_WORK_SAVE_CHAR, tv_sec);
					}else{
						if(tv_sec - CHAR_getWorkInt( i, CHAR_WORK_SAVE_CHAR) > interval){
							CHAR_charSaveFromConnect( i, FALSE );
							CHAR_setWorkInt( i, CHAR_WORK_SAVE_CHAR, tv_sec);
						}
					}
				}
#else
        for( i = 0; i < ConnectLen; i ++) {
            CONNECT_LOCK(i);
            if( Connect[i].use == TRUE 
                && Connect[i].state == LOGIN 
                && NowTime.tv_sec - Connect[i].lastCharSaveTime.tv_sec 
                > interval ){
                Connect[i].lastCharSaveTime = NowTime;
                CONNECT_UNLOCK(i);
                CHAR_charSaveFromConnect( CONNECT_getCharaindex( i ), FALSE );
            } else {
                CONNECT_UNLOCK(i);
            }
        }
#endif
    } else {
        ;
    }
}

/*------------------------------------------------------------
* fd �� valid ئ�ּ���������ëƩ����
* ¦��
*  fd          int         fd
* ߯Ի��
*  valid   TRUE(1)
*  invalid FALSE(0)
------------------------------------------------------------*/
ANYTHREAD INLINE int CONNECT_checkfd( int fd )
{
	if( 0 > fd ||  fd >= ConnectLen ){
		return FALSE;
	}
	CONNECT_LOCK(fd);
	if( Connect[fd].use == FALSE ){
		CONNECT_UNLOCK(fd);
		return FALSE;
	} else {
		CONNECT_UNLOCK(fd);
		return TRUE;
	}
}


/*------------------------------------------------------------
* cdkey ���� fd �����롣
* ����
*  cd      char*       cdkey
* �֤���
*  �ե�����ǥ�������ץ�  ��äơ� -1 �λ��ϥ��顼
------------------------------------------------------------*/
ANYTHREAD int getfdFromCdkey( char* cd )
{
	int i;
	for( i = 0 ;i < ConnectLen ; i ++ ){
		CONNECT_LOCK(i);
		if( Connect[i].use == TRUE &&
			strcmp( Connect[i].cdkey , cd ) == 0 ){
				CONNECT_UNLOCK(i);
				return i;
		}
		CONNECT_UNLOCK(i);
	}
	return -1;
}


/*------------------------------------------------------------
* charaindex ���� fd ë  �£�
*   ½��  ����������ë�������ף�
* ¦��
*  charaindex      int     ƽ�ҷ¼��̼������͵�
* ߯Ի��
*  �����̻ﷸū����������  ���Ȼ��� -1 ���ݷ��޷¡�
------------------------------------------------------------*/
ANYTHREAD int getfdFromCharaIndex( int charaindex )
{
#if 1
	int ret;
	if( !CHAR_CHECKINDEX( charaindex)) return -1;
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) return -1;
	ret = CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	if( ret < 0 || ret >= ConnectLen ) return -1;
	return ret;
#else
	int i;
	for( i = 0 ;i < ConnectLen ; i ++ ){
		CONNECT_LOCK(i);
		if( Connect[i].use == TRUE
			&& Connect[i].charaindex == charaindex ){
				CONNECT_UNLOCK(i);
				return i;
		}
		CONNECT_UNLOCK(i);
	}
	return -1;
#endif
}
/*------------------------------------------------------------
* charaindex ���� cdkey ë  �£�
* ¦��
*  charaindex  int     ƽ�ҷ¼��̼������͵�
* ߯Ի��
*  0ئ����  ��  ئ����  
------------------------------------------------------------*/
ANYTHREAD int getcdkeyFromCharaIndex( int charaindex , char *out, int outlen )
{
	int i;

	for( i = 0 ;i < ConnectLen ; i ++ ){
		CONNECT_LOCK(i);
		if( Connect[i].use == TRUE
			&& Connect[i].charaindex == charaindex ){
				snprintf( out, outlen, "%s" , Connect[i].cdkey );
				CONNECT_UNLOCK(i);
				return 0;
		}
		CONNECT_UNLOCK(i);
	}
	return -1;
}


/*------------------------------------------------------------
* Ʊ��fdid ���ۤ�õ��
* ����
*  fdid    int     fd��id
* �֤���
*  -1 ���ݷ��޷¡�
------------------------------------------------------------*/
ANYTHREAD int getfdFromFdid( int fdid )
{
	int i;

	for( i=0; i<ConnectLen ; i ++ ){
		CONNECT_LOCK(i);
		if( Connect[i].use == TRUE
			&& Connect[i].fdid == fdid ){
				CONNECT_UNLOCK(i);
				return i;
		}
		CONNECT_UNLOCK(i);
	}
	return -1;
}

/*------------------------------------------------------------
* fdid ���饭����index ���Ȥ�롣
* ����
*  fdid    int     fd��id
* �֤���
*  -1 �λ��ϥ�������Υ����ϤߤĤ���ʤ��ä���0�ʾ�ʤ�
* ��������Υ����Υ����Ϥ���Ĥؤ� index
------------------------------------------------------------*/
ANYTHREAD int getCharindexFromFdid( int fdid )
{
	int i;

	for( i=0; i<ConnectLen ; i ++ ){
		CONNECT_LOCK(i);
		if( Connect[i].use == TRUE
			&& Connect[i].fdid == fdid &&
			Connect[i].charaindex >= 0 ){
				int a = Connect[i].charaindex;
				CONNECT_UNLOCK(i);
				return a;
		}
		CONNECT_UNLOCK(i);
	}

	return -1;
}
/*------------------------------------------------------------
* �����index ���� fdid ���Ȥ�롣
* ����
*  charind  int     �ե�����ǥ�������ץ�
* ������� fdid  ����ä��饭���ind����������
------------------------------------------------------------*/
ANYTHREAD int getFdidFromCharaIndex( int charind )
{
	int i;

	for( i=0; i<ConnectLen ; i ++ ){
		CONNECT_LOCK(i);
		if( Connect[i].use == TRUE
			&& Connect[i].charaindex == charind ){
				int a = Connect[i].fdid;
				CONNECT_UNLOCK(i);
				return a;
		}
		CONNECT_UNLOCK(i);
	}

	return -1;
}


/*------------------------------------------------------------
* fd�˳�����Ƥ�줿��³�����饤����Ȥ��ȡ�BOOL���֤�
* �٤����顼�����å��Ϥ��ʤ���
* ����
*  fd  int     �ե�����ǥ�������ץ�
------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isCLI( int fd )
{
	int a;
	CONNECT_LOCK(fd);
	a = ( Connect[fd].ctype == CLI ? TRUE : FALSE  );
	CONNECT_UNLOCK(fd);
	return a;
}


/*------------------------------------------------------------
* fd�˳�����Ƥ�줿��³����������ȥ����Ф��ȡ�BOOL���֤�
* �٤����顼�����å��Ϥ��ʤ���
* ����
*  fd  int     �ե�����ǥ�������ץ�
------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isAC( int fd )
{
	int a;
	CONNECT_LOCK(fd);
	a = ( Connect[fd].ctype == AC ? TRUE : FALSE  );
	CONNECT_UNLOCK(fd);
	return a;
}

/*------------------------------------------------------------
* fd�˳�����Ƥ�줿��³����������֤Ǥ��뤫�ɤ���
* ���֤�
* ����
*  fd  int     �ե�����ǥ�������ץ�
------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isUnderLogin( int fd )
{
	int a;
	CONNECT_LOCK(fd);
	a = ( Connect[fd].state == LOGIN ? TRUE : FALSE  );
	CONNECT_UNLOCK(fd);
	return a;
}

/*------------------------------------------------------------
* Login�����椫�ɤ���Ĵ�٤�
* ����
*  fd  int     �ե�����ǥ�������ץ�
------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isWhileLogin( int fd )
{
	int a;
	CONNECT_LOCK(fd);
	a = ( Connect[fd].state == WHILELOGIN ? TRUE : FALSE  );
	CONNECT_UNLOCK(fd);
	return a;
}

/*------------------------------------------------------------
* �����󤷤Ƥ��ʤ����֤�
* �ɤ������֤�
* ����
*  fd  int     �ե�����ǥ�������ץ�
------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isNOTLOGIN( int fd )
{
	int a;
	CONNECT_LOCK(fd);
	a = ( Connect[fd].state == NOTLOGIN ? TRUE : FALSE  );
	CONNECT_UNLOCK(fd);
	return a;
}

/*------------------------------------------------------------
* �����󤷤Ƥ�����֤�
* �ɤ������֤�
* ����
*  fd  int     �ե�����ǥ�������ץ�
------------------------------------------------------------*/
ANYTHREAD BOOL CONNECT_isLOGIN( int fd )
{
	int a;
	CONNECT_LOCK(fd);
	a = ( Connect[fd].state == LOGIN ? TRUE : FALSE  );
	CONNECT_UNLOCK(fd);
	return a;
}



/*------------------------------------------------------------
* ��������³���ڤäơ���������ȥ����Ф˥ǡ�������¸���褦�Ȥ��롣
* �������֤���
*  �ʤ�
------------------------------------------------------------*/
void closeAllConnectionandSaveData( void )
{
	int     i;
	int		num;

	/*  ����ʾ� accept ���ʤ��褦�ˤ���    */
	SERVSTATE_setCloseallsocketnum(0);

	/*  �����������    */
	for( i = 0 ; i<ConnectLen ; i++ ){
		if( CONNECT_getUse_debug(i,1413) == TRUE ){
			BOOL    clilogin=FALSE;
			if( CONNECT_isAC( i ) )continue;
			if( CONNECT_isCLI( i ) && CONNECT_isLOGIN( i ) )clilogin = TRUE;
			//print("����444444");
			CONNECT_endOne_debug(i);
			// Nuke +1 0901: Why close
			//print("closed in closeAllConnectionandSaveData");

			if( clilogin ){
				CONNECT_setUse(i,TRUE);
				CONNECT_setState(i,WHILECLOSEALLSOCKETSSAVE );
				SERVSTATE_incrementCloseallsocketnum();
			}
		}
	}
	num = SERVSTATE_getCloseallsocketnum();
	if( num == 0 ) {
		SERVSTATE_SetAcceptMore( -1 );
	}else {
		SERVSTATE_SetAcceptMore( 0 );
	}
	print( "\n��������������Ŀ:%d\n", num );
}
//andy_add 2003/02/12
void CONNECT_SysEvent_Loop( void)
{
	static time_t checkT=0;
	static int chikulatime = 0;
#ifdef _PETSKILL_BECOMEPIG
	static int chikulatime2 = 0;
	static time_t checkT2=0;
#endif
	int NowTimes = time(NULL);

	if( checkT != NowTimes && (checkT+10) <= NowTimes )	{
		int i;
		checkT = time(NULL);
		chikulatime++;//ÿ10��
		if( chikulatime > 10000 ) chikulatime = 0;
		if( getReloadNpcTime() > 0){
					if ( chikulatime % getReloadNpcTime() == 0 ) {
						NPC_reloadNPC();
						MAPPOINT_resetMapWarpPoint( 1);
						MAPPOINT_loadMapWarpPoint();
					}
				}
		
		if( chikulatime % 60 == 0 ){ //ÿ600���ض���Ծ��
			MAPPOINT_resetMapWarpPoint( 1);
			MAPPOINT_loadMapWarpPoint();
		}
				
		int playernum = CHAR_getPlayerMaxNum();
		int charaindex;
		for ( charaindex = 0;charaindex < playernum; charaindex++ ) {
			if( !CHAR_CHECKINDEX(charaindex) )continue;
        if ( chikulatime % 6 == 0 ) { // ÿ60��
          // shan 2001/12/27 Begin

#ifdef _PETSKILL_BECOMEPIG
        if ( CHAR_CHECKINDEX( charaindex ) )
          if ( CHAR_getInt( charaindex, CHAR_BECOMEPIG ) > -1 ) { //���������״̬

            if ( ( CHAR_getInt( charaindex, CHAR_BECOMEPIG ) - 1 ) <= 0 ) { //����ʱ�������
              CHAR_setInt( charaindex, CHAR_BECOMEPIG, 0 );

              if ( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ) { //������ս��״̬��
                CHAR_setInt( charaindex, CHAR_BECOMEPIG, -1 ); //��������״̬
                CHAR_complianceParameter( charaindex );
                CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ) );
                CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER );
                CHAR_talkToCli( charaindex, -1, "������ʧЧ�ˡ�", CHAR_COLORWHITE );
              }
            }
            else {
              CHAR_setInt( charaindex, CHAR_BECOMEPIG, max(0,CHAR_getInt( charaindex, CHAR_BECOMEPIG ) - 60) );
            }
          }
#endif
        }

				if ( chikulatime % 30 == 0 ) { // ÿ300��
#ifdef _ITEM_ADDEXP	//vincent ��������
					if ( CHAR_getInt( charaindex, CHAR_ADDEXPPOWER ) > 0 &&
						CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ) {
							int exptime;
							exptime = CHAR_getInt( charaindex, CHAR_ADDEXPTIME ) - 300;

							if ( exptime <= 0 ) {
								CHAR_setInt( charaindex, CHAR_ADDEXPPOWER, 0 );
								CHAR_setInt( charaindex, CHAR_ADDEXPTIME, 0 );
								CHAR_talkToCli( charaindex,-1,"����ѧϰ�����������ʧ��!",CHAR_COLORYELLOW);
							}
							else {
								CHAR_setInt( charaindex, CHAR_ADDEXPTIME, exptime );
								//print("\n ���ADDEXPTIME %d ", exptime);

								if ( ( exptime % ( 60 * 60 ) ) < 300 && exptime >= ( 60 * 60 ) ) {
									char msg[ 1024 ];
									sprintf( msg, "����ѧϰ���������ʣ��Լ %d Сʱ��", (int)(exptime/(60*60)) );
									//sprintf( msg, "����ѧϰ���������ʣ��Լ %d �֡�", (int)(exptime/(60)) );
									CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW );
								}
							}

					}
					
				if ( CHAR_getInt( charaindex, CHAR_VIPRIDE ) > 0 && CHAR_getInt( charaindex, CHAR_VIPTIME ) > 0 ){
					time_t timep;
					struct tm *p;
					time(&timep);
					p=localtime(&timep);
					timep = mktime(p);
					if(CHAR_getInt( charaindex, CHAR_VIPTIME )<timep){
						CHAR_setInt(charaindex, CHAR_VIPRIDE,0);
						CHAR_setInt(charaindex, CHAR_VIPTIME,0);
						CHAR_loginCheckUserItem(charaindex);
					}
				}

#endif
#ifdef _VIP_BATTLE_EXP
					if ( CHAR_getInt( charaindex, CHAR_NVIPEXP ) > 0 &&
						CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER ) {
							int nviptime;
							nviptime = CHAR_getInt( charaindex, CHAR_NVIPTIME ) - 300;

							if ( nviptime <= 0 ) {
								CHAR_setInt( charaindex, CHAR_NVIPEXP, 0 );
								CHAR_setInt( charaindex, CHAR_NVIPTIME, 0 );
								CHAR_talkToCli( charaindex,-1,"���⾭��ʱ���ѵ��ڡ�",CHAR_COLORYELLOW);
							}
							else {
								CHAR_setInt( charaindex, CHAR_NVIPTIME, nviptime );
								if ( ( nviptime % ( 60 * 60 ) ) < 300 && nviptime >= ( 60 * 60 ) ) {
									char token[ 1024 ];
									sprintf( token, "���⾭��ʱ��ʣ�� %d Сʱ��", (int)(nviptime/(60*60)) );
									CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW );
								}
							}
					}
#endif
#ifdef	_OFFLINE_SYSTEM
				if( getOfflineCf()==1 && CHAR_getWorkInt(charaindex,CHAR_WORK_OFFLINE) !=0 && CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR)!=1 && CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR)!=2){
					if(CHAR_getInt(charaindex,CHAR_OFFTIME)>0){
						int offtime = CHAR_getInt(charaindex,CHAR_OFFTIME) - 300;
						if(offtime<=0){
							CHAR_setInt(charaindex,CHAR_OFFTIME,0);
							CHAR_logout(charaindex,TRUE);
							sasql_online(CHAR_getChar(charaindex,CHAR_CDKEY),NULL,NULL,NULL,NULL,0);
						}else{
							CHAR_setInt(charaindex,CHAR_OFFTIME,offtime);
						}
					}else{
						CHAR_logout(charaindex,TRUE);
						sasql_online(CHAR_getChar(charaindex,CHAR_CDKEY),NULL,NULL,NULL,NULL,0);
					}
				}
#endif
#ifdef _ITEM_METAMO
					if ( CHAR_getWorkInt( charaindex, CHAR_WORKITEMMETAMO ) < NowTime.tv_sec
						&& CHAR_getWorkInt( charaindex, CHAR_WORKITEMMETAMO ) != 0 ) {
							CHAR_setWorkInt( charaindex, CHAR_WORKITEMMETAMO, 0 );
							CHAR_setWorkInt( charaindex, CHAR_WORKNPCMETAMO, 0 ); //��npc�Ի���ı���ҲҪ�����
							CHAR_complianceParameter( charaindex );
							CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ) );
							CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER );
							CHAR_talkToCli( charaindex, -1, "����ʧЧ�ˡ�", CHAR_COLORWHITE );
					}

#endif
#ifdef _ITEM_TIME_LIMIT
					ITEM_TimeLimit( charaindex ); // (�ɿ���) shan time limit of item. code:shan

#endif

				} //%30
        
#ifdef _PETSKILL_BECOMEPIG
        if ( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ) { //������ս��״̬��

          if ( CHAR_getInt( charaindex, CHAR_BECOMEPIG ) > -1 ) { //���������״̬
            char temp[ 256 ];
            sprintf( temp, "����ʱ��:%d��", CHAR_getInt( charaindex, CHAR_BECOMEPIG ) );
            CHAR_talkToCli( charaindex, -1, temp, CHAR_COLORWHITE );
          }
        }

#endif
        //10��
#ifdef _MAP_TIME
        if ( CHAR_getWorkInt( charaindex, CHAR_WORK_MAP_TIME ) > 0
             && CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ) {
          CHAR_setWorkInt( charaindex, CHAR_WORK_MAP_TIME, CHAR_getWorkInt( charaindex, CHAR_WORK_MAP_TIME ) - 10 );

          if ( CHAR_getWorkInt( charaindex, CHAR_WORK_MAP_TIME ) <= 0 ) {
            // ʱ�䵽��,�������
            CHAR_talkToCli( charaindex, -1, "����Ϊ�ܲ��˸��ȶ������������ѷ���ڡ�", CHAR_COLORRED );
            CHAR_warpToSpecificPoint( charaindex, 30008, 39, 38 );
            CHAR_setInt( charaindex, CHAR_HP, 1 );
            CHAR_AddCharm( charaindex, -3 );
            CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_HP );
            CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_CHARM );
          }
          else {
            char szMsg[ 64 ];
            sprintf( szMsg, "������ȵĻ�������ֻ���ٴ� %d �롣", CHAR_getWorkInt( charaindex, CHAR_WORK_MAP_TIME ) );
            CHAR_talkToCli( charaindex, -1, szMsg, CHAR_COLORRED );
          }
        }
#endif
		}
		for ( i = 0;i < ConnectLen; i++ ) {
			if ( ( Connect[ i ].use ) && ( i != acfd )) {
				

				if ( chikulatime % 6 == 0 ) { // ÿ60��
					// shan 2001/12/27 Begin

					if ( CHAR_getWorkInt( Connect[ i ].charaindex, CHAR_WORKBATTLEMODE )
						!= BATTLE_CHARMODE_NONE ) {

							struct timeval recvtime;
							CONNECT_GetBattleRecvTime( i, &recvtime );

							if ( time_diff( NowTime, recvtime ) > 360 ) {
								//print("����55555");
								CONNECT_endOne_debug( i );
							}
					}

					// End
				} //%30
				
#ifdef	_OFFLINE_SYSTEM
				if( chikulatime % 60 == 0 ){//ÿ600��
					if ( CHAR_getWorkInt( Connect[ i ].charaindex, CHAR_WORK_OFFLINE )==0){
						if( strlen(CHAR_getChar(Connect[ i ].charaindex,CHAR_OFFBUF))<1){
							time_t timep;
							struct tm *p;
							time(&timep);
							p=localtime(&timep); /*ȡ�õ���ʱ��*/
							int mon=1+p->tm_mon;
							int date=p->tm_mday;
							char offbuf[64];
							sprintf(offbuf,"%d-%d-%d",mon,date,0);
							if(timep-Connect[ i ].starttime >= 600 && CHAR_getInt(Connect[ i ].charaindex,CHAR_OFFTIME)<86400){
								sprintf(offbuf,"%d-%d-%d",mon,date,1);
								Connect[ i ].starttime = timep;
								CHAR_setInt(Connect[ i ].charaindex,CHAR_OFFTIME,CHAR_getInt(Connect[ i ].charaindex,CHAR_OFFTIME)+600);
							}
							CHAR_setChar(Connect[ i ].charaindex,CHAR_OFFBUF,offbuf);
						}else{
							char offbuf[64];
							strcpy(offbuf,CHAR_getChar(Connect[ i ].charaindex,CHAR_OFFBUF));
							char offtmp[10];
							int mymon,mydate,mycnt;
							getStringFromIndexWithDelim(offbuf,"-", 1,offtmp,sizeof(offtmp));
							mymon = atoi(offtmp);
							getStringFromIndexWithDelim(offbuf,"-", 2,offtmp,sizeof(offtmp));
							mydate = atoi(offtmp);
							getStringFromIndexWithDelim(offbuf,"-", 3,offtmp,sizeof(offtmp));
							mycnt = atoi(offtmp);
							time_t timep;
							struct tm *p;
							time(&timep);
							p=localtime(&timep); /*ȡ�õ���ʱ��*/
							int mon=1+p->tm_mon;
							int date=p->tm_mday;
							if(mymon!=mon || mydate!=date){
								sprintf(offbuf,"%d-%d-%d",mon,date,0);
								if(timep-Connect[ i ].starttime >= 600 && CHAR_getInt(Connect[ i ].charaindex,CHAR_OFFTIME)<432000){
									sprintf(offbuf,"%d-%d-%d",mon,date,1);
									Connect[ i ].starttime = timep;
									CHAR_setInt(Connect[ i ].charaindex,CHAR_OFFTIME,CHAR_getInt(Connect[ i ].charaindex,CHAR_OFFTIME)+600);
								}
								CHAR_setChar(Connect[ i ].charaindex,CHAR_OFFBUF,offbuf);
							}else{
								if(mycnt<24 && timep-Connect[ i ].starttime >= 600 && CHAR_getInt(Connect[ i ].charaindex,CHAR_OFFTIME)<432000){
									sprintf(offbuf,"%d-%d-%d",mon,date,mycnt+1);
									Connect[ i ].starttime = timep;
									CHAR_setInt(Connect[ i ].charaindex,CHAR_OFFTIME,CHAR_getInt(Connect[ i ].charaindex,CHAR_OFFTIME)+600);
									CHAR_setChar(Connect[ i ].charaindex,CHAR_OFFBUF,offbuf);
								}
							}
						}
					}
				}
#endif

#ifndef _USER_CHARLOOPS
				//here ԭ������
				if ( Connect[ i ].stayencount ) {
					if ( Connect[ i ].BDTime < time( NULL ) ) {
						if ( CHAR_getWorkInt( Connect[ i ].charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ) {
							lssproto_EN_recv( i, CHAR_getInt( Connect[ i ].charaindex, CHAR_X ),
								CHAR_getInt( Connect[ i ].charaindex, CHAR_Y ) );
							Connect[ i ].BDTime = time( NULL );
						}
					}
				}

#endif
#ifdef _CHIKULA_STONE
				if ( chikulatime % 3 == 0 && getChiStone( i ) > 0 ) { //�Զ���Ѫ
					CHAR_AutoChikulaStone( Connect[ i ].charaindex, getChiStone( i ) );
				}

#endif

				if ( chikulatime % 6 == 0 ) { //ˮ����״̬
#ifdef _STATUS_WATERWORD
					CHAR_CheckWaterStatus( Connect[ i ].charaindex );
#endif
					// Nuke 0626: No enemy

					if ( Connect[ i ].noenemy > 0 ) {
						Connect[ i ].noenemy--;

						if ( Connect[ i ].noenemy == 0 ) {
							CHAR_talkToCli( CONNECT_getCharaindex( i ), -1, "�ػ���ʧ�ˡ�", CHAR_COLORWHITE );
						}
					}
				}

				//ÿ10��
#ifdef _TYPE_TOXICATION //�ж�
				if ( Connect[ i ].toxication > 0 ) {
					CHAR_ComToxicationHp( Connect[ i ].charaindex );
				}

#endif
				// Nuke 0624 Avoid Useless Connection
				if ( Connect[ i ].state == NOTLOGIN ) {
					Connect[ i ].cotime++;
					if ( Connect[ i ].cotime > 30 ) {
						//print( "LATE" );
						CONNECT_endOne_debug( i );
					}
				}else{
					Connect[ i ].cotime = 0;
				}

				if ( ( Connect[ i ].nu <= 22 ) ) {
					int r;

					if ( Connect[ i ].nu <= 0 ) {
						Connect[ i ].nu_decrease++;

						if ( Connect[ i ].nu_decrease >= 30 )
							Connect[ i ].nu_decrease = 30;

						if ( Connect[ i ].nu_decrease > 22 ) logSpeed( i );
					}
					else {
						Connect[ i ].nu_decrease -= 1;

						if ( Connect[ i ].nu_decrease < 0 )
							Connect[ i ].nu_decrease = 0;
					}

					r = 22 - Connect[ i ].nu_decrease;
					r = ( r >= 15 ) ? r : 15;
					lssproto_NU_send( i, r );
					Connect[ i ].nu += r;
					//print("\nr=%d,nu=%d\n",r,Connect[ i ].nu);
				}
				// Nuke 1213: Flow control 2
				Connect[ i ].packetin--; // Remove a counter

				if ( Connect[ i ].packetin <= 0 ) { // Time out, drop this line
					//print( "Drop line: sd=%d\n", i );
					CONNECT_endOne_debug( i );
				}

				//10��

			}
		}
	}

//#ifdef _PETSKILL_BECOMEPIG
//	if ( checkT2 != NowTimes && ( checkT2 ) <= NowTimes ) {
//		int i;
//		checkT2 = time( NULL );
//		++chikulatime2; //ÿ1��
//
//		if ( chikulatime2 > 1000 ) chikulatime2 = 0;
//
//		for ( i = 0;i < ConnectLen; i++ ) {
//			if ( ( Connect[ i ].use ) && ( i != acfd )) {
//				//������
//
//				if ( CHAR_CHECKINDEX( Connect[ i ].charaindex ) )
//					if ( CHAR_getInt( Connect[ i ].charaindex, CHAR_BECOMEPIG ) > -1 ) { //���������״̬
//
//						if ( ( CHAR_getInt( Connect[ i ].charaindex, CHAR_BECOMEPIG ) - 1 ) <= 0 ) { //����ʱ�������
//							CHAR_setInt( Connect[ i ].charaindex, CHAR_BECOMEPIG, 0 );
//
//							if ( CHAR_getWorkInt( Connect[ i ].charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ) { //������ս��״̬��
//								CHAR_setInt( Connect[ i ].charaindex, CHAR_BECOMEPIG, -1 ); //��������״̬
//								CHAR_complianceParameter( Connect[ i ].charaindex );
//								CHAR_sendCToArroundCharacter( CHAR_getWorkInt( Connect[ i ].charaindex , CHAR_WORKOBJINDEX ) );
//								CHAR_send_P_StatusString( Connect[ i ].charaindex , CHAR_P_STRING_BASEBASEIMAGENUMBER );
//								CHAR_talkToCli( Connect[ i ].charaindex, -1, "������ʧЧ�ˡ�", CHAR_COLORWHITE );
//							}
//						}
//						else {
//							CHAR_setInt( Connect[ i ].charaindex, CHAR_BECOMEPIG, CHAR_getInt( Connect[ i ].charaindex, CHAR_BECOMEPIG ) - 1 );
//						}
//					}
//			}
//		}
//	}
//#endif

}

//int isThereInvalidIP(unsigned long ip, unsigned long* userip, unsigned long useripcount)
//{
//	unsigned long i = 0;
//	for(; i < useripcount; ++i )
//	{
//		if( userip[i] == ip && userip_count_data[i] >= getAttCnt())
//			return 1;
//	}
//
//	return 0;
//}

// Nuke 0126: Resource protection
int isThereThisIP(unsigned long ip)
{
	unsigned long ipip = ip;
	char ipstr[32];
	int astr,bstr,cstr,dstr;
	astr=(ipip % 0x100); ipip=ipip / 0x100;
	bstr=(ipip % 0x100); ipip=ipip / 0x100;
	cstr=(ipip % 0x100); ipip=ipip / 0x100;
	dstr=(ipip % 0x100);
	sprintf(ipstr,"%d.%d.%d.%d",astr,bstr,cstr,dstr);
	if(strcmp(ipstr,getNoAttIp(0))==0 || strcmp(ipstr,getNoAttIp(1))==0 || strcmp(ipstr,getNoAttIp(2))==0 || strcmp(ipstr,getNoAttIp(3))==0 || strcmp(ipstr,getNoAttIp(4))==0)
		return 0;
	int i,cmpnum = 0,j;
	j = getSameIpLogin();
	if(j <= 0)return 0;
	unsigned long ipa;

	for ( i = 0; i < ConnectLen ; i++ )
	{
		if ( !Connect[ i ].use ) continue;

		//if ( Connect[ i ].state == NOTLOGIN || Connect[ i ].state == WHILEDOWNLOADCHARLIST ) {
		memcpy( &ipa, &Connect[ i ].sin.sin_addr, 4 );
		if (ipa==ip && Connect[ i ].state == NOTLOGIN ) return 1;
		//if(getLoginJqmType()==1) return 0; //�����������IP������Ч
		if (ipa==ip) cmpnum++;
		 
		if (cmpnum >= j){
			//print("���Ƶ�½IP��:%d %d",cmpnum,j);
			return 1;
		}
	}

	return 0;
}

int isManRen(void)
{
	int i;
	int maxnum = CHAR_getPlayerMaxNum()-10;
	for(i=0;i<maxnum;i++){
		if( !CHAR_CHECKINDEX( i) ) return 0;
	}
	return 1;
}

int isSameThisIP(unsigned long ip,int ipcnt)
{
	int i,cmpnum = 0,j;
	if(ipcnt <= 0)return 0;
	unsigned long ipa;

	for ( i = 0; i < ConnectLen ; i++ )
	{
		if ( !Connect[ i ].use ) continue;

		//if ( Connect[ i ].state == NOTLOGIN || Connect[ i ].state == WHILEDOWNLOADCHARLIST ) {
		memcpy( &ipa, &Connect[ i ].sin.sin_addr, 4 );

		if (ipa==ip) cmpnum++;
		if (cmpnum >= ipcnt){
			//print("���Ƶ�½IP��:%d %d",cmpnum,j);
			return 1;
		}
	}

	return 0;
}

int player_online = 0;
int player_maxonline = 0;

SINGLETHREAD BOOL netloop_faster( void )
{
	//DebugMainFunction="netloop_faster1";
	int ret , loop_num;
	struct timeval tmv;    /*timeval*/
	static int fdremember = 0;

	static unsigned int total_item_use=0;
	static int petcnt=0;
	//    static unsigned int nu_time=0;
	struct timeval st, et;
	unsigned int casend_interval_us , cdsend_interval_us;
	int acceptmore = SERVSTATE_getAcceptmore();
	fd_set rfds, wfds , efds;  
	unsigned int looptime_us;
	int allowerrornum = getAllowerrornum();
	int acwritesize = getAcwriteSize();
#ifdef _AC_PIORITY
	static int flag_ac=1;
	static int fdremembercopy=0;
	static int totalloop=0;
	static int totalfd=0;
	static int totalacfd=0;
	static int counter=0;
#endif
	/*
	{
	int errorcode;
	int errorcodelen;
	int qs;

	errorcodelen = sizeof(errorcode);
	qs = getsockopt( acfd, SOL_SOCKET, SO_RCVBUF , &errorcode, &errorcodelen);
	//andy_log
	print("\n\n GETSOCKOPT SO_RCVBUF: [ %d, %d, %d] \n", qs, errorcode, errorcodelen);
	}
	*/
	//DebugMainFunction="netloop_faster2";
	looptime_us = getOnelooptime_ms()*1000 ;
	casend_interval_us = getCAsendinterval_ms()*1000;
	cdsend_interval_us = getCDsendinterval_ms()*1000;
	//DebugMainFunction="netloop_faster3";
	FD_ZERO( & rfds );
	FD_ZERO( & wfds );
	FD_ZERO( & efds );
	FD_SET( bindedfd , & rfds );
	FD_SET( bindedfd , & wfds );
	FD_SET( bindedfd , & efds );
	tmv.tv_sec = tmv.tv_usec = 0;
	ret = select( bindedfd + 1 , &rfds,&wfds,&efds,&tmv );
	if( ret < 0 && ( errno != EINTR )){
		;
	}
	//DebugMainFunction="netloop_faster4";
	if( ret > 0 && FD_ISSET(bindedfd , &rfds ) ){
		struct sockaddr_in sin;
		int addrlen=sizeof( struct sockaddr_in );
		int sockfd;
		//    int flags=fcntl(bindedfd,F_GETFL,0);//ͨ�� F_GETFL ��ȡ�ļ�������־��flags
		//		fcntl(bindedfd,F_SETFL,flags|O_NONBLOCK|O_NDELAY);//�˴�ֻ�ǰ��������Ĵ�λ��ΪO_NONBLOCK
		sockfd = accept( bindedfd ,(struct sockaddr*) &sin  , &addrlen );
		if( sockfd == -1 && errno == EINTR ){
			print( "accept err:%s\n", strerror(errno));;
		}
#ifdef _SOCKET_NONBLOCK
		else if (sockfd != -1 && fcntl (sockfd, F_SETFL, O_NONBLOCK) < 0) {
			fprintf (stderr, "Set sockfd Nonblock error.\n");
			close (sockfd);
		}
#endif
		else if( sockfd != -1 )
		{
			unsigned long sinip;

			memcpy( &sinip, &sin.sin_addr, 4);
//			if( isThereInvalidIP( sinip, userip, useripcount ) )
//			{
//				// �����ˣ����°�������
//				int ipa,ipb,ipc,ipd;
//				char ip[32];
//				ipa=(sinip % 0x100); sinip=sinip / 0x100;
//				ipb=(sinip % 0x100); sinip=sinip / 0x100;
//				ipc=(sinip % 0x100); sinip=sinip / 0x100;
//				ipd=(sinip % 0x100);
//				sprintf(ip,"%d.%d.%d.%d",ipa,ipb,ipc,ipd);
//
//				close(sockfd);
//
//				char systemstr[256];
//				sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
//				system(systemstr);
//			}
//			else
			{

				int cono=1, from_acsv = 0;
				if (cono_check&CONO_CHECK_LOGIN){
					if( StateTable[WHILELOGIN]+StateTable[WHILELOGOUTSAVE] > QUEUE_LENGTH1 ||
						StateTable[WHILEDOWNLOADCHARLIST] > QUEUE_LENGTH2 ){
							print("err State[%d,%d,%d]!!\n", StateTable[WHILELOGIN],
								StateTable[WHILELOGOUTSAVE],
								StateTable[WHILEDOWNLOADCHARLIST] );

							CONNECT_checkStatecount( WHILEDOWNLOADCHARLIST);
							cono=0;
					}
				}
				if (cono_check&CONO_CHECK_ITEM)
					if ((total_item_use+3000) >= MAX_item_use){
						print("��ʹ����Ʒ������!!");
						cono=0;
					}
					if (cono_check&CONO_CHECK_PET)
						if( (petcnt+1000) >= CHAR_getPetMaxNum() ){
							print("��ʹ�ó���������!!");
							cono=0;
						}

						//print("CO");

						{
							float fs=0.0;
							if( (fs = ((float)Connect[acfd].rbuse/AC_RBSIZE) ) > 0.6 ){
								print( "andy AC rbuse: %3.2f [%4d]\n", fs, Connect[acfd].rbuse );
								if( fs > 0.78 ) cono = 0;
							}
						}

						memcpy( &sinip, &sin.sin_addr, 4);
						// Nuke *1 0126: Resource protection
						
						if((cono == 0) || (acceptmore <= 0) || isThereThisIP( sinip) ){
							// Nuke +2 Errormessage
							char mess[64]="E�ŷ���æ���У����Ժ����ԡ�";
							if (!from_acsv)
								write(sockfd,mess,strlen(mess)+1);
							//print( "accept but drop[cono:%d,acceptmore:%d,isThereThisIP=%d]\n", cono, acceptmore,isThereThisIP( sinip));
							close(sockfd);
#ifdef _SAME_IP_ONLINE_NUM
    	       }else if(SameIpOnlineNum(sinip) == 1){
               char mess[64];
               sprintf(mess, "E����IP����%d�������ˣ����������½���Ծ�ά����Ϸƽ�⣡\n", getSameIpOnlineNum());
               write(sockfd,mess,strlen(mess)+1);
	             close(sockfd);
#endif
						}else if( sockfd < ConnectLen && sockfd-player_online<getManRenNum()){
							char mess[64] = "A";// Nuke +2 Errormessage
							if( bNewServer ){
								strcpy(mess,getSaVersion());
								//mess[0]=getSaVersion();	  // 7.0

							}else
								mess[0]='$';
							
							//char mess[1024]="E�ŷ���æ���У����Ժ����ԡ�";
							int servertype=0;
							if (!from_acsv){
#ifdef _NO_FULLPLAYER_ATT
								if(sockfd-player_online>=getNoFullPlayer()){
									time_t curtime;
									struct tm *p;
									time(&curtime);
									p=localtime(&curtime);
									curtime = mktime(p);
									if(curtime-atttime>getNoFullTime()){
										atttime = curtime;
										int j,ipa,ipb,ipc,ipd;
										char ip[32];
										char systemstr[256];
										unsigned long tmpip;
										for(j=12;j<ConnectLen;j++){
											tmpip=CONNECT_get_userip(j);
											if(j != acfd && j!=sockfd && CONNECT_getUse(j) && CONNECT_getState(j)==NOTLOGIN){
												ipa=(tmpip % 0x100); tmpip=tmpip / 0x100;
												ipb=(tmpip % 0x100); tmpip=tmpip / 0x100;
												ipc=(tmpip % 0x100); tmpip=tmpip / 0x100;
												ipd=(tmpip % 0x100);
												sprintf(ip,"%d.%d.%d.%d",ipa,ipb,ipc,ipd);
												if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
													//if(sasql_query_online_ip(ip)==0 && sasql_check_lockip(ip,getFengType())<1){
													if(getFengType()==1){
														if(sasql_query_online_ip(ip)==0){
															//if(sasql_add_lockip(ip,getFengType(),getServernumber(),1)==1){
#ifdef _CAX_OFF_IPTABLES									
															if(strcmp(ip,"0.0.0.0")!=0){
																sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
																system(systemstr);
																logCC(ip);
															}
#endif															
															CONNECT_endOne_debug(j);	
															//}
														}
													}else if(getFengType()==2){
#ifdef _CAX_OFF_IPTABLES														
														if(strcmp(ip,"0.0.0.0")!=0){
															sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
															system(systemstr);
															logCC(ip);
														}
#endif														
														CONNECT_endOne_debug(j);
													}else{
														CONNECT_endOne_debug(j);
													}
												}
											}
										}
									}
								}
//								if(sockfd-player_online>=getNoFullEndPlayer()){
//									time_t curtime;
//									struct tm *p;
//									time(&curtime);
//									p=localtime(&curtime);
//									curtime = mktime(p);
//									if(curtime-atttime>getNoFullEndTime()){
//										atttime = curtime;
//										int j,ipa,ipb,ipc,ipd;
//										char ip[32];
//										char systemstr[256];
//										unsigned long tmpip;
//										for(j=12;j<ConnectLen;j++){
//											tmpip=CONNECT_get_userip(j);
//											if(j != acfd && j!=sockfd && CONNECT_getUse(j)){
//												ipa=(tmpip % 0x100); tmpip=tmpip / 0x100;
//												ipb=(tmpip % 0x100); tmpip=tmpip / 0x100;
//												ipc=(tmpip % 0x100); tmpip=tmpip / 0x100;
//												ipd=(tmpip % 0x100);
//												sprintf(ip,"%d.%d.%d.%d",ipa,ipb,ipc,ipd);
//												if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
//													if(sasql_query_online_ip(ip)==0 && sasql_check_lockip(ip,getFengType())<1){
//														if(sasql_add_lockip(ip,getFengType(),getServernumber(),5)==1){
//															sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
//															CONNECT_endOne_debug(j);
//															system(systemstr);
//														}
//													}
//												}
//											}
//										}
//									}
//								}
								if(servertype==0)
#endif								
								{
									if(strcmp(mess,getSaVersion())!=0)
										send(sockfd,mess,strlen(mess)+1,0);
								}
							}
							if(servertype==0){
								initConnectOne(sockfd,&sin,addrlen);
								char enkey[32];
								char verenkey[32];
								memset(enkey, 0, sizeof(enkey));
								strcpy(enkey,Connect[ sockfd ].defaultkey);
								encrypt_key(enkey);
								sprintf(verenkey,"%s%s",mess,enkey);
								send(sockfd,verenkey,strlen(verenkey)+1,0);

							//		    		SetTcpBuf(sockfd);

							if ( getNodelay() ) {
								int flag = 1;
								int result = setsockopt( sockfd, IPPROTO_TCP, TCP_NODELAY,
									( char* ) & flag, sizeof( int ) );

								if ( result < 0 ) {
									close(sockfd);
									print( "setsockopt TCP_NODELAY failed:%s\n",
										strerror( errno ) );
								}
								else {
									print( "NO" );
								}
							}
							}
						}else if (strcmp(Connect[ sockfd ].cdkey, "gm")!=0) {
							// Nuke +2 Errormessage	            
							char mess[64]="E�ŷ����������������Ժ����ԡ�";
							if (!from_acsv){ 
									int j;
									for(j=12;j<ConnectLen;j++){
										if(j != acfd && j!=sockfd && strlen(Connect[ j ].cdkey)<1){
											CONNECT_endOne_debug(j);
										}
									}						
								write(sockfd,mess,strlen(mess)+1);
							}
							close(sockfd);
							// Nuke +1 0901: Why close
//#ifdef _NO_FULLPLAYER_ATT
//							if(getNoFullPlayer()>0){
//								int j,ipa,ipb,ipc,ipd;
//								char ip[32];
//								char systemstr[256];
//								unsigned long tmpip;
//								time_t curtime;
//								struct tm *p;
//								time(&curtime);
//								p=localtime(&curtime);
//								curtime = mktime(p);
//								for(j=13;j<ConnectLen;j++){
//									tmpip=CONNECT_get_userip(j);
//									if(j != acfd && Connect[ sockfd ].starttime>0 && curtime-Connect[ sockfd ].starttime>=getNoFullPlayer()){
//										ipa=(tmpip % 0x100); tmpip=tmpip / 0x100;
//										ipb=(tmpip % 0x100); tmpip=tmpip / 0x100;
//										ipc=(tmpip % 0x100); tmpip=tmpip / 0x100;
//										ipd=(tmpip % 0x100);
//										sprintf(ip,"%d.%d.%d.%d",ipa,ipb,ipc,ipd);
//										if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
//											sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
//											CONNECT_endOne_debug(j);
//											system(systemstr);
//										}
//									}
//								}
//							}
//#endif
						}
			}
		}
	}
	//DebugMainFunction="netloop_faster5";
	loop_num=0;
	gettimeofday( &st, NULL );
	while(1)
	{
		//DebugMainFunction="netloop_faster6";
		char buf[ 65535 * 2 ];
		int j;
		//ttom+1 for the debug
		static int i_tto = 0;
		static int i_timeNu = 0;
		//DebugMainFunction="netloop_faster7";
		gettimeofday( &et, NULL );
		//DebugMainFunction="netloop_faster8";
		if( time_diff_us( et,st) >= looptime_us ) //ִ��ÿ����0.1����Ҫ���ĵĹ���
		{
#define LOOP_NUM_ADD_CREDIT 5
#define CREDIT_SPOOL 3

			switch ( acceptmore ) {
	case - 1:
		print( "#" );
		break;
	case 0:
		print( "$" );

		if ( !b_first_shutdown ) {
			b_first_shutdown = TRUE;
			i_shutdown_time = SERVSTATE_getLimittime();
			print("\n �رշ�����ʱ��=%d",i_shutdown_time);
		}
		break;
	default:
		{
			static int i_counter=0;
			// Syu ADD ��ʱ��ȡAnnounce
			static int j_counter=0;
			// Syu ADD ÿСʱ���¸���Ӣ��ս�����а�����
			static int h_counter=0;
			// �������ļ�ʱ��
			static long total_count=0;
      static int checktime_counter=0;

#ifdef _AUTO_PK
			static int h_autopk=0;
#endif

#ifdef _LOOP_ANNOUNCE
			static int loop_counter=0;
#endif
#ifdef _LUCK_MAN
			static int lucktime_count=0;
#endif
#ifdef _QUESTION_ONLINE
			static int questiontime_count=0;
#endif

			int i;
			int item_max;

			if ( i_counter > 10 ) { //10��
				player_online = 0; //looptime_us
#ifdef _AC_PIORITY
				//print("\n<TL:%0.2f,FD=%d,LOOP=%d,ACFD=%d>",
				// (totalfd*1.0)/(totalloop*1.0),
				// totalfd,totalloop,totalacfd);
				totalloop = 0; totalfd = 0; totalacfd = 0;
#endif
				i_counter = 0;
				item_max = ITEM_getITEM_itemnum();
				total_item_use = ITEM_getITEM_UseItemnum();
				for ( i = 0;i < ConnectLen; i++ ) {
					if ( ( Connect[ i ].use ) && ( i != acfd )) {
						if ( CHAR_CHECKINDEX( Connect[ i ].charaindex ) )
							player_online++;
					}
				}
				if(player_online>player_maxonline){
					player_maxonline=player_online;
				}
				{
					int max, min;
					//			  int MaxItemNums;
					char buff1[ 512 ];
//					char szBuff1[ 256 ];
//#ifdef _ASSESS_SYSEFFICACY
//					{
//						float TVsec;
//						ASSESS_getSysEfficacy( &TVsec);
//						sprintf( szBuff1, "Sys:[%2.4f] \n", TVsec);
//					}
//#endif
					//MaxItemNums = ITEM_getITEM_itemnum();
					//							MaxItemNums = getItemnum();
					memset( buff1, 0, sizeof( buff1));
					CHAR_getCharOnArrayPercentage( 1, &max, &min, &petcnt);
					/*
					sprintf( buff1,"\n���=%d ����=%3.1f%% ��Ʒ=%3.1f%% �ʼ�:%d ս��:%d\n%s",
					player_online, (float)((float)(petcnt*100)/max),
					(float)((float)(total_item_use*100)/MaxItemNums),
					PETMAIL_getPetMailTotalnums(), Battle_getTotalBattleNum(), szBuff1 );
					*/
					if((petcnt+500)>=getPetcharnum() || (total_item_use+2000)>=getItemnum()*0.98){
						lssproto_Shutdown_recv(0, "hogehoge", 1);
					}
					sprintf( buff1,"\n���=%d ����=%d ��Ʒ=%d �ʼ�:%d ս��:%d",
						player_online, petcnt,total_item_use,
						PETMAIL_getPetMailTotalnums(), 
						Battle_getTotalBattleNum() );
					//sprintf( buff1,"\nPlayer=%d PM:%d B:%d %s",
					//player_online, 
					//PETMAIL_getPetMailTotalnums(), Battle_getTotalBattleNum(), szBuff1 );

					buff1[ strlen( buff1)+1]	= 0;
					print("%s", buff1);
#ifdef _ASSESS_SYSEFFICACY_SUB
					{
//						float TVsec;
//						ASSESS_getSysEfficacy_sub( &TVsec, 1);
//						sprintf( szBuff1, "NT:[%2.4f] ", TVsec);
//						strcpy( buff1, szBuff1);
//
//						ASSESS_getSysEfficacy_sub( &TVsec, 2);
//						sprintf( szBuff1, "NG:[%2.4f] ", TVsec);
//						strcat( buff1, szBuff1);
//
//						ASSESS_getSysEfficacy_sub( &TVsec, 3);
//						sprintf( szBuff1, "BT:[%2.4f] ", TVsec);
//						strcat( buff1, szBuff1);
//
//						ASSESS_getSysEfficacy_sub( &TVsec, 4);
//						sprintf( szBuff1, "CH:[%2.4f] \n", TVsec);
//						strcat( buff1, szBuff1);
						/*
						ASSESS_getSysEfficacy_sub( &TVsec, 5);
						sprintf( szBuff1, "PM:[%2.4f] \n", TVsec);
						strcat( buff1, szBuff1);

						ASSESS_getSysEfficacy_sub( &TVsec, 6);
						sprintf( szBuff1, "FM:[%2.4f] ", TVsec);
						strcat( buff1, szBuff1);

						ASSESS_getSysEfficacy_sub( &TVsec, 7);
						sprintf( szBuff1, "SV:[%2.4f] ", TVsec);
						strcat( buff1, szBuff1);

						ASSESS_getSysEfficacy_sub( &TVsec, 9);
						sprintf( szBuff1, "AG:[%2.4f] ", TVsec);
						strcat( buff1, szBuff1);

						ASSESS_getSysEfficacy_sub( &TVsec, 10);
						sprintf( szBuff1, "CE:[%2.4f] \n", TVsec);
						strcat( buff1, szBuff1);
						*/
						//buff1[ strlen( buff1)+1]	= 0;
						//print("%s.", buff1);
					}
#endif
				}
#ifdef _TIME_TICKET
				check_TimeTicket();
#endif

			}

#ifdef _LOOP_ANNOUNCE
			if ( loop_counter > 60*getLoopAnnounceTime() && getLoopAnnounceMax()>0)
			{
				int     i;
				int     playernum = CHAR_getPlayerMaxNum();
				static int index;

				for( i = 0 ; i < playernum ; i++) {
					if( CHAR_getCharUse(i) != FALSE ) {
						char buff[36];
						snprintf( buff, sizeof( buff),"%s���档",getGameserverID());
						CHAR_talkToCli( i, -1, buff, CHAR_COLORYELLOW);
						CHAR_talkToCli( i, -1, getLoopAnnounce(index % getLoopAnnounceMax()), CHAR_COLORYELLOW);
					}
				}
				index++;
				loop_counter=0;
			}
#endif

#ifdef _LUCK_MAN
			if( lucktime_count > 60*getLuckTime() && getLuckItem()>0 &&getLuckTime()>0 && CHAR_getPlayerMaxNum()>0)
			{
				int     i;
				int     playernum;
				int luckcharaindex,luckret,luckitemindex;
				char* luckname=NULL;
				char luckbuf[256];
				goto LUCKBEGIN;
LUCKBEGIN:
				playernum = CHAR_getPlayerMaxNum();
				luckcharaindex = rand()%playernum;

				luckitemindex = ITEM_makeItemAndRegist( getLuckItem());
				luckret = CHAR_addItemSpecificItemIndex(luckcharaindex,luckitemindex);
				if( luckret < 0 || luckret >= CHAR_MAXITEMHAVE )
				{
					ITEM_endExistItemsOne( luckitemindex);
					goto LUCKBEGIN;
				}
				else
				{
					luckname=CHAR_getChar(luckcharaindex,CHAR_NAME);
					CHAR_talkToCli( luckcharaindex, -1, "��ϲ����Ϊ���������ǣ�", CHAR_COLORYELLOW);
					sprintf( luckbuf, "��ϲ��ҡ�%s����Ϊ���������ǣ���ý�Ʒ��%s����",luckname,ITEM_getChar(luckitemindex,ITEM_NAME));
					for( i = 0 ; i < playernum ; i++) {
						if( CHAR_getCharUse(i) != FALSE ) {
							CHAR_talkToCli( i, -1, luckbuf, CHAR_COLORRED);
						}
					}
					lucktime_count=0;
				}
			}
#endif

#ifdef _QUESTION_ONLINE
			if(getQustionTime()>0 && questiontime_count>getQustionTime()*60 && CHAR_getPlayerMaxNum()>0)
			{
				char *res;
				res = sasql_query_question();
				if(strcmp(res,"err")!=0)
				{
					saacproto_ACQuestion_recv(res);
				}
				questiontime_count=0;
			}
#endif

#ifdef _AUTO_PK
			if(AutoPk_PKTimeGet()>-1){
				if ( h_autopk > 60 )
				{
					h_autopk=0;
					AutoPk_PKSystemInfo();
				}
			}else{
				if(autopkone == 0){
					AutoPk_GetAwardStr();
					autopkone = 1;
				}
				if(autopkstart==1){
					if(h_autopk > 10){
						h_autopk = 0;
						if(autopkend==1){
/*							time_t timep;
							time(&timep);
							int j;
							for( j = 0; j < BATTLE_battlenum; j ++ ){
								if( BattleArray[j].use == FALSE ){
									continue;
								}else{
									if(BattleArray[j].floor!=20000){
										continue;
									}else{
										if(timep-BattleArray[j].battletime>=getAutoPkBattleTime()){
											int pkfd1 = getfdFromCharaIndex(BattleArray[j].leaderindex);
											int pkfd2 = getfdFromCharaIndex(BattleArray[j].rivalindex);
											lssproto_CharLogout_recv(pkfd1,0);
											lssproto_CharLogout_recv(pkfd2,0);
										}
									}
								}
							}
							int playernum = CHAR_getPlayerMaxNum();
							int i;
							for(i=0;i<playernum;i++){
								if(CHAR_CHECKINDEX(i) == FALSE) continue;
								if(CHAR_getInt(i, CHAR_FLOOR) == 20000 )
		 							CHAR_talkToCli( i, -1, "����PK����������60���ʼ��һ�ֱ�����", CHAR_COLORRED );
							}*/
							autopkend = 0;
						}else{
							AutoPk_PKSystemStart();
							autopkend = 1;
						}
					}
				}else{
					if(getAutoPkTime()>-1 && getAutoPkTime()<24){
						time_t timep;
						struct tm *p;
						time(&timep);
						p=localtime(&timep); /*ȡ�õ���ʱ��*/
						int hour=p->tm_hour;
						int min=p->tm_min;
						if(getAutoPkTime() == hour+1 && min >=30){
							AutoPk_PKTimeSet(30);
							AutoPk_GetAwardStr();
						}
					}
				}
			}
#endif

			// Syu ADD ��ʱ��ȡAnnounce
			if ( j_counter > 60*60 )
			{//6000 Լ 600��=10����
				j_counter=0;
				print("\n��ʱ��ȡ����");
				LoadAnnounce();
			}

			if ( checktime_counter > 60*60)
			{
				checktime_counter=0;
 //       if (strcmp(getlocaltime(), TimeLimitLocal)>0){
   //       	exit(0);
      //  }
   	  }
			/*
			#ifdef _ALLDOMAN     // Syu ADD ���а�NPC
			// Syu ADD ÿСʱ���¸���Ӣ��ս�����а�����
			if ( h_counter > 60*60 ){//36000 Լ 3600��=60����
			h_counter=0;
			print("\nSyu log LoadHerolist");
			saacproto_UpdataStele_send ( acfd , "FirstLoad", "LoadHerolist" , "����" , 0 , 0 , 0 , 999 ) ;
			}
			#endif
			*/

			if ( i_timeNu != time( NULL ) )  // ÿ����ִ��һ��
			{
				i_timeNu = time( NULL );

#ifdef _DEL_DROP_GOLD
				//if( total_count % 60 == 0 ) { //ÿ����ִ��
				// GOLD_DeleteTimeCheckLoop();
				//}
#endif
				//if( total_count % 60*10 == 0 ) { //ÿ10����ִ��
				//}
#ifdef _ALLBLUES_LUA_1_5
						if( total_count % 60 == 0 ) { //ÿ����ִ��
							NetLoopFunction();
						}
#endif				
#ifdef	_PK_LUCK_MAN
			if ( total_count % 60 == 0 && getPkLuckTimeMin()>-1 && getPkLuckTimeMax()>-1 ){
				time_t timep;
				struct tm *p;
				time(&timep);
				p=localtime(&timep); /*ȡ�õ���ʱ��*/
				int hour=p->tm_hour;
				int min=p->tm_min;
				if(hour>=getPkLuckTimeMin()&&hour<=getPkLuckTimeMax()&&pkluckhour!=hour){
					if(pkluckmin==-1){
						pkluckmin = RAND(1,59);
						pklucknum = 0;
					}
					if(min==pkluckmin){
						pkluckhour = hour;
						pkluckmin=-1;
						int pkluckplayerindex[100];
						int j;
						for ( j = 0;j < ConnectLen; j++ ) {
							if ( ( Connect[ j ].use ) && ( j != acfd )) {
								if ( CHAR_CHECKINDEX( Connect[ j ].charaindex ) ){
									if( CHAR_getWorkInt( Connect[ j ].charaindex, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE && getPkLuckFloor(CHAR_getInt(Connect[ j ].charaindex,CHAR_FLOOR))>0 && CHAR_getInt(Connect[ j ].charaindex,CHAR_LV)>=getPkLuckLevel()){
										if(BattleArray[CHAR_getWorkInt( Connect[ j ].charaindex,CHAR_WORKBATTLEINDEX)].type == BATTLE_TYPE_P_vs_P && BattleArray[CHAR_getWorkInt( Connect[ j ].charaindex,CHAR_WORKBATTLEINDEX)].turn<getPkLuckBattleTurn()){
											if(pklucknum<100){
												pkluckplayerindex[pklucknum] = Connect[ j ].charaindex;
												pklucknum ++;
											}
										}
									}
								}
							}
						}
						if(pklucknum>10){
							int k,l;
							int pkluckmanindex[100];
							pkluckmanindex[0] = -1;
							int m = getPkLuckNum();
							if(pklucknum<m) k = pklucknum;
							else k = m;
							for(l=0;l<k;l++){
								int n;
								int randmax = 1;
									while(randmax<=100){
										n = RAND(1,pklucknum);
										if(CHAR_CHECKINDEX(pkluckplayerindex[n-1])) break;
										randmax++;
									}
									if(randmax>100) continue;
									char tmpbuf[256];
									int mapnum = getPkLuckFloor(CHAR_getInt(pkluckplayerindex[n-1],CHAR_FLOOR));
									char *mapname = getPkLuckMapName(mapnum-1);
									sprintf(tmpbuf,"[�У˿�Ѷ]��ϲ���[%s]��[%s]��PK�б���������",CHAR_getChar(pkluckplayerindex[n-1],CHAR_NAME),mapname);
									int lucktemp[4];
									char luckbuf[4][32] = {"����","����","��Ա��","����"};
									lucktemp[0] = getPkLuckFame();
									lucktemp[1] = getPkLuckVigor();
									lucktemp[2] = getPkLuckVipPoint();
									lucktemp[3] = getPkLuckAmPoint();
									int lucktype;
									randmax = 1;
									while(randmax<=10){
										lucktype = RAND(1,4);
										if(lucktemp[lucktype-1]>0)
											break;
										randmax++;
									}
									if(randmax>10) continue;
									if(lucktype==1){
										CHAR_setInt(pkluckplayerindex[n-1],CHAR_FAME,CHAR_getInt(pkluckplayerindex[n-1],CHAR_FAME)+lucktemp[0]*100);
									}
									else if(lucktype==2){
										CHAR_setInt(pkluckplayerindex[n-1],CHAR_VIGOR,CHAR_getInt(pkluckplayerindex[n-1],CHAR_VIGOR)+lucktemp[1]);
									}
									else if(lucktype==3){
										sasql_vippoint(CHAR_getChar(pkluckplayerindex[n-1],CHAR_CDKEY),lucktemp[2],1);
									}
									else if(lucktype==4){
										sasql_ampoint(CHAR_getChar(pkluckplayerindex[n-1],CHAR_CDKEY),lucktemp[3],1);
									}
									sprintf(tmpbuf,"%s,���%s %d!",tmpbuf,luckbuf[lucktype-1],lucktemp[lucktype-1]);
									pkluckplayerindex[n-1] = -1;
									int     i;
									int     playernum = CHAR_getPlayerMaxNum();
									for( i = 0 ; i < playernum ; i++) {
										if( CHAR_getCharUse(i) != FALSE ) {
										CHAR_talkToCli( i, -1, tmpbuf, CHAR_COLORRED);
										}
									}
							}
						}
					}
				}
			}
#endif

#ifdef	_17CSA_YANZHENG
				if( total_count % (60*60) == 0 ) { //ÿ60����ִ��
					if(yzcnt>0){
						if(yzcnt>=3){
							exit(1);
						}else{
							if(attestation()<1){
								yzcnt++;
							}else{
								yzcnt=0;
							}
						}
					}
				}
				
				if( total_count % (60*60*24) == 0 ) { //ÿ24Сʱִ��
					if(attestation()<1){
						if(yzcnt<3){
							yzcnt++;
						}else{
							exit(1);
						}
					}
				}
#endif				

#ifdef _AUTO_PK
				if(AutoPk_PKTimeGet()>-1 || autopkstart==1)
					h_autopk++;
#endif
				i_counter++;
				// Syu ADD ��ʱ��ȡAnnounce
				j_counter++;
				// Syu ADD ÿСʱ���¸���Ӣ��ս�����а�����
				h_counter++;

				total_count++;
        checktime_counter++;
#ifdef _LOOP_ANNOUNCE
				loop_counter++;
#endif
#ifdef _LUCK_MAN
				if(getLuckItem()>0 &&getLuckTime()>0 && CHAR_getPlayerMaxNum()>0){
					lucktime_count++;
				}
#endif
#ifdef _QUESTION_ONLINE
				if(getQustionTime()>0 && CHAR_getPlayerMaxNum()>0){
					questiontime_count++;
					if(questionstarttime>0)
					{
						time_t timep;
						struct tm *p;
						time(&timep);
						p=localtime(&timep);
						timep = mktime(p);
						if( timep-questionstarttime>30 || questionman>=3){
							questionstarttime=0;
							questionman=0;
							char answerbuf[256];
							sprintf(answerbuf,"�������ʴ𡿱������(%s)����û�д�Ե���Ҽ�ס��Ŷ��",answer);
							int i_count,playmaxnum;
							playmaxnum=CHAR_getPlayerMaxNum();
							for( i_count = 0 ; i_count < playmaxnum ; i_count++) {
								if( CHAR_getCharUse(i_count) != FALSE ) {
									if(CHAR_getWorkInt(i_count,CHAR_WORKQUESTIONFLG)!=1){
										CHAR_talkToCli( i_count, -1, answerbuf, CHAR_COLORRED);
									}
							 }
							}
						}
					}
				}
#endif
			}
		}

		if ( ( i_tto % 60 ) == 0 ) {
			i_tto = 0;
			print( "." );
		}
		i_tto++;

		//andy add 2003/0212------------------------------------------
		CONNECT_SysEvent_Loop( );

		//------------------------------------------------------------
			} // switch()

#ifdef _AC_PIORITY
			if ( flag_ac == 2 ) fdremember = fdremembercopy;

			flag_ac = 1;

			totalloop++;

#endif
			break; // Break while
		} // if(>0.1sec)

		loop_num++;
		//DebugMainFunction="netloop_faster9";
#ifdef _AC_PIORITY
		switch ( flag_ac ) {
	case 1:
		fdremembercopy = fdremember;
		fdremember = acfd;
		flag_ac = 2;
		break;
	case 2:
		counter++;

		if ( counter >= 3 ) {
			counter = 0;
			fdremember = fdremembercopy + 1;
			flag_ac = 0;
		}

		break;
	default:
		fdremember++;
		break;
		}

#else
		fdremember++;

#endif
		//DebugMainFunction="netloop_faster10";
		if ( fdremember == ConnectLen ) fdremember = 0;

		if ( Connect[ fdremember ].use == FALSE ) continue;

		if ( Connect[ fdremember ].state == WHILECLOSEALLSOCKETSSAVE ) continue;
			
		if ( getKongType()==1 && Connect[ fdremember ].state == NOTLOGIN && Connect[ fdremember ].ctype!=CLI ){
	    	if ( fdremember != acfd ){
		    	if(Connect[fdremember].connecttime + 20 < time(NULL)){
			    	unsigned long tmpip;
					  int a,b,c,d;
					  char ip[32];
					  char ip2[32];
					  tmpip=CONNECT_get_userip(fdremember);
					  a=(tmpip % 0x100); tmpip=tmpip / 0x100;
					  b=(tmpip % 0x100); tmpip=tmpip / 0x100;
					  c=(tmpip % 0x100); tmpip=tmpip / 0x100;
					  d=(tmpip % 0x100);
					  sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
					  if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
#ifdef _CAX_OFF_IPTABLES					  	
				    	char token[256];
				    	sprintf(token, "iptables -I INPUT -s %s -j DROP",ip); 
				    	//printf("%s\n", token);
				    	logKong(ip);
				    	system(token);
#endif				    	
						  char mess[64]="E��ip";
						  write(fdremember,mess,strlen(mess)+1);
							CONNECT_endOne_debug(fdremember );
				    	continue;
				    }
			    }
			  }
    }
		//DebugMainFunction="netloop_faster11";
#ifdef _AC_PIORITY
		totalfd++;

		if ( fdremember == acfd ) totalacfd++;

#endif
		//DebugMainFunction="netloop_faster12";
		/* read select */
		FD_ZERO( & rfds );
		FD_ZERO( & wfds );
		FD_ZERO( & efds );

		FD_SET( fdremember , & rfds );
		FD_SET( fdremember , & wfds );
		FD_SET( fdremember , & efds );
		tmv.tv_sec = tmv.tv_usec = 0;
		ret = select( fdremember + 1 , &rfds,&wfds,&efds,&tmv );

		if ( ret > 0 && FD_ISSET( fdremember, &rfds ) ) {
			errno = 0;
			memset( buf, 0, sizeof( buf ) );
			//      int flags=fcntl(fdremember,F_GETFL,0);//ͨ�� F_GETFL ��ȡ�ļ�������־��flags
			//			fcntl(fdremember,F_SETFL,flags|O_NONBLOCK|O_NDELAY);//�˴�ֻ�ǰ��������Ĵ�λ��ΪO_NONBLOCK
			ret = read( fdremember, buf, sizeof( buf ) );

			if ( ret > 0 && sizeof( buf ) <= ret ) {
				//print( "��ȡ(%s)���峤��:%d - %d !!\n", ( fdremember == acfd ) ? "SAAC" : "����", ret, sizeof( buf ) );
			}

			if( (ret == -1 && errno != EINTR) || ret == 0 ){
				if( fdremember == acfd ){
					print( "��ȡ����:%d %s\n",ret,strerror(errno));
					print( "gmsv��acsvʧȥ����! �쳣��ֹ...\n" );
					sigshutdown( -1 );
					exit(1);
				}else {
					if( ret == -1 ){
						if(errno==113 || errno==104){
							continue;
						}else{
							//print( "��ȡ����: %d %s \n", errno, strerror( errno));
						}
					}
					//print( "\nRCL " );
					CONNECT_endOne_debug(fdremember );

					continue;
				}
			}else{
				if( appendRB( fdremember, buf, ret ) == -2 && getErrUserDownFlg() == 1){
					//print("����77777");
					CONNECT_endOne_debug(fdremember );
					continue;
				}else{
					Connect[fdremember].lastreadtime = NowTime;
					Connect[fdremember].lastreadtime.tv_sec -= DEBUG_ADJUSTTIME;
					Connect[fdremember].packetin = 30;
				}
			}
		}else if( ret < 0 && errno != EINTR){
			if( fdremember != acfd ){
				//if(CHAR_getWorkInt(Connect[fdremember].charaindex,CHAR_WORKNOONLINE)!=1){
					//print( "\n��ȡ���Ӵ���:%d %s\n", errno, strerror( errno ));
					CONNECT_endOne_debug(fdremember );
					continue;
				//}
			}
		}
		
		//DebugMainFunction="netloop_faster13";

		for ( j = 0; j < 3; j ++ ) {
			char rbmess[ 65535 * 2 ];
			memset( rbmess, 0, sizeof( rbmess ) );

			if ( GetOneLine_fix( fdremember, rbmess, sizeof( rbmess ) ) == FALSE ) continue;

			if ( !( ( rbmess[ 0 ] == '\r' && rbmess[ 1 ] == '\n' ) || rbmess[ 0 ] == '\n' ) ) {
				if ( fdremember == acfd ) {
#ifdef _DEBUG
					//printf("��ȡSAAC����:%s\n",rbmess);
#endif
					//DebugMainFunction="netloop_faster14";
					if ( saacproto_ClientDispatchMessage( fdremember, rbmess ) < 0 ) {
						//print("\nSAAC:DispatchMsg_Error!!!\n");
					}
				}else {
#ifdef _DEBUG
					//printf("��ȡ�ͻ�������:%s\n",rbmess);
#endif
					//DebugMainFunction="netloop_faster15";
					if ( lssproto_ServerDispatchMessage( fdremember, rbmess ) < 0 ) {
						//print("\nLSSP:DispatchMsg_Error!!! \n");
//						unsigned long tmpip;
//    				int a,b,c,d;
//    				char ip[32];
//    				tmpip=CONNECT_get_userip(fdremember);
//    				a=(tmpip % 0x100); tmpip=tmpip / 0x100;
//    				b=(tmpip % 0x100); tmpip=tmpip / 0x100;
//    				c=(tmpip % 0x100); tmpip=tmpip / 0x100;
//    				d=(tmpip % 0x100);
//    				sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
//    				time_t curtime;
//						struct tm *p;
//						time(&curtime);
//						p=localtime(&curtime);
//						curtime = mktime(p);
//    				int j=0;
//    				for(; j < useripcount; ++j ){
//							if( userip[j] == tmpip ){
//								if(userip_count_data[j]>=getAttDmeCnt()){
//
//									print( "�Ƿ����=%d �Ƿ�IP=%s \n", fdremember, ip );
//
//									char systemerrstr[256];
//									sprintf(systemerrstr,"iptables -I INPUT -s %s -j DROP",ip);
//									system(systemerrstr);
//									userip_count_data[j] = 1;
//									useriptime[j] = curtime;
//									CONNECT_endOne_debug( fdremember );
//									break;
//								}
//							if(curtime-useriptime[j]<=getAttDmeTime()){
//								++userip_count_data[j];
//								break;
//							}else{
//								useriptime[j]=curtime;
//								break;
//							}
//						}
//					}
//					if(strcmp(ip,"59.53.88.217")!=0 && strcmp(ip,"58.17.30.87")!=0){
//						if(j>= useripcount && useripcount<useripnum && strlen(Connect[ fdremember ].cdkey)<1){
//						userip[useripcount] = tmpip;
//						userip_count_data[useripcount] = 1;
//						useriptime[useripcount] = curtime;
//						useripcount++;
//						}
//					}
						//DebugMainFunction="netloop_faster16";
						if(++Connect[ fdremember ].errornum > allowerrornum )
							break;
					}else{
						//DebugMainFunction="netloop_faster17";
						if(CONNECT_getState(fdremember)==NOTLOGIN){
							Connect[ fdremember ].rightnum++;
						}
						if(Connect[ fdremember ].rightnum>getRightNum()){	//��ֹ��ȷ�������
							unsigned long tmpip;
    					int a,b,c,d;
    					char ip[32];
    					tmpip=CONNECT_get_userip(fdremember);
    					a=(tmpip % 0x100); tmpip=tmpip / 0x100;
   						b=(tmpip % 0x100); tmpip=tmpip / 0x100;
    					c=(tmpip % 0x100); tmpip=tmpip / 0x100;
    					d=(tmpip % 0x100);
    					sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
							if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
								//if(CONNECT_getState(fdremember)==NOTLOGIN && sasql_check_lockip(ip,getFengType())<1){
								if(CONNECT_getState(fdremember)==NOTLOGIN){
									if(getRightMode()==0){
										if(sasql_query_online_ip(ip)==0){
											//if(sasql_add_lockip(ip,getFengType(),getServernumber(),4)==1){
#ifdef _CAX_OFF_IPTABLES											
											if(strcmp(ip,"0.0.0.0")!=0){
												char systemstr[128];
												sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
												system(systemstr);
												logRightBao(ip,strlen(rbmess));
											}
#endif											
											CONNECT_endOne_debug(fdremember);
											break;
											//}
										}
									}else{
										//if(sasql_add_lockip(ip,getFengType(),getServernumber(),4)==1){
#ifdef _CAX_OFF_IPTABLES										
										if(strcmp(ip,"0.0.0.0")!=0){
											char systemstr[128];
											sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
											system(systemstr);
											logRightBao(ip,strlen(rbmess));
										}
#endif										
										CONNECT_endOne_debug(fdremember);
										break;
										//}
									}
								}
							}else{
								if(CONNECT_getState(fdremember)==NOTLOGIN){
									CONNECT_endOne_debug(fdremember);
									break;
								}
							}
						}
						//DebugMainFunction="netloop_faster18";
					}
				}
			}
		}
		//DebugMainFunction="netloop_faster19";
		if ( Connect[ fdremember ].errornum > getFengerrornum() ) {
			unsigned long tmpip;
    	int a,b,c,d;
    	char ip[32];
    	tmpip=CONNECT_get_userip(fdremember);
    	a=(tmpip % 0x100); tmpip=tmpip / 0x100;
   		b=(tmpip % 0x100); tmpip=tmpip / 0x100;
    	c=(tmpip % 0x100); tmpip=tmpip / 0x100;
    	d=(tmpip % 0x100);
    	sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
			if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
				//if(CONNECT_getState(fdremember)==NOTLOGIN && sasql_check_lockip(ip,getFengType())<1){
				if(CONNECT_getState(fdremember)==NOTLOGIN){
					if(getErrorMode()==0){
						if(sasql_query_online_ip(ip)==0){
							//if(sasql_add_lockip(ip,getFengType(),getServernumber(),3)==1){
#ifdef _CAX_OFF_IPTABLES							
							if(strcmp(ip,"0.0.0.0")!=0){
								char systemstr[128];
								sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
								system(systemstr);
								logWrongBao(ip);
							}
#endif							
							CONNECT_endOne_debug(fdremember);
							continue;
							//}
						}
					}else{
						//if(sasql_add_lockip(ip,getFengType(),getServernumber(),3)==1){
#ifdef _CAX_OFF_IPTABLES					
						if(strcmp(ip,"0.0.0.0")!=0){
							char systemstr[128];
							sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
							system(systemstr);
							logWrongBao(ip);
						}
#endif						
						CONNECT_endOne_debug(fdremember);
						continue;
						//}
					}
				}
			}else{
				if(CONNECT_getState(fdremember)==NOTLOGIN){
					CONNECT_endOne_debug( fdremember );
					continue;
				}
			}
		}
		//DebugMainFunction="netloop_faster20";
		if ( Connect[ fdremember ].errornum > allowerrornum ) {
			//print( "�û�:%s����̫������ˣ�����ǿ�ƹر�\n",inet_ntoa(Connect[fdremember].sin.sin_addr ));
			logHackKick(fdremember);
				CONNECT_endOne_debug( fdremember );
				continue;
		}
		//DebugMainFunction="netloop_faster21";
		if ( Connect[ fdremember ].CAbufsiz > 0 
			&& time_diff_us( et, Connect[ fdremember ].lastCAsendtime ) > casend_interval_us ) {
				CAsend( fdremember );
				Connect[ fdremember ].lastCAsendtime = et;
		}
		//DebugMainFunction="netloop_faster22";
		if ( Connect[ fdremember ].CDbufsiz > 0 
			&& time_diff_us( et, Connect[ fdremember ].lastCDsendtime ) > cdsend_interval_us ) {
				CDsend( fdremember );
				Connect[ fdremember ].lastCDsendtime = et;
		}
		//DebugMainFunction="netloop_faster23";
		if ( Connect[ fdremember ].wbuse > 0 ) {
			FD_ZERO( & rfds );
			FD_ZERO( & wfds );
			FD_ZERO( & efds );

			FD_SET( fdremember , & rfds );
			FD_SET( fdremember , & wfds );
			FD_SET( fdremember , & efds );
			tmv.tv_sec = tmv.tv_usec = 0;
			ret = select( fdremember + 1 , &rfds,&wfds,&efds,&tmv );

			if ( ret > 0 && FD_ISSET( fdremember , &wfds )) {
				//Nuke start 0907: Protect gmsv
				//				int flags=fcntl(fdremember,F_GETFL,0);//ͨ�� F_GETFL ��ȡ�ļ�������־��flags
				//				fcntl(fdremember,F_SETFL,flags|O_NONBLOCK|O_NDELAY);//�˴�ֻ�ǰ��������Ĵ�λ��ΪO_NONBLOCK
				if ( fdremember == acfd ) {
					ret = write( fdremember , Connect[ fdremember ].wb ,
						( Connect[fdremember].wbuse < acwritesize) ? Connect[fdremember].wbuse : acwritesize );
#ifdef _DEBUG
					//printf("����SAAC����:%s\n",Connect[ fdremember ].wb);
					memset( Connect[ fdremember ].wb, 0, WBSIZE );
#endif
				}else {
							ret = write( fdremember , Connect[fdremember].wb ,
								(Connect[fdremember].wbuse < 1024*16) ?
								Connect[fdremember].wbuse : 1024*16 ); 
				}

				// Nuke end

				if ( ret == -1 && errno != EINTR ) {
					//print( "д�뷵��: %d %s \n", errno, strerror( errno));
					CONNECT_endOne_debug( fdremember );
					continue;
				} else if( ret > 0 ){
					shiftWB( fdremember, ret );
				}
			}else if( ret < 0 && errno != EINTR  ){
				//if(CHAR_getWorkInt(Connect[fdremember].charaindex,CHAR_WORKNOONLINE)!=1){
					//print( "\nд�����Ӵ���:%d %s\n",errno, strerror( errno ));
					CONNECT_endOne_debug(fdremember );
				//}
			}
		}
		//DebugMainFunction="netloop_faster24";
		/* �����ॢ���Ȥν��� */
		if ( fdremember == acfd )
			continue;
		//DebugMainFunction="netloop_faster25";
		//ttom start : because of the second have this
		if ( Connect[ fdremember ].close_request ) {
			     //print( "ǿ�ȹر�:%s \n",inet_ntoa(Connect[fdremember].sin.sin_addr ));
			CONNECT_endOne_debug( fdremember );
			continue;
		}
		//DebugMainFunction="netloop_faster27";
		//ttom end
	}

	return TRUE;
}

ANYTHREAD void outputNetProcLog( int fd, int mode)
{
	int i;
	int c_use = 0, c_notdetect = 0 ;
	int c_ac = 0, c_cli = 0 , c_adm = 0, c_max = 0;
	int login = 0;
	char buffer[ 4096 ];
	char buffer2[ 4096 ];

	strcpysafe( buffer, sizeof( buffer ), "Server Status\n" );
	c_max = ConnectLen;


	for ( i = 0;i < c_max;i++ ) {
		CONNECT_LOCK( i );

		if ( Connect[ i ].use ) {
			c_use ++;

			switch ( Connect[ i ].ctype ) {
	  case NOTDETECTED: c_notdetect++; break;
	  case AC: c_ac ++; break;
	  case CLI: c_cli ++; break;
	  case ADM: c_adm ++; break;
			}

			if ( Connect[ i ].charaindex >= 0 ) {
				login ++;
			}
		}

		CONNECT_UNLOCK( i );
	}

	snprintf( buffer2 , sizeof( buffer2 ) ,
		"connect_use=%d\n"
		"connect_notdetect=%d\n"
		"connect_ac=%d\n"
		"connect_cli=%d\n"
		"connect_adm=%d\n"
		"connect_max=%d\n"
		"login=%d\n",
		c_use , c_notdetect, c_ac, c_cli, c_adm, c_max, login );
	strcatsafe( buffer , sizeof( buffer ), buffer2 );
	{
		int char_max = CHAR_getCharNum();
		int char_use = 0 ;
		int pet_use = 0;

		for ( i = 0;i < char_max;i++ ) {
			if ( CHAR_getCharUse( i ) ) {
				char_use++;

				if ( CHAR_getInt( i, CHAR_WHICHTYPE ) == CHAR_TYPEPET ) {
					pet_use ++;
				}
			}
		}

		snprintf( buffer2, sizeof( buffer2 ) ,
			"char_use=%d\n"
			"char_max=%d\n"
			"pet_use=%d\n",
			char_use , char_max, pet_use );
		strcatsafe( buffer , sizeof( buffer ), buffer2 );
	}

	{

		int i;
		int item_max = ITEM_getITEM_itemnum();
		int item_use = 0;

		for ( i = 0;i < item_max;i++ ) {
			if ( ITEM_getITEM_use( i ) ) {
				item_use ++;
			}
		}

		snprintf( buffer2, sizeof( buffer2 ),
			"item_use=%d\n"
			"item_max=%d\n",
			item_use , item_max );
		strcatsafe( buffer , sizeof( buffer ), buffer2 );
	}

	{
		int i , obj_use = 0;
		int obj_max = OBJECT_getNum();

		for ( i = 0;i < obj_max;i++ ) {
			if ( OBJECT_getType( i ) != OBJTYPE_NOUSE ) {
				obj_use ++;
			}
		}

		snprintf( buffer2, sizeof( buffer2 ) ,
			"object_use=%d\n"
			"object_max=%d\n",
			obj_use , obj_max );
		strcatsafe( buffer , sizeof( buffer ) , buffer2 );
	}

	if ( mode == 0 ) {
		printl( LOG_PROC , buffer );
	}else if( mode == 1 ) { 
		lssproto_ProcGet_send( fd, buffer );
	}
}

/*------------------------------------------------------------
* cdkey ���� fd �����롣
* ����
*  cd      char*       cdkey
* �֤���
*  �ե�����ǥ�������ץ�  ��äơ� -1 �λ��ϥ��顼
------------------------------------------------------------*/
ANYTHREAD int getfdFromCdkeyWithLogin( char* cd )
{
	int i;

	for ( i = 0 ;i < ConnectLen ; i ++ ) {
		CONNECT_LOCK( i );

		if ( Connect[ i ].use == TRUE
			&& Connect[ i ].state != NOTLOGIN // Nuke 0514: Avoid duplicated login
			&& strcmp( Connect[ i ].cdkey , cd ) == 0 ) {
				CONNECT_UNLOCK( i );
				return i;
		}

		CONNECT_UNLOCK( i );
	}

	return -1;
}


/***********************************************************************
MTIO ����
***********************************************************************/
// Nuke start 08/27: For acceleration avoidance
//ttom+1
#define m_cktime 500
//static float m_cktime=0;

int checkWalkTime(int fd)
{
	int me, interval;
	//ttom
	float f_Wtime, f_WLtime, f_interval;
	// Nuke
	return 0;

	//ARM&Tom correct
	//me=CONNECT_getCharaindex(fd);
	me = fd;
	Connect[ me ].Walktime = time( 0 );

	gettimeofday( &Connect[ me ].Wtime, ( struct timezone* ) NULL ); //
	interval = abs( Connect[ me ].Walktime - Connect[ me ].lastWalktime );
	//ttom
	f_Wtime = Connect[ me ].Wtime.tv_sec * 1000 + Connect[ me ].Wtime.tv_usec / 1000;
	f_WLtime = Connect[ me ].WLtime.tv_sec * 1000 + Connect[ me ].WLtime.tv_usec / 1000;
	f_interval = abs( f_Wtime - f_WLtime );
	//ttom
	Connect[ me ].Walkrestore--;

	if ( Connect[ me ].Walkrestore <= 0 ) {
		//print("Walkspool restored ");
		Connect[ me ].Walkspool = WALK_SPOOL;
		Connect[ me ].Walkrestore = WALK_RESTORE;
	}

	if ( f_interval > m_cktime ) {
		Connect[ me ].WLtime = Connect[ me ].Wtime;
		Connect[ me ].Walkcount = 0;
	} else {
		Connect[ me ].Walkcount++;

		if ( Connect[ me ].Walkcount > 1 ) {
			Connect[ me ].Walkspool--;

			if ( Connect[ me ].Walkspool > 0 ) {
				Connect[ me ].Walkcount = 0;
				print( "Walkspool consumed as %d", Connect[ me ].Walkspool );
				return 0;
			}

			/*Connect[me].Walkspool=WALK_SPOOL;
			Connect[me].Walkrestore=WALK_RESTORE;
			print("Walk dropped \n");
			Connect[me].credit=-10;
			return 0;*/
			return -1;
		}
	}
	return 0;
}
int setBtime(int fd)
{
	int me, interval;
	//ARM & Tom
	//me=CONNECT_getCharaindex(fd);
	me = fd;
	Connect[ me ].BEOrestore--;

	if ( Connect[ me ].BEOrestore <= 0 ) {
		Connect[ me ].BEOrestore = BEO_RESTORE;
		Connect[ me ].BEOspool = BEO_SPOOL;
		//print("BEOspool restored ");
	}

	Connect[ me ].lastlastBtime = Connect[ me ].lastBtime;
	Connect[ me ].lastBtime = Connect[ me ].Btime;
	Connect[ me ].Btime = time( 0 );
	interval = abs( Connect[ me ].Btime - Connect[ me ].lastlastBtime );
	//print("B3interval:%d ",interval);

	if ( interval < B3_TOLERANCE ) {
		Connect[ me ].BEOspool--;
		//print("B3spool consumed as:%d ",Connect[me].BEOspool);

		if ( Connect[ me ].BEOspool <= 0 ) return -1;
		else return 0;
	} else return 0;
}
int checkBEOTime(int fd)
{
	int me, interval;
	//ARM & Tom
	//me=CONNECT_getCharaindex(fd);
	me = fd;
	Connect[ me ].EOtime = time( 0 );
	interval = abs( Connect[ me ].EOtime - Connect[ me ].Btime );
	//print("BEOinterval:%d ",interval);

	if ( interval < BEO_TOLERANCE ) {
		Connect[ me ].BEOspool--;
		//print("BEOspool consumed as:%d ",Connect[me].BEOspool);
		// Nuke 0626: Do not kick out
		if (Connect[me].BEOspool <= 0) { Connect[me].nu_decrease++; return -1; }
		else return 0;
	} else return 0;
}
int ITEM_getRatio()
{
	int i, r;
	int item_max = ITEM_getITEM_itemnum();
	int item_use = 0;

	for ( i = 0;i < item_max;i++ ) {
		if ( ITEM_getITEM_use( i ) ) {
			item_use ++;
		}
	}

	r = ( item_use * 100 ) / item_max;
	print( "ItemRatio=%d%% ", r );
	return r;
}
int CHAR_players()
{
	int i;
	int chars = 0;
	int players = 0, pets = 0, others = 0;
	int whichtype = -1;
	int objnum = OBJECT_getNum();
	/* ���ڷ�obj������ */

	for ( i = 0 ; i < objnum ; i++ ) {
		switch ( OBJECT_getType( i ) ) {
	case OBJTYPE_CHARA:
		chars++;
		whichtype = CHAR_getInt( OBJECT_getIndex( i ), CHAR_WHICHTYPE );

		if ( whichtype == CHAR_TYPEPLAYER ) players++;
		else if ( whichtype == CHAR_TYPEPET ) pets++;
		else others ++;

		break;

	default:
		break;
		}
	}

	return players;
}
void sigusr1(int i)
{
	signal( SIGUSR1, sigusr1 );
	cono_check = ( cono_check + 1 ) % 4;
	print( "Cono Check is login:%d item:%d", cono_check & 1, cono_check & 2 );
}
// Arminius 6.26
void sigusr2(int i)
{
	signal( SIGUSR2, sigusr2 );
	print( "\nReceived Shutdown signal...\n\n" );
	lssproto_Shutdown_recv( 0, "hogehoge", 5 ); // 5������ά��
}

// Nuke end
//ttom start
void CONNECT_set_watchmode(int fd, BOOL B_Watch)
{
	//int me;
	//me = CONNECT_getCharaindex( fd );
	Connect[ fd ].in_watch_mode = B_Watch;
}
BOOL CONNECT_get_watchmode(int fd)
{
	//int me;
	BOOL B_ret;
	//me = CONNECT_getCharaindex( fd );
	B_ret = Connect[ fd ].in_watch_mode;
	return B_ret;
}
BOOL CONNECT_get_shutup(int fd)
{
	//int me;
	BOOL B_ret;
	//me = CONNECT_getCharaindex( fd );
	B_ret = Connect[ fd ].b_shut_up;
	return B_ret;
}
void CONNECT_set_shutup(int fd,BOOL b_shut)
{
	//int me;
	//me = CONNECT_getCharaindex( fd );
	Connect[ fd ].b_shut_up = b_shut;
}
unsigned long CONNECT_get_userip(int fd)
{
	unsigned long ip;
	memcpy( &ip, &Connect[ fd ].sin.sin_addr, sizeof( long ) );
	return ip;
}
void CONNECT_set_pass(int fd,BOOL b_ps)
{
	int me;
	me = CONNECT_getCharaindex( fd );
	Connect[ me ].b_pass = b_ps;
}
BOOL CONNECT_get_pass(int fd)
{
	//int me;
	BOOL B_ret;
	//me = CONNECT_getCharaindex( fd );
	B_ret = Connect[ fd ].b_pass;
	return B_ret;
}
void CONNECT_set_first_warp(int fd,BOOL b_ps)
{
	//int me;
	//me = CONNECT_getCharaindex( fd );
	Connect[ fd ].b_first_warp = b_ps;
}
BOOL CONNECT_get_first_warp(int fd)
{
	//int me;
	BOOL B_ret;
	//me = CONNECT_getCharaindex( fd );
	B_ret = Connect[ fd ].b_first_warp;
	return B_ret;
}
void CONNECT_set_state_trans(int fd,int a)
{
	//int me;
	//me = CONNECT_getCharaindex( fd );
	Connect[ fd ].state_trans = a;
}
int CONNECT_get_state_trans(int fd)
{
	int i_ret;
	//me = CONNECT_getCharaindex( fd );
	i_ret = Connect[ fd ].state_trans;
	return i_ret;
}
//ttom end

// Arminius 6.22 encounter
int CONNECT_get_CEP(int fd)
{
	return Connect[ fd ].CEP;
}

void CONNECT_set_CEP(int fd, int cep)
{
	Connect[ fd ].CEP = cep;
}
// Arminius end

// Arminius 7.12 login announce
int CONNECT_get_announced(int fd)
{
	return Connect[ fd ].announced;
}

void CONNECT_set_announced(int fd, int a)
{
	Connect[ fd ].announced = a;
}

// shan trade(DoubleCheck) begin
int CONNECT_get_confirm(int fd)
{
	return Connect[ fd ].confirm_key;
}
void CONNECT_set_confirm(int fd, BOOL b)
{
	Connect[ fd ].confirm_key = b;
}
// end
int isDie(int fd)
{
	return ( Connect[ fd ].die );
}

void setDie(int fd)
{
	Connect[ fd ].die = 1;
}

int checkNu(fd)
{
	Connect[ fd ].nu--;
	//print("NU=%d\n",Connect[fd].nu);

	if ( Connect[ fd ].nu < 0 ) return -1;

	return 0;
}

int checkKe(fd)
{
	Connect[ fd ].ke--;
	//print("KE=%d\n",Connect[fd].ke);

	if ( Connect[ fd ].ke < 0 ) return -1;

	return 0;
}

// Nuke start 0626: For no enemy function
void setNoenemy(fd)
{
	Connect[ fd ].noenemy = 6;
}
void clearNoenemy(fd)
{
	Connect[ fd ].noenemy = 0;
}
int getNoenemy(fd)
{
	return Connect[ fd ].noenemy;
}
// Nuke end

// Arminius 7/2: Ra's amulet
void setEqNoenemy(int fd, int level)
{
	Connect[ fd ].eqnoenemy = level;
}

void clearEqNoenemy(int fd)
{
	Connect[ fd ].eqnoenemy = 0;
}

int getEqNoenemy(int fd)
{
	return Connect[ fd ].eqnoenemy;
}

#ifdef _Item_MoonAct
void setEqRandenemy(int fd, int level)
{
	Connect[ fd ].eqrandenemy = level;
}

void clearEqRandenemy(int fd)
{
	Connect[ fd ].eqrandenemy = 0;
}

int getEqRandenemy(int fd)
{
	return Connect[ fd ].eqrandenemy;
}

#endif

#ifdef _CHIKULA_STONE
void setChiStone(int fd, int nums)
{
	Connect[ fd ].chistone = nums;
}
int getChiStone(int fd)
{
	return Connect[ fd ].chistone;
}
#endif

// Arminius 7.31 cursed stone
void setStayEncount(int fd)
{
	Connect[ fd ].stayencount = 1;
}

void clearStayEncount(int fd)
{
	Connect[ fd ].stayencount = 0;
}

int getStayEncount(int fd)
{
	return Connect[ fd ].stayencount;
}

void CONNECT_setBDTime( int fd, int nums)
{
	Connect[ fd ].BDTime = nums;
}

int CONNECT_getBDTime( int fd)
{
	return Connect[ fd ].BDTime;
}

#ifdef _TYPE_TOXICATION
void setToxication( int fd, int flg)
{
	Connect[ fd ].toxication = flg;
}
int getToxication( int fd)
{
	return Connect[ fd ].toxication;
}
#endif

#ifdef _NO_WARP
// shan hjj add Begin
int CONNECT_get_seqno(int fd)
{   
	return Connect[fd].seqno;
}
void CONNECT_set_seqno(int fd, int a)
{
	if( (Connect[fd].seqno==CHAR_WINDOWTYPE_QUIZ_MAIN)&&(a==0) )  
		a = CHAR_WINDOWTYPE_QUIZ_MAIN;
	Connect[fd].seqno = a; 
}
int CONNECT_get_selectbutton(int fd)
{
	return Connect[fd].selectbutton;
}
void CONNECT_set_selectbutton(int fd, int a)
{
	Connect[fd].selectbutton = a;	
}
// shan End
#endif

#ifdef _BATTLE_TIMESPEED
void RescueEntryBTime( int charaindex, int fd, unsigned int lowTime, unsigned int battletime)
{
	int NowTime = ( int ) time( NULL );

	Connect[ fd ].CBTime = NowTime;
	//Connect[fd].CBTime+battletime
}

BOOL CheckDefBTime( int charaindex, int fd, unsigned int lowTime, unsigned int battletime, unsigned int addTime)//lowTime�ӳ�ʱ��
{
#ifdef CXC_BOXC_WUGUA
	if( fd < 0 ||  fd >= ConnectLen ){
		return TRUE;
	}
#endif	
	int delayTime = 0;
	unsigned int NowTime = ( unsigned int ) time( NULL );

	//print(" NowTime=%d lowTime=%d battleTime=%d CBTime=%d", NowTime, lowTime, battletime, Connect[fd].CBTime);

	lowTime += battletime;

	if ( ( Connect[ fd ].CBTime + battletime ) > lowTime ) lowTime = Connect[ fd ].CBTime + battletime;
#ifndef CXC_BOXC_WUGUA		

	if ( NowTime < lowTime ) { //lowTimeӦ�õ�ս������ʱ��
		int r = 0;
		delayTime = lowTime - NowTime;
		delayTime = ( delayTime <= 0 ) ? 1 : delayTime;
		r = ( -4 ) * ( delayTime + 2 );
		//print("\nbr=%d\n",r);
//		if(getSaType()==8018)
//			r = 0;
		lssproto_NU_send( fd, r );
		Connect[ fd ].nu += r;
	}

#endif
#ifdef CXC_BOXC_WUGUA
  lssproto_NU_send( fd, 0 );
#endif
	//Connect[fd].BDTime = (NowTime+20)+delayTime;
#ifdef _FIX_CHARLOOPS
	if(getCharloops()>0)
		Connect[fd].BDTime = NowTime + rand() % getCharloops() ; // �񱦵ȴ�ʱ��
	else
		Connect[fd].BDTime = NowTime;
#else
	Connect[ fd ].BDTime = ( NowTime + rand() % 5 ) + delayTime + addTime; // �񱦵ȴ�ʱ��
#endif
	//print(" BDTime=%d ", Connect[fd].BDTime);
	return TRUE;
}
#endif

BOOL MSBUF_CHECKbuflen( int size, float defp)
{
	return TRUE;
}

void SetTcpBuf( int sockfd )
{

	unsigned long param=1;
	int nRecvBuf = getrecvbuffer()*1024;
	int nSendBuf = getsendbuffer()*1024;
	//  int nRecvlowatBuf = getrecvlowatbuffer();
	//	int nNetTimeout=0;//0��
	//	BOOL bDontLinger = FALSE; 
	//	BOOL bReuseaddr=TRUE;

	struct linger rLinger;
	rLinger.l_onoff  = 1;   // ��linegr����
	rLinger.l_linger = 0;   // �����ӳ�ʱ��Ϊ 0 ��, ע�� TCPIP�����رգ������п��ܳ��ֻ���

	//���ý��ջ���
	if(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,(char*)&nRecvBuf, sizeof(int)) < 0)
		print( "\n����SO_RCVBUFʧ��!!!!\n" );

	//���÷��ͻ���
	if(setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&nSendBuf, sizeof(int)) < 0)
		print( "\n����SO_SNDBUFʧ��!!!!\n" );
	/*	
	if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(int)) < 0)
	print( "\n����SO_SNDTIMEOʧ��!!!!\n" );

	if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int)) < 0)
	print( "\n����SO_RCVTIMEOʧ��!!!!\n" );

	if(setsockopt(sockfd,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL)) < 0)
	print( "\n����SO_DONTLINGERʧ��!!!!\n" );

	if(setsockopt(sockfd,SOL_SOCKET ,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(BOOL)) < 0)
	print( "\n����SO_REUSEADDRʧ��!!!!\n" );
	*/
	if(setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char *)&rLinger, sizeof(rLinger)) < 0)
		print( "\n����SO_LINGERʧ��!!!!\n" );
	/*  //���ý��ջ�������
	if (setsockopt( sockfd, SOL_SOCKET, SO_RCVLOWAT, (char*)&nRecvlowatBuf, sizeof(int) < 0)
	print( "\n���ý���SO_RCVLOWATʧ��!!!!\n" );
	*/

	//	if(ioctl(sockfd, FIONBIO, &param))
	//		print( "\n���ý���FIONBIOʧ��!!!!\n" );

	//	int flags=fcntl(sockfd,F_GETFL,0);//ͨ�� F_GETFL ��ȡ�ļ�������־��flags
	//	fcntl(sockfd,F_SETFL,flags|O_NONBLOCK|O_NDELAY);//�˴�ֻ�ǰ��������Ĵ�λ��ΪO_NONBLOCK
}


#ifdef _SAME_IP_ONLINE_NUM
int SameIpOnlineNum( unsigned long ip ){
#ifdef _CHECK_SEVER_IP
	if(checkServerIp(ip) == TRUE){
		return 0;
	}
#endif
	int MAX_USER = getFdnum();
	int i, num=0;
	if(getSameIpOnlineNum()>0){
		for(i=4;i<MAX_USER;i++){
	    int i_use;
	    i_use=CONNECT_getUse(i);
	    if(i_use){
		    if(ip == CONNECT_get_userip(i)){
					num++;
					if(num>= getSameIpOnlineNum()){
						return 1;
					}
				}
			}
		}
	}
	
	return 0;
}
#endif

#ifdef _CAX_LNS_NLSUOXU
BOOL DelTimer_net(int ID)
{
	if (ID-1 < 0 || ID-1 > arraysizeof( TimerLua )) return FALSE;
	TimerLua[ID-1].FileName = NULL;
	TimerLua[ID-1].FuncName = NULL;
	TimerLua[ID-1].EspTime = -1;
	TimerLua[ID].MespTime=-1;
	TimerLua[ID-1].ID = -1;
	TimerLua[ID-1].GetTime = -1;
	return	TRUE;
}
#endif


#ifdef _CAX_LNS_CHARSUOXU
char* CONNECT_get_userip2(int fd)
{
	return inet_ntoa(Connect[fd].sin.sin_addr );
}
int CONNECT_get_userport(int fd)
{
	return Connect[ fd ].sin.sin_port;
}
#endif