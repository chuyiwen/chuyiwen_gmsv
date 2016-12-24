#include "npc_lua.h"
#include "util.h"
#include "char.h"
#include "char_base.h"
#include "anim_tbl.h"
#include "object.h"
#include "net.h"
#include "npcutil.h"
#include "npc_eventaction.h"
#include "battle.h"
#include "readmap.h"
#include <stdio.h>
#include <string.h>
#include "autil.h"
#include "configfile.h"
#ifdef _JZ_NEWSCRIPT_LUA

extern char M_OutErrMsg[1024];
extern lua_State *M_Script_Lua;
extern unsigned int M_Create_Num;
static char Buff_Data[1024*64];
static int Check_Num=0;


//////////////////////////////////////////////////////////////////////////////
int NPC_Lua_NL_GetErrorStr(lua_State *_NLL)
{
	CheckEx(_NLL, 0);

	LRetMsg(_NLL, M_OutErrMsg);
}

BOOL NPC_Lua_NL_Mod(lua_State *_NLL)
{
	CheckEx2(_NLL, 1, 2);
	int num = (int)lua_tointeger(_NLL, 1);
	int num2 = 2;
	int TM_Top = lua_gettop(_NLL);
	if (TM_Top == 2)
	{
		int num2 = (int)lua_tointeger(_NLL, 2);
	}
	if(num%num2==0)
		{
			LRetBool(_NLL,TRUE);
		}
	else
		{
			LRetBool(_NLL,FALSE);
		}
}

BOOL NPC_Lua_CheckNpcEventFree(lua_State *_NLL)
{
	CheckEx(_NLL, 3);
	char *TM_NpcStr = lua_tostring(_NLL, 3);
	int TM_NPCindex = (int)lua_tointeger(_NLL, 1);
	int TM_index = (int)lua_tointeger(_NLL, 2);
	char magicname[256];
	int i = 1;
	BOOL ret;
	BOOL ret1;
	while (TRUE) {
		 ret = getStringFromIndexWithDelim( TM_NpcStr, "|", i, magicname,  sizeof( magicname));
		 if (!ret) 
		 {
		 	ret = getStringFromIndexWithDelim( TM_NpcStr, "/", i, magicname,  sizeof( magicname));
		 	if (!ret) 
		 	{
		 		ret = getStringFromIndexWithDelim( TM_NpcStr, "\\", i, magicname,  sizeof( magicname));
		 		if (!ret)
		 		{
		 			break;
		 		}
		 	}
		 }
		 ret1 = ActionNpc_CheckFree(TM_NPCindex,TM_index,magicname);
		 if (!ret1) 
		 {
		 		break;
		 }
		 i +=1;
	}
	LRetBool(_NLL, ret1);
}

BOOL NPC_Lua_DoNpcEventAction(lua_State *_NLL)
{
	CheckEx(_NLL, 3);
	char *TM_NpcStr = lua_tostring(_NLL, 3);
	int TM_NPCindex = (int)lua_tointeger(_NLL, 1);
	int TM_index = (int)lua_tointeger(_NLL, 2);
	char magicname[256];
	int i = 1;
	BOOL ret;
	BOOL ret1;
	while (TRUE) {
		 ret = getStringFromIndexWithDelim( TM_NpcStr, "|", i, magicname,  sizeof( magicname));
		 if (!ret) 
		 {
		 	ret = getStringFromIndexWithDelim( TM_NpcStr, "/", i, magicname,  sizeof( magicname));
		 	if (!ret) 
		 	{
		 		ret = getStringFromIndexWithDelim( TM_NpcStr, "\\", i, magicname,  sizeof( magicname));
		 		if (!ret)
		 		{
		 			break;
		 		}
		 	}
		 }
		 ret1 = Action_RunDoEventAction(TM_NPCindex,TM_index,magicname);
		 i +=1;
	}
	LRetBool(_NLL, ret1);
}


int NPC_Lua_NL_CreateNpc(lua_State *_NLL)
{
	//参数有4个
	CheckEx2(_NLL, 2, 4);
	char *TM_DoFile = lua_tostring(_NLL, 1);
	char *TM_InitFuncName = lua_tostring(_NLL, 2);
	BOOL TM_IsFly = FALSE;
	char *TM_seek = NULL;
	int TM_Top = lua_gettop(_NLL);

	if(TM_Top >= 3)
	{
		TM_IsFly = (BOOL)lua_toboolean(_NLL, 3);
	}
	if(TM_Top == 4)
	{
		TM_seek = lua_tostring(_NLL, 4);
	}

	int TM_Ret = NPC_Lua_Create(TM_DoFile, TM_InitFuncName, TM_seek, TM_IsFly);

	LRet(TM_Ret);
}


int NPC_Lua_NL_DelNpc(lua_State *_NLL)
{
	CheckEx(_NLL, 1);
	CheckIndexNull(_NLL, 1);
	int TM_index = (int)lua_tointeger(_NLL, 1);
	int TM_Ret = NPC_Lua_Del(TM_index);
	LRetInt(_NLL, TM_Ret);
}

