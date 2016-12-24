#include "version.h"

#ifdef _NPC_WELFARE
#ifdef _PROFESSION_SKILL			// WON ADD ����ְҵ����

#include <string.h>
#include "char.h"
#include "object.h"
#include "char_base.h"
#include "npcutil.h"
#include "lssproto_serv.h"
#include "pet_skill.h"
#include "readmap.h"
#include "battle.h"
#include "profession_skill.h"
#include "chatmagic.h"
#include "npc_welfare.h"
#include "npc_exchangeman.h"

static void NPC_Welfare_selectWindow(int meindex, int toindex, int num, int select);


//����ÿ�NPC�ĳ�ʼ��
BOOL NPC_WelfareInit( int meindex )
{
    CHAR_setInt( meindex , CHAR_WHICHTYPE , CHAR_TYPEWELFARE);
    //print("����:%s\n",CHAR_getChar( meindex, CHAR_NAME));
    return TRUE;
}

//�Ի�ʱ�Ĵ���
void NPC_WelfareTalked( int meindex , int talkerindex , char *szMes ,int color )
{
    if( CHAR_getInt( talkerindex , CHAR_WHICHTYPE ) != CHAR_TYPEPLAYER ) {
    	return;
    }
	if(NPC_Util_isFaceToFace(talkerindex,meindex,2 )==FALSE){
		if( NPC_Util_CharDistance( talkerindex, meindex ) > 1) return;
	}
	NPC_Welfare_selectWindow( meindex, talkerindex,0,-1);
}

static void NPC_Welfare_selectWindow( int meindex, int toindex, int num,int select)
{
	switch(num){
	  case 0:
		  //free
		NPC_WelfareMakeStr(meindex, toindex, select);
		break;
	  case 1:
		break;
	}
}

