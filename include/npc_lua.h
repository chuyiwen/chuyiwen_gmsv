#ifndef __NPC_LUA__
#define __NPC_LUA__

#include "npc_lua_interface.h"
#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define LUA_DATALINE0 0
#define LUA_DATALINE1 2000
#define LUA_DATALINE2 4000
#define LUA_DATALINE3 6000
#define LUA_DATALINE4 8000
#define LUA_DATALINE5 10000

typedef struct __CREATEENEMY
{
	int EnemyId;
	int BaseLevel;
	int SkillType;
}CREATEENEMY,*PCREATEENEMY;

typedef struct __SCRIPTREGLIB
{
	const char *LibName;				//��������
	luaL_reg *FuncRegList;			//��Ҫע��ĺ����⺯���б�
}SCRIPTREGLIB,*PSCRIPTREGLIB;

typedef struct __SCRIPTREGCLASS
{
	const char *ClassName;			//������
	PSCRIPTREGLIB NewLib;				//����������ʵ���� SCRIPTREGLIB
	luaL_reg *FuncRegList;			//��Ҫע����ຯ���б�
}SCRIPTREGCLASS,*PSCRIPTREGCLASS;

typedef struct __SCRIPTREGARRAY
{
	const char *ArrayName;			//������
	const char **SetMetaTable;	//��������������Ӧ����
	const char **SetFuncName;		//��������������Ӧ�����ĺ�������
	PSCRIPTREGLIB FuncList;			//������Ӧ��Ӧ SetMetaTable �Ĵ������б�
}SCRIPTREGARRAY,*PSCRIPTREGARRAY;

typedef struct __ARRAY_NTINT
{
	size_t Size;
	int Num[1];
}ARRAY_NTINT,*PARRAY_NTINT;

//����NPC-����NPCΨһ����
int NPC_Lua_Create(const char *_DoFile, const char *_InitFuncName, char *_seek, BOOL _IsFly);

//ɾ��NPC ����NPCΨһ����
int NPC_Lua_Del(int _index);

//����ս��
int NPC_Lua_CreateVsEnemy(lua_State *_NLL, int _CharaIndex, int _NpcIndex, const char *_DoFunc, PCREATEENEMY _CreateEnemy, int _ARLen, int _Flg);

const char *NPC_Lua_SetErrorStr(const char *ErrStr);

//ע����չ���ýӿ�
int NPC_Lua_RegCallEx(lua_State * _NLL, luaL_Reg *_RegList);
//ע����չ������ӿ�
int NPC_Lua_RegLibEx(lua_State * _NLL, PSCRIPTREGLIB _RegLib);
//ע����չ��ӿ�
int NPC_Lua_RegClassEx(lua_State * _NLL, PSCRIPTREGCLASS _RegClass);
//ע����չ����ӿ�
int NPC_Lua_RegArrayEx(lua_State * _NLL, PSCRIPTREGARRAY _RegArray);

//��չ���ͽӿں���
//NTInt
int NPC_Lua_NTInt_New(lua_State *_NLL);
int NPC_Lua_NTInt_Get(lua_State *_NLL);
int NPC_Lua_NTInt_Set(lua_State *_NLL);
int NPC_Lua_NTInt_Len(lua_State *_NLL);

