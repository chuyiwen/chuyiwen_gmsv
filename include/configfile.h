#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#include "common.h"

#undef EXTERN
#ifdef __CONFIGFILE_C__
#define EXTERN
#else  /*  __CONFIGFILE_C__  */
#define EXTERN extern
#endif /*  __CONFIGFILE_C__  */

// Arminius 7.12 login announce
extern char announcetext[8192];
void AnnounceToPlayer(int charaindex);
// Robin 0720
void AnnounceToPlayerWN(int fd);
void LoadAnnounce(void);

#ifdef _PET_TALKPRO
	#define PETTALK_MAXID 10        //增加pettalk数量到10
	typedef struct {
		int ID;
		char DATA[1024*64];	//1M
	}PTALK;
	extern PTALK pettalktext[PETTALK_MAXID];
	void LoadPetTalk(void);
#else
	
extern char pettalktext[4096];
void LoadPetTalk(void);
#endif

#ifdef _GAMBLE_BANK
#define GAMBLEBANK_ITEMSMAX 100
#define DEFEND_ITEMSMAX 40
typedef struct	REGAMBLEBANKITEMS	{
	char name[128];
	int Gnum;
	int ItemId;
	int type;
}GAMBLEBANK_ITEMS;

extern GAMBLEBANK_ITEMS GB_ITEMS[GAMBLEBANK_ITEMSMAX];

void Load_GambleBankItems( void);
#endif

#ifdef _CFREE_petskill
#define PETSKILL_CODE 500
typedef struct	REPETSKILL_CODES	{
	char name[128];
	int TempNo;
	int PetId;
	char Code[256];
}PETSKILL_CODES;

extern PETSKILL_CODES Code_skill[PETSKILL_CODE];
void Load_PetSkillCodes( void);
#endif

#ifdef _GMRELOAD
#define GMMAXNUM	100
struct GMINFO
{
	char cdkey[24];
	int  level;
};
extern struct GMINFO gminfo[GMMAXNUM];
#endif

BOOL readconfigfile( char* filename );

void  defaultConfig( char* argv0 );
char* getProgname( void );
char* getConfigfilename( void );
void setConfigfilename( char* newv );

unsigned int getDebuglevel( void );
unsigned int setDebuglevel( unsigned int newv );
unsigned int getMemoryunit( void );
unsigned int getMemoryunitnum( void );
char*   getAccountservername( void );
unsigned short   getAccountserverport( void );
char*   getAccountserverpasswd( void );
char*   getGameservername( void );
unsigned short getPortnumber( void );

int getServernumber( void );
int getReuseaddr( void );
int getNodelay( void );
int getLogWriteTime(void);
int getLogIOTime( void);
int getLogGameTime(void);
int getLogNetloopFaster(void);
int getSaacwritenum( void );
void setSaacwritenum( int num );
int getSaacreadnum( void );
void setSaacreadnum( int num );

unsigned int getFdnum( void );
unsigned int setFdnum(int temp_fdnum);
unsigned int getPlayercharnum( void );
unsigned int getPetcharnum( void );
unsigned int getOtherscharnum( void );
unsigned int getObjnum( void );
unsigned int getItemnum( void );
unsigned int getBattlenum( void );

char* getTopdir( void );
char* getMapdir( void );
char* getMaptilefile( void );
char* getBattleMapfile( void );
char* getItemfile( void );
char* getInvfile( void );
char* getAppearfile( void );
char* getEffectfile( void );
char* getTitleNamefile( void );
char* getTitleConfigfile( void );
char* getLsgenlogfilename( void );
char* getStoredir( void );
#ifdef _STORECHAR
char* getStorechar( void );
#endif
char* getNpcdir( void );
char* getLogdir( void );
char* getLogconffile( void );
char* getChatMagicPasswd( void );
unsigned int getChatMagicCDKeyCheck( void );


