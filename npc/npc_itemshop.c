#include "version.h"
#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "npc_eventaction.h"
#include "readmap.h"
#include "log.h"
#include "family.h"

#define MAXSHOPITEM 33
static void NPC_ItemShop_selectWindow( int meindex, int talker, int num,int select);
void NPC_ItemShop_BuyMain(int meindex,int talker,int before );
void NPC_GetItemList(char *argstr,char * argtoken2);
	#ifdef _NEW_MANOR_LAW
	void NPC_ItemStrStr(int itemID,double rate,char *name,char *token2,int iCostFame,int iChangeItemCost);
	#else
	void NPC_ItemStrStr(int itemID,double rate,char *name,char *token2);
	#endif

BOOL NPC_SetNewItem(int meindex,int talker,char *data);
BOOL NPC_SellNewItem(int meindex,int talker,char *data);

void NPC_ItemShop_Menu(int meindex,int talker);

int NPC_GetLimtItemList(int talker,char *argstr,char *token2,int sell);

void NPC_ItemShop_SellMain(int meindex,int talker,int select);
int NPC_GetSellItemList(int itemindex,int flg,char *argstr,char *argtoken,int select,int sell);
#ifdef _NEW_MANOR_LAW
BOOL NPC_AddItemBuy(int meindex, int talker,int itemID,int kosuu,double rate,int iCostFame,int iChangeItemCost);
#else
BOOL NPC_AddItemBuy(int meindex, int talker,int itemID,int kosuu,double rate);
#endif
int NPC_SellItemstrsStr(int itemindex,int flg,double rate,char *argtoken,int select,int sell);
void NPC_LimitItemShop(int meindex,int talker,int select);
void NPC_ExpressmanCheck(int meindex,int talker);

enum{
	NPC_SHOP_WORK_NO 		= CHAR_NPCWORKINT1,
	NPC_SHOP_WORK_EV 		= CHAR_NPCWORKINT2,
	NPC_SHOP_WORK_EXPRESS	= CHAR_NPCWORKINT3,
};

		  

typedef struct {
	char	arg[32];
	int		type;
}NPC_Shop;


static NPC_Shop		TypeTable[] = {
	{ "FIST",		0 }, //ȭ
	{ "AXE",		1 }, //��
	{ "CLUB",		2 }, //����
	{ "SPEAR",		3},  //ì
	{ "BOW",		4},  //��
	{ "SHIELD",		5},  //��
	{ "HELM",		6 }, //ͷ��
	{ "ARMOUR",		7 }, //����
	{ "BRACELET",	8},  //����
	{ "ANCLET",		9 }, //����
	{ "NECKLACE",	10}, //����
	{ "RING",		11}, //��ָ
	{ "BELT",		12}, //����
	{ "EARRING",	13}, //����
	{ "NOSERING",	14}, //�ǻ�
	{ "AMULET",		15}, //�����
	{ "OTHER",		16}, 
	{ "BOOMERANG",	17}, //������
	{ "BOUNDTHROW",	18}, 
	{ "BREAKTHROW",	19}, //Ͷ����
#ifdef _ITEM_TYPETABLE
	{ "DISH",	20},
	{ "METAL",	21},
	{ "JEWEL",	22},
	{ "WARES",	23},
	{ "WBELT",	24},
	{ "WSHIELD", 25},
	{ "WSHOES",	26},
	{ "WGLOVE",	27},
	{ "ANGELTOKEN",	28},
	{ "HEROTOKEN",	29},
#endif
	{ "ACCESSORY",	30}, //���
	{ "OFFENCE",	40}, //����
	{ "DEFENCE",	50}, //����

};

BOOL NPC_ItemShopInit( int meindex )
{

	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];

	CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEITEMSHOP );
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("GetArgStrErr");
		return FALSE;
	}
	if(strstr(argstr,"LIMITSHOP") != NULL) {
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_NO, 1);
	}else{
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_NO, 0);
	}
	if(strstr( argstr, "EVENT") != NULL) {
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EV, 1);
	}else{
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EV, 0);
	}
	if(strstr( argstr, "EXPRESS") != NULL) {
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EXPRESS, 1);
	}else{
		CHAR_setWorkInt( meindex, NPC_SHOP_WORK_EXPRESS, 0);
	}
	
	return TRUE;
}

void NPC_ItemShopTalked( int meindex , int talker , char *szMes ,int color )
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	buff[1024];
	char	buf2[256];
	char 	token[1024];
	int 	i = 1;
	BOOL	sellonlyflg = FALSE;
	char	sellmsg[1024];

    if( CHAR_getInt( talker , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }

	if(NPC_Util_isFaceToFace( meindex, talker, 2) == FALSE) {//��̸ʱ����Ƿ������
		if( NPC_Util_CharDistance( talker, meindex ) > 1) return; //��������1����
	}

	//ȡ��npc�����趨���ڵ�����,��ΪNULL,�����
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof( argstr)) == NULL){
    	print("itemshopGetArgStrErr");
    	return;
    }

	//ȡ��NPCֻ������ѶϢ
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sellonly_msg", token, sizeof( token))	!= NULL){
		sellonlyflg = TRUE; //NPC��������ҵĶ���
		strcpysafe(sellmsg, sizeof( sellmsg), token);//����:sellmsg��ֵ������->������ר���������ĵꡣ
	}

    //ȡ�����������ָ��. buffΪһ������ָ����ִ�,����:��,����,��л��,kau,buy,menu,лл,����,����Ҵ����Щ�־�ʱ,�Ϳ���������
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "buy_msg",	buff, sizeof( buff)) != NULL ){
	    while(getStringFromIndexWithDelim(buff,",",i,buf2,sizeof(buf2)) != FALSE ){
			i++; //��������ָ����8��,i�ͻ�ӵ�8
			if( strstr( szMes, buf2) != NULL) {
				if( CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EV) == 0) {
					if( CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
						if( sellonlyflg ) {
							NPC_ItemShop_selectWindow( meindex, talker, 3, -1);
							return;
						}
					}else{
						NPC_ItemShop_selectWindow( meindex, talker, 1, -1);
						return;
					}
				}else{
					if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
						if( sellonlyflg) {
							NPC_ItemShop_selectWindow( meindex, talker, 3, -1);
							return;
						}
					}else{
						NPC_ItemShop_selectWindow( meindex, talker, 1, -1);
						return;
					}
		 			return;
		 	 	}
			}
		}
	}
	i=1;

    //ȡ�������������ָ��. buffΪһ��������ָ����ִ�,����:������,��,sell,uru  ����Ҵ����Щ�־�ʱ,�Ϳ�����������
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "sell_msg", buff, sizeof( buff)) != NULL ){
	    while( getStringFromIndexWithDelim(buff,",", i,buf2,sizeof(buf2)) != FALSE ){
			i++;
			if(strstr(szMes,buf2) != NULL) {
				NPC_ItemShop_selectWindow( meindex, talker, 2, -1);
				return;
			}
		}
	}
	i = 1;

	//����ѶϢ
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "other_msg", buff, sizeof( buff)) != NULL ){
	    while(getStringFromIndexWithDelim( buff, ",", i, buf2, sizeof( buf2)) != FALSE ){
			i++;
			if(strstr(szMes,buf2) != NULL) {
				if(NPC_Util_GetStrFromStrWithDelim( argstr, "hint_msg", 
				token, sizeof( token)) != NULL)
				{
					CHAR_talkToCli( talker, meindex, token, CHAR_COLORWHITE);
					return;
				}
			}
		}	
	}

	if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
		if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EV) == 1) {
			if( sellonlyflg) {
				CHAR_talkToCli( talker, meindex, sellmsg, CHAR_COLORWHITE);
				return;
			}
		}else{
			if( sellonlyflg) {
				NPC_ItemShop_selectWindow( meindex, talker, 3, -1);
				return;
			}
		}
	}else{
		if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EV) == 1) {
			if( sellonlyflg) {
				CHAR_talkToCli( talker, meindex, sellmsg, CHAR_COLORWHITE);
				return;
			}
		}else{			
			if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EXPRESS) == 1) {
				NPC_ExpressmanCheck( meindex, talker);
			}else{
				NPC_ItemShop_selectWindow( meindex, talker, 0, -1);
			}
		}
	}
				
}

static void NPC_ItemShop_selectWindow( int meindex, int talker, int num,int select)
{
	switch( num) {
	  case 0:
		CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);//���ͽ�Ǯ,��100Ԫ �ͳ�ȥ�����ϸ�ʽ����Ϊ P8Nz2|100|
		
		if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_EXPRESS) == 1 ) {
			if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) ==0 ) {
				NPC_ExpressmanCheck( meindex, talker);//��ë���� 
			}
		}else if(CHAR_getWorkInt( meindex, NPC_SHOP_WORK_NO) == 1) {
		
		}else{
		  	NPC_ItemShop_Menu( meindex, talker);//ѡ��(��,��,�뿪)��С�Ӵ�	
		}
	  	break;

	  case 1://�������Ӵ�
		CHAR_sendStatusString( talker,"I");//��������������еĵ��߸�Client
	  	NPC_ItemShop_BuyMain( meindex, talker, select);
	  	break;

	  case 2://�������Ӵ�
		CHAR_sendStatusString( talker,"I");
	  	NPC_ItemShop_SellMain( meindex, talker, select);
	  	break;

	  case 3:
	  	NPC_LimitItemShop( meindex, talker, select);
	  	break;

	}
}

void NPC_ItemShopWindowTalked( int meindex, int talkerindex, 
								int seqno, int select, char *data)
{
	if( NPC_Util_CharDistance( talkerindex, meindex ) > 3) {
		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
		return;
	}

	makeStringFromEscaped( data);

	switch( seqno){

	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG:
		if(atoi( data) == 1 )	NPC_ItemShop_selectWindow(meindex, talkerindex, 1, -1);
		if(atoi( data) == 2)	NPC_ItemShop_selectWindow(meindex, talkerindex, 2, -1);
		if(atoi( data) == 3)	return;/*--������ئ��--*/
		break;
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG:
		if(NPC_SetNewItem(meindex , talkerindex, data) == TRUE) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 1, 0);
		}else{
			NPC_ItemShop_selectWindow( meindex, talkerindex ,0, -1);
		}

		break;
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG:
		if(NPC_SellNewItem(meindex , talkerindex, data) == TRUE) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 2, 0);

		}else{
			NPC_ItemShop_selectWindow( meindex,  talkerindex, 0, -1);
		}

		break;
			
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT:
		 if(select == WINDOW_BUTTONTYPE_YES) {
			NPC_ItemShop_selectWindow( meindex, talkerindex ,2, -1);

		}else  if(select == WINDOW_BUTTONTYPE_NO) {
			return;
		}else if(select == WINDOW_BUTTONTYPE_OK) {
				NPC_ItemShop_selectWindow( meindex, talkerindex, 2, -1);
		}
		break;
	
	  case CHAR_WINDOWTYPE_WINDOWITEMSHOP_EXPRESS:
		if(atoi(data) == 2) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 1, -1);
		}else if(atoi( data) == 4) {
			NPC_ItemShop_selectWindow( meindex, talkerindex, 2, -1);
		}
	}
}

void NPC_ItemShop_BuyMain(int meindex,int talker,int before )
{      
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[NPC_UTIL_GETARGSTR_BUFSIZE];
	int fd = getfdFromCharaIndex( talker);
    
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buff2[256];
  char buff[256];
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("itemshop_GetArgStr_Err");
       	return;
    }
#ifdef _ADD_STATUS_2
	sprintf(token,"FAME|%d",CHAR_getInt(talker,CHAR_FAME)/100);
	lssproto_S2_send(fd,token);
#endif
	if(before != -1) {
		sprintf(token,"0|0");

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
	}else{

		NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "buy_main", buff2, sizeof( buff2));

		sprintf(token,"0|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getChar( meindex, CHAR_NAME), buff, buff2);

		NPC_Util_GetStrFromStrWithDelim( argstr, "what_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "level_msg", buff2, sizeof( buff));

		snprintf( token2, sizeof( token2), "%s|%s", buff, buff2);
	
	    strncat( token, token2, sizeof( token));
			
		NPC_Util_GetStrFromStrWithDelim( argstr, "realy_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "itemfull_msg", buff2, sizeof( buff2));

		sprintf( token2, "|%s|%s", buff, buff2);
		strncat(token , token2,sizeof(token));
		strcpy(token2, "|");

		NPC_GetItemList( argstr, token2);
		strncat( token, token2, sizeof( token));
	}

	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_BUY_MSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

}
void NPC_GetItemList(char *argstr,char *argtoken)
{
	int i = 1;
	int tmp;
	char *name ="\0";
	char buff2[256];
#ifdef _NEW_MANOR_LAW
	char buff3[NPC_UTIL_GETARGSTR_LINEMAX];
	char buff4[256];
	char buff5[NPC_UTIL_GETARGSTR_LINEMAX];
	char buff6[256];
	int iCostFame = 0;
	int iChangeItemCost = 0;
#endif
	char buff[NPC_UTIL_GETARGSTR_LINEMAX];
	char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
	double rate = 1.0;
	int loopcnt = 0;

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "buy_rate", buff2, sizeof( buff2))
	 != NULL){
		rate = atof( buff2);
	}
#ifdef _NEW_MANOR_LAW
	memset(buff3,0,sizeof(buff3));
	memset(buff5,0,sizeof(buff5));
	if(NPC_Util_GetStrFromStrWithDelim(argstr,"CostFame",buff3,sizeof(buff3)) == NULL) iCostFame = -1;
	if(NPC_Util_GetStrFromStrWithDelim(argstr,"ChangeItemCost",buff5,sizeof(buff5)) == NULL) iChangeItemCost = -1;