//�ӿں���
////////////////////////////////////////////////NL/////////////////////////////////////////////////
int NPC_Lua_NL_GetErrorStr(lua_State *_NLL);						//���ش�����Ϣ
int NPC_Lua_NL_CreateNpc(lua_State *_NLL);							//����NPC
int NPC_Lua_NL_DelNpc(lua_State *_NLL);									//ɾ��NPC
int NPC_Lua_NL_Mod(lua_State *_NLL);									  //������
int NPC_Lua_NL_GetStringFromIndexWithDelim(lua_State *_NLL);									//ʹ��ָ���ַ��ָ��ַ����������ص�����ָ��indexλ�õ��ַ���
//int NPC_Lua_NL_ANSI_PlayerLoop(lua_State *_NLL);		    //�������д����������������
int NPC_Lua_NL_PetLoopGetNext(lua_State *_NLL);		      //�������д��ڳ�����س��������� д�����ƶ�ʱ��
int NPC_Lua_NL_ItemLoopGetNext(lua_State *_NLL);		    //�������д��ڵ��ߣ����ص��������� ����
int NPC_Lua_NL_PlayerLoopGetNext(lua_State *_NLL);		  //�������д�������������������� ����
int NPC_Lua_NL_GetConfigLineType(lua_State *_NLL);	  	//����0Ϊ�ַ����Ρ�1ΪSHORT�Ρ�2ΪINT�͡�3ΪDOUBLE�͡���������ڸ��ֶΣ�����-1.
int NPC_Lua_NL_GetConfigLineVal(lua_State *_NLL);	  	  //���Ƿ���[�ַ���]�������Ҫת��Ϊ���֣�����tonumber()�������Ҳ������ֶη���nil
BOOL NPC_Lua_CheckNpcEventFree(lua_State *_NLL);									//�������Ƿ�����ĳ����
int NPC_Lua_DoNpcEventAction(lua_State *_NLL);									//ִ��NPC�ű�����
int NPC_Lua_NL_SetTimer(lua_State *_NLL);	  	          //�趨һ����ʱ��������ĺ��������趨�ļ��ʱ�����ڵ��á�
int NPC_Lua_NL_DelTimer(lua_State *_NLL);	  	          //ɾ��һ����ʱ����һ����ʱ����ɾ�����Ͳ����ٱ����ڵ��ã�
int NPC_Lua_NL_RunSaFuncII(lua_State *_NLL);	  	      //����һ��������ڶ���õ�2��INT�����ĺ�����
int NPC_Lua_NL_RunSaFuncIII(lua_State *_NLL);	  	      //����һ��������ڶ���õ�3��INT�����ĺ���(��ITEM_USE��ʹ�õ��ߺ�Ļص���������
int NPC_Lua_NL_ANSI_PlayerLoop(lua_State *_NLL);		    //�������д����������������
int NPC_Lua_NL_ANSI_PetLoop(lua_State *_NLL);		        //�������д��ڳ������������
int NPC_Lua_NL_ANSI_ItemLoop(lua_State *_NLL);		      //�������д��ڵ��ߣ�����������
int NPC_Lua_NL_ClsMk(lua_State *_NLL);
int NPC_Lua_NL_AddCheckNum(lua_State *_NLL);
int NPC_Lua_NL_Mkstring(lua_State *_NLL);
int NPC_Lua_NL_Mkint(lua_State *_NLL);
int NPC_Lua_NL_SendMesg(lua_State *_NLL);

////////////////////////////////////////////////NLG////////////////////////////////////////////////
//���ܽӿ�
int NPC_Lua_NLG_UpItem(lua_State *_NLL);
int NPC_Lua_NLG_GiveItemOne(lua_State *_NLL);
int NPC_Lua_NLG_GivePetOne(lua_State *_NLL);
int NPC_Lua_NLG_DelItemByIndex(lua_State *_NLL);	
int NPC_Lua_NLG_FindPet(lua_State *_NLL);								//���ҳ�������λ
int NPC_Lua_NLG_GetMap(lua_State *_NLL);						    //��õ�ǰλ�õĽ�����
int NPC_Lua_NLG_PetUp(lua_State *_NLL);								  //��������
int NPC_Lua_NLG_AddSk(lua_State *_NLL);								  //��ְҵ����
int NPC_Lua_NLG_SetZy(lua_State *_NLL);								  //�趨ְҵ
int NPC_Lua_Char_GetPlayerMaxNum(lua_State *_NLL);			//��ȡ�������
int NPC_Lua_NLG_GetXY(lua_State *_NLL);							  	//��ȡһ����ͼ������
int NPC_Lua_NLG_WalkJc(lua_State *_NLL);							  //����ϰ���
int NPC_Lua_NLG_KickPlayer(lua_State *_NLL);				  	//T��ָ�����
int NPC_Lua_NLG_HealOne(lua_State *_NLL);				  	    //ָ��һ������ �ظ���HP ��MP
int NPC_Lua_NLG_HealAll(lua_State *_NLL);				  	    //ָ��һ�����MP �ظ����а�������HP
int NPC_Lua_NLG_LevelUpTo(lua_State *_NLL);				  	  //��������ĵȼ���
int NPC_Lua_NLG_AddExp(lua_State *_NLL);			  	  	  //����һ�������顣
int NPC_Lua_NLG_UpPet(lua_State *_NLL);			  	  	    //���¶���ĳ�����ڸ��¡��ͻ��ˡ��������ݣ���ĳЩ��̨�������Ա��޸ĵ�����²��ܰѸ������ݷ��͵�ǰ̨��
int NPC_Lua_NLG_DelPetByPos(lua_State *_NLL);						//ɾ�� ���� ����ָ����λ�ĳ���
int NPC_Lua_NLG_DelPetByIndex(lua_State *_NLL);					//ɾ�� ���� ����ָ�������ţ���ID���ĳ���
int NPC_Lua_NLG_GivePetByIndex(lua_State *_NLL);				//����һָֻ�������ŵĳ����ָ�� ���� ���س�����λ��
int NPC_Lua_NLG_GiveItemByIndex(lua_State *_NLL);				//����һ������indexָ����� ���ص�����λ
int NPC_Lua_NLG_WarpToSpecificPoint(lua_State *_NLL);	  //�ƶ�NPC ���������ƶ�
BOOL NPC_Lua_NLG_UpSkillupPoint(lua_State *_NLL);       //�����������
int NPC_Lua_NLG_StayEncount(lua_State *_NLL);	          //ԭ��

int NPC_Lua_NLG_CheckInFront(lua_State *_NLL);					//���ڼ��ĳ�������Ƿ���ĳ��������ǰ
int NPC_Lua_NLG_CheckObj(lua_State *_NLL);							//���ĳ����ͼλ���Ƿ��ж������
int NPC_Lua_NLG_CharLook(lua_State *_NLL);							//�������﷽��(���͸��·��)
int NPC_Lua_NLG_CreateBattle(lua_State *_NLL);					//����ս��
int NPC_Lua_NLG_InputCard(lua_State *_NLL);							//����һ���Ի���������������⿨��
int NPC_Lua_NLG_CreateBattlePvP(lua_State *_NLL);

int NPC_Lua_NLG_SearchWatchBattleRandIndex(lua_State *_NLL);

int NPC_Lua_NLG_DelPet(lua_State *_NLL);								//ɾ��ָ����ҳ�����һ����������
int NPC_Lua_NLG_DelHaveIndexPet(lua_State *_NLL);
int NPC_Lua_NLG_DelItem(lua_State *_NLL);								//ɾ��ָ����ҵ�����һ����������
int NPC_Lua_NLG_DelItemByPos(lua_State *_NLL);								//ɾ��ָ����ҵ�����һ����������
int NPC_Lua_NLG_DischargeParty(lua_State *_NLL);				//��ɢ �Ŷ�

int NPC_Lua_NLG_GivePet(lua_State *_NLL);								//��ָ�����һ����������
int NPC_Lua_NLG_GiveOnePet(lua_State *_NLL);								//��ָ�����һ������
int NPC_Lua_NLG_GiveItem(lua_State *_NLL);							//��ָ�����һ����������
int NPC_Lua_NLG_GiveOneItem(lua_State *_NLL);							//��ָ�����һ������
int NPC_Lua_NLG_GiveRandItem(lua_State *_NLL);
int NPC_Lua_Char_GetOnLinePlayer(lua_State *_NLL);			//��ȡ��ǰ��������

int NPC_Lua_NLG_ShowWindowTalked(lua_State *_NLL);			//��ָ����ҿͻ�����ʾָ�����ݡ����͵ĶԻ���
int NPC_Lua_NLG_SetAction(lua_State *_NLL);							//���ö���Ķ���

int NPC_Lua_NLG_TalkToCli(lua_State *_NLL);							//��ͨ˵�������Զ�ȫ��������˵
int NPC_Lua_NLG_TalkToFloor(lua_State *_NLL);						//����ĳһָ����ͼ�ڵ����˵��

int NPC_Lua_NLG_UpChar(lua_State *_NLL);								//��NPC��Χ����ҷ���NPC�����ݸ��·��
int NPC_Lua_NLG_UpStateBySecond(lua_State *_NLL);				//��ָ����ҷ��͵�2��״̬���·��
int NPC_Lua_NLG_UpStateByThird(lua_State *_NLL);				//��ָ����ҷ��͵�3��״̬���·��
int NPC_Lua_NLG_Update_Party(lua_State *_NLL);					//��������ڵ��Ŷӳ�Ա������ҵ�״̬���·��

int NPC_Lua_NLG_Warp(lua_State *_NLL);									//����һ������
int NPC_Lua_NLG_WalkMove(lua_State *_NLL);							//�ƶ�һ������
int NPC_Lua_NLG_WatchEntry(lua_State *_NLL);						//��ս
int NPC_Lua_NLG_HealerAllHeal(lua_State *_NLL);         //��Ѫ
int NPC_Lua_NLG_GetMaxPlayNum(lua_State *_NLL);					//��ȡ���������������
int NPC_Lua_NLG_CheckPlayIndex(lua_State *_NLL);				//�����������Ƿ�����
int NPC_Lua_NLG_Save(lua_State *_NLL);
////add by mo
int NPC_Lua_NLG_SetExp(lua_State *_NLL);							//���÷��������鱶��
int NPC_Lua_NLG_GetExp(lua_State *_NLL);							//��ȡ���������鱶��
#ifdef _PETTRANS_RANGE
int NPC_Lua_NLG_SetPetTransRange(lua_State *_NLL);				//���ó���ת����Χ����
int NPC_Lua_NLG_GetPetTransRange(lua_State *_NLL);				//��ó���ת����Χ����
#endif
///////////////////////////////////////////////Char////////////////////////////////////////////////
//�������ݵĽӿ�
int NPC_Lua_Char_ClrEvtEnd(lua_State *_NLL);						//����������������־
int NPC_Lua_Char_ClrEvtNow(lua_State *_NLL);						//������������������־
int NPC_Lua_Char_ClrEvt(lua_State *_NLL);								//������������־
int NPC_Lua_Char_GetAllocPoint(lua_State *_NLL);	      //��ȡ����CharIndex����ά�ɳ�
int NPC_Lua_Char_GetDataFromEnemyBase(lua_State *_NLL);	//��ȡ��Ӧ ��־ �ĳ���ģ�����ݣ�enemybaseX.txt����
int NPC_Lua_Char_GetPlayerIndex(lua_State *_NLL);       //�������
int NPC_Lua_Char_FindItemIndex(lua_State *_NLL);				//����ָ�����������Ƿ�ӵ��ĳ����Ϊָ����������
int NPC_Lua_Char_FindPetIndex(lua_State *_NLL);		      //����ָ�����������Ƿ�ӵ�г���Ϊָ����������
int NPC_Lua_CHAR_CHECKINDEX(lua_State *_NLL);				     //�������
int NPC_Lua_Char_GetPetSkill(lua_State *_NLL);				  //ȡ�ó���ָ���������ļ��ܱ��
int NPC_Lua_Char_GetPetSkillNo(lua_State *_NLL);	      //��ȡChar����
int NPC_Lua_Char_RidNo(lua_State *_NLL);								//������������־
int NPC_Lua_Char_GetEmptyItemBoxNum(lua_State *_NLL);	  //����ָ���������ϵ�������λ������
int NPC_Lua_Char_GetEmptyPetBoxNum(lua_State *_NLL);	  //����ָ���������ϳ�������λ������
int NPC_Lua_Char_GetPlayerItemNum(lua_State *_NLL);	    //����ָ����������ΪĳID�ĵ��ߵ�����
int NPC_Lua_Char_GetPlayerPetNum(lua_State *_NLL);	    //����ָ����������ΪĳID�ĳ��������
int NPC_ABLua_char_getPlayerMaxNum(lua_State *_NLL);    //����������
int NPC_ABLua_char_getBattleexp(lua_State *_NLL);       //ս�����鱶��
int NPC_Lua_Char_SetPetSkill(lua_State *_NLL);				  //���ó���ָ���������ļ��ܱ��
int NPC_Lua_Char_SetPetSkillNo(lua_State *_NLL);	
int NPC_Lua_Char_ComplianceParameter(lua_State *_NLL);	//���ݶ������������¼��㲢����WORK���ԣ��Դﵽ��Ч��Ŀ�ġ�
int NPC_Lua_Char_HcItemId(lua_State *_NLL);					  	//�ϳ��õ����������Ϸ�����Ʒ��
int NPC_Lua_Char_DelHcItem(lua_State *_NLL);					  //ɾ������ĳ��λ�õ���Ʒ��
int NPC_Lua_Char_GETFM(lua_State *_NLL);					      //��ȡ�������� ���� �ʽ�
int NPC_Lua_Char_FindPetEnemyBaseId(lua_State *_NLL);		//����ָ�����������Ƿ�ӵ�г���EnemyBase�ļ��Ķ�ӦID
int NPC_Lua_Char_ITEMID_NAME(lua_State *_NLL);					//���ָ������ID������
int NPC_Lua_Char_ITEMID_NAME2(lua_State *_NLL);					//���ָ������ID��˵��
int NPC_Lua_Char_ITEMID_LEVEL(lua_State *_NLL);					//���ָ������ID�ĵȼ�
int NPC_Lua_Char_ITEMID_GOLD(lua_State *_NLL);					//���ָ������ID�ļ۸�
int NPC_Lua_Char_ITEMID_RANO(lua_State *_NLL);					//���ָ������ID��ͼ��
//int NPC_Lua_Char_CharRidNo(lua_State *_NLL);					  //�������Ƿ��Ѿ�����ĳ���֤��
int NPC_Lua_Char_GetPetOwner(lua_State *_NLL);				  //��ó������˵�������
int NPC_Lua_Char_GetEnemyBaseIdFromEnemyId(lua_State *_NLL);	//ͨ��enemy.txt�������õĳ�����������enemybase.txt�����趨�ĳ����š�
int NPC_Lua_Char_GetEnemyIdFromEnemyBaseId(lua_State *_NLL);	//ͨ��enemybase.txt�������õĳ�����������enemy.txt�����趨�ĳ����š�
int NPC_Lua_Char_GetIp(lua_State *_NLL);	              //��ȡָ�������IP��ַ
int NPC_Lua_Char_SetAllocPoint(lua_State *_NLL);	      //���ó���CharIndex����ά�ɳ�
int NPC_Lua_Char_GetMaxPetIndex(lua_State *_NLL);	      //��ȡ����������
int NPC_Lua_Char_GetMaxPlayerIndex(lua_State *_NLL);	  //��ȡ�����������
int NPC_Lua_Char_SavePetToString(lua_State *_NLL);	    //����һ������������ַ�����
int NPC_Lua_Char_LoadPetFromString(lua_State *_NLL);	  //����һ������������ַ���������һ�����
int NPC_Lua_Char_GetPlayerFromAccAndName(lua_State *_NLL);//������ҵ��ʺź���������Ѱ����ҡ��ʺź�����������Ҳ�ɲ��ҡ����ɶ�����
int NPC_Lua_Char_CheckPet(lua_State *_NLL);             //�����������Ƿ���Ч��
int NPC_Lua_Char_CheckPlayer(lua_State *_NLL);          //������������Ƿ���Ч��
BOOL NPC_Lua_Char_JoinParty(lua_State *_NLL);           //�����Ŷ�
int NPC_Lua_Char_getFamilyPlayIndex(lua_State *_NLL);   //��ü������������������

int NPC_Lua_Char_SetEvtEnd(lua_State *_NLL);						//���������־Ϊ����״̬
int NPC_Lua_Char_SetEvtNow(lua_State *_NLL);						//���������־Ϊ������״̬
int NPC_Lua_Char_SetData(lua_State *_NLL);							//����Char����

int NPC_Lua_Char_getVipPoint(lua_State *_NLL);          //��Ա�����
int NPC_Lua_Char_getjfPoint(lua_State *_NLL);           //���ָ���
int NPC_Lua_Char_getxjPoint(lua_State *_NLL);           //�ֽ����
int NPC_Lua_Char_setVipPoint(lua_State *_NLL);          //��Ա�����
int NPC_Lua_Char_setjfPoint(lua_State *_NLL);           //���ֲ���
int NPC_Lua_Char_setxjPoint(lua_State *_NLL);           //�ֽ����

//�¼����õĽӿ�
int NPC_Lua_Char_SetWalkPreEvent(lua_State *_NLL);			//����WalkPre�¼���Ӧ
int NPC_Lua_Char_SetWalkPostEvent(lua_State *_NLL);			//����WalkPost�¼���Ӧ
int NPC_Lua_Char_SetPreOverEvent(lua_State *_NLL);			//����PreOver�¼���Ӧ
int NPC_Lua_Char_SetPostOverEvent(lua_State *_NLL);			//����PostOver�¼���Ӧ
int NPC_Lua_Char_SetWatchEvent(lua_State *_NLL);				//����Watch�¼���Ӧ
int NPC_Lua_Char_SetLoopEvent(lua_State *_NLL);					//����Loop�¼���Ӧ
int NPC_Lua_Char_SetTalkedEvent(lua_State *_NLL);				//����Talked�¼���Ӧ
int NPC_Lua_Char_SetOFFEvent(lua_State *_NLL);					//����OFF�¼���Ӧ
int NPC_Lua_Char_SetLookedEvent(lua_State *_NLL);				//����Looked�¼���Ӧ
int NPC_Lua_Char_SetItemPutEvent(lua_State *_NLL);			//����ItemPut�¼���Ӧ
int NPC_Lua_Char_SetWindowTalkedEvent(lua_State *_NLL);	//����WindowTalked�¼���Ӧ
#ifdef _USER_CHARLOOPS
int NPC_Lua_Char_SetCharLoopsEvent(lua_State *_NLL);		//����CharLoops�¼���Ӧ
int NPC_Lua_Char_SetBattleProPertyEvent(lua_State *_NLL);//����BattleProPerty�¼���Ӧ
#endif

//��ȡ���ݵĽӿ�
int NPC_Lua_Char_IsEventEnd(lua_State *_NLL);						//����Ƿ�����������־
int NPC_Lua_Char_IsEventNow(lua_State *_NLL);						//����Ƿ��������������־

int NPC_Lua_Char_FindItemId(lua_State *_NLL);						//����ָ�����������Ƿ�ӵ��ĳ����ID
int NPC_Lua_Char_FindPetEnemyId(lua_State *_NLL);				//����ָ�����������Ƿ�ӵ�г���Enemy�ļ��Ķ�ӦID

