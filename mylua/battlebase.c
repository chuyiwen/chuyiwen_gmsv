#include <string.h>
#include "version.h"
#include "char_base.h"
#include "enemy.h"
#include "object.h"
#include "char.h"
#include "lssproto_serv.h"
#include "battle.h"
#include "battle_command.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "mylua/mylua.h"
#include "mylua/base.h"
#ifdef _ALLBLUES_LUA   

static int CreateVsEnemy (lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int npcindex = luaL_checkint(L, 2);
	int enemytable[11];
	int i;
	for( i = 0; i < 11; i ++ ) {
		enemytable[i] = -1;
	}
	luaL_checktype(L, 3, LUA_TTABLE);
	int n = luaL_getn(L, 3);
	int id = 0;
	for(i = 0; i < n; i++){
		int enemyid = getArrayInt(L, i);
		int curEnemy = ENEMY_getEnemyArrayFromId( enemyid );
		
		if( ENEMY_CHECKINDEX( curEnemy) ) {
			enemytable[id++] = curEnemy;
		}else{
			enemytable[i] = -1;
		}
	}
	int ret = BATTLE_CreateVsEnemyNew(charaindex, npcindex, enemytable);
	if( ret == 0 ) {
		if(npcindex>-1){
			CHAR_setWorkInt( npcindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_INIT );
			CHAR_sendBattleEffect( npcindex, ON);
		}
		lua_pushinteger(L, CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX));
		return 1;
	}else{
		return 0;
	}
}

static int getCharOne(lua_State *L) 
{
	const int battle = luaL_checkint(L, 1);
	const int num = luaL_checkint(L, 2);
	const int side = luaL_checkint(L, 3);
	
	lua_pushinteger(L, BattleArray[battle].Side[side].Entry[num].charaindex);
	
	return 1;
}


#ifdef _ALLBLUES_LUA_1_4
static CharBase BattleBaseEvent[] = {
	{{"结束事件"}, 		BATTLE_FINISH}
	,{{"逃跑事件"}, 	BATTLE_ESCAPE}
}; 