#endif
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "ItemList", buff, sizeof( buff)) != NULL ){
	    while( getStringFromIndexWithDelim(buff,",",i,buff2,sizeof(buff2)) !=FALSE ) {
#ifdef _NEW_MANOR_LAW
				if(iCostFame > -1){
					if(getStringFromIndexWithDelim(buff3,",",i,buff4,sizeof(buff4)) != FALSE) iCostFame = atoi(buff4);
				}
				if(iChangeItemCost > -1){
					if(getStringFromIndexWithDelim(buff5,",",i,buff6,sizeof(buff6)) != FALSE) iChangeItemCost = atoi(buff6);
				}
#endif
			i++;
			if(strstr( buff2, "-") == NULL) {
			 	name = ITEM_getNameFromNumber( atoi(buff2));
				if(name == "\0") continue;
				loopcnt++;
				if(loopcnt == MAXSHOPITEM) break;
	#ifdef _NEW_MANOR_LAW
				NPC_ItemStrStr( atoi( buff2), rate, name, token2,iCostFame,iChangeItemCost);
	#else
				NPC_ItemStrStr( atoi( buff2), rate, name, token2);
	#endif
	    		strncat( argtoken, token2, sizeof(token2));
			}else{
				int start;
				int end;
				getStringFromIndexWithDelim( buff2, "-", 1, token2, sizeof(token2));
				start = atoi( token2);
				getStringFromIndexWithDelim( buff2, "-", 2 ,token2, sizeof(token2));
				end = atoi( token2);
				if(start > end){
					tmp = start;
					start = end;
					end = tmp;
				}
				end++;
				for(; start < end ; start++ ) {
					/*--���ڒo�???--*/

				 	name = ITEM_getNameFromNumber( start );
					if(name == "\0") continue;
					loopcnt++;
					if(loopcnt == MAXSHOPITEM) break;
	#ifdef _NEW_MANOR_LAW
					NPC_ItemStrStr( start, rate, name, token2,iCostFame,iChangeItemCost);
	#else
					NPC_ItemStrStr( start, rate, name, token2);
	#endif
		    		strncat( argtoken, token2, sizeof(token2));
				}
			}
		}
	}
}

	#ifdef _NEW_MANOR_LAW
		void NPC_ItemStrStr(int itemID,double rate,char *name,char *token2,int iCostFame,int iChangeItemCost)
	#else
		void NPC_ItemStrStr(int itemID,double rate,char *name,char *token2)
	#endif
{

	int gold;
	int level;
	int graNo;
	char info[1024];
	char escape[256];	

	#ifdef _NEW_MANOR_LAW
	if(iChangeItemCost > -1) gold = iChangeItemCost;
	else 
	#endif
	gold  = ITEM_getcostFromITEMtabl( itemID);
	level = ITEM_getlevelFromITEMtabl( itemID);
	graNo = ITEM_getgraNoFromITEMtabl( itemID);
	strcpy(escape,ITEM_getItemInfoFromNumber( itemID));
	gold=(int)(gold * rate);

	makeEscapeString( escape, info, sizeof( info));
	makeEscapeString( name, escape, sizeof( escape));
#ifdef _NEW_MANOR_LAW
	sprintf(token2,"%s|0|%d|%d|%d|%s|%d|",escape,level,gold,graNo,info,iCostFame < 0 ? -1:iCostFame/100);
#else
	sprintf( token2, "%s|0|%d|%d|%d|%s|", escape, level, gold, graNo, info);
#endif
}

BOOL NPC_SetNewItem(int meindex,int talker,char *data)
{

	char buf[1024];
	char buff2[128];
#ifdef _NEW_MANOR_LAW
	char buff3[1024];
	char buff4[128];
	char buff5[1024];
	char buff6[128];
	int iCostFame = 0;
	int iChangeItemCost = 0;
#endif
	int i = 1, j = 1;
	int select;
	int kosuu = 0;
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	double rate = 1.0;
	int gold = 0;		
	int kosuucnt = 0;
	int itemindex;

	/*--߯�Ȼ����׷�����ë�������������ѱ���ݩ--*/
	getStringFromIndexWithDelim( data, "|", 1, buf, sizeof( buf));
	select = atoi(buf);
	if(select == 0) return FALSE;
	getStringFromIndexWithDelim( data, "|", 2, buf, sizeof( buf));
	kosuu = atoi(buf);
	if( kosuu <= 0 ) return FALSE;
	
	/*--���Ѽ���������  ����    �¾���-*/
	for( i = CHAR_STARTITEMARRAY ; i < CHAR_MAXITEMHAVE ; i++ ) {
		itemindex = CHAR_getItemIndex( talker , i );
		if( !ITEM_CHECKINDEX( itemindex) ) {
			kosuucnt++;
		}
	}

	/*--߯�Ȼ�  �����Ѽ�  ��  �˼�����  �ӡ��ﴡ  ��Ի�������ƾ����м�ƥ--*/
	/*--�ӡ��������  ë  ľ��--*/
	if( kosuucnt < kosuu) kosuu = kosuucnt;
		
	/*--δ���������޷¡�--*/
	if(kosuucnt == 0 ) return FALSE;

	i = 1;

	/*--���Ҽ������̻�  ���跴�����̻ﻥ����ئ���������巴��  --*/
	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
   	print("shop_GetArgStr_Err");
   	return FALSE;
	}

	/*---������ë��    ئ��ľ��1.0)-*/
	if(NPC_Util_GetStrFromStrWithDelim( argstr, "buy_rate", buf, sizeof( buf)) != NULL) {
		rate= atof( buf);
	}

#ifdef _NEW_MANOR_LAW
	memset(buff3,0,sizeof(buff3));
	memset(buff5,0,sizeof(buff5));
	if(NPC_Util_GetStrFromStrWithDelim(argstr,"CostFame",buff3,sizeof(buff3)) == NULL) iCostFame = -1;
	if(NPC_Util_GetStrFromStrWithDelim(argstr,"ChangeItemCost",buff5,sizeof(buff5)) == NULL) iChangeItemCost = -1;
