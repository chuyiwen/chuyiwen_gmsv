#include <string.h>
#include "common.h"
#include "version.h"
#include "pet_skill.h"
#include "mylua/base.h"
#include "char.h"
#include "item.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lssproto_serv.h"

#ifdef _ALLBLUES_LUA_1_8 
#include "mylua/mylua.h"
extern MY_Lua MYLua;
static CharBase PetSkillBaseInt[] = {
	{{"���"},							PETSKILL_ID}
	,{{"�ֶ�"},							PETSKILL_FIELD}
	,{{"Ŀ��"},							PETSKILL_TARGET}
#ifdef _PETSKILL2_TXT
	,{{"����"},							PETSKILL_USETYPE}
#endif
	,{{"��ֵ"},							PETSKILL_COST}
	,{{"�Ƿ�"},							PETSKILL_ILLEGAL}
}; 

static CharBase PetSkillBaseChar[] = {
	{{"����"},					PETSKILL_NAME}
	,{{"ע��"},					PETSKILL_COMMENT}
	,{{"����"},					PETSKILL_FUNCNAME}
	,{{"ѡ��"},					PETSKILL_OPTION}
#ifdef _CFREE_petskill
	,{{"����"},					PETSKILL_FREE}
	,{{"����"},					PETSKILL_KINDCODE}
#endif
}; 

static int getInt(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseInt, arraysizeof(PetSkillBaseInt));

	lua_pushinteger(L, PETSKILL_getInt(index, element));
	return 1;
}

static int setInt(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseInt, arraysizeof(PetSkillBaseInt));
	const int data = luaL_checkint(L, 3);
	
  if(PETSKILL_setInt(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int getChar(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseChar, arraysizeof(PetSkillBaseChar));

	lua_pushstring(L, PETSKILL_getChar(index, element));
	return 1;
}

static int setChar(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	const int element = getCharBaseValue(L, 2, PetSkillBaseChar, arraysizeof(PetSkillBaseChar));
	char *data = luaL_checklstring(L, 3, &l);
	
  if(PETSKILL_setChar(index, element, data) == -1){
  	return 0;
  }else{
  	return 1;
  }
}

static int check(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);

	lua_pushinteger(L, PETSKILL_CHECKINDEX(index));
	return 1;
}

static int getPetskillArray(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);

	lua_pushinteger(L, PETSKILL_getPetskillArray(index));
	return 1;
}

static const luaL_Reg petskilllib[] = {
	{"getInt", 									getInt},
	{"setInt", 									setInt},
	{"getChar", 								getChar},
	{"setChar", 								setChar},
	{"check", 									check},
	{"getPetskillArray", 				getPetskillArray},
  {NULL, 						NULL}
};

LUALIB_API int luaopen_PetSkill (lua_State *L) {
  luaL_register(L, "petskill", petskilllib);
  return 1;
}

#endif

