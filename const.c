#include "version.h"
#include "common.h"

#include "char.h"
#include "char_data.h"
#include "item.h"
#include "battle.h"
#include "object.h"
#ifdef _CAX_ADD_LUADX
#include "enemy.h"
#endif
//0-1999 CHAR_getInt
//2000-3999 CHAR_getChar
//4000-5999 CHAR_getWorkInt
//6000-7999 CHAR_getWorkChar
//8000-9999 CHAR_getFlg
#define LUA_DATALINE0 0
#define LUA_DATALINE1 2000
#define LUA_DATALINE2 4000
#define LUA_DATALINE3 6000
#define LUA_DATALINE4 8000
#define LUA_DATALINE5 10000

typedef struct __lua_const {
  const char *name;
  unsigned int size;
  int vl;
}lua_const;

lua_const M_Lua_RegConstList[] = {
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + CHAR_DATAPLACENUMBER},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_BASEIMAGENUMBER},
	{"����_ԭ��%", sizeof("����_ԭ��%"), LUA_DATALINE0 + CHAR_BASEBASEIMAGENUMBER},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + CHAR_FACEIMAGENUMBER},
	{"����_��ͼ%", sizeof("����_��ͼ%"), LUA_DATALINE0 + CHAR_FLOOR},
	{"����_X%", sizeof("����_X%"), LUA_DATALINE0 + CHAR_X},
	{"����_Y%", sizeof("����_Y%"), LUA_DATALINE0 + CHAR_Y},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_DIR},
	{"����_�ȼ�%", sizeof("����_�ȼ�%"), LUA_DATALINE0 + CHAR_LV},
	{"����_ʯ��%", sizeof("����_ʯ��%"), LUA_DATALINE0 + CHAR_GOLD},
	{"����_Ѫ%", sizeof("����_Ѫ%"), LUA_DATALINE0 + CHAR_HP},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + CHAR_MP},
	{"����_M��%", sizeof("����_M��%"), LUA_DATALINE0 + CHAR_MAXMP},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_VITAL},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_STR},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_TOUGH},
	{"����_�ٶ�%", sizeof("����_�ٶ�%"), LUA_DATALINE0 + CHAR_DEX},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_CHARM},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + CHAR_LUCK},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + CHAR_EARTHAT},
	{"����_ˮ%", sizeof("����_ˮ%"), LUA_DATALINE0 + CHAR_WATERAT},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + CHAR_FIREAT},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + CHAR_WINDAT},
	{"����_ս��%", sizeof("����_ս��%"), LUA_DATALINE0 + CHAR_DEFAULTPET},
	{"����_CL%", sizeof("����_CL%"), LUA_DATALINE0 + CHAR_CRITIAL},
	{"����_CR%", sizeof("����_CR%"), LUA_DATALINE0 + CHAR_COUNTER},
	{"����_��ϡ%", sizeof("����_��ϡ%"), LUA_DATALINE0 + CHAR_RARE},
	{"����_RH%", sizeof("����_RH%"), LUA_DATALINE0 + CHAR_RADARSTRENGTH},
	{"����_CE%", sizeof("����_CE%"), LUA_DATALINE0 + CHAR_CHATVOLUME},
	{"����_ML%", sizeof("����_ML%"), LUA_DATALINE0 + CHAR_MERCHANTLEVEL},
	{"����_��¼%", sizeof("����_��¼%"), LUA_DATALINE0 + CHAR_SAVEPOINT},
	{"����_DM%", sizeof("����_DM%"), LUA_DATALINE0 + CHAR_DETERMINEITEM},
	{"����_�ƺ�%", sizeof("����_�ƺ�%"), LUA_DATALINE0 + CHAR_INDEXOFEQTITLE},
	{"����_�ƶ�%", sizeof("����_�ƶ�%"), LUA_DATALINE0 + CHAR_POISON},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_PARALYSIS},
	{"����_��Ĭ%", sizeof("����_��Ĭ%"), LUA_DATALINE0 + CHAR_SLEEP},
	{"����_��ʯ%", sizeof("����_��ʯ%"), LUA_DATALINE0 + CHAR_STONE},
	{"����_�ư�%", sizeof("����_�ư�%"), LUA_DATALINE0 + CHAR_DRUNK},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_CONFUSION},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_LOGINCOUNT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_DEADCOUNT},
	{"����_�ߴ�%", sizeof("����_�ߴ�%"), LUA_DATALINE0 + CHAR_WALKCOUNT},
	{"����_˵��%", sizeof("����_˵��%"), LUA_DATALINE0 + CHAR_TALKCOUNT},
	{"����_�˴�%", sizeof("����_�˴�%"), LUA_DATALINE0 + CHAR_DAMAGECOUNT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_GETPETCOUNT},
	{"����_ɱ��%", sizeof("����_ɱ��%"), LUA_DATALINE0 + CHAR_KILLPETCOUNT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_DEADPETCOUNT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_SENDMAILCOUNT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_MERGEITEMCOUNT},
	{"����_PK��%", sizeof("����_PK��%"), LUA_DATALINE0 + CHAR_DUELBATTLECOUNT},
	{"����_PӮ��%", sizeof("����_PӮ��%"), LUA_DATALINE0 + CHAR_DUELWINCOUNT},
	{"����_P����%", sizeof("����_P����%"), LUA_DATALINE0 + CHAR_DUELLOSECOUNT},
	{"����_P����%", sizeof("����_P����%"), LUA_DATALINE0 + CHAR_DUELSTWINCOUNT},
	{"����_P���%", sizeof("����_P���%"), LUA_DATALINE0 + CHAR_DUELMAXSTWINCOUNT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_WHICHTYPE},
	{"����_�Ƽ�%", sizeof("����_�Ƽ�%"), LUA_DATALINE0 + CHAR_WALKINTERVAL},
	{"����_ѭʱ%", sizeof("����_ѭʱ%"), LUA_DATALINE0 + CHAR_LOOPINTERVAL},
#ifdef _NEWOPEN_MAXEXP
	{"����_�ɾ�%", sizeof("����_�ɾ�%"), LUA_DATALINE0 + CHAR_OLDEXP},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_EXP},
#else
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_EXP},
#endif

