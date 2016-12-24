#include "version.h"
#include <time.h>
#include "char.h"
#include "object.h"
#include "lssproto_serv.h"
#include "npcutil.h"
#include "handletime.h"
#include "npc_fmchallenge.h"
#include "family.h"

// ȫ���ļ��� pk ����
enum {
	NPC_WORK_ID = CHAR_NPCWORKINT1,		// �Ǽ�Ա ID, �� 0 ��ʼ
};

enum {
	CHAR_WORK_PAGE = CHAR_WORKSHOPRELEVANT,		// ��ҳ��
	CHAR_WORK_DUELTIME = CHAR_WORKSHOPRELEVANTSEC,	// ��ѡ��� pk ʱ��
};

#define SCHEDULEFILEDIR		"./Schedule/"

void NPC_LoadPKSchedule(int meindex);	// Load schedule from disk
void NPC_SavePKSchedule(int meindex);	// save schedule to disk
// �����ų̱��� data
void NPC_LIST_gendata(int meindex, int talkerindex, int page, char *buf, int size);
// ����ѡ������ data
void NPC_SELECT_gendata(int meindex, int page, char *buf, int size);
// �����ų���ϸ�� data
void NPC_DETAIL_gendata(int meindex, char *buf, int size, int dueltime);

BOOL NPC_SchedulemanInit( int meindex )
{
  char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
  int meid;
  int interval;

  CHAR_setInt( meindex, CHAR_WHICHTYPE, CHAR_TYPEFMSCHEDULEMAN );
  CHAR_setWorkInt ( meindex, NPC_WORK_PREVIOUSCHECKTIME, -1);

  // ����
  NPC_Util_GetArgStr(meindex, argstr, sizeof(argstr));
  meid = NPC_Util_GetNumFromStrWithDelim(argstr, "id" );
  if ((meid<0) || (meid>=MAX_SCHEDULEMAN)) {
    print("SCHEDULEMAN init error: invalid ID(%d)\n",meid);
    meid=0;
  }
  CHAR_setWorkInt(meindex, NPC_WORK_ID, meid);

  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "loopinterval" );
  if ((interval<100) || (interval>10000)) interval=1000;
  CHAR_setInt(meindex, CHAR_LOOPINTERVAL, interval);

  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "challengetimeout");
  if ((interval<60) || (interval>60*60)) interval=20*60;
  CHAR_setWorkInt(meindex, NPC_WORK_CHALLENGETIMEOUT, interval);
  
  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "settingtimeout");
  if ((interval<10) || (interval>60*60)) interval=60;
  CHAR_setWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT, interval);

  interval = NPC_Util_GetNumFromStrWithDelim(argstr, "fightinterval");
  if ((interval<5) || (interval>60)) interval = 50;
  CHAR_setWorkInt(meindex, NPC_WORK_FIGHTINTERVAL, interval);

  NPC_LoadPKSchedule(meindex);

  return TRUE;
}

void NPC_SchedulemanTalked(int meindex, int talkerindex, char *msg, int color)
{
//  CHAR_talkToCli(talkerindex, meindex, "��ã����Ǽ���У˵Ǽ�Ա��", color);
  char buf[4096];
  int fd;

  if (NPC_Util_CharDistance(meindex, talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;

  CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, 0);
  NPC_LIST_gendata(meindex, talkerindex, 0, buf, sizeof(buf));
  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULELIST,
        		WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_CANCEL,
        		CHAR_WINDOWTYPE_SCHEDULEMAN_START,
			CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
			buf);
}

