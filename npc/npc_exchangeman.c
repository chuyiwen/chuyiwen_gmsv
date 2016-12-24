#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "net.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "npc_exchangeman.h"
#include "npc_eventaction.h"
#include "log.h"
#include "battle.h"
#include "handletime.h"
#include "enemy.h"
#include "npc_warp.h"

int year,mon,date,day,hour,min,sec;
#ifdef _ALL_ITEM
extern int allnum1,allnum2,allnum3,allnum4,allnum5;
extern char hanzibuf[5000][8];
#endif
#ifdef _ANGEL_SUMMON
extern struct MissionInfo missionlist[MAXMISSION];
extern struct MissionTable missiontable[MAXMISSIONTABLE];
#endif
#ifdef _TRANS_7_NPC
BOOL NPC_EventTRANS(int meindex, int talker, char *buff2,int mode);
#endif
static void NPC_ExChangeMan_selectWindow( int meindex, int talker,int num);
BOOL NPC_TypeCheck(int meindex,int talker,char *szMes);
int NPC_ExChangeManEventCheck( int meindex, int talker, char *buff1);
BOOL NPC_EventItemCheck(int meindex,int talker,int itemNo,int flg);
BOOL NPC_EventLevelCheck(int meindex,int talker,int level,int flg);
BOOL NPC_EventVipCheck(int meindex,int talker,int vip,int flg);
BOOL NPC_EventSuperCheck(int meindex,int talker,int vip,int flg);
BOOL NPC_EventVipPointCheck(int meindex,int talker,int vippoint,int flg);
BOOL NPC_EventNpcTimeCheck(int meindex,int talker,int npctime,int flg);
#ifdef _EV_NUM
BOOL NPC_EventEvNumCheck(int meindex,int talker,int evnum,int flg);
#endif
// Arminius 8.14 move to .h (for pet talk)
//BOOL NPC_EventBigSmallCheck(int meindex,int talker,char* buf);
BOOL NPC_EventFreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg);
BOOL NPC_EventBigSmallLastCheck(int point1,int mypoint,int flg);
BOOL NPC_ENDEventNoCheck(int meindex,int talker,int shiftbit,int flg);
BOOL NPC_NOWEventNoCheck(int meindex,int talker,int shiftbit,int flg);
BOOL NPC_TiemCheck(int meindex,int talker,int time,int flg);

#ifdef _CAX_NPCTIME_IF
/*CCIS_WXscript add 09/02/21 ��� Сʱ*/
BOOL NPC_TiemCheckWithHour(int meindex,int talker,int time,int flg);
/*CCIS_WXscript add 09/02/21 ��� ���ڼ�*/
BOOL NPC_TiemCheckWithDay(int meindex,int talker,int time,int flg);
#endif

BOOL NPC_EventAddPet(int meindex, int talker, char *buff2,int mode);
BOOL NPC_RandItemGet(int meindex, int talker,int rand_j,char *buf);

BOOL NPC_PetLvCheckType2(int petindex,int meindex,int talker,char *buf,int mode);
BOOL NPC_PetLvCheck(int meindex,int talker,char *buf,int mode);
#ifdef	_NPC_PETSKILLUP
BOOL NPC_PetSkillCheck(int meindex,int talker,char *buf,int mode);
#endif
void NPC_RequestMain(int meindex,int talker,char *buf);
void NPC_AcceptMain(int meindex,int  talker ,char*buf);
void NPC_MsgDisp(int meindex,int talker,int num);
BOOL NPC_EventAdd(int meindex,int talker,int mode);
BOOL NPC_AcceptDel(int meindex,int talker,int mode);
BOOL NPC_SavePointCheck(int meindex,int talker,int shiftbit,int flg);

BOOL NPC_EventReduce(int meindex,int talker,char *buf);
BOOL NPC_EventDelItem(int meindex,int talker,char *buf,int breakflg);
BOOL NPC_EventDelItemEVDEL(int meindex,int talker,char *buf,char *nbuf,int breakflg);
BOOL NPC_EventAddItem(int meindex,int talker,char *buf);
int NPC_EventFile(int meindex,int talker,char *buf);
BOOL NPC_EventDelPet(int  meindex,int  talker, int petsel);
void NPC_PetSkillMakeStr(int meindex,int toindex,int select);
void NPC_CleanMain(int meindex,int talker,char *buf);	
void NPC_EventPetSkill( int meindex, int talker, char *data);
BOOL NPC_ItemFullCheck(int meindex,int talker,char *buf,int mode,int evcnt);
void NPC_CharmStatus(int meindex,int talker);
BOOL NPC_ImageCheck(int meindex,int talker,int kosuu,int flg);
int NPC_EventGetCost(int meindex,int talker,char *arg);

BOOL NPC_EventWarpNpc(int meindex,char *buf);

enum {
	CHAR_WORK_EVENTWARP	= CHAR_NPCWORKINT4,

};

/*********************************
* ������
*********************************/
BOOL NPC_ExChangeManInit( int meindex )
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024*2];
	int i = 1;
	char buf4[256];

	CHAR_setWorkInt(meindex,CHAR_WORK_EVENTWARP,1);

	/*--NPC��������ë����������--*/
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEEVENT );


	/*--ɬ�ð����̻�ë  ��  ��--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL) {
		print("NPC_ExChange.c TypeCheck: GetArgStrErr\n");
		print("NPCName=%s\n", CHAR_getChar( meindex ,CHAR_NAME));
		return FALSE;
	}

	i = 1;
	/*--��ì����NPC������ʧ��  ةë������������--*/
	/*--����������  ��  ���ƻ������¾�����ʸ����������ƥ˪ľ�¾���--*/
	while(getStringFromIndexWithDelim( argstr ,"EventEnd" ,i ,buf ,sizeof(buf))
	!= FALSE)
	{
		i++;
		/*--���ƥ̤��ľ�����¼����������߼���ë����-*/
		if(strstr( buf,"GetItem") != NULL) {
			if(strstr( buf,"ItemFullMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### EventNo=%d %s:ItemFullMsg ����###",
				atoi( buf4),
				CHAR_getChar( meindex, CHAR_NAME));
			}
		}

		if(strstr( buf,"GetPet") != NULL) {
			if(strstr( buf,"PetFullMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### ventNo=%d %s:PetFullMsg ���� ###",
				atoi(buf4),
				CHAR_getChar(meindex,CHAR_NAME));
			}
		}

		if(strstr( buf,"GetEgg") != NULL) {
			if(strstr( buf,"PetFullMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### ventNo=%d %s:PetFullMsg ���� ###",
				atoi(buf4),
				CHAR_getChar(meindex,CHAR_NAME));
			}
		}

		if(strstr( buf,"GetStone") != NULL){
			if(strstr( buf,"StoneFullMsg") == NULL){
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### EventNo=%d %s:StonFullMsg ���� ###",
				atoi( buf4),
				CHAR_getChar( meindex, CHAR_NAME));
			}
		}

		if(strstr( buf,"DelStone") != NULL) {
			if(strstr( buf,"StoneLessMsg") == NULL) {
				NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",buf4,sizeof(buf4));
				print("\n### EventNo=%d %s:StoneLessMsg ���� ###",
				atoi(buf4),
				CHAR_getChar(meindex,CHAR_NAME));
			}
		}
	}

	i = 1;

	return TRUE;

}

/*--      ���  --*/
BOOL NPC_EventWarpNpc(int meindex,char *buf)
{
	char token[32];
	char token2[32];
	char buf2[32];
	int floor,x,y,x1,x2,y1,y2;
	int ret;
	int cnt;
	int dcnt = 1;

	/*--�ػ��������󼰴�  --*/
	cnt = CHAR_getWorkInt(meindex,CHAR_WORK_EVENTWARP);
	while(getStringFromIndexWithDelim(buf,",", dcnt,buf2,sizeof(buf2) )
	 !=FALSE ){
		dcnt++;
	}
	//cnt = RAND(1,dcnt);
	if(dcnt <= cnt) cnt=1;

	while(getStringFromIndexWithDelim(buf,",", cnt,buf2,sizeof(buf2)) !=FALSE )
	{
		cnt++;
	    ret=getStringFromIndexWithDelim(buf2,".", 1,token,sizeof(token));
        if( ret == FALSE ) {
        	 continue;
       	}
        floor = atoi( token );

        ret=getStringFromIndexWithDelim(buf2,".", 2,token,sizeof(token));
        if( ret == FALSE ){
          continue;
        }
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
        //x = atoi( token );

        ret=getStringFromIndexWithDelim(buf2,".", 3,token,sizeof(token));
        if( ret == FALSE ) {
         continue;
        }
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
        //y = atoi( token );
		//print("\nfloor=%d,x=%d,y=%d\n",floor,x,y);
		CHAR_setWorkInt(meindex,CHAR_WORK_EVENTWARP, cnt);
        CHAR_warpToSpecificPoint(meindex, floor, x, y);
		return TRUE;
	}
	
	return FALSE;

}


/*********************************
*   �ƾ�����ľ���ݼ���
*********************************/
void NPC_ExChangeManTalked( int meindex , int talkerindex , char *szMes ,int color )
{
	/* �����������帲�ƻ�����  ɱ���� */
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	/*--  �������¾���������--*/
	if(NPC_Util_isFaceToFace( meindex ,talkerindex ,2) == FALSE) {
		/* ���������  ���� */
		if(NPC_Util_isFaceToChara(talkerindex,meindex,1 ) == FALSE) return;
	}
	
	/*--������������������    ë�����--*/
	CHAR_setWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANT, 0);
	CHAR_setWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANTSEC, 0);
	CHAR_setWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANTTRD, 0);

#ifdef	_NPC_RAND_MSG	
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("NPC_ExChange.c TypeCheck: GetArgStrErr\n");
		print("NPCName=%s\n", CHAR_getChar(meindex,CHAR_NAME));
		return;
	}
	if(strstr(argstr,"STARTRAND")!=NULL){
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
					sprintf(errbuf,"������%d��ſ��Լ����˲�����",getEditBaseTime()*randwrong-(timep - randtime));
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
//			sprintf(errbuf,"������%d��ſ��Լ����˲�����",getEditBaseTime()-(timep-CHAR_getWorkInt(talkerindex,CHAR_WORKRANDEDITBASETIME)));
//			CHAR_talkToCli(talkerindex, -1, errbuf, CHAR_COLORRED);
//			return;
//		}
		char jiabuf[][5]={"��","��","��","��","��","ʮ","��"};
		char jianbuf[][5]={"��","һ","-","��","-","��","��"};
		char chengbuf[][5]={"��","*","x","X","��","��","��"};
		char numbuf[][5]={"��","��","��","��","��","��","��","��","��","��"};
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
		if(randtype==1){
			if(m==1){
				typebuf=jiabuf[l-1];
				k = i+j;
			}else{
				typebuf=jianbuf[l-1];
				k = i-j;
			}
			char ibuf[10],jbuf[10];
			if(i<10){
				sprintf(ibuf,"%s",numbuf[i]);
			}else{
				int ia,ib;
				ia = i/10;
				ib = i - ia*10;
				sprintf(ibuf,"%s",numbuf[ia]);
				strcat(ibuf,numbuf[ib]);
			}
			if(j<10){
				sprintf(jbuf,"%s",numbuf[j]);
			}else{
				int ja,jb;
				ja = j/10;
				jb = j - ja*10;
				sprintf(jbuf,"%s",numbuf[ja]);
				strcat(jbuf,numbuf[jb]);
			}
			sprintf(buf,"%d|%d|%s",k,meindex,szMes);
			CHAR_setWorkChar(talkerindex,CHAR_WORKRANDMSG,buf);
			int fd = getfdFromCharaIndex( talkerindex );
			sprintf(buf,"���������������ʽ�Ľ��(%s%s%s=?)��",ibuf,typebuf,jbuf);
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
				WINDOW_BUTTONTYPE_OK,
				CHAR_WINDOWTYPE_NPC_RAND1_MSG,
				-1,
				buf);
		}else if(randtype==2){
			if(m==1){
				typebuf=jiabuf[l-1];
				k = i+j;
			}else if(m==2){
				typebuf=jianbuf[l-1];
				k = i-j;
			}else{
				typebuf=chengbuf[l-1];
				k = i*j;
			}
			char ibuf[10],jbuf[10];
			if(i<10){
				sprintf(ibuf,"%s",numbuf[i]);
			}else{
				int ia,ib;
				ia = i/10;
				ib = i - ia*10;
				sprintf(ibuf,"%s",numbuf[ia]);
				strcat(ibuf,numbuf[ib]);
			}
			if(j<10){
				sprintf(jbuf,"%s",numbuf[j]);
			}else{
				int ja,jb;
				ja = j/10;
				jb = j - ja*10;
				sprintf(jbuf,"%s",numbuf[ja]);
				strcat(jbuf,numbuf[jb]);
			}
			int n;
			int randnum1;
			char tempret[5][32];
			randnum1 = RAND(1,5);
			int randnumbuf[4];
			int tmpi=0;
			while(tmpi<100){
				randnumbuf[0] = RAND(1,100);
				randnumbuf[1] = RAND(1,100);
				randnumbuf[2] = RAND(1,100);
				randnumbuf[3] = RAND(1,100);
				if(randnumbuf[0]!=randnumbuf[1]!=randnumbuf[2]!=randnumbuf[3]!=k){
					break;
				}
				tmpi++;
			}
			tmpi=0;
			for(n=1;n<=5;n++){
				if(n==randnum1){
					sprintf(tempret[n-1],"%d",k);
				}else if(tmpi<4){
					sprintf(tempret[n-1],"%d",randnumbuf[tmpi]);
					tmpi++;
				}
			}
			sprintf(buf,"%d|%d|%s",randnum1,meindex,szMes);
			CHAR_setWorkChar(talkerindex, CHAR_WORKRANDMSG, buf);
			int fd = getfdFromCharaIndex( talkerindex );
			sprintf(buf,"3\n��ѡ�����������ʽ�Ľ��(%s%s%s=?)��\n \n \nѡ��һ��%s\nѡ�����%s\nѡ������%s\nѡ���ģ�%s\nѡ���壺%s\n",ibuf,typebuf,jbuf,tempret[0],tempret[1],tempret[2],tempret[3],tempret[4]);
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
				WINDOW_BUTTONTYPE_OK,
				CHAR_WINDOWTYPE_NPC_RAND2_MSG,
				-1,
				buf);
		}else if(randtype==3){
			sprintf(arg,"%s",sasql_rand_buf());
			if(strcmp(arg,"err")==0) return;
			char tempbuf[64];
			char randquestion[64];
			char randrightanswer[64];
			if(getStringFromIndexWithDelim(arg,"|", 1, tempbuf, sizeof(tempbuf)) == FALSE)
				return;
			strcpy(randquestion,tempbuf);
			if(getStringFromIndexWithDelim(arg,"|", 2, tempbuf, sizeof(tempbuf)) == FALSE)
				return;
			strcpy(randrightanswer,tempbuf);
			sprintf(buf,"%s|%d|%s",randrightanswer,meindex,szMes);
			CHAR_setWorkChar(talkerindex,CHAR_WORKRANDMSG,buf);
			int fd = getfdFromCharaIndex( talkerindex );
			sprintf(buf,"%s:",randquestion);
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
				WINDOW_BUTTONTYPE_OK,
				CHAR_WINDOWTYPE_NPC_RAND3_MSG,
				-1,
				buf);
		}else{
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
					strcat(randquestion,"�����ҳ���ͬ���飩");
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
				sprintf(randquestion,"[%s]�����ҳ���ͬӢ�ģ�",tempbuf);
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
							sprintf(tempret[n-1],"ѡ��( %d %s )",rightnum,randrightanswer);
						else
							sprintf(tempret[n-1],"ѡ��( %s %d )",randrightanswer,rightnum);
					}else{
						if(m==1){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"ѡ��( %d %s )",wrongnum1,randwronganswer1);
							else
								sprintf(tempret[n-1],"ѡ��( %s %d )",randwronganswer1,wrongnum1);
							m = 2;
						}else if(m==2){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"ѡ��( %d %s )",wrongnum2,randwronganswer2);
							else
								sprintf(tempret[n-1],"ѡ��( %s %d )",randwronganswer2,wrongnum2);
							m = 3;
						}else if(m==3){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"ѡ��( %d %s )",wrongnum3,randwronganswer3);
							else
								sprintf(tempret[n-1],"ѡ��( %s %d )",randwronganswer3,wrongnum3);
							m = 4;
						}else if(m==4){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"ѡ��( %d %s )",wrongnum4,randwronganswer4);
							else
								sprintf(tempret[n-1],"ѡ��( %s %d )",randwronganswer4,wrongnum4);
							m = 5;
						}
					}
				}
			int windowtype = 0;
				int selecttype = RAND(1,100);
				if(selecttype<=30){
					sprintf(buf,"%d|%d|%s",rightnum,meindex,szMes);
					windowtype = WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT;
				}else{
					sprintf(buf,"%d|%d|%s",randnum1,meindex,szMes);
					windowtype = WINDOW_MESSAGETYPE_SELECT;
				}
			CHAR_setWorkChar(talkerindex, CHAR_WORKRANDMSG, buf);
			int fd = getfdFromCharaIndex( talkerindex );
			sprintf(buf,"%s\n%s\n%s\n%s\n%s\n%s\n������ȷ�������ڵ�5λ���ֻ��ߵ�ѡ��",randquestion,tempret[0],tempret[1],tempret[2],tempret[3],tempret[4]);
			lssproto_WN_send( fd, windowtype, 
				WINDOW_BUTTONTYPE_OK,
				CHAR_WINDOWTYPE_NPC_RAND4_MSG,
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
#endif

	/*--������  ��������--*/
	NPC_TypeCheck( meindex, talkerindex, szMes);

}

/*------------------------------------------------------------
 *ɬ�ð����̻�ë  ��  �ģ�������ؤ������������  ë����
 *-------------------------------------------------------------*/
BOOL NPC_TypeCheck(int meindex,int talker,char *szMes)
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024*2];
	char buf2[512];
	int i = 1;
	int EvNo;
	int evch;
	int q=1;
	
	/*--ɬ�ð����̻�ë  ��  ��--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("NPC_ExChange.c TypeCheck: GetArgStrErr\n");
		print("NPCName=%s\n", CHAR_getChar(meindex,CHAR_NAME));
		return FALSE;
	}

	/*-- ɬ�ð����̻�ë������� --*/
	while(getStringFromIndexWithDelim( argstr,"EventEnd",i,buf,sizeof( buf))
	!= FALSE){
		i++;
		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT, 0);
		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);

		/*-- ��ì�����ϼ����ë������ --*/
		if(NPC_Util_GetStrFromStrWithDelim(buf,"EventNo",
								buf2, sizeof( buf2) ) == NULL) continue;

		if(strstr( buf2, "-") != 0) {
			EvNo = -1;
		}else{
			if(strstr(buf2,",") != NULL){
				char buf3[512];
				while(getStringFromIndexWithDelim( buf2, ",", q, buf3, sizeof( buf3))
				!= FALSE) {
					q++;
				}
				q--;
				q = rand()%q + 1;
				getStringFromIndexWithDelim( buf2, ",", q, buf3, sizeof( buf3));
				EvNo = atoi( buf3);
			}else{
				EvNo = atoi( buf2);
			}
		}

		/*-- �׷º���������ë�浤����ƥ����ì��������  �ƻ������������ݳ� --*/
		if(NPC_EventCheckFlg( talker, EvNo) == TRUE) continue;

		/*--ƽ����������������ë�浤--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"KeyWord", buf2,sizeof( buf2) ) 
		 != NULL){
			if(strcmp( szMes, buf2) != 0) continue;
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANT, i);
		}

		/*--ʸ������  󡼰�������ͻ�  �Ȼ��¾�ë��������--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"Pet_Name", buf2,sizeof( buf2)) != NULL){
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 1);
		}
		evch = NPC_ExChangeManEventCheck( meindex, talker, buf);
		if(evch != -1) {
			if(NPC_Util_GetStrFromStrWithDelim( buf,"TYPE", buf2, sizeof( buf2)) != NULL){
				
				if( strstr( buf2, "REQUEST") != NULL) {
					NPC_RequestMain( meindex, talker, buf);
					return TRUE;
				}else if( strstr( buf2, "ACCEPT") != NULL) {
					/*-��  ë��������  --*/
					NPC_AcceptMain( meindex, talker, buf);
					return TRUE;
					
				}else if(strstr( buf2, "MESSAGE") != NULL) {
					char token[14*100];
					char token2[14*100];
					if(NPC_Util_GetStrFromStrWithDelim( buf,"NomalMsg",	token,sizeof( token) )
						!= NULL ) {
						if(NPC_AcceptDel( meindex, talker, 0) == FALSE) return FALSE;
						if(EvNo != -1)
							NPC_NowEventSetFlg( talker, EvNo);
						CHAR_talkToCli( talker, meindex, token, CHAR_COLORWHITE);
						if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp", token2,sizeof( token2) )
							!= NULL){
							NPC_EventWarpNpc(meindex,token2);
						}
						if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", buf2, sizeof( buf2) )
							!=NULL){
							char buf3[16];
							int k = 1;
							if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
							while(getStringFromIndexWithDelim(buf2 , "," ,k, buf3, sizeof(buf3))
								!=FALSE ){	
								k++;
								NPC_EventSetFlg( talker, atoi( buf3));
							}
						}
						if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
							buf2, sizeof( buf2)) != NULL) {
							char buf3[16];
							int k = 1;
							k= 1 ;
							while(getStringFromIndexWithDelim(buf2 , "," , k, 
								buf3, sizeof(buf3))
								!= FALSE ){
								NPC_EndEventSetFlgCls( talker, atoi(buf3));
								k++;
							}
						}
#ifdef _ANGEL_SUMMON
						// ����ٻ�����
						if( NPC_Util_GetStrFromStrWithDelim( buf, "MISSIONOVER",
							buf2, sizeof( buf2)) != NULL)
						{
							int mindex;
							int mission;
							char nameinfo[64];
							
							mission = atoi( buf2);
							mindex = checkIfAngel(talker);
							
							if( mission == missiontable[mindex].mission )
							{
								char msg[1024];
								
								print(" ====����ٻ�����==== ");

								CHAR_setInt( talker, CHAR_HEROCNT, 
									CHAR_getInt( talker, CHAR_HEROCNT)+1 );
								getMissionNameInfo( talker, nameinfo);
								saacproto_ACMissionTable_send( acfd, MISSION_HERO_COMPLETE, 4, nameinfo, "");
								sprintf( msg, " ������� i:%d m:%d %s ", mindex, mission, nameinfo);
								print( msg);
								LogAngel( msg );
							}
							
						}
						
						// ����ٻ�����
						if( NPC_Util_GetStrFromStrWithDelim( buf, "MISSIONCLEAN",
							buf2, sizeof( buf2)) != NULL)
						{
							int mindex;
							int mission;
							char nameinfo[64];
							
							mission = atoi( buf2);
							mindex = checkIfAngel( talker);
							
							if( mission == missiontable[mindex].mission )
							{
								char msg[1024];

								getMissionNameInfo( talker, nameinfo);
								saacproto_ACMissionTable_send( acfd, mindex, 3, nameinfo, "");

								sprintf( msg, " �������� i:%d m:%d %s ", mindex, mission, nameinfo);
								print( msg);
								LogAngel( msg );
							}
							
						}
#endif

					}else if(NPC_Util_GetStrFromStrWithDelim(buf, "NomalWindowMsg",
						token, sizeof( token) )
						!=NULL) {
						/*--���̼�����ƥ����������ë߯��--*/
						int fd = getfdFromCharaIndex( talker);
#ifdef _NEWEVENT
						if(NPC_Util_GetStrFromStrWithDelim(buf, "NomalWindowMsg1",
							buf2, sizeof(buf2)) != NULL){
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
						}else {
#endif
							char token2[14*100];
							if(NPC_AcceptDel( meindex, talker,0) == FALSE) return FALSE;
							if( EvNo != -1) NPC_NowEventSetFlg( talker, EvNo);
							
							if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
								token2,sizeof( token2) ) != NULL){
								NPC_EventWarpNpc(meindex,token2);
							}
							if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
								buf2, sizeof( buf2) ) !=NULL){
								char buf3[16];
								int k = 1;
								if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
								while(getStringFromIndexWithDelim(buf2 , "," ,k, 
									buf3, sizeof(buf3)) !=FALSE ){	
									k++;
									NPC_EventSetFlg( talker, atoi( buf3));
								}
							}
							//��ì�����׷º�ë����
							if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
								buf2, sizeof( buf2)) != NULL) {
								char buf3[16];
								int k = 1;
								
								k= 1 ;
								while(getStringFromIndexWithDelim(buf2 , "," , k, 
									buf3, sizeof(buf3))	!= FALSE ){
									NPC_EndEventSetFlgCls( talker, atoi(buf3));
									k++;
								}
							}
#ifdef _ANGEL_SUMMON
							// ����ٻ�����
							if( NPC_Util_GetStrFromStrWithDelim( buf, "MISSIONOVER",
								buf2, sizeof( buf2)) != NULL)
							{
								int mindex;
								int mission;
								char nameinfo[64];
								
								mission = atoi( buf2);
								mindex = checkIfAngel( talker);
								
								if( mission == missiontable[mindex].mission )
								{
									char msg[1024];
									
									print(" ====����ٻ�����==== ");

									CHAR_setInt( talker, CHAR_HEROCNT, 
										CHAR_getInt( talker, CHAR_HEROCNT)+1 );
									getMissionNameInfo( talker, nameinfo);
									saacproto_ACMissionTable_send( acfd, MISSION_HERO_COMPLETE, 4, nameinfo, "");
									sprintf( msg, " ������� i:%d m:%d %s ", mindex, mission, nameinfo);
									print( msg);
									LogAngel( msg );
								}
								
							}
							
							// ����ٻ�����
							if( NPC_Util_GetStrFromStrWithDelim( buf, "MISSIONCLEAN",
								buf2, sizeof( buf2)) != NULL)
							{
								int mindex;
								int mission;
								char nameinfo[64];
								
								mission = atoi( buf2);
								mindex = checkIfAngel( talker);
								
								if( mission == missiontable[mindex].mission )
								{
									print(" ====����ٻ�����==== ");
									
									getMissionNameInfo( talker, nameinfo);
									saacproto_ACMissionTable_send( acfd, mindex, 3, nameinfo, "");
								}
								
							}
#endif
							
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_YES,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
#ifdef _NEWEVENT
						}
#endif
					}
					
					return TRUE;
					
				}else if(strstr( buf2, "CLEAN") != NULL) {
					NPC_CleanMain( meindex, talker, buf);
					return TRUE;
				}
			}
		}
	}

	strcpy( buf, "\0");
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "NomalMainMsg", buf, sizeof( buf)) == NULL )	{
		print("\n err:NOT FIND NomalMainMsg");
		return FALSE;
	}
	i = 1;
	while(getStringFromIndexWithDelim( buf, ",", i, buf2, sizeof( buf2))!= FALSE){
		i++;
	}
	i--;
	i = rand()%i + 1;
	getStringFromIndexWithDelim( buf,",", i, buf2, sizeof( buf2));
	CHAR_talkToCli( talker, meindex, buf2,  CHAR_COLORWHITE);
	return TRUE;
}


/*-----------------------------------
 *������  ����  ë������
 ------------------------------------*/
static void NPC_ExChangeMan_selectWindow( int meindex, int talker,int num)
{

	switch( num) {
		/*--���;޵�����--*/
		  case 0:
		  /*-��ì����  ئ��ƥ����ì����ë�����¾�������ë  ��--*/
		  	NPC_MsgDisp( meindex, talker, 0);
			break;

		  case 1:
		  /*--ޥ�����ì�����浤--*/
	  		NPC_MsgDisp( meindex, talker, 5);
		  	break;

			/*--ʧ�ͱ�������--*/
		  case 2:
			/*--������Ի--*/
			NPC_MsgDisp( meindex, talker, 6);
			break;

		  case 3:
			/*--������Ի--*/
			NPC_MsgDisp( meindex, talker, 11);
			break;
	}

}

/*-------------------------------------
 *��ì����ë��  ������  �����̼�
 --------------------------------------*/
void NPC_RequestMain(int meindex,int talker,char *buf)
{
	int shiftbit;
	char buf2[128];
	int q=1;
	/*--��ì�����ϼ����ë������--*/
	NPC_Util_GetStrFromStrWithDelim( buf,"EventNo", buf2,sizeof( buf2) );
				if(strstr(buf2,",") != NULL){
				char buf3[128];
				while(getStringFromIndexWithDelim( buf2, ",", q, buf3, sizeof( buf3))
				!= FALSE) {
					q++;
				}
				q--;
				q = rand()%q + 1;
				getStringFromIndexWithDelim( buf2, ",", q, buf3, sizeof( buf3));
				shiftbit = atoi( buf3);
			}else{
				shiftbit = atoi( buf2);
			}

	if(NPC_NowEventCheckFlg( talker, shiftbit) == TRUE) {
		NPC_ExChangeMan_selectWindow( meindex, talker, 0);
	}else{
		NPC_ExChangeMan_selectWindow( meindex, talker, 1);
	}
}

/*---------------------------------------
 * ��ì������  �����̼�
 ----------------------------------------*/
void NPC_AcceptMain(int meindex,int  talker ,char *buf)
{

	NPC_ExChangeMan_selectWindow( meindex, talker, 2);
}

/*----------------------------------------
 *  �׷º�ë����ʧ���¿м����̼�
 ----------------------------------------*/
void NPC_CleanMain(int meindex,int talker,char *buf)
{
	NPC_ExChangeMan_selectWindow( meindex, talker, 3);
}

/*-----------------------------------------
�ͷ���ʧ��������߯�Ȼ������ݱ���̫���ľ�£�
-------------------------------------------*/
void NPC_ExChangeManWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	/*--       ������Ի����ئ�����巴��   --*/
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}


	switch( seqno) {

		/*--�׷º�ë�����ݼ����̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_CLEANMSG:
		if( select == WINDOW_BUTTONTYPE_YES) {
			NPC_MsgDisp( meindex, talkerindex, 12);
		}else if(select == WINDOW_BUTTONTYPE_NO) {
		}
		break;

#ifdef _NEWEVENT
	  case CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG:
		if(select == WINDOW_BUTTONTYPE_NEXT) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD) + 1);
			NPC_MsgDisp( meindex, talkerindex, 99);
		}else if(select == WINDOW_BUTTONTYPE_PREV) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD) - 1);
			NPC_MsgDisp( meindex, talkerindex, 99);
		}
		break;
#else
	  case CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG:
		break;
#endif
		/*--���;޵�����ؤԻ���絤����������--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK:
			if(select == WINDOW_BUTTONTYPE_NEXT) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
				NPC_MsgDisp( meindex, talkerindex, 22);
			}else if(select == WINDOW_BUTTONTYPE_PREV) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
				NPC_MsgDisp( meindex, talkerindex, 22);
			}
		break;
		
		/*--ʧ�ͱ�������ؤԻ���絤����������--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK:
			if(select == WINDOW_BUTTONTYPE_NEXT) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
				NPC_MsgDisp( meindex, talkerindex, 82);
			}else if(select == WINDOW_BUTTONTYPE_PREV) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
				NPC_MsgDisp( meindex, talkerindex, 82);
			}
		break;

		/*--�ػ�����ì����ë�����¾�--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_NOWEVENT:
		if(select == WINDOW_BUTTONTYPE_YES) {
			NPC_MsgDisp( meindex, talkerindex, 7);
		}else if(select == WINDOW_BUTTONTYPE_NO){
			NPC_MsgDisp( meindex, talkerindex, 1);
		}
		break;
		
		/*--���;޵��������̼����̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG:
		if(select == WINDOW_BUTTONTYPE_YES) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,0);
			NPC_MsgDisp( meindex, talkerindex, 2);
		}else if(select == WINDOW_BUTTONTYPE_NO) {
		
		}else if(select == WINDOW_BUTTONTYPE_NEXT) {
			 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
						CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
			NPC_MsgDisp( meindex, talkerindex, 55);
		}else if(select == WINDOW_BUTTONTYPE_PREV) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
			NPC_MsgDisp( meindex, talkerindex, 55);
		}
		break;

		/*--������Ի�����̼������̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG:
		if(select == WINDOW_BUTTONTYPE_YES) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,0);
#ifdef _NEWEVENT
			NPC_MsgDisp( meindex, talkerindex, 81);
#else
			NPC_MsgDisp( meindex, talkerindex, 8);
#endif
		}else if(select == WINDOW_BUTTONTYPE_NO) {
		}else if(select == WINDOW_BUTTONTYPE_NEXT) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
			NPC_MsgDisp( meindex, talkerindex, 66);
		}else if(select == WINDOW_BUTTONTYPE_PREV) {
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
				CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
			NPC_MsgDisp( meindex, talkerindex, 66);
		}
		break;

#ifdef _NEWEVENT
	  case CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW:
		  if(select == WINDOW_BUTTONTYPE_OK){
			CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,0);
				NPC_MsgDisp( meindex, talkerindex, 8);
		  }else if(select == WINDOW_BUTTONTYPE_NEXT) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)+1);
				NPC_MsgDisp( meindex, talkerindex, 81);
		  }else if(select == WINDOW_BUTTONTYPE_PREV) {
				 CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD,
					CHAR_getWorkInt(talkerindex,CHAR_WORKSHOPRELEVANTTRD)-1);
				NPC_MsgDisp( meindex, talkerindex, 81);
		  }
		  break;
#endif
		/*--ʸ������  ëܸ�������̼�����--*/
	  case CHAR_WINDOWTYPE_WINDOWPETSKILLSHOP:
			if(CHAR_getWorkInt( talkerindex ,CHAR_WORKSHOPRELEVANTTRD) ==0) {
				NPC_EventPetSkill( meindex, talkerindex, data);
				NPC_MsgDisp( meindex, talkerindex, 25);
			}else{
				NPC_EventPetSkill( meindex, talkerindex, data);
				NPC_MsgDisp( meindex, talkerindex, 88);
			}
		break;
	}
}


/*-------------------------
 *  ����ë�����ƻ����¾�
 --------------------------*/
int NPC_ExChangeManEventCheck( int meindex, int talker, char *buff1)
{
	char buff2[512];
	char buff3[128];
	char buf[512];
	int i = 1,j = 1;
	int loop = 0;
	
	NPC_Util_GetStrFromStrWithDelim( buff1, "EVENT", buf, sizeof( buf));

	/*-- EVENt������ë��  ��έ������ --*/
	while( getStringFromIndexWithDelim(buf, ",", i, buff2,sizeof( buff2))
	 !=FALSE )
	{
		i++;
		/*-- "&"��ؤ�¼�ƥ  ������ --*/
		if(strstr( buff2, "&")!=NULL){
			j = 1;
			loop = 0;
			while(getStringFromIndexWithDelim(buff2, "&", j, buff3, sizeof( buff3))
			 != FALSE )
			{
				j++;
				if(NPC_EventBigSmallCheck( meindex, talker, buff3) == FALSE)
				{
					loop = 1;
					break;
				}
			}
				
			if(loop == 0){
				i--;
				return i;
			}
		/*--���������繴--*/
		}else{
			if(NPC_EventBigSmallCheck( meindex, talker, buff2) == TRUE) {
				i--;
				return i;
			}
		}
	
	}

	return -1;

}

/*---------------------------------
 *   ����į��������
 --------------------------------*/
BOOL NPC_EventBigSmallCheck(int meindex,int talker, char* buf)
{

	char buff2[128];
	int kosuu;
	
	/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
	if(strstr( buf, "PET") != NULL) {
		if(strstr( buf, "EV") != NULL) {
			if(NPC_PetLvCheck( meindex, talker, buf, 1) != FALSE) return TRUE;
		}else{
			if(NPC_PetLvCheck( meindex, talker, buf, 0) != FALSE) return TRUE;
		}
		return FALSE;
	}
#ifdef	_NPC_PETSKILLUP	
	if(strstr( buf, "PSKILL") != NULL) {
		if(strstr( buf, "EV") != NULL) {
			if(NPC_PetSkillCheck( meindex, talker, buf, 1) != FALSE) return TRUE;
		}else{
			if(NPC_PetSkillCheck( meindex, talker, buf, 0) != FALSE) return TRUE;
		}
		return FALSE;
	}
#endif
	/*--��Ʊ����  ɧ����į�������� --*/
	if(strstr( buf, "<") != NULL) {
		getStringFromIndexWithDelim( buf, "<", 2, buff2, sizeof( buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "<", 1, buff2, sizeof( buff2));

		if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 1) == TRUE) {
			return TRUE;
		}

	}else if(strstr( buf, ">") != NULL) {
		getStringFromIndexWithDelim( buf, ">", 2, buff2, sizeof(buff2));
		kosuu = atoi(buff2);
		getStringFromIndexWithDelim( buf, ">" ,1, buff2, sizeof(buff2));

		if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 2)==TRUE) {
			return TRUE;
		}

	}else if(strstr( buf, "!=" ) != NULL) {
		getStringFromIndexWithDelim( buf, "!=", 2, buff2, sizeof( buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "!=", 1, buff2, sizeof( buff2));
		if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 3) == TRUE) {
			return TRUE;
		}
		
	}else if(strstr( buf, "=") != NULL) {
		getStringFromIndexWithDelim( buf, "=", 2, buff2, sizeof( buff2));
		kosuu = atoi( buff2);
		getStringFromIndexWithDelim( buf, "=", 1, buff2, sizeof( buff2));

		if(strstr( buf, "*") != NULL) {
			if(NPC_EventReduce( meindex, talker, buf) == TRUE){
				return TRUE;
			}
		}else if(NPC_EventFreeIfCheck( meindex, talker, buff2, kosuu, 0)
		== TRUE)
		{
			return TRUE;
		}
	}
	
	return FALSE;

}


/*-----------------------------------
 * ��  ë  ���ƻ����¾�����������
 *-----------------------------------*/