#endif

	/*--ʧ��  ة��ܰ��ë�浤���Ƿ-*/
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "ItemList", buf, sizeof( buf)) != NULL ){
		while(getStringFromIndexWithDelim(buf , "," , j, buff2, sizeof(buff2)) != FALSE ){
#ifdef _NEW_MANOR_LAW
			if(iCostFame > -1){
				if(getStringFromIndexWithDelim(buff3,",",j,buff4,sizeof(buff4)) != FALSE) iCostFame = atoi(buff4);
			}
			if(iChangeItemCost > -1){
				if(getStringFromIndexWithDelim(buff5,",",j,buff6,sizeof(buff6)) != FALSE) iChangeItemCost = atoi(buff6);
			}
#endif
			j++;
			/*--  "-"��ֳ��ľ�����¾�������--*/
			if(strstr( buff2, "-") == NULL) {
				if( ITEM_getcostFromITEMtabl(atoi(buff2)) !=-1) {
					if ( i == select) {
						/*---ʧ��  ة������---*/
						/*--����������--*/
#ifdef _NEW_MANOR_LAW
						if(NPC_AddItemBuy(meindex, talker,atoi(buff2),kosuu,rate,iCostFame,iChangeItemCost) != TRUE)
#else
						if(NPC_AddItemBuy(meindex, talker,atoi(buff2),kosuu,rate) != TRUE)
#endif
						{
							return FALSE;
						}
						return TRUE;
					}
					i++;		
				}
			}else{
				/*--ʧ��  ة��  15-25  ����ƥ˪��ľ������--*/
				int start;
				int end;

				/* "-"ƥ����ľ��炙����Ѱ���  ���Ѱ�ë��  --*/
				getStringFromIndexWithDelim( buff2, "-", 1, argstr, sizeof(argstr));
				start = atoi( argstr);
				getStringFromIndexWithDelim( buff2, "-", 2 ,argstr, sizeof(argstr));
				end = atoi( argstr);
				end++;

				/*--  į���ѱ�ئ�Ȼ������գ�  ľ׸����**/
				if(start > end){
					gold = start;
					start = end;
					end = gold;
				}

				/*--"-"ƥ����ľ���м�ʧ��  ةë��  ë  ��--*/
				for(; start < end ; start++ ) {
					if( ITEM_getcostFromITEMtabl( start) != -1) {
						if ( i == select) {
							/*---ʧ��  ة������---*/
							/*--����������--*/
#ifdef _NEW_MANOR_LAW
							if(NPC_AddItemBuy(meindex, talker, start, kosuu, rate,iCostFame,iChangeItemCost) != TRUE)
#else
							if(NPC_AddItemBuy(meindex, talker, start, kosuu, rate) != TRUE)
#endif
							{
								return FALSE;
							}
							return TRUE;
						}
						i++;
					}
				}
			}
		}
	}

	return FALSE;

}

/*---------------------------------------------
 *ʧ��  ة��ܰ��ë�浤
 *--------------------------------------------*/
#ifdef _NEW_MANOR_LAW
BOOL NPC_AddItemBuy(int meindex, int talker,int itemID,int kosuu,double rate,int iCostFame,int iChangeItemCost)
#else
BOOL NPC_AddItemBuy(int meindex, int talker,int itemID,int kosuu,double rate)
#endif
{

	int itemindex;
	int i;
	int gold;
	int ret;
	int maxgold;
#ifdef _NEW_MANOR_LAW
	int iTotalCostFame = -1;
#endif
	#ifdef _NEW_MANOR_LAW
	if(iChangeItemCost > -1) gold = iChangeItemCost;
	else
	#endif
	gold = ITEM_getcostFromITEMtabl( itemID);
	gold = (int)(gold * rate);

	maxgold = gold * kosuu;
	if(CHAR_getInt( talker, CHAR_GOLD) < maxgold ) return FALSE;
#ifdef _NEW_MANOR_LAW
	if(iCostFame > 0){
		iTotalCostFame= iCostFame * kosuu;
		if(CHAR_getInt(talker,CHAR_FAME) < iTotalCostFame) return FALSE;
	}
#endif

//	if( addNpcFamilyTax( meindex, talker, maxgold*0.4 ) )
//		print(" FamilyTaxDone! ");
//	else
//		print(" FamilyTaxError!");
	addNpcFamilyTax( meindex, talker, maxgold*0.4 );
	for(i = 0 ; i < kosuu ; i++){
		itemindex = ITEM_makeItemAndRegist( itemID);
		
		if(itemindex == -1) return FALSE;
		ret = CHAR_addItemSpecificItemIndex( talker, itemindex);
		if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
			print( "npc_itemshop.c: additem error itemindex[%d]\n", itemindex);
			ITEM_endExistItemsOne( itemindex);
			return FALSE;
		}
		CHAR_sendItemDataOne( talker, ret);
	}
	CHAR_DelGold( talker, maxgold);
#ifdef _NEW_MANOR_LAW
	if(iTotalCostFame > 0){
		CHAR_setInt(talker,CHAR_FAME,CHAR_getInt(talker,CHAR_FAME) - iTotalCostFame);
		if(CHAR_getInt(talker,CHAR_FMINDEX) != -1 && (strcmp(CHAR_getChar(talker,CHAR_FMNAME),""))){
			int fd = getfdFromCharaIndex(talker);
			char	buf[256];
			sprintf(buf,"%d",CHAR_getInt(talker,CHAR_FAME));
			saacproto_ACFixFMData_send(acfd,
				CHAR_getChar(talker,CHAR_FMNAME),
				CHAR_getInt(talker,CHAR_FMINDEX),
				CHAR_getWorkInt(talker,CHAR_WORKFMINDEXI),
				FM_FIX_FAME,buf,"",
				CHAR_getWorkInt(talker,CHAR_WORKFMCHARINDEX),
				CONNECT_getFdid(fd));
			LogFMFameShop(CHAR_getChar(talker,CHAR_FMNAME),
										CHAR_getChar(talker,CHAR_CDKEY),
										CHAR_getChar(talker,CHAR_NAME),
										CHAR_getInt(talker,CHAR_FAME),
										iTotalCostFame);
		}
	}
