#include <string.h>
#include "char.h"
#include "battle.h"
#include "object.h"
#include "char_base.h"
#include "enemy.h"
#include "mylua/base.h"
#include "npcutil.h"
#include "readmap.h"
#include "log.h"
#include "pet.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "version.h"
#include "handletime.h"
#include "item_event.h"
#include "lssproto_serv.h"
#ifdef _ALLBLUES_LUA   
#include "mylua/mylua.h"
extern MY_Lua MYLua;
static CharBase CharBaseWorkInt[] = {
	{{"����"}, 						CHAR_WORKOBJINDEX}
#ifdef _FONT_SIZE
	,{{"�ֺ�"}, 					CHAR_WORKFONTSIZE}
#endif
	,{{"ս��"}, 					CHAR_WORKBATTLEMODE}
	,{{"���"}, 					CHAR_WORKPARTYMODE}
	,{{"ս������"}, 			CHAR_WORKBATTLEINDEX}
	,{{"NPC��ʱ1"}, 			CHAR_NPCWORKINT1}
  ,{{"NPC��ʱ2"}, 			CHAR_NPCWORKINT2}
  ,{{"NPC��ʱ3"}, 			CHAR_NPCWORKINT3}
  ,{{"NPC��ʱ4"}, 			CHAR_NPCWORKINT4}
  ,{{"NPC��ʱ5"}, 			CHAR_NPCWORKINT5}
  ,{{"NPC��ʱ6"},				CHAR_NPCWORKINT6}
  ,{{"NPC��ʱ7"},				CHAR_NPCWORKINT7}
  ,{{"NPC��ʱ8"},				CHAR_NPCWORKINT8}
  ,{{"NPC��ʱ9"},				CHAR_NPCWORKINT9}
  ,{{"NPC��ʱ10"},			CHAR_NPCWORKINT10}
  ,{{"NPC��ʱ11"},			CHAR_NPCWORKINT11}
	,{{"NPC��ʱ12"},			CHAR_NPCWORKINT12}
	,{{"NPC��ʱ13"},			CHAR_NPCWORKINT13}
	,{{"����ģʽ"},				CHAR_WORKPETFOLLOWMODE}
  ,{{"��Ա1"},					CHAR_WORKPARTYINDEX1}
  ,{{"��Ա2"},					CHAR_WORKPARTYINDEX2}
  ,{{"��Ա3"},					CHAR_WORKPARTYINDEX3}
	,{{"��Ա4"},					CHAR_WORKPARTYINDEX4}
	,{{"��Ա5"},					CHAR_WORKPARTYINDEX5}
	,{{"���HP"},					CHAR_WORKMAXHP}
	,{{"���MP"},					CHAR_WORKMAXMP}
	,{{"������"},					CHAR_WORKATTACKPOWER}
	,{{"������"},					CHAR_WORKDEFENCEPOWER}
	,{{"������"},					CHAR_WORKQUICK}
	,{{"��������"},				CHAR_WORKFIXVITAL}
	,{{"��������"},				CHAR_WORKFIXSTR}
	,{{"��������"},				CHAR_WORKFIXTOUGH}
	,{{"�����ٶ�"},				CHAR_WORKFIXDEX}
	,{{"�����ͼ"},				CHAR_WORKFMFLOOR}
	,{{"����"},						CHAR_WORKATTACKPOWER}
	,{{"����"},						CHAR_WORKDEFENCEPOWER}
	,{{"����"},						CHAR_WORKQUICK}
	,{{"����"},						CHAR_WORKDBATTLEESCAPE}
#ifdef _OFFLINE_SYSTEM
	,{{"����"}, 					CHAR_WORK_OFFLINE}
#endif
	,{{"��þ���"},				CHAR_WORKGETEXP}
#ifdef _AUTO_PK
	,{{"�Զ�PK��"},				CHAR_WORK_AUTOPK}
	,{{"�Զ�PK����"},			CHAR_WORK_AUTOPK_DEAD}
#endif
#ifdef _SPECIAL_COUNTER
	,{{"������"}, 				CHAR_WORK_COUNTER}
#endif
#ifdef _SPECIAL_TIMER
	,{{"��ʱ��"}, 				CHAR_WORK_TIMER}
#endif
#ifdef _STREET_VENDOR	
	,{{"��̯"}, 					CHAR_WORKSTREETVENDOR}
#endif
#ifdef _RECORD_IP	
	,{{"IP"}, 						CHAR_WORK_RECORD_IP}
#endif
#ifdef _ONLINE_TIME
	,{{"��½ʱ��"}, 			CHAR_WORK_LOGIN_TIME}
#endif
 // ,{{"����ӳ�"},        CHAR_WORKITEM_ADDEXP}
 // ,{{"����ʱЧ"},        CHAR_WORKITEM_ADDEXPTIME}
};

static CharBase CharBaseWorkChar[] = {
	{{"NPC��ʱ1"}, 				CHAR_NPCWORKCHAR1}
  ,{{"NPC��ʱ2"}, 			CHAR_NPCWORKCHAR2}
  ,{{"NPC��ʱ3"}, 			CHAR_NPCWORKCHAR3}
  ,{{"NPC��ʱ4"}, 			CHAR_NPCWORKCHAR4}
  ,{{"NPC��ʱ5"}, 			CHAR_NPCWORKCHAR5}
  ,{{"NPC��ʱ6"},				CHAR_NPCWORKCHAR6}
#ifdef _STREET_VENDOR	
	,{{"��̯"}, 						CHAR_STREETVENDOR_NAME}
#endif
};

static CharBase CharBaseValue[] = {
	{{"��"}, 					0}
	,{{"����"}, 			1}
	,{{"��"}, 				2}
	,{{"����"}, 			3}
	,{{"��"}, 				4}
	,{{"����"}, 			5}
	,{{"��"}, 				6}
	,{{"����"}, 			7}
	,{{"������"}, 		CHAR_TYPENONE}
	,{{"���"}, 			CHAR_TYPEPLAYER}
	,{{"����"}, 			CHAR_TYPEENEMY}
	,{{"����"}, 			CHAR_TYPEPET}
#ifdef _ALLBLUES_LUA 
	,{{"LUA"}, 			CHAR_TYPELUANPC}
#endif
#ifdef _PLAYER_NPC 
	,{{"����"}, 			CHAR_TYPEPLAYERNPC}
	,{{"���"}, 			CHAR_TYPEPLAYERPETNPC}
#endif
};

static CharBase CharBaseWorkValue[] = {
	{{"��"}, 									CHAR_PARTY_NONE}
	,{{"�ӳ�"}, 							CHAR_PARTY_LEADER}
	,{{"��Ա"}, 							CHAR_PARTY_CLIENT}
	,{{"��ս��"}, 						BATTLE_CHARMODE_NONE}
	,{{"ս����ʼ��"}, 				BATTLE_CHARMODE_INIT}
	,{{"�ȴ�ս��"}, 					BATTLE_CHARMODE_C_WAIT}
	,{{"ȷ��ս��"}, 					BATTLE_CHARMODE_C_OK}
	,{{"����ս��"}, 					BATTLE_CHARMODE_RESCUE}
	,{{"����ս��"}, 					BATTLE_CHARMODE_FINAL}
	,{{"��ս��ʼ��"}, 				BATTLE_CHARMODE_WATCHINIT}
	,{{"û�м���"}, 					FMMEMBER_NONE}
  ,{{"��������"}, 					FMMEMBER_APPLY}
  ,{{"�峤"}, 							FMMEMBER_LEADER}
  ,{{"��Ա"}, 							FMMEMBER_MEMBER}
  ,{{"����"}, 							FMMEMBER_ELDER}
};