unsigned int getFilesearchnum( void );
unsigned int getNpctemplatenum( void );
unsigned int getNpccreatenum( void );
unsigned int getWalksendinterval( void );
void		 setWalksendinterval( unsigned int );
unsigned int getCAsendinterval_ms( void );
void 		 setCAsendinterval_ms( unsigned int );
unsigned int getCDsendinterval_ms( void );
void		 setCDsendinterval_ms( unsigned int );
unsigned int getOnelooptime_ms( void );
void		 setOnelooptime_ms( unsigned int );
unsigned int getCharSavesendinterval( void );
void setCharSavesendinterval( unsigned int interval);
unsigned int getAddressbookoffmsgnum( void );
unsigned int getProtocolreadfrequency( void );
unsigned int getAllowerrornum( void );
unsigned int getFengerrornum( void );
unsigned int getLogHour( void );
unsigned int getBattleDebugMsg( void );
void setBattleDebugMsg( unsigned int );

char* getEncountfile( void );
char* getEnemyfile( void );
char* getGroupfile( void );
char* getEnemyBasefile( void );
char* getMagicfile( void );
#ifdef _ATTACK_MAGIC

char* getAttMagicfileName( void );

#endif

char* getPetskillfile( void );
char *getItematomfile( void );
char *getQuizfile( void );

#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
char* getProfession( void );
#endif

unsigned int getPetdeletetime( void );
void setPetdeletetime( unsigned int interval );

unsigned int getItemdeletetime( void );
void setItemdeletetime( unsigned int interval );
//ttom add this because the second have this function
//unsigned int getAcwriteSize( void );

#ifdef _DEL_DROP_GOLD
unsigned int getGolddeletetime( void );
void setGolddeletetime( unsigned int interval );
#endif

unsigned int setEncodeKey( void );
unsigned int setAcWBSize( void );
//ttom end

// CoolFish: +2 2001/4/18
unsigned int getAcwriteSize( void );
unsigned int getErrUserDownFlg( void );

// Arminius 7.24 manor pk
char* getGameserverID( void );
unsigned short getAllowManorPK( void );

// Terry 2001/10/03 service ap
char* getApID(void);
unsigned short getApPort(void);
int getLoopTime(void);
int getEnableService(void);

#ifdef _GMRELOAD
char* getGMSetfile(void);
BOOL LoadGMSet(char *filename);
#endif

#ifdef _ITEM_QUITPARTY
char* getitemquitparty( void );
#endif

#ifdef _NEW_PLAYER_CF	
int getNewplayertrans( void );
int getNewplayerlv( void );
int getNewplayergivepet( unsigned int index );
void setNewplayergivepet( unsigned int index ,unsigned int interval);
int getNewplayerpetlv( void );
int getNewplayergivegold( void );
int getRidePetLevel( void );
#ifdef _VIP_SERVER
int getNewplayergivevip( void );
#endif
#endif

#ifdef _USER_EXP_CF
BOOL LoadEXP( char* filename );
char* getEXPfile( void );
int getNeedLevelUpTbls( int level );
#endif

#ifdef _UNREG_NEMA
char* getUnregname( int index );
#endif

#ifdef _TRANS_LEVEL_CF
int getChartrans( void );
int getPettrans( void );
int getYBLevel( void );
int getMaxLevel( void );
#endif

#ifdef _POINT
char* getPoint( void );
int getTransPoint( int index );
#endif

#ifdef _PET_UP
int getPetup( void );
#endif
#ifdef _WATCH_FLOOR
int getWatchFloor( unsigned int index );
char* getWatchFloorCF( void );
#endif

#ifdef _BATTLE_FLOOR
int getBattleFloor( unsigned int index );
char* getBattleFloorCF( void );
#endif

#ifdef _LOOP_ANNOUNCE
char* getLoopAnnouncePath( void );
int loadLoopAnnounce( void );
int getLoopAnnounceTime( void );
char* getLoopAnnounce( int index );
int getLoopAnnounceMax( void );
#endif

#ifdef _SKILLUPPOINT_CF
int getSkup( void );
#endif
#ifdef _RIDELEVEL
int getRideLevel( void );
#endif
#ifdef _REVLEVEL
char* getRevLevel( void );
#endif
#ifdef _NEW_PLAYER_RIDE
char* getPlayerRide( void );
#endif
#ifdef _FIX_CHARLOOPS
int getCharloops( void );
#endif
#ifdef _PLAYER_ANNOUNCE
int getPAnnounce( void );
#endif
#ifdef _PLAYER_MOVE
int getPMove( void );
#endif