void NPC_WelfareWindowTalked( int meindex, int talkerindex, 
		int seqno, int select, char *data)
{
	int skill, i, j;
	int cost;
	int skillID=0;
	char buf[64];
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char msg[512];
	char message[64];
	int fd = getfdFromCharaIndex( talkerindex );
	double rate= 1.0;
	int skillarray;
	int p_class=0, need_class=0;
	int profession_skill_point;

	memset( message, -1, sizeof(message) );

	if( NPC_Util_CharDistance( talkerindex, meindex ) > 2) {
		return;
	}

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");
		return;
	}

	makeStringFromEscaped( data);
	getStringFromIndexWithDelim(data,"|",1,buf,sizeof(buf));
	skill=atoi(buf);
	getStringFromIndexWithDelim(data,"|",2,buf,sizeof(buf));
	cost=atoi(buf);
	
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "profession_skill", msg, sizeof( msg)) != NULL){
		getStringFromIndexWithDelim(msg,",",skill,buf,sizeof(buf));
		skillID=atoi(buf);
	}


	if(skillID < 0) return ;

	if( CHAR_getWorkInt( CONNECT_getCharaindex(fd), CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "skill_rate", msg, sizeof( msg)) != NULL){
		rate = atof( msg);
	}

	skillarray = PROFESSION_SKILL_getskillArray( skillID );

	if(skillarray == -1){
		CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
		return ;
	}

	// �ж�ְҵ
	{
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "profession_class", msg, sizeof(msg) ) == NULL){
			return ;
		}else{
			p_class = atoi(msg);	
		}
		
		// ��������ְҵ
		need_class = PROFESSION_SKILL_getInt( skillarray, PROFESSION_SKILL_PROFESSION_CLASS );

		if( CHAR_getInt( talkerindex, PROFESSION_CLASS ) == 0 ){
			char token[256];
			memset(token, -1, sizeof(token) );
			sprintf( token, "����δ��ְ!" );
			CHAR_talkToCli( talkerindex, meindex, token, CHAR_COLORYELLOW);
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			return;

		}

		if( CHAR_getInt( talkerindex, PROFESSION_CLASS ) != need_class && need_class != 4 ){
			char token[256];
			memset(token, -1, sizeof(token) );
			sprintf( token, "�˼����㲻��ѧ�!" );
			CHAR_talkToCli( talkerindex, meindex, token, CHAR_COLORYELLOW);
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			return;
		}
		

	}

	// �ж�ʣ�����
	{
		profession_skill_point = CHAR_getInt( talkerindex, PROFESSION_SKILL_POINT );
		if( profession_skill_point <= 0 ){
			char token[256];
			memset(token, -1, sizeof(token) );
			sprintf( token, "��Ŀǰû��ѧϰ����!" );
			CHAR_talkToCli( talkerindex, meindex, token, CHAR_COLORYELLOW);
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			return;
		}
	}

	// �ж��Ƿ���ѧϰ������
	{
		int temp[4] = {0}, count = 0, flag = -1 , need_percent = -1, need_count = 0;

		for( i=0; i<4; i++){
			int limit = -1;
			flag = -1;
			limit = PROFESSION_SKILL_getInt( skillarray, PROFESSION_SKILL_LIMIT1+i*2 );
			need_percent = PROFESSION_SKILL_getInt( skillarray, PROFESSION_SKILL_LIMIT1+i*2 + 1 );
					
			if( limit!= 0  && need_percent == 0 ) need_count++;

			if( limit == 0 ){
				continue;
			}else if( limit == -1 ){
				for( j=0; j<PROFESSION_MAX_LEVEL; j++ ){
					if( CHAR_getCharSkill( talkerindex, j) > 0 ) break;
				}
				if( j >=  PROFESSION_MAX_LEVEL ){
					char token[256];	
					sprintf( token, "����ѧ���Σ���ս�����ܲ���ѧϰ��");
					CHAR_talkToCli( talkerindex, meindex, token, CHAR_COLORYELLOW);
					CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
					return;
				}
			}else{
				flag = -1;
				for( j=0; j<PROFESSION_MAX_LEVEL; j++ ){
					// �жϼ���
					if( CHAR_getCharSkill( talkerindex, j) == limit ){
						int skill_level = -1;
						CHAR_HaveSkill* hskill;
						flag = -2;
						hskill = CHAR_getCharHaveSkill( talkerindex, j );
						skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);	
						
						// �ж�������
						if( skill_level >= need_percent ){
							flag = 1;	break;
						}					
					}
				}
				
				// δ�������
				if( flag == -1 && need_percent == 0 ){
						temp[count] = limit;
						count ++;					
				}else if( flag == -1 ){
					char token[256];	
					memset(token, -1, sizeof(token) );
					sprintf( token, "��δѧ��%s!", PROFESSION_SKILL_getChar( limit, PROFESSION_SKILL_NAME) );
					CHAR_talkToCli( talkerindex, -1, token, CHAR_COLORYELLOW);
					CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
					return;
				}else if( flag == -2 ){
					char token[256];	
					memset(token, -1, sizeof(token) );
					sprintf( token, "%s�����Ȳ���%d!", PROFESSION_SKILL_getChar( limit, PROFESSION_SKILL_NAME), need_percent );
					CHAR_talkToCli( talkerindex, -1, token, CHAR_COLORYELLOW);
					CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
					return;
				}
			}
		}
	
		if( count == need_count &&  need_count != 0 ){
			char msg[256] = {0};
		
			sprintf( msg, "����ѧ�� " );
			for( i = 0; i< count; i++ ){
				if( temp[i] != 0 ){
					char token[50] = {0};
					sprintf( token, "%s ", PROFESSION_SKILL_getChar( temp[i], PROFESSION_SKILL_NAME) );
					strcat( msg, token );
				}
			}
			sprintf( msg, "%s ��һ�ּ���", msg );
			CHAR_talkToCli( talkerindex, -1, msg, CHAR_COLORYELLOW);
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);

			return;
		}

	}

	// �жϽ�Ǯ	
	{
		cost = PROFESSION_SKILL_getInt( skillarray, PROFESSION_SKILL_COST );
		cost = cost * rate;

		if(CHAR_getInt(talkerindex,CHAR_GOLD) < cost){
			CHAR_send_P_StatusString( talkerindex, CHAR_P_STRING_GOLD);
			return;
		}
	}

#ifdef _NPC_ProfessionTrans
	//�ж�ת��
	{
		if( NPC_Util_GetStrFromStrWithDelim( argstr, "trans", msg, sizeof(msg) ) != NULL){
		    int trans = atoi(msg);
			print("\nת��Ҫ��:%d,%d",CHAR_getInt( talkerindex, CHAR_TRANSMIGRATION),trans);
			if( CHAR_getInt( talkerindex, CHAR_TRANSMIGRATION) < trans){ 
				if( NPC_Util_GetStrFromStrWithDelim( argstr, "trans_msg", msg, sizeof(msg) ) != NULL){
                    CHAR_talkToCli( talkerindex, meindex, msg, CHAR_COLORYELLOW);
				    return;
				}
				else{
                    CHAR_talkToCli( talkerindex, meindex, "����ת����������Ŷ��", CHAR_COLORYELLOW);
				    return;
				}
			}
		}
	}