static int CreateRandVsPlayer (lua_State *L) 
{
	const int floorid = luaL_checkint(L, 1);
	int charaindex1 = -1, charaindex2 = -1;
	int num = 0;
	int i = 0; 
	int playernum = CHAR_getPlayerMaxNum();
	for (i = 0; i < playernum; i++){
		if (CHAR_getCharUse(i) == FALSE) continue;
	  if ( !CHAR_CHECKINDEX( i ) )continue;
	  if ( CHAR_getInt( i, CHAR_FLOOR ) == floorid ) {
			if (CHAR_getWorkInt(i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
				if(charaindex1 == -1){
					charaindex1 = i;
				}else{
					charaindex2 = i;
				}
				if(CHAR_CHECKINDEX( charaindex1 ) && CHAR_CHECKINDEX( charaindex2 ) ){
					BATTLE_CreateVsPlayer(charaindex1, charaindex2);
					num ++;
				}
			}
		}
	}

	lua_pushinteger(L, num);
		
	return 1;
}

static int CreateVsPlayer(lua_State *L) 
{
	const int charaindex1 = luaL_checkint(L, 1);
	const int charaindex2 = luaL_checkint(L, 2);
	
	if(CHAR_CHECKINDEX( charaindex1 ) && CHAR_CHECKINDEX( charaindex2 ) ){
		if( CHAR_getWorkInt( charaindex1, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE 
			&& CHAR_getWorkInt( charaindex2, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){
			if(BATTLE_CreateVsPlayer(charaindex1, charaindex2) == 0){
				lua_pushinteger(L, CHAR_getWorkInt( charaindex1, CHAR_WORKBATTLEINDEX));
				return 1;
			}
		}
	}
	lua_pushinteger(L, -1);
	return 1;
}

static int setLUAFunctionPointer(lua_State *L) 
{
	size_t l;
	const int battleindex = luaL_checkint(L, 1);
	const int functype = getCharBaseValue(L, 2, BattleBaseEvent, arraysizeof(BattleBaseEvent));
  char *luafunctable=luaL_checklstring(L, 3, &l);

	BATTLE_setLUAFunction(battleindex, functype, L, luafunctable);
	
	return 1;
}

static int WatchEntry(lua_State *L) 
{
	const int meindex = luaL_checkint(L, 1);
	const int toindex = luaL_checkint(L, 2);

	BATTLE_WatchEntry(meindex, toindex);
	
	return 1;
}

static int NewEntry(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int battleindex = luaL_checkint(L, 2);
	const int side = luaL_checkint(L, 3);
	int fd = getfdFromCharaIndex(charaindex);
	int iRet = BATTLE_NewEntry(charaindex, battleindex, side);
	if( iRet == 0 ){
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE );
		iRet = BATTLE_PetDefaultEntry(
				charaindex,
				battleindex,
				side
		);
	}
	if( iRet == 0 ){
		int		flg;
		if( CHAR_getInt( charaindex, CHAR_DEFAULTPET ) >= 0 ){
			int pindex = CHAR_getCharPet( charaindex, CHAR_getInt( charaindex, CHAR_DEFAULTPET ) );
			CHAR_setWorkInt( pindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_RESCUE );
		}
		if( fd != -1 ){
			lssproto_EN_send( fd, BattleArray[battleindex].type, BattleArray[battleindex].field_no );
		}
		flg = (BattleArray[battleindex].Side[side].flg & BSIDE_FLG_HELP_OK)? TRUE:FALSE;
		lssproto_HL_send( fd, flg);
		char szBuffer[256];
		sprintf( szBuffer, "BP|%X|%X|%X",
			BATTLE_Index2No( battleindex, charaindex ), BP_FLG_JOIN, CHAR_getInt(charaindex, CHAR_MP ) );
		BATTLE_CommandSend( charaindex, szBuffer );
		lua_pushinteger(L, 1);
	}else{
		if( fd != -1 ){
			lssproto_EN_send( fd, FALSE, BattleArray[battleindex].field_no );
		}
		lua_pushinteger(L, 0);
	}
	
	return 1;
}
#endif

#ifdef _ALLBLUES_LUA_1_9
static int CreateForWatcher(lua_State *L) 
{
	const int meindex = luaL_checkint(L, 1);
	const int baatleindex = luaL_checkint(L, 2);

	BATTLE_CreateForWatcher(meindex, baatleindex);
	
	return 1;
}

static int checkindex(lua_State *L) 
{
	const int baatleindex = luaL_checkint(L, 1);

	lua_pushinteger(L, BATTLE_CHECKINDEX(baatleindex));

	return 1;
}

static int getType(lua_State *L) 
{
	const int baatleindex = luaL_checkint(L, 1);

	lua_pushinteger(L, BATTLE_getType(baatleindex));

	return 1;
}

#ifdef _BATTLE_TIMESPEED
static int getCreateTime(lua_State *L) 
{
	const int baatleindex = luaL_checkint(L, 1);

	lua_pushinteger(L, BATTLE_getCreateTime(baatleindex));

	return 1;
}
#endif

static int getBattleFloor(lua_State *L) 
{
	const int baatleindex = luaL_checkint(L, 1);

	lua_pushinteger(L, BATTLE_getBattleFloor(baatleindex));

	return 1;
}
#endif

static const luaL_Reg battlelib[] = {
  {"CreateVsEnemy", 						CreateVsEnemy},
  {"getCharOne", 								getCharOne},
#ifdef _ALLBLUES_LUA_1_4
  {"CreateRandVsPlayer", 				CreateRandVsPlayer},
  {"CreateVsPlayer", 						CreateVsPlayer},
  {"setLUAFunctionPointer", 		setLUAFunctionPointer},
  {"WatchEntry", 								WatchEntry},
	{"NewEntry", 									NewEntry},
#endif
#ifdef _ALLBLUES_LUA_1_9
	{"CreateForWatcher", 					CreateForWatcher},
	{"checkindex", 								checkindex},
	{"getType", 									getType},
#ifdef _BATTLE_TIMESPEED
	{"getCreateTime", 						getCreateTime},
#endif
	{"getBattleFloor", 						getBattleFloor},
#endif
  {NULL, NULL}
};


LUALIB_API int luaopen_Battle (lua_State *L) {
  luaL_register(L, "battle", battlelib);
  return 1;
}

#endif