void NPC_SchedulemanWindowTalked(int meindex, int talkerindex,
				int seqno, int select, char *data)
{
  int page;
  int buttontype;
  char buf[4096],token[256];
  int fd,i,dt,a;
  int fmpks_pos;

  if (NPC_Util_CharDistance(meindex,talkerindex) > 2) return;
  if (!NPC_Util_isFaceToFace(meindex, talkerindex, 2)) return;
  fd = getfdFromCharaIndex(talkerindex);
  if (fd == -1) return;
  fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

//  print("seqno=%d select=%d data=%s\n", seqno,select,data);

  switch (seqno) {
  case CHAR_WINDOWTYPE_SCHEDULEMAN_START:
    switch (select) {
    case WINDOW_BUTTONTYPE_NEXT:
    case WINDOW_BUTTONTYPE_PREV:
      page=CHAR_getWorkInt(talkerindex, CHAR_WORK_PAGE)
           + MAXSCHEDULEINONEWINDOW
             * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
      buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_CANCEL;
      if (page<0) {
        page=0;
        buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_CANCEL;
      }
      if (page>MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW) {
        page=MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW;
        buttontype=WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_CANCEL;
      }
      CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, page);
      // Send WN
      NPC_LIST_gendata(meindex, talkerindex, page, buf, sizeof(buf));
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULELIST,
        		   buttontype,
        		   CHAR_WINDOWTYPE_SCHEDULEMAN_START,
			   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
			   buf);
      break;
    case WINDOW_BUTTONTYPE_OK:
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
          (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)) {
        // ֻ���ѳ���������峤����ʹ�ý�һ���Ĺ��� (�趨�����ġ�ͬ��)

        // decide: send family list or detail or accept
        dt=atoi(data);
        for (i=0; i<MAX_SCHEDULE; i++) {
          if (fmpks[fmpks_pos+i].dueltime==dt) {
            if (i==0) {
              CHAR_talkToCli(talkerindex, meindex,
                "���ʱ���޷�����ԤԼ��", CHAR_COLORWHITE);
              break;
            }


            switch (fmpks[fmpks_pos+i].flag) {
            case FMPKS_FLAG_NONE:
              if (NPC_AlreadyScheduled(meindex, talkerindex)) {
                // ͬһ������ֻ����һ���ų�
                CHAR_talkToCli(talkerindex, meindex,
                  "���Ѿ����Ź�һ��ս������ʱ��������������ɡ�", CHAR_COLORWHITE);
              } else {
                fmpks[fmpks_pos+i].host_index=CHAR_getInt(talkerindex, CHAR_FMINDEX);
                strcpy(fmpks[fmpks_pos+i].host_name,makeStringFromEscaped(CHAR_getChar(talkerindex, CHAR_FMNAME)));
                
//                print("CHAR_FMNAME: %s\n",fmpks[fmpks_pos+i].host_name);
                
                fmpks[fmpks_pos+i].guest_index=-1;
                strcpy(fmpks[fmpks_pos+i].guest_name," ");
                fmpks[fmpks_pos+i].prepare_time=5;	// default
#ifdef	_FMWAR_PLAYERNUM
                fmpks[fmpks_pos+i].max_player=getFmWarPlayerNum();	// default
#else
								fmpks[fmpks_pos+i].max_player=50;	// default
#endif                
                fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SETTING;
                fmpks[fmpks_pos+i].setting_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT);
                // �ͳ�ѡ�������б�
                CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, 1); // page 1
                CHAR_setWorkInt(talkerindex, CHAR_WORK_DUELTIME, dt);
                NPC_SELECT_gendata(meindex, 1, buf, sizeof(buf));
                buttontype=0;
                if (familyNumTotal>MAXFAMILYINONEWINDOW)
                  buttontype |= WINDOW_BUTTONTYPE_NEXT;
                lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULESELECTFAMILY,
        			   buttontype,
        			   CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
              }
              break;
            case FMPKS_FLAG_CHALLENGE:
              { // �����峤���������������Ͷ�����ͬ�� pk�������˹���
                int tkfmindex=CHAR_getInt(talkerindex, CHAR_FMINDEX);
                if (tkfmindex==fmpks[fmpks_pos+i].host_index) {
                  fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SETTING;
                  fmpks[fmpks_pos+i].setting_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT);
                  // �ͳ�ϸ���������б�
                  CHAR_setWorkInt(talkerindex, CHAR_WORK_DUELTIME, dt);
                  NPC_DETAIL_gendata(meindex, buf, sizeof(buf), dt);
                  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL,
        	  		   WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL,
        	 		   CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
                } else if (tkfmindex==fmpks[fmpks_pos+i].guest_index) {
                  fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SCHEDULED;
  		  page=CHAR_getWorkInt(talkerindex, CHAR_WORK_PAGE);
   		  buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_OK;
		  if (page<0) {
		    page=0;
		    buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_OK;
		  }
		  if (page>MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW) {
 		    page=MAX_SCHEDULE-MAXSCHEDULEINONEWINDOW;
		    buttontype=WINDOW_BUTTONTYPE_PREV | WINDOW_BUTTONTYPE_OK;
		  }
                  CHAR_talkToCli(talkerindex, meindex,
                      "��ļ����Ѿ�������ս��", CHAR_COLORWHITE);
                  // �����ͳ� list
      		  NPC_LIST_gendata(meindex, talkerindex, page, buf, sizeof(buf));
		  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULELIST,
        			   buttontype,
        			   CHAR_WINDOWTYPE_SCHEDULEMAN_START,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
                }
              }
              break;
            case FMPKS_FLAG_SETTING:
              { // �����峤������������
                int tkfmindex=CHAR_getInt(talkerindex, CHAR_FMINDEX);
                if (tkfmindex==fmpks[fmpks_pos+i].host_index) {
                  fmpks[fmpks_pos+i].flag=FMPKS_FLAG_SETTING;
                  fmpks[fmpks_pos+i].setting_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_SETTINGTIMEOUT);
                  // �ͳ�ϸ���������б�
                  CHAR_setWorkInt(talkerindex, CHAR_WORK_DUELTIME, dt);
                  NPC_DETAIL_gendata(meindex, buf, sizeof(buf), dt);
                  lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL,
        	  		   WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL,
        	 		   CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
                }
              }
            }
            break;
          }
        }
        NPC_SavePKSchedule(meindex);
      } else {
  
			if (CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)!=FMMEMBER_LEADER) {        
			  CHAR_talkToCli(talkerindex, meindex,
				"ֻ���峤����ԤԼ����У�ม�", CHAR_COLORWHITE);
			} else if (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)!=1) {
			  CHAR_talkToCli(talkerindex, meindex,
				"��ļ��廹û����ʽ����ม�", CHAR_COLORWHITE);
			}
      }

      break;
    }
    break;
  case CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT:
    switch (select) {
    case WINDOW_BUTTONTYPE_PREV:
    case WINDOW_BUTTONTYPE_NEXT:
      page=CHAR_getWorkInt(talkerindex, CHAR_WORK_PAGE)
           + MAXFAMILYINONEWINDOW
             * (( select == WINDOW_BUTTONTYPE_NEXT) ? 1 : -1);
      buttontype=WINDOW_BUTTONTYPE_NEXT | WINDOW_BUTTONTYPE_PREV;
      if (page<1) {
        page=1;
        buttontype=WINDOW_BUTTONTYPE_NEXT;
      }
      if (page>familyNumTotal-MAXFAMILYINONEWINDOW+1) {
        page=familyNumTotal-MAXFAMILYINONEWINDOW+1;
        buttontype=WINDOW_BUTTONTYPE_PREV;
      }
      CHAR_setWorkInt(talkerindex, CHAR_WORK_PAGE, page);
      // Send WN
      NPC_SELECT_gendata(meindex, page, buf, sizeof(buf));
      lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULESELECTFAMILY,
        		   buttontype,
        		   CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT,
			   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
			   buf);
      break;
    case WINDOW_BUTTONTYPE_OK:
#ifdef _FMVER21
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
#else
      if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&
#endif      
          (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)) {
        dt=CHAR_getWorkInt(talkerindex, CHAR_WORK_DUELTIME);
        for (i=0; i<MAX_SCHEDULE; i++) {
          if (fmpks[fmpks_pos+i].dueltime==dt) {
            if (fmpks[fmpks_pos+i].host_index==CHAR_getInt(talkerindex, CHAR_FMINDEX)) {
              if (getStringFromIndexWithDelim(data,"|",1,token,sizeof(token))) {
                a=atoi(token);
                if (a!=fmpks[fmpks_pos+i].host_index) {
                  fmpks[fmpks_pos+i].guest_index=a;
                  // ����Ҫ�ټ���Ƿ�������ͶӼ��� /**/
                  if (getStringFromIndexWithDelim(data,"|",2,token,sizeof(token))) {
                    strcpy(fmpks[fmpks_pos+i].guest_name, makeStringFromEscaped(token));
                    // �ͳ� detail �༭��
                    NPC_DETAIL_gendata(meindex, buf, sizeof(buf), dt);
                    lssproto_WN_send(fd, WINDOW_MESSAGETYPE_PKSCHEDULEDETAIL,
        	  		   WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL,
        	 		   CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL,
				   CHAR_getWorkInt(meindex, CHAR_WORKOBJINDEX),
				   buf);
                  }
                }
              }
            }
          }
        }
      }
      break;
    }
    break;
  case CHAR_WINDOWTYPE_SCHEDULEMAN_DETAIL:
#ifdef _FMVER21  
    if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==FMMEMBER_LEADER) &&
#else
    if ((CHAR_getInt(talkerindex,CHAR_FMLEADERFLAG)==1) &&
#endif
        (CHAR_getWorkInt(talkerindex, CHAR_WORKFMSETUPFLAG)==1)) {
      dt=CHAR_getWorkInt(talkerindex, CHAR_WORK_DUELTIME);
      for (i=0; i<MAX_SCHEDULE; i++) {
        if (fmpks[fmpks_pos+i].dueltime==dt) {
          if (fmpks[fmpks_pos+i].host_index==CHAR_getInt(talkerindex, CHAR_FMINDEX)) {
            switch (select) {
            case WINDOW_BUTTONTYPE_OK:
              {
                // ׼��ʱ��
                if (getStringFromIndexWithDelim(data,"|",4,token,sizeof(token))) {
                  a=atoi(token);
                  if ((a>0) && (a<=40)) fmpks[fmpks_pos+i].prepare_time=a;
                }
                // �������
                if (getStringFromIndexWithDelim(data,"|",5,token,sizeof(token))) {
                  a=atoi(token);
#ifdef	_FMWAR_PLAYERNUM
                  if ((a>0) && (a<=getFmWarPlayerNum())) fmpks[fmpks_pos+i].max_player=a;
#else
									if ((a>0) && (a<=50)) fmpks[fmpks_pos+i].max_player=a;
#endif
                }
                fmpks[fmpks_pos+i].flag=FMPKS_FLAG_CHALLENGE;
                fmpks[fmpks_pos+i].challenge_timeout=
                    CHAR_getWorkInt(meindex, NPC_WORK_CHALLENGETIMEOUT);
                CHAR_talkToCli(talkerindex, meindex,
                    "������ս�趨��ɡ�", CHAR_COLORWHITE);
              }
              break;
            case WINDOW_BUTTONTYPE_CANCEL:
              fmpks[fmpks_pos+i].flag=FMPKS_FLAG_NONE;
              CHAR_talkToCli(talkerindex, meindex,
                  "���������ս��", CHAR_COLORWHITE);
              break;
            }
            NPC_SavePKSchedule(meindex);
          }
        }
      }
    }
    break;
  }
}