#endif
	//CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	return TRUE;

}

//ѡ�� (��,��,��ȥ) ��С�Ӵ�
void NPC_ItemShop_Menu(int meindex,int talker)
{	
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	token[NPC_UTIL_GETARGSTR_LINEMAX];
	char buff[256];
	int fd = getfdFromCharaIndex( talker);

    //argstrȡ�������趨����ѶϢ: ����->buy_rate:1.0|sell_rate:0.2|buy_msg:��,����,��л��,kau,buy............. (�м�ķָ���Ƕ���ʱ�����)
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("shop_GetArgStr_Err");
       	return;
    }
    
	//tokenΪ�Ӵ������title����  ����: ��ķ�����ķ��ߵ�|��ӭ����
    NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
#ifdef _NEW_MANOR_LAW
	snprintf(token, sizeof(token),"%s|%s|%d",CHAR_getChar(meindex,CHAR_NAME),buff,CHAR_getInt(talker,CHAR_FAME)/100);
#else
	snprintf(token, sizeof(token),"%s|%s",CHAR_getChar( meindex, CHAR_NAME), buff);
#endif

	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMENU, 
				WINDOW_BUTTONTYPE_NONE, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

}

void NPC_ItemShop_SellMain(int meindex,int talker,int before)
{
	char	argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char	token[NPC_UTIL_GETARGSTR_BUFSIZE];
	int fd = getfdFromCharaIndex( talker);

	//ȡ��npc�趨����
    if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
    }

#ifdef _ADD_STATUS_2
	sprintf(token,"FAME|%d",CHAR_getInt(talker,CHAR_FAME)/100);
	lssproto_S2_send(fd,token);