int getrecvbuffer( void );
int getsendbuffer( void );
int getrecvlowatbuffer( void );
int getrunlevel( void );

#ifdef _SHOW_VIP_CF
int getShowVip( void );
#endif

#ifdef _PLAYER_NUM
int getPlayerNum( void );
void setPlayerNum( int num );
char* getPlayerBase( void );
#endif
#ifdef _BATTLE_GOLD
int getBattleGold( void );
#endif

#ifdef _ANGEL_TIME
int getAngelPlayerTime( void );
int getAngelPlayerMun( void );
#endif

#ifdef _RIDEMODE_20
int getRideMode( void );
#endif

#ifdef _FM_POINT_PK
char *getFmPointPK( void );
#endif

#ifdef _ENEMY_ACTION
int getEnemyAction( void );
#endif

#ifdef _FUSIONBEIT_TRANS
int getFusionbeitTrans( void );
#endif

char *getMacCheck( void );
int getCpuUse( void );

#ifdef _CHECK_PEPEAT
int getCheckRepeat( void );
#endif

#ifdef _VIP_ALL
int getCheckVip( void );
#endif
#ifdef _FM_JOINLIMIT
int getJoinFamilyTime( void );
#endif

#ifdef _NOWEN_EV
int getNowEvent(int Num);
int getEndEvent(int Num);
#endif

