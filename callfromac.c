#include <stdio.h>
#include "version.h"
#include "common.h"
#include "saacproto_cli.h"
#include "lssproto_serv.h"
#include "net.h"
#include "char.h"
#include "object.h"
#include "readmap.h"
#include "msignal.h"
#include "map_deal.h"
#include "npc_duelranking.h"
#include "npc_warpman.h"
#include "handletime.h"
#include "npc_scheduleman.h"
#include "configfile.h"
#include "chatroom.h"
#include "npc_eventaction.h"
// CoolFish: Family 2001/5/24
#include "family.h"
#include "log.h"
#include "buf.h"
#ifdef _ALLDOMAN // (���ɿ�) Syu ADD ���а�NPC
#include "npc_alldoman.h"
#endif
#include "enemy.h"
time_t initTime =0;

extern BOOL bNewServer;
extern int i_shutdown_time;//ttom
extern int manorflag[10];
extern char manorname[10][32];
#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif

char question[64];
char answer[64];
int questionstarttime=0;
int questionman=0;
#ifdef _ALL_ITEM
int allnum1=0;
int allnum2=0;
int allnum3=0;
int allnum4=0;
int allnum5=0;
#endif
	
void saacproto_ACGmsvDownRequest_recv( int fd, int min)
{
	char	buff[32];
    int     i;
    int     playernum = CHAR_getPlayerMaxNum();

		snprintf( buff, sizeof( buff),"%s���档",getGameserverID());
    for( i = 0 ; i < playernum ; i++) {
        if( CHAR_getCharUse(i) != FALSE ) {
			CHAR_talkToCli( i, -1, buff, CHAR_COLORYELLOW);
		}
	}
	SERVSTATE_setLimittime(min);
	SERVSTATE_setShutdown( NowTime.tv_sec );
	SERVSTATE_setDsptime( 0 );
	
}
//����Ƿ��к�ac����,�Լ��������
void saacproto_ACServerLogin_recv (int fd, char* result,char* data)
{
    if( strcmp( result , SUCCESSFUL ) != 0 ){
        print( "��AC����������ʧ��!�˳�����...[%s][%s]\n",
              result,data );
        shutdownProgram( );
        exit(1);
    }
    print("��AC���������ӳɹ���\n");
	time(&initTime);
/*
		print("���������ͣ�");
    if( bNewServer )
    	print("�·�����! \n");
    else
    	print("�ɷ�����! \n");
*/
#ifdef _SERVER_NUMS
		print(" ����������!\n\n");
#endif

		printf( "GMSV����汾: <%s %s>\n" , SERVER_VERSION, "��Ѱ�");
		printf( "GMSV����ʱ��: %s %s \n" , __DATE__ , __TIME__ );
		
    printf( "�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~������������~�~�~�~�~�~�~�~�~�~�~�~�~�~�~\n" );
    printf( "�~                                                                      �~\n" );
    printf( "�~                                                                      �~\n" );
		printf( "�~     ��Ϊ��ѷ���ˡ��Ͻ���ҵ��;,��������ɵĺ�����ге���          �~\n" );
		printf( "�~                                                                      �~\n" );
		printf( "�~                                                                      �~\n" );
		printf( "�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~�~\n" );
    // Robin 0530
    //saacproto_ACShowFMList_send( acfd );
    print("\n��ʼ������...");
    FAMILY_Init();
    print("���\n");
#ifdef _ANGEL_SUMMON
	print("��ʼ�������ٻ�����...");
	saacproto_ACMissionTable_send( acfd, -1, 1, "", "");
	print("���\n");
	

#endif
}

void saacproto_ACCharList_recv(int fd, char* result, char* data,int retfd)
{
    int clifd = getfdFromFdid(retfd);

    if( CONNECT_checkfd(clifd) == FALSE )
			return;

		lssproto_CharList_send( clifd, result , data );
    CONNECT_setState( clifd, NOTLOGIN );
}

void saacproto_ACCharLoad_recv(int fd , char* result,char* data,int retfd,int saveindex)
{
    int clifd = getfdFromFdid(retfd);
    if( CONNECT_checkfd( clifd ) == FALSE )return;
    if(( strcmp( result , SUCCESSFUL ) == 0 )&&(data[0])) {
			CHAR_login( clifd,data,saveindex );
    }else{
		char cdkey[64];
#ifndef _FIX_MESSAGE             // WON ADD �޸�WORKER�������
		data = "";		
#endif
		CONNECT_getCdkey( clifd, cdkey , sizeof( cdkey ));
		print(" (%s)ACCharLoad����:%s ", cdkey, data);
        lssproto_CharLogin_send( clifd, result ,data );
        CONNECT_setState( clifd, NOTLOGIN );
    }
}
 
void saacproto_ACCharSave_recv(int fd ,char* result, char* data,int retfd)
{
    int clifd = getfdFromFdid(retfd) ,  fdid;
    char cdkey[CDKEYLEN], passwd[PASSWDLEN], charname[CHARNAMELEN];

    if( CONNECT_checkfd( clifd )== FALSE ) return;

    CONNECT_getCdkey( clifd, cdkey, sizeof( cdkey ));
    CONNECT_getPasswd( clifd, passwd, sizeof( passwd));

    CONNECT_getCharname( clifd, charname, sizeof( charname ));
    fdid = CONNECT_getFdid( clifd );
    unsigned long ip;
		int a,b,c,d;
    switch( CONNECT_getState( clifd ) ){
    case WHILECREATE:
        /* ��ľë�ͷ���ʧ�������е�( �������� )*/
        if( strcmp(result,FAILED) ==0 ) data ="";
        lssproto_CreateNewChar_send( clifd , result , data);
        CONNECT_setState( clifd, NOTLOGIN );
        break;

    case WHILELOGOUTSAVE:
        if( strcmp( result , SUCCESSFUL ) == 0 )
            lssproto_CharLogout_send( clifd, result , "success" );
        else
            /* �޷¡�֧����������   */
            lssproto_CharLogout_send( clifd, result , "Cannot save" );
        CONNECT_setState( clifd, NOTLOGIN );
        CONNECT_setCharaindex( clifd, -1 );
        break;

    case WHILELOSTCHARSAVE:
				ip=CONNECT_get_userip(clifd);
		    
				a=(ip % 0x100); ip=ip / 0x100;
	  		b=(ip % 0x100); ip=ip / 0x100;
	  		c=(ip % 0x100); ip=ip / 0x100;
	  		d=(ip % 0x100);
	  		char userip[32];
	  		sprintf(userip,"%d.%d.%d.%d",a,b,c,d);
        saacproto_ACCharDelete_send( acfd, cdkey,passwd,charname,"",fdid,userip );
        CONNECT_setState( clifd, WHILELOSTCHARDELETE );
        CONNECT_setCharaindex( clifd, -1 );
        break;

    case WHILECLOSEALLSOCKETSSAVE:
        CONNECT_setState( clifd, NOTLOGIN );
        // Arminius debug
        // CONNECT_endOne_debug(clifd);
        CONNECT_setUse( clifd , FALSE);
        
        SERVSTATE_decrementCloseallsocketnum();
        if( SERVSTATE_getCloseallsocketnum() == 0 ) {
		    SERVSTATE_SetAcceptMore( -1 );
            print( "��������浵\n" );
            print("\n �رշ�����ʱ�� =%d",i_shutdown_time);//ttom
            if(i_shutdown_time==1){
		    system( "�����ر�" );
			sigshutdown( 0 );
	    }		
	}
    default:
        break;
    }
}


void saacproto_ACCharDelete_recv(int fd,char* result,char* data,int retfd)
{
    int clifd = getfdFromFdid(retfd);
    if( CONNECT_checkfd( clifd ) == FALSE )return;

    switch( CONNECT_getState(clifd) ){
    case WHILELOSTCHARDELETE:
        /*  ������ئ��  */
        CONNECT_setState(clifd, NOTLOGIN );
        CONNECT_setCharaindex( clifd, -1 );
        break;
    case WHILECHARDELETE:
        /*  ��������˪��  */
        if( strcmp(result,FAILED) == 0)data="";
        lssproto_CharDelete_send( clifd, result , data );
        CONNECT_setState( clifd,  NOTLOGIN );
        CONNECT_setCharaindex( clifd, -1 );
        break;
    default:
        break;
    }
}

/*
 */
void saacproto_ACLock_recv(int fd , char* result, char* data ,int retfd)
{
    int clifd = getfdFromFdid(retfd);
    char cdkey[CDKEYLEN];
    int cindex = getCharindexFromFdid(retfd);

    if( CONNECT_checkfd(clifd) == FALSE) return;

    // Arminius 7.25 GM unlock test
    if (strncmp(data, "USRLOCKED", 9)==0) {
      char buf[4096];
      sprintf(buf, "%s", &data[10]);
      CHAR_talkToCli(cindex, -1, buf, CHAR_COLORYELLOW);
      return;
    } else if (strncmp(data, "USRUNLOCKED", 11)==0) {
      CHAR_talkToCli(cindex, -1, "User not locked.", CHAR_COLORYELLOW);
      return;
    } else if (strncmp(data, "GMUNLOCKSUCCESS", 15)==0) {
      CHAR_talkToCli(cindex, -1, "User unlock success", CHAR_COLORYELLOW);
      return;
    } else if (strncmp(data, "GMUNLOCKFAIL", 12)==0) {
      CHAR_talkToCli(cindex, -1, "User unlock failed", CHAR_COLORYELLOW);
      return;
    } else if (strncmp(data, "GMUNLOCKALL", 11)==0) {
      CHAR_talkToCli(cindex, -1, "Server unlocked", CHAR_COLORYELLOW);
      return;
    }

    CONNECT_getCdkey( clifd, cdkey , sizeof( cdkey ));
    
    switch( CONNECT_getState( clifd ) ){
    case WHILECANNOTLOGIN:
        if( strcmp( result , SUCCESSFUL ) == 0 ){
            CONNECT_setState( clifd, NOTLOGIN );

        }else{
            /*
             * ����������ëέ��
             */
            char    mesg[128];
            snprintf( mesg ,sizeof( mesg ), "%s hasn't been locked" ,cdkey);
            if( strcmp( data , mesg ) == 0 ){
                /*
                 */
                CONNECT_setState( clifd, NOTLOGIN );
            }else{
                /*
                 * ��պ������ݩ���Ʒ���������
                 */
                saacproto_ACLock_send(fd, cdkey ,
                                      UNLOCK, CONNECT_getFdid( clifd) );
            }
        }

        break;
    default:
        break;
    }
}

void saacproto_ACUCheck_recv( int fd , char *cd )
{
    int conind;
    int flag = 0;
    conind = getfdFromCdkeyWithLogin( cd );
    flag = ( conind >= 0 ) ? 1 : 0;
    saacproto_ACUCheck_send( fd , cd , flag );
}


/* ---------------------------------------------
 * --------------------------------------------*/
void saacproto_DBUpdateEntryInt_recv( int fd,char* result,
									char* table,char* key,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
		print( "failed: DBUpdateEntryInt\n");
		return;
	}
}
/* ---------------------------------------------
 * --------------------------------------------*/
void saacproto_DBUpdateEntryString_recv( int fd,char* result,
									char* table,char* key,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
			print( "failed: DBUpdateEntryString\n");
		return;
	}
}

/* ---------------------------------------------
 * --------------------------------------------*/
void saacproto_DBGetEntryRank_recv( int fd,char* result,int rank,int count,
									char* table,char* key,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
		print( "failed: DBGetEntryRank\n");
		return;
	}
	if( strcmp( table, DB_DUELPOINT) == 0 ) {
	    if( CHAR_getInt( OBJECT_getIndex( msgid2), CHAR_WHICHTYPE)	== CHAR_TYPEDUELRANKING){
			NPC_Duelranking_CallMyRanking( count, msgid, msgid2);
		}else {
			NPC_GetDuelRank( rank, msgid, msgid2);
		}
	}
}

void saacproto_DBDeleteEntryInt_recv( int fd,char* result,
									char* table,char* key,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
		print( "failed: DBDeleteEntryInt\n");
		return;
	}

}
/* ---------------------------------------------
 * --------------------------------------------*/
void saacproto_DBDeleteEntryString_recv( int fd,char* result,
									char* table,char* key,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
		print( "failed: DBDeleteEntryString\n");
		return;
	}

}


/* ---------------------------------------------
 * --------------------------------------------*/
void saacproto_DBGetEntryInt_recv( int fd,char* result,int value,
									char* table,char* key,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
		print( "failed: DBGetEntryInt\n");
		return;
	}
}
/* ---------------------------------------------
 * --------------------------------------------*/
void saacproto_DBGetEntryString_recv( int fd,char* result,char *value,
									char* table,char* key,int msgid,int msgid2 )
{
	char	cdkey[32];
	char	charaname[32];

	if( strcmp( table, DB_ADDRESSBOOK) == 0 ){
		int mode = 1;
		if( strcmp( result, NET_STRING_FAILED) == 0 ) {
			mode = 0;
		}
		makeStringFromEscaped( key);
		if( !getStringFromIndexWithDelim( key, "_", 1, cdkey, sizeof( cdkey)) ){
			return;
		}
		if( !getStringFromIndexWithDelim( key, "_", 2, charaname, sizeof( charaname)) ){
			return;
		}
		ADDRESSBOOK_DispatchMessage( cdkey, charaname, value, mode);
	}else {
		if( strcmp( result, NET_STRING_FAILED) == 0 ) {
			print( "failed: DBGetEntryString\n");
			return;
		}
	}
}

void saacproto_DBGetEntryByRank_recv( int fd,char* result,char* list,
										char* table,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
		print( "failed: DBGetEntryByRank\n");
		return;
	}
}
/* ---------------------------------------------
 * --------------------------------------------*/
void saacproto_DBGetEntryByCount_recv( int fd,char* result,char* list,
										char* table,int count_start,int msgid,int msgid2 )
{
	if( strcmp( result, NET_STRING_FAILED) == 0 ) {
		print( "failed: DBGetEntryByCount\n");
		return;
	}
	if( strcmp( table, DB_DUELPOINT) == 0 ) {
		NPC_Duelranking_PrintRanking( list, count_start, msgid, msgid2);
	}
}
#ifdef _ALLDOMAN // (���ɿ�) Syu ADD ���а�NPC
void saacproto_UpdataStele_recv( int fd , char *token)
{
	NPC_AlldomanWriteStele( token ); 
}
void saacproto_S_UpdataStele_recv( int i , char *ocdkey , char *oname , char *ncdkey , 
			  char *nname , char *title	, int level , int trns , int floor ) 
{
	print("\nSyu log �յ�Single=> %s , %s , %s , %s ",ocdkey , oname , ncdkey , nname);
	NPC_Alldoman_S_WriteStele( ocdkey , oname , ncdkey , nname , title	, level , trns , floor ) ;
}

#endif
/* ---------------------------------------------
 * Ƥ�����ƽ�ҵ������������ⰾ����Ի
 * --------------------------------------------*/
void saacproto_Broadcast_recv( int fd,char* id,char* charname,char *message )
{
	if( strstr( message, "online") == 0 ||
		strstr( message, "offline") == 0 || 
		strstr( message, "param") == 0  ||
		strstr( message, "chardelete") == 0 ){
		char buff[512];
		char escapebuf[1024];

		snprintf( buff, sizeof( buff),"%s_%s", id, charname );
		makeEscapeString( buff, escapebuf, sizeof(escapebuf));

		saacproto_DBGetEntryString_send( acfd, DB_ADDRESSBOOK, escapebuf, 0,0);
	}
}

