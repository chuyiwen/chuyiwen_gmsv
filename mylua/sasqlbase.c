#include <string.h>
#include "lua.h"
#include "version.h"
#include "lauxlib.h"
#include "lualib.h"
#include "net.h"
#include "log.h"
#include "sasql.h"

#ifdef _ALLBLUES_LUA
#ifdef _ALLBLUES_LUA_1_4
#ifdef _SASQL
#include <mysql/mysql.h>
extern MYSQL mysql;
MYSQL_RES *mysql_result;
MYSQL_ROW mysql_row;
MYSQL_FIELD *fields;
int numrow = 0;
int numfields = 0;
static int setVipPoint(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int point = luaL_checkint(L, 2);

  sasql_setVipPoint(charaindex, point);
	return 1;
}

static int setJfPoint(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int point = luaL_checkint(L, 2);

  sasql_setJfPoint(charaindex, point);
	return 1;
}

static int setxjPoint(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);
	const int point = luaL_checkint(L, 2);

  sasql_setxjPoint(charaindex, point);
	return 1;
}

static int getVipPoint(lua_State *L) 
{
	const int charaindex = luaL_checkint(L, 1);

	lua_pushinteger(L, sasql_getVipPoint(charaindex));
	return 1;
}

static int getJfPoint(lua_State *L) 
{
//	printf("第一次访问\n");
	const int charaindex = luaL_checkint(L, 1);

	lua_pushinteger(L, sasql_jifenPoint(charaindex));
	return 1;
}

static int getxjPoint(lua_State *L) 
{
//	printf("第一次访问\n");
	const int charaindex = luaL_checkint(L, 1);

	lua_pushinteger(L, sasql_xjPoint(charaindex));
	return 1;
}

static int setVipPointForCdkey(lua_State *L) 
{
	size_t l;
	char *id=luaL_checklstring(L, 1, &l);
	const int point = luaL_checkint(L, 2);

  sasql_setVipPointForCdkey(id, point);
	return 1;
}

static int getVipPointForCdkey(lua_State *L) 
{
	size_t l;
	char *id=luaL_checklstring(L, 1, &l);
	lua_pushinteger(L, sasql_getVipPointForCdkey(id));
	return 1;
}

static int query(lua_State *L) 
{
	size_t l;
	char *data=luaL_checklstring(L, 1, &l);

	if(!sasql_mysql_query(data)){
		lua_pushinteger(L, 1);
	}else{
		lua_pushinteger(L, 0);
	}
	return 1;
}

static int free_result(lua_State *L) 
{
	mysql_free_result(mysql_result);
	return 1;
}

static int store_result(lua_State *L) 
{
	 mysql_result=mysql_store_result(&mysql);
	return 1;
}

static int num_rows(lua_State *L) 
{
	numrow=mysql_num_rows(mysql_result);
	lua_pushinteger(L, numrow);
	return 1;
}

static int num_fields(lua_State *L) 
{
	lua_pushinteger(L, numfields);
	return 1;
}

static int fetch_row(lua_State *L) 
{
	mysql_row = mysql_fetch_row(mysql_result);
	return 1;
}

static int data(lua_State *L) 
{
	int id = luaL_checkint(L, 1) - 1;
	lua_pushstring(L, mysql_row[id]);
	return 1;
}

static const luaL_Reg sasqllib[] = {
  {"setVipPoint", 						setVipPoint},
  {"getVipPoint", 						getVipPoint},
  {"setJfPoint", 						  setJfPoint},
  {"getJfPoint", 						  getJfPoint},
  {"setxjPoint", 						  setxjPoint},
  {"getxjPoint", 						  getxjPoint},
	{"query", 									query},
	{"free_result", 						free_result},
	{"store_result", 						store_result},
	{"num_rows", 								num_rows},
	{"fetch_row", 							fetch_row},
	{"num_fields", 							num_fields},
	{"data", 										data},
	{"setVipPointForCdkey", 		setVipPointForCdkey},
  {"getVipPointForCdkey", 		getVipPointForCdkey},
  {NULL, 							NULL}
};

LUALIB_API int luaopen_Sasql (lua_State *L) {
  luaL_register(L, "sasql", sasqllib);
  return 1;
}
#endif
#endif
#endif
