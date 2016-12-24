#include <string.h>
#include "common.h"
#include "char_base.h"
#include "item.h"
#include "configfile.h"
#include "battle.h"
#include "mylua/base.h"
#include "version.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "mylua/mylua.h"
#include "mylua/function.h"
#include "char.h"


#ifdef _ALLBLUES_LUA   
extern MY_Lua MYLua;

//lua_State *M_Script_Lua = NULL;	//实例句柄

lua_State *FindLua(char *filename)
{
	MY_Lua *mylua = &MYLua;
	char newfilename[256];
  while(mylua->next != NULL){
  	if(strcmptail( mylua->luapath, ".allblues" ) == 0 ){
  		sprintf(newfilename, "%s.allblues", filename);
  	}else{
  		sprintf(newfilename, "%s", filename);
  	}

  	if(strcmp(newfilename, mylua->luapath) == 0){
  		return mylua->lua;
		}
  	mylua = mylua->next;
  }
	
	return NULL;
}

#ifdef _ALLBLUES_LUA_1_9
BOOL EquipEffectFunction( int charaindex, int id )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/equipeffectfunction.lua");
	  if (lua == NULL)return TRUE;
	}

	lua_getglobal(lua, "EquipEffectFunction");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  //依次放入二个参数
  lua_pushnumber(lua,charaindex); 
	lua_pushnumber(lua,id); 

	if (lua_pcall(lua, 2, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	if (!lua_isnumber(lua, -1))
		error(lua, "function `f' must return a number");

	int ret = lua_tonumber(lua, -1);
	lua_pop(lua, 1);
  return ret;
}

BOOL WalkFunction( int charaindex )
{
	MY_Lua *mylua = &MYLua;
  while(mylua->lua != NULL){
		lua_getglobal(mylua->lua, "WalkFunction");

		if (!lua_isfunction(mylua->lua, -1)) {
	    lua_pop(mylua->lua, 1);
	    mylua = mylua->next;
	    continue;
	  }
	  
	  lua_pushnumber(mylua->lua,charaindex); 
	  
		if (lua_pcall(mylua->lua, 1, 1, 0) != 0)
			error(mylua->lua, "error running function `f': %s", lua_tostring(mylua->lua, -1));
	
		if (!lua_isnumber(mylua->lua, -1))
			error(mylua->lua, "function `f' must return a number");
	
		int ret = lua_tonumber(mylua->lua, -1);
		lua_pop(mylua->lua, 1);
		if(ret == TRUE){
	  	return TRUE;
	  }
	  
	  mylua = mylua->next;
	}

  return FALSE;
}
#ifdef _ITEM_OVER_LAP
BOOL ItemOverlapFunction( int charindex, int fromitemindex, int toitemindex )
{
	MY_Lua *mylua = &MYLua;
  while(mylua->lua != NULL){
		lua_getglobal(mylua->lua, "ItemOverlapFunction");

		if (!lua_isfunction(mylua->lua, -1)) {
	    lua_pop(mylua->lua, 1);
	    mylua = mylua->next;
	    continue;
	  }
	  //依次放入二个参数
	  lua_pushnumber(mylua->lua,charindex); 
	  lua_pushnumber(mylua->lua,fromitemindex); 
	  lua_pushnumber(mylua->lua,toitemindex); 

		lua_pcall(mylua->lua, 3, 1, 0);

		lua_isnumber(mylua->lua, -1);

		int ret = lua_tonumber(mylua->lua, -1);
		lua_pop(mylua->lua, 1);
		if(ret == 1){
			mylua = mylua->next;
	    continue;
		}
  	return ret;
	}

  return TRUE;
}

BOOL ItemOverlapedFunction( int charindex, int fromitemindex, int fromid, int toitemindex, int toid)
{
	MY_Lua *mylua = &MYLua;
  while(mylua->lua != NULL){
		lua_getglobal(mylua->lua, "ItemOverlapedFunction");

		if (!lua_isfunction(mylua->lua, -1)) {
	    lua_pop(mylua->lua, 1);
	    mylua = mylua->next;
	    continue;
	  }
	  //依次放入二个参数
	  lua_pushnumber(mylua->lua,charindex); 
	  lua_pushnumber(mylua->lua,fromitemindex); 
	  lua_pushnumber(mylua->lua,fromid); 
	  lua_pushnumber(mylua->lua,toitemindex); 
	  lua_pushnumber(mylua->lua,toid); 
	  
	  docall(mylua->lua, 5, 1);
	  mylua = mylua->next;
	}

  return TRUE;
}
#endif

BOOL BattleFinishFunction( int charaindex, int battletime, int battleturn, int battletype )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/battlefinishfunction.lua");
	  if (lua == NULL)return TRUE;
	}

	lua_getglobal(lua, "BattleFinishFunction");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  //依次放入二个参数
  lua_pushnumber(lua,charaindex); 
	lua_pushnumber(lua,battletime); 
	lua_pushnumber(lua,battleturn); 
	lua_pushnumber(lua,battletype); 

	docall(lua, 4, 1);

  return TRUE;
}