void NPC_SchedulemanLoop(int meindex)
{
//  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

/*
  print("Scheduleman: fmpks_pos=%d dueltime=%d host=%s guest=%s\n",
       fmpks_pos, fmpks[fmpks_pos].dueltime, fmpks[fmpks_pos].host_name, fmpks[fmpks_pos].guest_name);
*/

  NPC_RemoveExpiredBattle(meindex);
  NPC_ProcessTimeout(meindex);
}

// �Ƴ����ڵ�ս��
void NPC_RemoveExpiredBattle(int meindex)
{
  struct tm tm1;
  int keeptime;
  int i,expired=-1;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  int prevckt = CHAR_getWorkInt(meindex, NPC_WORK_PREVIOUSCHECKTIME);
  int fin = CHAR_getWorkInt(meindex, NPC_WORK_FIGHTINTERVAL);
  int h,d;

  // ��Ŀǰ��ʱ�̵���׼ȥ�Ƴ����ڵ��ų�
  memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
/*
  if (tm1.tm_hour==0)
    keeptime=0;
  else
    keeptime=(tm1.tm_hour-1)*100 + tm1.tm_min;
*/
  if (tm1.tm_min<fin)
    keeptime = (tm1.tm_hour-1)*100 + 60 - (fin - tm1.tm_min);
  else
    keeptime = tm1.tm_hour*100 + tm1.tm_min - fin;

  CHAR_setWorkInt(meindex, NPC_WORK_PREVIOUSCHECKTIME, keeptime);
  if (keeptime<prevckt) keeptime+=10000;	// ����

  // ������Щ�ǹ��ڵ�
  i=0;
  while ((i<MAX_SCHEDULE) && (fmpks[fmpks_pos+i].dueltime<=keeptime)) {
    expired=i;
    i++;
  }
  
  if (expired>=0) {
    // �ƶ�������ų���ȡ���Ѿ�ʧЧ���ų�
    for (i=expired+1; i<MAX_SCHEDULE; i++) {
      memcpy(&fmpks[fmpks_pos+i-expired-1],
             &fmpks[fmpks_pos+i],sizeof(FamilyPKSchedule));
    }
    // ��������ų����δ�ų�
    for (i=MAX_SCHEDULE-expired-1; i<MAX_SCHEDULE; i++) {
      memset(&fmpks[fmpks_pos+i], 0, sizeof(FamilyPKSchedule));
      fmpks[fmpks_pos+i].flag=-1;
// Terry add 2004/06/10 Ҫ��host_index �� guest_index �趨Ϊ -1
			fmpks[fmpks_pos+i].host_index = -1;
			fmpks[fmpks_pos+i].guest_index = -1;
// end
    }
    // ���¶���ʱ��
/*    
    h=(fmpks[fmpks_pos].dueltime%10000)/100;
    d=0;
    for (i=0; i<MAX_SCHEDULE; i++) {
      fmpks[fmpks_pos+i].dueltime=d+h*100;
      if (h==23) {
        h=0;
        d=10000;
      } else h++;
    }
*/
    if (expired==23)
      fmpks[fmpks_pos].dueltime = tm1.tm_hour*100 + ((int)(tm1.tm_min/fin)+1)*fin;

    print("scheduleman: dueltime[0] = %d\n", fmpks[fmpks_pos].dueltime);

    h=(fmpks[fmpks_pos].dueltime%10000);
    d=0;
    for (i=1; i<MAX_SCHEDULE; i++) {
      h=h+fin;
      if ( (h%100) >=60 ) h=h+100-60;
      if (h>=2400) {
        h = h-2400;
        d = 10000;
      }
      fmpks[fmpks_pos+i].dueltime=d+h;
    }
  }
}