#ifdef _CAX_ADD_LUADX
	{"����_ѪMAX%", sizeof("����_ѪMAX%"), LUA_DATALINE0 + CHAR_WORKMAXHP},
	{"����_��MAX%", sizeof("����_��MAX%"), LUA_DATALINE0 + CHAR_WORKMAXMP},
	{"����_�����ͼ%", sizeof("����_�����ͼ%"), LUA_DATALINE2 + CHAR_WORKFMFLOOR},
	{"����_�ɳ�%", sizeof("����_�ɳ�%"), LUA_DATALINE0 + CHAR_ALLOCPOINT},
	{"����_PETRANK%", sizeof("����_PETRANK%"), LUA_DATALINE0 + CHAR_PETRANK},
	{"����_ENEMYBASE���%", sizeof("����_ENEMYBASE���%"), LUA_DATALINE0 + CHAR_PETID},
	{"����_��̯%", sizeof("����_��̯%"), LUA_DATALINE0 + CHAR_WORKSTREETVENDOR},
	{"����_��̯��%", sizeof("����_��̯��%"), LUA_DATALINE0 + CHAR_WORKSTREETVENDOR_WHO},
	{"����_̯��%", sizeof("����_̯��%"), LUA_DATALINE0 + CHAR_STREETVENDOR_NAME},
	{"����_���%", sizeof("����_���%"), LUA_DATALINE0 + CHAR_LOVE},
	{"����_��ż��%", sizeof("����_��ż��%"), LUA_DATALINE0 + CHAR_LOVERID},
	{"����_��ż��%", sizeof("����_��ż��%"), LUA_DATALINE0 + CHAR_LOVERNAME},
	{"�ɳ�_�ٶ�%", sizeof("�ɳ�_�ٶ�%"), 0},
	{"�ɳ�_����%", sizeof("�ɳ�_����%"), 1},
	{"�ɳ�_����%", sizeof("�ɳ�_����%"), 2},
	{"�ɳ�_����%", sizeof("�ɳ�_����%"), 3},
	{"����_�ٶȳɳ�%", sizeof("����_�ٶȳɳ�%"), 0},
	{"����_�����ɳ�%", sizeof("����_�����ɳ�%"), 1},
	{"����_�����ɳ�%", sizeof("����_�����ɳ�%"), 2},
	{"����_�����ɳ�%", sizeof("����_�����ɳ�%"), 3},
	{"��ģ_���%", sizeof("��ģ_���%"), LUA_DATALINE0 + E_T_TEMPNO},
	{"��ģ_��ʼ%", sizeof("��ģ_��ʼ%"), LUA_DATALINE0 + E_T_INITNUM},
	{"��ģ_������%", sizeof("��ģ_������%"), E_T_LVUPPOINT},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_BASEVITAL},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_BASESTR},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_BASETGH},
	{"��ģ_�ٶ�%", sizeof("��ģ_�ٶ�%"), LUA_DATALINE0 + E_T_BASEDEX},
	{"��ģ_AI%", sizeof("��ģ_AI%"), LUA_DATALINE0 + E_T_MODAI},
	{"��ģ_GET%", sizeof("��ģ_GET%"), LUA_DATALINE0 + E_T_GET},
	{"��ģ_��%", sizeof("��ģ_��%"), LUA_DATALINE0 + E_T_EARTHAT},
	{"��ģ_ˮ%", sizeof("��ģ_ˮ%"), LUA_DATALINE0 + E_T_WATERAT},
	{"��ģ_��%", sizeof("��ģ_��%"), LUA_DATALINE0 + E_T_FIREAT},
	{"��ģ_��%", sizeof("��ģ_��%"), LUA_DATALINE0 + E_T_WINDAT},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_POISON},
	{"��ģ_�鿹%", sizeof("��ģ_�鿹%"), LUA_DATALINE0 + E_T_PARALYSIS},
	{"��ģ_Ĭ��%", sizeof("��ģ_Ĭ��%"), LUA_DATALINE0 + E_T_SLEEP},
	{"��ģ_ʯ��%", sizeof("��ģ_ʯ��%"), LUA_DATALINE0 + E_T_STONE},
	{"��ģ_��%", sizeof("��ģ_��%"), LUA_DATALINE0 + E_T_DRUNK},
	{"��ģ_�ҿ�%", sizeof("��ģ_�ҿ�%"), LUA_DATALINE0 + E_T_CONFUSION},
	{"��ģ_�輼1%", sizeof("��ģ_�輼1%"), LUA_DATALINE0 + E_T_PETSKILL1},
	{"��ģ_�輼2%", sizeof("��ģ_�輼2%"), LUA_DATALINE0 + E_T_PETSKILL2},
	{"��ģ_�輼3%", sizeof("��ģ_�輼3%"), LUA_DATALINE0 + E_T_PETSKILL3},
	{"��ģ_�輼4%", sizeof("��ģ_�輼4%"), LUA_DATALINE0 + E_T_PETSKILL4},
	{"��ģ_�輼5%", sizeof("��ģ_�輼5%"), LUA_DATALINE0 + E_T_PETSKILL5},
	{"��ģ_�輼6%", sizeof("��ģ_�輼6%"), LUA_DATALINE0 + E_T_PETSKILL6},
	{"��ģ_�輼7%", sizeof("��ģ_�輼7%"), LUA_DATALINE0 + E_T_PETSKILL7},
	{"��ģ_��ϡ%", sizeof("��ģ_��ϡ%"), LUA_DATALINE0 + E_T_RARE},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_CRITICAL},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_COUNTER},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_SLOT},
	{"��ģ_ͼ��%", sizeof("��ģ_ͼ��%"), LUA_DATALINE0 + E_T_IMGNUMBER},
	{"��ģ_FLG%", sizeof("��ģ_FLG%"), LUA_DATALINE0 + E_T_PETFLG},
	{"��ģ_�ߴ�%", sizeof("��ģ_�ߴ�%"), LUA_DATALINE0 + E_T_SIZE},
	{"��ģ_����%", sizeof("��ģ_����%"), LUA_DATALINE0 + E_T_LIMITLEVEL},
	{"��ģ_�ں���%", sizeof("��ģ_�ں���%"), LUA_DATALINE0 + E_T_FUSIONCODE},
	{"��ģ_ԭ��%", sizeof("��ģ_ԭ��%"), LUA_DATALINE1 + E_T_NAME},
#endif

	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_LASTTALKELDER},
	{"����_���%", sizeof("����_���%"), LUA_DATALINE0 + CHAR_SKILLUPPOINT},
	{"����_LT%", sizeof("����_LT%"), LUA_DATALINE0 + CHAR_LEVELUPPOINT},
	{"����_ͼ��%", sizeof("����_ͼ��%"), LUA_DATALINE0 + CHAR_IMAGETYPE},
	{"����_CLR%", sizeof("����_CLR%"), LUA_DATALINE0 + CHAR_NAMECOLOR},
	{"����_��ɫ%", sizeof("����_��ɫ%"), LUA_DATALINE0 + CHAR_POPUPNAMECOLOR},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_LASTTIMESETLUCK},
	{"����_DP%", sizeof("����_DP%"), LUA_DATALINE0 + CHAR_DUELPOINT},
	{"����_ת��%", sizeof("����_ת��%"), LUA_DATALINE0 + CHAR_TRANSMIGRATION},
	{"����_ת��%", sizeof("����_ת��%"), LUA_DATALINE0 + CHAR_TRANSEQUATION},
	{"����_IA%", sizeof("����_IA%"), LUA_DATALINE0 + CHAR_INITDATA},
	{"����_��ʱ%", sizeof("����_��ʱ%"), LUA_DATALINE0 + CHAR_SILENT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_FMINDEX},
	{"����_��ְ%", sizeof("����_��ְ%"), LUA_DATALINE0 + CHAR_FMLEADERFLAG},
	{"����_�ؾ�%", sizeof("����_�ؾ�%"), LUA_DATALINE0 + CHAR_FMSPRITE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_BANKGOLD},
	{"����_���%", sizeof("����_���%"), LUA_DATALINE0 + CHAR_RIDEPET},
	{"����_���%", sizeof("����_���%"), LUA_DATALINE0 + CHAR_LEARNRIDE},
	{"����_���%", sizeof("����_���%"), LUA_DATALINE0 + CHAR_LIMITLEVEL},
#ifdef _PET_FUSION
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_FUSIONCODE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_FUSIONINDEX},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_FUSIONRAISE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_FUSIONBEIT},
	{"����_��ʱ%", sizeof("����_��ʱ%"), LUA_DATALINE0 + CHAR_FUSIONTIMELIMIT},
#endif
#ifdef _DEATH_CONTEND
	{"����_�ӱ�%", sizeof("����_�ӱ�%"), LUA_DATALINE0 + CHAR_PKLISTTEAMNUM},
	{"����_���%", sizeof("����_���%"), LUA_DATALINE0 + CHAR_PKLISTLEADER},
#endif
#ifdef _PERSONAL_FAME
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_FAME},
#endif
#ifdef _CAX_Mercenary
  {"����_Ӷ������%",       sizeof("����_Ӷ������%"), LUA_DATALINE0 + CHAR_MERCENARY},  //Ӷ������
#endif
#ifdef _VIP_RIDE
	{"����_VIP����%", sizeof("����_VIP����%"), LUA_DATALINE0 + CHAR_VIPRIDE},
	{"����_VIPʱ��%", sizeof("����_VIPʱ��%"), LUA_DATALINE0 + CHAR_VIPTIME},
#endif
#ifdef _VIP_SERVER
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_AMPOINT},
#endif
#ifdef _EV_NUM
	{"����_�ܻ�%", sizeof("����_�ܻ�%"), LUA_DATALINE0 + CHAR_EVNUM},
