#ifndef _STU_SHOP_H
#define _STU_SHOP_H

void NPC_StushopTalked( int meindex, int talkerindex, char *msg, int color );
BOOL NPC_StushopInit( int meindex );
void NPC_StushopWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);
void NPC_StushopLoop( int meindex);

#endif
