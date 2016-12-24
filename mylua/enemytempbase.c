#include <string.h>
#include <stdio.h>
#include "char.h"
#include "readmap.h"
#include "map_deal.h"
#include "char_base.h"
#include "enemy.h"
#include "mylua/base.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lssproto_serv.h"

#ifdef _ALLBLUES_LUA   
#ifdef _ALLBLUES_LUA_1_4

static CharBase EnemytempBaseInt[] = {
	{{"���"},				E_T_TEMPNO}      
	,{{"��ʼֵ"},			E_T_INITNUM}     
	,{{"�ɳ���"},			E_T_LVUPPOINT}   
	,{{"����"},				E_T_BASEVITAL}   
	,{{"����"},				E_T_BASESTR}     
	,{{"����"},				E_T_BASETGH}     
	,{{"�ٶ�"},				E_T_BASEDEX}     
	,{{"����"},				E_T_MODAI}       
	,{{"��"},					E_T_GET}         
	,{{"��"},					E_T_EARTHAT}     
	,{{"ˮ"},					E_T_WATERAT}     
	,{{"��"},					E_T_FIREAT}      
	,{{"��"},					E_T_WINDAT}      
	,{{"����"},				E_T_POISON}      
	,{{"�鿹"},				E_T_PARALYSIS}   
	,{{"˯��"},				E_T_SLEEP}       
	,{{"ʯ��"},				E_T_STONE}       
	,{{"�ƿ�"},				E_T_DRUNK}       
	,{{"�쿹"},				E_T_CONFUSION}   
	,{{"����1"},			E_T_PETSKILL1}   
	,{{"����2"},			E_T_PETSKILL2}   
	,{{"����3"},			E_T_PETSKILL3}   
	,{{"����4"},			E_T_PETSKILL4}   
	,{{"����5"},			E_T_PETSKILL5}   
	,{{"����6"},			E_T_PETSKILL6}   
	,{{"����7"},			E_T_PETSKILL7}   
	,{{"����"},				E_T_RARE}        
	,{{"������"},			E_T_CRITICAL}    
	,{{"������"},			E_T_COUNTER}     
	,{{"������"},			E_T_SLOT}        
	,{{"����"},				E_T_IMGNUMBER}   
	,{{"�ȼ�����"},		E_T_LIMITLEVEL}  
};

static CharBase EnemytempBaseChar[] = {
	{{"����"},				E_T_NAME}
};

static int getInt (lua_State *L) 
{
  const int array = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, EnemytempBaseInt, arraysizeof(EnemytempBaseInt));
  lua_pushinteger(L, ENEMYTEMP_getInt(array, element));
  return 1;
}

static int getChar (lua_State *L) 
{
  const int array = luaL_checkint(L, 1);
  const int element = getCharBaseValue(L, 2, EnemytempBaseChar, arraysizeof(EnemytempBaseChar));
  lua_pushstring(L, ENEMYTEMP_getChar(array, element));
  return 1;
}

static int getEnemyTempArray(lua_State *L) 
{
  const int enemyid = luaL_checkint(L, 1);
  lua_pushinteger(L, ENEMYTEMP_getEnemyTempArray(enemyid));
  return 1;
}


static int getEnemyTempArrayFromTempNo(lua_State *L) 
{
  const int array = luaL_checkint(L, 1);
  lua_pushinteger(L, ENEMYTEMP_getEnemyTempArrayFromTempNo(array));
  return 1;
}

static int getEnemyTempArrayFromInitnum(lua_State *L) 
{
  const int array = luaL_checkint(L, 1);
  lua_pushinteger(L, ENEMYTEMP_getEnemyTempArrayFromInitnum(array));
  return 1;
}

static int getEnemyTempNameFromEnemyID(lua_State *L) 
{
  const int enemyid = luaL_checkint(L, 1);
  int i;
	for( i = 0; i < ENEMY_getEnemyNum(); i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == ENEMY_getEnemyNum() ){
		lua_pushstring(L, "");
		return 1;
	}
	int tarray = ENEMYTEMP_getEnemyTempArray( i );         
	          
	if( !ENEMYTEMP_CHECKINDEX( tarray)) {
		lua_pushstring(L, "");
	}else{
		lua_pushstring(L, ENEMYTEMP_getChar(tarray, E_T_NAME));
	}
  return 1;
}


static int getEnemyTempIDFromEnemyID(lua_State *L) 
{
  const int enemyid = luaL_checkint(L, 1);
  int i;
	for( i = 0; i < ENEMY_getEnemyNum(); i ++ ) {
		if( ENEMY_getInt( i, ENEMY_ID) == enemyid) {
			break;
		}
	}
	if( i == ENEMY_getEnemyNum() ){
		lua_pushinteger(L, -1);
		return 1;
	}
	lua_pushinteger(L, ENEMY_getInt( i, ENEMY_TEMPNO));
  return 1;
}




static const luaL_Reg enemytemplib[] = {
	{"getInt", 												getInt},
	{"getChar", 											getChar},
	{"getEnemyTempArray", 						getEnemyTempArray},
	{"getEnemyTempArrayFromTempNo", 	getEnemyTempArrayFromTempNo},
	{"getEnemyTempArrayFromInitnum", 	getEnemyTempArrayFromInitnum},
	{"getEnemyTempNameFromEnemyID", 	getEnemyTempNameFromEnemyID},
	{"getEnemyTempIDFromEnemyID", 		getEnemyTempIDFromEnemyID},
  {NULL, 									NULL}
};

LUALIB_API int luaopen_Enemytemp (lua_State *L) {
  luaL_register(L, "enemytemp", enemytemplib);
  return 1;
}
#endif

#endif