#endif
  {"����_������%",     sizeof("����_������%"), LUA_DATALINE0 + CHAR_MISSIONTRAIN_NUM},
  {"����_������ʱ��%", sizeof("����_������ʱ��%"), LUA_DATALINE0 + CHAR_MISSIONTRAIN_TIME},
  {"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_MISSIONTRAIN_NUM}, // �ܻ���
  {"����_����%",       sizeof("����_����%"), LUA_DATALINE0 + CHAR_EVOLVE},
  {"����_����ʱ��%",   sizeof("����_����ʱ��%"), LUA_DATALINE0 + CHAR_ONLINE_TIME},
  {"����_¥��ʱ��%",   sizeof("����_¥��ʱ��%"), LUA_DATALINE0 + CHAR_FLOORDIFI_TIME},
  {"����_��Ʒ%",       sizeof("����_��Ʒ%"), LUA_DATALINE0 + CHAR_SUPER},
  {"����_�ƺŵȼ�%",   sizeof("����_�ƺŵȼ�%"), LUA_DATALINE0 + CHAR_TITLE_LV},
  {"����_�ƺ�ʱ��%",   sizeof("����_�ƺ�ʱ��%"), LUA_DATALINE0 + CHAR_TITLE_TIME},
  {"����_�����ʱ%",   sizeof("����_�����ʱ%"), LUA_DATALINE0 + CHAR_MISSION_TIME},

#ifdef _ATTACK_MAGIC
	{"����_��ħ��%", sizeof("����_��ħ��%"), LUA_DATALINE0 + CHAR_EARTH_EXP},
	{"����_ˮħ��%", sizeof("����_ˮħ��%"), LUA_DATALINE0 + CHAR_WATER_EXP},
	{"����_��ħ��%", sizeof("����_��ħ��%"), LUA_DATALINE0 + CHAR_FIRE_EXP},
	{"����_��ħ��%", sizeof("����_��ħ��%"), LUA_DATALINE0 + CHAR_WIND_EXP},
	{"����_��ħ��%", sizeof("����_��ħ��%"), LUA_DATALINE0 + CHAR_EARTH_RESIST},
	{"����_ˮħ��%", sizeof("����_ˮħ��%"), LUA_DATALINE0 + CHAR_WATER_RESIST},
	{"����_��ħ��%", sizeof("����_��ħ��%"), LUA_DATALINE0 + CHAR_FIRE_RESIST},
	{"����_��ħ��%", sizeof("����_��ħ��%"), LUA_DATALINE0 + CHAR_WIND_RESIST},
	{"����_���쾭%", sizeof("����_���쾭%"), LUA_DATALINE0 + CHAR_EARTH_ATTMAGIC_EXP},
	{"����_ˮ�쾭%", sizeof("����_ˮ�쾭%"), LUA_DATALINE0 + CHAR_WATER_ATTMAGIC_EXP},
	{"����_���쾭%", sizeof("����_���쾭%"), LUA_DATALINE0 + CHAR_FIRE_ATTMAGIC_EXP},
	{"����_���쾭%", sizeof("����_���쾭%"), LUA_DATALINE0 + CHAR_WIND_ATTMAGIC_EXP},
	{"����_�ؿ���%", sizeof("����_�ؿ���%"), LUA_DATALINE0 + CHAR_EARTH_DEFMAGIC_EXP},
	{"����_ˮ����%", sizeof("����_ˮ����%"), LUA_DATALINE0 + CHAR_WATER_DEFMAGIC_EXP},
	{"����_�𿹾�%", sizeof("����_�𿹾�%"), LUA_DATALINE0 + CHAR_FIRE_DEFMAGIC_EXP},
	{"����_�翹��%", sizeof("����_�翹��%"), LUA_DATALINE0 + CHAR_WIND_DEFMAGIC_EXP},
#endif
#ifdef _GAMBLE_BANK
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_PERSONAGOLD},
#endif
#ifdef _DROPSTAKENEW
	{"����_�ķ�%", sizeof("����_�ķ�%"), LUA_DATALINE0 + CHAR_GAMBLENUM},
#endif
#ifdef _AUCTIONEER
	{"����_�ĵ�%", sizeof("����_�ĵ�%"), LUA_DATALINE0 + CHAR_AUCGOLD},
#endif
#ifdef _PET_EVOLUTION
	{"����_Ѫҩ��%", sizeof("����_Ѫҩ��%"), LUA_DATALINE0 + CHAR_EVOLUTIONBASEVTL},
	{"����_��ҩ��%", sizeof("����_��ҩ��%"), LUA_DATALINE0 + CHAR_EVOLUTIONBASESTR},
	{"����_��ҩ��%", sizeof("����_��ҩ��%"), LUA_DATALINE0 + CHAR_EVOLUTIONBASETGH},
	{"����_��ҩ��%", sizeof("����_��ҩ��%"), LUA_DATALINE0 + CHAR_EVOLUTIONBASEDEX},
#endif
#ifdef _FAMILYBANKSTONELOG
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_FMBANKGOLD},
#endif
#ifdef _FM_JOINLIMIT
	{"����_�����%", sizeof("����_�����%"), LUA_DATALINE0 + CHAR_FMTIMELIMIT},
#endif
#ifdef _CHAR_PROFESSION			// WON ADD ����ְҵ
	{"����_ְҵ%", sizeof("����_ְҵ%"), LUA_DATALINE0 + PROFESSION_CLASS},
	{"����_ְ��%", sizeof("����_ְ��%"), LUA_DATALINE0 + PROFESSION_LEVEL},
	//{"����_ְ��", sizeof("����_ְ��"), LUA_DATALINE0 + PROFESSION_EXP},
	{"����_ְ����%", sizeof("����_ְ����%"), LUA_DATALINE0 + PROFESSION_SKILL_POINT},
	{"����_ְ����%", sizeof("����_ְ����%"), LUA_DATALINE0 + ATTACHPILE},
	{"����_ְ����%", sizeof("����_ְ����%"), LUA_DATALINE0 + PROFESSION_FIRE_P},
	{"����_ְ����%", sizeof("����_ְ����%"), LUA_DATALINE0 + PROFESSION_ICE_P},
	{"����_ְ����%", sizeof("����_ְ����%"), LUA_DATALINE0 + PROFESSION_THUNDER_P},
	{"����_ְ��%", sizeof("����_ְ��%"), LUA_DATALINE0 + PROFESSION_FIRE_R},
	{"����_ְ����%", sizeof("����_ְ����%"), LUA_DATALINE0 + PROFESSION_ICE_R},
	{"����_ְ�׿�%", sizeof("����_ְ�׿�%"), LUA_DATALINE0 + PROFESSION_THUNDER_R},
#endif
#ifdef _ALLDOMAN // (���ɿ�) Syu ADD ���а�NPC
	{"����_ս��%", sizeof("����_ս��%"), LUA_DATALINE0 + CHAR_HEROFLOOR},
#endif
#ifdef _PETSKILL_BECOMEPIG
	{"����_����ʱ%", sizeof("����_����ʱ%"), LUA_DATALINE0 + CHAR_BECOMEPIG},
	{"����_������%", sizeof("����_������%"), LUA_DATALINE0 + CHAR_BECOMEPIG_BBI},
#endif
	{"����_����ʱ%", sizeof("����_����ʱ%"), LUA_DATALINE0 + CHAR_LASTLEAVETIME},
#ifdef _NEW_MANOR_LAW
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_MOMENTUM},
#endif
#ifdef _ANGEL_SUMMON
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_HEROCNT},
#endif
#ifdef _TEACHER_SYSTEM
	{"����_������%", sizeof("����_������%"), LUA_DATALINE0 + CHAR_TEACHER_FAME},
#endif
#ifdef _CAX_ESC_REPORT
	{"����_ǩ��%", sizeof("����_ǩ��%"), LUA_DATALINE0 + CHAR_REPORT},
