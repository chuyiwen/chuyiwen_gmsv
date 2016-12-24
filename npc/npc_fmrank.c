#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "npc_fmrank.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "battle.h"
#include "log.h"
#include "enemy.h"
#include "handletime.h"
#include "npc_eventaction.h"
#include "npc_healer.h"
#include "configfile.h"
#include "sasql.h"

extern char fmrankname[20][50];
extern int fmrankpoint[20];
int fmrankcnt=0;
enum {
	WINDOW_START=1,
	WINDOW_SELECT,
	WINDOW_FMRANKTOP_NEXT,
	WINDOW_FMRANKTOP_PREV,
};


enum {
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT1,
	NPC_WORK_TIMEFLAG = CHAR_NPCWORKINT9,
/*
	NPC_WORK_ROUTETOY = CHAR_NPCWORKINT2,
	NPC_WORK_ROUTEPOINT = CHAR_NPCWORKINT3,
	NPC_WORK_ROUNDTRIP = CHAR_NPCWORKINT4,
	NPC_WORK_MODE = CHAR_NPCWORKINT5,
	NPC_WORK_CURRENTROUTE = CHAR_NPCWORKINT6, 
	NPC_WORK_ROUTEMAX = CHAR_NPCWORKINT7,
	NPC_WORK_WAITTIME = CHAR_NPCWORKINT8,
	NPC_WORK_CURRENTTIME = CHAR_NPCWORKINT9,
	NPC_WORK_SEFLG = CHAR_NPCWORKINT10,
*/
};
enum {
	NPC_FMRANK_START,
	NPC_FMRANK_SELECT,
	NPC_FMRANK_ADDPOINT,
	NPC_FMRANK_ADDFM,
	NPC_FMRANK_TOP20,
};
#define STANDBYTIME 50

static void NPC_FmRank_selectWindow( int meindex, int toindex, int num,int select);

BOOL NPC_FmRankInit( int meindex )
{
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
		
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return FALSE;
	}

//  	CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TRANSERMANS);

	//CHAR_setInt( meindex, CHAR_LOOPINTERVAL, NEWNPCMAN_STANDBY);
	CHAR_setWorkInt( meindex, NPC_WORK_CURRENTTIME, NowTime.tv_sec);
	return TRUE;
}
//CHAR_setWorkInt(talkerindex,CHAR_WORKSHOPRELEVANT,1);
void NPC_FmRankTalked( int meindex, int talkerindex, char *msg, int color )
{
	if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER )
    		return;
	if(NPC_Util_isFaceToFace( meindex ,talkerindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( talkerindex, meindex, 1) == FALSE)
			return;
	}

	CHAR_setWorkInt( talkerindex, CHAR_WORKSHOPRELEVANT, 0);
	NPC_FmRank_selectWindow( meindex, talkerindex, WINDOW_START, 0);
}

void NPC_FmRankWindowTalked( int meindex, int talkerindex, int seqno,
								int select, char *data)
{
	int myfmindex=-1;
	if( select == WINDOW_BUTTONTYPE_CANCEL || select == WINDOW_BUTTONTYPE_NO)
		return;
	switch( seqno)	{
	case NPC_FMRANK_START:
		break;
	case NPC_FMRANK_SELECT:
		NPC_FmRank_selectWindow( meindex, talkerindex, WINDOW_SELECT, atoi( data));
		break;
	case NPC_FMRANK_ADDPOINT:
		if(atoi(data)<1000 || atoi(data)>1000000){
			CHAR_talkToCli( talkerindex, -1,"��׷�ӵĵ���С��1000���ֻ����100W���֣�",CHAR_COLORRED);
			return;
		}
		if(sasql_ampoint(CHAR_getChar(talkerindex,CHAR_CDKEY),0,0)<atoi(data)){
			CHAR_talkToCli( talkerindex, -1,"���ġ����˻��֡����ھ��۵ĵ�����",CHAR_COLORRED);
			return;
		}
		myfmindex = CHAR_getInt(talkerindex, CHAR_FMINDEX);
		if(myfmindex>0){
			int myfmpoint = sasql_fmpoint_query(myfmindex);
			if(myfmpoint>0){
				if(sasql_fmpoint_add(myfmindex,(myfmpoint+atoi(data)))==1){
					sasql_ampoint(CHAR_getChar(talkerindex,CHAR_CDKEY),-atoi(data),1);
					CHAR_talkToCli( talkerindex, -1,"���Ѹ��Լ��ļ��徺�۵����ɹ���",CHAR_COLORRED);
					return;
				}
			}
		}
		break;
	case NPC_FMRANK_ADDFM:
		if(atoi(data)<5000 || atoi(data)>1000000){
			CHAR_talkToCli( talkerindex, -1,"���״ξ��۵���С��5000���ֻ����100W���֣�",CHAR_COLORRED);
			return;
		}
		if(sasql_ampoint(CHAR_getChar(talkerindex,CHAR_CDKEY),0,0)<atoi(data)){
			CHAR_talkToCli( talkerindex, -1,"���ġ����˻��֡����ھ��۵ĵ�����",CHAR_COLORRED);
			return;
		}
		myfmindex = CHAR_getInt(talkerindex, CHAR_FMINDEX);
		if(myfmindex>0){
			int myfmpoint = sasql_fmpoint_query(myfmindex);
			char* fmname = CHAR_getChar(talkerindex,CHAR_FMNAME);
			if(myfmpoint<=0){
				if(sasql_fmindex_add(myfmindex,fmname,atoi(data))==1){
					sasql_ampoint(CHAR_getChar(talkerindex,CHAR_CDKEY),-atoi(data),1);
					CHAR_talkToCli( talkerindex, -1,"���Ѹ��Լ��ļ��徺�۵����ɹ���",CHAR_COLORRED);
					return;
				}
			}
		}
		break;
	case NPC_FMRANK_TOP20:
		if( select == WINDOW_BUTTONTYPE_CANCEL )
			return;
		if( select == WINDOW_BUTTONTYPE_NEXT )
		{
			if(fmrankcnt>7){
				NPC_FmRank_selectWindow( meindex, talkerindex, WINDOW_FMRANKTOP_NEXT, atoi( data));
				return;
			}
		}
		if( select == WINDOW_BUTTONTYPE_PREV )
		{
			if(fmrankcnt>7){
				NPC_FmRank_selectWindow( meindex, talkerindex, WINDOW_FMRANKTOP_PREV, atoi( data));
				return;
			}
		}
		break;
	}
}