BOOL NPC_EventFreeIfCheck(int meindex,int talker,char* buf,int kosuu,int flg)
{

	/*--��ì����������--*/
	if(strcmp( buf, "LV") == 0) {
		if(NPC_EventLevelCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	
	if(strcmp( buf, "AMPOINT") == 0) {
		if(NPC_EventAmPointCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	
	if(strcmp( buf, "FAME") == 0) {
		if(NPC_EventFameCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
#ifndef _CAX_NPCTIME_OFF	
	if(strcmp( buf, "YEAR") == 0) {
		if(NPC_EventYearCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	if(strcmp( buf, "MON") == 0) {
		if(NPC_EventMonCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	if(strcmp( buf, "DATE") == 0) {
		if(NPC_EventDateCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	if(strcmp( buf, "DAY") == 0) {
		if(NPC_EventDayCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	if(strcmp( buf, "HOUR") == 0) {
		if(NPC_EventHourCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	
	if(strcmp( buf, "MIN") == 0) {
		if(NPC_EventMinCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	
	if(strcmp( buf, "SEC") == 0) {
		if(NPC_EventSecCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
#endif

	if(strcmp(buf,"VIP")==0){
		if(NPC_EventVipCheck(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	
	if(strcmp(buf,"SUPER")==0){
		if(NPC_EventSuperCheck(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	
	if(strcmp(buf,"VIPPOINT")==0){
		if(NPC_EventVipPointCheck(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	
	if(strcmp(buf,"NPCTIME")==0){
		if(NPC_EventNpcTimeCheck(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}

#ifdef _EV_NUM
	if(strcmp(buf,"EVNUM")==0){
		if(NPC_EventEvNumCheck(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	if(strcmp(buf,"EVTHINGS")==0){
		int evitem;
		evitem = CHAR_getInt(talker,CHAR_EVITEM);
			if(evitem>0){
				if(NPC_EventItemCheck(meindex,talker,evitem,flg)==TRUE)
					return TRUE;
			}else{
				return TRUE;
			}
	}
	if(strcmp(buf,"EVCODE")==0){
		if(NPC_EventEvCodeCheck(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
#endif

#ifdef _ALL_ITEM
	if(strcmp(buf,"ALLNUM1")==0){
		if(NPC_EventAllNum1Check(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	if(strcmp(buf,"ALLNUM2")==0){
		if(NPC_EventAllNum2Check(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	if(strcmp(buf,"ALLNUM3")==0){
		if(NPC_EventAllNum3Check(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	if(strcmp(buf,"ALLNUM4")==0){
		if(NPC_EventAllNum4Check(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	if(strcmp(buf,"ALLNUM5")==0){
		if(NPC_EventAllNum5Check(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
#endif
	
	if(strcmp(buf,"EQUIT")==0){
		if(NPC_EventEquitCheck(meindex,talker,kosuu,flg)==TRUE)
			return TRUE;
	}
	
	/*--ʧ��  ة��������--*/
	if(strcmp( buf, "ITEM") == 0) {
		if(NPC_EventItemCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	/*--��  ��ì������������--*/
	if(strcmp( buf, "ENDEV") == 0) {
		if(NPC_ENDEventNoCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	if(strcmp( buf, "NOWEV") == 0) {
		if(NPC_NOWEventNoCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	if(strcmp( buf, "SP" ) == 0) {
		if(NPC_SavePointCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

	if(strcmp( buf, "TIME") == 0) {
		if(NPC_TiemCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
#ifdef _CAX_NPCTIME_IF
/*CCIS_WXscript add 09/02/21 ENENT CHECK */
	if(strcmp( buf, "HOUR") == 0) {
		if(NPC_TiemCheckWithHour( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
	if(strcmp( buf, "DAY") == 0) {
		if(NPC_TiemCheckWithDay ( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}
/*CCIS_WXscript add 09/02/21 ENENT CHECK */
#endif

#ifdef _PROFESSION_SKILL			// WON ADD ����ְҵ����
	if(strcmp(buf,"CLASS")==0){
		if(NPC_ActionClassCheck(meindex,talker,kosuu,flg)==TRUE)
				return TRUE;
	}
#endif

	if(strcmp( buf, "IMAGE") == 0) {
		if(NPC_ImageCheck( meindex, talker, kosuu, flg) == TRUE) {
			return TRUE;
		}
	}

#ifdef _ANGEL_SUMMON
	if(strcmp(buf,"ANGEL_NOW")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_DOING, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_NOW")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_DOING, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_OVER")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_OVER")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_OUT")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_TIMEOVER, 0)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_OUT")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_TIMEOVER, 0)==TRUE)
				return TRUE;
	}

	if(strcmp(buf,"ANGEL_I_NOW")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_DOING, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_I_NOW")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_DOING, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_I_OVER")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_I_OVER")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_HERO_COMPLETE, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"ANGEL_I_OUT")==0){
		if( NPC_ActionMissionAngelCheck(meindex,talker,kosuu, MISSION_TIMEOVER, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp(buf,"HERO_I_OUT")==0){
		if( NPC_ActionMissionHeroCheck(meindex,talker,kosuu, MISSION_TIMEOVER, TRUE)==TRUE)
				return TRUE;
	}
	if(strcmp( buf, "HEROCNT") == 0) {
		//if(NPC_EventLevelCheck( meindex, talker, kosuu, flg) == TRUE) {
		if(NPC_HeroCompleteCountCheck( meindex, talker, kosuu, flg) == TRUE)
			return TRUE;
	}

#endif

	if(strcmp(buf, "SKUP")==0){
			int myskup;
			myskup = CHAR_getInt(talker,CHAR_SKILLUPPOINT);
			if(NPC_EventBigSmallLastCheck( kosuu, myskup, flg) == TRUE)
				return TRUE;
	}
#ifdef _NPC_EXCHANGEMANTRANS
	//�ж�ת��
	{
		//print("\n�ж�ת��");
		if(strcmp(buf,"TRANS")==0){
			int mytrans;
			mytrans = CHAR_getInt(talker,CHAR_TRANSMIGRATION);
			if(NPC_EventBigSmallLastCheck( kosuu, mytrans, flg) == TRUE)
				return TRUE;
		}
	}
#endif
#ifdef _PROSK99
	if(strstr( buf, "PROSK" ) != NULL) {//PROSK99>16 ְ���ȼ���춵��99%������Ҫ����16��
		char *p=NULL;
		int i,level=0,count=0;
		CHAR_HaveSkill *pSkil;
		if(  strstr( buf, "PROSK" ) ){
			p = buf;
			level = atoi(p+5);
		}else 
			level = 100;
		for( i = 0; i < CHAR_SKILLMAXHAVE; i ++ ){
			if( CHAR_getCharHaveSkill( talker, i ) ){
				pSkil = CHAR_getCharHaveSkill( talker, i );
				if( SKILL_getInt( &pSkil->skill, SKILL_LEVEL) >= level )
					++count;
			}
		}	
		if(NPC_EventBigSmallLastCheck( kosuu, count, flg) == TRUE)
			return TRUE;
	}
#endif

#ifdef _ADD_reITEM
	if(strstr( buf, "reITEM" ) != NULL) {
		int i,count=0,itemindex=-1;
		for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex = CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX( itemindex) )
				++count;
		}
		if(NPC_EventBigSmallLastCheck( kosuu, count, flg) == TRUE)
			return TRUE;
	}
#endif

	return FALSE;

}

/*--------------------------------
 *  ʸ��������ì��ë������������
 ---------------------------------*/
BOOL NPC_PetLvCheck(int meindex,int talker,char *buf,int mode)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[16];
	char buf3[8];
	int petno;
	int baseno;
	int petlevel = 0;
	int flg=0;
	int mypetlevel = 0;
	int i = 0;
	int petindex;
	char name[512];
	int cnt = 0;
	int loop;
	int pettran=-1;   //�жϳ���ת��
	

	/*--϶�ý�ľ��ʸ�������ϼ����  ID)ë��  --*/
	getStringFromIndexWithDelim( buf, "-", 2, buff2, sizeof( buff2));
	if(strstr(buff2,"*") != NULL) {
		getStringFromIndexWithDelim( buff2, "*", 1, buf3, sizeof( buf3));
		petno = atoi( buf3);
		getStringFromIndexWithDelim( buff2, "*", 2, buf3, sizeof( buf3));
		loop = atoi( buf3);

	}else{
		petno = atoi( buff2);
		loop = 1;

	}
	
	if(getStringFromIndexWithDelim( buf, "-", 3, buff2, sizeof( buff2)))  //�жϳ���ת��
		{
			pettran = atoi( buff2);
		}
		
	/*--ʸ����  �����л����¾�����������--*/
	for(i=0; i < CHAR_MAXPETHAVE ; i++) {
		if(loop == cnt) return TRUE;
		
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  continue;
		if(pettran != -1)              //�жϳ���ת��
			{
				if(pettran!=CHAR_getInt( petindex, CHAR_TRANSMIGRATION))
					continue;
			}
	
		/*--ʸ������IDë��  �ƻ�����--*/
		baseno = CHAR_getInt( petindex, CHAR_PETID);

		/* ʸ������ID����ػ���ƹ����ëƩ���� */
		if( (baseno == petno) && (CHAR_getInt( petindex, CHAR_ENDEVENT)
		 == mode))
		 {
			/*--��ì��ë������������--*/
			mypetlevel = CHAR_getInt( petindex, CHAR_LV);
			getStringFromIndexWithDelim( buf, "-", 1, buff2, sizeof( buff2));

			if(strstr( buf, "<") != NULL) {
				flg = 1;
				getStringFromIndexWithDelim( buff2, "<", 2, buf3, sizeof(buf3));
			}else if(strstr( buf, ">") != NULL) {
				getStringFromIndexWithDelim( buff2, ">", 2, buf3, sizeof(buf3));
				flg = 2;
			}else if(strstr( buf, "=") != NULL) {
				getStringFromIndexWithDelim( buff2, "=", 2, buf3, sizeof(buf3));
				flg = 0;
			}

			petlevel = atoi( buf3);

			if(NPC_EventBigSmallLastCheck( petlevel, mypetlevel, flg) == TRUE) {
				/*--ʸ������  󡻥ɬ�ý�ľ��������  󡼰��������  ��--*/
				if(CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC)==1) {
					char *usename;
					
					/*--ɬ�ð����̻Ｐ  ��  ��--*/
					if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
						print("PetLevel:GetArgStrErr");
						return FALSE;
					}

					usename = CHAR_getUseName( petindex);
					NPC_Util_GetStrFromStrWithDelim( argstr, "Pet_Name",
													name,sizeof( name));
	
					if(strcmp( name, usename) == 0)
					{
						cnt++;
						continue;
					}
				}else {
					CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
					cnt++;
					continue;
				}
			}
		}
	}
	
	if(loop == cnt) return TRUE;
	if(i == CHAR_MAXPETHAVE) return FALSE;

	return FALSE;
}


/*--------------------------------
 *  ʸ��������ì��ë������������
 ---------------------------------*/
int NPC_PetLvCheckType2(int petindex,int meindex,int talker,char *buf,int mode)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[16];
	char buf3[8];
	int petno;
	int baseno;
	int petlevel = 0;
	int flg = 0;
	int mypetlevel = 0;
	char name[512];
	
	/*--϶�ý�ľ��ʸ�������ϼ����  ID)ë��  --*/
	getStringFromIndexWithDelim( buf, "-", 2, buff2, sizeof( buff2));
	if(strstr(buff2,"*") != NULL) {
		getStringFromIndexWithDelim( buff2, "*", 1, buf3, sizeof( buf3));
		petno = atoi( buf3);
	}else{
		petno = atoi( buff2);
	}

	
	/*--ʸ������IDë��  �ƻ�����--*/
	baseno = CHAR_getInt( petindex, CHAR_PETID);

	/*--ʸ������ID����ػ���ƹ����ëƩ����--*/
	if( (baseno == petno) && (CHAR_getInt( petindex, CHAR_ENDEVENT)
	 == mode))
	{
		/*--��ì��ë������������--*/
		mypetlevel=CHAR_getInt( petindex, CHAR_LV);
		getStringFromIndexWithDelim( buf, "-", 1,buff2,sizeof(buff2));

		if(strstr( buf, "<") != NULL) {
			flg = 1;
			getStringFromIndexWithDelim( buff2, "<", 2,
										 buf3, sizeof(buf3));
		}else if(strstr( buf, ">") != NULL) {
			getStringFromIndexWithDelim( buff2, ">", 2, buf3, sizeof(buf3));
			flg = 2;
		}else if(strstr( buf, "=") != NULL) {
			getStringFromIndexWithDelim( buff2, "=", 2, buf3, sizeof(buf3));
			flg = 0;
		}
		
		petlevel = atoi(buf3);

		if(NPC_EventBigSmallLastCheck( petlevel, mypetlevel, flg)
		 == TRUE)
		{
			/*--ʸ������  󡻥ɬ�ý�ľ��������  󡼰��������  ��--*/
			if(CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTSEC) == 1) {
				char *usename;

				/*--ɬ�ð����̻Ｐ  ��  ��--*/
				if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
					print("PetLevel:GetArgStrErr");
					return FALSE;
				}
				
				usename = CHAR_getUseName( petindex);
				NPC_Util_GetStrFromStrWithDelim(argstr,"Pet_Name",
												name, sizeof( name));
	
				if(strcmp( name, usename) == 0)
				{
					return TRUE;
				}
			}else {
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
				return TRUE;
			}
		}
	}

	return FALSE;

}

#ifdef	_NPC_PETSKILLUP
BOOL NPC_PetSkillCheck(int meindex,int talker,char *buf,int mode)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[64];
	char buf3[32];
	char buf4[32];
	int petskill;
	int npcskill;
	int skillno;
	int petlevel = 0;
	int flg=0;
	int mypetlevel = 0;
	int i = 0;
	int petindex;
	char name[512];
	int cnt = 0;
	int loop;
	

	/*--϶�ý�ľ��ʸ�������ϼ����  ID)ë��  --*/
	if(strstr( buf, "<") != NULL) {
			flg = 1;
			getStringFromIndexWithDelim( buf, "<", 2,
										 buff2, sizeof(buff2));
		}else if(strstr( buf, ">") != NULL) {
			getStringFromIndexWithDelim( buf, ">", 2, buff2, sizeof(buff2));
			flg = 2;
		}else if(strstr( buf, "=") != NULL) {
			getStringFromIndexWithDelim( buf, "=", 2, buff2, sizeof(buff2));
			flg = 0;
		}
	getStringFromIndexWithDelim( buff2, "-", 1, buf3, sizeof( buf3));
	petlevel = atoi(buf3);
	getStringFromIndexWithDelim( buff2, "-", 2, buf3, sizeof( buf3));
	skillno = atoi( buf3)-1;
	getStringFromIndexWithDelim( buff2, "-", 3, buf3, sizeof( buf3));
	getStringFromIndexWithDelim( buf3, "*", 1, buf4, sizeof( buf4));
	npcskill = atoi( buf4);
	getStringFromIndexWithDelim( buf3, "*", 2, buf4, sizeof( buf4));
	loop = atoi( buf4);
		
	/*--ʸ����  �����л����¾�����������--*/
	for(i=0; i < CHAR_MAXPETHAVE ; i++) {
		//if(loop == cnt) return TRUE;
		
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  continue;
		CHAR_setWorkInt(petindex,CHAR_WORKPETSKILLUP,0);
		if(loop==cnt)continue;
		/*--ʸ������IDë��  �ƻ�����--*/

		/* ʸ������ID����ػ���ƹ����ëƩ���� */
		if(CHAR_getInt( petindex, CHAR_ENDEVENT)== mode)
		 {
			/*--��ì��ë������������--*/
			mypetlevel = CHAR_getInt( petindex, CHAR_LV);

			if(NPC_EventBigSmallLastCheck( petlevel, mypetlevel, flg) == TRUE) {
				if( !CHAR_CHECKPETSKILLINDEX( skillno)){
	 				continue; 
				}
				if(CHAR_getPetSkill( petindex, skillno)!=npcskill){
					continue;
				} 
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
				CHAR_setWorkInt(petindex,CHAR_WORKPETSKILLUP,1);
				cnt++;
				continue;
			}
		}
	}
	
	if(loop <= cnt) return TRUE;
	if(i == CHAR_MAXPETHAVE) return FALSE;

	return FALSE;
}
#endif

/*------------------------
 *��  ������������
 --------------------------*/
BOOL NPC_SavePointCheck(int meindex,int talker,int shiftbit,int flg)
{
	int point;

	point = CHAR_getInt( talker, CHAR_SAVEPOINT);
	
	if( (point & ( 1<< shiftbit))  == ( 1<< shiftbit)) {
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;
}

/*------------------------
 *�̶����⼰��������
 --------------------------*/
BOOL NPC_ImageCheck(int meindex,int talker,int kosuu,int flg)
{
	int image;

	image = CHAR_getInt( talker, CHAR_IMAGETYPE);
	
	if(NPC_EventBigSmallLastCheck( image, kosuu, flg) == TRUE) {
		return TRUE;
	}
	return FALSE;

}


/*-----------------------------------------
 * ���޼���������ë�浤(ؤ����������)
 ------------------------------------------*/
BOOL NPC_TiemCheck(int meindex,int talker,int time,int flg)
{
	LSTIME		nowlstime;
	int now;

	RealTimeToLSTime( NowTime.tv_sec, &nowlstime);
	now = getLSTime( &nowlstime);

	if(NPC_EventBigSmallLastCheck( time, now, flg) == TRUE) {
		return TRUE;
	}
	return FALSE;

}

#ifdef _CAX_NPCTIME_IF
/*-----------------------------------------
 * Time Check     ENENT:NPC CCIS_WXscript add
 ------------------------------------------*/
BOOL NPC_TiemCheckWithHour(int meindex,int talker,int ttime,int flg)
{
	time_t cc_seconds; 
	struct tm * cc_timeinfo; 
	int cc_sec , now; 

	cc_seconds = time (NULL); 
	cc_timeinfo = localtime ( &cc_seconds ); 

	now = cc_timeinfo->tm_hour;

	if(NPC_EventBigSmallLastCheck( ttime, now, flg) == TRUE) {
		return TRUE;
	}
	return FALSE;

}

/*-----------------------------------------
 * Time Check     ENENT:NPC CCIS_WXscript add
 ------------------------------------------*/
BOOL NPC_TiemCheckWithDay (int meindex,int talker,int ttime,int flg)
{
	time_t cc_seconds; 
	struct tm * cc_timeinfo; 
	int cc_sec , now; 

	cc_seconds = time (NULL); 
	cc_timeinfo = localtime ( &cc_seconds ); 

	now = cc_timeinfo->tm_wday;

	if(NPC_EventBigSmallLastCheck( ttime, now, flg) == TRUE) {
		return TRUE;
	}
	return FALSE;

}
#endif

/*------------------------------
 *--Itemë��������
 -------------------------------*/
BOOL NPC_EventItemCheck(int meindex,int talker,int itemNo,int flg)
{
	int i;
	int itemindex = -1;
	int id;

	//Change fix ����鴩�������ϵĵ���
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {

		itemindex=CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) ) {
			id=ITEM_getInt( itemindex ,ITEM_ID );

			if(NPC_EventBigSmallLastCheck( itemNo, id, flg) == TRUE) {
				if(flg == 0)	return TRUE;
				continue;
			}else{
				if(flg == 0) continue;
				return FALSE;
			}
		}
	}

	if(flg == 3) return TRUE;

	return FALSE;

}


/*------------------------------
 * levelë��������
 -------------------------------*/
BOOL NPC_EventLevelCheck(int meindex,int talker,int level,int flg)
{

	int mylevel;
	mylevel = CHAR_getInt(talker,CHAR_LV);

	if(NPC_EventBigSmallLastCheck( level, mylevel, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

BOOL NPC_EventAmPointCheck(int meindex,int talker,int ampoint,int flg)
{

	int myampoint;
	myampoint = sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), 0,0);

	if(NPC_EventBigSmallLastCheck( ampoint, myampoint, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

BOOL NPC_EventFameCheck(int meindex,int talker,int fame,int flg)
{

	int myfame;
	myfame = CHAR_getInt(talker,CHAR_FAME)/100;

	if(NPC_EventBigSmallLastCheck( fame, myfame, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

#ifndef _CAX_NPCTIME_OFF
BOOL NPC_EventYearCheck(int meindex,int talker,int npcyear,int flg)
{
	NPC_EventTime();
	if(NPC_EventBigSmallLastCheck( npcyear, year, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventMonCheck(int meindex,int talker,int npcmon,int flg)
{
	NPC_EventTime();
	if(NPC_EventBigSmallLastCheck( npcmon, mon, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventDateCheck(int meindex,int talker,int npcdate,int flg)
{
	NPC_EventTime();
	if(NPC_EventBigSmallLastCheck( npcdate, date, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventDayCheck(int meindex,int talker,int npcday,int flg)
{
	NPC_EventTime();
	if(NPC_EventBigSmallLastCheck( npcday, day, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventHourCheck(int meindex,int talker,int npchour,int flg)
{
	NPC_EventTime();
	if(NPC_EventBigSmallLastCheck( npchour, hour, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventMinCheck(int meindex,int talker,int npcmin,int flg)
{
	NPC_EventTime();
	if(NPC_EventBigSmallLastCheck( npcmin, min, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

BOOL NPC_EventSecCheck(int meindex,int talker,int npcsec,int flg)
{
	NPC_EventTime();
	if(NPC_EventBigSmallLastCheck( npcsec, sec, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
#endif

BOOL NPC_EventVipCheck(int meindex,int talker,int vip,int flg)
{

	int myVIP;
	myVIP = CHAR_getInt(talker,CHAR_VIPRIDE);

	if(NPC_EventBigSmallLastCheck( vip, myVIP, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

BOOL NPC_EventSuperCheck(int meindex,int talker,int super,int flg)
{

	int mySUPER;
	mySUPER = CHAR_getInt(talker,CHAR_SUPER);

	if(NPC_EventBigSmallLastCheck( super, mySUPER, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

BOOL NPC_EventVipPointCheck(int meindex,int talker,int vippoint,int flg)
{

	int myVIPPOINT;
	myVIPPOINT = sasql_vippoint(CHAR_getUseID(talker),0,0);

	if(NPC_EventBigSmallLastCheck( vippoint, myVIPPOINT, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

BOOL NPC_EventNpcTimeCheck(int meindex,int talker,int npctime,int flg)
{

	int myNPCTIME;
	myNPCTIME = CHAR_getInt(talker,CHAR_NPCTIME);
	
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep); /*ȡ�õ���ʱ��*/
	timep=mktime(p);
	if(npctime==0){
		if(myNPCTIME>=timep) return TRUE;
		return FALSE;
	}else{
		if(myNPCTIME<=timep) return TRUE;
		return FALSE;
	}
	
	return FALSE;

}

#ifdef _EV_NUM
BOOL NPC_EventEvNumCheck(int meindex,int talker,int evnum,int flg)
{

	int myEVNUM;
	myEVNUM = CHAR_getInt(talker,CHAR_EVNUM);

	if(NPC_EventBigSmallLastCheck( evnum, myEVNUM, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}

BOOL NPC_EventEvCodeCheck(int meindex,int talker,int npcno,int flg)
{
	char *evcode=NULL;
	evcode = CHAR_getChar(talker,CHAR_EVCODE);
	char errmsg[256];
	if(strlen(evcode)<=0) return FALSE;
	char tmpbuf[256];
	memset( tmpbuf, 0, sizeof( tmpbuf ) );
	int mynpcno,evflg,itemid,itemnum,petpicid,petminlv,petmaxlv;
	char *petname;
	if(getStringFromIndexWithDelim(evcode,"|", 1, tmpbuf, sizeof(tmpbuf)) == FALSE)
		return FALSE;
	mynpcno = atoi(tmpbuf);
	if(getStringFromIndexWithDelim(evcode,"|", 2, tmpbuf, sizeof(tmpbuf)) == FALSE)
		return FALSE;
	evflg = atoi(tmpbuf);
	if(evflg<1 || evflg>3) return FALSE;
	if(evflg==2){
		if(getStringFromIndexWithDelim(evcode,"|", 3, tmpbuf, sizeof(tmpbuf)) == FALSE)
			return FALSE;
		itemid = atoi(tmpbuf);
		if(getStringFromIndexWithDelim(evcode,"|", 4, tmpbuf, sizeof(tmpbuf)) == FALSE)
			return FALSE;
		itemnum = atoi(tmpbuf);
	}else if(evflg==3){
		if(getStringFromIndexWithDelim(evcode,"|", 3, tmpbuf, sizeof(tmpbuf)) == FALSE)
			return FALSE;
		petpicid = atoi(tmpbuf);
		if(getStringFromIndexWithDelim(evcode,"|", 4, tmpbuf, sizeof(tmpbuf)) == FALSE)
			return FALSE;
		petname= tmpbuf;
		if(getStringFromIndexWithDelim(evcode,"|", 5, tmpbuf, sizeof(tmpbuf)) == FALSE)
			return FALSE;
		petminlv = atoi(tmpbuf);
		if(getStringFromIndexWithDelim(evcode,"|", 6, tmpbuf, sizeof(tmpbuf)) == FALSE)
			return FALSE;
		petmaxlv = atoi(tmpbuf);
	}
	if(npcno!=mynpcno){
		char *npcmsg;
		npcmsg=sasql_ev_npcmsg(mynpcno);
		if(strcmp(npcmsg,"err")==0) return FALSE;
		if(evflg==1){
			sprintf(errmsg,"\n�뵽<%s>������\n",npcmsg);
			lssproto_WN_send( getfdFromCharaIndex(talker), WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OK,
            -1,
            -1,
            errmsg);
      return FALSE;
		}else if(evflg==2){
			sprintf(errmsg,"\n�����%d��<%s>��<%s>������\n",itemnum,ITEM_getNameFromNumber( itemid),npcmsg);
			lssproto_WN_send( getfdFromCharaIndex(talker), WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OK,
            -1,
            -1,
            errmsg);
      return FALSE;
		}else if(evflg==3){
			sprintf(errmsg,"\n����ŵȼ�Ϊ%d-%d֮��ĳ���<%s>��<%s>������\n",petminlv,petmaxlv,petname,npcmsg);
			lssproto_WN_send( getfdFromCharaIndex(talker), WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OK,
            -1,
            -1,
            errmsg);
      return FALSE;
		}
	}else{
		if(evflg==1){
			return TRUE;
		}else if(evflg==2){
			sprintf(errmsg,"ITEM=%d*%d",itemid,itemnum);
			if(NPC_EventReduce(meindex,talker,errmsg)==TRUE){
				return TRUE;
			}else{
				sprintf(errmsg,"\n�����%d��<%s>�����⽻����\n",itemnum,ITEM_getNameFromNumber( itemid));
				lssproto_WN_send( getfdFromCharaIndex(talker), WINDOW_MESSAGETYPE_MESSAGE,
            WINDOW_BUTTONTYPE_OK,
            -1,
            -1,
            errmsg);
      	return FALSE;
			}
		}else if(evflg==3){
			int peti,petindex=-1,mypetpicid,mypetlv;
			for(peti=0;peti<CHAR_MAXPETHAVE;peti++){
				petindex = CHAR_getCharPet( talker, peti);
				if(petindex==-1) continue;
				mypetpicid = CHAR_getInt(petindex,CHAR_BASEBASEIMAGENUMBER);
				mypetlv=CHAR_getInt(petindex,CHAR_LV);
				if(mypetpicid == petpicid && mypetlv>=petminlv && mypetlv<=petmaxlv){
					return TRUE;
				}
				continue;
			}
			if(peti>=CHAR_MAXPETHAVE){
				sprintf(errmsg,"\n����ŵȼ�Ϊ%d-%d֮��ĳ���<%s>���Ҵ�������\n",petminlv,petmaxlv,petname);
				lssproto_WN_send( getfdFromCharaIndex(talker), WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OK,
            -1,
            -1,
            errmsg);
      	return FALSE;
			}
		}
	}
	return FALSE;
}
#endif

#ifdef _ALL_ITEM
BOOL NPC_EventAllNum1Check(int meindex,int talker,int npcallnum1,int flg)
{

	int myALLNUM1;
	myALLNUM1 = sasql_allnum(1,0,0);

	if(NPC_EventBigSmallLastCheck( npcallnum1, myALLNUM1, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventAllNum2Check(int meindex,int talker,int npcallnum2,int flg)
{

	int myALLNUM2;
	myALLNUM2 == sasql_allnum(2,0,0);

	if(NPC_EventBigSmallLastCheck( npcallnum2, myALLNUM2, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventAllNum3Check(int meindex,int talker,int npcallnum3,int flg)
{

	int myALLNUM3;
	myALLNUM3 = sasql_allnum(3,0,0);

	if(NPC_EventBigSmallLastCheck( npcallnum3, myALLNUM3, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventAllNum4Check(int meindex,int talker,int npcallnum4,int flg)
{

	int myALLNUM4;
	myALLNUM4 = sasql_allnum(4,0,0);

	if(NPC_EventBigSmallLastCheck( npcallnum4, myALLNUM4, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
BOOL NPC_EventAllNum5Check(int meindex,int talker,int npcallnum5,int flg)
{

	int myALLNUM5;
	myALLNUM5 = sasql_allnum(5,0,0);

	if(NPC_EventBigSmallLastCheck( npcallnum5, myALLNUM5, flg) == TRUE)
	{
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;

}
#endif

BOOL NPC_EventEquitCheck(int meindex,int talker,int itemNo, int flg)
{
	int i;
	int itemindex=-1;
	int id;
	for( i=0;i<CHAR_STARTITEMARRAY;i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if( NPC_ActionBigSmallLastCheck(itemNo,id,flg) == TRUE )
				return TRUE;
		}
	}

	return FALSE;
}

/*------------------------------
 * ENDEventNoë��������
 -------------------------------*/
BOOL NPC_ENDEventNoCheck(int meindex,int talker,int shiftbit,int flg)
{

	if(NPC_EventCheckFlg( talker, shiftbit) == TRUE) {
		if(flg == 3) return FALSE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;
}


/*------------------------------
 * NOWEventNoë��������
 -------------------------------*/
BOOL NPC_NOWEventNoCheck(int meindex,int talker,int shiftbit,int flg)
{
	if(NPC_NowEventCheckFlg( talker, shiftbit) == TRUE) {
		if(flg == 3) return TRUE;
		return TRUE;
	}

	if(flg == 3) return TRUE;
	return FALSE;
}

/*---------------------------------------
 *    ����������
 --------------------------------------*/
BOOL NPC_EventBigSmallLastCheck(int point1,int mypoint,int flg)
{

	if(flg == 0) {
		if(point1 == mypoint) {
			return TRUE;
		}
	}else if(flg == 1) {
		if(mypoint < point1) {
			return TRUE;
		}
	}else if(flg == 2) {
		if(mypoint > point1) {
			return TRUE;
		}
	}else if(flg == 3) {
		if(point1 != mypoint) {
			return TRUE;
		}
	}
	return FALSE;
}


/*---------------------------------
 *����������ë��  ����
 *----------------------------------*/
#ifdef _CHANNEL_MODIFY
extern int *piOccChannelMember;
#endif
void NPC_MsgDisp(int meindex,int talker,int num)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
#ifdef _NEWEVENT
	char buf[4096];
	char token2[4096];
	char buf2[4096];
	char token[4096];
	char tmp[4096];
#else
	char buf[1024*2];
	char token2[100*14];
	char buf2[512];
	char token[512];
	char tmp[32];
#endif
	int i = 1;
	int q=1;
	int work = 0;
	int EvNo = 0;
	int fd = getfdFromCharaIndex( talker);
	int buttontype = WINDOW_BUTTONTYPE_OK;
	int windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG;
	int evch;
	

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("MsgDisp:GetArgStrErr");
		print("NPCName=%s\n", CHAR_getChar( meindex, CHAR_NAME));
		return ;
	}
	if(num == 22 || num == 82 || num == 55 || num == 66) {
		int pwork;

		pwork = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
		pwork = (pwork / 100) -1 ;

		getStringFromIndexWithDelim( argstr, "EventEnd", pwork, buf,sizeof( buf));
		
	}else{
		/*ɬ�ð����̻�ë�������*/
		while(getStringFromIndexWithDelim( argstr, "EventEnd", i, buf, sizeof(buf))
		 != FALSE)
		{
			i++;
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
			
			/*--��ì�����ϼ����ë������--*/
			NPC_Util_GetStrFromStrWithDelim( buf, "EventNo", buf2, sizeof( buf2));
			if(strstr( buf2, "-1") != 0) {
				EvNo = -1;
			}else{
				if(strstr(buf2,",") != NULL){
					char buf3[128];
					while(getStringFromIndexWithDelim( buf2, ",", q, buf3, sizeof( buf3))
					!= FALSE) {
						q++;
					}
					q--;
					q = rand()%q + 1;
					getStringFromIndexWithDelim( buf2, ",", q, buf3, sizeof( buf3));
					EvNo = atoi( buf3);
				}else{
					EvNo = atoi( buf2);
				}
			}
			//EvNo = atoi( buf2);

			/*--�׷º���������ë�浤����ƥ�������ì��������  �ƻ������������ݳ�--*/
			if(NPC_EventCheckFlg( talker, EvNo) == TRUE) continue;

			/*--ƽ����������������ë�浤--*/
			if(NPC_Util_GetStrFromStrWithDelim( buf,"Pet_Name", buf2,sizeof( buf2)) 
			 != NULL )
			{
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 1);
			}

			/*--ƽ����������������ë�浤--*/
			if(NPC_Util_GetStrFromStrWithDelim( buf,"KeyWord", buf2,sizeof( buf2) ) 
			 != NULL)
			{
					if(CHAR_getWorkInt( talker ,CHAR_WORKSHOPRELEVANT) != i) continue;
			}

			evch = NPC_ExChangeManEventCheck( meindex, talker, buf);

			if(evch != -1) {

			/*--��ì������������������--*/
		//	if(NPC_ExChangeManEventCheck( meindex, talker, buf) == TRUE) {
				break;
			}
		}
	}
	
	//print(" NPC_MsgDisp:%d ", num);
	
	switch( num) {
		  case 0:
			/*�ػ�����ì����ë�����¾�������ë  �ʶ���������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "StopMsg", 
												buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token,sizeof( buf2), buf2);
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_NOWEVENT;
			}
		break;
		
		  case 1:
		  	/*--֧���¾�������ë  ���׽��м�NOë����������������������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "NoStopMsg",
												 buf2, sizeof( buf2))
			 != NULL)
			{
				strcpysafe( token, sizeof(buf2), buf2);

			}
		break;
		
		  case 7:
			/*--�ػ�����ì����ë���������弰����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "EndStopMsg",
												 buf2, sizeof( buf2))
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
			
				/*--��ì����  ë  �Ȼ������������ջ�--*/
				NPC_EventAdd( meindex, talker, 1);
			
				/*--�׷º�ë����--*/
				NPC_NowEventSetFlgCls( talker, EvNo);
			
				/*--Ӭ���׼�ƥ��    ë������--*/
				if(CHAR_getInt( talker, CHAR_CHARM) > 0) {
					CHAR_setInt( talker, CHAR_CHARM, CHAR_getInt(talker,CHAR_CHARM) -1);
					CHAR_complianceParameter( talker );
					CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);
					NPC_CharmStatus( meindex, talker);
				}
			}
		break;
		
		  case 2:
				/*--ؤԻ���絤����������--*/
				if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
													 buf2, sizeof( buf2) )
				 != NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
					/*--��ì����  �׷º�ë������--*/
					if(NPC_EventAdd(meindex, talker, 0) != FALSE) {
						NPC_NowEventSetFlg( talker, EvNo);
					}
					else {
						return;
					}
				
				
					if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
													buf2, sizeof( buf2) )
					!=NULL)
					{
						char buf3[16];
						int k = 1;
						if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
						
						while(getStringFromIndexWithDelim(buf2 , "," ,k, 
												buf3, sizeof(buf3))
						 !=FALSE )
						{	
							k++;
							NPC_EventSetFlg( talker, atoi( buf3));
						}
					}

					if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
													 buf2, sizeof( buf2)) 
					 != NULL) {
					 	char buf3[16];
						int k = 1;

					 	k= 1 ;
						while(getStringFromIndexWithDelim(buf2 , "," , k, 
															buf3, sizeof(buf3))
						!= FALSE )
						{
							NPC_EndEventSetFlgCls( talker, atoi(buf3));
							k++;
						}
					}

				
				
				}
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work + ( i * 100);
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);
				
				if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
										token2,sizeof( token2) )
				!= NULL){
					NPC_EventWarpNpc(meindex,token2);
				}

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "ThanksMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}

		break;

		   case 22:
				
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work % 100;

				if(work == 1){
					if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
														 buf2, sizeof( buf2) )
					 != NULL)
					{
						strcpysafe( token,sizeof( buf2), buf2);
					}

					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, "ThanksMsg2") != NULL) {
					
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				
				}else{
					
					sprintf( tmp, "ThanksMsg%d", work);
					if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
														 buf2, sizeof( buf2) )
					 != NULL) {
						strcpysafe( token,sizeof( buf2), buf2);
					}
	
					work++;
					sprintf( tmp, "ThanksMsg%d", work);
	
	
					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, tmp) != NULL) {
						
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}else{
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_OK,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQTHANK,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				}
		break;

		  case 25:
			  /*--ؤԻ���絤����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token,sizeof( buf2), buf2);
				/*--��ì����  �׷º�ë������--*/
				if(NPC_EventAdd(meindex, talker, 2) != FALSE) {
					NPC_NowEventSetFlg( talker, EvNo);
				}
			}
		break;


		  case 3:
			/*--  ɧ������������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "NomalMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe(token, sizeof( buf2), buf2);
			}
		break;

		  case 4:
		  	/*--ʧ��  ة���������м��ݼ�����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "ItemFullMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe(token, sizeof( buf2), buf2);
			}
		break;

		case 5:
			/*--��  ë���¶˼����̼�������������--*/
				if( NPC_Util_GetStrFromStrWithDelim( buf, "RequestMsg",
													 buf2, sizeof( buf2) )
				 != NULL)
				{
          
          if(strstr(buf2,"%8d") != NULL) {
						char buf3[64];
						int cost;
						
						NPC_Util_GetStrFromStrWithDelim( buf, "DelStone",
													 buf3, sizeof( buf3) );
						cost = NPC_EventGetCost( meindex, talker, buf3);
						
						sprintf(token,buf2,cost);

					}else{
						strcpysafe( token,sizeof( buf2), buf2);
					}
					buttontype = WINDOW_BUTTONTYPE_YESNO;
					windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG;
				}

				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work + ( i * 100);
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);

				/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
				if(strstr( buf, "RequestMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
		break;

	   case 55:
				
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work % 100;

				if(work == 1){
					if( NPC_Util_GetStrFromStrWithDelim( buf, "RequestMsg",
														 buf2, sizeof( buf2) )
					 != NULL)
					{
						strcpysafe( token, sizeof( buf2), buf2);
					}

					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, "RequestMsg2") != NULL){
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				}else{
					
					sprintf( tmp, "RequestMsg%d", work);
					if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
														 buf2, sizeof( buf2) )
					 != NULL){
						strcpysafe( token,sizeof( buf2), buf2);
							
					}
					work++;
					sprintf( tmp, "RequestMsg%d", work);
					/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
					if(strstr( buf, tmp) != NULL) {
						
						/*���ƥ˪������*/
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_NEXT,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}else{
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
									WINDOW_BUTTONTYPE_YESNO,
									CHAR_WINDOWTYPE_WINDOWEVENT_REQMAINMSG,
									CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
									token);
						return;
					}
				}
		break;


		  case 6:
			  //print(" 6_��Ӧ? ");
			/*--������Ի���˼����̼�������������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "AcceptMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				if(strstr(buf2,"%8d") != NULL) {
					char buf3[64];
					int cost;
						
					NPC_Util_GetStrFromStrWithDelim( buf, "DelStone",
											 buf3, sizeof( buf3) );
					cost = NPC_EventGetCost( meindex, talker, buf3);
						
					sprintf( token, buf2, cost);
				}else{
					strcpysafe( token, sizeof( buf2), buf2);
				}
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG;
			}
	
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work + ( i * 100);
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);

			/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
			if(strstr( buf, "AcceptMsg2") != NULL) {
				/*���ƥ˪������*/
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_NEXT,
							CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token);
				return;
			}
		break;


	   case 66:
		   //print(" 66_��Ӧ? ");
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;

			if(work == 1){
				if( NPC_Util_GetStrFromStrWithDelim( buf, "AcceptMsg",
													 buf2, sizeof( buf2) )
				 != NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
				}

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "AcceptMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}else{
			
				sprintf( tmp, "AcceptMsg%d", work);
				if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
													 buf2, sizeof( buf2) )
				 != NULL){
					strcpysafe( token,sizeof( buf2), buf2);
				}
				work++;
				sprintf( tmp, "AcceptMsg%d", work);
				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, tmp) != NULL) {
					
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}else{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_YESNO,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCMAINMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}
		break;

		case 8:
				//print(" 8_��Ӧ? ");
			
				if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
													buf2, sizeof( buf2) )
				 != NULL)
				{
					strcpysafe( token, sizeof( buf2), buf2);
					/*--��ì����  �׷º�ë������--*/
					if(NPC_AcceptDel( meindex, talker,0) == FALSE){
						return ;
					}
					
					if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
						buf2, sizeof( buf2) )
						!=NULL)
					{
						char buf3[16];
						int k = 1;
						if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
						
						while(getStringFromIndexWithDelim(buf2 , "," ,k, 
							buf3, sizeof(buf3))
							!=FALSE )
						{	
							k++;
							NPC_EventSetFlg( talker, atoi( buf3));
						}
					}
					
					if( NPC_Util_GetStrFromStrWithDelim( buf, "Charm",
						buf2, sizeof( buf2))
						!=NULL)
					{
						/*--�����ƥ  ���գ�    ë������---*/
						if( CHAR_getInt( talker, CHAR_CHARM) < 100 && EvNo > 0 ){
							if(CHAR_getInt( talker, CHAR_CHARM) + atoi(buf2) > 100){
								CHAR_setInt( talker, CHAR_CHARM,100);
								
							}else{
								CHAR_setInt( talker, CHAR_CHARM, 
									CHAR_getInt(talker,CHAR_CHARM) + atoi(buf2));
								
							}
							
							CHAR_complianceParameter( talker );
							CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);
							NPC_CharmStatus( meindex, talker);
						}
					}
					
					if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
						buf2, sizeof( buf2)) 
						!= NULL) {
						char buf3[16];
						int k = 1;
						
						k= 1 ;
						while(getStringFromIndexWithDelim(buf2 , "," , k, 
							buf3, sizeof(buf3))
							!= FALSE )
						{
							NPC_EndEventSetFlgCls( talker, atoi(buf3));
							k++;
						}
					}
					
					
					// Robin add ���ְҵ
					if( NPC_Util_GetStrFromStrWithDelim( buf, "PROFESSION",
						buf2, sizeof( buf2)) != NULL)
					{
						int p_class = atoi( buf2);
						//Change fix 2004/07/05
						for( i=0; i<CHAR_STARTITEMARRAY; i++){ //���װ���е���
							if( ITEM_CHECKINDEX( CHAR_getItemIndex( talker , i ) )){
								if( CHAR_getInt( talker, PROFESSION_CLASS ) != 0
									&& CHAR_getInt( talker, PROFESSION_CLASS ) != ITEM_getInt( CHAR_getItemIndex( talker , i ), ITEM_NEEDPROFESSION) 
									&& ITEM_getInt( CHAR_getItemIndex( talker , i ), ITEM_NEEDPROFESSION) > 0 ){
									p_class = 1;
									CHAR_talkToCli( talker, -1, "���Ȱ�ְҵװ��ͨͨж�£�", CHAR_COLORWHITE);
									break;
								}
							}
						}
						if( p_class == 0)
						{
							print(" ====���ְҵ==== ");
							
							// ��ԭ����
							//if( PROFESSION_RESTORE_POINT( talker ) != 1 )	return FALSE;
							
							// �趨ְҵ����
							CHAR_setInt( talker, PROFESSION_CLASS, p_class );	
#ifdef _CHANNEL_MODIFY
							if(CHAR_getInt(talker,PROFESSION_CLASS) > 0){
								int i,pclass = CHAR_getInt(talker,PROFESSION_CLASS) - 1;
								for(i=0;i<getFdnum();i++){
									if(*(piOccChannelMember + (pclass * getFdnum()) + i) == talker){
										*(piOccChannelMember + (pclass * getFdnum()) + i) = -1;
										break;
									}
								}
							}
#endif
							
							// �趨ְҵ�ȼ� 0
							CHAR_setInt( talker, PROFESSION_LEVEL, 0 );
							
							// ���ܵ�������
							CHAR_setInt(talker, PROFESSION_SKILL_POINT, 0 );
							
							// ��ԭ����
							for( i=0; i<3; i++ )
								CHAR_setInt( talker, PROFESSION_FIRE_R+i, /*CHAR_getInt( toindex, PROFESSION_FIRE_R+i) + value*/0 );
							
							// ��ԭ��ʦMP����
							CHAR_setInt( talker , CHAR_MAXMP , 100 );
							
							// ɾ�����м���
							CHAR_CHAT_DEBUG_delsk( talker, "all" );
							
							// ?? Andy add
							CHAR_setInt( talker , ATTACHPILE, 0);
							
							// ����������
							NPC_EndEventSetFlgCls( talker, 145);
							NPC_EndEventSetFlgCls( talker, 146);
							NPC_EndEventSetFlgCls( talker, 147);					
							
							CHAR_sendStatusString( talker , "S");
							
							CHAR_sendCToArroundCharacter( CHAR_getWorkInt( talker , CHAR_WORKOBJINDEX ));
							
						}
						
					}

#ifdef _ANGEL_SUMMON
					// ����ٻ�����
					if( NPC_Util_GetStrFromStrWithDelim( buf, "MISSIONOVER",
						buf2, sizeof( buf2)) != NULL)
					{
						int mindex;
						int mission;
						char nameinfo[64];
						
						mission = atoi( buf2);
						mindex = checkIfAngel( talker);

						if( mission == missiontable[mindex].mission )
						{
							char msg[1024];
							
							print(" ====����ٻ�����==== ");

							CHAR_setInt( talker, CHAR_HEROCNT, 
								CHAR_getInt( talker, CHAR_HEROCNT)+1 );
							getMissionNameInfo( talker, nameinfo);
							saacproto_ACMissionTable_send( acfd, MISSION_HERO_COMPLETE, 4, nameinfo, "");
							sprintf( msg, " ������� i:%d m:%d %s ", mindex, mission, nameinfo);
							print( msg);
							LogAngel( msg );
						}
						
					}

					// ����ٻ�����
					if( NPC_Util_GetStrFromStrWithDelim( buf, "MISSIONCLEAN",
						buf2, sizeof( buf2)) != NULL)
					{
						int mindex;
						int mission;
						char nameinfo[64];
						
						mission = atoi( buf2);
						mindex = checkIfAngel( talker);

						if( mission == missiontable[mindex].mission )
						{
							print(" ====����ٻ�����==== ");

							getMissionNameInfo( talker, nameinfo);
							saacproto_ACMissionTable_send( acfd, mindex, 3, nameinfo, "");
						}
						
					}
#endif
				}
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
				work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
				work = work + ( i * 100 );
				CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);
	
				if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
													token2,sizeof( token2) )
					!= NULL){
						NPC_EventWarpNpc(meindex,token2);
				}
				/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
#ifdef _NEWEVENT
#else
				if(strstr( buf, "ThanksMsg2") != NULL){
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
#endif
		break;

#ifdef _NEWEVENT
		case 81:
			//print(" 81_��Ӧ? ");

			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;
			sprintf(tmp, "ThanksMsg%d", work + 1);
			if(work == 0 && strstr(buf, tmp) == NULL)
			{
				if(NPC_Util_GetStrFromStrWithDelim(buf, "ThanksMsg",
					buf2, sizeof( buf2)) != NULL)
					strcpysafe( token,sizeof( buf2), buf2);
				if(strstr(buf, "ThanksMsg2") != NULL)
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
					WINDOW_BUTTONTYPE_NEXT,
					CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
					CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
					token);
				else
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
					CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
					token);
				return;
			}
			else
			{
				if(NPC_Util_GetStrFromStrWithDelim(buf, tmp,
					buf2, sizeof( buf2)) != NULL)
					strcpysafe( token,sizeof( buf2), buf2);
				work++;
				sprintf(tmp, "ThanksMsg%d", work + 1);
				
				if(strstr(buf, tmp) != NULL)
				{
					lssproto_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_NEXT,
						CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
						CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
						token);
					return;
				}
				else
				{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
						WINDOW_BUTTONTYPE_OK,
						CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANKNEW,
						CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
						token);
					return;
				}
			}
			
			break;
#endif	
	
	   case 82:
			work = CHAR_getWorkInt(talker,CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;

			if(work == 1){
				if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
													 buf2, sizeof( buf2))
				 != NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
				}

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "ThanksMsg2") != NULL){
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}else{
				
				sprintf( tmp, "ThanksMsg%d", work);
				if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
													 buf2, sizeof( buf2) )
				!= NULL)
				{
					strcpysafe( token,sizeof( buf2), buf2);
				}
				
				work++;
				sprintf( tmp, "ThanksMsg%d", work);

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, tmp) != NULL) {
						
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}else{
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								CHAR_WINDOWTYPE_WINDOWEVENT_ACCTHANK,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}
			break;

		case 88:
			/*--ʧ�ͱ�������--*/
			/*--ؤԻ���絤����������--*/
			/*--ʸ������  ��--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "ThanksMsg",
												buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				/*--��ì����  �׷º�ë������--*/
				if(NPC_AcceptDel( meindex, talker,1) == FALSE){
					return ;
				}
				
				if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
												buf2, sizeof( buf2) )
				!=NULL)
				{
					char buf3[16];
					int k = 1;
				
					if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
					
					while(getStringFromIndexWithDelim(buf2 , "," ,k, 
											buf3, sizeof( buf3))
					 !=FALSE )
					{	
						k++;
						NPC_EventSetFlg( talker, atoi( buf3));
					}
				}

				if( NPC_Util_GetStrFromStrWithDelim( buf, "Charm",
													buf2, sizeof( buf2))
				!=NULL)
				{
					/*--�����ƥ  ���գ�    ë������---*/
					if( CHAR_getInt( talker, CHAR_CHARM) < 100 && EvNo > 0 ){
						if(CHAR_getInt( talker, CHAR_CHARM) + atoi(buf2) > 100){
							CHAR_setInt( talker, CHAR_CHARM,100);
						}else{
							CHAR_setInt( talker, CHAR_CHARM, 
									CHAR_getInt(talker,CHAR_CHARM) + atoi( buf2));
						}
						CHAR_complianceParameter( talker );
						CHAR_send_P_StatusString( talker, CHAR_P_STRING_CHARM);
						NPC_CharmStatus( meindex, talker);
					}
				}
			}

		break;


		  case 9:
			
		  	if( NPC_Util_GetStrFromStrWithDelim( buf, "NomalWindowMsg",
												buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				if(EvNo != -1) NPC_NowEventSetFlg( talker, EvNo);
			}
#ifdef _NEWEVENT
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work + ( i * 100);
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, work);

			/*--��ʸ���⻥ؤ��������ʾ�������̼�������������ë������--*/
			if(strstr( buf, "NomalWindowMsg2") != NULL) {
				/*���ƥ˪������*/
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
							WINDOW_BUTTONTYPE_NEXT,
							CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
							CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
							token);
				return;
			}
#endif
		  	break;
		  	
#ifdef _NEWEVENT
	  case 99:
			work = CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD);
			work = work % 100;

			if(work == 1)
			{
				if( NPC_Util_GetStrFromStrWithDelim( buf, "NomalWindowMsg1",
									 buf2, sizeof( buf2) ) != NULL)
					strcpysafe( token,sizeof( buf2), buf2);

				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, "NomalWindowMsg2") != NULL) {
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}else{
			
				sprintf( tmp, "NomalWindowMsg%d", work);
				if( NPC_Util_GetStrFromStrWithDelim( buf, tmp,
													 buf2, sizeof( buf2) )
				 != NULL){
					strcpysafe( token,sizeof( buf2), buf2);
				}
				work++;
				sprintf( tmp, "NomalWindowMsg%d", work);
				/*--��ʸ���⻥ؤ��������ʾ���������̼�������������ë������--*/
				if(strstr( buf, tmp) != NULL) {
					
					/*���ƥ˪������*/
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_NEXT,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}else{
					int fd = getfdFromCharaIndex( talker);
					char token2[14*100];
					/*--���ƥ��ʧ��  ةئ����ܰ��ë�浤--*/
					if(NPC_AcceptDel( meindex, talker,0) == FALSE) return;
	
					/*�׷º�ë�������ƻ���ľ�ɣ�����������--*/
					if( EvNo != -1) NPC_NowEventSetFlg( talker, EvNo);
						
					if(NPC_Util_GetStrFromStrWithDelim( buf,"NpcWarp",
										token2,sizeof( token2) )!= NULL){
								NPC_EventWarpNpc(meindex,token2);
					}
					
					//�׷º�ë  ����
					if(NPC_Util_GetStrFromStrWithDelim( buf, "EndSetFlg", 
										buf2, sizeof( buf2) )!=NULL)
					{
						char buf3[16];
						int k = 1;
				
						if(EvNo != -1) NPC_NowEventSetFlgCls( talker, EvNo);
				
						while(getStringFromIndexWithDelim(buf2 , "," ,k, 
								buf3, sizeof(buf3)) !=FALSE )
						{	
							k++;
							NPC_EventSetFlg( talker, atoi( buf3));
						}
					}
					//��ì�����׷º�ë����
					if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
							 buf2, sizeof( buf2)) != NULL) {
						 	char buf3[16];
							int k = 1;

						 	k= 1 ;
							while(getStringFromIndexWithDelim(buf2 , "," , k, 
																buf3, sizeof(buf3))
							!= FALSE )
							{
								NPC_EndEventSetFlgCls( talker, atoi(buf3));
								k++;
							}
					}
					lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_YES,
								CHAR_WINDOWTYPE_WINDOWEVENT_STARTMSG,
								CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
								token);
					return;
				}
			}
		  break;
#endif
		  case 10:
			  
			/*--ʸ�������������м��ݼ�����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "PetFullMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				print(" PetFullMsg:%s ", token);
			}
			else
			{
				print(" PetFullMsg_Error:%d ", __LINE__);
			}
		break;

	  case 11:
			/*--�׷º�ë�������弰����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanMainMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
				buttontype = WINDOW_BUTTONTYPE_YESNO;
				windowtype = CHAR_WINDOWTYPE_WINDOWEVENT_CLEANMSG;
			}
		break;
	  case 12:
			/*--�׷º�ë������  ����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlgMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				char buf3[64];
				int loop=1;
				
				strcpysafe( token, sizeof( buf2), buf2);
				NPC_Util_GetStrFromStrWithDelim( buf, "CleanFlg",
												 buf2, sizeof( buf2) );
				while(getStringFromIndexWithDelim(buf2 , "," , loop, buf3, sizeof(buf3))
				!= FALSE )
				{
					NPC_EndEventSetFlgCls( talker, atoi(buf3));
					loop++;
				}
			}
		break;

	  case 13:
			/*--���Ż��������м�����������--*/
			if( NPC_Util_GetStrFromStrWithDelim( buf, "StoneFullMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
			}
		break;
		
		 case 14:
			/*--���Ż���Իئ�ж���������--*/
					if( NPC_Util_GetStrFromStrWithDelim( buf, "StoneLessMsg",
												 buf2, sizeof( buf2) )
			 != NULL)
			{
				strcpysafe( token, sizeof( buf2), buf2);
			}
		break;
	}

	/*���ƥ˪������*/
#ifdef _NEWEVENT
	if(num != 8)
#endif
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
					buttontype,
					windowtype,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);

}

/*--------------------------------------
 *��ì�����ü�ʧ��  ةëܰ������
 ---------------------------------------*/
BOOL NPC_EventAdd(int meindex,int talker,int mode)
{

	char buf[64];
	char buff2[1024];
	int j = 1,i = 0;
	int kosuucnt = 0;
	int itemindex;
	char buff[1024*2];
	int rand_j = 0;
	int rand_flg = 0;
	int rand_cnt = 0;
	int evcnt;
	int b_flg = 0;

	/*--ɬ�ð����̻�  ��̤�л�ؤ������ë  ���ƻ������ּ�ë  ������--*/
	evcnt = NPC_EventFile( meindex, talker, buff);

	if(evcnt == -1) return FALSE;

//	if(NPC_EventFile( meindex, talker, buff) == FALSE) return FALSE;

	/*--ʧ��  ة����ë��������--*/
	if(NPC_ItemFullCheck( meindex, talker, buff, mode, evcnt) == FALSE){
		NPC_MsgDisp( meindex, talker, 4);
		return FALSE;
	}

	/*--�����������ͱ�  ��--*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		
		stone = NPC_EventGetCost( meindex, talker, buff2);

		if( (CHAR_getInt( talker, CHAR_GOLD) - stone) < 0 ){
			NPC_MsgDisp( meindex, talker, 14);
			return FALSE;
		}
	}


	/*--ʸ������  ëܰ������--*/
	if((NPC_Util_GetStrFromStrWithDelim( buff, "pet_skill", buff2, sizeof( buff2) )
	 !=NULL) && mode == 0 ){
		if(mode == 0) {
			NPC_PetSkillMakeStr( meindex, talker, 1);
			return TRUE;
		}
	}
	
	
	if(mode == 2) mode = 0;
	
	/*--ʸ����ëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetPet", buff2, sizeof( buff2) )
	 !=NULL)
	{	
		if(mode == 0){
			if(NPC_EventAddPet( meindex, talker, buff2,0) == FALSE) {
				NPC_MsgDisp( meindex, talker, 10);
				return FALSE;
			}
		}else if(mode == 1){
		
		}
	}

	// Robin add ��NPCȡ���ﵰ
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetEgg", buff2, sizeof( buff2) )
	 !=NULL)
	{	
		if(mode == 0){
			if(NPC_EventAddEgg( meindex, talker, buff2,0) == FALSE) {
				print(" AddEgg_Error:%d ", __LINE__);
				NPC_MsgDisp( meindex, talker, 10);
				return FALSE;
			}
		}else if(mode == 1){
		
		}
	}

	if(NPC_Util_GetStrFromStrWithDelim( buff, "DelItem", buff2, sizeof( buff2) ) !=NULL){
		if(strstr(buff2,"EVDEL") != NULL){
			char work[256];
			char nbuf[256];
			
			NPC_Util_GetStrFromStrWithDelim(buf, "EVENT", buff2, sizeof( buff2));
			getStringFromIndexWithDelim( buff2, ",", evcnt, work, sizeof( work));

			if(strstr(buff,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}
			
			if(NPC_Util_GetStrFromStrWithDelim( buff, "NotDel", nbuf, sizeof( nbuf)) !=  NULL) {
				NPC_EventDelItemEVDEL( meindex, talker, work, nbuf,b_flg);
			}else{
				NPC_EventDelItemEVDEL( meindex, talker, work, "-1",b_flg);
			}
			
		}else{
			if(strstr(buff,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}

			NPC_EventDelItem( meindex, talker, buff2, b_flg);
		}
	}

	/*--����ë����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		char token[128];
		
		stone = NPC_EventGetCost( meindex, talker, buff2);

		CHAR_setInt(talker,CHAR_GOLD,CHAR_getInt( talker, CHAR_GOLD) - stone);

		/*--�ɷ¶�����˪Ի--*/
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
		sprintf(token, "����%d��stone��", stone);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}


	/*--�¼�ĸةʧ��  ة---*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetRandItem", buf, sizeof( buf) ) != NULL ){
		j=1;
		while( getStringFromIndexWithDelim( buf , "," , j, buff2, sizeof( buff2)) != FALSE ){
			j++;
			rand_j++;
		}
		rand_cnt = 1;
	}


	/*--  ɧ��ʧ��  ة������--*/
	if(NPC_Util_GetStrFromStrWithDelim( buff, "GetItem", buf, sizeof( buf)) != NULL ){
		if(mode == 0) {
			j = 1;
			/*--���ۼ�ʧ��  ةë  �ʾ���--*/
			while(getStringFromIndexWithDelim( buf, "," , j, buff2, sizeof(buff2)) !=FALSE ){
				j++;
			}
			
			j--;
			for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex=CHAR_getItemIndex( talker , i );
				if( !ITEM_CHECKINDEX( itemindex) ){
					kosuucnt++;
				 }
			}
			j= j + rand_cnt;
			if( kosuucnt < j ){
				NPC_MsgDisp( meindex, talker, 4);
				return FALSE;

			}else{
				if(NPC_Util_GetStrFromStrWithDelim( buff, "GetRandItem", buff2, sizeof( buff2))	!=NULL)	{
	 				rand_flg = 1;
					NPC_RandItemGet( meindex, talker, rand_j, buff2);
	 			}
				if(NPC_EventAddItem( meindex, talker, buf) == FALSE) return FALSE;
			}
		}else if(mode == 1){
			if(strstr(buff,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}
			NPC_EventDelItem( meindex, talker, buf,b_flg);
		}
	}

	if( (NPC_Util_GetStrFromStrWithDelim( buff, "GetRandItem", buf, sizeof( buf) )
		!=NULL) && (rand_flg == 0) && mode == 0 )
	{	
		/*--��������   ����    �¾���-*/
		for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex=CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX( itemindex) ){
				kosuucnt++;
			 }
		}
		/*--���廥ؤ�¾�������--*/
		if( kosuucnt == 0 ){
			NPC_MsgDisp( meindex, talker, 4);
			return FALSE;
		}

	
		NPC_RandItemGet( meindex, talker, rand_j,buf);
	}
	
#ifdef _EXCHANGEMAN_REQUEST_DELPET
	if( NPC_Util_GetStrFromStrWithDelim( buff, "DelPet", buff2, sizeof( buff2) ) != NULL && mode == 0 ) {
		char buf4[32];
		char buf5[256];
		int i=1;
		char work[256];
		int j=1;
		
		if(strstr(buff2,"EVDEL") != NULL){
			NPC_Util_GetStrFromStrWithDelim( buf, "EVENT", buf5, sizeof( buf5));
			getStringFromIndexWithDelim( buf5, ",",evcnt , buff2, sizeof( buff2));	
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work))!=FALSE){
				j++;			
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;		
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work)) != FALSE){
				j++;
				if(strstr( work, "PET") == NULL ) continue;
	
				if(strstr(work,"*") != NULL){
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1)
							== FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;

					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1) == FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}
					
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		
		}else{
			/*--�ֵ�1պ��ʸ����ë  �Ȼ����¾�����������ë�浤--*/
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work))
			!=FALSE)
			{
				j++;
				/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;
			
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work)) != FALSE)
			{
				j++;
				if(strstr(work,"*") != NULL)
				{
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1) == FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;
					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1) == FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0) == FALSE) {
								continue;
							}
						}
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		}
	}
#endif
	return TRUE;

}


/*--------------------------------------
 *�¼�ĸةƥʧ��  ةë������������
 ------------------------------------ */
BOOL NPC_RandItemGet(int meidex,int talker,int rand_j,char *buf)
{
	char buff2[64];
	int randitem;
	int ret;
	int itemindex;
	char token[128];

	if(rand_j == 0) {
		print("Event:��춣��Ľ��룬���ִ���");
 		return FALSE;
 	}
 	
	//randitem = rand()%rand_j;
	//srand((int)time(0));
	randitem = RAND(1,rand_j);
	if(randitem == 0) randitem = rand_j;

	getStringFromIndexWithDelim(buf , "," , randitem, buff2, sizeof(buff2)) ;

	itemindex = ITEM_makeItemAndRegist( atoi( buff2));

	if(itemindex == -1) return FALSE;
	
	/*ʧ��  ة��ܰ��(  ��ʧ��  ة  ����ľ��������  */
	ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
	if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
		print( "npc_exchangeman.c: additem error itemindex[%d]\n", itemindex);
		ITEM_endExistItemsOne( itemindex);
		return FALSE;
	}

	if(itemindex != -1) {
		LogItem(
			CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
			CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
			itemindex,
#else
			ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
			"EventAddItem(�����������õ��ĵ���)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
 			CHAR_getInt( talker,CHAR_Y ),
            ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)
		);
	}
					
	sprintf(token,"������%s",ITEM_getChar( itemindex, ITEM_NAME));
	CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

	CHAR_sendItemDataOne( talker, ret);
	return TRUE;

}


/*----------------------------------------------------
 * ��ì����
 ----------------------------------------------------*/
BOOL NPC_AcceptDel(int meindex,int talker,int mode )
{

	char buf[1024*2];
	char buff2[1024];
	int rand_j = 0;
	int j = 0;
	int evcnt;
	int b_flg = 0;
	evcnt = NPC_EventFile( meindex, talker, buf);
	if(evcnt == -1) return FALSE;

	/*--ɬ�ð����̻�  �微��ľ����������ë  ���ƻ������ּ�ë  ������--*/
//	if(NPC_EventFile(meindex,talker,buf)==FALSE) return FALSE;

	/*--ʧ��  ة����ë��������--*/
	if(NPC_ItemFullCheck( meindex, talker, buf, mode, evcnt) == FALSE){
		NPC_MsgDisp( meindex, talker, 4);
		return FALSE;
	}
	
	/*--�����������ͱ�  ��--*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		stone = NPC_EventGetCost( meindex, talker, buff2);

		if( (CHAR_getInt( talker, CHAR_GOLD) - stone) < 0 ) {
			NPC_MsgDisp( meindex, talker, 14);
			return FALSE;
		}
	}


	/*--�����������ͱ�  ��--*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetStone", buff2, sizeof( buff2) )!=NULL){
		int stone;
		stone = atoi(buff2);

		if( (CHAR_getInt( talker, CHAR_GOLD) + stone) >= CHAR_getMaxHaveGold( talker) ) {
			NPC_MsgDisp( meindex, talker, 13);
			return FALSE;
		}
	}


	/*--ʸ������  ëܰ������--*/
	if((NPC_Util_GetStrFromStrWithDelim( buf, "pet_skill", buff2, sizeof( buff2) )
	 !=NULL) && mode == 0 ){
		if(mode == 0){
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTTRD, 1);
			NPC_PetSkillMakeStr( meindex, talker, 1);
			return TRUE;
		}
	}

	/*--ʸ����ë����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelPet", buff2, sizeof( buff2) ) != NULL) {
		char buf4[32];
		char buf5[256];
		int i=1;
		char work[256];
		int j=1;
		
		if(strstr(buff2,"EVDEL") != NULL){
			NPC_Util_GetStrFromStrWithDelim( buf, "EVENT", buf5, sizeof( buf5));
			getStringFromIndexWithDelim( buf5, ",",evcnt , buff2, sizeof( buff2));
		
		
			/*--�ֵ�1պ��ʸ����ë  �Ȼ����¾�����������ë�浤--*/
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work))
			!=FALSE)
			{
				j++;
				
				/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;
			
			
			while(getStringFromIndexWithDelim( buff2, "&", j, work, sizeof( work))
			!= FALSE)
			{
				j++;
				if(strstr( work, "PET") == NULL ) continue;
	
				if(strstr(work,"*") != NULL)
				{
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1)
							== FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) 
							== FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;

					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1)
							== FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0)
							== FALSE) {
								continue;
							}
						}
					
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		
		}else{
		
			/*--�ֵ�1պ��ʸ����ë  �Ȼ����¾�����������ë�浤--*/
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work))
			!=FALSE)
			{
				j++;
				/*--ʸ�������������ͻ������Ȼ���������������PET����ë������������ --*/
				if(strstr( work, "PET") != NULL) {
					if(strstr( work, "EV") != NULL) {
						if(NPC_PetLvCheck( meindex, talker, work, 1) == FALSE) return FALSE;
					}else{
						if(NPC_PetLvCheck( meindex, talker, work, 0) == FALSE) return FALSE;
					}
				}
			}
			j = 1;
			
			
			while(getStringFromIndexWithDelim( buff2, ",", j, work, sizeof( work))
			!= FALSE)
			{
				j++;
				if(strstr(work,"*") != NULL)
				{
					int cnt=0;
					int petcnt=0;
					int petindex;
					
					getStringFromIndexWithDelim( work, "*", 2, buf4, sizeof( buf4));
					cnt = atoi(buf4);
					
					for(i=0 ; i < CHAR_MAXPETHAVE ; i++){
						petindex = CHAR_getCharPet( talker, i);
		
						if( petindex == -1  )  continue;
		
						if(strstr( work, "EV") != NULL) {
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 1)
							== FALSE) {
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2(petindex, meindex, talker, work, 0) 
							== FALSE) {
								continue;
							}
						}

						if(NPC_EventDelPet( meindex, talker,i) == FALSE) return FALSE;
						
						petcnt++;
						/*--϶�������м�ʸ����ë�����׾���������--*/
						if(cnt == petcnt) break;

					}
				}else{
					/*--ʸ����ë����������ë϶���ƻ�ئ�м�ƥ���ʸ����ë�廯����--*/

					int petindex;

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ) {
						petindex = CHAR_getCharPet( talker, i);

					    if( !CHAR_CHECKINDEX( petindex) )  continue;

						if(strstr( buff2, "EV") != NULL){
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 1)
							== FALSE){
								continue;
							}
						}else{
							if(NPC_PetLvCheckType2( petindex, meindex, talker, work, 0)
							== FALSE) {
								continue;
							}
						}
					
						if(NPC_EventDelPet( meindex, talker, i) == FALSE) return FALSE;
					}
				}
			}
		}
	}
	


	/*--����ëؤ����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		char token[128];

		stone = atoi(buff2);
		CHAR_setInt(talker,CHAR_GOLD,CHAR_getInt( talker, CHAR_GOLD) + stone);
		/*--�ɷ¶�����˪Ի--*/
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	
		sprintf( token,"�õ�%d��stone", stone);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}


	/*--ʸ����ëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetPet", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventAddPet( meindex, talker, buff2,1) == FALSE) {
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}
	
#ifdef _TRANS_7_NPC
	if(NPC_Util_GetStrFromStrWithDelim( buf, "TRANS7", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventTRANS( meindex, talker, buff2,1) == FALSE) {
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "TRANSNEW7", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventNewTRANS( meindex, talker, buff2,1) == FALSE) {
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}
#ifdef _TRANS7_POINT
	if(NPC_Util_GetStrFromStrWithDelim( buf, "TRANSNEWSEVEN", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventNewTRANSSEVEN( meindex, talker, buff2,1) == FALSE) {
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}
#endif
#endif	

	// Robin add ��NPCȡ���ﵰ
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetEgg", buff2, sizeof( buff2) )
	!=NULL)
	{
		if(NPC_EventAddEgg( meindex, talker, buff2,1) == FALSE) {
			print(" AddEgg_Error:%d ", __LINE__);
			NPC_MsgDisp( meindex, talker, 10);
			return FALSE;
		}
	}

	/*--ʧ��  ةë����--*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelItem", buff2, sizeof( buff2) )	!= NULL){
		if(strstr(buff2,"EVDEL") != NULL){
			char work[256];
			char nbuf[256];
		
			NPC_Util_GetStrFromStrWithDelim(buf, "EVENT", buff2, sizeof( buff2));
			getStringFromIndexWithDelim( buff2, ",", evcnt, work, sizeof( work));

			if(strstr(buf,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}

			if(NPC_Util_GetStrFromStrWithDelim( buf, "NotDel", nbuf, sizeof( nbuf))	!= NULL ){
				NPC_EventDelItemEVDEL( meindex, talker, work, nbuf,b_flg);
			}else{
				NPC_EventDelItemEVDEL( meindex, talker, work, "-1",b_flg);
			}
		}else{
			if(strstr(buf,"Break")!=NULL){
				b_flg=1;
			}else{
				b_flg=0;
			}
			NPC_EventDelItem( meindex, talker, buff2,b_flg);
		}
	}


	/*--����ë����---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelStone", buff2, sizeof( buff2) )
	!=NULL)
	{
		int stone;
		char token[128];
		
		stone = NPC_EventGetCost( meindex, talker, buff2);

		CHAR_setInt(talker,CHAR_GOLD,CHAR_getInt( talker, CHAR_GOLD) - stone);
		/*--�ɷ¶�����˪Ի--*/
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
		sprintf( token, "����%d��stone��", stone);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "AddFame", buff2, sizeof( buff2) )
	!=NULL)
	{
		int npcfame;
		char token[128];
		
		npcfame = atoi(buff2)*100;
		if(CHAR_getInt(talker,CHAR_FAME)+npcfame>1000000)
			{
				CHAR_talkToCli( talker, -1, "�������������!", CHAR_COLORRED);
			}
		else
			{
				CHAR_setInt(talker,CHAR_FAME,CHAR_getInt( talker, CHAR_FAME) + npcfame);
		/*--�ɷ¶�����˪Ի--*/
				sprintf( token, "���%d��������", npcfame/100);
				CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
			}
	}
#ifdef _EV_NUM	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelEvItem", buff2, sizeof( buff2) )	!= NULL){
		char token[16];
		if(CHAR_getInt(talker,CHAR_EVITEM)>0){
			sprintf(token,"%d*1",CHAR_getInt(talker,CHAR_EVITEM));
			NPC_EventDelItem( meindex, talker, token,0);
		}
	}
#endif	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelFame", buff2, sizeof( buff2) )
	!=NULL)
	{
		int npcfame;
		char token[128];
		
		npcfame = atoi(buff2)*100;
		
		if(CHAR_getInt(talker,CHAR_FAME)<npcfame)
			{
				CHAR_talkToCli( talker, -1, "�����������Ŷ!", CHAR_COLORRED);
			}
		else
			{
				CHAR_setInt(talker,CHAR_FAME,CHAR_getInt( talker, CHAR_FAME) - npcfame);
				sprintf( token, "����%d��������", npcfame/100);
				CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
			}
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "AddPoint", buff2, sizeof( buff2) )
	!=NULL)
	{
		int point;
		char token[128];
		point = atoi(buff2);
		sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), point,1);
		sprintf( token, "�õ�%d�Ļ��֡�", point);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelPoint", buff2, sizeof( buff2) )
	!=NULL)
	{
		int point;
		char token[128];
		point = atoi(buff2);
		sasql_ampoint( CHAR_getChar(talker,CHAR_CDKEY), -point,1);
		sprintf( token, "����%d�Ļ��֡�", point);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "AddVipPoint", buff2, sizeof( buff2) )
	!=NULL)
	{
		int point,mypoint;
		char token[128];
		int fd = getfdFromCharaIndex( talker);
		char* id = CHAR_getChar(talker, CHAR_CDKEY);
		point = atoi(buff2);
		sasql_vippoint(id,point,1);
		sprintf( token, "�õ�%d�Ļ�Ա�㡣", point);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelVipPoint", buff2, sizeof( buff2) )
	!=NULL)
	{
		int point,mypoint;
		char token[128];
		int fd = getfdFromCharaIndex( talker);
		char* id = CHAR_getChar(talker, CHAR_CDKEY);
		point = atoi(buff2);
		sasql_vippoint(id,-point,1);
		sprintf( token, "����%d�Ļ�Ա�㡣", point);
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetNpcTime", buff2, sizeof( buff2) )
	!=NULL)
	{
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep); /*ȡ�õ���ʱ��*/
		timep = mktime(p);
		CHAR_setInt(talker,CHAR_NPCTIME,timep+atoi(buff2));
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "Warp", buff2, sizeof( buff2) )
	!=NULL)
	{
		char buf3[32];
		int nfloor,nx,ny;
		getStringFromIndexWithDelim( buff2, ",", 1, buf3, sizeof( buf3));
		nfloor=atoi(buf3);
		getStringFromIndexWithDelim( buff2, ",", 2, buf3, sizeof( buf3));
		nx=atoi(buf3);
		getStringFromIndexWithDelim( buff2, ",", 3, buf3, sizeof( buf3));
		ny=atoi(buf3);
		CHAR_warpToSpecificPoint(talker,nfloor,nx,ny);
	}
	
	
#ifdef	_NPC_PETSKILLUP
	if(NPC_Util_GetStrFromStrWithDelim( buf, "PSkillUp", buff2, sizeof( buff2) )
	!=NULL)
	{
		char buf3[64],buf4[32],buf5[32];
		int skillnum,i,skillid,skillno,petindex;
		int pcnt=0;
		getStringFromIndexWithDelim( buff2, ",", 1, buf3, sizeof( buf3));
		skillnum=atoi(buf3);
		getStringFromIndexWithDelim( buff2, ",", 2, buf3, sizeof( buf3));
		for(i=1;i<=skillnum;i++)
		{
			pcnt=0;
			memset( buf4 , 0 , sizeof( buf4 ) );
			memset( buf5 , 0 , sizeof( buf5 ) );
			getStringFromIndexWithDelim( buf3, ";", i, buf4, sizeof( buf4));
			getStringFromIndexWithDelim( buf4, "-", 1, buf5, sizeof( buf5));
			skillno=atoi(buf5)-1;
			getStringFromIndexWithDelim( buf4, "-", 2, buf5, sizeof( buf5));
			skillid=atoi(buf5);
			int j;
			for(j=0; j < CHAR_MAXPETHAVE ; j++) {
				petindex = CHAR_getCharPet( talker, j);
				if( petindex == -1  )  continue;
				if(CHAR_getWorkInt(petindex,CHAR_WORKPETSKILLUP)==1){
					pcnt++;
					if(pcnt==1){
						CHAR_setPetSkill( petindex, skillno, skillid);
						if(i==skillnum){
							CHAR_setWorkInt(petindex,CHAR_WORKPETSKILLUP,0);
							char msgbuf[128];
							snprintf( msgbuf, sizeof( msgbuf ), "W%d",j);
							CHAR_sendStatusString( talker, msgbuf );
							CHAR_sendStatusString( talker, "P");
							sprintf(msgbuf,"��ϲ���ġ�%s�����＼�ܽ����ɹ���",CHAR_getChar(petindex,CHAR_NAME));
							CHAR_talkToCli(talker, -1, msgbuf, CHAR_COLORYELLOW);
							CHAR_charSaveFromConnect(talker, FALSE);
							CHAR_talkToCli(talker, -1, "ϵͳΪ���Զ��浵��", CHAR_COLORRED);
							if(NPC_Util_GetStrFromStrWithDelim( buf, "JinHuaMsg", buff2, sizeof( buff2) )!=NULL){
								int k;
								int playernum = CHAR_getPlayerMaxNum();
								char jhmsg[256];
								sprintf(jhmsg,"���輼����������ϲ��ҡ�%s���������%s����Ŀǰ����%s",CHAR_getChar(talker,CHAR_NAME),CHAR_getChar(petindex,CHAR_NAME),buff2);
								for(k=0;k<playernum;k++){
									CHAR_talkToCli( k, -1, jhmsg, CHAR_COLORRED);
								}
							}
						}
						//continue;
					}else{
						if(i==skillnum){
							NPC_EventDelPet(meindex,talker,j);
						}
					}
				}
			}
		}
	}
#endif	
	
#ifdef _EV_NUM	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetEvCode", buff2, sizeof( buff2) )
	!=NULL)
	{
		int evcodeflg;
		evcodeflg=atoi(buff2);
		if(evcodeflg==0){
			NPC_EventSetEvCode(meindex,talker);
		}else{
			char *evcode=NULL;
			evcode = CHAR_getChar(talker,CHAR_EVCODE);
			if(strlen(evcode)<=0) return FALSE;
			char tmpbuf[256];
			memset( tmpbuf, 0, sizeof( tmpbuf ) );
			int evflg;
			if(getStringFromIndexWithDelim(evcode,"|", 2, tmpbuf, sizeof(tmpbuf)) == FALSE)
				return FALSE;
			evflg = atoi(tmpbuf);
			if(evflg<1||evflg>3) return FALSE;
			if(evflg==1){
				NPC_EventSetEvCode(meindex,talker);
			}else if(evflg==2){
				int itemid,itemnum;
				if(getStringFromIndexWithDelim(evcode,"|", 3, tmpbuf, sizeof(tmpbuf)) == FALSE)
					return FALSE;
				itemid=atoi(tmpbuf);
				if(getStringFromIndexWithDelim(evcode,"|", 4, tmpbuf, sizeof(tmpbuf)) == FALSE)
					return FALSE;
				itemnum=atoi(tmpbuf);
				sprintf(tmpbuf,"%d*%d",itemid,itemnum);
				NPC_EventDelItem(meindex,talker,tmpbuf,0);
				NPC_EventSetEvCode(meindex,talker);
			}else if(evflg==3){
				int petpicid,petminlv,petmaxlv,peti,petindex;
				if(getStringFromIndexWithDelim(evcode,"|", 3, tmpbuf, sizeof(tmpbuf)) == FALSE)
					return FALSE;
				petpicid=atoi(tmpbuf);
				if(getStringFromIndexWithDelim(evcode,"|", 5, tmpbuf, sizeof(tmpbuf)) == FALSE)
					return FALSE;
				petminlv=atoi(tmpbuf);
				if(getStringFromIndexWithDelim(evcode,"|", 6, tmpbuf, sizeof(tmpbuf)) == FALSE)
					return FALSE;
				petmaxlv=atoi(tmpbuf);
				for(peti=0;peti<CHAR_MAXPETHAVE;peti++){
					petindex = CHAR_getCharPet( talker, peti);
					if(petindex==-1) continue;
					if(CHAR_getInt(petindex,CHAR_BASEBASEIMAGENUMBER)==petpicid && CHAR_getInt(petindex,CHAR_LV)>=petminlv && CHAR_getInt(petindex,CHAR_LV)<=petmaxlv){
						if(NPC_EventDelPet( meindex, talker,peti) == FALSE) continue;
						NPC_EventSetEvCode(meindex,talker);
					}else{
						continue;
					}
				}
				if(peti>=CHAR_MAXPETHAVE) return FALSE;
			}
		}
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "AddEvNum", buff2, sizeof( buff2) )
	!=NULL)
	{
		int evnum;
		char token[128];
		evnum = atoi(buff2);
		CHAR_setInt(talker,CHAR_EVNUM,CHAR_getInt( talker, CHAR_EVNUM) + evnum);
		sprintf( token, "�ܻ���������%d�������ܻ�����Ϊ%d��", evnum,CHAR_getInt(talker,CHAR_EVNUM));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelEvNum", buff2, sizeof( buff2) )
	!=NULL)
	{
		int evnum;
		char token[128];
		evnum = atoi(buff2);
		CHAR_setInt(talker,CHAR_EVNUM,CHAR_getInt( talker, CHAR_EVNUM) - evnum);
		sprintf( token, "�ܻ���������%d�������ܻ�����Ϊ%d��", evnum,CHAR_getInt(talker,CHAR_EVNUM));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetEvNum", buff2, sizeof( buff2) )
	!=NULL)
	{
		int evnum;
		evnum = atoi(buff2);
		CHAR_setInt(talker,CHAR_EVNUM,evnum);
		CHAR_setInt(talker,CHAR_EVITEM,0);
		CHAR_talkToCli( talker, -1, "�ܻ������Ѿ���գ�", CHAR_COLORWHITE);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetEvThings", buff2, sizeof( buff2) )
	!=NULL)
	{
		struct timeval evt;
		int evflg = atoi(buff2);
		if(evflg==0){
			CHAR_setInt(talker,CHAR_EVITEM,0);
		}else{
			int evi,evj;
			gettimeofday(&evt,NULL);
			evi = RAND(0,1);
			if(evi==0){
				CHAR_setInt(talker,CHAR_EVITEM,0);
			}else{
				evi = RAND(32700,32714);
				CHAR_setInt(talker,CHAR_EVITEM,evi);
			}
		}
	}
#endif
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetVip", buff2, sizeof( buff2) )
	!=NULL)
	{
		int npcvip = atoi(buff2);
		CHAR_setInt(talker,CHAR_VIPRIDE,npcvip);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetSuper", buff2, sizeof( buff2) )
	!=NULL)
	{
		int npcsuper = atoi(buff2);
		CHAR_setInt(talker,CHAR_SUPER,npcsuper);
	}
	
#ifdef _ALL_ITEM
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetAllNum1", buff2, sizeof( buff2) )
	!=NULL)
	{
		sasql_allnum(1,atoi(buff2),1);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetAllNum2", buff2, sizeof( buff2) )
	!=NULL)
	{
		sasql_allnum(2,atoi(buff2),1);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetAllNum3", buff2, sizeof( buff2) )
	!=NULL)
	{
		sasql_allnum(3,atoi(buff2),1);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetAllNum4", buff2, sizeof( buff2) )
	!=NULL)
	{
		sasql_allnum(4,atoi(buff2),1);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "SetAllNum5", buff2, sizeof( buff2) )
	!=NULL)
	{
		sasql_allnum(5,atoi(buff2),1);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "AllNumMsg", buff2, sizeof( buff2) )
	!=NULL)
	{
		int allnumflag;
		int fd = getfdFromCharaIndex( talker);
		allnumflag = atoi(buff2);
		char token[128];
		if(allnumflag == 1)
			sprintf(token,"Ŀǰ���Ѽ���ĿΪ%d",sasql_allnum(allnumflag,0,0));
		else if(allnumflag == 2)
			sprintf(token,"Ŀǰ���Ѽ���ĿΪ%d",sasql_allnum(allnumflag,0,0));
		else if(allnumflag == 3)
			sprintf(token,"Ŀǰ���Ѽ���ĿΪ%d",sasql_allnum(allnumflag,0,0));
		else if(allnumflag == 4)
			sprintf(token,"Ŀǰ���Ѽ���ĿΪ%d",sasql_allnum(allnumflag,0,0));
		else if(allnumflag == 5)
			sprintf(token,"Ŀǰ���Ѽ���ĿΪ%d",sasql_allnum(allnumflag,0,0));
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
								WINDOW_BUTTONTYPE_OK,
								-1,
								-1,
								token);
	}
#endif

	/*--�¼�ĸةʧ��  ةëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetRandItem", buff2, sizeof( buff2) )
	!=NULL)
	{
		char buf3[32];
		j = 1;
		while(getStringFromIndexWithDelim(buff2 , "," , j, buf3, sizeof( buf3))
		 != FALSE )
		{
			j++;
			rand_j++;
		}
		NPC_RandItemGet( meindex, talker, rand_j, buff2);
	}


	/*--ʧ��  ةëܰ������---*/
	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetItem", buff2, sizeof( buff2) )
	!=NULL)
	{
		 NPC_EventAddItem( meindex, talker, buff2);
	}
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "HeChengMsg", buff2, sizeof( buff2) )
	!=NULL)
	{
		int k;
		int playernum = CHAR_getPlayerMaxNum();
		char hcmsg[256];
		sprintf(hcmsg,"���ϳɴ��ˡ�����ϲ��ҡ�%s���ɹ��ϳ�%s",CHAR_getChar(talker,CHAR_NAME),buff2);
		for(k=0;k<playernum;k++){
			CHAR_talkToCli( k, -1, hcmsg, CHAR_COLORRED);
		}
	}
	
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "Save", buff2, sizeof( buff2) )
	!=NULL)
	{
		int savetime;
		int npcsavetime = atoi(buff2);
		savetime = CHAR_getWorkInt( talker, CHAR_WORKSAVETIME)+(60*npcsavetime);
		time_t timep;
		struct tm *p;
		time(&timep);
		p=localtime(&timep);
		timep = mktime(p);
		if(savetime>timep)
			{
				CHAR_talkToCli( talker, -1, "�洢ʱ��δ��", CHAR_COLORRED );
			}
		else
			{
				//int fd = getfdFromCharaIndex( talker);
				CHAR_charSaveFromConnect(talker, FALSE);
				CHAR_setWorkInt(talker,CHAR_WORKSAVETIME,timep);
				CHAR_talkToCli(talker, -1, "ϵͳΪ���Զ��浵��", CHAR_COLORRED);
			}
	}

	/*--�ɷ¶�������˪��--*/
	CHAR_complianceParameter( talker );
	CHAR_send_P_StatusString( talker ,
					CHAR_P_STRING_ATK|CHAR_P_STRING_DEF|
					CHAR_P_STRING_CHARM|CHAR_P_STRING_QUICK|
					CHAR_P_STRING_WATER|CHAR_P_STRING_FIRE|
					CHAR_P_STRING_WIND|CHAR_P_STRING_EARTH
				);
	
	return TRUE;

}