#endif
#ifdef _RACEMAN
	{"����_CKN%", sizeof("����_CKN%"), LUA_DATALINE0 + CHAR_CHECKIN},
	{"����_CN1%", sizeof("����_CN1%"), LUA_DATALINE0 + CHAR_CATCHCNT1},
	{"����_CN2%", sizeof("����_CN2%"), LUA_DATALINE0 + CHAR_CATCHCNT2},
	{"����_CN3%", sizeof("����_CN3%"), LUA_DATALINE0 + CHAR_CATCHCNT3},
	{"����_CN4%", sizeof("����_CN4%"), LUA_DATALINE0 + CHAR_CATCHCNT4},
	{"����_CN5%", sizeof("����_CN5%"), LUA_DATALINE0 + CHAR_CATCHCNT5},
	{"����_KN1%", sizeof("����_KN1%"), LUA_DATALINE0 + CHAR_KINDCNT1},
	{"����_KN2%", sizeof("����_KN2%"), LUA_DATALINE0 + CHAR_KINDCNT2},
	{"����_KN3%", sizeof("����_KN3%"), LUA_DATALINE0 + CHAR_KINDCNT3},
	{"����_KN4%", sizeof("����_KN4%"), LUA_DATALINE0 + CHAR_KINDCNT4},
	{"����_KN5%", sizeof("����_KN5%"), LUA_DATALINE0 + CHAR_KINDCNT5},
	{"����_KN6%", sizeof("����_KN6%"), LUA_DATALINE0 + CHAR_KINDCNT6},
	{"����_KN7%", sizeof("����_KN7%"), LUA_DATALINE0 + CHAR_KINDCNT7},
	{"����_KN8%", sizeof("����_KN8%"), LUA_DATALINE0 + CHAR_KINDCNT8},
	{"����_KN9%", sizeof("����_KN9%"), LUA_DATALINE0 + CHAR_KINDCNT9},
	{"����_KN10%", sizeof("����_KN10%"), LUA_DATALINE0 + CHAR_KINDCNT10},
#endif
  {"����_ף��%", sizeof("����_ף��%"), LUA_DATALINE0 + CHAR_BEATITUDE},
#ifdef	_VIGOR_SYS
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + CHAR_VIGOR},
#endif
  {"����_��������1%",   sizeof("����_��������1%"), LUA_DATALINE0 + CHAR_LUASAVE_WORKINT_1},  //lua�洢 �����������
  {"����_��������2%",   sizeof("����_��������2%"), LUA_DATALINE0 + CHAR_LUASAVE_WORKINT_2},  //lua�洢 �����������
  {"����_��������3%",   sizeof("����_��������3%"), LUA_DATALINE0 + CHAR_LUASAVE_WORKINT_3},  //lua�洢 �����������
  {"����_�����洢1%", sizeof("����_�����洢1%"), LUA_DATALINE0 + CHAR_LUASAVE_INT_1},      //�����洢���������浵��
  {"����_�����洢2%", sizeof("����_�����洢2%"), LUA_DATALINE0 + CHAR_LUASAVE_INT_2},      //�����洢���������浵��
  {"����_�����洢3%", sizeof("����_�����洢3%"), LUA_DATALINE0 + CHAR_LUASAVE_INT_3},      //�����洢���������浵��
 // {"����_IP%",sizeof("����_IP%"),LUA_DATALINE1 + CHAR_CHARPLAYIP},
 
  {"����_��Ա�ȼ�%", sizeof("����_��Ա�ȼ�%"), LUA_DATALINE0 + CHAR_VIPRIDE},
	{"����_��Աʱ��%", sizeof("����_��Աʱ��%"), LUA_DATALINE0 + CHAR_VIPTIME},
	
	{"����_NPCʱ��%", sizeof("����_NPCʱ��%"), LUA_DATALINE0 + CHAR_NPCTIME},

	{"����_ԭ��%", sizeof("����_ԭ��%"), LUA_DATALINE1 + CHAR_NAME},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE1 + CHAR_USERPETNAME},
	{"����_�ʺ�%", sizeof("����_�ʺ�%"), LUA_DATALINE1 + CHAR_CDKEY},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE1 + CHAR_OWNERCDKEY},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE1 + CHAR_OWNERCHARANAME},
	{"����_������%", sizeof("����_������%"), LUA_DATALINE1 + CHAR_FMNAME},
#ifdef _TEACHER_SYSTEM
	{"����_��ʦ��%", sizeof("����_��ʦ��%"), LUA_DATALINE1 + CHAR_TEACHER_ID},
	{"����_��ʦ��%", sizeof("����_��ʦ��%"), LUA_DATALINE1 + CHAR_TEACHER_NAME},
#endif
#ifdef _CAX_LUA_BETT01
	{"����_ս��1%", sizeof("����_ս��1%"), LUA_DATALINE2 + CHAR_WORKBATTLECOM1},
	{"����_ս��2%", sizeof("����_ս��2%"), LUA_DATALINE2 + CHAR_WORKBATTLECOM2},
	{"����_սģ%", sizeof("����_սģ%"), LUA_DATALINE2 + CHAR_WORKBATTLEMODE},
#endif
	{"����_��ģʽ%", sizeof("����_��ģʽ%"), LUA_DATALINE2 + CHAR_WORKPARTYMODE},
	{"����_������%", sizeof("����_������%"), LUA_DATALINE2 + CHAR_WORKOBJINDEX},
#ifdef _ADD_ACTION          //npc����
	{"����_����%", sizeof("����_����%"), LUA_DATALINE2 + CHAR_WORKACTION},
#endif
	{"����_LUA%", sizeof("����_LUA%"), LUA_DATALINE2 + CHAR_WORKLUANPCINT},
	{"����_1LUA%", sizeof("����_1LUA%"), LUA_DATALINE2 + CHAR_WORKLUANPCINT1},

	{"����_IAK%", sizeof("����_IAK%"), LUA_DATALINE4 + CHAR_ISATTACK},
	{"����_ս��%", sizeof("����_ս��%"), LUA_DATALINE4 + CHAR_ISATTACKED},
	{"����_IOR%", sizeof("����_IOR%"), LUA_DATALINE4 + CHAR_ISOVER},
	{"����_��·%", sizeof("����_��·%"), LUA_DATALINE4 + CHAR_ISOVERED},
	{"����_HHT%", sizeof("����_HHT%"), LUA_DATALINE4 + CHAR_HAVEHEIGHT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE4 + CHAR_ISVISIBLE},
	{"����_ITP%", sizeof("����_ITP%"), LUA_DATALINE4 + CHAR_ISTRANSPARENT},
	{"����_IFG%", sizeof("����_IFG%"), LUA_DATALINE4 + CHAR_ISFLYING},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE4 + CHAR_ISDIE},
	{"����_IBIG%", sizeof("����_IBIG%"), LUA_DATALINE4 + CHAR_ISBIG},
	{"����_��ս%", sizeof("����_��ս%"), LUA_DATALINE4 + CHAR_ISSHOWBATTLEMSG},
	{"����_��ӱ�%", sizeof("����_��ӱ�%"), LUA_DATALINE4 + CHAR_ISPARTY},
	{"����_�����%", sizeof("����_�����%"), LUA_DATALINE4 + CHAR_ISWARP},
	{"����_��ս��%", sizeof("����_��ս��%"), LUA_DATALINE4 + CHAR_ISDUEL},
	{"����_���ı�%", sizeof("����_���ı�%"), LUA_DATALINE4 + CHAR_ISPARTYCHAT},
	{"����_��Ƭ��%", sizeof("����_��Ƭ��%"), LUA_DATALINE4 + CHAR_ISTRADECARD},
	{"����_���ױ�%", sizeof("����_���ױ�%"), LUA_DATALINE4 + CHAR_ISTRADE},
