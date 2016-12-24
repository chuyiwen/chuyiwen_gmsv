#include <string.h>
#include "lua.h"
#include "version.h"
#include "lauxlib.h"
#include "lualib.h"
#include "net.h"
#include "log.h"

#ifdef _ALLBLUES_LUA   

static int endOne(lua_State *L) 
{
	const int sockfd = luaL_checkint(L, 1);
	
	CONNECT_endOne_debug(sockfd);

	return 1;
}

static int userip(lua_State *L) 
{
	int charaindex = luaL_checkint(L, 1);

	int sockfd = getfdFromCharaIndex(charaindex);

	unsigned long ip;
	int a,b,c,d;
	char strIP[32];
  ip = CONNECT_get_userip(sockfd);
  
  a=(ip % 0x100); ip=ip / 0x100;
  b=(ip % 0x100); ip=ip / 0x100;
  c=(ip % 0x100); ip=ip / 0x100;
  d=(ip % 0x100);
  
	sprintf(strIP, "%d.%d.%d.%d", a, b, c, d);

	lua_pushstring(L, strIP);
	return 1;
}

#ifdef _ALLBLUES_LUA_2_0
static int getIP(lua_State *L) 
{
	int sockfd = luaL_checkint(L, 1);

	unsigned long ip;
	int a,b,c,d;
	char strIP[32];
	ip = CONNECT_get_userip(sockfd);

	a=(ip % 0x100); ip=ip / 0x100;
	b=(ip % 0x100); ip=ip / 0x100;
	c=(ip % 0x100); ip=ip / 0x100;
	d=(ip % 0x100);

	sprintf(strIP, "%d.%d.%d.%d", a, b, c, d);

	lua_pushstring(L, strIP);
	return 1;
}

static int getUse(lua_State *L) 
{
	int fd = luaL_checkint(L, 1);
	lua_pushboolean(L,CONNECT_getUse(fd ));
	return 1;
}
#endif

static const luaL_Reg netlib[] = {
  {"endOne", 					endOne},
  {"userip", 					userip},
#ifdef _ALLBLUES_LUA_2_0
  {"getIP", 					getIP},
  {"getUse", 					getUse},
#endif
	{NULL, 							NULL}
};


LUALIB_API int luaopen_Net (lua_State *L) {
  luaL_register(L, "net", netlib);
  return 1;
}

#endif