#endif

	if(before != -1) {
		sprintf(token,"1|0");
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN
							+CHAR_getWorkInt(meindex,NPC_SHOP_WORK_NO), 
					WINDOW_BUTTONTYPE_NONE, 
					CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);
	
	}else{

		char token2[NPC_UTIL_GETARGSTR_BUFSIZE];
		char buff2[256];
	   	char buff[256];

		NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buff, sizeof( buff));
		NPC_Util_GetStrFromStrWithDelim( argstr, "sell_main", buff2, sizeof( buff));
		sprintf( token, "1|1|%d|%s|%s|%s|", CHAR_WINDOWTYPE_WINDOWITEMSHOP_STARTMSG,
				CHAR_getChar( meindex, CHAR_NAME), buff, buff2);

		NPC_Util_GetStrFromStrWithDelim( argstr, "stone_msg", buff, sizeof( buff));

		if(CHAR_getWorkInt(meindex,NPC_SHOP_WORK_EXPRESS) == 1 ) {
			NPC_Util_GetStrFromStrWithDelim( argstr, "exrealy_msg", buff2, sizeof(buff2));
		}else{
			NPC_Util_GetStrFromStrWithDelim( argstr, "realy_msg", buff2, sizeof( buff2));
		}
		sprintf( token2,"%s|%s|", buff, buff2);
		NPC_GetLimtItemList( talker,argstr, token2, -1);//��ϸ���Ҫ�����ĵ�������
		strncat( token, token2, sizeof( token));

		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_ITEMSHOPMAIN+
					CHAR_getWorkInt(meindex,NPC_SHOP_WORK_NO), 
					WINDOW_BUTTONTYPE_NONE, 
					CHAR_WINDOWTYPE_WINDOWITEMSHOP_SELL_MSG,
					CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
					token);

	}
}
int NPC_GetLimtItemList(int talker, char *argstr, char* token2,int sell)
{

	char token[NPC_UTIL_GETARGSTR_LINEMAX];
	char buff[NPC_UTIL_GETARGSTR_LINEMAX];
	char token3[NPC_UTIL_GETARGSTR_LINEMAX];
	int k = 0 , i = 1 , j = 0;
	int imax;
	int itemtype = 0;
	int itemindex;
	int okflg = 0;
	char buf[256];
	int flg=0;
	int cost;

	if(sell == -1 ){
		i = CHAR_STARTITEMARRAY;
		imax = CHAR_MAXITEMHAVE;
		flg = -1;
	}else{
		i= sell;
		imax= sell + 1;
		flg = 1;
	}	

	for( ; i < imax ; i++ ){
		okflg=0;
		itemindex = CHAR_getItemIndex( talker , i );
		
		if( ITEM_CHECKINDEX( itemindex) ){
			if( NPC_Util_GetStrFromStrWithDelim( argstr,"LimitItemType", buff, sizeof( buff)) != NULL ){
				k = 1;
				while(getStringFromIndexWithDelim(buff , "," , k, token, sizeof(token)) != FALSE ){
#ifdef _ITEM_TYPETABLE
					int cmpmaxitem = sizeof(TypeTable)/sizeof(TypeTable[0]);
#endif
					k++;
#ifdef _ITEM_TYPETABLE
					for(j = 0 ; j < cmpmaxitem ; j++){
#else
					for(j = 0 ; j < ITEM_CATEGORYNUM+3 ; j++){
#endif
						if(strcmp( TypeTable[ j].arg  , token) == 0 ) {
							itemtype = TypeTable[ j].type;
							if(ITEM_getInt(itemindex,ITEM_TYPE) == itemtype) {
								cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
								if(cost != -1) return cost;
								strncat( token2, token3, sizeof( token3));
								okflg = 1;
							}else if(itemtype == 30){
								if( 8 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 15) ){
									cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}else if(itemtype == 40){
								if(( 0 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 4)) 
									|| (17 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 19))) {
									cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}else if(itemtype == 50){
								if( 5 <= ITEM_getInt(itemindex,ITEM_TYPE) 
									&& (ITEM_getInt(itemindex,ITEM_TYPE) <= 7) ){
									cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
									if(cost != -1) return cost;
									strncat(token2,token3,sizeof(token3));
									okflg = 1;
								}
							}
							break;
						}
					}
					
					if(okflg == 1) break;
				}
			}
			if( (NPC_Util_GetStrFromStrWithDelim( argstr, "LimitItemNo", buff,sizeof( buff))!= NULL) && okflg == 0 ){
				k = 1;
				while(getStringFromIndexWithDelim(buff , "," , k, token, sizeof(token)) != FALSE ){
					k++;
					if(strstr( token, "-")==NULL && strcmp(token,"") != 0) {
						if(ITEM_getInt(itemindex,ITEM_ID) == atoi(token)) {
							cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
							if(cost != -1) return cost;
							strncat(token2,token3,sizeof(token3));
							okflg=1;
						}
					}else if (strstr( token, "-") != NULL){
						int start, end;
						int work;
						
						if( getStringFromIndexWithDelim( token, "-", 1, buf, sizeof(buf)) == FALSE )
							return -1;
						start = atoi( buf);
						if( getStringFromIndexWithDelim( token, "-", 2 ,buf, sizeof(buf)) == FALSE )
							return -1;
						end = atoi( buf);
						
						if(start > end){
							work = start;
							start = end;
							end = work;
						}
						
						end++;
						if( (start <= ITEM_getInt(itemindex,ITEM_ID)) && (ITEM_getInt(itemindex,ITEM_ID) < end) ){
							cost = NPC_GetSellItemList(itemindex,0,argstr,token3,i,sell);
							if(cost != -1) return cost;
							strncat(token2,token3,sizeof(token3));
							okflg = 1;
						}
					}
				}
			}
			
			if(okflg == 0) {
				cost = NPC_GetSellItemList(itemindex, 1, argstr, token3, i, sell);
				if(sell != -1) return -1;
				strncat( token2, token3, sizeof( token3));
			}
			
		}
	}
	return -1;
}



/*----------------------------------------------------------

	�ͷ���ʧ������˪�������������Ｐ����

 *----------------------------------------------------------*/
int NPC_GetSellItemList(int itemindex,int flg, char *argstr,char *argtoken,int select,int sell)
{
	char buff[256];
	double rate = 0.2;
	char buff2[256];
	char buff3[64];
	int k = 1;
	int cost = -1;

	if(NPC_Util_GetStrFromStrWithDelim( argstr,"special_item",buff, sizeof( buff)) != NULL){
		if(NPC_Util_GetStrFromStrWithDelim( argstr,"special_rate",buff2, sizeof( buff2)) != NULL ){
			rate = atof(buff2);
		}else{
			rate = 1.2;
		}
		
		while(getStringFromIndexWithDelim(buff , "," , k, buff2, sizeof(buff2)) !=FALSE ){
			k++;
			if(strstr( buff2, "-") == NULL && strcmp(buff2,"") != 0) {
				if(ITEM_getInt(itemindex,ITEM_ID) == atoi(buff2)){
					cost = NPC_SellItemstrsStr( itemindex,0, rate, argtoken,select,sell);
					return cost;
				}
			}else if (strstr( buff2, "-") != NULL){
				int start;
				int end;
				int work;
				getStringFromIndexWithDelim( buff2, "-", 1, buff3, sizeof(buff3));
				start = atoi( buff3);
				getStringFromIndexWithDelim( buff2, "-", 2 ,buff3, sizeof(buff3));
				end = atoi( buff3);

				if(start > end){
					work = start;
					start = end;
					end = work;
				}
				end++;
				if( (start <= ITEM_getInt(itemindex,ITEM_ID)) && (ITEM_getInt(itemindex,ITEM_ID) < end)	){
					cost = NPC_SellItemstrsStr( itemindex,0, rate, argtoken,select,sell);
					return cost;
				}
			}
		}
	}
	if( NPC_Util_GetStrFromStrWithDelim( argstr,"sell_rate",buff, sizeof( buff)) != NULL ){
		rate = atof(buff);
		cost = NPC_SellItemstrsStr( itemindex, flg ,rate, argtoken,select,sell);
		return cost;
	}

	return cost;
}

