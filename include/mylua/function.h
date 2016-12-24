
#ifndef __MYLUA_FUNCTION_H__
#define __MYLUA_FUNCTION_H__

BOOL RunCharTalkedEvent(int meindex, int toindex, char *messageeraseescape, int color, int channel);
BOOL RunCharLoopEvent(int meindex);
BOOL RunCharOverlapEvent( int meindex, int toindex);
BOOL RunCharBattleOverEvent( int meindex, int toindex, int iswin);
BOOL RunCharWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

#ifdef _ALLBLUES_LUA_1_2 
BOOL RunItemUseEvent( int itemindex, int charaindex, int toindex, int haveitemindex );
BOOL RunItemDieReLifeEvent( int toindex, int itemindex, int haveitemindex );
BOOL RunItemDetachEvent( int charaindex, int itemindex );
BOOL RunItemAttachEvent( int charaindex, int itemindex );
BOOL RunItemPickupEvent( int charaindex, int itemindex );
BOOL RunItemPostOverEvent( int itemindex, int charaindex );
BOOL RunItemPreOverEvent( int itemindex, int charaindex );
BOOL RunItemDropEvent( int charaindex, int itemindex );
BOOL RunUseChatMagic( int charaindex, char *data, lua_State *lua);
#endif
#ifdef _PETSKILL_SHOP_LUA   
BOOL FreePetSkillShop( int talkerindex, int petindex, int oldSkillID, int newSkillID);
#endif
//#ifdef _PETSKILL_SHOP_LUA   
//BOOL OffLineCommand( int battleindex, int charindex, int side);
//#endif
#ifdef _ALLBLUES_LUA_1_4
BOOL BattleFinish( int battleindex, int charaindex);
BOOL BattleEscape( int battleindex, int charaindex);
BOOL RunCharLogOutEvent( int charaindex);
#endif
#ifdef _ALLBLUES_LUA_1_5
BOOL FreePartyJoin( int charaindex, int toindex );
BOOL FreeVsPlayer( int charaindex, int toindex );
BOOL FreeCharLogin( int charaindex );
BOOL FreeCharCreate( int charaindex );
BOOL NetLoopFunction( void );
#endif
#ifdef _ALLBLUES_LUA_1_6
BOOL FamilyRideFunction( int meindex, int petindex, int petid );
BOOL CharTalkFunction( int charaindex, char *message, int color );
#endif
#ifdef _ALLBLUES_LUA_1_7
BOOL CharVsEnemyFunction( int charaindex );
#endif
#ifdef _ALLBLUES_LUA_1_8
BOOL CaptureOkFunction( int attackindex, int defindex );
BOOL CaptureCheckFunction( int attackindex, int defindex );
#endif
#ifdef _ALLBLUES_LUA_1_9
BOOL SetBattleEnmeyFunction(int meindex, int enemyindex, int id );
BOOL BattleFinishFunction( int charaindex, int battletime, int battleturn, int battletype );
#endif
#ifdef _ALLBLUES_LUA_2_0
BOOL fameButton( int index);  //声望按钮接口
BOOL FreeSamenu(int* function,int id, int charaindex ); //ESC
BOOL FreeLoginCheck(int fd);//玩家登陆回调
BOOL TeacherButton(int* function,int charaindex );//导师按钮回调
BOOL FreeCharExpSave(int charaindex, int exp );
BOOL FreeModeExp(int charaindex, int exp );//经验回调
#endif
#endif