static CharBase CharBaseInt[] = {
	{{"ͼ���"}, 			CHAR_BASEIMAGENUMBER}
	,{{"ԭͼ���"}, 	CHAR_BASEBASEIMAGENUMBER}
	,{{"��ͼ��"},			CHAR_FLOOR}
	,{{"����X"}, 			CHAR_X}
	,{{"����Y"}, 			CHAR_Y}
	,{{"����"}, 			CHAR_DIR}
	,{{"�ȼ�"}, 			CHAR_LV}
	,{{"ʯ��"}, 			CHAR_GOLD}
	,{{"HP"}, 				CHAR_HP}
	,{{"MP"}, 				CHAR_MP}
	,{{"���MP"}, 		CHAR_MAXMP}
	,{{"����"}, 			CHAR_VITAL}
	,{{"����"}, 			CHAR_STR}
	,{{"����"},				CHAR_TOUGH}
	,{{"�ٶ�"},				CHAR_DEX}
	,{{"��"},					CHAR_EARTHAT}
	,{{"ˮ"},					CHAR_WATERAT}
	,{{"��"}, 				CHAR_FIREAT}
	,{{"��"}, 				CHAR_WINDAT}
	,{{"ս��"}, 			CHAR_DEFAULTPET}
	,{{"����"}, 			CHAR_CHARM}
	,{{"����"}, 			CHAR_LUCK}
	,{{"��������"}, 	CHAR_DEADCOUNT}
	,{{"��·����"}, 	CHAR_WALKCOUNT}
	,{{"˵������"}, 	CHAR_TALKCOUNT}
	,{{"�𻵴���"}, 	CHAR_DAMAGECOUNT}
	,{{"�������"}, 	CHAR_GETPETCOUNT}
	,{{"ɱ�����"}, 	CHAR_KILLPETCOUNT}
	,{{"�������"}, 	CHAR_DEADPETCOUNT}
	,{{"���ʼ���"}, 	CHAR_SENDMAILCOUNT}
	,{{"�ϳɴ���"}, 	CHAR_MERGEITEMCOUNT}
	,{{"PK����"}, 		CHAR_DUELBATTLECOUNT}
	,{{"PKӮ��"},			CHAR_DUELWINCOUNT}
	,{{"PK����"}, 		CHAR_DUELLOSECOUNT}
	,{{"PK��ʤ"}, 		CHAR_DUELSTWINCOUNT}
	,{{"PK�����ʤ"},	CHAR_DUELMAXSTWINCOUNT}
	,{{"����"}, 			CHAR_WHICHTYPE}
	,{{"ѭ���¼�ʱ��"},   CHAR_LOOPINTERVAL}
#ifdef _NEWOPEN_MAXEXP
	,{{"�ܾ���"}, 		CHAR_OLDEXP}
#endif
	,{{"��ǰ����"}, 	CHAR_EXP}
	,{{"���ܵ�"}, 		CHAR_SKILLUPPOINT}
	,{{"������"}, 		CHAR_LEVELUPPOINT}
	,{{"DP"}, 				CHAR_DUELPOINT}
	,{{"����"}, 			CHAR_EXP}
	,{{"������"}, 		CHAR_LASTTALKELDER}
	,{{"ת��"}, 			CHAR_TRANSMIGRATION}
	,{{"����"}, 			CHAR_SILENT}
	,{{"��������"}, 	CHAR_FMINDEX}
	,{{"�����λ"}, 	CHAR_FMLEADERFLAG}
	,{{"��������"}, 	CHAR_BANKGOLD}
	,{{"���"}, 			CHAR_RIDEPET}
	,{{"����"}, 			CHAR_CRITIAL}
	,{{"����"}, 			CHAR_COUNTER}
#ifdef _GAMBLE_BANK
//	,{{"�ĳ�����"}, 	CHAR_RIDEPET}
#endif
#ifdef _DROPSTAKENEW
	,{{"�ĳ�����"}, 	CHAR_GAMBLENUM}
#endif
#ifdef _PERSONAL_FAME	// Arminius 8.30: �����������
	,{{"����"}, 			CHAR_FAME}
#endif
#ifdef _ATTACK_MAGIC
	,{{"��ħ��������"}, 			CHAR_EARTH_EXP}
	,{{"ˮħ��������"}, 			CHAR_WATER_EXP}
	,{{"��ħ��������"}, 			CHAR_FIRE_EXP}
	,{{"��ħ��������"}, 			CHAR_WIND_EXP}
	,{{"��ħ������"}, 				CHAR_EARTH_RESIST}
	,{{"ˮħ������"}, 				CHAR_WATER_RESIST}
	,{{"��ħ������"}, 				CHAR_FIRE_RESIST}
	,{{"��ħ������"}, 				CHAR_WIND_RESIST}
	,{{"��ħ�������Ⱦ���"}, 	CHAR_EARTH_ATTMAGIC_EXP}
	,{{"ˮħ�������Ⱦ���"}, 	CHAR_WATER_ATTMAGIC_EXP}
	,{{"��ħ�������Ⱦ���"}, 	CHAR_FIRE_ATTMAGIC_EXP}
	,{{"��ħ�������Ⱦ���"}, 	CHAR_WIND_ATTMAGIC_EXP}
	,{{"��ħ�����Ծ���ֵ"}, 	CHAR_EARTH_DEFMAGIC_EXP}
	,{{"ˮħ�����Ծ���ֵ"}, 	CHAR_WATER_DEFMAGIC_EXP}
	,{{"��ħ�����Ծ���ֵ"}, 	CHAR_FIRE_DEFMAGIC_EXP}
	,{{"��ħ�����Ծ���ֵ"}, 	CHAR_WIND_DEFMAGIC_EXP}
#endif
#ifdef _CHAR_PROFESSION			// WON ADD ����ְҵ
	,{{"ְҵ��"}, 	PROFESSION_CLASS}
	,{{"ְҵ�ȼ�"}, 	PROFESSION_LEVEL}
	,{{"���ܵ���"}, 	PROFESSION_SKILL_POINT}
	,{{"���Ӷѵ�"}, 	ATTACHPILE}
	,{{"��������"}, 	PROFESSION_FIRE_P}
	,{{"��������"}, 	PROFESSION_ICE_P}
	,{{"��������"}, 	PROFESSION_THUNDER_P}
	,{{"����"}, 	PROFESSION_FIRE_R}
	,{{"������"}, 	PROFESSION_ICE_R}
	,{{"�׿���"}, 	PROFESSION_THUNDER_R}
#endif
#ifdef _NEW_MANOR_LAW
	,{{"����"}, 		CHAR_MOMENTUM}
#endif
//#ifdef _VIP_SERVER
//	,{{"����"}, 		CHAR_AMPOINT}
//#endif
#ifdef _VIP_RIDE
	,{{"��Ա"}, 			CHAR_VIPRIDE}
	,{{"��Ա��Ч��"}, CHAR_VIPTIME}
#endif
#ifdef _ITEM_PET_LOCKED
	,{{"��ȫ��"}, 		CHAR_LOCKED}
#endif
#ifdef _ONLINE_SHOP
	,{{"����"}, 			CHAR_BJ}
#endif
#ifdef _CAX_ESC_REPORT
  ,{{"ǩ��"}, 			CHAR_REPORT}
#endif
	,{{"ģʽAI"}, 	CHAR_MODAI}
	,{{"�ɱ�AI"}, 	CHAR_VARIABLEAI}
	,{{"�輼λ"}, 	CHAR_SLOT}
	,{{"��ID"}, 		CHAR_PETID}
	,{{"��ID"}, 		CHAR_PETENEMYID}
	,{{"�ػ���"}, 	CHAR_PETFAMILY}
	,{{"����ֵ"}, 	CHAR_ALLOCPOINT}
	,{{"�ɳ�����"}, CHAR_PETRANK}
	,{{"���Ƶȼ�"}, CHAR_LIMITLEVEL}
	
/*char.setInt( petindex, "�ںϱ��", PetCode)
	char.setInt( petindex, "ת��", 2)
	char.setInt( petindex, "�ں�����", 3)
	char.setInt( petindex, "�ںϱ���", -1)
	char.setInt( petindex, "��װ��", -1)
	char.setInt( petindex, "ιʳ����", 30 )
	char.setInt( petindex, "�ںϳ�", 1)
	char.setInt( petindex, "ιʳʱ��", other.time())
	char.setInt( petindex, "�ں�����", 0)
	char.setInt( petindex, "�ں�����", 0)
	char.setInt( petindex, "�ں�����", 0)
	char.setInt( petindex, "�ں��ٶ�", 0)*/
//	,{{"�ں�����"}, CHAR_FUSIONCODE}//CHAR_FUSIONBEIT
	,{{"�ںϱ��"}, CHAR_FUSIONINDEX}//�ںϱ��--
	,{{"�ںϱ���"}, CHAR_FUSIONCODE}//-
	,{{"ιʳ����"}, CHAR_FUSIONRAISE}	//-
	,{{"�ںϳ�"},   CHAR_FUSIONBEIT}//	-
	,{{"ιʳʱ��"}, CHAR_FUSIONTIMELIMIT}//
  ,{{"�ں�����"}, CHAR_EVOLUTIONBASEVTL}//
	,{{"�ں�����"}, CHAR_EVOLUTIONBASESTR}//
	,{{"�ں�����"}, CHAR_EVOLUTIONBASETGH}//
	,{{"�ں��ٶ�"}, CHAR_EVOLUTIONBASEDEX}//
	

#ifdef _PET_BEATITUDE
	,{{"����ֵ"}, 	CHAR_BEATITUDE}
#endif
#ifdef _CAMEO_MONEY
	,{{"����"}, 		CHAR_CAMEO}
#endif
#ifdef _MISSION_TRAIN
	,{{"������"}, 	CHAR_MISSIONTRAIN_NUM}
	,{{"������ʱ��"}, 	CHAR_MISSIONTRAIN_TIME}
#endif
#ifdef _PET_EVOLVE
	,{{"����"}, 		CHAR_EVOLVE}
#endif
#ifdef _ONLINE_TIME
	,{{"����ʱ��"}, 		CHAR_ONLINE_TIME}
#endif
  ,{{"����ʱ��"}, 		CHAR_OFFTIME}
#ifdef _FLOOR_DIFI_FIELD
	,{{"¥��ʱ��"}, 		CHAR_FLOORDIFI_TIME}
#endif
//#ifdef _ACTIVE_GAME
//	,{{"����"}, 		CHAR_ACTIVE}
//#endif
  ,{{"����ӳ�"},        CHAR_ADDEXPPOWER}    //����ӳ�
	,{{"����ʱЧ"},        CHAR_ADDEXPTIME}     //����ʱЧ  /60����
  ,{{"����"}, 		      CHAR_VIGOR}
	,{{"����"}, 		      CHAR_EVNUM}
  ,{{"��������1"},        CHAR_LUASAVE_WORKINT_1}  //lua�洢 �����������
  ,{{"��������2"},        CHAR_LUASAVE_WORKINT_2}  //lua�洢 �����������
  ,{{"��������3"},        CHAR_LUASAVE_WORKINT_3}  //lua�洢 �����������
  ,{{"�����洢1"},        CHAR_LUASAVE_INT_1}     //�����洢���������浵��1-3
  ,{{"�����洢2"},        CHAR_LUASAVE_INT_2}     //�����洢���������浵��1-3
  ,{{"�����洢3"},        CHAR_LUASAVE_INT_3}     //�����洢���������浵��1-3
#ifdef _CAX_Mercenary
  ,{{"Ӷ������"},        CHAR_MERCENARY}     //Ӷ������
#endif
#ifdef _SUPER
	,{{"��Ʒ"}, 		CHAR_SUPER}
#endif
#ifdef _ALLDOMAN
	,{{"Ӣ�۳ƺ�"}, 		CHAR_HEROFLOOR}
#endif

#ifdef _PLAYER_TITLE
	,{{"�ƺŵȼ�"}, 		CHAR_TITLE_LV}
	,{{"�ƺ�ʱ��"}, 		CHAR_TITLE_TIME}
#endif
#ifdef _MISSION_TIME
	,{{"�����ʱ"}, 		CHAR_MISSION_TIME}
#endif
#ifdef _REGISTRATION_TIME
	,{{"ǩ��ʱ��"}, 		CHAR_REGISTRATION_TIME}
	,{{"ǩ������"}, 		CHAR_REGISTRATION_COUNT}
	,{{"ǩ������ʱ��"}, CHAR_REGISTRATION_ONLINE_TIME}
#endif
	,{{"֤�����"}, 	CHAR_LOWRIDEPETS}
	,{{"֤�����1"}, 	CHAR_LOWRIDEPETS1}
	,{{"֤�����2"}, 	CHAR_HIGHRIDEPETS2}
};

static CharBase CharBaseChar[] = {
	{{"����"}, 				CHAR_NAME}
	,{{"�ǳ�"}, 			CHAR_OWNTITLE}
	,{{"����"},				CHAR_USERPETNAME}
	,{{"�˺�"}, 			CHAR_CDKEY}
	,{{"����"}, 			CHAR_FMNAME}
#ifdef _TEACHER_SYSTEM
	,{{"��ʦ�ʺ�"}, 	CHAR_TEACHER_ID}
	,{{"��ʦ����"}, 	CHAR_TEACHER_NAME}
#endif
#ifdef _ITEM_SETLOVER
	,{{"�����˺�"}, 	CHAR_LOVERID}
	,{{"��������"},		CHAR_LOVERNAME}
#endif
#ifdef _NEW_NAME
	,{{"�ƺ�"},				CHAR_NEWNAME}
#endif
#ifdef _LOTTERY_SYSTEM
	,{{"��Ʊ"},				CHAR_LOTTERY_VALUE}
#endif
	,{{"�����˺�"},		CHAR_OWNERCDKEY}
	,{{"��������"},		CHAR_OWNERCHARANAME}
};
		

static CharBase CharBaseEvent[] = {
	{{"��ʼ���¼�"}, 		CHAR_INITFUNC}
	//,{{"�����¼�"}, 		CHAR_WALKPREFUNC}
	//,{{"���ߺ��¼�"},		CHAR_WALKPOSTFUNC}
	//,{{"δ֪�¼�"}, 		CHAR_PREOVERFUNC}
	//,{{"δ֪�¼�"}, 		CHAR_POSTOVERFUNC}
	//,{{"�ۿ��¼�"}, 		CHAR_WATCHFUNC}
	,{{"ѭ���¼�"}, 		CHAR_LOOPFUNC}
	//,{{"�����¼�"}, 		CHAR_DYINGFUNC}
	,{{"�Ի��¼�"},			CHAR_TALKEDFUNC}
	//,{{"�����¼�"},			CHAR_PREATTACKEDFUNC}
	//,{{"�������¼�"}, 	CHAR_POSTATTACKEDFUNC}
	//,{{"�뿪�¼�"}, 		CHAR_OFFFUNC}
	//,{{"���¼�"}, 			CHAR_LOOKEDFUNC}
	//,{{"�����¼�"},			CHAR_ITEMPUTFUNC}
	//,{{"����Ի��¼�"}, CHAR_SPECIALTALKEDFUNC}
	,{{"�����¼�"}, 		CHAR_WINDOWTALKEDFUNC}
#ifdef _USER_CHARLOOPS
	//,{{"ѭ���¼�1"}, 		CHAR_LOOPFUNCTEMP1}
	//,{{"ѭ���¼�2"}, 		CHAR_LOOPFUNCTEMP2}
	//,{{"δ֪�¼�"}, 		CHAR_BATTLEPROPERTY}
#endif
	,{{"�ص��¼�"}, 		CHAR_OVERLAPEDFUNC}
	,{{"ս���¼�"}, 		CHAR_BATTLEOVERDFUNC}
#ifdef _ALLBLUES_LUA_1_4
	,{{"�ǳ��¼�"}, 		CHAR_LOGINOUTFUNC}
#endif
#ifdef _ALLBLUES_LUA_1_9
	,{{"ս�������¼�"}, CHAR_BATTLESETFUNC}
#endif
}; 
	
static CharBase CharBaseColor[] = {
	{{"��ɫ"}, 			CHAR_COLORWHITE}
	,{{"��ɫ"}, 		CHAR_COLORCYAN}
	,{{"��ɫ"},			CHAR_COLORPURPLE}
	,{{"��ɫ"}, 		CHAR_COLORBLUE}
	,{{"��ɫ"}, 		CHAR_COLORYELLOW}
	,{{"��ɫ"}, 		CHAR_COLORGREEN}
	,{{"��ɫ"}, 		CHAR_COLORRED}
	,{{"�Ұ�ɫ"}, 	CHAR_COLORGRAY}
	,{{"����ɫ"}, 	CHAR_COLORBLUE2}
	,{{"����ɫ"}, 	CHAR_COLORGREEN2}
}; 

static CharBase CharBaseUpdata[] = {
	{{"HP"},						CHAR_P_STRING_HP}
	,{{"MAXHP"},				CHAR_P_STRING_MAXHP}
	,{{"MP"},						CHAR_P_STRING_MP}
	,{{"MAXMP"},				CHAR_P_STRING_MAXMP}
	,{{"����"},					CHAR_P_STRING_VITAL}
	,{{"����"},					CHAR_P_STRING_STR}
	,{{"����"},					CHAR_P_STRING_TOUGH}
	,{{"�ٶ�"},					CHAR_P_STRING_DEX}
	,{{"����"},					CHAR_P_STRING_EXP}
	,{{"��һ������"},		CHAR_P_STRING_NEXTEXP}
	,{{"�ȼ�"},					CHAR_P_STRING_LV}
	,{{"������"},				CHAR_P_STRING_ATK}
	,{{"������"},				CHAR_P_STRING_DEF}
	,{{"������"},				CHAR_P_STRING_QUICK}
	,{{"����"},					CHAR_P_STRING_CHARM}
	,{{"����"},					CHAR_P_STRING_LUCK}
	,{{"��"},						CHAR_P_STRING_EARTH}
	,{{"ˮ"},						CHAR_P_STRING_WATER}
	,{{"��"},						CHAR_P_STRING_FIRE}
	,{{"��"},						CHAR_P_STRING_WIND}
	,{{"ʯ��"},					CHAR_P_STRING_GOLD}
	,{{"����"},					CHAR_P_STRING_TITLE}
	,{{"DP"},						CHAR_P_STRING_DUELPOINT}
	,{{"ת��"},					CHAR_P_STRING_TRANSMIGRATION}
	,{{"����"},					CHAR_P_STRING_NAME}
	,{{"�ǳ�"},					CHAR_P_STRING_OWNTITLE}
	,{{"���"},					CHAR_P_STRING_RIDEPET}
	,{{"ѧ��"},					CHAR_P_STRING_LEARNRIDE}
	,{{"ͼ��"},					CHAR_P_STRING_BASEBASEIMAGENUMBER}
	,{{"������"},				CHAR_P_STRING_SKYWALKER}
	,{{"����ģʽ"},			CHAR_P_STRING_DEBUGMODE}
}; 

