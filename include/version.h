#include "sasql.h"
#ifndef __VERSION_H__
#define __VERSION_H__
#include "correct_bug.h"  //BUG FIX

/*Server �汾����*/
#define _NEW_SERVER_
#define _SERVER_NUMS

#define UNIQUE_KEYCODEITEM	100
#define UNIQUE_KEYCODEPET	"i"
//#define _DEFAULT_PKEY "Wonwonwon"	// (�ɿ���)( ʯ�� 6.0 )
//#define _RUNNING_KEY  "11111111"  // (�ɿ���)( ʯ�� 6.0 )
//#define _DEFAULT_PKEY "ttttttttt"   // (�ɿ���)( ʯ�� 6.0 )
//#define _RUNNING_KEY  "20041215"    // (�ɿ���)( ʯ�� 6.0 )

/* -------------------------------------------------------------------
 * ר��������ׯ԰�����޸�	��Ա��С褡���ѫ��־ΰ��С��
 */

/*
�޸�װ԰������ʽ��
Account Server ��ʽ:
	acfamily.h	MAX_FMPOINTҪ�޸�
Account Server ��Ϸ����:
	saac/family/fmpoint/db_fmpoint������������ׯ԰���������趨
	ׯ԰����ͼ��|ׯ԰���������|ׯ԰���������|ׯ԰�峤ͼ��|ׯ԰�ڽ�֮��ׯ|ռ��ׯ԰֮���� index|ռ��ׯ԰֮���� name|ռ��ׯ԰֮�����ۺ�����ֵ
	���磺
		100|607|563|1041|1|3|shan|3132238
		200|73|588|2031|2|||
Game Server ��ʽ��
	version.h FAMILY_MAXHOME��MANORNUMҪ����
	family.c	 FM_PKFLOOR	������ս��ͼ��
				 FAMILY_RidePet() ��������峤��ͼ��
				 FAMILY_LeaderFunc() ����������뺯id
	npcutil.c addNpcFamilyTax function�������(ׯ԰ͼ��%100)��Ϊׯ԰˰�ղ���
Game Server ��Ϸ����:
	gmsv_v2/data/npc/family/manorsman.arg* id������
	gmsv_v2/data/npc/family/scheduleman.arg* id������
*/

// ---------------------------BOXC�޸�----------------------------------------
// ---------------------------------------------------------------------------
#define CAX_STOP_FUN                 // ���ε�һЩ����,�رմ���Ϊ����
#ifndef CAX_STOP_FUN
#define CAX_RIDE_GENXIN              // ����·ɻ������������費��������
#define _NO_QUICK_WAR                // ��ֹ��������
#define _CAX_NPCTIME_OFF             // ɾ������ʱ���ж�NPC��䣨CAX�޸ģ���Ϊ��Ե�����NPC��䣬����֧��ĳЩF��
#endif

#define _ITEM_COLOER                // ��Ʒ��ɫ����
#define _SAME_IP_ONLINE_NUM		      // ͬIP��½��
#define _CHECK_SEVER_IP				      // �жϷ�����IP
#define _FIX_CORE_LOOP				      // �޸�CORE_LOOP
#define _FIX_CHAR_LOOP				      // ��������ѭ��
#define _FIX_SAVE_CHAR				      // �������浵����
#define _CAX_ESC_FENGBAO            // ESC���
//#define _CAX_ESC_REPORT             // ÿ��ǩ��
//#define _CAX_ESC_SAVE_DATA          // �������ݱ���
#define _FIX_COPY_MM_BUG            // �޸�����MM��BUG
//#define _TAKE_ITEMDAMAGE_FIX        // ����װ����
//#define _TAKE_ITEMDAMAGE_FOR_PET    // ��װװ����
#define _PETSKILL_SHOP_LUA          // ���＼���̵�LUA
#define _PLAYER_NPC                 // ������
#define _SERVER_DEF                 // �����ȫ����
#define _CARD_PRODUCTION            // �㿨����[gm card ���� ��ֵ]�������LNS��ֵ�ӿ�
#define _RECORD_IP						      // ��¼IP
#define _CAX_FAMEBUTTON						  // �����ص�-AB���棬������CF�����Ƿ񿪷�
#define _CAX_Mercenary					    // Ӷ���ֶ�
#define _PET_TRANS_ABILITY          // ����ת��������������
#define _PET_3TRANS				        	// ������ת
#define _NO_MAGIC                   // ��ֹʹ�þ����ͼ
#define _illegalpetskill_CF         // �Զ����ֹ�Ŵ����＼��
#define _FIX_SUPERPERSON_STRANS     // ��������Լ�Ʒ�˴ﵽ5ת��ǿ��ת��6
#define _CAX_LUA_BETT01			        // LUA���ӽӿ������
#define _LUA_Debug                  // LUA�������
#define _CAX_ADD_LUADX					    // LUAһЩ��չ
#define _CAX_PET_EVOLUTION				  // 2ת������ںϿ��ء�// �ںϳ�������ֵCF�޸� //
#define _TRANS_7_COLOR						  // �ͻ����Ƿ��PING��// CF��������Ƿ���Ի�����
#define _CAX_PET_ITEMRIDE				    // ������װ�����CF��������
#define _CAX_ZHUANGYUAN_QTJ				  // ��սʱ���ֹʹ�õ��߾���
#define _CAX_GM_RERIDE              // �ض�ride.exe�ļ�����
#define _CAX_GM_RESKILLCODE         // �ض�skillcode.exe�ļ�����
#define _CAX_GM_RELEADERRIDE        // �ض�leaderride.exe�ļ�����
#define _RIDEBUG                  // �����ս������BUG������Ϊ��ֹ�������������ս����ͬһֻ���
#define _DAMMAGE_CALC					      // �Զ����˺�
#define _FREE_SAVE    				      // ��������¼
#define _GM_SAVE_ALL_CHAR           // GM����浵
#define _SPECIAL_SUIT               // ������װ
#define _MANOR_EQUIP                // ׯ԰ר����װ
#define _NULL_CHECK_ITEM            // ��ͷ֧Ʊ����
#define _RE_GM_COMMAND              // �Զ���GM��������
#define _CAX_LNS_NLSUOXU            // LNS-NL������
#define _CAX_LNS_CHARSUOXU          // LNS-CHAR������
#define _CAX_LNS_NLGSUOXU           // LNS-NLG������
#define _CAX_LNS_MAPSUOXU           // LNS-MAP������
#define _CAX_LNS_MYSQLSUOXU         // LNS-MYSQL������
//#define _CAX_OFF_IPTABLES         // �ر���˿�Զ�����ǽ�������ã��򿪴������Ч��
#define _CAX_FAME_KOUFEI            // 10�����ÿ��������֤�����۷�
#define _CAX_ITEM_ADDEXP            // �Զ����ǻ۹��������ʱ��
//#define _CAX_FM_QISHINUM            // �ر�Ϊ1����Ϊ2  ####  1.���������Ƿ�Ϊ��С500���ƹ��� 2.Ϊׯ԰�������Ƶ���ɣ��ر�Ϊ1����Ϊ2��
//#define _CAX_METAMO_OFF             // �򿪿��ſͻ��˱�ɫ����
#define _CAX_NEWMAC_LOGIN             // ����71�ŷ��MAC��ַ����
#define _NO_STW_ENEMY                 // ��ֹ��ҿ�������
#define _FM_FMPOINTPK_LIST		        // ׯ԰��ս�б�
#define	_FM_NPC_LOOK_WAR		          // ���岼�������Կ�����ս�б�,�ۿ���սNPC��BOXCɾ������Ķ�ս�б�
#define _FM_FMPOINTPK_LIST		        // ׯ԰��ս�б�
#define _CAX_NPCTIME_IF		            // NPCʱ���ж����
//########################CAX��Ӷ�09����85�ͻ�����Ӧ���޸�##################
#define _CAX_TALK_LJIU               // ֧�����˵��
#define _CAX_DENGON_GG               // ֧�ֹ�������
//###################################added by mo#############################
#define _SHARE_EXP					     		     // ���鹲��
#define _TEAM_ADDEXP									   // ��Ӿ���ӳ�
#define _FIX_EARTHROUND 						     // �޸�����һ�ܳ�������BUG
#define _FIX_ContinuationAttack 			//�޸����ع���Ŀ�����(��3 ��5֮��)
#define _TEAM_WARP										   // �޸�ESCΪ�ŶӴ���
#define	_FM_NPC_LOOK_WAR1		          // ���岼�������Կ�����ս�б�(���Ӹ߰汾ׯ԰)
#define _DEX_FIX		 													//����ϵ��
#define _LOCK_PET_ITEM										// �󶨳��������
//#define _PET_TRANS_SETANS   //֧�ֳ���ת������(ɾ���˹���)
#define _MAX_BEATITUDE_BUFF //��ף���ӳ�
#define _PETTRANS_RANGE     //֧�ֳ���ת���������Χ����
#define _ITEM_LUA				 //����LUA֧��
#define _FM_EXP_ADD						//ׯ԰���徭��ӳ�
#define _FIX_STW_SPEED_ENEMY	// �޸���ҿ�������
#define _NO_QUICK_WAR //��ֹ��ҿ�������
#define _FIX_LSSP_S_BUG		//�޸�S����ķ������ֵ���ͣ�˵�BUG
#define _UNLAW_THIS_LOGOUT       			// ��ֹԭ��
#define _NO_JOINFLOOR    								 //��ֹ��ӵ�ͼ
#define _NO_HELP_MAP   									//��ֹhelp��ͼ
#define _REGISTRATION_TIME 	//09��ǩ���ֶ�
#define _FIX_TRADE_COPYPET  //�޸����׸��Ƴ���BUG
#define _MO_LOGINCHECK										//��ֹС�ӹҵȷǷ���ҵĵ�¼��֤
#ifdef _MO_LOGINCHECK										//XXTEA�㷨һЩ������
#define XXTEA_ROUND					8									//��������
#define XXTEA_KEY_1					1241503322										//key1
#define XXTEA_KEY_2					1241503322										//key2
#define XXTEA_KEY_3					1241503322										//key3
#define XXTEA_KEY_4					1241503322										//key4
#endif
#define _FIX_PLAYER_NPC_PET							//���Ӽ���NPC����ɾ���Ľӿ�
#define _UNTEXT_TALK                 // ���ιؼ���
#define _FIX_LONGCHARNAME_ATTACK				//�޸������������ֳ������������

#define _AVOIEDamageCharName                                  // ��ֹΣ�յ�����
#define _AVOIDATTACK_IN_CHAR_Talk_messageToLong               // ��ֹ�� CHAR_Talk ������ message ̫��
#define _AVOIDATTACK_IN_ADDRESSBOOK_sendMessage_textToLong    // ��ֹ�� ADDRESSBOOK_sendMessage ������ text ̫��
#define _AVOIDATTACK_IN_PETMAIL_sendPetMail_textToLong        // ��ֹ�� PETMAIL_sendPetMail ������ text ̫��
#define _AVOIDATTACK_IN_lssproto_WN_recv_dataToLong           // ��ֹ�� lssproto_WN_recv ������ data ̫��
#define _AVOIDATTACK_IN_ChatRoom_Create_messageToLong         // ��ֹ�� ChatRoom_Create ������ message ̫��
#define CC_ADD_ADDEXPCHECKDEGREE	                            // (�ɿ���)  �����������ٷֱ�  ����ʱ:��ͬ���ɵ���

                                                              
//#define CXC_BOXC_WUGUA            // �޹�F���ã�����ҵ�F��ر����
#ifdef CXC_BOXC_WUGUA
#define _CAX_GUANZHAN					      // �޹�Fʹ�õ������ս /gz
#define _CAX_LVTISHI					      // �ȼ�1�ĳ�����ʾΪ������ץȡ���Զ�ɾ����������
#endif

//##########����ʯ��ר�����������Եdata������ʯ����Ҫ�ر�#############################
//#define _PET_AMOI__E              // �ٷ������ļ���ʽ
//#define _PETITEM__AMOI_E          // �ٷ������ļ���ʽ
//#####################################################################################
//
//

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#define _FAMILY_MANORNUM_CHANGE	 // CoolFish �����޸�װ԰����(4.0)
#ifdef _FAMILY_MANORNUM_CHANGE
#ifdef CXC_BOXC_WUGUA
#define FAMILY_MAXHOME 2        // (4.0) ����ݵ�
#define MANORNUM       2        // (4.0) ׯ԰����
#define FMPOINTNUM     2        // (4.0) �оݵ������������
#else
#define FAMILY_MAXHOME 4        // (4.0) ����ݵ�
#define MANORNUM       4        // (4.0) ׯ԰����
#define FMPOINTNUM     4        // (4.0) �оݵ������������
#endif
#endif
#define _FMVER21                 // (�ɿ���) ����ְ�ƹ���
#define _PERSONAL_FAME           // (�ɿ���) Arminius 8.30 �����������
// -------------------------------------------------------------------
//ר����ʯ����Զ���		��Ա������������

#define _EVERYONE_RIDE           // (�ɿ���) Robin 0924 ���ƽ��
// -------------------------------------------------------------------
//ר��������������������	��Ա����־
#define _ATTACK_MAGIC            // (�ɿ���)	�ṩս���еĹ���������
#define _ADD_ACTION              // (�ɿ���) һ��NPC����״�����趨

// -------------------------------------------------------------------
//ר����������߱���		��Ա��С�
#define _UNIQUE_P_I              // (�ɿ���) CoolFish 2001/10/11

