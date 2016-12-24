0034:npc_petfusion.h:014E:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETFUSION_H__
#define __NPC_PETFUSION_H__


void NPC_PetFusionManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_PetFusionManInit( int meindex );
void NPC_PetFusionManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetFusionManLoop( int meindex);

#endif

0032:npc_petmaker.h:0E4:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETMAKER_H__
#define __NPC_PETMAKER_H__

void NPC_PetMakerTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetMakerInit( int meindex );

#endif
 /*__NPC_PETMAKER_H__*/
0038:npc_petracemaster.h:0156:1:14=48041e4b:16=47d21313:#ifndef NPC_PetRace_Master_
#define NPC_PetRace_Master_

BOOL NPC_PetRaceMasterInit( int meindex );
void NPC_PetRaceMasterLoop( int meindex );
void NPC_PetRaceMasterTalked( int meindex , int talkerindex , char *szMes , int color );
void NPC_PetRaceMasterWindowTalked(int meindex, int talkerindex, int seqno, int select, char *data);

#endif

0034:npc_petracepet.h:0E2:1:14=48041e4b:16=47d21313:#ifndef NPC_PetRace_Pet_
#define NPC_PetRace_Pet_

BOOL NPC_PetRacePetInit( int meindex );
void NPC_PetRacePetLoop( int meindex );
void NPC_PetRacePetTalked( int meindex , int talkerindex , char *szMes , int color );

#endif

0032:npc_petshop.h:0178:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETSHOP_H__
#define __NPC_PETSHOP_H__

void NPC_PetShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetShopInit( int meindex );
void NPC_PetShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetShopLooked( int meindex , int lookedindex);


#endif 
/*__NPC_PETSHOP_H__*/
0037:npc_petskillshop.h:01E1:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETSKILLSHOP_H__
#define __NPC_PETSKILLSHOP_H__

void NPC_PetSkillShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_PetSkillShopInit( int meindex );
void NPC_PetSkillShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_PetSkillShopLooked( int meindex , int lookedindex);

void NPC_PetSkillMakeStr(int meindex,int toindex,int select);
		
#endif
 /*__NPC_WINDOWPETSKILLSHOP_H__*/
0036:npc_pettransman.h:0206:1:14=48041e4b:16=47d21313:#ifndef __NPC_PETTRANSMAN_H__
#define __NPC_PETTRANSMAN_H__

BOOL NPC_PetTransManInit(int meindex);
void NPC_PetTransMan_selectWindow(int meindex,int toindex,int num,int select);
void NPC_PetTransManWindowTalked(int meindex,int talkerindex,int seqno,int select,char *data);
void NPC_PetTransManTalked(int meindex,int talkerindex,char *szMes,int color);
int NPC_PetTransManCheck( int meindex, int toindex, int select);
BOOL NPC_PetTransManStatus( int meindex, int toindex, int petNo);
#endif
 /*__NPC_PETTRANSMAN_H__*/
0032:npc_pkpetshop.h:00:1:14=48041e4b:16=47d21313:0037:npc_poolitemshop.h:0184:1:14=48041e4b:16=47d21313:#ifndef __NPC_POOLITEMSHOP_H__
#define __NPC_POOLITEMSHOP_H__


void NPC_PoolItemShopLoop( int meindex);
BOOL NPC_PoolItemShopInit( int meindex);
void NPC_PoolItemShopTalked( int meindex , int talkerindex , 
							char *szMes ,int color );
void NPC_PoolItemShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data);


#endif
 /*__NPC_POOLITEMSHOP_H__*/
002F:npc_quiz.h:02C0:1:14=48041e4b:16=47d21313:#ifndef __NPC_QUIZ_H__
#define __NPC_QUIZ_H__