/*--------------------------
 * ��ì����ʸ����ë����
 ---------------------------*/
BOOL NPC_EventDelPet(int  meindex,int  talker, int petsel)
{

	int petindex;
	char szPet[128];
	int defpet;
	char msgbuf[64];

	int fd = getfdFromCharaIndex( talker );

	petindex = CHAR_getCharPet( talker, petsel);

    if( !CHAR_CHECKINDEX(petindex) ) return FALSE;

	if( CHAR_getInt( talker, CHAR_RIDEPET) == petsel ) {

		//CHAR_talkToCli( talker, -1, "����еĳ����޷�������", CHAR_COLORYELLOW );
    	//return	FALSE;

		CHAR_setInt( talker, CHAR_RIDEPET, -1);
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_RIDEPET );
		CHAR_complianceParameter( talker );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( talker , CHAR_WORKOBJINDEX ));
		print(" DelRidePet ");
	}
	print(" EventDelPet ");

	/*--����ƽ�ҷ»���    ئ�գ�ʸ����ë�����ڱ�  --*/
	if( CHAR_getWorkInt( CONNECT_getCharaindex( fd),
   	                     CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return FALSE;
	/*--����ʸ�����������ﾮ����������������--*/
	defpet = CHAR_getInt( talker, CHAR_DEFAULTPET);
	if(defpet == petsel){
		CHAR_setInt( talker, CHAR_DEFAULTPET, -1);
		lssproto_KS_send( fd, -1, TRUE);
	}

	snprintf( msgbuf,sizeof( msgbuf), "����%s��",
									CHAR_getChar( petindex, CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);

	// ʸ����ë��  ���׷��
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"EvnetDell(����ɾ��)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);


	/*   ��  �������� */
	CHAR_setCharPet( talker, petsel, -1);

	CHAR_endCharOneArray( petindex );

	snprintf( szPet, sizeof( szPet ), "K%d", petsel);
	// ޥ����ʸ������  ������˪Ի������
	CHAR_sendStatusString( talker, szPet );

	return TRUE;

}


/*----------------------------
 *  ʸ����ëܰ������
 ------------------------------*/
BOOL NPC_EventAddPet(int meindex, int talker, char *buff2,int mode)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	enemyid;
	int	i;
	int petindex;

	/*--ʸ����  �����л����¾�����������--*/
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  break;
	}

	if(i == CHAR_MAXPETHAVE) return FALSE;


	/* ¦�Ѽ�ID����indexë��   */
	if(strstr(buff2,",") != NULL){
		char buf2[16];
		while(getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2))
		!= FALSE) {
			i++;
		}
		i--;
		i = rand()%i + 1;
		getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2));
		enemyid = atoi( buf2);
	}else{
		enemyid = atoi( buff2);
	}
	
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}

	if( i == enemynum ) return FALSE;

	ret = ENEMY_createPetFromEnemyIndex( talker, i);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( talker, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE ) i = 0;
	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
	}

	petindex = CHAR_getCharPet(talker,i);

    if( !CHAR_CHECKINDEX( petindex) )return FALSE;
	CHAR_complianceParameter( petindex );
	snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
	CHAR_sendStatusString( talker, msgbuf );

	snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
	CHAR_sendStatusString( talker, msgbuf );
	snprintf( msgbuf,sizeof( msgbuf), "�õ�%s��",
								CHAR_getChar(petindex,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);

	// ʸ����ë���  ľ�׷��
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"EventGet(����õ�)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);



	if(mode == 0) {
		/*--��ì�����ü�ʸ����--*/
		CHAR_setInt( petindex, CHAR_ENDEVENT, 1);
	}

	return TRUE;

}