static CharBase CharBaseFlg[] = {
	{{"�ɼ�"}, 						CHAR_ISVISIBLE}
	,{{"͸��"},       		CHAR_ISTRANSPARENT}
  ,{{"����"},       		CHAR_ISFLYING}
  ,{{"����"},       		CHAR_ISDIE}
  ,{{"���"},       		CHAR_ISPARTY}
  ,{{"����"},       		CHAR_ISDUEL}
  ,{{"��Ƭ"},       		CHAR_ISTRADECARD}
  ,{{"����"},       		CHAR_ISTRADE}
#ifdef _CHANNEL_MODIFY
  ,{{"����Ƶ��"},       CHAR_ISTELL}
  ,{{"����Ƶ��"},       CHAR_ISFM}
  ,{{"ְҵƵ��"},       CHAR_ISOCC}
  ,{{"�Ի�����"},       CHAR_ISSAVE}
  ,{{"������"},       	CHAR_ISCHAT}
#ifdef _CHATROOMPROTOCOL
#ifdef _THE_WORLD_SEND
  ,{{"����Ƶ��"},       CHAR_ISWORLD}
#endif
#ifdef _ONLINE_TALK_IP
  ,{{"�ٱ�Ƶ��"},       CHAR_ISTALKIP}
#endif
#endif
#endif
};

static CharBase CharBaseAction[] = {
	{{"վ��"},					CHAR_ACTSTAND}
	,{{"�߶�"},					CHAR_ACTWALK}
	,{{"����"},					CHAR_ACTATTACK}
	,{{"Ͷ��"},					CHAR_ACTTHROW}
	,{{"����"},					CHAR_ACTDAMAGE}
	,{{"����"},					CHAR_ACTDEAD}
	,{{"ħ��"},					CHAR_ACTMAGIC}
	,{{"����"},					CHAR_ACTITEM}
	,{{"Ч��"},					CHAR_ACTEFFECT}
	,{{"��"},						CHAR_ACTDOWN}
	,{{"��"},						CHAR_ACTSIT}
	,{{"����"},					CHAR_ACTHAND}
	,{{"����"},					CHAR_ACTPLEASURE}
	,{{"��ŭ"},					CHAR_ACTANGRY}
	,{{"����"},					CHAR_ACTSAD}
	,{{"����"},					CHAR_ACTGUARD}
	,{{"��·"},					CHAR_ACTACTIONWALK}
	,{{"��ͷ"},					CHAR_ACTNOD}
	,{{"վ��"},					CHAR_ACTACTIONSTAND}
	,{{"ս��"},					CHAR_ACTBATTLE}
	,{{"����"},					CHAR_ACTLEADER}
	,{{"��ս"},					CHAR_ACTBATTLEWATCH}
	,{{"δ֪"},					CHAR_ACTPOPUPNAME}
	,{{"��ת"},					CHAR_ACTTURN}
	,{{"����"},					CHAR_ACTWARP}
	,{{"����"},					CHAR_ACTTRADE}
#ifdef _ANGEL_SUMMON
	,{{"��ʹ"},					CHAR_ACTANGEL}
#endif

#ifdef _MIND_ICON
	,{{"��˼"},					CHAR_MIND}
#endif
#ifdef _STREET_VENDOR
	,{{"��̯̲"},			CHAR_STREETVENDOR_OPEN}
	,{{"�ر�̯̲"},			CHAR_STREETVENDOR_CLOSE}
#endif
};


static int getCharNum (lua_State *L) 
{
	lua_pushinteger(L, CHAR_getCharNum());

  return 1;
}

static int getPlayerMaxNum (lua_State *L) 
{
	lua_pushinteger(L, CHAR_getPlayerMaxNum());

  return 1;
}

static int getPetMaxNum (lua_State *L) 
{
	lua_pushinteger(L, CHAR_getPetMaxNum());

  return 1;
}

static int getOthersMaxNum (lua_State *L) 
{
	lua_pushinteger(L, CHAR_getOthersMaxNum());

  return 1;
}


static int check (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
	lua_pushinteger(L, CHAR_CHECKINDEX(index));

  return 1;
}

static int setFlg (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseFlg, arraysizeof(CharBaseFlg));
  const int data = luaL_checkint(L, 3);
  if(CHAR_setFlg(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getFlg (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseFlg, arraysizeof(CharBaseFlg));
  
	lua_pushinteger(L, CHAR_getFlg(index, element));
  return 1;
}

static int setWorkChar (lua_State *L) 
{
	size_t l;
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseWorkChar, arraysizeof(CharBaseWorkChar));
  char *data = luaL_checklstring(L, 3, &l);
  if(CHAR_setWorkChar(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getWorkChar (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseWorkChar, arraysizeof(CharBaseWorkChar));
  lua_pushstring(L, CHAR_getWorkChar(index, element));
  return 1;
}


static int setWorkInt (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseWorkInt, arraysizeof(CharBaseWorkInt));
  const int data = getCharBaseValue(L, 3, CharBaseWorkValue, arraysizeof(CharBaseWorkValue));
   //print("index = %d  element =%d data =%d \n ",index,element,data);
  if(CHAR_setWorkInt(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getWorkInt (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseWorkInt, arraysizeof(CharBaseWorkInt));
  lua_pushinteger(L, CHAR_getWorkInt(index, element));
  return 1;
}

static int setChar (lua_State *L) 
{
  size_t l;
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseChar, arraysizeof(CharBaseChar));
  char *data = luaL_checklstring(L, 3, &l);
  if(CHAR_setChar(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getChar (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseChar, arraysizeof(CharBaseChar));
  lua_pushstring(L, CHAR_getChar(index, element));
  return 1;
}

static int setInt (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseInt, arraysizeof(CharBaseInt));
  const int data = getCharBaseValue(L, 3, CharBaseValue, arraysizeof(CharBaseValue));
  if(CHAR_setInt(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getInt (lua_State *L) 
{
  const int index = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, CharBaseInt, arraysizeof(CharBaseInt));
  lua_pushinteger(L, CHAR_getInt(index, element));
  return 1;
}

static int setFunctionPointer(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
  const int functype = getCharBaseValue(L, 2, CharBaseEvent, arraysizeof(CharBaseEvent));
  char *luafunctable=luaL_checklstring(L, 3, &l);
	char *luafunctablepath=luaL_checklstring(L, 4, &l);
	if(strlen(luafunctablepath) > 0){
		MY_Lua *mylua = &MYLua;
	  while(mylua->next != NULL){
	  	if(strcmp(mylua->luapath, luafunctablepath) == 0){
				return CHAR_setLUAFunction( index, functype, mylua->lua, luafunctable );
	  	}
	  	mylua = mylua->next;
	  }
	}else{
		return CHAR_setLUAFunction( index, functype, L, luafunctable );
	}
	return 1;
}

static int delFunctionPointer(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
  const int functype = getCharBaseValue(L, 2, CharBaseEvent, arraysizeof(CharBaseEvent));

	return CHAR_delLUAFunction( index, functype );
}

static int TalkToCli(lua_State *L) 
{
	size_t l;
	const int talkedcharaindex = luaL_checkint(L, 1);
	const int talkcharaindex = luaL_checkint(L, 2);
	char *message=luaL_checklstring(L, 3, &l);
	const int color = getCharBaseValue(L, 4, CharBaseColor, arraysizeof(CharBaseColor));
	CHAR_talkToCli(talkedcharaindex, talkcharaindex, message, color);
	return 1;
}

static int TalkToRound(lua_State *L) 
{
	size_t l;
	const int talkedcharaindex = luaL_checkint(L, 1);
	char *message=luaL_checklstring(L, 2, &l);
	const int color = getCharBaseValue(L, 3, CharBaseColor, arraysizeof(CharBaseColor));

	int fl = CHAR_getInt( talkedcharaindex, CHAR_FLOOR) ;
	int x  = CHAR_getInt( talkedcharaindex, CHAR_X) ;
	int y  = CHAR_getInt( talkedcharaindex, CHAR_Y) ;
	
	int i,j;

	int range = 8;
	
	for( i=x-range ; i<=x+range ; i++ ){
		for( j=y-range ; j<=y+range ; j++ ){
			OBJECT  object;
			for( object = MAP_getTopObj( fl,i,j ) ; object ; object = NEXT_OBJECT(object) ){
				int objindex = GET_OBJINDEX(object);
				int	toindex = OBJECT_getIndex( objindex);
				if( OBJECT_getType(objindex) == OBJTYPE_CHARA && toindex != talkedcharaindex ){
					if( CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
						typedef void (*TALKF)(int,int,char*,int,int);
						TALKF   talkedfunc=NULL;
						talkedfunc = (TALKF)CHAR_getFunctionPointer( toindex, CHAR_TALKEDFUNC);
						if( talkedfunc ) {
							talkedfunc(toindex, talkedcharaindex, message, color, -1);
						}
					}
				}
			}
		}
	}

	return 1;
}

static int WalkPoint(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ) return 0;
	
	const int x = luaL_checkint(L, 2);
	const int y = luaL_checkint(L, 3);

	POINT	start, end;
	
	start.x = CHAR_getInt( index, CHAR_X);
	start.y = CHAR_getInt( index, CHAR_Y);
	
	end.x = x;
	end.y = y;

	int pos = NPC_Util_getDirFromTwoPoint( &start, &end);
	if( pos != - 1 ) {
		CHAR_walk( index, pos, 0);
	}
	return 1;
}

static int Walk(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ) return 0;
	
	const int dir = luaL_checkint(L, 2);
	const int step = luaL_checkint(L, 3);

	POINT	start, end;
	
	start.x = CHAR_getInt( index, CHAR_X);
	start.y = CHAR_getInt( index, CHAR_Y);
	
	end.x = start.x;
	end.y = start.y;
	switch(dir){
		case 0:
		{
			end.x = start.x - step;
			break;
		}
		case 1:
		{
			end.x = start.x + step;
			end.y = start.y - step;
			break;
		}
		case 2:
		{
			end.x = start.x + step;
			break;
		}
		case 3:
		{
			end.x = start.x + step;
			end.y = start.y + step;
			
			break;
		}
		case 4:
		{
			end.y = start.y + step;
			break;
		}
		case 5:
		{
			end.x = start.x - step;
			end.y = start.y + step;
			break;
		}
		case 6:
		{
			end.y = start.y - step;
			break;
		}
		case 7:
		{
			end.x = start.x - step;
			end.y = start.y - step;
			break;
		}
	}

	int pos = NPC_Util_getDirFromTwoPoint( &start, &end);
	if( dir != - 1 ) {
		CHAR_walk( index, pos, 0);
	}
	return 1;
}

static int BoundRandWalk(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ) return 0;
	
	const int minX = min(luaL_checkint(L, 2), luaL_checkint(L, 4));
	const int minY = min(luaL_checkint(L, 3), luaL_checkint(L, 5));
	const int maxX = max(luaL_checkint(L, 2), luaL_checkint(L, 4));
	const int maxY = max(luaL_checkint(L, 3), luaL_checkint(L, 5));
	
	POINT	start, end;
	int stepX = (rand() % 10) - 5;
	int stepY = (rand() % 10) - 5;
	
	start.x = CHAR_getInt( index, CHAR_X);
	start.y = CHAR_getInt( index, CHAR_Y);
	
	end.x = start.x + stepX;
	if(end.x>maxX)end.x=maxX;
	else if(end.x<minX)end.x=minX;
		
	end.y = start.y + stepY;
	if(end.y>maxY)end.y=maxY;
	else if(end.y<minY)end.y=minY;

	int dir = NPC_Util_getDirFromTwoPoint( &start, &end);

	if( dir != - 1 ) {
		CHAR_walk( index, dir, 0);
	}
	return 1;
}

static int RandRandWalk(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ) return 0;

	POINT	start, end;
	int stepX = (rand() % 8) - 4;
	int stepY = (rand() % 8) - 4;
	
	start.x = CHAR_getInt( index, CHAR_X);
	start.y = CHAR_getInt( index, CHAR_Y);
	
	end.x = start.x + stepX;
	end.y = start.y + stepY;

	

	int dir = NPC_Util_getDirFromTwoPoint( &start, &end);
	if( dir != - 1 ) {
		CHAR_walk( index, dir, 0);
	}
	return 1;
}

static int ToAroundChar(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	int objindex = CHAR_getWorkInt( index,CHAR_WORKOBJINDEX );
	CHAR_sendCToArroundCharacter( objindex);
	return 1;
}

static int talkToFloor(lua_State *L) 
{
	size_t l;
	const int floor = luaL_checkint(L, 1);
	const int talkindex = luaL_checkint(L, 2);
	char *message=luaL_checklstring(L, 3, &l);
	const int color = getCharBaseValue(L, 4, CharBaseColor, arraysizeof(CharBaseColor));
	CHAR_talkToFloor(floor, talkindex, message, color);
	return 1;
}

static int talkToParty(lua_State *L) 
{
	size_t l;
	const int talkedcharaindex = luaL_checkint(L, 1);
	const int talkcharaindex = luaL_checkint(L, 2);
	char *message=luaL_checklstring(L, 3, &l);
	const int color = getCharBaseValue(L, 4, CharBaseColor, arraysizeof(CharBaseColor));
	CHAR_talkToCliAndParty(talkedcharaindex, talkcharaindex, message, color);
	return 1;
}

static int talkToServer(lua_State *L) 
{
	size_t l;
	const int talkcharaindex = luaL_checkint(L, 1);
	char *message=luaL_checklstring(L, 2, &l);
	const int color = getCharBaseValue(L, 3, CharBaseColor, arraysizeof(CharBaseColor));
	
  int     i;
  int     playernum = CHAR_getPlayerMaxNum();

  for( i = 0 ; i < playernum ; i++) {
    if( CHAR_getCharUse(i) != FALSE ) {
			 CHAR_talkToCli( i, talkcharaindex, message, color);
    }
	}

	return 1;
}
#ifdef _ALLBLUES_LUA_1_1
static int WarpToSpecificPoint(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int floor = luaL_checkint(L, 2);
	const int x = luaL_checkint(L, 3);
	const int y = luaL_checkint(L, 4);

	CHAR_warpToSpecificPoint( charaindex, floor, x, y );
	return 1;
}
static int MapAllWarp(lua_State *L) 
{
	const int oldfloor = luaL_checkint(L, 1);
	const int newfloor = luaL_checkint(L, 2);
	const int x = luaL_checkint(L, 3);
	const int y = luaL_checkint(L, 4);

	int     i;
  int     playernum = CHAR_getPlayerMaxNum();

  for( i = 0 ; i < playernum ; i++) {
    if( CHAR_getCharUse(i) == FALSE )continue;
    if( CHAR_getInt(i, CHAR_FLOOR) != oldfloor )continue;
		CHAR_warpToSpecificPoint( i, newfloor, x, y );
	}
	return 1;
}
#endif


#ifdef _ALLBLUES_LUA_1_3
static int createPet(lua_State *L) 
{
	const int enemyid = luaL_checkint(L, 1);
	const int UpLevel = luaL_checkint(L, 2);
	
	int i;
	for( i = 0; i < ENEMY_getEnemyNum(); i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == ENEMY_getEnemyNum() ){
		lua_pushinteger(L, -1);
		return 1;
	}
	
	int petindex = ENEMY_createPet( i, -1, -1, -1, -1 );

	for( i = 0; i < UpLevel; i ++ ){
		CHAR_PetLevelUp( petindex );
		CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
	}
	CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) + UpLevel );
	CHAR_complianceParameter( petindex );
	CHAR_setInt( petindex, CHAR_HP, CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ));
	
	CHAR_LoginBesideSetWorkInt( petindex, -1 );
	lua_pushinteger(L, petindex);
	
	return 1;
}

static int setCharPet(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int petindex = luaL_checkint(L, 2);
	
	int havepetelement = CHAR_getCharPetElement( index);
  if( havepetelement > -1 ){
  	CHAR_setCharPet( index, havepetelement, petindex);
  	CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, index);
  }
	
	lua_pushinteger(L, havepetelement);
	
	return 1;
}

static int getCharPet(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int havepetid = luaL_checkint(L, 2);

	lua_pushinteger(L, CHAR_getCharPet( index, havepetid));
	
	return 1;
}

static int DelItem(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int itemindex = luaL_checkint(L, 2);
	
	CHAR_DelItem( charaindex, itemindex);

	return 1;
}

static int getFd(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);

	lua_pushinteger(L, getfdFromCharaIndex(charaindex));
	return 1;
}

static int Updata(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int indextable = getCharBaseValue(L, 2, CharBaseUpdata, arraysizeof(CharBaseUpdata));

	CHAR_send_P_StatusString( charaindex , indextable);
	return 1;
}

static int Additem(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int itemid = luaL_checkint(L, 2);
	int itemindex = -1;
	int emptyitemindexinchara = CHAR_findEmptyItemBox( charaindex );
	
	if( emptyitemindexinchara >= 0 ){
		itemindex = ITEM_makeItemAndRegist( itemid );
		
		if( itemindex > -1 ){
	  	CHAR_setItemIndex( charaindex, emptyitemindexinchara, itemindex );
	  	ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX, -1);
	  	ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX, charaindex);
	  	CHAR_sendItemDataOne( charaindex, emptyitemindexinchara);
			LogItem(
				CHAR_getChar( charaindex, CHAR_NAME ),
				CHAR_getChar( charaindex, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
				itemindex,
#else
				atoi( message),
#endif
				"AddItem(��������LUA)",
		      CHAR_getInt( charaindex,CHAR_FLOOR),
					CHAR_getInt( charaindex,CHAR_X ),
		      CHAR_getInt( charaindex,CHAR_Y ),
		      ITEM_getChar( itemindex, ITEM_UNIQUECODE),
					ITEM_getChar( itemindex, ITEM_NAME),
					ITEM_getInt( itemindex, ITEM_ID)
		
			);
		}
	}
	lua_pushinteger(L, itemindex);
	return 1;
}

static int Finditem(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int itemid = luaL_checkint(L, 2);
	int i;
	int itemindex=-1;
	int id;
	for( i=CHAR_STARTITEMARRAY;i<CHAR_MAXITEMHAVE;i++ ){
		itemindex = CHAR_getItemIndex( charaindex , i );
		if( ITEM_CHECKINDEX( itemindex) )	{
			id=ITEM_getInt(itemindex ,ITEM_ID );
			if(id == itemid){
				lua_pushinteger(L, itemindex);
				return 1;
			}
		}
	}
	lua_pushinteger(L, -1);
	return 1;
}

