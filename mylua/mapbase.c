#include <string.h>
#include <stdio.h>
#include "char.h"
#include "readmap.h"
#include "map_deal.h"
#include "char_base.h"
#include "mylua/base.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lssproto_serv.h"

#ifdef _ALLBLUES_LUA   
#ifdef _ALLBLUES_LUA_1_1
static int RandMap(lua_State *L) 
{
	lua_pushinteger(L, MAP_getfloorId(rand()% MAP_getMapNum()));
	return 1;
}

static int RandXAndY(lua_State *L) 
{
	const int ff = luaL_checkint(L, 1);
	int fx,fy;
	int i;
	for(i=0;i<50;i++){
		fx = rand() % MAP_getfloorX(ff);
		fy = rand() % MAP_getfloorY(ff);
		
		if(MAP_walkAbleFromPoint( ff,fx,fy,0 )==TRUE){
			lua_pushinteger(L, (fx << 16)|fy );
			return 1;
		}
	}
	lua_pushinteger(L, -1);
	return 1;
}

static int getX(lua_State *L) 
{
	const int XandY = luaL_checkint(L, 1);
	lua_pushinteger(L, (XandY >> 16)&0xFFFF );
	return 1;
}

static int getY(lua_State *L) 
{
	const int XandY = luaL_checkint(L, 1);
	lua_pushinteger(L, XandY&0xFFFF );
	return 1;
}
static int getfloorX(lua_State *L) 
{
	const int floorid = luaL_checkint(L, 1);
	lua_pushinteger(L, MAP_getfloorX(floorid) );
	return 1;
}

static int getfloorY(lua_State *L) 
{
	const int floorid = luaL_checkint(L, 1);
	lua_pushinteger(L, MAP_getfloorY(floorid) );
	return 1;
}

static int getFloorName(lua_State *L) 
{
	const int floorid = luaL_checkint(L, 1);
	
	char escapeshowstring[256];
	char *showstr = MAP_getfloorShowstring(floorid);

	getStringFromIndexWithDelim( showstr, "|", 1, escapeshowstring, sizeof( escapeshowstring));

	lua_pushstring(L, escapeshowstring);
	return 1;
}

static const luaL_Reg maplib[] = {
	{"RandMap", 			RandMap},
	{"RandXAndY", 		RandXAndY},
	{"getfloorX", 		getfloorX},
	{"getfloorY", 		getfloorY},
	{"getFloorName", 	getFloorName},
	{"getX", 					getX},
	{"getY", 					getY},
  {NULL, 						NULL}
};

LUALIB_API int luaopen_Map (lua_State *L) {
  luaL_register(L, "map", maplib);
  return 1;
}
#endif

#endif