int NPC_Lua_NL_GetStringFromIndexWithDelim(lua_State *_NLL)
{
	CheckEx(_NLL, 3);
	CheckIndexNull(_NLL, 1);
	CheckIndexNull(_NLL, 2);
	char* string = (char*)lua_tostring(_NLL, 1);
	char* delim = (char*)lua_tostring(_NLL, 2);
	int index = (int)lua_tointeger(_NLL, 3);
	char token[128];
	getStringFromIndexWithDelim( string,delim,index,token,sizeof(token));
	LRetMsg(_NLL,token);
}

BOOL NPC_Lua_NL_ANSI_PlayerLoop(lua_State *_NLL)
{
	CheckEx2(_NLL,0, 1);
	int i;
	static int indexn = 0;
	int TM_Top = lua_gettop(_NLL);
	BOOL TM_IsFly = FALSE;
	if(TM_Top >= 1)
	{
		TM_IsFly = (BOOL)lua_toboolean(_NLL, 1);	
	}
	if (TM_IsFly){
		indexn = 0;
		LRetBool(_NLL, TRUE);
		return;
	}
	int playernum = CHAR_getPlayerMaxNum();
	for( i = indexn ; i < playernum ; i++) {
		if( CHAR_CHECKINDEX(i) ){
			indexn++;
			LRetInt(_NLL, i);
			break;
		}
	}
	LRetInt(_NLL, -1);
}

BOOL NPC_Lua_NL_ANSI_PetLoop(lua_State *_NLL)
{
	CheckEx2(_NLL,0, 1);
	int i;
	int MAXPLAYER = CHAR_getPlayerMaxNum();
	int MAXPET = CHAR_getPetMaxNum();
	static int indexn = 0;
	int TM_Top = lua_gettop(_NLL);
	BOOL TM_IsFly = FALSE;
	if(TM_Top >= 1)
	{
		TM_IsFly = (BOOL)lua_toboolean(_NLL, 1);	
	}
	if (TM_IsFly){
		indexn = MAXPLAYER;
		LRetBool(_NLL, TRUE);
		return;
	}

	for( i = indexn ; i < MAXPLAYER+MAXPET ; i++) {
		if( CHAR_CHECKINDEX(i) ){
			indexn++;
			LRetInt(_NLL, i);
			break;
		}
	}
	LRetInt(_NLL, -1);
}



BOOL NPC_Lua_NL_ANSI_ItemLoop(lua_State *_NLL)
{
	CheckEx2(_NLL,0, 1);
	int i;
	static int indexn = 0;
	int TM_Top = lua_gettop(_NLL);
	BOOL TM_IsFly = FALSE;
	if(TM_Top >= 1)
	{
		TM_IsFly = (BOOL)lua_toboolean(_NLL, 1);	
	}
	if (TM_IsFly){
		indexn = 0;
		LRetBool(_NLL, TRUE);
		return;
	}
	int itemnum = ITEM_getITEM_itemnum();
	for( i = indexn ; i < itemnum ; i++) {
		if( ITEM_CHECKINDEX(i) ){
			indexn++;
			LRetInt(_NLL, i);
			break;
		}
	}
	LRetInt(_NLL, -1);
}

BOOL NPC_Lua_NL_PetLoopGetNext(lua_State *_NLL)
{
	int MAXPLAYER = CHAR_getPlayerMaxNum();
	int MAXPET = CHAR_getPetMaxNum();

	CheckEx(_NLL, 1);
	int i;
	char *TM_FuncName = lua_tostring(_NLL, 1);
	char TM_RetBuff[128];

	for( i = MAXPLAYER ; i < MAXPLAYER+MAXPET ; i++) {
		if( CHAR_CHECKINDEX(i) ){
			NPC_Lua_CallFunc(TM_FuncName, &TM_RetBuff, sizeof(TM_RetBuff), i);
		}
	}
	LRetBool(_NLL, TRUE);

}

int NPC_Lua_NL_ItemLoopGetNext(lua_State *_NLL)
{
	int itemnum = ITEM_getITEM_itemnum();
	CheckEx(_NLL, 1);
	int i;
	char *TM_FuncName = lua_tostring(_NLL, 1);
	char TM_RetBuff[128];

	for( i = 0 ; i < itemnum ; i++) {
		if( ITEM_CHECKINDEX(i) ){
			NPC_Lua_CallFunc(TM_FuncName, &TM_RetBuff, sizeof(TM_RetBuff), i);
		}
	}
	LRetBool(_NLL, TRUE);

}

BOOL NPC_Lua_NL_PlayerLoopGetNext(lua_State *_NLL)
{
	CheckEx(_NLL, 1);
	int i;
	char *TM_FuncName = lua_tostring(_NLL, 1);
	char TM_RetBuff[128];
	int playernum = CHAR_getPlayerMaxNum();
	for( i = 0 ; i < playernum ; i++) {
		if( CHAR_CHECKINDEX(i) ){
			NPC_Lua_CallFunc(TM_FuncName, &TM_RetBuff, sizeof(TM_RetBuff), i);
		}
	}
	LRetBool(_NLL, TRUE);
}

int NPC_Lua_NL_GetConfigLineType(lua_State *_NLL)
{
	CheckEx(_NLL, 1);
	int i;
	char *TM_ConfigName = lua_tostring(_NLL, 1);
	int TM_Type = GetConfigLineType(TM_ConfigName);
	LRetInt(_NLL, TM_Type);
}

int NPC_Lua_NL_GetConfigLineVal(lua_State *_NLL)
{
	CheckEx(_NLL, 1);
	int i;
	char *TM_ConfigName = lua_tostring(_NLL, 1);
	char *TM_RETMSG = GetConfigLineVal(TM_ConfigName);
	LRetMsg(_NLL, TM_RETMSG);
}

int NPC_Lua_NL_SetTimer(lua_State *_NLL)
{
	CheckEx(_NLL, 3);
	int i;
	char *TM_FileName = lua_tostring(_NLL, 1);
	char *TM_FuncName = lua_tostring(_NLL, 2);
	unsigned int EspTime = (int)lua_tointeger(_NLL, 3);
	int Timer_s = SetTimer_net(TM_FileName,TM_FuncName,EspTime);
	LRetInt(_NLL, Timer_s);
}

int NPC_Lua_NL_DelTimer(lua_State *_NLL)
{
	CheckEx(_NLL, 1);
	CheckIndexNull(_NLL, 1);
	int ID = (int)lua_tointeger(_NLL, 1);
	BOOL TM_Ret = DelTimer_net(ID);
	LRetBool(_NLL, TM_Ret);
}

int NPC_Lua_NL_RunSaFuncII(lua_State *_NLL)
{
	CheckEx(_NLL, 3);
	CheckIndexNull(_NLL, 1);
	char *TM_FuncName = lua_tostring(_NLL, 1);
	int i1 = (int)lua_tointeger(_NLL, 2);
	int i2 = (int)lua_tointeger(_NLL, 3);
	typedef void (*GMSVFUNC)(int,int);
	GMSVFUNC atf;
	atf=(GMSVFUNC)(void*)TM_FuncName;
	if( atf )
		atf( i1,i2 );
}

int NPC_Lua_NL_RunSaFuncIII(lua_State *_NLL)
{
	CheckEx(_NLL, 4);
	CheckIndexNull(_NLL, 1);
	char *TM_FuncName = lua_tostring(_NLL, 1);
	int i1 = (int)lua_tointeger(_NLL, 2);
	int i2 = (int)lua_tointeger(_NLL, 3);
	int i3 = (int)lua_tointeger(_NLL, 3);
	typedef void (*GMSVFUNC)(int,int,int);
	GMSVFUNC atf;
	atf=(GMSVFUNC)(void*) TM_FuncName;
	if( atf )
		atf( i1,i2,i3);
}

int NPC_Lua_NL_ClsMk(lua_State *_NLL)
{
	Check_Num=0;
	memset(Buff_Data,0,sizeof(Buff_Data));
	LRetInt(_NLL, 1);
}

int NPC_Lua_NL_AddCheckNum(lua_State *_NLL)
{

	CheckEx(_NLL,1);
	CheckIndexNull(_NLL, 1);
	int i = (int)lua_tointeger(_NLL, 1);
	Check_Num += i;
	LRetInt(_NLL, Check_Num);
}


int NPC_Lua_NL_Mkstring(lua_State *_NLL)
{

	CheckEx(_NLL, 2);
	CheckIndexNull(_NLL, 1);
	int TM_index = (int)lua_tointeger(_NLL, 1);
	int fd = getfdFromCharaIndex(TM_index );
	char *TM_data = lua_tostring(_NLL, 2);
	int checknum=0;
	checknum = util_mkstring(fd,Buff_Data, TM_data);
	LRetInt(_NLL, checknum);
}

int NPC_Lua_NL_Mkint(lua_State *_NLL)
{

	CheckEx(_NLL,2);
	CheckIndexNull(_NLL, 1);
	int TM_index = (int)lua_tointeger(_NLL, 1);
	int fd = getfdFromCharaIndex(TM_index );
	int i = (int)lua_tointeger(_NLL, 2);
	int checknum=util_mkint(fd,Buff_Data, i);
	LRetInt(_NLL, checknum);
}

int NPC_Lua_NL_SendMesg(lua_State *_NLL)
{

	CheckEx(_NLL,2);
	CheckIndexNull(_NLL, 1);
	int charaindex = (int)lua_tointeger(_NLL, 1);
	int fengbaohao = (int)lua_tointeger(_NLL, 2);
	int fd = getfdFromCharaIndex(charaindex );
	util_SendMesg(fd, fengbaohao, Buff_Data);
	LRetInt(_NLL, 1);

}

#endif //#ifdef _JZ_NEWSCRIPT_LUA

