#include "npc_lua.h"
#include "util.h"
#include "char.h"
#include "char_base.h"
#include "anim_tbl.h"
#include "object.h"
#include "net.h"
#include "npcutil.h"
#include "npc_eventaction.h"
#include "battle.h"
#include "readmap.h"

#ifdef _JZ_NEWSCRIPT_LUA

extern char M_OutErrMsg[1024];
extern lua_State *M_Script_Lua;
extern unsigned int M_Create_Num;

//////////////////////////////////////////////////////////////////////////////
int NPC_Lua_RegCallEx(lua_State * _NLL, luaL_Reg *_RegList)
{
	assert(_RegList != NULL && _NLL != NULL);

	int i = 0;
	if((_NLL != NULL) && (_RegList != NULL))
	{
		while(_RegList->func != NULL)
		{
			lua_register(_NLL, _RegList->name, _RegList->func);
			i++;
			_RegList++;
		}
	}
	return i;
}

int NPC_Lua_RegLibEx(lua_State * _NLL, PSCRIPTREGLIB _RegLib)
{
	assert(_RegLib != NULL && _NLL != NULL);

	int i = 0;
	if((_NLL != NULL) && (_RegLib != NULL))
	{
		while(_RegLib->LibName != NULL)
		{
			luaL_openlib(_NLL, _RegLib->LibName, _RegLib->FuncRegList, 0);
			i++;
			_RegLib++;
		}
	}
	return i;
}

int NPC_Lua_RegClassEx(lua_State * _NLL, PSCRIPTREGCLASS _RegClass)
{
	assert(_RegClass != NULL && _NLL != NULL);
	
	int i = 0;
	if((_NLL != NULL) && (_RegClass != NULL))
	{
		while(_RegClass->ClassName != NULL)
		{
			luaL_newmetatable(_NLL, _RegClass->ClassName);
	
			lua_pushstring(_NLL, "__index");
			lua_pushvalue(_NLL, -2);
			lua_settable(_NLL, -3);
	
			luaL_openlib(_NLL, NULL, _RegClass->FuncRegList, 0);
	
			luaL_openlib(_NLL, _RegClass->NewLib , _RegClass->NewLib->FuncRegList, 0);
			
			i++;
			_RegClass++;
		}
	}
	return i;
}

int NPC_Lua_RegArrayEx(lua_State * _NLL, PSCRIPTREGARRAY _RegArray)
{
	assert(_RegArray != NULL && _NLL != NULL);
	
	int i = 0;
	if((_NLL != NULL) && (_RegArray != NULL))
	{
		int j = 0;
		while(_RegArray->ArrayName != NULL)
		{
			luaL_newmetatable(_NLL, _RegArray->ArrayName);
			luaL_openlib(_NLL, _RegArray->FuncList->LibName, _RegArray->FuncList->FuncRegList, 0);

			j = 0;
			while(_RegArray->SetMetaTable[j] != NULL)
			{
				lua_pushstring(_NLL, _RegArray->SetMetaTable[j]);
				lua_pushstring(_NLL, _RegArray->SetFuncName[j]);
				lua_gettable(_NLL, 2);
				lua_settable(_NLL, 1);
				j++;
			}
			i++;
			_RegArray++;
		}
	}
	return i;
}

const char *NPC_Lua_SetErrorStr(const char *ErrStr)
{
	if(ErrStr == NULL)
	{
		M_OutErrMsg[0] = '\0';
		return M_OutErrMsg;
	}
	return (const char *)strcpy_s(M_OutErrMsg, sizeof(M_OutErrMsg), ErrStr);
}