// -------------------------------------------------------------------
//ר�����°����ﴢ��pn �� ��������˷ 1018	��Ա����ѫ
#define _NEWSAVE                 // (�ɿ���) Robin
#define _STORECHAR               // (�ɿ���) Robin
#define _DROPCHECK               // (�ɿ���) Robin ��ֹ�ظ�������
#define _DROPCHECK2              // (�ɿ���) Robin ��ֹ�ظ�������
// -------------------------------------------------------------------
//ר�� : ������	��Ա : ���
#define _PET_TRANS               // (�ɿ���) ����ת������
#define _NEW_WARPMAN             // (�ɿ���) NPC����WARP����
#define _PET_TALK                // (�ɿ���) ����Ի�
#define _PET_LIMITLEVEL          // (�ɿ���) ��������(����)
#define _MAP_NOEXIT              // (�ɿ���) �����ͼ����ԭ��&ԭ�ǻ�ָ����
// -------------------------------------------------------------------
// ר����GMָ��㼶Reload		��Ա��С�
#define _GMRELOAD                // (�ɿ���) CoolFish: 2001/11/12
// -------------------------------------------------------------------
// ר���������½	��Ա�����������С�
#define _GAMBLE_ROULETTE         // (�ɿ���) ��� �ĳ�����	����С��
#define _FIX_GAMBLENUM           // (�ɿ���) �������� : ���߿ۻ���
#define _GAMBLE_BANK             // (�ɿ���) ���  �ĳ�����
#define _DROPSTAKENEW            // (�ɿ���) CoolFish:  �ĳ���ע
#define _PETRACE                 // (�ɿ���) CoolFish: 2001/12/3 ���ﾺ�ٳ�
#define _NEWEVENT                // (�ɿ���) CoolFish: 2001/12/5 �µ��������
// -------------------------------------------------------------------
//ר������������	��Ա������
#define _ITEM_FIRECRACKER        // (�ɿ���) ս����ʹ�ñ��ڿ��԰���������
// -------------------------------------------------------------------
//�¹��ܣ�ׯ԰PK����(��綨��)		��Ա��־ΰ��־��
#define _MANOR_PKRULE            // (�ɿ���)
// -------------------------------------------------------------------
// �¹��ܣ��¹���&���＼��		��Ա�����
#define _PSKILL_FALLGROUND       // (�ɿ���) ���＼��  (������)
#define _ENEMY_FALLGROUND        // (�ɿ���) NPC ����
#define _BATTLE_NEWPOWER         // (�ɿ���)
#define _Item_MoonAct            // (�ɿ���) ������ػ�
#define _Item_DeathAct           // (�ɿ���) ��ħ������
#define _Item_PetAct             // (�ɿ���) ѱ�޽�ָ
#define _Item_ReLifeAct          // (�ɿ���) ��Ҫitemset1.txt ���ܿ�) ��������
#define _BATTLESTEAL_FIX         // (�ɿ���) ����͵��
#define _CFREE_petskill          // (�ɿ���) ��Ҫpetskill1.txt ���ܿ�) ��ѧϰ���＼�ܼ���
#define _data_newreadfile        // (�ɿ���) �µ�server setup.cf ���� ���� itemset1.txt petskill1.txt �����µ�
#define _ITEM_MAXUSERNUM         // (�ɿ���) �� itemset1.txt ITEM ʹ�ô���
// -------------------------------------------------------------------
//�¹��ܣ��������    ��Ա�����
#define _PET_TALKPRO             // (�ɿ���) ����Ի���ǿ  & �� END
#define _PRO_NPCFIX              // (�ɿ���) NPC���ܼ�ǿ
#define _PRO_BATTLEENEMYSKILL    // (�ɿ���) ս����ǿ  ����NPC��AI����
#define _NEW_WARPPOINT           // (�ɿ���) ���͵���������ж�WARP
#define _FIX_CHECKITEM           // (�ɿ���) ���߲����Ƿ����
// -------------------------------------------------------------------
//�¹��ܣ������  ��Ա��С��,WON
#define _ADD_ENCOUNT             // (�ɿ���) WON ���ӵ����������޼�
#define _WON_PET_MAIL_LOG        // (�ɿ���) WON ��Ҽĳ��� LOG
// -------------------------------------------------------------------
// �¹��ܣ���������  4.0  ��Ա�����, ־ΰ , ���� , ־��
//#define _LOSE_FINCH_           // (���ɿ�) ANDY ��ȸ����
#define _FIX_FAMILY_PK_LIMIT     // (�ɿ���) WON ������սׯ԰����ǰʮ���Ϊǰ��ʮ��
#define _ADD_POOL_ITEM           // (�ɿ���) WON ���ӿɼķŵĵ���
#define _ITEM_METAMO             // (�ɿ���) shan metamo sprite. code:robin->shan
#define _ENEMY_ATTACK_AI         // (�ɿ���) shan enemy attack mode. code:shan
#define _ITEM_TIME_LIMIT         // (�ɿ���) shan time limit of item. code:shan
#define _MIND_ICON               // (�ɿ���) shan show icon from char mind. code:shan
#define _BATTLENPC_WARP_PLAYER   // (�ɿ���) shan npc warp player when n round. code:shan
#define _ITEM_WARP_FIX_BI        // (�ɿ���) shan fix bbi to bi. code:shan
#define _SKILL_DAMAGETOHP        // (�ɿ���) ANDY ��Ѫ��
#define _Skill_MPDAMAGE          // (�ɿ���) ANDY MP�˺�
#define _WAEI_KICK               // (�ɿ���) ANDY ����������
#define _BATTLE_TIMESPEED        // (�ɿ���) ANDY ս��ʱ�� ����(�������ؿ��������ٹ���)
#define _NEW_RIDEPETS            // (�ɿ���) ANDY �����
#define _FIXBUG_ATTACKBOW        // (�ɿ���) ANDY �������ʹ��Ͷ������bug
#define _NPCCHANGE_PLAYERIMG     // (�ɿ���) ANDY NPC�ı����ͼ��, ǿ������,�����ͼ�� (����)
#define _ITEM_ATTSKILLMAGIC      // (�ɿ���) ANDY ���߼���
#define _EQUIT_DEFMAGIC          // (�ɿ���) ANDY ��ħװ��
#define _FIX_MAGICDAMAGE         // (�ɿ���) ANDY ����ħ����ʽ ���ڿ���
#define _ITEM_CONSTITUTION       // (�ɿ���) ANDY ������������
#define _MAGIC_REHPAI            // (�ɿ���) ANDY ��ѪAI
#define _TRANSER_MAN             // (�ɿ���) ANDY ����ʦ
#define _TAKE_ITEMDAMAGE         // (�ɿ���) ANDY ������  ITEMSET2
#define _FIX_FIRECRACKER         // (�ɿ���) ANDY ���� ����
#define _FIX_ITEMPROB            // (�ɿ���) ANDY ���� ������
#define _ADD_DEAMGEDEFC          // (�ɿ���) ANDY �����˺� & ���ӹ���  ITEMSET2
#define _FIX_MAXCHARMP           // (�ɿ���) ANDY MP����
#define _ITEM_INSLAY             // (�ɿ���) ANDY �ⱦʯ ����
#define _ITEMSET2_ITEM           // (�ɿ���) ANDY ITEMSET2
#define _ITEM_NPCCHANGE          // (�ɿ���) ANDY ���� (����, ����)
#define _PETSKILL_FIXITEM        // (�ɿ���) ANDY �޸� && ͬ������
#define _FIX_SETWORKINT          // (�ɿ���) ANDY (FIXWORKINT)
#define _ITEM_ADDEXP             // (�ɿ���) vincent  ����:ָ��ʱ�������Ӿ���ֵ��������
#define _ITEM_ADDEXP2            // �ǹ�ʱ������ۼ� Robin
#define _FIX_METAMORIDE          // (�ɿ���) ANDY
#define _CHECK_ITEMDAMAGE        // (�ɿ���) ����������  7/26
#define _SKILL_WILDVIOLENT_ATT   // (�ɿ���) vincent  �輼:�񱩹���
#define _MAGIC_WEAKEN            // (�ɿ���) vincent  ����:����
#define _SKILL_WEAKEN            // (�ɿ���) vincent  �輼:����//��Ҫ��#define _MAGIC_WEAKEN
#define _SKILL_SPEEDY_ATT        // (�ɿ���) vincent  �輼:���ٹ���
#define _SKILL_GUARDBREAK2       // (�ɿ���) vincent  �輼:�Ƴ�����2
#define _SKILL_SACRIFICE         // (�ɿ���) vincent  �輼:��Ԯ
#define _PSKILL_MODIFY           // (�ɿ���) ANDY ����ǿ������
#define _PSKILL_MDFYATTACK       // (�ɿ���) ANDY ����ת������
#define _MAGIC_DEFMAGICATT       // (�ɿ���) ANDY ħ������
#define _MAGIC_SUPERWALL         // (�ɿ���) ANDY ���ڷ���
#define _OTHER_MAGICSTAUTS       // (�ɿ���) ANDY
#define _SKILL_TOOTH             // (�ɿ���) ANDY   ����
#define _MAGIC_DEEPPOISON        // (�ɿ���) vincent  ����:�綾
#define _MAGIC_BARRIER           // (�ɿ���) vincent  ����:ħ��
#define _MAGIC_NOCAST            // (�ɿ���) vincent  ����:��Ĭ
#define _ITEM_CRACKER            // (�ɿ���) vincent  ����:���� //��Ҫ��#define _MIND_ICON
#define _SKILL_DEEPPOISON        // (�ɿ���) vincent  �輼:�綾 //��Ҫ��#define _MAGIC_DEEPPOISON
#define _SKILL_BARRIER           // (�ɿ���) vincent  �輼:ħ�� //��Ҫ��#define _MAGIC_BARRIER
#define _SKILL_NOCAST            // (�ɿ���) vincent  �輼:��Ĭ //��Ҫ��#define _MAGIC_NOCAST
#define _SKILL_ROAR              // (�ɿ���) vincent  �輼:���(������)
#define _SKILL_REFRESH           // (�ɿ���) vincent  �輼:����쳣״̬
#define _ITEM_REFRESH            // (�ɿ���) vincent  ����쳣״̬����
#define _MAGIC_TOCALL            // (�ɿ���) kjl     �ٻ�  02/06/20 kjl

// -------------------------------------------------------------------
// �¹��ܣ�����������  ��Ա�����
#define _ITEMSET3_ITEM           // (�ɿ���) ANDY itemset3.txt
#define _SUIT_ITEM               // (�ɿ���) ANDY (��װ) ������ ����itemset3.txt
#define _PETSKILL_SETDUCK        // (�ɿ���) ANDY ���漼��
#define _VARY_WOLF               // (�ɿ���) pet skill : vary wolf. code:shan
// -------------------------------------------------------------------
// �¹��ܣ�ר��5.0    ��Ա��
#define _USEWARP_FORNUM          // (�ɿ���) ANDY ������ë ������
#define _IMPRECATE_ITEM          // (�ɿ���) ANDY ������� ������
#define _MAGICPET_SKILL          // (�ɿ���) ANDY ħ���輼��
#define _ITEM_CHECKWARES         // (�ɿ���) ANDY �������Ͳ���
#define _NPC_REPLACEMENT         // (�ɿ���) ANDY �û�ս��npc
#define _EMENY_CHANCEMAN         // (�ɿ���) ANDY EMENY ѡ��
#define _TYPE_TOXICATION         // (�ɿ���) ANDY �ж��޷���Ѫ
#define _PET_FUSION              // (�ɿ���) ANDY �����ں� enemybase1.txt
#define _NPC_FUSION              // (�ɿ���) ANDY NPC�����ں�
#define _ITEM_EDITBASES          // (�ɿ���) ANDY Ӫ����
#define _PET_EVOLUTION           // (�ɿ���) ANDY ������

// -------------------------------------------------------------------
// �¹��ܣ�5.0׷�Ӳ���    ��Ա��
#define _AVID_TRADETRYBUG        // (�ɿ���) ANDY ��ֹ�����ж���/���� ��(����)�ʼ� ʯ��
#define _ASSESS_SYSEFFICACY      // (�ɿ���) ANDY ����ϵͳЧ�� 12/04 ����
#define _ASSESS_SYSEFFICACY_SUB  // (�ɿ���) Robin ����ϵͳЧ�� ���Ӽ���ڻ�Ȧ
#define _PREVENT_TEAMATTACK      // (�ɿ���) ANDY ��ֹͬ�ӻ��� 12/04 ����
#define _ITEM_ORNAMENTS          // (�ɿ���) ANDY װ�ε���  12/04 ����  //���沿
#define _CHIKULA_STONE           // (�ɿ���) ANDY �����֮ʯ 12/04 ����  //������ ���
#define _SEND_EFFECT             // (�ɿ���) WON  AC����ѩ���������Ч 12/04 ����
#define _PETMAIL_DEFNUMS         // (�ɿ���) ANDY �����ʼ�����
#define _TEAM_KICKPARTY          // (�ɿ���) ANDY �ӳ�����(��ս��״̬)
#define _PETSKILL_TIMID          // (�ɿ���) ANDY �輼-��ս
#define _PETS_SELECTCON          // (�ɿ���) ANDY ���ﲻ�ɵ�ѡ��ս
#define _CHRISTMAS_REDSOCKS      // (�ɿ���) ANDY ʥ������
#define _FIX_ARRAYBUG            // (�ɿ���) ANDY ����array ��λ
#define _USER_CHARLOOPS          // (�ɿ���) ANDY �������LOOP
#define _BATTLE_PROPERTY         // (�ɿ���) ANDY ս����������
#define _PETSKILL_PROPERTY       // (�ɿ���) ANDY ����ħ�޳輼
#define _ITEM_FIXALLBASE         // (�ɿ���) ANDY �޸�֮ʯ
#define _ITEM_LOVERPARTY         // (�ɿ���) ANDY ���˽ڵ���
#define _ITEM_FORUSERNAMES       // (�ɿ���) ANDY ����������
#define _BATTLECOMMAND_TIME      // (�ɿ���) ANDY ��ֹ��ҿ�����(�غ�ս��ʱ��)
#define _NPCENEMY_ADDPOWER       // (�ɿ���) ANDY �޸Ĺ��������ֵ


// -------------------------------------------------------------------
// �¹��ܣ�ר��6.0    ��Ա��
//
#define _PETSKILL_CANNEDFOOD     // (�ɿ���) ANDY ���＼�ܹ�ͷ  for 6.0
#define _TEST_DROPITEMS          // (�ɿ���) ANDY ���Զ���
#define _MAP_WARPPOINT           // (�ɿ���) ANDY Map WarpPoint
#define _TREASURE_BOX            // (�ɿ���) ANDY ����
#define _BENEMY_EVENTRUN         // (�ɿ���) ANDY ս��npc������
#define _BATTLE_ABDUCTII         // (�ɿ���) ANDY ��;ⷰ�2
#define _BATTLE_LIGHTTAKE        // (�ɿ���) ANDY �ɹ���
#define _BATTLE_ATTCRAZED        // (�ɿ���) ANDY �����
#define _STATUS_WATERWORD        // (�ɿ���) ANDY ˮ����״̬
#define _ITEM_WATERWORDSTATUS    // (�ɿ���) ANDY ˮ����״̬����
#define _CAPTURE_FREES           // (�ɿ���) ANDY ��ץ����
#define _THROWITEM_ITEMS         // (�ɿ���) �������н�Ʒ
#define _OBJSEND_C               // (�ɿ���) ANDY _OBJSEND_C
#define _ADD_SHOW_ITEMDAMAGE     // (�ɿ���) WON  ��ʾ��Ʒ�;ö�

//--------------------------------------------------------------------------
//ר�� 7.0  ְҵϵͳ  ��Ա����� ־�� ��� ����

#define _CHANGETRADERULE         // (�ɿ���) Syu ADD ���׹����޶�
#define _TRADE_SHOWHP            // (�ɿ���) Syu ADD ������ʾѪ��
#define _SHOOTCHESTNUT           // (�ɿ���) Syu ADD �輼��������
#define _SKILLLIMIT              // (�ɿ���) Syu ADD ���ù����ҷ�����
#define _TRADESYSTEM2            // (�ɿ���) Syu ADD �½���ϵͳ
#define _NPC_NOSAVEPOINT         // (�ɿ���) ANDY ��¼�㲻����������
#define _ACFMPK_LIST             // (�ɿ���) ANDY �����ս�б�����
#define _PETSKILL2_TXT           // (�ɿ���) ANDY petskill2.txt
#define _PETSKILL_CHECKTYPE      // (�ɿ���) ANDY ���＼��ʹ��ʱ��
#define _PETSKILL_TEAR           // (�ɿ���) ANDY �輼 ˺���˿�
#define _NPC_MAKEPAIR            // (�ɿ���) ANDY NPC ���
#define _ITEMSET4_TXT            // (�ɿ���) ANDY itemset4.txt
#define _ITEM_PILENUMS           // (�ɿ���) ANDY �����زĶѵ� �� itemset4
#define _ITEM_PILEFORTRADE       // (�ɿ���) ANDY ���׶ѵ�
#define _ITEM_EQUITSPACE         // (�ɿ���) ANDY ��װ����λ
#define _PET_LOSTPET             // (�ɿ���) ANDY ��ʧ������Ѱ�ƻ�����
#define _ITEMSET5_TXT            // (�ɿ���) ANDY itemset5.txt
#define _ITEMSET6_TXT            // (�ɿ���) ANDY itemset6.txt
#define _EQUIT_ARRANGE           // (�ɿ���) ANDY ���� ��Ҫ itemset5.txt
#define _EQUIT_SEQUENCE          // (�ɿ���) ANDY ����˳�� ��Ҫ itemset5.txt
#define _EQUIT_ADDPILE           // (�ɿ���) ANDY ���ӿɶѵ��� ��Ҫ itemset5.txt
#define _EQUIT_HITRIGHT          // (�ɿ���) ANDY ���� ��Ҫ itemset5.txt
#define _EQUIT_NEGLECTGUARD      // (�ɿ���) ANDY ����Ŀ�������% ��Ҫ itemset6.txt
#define _EQUIT_NEWGLOVE          // (�ɿ���) ANDY ������λ
#define _FIX_MAX_GOLD            // (�ɿ���) WON ADD ���������Ǯ����
#define _PET_SKILL_SARS          // (�ɿ���) WON ADD ��ɷ����
#define _SONIC_ATTACK            // (�ɿ���) WON ADD ��������
#define _NET_REDUCESEND          // (�ɿ���) ANDY ����DB���ϴ���
#define _FEV_ADD_NEW_ITEM        // (�ɿ���) Change ���Ӹ����ؾ�
#define _ALLDOMAN                // (�ɿ���) Syu ADD ���а�NPC
#define _LOCKHELP_OK             // (�ɿ���) Syu ADD �������ɼ���ս��
#define _TELLCHANNEL             // (�ɿ���) Syu ADD ����Ƶ��
// �¹��ܣ�ְҵ
#define _NEWREQUESTPROTOCOL      // (�ɿ���) Syu ADD ����ProtocolҪ��ϸ��
#define _OUTOFBATTLESKILL        // (�ɿ���) Syu ADD ��ս��ʱ����Protocol
#define _CHAR_PROFESSION         // (�ɿ���) WON ADD ����ְҵ��λ
#define _PROFESSION_SKILL        // (�ɿ���) WON ADD ����ְҵ����
#define _NPC_WELFARE             // (�ɿ���) WON ADD ְҵNPC
#define _NPC_ADDLEVELUP          // (�ɿ���) ANDY NPC������ҵȼ�
#define _CHAR_FIXDATADEF         // (�ɿ���) ANDY �����������ϳ�ʼ
//ϵͳ����
#define _SIMPLIFY_ITEMSTRING     // (�ɿ���) ANDY �򻯵����ִ�
#define _CHAR_NEWLOGOUT          // (�ɿ���) ANDY �ǳ��»���
#define _CHATROOMPROTOCOL        // (�ɿ���) Syu ADD ������Ƶ�� 8/27

#define _CHAR_POOLITEM           // (�ɿ���) ANDY ������ֿ߲⹲��
#define _NPC_DEPOTITEM           // (�ɿ���) ANDY ������ֿ߲⹲��

#define _SIMPLIFY_PETSTRING      // (�ɿ���) ANDY �򻯳���浵�ִ�
#define _SIMPLIFY_ITEMSTRING2    // (�ɿ���) ANDY �򻯵����ִ�2
#define _WOLF_TAKE_AXE           // (�ɿ���) WON ADD ץ˫ͷ�ǵ�����
#define _FIX_UNNECESSARY         // (�ɿ���) ANDY ��������Ҫ���ж�
#define _ITEM_MAGICRECOVERY      // (�ɿ���) ANDY �¹⾵��
//9/17����
#define _PETSKILL_GYRATE         // (�ɿ���) Change �輼:��������
#define _PETSKILL_ACUPUNCTURE    // (�ɿ���) Change �輼:�����Ƥ (client�˵�_PETSKILL_ACUPUNCTUREҲҪ��)
#define _PETSKILL_RETRACE        // (�ɿ���) Change �輼:׷������
#define _PETSKILL_HECTOR         // (�ɿ���) Change �輼:����
//10/13����
#define _PETSKILL_FIREKILL       // (�ɿ���) Change �輼:������ɱ
#define _PETSKILL_DAMAGETOHP     // (�ɿ���) Change �輼:���¿���(��Ѫ���ı���)
#define _PETSKILL_BECOMEFOX      // (�ɿ���) Change �輼:�Ļ���

//11/12����
#define _PETSKILL_SHOWMERCY      // (�ɿ�) Change �輼:��������
#define _NPC_ActionFreeCmp       // (�ɿ�) Change ��NPC���������µıȽϷ�ʽ
//--------------------------------------------------------------------------


//11/26
#define _NPC_ProfessionTrans     // (�ɿ�) Change ��ְҵNPC�����ж�ת��
#define _NPC_NewDelPet           // (�ɿ�) Change ��NPC����ɾ������(ԭ��������,ֻ����warpmanʹ��ʱ���ɹ�,��������һ����һ��)
#define _ALLDOMAN_DEBUG          // (�ɿ�) Change ��������server�����а���ʧbug
//20031217
#define _CHRISTMAS_REDSOCKS_NEW  // (�ɿ�) Change ��ʥ������
#define _PETSKILL_COMBINED       // (�ɿ�) Change �輼:�ѵú�Ϳ [�ۺ�ħ��(����������ȡ��ħ��)]
//���˽ڻ
#define _NPC_ADDWARPMAN1         // (�ɿ���) Change npcgen_warpman���ӹ���(�趨����������趨�����Ů����)
#define _ITEM_QUITPARTY          // (�ɿ���) Change �����ɢʱ�ض����߼�����ʧ
#define _ITEM_ADDEQUIPEXP        // (�ɿ�) Change װ���ᾭ��ֵ����
//2004/2/18
#define _ITEM_LVUPUP             // (�ɿ�) Change ͻ�Ƴ���ȼ����Ƶ�ҩ(����ר��)
//2004/02/25
#define _PETSKILL_BECOMEPIG      // (�ɿ�) Change �輼:������
#define _ITEM_UNBECOMEPIG        // (�ɿ�) Change �������Ľ�ҩ
#define _PET_TALKBBI             // (�ɿ�) Change �����ж�bbi
#define _ITEM_USEMAGIC           // (�ɿ�) Change �ɵ���ʹ��ħ��
//2004/04/07
#define _ITEM_PROPERTY           // (�ɿ�) Change �ı������Ե���
#define _SUIT_ADDENDUM           // (�ɿ�) Change ����װ���ӹ��� (_SUIT_ITEMҪ��)
//2004/05/05
#define _ITEM_CHECKDROPATLOGOUT  // (�ɿ�) Change ���ؼ�¼��ʱ�������޵ǳ�����ʧ����,����,���ɻؼ�¼��
#define _FM_MODIFY               // ���幦���޸�(����)
#define _CHANNEL_MODIFY          // Ƶ����������(����)
#define _STANDBYPET              // Robin ��������
#define  _PROSKILL_OPTIMUM       // Robin ְҵ���ܴ�����ѻ�
#define  _PETSKILL_OPTIMUM       // Robin ���＼�ܴ�����ѻ�
#define _MAGIC_OPTIMUM           // Robin Magic table ��ѻ�
#define _AC_PIORITY              // Nuke ���AC������Ȩ(�ɿ�)
//2004/05/12
#define _CHAR_POOLPET            // (�ɿ���) Robin �������ֿ⹲��
#define _NPC_DEPOTPET            // (�ɿ���) Robin �������ֿ⹲��
#define _MAGIC_RESIST_EQUIT      // (�ɿ�) WON ADD ְҵ����װ��

// -------------------------------------------------------------------
//��������
#define _NEWOPEN_MAXEXP          // (�ɿ�) ANDY MAX����ֵ
// -------------------------------------------------------------------
// �¹��ܣ�GM��Ƶ��������
//���̰�
#define _REDHAT_V9               //(�ɿ�) Change ��REDHAT9.0�İ汾�±���ʱ��
//--------------------------------------------------------------------------
//ר�� 7.5  ������л�  ��Ա����ѫ ���� С��
#define _NPC_EXCHANGEMANTRANS    // (�ɿ�) Change ��exchangeman�����ж�ת��
#define  _ANGEL_SUMMON           // Robin ��ʹ�ٻ�
#ifdef _ANGEL_SUMMON
  #define _ADD_NEWEVENT
#endif
// Terry define start -------------------------------------------------------------------
#define _STREET_VENDOR           // ��̯����(����)
#define _WATCH_EVENT             // ��ѯ������(����)
#define _NEW_MANOR_LAW           // ��ׯ԰����(����)
#define _MAP_TIME                // �����ͼ,�ᵹ��(����)
#define _PETSKILL_LER            // �׶�����(����)

// Terry define end   -------------------------------------------------------------------
#define _FIX_MAGIC_RESIST        // (�ɿ�) Change ְҵħ������
#define _ADD_DUNGEON             // (�ɿ�) Change ׷�ӵ���
#define _PROFESSION_ADDSKILL     // (�ɿ�) Change ׷��ְҵ����
#define _ITEM_ADDPETEXP          // (�ɿ�) Change ���ӳ��ﾭ��ֵ����(�ɳԴ˵��߳���,ƽ�����ɻ�þ���ֵ)
#define _TRANS_6                 // (�ɿ�) Change ����6ת
#define _PET_2TRANS              // (�ɿ�) Change ����2ת
#define _PET_2LIMITLEVEL         // (�ɿ�) Change ��������2ת������
#define _MAGICSTAUTS_RESIST      // (�ɿ�) Change (����)���Ծ���
#define _EQUIT_RESIST            // (�ɿ�) Change ��һװ������(�ɿ���һ�����쳣״̬)�п��� �� �� ���� ħ�� ��Ĭ ����
#define _SUIT_TWFWENDUM          // (�ɿ�) Change ��������װ���� (_SUIT_ITEMҪ��)
#define _EXCHANGEMAN_REQUEST_DELPET   // (�ɿ�) Change ��exchangeman��TYPE:REQUEST����ɾ������
//--------------------------------------------------------------------------
#define _DEL_DROP_GOLD           // ɾ�����Ϲ�ʱ��ʯ��  Robin
#define _ITEM_STONE              // cyg ��ֵ�ʯͷ
#define _HELP_NEWHAND            // cyg �´���ɫ�������ֵ���)
#define _DEF_GETYOU              // cyg GMָ�� ��ѯ�Լ�����  Χ�ڵ�����ʺ�
#define _DEF_NEWSEND             // cyg GMָ�� �������ʱ�ɼӾ���Ҳ�ɲ���
#define _DEF_SUPERSEND           // cyg GMָ�� �ɴ�����ĳ�ʺ�Ϊ���ĸ���3�����������
#define _SUIT_ADDPART3           // (�ɿ�) Change ��װ���ܵ����� �ر����% ��Ĭ�������% �����% �����% �����%
#define _PETSKILL_2TIMID         // (�ɿ�) Change ��ս�ڶ���(��ʨŭ��)
#define _SHOW_FUSION             // (�ɿ�) Change �ںϳ�����CLIENT��ֻ��ʾת����,�޸�Ϊ�ںϳ���ʾΪ�ں�(client��ҲҪ��)
#define _FONT_SIZE               // (�ɿ�) Robin ���ʹ�С����
#define _PROSK99                 // (�ɿ�) Change ���ְ���ȼ��Ͱ���
#define _TIME_TICKET             // Robin ��ʱ�볡 ɭ��׽����
#define _HALLOWEEN_EFFECT        // ��ʥ����Ч
#define _ADD_STATUS_2            // ������������״̬�ڶ���
#define _ADD_reITEM              // (�ɿ�) Change ��������λ�ո�
#define _ADD_NOITEM_BATTLE       // ����NPC�����������û���߻����ս��
#define _PETSKILL_BATTLE_MODEL   // ���＼��ս��ģ��
#define _PETSKILL_ANTINTER       // (�ɿ�) Change �輼:��֮�� �ο�_PETSKILL_2TIMID
#define _PETSKILL_REGRET         // (�ɿ�) Change �輼:����һ�� �ο�_SONIC_ATTACK
#define _SUIT_ADDPART4           // (�ɿ�) Change ��װ���ܵ��ĵ� �ж�����% ��ǿ��ʦħ��(����30%) �ֿ���ʦ��ħ��

//��ͨ����������-------------------------------------
#define _PUB_ALL
#ifdef _PUB_ALL
//#define _PROSKILL_ERR_KICK     // �Ƿ������Զ�T����
#define _ITEM_SETLOVER           // ��鹦��
#define _CREATE_MM_1_2           // ������Ʒ���ٷ���
#define _SendTo                  // ���͵�������
#define _GET_BATTLE_EXP          // ��������
#define _NEW_PLAYER_CF           // ����״̬����
#define _USER_EXP_CF             // �û��Զ��徭��
#define _TRANS_LEVEL_CF          // ת����Խ160������
#define _UNLAW_WARP_FLOOR        // ��ֹ���͵���
#define _UNREG_NEMA              // ��ֹ��������
#define _GM_ITEM                 // ���GM������Ʒ
#define _GM_METAMO_RIDE          // GM���������г�
#define _GM_RIDE                 // GM�����������Ȩ��
#define _LOCK_IP                 // ����IP
#define _POINT                   // ��������
#define _VIP_SERVER              // ��Ա����
#define _WATCH_FLOOR             // �Զ���ɹ�ս��ͼ
#define _BATTLE_FLOOR            // �Զ���ǿ��ս����ͼ
#define _VIP_SHOP                // ��Ա�̵�
#define _LOOP_ANNOUNCE           // ѭ������
#define _SKILLUPPOINT_CF         // �Զ�����������
#define _RIDELEVEL               // �Զ���������ȼ�
#define _REVLEVEL                // �Զ���ԭ�ȼ�
#define _FM_ITEM                 // ��������
#define _LUCK_ITEM               // ��������
#define _NEW_PLAYER_RIDE         // �������������
#define _FIX_CHARLOOPS           // ������ħ��ʯ����ʱ��
#define _PLAYER_ANNOUNCE         // �������
#define _BATTLE_GOLD             // ��ȡ����ͬʱ��ý�Ǯ
#define _ITEM_GOLD               // ��Ǯ����Ʒ
#define _PLAYER_NUM              // �����������
#define _PET_MM                  // �����ʯMM
#define _MYSTERIOUS_GIFT         // ��������
#define _RELOAD_CF               // �ض�CF
#define _PET_UP                  // �ɼ���˶����ĳ���
#define _TRANS                   // ����ת������
#define _TRANS_7                 // ʵ������7ת
#define _RIDEMODE_20             // 2.0���ģʽ
#define _TALK_ACTION             // ˵������
#define _ENEMY_ACTION            // ��Ҳ������л���
#define _CHAR_CDKEY              // ��ֹ��������
#define _GMSV_DEBUG              // GMSV������Ϣ
#define _CHECK_PEPEAT            // �Ƿ�������
#define _SILENTZERO              // ������Ʒ
#define _SAVE_GAME_ID            // ��ʾ����ķ�������
#define _ANGLE_EMAIL             // ���ٺ��Զ�������Ƭ
#define _ALL_TALK                // ����Ƶ��
#endif
//��Ա��׷�ӹ���--------------------------------------
#define _VIP_ALL
#ifdef _VIP_ALL
#define _VIP_RIDE                // ��Ա��ڼ�
#define _AUTO_PK                 // �Զ���PKϵͳ
#define _FM_METAMO               // ����ר�ñ����ָ
#define _ITEM_METAMO_TIME        // ���Զ������Ľ�ָ
#define _SHOW_VIP_CF             // �Զ������VIP�����ˣ����壬������ʾ��ʽ
#define _GM_KING                 // GM��ħ��
//#define _MULTI_THREAD_KERNEL   // ��д���̺߳���
#define _ANGEL_TIME              // �Զ��徫���ٻ�ʱ��
#define _FM_POINT_PK             // ׯ԰���廥��ׯ԰
#define _PLAYER_MOVE             // ���˳�ƹ���
#define _BATTLE_PK               // ǿ��PK���ߣ����˻ؼ�¼��
#define _FUSIONBEIT_TRANS        // �ںϳ���ת��
#define _FUSIONBEIT_FIX          // ȡ���ںϳ������
#define _TRANS_7_NPC             // ������תNPC
#define _MOMENTUM_NPC            // ���ƽű�
#define _DP_NPC                  // DP�ű�
#define _RIDE_CF                 // �Զ������
#define _PET_LEVEL_ITEM          // ����ȼ�������Ʒ
#define _FM_LEADER_RIDE          // �峤ר�����
//#define _DBSAVE_CHAR           // ˫��浵
#define _FM_JOINLIMIT            // ������ٴμ�������ʱ������
#define _PET_BEATITUDE           // ����ף��
#define _ITEM_EFMETAMO           // ���ñ����ָ
#define _OPEN_E_PETSKILL         // ���ų���E����
#define _ITEM_RIDE               // ѱ��װ��
#define _MAKE_MAP                // ��ͼ����
#define _MAKE_PET_CF             // �Զ������ɳ�
#define _GET_MULTI_ITEM          // һ�θ������Ʒ
#define _VIP_ANNOUNCE            // ��Ա����ϵͳ��������
#define _NOWEN_EV                // �����Զ�NOW��EN�������
#define _NEW_NAME                // �Զ���ƺ�
#define _MAKE_PET_ABILITY        // ������Χ����
#define _ZANZHU_SERVER           // ������ϵͳ
#define _TRADE_PK                // �Ĳ�PK
#define _JK_CF_DELPETITEM        // ɾ��CF�����õĳ���͵���
#define _SAFE_PASSWORD           // ��ȫ��
#define _POINT_CARD              // ��ֵ��
#define _NEED_ITEM_ENEMY         // �Զ���ץ�����
#define _SUPER_MAN_ITEM          // ������Ʒ�˵���
#define _DIY_INSLAY              // �Զ���װ����Ƕ
#define _MAP_HEAL                // �Զ����ͼս����ָ�����
#define _NO_WARP
//#define _BATTLE_NO_WANG        // �׶���ħ�Ų�����
#define _LUCK_MAN                // ������
#define _QUESTION_ONLINE				 // �����ʴ�
#define _ONLINE_SHOP             // �����̳�
#define _SQL_BUY_FUNC            // �����
#define _MYSQL_GMSV						   // ����MYSQL
#ifdef _MYSQL_GMSV
 #define _SASQL
#endif
#define	_8018_SA						     // 8018֧��
//���ӹ��ܿ�ʼ
#define _NO_STREET_MAP					 // ���ɰ�̯��ͼ
#define _TRANS7_POINT            // ��ת����
#define _PET_BUG								 // ����BUG
//#define _JOB_AUTOPK            // ְҵ�Զ�PK
#define _EV_NUM								   // �ܻ�
#define _ALL_ITEM                // ȫ�ֱ���
#define _NOJOB_PK                // ��ְҵPK
#define _NONUM_AUTOPK            // ��̭���Զ�PK
//#define _NO_ATTACK						 // ����CC����
#define _NO_TRANS_ANGLE          // 6ת���辫���ٻ�
#define _VIGOR_SYS               // ����ϵͳ
#define _OFFLINE_SYSTEM					 // ���߹һ�
#define	_NPCENEMY_1							 // ����NPCENEMY����
#define _WAN_FIX							   // �����Ż��޸�
#define _WARNPC_CTRL					   // ħ������ϵͳ�������ٺ�ͬһIP������ӣ�
#define	_ITEM_RESET						   // ���߻�¯�ͻ���
#define	_NO_DAOLUAN						   // ��ֹ����ϵͳ
#define	_FMWAR_PLAYERNUM			   // �Զ�����ս����
#define	_PETMAIL_LV						   // �����ʼ��ʼĳ���ȼ����ƺͿ���
#define	_FMZUZHANG_MSG				   // ׯ԰�峤����֪ͨ
#define	_FM_WELFARE						   // ׯ԰���帣��
#define	_FMWAR_MSG						   // �Զ���ׯ԰����ս������ش���
#define	_ZHIPIAO_SYSTEM				   // ֧Ʊϵͳ
#define	_FM_SERVERNO_SYS			   // ָ����·���崴���ͽ�ɢ
#define	_BATTLEMAP_CTRL				   // ָ����ͼս��ʱ��
#define	_STU_SYS							   // ʦͽϵͳ��ǿ
#define	_NPC_ITEMUP						   // NPC��������
#define	_NO_DROPITEM				   	 // �󶨵��ߣ�����ʱ������ʾ
#define	_PET_RESET						   // �����¯
#define	_FMRANK_POINT					   // ���徺������
#define	_PETSKILL_DEL_SHOP		   // ���＼�ܻ�¯
#define	_ASSA_TOP						     // С�����֧��
#define	_NPC_PETSKILLUP			     // NPC���＼�ܽ���
#define	_PETSKILL_BINDING		     // ���＼�ܰ󶨣�������ż��ܵĳ��ﲻ�ܽ��ף��ӳ�����̯��
#define	_PETSKILL_DEL_NPC		     // ���＼�ܻ�¯
#define	_NO_FULLPLAYER_ATT	     // �����˹���
#define	_ATTREVERSE_FIX			     // ���Ի�ԭ�޸�
#define	_NEWFM_GOLD					     // �½�������Ҫ��Ǯ��
#define	_PSKILL_GYRATE			     // GYRATE���＼������ǿ��
#define	_PSKILL_BATTLE_MODEL	   // Ⱥ�����＼������ǿ��
#define	_JOB_FIX							   // ְҵƽ��
#define	_RMB_SYSTEM					     // �ֽ��˻�ϵͳ
#define	_PET_STANDBY				     // ������ս���в�����
#define	_MERGE_TIME					     // �Զ���ϳ�����ʱ��
#define	_EXP_TIME_OUT				     // ������Ϣ����ʾ�����Ч����ʱ��
#define	_PETUP_GET_EXP			     // ��������ǿ�ƶ���131-140���飬131��ǰ�������60000
//#define	_MM_NO_JIAOBEN			     // ��ֹ��MM�ű�
#define	_CHANGE_PETNAME_FIX	     // �޸���������������ݻ���BUG
#define	_DP_ZHIPIAO_ITEM		     // DP֧Ʊ����
#define	_DP_140_160_MODE		     // ȡ��ԭ�������������DPģʽ����Ϊ140���10W��160���100W
//#define	_NPC_RAND_MSG				     // NPC�м��������֤�Ӽ����ж�
#define	_PETSKILL_NONO_FIX	     // �޸����＼���ص�BUG
#define	_LOOK_STREET_TIME			   // ��̯ʱ�䣬��ֹ��̯
#define	_BATTLE_EQUIT_OTHER	     // ս�����Ƿ�ɻ������Լ���Ʒ
#define	_PK_LUCK_MAN				     // PK���˶�
#define	_SUPERMAN_FIX				     // ��Ʒ�˵�������ʱ������
#define	_DROPITEM_COLOR			     // ������ʧ�ĵ��߸ı���ɫ
#define	_ALL_RANDMODE				     // ���ű���֤ģʽ����
#define	_PICKUP_ITEM_OTHER		   // �ӵ��ߺ��Ƿ�ֻ���Լ���ȡ
#define	_FM_LIAOLI						   // �������Ϳ��Լ�������
#define	_TRADE_ITEM_FIX			     // ��̯����ʱ��ʾ������ʵ����
#define _PETMAIL_TIME				     // �����ʼ��Զ���ʱ�䣨Ҳ���ǳ������ݣ�
#define	_GJS_TYPE						     // �⾵���Ƿ���ԶԶԷ�ʹ��
#define	_PET_BINDING				     // ����󶨣���*�������ֵĳ��ﲻ�ܽ��ף��ӳ�����̯��
//#define	_NOT_PETRIDE					 // ����������ƣ���*�������ֵĳ��ﲻ����ˣ�
#define	_BATTLEEND_FIX			     // �����ͼս���˳���ʱ�䱣��120��
#define	_BIG_POOL_TYPE			     // �����ֿ�ֻ����ȡ�������Դ�
#define	_ONLINENUM_FIX			     // ͻ�������������1024
#define	_FLOOR_PARTY_CTRL		     // ָ����ͼ�������
#define	_NO_RIDE_ID						   // CF��ָ�����ﲻ��Ҫ��֤
#define _STREET_FAX              // ��̯����˰��
#define _XC_CANGKU               // ����ֿ�
#endif