BOOL SetBattleEnmeyFunction(int meindex, int enemyindex, int id )
{
	lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_BATTLESETFUNC);
  if ( lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, meindex); 
  lua_pushnumber(lua, enemyindex); 
  lua_pushnumber(lua, id); 
  docall(lua, 3, 1);

  return TRUE;
}

#endif

#ifdef _ALLBLUES_LUA_1_8
BOOL CaptureOkFunction( int attackindex, int defindex )
{
	MY_Lua *mylua = &MYLua;
  while(mylua->lua != NULL){
		lua_getglobal(mylua->lua, "CaptureOkFunction");

		if (!lua_isfunction(mylua->lua, -1)) {
	    lua_pop(mylua->lua, 1);
	    mylua = mylua->next;
	    continue;
	  }
	  //依次放入二个参数
	  lua_pushnumber(mylua->lua,attackindex); 
	  lua_pushnumber(mylua->lua,defindex); 
	  
	  docall(mylua->lua, 2, 1);
	  mylua = mylua->next;
	}

  return TRUE;
}

BOOL CaptureCheckFunction( int attackindex, int defindex )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/capturefunction.lua");
	  if (lua == NULL)return TRUE;
	}

	lua_getglobal(lua, "CaptureCheckFunction");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }

  //依次放入二个参数
  lua_pushnumber(lua,attackindex); 
  lua_pushnumber(lua,defindex); 

	if (lua_pcall(lua, 2, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	if (!lua_isnumber(lua, -1))
		error(lua, "function `f' must return a number");

	int ret = lua_tonumber(lua, -1);
	lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_7
BOOL CharVsEnemyFunction( int charaindex )
{
	MY_Lua *mylua = &MYLua;
  while(mylua->lua != NULL){
		lua_getglobal(mylua->lua, "CharVsEnemyFunction");

		if (!lua_isfunction(mylua->lua, -1)) {
	    lua_pop(mylua->lua, 1);
	    mylua = mylua->next;
	    continue;
	  }
	  
	  lua_pushnumber(mylua->lua,charaindex); 
	  
		if (lua_pcall(mylua->lua, 1, 1, 0) != 0)
			error(mylua->lua, "error running function `f': %s", lua_tostring(mylua->lua, -1));
	
		if (!lua_isnumber(mylua->lua, -1))
			error(mylua->lua, "function `f' must return a number");
	
		int ret = lua_tonumber(mylua->lua, -1);
		lua_pop(mylua->lua, 1);
		if(ret == TRUE){
	  	return TRUE;
	  }
	  
	  mylua = mylua->next;
	}

  return FALSE;
}
#endif

#ifdef _ALLBLUES_LUA_1_6
BOOL CharTalkFunction( int charaindex, char *message, int color )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/chartalkfunction.lua");
	  if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "CharTalkFunction");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua,charaindex); 
  lua_pushstring(lua,message); 
	lua_pushnumber(lua,color); 
	
	if (lua_pcall(lua, 3, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	if (!lua_isnumber(lua, -1))
		error(lua, "function `f' must return a number");

	int ret = lua_tonumber(lua, -1);
	lua_pop(lua, 1);
  return ret;
}

BOOL FamilyRideFunction( int meindex, int petindex, int petid )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/familyridefunction.lua");
	  if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "FamilyRideFunction");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua,meindex); 
  lua_pushnumber(lua,petindex); 
	lua_pushnumber(lua,petid); 
	
	if (lua_pcall(lua, 3, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	if (!lua_isnumber(lua, -1))
		error(lua, "function `f' must return a number");

	int ret = lua_tonumber(lua, -1);
	lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_5
BOOL NetLoopFunction( void )
{
	MY_Lua *mylua = &MYLua;
  while(mylua->lua != NULL){
		lua_getglobal(mylua->lua, "NetLoopFunction");

		if (!lua_isfunction(mylua->lua, -1)) {
	    lua_pop(mylua->lua, 1);
	    mylua = mylua->next;
	    continue;
	  }
	  docall(mylua->lua, 0, 1);
	  mylua = mylua->next;
	}

  return TRUE;
}

BOOL FreeCharCreate( int charaindex )
{
	static lua_State *lua;


  if (lua == NULL){
  	lua = FindLua("data/ablua/freecharcreate.lua");
	  if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "FreeCharCreate");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua,charaindex); 

	docall(lua, 1, 1);

  return TRUE;
}

BOOL FreeCharLogin( int charaindex )
{
	static lua_State *lua;


  if (lua == NULL){
  	lua = FindLua("data/ablua/freecharlogin.lua");
	  if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "FreeCharLogin");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua,charaindex); 

	docall(lua, 1, 1);

  return TRUE;
}


BOOL FreeVsPlayer( int charaindex, int toindex )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/freevsplayer.lua");
	  if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "FreeVsPlayer");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入二个参数
  lua_pushnumber(lua,charaindex); 
  lua_pushnumber(lua,toindex); 

	if (lua_pcall(lua, 2, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	if (!lua_isnumber(lua, -1))
		error(lua, "function `f' must return a number");

	int ret = lua_tonumber(lua, -1);
	lua_pop(lua, 1);
  return ret;
}

BOOL FreePartyJoin( int charaindex, int toindex )
{
	MY_Lua *mylua = &MYLua;
  while(mylua->lua != NULL){
		lua_getglobal(mylua->lua, "FreePartyJoin");

		if (!lua_isfunction(mylua->lua, -1)) {
	    lua_pop(mylua->lua, 1);
	    mylua = mylua->next;
	    continue;
	  }
  	//依次放入二个参数
	  lua_pushnumber(mylua->lua,charaindex); 
	  lua_pushnumber(mylua->lua,toindex); 
		  
		if (lua_pcall(mylua->lua, 2, 1, 0) != 0)
			error(mylua->lua, "error running function `f': %s", lua_tostring(mylua->lua, -1));

	
		if (!lua_isnumber(mylua->lua, -1))
			error(mylua->lua, "function `f' must return a number");
	
		int ret = lua_tonumber(mylua->lua, -1);
		lua_pop(mylua->lua, 1);
		if(ret == FALSE){
	  	return FALSE;
	  }
	  
	  mylua = mylua->next;
	}

  return TRUE;
}
#endif

#ifdef _ALLBLUES_LUA_1_4

BOOL RunCharLogOutEvent( int charaindex)
{
	lua_State *lua = CHAR_getLUAFunction(charaindex, CHAR_LOGINOUTFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入二个参数
  lua_pushnumber(lua, charaindex); 

  docall(lua, 1, 1);

  return TRUE;
}

BOOL BattleFinish( int battleindex, int charaindex)
{
	lua_State *lua = BATTLE_getLUAFunction(battleindex, BATTLE_FINISH);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入二个参数
  lua_pushnumber(lua, charaindex); 

  docall(lua, 1, 1);

  return TRUE;
}

BOOL BattleEscape( int battleindex, int charaindex)
{
	lua_State *lua = BATTLE_getLUAFunction(battleindex, BATTLE_ESCAPE);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入二个参数
  lua_pushnumber(lua, charaindex); 

  docall(lua, 1, 1);

  return TRUE;
}
#endif
/*
#ifdef _OFFLINE_SYSTEM   
BOOL OffLineCommand( int battleindex, int charaindex, int side)
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/offlinecommand.lua");
	  if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "OffLineCommand");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua,battleindex); 
  lua_pushnumber(lua,charaindex); 
	lua_pushnumber(lua,side); 
	
  docall(lua, 3, 1);

  return TRUE;
}
#endif
*/
#ifdef _PETSKILL_SHOP_LUA   
BOOL FreePetSkillShop( int talkerindex, int petindex, int oldSkillID, int newSkillID)
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/freepetskillshop.lua");
	  if (lua == NULL)return TRUE;
	}

	lua_getglobal(lua, "FreePetSkillShop");

	if (!lua_isfunction(lua, -1)) {
    return TRUE;
  }
  //依次放入三个参数
  lua_pushnumber(lua,talkerindex); 
	lua_pushnumber(lua,petindex); 
	lua_pushnumber(lua,oldSkillID); 
	lua_pushnumber(lua,newSkillID); 

	if (lua_pcall(lua, 4, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	if (!lua_isnumber(lua, -1))
		error(lua, "function `f' must return a number");

	int ret = lua_tonumber(lua, -1);
	lua_pop(lua, 1);
  return ret;
}
#endif

#ifdef _ALLBLUES_LUA_1_2
BOOL RunUseChatMagic( int charaindex, char *data, lua_State *lua)
{
	if(lua == NULL)return FALSE;
	
  //依次放入二个参数
  lua_pushnumber(lua, charaindex); 
	lua_pushstring(lua, data); 

  docall(lua, 2, 1);

  return TRUE;
}


BOOL RunItemUseEvent( int itemindex, int charaindex, int toindex, int haveitemindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_USEFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入四个参数
  lua_pushnumber(lua, itemindex); 
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, toindex); 
	lua_pushnumber(lua, haveitemindex); 

  docall(lua, 4, 1);

  return TRUE;
}

BOOL RunItemDieReLifeEvent( int charaindex, int itemindex, int haveitemindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_DIERELIFEFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入三个参数
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, itemindex); 
	lua_pushnumber(lua, haveitemindex); 

  docall(lua, 3, 1);

  return TRUE;
}

