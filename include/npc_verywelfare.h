0034:npc_vigorshop.h:013D:1:14=48041e4b:16=47d21313:
void NPC_ItemVigorShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_ItemVigorShopInit( int meindex );
void NPC_ItemVigorShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_ItemVigorShopLooked( int meindex , int lookedindex);

0038:npc_vigorshop.h.bak:017F:1:14=48041e4b:16=47d21313:#ifndef __NPC_ITEMSHOP_H__
#define __NPC_ITEMSHOP_H__

void NPC_ItemShopTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_ItemShopInit( int meindex );
void NPC_ItemShopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_ItemShopLooked( int meindex , int lookedindex);


#endif
 /*__NPC_ITEMSHOP_H__*/
0033:npc_vippoint.h:0128:1:14=48041e4b:16=47d21313:#ifndef _VIP_SHOP_H
#define _VIP_SHOP_H

void NPC_VipshopTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_VipshopInit( int meindex );
void NPC_VipshopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_VipshopLoop( int meindex);

#endif
0032:npc_vipshop.h:0128:1:14=48041e4b:16=47d21313:#ifndef _VIP_SHOP_H
#define _VIP_SHOP_H

void NPC_VipshopTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_VipshopInit( int meindex );
void NPC_VipshopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_VipshopLoop( int meindex);

#endif
002F:npc_warp.h:019C:1:14=48041e4b:16=47d21313:#ifndef __NPC_WARP_H__
#define __NPC_WARP_H__
BOOL NPC_WarpInit( int charaindex );
void NPC_WarpPostOver( int meindex,int charaindex );
void NPC_WarpWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );
int NPC_WarpSearchByPosition( int fl , int x, int y);
void NPC_WarpWarpCharacter( int warpnpcindex, int charaindex );
#endif
 /*__NPC_WARP_H__*/
0032:npc_warpman.h:021D:1:14=48041e4b:16=47d21313:#ifndef __NPC_WARPMAN_H__
#define __NPC_WARPMAN_H__

void NPC_WarpManTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_WarpManInit( int meindex );
void NPC_WarpManWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_GetDuelRank(int rank,int msgid,int msgid2);
void NPC_WarpManLoop( int meindex);
void NPC_WarpManWatch( int meobjindex, int objindex, CHAR_ACTION act,
                    int x,int y,int dir, int* opt,int optlen );
#endif 
/*__NPC_WARPMAN_H__*/
0032:npc_welfare.h:0149:1:14=48041e4b:16=47d21313:#ifndef __NPC_WELFARE_H__
#define __NPC_WELFARE_H__

BOOL NPC_WelfareInit( int meindex );
void NPC_WelfareTalked( int meindex , int talker , char *msg ,int color );
void NPC_WelfareWindowTalked(int meindex, int talkerindex, int seqno, int select,char *data);
void NPC_WelfareMakeStr(int meindex,int toindex,int select);


#endif
0033:npc_welfare2.h:014E:1:14=48041e4b:16=47d21313:#ifndef __NPC_WELFARE2_H__
#define __NPC_WELFARE2_H__

BOOL NPC_WelfareInit2( int meindex );
void NPC_WelfareTalked2( int meindex , int talker , char *msg ,int color );
void NPC_WelfareWindowTalked2(int meindex, int talkerindex, int seqno, int select,char *data);
void NPC_WelfareMakeStr2(int meindex,int toindex,int select);

#endif
0037:npc_windowhealer.h:019A:1:14=48041e4b:16=47d21313: