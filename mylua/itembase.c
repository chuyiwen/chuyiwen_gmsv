#include <string.h>
#include "common.h"
#include "char_base.h"
#include "mylua/base.h"
#include "char.h"
#include "item.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lssproto_serv.h"

#ifdef _ALLBLUES_LUA_1_2   
#include "mylua/mylua.h"
extern MY_Lua MYLua;
static CharBase ItemBaseInt[] = {
	{{"���"},							ITEM_ID}
	,{{"ͼ��"},							ITEM_BASEIMAGENUMBER}
	,{{"��ֵ"},							ITEM_COST}
	,{{"����"},							ITEM_TYPE}
	,{{"ʹ��"},							ITEM_ABLEUSEFIELD}
	,{{"Ŀ��"},							ITEM_TARGET}
	,{{"�ȼ�"},							ITEM_LEVEL}
	,{{"����"},							ITEM_DAMAGEBREAK}
	,{{"�ѵ�"},							ITEM_USEPILENUMS}
	,{{"�ص�"},							ITEM_CANBEPILE}
	,{{"�蹥"},							ITEM_NEEDSTR}
	,{{"����"},							ITEM_NEEDDEX}
	,{{"��ת"},							ITEM_NEEDTRANS}
	,{{"��ְҵ"},						ITEM_NEEDPROFESSION}
#ifdef _TAKE_ITEMDAMAGE
	,{{"��С��"},						ITEM_DAMAGECRUSHE}
	,{{"����"},						ITEM_MAXDAMAGECRUSHE}
#endif
	,{{"��"},								ITEM_OTHERDAMAGE}
	,{{"��"},								ITEM_OTHERDEFC}
	,{{"��װ"},							ITEM_SUITCODE}
	,{{"��С����"},					ITEM_ATTACKNUM_MIN}
	,{{"��󹥻�"},					ITEM_ATTACKNUM_MAX}
	,{{"��"},								ITEM_MODIFYATTACK}
	,{{"��"},								ITEM_MODIFYDEFENCE}
	,{{"��"},								ITEM_MODIFYQUICK}
	,{{"HP"},								ITEM_MODIFYHP}
	,{{"MP"},								ITEM_MODIFYMP}
	,{{"����"},							ITEM_MODIFYLUCK}
	,{{"����"},							ITEM_MODIFYCHARM}
	,{{"�ر�"},							ITEM_MODIFYAVOID}
	,{{"����"},							ITEM_MODIFYATTRIB}
	,{{"���Ա���"},					ITEM_MODIFYATTRIBVALUE}
	,{{"����"},							ITEM_MAGICID}
	,{{"�о���"},						ITEM_MAGICPROB}
	,{{"����MP"},						ITEM_MAGICUSEMP}
	,{{"��"},							ITEM_MODIFYARRANGE}
	,{{"����"},							ITEM_MODIFYSEQUENCE}
	,{{"����"},							ITEM_ATTACHPILE}
	,{{"����"},							ITEM_HITRIGHT}
	,{{"����"},							ITEM_NEGLECTGUARD}
	,{{"����"},							ITEM_POISON}
	,{{"����"},							ITEM_PARALYSIS}
	,{{"˯��"},							ITEM_SLEEP}
	,{{"ʯ��"},							ITEM_STONE}
	,{{"����"},							ITEM_DRUNK}
	,{{"����"},							ITEM_CONFUSION}
	,{{"����"},							ITEM_CRITICAL}
	,{{"����"},							ITEM_USEACTION}
	,{{"�ǳ���ʧ"},					ITEM_DROPATLOGOUT}
	,{{"������ʧ"},					ITEM_VANISHATDROP}
	,{{""},									ITEM_ISOVERED}
	,{{"�ʼ�"},							ITEM_CANPETMAIL}
	,{{"�ϳɴ�"},						ITEM_CANMERGEFROM}
	,{{"�ϳ���"},						ITEM_CANMERGETO}
	,{{"����0"},						ITEM_INGVALUE0}
	,{{"����1"},						ITEM_INGVALUE1}
	,{{"����2"},						ITEM_INGVALUE2}
	,{{"����3"},						ITEM_INGVALUE3}
	,{{"����4"},						ITEM_INGVALUE4}
	,{{"��ɫ"},							ITEM_COLOER}
	,{{"��Ʒ�ȼ�"},					ITEM_LEAKLEVEL}
	
}; 

static CharBase ItemBaseChar[] = {
	{{"����"},					ITEM_NAME}
	,{{"��ʾ��"},				ITEM_SECRETNAME}
	,{{"˵��"},					ITEM_EFFECTSTRING}
	,{{"�ֶ�"},					ITEM_ARGUMENT}
	,{{"�ɷ���0"},			ITEM_INGNAME0}
	,{{"�ɷ���1"},			ITEM_INGNAME1}
	,{{"�ɷ���2"},			ITEM_INGNAME2}
	,{{"�ɷ���3"},			ITEM_INGNAME3}
	,{{"�ɷ���4"},			ITEM_INGNAME4}
#ifdef _ANGEL_SUMMON
	,{{"����"},					ITEM_ANGELMISSION}
	,{{"��ʹ"},					ITEM_ANGELINFO}
	,{{"Ӣ��"},					ITEM_HEROINFO}
#endif

}; 

static CharBase ItemBaseValue[] = {
	{{"צ"},							ITEM_FIST}
	,{{"��"},							ITEM_AXE}
	,{{"��"},							ITEM_CLUB}
	,{{"ǹ"},							ITEM_SPEAR}
	,{{"��"},							ITEM_BOW}
	,{{"��"},							ITEM_SHIELD}
	,{{"��"},							ITEM_HELM}
	,{{"��"},							ITEM_ARMOUR}
	,{{"��"},							ITEM_BRACELET}
	,{{"��"},							ITEM_MUSIC}
	,{{"��"},							ITEM_NECKLACE}
	,{{"��"},							ITEM_RING}
	,{{"��"},							ITEM_BELT}
	,{{"����"},						ITEM_EARRING}
	,{{"�ǻ�"},						ITEM_NOSERING}
	,{{"�����"},					ITEM_AMULET}
	,{{"����"},						ITEM_OTHER}
	,{{"������"},					ITEM_BOOMERANG}
	,{{"Ͷ����"},					ITEM_BOUNDTHROW}
	,{{"Ͷ��ʯ"},					ITEM_BREAKTHROW}
	,{{"��"},							ITEM_DISH}
#ifdef _ITEM_INSLAY
	,{{"����"},						ITEM_METAL}
	,{{"��ʯ"},						ITEM_JEWEL}
#endif
#ifdef _ITEM_CHECKWARES
	,{{"����"},						ITEM_WARES}
#endif
#ifdef _ITEM_EQUITSPACE
	,{{"����"},						ITEM_WBELT}
	,{{"����"},						ITEM_WSHIELD}
	,{{"Ь��"},						ITEM_WSHOES}
#endif
#ifdef _EQUIT_NEWGLOVE 
	,{{"����"},						ITEM_WGLOVE}
#endif
#ifdef _ALCHEMIST
	,{{"������"},					ITEM_ALCHEMIST}
#endif
#ifdef _PET_ITEM
	,{{"��ͷ"},						ITEM_PET_HEAD}
	,{{"����"},						ITEM_PET_TOOTH} 
	,{{"��צ"},						ITEM_PET_CLAW}
	,{{"����"},						ITEM_PET_BREAST}
	,{{"�豳"},						ITEM_PET_BACK}
	,{{"���"},						ITEM_PET_WING}
	,{{"���"},						ITEM_PET_FEET}
#endif
}; 

static CharBase ItemBaseWorkInt[] = {
	{{"����"}, 						ITEM_WORKOBJINDEX}
	,{{"���"}, 					ITEM_WORKCHARAINDEX}
}; 

static CharBase ItemBaseEvent[] = {
	{{"δ֪�¼�"}, 				ITEM_PREOVERFUNC}
	,{{"�ʼ��¼�"}, 			ITEM_POSTOVERFUNC}
	,{{"ʹ���¼�"}, 			ITEM_USEFUNC}
	,{{"װ���¼�"}, 			ITEM_ATTACHFUNC}
	,{{"ж���¼�"}, 			ITEM_DETACHFUNC}
	,{{"�����¼�"}, 			ITEM_DROPFUNC}
	,{{"�����¼�"}, 			ITEM_PICKUPFUNC}
	,{{"�����¼�"}, 			ITEM_DIERELIFEFUNC}
}; 

static int getInt(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, ItemBaseInt, arraysizeof(ItemBaseInt));

	lua_pushinteger(L, ITEM_getInt(index, element));
	return 1;
}

static int setInt(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, ItemBaseInt, arraysizeof(ItemBaseInt));
	const int data = luaL_checkint(L, 3);
	
  if(ITEM_setInt(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getChar(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, ItemBaseChar, arraysizeof(ItemBaseChar));

	lua_pushstring(L, ITEM_getChar(index, element));
	return 1;
}

static int setChar(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, ItemBaseChar, arraysizeof(ItemBaseChar));
	char *data = luaL_checklstring(L, 3, &l);
	
  if(ITEM_setChar(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getWorkInt(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, ItemBaseWorkInt, arraysizeof(ItemBaseWorkInt));

	lua_pushinteger(L, ITEM_getWorkInt(index, element));
	return 1;
}

static int setWorkInt(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, ItemBaseWorkInt, arraysizeof(ItemBaseWorkInt));
	const int data = luaL_checkint(L, 3);
	
  if(ITEM_setWorkInt(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getNameFromNumber(lua_State *L) 
{
	const int id = luaL_checkint(L, 1);
	
	lua_pushstring(L, ITEM_getNameFromNumber(id));
	
	return 1;
}

static int getcostFromITEMtabl(lua_State *L) 
{
	const int id = luaL_checkint(L, 1);
	
	lua_pushinteger(L, ITEM_getcostFromITEMtabl(id));
	
	return 1;
}

static int getlevelFromITEMtabl(lua_State *L) 
{
	const int id = luaL_checkint(L, 1);
	
	lua_pushinteger(L, ITEM_getlevelFromITEMtabl(id));
	
	return 1;
}

static int getgraNoFromITEMtabl(lua_State *L) 
{
	const int id = luaL_checkint(L, 1);
	
	lua_pushinteger(L, ITEM_getgraNoFromITEMtabl(id));
	
	return 1;
}

static int getItemInfoFromNumber(lua_State *L) 
{
	const int id = luaL_checkint(L, 1);
	
	lua_pushstring(L, ITEM_getItemInfoFromNumber(id));
	
	return 1;
}

static int setFunctionPointer(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
  const int functype = getCharBaseValue(L, 2, ItemBaseEvent, arraysizeof(ItemBaseEvent));
  char *luafuncname=luaL_checklstring(L, 3, &l);

	return ITEM_setLUAFunction( index, functype, luafuncname );
}

static int addLUAListFunction(lua_State *L) 
{
	size_t l;
	char *luafuncname=luaL_checklstring(L, 1, &l);
  char *luafunctable=luaL_checklstring(L, 2, &l);
	char *luafunctablepath=luaL_checklstring(L, 3, &l);

	if(strlen(luafunctablepath) > 0){
		MY_Lua *mylua = &MYLua;
	  while(mylua->next != NULL){
	  	if(strcmp(mylua->luapath, luafunctablepath) == 0){
	  		return ITEM_addLUAListFunction( mylua->lua, luafuncname, luafunctable);
	  	}
	  	mylua = mylua->next;
	  }
	}else{
		return ITEM_addLUAListFunction(  L, luafuncname, luafunctable );
	}
	return 1;
}

static int UpdataItemOne(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int itemindex = luaL_checkint(L, 2);

	int i;
	for( i = 0; i < CHAR_MAXITEMHAVE ; i++ ){
		if(itemindex == CHAR_getItemIndex( charaindex , i )){
			CHAR_sendItemDataOne( charaindex, i);
			break;
		}
	}

	return 1;
}

static int UpdataHaveItemOne(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int haveitemindex = luaL_checkint(L, 2);

	CHAR_sendItemDataOne( charaindex, haveitemindex);

	return 1;
}

static const luaL_Reg itemlib[] = {
	{"getInt", 									getInt},
	{"setInt", 									setInt},
	{"getChar", 								getChar},
	{"setChar", 								setChar},
	{"getWorkInt", 							getWorkInt},
	{"setWorkInt", 							setWorkInt},
	{"setFunctionPointer", 			setFunctionPointer},
	{"addLUAListFunction", 			addLUAListFunction},
	{"getNameFromNumber", 			getNameFromNumber},
	{"getcostFromITEMtabl", 		getcostFromITEMtabl},
	{"getlevelFromITEMtabl", 		getlevelFromITEMtabl},
	{"getgraNoFromITEMtabl", 		getgraNoFromITEMtabl},
	{"getItemInfoFromNumber", 	getItemInfoFromNumber},
	{"UpdataItemOne", 					UpdataItemOne},
	{"UpdataHaveItemOne", 			UpdataHaveItemOne},
  {NULL, 						NULL}
};

LUALIB_API int luaopen_Item (lua_State *L) {
  luaL_register(L, "item", itemlib);
  return 1;
}

#endif

