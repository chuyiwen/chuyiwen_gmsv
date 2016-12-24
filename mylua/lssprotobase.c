#include <string.h>
#include "char.h"
#include "char_base.h"
#include "mylua/base.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lssproto_serv.h"
#ifdef _ALLBLUES_LUA_2_0
#include "autil.h"
#endif
#ifdef _ALLBLUES_LUA   


#define		WINDOW_BUTTONTYPE_NONE		(0)
#define		WINDOW_BUTTONTYPE_OK		(1 << 0)
#define		WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
#define		WINDOW_BUTTONTYPE_YES		(1 << 2)
#define		WINDOW_BUTTONTYPE_NO		(1 << 3)
#define		WINDOW_BUTTONTYPE_PREV		(1 << 4)
#define		WINDOW_BUTTONTYPE_NEXT		(1 << 5)

#define		WINDOW_BUTTONTYPE_OKCANCEL	(WINDOW_BUTTONTYPE_OK | WINDOW_BUTTONTYPE_CANCEL)
#define		WINDOW_BUTTONTYPE_YESNO	(WINDOW_BUTTONTYPE_YES | WINDOW_BUTTONTYPE_NO)


static CharBase CharBaseWindow[] = {
	{{"对话框"}, 					WINDOW_MESSAGETYPE_MESSAGE}
	,{{"输入框"}, 				WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT}
	,{{"选择框"},					WINDOW_MESSAGETYPE_SELECT}
	,{{"宠物框"}, 				WINDOW_MESSAGETYPE_PETSELECT}
	,{{"卖道具框"}, 			WINDOW_MESSAGETYPE_ITEMSHOPMENU}
	,{{"买道具框"}, 			WINDOW_MESSAGETYPE_ITEMSHOPMAIN}
	,{{"学技能框"}, 				WINDOW_MESSAGETYPE_PETSKILLSHOP}
#ifdef _NEW_MANOR_LAW
	,{{"十大气势家族"}, 	WINDOW_FMMESSAGETYPE_10_MEMONTUM}
	,{{"家族气势列表"}, 	WINDOW_FMMESSAGETYPE_FM_MEMONTUM}
#endif
	,{{"家族成员列表"}, 	WINDOW_FMMESSAGETYPE_DENGON}
	,{{"庄园列表"}, 			WINDOW_FMMESSAGETYPE_POINTLIST}
	,{{"前三大列表"}, 		WINDOW_FMMESSAGETYPE_TOP30DP}
	,{{"银行框"}, 				WINDOW_MESSAGETYPE_BANK}
	,{{"宽对话框"}, 			WINDOW_MESSAGETYPE_WIDEMESSAGE}
	,{{"宽输入框"}, 			WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT}
	
}; 

static CharBase CharBaseButton[] = {
	{{"无按钮"}, 					WINDOW_BUTTONTYPE_NONE}
	,{{"确定"}, 					WINDOW_BUTTONTYPE_OK}
	,{{"取消"},						WINDOW_BUTTONTYPE_CANCEL}
	,{{"YES"}, 						WINDOW_BUTTONTYPE_YES}      
	,{{"NO"}, 						WINDOW_BUTTONTYPE_NO}
	,{{"上一页"}, 				WINDOW_BUTTONTYPE_PREV|WINDOW_BUTTONTYPE_CANCEL}
	,{{"下一页"}, 				WINDOW_BUTTONTYPE_NEXT|WINDOW_BUTTONTYPE_CANCEL}
}; 

static CharBase CharBaseSeqNo[] = {
	{{""}, 								-1}
#ifdef _NEW_STREET_VENDOR
	,{{"摆摊类型"}, 			CHAR_WINDOWTYPE_STREET_VENDOR_TYPE}
#endif
#ifdef _PETSKILL_CANNEDFOOD
	,{{"宠物技能选择"}, 	ITEM_WINDOWTYPE_SELECTPETSKILL_SELECT}
#endif
#ifdef _ITEM_OVER_LAP
	,{{"道具重叠"}, 			CHAR_WINDOWTYPE_ITEMOVERLAP}
#endif	
}; 


static int windows_send(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	if(CHAR_CHECKINDEX(index) == FALSE) return 0;
	int fd = getfdFromCharaIndex( index);
  const int windowtype = getCharBaseValue(L, 2, CharBaseWindow, arraysizeof(CharBaseWindow));
  const int buttontype = getCharBaseValue(L, 3, CharBaseButton, arraysizeof(CharBaseButton));
  const int seqno = getCharBaseValue(L, 4, CharBaseSeqNo, arraysizeof(CharBaseSeqNo));
  const int objindex=luaL_checkint(L, 5);
  char *data=luaL_checklstring(L, 6, &l);


	lssproto_WN_send(fd, windowtype, buttontype, seqno, objindex, data);
	return 1;
}

static int show(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	if(CHAR_CHECKINDEX(index) == FALSE) return 0;
	int fd = getfdFromCharaIndex( index);
  char *data=luaL_checklstring(L, 2, &l);


	lssproto_C_send(fd, data);
	return 1;
}

static int FM(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	if(CHAR_CHECKINDEX(index) == FALSE) return 0;
	int fd = getfdFromCharaIndex( index);
  char *data=luaL_checklstring(L, 2, &l);
  
	lssproto_FM_send(fd, data);
	return 1;
}

static int S2(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	if(CHAR_CHECKINDEX(index) == FALSE) return 0;
	int fd = getfdFromCharaIndex( index);
  char *data=luaL_checklstring(L, 2, &l);
  
	lssproto_S2_send(fd, data);
	return 1;
}

#ifdef _ALLBLUES_LUA_2_0
/*
static int CharList(lua_State *L) 
{
	int fd = luaL_checkint(L, 1);
	size_t l;
	char *data=luaL_checklstring(L, 2, &l);
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");
	checksum += util_mkstring(fd,buffer, "failed");
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	printf("data = %s\n",data);
	util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);
	return 1;
}
*/
static int CharList(lua_State *L) 
{
	size_t l;
	const int fd = luaL_checkint(L, 1);
  char *data=luaL_checklstring(L, 2, &l);

	lssproto_CharList_send(fd, FAILED, data);
	return 1;
}

#ifdef _CAX_DENGON_GG
static int dengon(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	if(CHAR_CHECKINDEX(index) == FALSE) return 0;
	int fd = getfdFromCharaIndex( index);
  char *data=luaL_checklstring(L, 2, &l);
  const int color=luaL_checkint(L, 3);
  const int num=luaL_checkint(L, 4);
  
	lssproto_DENGON_send(fd, data, color, num);
	return 1;
}
#endif

#endif




static const luaL_Reg lssprotolib[] = {
	{"windows", 			windows_send},
	{"show", 					show},
	{"FM", 						FM},
	{"S2", 						S2},
#ifdef _ALLBLUES_LUA_2_0
	{"CharList",           CharList},
#ifdef _CAX_DENGON_GG
  {"dengon", 				dengon},
#endif
#endif
  {NULL, 						NULL}
};

LUALIB_API int luaopen_Lssproto (lua_State *L) {
  luaL_register(L, "lssproto", lssprotolib);
  return 1;
}

#endif