// ���� timeout
void NPC_ProcessTimeout(int meindex)
{
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  for (i=0; i<MAX_SCHEDULE; i++) {
    if (fmpks[fmpks_pos+i].flag==FMPKS_FLAG_CHALLENGE) {
      if (fmpks[fmpks_pos+i].challenge_timeout>=0) {
        fmpks[fmpks_pos+i].challenge_timeout--;
        if (fmpks[fmpks_pos+i].challenge_timeout<0) {
	  fmpks[fmpks_pos+i].host_index = -1;
	  strcpy(fmpks[fmpks_pos+i].host_name, "-1");
	  fmpks[fmpks_pos+i].guest_index = -1;
	  strcpy(fmpks[fmpks_pos+i].guest_name, "-1");
	  fmpks[fmpks_pos+i].prepare_time = -1;
	  fmpks[fmpks_pos+i].max_player = -1;
          fmpks[fmpks_pos+i].flag=-1;
        }
      }
    }
    if (fmpks[fmpks_pos+i].flag==FMPKS_FLAG_SETTING) {
      if (fmpks[fmpks_pos+i].setting_timeout>=0) {
        fmpks[fmpks_pos+i].setting_timeout--;
        if (fmpks[fmpks_pos+i].setting_timeout<0) {
	  fmpks[fmpks_pos+i].host_index = -1;
	  strcpy(fmpks[fmpks_pos+i].host_name, "-1");
	  fmpks[fmpks_pos+i].guest_index = -1;
	  strcpy(fmpks[fmpks_pos+i].guest_name, "-1");
	  fmpks[fmpks_pos+i].prepare_time = -1;
	  fmpks[fmpks_pos+i].max_player = -1;
          fmpks[fmpks_pos+i].flag=-1;
        }
      }
    }
  }
}

// ���, һ������ֻ�ܰ���һ��ս��
BOOL NPC_AlreadyScheduled(int meindex, int talkerindex)
{
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  int tkfmindex = CHAR_getInt(talkerindex, CHAR_FMINDEX);
  int flag=1;

  for (i=0; i<MAX_SCHEDULE; i++) {
    if (fmpks[fmpks_pos+i].flag>=FMPKS_FLAG_SCHEDULED) {
      if (fmpks[fmpks_pos+i].host_index==tkfmindex) return TRUE;
    } else if (fmpks[fmpks_pos+i].flag!=FMPKS_FLAG_NONE) {
      if (flag==1) flag=0; else return TRUE;
    }
  }
  return FALSE;
}

// ��ȡ schedule ����
void NPC_LoadPKSchedule(int meindex)
{
  char filename[256],tmp[4096],token[256];
  FILE *f;
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  snprintf(filename,sizeof(filename), "%s%d_%d_%d",
           SCHEDULEFILEDIR, CHAR_getInt(meindex, CHAR_FLOOR),
           CHAR_getInt(meindex, CHAR_X),
           CHAR_getInt(meindex, CHAR_Y) );

  if( ! (f=fopen( filename, "r" )) ){	// create new schedule file
    f = fopen( filename, "w" );
    if( !f ){
      print( "ERROR:Can't create Schedule file %s!\n",filename );
      return;
    }
    for (i=0; i<MAX_SCHEDULE; i++){
      sprintf(tmp,"%d|-1|-1|-1|-1|-1|-1|-1\n",i);
      fwrite( tmp, strlen(tmp), 1, f);
    }
  }

  fseek(f, 0, SEEK_SET);	// ������ͷ
  for( i = 0; i < MAX_SCHEDULE; i++ ){
    fgets(tmp, sizeof(tmp), f);
    fmpks[fmpks_pos+i].flag=-1;

    // ʱ��
    if (getStringFromIndexWithDelim(tmp,"|",1,token,sizeof(token))) {
      fmpks[fmpks_pos+i].dueltime=atoi(token);
    } else continue;
    // ���� familyindex
    if (getStringFromIndexWithDelim(tmp,"|",2,token,sizeof(token))) {
      fmpks[fmpks_pos+i].host_index=atoi(token);
    } else continue;
    // ���� ������
    if (getStringFromIndexWithDelim(tmp,"|",3,token,sizeof(token))) {
      strcpy(fmpks[fmpks_pos+i].host_name,makeStringFromEscaped(token));
    } else continue;
    // �Ͷ� familyindex
    if (getStringFromIndexWithDelim(tmp,"|",4,token,sizeof(token))) {
      fmpks[fmpks_pos+i].guest_index=atoi(token);
    } else continue;
    // �Ͷ� ������
    if (getStringFromIndexWithDelim(tmp,"|",5,token,sizeof(token))) {
      strcpy(fmpks[fmpks_pos+i].guest_name,makeStringFromEscaped(token));
    } else continue;
    // ׼��ʱ��
    if (getStringFromIndexWithDelim(tmp,"|",6,token,sizeof(token))) {
      fmpks[fmpks_pos+i].prepare_time=atoi(token);
    } else continue;
    // �������
    if (getStringFromIndexWithDelim(tmp,"|",7,token,sizeof(token))) {
      fmpks[fmpks_pos+i].max_player=atoi(token);
    } else continue;
    // ���
    if (getStringFromIndexWithDelim(tmp,"|",8,token,sizeof(token))) {
      fmpks[fmpks_pos+i].flag=atoi(token);
    } else continue;
  }
  fclose(f);
  
  NPC_RemoveExpiredBattle(meindex);
}