#ifdef _TRANS_7_NPC
BOOL NPC_EventNewTRANS(int meindex, int talker, char *buff2,int mode)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<160){
		sprintf( token, "����������ת��ȼ�С��160��������ת��ʧ��!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
#ifdef _ADD_POOL_ITEM			   // WON ADD ���ӿɼķŵĵ���
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// ���ӵĵ��߼ķ���
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// ���ӵĳ���ķ���
	int j,item_sum=0, pet_sum=0;

	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// �������ļĵ�����
	pet_sum  += 5;							    // �������ļĳ���

#endif
//	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);
	if(CHAR_getInt(talker,CHAR_SUPER)==1)
  	CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+80);
  else 
  	{
  		if(CHAR_getInt(talker,CHAR_VIPRIDE)>0)
  			CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+75);
  		else
  			CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+70);
  	}
  CHAR_Skillupsend( talker );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
	
	CHAR_talkToCli( talker, -1, buff2, CHAR_COLORYELLOW );
}
#ifdef _TRANS7_POINT
BOOL NPC_EventNewTRANSSEVEN(int meindex, int talker, char *buff2,int mode)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<160){
		sprintf( token, "����������ת��ȼ�С��160��������ת��ʧ��!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
#ifdef _ADD_POOL_ITEM			   // WON ADD ���ӿɼķŵĵ���
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// ���ӵĵ��߼ķ���
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// ���ӵĳ���ķ���
	int j,item_sum=0, pet_sum=0;

	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// �������ļĵ�����
	pet_sum  += 5;							    // �������ļĳ���

#endif
//	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);
	CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_SKILLUPPOINT)+getTrans7Point());
  CHAR_Skillupsend( talker );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
	
	CHAR_talkToCli( talker, -1, buff2, CHAR_COLORYELLOW );
}
#endif
BOOL NPC_EventTRANS(int meindex, int talker, char *buff2,int mode)
{
	char token[128];
	int work[10];
	int Trans=CHAR_getInt(talker,CHAR_TRANSMIGRATION)+1;
	if(Trans>7 || CHAR_getInt(talker,CHAR_LV)<80){
		sprintf( token, "����������ת��ȼ�С��80��������ת��ʧ��!", CHAR_getChar( talker, CHAR_NAME));
		CHAR_talkToCli( talker, -1, token, CHAR_COLORYELLOW );
		return;
	}
#ifdef _ADD_POOL_ITEM			   // WON ADD ���ӿɼķŵĵ���
	int tran_pool_item[5] = { 4, 4, 4, 4, 4 };		// ���ӵĵ��߼ķ���
	int tran_pool_pet[5] = { 2, 2, 2, 2, 2};		// ���ӵĳ���ķ���
	int j,item_sum=0, pet_sum=0;

	for(j=0; j<5 ;j++){
		item_sum += tran_pool_item[j] ;			
		pet_sum += tran_pool_pet[j];		
	}

	item_sum += 10;								// �������ļĵ�����
	pet_sum  += 5;							    // �������ļĳ���

#endif
	NPC_TransmigrationStatus(talker, talker, work);
	NPC_TransmigrationFlg_CLS(talker, talker);
	CHAR_setInt(talker ,CHAR_TRANSMIGRATION, Trans);
	CHAR_setInt(talker, CHAR_LV ,1);
	CHAR_setMaxExp( talker, 0);
  CHAR_setInt( talker,CHAR_SKILLUPPOINT,CHAR_getInt( talker, CHAR_TRANSMIGRATION)*10);
  CHAR_Skillupsend( talker );
  CHAR_setInt( talker,CHAR_RIDEPET, -1 );
  CHAR_setInt( talker , CHAR_BASEIMAGENUMBER , CHAR_getInt( talker , CHAR_BASEBASEIMAGENUMBER) );
	CHAR_sendStatusString( talker , "P");
	
	CHAR_talkToCli( talker, -1, buff2, CHAR_COLORYELLOW );
}
#endif	