BOOL RunItemDetachEvent( int charaindex, int itemindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_DETACHFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入二个参数
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, itemindex); 

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemAttachEvent( int charaindex, int itemindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_ATTACHFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入二个参数
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, itemindex); 

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPickupEvent( int charaindex, int itemindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_PICKUPFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
	
  //依次放入二个参数
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, itemindex); 

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPostOverEvent( int itemindex, int charaindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_POSTOVERFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
  //依次放入二个参数
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, itemindex); 

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemPreOverEvent( int itemindex, int charaindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_PREOVERFUNC);
  if ( lua == NULL) {
    return FALSE;
  }
  //依次放入二个参数
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, itemindex); 

  docall(lua, 2, 1);

  return TRUE;
}

BOOL RunItemDropEvent( int charaindex, int itemindex )
{
	lua_State *lua = ITEM_getLUAFunction(itemindex, ITEM_DROPFUNC);
  if ( lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
	lua_pushnumber(lua, charaindex); 
	lua_pushnumber(lua, itemindex); 

  docall(lua, 2, 1);

  return TRUE;
}

#endif


BOOL RunCharTalkedEvent(int meindex, int toindex, char *messageeraseescape, int color, int channel)
{
	lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_TALKEDFUNC);
  if ( lua == NULL) {
    return FALSE;
  }

  //依次放入五个参数
  lua_pushnumber(lua,meindex); 
	lua_pushnumber(lua,toindex); 
	lua_pushstring(lua,messageeraseescape); 
	lua_pushnumber(lua,color); 
	lua_pushnumber(lua,channel); 

  docall(lua, 5, 1);

  return TRUE;
}


BOOL RunCharLoopEvent(int meindex)
{
	lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_LOOPFUNC);
  if ( lua == NULL) {
    return FALSE;
  }

  //依次放入一个参数
  lua_pushnumber(lua,meindex); 
  docall(lua, 1, 1);

  return TRUE;
}