#ifdef _JK_CF_DELPETITEM        //删除CF指定物品和宠物
char *getDelPet(void);
char *getDelItem(void);
#endif
int getBalltePoint(int Num);
#ifdef _ALL_TALK
int getTheWorldTrans( void );
int getTheWorldFame( void );
int getTheWorldTime( void );
int getTheWorldCnt( void );
#endif
#ifdef _DIY_INSLAY
int getInslayNum(void);
#endif
int getSafeMode(void);
int getTradeTax(void);
#ifdef _MAP_HEAL
int getMapHeal(int Num);
#endif
#ifdef _VIP_RIDE
int getVipMsgType(void);
#endif
#ifdef _VIP_BATTLE_EXP
int getVipBattleExp( void );
#endif
int getItemPoolBug(void);
int getSameIpLogin(void);
int getPetRideTrans(void);
#ifdef _LUCK_MAN
int getLuckTime( void );
int getLuckItem( void );
#endif
#ifdef _QUESTION_ONLINE
int getQustionTime( void );
int getQuestionItem( int num );
#endif
#ifdef _NO_STREET_MAP
int getNoStreetMap( int num );
#endif
#ifdef _STREET_FAX
int getStreetFax( int num );
#endif
int getFmWarTime( int num );
#endif
#ifdef _JZ_NEWSCRIPT_LUA
char *getLuaFile(void);
#endif
#ifdef _TRANS7_POINT
int getTrans7Point(void);
#endif
#ifdef _NOJOB_PK
int getNoJobPkMap(void);
#endif
#ifdef _UNTEXT_TALK
char *getUnText(int num);
#endif
BOOL LoadStreetItemUnText( void );
char *getStreetItemUnName(int num);
#ifdef _NO_ATTACK
int getAttTime(void);
int getAttSafeTime(void);
int getAttCnt(void);
int getLateTime(void);
int getAttDmeTime(void);
int getAttDmeCnt(void);
#endif
char* getNoAttIp(int num);
#ifdef _NO_TRANS_ANGLE
int getTrans6Angle(void);
#endif
#ifdef _PET_BUG
int getPetPoint(int num);
#endif
#ifdef _VIGOR_SYS
int getVigorTime(int num);
int getVigorMax(void);
#endif
#ifdef _AUTO_PK
int getAutoPkTime(void);
int getAutoPkMinNum(void);
int getAutoPkBattleTime(void);
int getAutoPkMinLv(void);
int getAutoPkVigorPkNum(void);
int getAutoPkVigorPkCnt(void);
#endif
#ifdef	_BATTLEMAP_CTRL
int getBattleTimeFloor(int num);
int getBattleTimePlayerNum(int num);
int getBattleTimeSec(int num);
#endif
#ifdef	_FM_SERVERNO_SYS
int getFmServerNo(void);
char* getFmServerMsg(void);
#endif
#ifdef	_ZHIPIAO_SYSTEM
int getZhiPiao(void);
int getPiaoTime(void);
#endif
#ifdef	_FMWAR_MSG
char* getFmWarMsg(void);
#endif
#ifdef	_FM_WELFARE	
char* getFmWelfare(int num);
#endif
#ifdef	_FMZUZHANG_MSG
char* getFmZuzhang(void);
#endif
#ifdef	_PETMAIL_LV
int getPetMailLv(void);
int getPetMailCf(void);
#endif
#ifdef	_FMWAR_PLAYERNUM	
int getFmWarPlayerNum(void);
#endif
#ifdef _WAN_FIX
int getVsFlg(void);
#endif
#ifdef	_WARNPC_CTRL
int getPartyMap(int num);
int getMapSameIp(int num);
#endif
#ifdef	_NO_DAOLUAN
int getStreetTrn(void);
int getTalkTrn(void);
int getTalkLv(void);
char* getTalkMsg(void);
char* getTalkName(void);
#endif
#ifdef _NO_FULLPLAYER_ATT
int getNoFullPlayer(void);
int getNoFull2Player(void);
int getNoCdkeyPlayer(void);
int getNoCdkeyMode(void);
int getNoCdkeyType(void);
int getNoFullTime(void);
int getFengType(void);
int getNoFullEndPlayer(void);
int getNoFullEndTime(void);
int getManRenNum(void);
int getBigBao(void);
int getBigBao2(void);
#endif
#ifdef _ATTREVERSE_FIX
int getAttReverse(void);
#endif
#ifdef _NEWFM_GOLD
int getNewFmGold(void);
#endif
#ifdef	_MERGE_TIME
int getMergeTime(void);
#endif
#ifdef	_PETUP_GET_EXP
int getPetUpGetExp(void);
#endif
#ifdef	_MM_NO_JIAOBEN
int getNoMMJiaoben(void);
int getEditBaseTime(void);
#endif
#ifdef	_DP_140_160_MODE
int getNewDpMode(void);
#endif
#ifdef	_LOOK_STREET_TIME
int getLookStreetTime(void);
#endif	
#ifdef	_BATTLE_EQUIT_OTHER
int getBattleEquitOther(void);
#endif
#ifdef	_PK_LUCK_MAN
int getPkLuckFloor(int floor);
char* getPkLuckMapName(int num);
int getPkLuckTimeMin(void);
int getPkLuckTimeMax(void);
int getPkLuckNum(void);
int getPkLuckFame(void);
int getPkLuckVigor(void);
int getPkLuckVipPoint(void);
int getPkLuckAmPoint(void);
int getPkLuckLevel(void);
int getPkLuckBattleTurn(void);
#endif
#ifdef	_NO_DAOLUAN
int getTalkFloor(int num);
#endif
#ifdef	_SUPERMAN_FIX
int getSuperManPoint(void);
#endif
#ifdef	_PICKUP_ITEM_OTHER
int getPickupItem(void);
char* getPickItemId(void);
#endif
#ifdef	_FM_LIAOLI
int getFmLiaoLiType(void);
#endif
#ifdef	_TRADE_ITEM_FIX
int getTradeItemFix(void);
#endif
#ifdef	_PETMAIL_TIME
int getPetMailTime(void);
#endif
int getRandMin(void);
int getRandMax(void);
#ifdef	_GJS_TYPE
int getGjsType(void);
#endif
int getTicketCf(void);
char* getTicketMsg(void);

char* getDefaultKey(int cli_fd);
char* getRunningKey(void);
char* getSaVersion(void);
int getSaType(void);
#ifdef	_BATTLEEND_FIX
int getBattleEndMode(void);
#endif
#ifdef	_BIG_POOL_TYPE
int getPetPoolType(void);
int getItemPoolType(void);
#endif
int getNoStayBattle(void);
int getRightNum(void);
int getRightMode(void);
int getErrorMode(void);
#ifdef	_FLOOR_PARTY_CTRL
int getPartyFloor(int num);
#endif
int getBattleTimtOut(void);
int getReloadNpcTime(void);
int getReloadNpcType(void);
int getOfflineBattleTime(void);
int getOfflineCf(void);
int getOfflineMaxNum(void);
int getOfflineJqmMaxNum(void);
int getKongType(void);
int getJiFenBaiTan(void);
int getNoPkMap(void);
int getFmBuffTrans(void);
char* getNoMpMagic(void);
int getLoginJqmType(void);
char* getFmMagicMpRate(int num);
int getSameIpNum(void);
int getSameJqmNum(void);
int getSameJqmAllNum(void);

int getMmType(void);
char* getMmMsg(void);

int getLockType(void);
int getAllTalkPoint(void);
#ifdef	_NO_RIDE_ID
char* getNoRideId(void);
#endif
int getLoginType(void);

#ifdef _GET_BATTLE_EXP
unsigned int getBattleexp( void );
void setBattleexp( int exp );
#endif

#ifdef _PETTRANS_RANGE
void setPetTransRangeX(int num);
int  getPetTransRangeX(void);
void setPetTransRangeY(int num);
int  getPetTransRangeY(void);
void setPetTransRangeZ(int num);
int  getPetTransRangeZ(void);
#endif

#ifdef _ITEM_LUA
const char *getitemluafile(void);
BOOL ITEM_LuaInit(const char * _FileName);
#endif

#ifdef _SAME_IP_ONLINE_NUM
int getSameIpOnlineNum();
#endif

#ifdef _CHECK_SEVER_IP
int checkServerIp( unsigned int ip );
#endif
BOOL luareadconfigfile( char* data );

#ifdef _CAX_ESC_REPORT
int getReportItem(unsigned int index);
int getVipReportItem(unsigned int index);
int getReportLv(void);
int getReportTa(void);
#endif

#ifdef _PETSKILL_SHOP_LUA
char *getFreePetSkillShopPath();
#endif

#ifdef _CAX_FAMEBUTTON
char* getfamebutton( void );
#endif

#ifdef _PET_TRANS_ABILITY
int getPetTransAbility();
int getPetTransAbility1();
int getPetTransAbility2();
int getPetTransAbility3();
#endif

#ifdef _PET_3TRANS
int getMMPETID1(void);
int getMMPETID2(void);
#endif

#ifdef _NO_MAGIC
int getnomagicmap(int num);
#endif

#ifdef _illegalpetskill_CF
int getIllegalPetskill(int index);
#endif

#ifdef _CAX_PET_EVOLUTION
char getpetevotyb(void);
int  getpetevotyb1(void);
#endif

#ifdef _TRANS_7_COLOR
//int getTranColor(void);
char getping(void);
char getpetsummon(void);
#endif

#ifdef _CAX_PET_ITEMRIDE
char getpetitemride(void);
#endif

#ifdef _DAMMAGE_CALC
int getDamageCalc();
#endif

#ifdef _CAX_LNS_NLSUOXU
int GetConfigLineType(char *TM_ConfigName);
char* GetConfigLineVal(char *TM_ConfigName);
int getCopymapstartingID( void );
#endif

#ifdef _CAX_FAME_KOUFEI
int getfamekoufei();
#endif

#ifdef _CAX_ITEM_ADDEXP
int getexptime();
#endif


#ifdef _SHARE_EXP
int getExpShare(void);
#endif
#ifdef _TEAM_ADDEXP
int getTeamAddExp(void);
#endif

#ifdef _NO_STW_ENEMY
int getNoSTWNenemy( void );
int getNoSTWNenemyPoint( void );
#endif

#ifdef _DEX_FIX
int  getDexFixPer(void);
#endif
#ifdef _FM_EXP_ADD
unsigned int getFmAddExp( void );
#endif
