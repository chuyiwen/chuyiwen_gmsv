#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "char.h"
#include "char_base.h"
#include "mylua/base.h"
#include "util.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lssproto_serv.h"
#include "mylua/mylua.h"

#ifdef _ALLBLUES_LUA   
extern MY_Lua MYLua;

static int nowtime(lua_State *L) 
{
	lua_pushinteger(L, (int) time( NULL ));
	return 1;
}

static int strtoi(lua_State *L) 
{
	size_t l;
	char *data=luaL_checklstring(L, 1, &l);
	lua_pushinteger(L, atoi(data));
	return 1;
}

static int getString(lua_State *L) 
{
	size_t l;
	char *data = luaL_checklstring(L, 1, &l);
	char *delim = luaL_checklstring(L, 2, &l);
	const int index = luaL_checkint(L, 3);
	
	char token[256];
	if(getStringFromIndexWithDelim( data, delim, index, token, sizeof( token))){
		lua_pushstring(L, token);
	}else{
		lua_pushstring(L, "");
	}
	return 1;
}

static int CallFunction(lua_State *L) 
{
	size_t l;
	char *funcname = luaL_checklstring(L, 1, &l);
	char *filename = luaL_checklstring(L, 2, &l);
	char newfilename[256];
	luaL_checktype(L, 3, LUA_TTABLE);
	int n = luaL_getn(L, 3);

	lua_State *lua = NULL;

	MY_Lua *mylua = &MYLua;
  while(mylua->next != NULL){
  	if(strcmptail( mylua->luapath, ".allblues" ) == 0 
  		&& strcmptail( filename, ".lua" ) == 0 ){
  		sprintf(newfilename, "%s.allblues", filename);
  	}else{
  		sprintf(newfilename, "%s", filename);
  	}

  	if(strcmp(newfilename, mylua->luapath) == 0){
  		lua = mylua->lua;
			break;
		}
  	mylua = mylua->next;
  }
  if (lua == NULL) {
		return FALSE;
	}

	lua_getglobal(lua, funcname);

	if (!lua_isfunction(lua, -1)) {
    return FALSE;
  }

	int i;

	for(i = 0; i < n; i++){
		lua_pushnumber(lua, getArrayInt(L, i)); 
	}


	lua_pcall(lua, n, 1, 0);

	lua_isnumber(lua, -1);

	int ret = lua_tonumber(lua, -1);
	lua_pushinteger(L, ret);
  return TRUE;
}


static int c10to62(lua_State *L) 
{
	const int index = luaL_checkint(L, 1);
	
	char token[256];
	cnv10to62( index,	token, sizeof(token));
	
	lua_pushstring(L, token);
	return 1;
}

static const luaL_Reg otherlib[] = {
	{"time", 					nowtime},
	{"atoi", 					strtoi},
	{"getString", 		getString},
	{"CallFunction", 	CallFunction},
	{"c10to62", 				c10to62},
  {NULL, 						NULL}
};

LUALIB_API int luaopen_Other (lua_State *L) {
  luaL_register(L, "other", otherlib);
  return 1;
}

#endif