void saacproto_Message_recv( int fd,char* id_from,char* charname_from,
							char* id_to,char* charname_to,
							char* message,int option,int mesgid )
{
	BOOL	ret;
	ret = ADDRESSBOOK_sendMessage_FromOther( id_from, charname_from, 
										id_to, charname_to,
										message, option
										);
	if( ret == TRUE) {
		saacproto_MessageAck_send( acfd, id_to, charname_to, 
							  		"successful",mesgid);
	}
}

/* ---------------------------------------------
 *	CoolFish: Family 2001/5/9
 * --------------------------------------------*/
void saacproto_ACAddFM_recv(int fd, char *result, int fmindex, int charfdid,
	int index)
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
//	print("ACAddFM_0\n");
	if (CONNECT_checkfd(clifd) == FALSE)	return;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
//	print("ACAddFM_1 clifd:%d ret:%d fmindex:%d index:%d\n",
//		clifd, ret, fmindex, index);
	ACAddFM(clifd, ret, fmindex, index);
}
void saacproto_ACJoinFM_recv(int fd, char *result, int recv, int charfdid)
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
	if (CONNECT_checkfd(clifd) == FALSE)	return;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACJoinFM(clifd, ret, recv);
}
void saacproto_ACLeaveFM_recv(int fd, char *result, int resultflag, int charfdid)
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
	if (CONNECT_checkfd(clifd) == FALSE)	return;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACLeaveFM(clifd, ret, resultflag);
	
	print(" ACLeaveFM_%d ", ret );
}
void saacproto_ACChangeFM_recv(int fd, char *result, int charfdid)
{
}
void saacproto_ACDelFM_recv(int fd, char *result, int charfdid)
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
	if (CONNECT_checkfd(clifd) == FALSE)	return;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	
	ACDelFM(clifd, ret);
	
	print(" ACDelFM_%d ", ret);
}
void saacproto_ACShowFMList_recv(int fd, char *result, int fmnum, char *data)
{
	int ret;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACShowFMList(ret, fmnum, data);
}

void saacproto_ACShowMemberList_recv(int fd, char *result, int index, int fmmemnum,
	int fmacceptflag, int fmjoinnum, char *data)
{
	int ret;
	// fmmemnum -1:�޴�����ֵ, -2:�޸��� else:��ʾ�˼�������
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACShowMemberList(ret, index, fmmemnum, fmacceptflag, fmjoinnum, data);
}
void saacproto_ACFMDetail_recv(int fd, char *result, char *data, int charfdid)
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
	if (CONNECT_checkfd(clifd) == FALSE)	return;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACFMDetail(ret, data, clifd );
}
void saacproto_ACMemberJoinFM_recv(int fd, char *result, char *data, int charfdid)
{

}
void saacproto_ACMemberLeaveFM_recv(int fd, char *result, char *data, int charfdid)
{

}
#ifdef _PERSONAL_FAME   // Arminius: �����������
void saacproto_ACFMCharLogin_recv(int fd, char *result, int index, int floor,
	int fmdp, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfame, int charfdid
#ifdef _NEW_MANOR_LAW
	,int momentum
#endif
	)
#else
void saacproto_ACFMCharLogin_recv(int fd, char *result, int index, int floor,
	int fmdp, int joinflag, int fmsetupflag, int flag, int charindex,
	int charfdid)
#endif
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
	if (CONNECT_checkfd(clifd) == FALSE)	return;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
//	print("Login fd:%d result:%s index:%d floor:%d fmdp:%d joinflag:%d setup:%d charfdid:%d\n",
//		fd, result, index, floor, fmdp, joinflag, fmsetupflag, charfdid);
#ifdef _PERSONAL_FAME   // Arminius: �����������
//        print("Login charfame:%d\n", charfame);
	ACFMCharLogin(clifd, ret, index, floor, fmdp, joinflag, fmsetupflag,
		flag, charindex, charfame
	#ifdef _NEW_MANOR_LAW
		,momentum
	#endif
		);
#else
	ACFMCharLogin(clifd, ret, index, floor, fmdp, joinflag, fmsetupflag,
		flag, charindex);
#endif
}
void saacproto_ACFMCharLogout_recv(int fd, char *result, int charfdid)
{
}
void saacproto_ACFMReadMemo_recv(int fd, char *result, int index, int num,
        int dataindex, char *data)
{
	int ret;
	// index(���� WORK ������)��num(���ϱ��������35��)
	// dataindex(���µ�������)��data(����)��
	// ע��num -1:�޴�����ֵ, -2:�޸��� else:��ʾ�˼�������
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACShowFMMemo(ret, index, num, dataindex, data);
}
void saacproto_ACFMWriteMemo_recv(int fd, char *result, int index)
{
}
void saacproto_ACFMPointList_recv(int fd, char *result, char *data)
{
	int ret;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACShowPointList(ret, data);
}