#ifdef _CHANNEL_MODIFY
	{"����_��Ƶ��%", sizeof("����_��Ƶ��%"), LUA_DATALINE4 + CHAR_ISTELL},
	{"����_��Ƶ��%", sizeof("����_��Ƶ��%"), LUA_DATALINE4 + CHAR_ISFM},
	{"����_ְƵ��%", sizeof("����_ְƵ��%"), LUA_DATALINE4 + CHAR_ISOCC},
	{"����_�Ի���%", sizeof("����_�Ի���%"), LUA_DATALINE4 + CHAR_ISSAVE},
	{"����_��Ƶ��%", sizeof("����_��Ƶ��%"), LUA_DATALINE4 + CHAR_ISCHAT},
#endif

	{"����_ID%", sizeof("����_ID%"), LUA_DATALINE0 + ITEM_ID},
	{"����_ͼ��%", sizeof("����_ͼ��%"), LUA_DATALINE0 + ITEM_BASEIMAGENUMBER},
	{"����_�ɱ�%", sizeof("����_�ɱ�%"), LUA_DATALINE0 + ITEM_COST},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_TYPE},
	{"����_AUF%", sizeof("����_AUF%"), LUA_DATALINE0 + ITEM_ABLEUSEFIELD},
	{"����_TARGET%", sizeof("����_TARGET%"), LUA_DATALINE0 + ITEM_TARGET},
	{"����_�ȼ�%", sizeof("����_�ȼ�%"), LUA_DATALINE0 + ITEM_LEVEL},
#ifdef _ITEM_MAXUSERNUM
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_DAMAGEBREAK},
#endif
#ifdef _ITEMSET4_TXT
	{"����_�ѵ�%", sizeof("����_�ѵ�%"), LUA_DATALINE0 + ITEM_USEPILENUMS},
	{"����_�ɵ�%", sizeof("����_�ɵ�%"), LUA_DATALINE0 + ITEM_CANBEPILE},
	{"����_STR%", sizeof("����_STR%"), LUA_DATALINE0 + ITEM_NEEDSTR},
	{"����_DEX%", sizeof("����_DEX%"), LUA_DATALINE0 + ITEM_NEEDDEX},
	{"����_ת��%", sizeof("����_ת��%"), LUA_DATALINE0 + ITEM_NEEDTRANS},
	{"����_ְҵ%", sizeof("����_ְҵ%"), LUA_DATALINE0 + ITEM_NEEDPROFESSION},
#endif
#ifdef _TAKE_ITEMDAMAGE
	{"����_�;�%", sizeof("����_�;�%"), LUA_DATALINE0 + ITEM_DAMAGECRUSHE},
	{"����_MAX�;�%", sizeof("����_MAX�;�%"), LUA_DATALINE0 + ITEM_MAXDAMAGECRUSHE},
#endif
#ifdef _ADD_DEAMGEDEFC
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + ITEM_OTHERDAMAGE},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + ITEM_OTHERDEFC},
#endif
#ifdef _SUIT_ITEM
	{"����_��%", sizeof("����_��%"), LUA_DATALINE0 + ITEM_SUITCODE},
#endif
	{"����_AKMIN%", sizeof("����_AKMIN%"), LUA_DATALINE0 + ITEM_ATTACKNUM_MIN},
	{"����_AKMAX%", sizeof("����_AKMAX%"), LUA_DATALINE0 + ITEM_ATTACKNUM_MAX},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYATTACK},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYDEFENCE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYQUICK},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYHP},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYMP},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYLUCK},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYCHARM},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYAVOID},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYATTRIB},
	{"����_����ֵ%", sizeof("����_����ֵ%"), LUA_DATALINE0 + ITEM_MODIFYATTRIBVALUE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MAGICID},
	{"����_������%", sizeof("����_������%"), LUA_DATALINE0 + ITEM_MAGICPROB},
	{"����_�����%", sizeof("����_�����%"), LUA_DATALINE0 + ITEM_MAGICUSEMP},
#ifdef _ITEMSET5_TXT
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYARRANGE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MODIFYSEQUENCE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_ATTACHPILE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_HITRIGHT},
#endif
#ifdef _ITEMSET6_TXT
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_NEGLECTGUARD},
#endif
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_POISON},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_PARALYSIS},
	{"����_˯��%", sizeof("����_˯��%"), LUA_DATALINE0 + ITEM_SLEEP},
	{"����_ʯ��%", sizeof("����_ʯ��%"), LUA_DATALINE0 + ITEM_STONE},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_DRUNK},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_CONFUSION},
	{"����_��һ��%", sizeof("����_��һ��%"), LUA_DATALINE0 + ITEM_CRITICAL},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_USEACTION},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_DROPATLOGOUT},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_VANISHATDROP},
	{"����_��·%", sizeof("����_��·%"), LUA_DATALINE0 + ITEM_ISOVERED},
	{"����_�ʼ�%", sizeof("����_�ʼ�%"), LUA_DATALINE0 + ITEM_CANPETMAIL},
	{"����_�Ӻϳ�%", sizeof("����_�Ӻϳ�%"), LUA_DATALINE0 + ITEM_CANMERGEFROM},
	{"����_�ϳɵ�%", sizeof("����_�ϳɵ�%"), LUA_DATALINE0 + ITEM_CANMERGETO},
	{"����_����0%", sizeof("����_����0%"), LUA_DATALINE0 + ITEM_INGVALUE0},
	{"����_����1%", sizeof("����_����1%"), LUA_DATALINE0 + ITEM_INGVALUE1},
	{"����_����2%", sizeof("����_����2%"), LUA_DATALINE0 + ITEM_INGVALUE2},
	{"����_����3%", sizeof("����_����3%"), LUA_DATALINE0 + ITEM_INGVALUE3},
	{"����_����4%", sizeof("����_����4%"), LUA_DATALINE0 + ITEM_INGVALUE4},
	{"����_��ʱ%", sizeof("����_��ʱ%"), LUA_DATALINE0 + ITEM_PUTTIME},
	{"����_LL%", sizeof("����_LL%"), LUA_DATALINE0 + ITEM_LEAKLEVEL},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE0 + ITEM_MERGEFLG},
	{"����_���%", sizeof("����_���%"), LUA_DATALINE0 + ITEM_CRUSHLEVEL},
	{"����_��ʱ%", sizeof("����_��ʱ%"), LUA_DATALINE0 + ITEM_TIME},
	{"����_VAR1%", sizeof("����_VAR1%"), LUA_DATALINE0 + ITEM_VAR1},
	{"����_VAR2%", sizeof("����_VAR2%"), LUA_DATALINE0 + ITEM_VAR2},
	{"����_VAR3%", sizeof("����_VAR3%"), LUA_DATALINE0 + ITEM_VAR3},
	{"����_VAR4%", sizeof("����_VAR4%"), LUA_DATALINE0 + ITEM_VAR4},

	{"����_����%", sizeof("����_����%"), LUA_DATALINE2 + ITEM_WORKOBJINDEX},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE2 + ITEM_WORKCHARAINDEX},
#ifdef _ITEM_TIME_LIMIT
	{"����_��ʱ%", sizeof("����_��ʱ%"), LUA_DATALINE2 + ITEM_WORKTIMELIMIT},
#endif

	{"����_ԭ��%", sizeof("����_ԭ��%"), LUA_DATALINE1 + ITEM_NAME},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE1 + ITEM_SECRETNAME},
	{"����_˵��%", sizeof("����_˵��%"), LUA_DATALINE1 + ITEM_EFFECTSTRING},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE1 + ITEM_ARGUMENT},
	{"����_��%", sizeof("����_��%"), LUA_DATALINE1 + ITEM_CDKEY},
	{"����_����%", sizeof("����_����%"), LUA_DATALINE1 + ITEM_FORUSERCDKEY},
	{"����_��0%", sizeof("����_��0%"), LUA_DATALINE1 + ITEM_INGNAME0},
	{"����_��1%", sizeof("����_��1%"), LUA_DATALINE1 + ITEM_INGNAME1},
	{"����_��2%", sizeof("����_��2%"), LUA_DATALINE1 + ITEM_INGNAME2},
	{"����_��3%", sizeof("����_��3%"), LUA_DATALINE1 + ITEM_INGNAME3},
	{"����_��4%", sizeof("����_��4%"), LUA_DATALINE1 + ITEM_INGNAME4},

	{"����_ͷλ%", sizeof("����_ͷλ%"), CHAR_HEAD},
	{"����_��λ%", sizeof("����_��λ%"), CHAR_BODY},
	{"����_����%", sizeof("����_����%"), CHAR_ARM},
	{"����_��1%", sizeof("����_��1%"), CHAR_DECORATION1},
	{"����_��2%", sizeof("����_��2%"), CHAR_DECORATION2},
#ifdef _ITEM_EQUITSPACE
	{"����_����%", sizeof("����_����%"), CHAR_EQSHIELD},
	{"����_��λ%", sizeof("����_��λ%"), CHAR_EQBELT},
	{"����_��λ%", sizeof("����_��λ%"), CHAR_EQSHOES},
#endif
#ifdef _EQUIT_NEWGLOVE
	{"����_����%", sizeof("����_����%"), CHAR_EQGLOVE},
#endif
	{"����_1%", sizeof("����_1%"), CHAR_STARTITEMARRAY + 0},
	{"����_2%", sizeof("����_2%"), CHAR_STARTITEMARRAY + 1},
	{"����_3%", sizeof("����_3%"), CHAR_STARTITEMARRAY + 2},
	{"����_4%", sizeof("����_4%"), CHAR_STARTITEMARRAY + 3},
	{"����_5%", sizeof("����_5%"), CHAR_STARTITEMARRAY + 4},
	{"����_6%", sizeof("����_6%"), CHAR_STARTITEMARRAY + 5},
	{"����_7%", sizeof("����_7%"), CHAR_STARTITEMARRAY + 6},
	{"����_8%", sizeof("����_8%"), CHAR_STARTITEMARRAY + 7},
	{"����_9%", sizeof("����_9%"), CHAR_STARTITEMARRAY + 8},
	{"����_10%", sizeof("����_10%"), CHAR_STARTITEMARRAY + 9},
	{"����_11%", sizeof("����_11%"), CHAR_STARTITEMARRAY + 10},
	{"����_12%", sizeof("����_12%"), CHAR_STARTITEMARRAY + 11},
	{"����_13%", sizeof("����_13%"), CHAR_STARTITEMARRAY + 12},
	{"����_14%", sizeof("����_14%"), CHAR_STARTITEMARRAY + 13},
	{"����_15%", sizeof("����_15%"), CHAR_STARTITEMARRAY + 14},

	{"����_1%", sizeof("����_1%"), 0},
	{"����_2%", sizeof("����_2%"), 1},
	{"����_3%", sizeof("����_3%"), 2},
	{"����_4%", sizeof("����_4%"), 3},
	{"����_5%", sizeof("����_5%"), 4},
	
	{"��Ա_1%", sizeof("��Ա_1%"), 1},
	{"��Ա_2%", sizeof("��Ա_2%"), 2},
	{"��Ա_3%", sizeof("��Ա_3%"), 3},
	{"��Ա_4%", sizeof("��Ա_4%"), 4},

	{"IT_צ%", sizeof("IT_צ%"), ITEM_FIST},
	{"IT_��ͷ%", sizeof("IT_��ͷ%"), ITEM_AXE},
	{"IT_����%", sizeof("IT_����%"), ITEM_CLUB},
	{"IT_ǹ%", sizeof("IT_ǹ%"), ITEM_SPEAR},
	{"IT_��%", sizeof("IT_��%"), ITEM_BOW},
	{"IT_SHIELD%", sizeof("IT_SHIELD%"), ITEM_SHIELD},
	{"IT_HELM%", sizeof("IT_HELM%"), ITEM_HELM},
	{"IT_ARMOUR%", sizeof("IT_ARMOUR%"), ITEM_ARMOUR},
	{"IT_�ֻ�%", sizeof("IT_�ֻ�%"), ITEM_BRACELET},
	{"IT_����%", sizeof("IT_����%"), ITEM_MUSIC},
	{"IT_����%", sizeof("IT_����%"), ITEM_NECKLACE},
	{"IT_��%", sizeof("IT_��%"), ITEM_RING},
	{"IT_BELT%", sizeof("IT_BELT%"), ITEM_BELT},
	{"IT_����%", sizeof("IT_����%"), ITEM_EARRING},
	{"IT_�ǻ�%", sizeof("IT_�ǻ�%"), ITEM_NOSERING},
	{"IT_�����%", sizeof("IT_�����%"), ITEM_AMULET},
	{"IT_����%", sizeof("IT_����%"), ITEM_OTHER},
	{"IT_������%", sizeof("IT_������%"), ITEM_BOOMERANG},
	{"IT_Ͷ����%", sizeof("IT_Ͷ����%"), ITEM_BOUNDTHROW},
	{"IT_Ͷ��ʯ%", sizeof("IT_Ͷ��ʯ%"), ITEM_BREAKTHROW},
	{"IT_DISH%", sizeof("IT_DISH%"), ITEM_DISH},
#ifdef _ITEM_INSLAY
	{"IT_����%", sizeof("IT_����%"), ITEM_METAL},
	{"IT_����%", sizeof("IT_����%"), ITEM_JEWEL},
#endif
#ifdef _ITEM_CHECKWARES
	{"IT_����%", sizeof("IT_����%"), ITEM_WARES},
#endif
#ifdef _ITEM_EQUITSPACE
	{"IT_����%", sizeof("IT_����%"), ITEM_WBELT},
	{"IT_����%", sizeof("IT_����%"), ITEM_WSHIELD},
	{"IT_Ь��%", sizeof("IT_Ь��%"), ITEM_WSHOES},
#endif
#ifdef _EQUIT_NEWGLOVE 
	{"IT_����%", sizeof("IT_����%"), ITEM_WGLOVE},
#endif
#ifdef _JZ_PETEQU
	{"IT_��ͷ��%", sizeof("IT_��ͷ��%"), ITEM_PET_HEAD},
	{"IT_����%", sizeof("IT_����%"), ITEM_PET_WING},
	{"IT_������%", sizeof("IT_������%"), ITEM_PET_TOOTH},
	{"IT_���ؼ�%", sizeof("IT_���ؼ�%"), ITEM_PET_BREAST},
	{"IT_�豳��%", sizeof("IT_�豳��%"), ITEM_PET_BACK},
	{"IT_��צ��%", sizeof("IT_��צ��%"), ITEM_PET_CLAW},
	{"IT_�軤��2%", sizeof("IT_�軤��2%"), ITEM_PET_FEET},
	{"IT_�軤��4%", sizeof("IT_�軤��4%"), ITEM_PET_FEET4},
	{"IT_������%", sizeof("IT_������%"), ITEM_PET_FIN},
#endif

	{"����_û%", sizeof("����_û%"), FMMEMBER_NONE},
	{"����_һ��%", sizeof("����_һ��%"), FMMEMBER_MEMBER},
	{"����_����%", sizeof("����_����%"), FMMEMBER_APPLY},
	{"����_�峤%", sizeof("����_�峤%"), FMMEMBER_LEADER},
	{"����_����%", sizeof("����_����%"), FMMEMBER_ELDER},
	//{"����_��˾", sizeof("����_��˾"), FMMEMBER_INVITE},
	//{"����_����", sizeof("����_����"), FMMEMBER_BAILEE},
	//{"����_����", sizeof("����_����"), FMMEMBER_VICELEADER},

	{"ս��_��%", sizeof("ս��_��%"), BATTLE_TYPE_NONE},
	{"ս��_�˹�%", sizeof("ս��_�˹�%"), BATTLE_TYPE_P_vs_E},
	{"ս��_����%", sizeof("ս��_����%"), BATTLE_TYPE_P_vs_P},
	{"ս��_�ֹ�%", sizeof("ս��_�ֹ�%"), BATTLE_TYPE_E_vs_E},
	{"ս��_��%", sizeof("ս��_��%"), BATTLE_TYPE_WATCH},
	{"ս��_DP%", sizeof("ս��_DP%"), BATTLE_TYPE_DP_BATTLE},
	{"ս��_BOSS%", sizeof("ս��_BOSS%"), BATTLE_TYPE_BOSS_BATTLE},

	{"ս��_��%", sizeof("ս��_��%"), BATTLE_MODE_NONE},
	{"ս��_��%", sizeof("ս��_��%"), BATTLE_MODE_INIT},
	{"ս��_��%", sizeof("ս��_��%"), BATTLE_MODE_BATTLE},
	{"ս��_����%", sizeof("ս��_����%"), BATTLE_MODE_FINISH},
	{"ս��_������%", sizeof("ս��_������%"), BATTLE_MODE_STOP},
	{"ս��_�۳�%", sizeof("ս��_�۳�%"), BATTLE_MODE_WATCHBC},
	{"ս��_��1%", sizeof("ս��_��1%"), BATTLE_MODE_WATCHPRE},
	{"ս��_��2%", sizeof("ս��_��2%"), BATTLE_MODE_WATCHWAIT},
	{"ս��_��3%", sizeof("ս��_��3%"), BATTLE_MODE_WATCHMOVIE},
	{"ս��_��4%", sizeof("ս��_��4%"), BATTLE_MODE_WATCHAFTER},

	{"Ů��%", sizeof("Ů��%"), CHAR_IMAGETYPE_GIRL},
	{"�к�%", sizeof("�к�%"), CHAR_IMAGETYPE_BOY},
	{"��ͯ%", sizeof("��ͯ%"), CHAR_IMAGETYPE_CHILDBOY},
	{"Ůͯ%", sizeof("Ůͯ%"), CHAR_IMAGETYPE_CHILDGIRL},
	{"����%", sizeof("����%"), CHAR_IMAGETYPE_MAN},
	{"��Ů%", sizeof("��Ů%"), CHAR_IMAGETYPE_WOMAN},
	{"����%", sizeof("����%"), CHAR_IMAGETYPE_OLDMAN},
	{"��%", sizeof("��%"), CHAR_IMAGETYPE_DOG},

	{"����_��%", sizeof("����_��%"), CHAR_TYPEPLAYER},
	{"����_��%", sizeof("����_��%"), CHAR_TYPEENEMY},
	{"����_��%", sizeof("����_��%"), CHAR_TYPEPET},
	{"����_NPC1%", sizeof("����_NPC1%"), CHAR_TYPEDOOR},
	{"����_NPC2%", sizeof("����_NPC2%"), CHAR_TYPEBOX},
	{"����_NPC3%", sizeof("����_NPC3%"), CHAR_TYPEMSG},
	{"����_NPC4%", sizeof("����_NPC4%"), CHAR_TYPEWARP},
	{"����_NPC5%", sizeof("����_NPC5%"), CHAR_TYPESHOP},
	{"����_NPC6%", sizeof("����_NPC6%"), CHAR_TYPEHEALER},
	{"����_NPC7%", sizeof("����_NPC7%"), CHAR_TYPEOLDMAN},
	{"����_NPC8%", sizeof("����_NPC8%"), CHAR_TYPEROOMADMIN},
	{"����_NPC9%", sizeof("����_NPC9%"), CHAR_TYPETOWNPEOPLE},
	{"����_NPC10%", sizeof("����_NPC10%"), CHAR_TYPEDENGON},
	{"����_NPC11%", sizeof("����_NPC11%"), CHAR_TYPEADM},
	{"����_NPC12%", sizeof("����_NPC12%"), CHAR_TYPETEMPLE},
	{"����_NPC13%", sizeof("����_NPC13%"), CHAR_TYPESTORYTELLER},
	{"����_NPC14%", sizeof("����_NPC14%"), CHAR_TYPERANKING},
	{"����_NPC15%", sizeof("����_NPC15%"), CHAR_TYPEOTHERNPC},
	{"����_NPC16%", sizeof("����_NPC16%"), CHAR_TYPEPRINTPASSMAN},
	{"����_NPC17%", sizeof("����_NPC17%"), CHAR_TYPENPCENEMY},
	{"����_NPC18%", sizeof("����_NPC18%"), CHAR_TYPEACTION},
	{"����_NPC19%", sizeof("����_NPC19%"), CHAR_TYPEWINDOWMAN},
	{"����_NPC20%", sizeof("����_NPC20%"), CHAR_TYPESAVEPOINT},
	{"����_NPC21%", sizeof("����_NPC21%"), CHAR_TYPEWINDOWHEALER},
	{"����_NPC22%", sizeof("����_NPC22%"), CHAR_TYPEITEMSHOP},
	{"����_NPC23%", sizeof("����_NPC23%"), CHAR_TYPESTONESHOP},
	{"����_NPC24%", sizeof("����_NPC24%"), CHAR_TYPEDUELRANKING},
	{"����_NPC25%", sizeof("����_NPC25%"), CHAR_TYPEWARPMAN},
	{"����_NPC26%", sizeof("����_NPC26%"), CHAR_TYPEEVENT},
	{"����_NPC27%", sizeof("����_NPC27%"), CHAR_TYPEMIC},
	{"����_NPC28%", sizeof("����_NPC28%"), CHAR_TYPELUCKYMAN},
	{"����_NPC29%", sizeof("����_NPC29%"), CHAR_TYPEBUS},
	{"����_NPC30%", sizeof("����_NPC30%"), CHAR_TYPECHARM},
	{"����_NPC31%", sizeof("����_NPC31%"), CHAR_TYPECHECKMAN},
	{"����_NPC32%", sizeof("����_NPC32%"), CHAR_TYPEJANKEN},
	{"����_NPC33%", sizeof("����_NPC33%"), CHAR_TYPETRANSMIGRATION},
	{"����_NPC34%", sizeof("����_NPC34%"), CHAR_TYPEFMWARPMAN},
	{"����_NPC35%", sizeof("����_NPC35%"), CHAR_TYPEFMSCHEDULEMAN},
	{"����_NPC36%", sizeof("����_NPC36%"), CHAR_TYPEMANORSCHEDULEMAN},
#ifdef _SERVICE
	{"����_NPC37%", sizeof("����_NPC37%"), CHAR_TYPESTONESERVICEMAN},
#endif
#ifdef _GAMBLE_BANK
	{"����_NPC38%", sizeof("����_NPC38%"), CHAR_GAMBLEBANK},
#endif
#ifdef _NEW_WARPMAN
	{"����_NPC39%", sizeof("����_NPC39%"), CHAR_NEWNPCMAN},
#endif
#ifdef _MARKET_TRADE
	{"����_NPC40%", sizeof("����_NPC40%"), CHAR_MAPTRADEMAN},
#endif
#ifdef _GAMBLE_ROULETTE
	{"����_NPC41%", sizeof("����_NPC41%"), CHAR_GAMBLEROULETTE},
	{"����_NPC42%", sizeof("����_NPC42%"), CHAR_GAMBLEMASTER},
#endif
#ifdef _TRANSER_MAN
	{"����_NPC43%", sizeof("����_NPC43%"), CHAR_TRANSERMANS},
#endif
#ifdef _JZ_NEW_ADDMEMBERPOINT
	{"����_NPC44%", sizeof("����_NPC44%"), CHAR_MEMBERMANS},
#endif
#ifdef _NPC_SELLSTH
	{"����_NPC45%", sizeof("����_NPC45%"), CHAR_SELLSTHMAN},
#endif
#ifdef _NPC_MAKEPAIR
	{"����_NPC46%", sizeof("����_NPC46%"), CHAR_MAKEPAIR},
#endif
#ifdef _NPC_FUSION
	{"����_NPC47%", sizeof("����_NPC47%"), CHAR_PETFUSIONMAN},