static int AddPet(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int enemyid = luaL_checkint(L, 2);
	const int UpLevel = luaL_checkint(L, 3);
	
	int i, havepetelement;
	for( i = 0; i < ENEMY_getEnemyNum(); i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == ENEMY_getEnemyNum() ){
		lua_pushinteger(L, -1);
		return 1;
	}
	
	for( havepetelement = 0; havepetelement < CHAR_MAXPETHAVE; havepetelement ++ ){
		int petindex = CHAR_getCharPet(index, havepetelement);
		if( !CHAR_CHECKINDEX(petindex))break;
	}
	
	if( havepetelement == CHAR_MAXPETHAVE ){
		lua_pushinteger(L, -1);
		return 1;
	}

	int petindex = ENEMY_createPet( i, -1, -1, -1, -1 );
	if( !CHAR_CHECKINDEX(petindex)){
		lua_pushinteger(L, -1);
		return 1;
	}
		
	for( i = CHAR_getInt( petindex, CHAR_LV ); i < UpLevel; i ++ ){
		CHAR_PetLevelUp( petindex );
		CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
		CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) + 1 );
	}
	
	CHAR_setCharPet( index, havepetelement, petindex);
	
	CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, index);
	
	char token[64];
	CHAR_setInt(petindex,CHAR_VARIABLEAI,10000);
	CHAR_complianceParameter( petindex );
	CHAR_setInt( petindex, CHAR_HP, CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ));

	snprintf( token, sizeof( token ), "K%d", havepetelement );
	CHAR_sendStatusString( index, token );
	snprintf( token, sizeof( token ), "W%d", havepetelement );
	CHAR_sendStatusString( index, token );
	
	LogPet(
			CHAR_getChar( index, CHAR_NAME ),
			CHAR_getChar( index, CHAR_CDKEY ),
			CHAR_getChar( petindex, CHAR_NAME),
			CHAR_getInt( petindex, CHAR_LV),
			"����LUA��char.AddPet",
			CHAR_getInt( index,CHAR_FLOOR),
			CHAR_getInt( index,CHAR_X ),
			CHAR_getInt( index,CHAR_Y ),
			CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
	
	lua_pushinteger(L, petindex);
	
	return 1;
}

static int AddPetCf(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int enemyid = luaL_checkint(L, 2);
	const int UpLevel = luaL_checkint(L, 3);
	const int trn = luaL_checkint(L, 4);
	const int vital = luaL_checkint(L, 5);
	const int str = luaL_checkint(L, 6);
	const int tgh = luaL_checkint(L, 7);
	const int dex = luaL_checkint(L, 8);
	
	int i, havepetelement;
	for( i = 0; i < ENEMY_getEnemyNum(); i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == ENEMY_getEnemyNum() ){
		lua_pushinteger(L, -1);
		return 1;
	}
	
	for( havepetelement = 0; havepetelement < CHAR_MAXPETHAVE; havepetelement ++ ){
		int petindex = CHAR_getCharPet(index, havepetelement);
		if( !CHAR_CHECKINDEX(petindex))break;
	}
	
	if( havepetelement == CHAR_MAXPETHAVE ){
		lua_pushinteger(L, -1);
		return 1;
	}

	int petindex = ENEMY_createPet( i, vital, str, tgh, dex );
	if( !CHAR_CHECKINDEX(petindex)){
		lua_pushinteger(L, -1);
		return 1;
	}

	for( i = CHAR_getInt( petindex, CHAR_LV ); i < UpLevel; i ++ ){
		CHAR_PetLevelUp( petindex );
		CHAR_PetAddVariableAi( petindex, AI_FIX_PETLEVELUP );
		CHAR_setInt( petindex, CHAR_LV, CHAR_getInt( petindex, CHAR_LV) + 1 );
		
	}
	CHAR_setInt( petindex, CHAR_TRANSMIGRATION, trn );
	
	CHAR_setCharPet( index, havepetelement, petindex);
		
//	CHAR_setChar( petindex, CHAR_OWNERCDKEY, CHAR_getChar( index, CHAR_CDKEY));
//	CHAR_setChar( petindex, CHAR_OWNERCHARANAME, CHAR_getChar( index, CHAR_NAME));
		
	CHAR_setWorkInt( petindex, CHAR_WORKPLAYERINDEX, index);
		
	char token[64];

	CHAR_setInt(petindex,CHAR_VARIABLEAI,10000);
	CHAR_complianceParameter( petindex );
	
	CHAR_setInt( petindex, CHAR_HP, CHAR_getWorkInt( petindex, CHAR_WORKMAXHP ));
	
	snprintf( token, sizeof( token ), "K%d", havepetelement );
	CHAR_sendStatusString( index, token );
	snprintf( token, sizeof( token ), "W%d", havepetelement );
	CHAR_sendStatusString( index, token );
	
	LogPet(
			CHAR_getChar( index, CHAR_NAME ),
			CHAR_getChar( index, CHAR_CDKEY ),
			CHAR_getChar( petindex, CHAR_NAME),
			CHAR_getInt( petindex, CHAR_LV),
			"����LUA��char.AddPetCf",
			CHAR_getInt( index,CHAR_FLOOR),
			CHAR_getInt( index,CHAR_X ),
			CHAR_getInt( index,CHAR_Y ),
			CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
	);
	
	lua_pushinteger(L, petindex);
	
	return 1;
}

#ifdef _PLAYER_NPC
static int setPlayerNpc(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int petindex = luaL_checkint(L, 2);
	int i;
	for( i = 1; i < CHAR_PARTYMAX; i ++ ) {
		if( CHAR_getWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1) == -1 ) {
			break;
		}
	}
	if(i == CHAR_PARTYMAX){
		CHAR_endCharOneArray( petindex );
	}else{
		if(CHAR_CHECKINDEX(petindex) == TRUE){
			Object object;
			int objindex;
		
			CHAR_setInt(petindex, CHAR_FLOOR, CHAR_getInt(charaindex, CHAR_FLOOR));
			CHAR_setInt(petindex, CHAR_X, CHAR_getInt(charaindex, CHAR_X));
			CHAR_setInt(petindex, CHAR_Y, CHAR_getInt(charaindex, CHAR_Y));
					
			object.type = OBJTYPE_CHARA;
			object.index = petindex;
			object.x =CHAR_getInt(petindex, CHAR_X);
			object.y = CHAR_getInt(petindex, CHAR_Y);
			object.floor = CHAR_getInt(petindex, CHAR_FLOOR);
			
			objindex = initObjectOne( &object );
			
			if( objindex == -1 ) {
			   CHAR_endCharOneArray( petindex );
			} else {
				CHAR_setWorkInt( petindex,CHAR_WORKOBJINDEX, objindex );
				if(CHAR_JoinParty_Main(petindex, charaindex) == FALSE){
					CHAR_CharaDelete( petindex );
					return 1;
				}
			  
			  CHAR_sendCToArroundCharacter( objindex);
			}
		}
	}
	return 1;
}
#endif
#endif
#ifdef _PETSKILL_SHOP_LUA   
static int FreePetSkill(lua_State *L) 
{
	const int petindex = luaL_checkint(L, 1);
	const int petskill = luaL_checkint(L, 2);
	
	int i;
	for( i=0; i < CHAR_MAXPETSKILLHAVE; i++)	{
		if( petskill == CHAR_getPetSkill( petindex, i) )	{
			lua_pushinteger(L, 1);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
}
#endif

static int Coordinate(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	if( CHAR_getWorkInt( index, CHAR_WORKBATTLEMODE ) != BATTLE_CHARMODE_NONE ) return 0;
	
	const int X = luaL_checkint(L, 2);
	const int Y = luaL_checkint(L, 3);
	
	POINT	start, end;

	start.x = CHAR_getInt( index, CHAR_X);
	start.y = CHAR_getInt( index, CHAR_Y);
	
	end.x = X;
	end.y = Y;

	int dir = NPC_Util_getDirFromTwoPoint( &start, &end);

	if( dir != - 1 ) {
		CHAR_walk( index, dir, 0);
	}
	return 1;
}

static int setPetSkill(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int havepetskillindex = luaL_checkint(L, 2);
	const int skillID = luaL_checkint(L, 3);
	
	CHAR_setPetSkill( index, havepetskillindex, skillID);
	
	return 1;
}

static int getPetSkill(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int havepetskillindex = luaL_checkint(L, 2);
	
	lua_pushinteger(L, CHAR_getPetSkill( index, havepetskillindex));

	return 1;
}


static int PetLevelUp(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	
	CHAR_PetLevelUp( index );
	
	return 1;
}

static int JoinParty(lua_State *L) 
{
	const int index1 = luaL_checkint(L, 1);
	const int index2 = luaL_checkint(L, 2);
	
	CHAR_JoinParty_Main(index2, index1);

	return 1;
}

#ifdef _ALLBLUES_LUA_1_4
static int getRightTo8(lua_State *L)
{
	const int value = luaL_checkint(L, 1);
	const int flg = luaL_checkint(L, 2);
	
	lua_pushinteger(L, (value >> (4 - flg) * 8) & 0xFF);

	return 1;
}

static int getLiftTo8(lua_State *L)
{
	const int value = luaL_checkint(L, 1);
	const int flg = luaL_checkint(L, 2);
	
	lua_pushinteger(L, (value << (4 - flg) * 8));

	return 1;
}

static int complianceParameter(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
	
	CHAR_complianceParameter(index);

	return 1;
}


static int sendStatusString(lua_State *L)
{
	size_t l;
	const int index = luaL_checkint(L, 1);
  char *data=luaL_checklstring(L, 2, &l);

	CHAR_sendStatusString( index, data );

	return 1;
}

static int sendBattleEffect(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
  const int val = luaL_checkint(L, 2);

	CHAR_sendBattleEffect( index, val );

	return 1;
}

static int sendWatchEvent(lua_State *L)
{
	const int objindex = luaL_checkint(L, 1);
	const int action = getCharBaseValue(L, 2, CharBaseAction, arraysizeof(CharBaseAction));
	const int flg = luaL_checkint(L, 3);

	luaL_checktype(L, 4, LUA_TTABLE);
	int n = luaL_getn(L, 4);
	int *opt =  (int *) malloc(n * sizeof(int));
	int i;

	for(i = 0; i < n; i++){
		opt[i] = getArrayInt(L, i);
	}

	CHAR_sendWatchEvent( objindex,	action, opt, n, flg);

	return 1;
}

static int dropPetAbsolute(lua_State *L)
{
	const int index = luaL_checkint(L, 1);
  const int fl = luaL_checkint(L, 2);
  const int fx = luaL_checkint(L, 3);
  const int fy = luaL_checkint(L, 4);
  const int dir = luaL_checkint(L, 5);
  const int val = luaL_checkint(L, 6);
	int dirx[9],diry[9];
	int floor, x, y, i;

	for( i  = 0 ; i < 7 ; i  ++ ){
	    dirx[i+2] = CHAR_getDX(dir + i+1);
	    diry[i+2] = CHAR_getDY(dir + i+1);
	}
	dirx[0] = CHAR_getDX(dir);
	diry[0] = CHAR_getDY(dir);
	dirx[1] = 0;
	diry[1] = 0;
	floor = fl;
	for( i = 0 ; i < 9 ; i ++ ){
	    int x=fx+dirx[i];
	    int y=fy+diry[i];
	    if( PET_isPutPoint( floor, x, y ) == TRUE ) {
	        break;
	    }
	}
	if( i == 9 ) i = 1;
	x = fx+dirx[i];
	y = fy+diry[i];

	int objindex = PET_dropPetAbsolute( index, floor, x, y, val );

	if( objindex == -1 ){
		return 1;
	}

	CHAR_setWorkInt( index, CHAR_WORKOBJINDEX, objindex );
	CHAR_setInt( index, CHAR_FLOOR, floor);
	CHAR_setInt( index, CHAR_X, x);
	CHAR_setInt( index, CHAR_Y, y);
	CHAR_setInt( index, CHAR_PUTPETTIME, NowTime.tv_sec);

	CHAR_sendCToArroundCharacter( objindex);

	return 1;
}

static int AllWarpToSpecificPoint(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int floor = luaL_checkint(L, 2);
	const int x = luaL_checkint(L, 3);
	const int y = luaL_checkint(L, 4);

	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ){
		int i;
		for( i = 0; i < CHAR_PARTYMAX; i ++ ){
			int subindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+i );
			if( CHAR_CHECKINDEX( subindex ) == FALSE ) continue;
			CHAR_warpToSpecificPoint( subindex, floor, x, y );
			ITEM_WarpDelErrorItem( subindex );
		}
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_CLIENT ){
		CHAR_talkToCli(charaindex, -1, "��Ա�޷�ʹ�á�", CHAR_COLORYELLOW);
		return FALSE;
	}else if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_NONE ){
		ITEM_WarpDelErrorItem( charaindex );
		CHAR_warpToSpecificPoint( charaindex, floor, x, y );
	}


	return 1;
}

