#include <string.h>
#include "char.h"
#include "char_base.h"
#include "family.h"
#include "mylua/base.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "saacproto_cli.h"

#ifdef _ALLBLUES_LUA

extern char	familyListBuf[MAXFAMILYLIST];

static int ShowFamilyList(lua_State *L) 
{
	size_t l;
	const int index = luaL_checkint(L, 1);
	char subbuf[256];
	if( getStringFromIndexWithDelim( familyListBuf, "|", index, subbuf, sizeof(subbuf) ) == TRUE){
		lua_pushstring(L, subbuf);
	}else{
		lua_pushstring(L, "");
	}
	return 1;
}

static const luaL_Reg Familylib[] = {
	{"ShowFamilyList", 	ShowFamilyList},
  {NULL, 						NULL}
};

LUALIB_API int luaopen_Family (lua_State *L) {
  luaL_register(L, "family", Familylib);
  return 1;
}

#endif