typedef struct NPC_Quiz{

	int 	no;				//  ����  į
	int 	type;			//  ����������  �ء�ة    �����̡�֭)	
	int		level;  		//  ������ì��
	int 	answertype;		//��������̫����(��  ����  ��ǩ��Ѩ����  
	int 	answerNo;		//  ��������
	char	question[512]; 	//  ��
	char	select1[128]; 	//��  ��1
	char	select2[128]; 	//��  ��2
	char	select3[128]; 	//��  ��3

}NPC_QUIZ;


void NPC_QuizTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_QuizInit( int meindex );
void NPC_QuizWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

BOOL QUIZ_initQuiz( char *filename);


#endif 

/*__NPC_QUIZ_H__*/
0033:npc_riderman.h:017F:1:14=48041e4b:16=47d21313:#ifndef __NPC_RIDERMAN_H__
#define __NPC_RIDERMAN_H__

void NPC_RidermanTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_RidermanInit( int meindex );
void NPC_RidermanLooked( int meindex, int lookedindex );
void NPC_RidermanWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif
 /*__NPC_RIDERMAN_H__*/
0037:npc_roomadminnew.h:03CA:1:14=48041e4b:16=47d21313:#ifndef __NPC_ROOMADMINNEW_H__
#define __NPC_ROOMADMINNEW_H__


void NPC_RoomAdminNewTalked( int meindex , int talkerindex , char *msg , int color );
void NPC_RoomAdminNewLoop( int meindex );
BOOL NPC_RoomAdminNewInit( int meindex );

BOOL NPC_RankingInit( int meindex );
void NPC_RankingTalked( int meindex , int talkerindex , char *msg ,int color );

BOOL NPC_PrintpassmanInit( int meindex );
void NPC_PrintpassmanTalked( int meindex , int talkerindex , char *msg ,int color );

typedef struct npc_roomadminnew_tag {
	int		expire;
	char	cdkey[CDKEYLEN];
	char	charaname[32];
	char	passwd[9];
}NPC_ROOMINFO;

typedef struct npc_roomadminnew_ranking_tag {
	int		gold;				/*   ڿ�ź� */
	int		biddate;			/*   ڿ���� */
	char	cdkey[CDKEYLEN];	/*   �������м�    ƽ�� */
	char	charaname[32];		/*   �������м�  � */
	char	owntitle[32];		/* ��į*/
	
}NPC_RANKING_INFO;

#endif /*__NPC_ROOMADMINNEW_H__*/

BOOL NPC_RoomAdminNew_ReadFile( char *roomname, NPC_ROOMINFO *data);
0030:npc_sample.h:075:1:14=48041e4b:16=47d21313:#ifndef __NPC_SAMPLE_H__
#define __NPC_SAMPLE_H__

void NPC_SampleLoop( int index );

#endif 

 /*__NPC_SAMPLE_H__*/
0034:npc_savepoint.h:014F:1:14=48041e4b:16=47d21313:#ifndef __NPC_SAVEPOINT_H__
#define __NPC_SAVEPOINT_H__


void NPC_SavePointTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_SavePointInit( int meindex );
void NPC_SavePointWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);


#endif 

/*__NPC_SAVEPOINT_H__*/
0036:npc_scheduleman.h:0A5A:1:14=48041e4b:16=47d21313:#ifndef __NPC_SCHEDULEMAN_H__
#define __NPC_SCHEDULEMAN_H__

#define CHARNAME_MAX	32

// ��� scheduleman ����
#ifdef _FAMILY_MANORNUM_CHANGE
#define MAX_SCHEDULEMAN	20
#else
#define MAX_SCHEDULEMAN	12
#endif

// ÿһ�� scheduleman �عܵ� schedule ����
#define MAX_SCHEDULE	24

// schedule �б��һҳ�����г����������
#define MAXSCHEDULEINONEWINDOW	10

// family �б��һҳ�����г����������
#define MAXFAMILYINONEWINDOW	10

// ÿһ�� scheduleman ��ʹ�õ� schedule ����λ��
// ��Ӧ�� fmpks �� ID*MAX_SCHEDULE �� (ID=0~MAX_SCHEDULEMAN-1)
// ID ��д�� data/npc ��ָ���� "id:" ��ֵ