// Robin add NPC�����ﵰ
BOOL NPC_EventAddEgg(int meindex, int talker, char *buff2,int mode)
{
	int	ret;
	char msgbuf[64];
	int	enemynum;
	int	enemyid;
	int	petid;
	int raise;
	int	i;
	int petindex;
	char buf2[32];
	char buf3[32];

	print("\n ������!!:%s ", buff2);

	/*--ʸ����  �����л����¾�����������--*/
	for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
		petindex = CHAR_getCharPet( talker, i);
		if( petindex == -1  )  break;
	}

	if(i == CHAR_MAXPETHAVE) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}


	/* ¦�Ѽ�ID����indexë��   */
	if(strstr(buff2,",") != NULL){ // �Ƿ�Ϊ�������
		while(getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2))
		!= FALSE) {
			i++;
		}
		i--;
		i = rand()%i + 1;
		getStringFromIndexWithDelim( buff2, ",", i, buf2, sizeof( buf2));

	}else{
		strcpy( buf2, buff2);
	}

	getStringFromIndexWithDelim( buf2, ";", 1, buf3, sizeof( buf3));
	enemyid = atoi( buf3); // ���ﵰID
	getStringFromIndexWithDelim( buf2, ";", 2, buf3, sizeof( buf3));
	petid = atoi( buf3); // �����ĳ���ID
	getStringFromIndexWithDelim( buf2, ";", 3, buf3, sizeof( buf3));
	raise = atoi( buf3); //   ������


	// ��enemyidת��enemybaseid
	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == petid) {
			break;
		}
	}
	if( i == enemynum ) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}
	petid = ENEMY_getInt( i, ENEMY_TEMPNO);


	enemynum = ENEMY_getEnemyNum();
	for( i = 0; i < enemynum; i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == enemynum ) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}

	ret = ENEMY_createPetFromEnemyIndex( talker, i);

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( CHAR_getCharPet( talker, i ) == ret )break;
	}
	if( i == CHAR_MAXPETHAVE ) i = 0;
	if( CHAR_CHECKINDEX( ret ) == TRUE ){
		CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
	}

	petindex = CHAR_getCharPet(talker,i);

    if( !CHAR_CHECKINDEX( petindex) ) {
		print(" AddEgg_Error:%d ", __LINE__);
		return FALSE;
	}

	// ��Ϊ���ﵰ
	CHAR_setInt( petindex, CHAR_FUSIONBEIT, 1);
	CHAR_setInt( petindex, CHAR_FUSIONCODE, -1);
	CHAR_setInt( petindex, CHAR_FUSIONRAISE, raise);
	CHAR_setInt( petindex, CHAR_FUSIONINDEX, petid);
	CHAR_setInt( petindex, CHAR_FUSIONTIMELIMIT, (int)time( NULL));
	
	CHAR_complianceParameter( petindex );
	snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
	CHAR_sendStatusString( talker, msgbuf );

	snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
	CHAR_sendStatusString( talker, msgbuf );
	snprintf( msgbuf,sizeof( msgbuf), "�õ�%s��",
								CHAR_getChar(petindex,CHAR_NAME));
	CHAR_talkToCli( talker, -1, msgbuf,  CHAR_COLORWHITE);

	// ʸ����ë���  ľ�׷��
	LogPet(
		CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
		CHAR_getChar( talker, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"EventGetEgg(����õ�)",
		CHAR_getInt( talker,CHAR_FLOOR),
		CHAR_getInt( talker,CHAR_X ),
		CHAR_getInt( talker,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);



	if(mode == 0) {
		/*--��ì�����ü�ʸ����--*/
		CHAR_setInt( petindex, CHAR_ENDEVENT, 1);
	}
	return TRUE;

}


/*---------------------------
 *  ��ì����  ë��������
 -----------------------------**/
BOOL NPC_EventDelItem(int meindex,int talker,char *buf,int breakflg)
{

	int i = 1, j = 1, k = 1 ;
	char buff3[128];
	char buf2[32];
	int itemindex;
	char token[256];

	while(getStringFromIndexWithDelim(buf , "," , k, buff3, sizeof(buff3))
	 !=FALSE )
	{
		k++;
		if(strstr(buff3,"*") != NULL) {
			int itemno;
			int kosuu;
			int id;
			int cnt=0;
				
			getStringFromIndexWithDelim( buff3, "*", 1, buf2, sizeof( buf2));
			itemno = atoi( buf2); 
			getStringFromIndexWithDelim( buff3, "*", 2, buf2, sizeof( buf2));
			kosuu = atoi( buf2);
	 
			if( breakflg != 1 )
                NPC_ActionDoPileDelItem( talker, itemno, kosuu);//�жѵ��Ĵ���
            else{
			    for( i =0 ; i < CHAR_MAXITEMHAVE ; i++ ){
				    itemindex = CHAR_getItemIndex( talker , i );
				    if( ITEM_CHECKINDEX( itemindex) ) {
					    id = ITEM_getInt( itemindex , ITEM_ID );
					    if(itemno == id) {
						    cnt++;
						    LogItem(
							CHAR_getChar( talker, CHAR_NAME ), // ƽ�ҷ�
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  // ʧ��  ة  į
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker, CHAR_FLOOR),
							CHAR_getInt( talker, CHAR_X ),
 							CHAR_getInt( talker, CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
	     					);
	 
		    				if(breakflg == 1){
			    			    sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							    CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
							}else{
							    sprintf(token,"����%s",ITEM_getChar( itemindex, ITEM_NAME));
							    CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
							}
						    //--ʧ��  ةë��ڽ--
						    CHAR_setItemIndex( talker, i ,-1);
						    ITEM_endExistItemsOne(itemindex);
						    CHAR_sendItemDataOne( talker, i);

						    if(cnt == kosuu){
							    break;
							}
						}
					}
				}
			}
		}
		else{
			/*--���Ϸ�  į��ʧ��  ةë����---*/
			for( j = 0 ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);

				if( ITEM_CHECKINDEX( itemindex)){
					if( atoi( buff3) == ITEM_getInt( itemindex, ITEM_ID)) {
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						if(breakflg == 1){
								sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}else{
							sprintf( token, "����%s��",
											ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}
						/*--ʧ��  ةë��ڽ--*/
						CHAR_setItemIndex( talker, j ,-1);
						ITEM_endExistItemsOne( itemindex);
						/*--�ɷ¶�����˪��--*/
						CHAR_sendItemDataOne( talker, j);
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL NPC_EventDelItemEVDEL(int meindex,int talker,char *buf,char *nbuf,int breakflg)
{

	int k = 1, l = 1;
	char buff3[128];
	char buf2[32];
	char buf4[32];



	while(getStringFromIndexWithDelim(buf , "&" , k, buf4, sizeof(buf4)) !=FALSE ){
		int itemno;
		k++;
		if(strstr(buf4,"ITEM") == NULL) continue;
		getStringFromIndexWithDelim(buf4 , "=" , 2, buff3, sizeof(buff3));

		if(strstr(buff3,"*") != NULL) {
			int kosuu;
#ifndef _ITEM_PILENUMS
			char token[256];
			int i, itemindex, id, cnt;
#endif
			getStringFromIndexWithDelim( buff3, "*", 1, buf2, sizeof( buf2));
			itemno = atoi( buf2); 
			getStringFromIndexWithDelim( buff3, "*", 2, buf2, sizeof( buf2));
			kosuu = atoi( buf2);
			if(strstr(nbuf,"-1") == NULL){
				l = 1;
				while(getStringFromIndexWithDelim(nbuf , "," , l, buf2, sizeof(buf2))){
					l++;
					if(itemno == atoi( buf2)){
						l = -1;
						break;
					}
				}
				if(l == -1) continue;
			}
#ifdef _ITEM_PILENUMS
			NPC_ActionDoPileDelItem( talker, itemno, kosuu);
#else
			cnt = 0;
			for( i =0 ; i < CHAR_MAXITEMHAVE ; i++ ){
				itemindex = CHAR_getItemIndex( talker , i );
				if( ITEM_CHECKINDEX( itemindex) ) {
					id = ITEM_getInt( itemindex , ITEM_ID );
					if(itemno == id) {
						cnt++;
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker, CHAR_FLOOR),
							CHAR_getInt( talker, CHAR_X ),
 							CHAR_getInt( talker, CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						if(breakflg == 1){
							sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}else{
							sprintf(token,"����%s",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}
						CHAR_setItemIndex( talker, i ,-1);
						ITEM_endExistItemsOne(itemindex);
						CHAR_sendItemDataOne( talker, i);
						if(cnt == kosuu){
							break;
						}
					}
				}
			}		
#endif
		}else{
#ifndef _ITEM_PILENUMS
			int j, itemindex;
			char token[256];
#endif
			if(strstr(nbuf,"-1") == NULL){
				l = 1;
				while(getStringFromIndexWithDelim(nbuf , "," , l, buf2, sizeof(buf2))){
					l++;
					if(atoi(buff3) == atoi( buf2)){
						l = -1;
						break;
					}
				}
				if(l == -1) continue;
			}
			itemno = -1;
#ifdef _ITEM_PILENUMS
			NPC_ActionDoPileDelItem( talker, itemno, itemno);
#else
			for( j = 0 ;  j < CHAR_MAXITEMHAVE ; j++){
				itemindex = CHAR_getItemIndex( talker ,j);
				if( ITEM_CHECKINDEX( itemindex)){
					if( atoi( buff3) == ITEM_getInt( itemindex, ITEM_ID)) {
						LogItem(
							CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
							CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
							itemindex,
#else
							ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
							"EventDelItem(�����������ջصĵ���)",
							CHAR_getInt( talker,CHAR_FLOOR),
							CHAR_getInt( talker,CHAR_X ),
							CHAR_getInt( talker,CHAR_Y ),
							ITEM_getChar( itemindex, ITEM_UNIQUECODE),
							ITEM_getChar( itemindex, ITEM_NAME),
							ITEM_getInt( itemindex, ITEM_ID)
						);
						if(breakflg == 1){
							sprintf(token,"%s ����",ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}else{
							sprintf( token, "����%s��",
										ITEM_getChar( itemindex, ITEM_NAME));
							CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);
						}
						CHAR_setItemIndex( talker, j ,-1);
						ITEM_endExistItemsOne( itemindex);
						CHAR_sendItemDataOne( talker, j);
					}
				}
			}
#endif
		}
	}

	return TRUE;
}


/*----------------------
 * ��ì����  ëܰ��]
 -----------------------*/
BOOL NPC_EventAddItem(int meindex,int talker,char *buf)
{

	char buff3[128];
	int i = 1;
	int itemindex;
	char buf3[32];
	int ret;
	char token[256];

	while(getStringFromIndexWithDelim(buf , "," , i, buff3, sizeof( buff3)) 
	!= FALSE )
	{
		i++;
		if(strstr( buff3, "*") != NULL) {
			
			int itemno;
			int kosuu;
			int loop = 0;

			getStringFromIndexWithDelim( buff3, "*", 1, buf3,sizeof( buf3));
			itemno = atoi( buf3);
			getStringFromIndexWithDelim( buff3, "*", 2, buf3,sizeof( buf3));
			kosuu = atoi( buf3);
	
			for(loop = 0 ; loop < kosuu ; loop++) {
				itemindex = ITEM_makeItemAndRegist( itemno);
	
				if(itemindex == -1) return FALSE;
	
				/*ʧ��  ة��ܰ��(  ��ʧ��  ة  ����ľ��������  */
				ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
				if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
				    print("npc_exchange:ACCEPTadditem error itemindex[%d]\n",itemindex);
					ITEM_endExistItemsOne( itemindex);
					return FALSE;
				}

				if(itemindex != -1) {
					LogItem(
						CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
						CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
						itemindex,
#else
	    				ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
						"EventAddItem(�����������õ��ĵ���)",
						CHAR_getInt( talker, CHAR_FLOOR),
						CHAR_getInt( talker, CHAR_X ),
						CHAR_getInt( talker, CHAR_Y ),
						ITEM_getChar( itemindex, ITEM_UNIQUECODE),
						ITEM_getChar( itemindex, ITEM_NAME),
						ITEM_getInt( itemindex, ITEM_ID)
					);
				}
				sprintf( token, "�õ�%s��", ITEM_getChar( itemindex, ITEM_NAME));
				CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

				CHAR_sendItemDataOne( talker, ret);
			}
		}else{
			itemindex = ITEM_makeItemAndRegist( atoi( buff3));
	
			if(itemindex == -1) return FALSE;

			/*ʧ��  ة��ܰ��(  ��ʧ��  ة  ����ľ��������  */
			ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
			if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
				print( "npc_exchange.c: ACCEPTadditem error itemindex[%d]\n", itemindex);
				ITEM_endExistItemsOne( itemindex);
				return FALSE;
			}

			if(itemindex != -1) {
				LogItem(
				CHAR_getChar( talker, CHAR_NAME ), /* ƽ�ҷ�   */
				CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
				itemindex,
#else
	       		ITEM_getInt( itemindex, ITEM_ID),  /* ʧ��  ة  į */
#endif
				"EventAddItem(�����������õ��ĵ���)",
				CHAR_getInt( talker, CHAR_FLOOR),
				CHAR_getInt( talker, CHAR_X ),
 				CHAR_getInt( talker, CHAR_Y ),
				ITEM_getChar( itemindex, ITEM_UNIQUECODE),
				ITEM_getChar( itemindex, ITEM_NAME),
				ITEM_getInt( itemindex, ITEM_ID)
			);
			}
			sprintf(token,"�õ�%s��", ITEM_getChar( itemindex, ITEM_NAME));
			CHAR_talkToCli( talker, -1, token, CHAR_COLORWHITE);

			CHAR_sendItemDataOne( talker, ret);
		}
	}

	return TRUE;
}


/*---------------------------------
 *����ë  ���ƻ����¼�ë  ������
 ------------- ---------------------*/
int NPC_EventFile(int meindex,int talker,char *arg)
{

	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
	char buf[1024*2];
	int i=1;
	int EvNo=0;
	int evch;
	int q=1;
	
	/*--ɬ�ð����̻�ë  ��  ��--*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {

		print("GetArgStrErr");
		return -1;
	}

	/*ɬ�ð����̻�ë�������*/
	while(getStringFromIndexWithDelim( argstr, "EventEnd", i, buf,sizeof( buf))
	!= FALSE)
	{
		CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 0);
		i++;
		/*--��ì�����ϼ����ë������--*/
		NPC_Util_GetStrFromStrWithDelim( buf, "EventNo", buff2,sizeof( buff2) );
				if(strstr(buff2,",") != NULL){
					char buf3[128];
					while(getStringFromIndexWithDelim( buff2, ",", q, buf3, sizeof( buf3))
					!= FALSE) {
						q++;
					}
					q--;
					q = rand()%q + 1;
					getStringFromIndexWithDelim( buff2, ",", q, buf3, sizeof( buf3));
					EvNo = atoi( buf3);
				}else{
					EvNo = atoi( buff2);
				}

		/*--�׷º���������ë�浤����ƥ�������ì��������  �ƻ������������ݳ�--*/
		if(NPC_EventCheckFlg( talker, EvNo) == TRUE) continue;

		/*--ƽ����������������ë�浤--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"Pet_Name", buff2,sizeof( buff2) ) 
		 != NULL)
		{
			CHAR_setWorkInt( talker, CHAR_WORKSHOPRELEVANTSEC, 1);
		}
		/*--ƽ����������������ë�浤--*/
		if(NPC_Util_GetStrFromStrWithDelim( buf,"KeyWord", buff2,sizeof( buff2) ) 
		 != NULL)
		{
			if(CHAR_getWorkInt( talker, CHAR_WORKSHOPRELEVANT) != i ) continue;
		}

		evch = NPC_ExChangeManEventCheck( meindex, talker, buf);

		if(evch != -1) {
		/*--��ì������������������--*/
//		if(NPC_ExChangeManEventCheck( meindex, talker, buf) == TRUE) {
			strcpysafe(arg, sizeof( buf) ,buf);
			return evch;
		}
	}

	return -1;

}


/*---------------------------------
 * ʧ��  ة���������ͷ���ë�浤
 -----------------------------------*/
BOOL NPC_EventReduce(int meindex,int talker,char *buf)
{

	char buf2[512];
	char buf3[256];
	int id = 0;
	int i;
	int itemindex;
	int itemno;
	int kosuu;
	int cnt = 0;
	
	getStringFromIndexWithDelim( buf, "=", 2, buf2, sizeof( buf2));
	getStringFromIndexWithDelim( buf2, "*", 1, buf3, sizeof( buf3));
	itemno = atoi( buf3);
	getStringFromIndexWithDelim( buf2, "*", 2, buf3, sizeof( buf3));
	kosuu = atoi( buf3);
	
	for( i = 0 ; i < CHAR_MAXITEMHAVE ; i++ ){
		itemindex = CHAR_getItemIndex( talker , i );
		if( ITEM_CHECKINDEX( itemindex) ){
			id = ITEM_getInt( itemindex ,ITEM_ID);
			if(itemno == id){
				//change add ���ӶԶѵ����ж�
				int pilenum = ITEM_getInt( itemindex, ITEM_USEPILENUMS);
				if( pilenum )
					cnt+=pilenum;
				else
					cnt++;

				if(cnt >= kosuu){
					return TRUE;
				}
			}
		}
	}

	return FALSE;

}

/*----------------------------------
 *ʸ�����嵩ƽ��ë����������
 ----------------------------------*/
void NPC_EventPetSkill( int meindex, int talker, char *data)
{

	int skill;
	int pet;
	int slot;
	int cost;
	int skillID = 0;
	char buf[64];
	char argstr[1024];
	char msg[512];
	int petindex;
	char msgbuf[128];
	int fd = getfdFromCharaIndex( talker );
	int evcnt;
	
	if( NPC_Util_CharDistance( talker, meindex ) > 2) {
		return;
	}

	evcnt = NPC_EventFile( meindex, talker, argstr);

	if(evcnt == -1) return;

	/*--ɬ�ð����̻�  ������k��  ��������ë  ���ƻ������ּ�ë  ������--*/
//	if(NPC_EventFile( meindex, talker, argstr) == FALSE) return ;

	/*�ͷ���ʧ��������߯�Ȼ����·�����--*/
	/*����  ë�����Ϸ־���(��    į)  ����ʸ��������(��    į)
	����������������(��    į)  ���*/
	makeStringFromEscaped( data);

	/*--����������Ի����--*/
	getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf));
	skill = atoi( buf);
	getStringFromIndexWithDelim( data, "|", 2, buf, sizeof( buf));
	pet = atoi( buf);
	getStringFromIndexWithDelim( data, "|", 3, buf ,sizeof( buf));
	slot = atoi(buf);
	getStringFromIndexWithDelim( data, "|", 4, buf, sizeof( buf));
	cost = atoi( buf);

	/*--��ƽ��    ë����������--*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "pet_skill", 
									msg, sizeof( msg)) != NULL) 
	{
		getStringFromIndexWithDelim( msg, ",", skill, buf ,sizeof( buf));
		skillID = atoi( buf);
	}
	slot--;


	petindex = CHAR_getCharPet( talker, pet-1);

	if(petindex == -1){
		print("PetindexErr");
		return;
	}

	/*--����ƽ�ҷ»���    ئ�գ�ʸ����ë�����ڱ�  --*/
	if( CHAR_getWorkInt( CONNECT_getCharaindex( fd),
                         CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;

	/*--ʸ������  ë����������--*/
	CHAR_setPetSkill( petindex, slot, skillID);
			
	/*--����ëӼ����--*/
	CHAR_setInt( talker, CHAR_GOLD, (CHAR_getInt( talker, CHAR_GOLD) - cost));

	snprintf( msgbuf, sizeof( msgbuf ), "W%d",pet-1);

	// ޥ����ʸ������  ������ë˪Ի������
	CHAR_sendStatusString( talker, msgbuf );
			
	CHAR_sendStatusString( talker, "P");

}


/*----------------------------------------
 *  ʧ��  ة���������о�����������������
----------- ------------------------------*/
BOOL NPC_ItemFullCheck(int meindex,int talker,char *buf,int mode,int evcnt)
{

	char buff2[256];
	int i = 1,j = 1;
	int maxitem = 0;
	int kosuucnt = 0;
	int itemindex;
	char buf3[256];
	int rand_j = 0;
	int rand_cnt = 0;
	char nbuf[256];
	char buff6[16];
	int l = 1;
	
	if(NPC_Util_GetStrFromStrWithDelim( buf, "DelItem", buff2, sizeof( buff2) )	!= NULL ){
		if(strstr(buff2,"EVDEL") != NULL){
			char buff4[64];
			char buff5[32];
			
			i = 1;
			NPC_Util_GetStrFromStrWithDelim( buf, "EVENT", buff2, sizeof( buff2));
			getStringFromIndexWithDelim(buff2, "," , evcnt, buf3, sizeof(buf3));
			
			while(getStringFromIndexWithDelim(buf3, "&" , j, buff4, sizeof(buff4))){
				j++;
				if(strstr(buff4,"ITEM") != NULL) {
					if(strstr(buff4,"*") != NULL) {
						int itemno;

						l = 1;
						getStringFromIndexWithDelim(buff4, "=" , 2, buff5, sizeof(buff5));
						getStringFromIndexWithDelim(buff5, "*" , 1, buff6, sizeof(buff6));
						itemno = atoi(buff6);
						
						if(NPC_Util_GetStrFromStrWithDelim( buf, "NotDel", nbuf, sizeof( nbuf))	!=NULL){
							while(getStringFromIndexWithDelim(nbuf , "," , l,buff6, sizeof(buff6)))
							{
								l++;
								if(itemno == atoi( buff6))
								{
									l = -1;
									break;
								}
							}
						}
						if(l == -1) continue;
						getStringFromIndexWithDelim(buff5, "*" , 2, buff6, sizeof(buff6));
						maxitem -= atoi( buff6);
					}else{
						getStringFromIndexWithDelim(buff4, "=" , 2, buff5, sizeof(buff5));
						l = 1; 

						if(NPC_Util_GetStrFromStrWithDelim( buf, "NotDel", nbuf, sizeof( nbuf))
						!=NULL){
							while(getStringFromIndexWithDelim(nbuf , "," , l,buff6, sizeof(buff6)))	{
								l++;
								if(atoi(buff5) == atoi( buff6)){
									l = -1;
									break;
								}
							}
						}
						if(l == -1) continue;
						for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
							itemindex = CHAR_getItemIndex( talker , i );
							if( ITEM_CHECKINDEX( itemindex)) {
								if(atoi( buff5) == ITEM_getInt( itemindex, ITEM_ID)) {
									maxitem--;
								}
							}
						}
					}
				}
			}
		}else{
			char buff3[128];
			i = 1;
			while(getStringFromIndexWithDelim(buff2, "," , i, buff3, sizeof(buff3)) !=FALSE ){
				i++;
				if(strstr( buff3, "*") != NULL){
					getStringFromIndexWithDelim( buff3, "*", 2, buf3, sizeof( buf3));
					maxitem -= atoi( buf3);
				}else{
					for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
						itemindex=CHAR_getItemIndex( talker , i );
						if( ITEM_CHECKINDEX( itemindex)) {
							if(atoi( buff3) == ITEM_getInt( itemindex, ITEM_ID)) {
							maxitem--;
							}
						}
					}
				}
			}

		}
	}

	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetRandItem", buff2, sizeof( buff2) ) !=NULL && mode == 0){
		j = 1;
		while(getStringFromIndexWithDelim(buff2 , "," , j, buf3, sizeof( buf3)) !=FALSE ){
			j++;
			rand_j++;
		}
		rand_cnt = 1;
		for( i = CHAR_STARTITEMARRAY ; i <CHAR_MAXITEMHAVE ; i++ ) {
			itemindex = CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX(itemindex) ){
				kosuucnt++;
			}
		}
		if( maxitem == 0 && kosuucnt== 0){
			return FALSE;
		}
	}


	if(NPC_Util_GetStrFromStrWithDelim( buf, "GetItem", buff2, sizeof( buff2) )
	!= NULL && mode == 0){
		char buff3[256];
		j = 1;
		while(getStringFromIndexWithDelim(buff2 , "," , j, buff3, sizeof( buff3)) != FALSE ){
			j++;
			if(strstr(buff3,"*") !=NULL ) {
				getStringFromIndexWithDelim( buff3, "*", 2, buf3,sizeof( buf3));
				maxitem += atoi( buf3);
			}else{
				maxitem++;
			}
		}
		kosuucnt=0;
		for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ){
			itemindex = CHAR_getItemIndex( talker , i );
			if( !ITEM_CHECKINDEX( itemindex)){
				kosuucnt++;
			 }
		}
		maxitem = maxitem + rand_cnt;
		if( kosuucnt < maxitem){
			return FALSE;
		}
	}


	return TRUE;
}