int NPC_Lua_Char_GetData(lua_State *_NLL);						//��ȡChar����
int NPC_Lua_Char_GetItemId(lua_State *_NLL);						//��ȡָ����������ָ��λ�õĵ���ID
int NPC_Lua_Char_GetPetEnemyId(lua_State *_NLL);				//��ȡָ����������ָ��λ�õĳ���Enemy�ļ��Ķ�ӦID
int NPC_Lua_Char_GetItemIndex(lua_State *_NLL);					//��ȡָ����������ָ��λ�õĵ�������
int NPC_Lua_Char_GetPetIndex(lua_State *_NLL);					//��ȡָ����������ָ��λ�õĳ����������
int NPC_Lua_Char_GetTeamIndex(lua_State *_NLL);					//��ȡ��Ա�Ķ�������
int NPC_Lua_Char_GetPlayerIndexByCdkey(lua_State *_NLL);		//ͨ���ʺŲ�ѯ��������
int NPC_Lua_Char_VipPoint(lua_State *_NLL);
int NPC_Lua_Char_HealAll(lua_State *_NLL);
int NPC_Lua_Char_GetPetSkillId(lua_State *_NLL);
int NPC_Lua_Char_GetPetSkillName(lua_State *_NLL);
int NPC_Lua_Char_GetPetSkillMsg(lua_State *_NLL);
int NPC_Lua_Char_SetPetSkill(lua_State *_NLL);
int NPC_Lua_Char_RandMsg(lua_State *_NLL);
int NPC_Lua_Char_GetBeatitudeNum(lua_State *_NLL);
///////////////////////////////////////////////GAME////////////////////////////////////////////////
int NPC_Lua_Game_FMPOINT_GetData(lua_State *_NLL);           //���ׯ԰�ݵ����Ϣ
int NPC_Lua_Game_FMPOINT_ACSetFMPoint(lua_State *_NLL);      //�趨����ݵ���Ϣ�������ݵ�δ��ռ��ʱ��Ч��
int NPC_Lua_Game_FMPOINT_ACFixFMPoint(lua_State *_NLL);      //�޸ļ���ݵ���Ϣ����������ơ�һ��������ս������
int NPC_Lua_Game_FMPOINT_ACCleanFMPoint(lua_State *_NLL);    //�������������ݵ㡣
///////////////////////////////////////////////Item////////////////////////////////////////////////
//��ȡ���ݵĽӿ�
int NPC_Lua_Item_GetData(lua_State *_NLL);							//��ȡ��������
int NPC_Lua_Item_GetDataFromItemSet(lua_State *_NLL);		//��ȡITEMSETX�ļ�������
//�������ݵĽӿ�
int NPC_Lua_Item_SetData(lua_State *_NLL);							//��ȡ��������

//�¼����õĽӿ�
int NPC_Lua_Item_SetPreOverEvent(lua_State *_NLL);			//����PreOver�¼���Ӧ
int NPC_Lua_Item_SetPostOverEvent(lua_State *_NLL);			//����PostOver�¼���Ӧ
int NPC_Lua_Item_SetWatchEvent(lua_State *_NLL);				//����Watch�¼���Ӧ
int NPC_Lua_Item_SetUseEvent(lua_State *_NLL);					//����Use�¼���Ӧ
int NPC_Lua_Item_SetAttachEvent(lua_State *_NLL);				//����Attach�¼���Ӧ
int NPC_Lua_Item_SetDetachEvent(lua_State *_NLL);				//����Detach�¼���Ӧ
int NPC_Lua_Item_SetDropEvent(lua_State *_NLL);					//����Drop�¼���Ӧ
int NPC_Lua_Item_SetPickUPEvent(lua_State *_NLL);				//����PickUP�¼���Ӧ
#ifdef _Item_ReLifeAct
int NPC_Lua_Item_SetDieReLifeEvent(lua_State *_NLL);		//����DieReLife�¼���Ӧ
#endif
///////////////////////////////////////////////BIT///////////////////////////////////////////////
int NPC_Lua_Bit_band(lua_State *_NLL);
int NPC_Lua_Bit_bor(lua_State *_NLL);
int NPC_Lua_Bit_bxor(lua_State *_NLL);
int NPC_Lua_Bit_lshift(lua_State *_NLL);
int NPC_Lua_Bit_rshift(lua_State *_NLL);
///////////////////////////////////////////////Obj////////////////////////////////////////////////
//��ȡ���ݵĽӿ�
int NPC_Lua_Obj_GetType(lua_State *_NLL);								//��ȡOBJ������
/*
int NPC_Lua_Obj_GetName(lua_State *_NLL);								//��ȡOBJ������
int NPC_Lua_Obj_GetDIR(lua_State *_NLL);								//��ȡOBJ�ķ���
int NPC_Lua_Obj_GetImageNum(lua_State *_NLL);						//��ȡOBJ�������
*/
int NPC_Lua_Obj_GetCharType(lua_State *_NLL);						//��ȡOBJ��Ӧ��Char���ݽṹ������
int NPC_Lua_Obj_GetCharIndex(lua_State *_NLL);					//��ȡOBJ��Ӧ��Char���ݽṹ������
int NPC_Lua_Obj_GetX(lua_State *_NLL);									//��ȡOBJ��X����
int NPC_Lua_Obj_GetY(lua_State *_NLL);									//��ȡOBJ��Y����
int NPC_Lua_Obj_GetFloor(lua_State *_NLL);							//��ȡOBJ���ڵĵ�ͼ���
//int NPC_Lua_Obj_GetDelTime(lua_State *_NLL);						//��ȡOBJ��ɾ��ʱ��

int NPC_Lua_Obj_SetType(lua_State *_NLL);								//��ȡOBJ������
/*
int NPC_Lua_Obj_SetName(lua_State *_NLL);								//��ȡOBJ������
int NPC_Lua_Obj_SetDIR(lua_State *_NLL);								//��ȡOBJ�ķ���
int NPC_Lua_Obj_SetImageNum(lua_State *_NLL);						//��ȡOBJ�������
*/
int NPC_Lua_Obj_SetCharType(lua_State *_NLL);						//��ȡOBJ��Ӧ��Char���ݽṹ������
int NPC_Lua_Obj_SetX(lua_State *_NLL);									//��ȡOBJ��X����
int NPC_Lua_Obj_SetY(lua_State *_NLL);									//��ȡOBJ��Y����
int NPC_Lua_Obj_SetFloor(lua_State *_NLL);							//��ȡOBJ���ڵĵ�ͼ���
//int NPC_Lua_Obj_SetDelTime(lua_State *_NLL);						//��ȡOBJ��ɾ��ʱ��

//////////////////////////////////////////////Battle///////////////////////////////////////////////
//��ȡ���ݵĽӿ�
int NPC_Lua_Battle_GetPlayIndex(lua_State *_NLL);				//��ȡս���е��������
int NPC_Lua_Battle_GetData(lua_State *_NLL);				    //��ȡս������
int NPC_Lua_Battle_SetData(lua_State *_NLL);				    //����ս������
int NPC_Lua_Battle_IndexToNo(lua_State *_NLL);				  //ͨ������������ȡ���ڵ�ս�����
int NPC_Lua_Battle_NoToIndex(lua_State *_NLL);				  //ͨ��ս����Ż�ȡ��������
int NPC_Lua_Battle_CheckIndex(lua_State *_NLL);				  //���ս�������Ƿ���Ч��
//�������ݵĽӿ�
int NPC_Lua_Battle_SetNORisk(lua_State *_NLL);					//�����Ƿ����޷���ģʽ
int NPC_Lua_Battle_SetMod(lua_State *_NLL);							//����ս��ģʽ��־
int NPC_Lua_Battle_SetType(lua_State *_NLL);						//����ս������
int NPC_Lua_Battle_Exit(lua_State *_NLL);				        //�˳�ս��
//�¼����õĽӿ�
int NPC_Lua_Battle_SetWinEvent(lua_State *_NLL);				//����Win�¼���Ӧ
int NPC_Lua_Battle_SetEndEvent(lua_State *_NLL);				//����Win�¼���Ӧ
///////////////////////////////////////////////Other///////////////////////////////////////////////
int NPC_Lua_GetFuncPoint(lua_State *_NLL);							//��ȡ����ָ��

//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////MAP////////////////////////////////////////////////
int NPC_Lua_Map_CheckCoordinates(lua_State *_NLL);         //���ĳ���Ƿ��ڵ�ͼ��Χ�ڡ�
int NPC_Lua_Map_GetExitFloorXY(lua_State *_NLL);           //����ͼ�Ƿ�֧�ֵǳ�
int NPC_Lua_Map_GetfloorX(lua_State *_NLL);                //��ȡ��ͼX����
int NPC_Lua_Map_GetfloorY(lua_State *_NLL);                 //��ȡ��ͼY����
int NPC_Lua_Map_GetTileAndObjId(lua_State *_NLL);           //��ȡ��ͼĳ��ĵذ��װ�β�
int NPC_Lua_Map_SetTileAndObjId(lua_State *_NLL);           //���õ�ͼĳ��ĵذ��װ�β�
int NPC_Lua_Map_GetWalkAbleFromPoint(lua_State *_NLL);      //��ȡĳ���Ƿ��������
int NPC_Lua_Map_GetImageData(lua_State *_NLL);              //��ȡͼƬ����
int NPC_Lua_Map_SetImageData(lua_State *_NLL);              //����ͼƬ����
int NPC_Lua_Map_GetTopObj(lua_State *_NLL);                 //������ȡ��ͼĳ���϶���
int NPC_Lua_Map_GetNextObj(lua_State *_NLL);                //��ȡ��һ����������
int NPC_Lua_Map_CheckImageIndex(lua_State *_NLL);           //���ĳ��ͼ��ͼƬ�Ƿ����
int NPC_Lua_Map_CheckIndex(lua_State *_NLL);                //���ĳ��ͼ�ŵ�ͼ�Ƿ����
int NPC_Lua_Map_MakeNewMap(lua_State *_NLL);                //����һ���µĵ�ͼ �����ص�ͼ��ID
int NPC_Lua_Map_DelNewMap(lua_State *_NLL);                 //ɾ��һ��������ͼ
int NPC_Lua_Map_SetExWarp(lua_State *_NLL);                 //��������ǳ����ͼ�Ĵ��͵�
int NPC_Lua_Map_SetMapPoint(lua_State *_NLL);               //��ӵ�ͼ���͵�
int NPC_Lua_Map_DelMapPoint(lua_State *_NLL);               //ɾ����ͼ���͵�
int NPC_Lua_Map_getFloorName(lua_State *_NLL);               //��õ�ͼ��
int NPC_Lua_Map_Upmap(lua_State *_NLL);                      //���µ�ͼ
///////////////////////////////////////////////MAP////////////////////////////////////////////////
///////////////////////////////////////////////Spell////////////////////////////////////////////////
int NPC_Lua_Spell_PETSKILL_GetData(lua_State *_NLL);    //��ȡ���＼��ID
int NPC_Lua_Spell_PROFESSION_GetData(lua_State *_NLL);  //��ȡְҵ����ID
int NPC_Lua_Spell_MAGIC_GetData(lua_State *_NLL);       //��ȡ���鼼��ID
int NPC_Lua_Spell_PETSKILL_SetData(lua_State *_NLL);    //���ö�ӦID�ļ������ݣ�
int NPC_Lua_Spell_PROFESSION_SetData(lua_State *_NLL);  //���ö�ӦID�ļ������ݣ�
int NPC_Lua_Spell_MAGIC_SetData(lua_State *_NLL);       //���ö�ӦID�ļ������ݣ�
///////////////////////////////////////////////Spell////////////////////////////////////////////////
///////////////////////////////////////////////SQL/////////////////////////////////////////////////
int NPC_Lua_SQL_Push(lua_State *_NLL);						//��������MySQLָ��
int NPC_Lua_SQL_PushPop(lua_State *_NLL);			//��������MySQLָ�������Ӧ����
int NPC_Lua_SQL_PushPopAdv(lua_State *_NLL);            //��SAAC����һ��SQL��ѯ���󣬲�ָ����SAAC���ش�����ʱ�Խ�����д���Ļص�����,�ú��������ص�һ�н�������ͷŽ������
int NPC_Lua_SQL_QueryFirstRow(lua_State *_NLL);         //�ص�����,�ú��������ص�һ�н�������ͷŽ������
int NPC_Lua_SQL_FetchRow(lua_State *_NLL);              //��һ�����
int NPC_Lua_SQL_FreeResult(lua_State *_NLL);            //�ͷŽ����
///////////////////////////////////////////////SQL///////////////////////////////////////////////
#define LRet(r) \
{ \
	NPC_Lua_SetErrorStr(NULL); \
	return r; \
}

#define LRetErr(c,r) \
{ \
	NPC_Lua_SetErrorStr(c); \
	return r; \
}

//////////////////////////////////////////////////////////////////////
//����һ��nil
#define LRetNull(L) \
{ \
	lua_pushnil(L); \
	LRet(1); \
}

//����һ��BOOL��LUA����
#define LRetBool(L, b) \
{ \
	lua_pushboolean(L, b); \
	LRet(1); \
}

//����һ��int��LUA����
#define LRetInt(L, i) \
{ \
	lua_pushinteger(L, i); \
	LRet(1); \
}

#define LRetMsg(L, c) \
{ \
	lua_pushstring(L, c); \
	LRet(1); \
}
////////////////////////////////////////////////////////////////////
//����һ������״̬��һ������Ϣ
#define LRetErrInt(L, i, c) \
{ \
	lua_pushinteger(L, i); \
	LRetErr(c,1); \
}

//����һ���ַ�����LUA����
#define LRetErrNull(L, c) \
{ \
	lua_pushnil(L); \
	LRetErr(c,1); \
}

//����һ���ַ�����LUA����
#define LRetErrMsg(L, c) \
{ \
	lua_pushstring(L, c); \
	LRetErr(c,1); \
}

//���ڼ������Ƿ��㹻
#define CheckEx(L, n) \
{	\
	if(lua_gettop(L) != n) \
	{ \
		luaL_argerror(L, 1, "������������"); \
	} \
}

#define CheckEx2(L, t, n) \
{	\
	if(lua_gettop(L) < (t) || lua_gettop(L) > n) \
	{ \
		luaL_argerror(L, 1, "������������"); \
	} \
}

#define CheckIndexNull(L, n) \
{ \
	if(lua_isnil((L), (n))) \
	{ \
		luaL_argerror((L), 1, "������������Ϊnil"); \
	} \
}

#define CheckBattleIndexNull(L, n) \
{ \
	if(lua_isnil((L), (n))) \
	{ \
		luaL_argerror((L), 1, "ս����������Ϊnil"); \
	} \
}

#define CheckItemIndexNull(L, n) \
{ \
	if(lua_isnil((L), (n))) \
	{ \
		luaL_argerror((L), 1, "������������Ϊnil"); \
	} \
}

#define CheckObjIndexNull(L, n) \
{ \
	if(lua_isnil((L), (n))) \
	{ \
		luaL_argerror((L), 1, "�����������Ϊnil"); \
	} \
}

#define LRetArray(L, Array, num) \
{ \
	Lua_ReturnArray(L, Array, num); \
	LRet(1); \
}

#ifdef _LUA_Debug
int pcall_callback_err_fun(lua_State* L,const char *_InitFuncName); //����LUA�ű�������Ϣ
#endif

#endif //#ifndef __NPC_LUA__