static int Findpet(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int petid = luaL_checkint(L, 2);
	const int lv = luaL_checkint(L, 3);
	int i;
	
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		int petindex = CHAR_getCharPet( charaindex, i );
		if(!CHAR_CHECKINDEX( petindex))continue;
		if( CHAR_getInt(petindex, CHAR_PETID) == petid ){
			if(lv > 0){
				if( CHAR_getInt(petindex, CHAR_LV) != lv){
					continue;
				}
			}
			lua_pushinteger(L, petindex);
			return 1;
		}
	}

	lua_pushinteger(L, -1);
	return 1;
}

static int FindPetFormMatemo(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int metamo = luaL_checkint(L, 2);
	const int lv = luaL_checkint(L, 3);
	int i;
	
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		int petindex = CHAR_getCharPet( charaindex, i );
		if(!CHAR_CHECKINDEX( petindex))continue;
		if( CHAR_getInt(petindex, CHAR_BASEBASEIMAGENUMBER) == metamo ){
			if(lv > 0){
				if( CHAR_getInt(petindex, CHAR_LV) != lv){
					continue;
				}
			}
			lua_pushinteger(L, petindex);
			return 1;
		}
	}

	lua_pushinteger(L, -1);
	return 1;
}

static int FindPetFormEnemyTempID(lua_State *L) 
{
	size_t l;
	const int charaindex = luaL_checkint(L, 1);
	char *data=luaL_checklstring(L, 2, &l);
	char token[64];
	int enemytempid = -1, lv = 0;
	int i;
	
	if(getStringFromIndexWithDelim(data, "-", 1, token, sizeof(token)) == TRUE){
		lv = atoi(token);
	}else{
		lua_pushinteger(L, -1);
	}
	
	if(getStringFromIndexWithDelim(data, "-", 2, token, sizeof(token)) == TRUE){
		enemytempid = atoi(token);
	}else{
		lua_pushinteger(L, -1);
	}

	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		int petindex = CHAR_getCharPet( charaindex, i );
		if(!CHAR_CHECKINDEX( petindex))continue;

		if( CHAR_getInt( petindex, CHAR_PETID) == enemytempid ){
			if(lv > 0){
				if( CHAR_getInt(petindex, CHAR_LV) != lv){
					continue;
				}
			}
			lua_pushinteger(L, petindex);
			return 1;
		}
	}

	lua_pushinteger(L, -1);
	return 1;
}

static int DelPet(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int petindex = luaL_checkint(L, 2);
	int i;
	char category[12];
	
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		if( petindex == CHAR_getCharPet( charaindex, i ) ){
			if( CHAR_getInt( charaindex, CHAR_RIDEPET) == i ) {
				CHAR_setInt( charaindex, CHAR_RIDEPET, -1);
				CHAR_send_P_StatusString( charaindex, CHAR_P_STRING_RIDEPET );
				CHAR_complianceParameter( charaindex );
				CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));
			}
			
			CHAR_endCharOneArray( petindex );
			CHAR_setCharPet( charaindex, i, -1);
			snprintf( category, sizeof( category), "K%d",i);
			CHAR_sendStatusString( charaindex, category );
			break;
		}
	}
	return 1;
}

#endif

#ifdef _ALLBLUES_LUA_1_7
static int findEmptyItemBox(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);

	lua_pushinteger(L, CHAR_findEmptyItemBox(charaindex));
	return 1;
}

static int findEmptyPetBox(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	int num = 0;
	int i;
	for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
		int petindex = CHAR_getCharPet( charaindex, i );
		if(!CHAR_CHECKINDEX(petindex)){
			num ++ ;
		}
	}
	lua_pushinteger(L, num);
	return 1;
}