int NPC_SellItemstrsStr(int itemindex,int flg,double rate,char *argtoken,int select,int sell)
{
	int cost;
	char escapedname[256];
	char name[256];	
	char *eff;
	

	cost = ITEM_getInt( itemindex, ITEM_COST);
	cost = (int)(cost * rate);

	if(sell != -1) return cost;

	strcpy( escapedname, ITEM_getChar( itemindex, ITEM_SECRETNAME));
	makeEscapeString( escapedname, name, sizeof( name));
	eff=ITEM_getChar(itemindex, ITEM_EFFECTSTRING);
	makeEscapeString( eff, escapedname, sizeof(escapedname));



	sprintf( argtoken,
#ifdef _ITEM_PILENUMS
			"%s|%d|%d|%d|%s|%d|%d|",
#else
			"%s|%d|%d|%d|%s|%d|",
#endif
			name, flg, cost,
			ITEM_getInt( itemindex, ITEM_BASEIMAGENUMBER),
			escapedname, select
#ifdef _ITEM_PILENUMS
			,ITEM_getInt( itemindex, ITEM_USEPILENUMS)
#endif
	);

	return -1;

}

BOOL NPC_SellNewItem(int meindex,int talker,char *data)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char token[256], token2[256];
	int cost, k, select, itemindex;
	int MyGold, MaxGold, sellnum=1;

	MaxGold = CHAR_getMaxHaveGold( talker);
	MyGold = CHAR_getInt( talker, CHAR_GOLD);

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
		print("GetArgStrErr");
		return FALSE;
	}
	getStringFromIndexWithDelim(data , "|" ,1, token, sizeof( token));
	select = atoi(token);
#ifdef _ITEM_PILENUMS
	getStringFromIndexWithDelim(data , "|" ,2, token, sizeof( token));
	sellnum = atoi(token);
#endif

	if( select < CHAR_STARTITEMARRAY || select >= CHAR_MAXITEMHAVE ) return FALSE;
	k = select;
	itemindex = CHAR_getItemIndex( talker , k);
	cost = NPC_GetLimtItemList( talker,argstr, token2,select);
	if( cost == -1 || (cost*sellnum)+MyGold >= MaxGold || !ITEM_CHECKINDEX( itemindex) ){
		int fd = getfdFromCharaIndex( talker);
		sprintf(token,"\n\n��ѽ!�Բ���" "\n\n�Բ��� ! �ɲ�������ѡһ���أ�" );	
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
				WINDOW_BUTTONTYPE_OK, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);
		return FALSE;
	}
#ifdef _ITEM_PILENUMS
	if( NPC_DelItem( talker, k, sellnum) == FALSE ) return FALSE;
#else
	{
		LogItem(
			CHAR_getChar( talker, CHAR_NAME ),
			CHAR_getChar( talker, CHAR_CDKEY ),
#ifdef _add_item_log_name  // WON ADD ��item��log������item����
			itemindex,
#else
	   		ITEM_getInt( itemindex, ITEM_ID ),
#endif
			"Sell(������)",
			CHAR_getInt( talker,CHAR_FLOOR),
			CHAR_getInt( talker,CHAR_X ),
			CHAR_getInt( talker,CHAR_Y ),
			ITEM_getChar( itemindex, ITEM_UNIQUECODE),
			ITEM_getChar( itemindex, ITEM_NAME),
			ITEM_getInt( itemindex, ITEM_ID)

		);
	}
	CHAR_DelItem( talker, k);
#endif
	CHAR_AddGold( talker, cost*sellnum);
	CHAR_send_P_StatusString( talker, CHAR_P_STRING_GOLD);
	return TRUE;
}



void NPC_LimitItemShop(int meindex,int talker,int select)
{

	int fd = getfdFromCharaIndex( talker);
	char token[NPC_UTIL_GETARGSTR_LINEMAX];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
	}

	if(NPC_Util_GetStrFromStrWithDelim( argstr, "sellonly_msg", buf, sizeof( buf))!=NULL){
		sprintf(token,"\n\n%s", buf);
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
				WINDOW_BUTTONTYPE_YESNO, 
				CHAR_WINDOWTYPE_WINDOWITEMSHOP_LIMIT,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

	}else{
		CHAR_talkToCli( talker, meindex, "��������ר�ŵꡣ",CHAR_COLORWHITE);
	}
	return;
}

void NPC_ExpressmanCheck(int meindex,int talker)
{
	int fd = getfdFromCharaIndex( talker);
	char token[1024];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char buf[1024];

	if( NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr)) == NULL) {
       	print("shop_GetArgStr_Err");
       	return;
	}
	NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", buf, sizeof( buf));
	sprintf(token,"4\n��������������%s\n\n%s"
					"\n\n������������  ��  ��������"
				  "\n\n��������  ���������"
					,CHAR_getChar(meindex,CHAR_NAME),buf);
	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
			WINDOW_BUTTONTYPE_CANCEL, 
			CHAR_WINDOWTYPE_WINDOWITEMSHOP_EXPRESS,
			CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
			token);

	return;


}