void saacproto_ACSetFMPoint_recv(int fd, char *result, int r, int charfdid)
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
	if( CONNECT_checkfd(clifd) == FALSE )return;
	if (strcmp(result, SUCCESSFUL) == 0){
		ret = 1;
	}else{
		ret = 0;
	}
	ACSetFMPoint(ret, r, clifd);

}
void saacproto_ACFixFMPoint_recv(int fd, char *result, int r)
{
}
void saacproto_ACFMAnnounce_recv(int fd, char *result, char *fmname,
	int fmindex, int index, int kindflag, char *data, int color)
{
	int ret;
	if (strcmp(result, SUCCESSFUL) == 0){
		ret = 1;
	}else{
		ret = 0;
	}
	ACFMAnnounce(ret, fmname, fmindex, index, kindflag, data, color);	
}	
#ifdef _NEW_MANOR_LAW
extern void SortManorSchedule();
#endif
void saacproto_ACShowTopFMList_recv(int fd, char *result, int kindflag, int num, char *data)
{
	int ret;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else	ret = 0;
	ACShowDpTop(ret, num, data, kindflag);
#ifdef _NEW_MANOR_LAW
	SortManorSchedule();
#endif
}
#ifdef _NEW_MANOR_LAW
extern struct  FMS_DPTOP fmdptop;
#endif
void saacproto_ACFixFMData_recv(int fd, char *result, int kindflag, char *data1,
	char *data2, int charfdid)
{
	int ret;
	int intdata;
	int clifd = getfdFromFdid(charfdid);
	int charaindex = CONNECT_getCharaindex( clifd );
	if( CONNECT_checkfd(clifd) == FALSE )return;
	
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else    ret = 0;
	
	if( !CHAR_CHECKINDEX(charaindex) )	return;
	if( kindflag == FM_FIX_FMRULE ){
		CHAR_talkToCli( charaindex , -1, "������ּ�޸ģϣˣ�", CHAR_COLORWHITE);
	}else if( kindflag == FM_FIX_FMGOLD ) {
		intdata = atoi( data1 );
		if( ret == 0 ) {
			if( atoi(data1) > 0 )	{
				CHAR_setInt( charaindex, CHAR_GOLD, CHAR_getInt( charaindex, CHAR_GOLD ) +atoi(data1) );
			}
			CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_GOLD);
			return;
		}else {
			if( atoi(data1) < 0 )	{
				CHAR_setInt( charaindex, CHAR_GOLD, CHAR_getInt( charaindex, CHAR_GOLD ) -atoi(data1) );
				CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_GOLD);
			}
			LogStone(
				atoi(data2),
				CHAR_getChar( charaindex, CHAR_NAME ),
				CHAR_getChar( charaindex, CHAR_CDKEY ),
				-intdata,
				CHAR_getInt( charaindex, CHAR_GOLD ),
				"FMBank(��������)",
				CHAR_getInt( charaindex,CHAR_FLOOR),
				CHAR_getInt( charaindex,CHAR_X ),
				CHAR_getInt( charaindex,CHAR_Y )
				);
		}
	}else if( kindflag == FM_FIX_FMLEADERCHANGE ){
		ACFMJob( clifd, ret, data1, data2 );
	}
	// CoolFish: 2001/10/03
	else if (kindflag == FM_FIX_FMADV || kindflag == FM_FIX_FMFEED
		|| kindflag == FM_FIX_FMSYNTHESIZE || kindflag == FM_FIX_FMDEALFOOD)
	{
		char	tmpbuf[1024], flag[128];
#ifdef _NEW_MANOR_LAW
		int i;
#endif
		if (kindflag == FM_FIX_FMADV)	strcpy(flag, "ADV");
		else if (kindflag == FM_FIX_FMFEED)	strcpy(flag, "FEED");
		else if (kindflag == FM_FIX_FMSYNTHESIZE)	strcpy(flag, "SYN");
		else if (kindflag == FM_FIX_FMDEALFOOD)	strcpy(flag, "FOOD");
		sprintf(tmpbuf, "%s::%d %s %s", flag,
			CHAR_getInt(charaindex, CHAR_FMINDEX),
			CHAR_getChar(charaindex, CHAR_FMNAME), data1);
		// print("ACFixFMData tmpbuf:%s\n", tmpbuf);
		LogFMPOP(tmpbuf);
#ifdef _NEW_MANOR_LAW
		for(i=0;i<FAMILY_MAXNUM;i++){
			// ��������
			if(fmdptop.fmtopid[i] == CHAR_getWorkInt(charaindex,CHAR_WORKFMINDEXI)) break;
		}
		if(i < FAMILY_MAXNUM){
			sprintf(tmpbuf, "MOMENTUM::%d %s %d",CHAR_getInt(charaindex,CHAR_FMINDEX),
				CHAR_getChar(charaindex,CHAR_FMNAME),fmdptop.fmMomentum[i]);
			LogFMPOP(tmpbuf);
		}
#endif
	}
}
void saacproto_ACFixFMPK_recv(int fd, char *result, int data, int winindex,
	int loseindex)
{
	int ret;
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else    ret = 0;
         
//        print("FixFMPK_recv result:%s data:%d winindex:%d loseindex:%d\n", result, data, winindex, loseindex);
        if (ret == 1)
        {
        	char	tmpbuf[256];
        	ACFixFMPK(winindex, loseindex, data);
        	sprintf(tmpbuf, "FMPK(����PK)::win(��ʤ����):%d lose(��ܼ���):%d point:%d", winindex, loseindex, data);
		//print("FixFMPK tmpbuf:%s\n", tmpbuf);
        	LogFMPOP(tmpbuf);
        }
}
void saacproto_ACGMFixFMData_recv(int fd, char *result, char *fmname, int charfdid)
{
	int ret;
	int clifd = getfdFromFdid(charfdid);
	char buf[256];
	if( CONNECT_checkfd(clifd) == FALSE )return;
	
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else    ret = 0;
         
        print("GMFixFMData_recv result:%s\n", result);
        
        if (ret == 1)
        {
           sprintf(buf, "�޸�%s���ϣϣˣ�", fmname);
           CHAR_talkToCli(CONNECT_getCharaindex(clifd) , -1, buf, CHAR_COLORWHITE);
        }
        else
        {
           sprintf(buf, "�޸�%s����ʧ�ܣ�", fmname);
           CHAR_talkToCli(CONNECT_getCharaindex(clifd) , -1, buf, CHAR_COLORWHITE);
        }
}
extern int familyTax[];
void saacproto_ACGetFMData_recv(int fd, char *result, int kindflag, int data,
	int charfdid)
{
	int ret;
	char buf[256];
	int clifd = getfdFromFdid(charfdid);
	if( CONNECT_checkfd(clifd) == FALSE )return;
	
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else    ret = 0;
	
	//print(" FMBank2_%d_%d_%d ", ret, kindflag, data );
	if( ret == 0 ) return;
	
	if( kindflag == 1 ) {
		int fmindex = CHAR_getWorkInt(CONNECT_getCharaindex(clifd), CHAR_WORKFMINDEXI);
		if( fmindex < 0 || fmindex >= FAMILY_MAXNUM ){
			print(" fmindex:%d if FALSE !!\n", fmindex);
			return;
		}
		familyTax[ fmindex] = data;
		sprintf( buf, "B|T|%d", data );
		lssproto_FM_send( clifd, buf );
	}
		
}
void saacproto_ACFMClearPK_recv(int fd, char *result, char *fmname, int fmindex,
        int index)
{
	int ret, i = 0;
	
	if (strcmp(result, SUCCESSFUL) == 0)	ret = 1;
	else    ret = 0;
	for (i = 0; i < MAX_SCHEDULEMAN * MAX_SCHEDULE; i++)
	{
	   {
	      if (fmpks[i].flag == -1)	continue;
	      if (fmpks[i].host_index == index
	      	 || fmpks[i].guest_index == index)
	      {
	         fmpks[i].host_index = -1;
	         strcpy(fmpks[i].host_name, "");
	         fmpks[i].guest_index = -1;
	         strcpy(fmpks[i].guest_name, "");
	         fmpks[i].prepare_time = -1;
	         fmpks[i].max_player = -1;
	         fmpks[i].win = -1;
	         fmpks[i].flag = -1;
	      }
	   }
	}
}

#ifdef _ACFMPK_LIST
void saacproto_ACSendFmPk_recv( int fd, int toindex, int flg)
{
	char buf[256];
	if( flg == 0){
		sprintf( buf, "ׯ԰��ս��¼ʧ�ܣ���");
		CHAR_talkToCli( toindex, -1, buf, CHAR_COLORRED);
	}else{
		sprintf( buf, "ׯ԰��ս��¼ȷ��OK����ú�׼����");
		CHAR_talkToCli( toindex, -1, buf, CHAR_COLORYELLOW);
	}
}

void saacproto_ACLoadFmPk_recv(int fd, char *data)
{
	char buf[10], fm_pk_data[128];
	char token[256], skip[256];
	int fm_pk_num , fmpks_pos=-1;
	easyGetTokenFromString( data , 1 , buf , sizeof(buf));
	fm_pk_num = atoi(buf);
	sprintf(fm_pk_data , "%s", data+2 );
	if( ((fmpks_pos=fm_pk_num*MAX_SCHEDULE)== -1) || (fm_pk_num > FMPOINTNUM) ){
		print("\n err fm_pk_num(%d)", fm_pk_num );
		return;
	}
	// ʱ��
	if (getStringFromIndexWithDelim(data,"|",2,token,sizeof(token))) {
		fmpks[fmpks_pos+1].dueltime=atoi(token);
	 }
	 // ���� familyindex
	 if (getStringFromIndexWithDelim(data,"|",3,token,sizeof(token))) {
		 fmpks[fmpks_pos+1].host_index=atoi(token);
	 }
	 // ���� ������
	 if (getStringFromIndexWithDelim(data,"|",4,token,sizeof(token))) {
		 strcpy(fmpks[fmpks_pos+1].host_name,makeStringFromEscaped(token));
	 }
	 // �Ͷ� familyindex
	 if (getStringFromIndexWithDelim(data,"|",5,token,sizeof(token))) {
		 fmpks[fmpks_pos+1].guest_index=atoi(token);
	 }
	 // �Ͷ� ������
	 if (getStringFromIndexWithDelim(data,"|",6,token,sizeof(token))) {
		 strcpy(fmpks[fmpks_pos+1].guest_name,makeStringFromEscaped(token));
	 }
	 // ׼��ʱ��
	 if (getStringFromIndexWithDelim(data,"|",7,token,sizeof(token))) {
		 fmpks[fmpks_pos+1].prepare_time=atoi(token);
	 }
	 // �������
	 if (getStringFromIndexWithDelim(data,"|",8,token,sizeof(token))) {
		 int maxnum = atoi( token);
#ifdef	_FMWAR_PLAYERNUM
		 if( maxnum < getFmWarPlayerNum() ) maxnum = getFmWarPlayerNum();
#else
		 if( maxnum < 50 ) maxnum = 50;	
#endif
		 fmpks[fmpks_pos+1].max_player = maxnum;
	 }
	 // ���
	 if (getStringFromIndexWithDelim(data,"|",9,token,sizeof(token))) {
		 fmpks[fmpks_pos+1].flag=atoi(token);
	 }
	 if( fmpks[fmpks_pos+1].flag == FMPKS_FLAG_MANOR_OTHERPLANET ){
		 // ��ս����
		 if (getStringFromIndexWithDelim(data,"|",10,token,sizeof(token))) {
			 strcpy(fmpks[fmpks_pos+2].host_name,makeStringFromEscaped(token));
			 strcpy(skip, makeStringFromEscaped(token));
			 if (strcmp(getGameserverID(), skip)==0) {
				 fmpks[fmpks_pos+1].flag=8;
			 }else if( fmpks[fmpks_pos+1].flag == 8) {
				 fmpks[fmpks_pos+1].flag=10;
			 }
		 }
	 }else{
		 strcpy( fmpks[fmpks_pos+2].host_name, "");
	 }
}
#endif


void saacproto_ACManorPKAck_recv(int fd, char *data)
{
  // ������� server �� ׯ԰ pk scheduleman
  int i;
  char token[256], skip[256];
  int char_max=CHAR_getCharNum();

  print("ACManorPKAck_recv: %s\n",data);

  for (i=0; i<char_max; i++) {
    if (CHAR_getCharUse(i)) {
      if (CHAR_getInt( i, CHAR_WHICHTYPE )==CHAR_TYPEMANORSCHEDULEMAN) {
        if (getStringFromIndexWithDelim(data,"|",1,token,sizeof(token))) {
          if (CHAR_getWorkInt(i, CHAR_NPCWORKINT2)==atoi(token)) {	// manor id
            int fmpks_pos = CHAR_getWorkInt(i, CHAR_NPCWORKINT1)*MAX_SCHEDULE;

            // ��ս����
            if (getStringFromIndexWithDelim(data,"|",10,token,sizeof(token))) {
              strcpy(skip, makeStringFromEscaped(token));
              print("ACManorPKAck: %d %s\n", fmpks_pos, skip);
              
              if (strcmp(getGameserverID(), skip)!=0) {
                strcpy(fmpks[fmpks_pos+2].host_name, skip);
                
                // ʱ��
                if (getStringFromIndexWithDelim(data,"|",2,token,sizeof(token))) {
                  fmpks[fmpks_pos+1].dueltime=atoi(token);
                }
                // ���� familyindex
                if (getStringFromIndexWithDelim(data,"|",3,token,sizeof(token))) {
                  fmpks[fmpks_pos+1].host_index=atoi(token);
                }
                // ���� ������
                if (getStringFromIndexWithDelim(data,"|",4,token,sizeof(token))) {
                  strcpy(fmpks[fmpks_pos+1].host_name,makeStringFromEscaped(token));
                }
                // �Ͷ� familyindex
                if (getStringFromIndexWithDelim(data,"|",5,token,sizeof(token))) {
                  fmpks[fmpks_pos+1].guest_index=atoi(token);
                }
                // �Ͷ� ������
                if (getStringFromIndexWithDelim(data,"|",6,token,sizeof(token))) {
                  strcpy(fmpks[fmpks_pos+1].guest_name,makeStringFromEscaped(token));
                }
                // ׼��ʱ��
                if (getStringFromIndexWithDelim(data,"|",7,token,sizeof(token))) {
                  fmpks[fmpks_pos+1].prepare_time=atoi(token);
                }
                // �������
                if (getStringFromIndexWithDelim(data,"|",8,token,sizeof(token))) {
                  fmpks[fmpks_pos+1].max_player=atoi(token);
                }
                // ���
                if (getStringFromIndexWithDelim(data,"|",9,token,sizeof(token))) {
                  fmpks[fmpks_pos+1].flag=atoi(token);
                }
                // ��ս����
                if (getStringFromIndexWithDelim(data,"|",10,token,sizeof(token))) {
                  strcpy(fmpks[fmpks_pos+2].host_name,makeStringFromEscaped(token));
                }
              }
            }
          }
        }
      }
    }
  }
}

#ifdef _WAEI_KICK
void saacproto_ACKick_recv(int fd , int act, char* data ,int retfd)
{
  int clifd = getfdFromFdid(retfd);
    //char cdkey[CDKEYLEN];
  int cindex = getCharindexFromFdid(retfd);
  
	//print("״̬��%s ", data);
	
	switch( act)	{
	case 0:	//FAIL
		if( CONNECT_checkfd(clifd) == FALSE) return;
		CHAR_talkToCli( cindex, -1, data, CHAR_COLORYELLOW);	
		break;
	case 1:	//����������
		{
			char cdkey[64];
			char szName[64];
			int i, MAX_USER=0;
			BOOL find=FALSE;
			
			//print("�������:%s", data);

			//MAX_USER=getFdnum();
			MAX_USER=getPlayercharnum();
			for(i=0;i<MAX_USER;i++){
				int i_use;
				//if(fd==getfdFromCharaIndex(i))continue;
				//i_use=CONNECT_getUse(i);
				if( CHAR_CHECKINDEX( i) ){
					strcpy(cdkey,CHAR_getChar(i,CHAR_CDKEY));
					if( !strcmp( cdkey, data) ){
#ifdef _OFFLINE_SYSTEM
						if(CHAR_getWorkInt( i, CHAR_WORK_OFFLINE )!=0){
							CHAR_logout(i,TRUE);
							sasql_online(cdkey,NULL,NULL,NULL,NULL,0);
							find=TRUE;
							return;
						}
#endif
						strcpy(szName,CHAR_getChar(i,CHAR_NAME));
						if( retfd == -1 ){
							print("���ظ�����");
							CHAR_talkToCli( i, -1, "���ظ��ʺŵ�������ߣ�",CHAR_COLORYELLOW);
						}else{
							print("�������� ");
							CHAR_talkToCli(i, -1, "��������Ͽ����ӡ�",CHAR_COLORYELLOW);
						}
						CONNECT_setCloseRequest( getfdFromCharaIndex(i) , 1 );
						find=TRUE;
					}
				}
			}
//			for(i=0;i<MAX_USER;i++){
//				int i_use;
//				if(fd==i)continue;
//				i_use=CONNECT_getUse(i);
//				if(i_use){
//					CONNECT_getCdkey( i, cdkey, sizeof( cdkey ));
//					if( !strcmp( cdkey, data) ){
//						int fd_charaindex = CONNECT_getCharaindex( i );
//						CONNECT_getCharname( i, szName, sizeof( szName ) );
//						if( retfd == -1 ){
//							print("���ظ�����");
//							CHAR_talkToCli( fd_charaindex, -1, "���ظ��ʺŵ�������ߣ�",CHAR_COLORYELLOW);
//						}else{
//							print("�������� ");
//							CHAR_talkToCli(fd_charaindex, -1, "��������Ͽ����ӡ�",CHAR_COLORYELLOW);
//						}
//						CONNECT_setCloseRequest( i , 1 );
//						find=TRUE;
//					}
//				}
//			}
			
//			if( find == TRUE && retfd != -1 && CHAR_CHECKINDEX( cindex) )	{
//				char buf1[256];//, buf2[256];
//				sprintf(buf1,"ϵͳ��[%s]����ŷ�����", /*CHAR_getChar( cindex, CHAR_NAME),*/ szName);
//				for( i=0; i<MAX_USER; i++){
//					if( CONNECT_getUse(i) )	{
//						int fd_charaindex = CONNECT_getCharaindex( i);
//						if( !CHAR_CHECKINDEX( fd_charaindex) ) continue;
//						CHAR_talkToCli( fd_charaindex, -1, buf1, CHAR_COLORYELLOW);
//						//						CHAR_talkToCli(fd_charaindex, -1, buf2, CHAR_COLORYELLOW);
//					}
//				}
//			} 
		}
		break;
	case 2:
		print("\n CASE 2");
		break;
	}
}

#endif


#ifdef _CHAR_POOLITEM
void saacproto_ACCharSavePoolItem_recv(int fd ,char* result, char* data,int retfd)
{
	int charaindex = getCharindexFromFdid( retfd);
	//print("\n ACCharSavePoolItem_recv:%s ", data);
	if( !CHAR_CHECKINDEX(charaindex) ) return;
	if( strstr( result, FAILED ) != NULL ){
		CHAR_talkToCli( charaindex, -1, "���ֿ߲����ϴ浵ʧ�ܣ�", CHAR_COLORYELLOW);
		return;
	}
}

void saacproto_ACCharGetPoolItem_recv(int fd ,char* result, char* data,int retfd, int meindex) 
{
	Char*   ch = NULL;
	int i, clifd, charaindex;

	//print("\n ACCharGetPoolItem_recv:%s ", data);

	if( strcmp( result , SUCCESSFUL ) != 0 ) return;
	charaindex = getCharindexFromFdid(retfd);
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	clifd = getfdFromCharaIndex( charaindex);
	if( CHAR_CheckDepotItem( charaindex) ) return;//�ֿ��Ѵ���

	if( (ch = CHAR_getCharPointer( charaindex)) == NULL ) return;
	ch->indexOfExistDepotItems = allocateMemory( sizeof( int ) * CHAR_MAXDEPOTITEMHAVE );
	if( ch->indexOfExistDepotItems == NULL ) return;
	for( i=0; i<CHAR_MAXDEPOTITEMHAVE; i++)
		ch->indexOfExistDepotItems[i] = -1;

	if( CHAR_makeDepotItemStringToChar( charaindex, data) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "���ֿ߲����϶�ȡʧ�ܣ�", CHAR_COLORYELLOW);
		CHAR_removeDepotItem( charaindex);
		return;
	}
	if( !CHAR_CHECKINDEX( meindex) ) return;
	if( clifd != -1 ) {
		char	message[1024];
		char	buf[1024];
		strcpy( message, 
			"3\n\n"
			"          ʹ�õ��ֿ߲�\n\n"
			"          ����ŵ��ߣ�\n"
			"          ��ȡ�ص��ߣ�\n"
		);
		lssproto_WN_send( clifd, WINDOW_MESSAGETYPE_SELECT, 
						WINDOW_BUTTONTYPE_CANCEL,
						311, //CHAR_WINDOWTYPE_DEPOTITEMSHOP_HANDLE,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( message, buf, sizeof(buf)) );
	}

}

#endif//������


#ifdef _CHAR_POOLPET
void saacproto_ACCharSavePoolPet_recv(int fd ,char* result, char* data,int retfd)
{
	int charaindex = getCharindexFromFdid( retfd);
	if( !CHAR_CHECKINDEX(charaindex) ) return;
	if( strstr( result, FAILED ) != NULL ){
		CHAR_talkToCli( charaindex, -1, "����ֿ����ϴ浵ʧ�ܣ�", CHAR_COLORYELLOW);
		return;

	}
}

void saacproto_ACCharGetPoolPet_recv(int fd ,char* result, char* data,int retfd, int meindex) 
{
	Char*   ch = NULL;
	int i, clifd, charaindex;

	if( strcmp( result , SUCCESSFUL ) != 0 ) return;
	charaindex = getCharindexFromFdid(retfd);
	if( !CHAR_CHECKINDEX( charaindex) ) return;
	clifd = getfdFromCharaIndex( charaindex);
	if( CHAR_CheckDepotPet( charaindex) ) return;//�ֿ��Ѵ���

	if( (ch = CHAR_getCharPointer( charaindex)) == NULL ) return;
	ch->indexOfExistDepotPets = allocateMemory( sizeof( int ) * CHAR_MAXDEPOTPETHAVE );
	if( ch->indexOfExistDepotPets == NULL ) return;
	for( i=0; i<CHAR_MAXDEPOTPETHAVE; i++)
		ch->indexOfExistDepotPets[i] = -1;

	if( CHAR_makeDepotPetStringToChar( charaindex, data) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "����ֿ����϶�ȡʧ�ܣ�", CHAR_COLORYELLOW);
		CHAR_removeDepotPet( charaindex);
		return;
	}
	if( !CHAR_CHECKINDEX( meindex) ) return;
	if( clifd != -1 ) {
		char	message[1024];
		char	buf[1024];
		strcpy( message, 
			"3\n\n"
			"	���ر�ע�⣺���𽫹��س�����빫���ֿ�\n�����������������﹫���ֿ⡻\n"
			"	         ������ ��ų��� ������\n"
			"	         ������ ȡ�س��� ������\n"
		);
		lssproto_WN_send( clifd, WINDOW_MESSAGETYPE_SELECT, 
						WINDOW_BUTTONTYPE_CANCEL,
						CHAR_WINDOWTYPE_DEPOTPETSHOP_HANDLE,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						makeEscapeString( message, buf, sizeof(buf)) );
	}

}

#endif

#ifdef _ANGEL_SUMMON
//void saacproto_ACMissionTable_recv( int fd, int num, int type, char *data, int charaindex)
void saacproto_ACMissionTable_recv( int fd, int num, int type, char *data, char* angelinfo)
{
	char msg[1024];
	
	if( type == 1 ) { // get data list
		//if( num == -1) // get all data
		{ 
			int di, ti;
			char onedata[1024], token[1024];

			if( num == -1 ) {
				print("������о����ٻ�����!!\n");
				memset( missiontable, 0, sizeof(missiontable));
			}
			else
				print("���һ������!!:%s\n", data);

			for( di =1; di <MAXMISSIONTABLE; di++) {
				if( !getStringFromIndexWithDelim( data, " ", di, onedata, sizeof(onedata)))
					break;
				if( onedata[0] == '\0' || onedata[0] == ' ') break;
				print("����%d:%s\n", di, onedata);
				getStringFromIndexWithDelim( onedata, "|", 1, token, sizeof(token));
				ti = atoi( token);
				if( ti <0 || ti >= MAXMISSIONTABLE)	continue;
				getStringFromIndexWithDelim( onedata, "|", 2, token, sizeof(token));
				strcpy( missiontable[ti].angelinfo, token);
				getStringFromIndexWithDelim( onedata, "|", 3, token, sizeof(token));
				strcpy( missiontable[ti].heroinfo, token);
				getStringFromIndexWithDelim( onedata, "|", 4, token, sizeof(token));
				missiontable[ti].mission = atoi( token);
				getStringFromIndexWithDelim( onedata, "|", 5, token, sizeof(token));
				missiontable[ti].flag = atoi( token);
				getStringFromIndexWithDelim( onedata, "|", 6, token, sizeof(token));
				missiontable[ti].time = atoi( token);
				getStringFromIndexWithDelim( onedata, "|", 7, token, sizeof(token));
				missiontable[ti].limittime = atoi( token);
			}
		}
	}
	else if( type == 2 ) { // add data return
		int angelindex =-1, i, maxChar;
		char nameinfo[128], msgbuf[1024];

		if( num == -1) {
			sprintf( msg, "��������\n");
			print( msg);
			LogAngel( msg);
			return;
		}

		sprintf( msgbuf, "�յ�AC��Ӧ(add data) num=%d nameinfo=%s ", num, angelinfo);
		print( msgbuf);
		LogAngel( msgbuf);

		maxChar = CHAR_getPlayerMaxNum();
		for( i =0; i <maxChar; i++) {
			if( !CHAR_CHECKINDEX( i) )	continue;
			getMissionNameInfo( i, nameinfo);
			if( strcmp( nameinfo, angelinfo) )	continue;
			
			angelindex = i;
			break;
		}

		if( angelindex == -1 ) {
			print("ʹ��������\n");
			return;
		}
			
		lssproto_WN_send( getfdFromCharaIndex(angelindex), WINDOW_MESSAGETYPE_ANGELMESSAGE, 
			WINDOW_BUTTONTYPE_YESNO, 
			CHAR_WINDOWTYPE_ANGEL_ASK,
			-1,
			"Ŀǰħ����Ű����������Ҫ��İ�æ��ǰ��Ѱ��������������Щħ�壬���Ƿ�Ը���æ��");
		
		CHAR_talkToCli( angelindex, -1, "���������ٻ��㡣", CHAR_COLORYELLOW);
	}
	else if( type == 3) {
		if( num <0 || num >= MAXMISSIONTABLE)	return;

		sprintf( msg, " AC��� ����:%d %s %s ", num, missiontable[num].angelinfo, missiontable[num].heroinfo);
		print( msg);
		LogAngel( msg);

		strcpy( missiontable[num].angelinfo, "");
		strcpy( missiontable[num].heroinfo, "");
		missiontable[num].mission = 0;
		missiontable[num].flag = 0;
		missiontable[num].time = 0;
		missiontable[num].limittime = 0;

	}

}