void NPC_CharmStatus(int meindex,int talker)
{
	int i = 0;
	int petindex;
	char petsend[64];	
	for(i = 0 ; i < CHAR_MAXPETHAVE ; i++) {
    	petindex = CHAR_getCharPet( talker, i);

		if( petindex == -1  )  continue;
		if( !CHAR_CHECKINDEX( talker ) )  continue;
		CHAR_complianceParameter( petindex );
		sprintf( petsend, "K%d", i );
		CHAR_sendStatusString( talker , petsend );
	}
}

int NPC_EventGetCost(int meindex,int talker,char *arg)
{

	int cost;
	int level;
	char buf[32];
	if(strstr( arg, "LV") != NULL) {
		level = CHAR_getInt( talker, CHAR_LV);
		getStringFromIndexWithDelim( arg, "*", 2, buf,sizeof( buf));
		cost = level * atoi( buf);
	}else{
		cost = atoi( arg);
	}
	return cost;

}

int NPC_EventTime(void)
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep); /*ȡ�õ���ʱ��*/
	year=1900+p->tm_year;
	mon=1+p->tm_mon;
	date=p->tm_mday;
	day=p->tm_wday;
	hour=p->tm_hour;
	min=p->tm_min;
	sec=p->tm_sec;
}

#ifdef	_EV_NUM
BOOL NPC_EventSetEvCode(int  meindex,int  talker)
{
	int evcodeflg,npcno,evtmp;
	char tmpbuf[256];
	char *evbuf=NULL;
	npcno=RAND(1,50);
	evtmp = RAND(1,100);
	if(evtmp<=60){
		evcodeflg=1;
	}else if(evtmp>60 && evtmp<=80){
		evcodeflg=2;
	}else{
		evcodeflg=3;
	}
	if(evcodeflg==1){
		sprintf(tmpbuf,"%d|%d",npcno,evcodeflg);
		CHAR_setChar(talker,CHAR_EVCODE,tmpbuf);
	}else if(evcodeflg==2){
		evbuf=sasql_ev_item();
		if(strcmp(evbuf,"err")==0){
			return FALSE;
		}else{
			sprintf(tmpbuf,"%d|%d|%s",npcno,evcodeflg,evbuf);
			CHAR_setChar(talker,CHAR_EVCODE,tmpbuf);
		}
	}else if(evcodeflg==3){
		evbuf=sasql_ev_pet();
		if(strcmp(evbuf,"err")==0){
			return FALSE;
		}else{
			sprintf(tmpbuf,"%d|%d|%s",npcno,evcodeflg,evbuf);
			CHAR_setChar(talker,CHAR_EVCODE,tmpbuf);
		}
	}
	return TRUE;
}
#endif