#endif
#ifdef _PAUCTION_MAN
	{"����_NPC48%", sizeof("����_NPC48%"), CHAR_PAUCTIONMAN},
#endif
#ifdef _ITEM_NPCCHANGE
	{"����_NPC49%", sizeof("����_NPC49%"), CHAR_ITEMCHANGENPC},
#endif
#ifdef _CFREE_petskill
	{"����_NPC50%", sizeof("����_NPC50%"), CHAR_FREESKILLSHOP},
#endif
#ifdef _PETRACE
	{"����_NPC51%", sizeof("����_NPC51%"), CHAR_PETRACEMASTER},
	{"����_NPC52%", sizeof("����_NPC52%"), CHAR_PETRACEPET},
#endif
#ifdef _AUCTIONEER
	{"����_NPC53%", sizeof("����_NPC53%"), CHAR_TYPEAUCTIONEER},
#endif
#ifdef _ALLDOMAN // (���ɿ�) Syu ADD ���а�NPC
	{"����_NPC54%", sizeof("����_NPC54%"), CHAR_TYPEALLDOMAN},
#endif
	{"����_NPC55%", sizeof("����_NPC55%"), CHAR_TYPEPETMAKER},
#ifdef _NPC_WELFARE
	{"����_NPC56%", sizeof("����_NPC56%"), CHAR_TYPEWELFARE},
#endif
#ifdef _NPC_VERYWELFARE
	{"����_NPC57%", sizeof("����_NPC57%"), CHAR_TYPEVERYWELFARE},
#endif
#ifdef _NPC_WELFARE_2				// WON ADD ְҵNPC-2
	{"����_NPC58%", sizeof("����_NPC58%"), CHAR_TYPEWELFARE2},
#endif
#ifdef _RACEMAN
	{"����_NPC59%", sizeof("����_NPC59%"), CHAR_RACEMAN},
#endif

	{"���_û%", sizeof("���_û%"), OBJTYPE_NOUSE},
	{"���_��%", sizeof("���_��%"), OBJTYPE_CHARA},
	{"���_��%", sizeof("���_��%"), OBJTYPE_ITEM},
	{"���_��%", sizeof("���_��%"), OBJTYPE_GOLD},
#ifdef _NPCSERVER_NEW
	{"���_NPC%", sizeof("���_NPC%"), OBJTYPE_NPCSCHARA},
#endif
#ifdef _MAP_WARPPOINT
	{"���_��%", sizeof("���_��%"), OBJTYPE_WARPPOINT},
#endif

	{"��ť_û%", sizeof("��ť_û%"), WINDOW_BUTTONTYPE_NONE},
	{"��ť_ȷ��%", sizeof("��ť_ȷ��%"), WINDOW_BUTTONTYPE_OK},
	{"��ť_�ر�%", sizeof("��ť_�ر�%"), WINDOW_BUTTONTYPE_CANCEL},
	{"��ť_��%", sizeof("��ť_��%"), WINDOW_BUTTONTYPE_YES},
	{"��ť_��%", sizeof("��ť_��%"), WINDOW_BUTTONTYPE_NO},
	{"��ť_��һҳ%", sizeof("��ť_��һҳ%"), WINDOW_BUTTONTYPE_PREV},
	{"��ť_��һҳ%", sizeof("��ť_��һҳ%"), WINDOW_BUTTONTYPE_NEXT},

	{"���_û��%", sizeof("���_û��%"), CHAR_PARTY_NONE},
	{"���_�ӳ�%", sizeof("���_�ӳ�%"), CHAR_PARTY_LEADER},
	{"���_��Ա%", sizeof("���_��Ա%"), CHAR_PARTY_CLIENT},

	{"����%", sizeof("����%"), 0},
	{"��%", sizeof("��%"), 1},
	{"����%", sizeof("����%"), 2},
	{"��%", sizeof("��%"), 3},
	{"����%", sizeof("����%"), 4},
	{"��%", sizeof("��%"), 5},
	{"����%", sizeof("����%"), 6},
	{"��%", sizeof("��%"), 7},

	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORWHITE},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORCYAN},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORPURPLE},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORBLUE},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORYELLOW},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORGREEN},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORRED},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORGRAY},
	{"����ɫ%", sizeof("����ɫ%"), CHAR_COLORBLUE2},
	{"����ɫ%", sizeof("����ɫ%"), CHAR_COLORGREEN2},
	{"��ɫ%", sizeof("��ɫ%"), CHAR_COLORBLACK},

	{"�Ӹ���_OBJ%", sizeof("�Ӹ���_OBJ%"), CHAR_N_STRING_OBJINDEX},
	{"�Ӹ���_LV%", sizeof("�Ӹ���_LV%"), CHAR_N_STRING_LV},
	{"�Ӹ���_MAXHP%", sizeof("�Ӹ���_MAXHP%"), CHAR_N_STRING_MAXHP},
	{"�Ӹ���_HP%", sizeof("�Ӹ���_HP%"), CHAR_N_STRING_HP},
	{"�Ӹ���_MP%", sizeof("�Ӹ���_MP%"), CHAR_N_STRING_MP},
	{"�Ӹ���_����%", sizeof("�Ӹ���_����%"), CHAR_N_STRING_NAME},
	
	{"����3_HP%", sizeof("����3_HP%"), CHAR_P_STRING_HP},
	{"����3_MAXHP%", sizeof("����3_MAXHP%"), CHAR_P_STRING_MAXHP},
	{"����3_MP%", sizeof("����3_MP%"), CHAR_P_STRING_MP},
	{"����3_MAXMP%", sizeof("����3_MAXMP%"), CHAR_P_STRING_MAXMP},
	{"����3_��%", sizeof("����3_��%"), CHAR_P_STRING_VITAL},
	{"����3_��%", sizeof("����3_��%"), CHAR_P_STRING_STR},
	{"����3_��%", sizeof("����3_��%"), CHAR_P_STRING_TOUGH},
	{"����3_��%", sizeof("����3_��%"), CHAR_P_STRING_DEX},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_EXP},
	{"����3_��һ������%", sizeof("����3_��һ������%"), CHAR_P_STRING_NEXTEXP},
	{"����3_�ȼ�%", sizeof("����3_�ȼ�%"), CHAR_P_STRING_LV},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_ATK},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_DEF},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_QUICK},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_CHARM},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_LUCK},
	{"����3_������%", sizeof("����3_������%"), CHAR_P_STRING_EARTH},
	{"����3_ˮ����%", sizeof("����3_ˮ����%"), CHAR_P_STRING_WATER},
	{"����3_������%", sizeof("����3_������%"), CHAR_P_STRING_FIRE},
	{"����3_������%", sizeof("����3_������%"), CHAR_P_STRING_WIND},
	{"����3_ʯ��%", sizeof("����3_ʯ��%"), CHAR_P_STRING_GOLD},
	{"����3_�ƺ�%", sizeof("����3_�ƺ�%"), CHAR_P_STRING_TITLE},
	{"����3_DP%", sizeof("����3_DP%"), CHAR_P_STRING_DUELPOINT},
	{"����3_ת��%", sizeof("����3_ת��%"), CHAR_P_STRING_TRANSMIGRATION},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_NAME},
	{"����3_�ǳ�%", sizeof("����3_�ǳ�%"), CHAR_P_STRING_OWNTITLE},
	{"����3_���%", sizeof("����3_���%"), CHAR_P_STRING_RIDEPET},
	{"����3_ѧ��ȼ�%", sizeof("����3_ѧ��ȼ�%"), CHAR_P_STRING_LEARNRIDE},
	{"����3_����%", sizeof("����3_����%"), CHAR_P_STRING_BASEBASEIMAGENUMBER},
	{"����3_������ģʽ%", sizeof("����3_������ģʽ%"), CHAR_P_STRING_SKYWALKER},
	{"����3_����ģʽ%", sizeof("����3_����ģʽ%"), CHAR_P_STRING_DEBUGMODE},

	{NULL, 0, 0},
};