BOOL RunCharOverlapEvent( int meindex, int toindex)
{
	lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_OVERLAPEDFUNC);
  if ( lua == NULL) {
    return FALSE;
  }

  //依次放入二个参数
  lua_pushnumber(lua, meindex); 
  lua_pushnumber(lua, toindex); 
  docall(lua, 2, 1);

  return TRUE;
}
                    
BOOL RunCharBattleOverEvent( int meindex, int battleindex, int iswin)
{
	lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_BATTLEOVERDFUNC);
  if ( lua == NULL) {
    return FALSE;
  }

  //依次放入三个参数
  lua_pushnumber(lua, meindex); 
  lua_pushnumber(lua, battleindex); 
  lua_pushnumber(lua, iswin); 
  docall(lua, 3, 1);

  return TRUE;
}

BOOL RunCharWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data)
{
	lua_State *lua = CHAR_getLUAFunction(meindex, CHAR_WINDOWTALKEDFUNC);
  if ( lua == NULL) {
    return FALSE;
  }

  //依次放入五个参数
  lua_pushnumber(lua, meindex); 
  lua_pushnumber(lua, talkerindex); 
  lua_pushnumber(lua, seqno); 
  lua_pushnumber(lua, select); 
  lua_pushstring(lua, data); 

  docall(lua, 5, 1);

  return TRUE;
}

