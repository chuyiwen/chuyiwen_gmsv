#include <string.h>
#include "common.h"
#include "char_talk.h"
#include "mylua/base.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lssproto_serv.h"
#ifdef _ALLBLUES_LUA_1_2
#include "mylua/mylua.h"
extern MY_Lua MYLua;

static int addLUAListFunction(lua_State *L) 
{
	size_t l;
	char *luafuncname = luaL_checklstring(L, 1, &l);
  char *luafunctable = luaL_checklstring(L, 2, &l);
	char *luafunctablepath = luaL_checklstring(L, 3, &l);
	const int gmlevel = luaL_checkint(L, 4);
	char *usestring = luaL_checklstring(L, 5, &l);
	
	
	if(strlen(luafunctablepath) > 0){
		MY_Lua *mylua = &MYLua;
	  while(mylua->next != NULL){
	  	if(strcmp(mylua->luapath, luafunctablepath) == 0){
				return MAGIC_addLUAListFunction( mylua->lua, luafuncname, luafunctable, gmlevel, usestring );
	  	}
	  	mylua = mylua->next;
	  }
	}else{
		return MAGIC_addLUAListFunction( L, luafuncname, luafunctable, gmlevel, usestring );
	}
	return 1;
	
}

static const luaL_Reg magiclib[] = {
	{"addLUAListFunction", 			addLUAListFunction},
  {NULL, 											NULL}
};

LUALIB_API int luaopen_Magic (lua_State *L) {
  luaL_register(L, "magic", magiclib);
  return 1;
}

#endif