void NPC_SavePKSchedule(int meindex)
{
  char filename[256],tmp[4096],n1[256],n2[256];
  FILE *f;
  int i;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;

  snprintf(filename,sizeof(filename), "%s%d_%d_%d",
           SCHEDULEFILEDIR, CHAR_getInt(meindex, CHAR_FLOOR),
           CHAR_getInt(meindex, CHAR_X),
           CHAR_getInt(meindex, CHAR_Y) );
           
  f=fopen(filename, "w");	// create new

  for( i = 0; i < MAX_SCHEDULE; i++ ){
    makeEscapeString(fmpks[fmpks_pos+i].host_name,n1,sizeof(n1));
    makeEscapeString(fmpks[fmpks_pos+i].guest_name,n2,sizeof(n2));
  
    sprintf(tmp, "%d|%d|%s|%d|%s|%d|%d|%d\n",
    	fmpks[fmpks_pos+i].dueltime,
    	fmpks[fmpks_pos+i].host_index,
    	n1,
    	fmpks[fmpks_pos+i].guest_index,
    	n2,
    	fmpks[fmpks_pos+i].prepare_time,
    	fmpks[fmpks_pos+i].max_player,
    	fmpks[fmpks_pos+i].flag);
  
    fwrite(tmp, strlen(tmp), 1, f);
  }
  fclose(f);
}

// ���� WN_PKSCHEDULELIST �� data
void NPC_LIST_gendata(int meindex, int talkerindex, int page, char *buf, int size)
{
  char tmp[4096], n1[256], n2[256];
  int i,flag;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  int tkfmindex = CHAR_getInt(talkerindex, CHAR_FMINDEX);
  int tkfmleader = CHAR_getInt(talkerindex, CHAR_FMLEADERFLAG);
  struct tm tm1;

  memset(buf, 0, size);		// clear buffer
  if ((page<0)||(page>=MAX_SCHEDULE)) return;

  // ��¼Ŀǰ��ʱ��
  memcpy( &tm1, localtime( (time_t *)&NowTime.tv_sec), sizeof( tm1));
  sprintf(buf, "%d|", tm1.tm_hour*100 + tm1.tm_min);
  
  for (i=page; i<MAX_SCHEDULE && i<page+4; i++) {
    if (i!=page) strcat(buf, "|");
    makeEscapeString(fmpks[fmpks_pos+i].host_name,n1,sizeof(n1));
    makeEscapeString(fmpks[fmpks_pos+i].guest_name,n2,sizeof(n2));
    flag=-1;
    switch (fmpks[fmpks_pos+i].flag) {
    case FMPKS_FLAG_CHALLENGE:
      if (tkfmindex==fmpks[fmpks_pos+i].host_index) {
#ifdef _FMVER21      
        if (tkfmleader==FMMEMBER_LEADER) flag=FLAG_MODIFY; else flag=FLAG_CONFIRM;
#else
        if (tkfmleader==1) flag=FLAG_MODIFY; else flag=FLAG_CONFIRM;
#endif        
      }
      if (tkfmindex==fmpks[fmpks_pos+i].guest_index) {
#ifdef _FMVER21      
        if (tkfmleader==FMMEMBER_LEADER) flag=FLAG_ACCEPT; else flag=FLAG_CONFIRM;
#else
        if (tkfmleader==1) flag=FLAG_ACCEPT; else flag=FLAG_CONFIRM;
#endif        
      }
      break;
    case FMPKS_FLAG_SETTING:
      flag=FLAG_SETTING;
      break;
    case FMPKS_FLAG_CONFIRMING:
      flag=FLAG_CONFIRM;
      break;
    case FMPKS_FLAG_SCHEDULED:
    case FMPKS_FLAG_DUEL:
    case FMPKS_FLAG_HOSTWIN:
    case FMPKS_FLAG_GUESTWIN:
      flag=FLAG_SCHEDULED;
      break;
    }

    sprintf(tmp, "%d|%s|%s|%d|%d|%d",
    	fmpks[fmpks_pos+i].dueltime,
    	n1,
    	n2,
    	fmpks[fmpks_pos+i].prepare_time,
    	fmpks[fmpks_pos+i].max_player,
    	flag);
    if (strlen(buf)+strlen(tmp)<size) {
      strcat(buf, tmp);
    } else {
      return;
    }
  }
}

// ���� WN_PKSCHEDULESELECTFAMILY �� data
void NPC_SELECT_gendata(int meindex, int page, char *buf, int size)
{
  int i,cnt=0;
  char token[4096];
  char fmindex[256];
  char fmname[256];
  
  memset(buf, 0, size);		// clear buffer
  strcpy(buf,"0");
  for (i=0; i<MAXFAMILYINONEWINDOW; i++) {
    if (getStringFromIndexWithDelim(familyListBuf,"|",page+i,token,sizeof(token))) {
      if ( (getStringFromIndexWithDelim(token," ",1,fmindex,sizeof(fmindex))) &&
           (getStringFromIndexWithDelim(token," ",2,fmname,sizeof(fmname))) ) {
        sprintf(token,"|%s|%s",fmindex,fmname);
        if (strlen(buf)+strlen(token)<size) {
          strcat(buf, token);
          cnt++;
          buf[0]=cnt+'0';
        } else return;
      }
    }
  }
}

// ���� WN_PKSCHEDULEDETAIL �� data
void NPC_DETAIL_gendata(int meindex, char *buf, int size, int dueltime)
{
  char n1[256], n2[256];
  int i=0;
  int fmpks_pos = CHAR_getWorkInt(meindex, NPC_WORK_ID)*MAX_SCHEDULE;
  
//  print("DETAIL: dueltime=%d\n",dueltime);
  
  while ((i<MAX_SCHEDULE)&&(fmpks[fmpks_pos+i].dueltime!=dueltime)) i++;
  if (fmpks[fmpks_pos+i].dueltime==dueltime) {
    makeEscapeString(fmpks[fmpks_pos+i].host_name,n1,sizeof(n1));
    makeEscapeString(fmpks[fmpks_pos+i].guest_name,n2,sizeof(n2));
    sprintf(buf, "%d|%s|%s|%d|%d",
    	fmpks[fmpks_pos+i].dueltime,
    	n1,
    	n2,
    	fmpks[fmpks_pos+i].prepare_time,
    	fmpks[fmpks_pos+i].max_player);
  } else {
    strcpy(buf, "-1|??|??|0|0");
    print("scheduleman: gen DETAIL error (%d).",dueltime);
  }
}