#endif

	// ���Ӽ���
	{
		int skill_level = 0;

		// ��ͨ���������ȸ�50�������10
		if( (skillID == 63) || (skillID == 64) || (skillID == 65)  ){
			int	Pskillid = -1;
			skill_level = 50;
			Pskillid = PROFESSION_SKILL_getskillArray( skillID );
		}else{
			skill_level = 10;
		}

		if( PROFESSION_SKILL_ADDSK( talkerindex, skillID, skill_level ) == -1 )	return;

		CHAR_DelGold( talkerindex, cost );

		CHAR_sendStatusString( talkerindex , "S");

		NPC_Welfare_selectWindow( meindex, talkerindex,0,-1);
	}

	// ����ѶϢ
	{
		char token[256];
		int next_profession_skill_point = profession_skill_point-1;
		
		CHAR_setInt( talkerindex, PROFESSION_SKILL_POINT, next_profession_skill_point );
		
		memset(token, -1, sizeof(token) );
		sprintf( token, "��ѧϰ�� %s��ʣ��ѧϰ���� %d"
				,PROFESSION_SKILL_getChar( skillarray, PROFESSION_SKILL_NAME )
				,next_profession_skill_point );
		CHAR_talkToCli( talkerindex, -1, token, CHAR_COLORYELLOW);
    
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( talkerindex , CHAR_WORKOBJINDEX ));
	}

}             

void NPC_WelfareMakeStr(int meindex,int toindex,int select)
{
	char argstr[NPC_UTIL_GETARGSTR_BUFSIZE];
	char msg[256];
	double rate=1.0;
	int i=0;
	char token[65530];
	int fd = getfdFromCharaIndex( toindex);

	if(select==0){
		sprintf(token,"0|0");
		lssproto_WN_send( fd, WINDOW_MESSAGETYPE_PETSKILLSHOP, 
			WINDOW_BUTTONTYPE_NONE, 
			CHAR_WINDOWTYPE_WINDOWPETSKILLSHOP,
			CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
			token);
		
	}

	if(NPC_Util_GetArgStr( meindex, argstr, sizeof(argstr))==NULL){
		print("GetArgStrErr");
		return;
	}

	if( NPC_Util_GetStrFromStrWithDelim( argstr, "main_msg", msg, sizeof( msg)) == NULL){
		print("mainERR");
		return ;
	}

	sprintf(token,"1|%s|%s", CHAR_getChar(meindex,CHAR_NAME), msg);
		
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "skill_rate", msg, sizeof( msg)) != NULL){
		rate=atof(msg);
	}

		
	if( NPC_Util_GetStrFromStrWithDelim( argstr, "profession_skill", msg, sizeof( msg) ) != NULL){
		int skillarray;
		int skillID;
		int cost;
		char token2[265];
		char buf[64];

		i=1;
	    while( getStringFromIndexWithDelim(msg,",",i,buf,sizeof(buf)) !=FALSE ){
			i++;
			skillID=atoi(buf);

			skillarray = PROFESSION_SKILL_getskillArray( skillID );

			if( PROFESSION_SKILL_CHECKINDEX( skillarray ) == FALSE )		continue;

			cost = PROFESSION_SKILL_getInt( skillarray, PROFESSION_SKILL_COST );

			cost = (int)cost*rate;

			sprintf(token2,"|%s|%d|%s|%d",
				PROFESSION_SKILL_getChar( skillarray, PROFESSION_SKILL_NAME ),	// ��������
				cost,															// ���
				PROFESSION_SKILL_getChar( skillarray, PROFESSION_SKILL_TXT ),	// ˵��
				PROFESSION_SKILL_getInt( skillarray, PROFESSION_SKILL_ICON )	// ͼʾ
			);
			strcat(token,token2);
		} 
	}

	lssproto_WN_send( fd, WINDOW_MESSAGETYPE_PROFESSIONSHOP, 
				WINDOW_BUTTONTYPE_NONE, 
				WINDOW_MESSAGETYPE_PROFESSIONSHOP,
				CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX),
				token);

	return;
}


#endif
#endif