BOOL fameButton( int index)
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/fameButton.lua");
	  if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "fameButton");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return FALSE;
  }
  //依次放入1个参数
  lua_pushnumber(lua,index); 

	if (lua_pcall(lua, 1, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	BOOL ret = (BOOL)lua_toboolean(lua, -1);
	lua_pop(lua, 1);
	return ret;

}

#ifdef _ALLBLUES_LUA_2_0

BOOL FreeModeExp(int charaindex, int exp )
{
	//printf("exp【1】 = %d \n",exp);
	static lua_State *lua;
	if (lua == NULL){
		lua = FindLua("data/ablua/freemodeexp.lua");
		if (lua == NULL)return FALSE;
	}
	lua_getglobal(lua, "FreeModeExp");

	if (!lua_isfunction(lua, -1)) {
		lua_pop(lua, 1);
		return TRUE;
	}
	int TM_Ret = 0;
	
	lua_pushnumber(lua,charaindex);
	lua_pushinteger(lua, (lua_Integer)exp);

  TM_Ret = lua_pcall(lua, 2, 0, 0);
  if(TM_Ret != 0)
	{
		print("FreeCharExpSave Lua Err :%d(%s)\n", TM_Ret, lua_tostring(lua, -1));
		lua_pop(lua, 1);

	  exp = &TM_Ret;
		return exp;

	}
}

/*
BOOL FreeModeExp(int charaindex,int* addexp, int getexp, int modexp )
{
	static lua_State *lua;
	if (lua == NULL){
		lua = FindLua("data/ablua/freemodeexp.lua");
		if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "FreeModeExp");

	if (!lua_isfunction(lua, -1)) {
		lua_pop(lua, 1);
		return TRUE;
	}
	lua_pushnumber(lua,charaindex); 
	lua_pushnumber(lua,getexp); 
	lua_pushnumber(lua,modexp); 

	if (lua_pcall(lua, 3, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	if (!lua_isnumber(lua, -1))
		error(lua, "function `f' must return a number");

	int ret = -1;
	ret = lua_tonumber(lua, -1);

	lua_pop(lua, 1);
	if(ret == -1 || ret == 0 ) return FALSE;
	else{
		addexp = &ret;
		return TRUE;
	} 
}
*/
/*BOOL FreeLoginCheck(int fd)
{
	static lua_State *lua;
	if (lua == NULL){
		lua = FindLua("data/ablua/freelogincheck.lua");
		if (lua == NULL)return FALSE;
	}

	lua_getglobal(lua, "FreeLoginCheck");

	if (!lua_isfunction(lua, -1)) {
		lua_pop(lua, 1);
		return FALSE;
	}
	lua_pushnumber(lua,fd); 

	if (lua_pcall(lua, 1, 1, 0) != 0)
		error(lua, "error running function `f': %s", lua_tostring(lua, -1));

	BOOL ret = (BOOL)lua_toboolean(lua, -1);
	lua_pop(lua, 1);
	return ret;
}
*/

#ifdef _CAX_ESC_REPORT
void report1( int charaindex ){
	int fd,itemindex,emptyitemindexinchara,count,countReport = 0,i;
	char token[256];
	if( (fd = getfdFromCharaIndex( charaindex)) == -1 ) return;
		if(CHAR_getInt(charaindex,CHAR_TRANSMIGRATION) < getReportTa() ||
				(CHAR_getInt(charaindex,CHAR_TRANSMIGRATION) >= getReportTa() && CHAR_getInt(charaindex,CHAR_LV) < getReportLv())){
				sprintf( token, "\n\n\n    每日签到只针对于%d转%d级以上的玩家开放。", getReportTa(),getReportLv());
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,token);
				return;
		}
		//判断是否签到
		int day;
		time_t t;
		struct tm *local;
		t=time(NULL);
		local=localtime(&t);
		day = (local->tm_year+1900) * 10000 + (local->tm_mon+1) * 100 + local->tm_mday;
		if(CHAR_getInt(charaindex,CHAR_REPORT) == day){//已经签到
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,"\n\n\n    你今天已经签到过了，请明天再来。");
			return;
		}

		//========普通玩家签到=========
		if(CHAR_getInt( charaindex , CHAR_VIPRIDE)== 0){
			//获得玩家背包空格数
			count = CHAR_findSurplusItemBox( charaindex );
			//获得赠送物品数
			for (i = 0; i < 10; i++) {
				if(getReportItem(i) !=-1){
					countReport++;
				}
			}
			if(count < countReport){
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,"\n\n\n    空间栏位不足。");
				return;
			}
			for (i = 0; i < 10; i++) {
				if(getReportItem(i) !=-1){
					#ifdef _ITEM_UP
					itemindex = ITEM_makeItemAndRegist( getReportItem(i) , charaindex);//获取一个道具索引
					#else
					itemindex = ITEM_makeItemAndRegist( getReportItem(i) );//获取一个道具索引
					#endif
					if( itemindex != -1 ){
						emptyitemindexinchara = CHAR_findEmptyItemBox( charaindex );//获取背包空间
						if( emptyitemindexinchara < 0 ){
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,"\n\n\n    空间栏位不足。");
							return;
						}
						CHAR_setItemIndex(charaindex, emptyitemindexinchara, itemindex );
						ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
						ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charaindex);
						CHAR_sendItemDataOne( charaindex, emptyitemindexinchara);
						sprintf( token, "获得%s。", ITEM_getChar( itemindex, ITEM_NAME));
						CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
						CHAR_setInt(charaindex,CHAR_REPORT,day);
					}
				}
			}
		}else{
			//========VIP玩家签到=========
			//获得玩家背包空格数
			count = CHAR_findSurplusItemBox( charaindex );
			//获得赠送物品数
			for (i = 0; i < 10; i++) {
				if(getVipReportItem(i) !=-1){
					countReport++;
				}
			}
			if(count < countReport){
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,"\n\n\n    空间栏位不足。");
				return;
			}
			for (i = 0; i < 10; i++) {
				if(getVipReportItem(i) !=-1){
					#ifdef _ITEM_UP
					itemindex = ITEM_makeItemAndRegist( getVipReportItem(i) , charaindex );//获取一个道具索引
					#else
					itemindex = ITEM_makeItemAndRegist( getVipReportItem(i) );//获取一个道具索引
					#endif
					if( itemindex != -1 ){
						emptyitemindexinchara = CHAR_findEmptyItemBox( charaindex );//获取背包空间
						if( emptyitemindexinchara < 0 ){
							lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,"\n\n\n    空间栏位不足。");
							return;
						}
						CHAR_setItemIndex(charaindex, emptyitemindexinchara, itemindex );
						ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
						ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charaindex);
						CHAR_sendItemDataOne( charaindex, emptyitemindexinchara);
						sprintf( token, "拿到%s。", ITEM_getChar( itemindex, ITEM_NAME));
						CHAR_talkToCli( charaindex, -1, token, CHAR_COLORYELLOW);
						CHAR_setInt(charaindex,CHAR_REPORT,day);
						lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,"\n\n\n    签到成功，欢迎你明天再来签到。");
					}
				}
			}
		}
		CHAR_charSaveFromConnect(fd,FALSE);
	}