// dueltime = (����?10000:0) + (Сʱ*100)

typedef struct tagFamilyPKSchedule {
	int	dueltime;	// ����ʱ��
	int	host_index;	// ���Ӽ���
	char host_name[CHARNAME_MAX];	// ���Ӽ�������
	int	guest_index;	// �ͶӼ���
	char guest_name[CHARNAME_MAX];	// �ͶӼ�������
	int	prepare_time;	// ׼��ʱ�� (1~40 ����)
	int	max_player;	// ���������� (1~������������)
	int	flag;		// ״̬
	int	win;		// ʤ�������趨
	int	challenge_timeout;	// ��սʱ��
	int	setting_timeout;	// �趨״̬�� timeout
	char gmsv_name[256];
} FamilyPKSchedule;

// ��¼�� FamilyPKSchedule (fmpks) �� flag ֵ
#define FMPKS_FLAG_NONE		-1	// û���κ��ų�
#define FMPKS_FLAG_CHALLENGE	0	// �ȴ��Ͷ�ͬ����
#define FMPKS_FLAG_SETTING	1	// ���������趨�ų� (ȡ��ʱ��� NONE)
#define FMPKS_FLAG_CONFIRMING	2	// �Ͷ�����ͬ����
#define FMPKS_FLAG_SCHEDULED	3	// �Ѿ��ź��ų̣���δ����
#define FMPKS_FLAG_DUEL		4	// ������
#define FMPKS_FLAG_HOSTWIN	5	// ����ʤ
#define FMPKS_FLAG_GUESTWIN	6	// �Ͷ�ʤ
#define FMPKS_FLAG_MANOR_BATTLEBEGIN	7	// ׯ԰��ս ս����
#define FMPKS_FLAG_MANOR_PREPARE	8	// ׯ԰��ս ׼����
#define FMPKS_FLAG_MANOR_PEACE	9	// ׯ԰��ս ��ս��
#define FMPKS_FLAG_MANOR_OTHERPLANET	10	// ׯ԰��ս �ڱ�������ս
#define FMPKS_FLAG_MANOR_BATTLEEND	11	// ׯ԰��ս ս������
#define FMPKS_FLAG_MANOR_PEACE_SAVE	12	// ��ׯ԰��ս�浵
#define FMPKS_FLAG_MANOR_READYTOFIGHT	13	// (GM: manorpk) ȫ��ϵ��ɿ�Լս״̬
#define FMPKS_FLAG_MANOR_CLEANFLAG	14	// (GM: manorpk) ���������״̬
#ifdef _NEW_MANOR_LAW
#define FMPKS_FLAG_WAIT		15	// ������ս��,�Ѽ�¼��������,�ȴ���ս�ų�
#endif

// ���� client �� flag
#define FLAG_NONE	-1
#define FLAG_MODIFY	0
#define FLAG_ACCEPT	1
#define FLAG_SETTING	2
#define FLAG_CONFIRM	3
#define FLAG_SCHEDULED	4

extern FamilyPKSchedule fmpks[MAX_SCHEDULE*MAX_SCHEDULEMAN];

BOOL NPC_SchedulemanInit(int meindex);
void NPC_SchedulemanTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_SchedulemanWindowTalked(int meindex, int talkerindex,
				 int seqno, int select, char *data);
void NPC_SchedulemanLoop(int meindex);

#endif 

/*__NPC_SCHEDULEMAN_H__*/
0035:npc_sellsthman.h:013A:1:14=48041e4b:16=47d21313:#ifndef _SELLSTH_MAN_H
#define _SELLSTH_MAN_H

void NPC_SellsthManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_SellsthManInit( int meindex );
void NPC_SellsthManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_SellsthManLoop( int meindex);

#endif
0034:npc_signboard.h:0124:1:14=48041e4b:16=47d21313:#ifndef __NPC_SIGNBOARD_H__
#define __NPC_SIGNBOARD_H__


BOOL NPC_SignBoardInit( int meindex );
void NPC_SignBoardWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_SignBoardLooked( int meindex , int lookedindex);


#endif
 /*__NPC_WINDOWSIGNBOAD_H__*/
0035:npc_simpleshop.h:0176:1:14=48041e4b:16=47d21313:#ifndef __NPC_SIMPLESHOP_H__
#define __NPC_SIMPLESHOP_H__

void NPC_SimpleShopTalked( int index, int talker, char *msg, int color );
void NPC_SimpleShopSpecialTalked( int index, int talker, char *msg, int color );
void NPC_LimitBuyInShopSpecialTalked( int index, int talker, char *msg, int color );
void NPC_SimpleShopInit( int meindex );

#endif 
 /*__NPC_SIMPLESHOP_H__*/
003A:npc_stoneserviceman.h:02AE:1:14=48041e4b:16=47d21313:#ifndef __NPC_STONESERVICEMAN_H__
#define __NPC_STONESERVICEMAN_H__

// ���ϴ���
typedef struct _List
{
  char Data[32];        // ����
  struct _List *Next;   // ָ����һ��List
}List,*LPList;    

BOOL NPC_StoneServiceManInit(int meindex);
void NPC_StoneServiceManLoop(int meindex);
void NPC_StoneServiceManTalked(int meindex,int talkerindex,char *msg,int color);
void NPC_StoneServiceManWindowTalked(int meindex,int talkerindex,int seqno,
                                     int select,char *data);
int SendToAP(int nAid,int nIndex,int nLen,void *Data1,void *Data2);
int RecvFromAP(int *nAid,int nIndex,int *nLen,LPList *Data);
int GetMsg(int meindex);
int ConnectToAP(void);
#endif
0035:npc_storyteller.h:0B9:1:14=48041e4b:16=47d21313:#ifndef _NPC_STORYTELLER_H_
#define _NPC_STORYTELLER_H_

BOOL NPC_StoryTellerInit( int meindex );
void NPC_StoryTellerTalked( int meindex , int talker , char *msg , int col );

#endif

0031:npc_sysinfo.h:0E4:1:14=48041e4b:16=47d21313:#ifndef __NPC_SYSINFO_H__
#define __NPC_SYSINFO_H__

BOOL NPC_SysinfoInit( int meindex );
void NPC_SysinfoLoop( int meindex );
void NPC_SysinfoTalked( int meindex, int tindex, char *msg, int color);
#endif /*__NPC_SYSINFO_H__*/
0032:npc_timeman.h:0167:1:14=48041e4b:16=47d21313:#ifndef __NPC_TIMEMAN_H__
#define __NPC_TIMEMAN_H__


void NPC_TimeManTalked( int meindex , int talkerindex , char *msg ,
                     int color );

BOOL NPC_TimeManInit( int meindex );

void NPC_TimeManWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );

#endif /*__NPC_TIMEMAN_H__*/
0034:npc_townpeople.h:0D1:1:14=48041e4b:16=47d21313:#ifndef __NPC_TOWNPEOPLE_H__
#define __NPC_TOWNPEOPLE_H__

void NPC_TownPeopleTalked( int index, int talker, char *msg, int color );
void NPC_TownPeopleInit( int meindex );

#endif  /* __NPC_TOWNPEOPLE_H__ */
0035:npc_transerman.h:013A:1:14=48041e4b:16=47d21313:#ifndef _TRANSER_MAN_H
#define _TRANSER_MAN_H

void NPC_TranserManTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_TranserManInit( int meindex );
void NPC_TranserManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_TranserManLoop( int meindex);

#endif
0039:npc_transmigration.h:016A:1:14=48041e4b:16=47d21313:#ifndef __NPC_TRANSMIGRATION_H__
#define __NPC_TRANSMIGRATION_H__

void NPC_TransmigrationTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_TransmigrationInit( int meindex );
void NPC_TransmigrationWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#endif 

/*__NPC_TRANSMIGRATION_H__*/