//1717sq����------------------------------------------
//����ABLUA-------------------------------------------
#define _ALLBLUES_LUA            // ʹ��LUA�ű�
#ifdef _ALLBLUES_LUA
 #define _ALLBLUES_LUA_1_1
 #define _ALLBLUES_LUA_1_2
 #define _ALLBLUES_LUA_1_3
 #define _ALLBLUES_LUA_1_4
 #define _ALLBLUES_LUA_1_5
 #define _ALLBLUES_LUA_1_6
 #define _ALLBLUES_LUA_1_7
 #define _ALLBLUES_LUA_1_8
 #define _ALLBLUES_LUA_1_9
 #define _ALLBLUES_LUA_2_0      // LUA2.0��
#endif

#define _NEW_CurrencyShow        // �µĻ�����ʾ
#define _CAMEO_MONEY      		   // ֧��AB�����ֶ�
#define _MISSION_TRAIN    		   // ֧��AB�������ֶ�
#define _PET_EVOLVE    				   // ֧��AB�����ֶ�
#define _FLOOR_DIFI_FIELD			   // ֧��AB¥���ֶ�
#define _MISSION_TIME					   // ֧��AB�����ʱ�ֶ�
#define _PLAYER_TITLE            // ֧��AB�ƺŵȼ��ֶ�
#define _COWRIE                  // ֧��AB�����ֶ�
#define _ONLINE_TIME             // ֧��AB����ʱ���ֶ�
#define _SPECIAL_COUNTER         // ֧��AB�������ֶ�
#define _SPECIAL_TIMER           // ֧��AB��ʱ���ֶ�
#define _setVipPointForCdkey     // ֧��AB��setVipPointForCdkey��SQL����(setVipPoint��getVipPoint��֧��)
#define _VIP_BATTLE_EXP          // ��Աս�������޸Ĳ���
#define _ALLBLUES_LUA_NEWADD     // �����ӵ�ABLUA�ӿ�
#ifdef _ALLBLUES_LUA_NEWADD
  #define _AutoBattle_LUA        // ԭ������
#endif


//˽���汾����----------------------------------------
#define _SA80
#ifdef _SA80
//#define _TEST
#ifndef _TEST
//#define _DEFAULT_PKEY "17csajkjz"   // ( ʯ�� 8.0 )
//#define _RUNNING_KEY  "20081026"    // ( ʯ�� 8.0 )
//#define _SA_VERSION 'C'
#define _DEFAULT_PKEY "www.longzoro.com"   // ( ʯ�� 8.0 )
#define _RUNNING_KEY  "www.longzoro.com"   // ( ʯ�� 8.0 )
#define _SA_VERSION 'L'
#define _ITEM_TYPETABLE          // (�ɿ�) Change ׷��typetable
#define _RIGHTCLICK              // Robin �����Ҽ�����
#define _JOBDAILY                // cyg ������־����
#define  _ALCHEMIST              // Robin ��  ����
#define _TEACHER_SYSTEM          // ��ʦϵͳ(����)
#define _PET_ITEM                // ����װ��
#else
#define _SA_VERSION 'F'
#define _DEFAULT_PKEY "upupupupp"// ( ʯ�� 7.5 )
#define _RUNNING_KEY  "20060707" // ( ʯ�� 7.5 )
#endif
#define SERVER_VERSION "GMSV-v1.0.0"
#else
//#define _SA75
#ifdef _SA75
#define _DEFAULT_PKEY "ttttttttt"// ( ʯ�� 7.5 )
#define _RUNNING_KEY  "20041215" // ( ʯ�� 7.5 )
#else
#define _DEFAULT_PKEY "upupupupp"// ( ʯ�� 7.5 )
#define _RUNNING_KEY  "20060427" // ( ʯ�� 7.5 )
#define _SA_VERSION_80           // ʯ��ʱ��8.0 �İ汾����
#endif
#define _SA_VERSION 'F'
#define _FIX_FM_FMPOINT          // �޸ļ���ݵ���ʾ��ʽ
#define SERVER_VERSION "GMSV-v1.0.0"
#endif

#define _JZ_NEWSCRIPT_LUA				 // ���� ��LUA�ű�����

//���⹦�ܿ���----------------------------------------
#define _YUANGUSA
#define _FreeServerVersion       // ���Ź���
#ifdef _FreeServerVersion
  #define _NOT_POJIE             // ����GM�������
  #define _LUA_RootSSH           // LUA��ȡroot(1717sq��ʾ:��л��ʹ�õ�������)
#endif


//#define _DEBUG                 // ����
//#define _TEST_SERVER           // ���Է�����
#endif

#define _17CSA_VERSION "8002"
//#define	_17CSA_YANZHENG				 //��ǿ��֤����
