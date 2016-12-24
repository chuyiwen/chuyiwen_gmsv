

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>

#include <errno.h>
#include <string.h>


#define BOOL int
#define FALSE 0
#define TRUE 1

void sasql_close( void );
BOOL sasql_init( void );
int sasql_query(char *nm, char *pas);
BOOL sasql_update(char *nm, char *path);
BOOL sasql_logindate(char *nm);
BOOL sasql_online( char *ID, char *NM, char *IP,int fame,int vigor, int flag );
BOOL sasql_register(char *id, char *ps);
BOOL sasql_craete_userinfo( void );
BOOL sasql_craete_lock( void );
BOOL sasql_chehk_lock( char *idip );
BOOL sasql_add_lock( char *idip );
BOOL sasql_del_lock( char *idip );
int sasql_vippoint( char *ID, int point,int flag );
int sasql_query_safepasswd(char *id, char *safepasswd);
int sasql_add_safepasswd(char *id, char *safepasswd);
int sasql_load_query(char *nm, char *pas);
int sasql_del_query(char *nm, char *pas);
BOOL sasql_add_card(char *pas, int num);
void sasql_query_card(char *id, char *cardps,int charaindex);
#ifdef _ONLINE_SHOP
int sasql_chehk_shop( char *dataname );
char* sasql_query_shop( char *dataname, int startrow, int shopnum );
int sasql_buy_shop( char *dataname, int startrow, int shopnum, int shopno );
#endif
#ifdef _ALL_ITEM
int sasql_allnum( int flag, int num, int flag1 );
#endif
char* sasql_query_question(void);
#ifdef _SQL_BUY_FUNC
char *sasql_OnlineBuy( char *id, char *costpasswd );
void sasql_OnlineBuy_add( char *coststr, int type, int num );
#endif
#ifdef	_PET_RESET
int sasql_resetpet_point(int petno);
int sasql_resetbdpet_point(int petno);
#endif
#ifdef	_ZHIPIAO_SYSTEM
int sasql_zhipiao_insert(char *petno,char *cdkey,int point);
int sasql_zhipiao_query(char *piaono);
int sasql_zhipiao_update(char *cdkey,char *piaono,int checktype);
#endif
#ifdef	_FMRANK_POINT
int sasql_fm_query(void);
int sasql_fmpoint_query(int fmindex);
int sasql_fmno_query(int fmindex);
int sasql_fmpoint_add(int fmindex,int point);
int sasql_fmindex_add(int fmindex,char *fmname,int point);
int sasql_fmindex_del(int fmindex);
#endif
#ifdef	_ITEM_RESET
int sasql_itemreset_query(int itemid);
int sasql_itemdel_query(int itemid);
#endif
int sasql_cdkey_lock(char *cdkey,int locktype);
int sasql_ampoint( char *ID, int point, int flag );
int sasql_cdkey_online( char *cdkey );
int sasql_check_iponline( char *IP );
#ifdef _EV_NUM
char* sasql_ev_npcmsg( int npcno );
char* sasql_ev_item( void );
char* sasql_ev_pet( void );
#endif
#ifdef	_RMB_SYSTEM
int sasql_rmbpoint( char *ID, int point, int flag );
#endif
char* sasql_rand_buf( void );
int sasql_check_lockip( char *IP ,int type);
int sasql_check_lockmac( char *mac);
int sasql_check_lockpcid( char *pcid);

int sasql_add_lockip(char *IP,int type,int online,int mode);
int sasql_query_online_ip(char *IP);
int sasql_online_ip(char *cdkey,char *IP);
int sasql_offlinenum( char *IP );
int sasql_offlinejqmnum( char *cdkey );
char* sasql_ip( char *cdkey );
BOOL sasql_online_jqm( char *ID, char *jqm, char *IP,int logintype );
int sasql_onlinejqmnum( char *jqm, int flag );
char* sasql_query_jqm( char *cdkey );

int sasql_getVipPoint( int charaindex );
int sasql_setVipPoint( int charaindex, int point );
int sasql_setVipPointForCdkey( char *id, int point );
int sasql_getVipPointForCdkey( char *id );
BOOL sasql_ckeckStrint( char *str );
#ifdef _CARD_PRODUCTION
void sasql_OnlineCost_add( int cost, int num);
#endif

int sasql_jifenPoint( int charaindex );
int sasql_setJfPoint( int charaindex, int point );
int sasql_xjPoint( int charaindex );
int sasql_setxjPoint( int charaindex, int point );

#ifdef _CAX_LNS_MYSQLSUOXU
void sasql_LUASQLAdv(char* sqlstr,char* filepath,char* function,int npcindex ,int charaindex,char* msg,int flg);
void sasql_LUASQL(char* sqlstr,int row,char* filepath,char* function,int npcindex ,int charaindex,char* msg);

char* sasql_Lua_QueryR(int row,char *sqlstr);
char* sasql_LuaAdv_QueryR(char *sqlstr,int flg);
#endif

#ifdef _CAX_NEWMAC_LOGIN
char* sasql_query_mac( char *cdkey );
BOOL sasql_online_mac( char *ID, char *mac, char *IP,int logintype );
int sasql_onlinemacnum( char *mac, int flag );
#endif