#endif

#ifdef _TEACHER_SYSTEM
void saacproto_ACCheckCharacterOnLine_recv( int acfd, int charaindex, int iOnline,char *data,int flag)
{
	switch(flag){
		case R_F_TEACHER_SYSTEM:
			CHAR_Teacher_system_View(charaindex,iOnline,data);
			break;
		default:printf("saacproto_ACCheckCharacterOnLine_recv(): error flag type!!(%d)\n",flag);
	}
}
#endif

void saacproto_ACCharLogin_recv(int fd , int clifd, int flag)
{
	if(flag==1)
		lssproto_ClientLogin_send( clifd , "ok" );
	else{
		lssproto_ClientLogin_send( clifd , "no" );
		CONNECT_endOne_debug(clifd);
	}
}

void saacproto_QuerySafePasswd_recv( int fd, char* id, int res, int charaindex )
{
	if(res == 1)
		{
			CHAR_setWorkInt( charaindex, CHAR_WORKSAFEMODE, 1);
			CHAR_talkToCli( charaindex, -1, "������İ�ȫ����ȷ��ϵͳ�ɹ�����������", CHAR_COLORRED);
		}
	if(res == 2)
		{
			CHAR_talkToCli( charaindex, -1, "������İ�ȫ�����ϵͳ�޷�����������", CHAR_COLORRED);
		}
	if(res == 3)
		{
			CHAR_talkToCli( charaindex, -1, "����û�����ð�ȫ�룬�뾡�����ã�", CHAR_COLORRED);
			CHAR_AddSafePassword(charaindex);
		}
	if(res == 4)
		{
			CHAR_setWorkInt( charaindex, CHAR_WORKSAFEMODE, 1);
			CHAR_talkToCli( charaindex, -1, "������İ�ȫ����ȷ��ϵͳ�ɹ�����������", CHAR_COLORRED);
			CHAR_talkToCli( charaindex, -1, "������ʾ1�������˺źͰ�ȫ�����ƣ�Ϊ�������˺Ű�ȫ���뾡���޸İ�ȫ�룡", CHAR_COLORRED);
			CHAR_talkToCli( charaindex, -1, "������ʾ2��Ϊȷ�������˺Ű�ȫ����δ�޸İ�ȫ��ǰ���޷�����ɾ�����������", CHAR_COLORRED);
		}
}

void saacproto_AddSafePasswd_recv( int fd, char* id, int res, int charaindex )
{
	if(res == 1)
		{
			CHAR_talkToCli( charaindex, -1, "���Ѿ��ɹ����õİ�ȫ�룡", CHAR_COLORRED);
		}
}

void saacproto_PointCardSucces_recv( int fd, char* id, int res, int point, int charaindex )
{
	char msgtmp[256];
	if(res > 0)
		{
			sprintf(msgtmp, "%d��Ա���ֵ�ɹ��������ڵĻ�Ա����Ϊ��%d",res,point);
			CHAR_talkToCli( charaindex, -1, msgtmp, CHAR_COLORYELLOW);
		}
}

void saacproto_PointCardErr_recv( int fd, char* id, int res, int charaindex )
{
	if(res == -1)
		{
			CHAR_talkToCli( charaindex, -1, "�ó�ֵ���Ѿ���ʹ�ã������ظ�ʹ�ã�лл��", CHAR_COLORRED);
		}
	else if(res == -2)
		{
			CHAR_talkToCli( charaindex, -1, "��ֵ��������д������������д��", CHAR_COLORRED);
		}
}

void saacproto_LoginTime_recv( int fd, char* id, char* res, int charaindex )
{
	char timemsg[256];
	sprintf(timemsg,"���ϴε�½ʱ��Ϊ��%s",res);
	CHAR_talkToCli( charaindex, -1, timemsg, CHAR_COLORYELLOW);

}

#ifdef _ONLINE_SHOP
void saacproto_ACShop_recv( int fd, int charaindex, int shopnum, int vippoint, int piece, int pieces, int shopflag, char* res )
{
	int clifd = getfdFromCharaIndex( charaindex);
	lssproto_SHOP_send(clifd,shopnum,vippoint,piece,pieces,shopflag,res);

}

void saacproto_ACShopBuy_recv( int charaindex, int shopflag, int shopno, int buynum )
{
	if(shopflag==1)
	{
	int	ret;
	char msgbuf[64], buf1[8], buf2[10];
	int	enemynum;
	int	i,j,k;
	int petindex, petindex2;
	for( k =1 ; k<=buynum ; k++ )
	{
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( charaindex, i);
		if( petindex == -1  )
			break;
	}

  if( i == CHAR_MAXPETHAVE )      {
		snprintf( msgbuf,sizeof( msgbuf), "������������");
		CHAR_talkToCli( charaindex, -1, msgbuf,  CHAR_COLORYELLOW);
		return;
	}

	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == shopno) {
				break;
			}
	}

	if( i == enemynum )
		return;

	ret = ENEMY_createPetFromEnemyIndex( charaindex, i);
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ )	{
		if( CHAR_getCharPet( charaindex, i ) == ret )
			break;
	}
	if( i == CHAR_MAXPETHAVE )
		CHAR_talkToCli( charaindex, -1, "��ȡ����ʧ�ܣ�", CHAR_COLORYELLOW);

	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
		CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
	}
	
	petindex2 = CHAR_getCharPet(charaindex, i);
	if( !CHAR_CHECKINDEX( petindex2) )
		return;
	snprintf( msgbuf,sizeof( msgbuf), "�õ�%s��", CHAR_getChar(petindex2,CHAR_NAME));
	CHAR_talkToCli( charaindex, -1, msgbuf,  CHAR_COLORYELLOW);
	for(j = 0; j < CHAR_MAXPETHAVE; j++){
		petindex = CHAR_getCharPet(charaindex, j);
		if( !CHAR_CHECKINDEX( petindex) )
			continue;
		CHAR_complianceParameter( petindex );
		snprintf( msgbuf, sizeof( msgbuf ), "K%d", j );
		CHAR_sendStatusString( charaindex, msgbuf );
		snprintf( msgbuf, sizeof( msgbuf ), "W%d", j );
		CHAR_sendStatusString( charaindex, msgbuf );
	}
	}
	}
	else
	{

  int emptyitemindexinchara, itemindex;
	int	i;
	char	msgbuf[128];

	if( buynum <= 0 ) buynum = 1;

	{
		for( i = 0; i < buynum; i ++ ) {
	    emptyitemindexinchara = CHAR_findEmptyItemBox( charaindex );

		if( emptyitemindexinchara < 0 ){
			sprintf( msgbuf, "��Ʒ��λ���㡣" );

			CHAR_talkToCli( charaindex, -1, msgbuf, CHAR_COLORYELLOW);

			return;
		}

		itemindex = ITEM_makeItemAndRegist( shopno );

	    if( itemindex != -1 ){
	        CHAR_setItemIndex( charaindex, emptyitemindexinchara, itemindex );
	        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
	        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charaindex);
	       	CHAR_sendItemDataOne( charaindex, emptyitemindexinchara);
			snprintf( msgbuf, sizeof( msgbuf), "�õ�%s��",
										ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( charaindex, -1,msgbuf, CHAR_COLORYELLOW);
			}
		}
	}
	}
}
#endif