#endif

#ifdef _CAX_ESC_SAVE_DATA
void SaveData(int charaindex){
	int fd;
	if( (fd = getfdFromCharaIndex( charaindex)) == -1 ) return;
		//int ownerindex = CHAR_getWorkInt(petindex, CHAR_WORKPLAYERINDEX);
	CHAR_charSaveFromConnect(charaindex,FALSE);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1,-1,"\n\n自动存档成功。");
}
#endif

BOOL FreeSamenu(int* function,int id, int charaindex )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/freesamenu.lua");
	  if (lua == NULL)return TRUE;
	}

	lua_getglobal(lua, "ESC_Recv");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
   int TM_Ret = 0;
#ifdef _CAX_ESC_REPORT
   if ( id == 6){
   	report1(charaindex);
   	//CHAR_talkToCli(charaindex,-1,"测试是否可以内部执行",CHAR_COLORRED);
   	return;
  }
#endif
#ifdef _CAX_ESC_SAVE_DATA
  if ( id == 7){
   	SaveData(charaindex);
   	//CHAR_talkToCli(charaindex,-1,"测试是否可以内部执行",CHAR_COLORRED);
   	return;
  }
#endif
  //依次放入二个参数
  //lua_pushstring(lua,message); 
  lua_getglobal(lua, (const char*)function);
  lua_pushinteger(lua, (lua_Integer)id);
	lua_pushnumber(lua,charaindex);
  TM_Ret = lua_pcall(lua, 2, 0, 0);
	if(TM_Ret != 0)
	{
		print("NPC_Lua_ESC_Recv Lua Err :%d(%s)\n", TM_Ret, lua_tostring(lua, -1));
		lua_pop(lua, 1);
		return;
	}
}
/*
BOOL TeacherButton(int* function,int charaindex )
{
	static lua_State *lua;

  if (lua == NULL){
  	lua = FindLua("data/ablua/TeacherButton.lua");
	  if (lua == NULL)return TRUE;
	}

	lua_getglobal(lua, "TeacherButton");

	if (!lua_isfunction(lua, -1)) {
    lua_pop(lua, 1);
    return TRUE;
  }
   int TM_Ret = 0;
  //依次放入二个参数
  //lua_pushstring(lua,message); 
  lua_getglobal(lua, (const char*)function);
	lua_pushinteger(lua, (lua_Integer)charaindex);
  TM_Ret = lua_pcall(lua, 1, 0, 0);
	if(TM_Ret != 0)
	{
		print("TeacherButton Lua Err :%d(%s)\n", TM_Ret, lua_tostring(lua, -1));
		lua_pop(lua, 1);
		return;
	}
}
	  
*/

BOOL FreeCharExpSave(int charaindex, int exp )
{
	//printf("exp【1】 = %d \n",exp);
	static lua_State *lua;
	if (lua == NULL){
		lua = FindLua("data/ablua/freecharexpsave.lua");
		if (lua == NULL)return FALSE;
	}
	lua_getglobal(lua, "FreeCharExpSave");

	if (!lua_isfunction(lua, -1)) {
		lua_pop(lua, 1);
		return TRUE;
	}
	int TM_Ret = 0;
	
	lua_pushnumber(lua,charaindex);
	lua_pushinteger(lua, (lua_Integer)exp);

  TM_Ret = lua_pcall(lua, 2, 0, 0);
  if(TM_Ret != 0)
	{
		print("FreeCharExpSave Lua Err :%d(%s)\n", TM_Ret, lua_tostring(lua, -1));
		lua_pop(lua, 1);
		return;
	}
}

#endif

#endif


