#define __MYLUA__H__
#ifdef  __MYLUA__H__
  
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
int myluaload (char *filename);
int remyluaload (char *filename);
int closemyluaload();
void CryptoAllbluesLUA(char *path, int flg, int id);
int dofile (lua_State *L, const char *name);
int docall (lua_State *L, int narg, int clear);
int getArrayInt(lua_State *L, int idx);
LUALIB_API void luaAB_openlibs (lua_State *L);

typedef struct tagMYLua
{
	lua_State *lua;
	char *luapath;
  struct tagMYLua *next;
}MY_Lua;

#endif


//返回一个int给LUA引擎
#define LRetInt(L, i) \
{ \
	lua_pushinteger(L, i); \
	LRet(1); \
}

#define LRet(r) \
{ \
	NPC_Lua_SetErrorStr(NULL); \
	return r; \
}

#define CheckEx2(L, t, n) \
{	\
	if(lua_gettop(L) < (t) || lua_gettop(L) > n) \
	{ \
		luaL_argerror(L, 1, "参数数量错误"); \
	} \
}

//返回一个BOOL给LUA引擎
#define LRetBool(L, b) \
{ \
	lua_pushboolean(L, b); \
	LRet(1); \
}
