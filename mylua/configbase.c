#include <string.h>
#include <stdio.h>
#include <math.h>
#include "char.h"
#include "char_base.h"
#include "mylua/base.h"
#include "mylua/mylua.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "configfile.h"

#ifdef _ALLBLUES_LUA   

#ifdef _ALLBLUES_LUA_1_1
static int set(lua_State *L) 
{
	size_t l;
  char *data=luaL_checklstring(L, 1, &l);
	luareadconfigfile(data);
	return 1;
}

static int checkIp(lua_State *L) 
{
  int ip = luaL_checkint(L, 1);

	lua_pushinteger(L, checkServerIp(ip));
	return 1;
}

static int config_getBattleexp(lua_State *_NLL) 
{
   LRetInt(_NLL, getBattleexp());
}

#ifdef _ALLBLUES_LUA_2_0
static int ABLUA_getGameservername(lua_State *L) 
{
	char* name = getGameservername();
	lua_pushstring(L, name);
	return 1;
}

static int ABLUAgetFdnum(lua_State *L) 
{
	int playnum = getFdnum();
	lua_pushinteger(L, playnum);
	return 1;
}
#endif

#ifdef _FM_EXP_ADD
static int getfmexp(lua_State *L)
{
	lua_pushinteger(L, getFmAddExp());
	return 1;
}
#endif

static const luaL_Reg configlib[] = {
	{"set", 			set},
	{"checkIp", 	checkIp},
	{"getBattleexp", 	config_getBattleexp},
#ifdef _FM_EXP_ADD
	{"getFmExp", 		getfmexp},
#endif
#ifdef _ALLBLUES_LUA_2_0
	{"getGameservername", 	ABLUA_getGameservername},
	{"getFdnum", 	ABLUAgetFdnum},
#endif
  {NULL, 				NULL}
};

LUALIB_API int luaopen_Config (lua_State *L) {
  luaL_register(L, "config", configlib);
  return 1;
}
#endif

#endif