static int dropPetFollow(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int havepetindex = luaL_checkint(L, 2);
	char szPet[128];
    int dirx[9],diry[9];
    int i;
    int objindex=-1;
    int floor,x,y;
    int petindex;

	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return FALSE;
    petindex = CHAR_getCharPet(charaindex,havepetindex);
    if( petindex == -1  ) return FALSE;
    if( !CHAR_CHECKINDEX( charaindex ) )return FALSE;
    if( CHAR_CHECKINDEX( petindex) == FALSE ) return FALSE;

	if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){
     	CHAR_talkToCli(charaindex, -1, "�����ػ����޷�������", CHAR_COLORYELLOW);
    	return	FALSE;  
  }
  if (CHAR_getInt(charaindex, CHAR_RIDEPET) == havepetindex){
  	CHAR_talkToCli(charaindex, -1, "����еĳ����޷����棡", CHAR_COLORYELLOW);
  	return	FALSE;
  }

	for( i  = 0 ; i < 7 ; i  ++ ){
	    dirx[i+2] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
	    diry[i+2] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
	}
	dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
	diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
	dirx[1] = 0;
	diry[1] = 0;

	floor = CHAR_getInt( charaindex,CHAR_FLOOR );
	for( i = 0 ; i < 9 ; i ++ ){
	    int x=CHAR_getInt(charaindex,CHAR_X)+dirx[i];
	    int y=CHAR_getInt(charaindex,CHAR_Y)+diry[i];
	    if( PET_isPutPoint( floor, x, y ) == TRUE ) {
	        break;
	    }
	}
	if( i == 9 ) i = 1;

	x=CHAR_getInt(charaindex,CHAR_X)+dirx[i];
	y=CHAR_getInt(charaindex,CHAR_Y)+diry[i];

  objindex = PET_dropPetAbsolute( petindex,floor,x,y, FALSE );
  if( objindex == -1 ) return FALSE;
  
  CHAR_setWorkInt( petindex,CHAR_WORKOBJINDEX,objindex );
  CHAR_setCharPet( charaindex, havepetindex, -1);
	CHAR_setInt( petindex, CHAR_FLOOR, floor);
	CHAR_setInt( petindex, CHAR_X, x);
	CHAR_setInt( petindex, CHAR_Y, y);
	CHAR_setInt( petindex, CHAR_PUTPETTIME, NowTime.tv_sec);
	if( havepetindex == CHAR_getInt( charaindex, CHAR_DEFAULTPET)) {
		int	fd;
		CHAR_setInt( charaindex, CHAR_DEFAULTPET, -1);
	    fd = getfdFromCharaIndex( charaindex);
		lssproto_KS_send( fd, havepetindex, 0);

	}
	CHAR_sendCToArroundCharacter( objindex);
  if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER){
#ifdef _PET_ITEM
		CHAR_sendPetItemData( charaindex, havepetindex);
#endif		
		snprintf( szPet, sizeof( szPet ), "K%d", havepetindex );
		CHAR_sendStatusString( charaindex, szPet );
  }

	CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, petindex);
	CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWMODE, CHAR_PETFOLLOW_NOW);
	CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
	CHAR_setInt( petindex, CHAR_PUTPETTIME, (int)(NowTime.tv_sec));
	CHAR_setInt( petindex, CHAR_WORKPLAYERINDEX, charaindex);
	LogPet(
		CHAR_getChar( charaindex, CHAR_NAME ),
		CHAR_getChar( charaindex, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"Follow(lua���)",
		CHAR_getInt( charaindex,CHAR_FLOOR),
		CHAR_getInt( charaindex,CHAR_X ),
		CHAR_getInt( charaindex,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)
	);
    return 1;
}

static int getItemIndex(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int haveitemindex = luaL_checkint(L, 2);
	
	lua_pushinteger(L, CHAR_getItemIndex( charaindex, haveitemindex ));
	return 1;
	
}

static int charSaveFromConnect(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	if( CHAR_getCharUse(charaindex) != FALSE ){
		CHAR_charSaveFromConnect( charaindex, FALSE );
   }
	return 1;
}

#ifdef _RECORD_IP
static int userip(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);

	unsigned long ip;
	int a,b,c,d;
	char strIP[32];
	
  ip = CHAR_getWorkInt(charaindex, CHAR_WORK_RECORD_IP);
  
  a=(ip % 0x100); ip=ip / 0x100;
  b=(ip % 0x100); ip=ip / 0x100;
  c=(ip % 0x100); ip=ip / 0x100;
  d=(ip % 0x100);
  
	sprintf(strIP, "%d.%d.%d.%d", a, b, c, d);

	lua_pushstring(L, strIP);
	return 1;
}
#endif

static int DischargeParty(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int flg = luaL_checkint(L, 2);
	CHAR_DischargeParty(charaindex, flg);
	return 1;
}


static int Skillupsend(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	
	CHAR_Skillupsend(charaindex);
	return 1;
}
#endif

#ifdef _ALLBLUES_LUA_1_9
static int logou(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	
	CHAR_logout(charaindex, TRUE);
	return 1;
}
#endif
//void CHAR_WXSYSCMD_setStayEncount(int charaindex, char* message )
#ifdef _ALLBLUES_LUA_2_0
static int Encounter(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	if(CHAR_CHECKINDEX(charaindex) == FALSE) return 0;
	int fd=CHAR_getWorkInt( charaindex, CHAR_WORKFD);
	setStayEncount(fd);
#ifdef _USER_CHARLOOPS
	{
		Char 	*ch;
		ch  = CHAR_getCharPointer( charaindex);
		if( ch == NULL ) return;
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
			sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "CHAR_BattleStayLoop");//ս��
		CHAR_setInt( charaindex, CHAR_LOOPINTERVAL, 2500);
		CHAR_constructFunctable( charaindex);
	}
#endif
}

static int GetOldLevelExp(lua_State *L) 
{
	size_t l;
	const int level = luaL_checkint(L, 1);
	CHAR_GetOldLevelExp(level);
	return 1;
}
#endif
#ifdef _AutoBattle_LUA
static int gcgc(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	
	Enemy_setStayLoop(charaindex);
	return 1;
}
#endif

#ifdef _FIX_PLAYER_NPC_PET
static int DelNpcPetByIndex(lua_State *L)
{
	const int petindex = luaL_checkint(L, 1);
	CHAR_endCharOneArray( petindex );
	return 1;
}

#endif

static const luaL_Reg charlib[] = {
	{"getCharNum", 						getCharNum},
	{"getPlayerMaxNum", 			getPlayerMaxNum},
	{"getPetMaxNum", 					getPetMaxNum},
	{"getOthersMaxNum", 			getOthersMaxNum},
	{"check", 								check},
	{"setWorkChar", 					setWorkChar},
	{"getWorkChar", 					getWorkChar},
	{"setWorkInt", 						setWorkInt},
	{"getWorkInt", 						getWorkInt},
	{"setChar", 							setChar},
  {"getChar", 							getChar},
  {"setInt", 								setInt},
  {"getInt", 								getInt},
  {"setFlg", 								setFlg},
  {"getFlg", 								getFlg},
  {"setFunctionPointer", 		setFunctionPointer},
  {"delFunctionPointer", 		delFunctionPointer},
  {"TalkToCli", 						TalkToCli},
  {"TalkToRound", 					TalkToRound},
  {"talkToFloor", 					talkToFloor},
  {"talkToParty", 					talkToParty},
  {"talkToServer", 					talkToServer},
  {"BoundRandWalk", 				BoundRandWalk},
  {"ToAroundChar", 					ToAroundChar},
#ifdef _ALLBLUES_LUA_1_1
  {"Walk", 									Walk},
  {"WalkPoint", 						WalkPoint},
  {"WarpToSpecificPoint", 	WarpToSpecificPoint},
  {"MapAllWarp", 						MapAllWarp},
  {"RandRandWalk", 					RandRandWalk},
#endif
#ifdef _ALLBLUES_LUA_1_3
	{"createPet", 						createPet},
	{"setCharPet", 						setCharPet},
	{"getCharPet", 						getCharPet},
	{"DelItem", 							DelItem},	
	{"getFd", 								getFd},
	{"Updata", 								Updata},
	{"Additem", 							Additem},
	{"AddPet", 								AddPet},
	{"Finditem", 							Finditem},
#ifdef _PLAYER_NPC
  {"setPlayerNpc", 					setPlayerNpc},
#endif
  {"Coordinate", 						Coordinate}, 
  {"setPetSkill", 					setPetSkill}, 
  {"getPetSkill", 					getPetSkill}, 
  {"PetLevelUp", 						PetLevelUp}, 
  {"JoinParty", 						JoinParty}, 
#endif
#ifdef _ALLBLUES_LUA_1_4
  {"getLiftTo8", 						getLiftTo8},
  {"getRightTo8", 					getRightTo8},
  {"complianceParameter", 	complianceParameter},
  {"sendStatusString", 			sendStatusString},
  {"sendBattleEffect", 			sendBattleEffect},
  {"dropPetAbsolute", 			dropPetAbsolute},
  {"AddPetCf", 							AddPetCf},
  {"sendWatchEvent", 				sendWatchEvent},
  {"AllWarpToSpecificPoint",AllWarpToSpecificPoint},
  {"Findpet",								Findpet},
  {"DelPet",								DelPet},
  {"FindPetFormEnemyTempID",FindPetFormEnemyTempID},
  {"FindPetFormMatemo",			FindPetFormMatemo},
#endif
#ifdef _PETSKILL_SHOP_LUA   
  {"FreePetSkill", 					FreePetSkill},
#endif
#ifdef _ALLBLUES_LUA_1_7
  {"findEmptyPetBox", 			findEmptyPetBox},
  {"findEmptyItemBox", 			findEmptyItemBox},
  {"dropPetFollow", 				dropPetFollow},
  {"getItemIndex", 					getItemIndex},
  {"charSaveFromConnect", 	charSaveFromConnect},
  {"DischargeParty", 				DischargeParty},
  {"Skillupsend", 					Skillupsend},
#ifdef _RECORD_IP
  {"userip", 								userip},
#endif
#endif
#ifdef _ALLBLUES_LUA_1_9
	{"logou", 								logou},
#endif
#ifdef _ALLBLUES_LUA_2_0
  {"Encounter", 						Encounter},
	{"GetOldLevelExp", 				GetOldLevelExp},
#endif
#ifdef _AutoBattle_LUA
	{"gcgc", 								  gcgc},
#endif
#ifdef _FIX_PLAYER_NPC_PET
	{"DelNpcPetByIndex", DelNpcPetByIndex},
#endif
  {NULL, 										NULL}
};

LUALIB_API int luaopen_Char (lua_State *L) {
  luaL_register(L, "char", charlib);
  return 1;
}

#endif