static void NPC_FmRank_selectWindow( int meindex, int toindex, int num,int select)
{
	int myfmindex=-1;
	char npcarg[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256];
	char buff[128];
	int buttontype = 0, windowtype = 0, windowno = 0;
	int fd = getfdFromCharaIndex( toindex);
	if(NPC_Util_isFaceToFace( meindex ,toindex , 2) == FALSE) {
		if(NPC_Util_isFaceToChara( toindex, meindex, 1) == FALSE)
			return;
	}

	memset( npcarg, 0, sizeof( npcarg));
	if(NPC_Util_GetArgStr( meindex, npcarg, sizeof(npcarg))==NULL){
		print("TRANSER_MAN: GetArgStrErr!!");
		return;
	}
	memset( token, 0, sizeof( token));

	switch( num)	{
	case WINDOW_START:
		sprintf(token, "3\n\n              ����徺����������Ա��\n\n"
											"                 ����������ϵͳ��\n"
											"                 ��׷�Ӿ��۵�����\n"
											"                 ���鿴�������Ρ�\n"
											"                 ���鿴ǰ��ʮ����\n"
											"                 ���������ܽ��ܡ�\n");
		CHAR_setWorkInt( toindex, CHAR_WORKSHOPRELEVANT, WINDOW_START);
		windowtype = WINDOW_MESSAGETYPE_SELECT;
		buttontype = WINDOW_BUTTONTYPE_CANCEL;
		windowno = NPC_FMRANK_SELECT;
		break;
	case WINDOW_SELECT:
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		if(select==1 || select == 2){
			myfmindex = CHAR_getInt(toindex, CHAR_FMINDEX);
			if(myfmindex>0){
				int myfmpoint = sasql_fmpoint_query(myfmindex);
				windowtype = WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT;
				if(myfmpoint>0){
					sprintf(token, "              ����徺����������Ա��\n\n"
				             	 "���ļ����Ѿ���������ϵͳ\n"
				             	 "���ڵľ��۵����ǣ�%d�㣡\n\n"
				             	 "������������\n"
				             	 "�����·���д��׷�ӵ�����㡾ȷ����",myfmpoint);
					buttontype = WINDOW_BUTTONTYPE_YESNO;
					windowno = NPC_FMRANK_ADDPOINT;
	      }else{
	      	sprintf(token, "              ����徺����������Ա��\n\n"
				             	 "���ļ��廹δ��������ϵͳ��\n\n"
				             	 "�����������ϵͳ\n"
				             	 "�����·���д���۵�����㡾ȷ����");
					buttontype = WINDOW_BUTTONTYPE_YESNO;
					windowno = NPC_FMRANK_ADDFM;
	      }
			}else{
				sprintf(token, "              ����徺����������Ա��\n\n"
				               "����û�м�����߳������壬���м�����ٽ��д˲�����");
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
			}
		}else if(select==3){
			myfmindex = CHAR_getInt(toindex, CHAR_FMINDEX);
			if(myfmindex>0){
				int myfmpoint = sasql_fmpoint_query(myfmindex);
				if(myfmpoint>0){
					int myfmno = sasql_fmno_query(myfmindex);
					sprintf(token, "              ����徺����������Ա��\n\n"
				             	 "���ļ����Ѿ���������ϵͳ\n\n"
				             	 "���ڵľ��۵����ǣ�%d��\n\n"
				             	 "��������%dλ��",myfmpoint,myfmno);
					buttontype = WINDOW_BUTTONTYPE_CANCEL;
				}else{
					sprintf(token, "              ����徺����������Ա��\n\n"
				             	 "���ļ��廹δ��������ϵͳ��\n");
					buttontype = WINDOW_BUTTONTYPE_CANCEL;
				}
			}else{
				sprintf(token, "              ����徺����������Ա��\n\n"
				               "����û�м�����߳������壬���м�����ٽ��д˲�����");
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
			}
		}else if(select==4){
			CHAR_setWorkInt(toindex,CHAR_WORKLISTPAGE,1);
			sprintf(token, "               ��ǰ20�����������б��\n");
			fmrankcnt = sasql_fm_query();
			if(fmrankcnt>0){
				int fmshowmax;
				if(fmrankcnt>7) fmshowmax=7;
				else fmshowmax = fmrankcnt;
				int i;
				for(i=0;i<fmshowmax;i++){
					sprintf(buff,"%d.%s   ���۵�����%d\n",i+1,fmrankname[i],fmrankpoint[i]);
					strcat(token,buff);
				}
			}
			if(fmrankcnt>7)
				buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
			else
				buttontype = WINDOW_BUTTONTYPE_CANCEL;
			windowno = NPC_FMRANK_TOP20;
		}else if(select==5){
			sprintf(token, "\n1�����þ���������ʽ    ������\n"
										 "2��ֻ��ʾǰ20λ����    ���ۡ�\n"
										 "3����������Ȼ����ǰ    ���š�\n"
										 "4�����ٳ���Ϊһ�ٵ�    ������\n"
										 "5��ˢ��ʱ��Ϊ15����    ��ϵ��\n"
										 "6��ÿ�����һ�����    ��ͳ��\n");
			buttontype = WINDOW_BUTTONTYPE_CANCEL;
		}
		break;
	case WINDOW_FMRANKTOP_NEXT:
		if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)<1) return;
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		sprintf(token, "               ��ǰ20�����������б��\n");
			if(fmrankcnt>0){
				int fmshowmax;
				if(fmrankcnt<=7) return;
				if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)==1){
					if(fmrankcnt>14){
						fmshowmax = 14;
						buttontype = WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
					}
					else{ 
						fmshowmax = fmrankcnt;
						buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_PREV;
					}
				}else if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)==2){
					if(fmrankcnt<=14) return;
					fmshowmax = fmrankcnt;
					buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_PREV;
				}else{
					return;
				}
				int i;
				for(i=7*CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE);i<fmshowmax;i++){
					sprintf(buff,"%d.%s   ���۵�����%d\n",i+1,fmrankname[i],fmrankpoint[i]);
					strcat(token,buff);
				}
				CHAR_setWorkInt(toindex,CHAR_WORKLISTPAGE,CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)+1);
				windowno = NPC_FMRANK_TOP20;
			}else
				return;
		break;
	case WINDOW_FMRANKTOP_PREV:
		if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)>3) return;
		windowtype = WINDOW_MESSAGETYPE_MESSAGE;
		sprintf(token, "               ��ǰ20�����������б��\n");
			if(fmrankcnt>0){
				int fmshowmax;
				if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)==3){
					if(fmrankcnt<=14) return;
					fmshowmax = 14;
					buttontype = WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_NEXT;
				}else if(CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)==2){
					if(fmrankcnt<=7) return;
					fmshowmax = 7;
					buttontype = WINDOW_BUTTONTYPE_CANCEL|WINDOW_BUTTONTYPE_NEXT;
				}
				int i;
				for(i=fmshowmax-7;i<fmshowmax;i++){
					sprintf(buff,"%d.%s   ���۵�����%d\n",i+1,fmrankname[i],fmrankpoint[i]);
					strcat(token,buff);
				}
				CHAR_setWorkInt(toindex,CHAR_WORKLISTPAGE,CHAR_getWorkInt(toindex,CHAR_WORKLISTPAGE)-1);
				windowno = NPC_FMRANK_TOP20;
			}else
				return;
		break;
	}
	lssproto_WN_send( fd, windowtype, buttontype, windowno,
		CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX), token);
}


void NPC_FmRankLoop( void )
{
}