//����NPC-����NPCΨһ����
int NPC_Lua_Create(const char *_DoFile, const char *_InitFuncName, char *_seek, BOOL _IsFly)
{
	assert(_InitFuncName != NULL);
	
	Char TM_char;
	Object TM_obj;
	int TM_charaindex;
	int TM_objindex;
	int TM_seekint = 0;

	//����ָ��Ĭ��ֵ��TM_char
	if(_seek == NULL)
	{
		TM_seekint = CHAR_seekGraphicNumberFromString("\0");
	}else
	{
		TM_seekint = CHAR_seekGraphicNumberFromString(_seek);
	}

	if( CHAR_getDefaultChar( &TM_char, TM_seekint) == FALSE )
	{
		LRetErrInt(M_Script_Lua, -1, "��������Ĭ��ֵʧ�ܡ������ڲ����ܲ��������");
	}
	
  //����ָ����LUA�ű��ļ�
	if(_DoFile != NULL && _DoFile[0] != '\0')
	{
		if (NPC_Lua_DoFile(_DoFile) == 1){
#ifdef _LUA_Debug
			pcall_callback_err_fun(M_Script_Lua,_InitFuncName);
#endif
		}
	}
	
	//����ָ����LUA�ű��ļ� CAX
//	NPC_Lua_DoFile(_DoFile);
	//ȡ�ýű��ж���ĳ�ʼ������ָ��
	lua_getglobal(M_Script_Lua, _InitFuncName);

	if(lua_type(M_Script_Lua, -1) != LUA_TFUNCTION)
	{
		print("NPC_Lua_Create err: �޷���ȡ��ʼ������ָ�롣\n");
		lua_pop(M_Script_Lua, 1);
		LRetErrInt(M_Script_Lua, -2, "�޷���ȡ��ʼ������ָ�롣");
	}else
	{
		//���溯��ָ��
		lua_pop(M_Script_Lua, 1);
		strcpy_s(TM_char.lua_charfunctable[CHAR_INITFUNC].string, sizeof(TM_char.lua_charfunctable[CHAR_INITFUNC].string), _InitFuncName);
		strcpy_s(TM_char.charfunctable[CHAR_INITFUNC].string, sizeof(TM_char.charfunctable[CHAR_INITFUNC].string), FUNCNAME_INITCALLBACK);
	}
	TM_char.data[CHAR_FLOOR] = 777;
	TM_char.data[CHAR_X] = 31;
	TM_char.data[CHAR_Y] = 19;
	TM_char.data[CHAR_DIR] = 4;
	TM_char.string[CHAR_NAME].string[0] = '\0';
	TM_char.data[CHAR_BASEBASEIMAGENUMBER] = TM_char.data[CHAR_BASEIMAGENUMBER] = 0;
#ifdef _ADD_ACTION    
	TM_char.data[CHAR_ACTIONSTYLE] = CHAR_ACTSTAND;
#endif
	TM_char.data[CHAR_FMINDEX] = -1;
	TM_char.string[CHAR_NPCARGUMENT].string[0] = '\0';
	//��������
	TM_char.data[CHAR_WHICHTYPE] = CHAR_LUANPC;
	//�������ʶ��NPC��NPC_Lua_Create����������NPC��ţ�Ŀǰûʲô��
	TM_char.data[CHAR_NPCCREATEINDEX] = M_Create_Num++;
	
	//ʵ����TM_char ��������֮��NPCʵ�����ݲ�����
	TM_charaindex = CHAR_initCharOneArray( &TM_char );
	if( TM_charaindex == -1 )
	{
		CHAR_endCharData( &TM_char );
		LRetErrInt(M_Script_Lua, -3, "�޷�ʵ����NPC���ݡ�");
	}

	CHAR_setInt(TM_charaindex, CHAR_INDEXOFEQTITLE, -1);
	CHAR_setWorkInt( TM_charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_NONE);

	//��ͼ��־����Ŀǰ��֪�����ã�
	if(_IsFly == TRUE)
	{
		CHAR_setFlg(TM_charaindex, CHAR_ISFLYING, 1);
	}

	TM_obj.type = OBJTYPE_CHARA;					//��������
	TM_obj.index= TM_charaindex;					//�����Ӧ��Char����
	TM_obj.x = CHAR_getInt(TM_charaindex, CHAR_X);		//�������ڵ�ͼ��X����
	TM_obj.y = CHAR_getInt(TM_charaindex, CHAR_Y);		//�������ڵ�ͼ��Y����
	TM_obj.floor = CHAR_getInt(TM_charaindex, CHAR_FLOOR);//�������ڵ�ͼ
	TM_objindex = initObjectOne( &TM_obj );				//����һ������ʵ����������֮��NPCʵ�������
	if( TM_objindex == -1 )
	{
		CHAR_endCharOneArray( TM_charaindex );
		LRetErrInt(M_Script_Lua, -4, "�޷�ʵ����NPC����");
  }

	CHAR_setWorkInt(TM_charaindex, CHAR_WORKOBJINDEX, TM_objindex);

#ifdef _ADD_ACTION
	//��������NPC�Ķ���
	CHAR_setWorkInt(TM_charaindex, CHAR_WORKACTION, CHAR_getInt( TM_charaindex, CHAR_ACTIONSTYLE ) );

	CHAR_sendWatchEvent(TM_objindex, CHAR_getWorkInt( TM_charaindex, CHAR_WORKACTION), NULL, 0, TRUE);
#else
	CHAR_sendWatchEvent(TM_objindex , CHAR_ACTSTAND, NULL, 0, TRUE);
#endif
	CHAR_setWorkInt(TM_charaindex,CHAR_WORKNPCTYPE,2);
	LRetInt(M_Script_Lua, TM_charaindex);
}

//ɾ��NPC ����NPCΨһ����
int NPC_Lua_Del(int _index)
{
	if (CHAR_getInt( _index, CHAR_WHICHTYPE )== CHAR_LUANPC)
	{
		int TM_Objindex = CHAR_getWorkInt( _index,CHAR_WORKOBJINDEX);
		if(CHECKOBJECTUSE(TM_Objindex) == TRUE)
		{
			CHAR_ObjectDelete(TM_Objindex);
			CHAR_endCharOneArray(_index);
			return 1;
		}else
		{
			CHAR_endCharOneArray(_index);
			return 0;
		}
	}
}

int NPC_Lua_GetFuncPoint(lua_State *_NLL)
{
	CheckEx2(_NLL, 2, LUA_MINSTACK);
	char *TM_DoFile = lua_tostring(_NLL, 1);
	char *TM_FuncName = lua_tostring(_NLL, 2);

	if(TM_DoFile != NULL)
	{
		if (NPC_Lua_DoFile(TM_DoFile) == 1){
#ifdef _LUA_Debug
			pcall_callback_err_fun(M_Script_Lua,TM_FuncName);
#endif
		}
	}

	if(TM_FuncName == NULL)
	{
		LRetErrInt(_NLL , -1, "����������Ϊ��");
	}

	lua_getglobal(_NLL, TM_FuncName);
	
	if(lua_type(_NLL, -1) != LUA_TFUNCTION)
	{
		LRetErrInt(_NLL , -2, "�޷���ȡ����ָ�롣");
	}
	LRetInt(_NLL, 1);
}
//////////////////////////////////////////////////////////////////////////////
//��չ���ͽӿں���
//NTInt
int NPC_Lua_NTInt_New(lua_State *_NLL)
{
	CheckEx(_NLL, 1);

	int TM_Len = luaL_checkint(_NLL, 1);
	if(TM_Len <= 0)
	{
		LRetErrInt(_NLL, -1, "��С����Ϊ0��");
	}

	size_t TM_Bytes = sizeof(size_t) + ( sizeof(int) * TM_Len );
	PARRAY_NTINT TM_Point = (PARRAY_NTINT *)lua_newuserdata(_NLL, TM_Bytes);
	
	if(TM_Point == NULL)
	{
		LRetErrInt(_NLL, -2, "ʵ����ʧ�ܣ����������ڴ档");
	}

	luaL_getmetatable(_NLL, "Array.NTInt");
	lua_setmetatable(_NLL, -2);

	TM_Point->Size = TM_Len;

	LRet(1);
}

int NPC_Lua_NTInt_Get(lua_State *_NLL)
{
	CheckEx(_NLL, 2);

	PARRAY_NTINT TM_Point = luaL_checkudata(_NLL, 1, "Array.NTInt");

	if(TM_Point == NULL)
	{
		luaL_argerror(_NLL, 1, "���� Array.NTInt ���ݡ�");
	}

	int TM_Index = (int)lua_tointeger(_NLL, 2);
	
	if(TM_Index < 1 || TM_Index > TM_Point->Size)
	{
		luaL_argerror(_NLL, 1, "�������ݷ�Χ�������������");
	}

	TM_Index--;

	LRetInt(_NLL, TM_Point->Num[TM_Index]);
}

int NPC_Lua_NTInt_Set(lua_State *_NLL)
{
	CheckEx(_NLL, 3);
	
	PARRAY_NTINT TM_Point = luaL_checkudata(_NLL, 1, "Array.NTInt");
	
	if(TM_Point == NULL)
	{
		luaL_argerror(_NLL, 1, "���� Array.NTInt ���ݡ�");
	}

	int TM_Index = (int)lua_tointeger(_NLL, 2);
	
	if(TM_Index < 1 || TM_Index > TM_Point->Size)
	{
		luaL_argerror(_NLL, 1, "�������ݷ�Χ�������������");
	}
	TM_Index--;

	int TM_Num = (int)lua_tointeger(_NLL, 3);

	TM_Point->Num[TM_Index] = TM_Num;

	LRetNull(_NLL);
}

int NPC_Lua_NTInt_Len(lua_State *_NLL)
{
	CheckEx(_NLL, 1);
	
	PARRAY_NTINT TM_Point = luaL_checkudata(_NLL, 1, "Array.NTInt");

	if(TM_Point == NULL)
	{
		luaL_argerror(_NLL, 1, "���� Array.NTInt ���ݡ�");
	}

	LRetInt(_NLL, TM_Point->Size);
}

#endif //#ifdef _JZ_NEWSCRIPT_LUA