#ifdef _QUESTION_ONLINE
void saacproto_ACQuestion_recv(char* res)
{
	if( getStringFromIndexWithDelim( res, ";", 1, question, sizeof( question)) == FALSE )
		return;
	if( getStringFromIndexWithDelim( res, ";", 2, answer, sizeof( answer)) == FALSE )
		return;
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep);
	timep = mktime(p);
	questionstarttime = timep;
	int playnum = CHAR_getPlayerMaxNum();
	int i;
	char questionres[256];
	sprintf(questionres,"��������\n���⣺%s\n����������𰸣�",question);
	for(i=0;i<playnum;i++)
	{
	if( CHAR_getCharUse(i) != FALSE ) {
		if(CHAR_getWorkInt(i,CHAR_WORKQUESTIONFLG)!=1){
			int fd = getfdFromCharaIndex( i);
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
					WINDOW_BUTTONTYPE_OKCANCEL,
					CHAR_WINDOWTYPE_QUESTIONONLINE,
					-1,
					questionres);
				}
			}
	}
}
#endif

#ifdef _ALL_ITEM
void saacproto_ACAllnum_recv(int flag,int allnum)
{
	if(flag == 1) allnum1 = allnum;
	else if(flag == 2) allnum2 = allnum;
	else if(flag == 3) allnum3 = allnum;
	else if(flag == 4) allnum4 = allnum;
	else if(flag == 5) allnum5 = allnum;
}
#endif

#ifdef _SQL_BUY_FUNC
void saacproto_OnlineBuy_recv(int fd, char *data)
{
  char buf[64];
  int charaindex = CONNECT_getCharaindex(fd);
  char  token[256];
  if(data[0]=='0'){
    int  ret;
    int  enemynum;
    int  enemyid;
    int  i;
    int vital,str,tough,dex;
    getStringFromIndexWithDelim(data,"|", 2, buf, sizeof(buf));
    enemyid=atoi(buf);
    getStringFromIndexWithDelim(data,"|", 3, buf, sizeof(buf));
    vital=atoi(buf);
    getStringFromIndexWithDelim(data,"|", 4, buf, sizeof(buf));
    str=atoi(buf);
    getStringFromIndexWithDelim(data,"|", 5, buf, sizeof(buf));
    tough=atoi(buf);
    getStringFromIndexWithDelim(data,"|", 6, buf, sizeof(buf));
    dex=atoi(buf);
    
    enemynum = ENEMY_getEnemyNum();
    for( i = 0; i < enemynum; i ++ ){
      if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
        break;
      }
    }
    if( i == enemynum ){
      CHAR_talkToCli( charaindex, -1, "�˳��ﲻ���ڣ����뱾������Ա��ϵ��", CHAR_COLORGREEN);
      return;
    }
  
    ret = ENEMY_createPetFromEnemyIndex(charaindex, i);
    if( !CHAR_CHECKINDEX( ret)){
      CHAR_talkToCli( charaindex, -1, "��ȡ����ʧ��", CHAR_COLORGREEN);
      return;
    }
    snprintf( token,sizeof( token), "��ȡ���� %s ��",ENEMY_getChar( i, ENEMY_NAME));
    CHAR_talkToCli( charaindex, -1, token,  CHAR_COLORGREEN);
    for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
      if( CHAR_getCharPet( charaindex, i ) == ret )break;
    }
    if( i == CHAR_MAXPETHAVE ){
      CHAR_talkToCli( charaindex, -1, "���ĳ���������", CHAR_COLORGREEN);
      return;
    }
  
    if( CHAR_CHECKINDEX( ret ) == TRUE ){
      if(vital > 0)
        CHAR_setInt( ret, CHAR_VITAL, vital);
      if(str > 0)
        CHAR_setInt( ret, CHAR_STR, str * 80);
      if(tough > 0)
        CHAR_setInt( ret, CHAR_TOUGH, tough * 80);
      if(dex > 0)
        CHAR_setInt( ret, CHAR_DEX, dex * 100);
        
      CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
      CHAR_setInt(ret, CHAR_VARIABLEAI, 10000);
    }

    CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
    CHAR_complianceParameter( ret );
    snprintf( token, sizeof( token ), "K%d", i );
    CHAR_sendStatusString( charaindex, token );
    snprintf( token, sizeof( token ), "W%d", i );
    CHAR_sendStatusString( charaindex, token );
    
    
  }else if(data[0]=='1'){
    int emptyitemindexinchara, itemindex;
    int itemid;
    getStringFromIndexWithDelim(data,"|", 2, buf, sizeof(buf));
    itemid = atoi( buf);
    emptyitemindexinchara = CHAR_findEmptyItemBox( charaindex );
    
    if( emptyitemindexinchara < 0 ){
      CHAR_talkToCli( charaindex, -1, "������Ʒ������", CHAR_COLORYELLOW);
      return;
    }
  
    itemindex = ITEM_makeItemAndRegist( itemid );
      
    if( itemindex != -1 ){
          CHAR_setItemIndex( charaindex, emptyitemindexinchara, itemindex );
          ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
          ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charaindex);
           CHAR_sendItemDataOne( charaindex, emptyitemindexinchara);

      snprintf( token, sizeof( token), "��ȡ��Ʒ %s", ITEM_getChar( itemindex, ITEM_NAME));
      CHAR_talkToCli( charaindex, -1,token, CHAR_COLORGREEN);
    }else{
      CHAR_talkToCli( charaindex, -1, "����Ʒ�����ڣ����뱾������Ա��ϵ��", CHAR_COLORYELLOW);
    }
    
  }else if(data[0]=='2'){
    getStringFromIndexWithDelim(data,"|", 2, buf, sizeof(buf));
    int gold = atoi(buf);
    CHAR_setInt( charaindex , CHAR_GOLD , CHAR_getInt( charaindex , CHAR_GOLD ) + gold);
    CHAR_complianceParameter( charaindex );
    CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_GOLD);
    sprintf( token, "��ȡ %d ʯ�ң�", gold);
    CHAR_talkToCli( charaindex, -1,token, CHAR_COLORGREEN );
  }else{
    CHAR_talkToCli( charaindex, -1, data, CHAR_COLORGREEN);
  }
}
#endif

void saacproto_ACFmPkOnlineSn_recv(int manorindex,char* gsname, int flag)
{
	if(flag == 1){
		sprintf(manorname[manorindex],"%s",gsname);
	}else{
		sprintf(manorname[manorindex],"\O");
	}
	manorflag[manorindex] = flag;
}

void saacproto_ACSystemAllTalk_recv(char* data)
{
	if(strlen(data)<1) return;
	int playnum = CHAR_getPlayerMaxNum();
	int i;
	for(i=0;i<playnum;i++){
		if( !CHAR_CHECKINDEX( i) )
			continue;
		CHAR_talkToCli( i, -1,data, CHAR_COLORCYAN );
	}
}

void saacproto_ACPlayerAllTalk_recv(char* servername,char* playername,char* data)
{
	if(strlen(data)<1) return;
	char alltalkbuf1[256];
	char alltalkbuf2[512];
	sprintf(alltalkbuf1,"�šš� ����%s��ȫ�������� �šš�",servername);
	sprintf(alltalkbuf2,"<������>%s˵��%s",playername,data);
	int playnum = CHAR_getPlayerMaxNum();
	int i;
	for(i=0;i<playnum;i++){
		if( !CHAR_CHECKINDEX( i) )
			continue;
		CHAR_talkToCliExt(i,-1,alltalkbuf1,CHAR_COLORCYAN,18);
		CHAR_talkToCli( i, -1,alltalkbuf2, CHAR_COLORGREEN );
	}
}
