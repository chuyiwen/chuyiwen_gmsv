#include "version.h"


#ifdef _PROFESSION_SKILL			// WON ADD ����ְҵ����
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "buf.h"
#include "profession_skill.h"
#include "char_base.h"
#include "battle.h"
#include "magic_base.h"
#include "configfile.h"
#include "char.h"
#include "lssproto_serv.h"
#include "skill.h"

static Professionskill	*PROFESSION_skill;

typedef struct tagProfessionskill_skillFunctionTable
{
	char			*functionname;
	PROFESSION_SKILL_CALLFUNC	func;
	int				hash;
//	int				status;
}PROFESSION_SKILL_skillFunctionTable;

static PROFESSION_SKILL_skillFunctionTable PROFESSION_SKILL_functbl[] = {

// ��ʦ����
	{ "PROFESSION_VOLCANO_SPRINGS", PROFESSION_volcano_springs, 0 },	// ��ɽȪ
	{ "PROFESSION_FIRE_BALL",		PROFESSION_fire_ball,		0 },	// ������
	{ "PROFESSION_FIRE_SPEAR",		PROFESSION_fire_spear,		0 },	// ����ǹ
	{ "PROFESSION_SUMMON_THUNDER",  PROFESSION_summon_thunder,  0 },	// ������
	{ "PROFESSION_CURRENT",			PROFESSION_current,			0 },	// ������
	{ "PROFESSION_STORM",			PROFESSION_storm,			0 },	// ������
	{ "PROFESSION_ICE_ARROW",		PROFESSION_ice_arrow,		0 },	// ������
	{ "PROFESSION_ICE_CRACK",		PROFESSION_ice_crack,		0 },	// ������
	{ "PROFESSION_ICE_MIRROR",		PROFESSION_ice_mirror,		0 },	// ������
	{ "PROFESSION_DOOM",			PROFESSION_doom,			0 },	// ����ĩ��
	{ "PROFESSION_BLOOD",			PROFESSION_blood,			0 },	// ��Ѫ����
	{ "PROFESSION_BLOOD_WORMS",		PROFESSION_blood_worms,		0 },	// ��Ѫ��
	{ "PROFESSION_SIGN",			PROFESSION_sign,			0 },	// һ���Ѫ
	{ "PROFESSION_FIRE_ENCLOSE",	PROFESSION_fire_enclose,	0 },	// ����
	{ "PROFESSION_ICE_ENCLOSE",		PROFESSION_ice_eclose,		0 },	// ������
	{ "PROFESSION_THUNDER_ENCLOSE",	PROFESSION_thunder_enclose, 0 },	// �׸���
	{ "PROFESSION_FIRE_PRACTICE",	PROFESSION_fire_practice,	0 },	// ��ħ��������
	{ "PROFESSION_ICE_PRACTICE",	PROFESSION_ice_practice,	0 },	// ��ħ��������
	{ "PROFESSION_THUNDER_PRACTICE",PROFESSION_thunder_practice,0 },	// ��ħ��������
	{ "PROFESSION_ENCLOSE",			PROFESSION_enclose,			0 },	// ������
	{ "PROFESSION_TRANSPOSE",		PROFESSION_transpose,		0 },	// ���λ�λ
//	{ "PROFESSION_ALCHEMY",			PROFESSION_alchemy,			0 },	// ������

// ��ʿ����
	{ "PROFESSION_CHAIN_ATK",		PROFESSION_chain_atk,		0 },	// ��������
	{ "PROFESSION_AVOID",			PROFESSION_avoid,			0 },	// �ر�
	{ "PROFESSION_WEAPON_FOCUS",	PROFESSION_weapon_focus,	0 },	// ����ר��	
	{ "PROFESSION_REBACK",			PROFESSION_reback,			0 },	// ״̬�ظ�
	{ "PROFESSION_BRUST",			PROFESSION_brust,			0 },	// ����
	{ "PROFESSION_CHAIN_ATK_2",		PROFESSION_chain_atk_2,		0 },	// ˫�ع���
	{ "PROFESSION_SCAPEGOAT",		PROFESSION_scapegoat,		0 },	// ����Ϊ��
	{ "PROFESSION_ENRAGE",			PROFESSION_enrage,			0 },	// ��������
	{ "PROFESSION_ENERGY_COLLECT",	PROFESSION_energy_collect,	0 },	// �����ۼ�
	{ "PROFESSION_FOCUS",			PROFESSION_focus,			0 },	// רעս��
	{ "PROFESSION_SHIELD_ATTACK",	PROFESSION_shield_attack,	0 },	// �ܻ�
	{ "PROFESSION_DUAL_WEAPON",		PROFESSION_dual_weapon,		0 },	// ������	
	{ "PROFESSION_DEFLECT",			PROFESSION_deflect,			0 },	// ��
	{ "PROFESSION_THROUGH_ATTACK",	PROFESSION_through_attack,	0 },	// �ᴩ����
	{ "PROFESSION_CAVALRY",			PROFESSION_cavalry,			0 },	// ���﹥��
	{ "PROFESSION_DEAD_ATTACK",		PROFESSION_dead_attack,		0 },	// ��������
	{ "PROFESSION_CONVOLUTE",		PROFESSION_convolute,		0 },	// ��������
	{ "PROFESSION_CHAOS",			PROFESSION_chaos,			0 },	// ���ҹ���

// ����
	{ "PROFESSION_TRACK",			PROFESSION_track,			0 },	// ׷Ѱ����
	{ "PROFESSION_ESCAPE",			PROFESSION_escape,			0 },	// �ر�ս��
	{ "PROFESSION_DOCILE",			PROFESSION_docile,			0 },	// ѱ������
	{ "PROFESSION_TRAP",			PROFESSION_trap,			0 },	// ����
	{ "PROFESSION_ENRAGE_PET",		PROFESSION_enrage_pet,		0 },	// ��ŭ����
	{ "PROFESSION_DRAGNET",			PROFESSION_dragnet,			0 },	// ���޵���
	{ "PROFESSION_ENTWINE",			PROFESSION_entwine,			0 },	// ��������
	{ "PROFESSION_AUTARKY",			PROFESSION_autarky,			0 },	// �Ը�����
	{ "PROFESSION_PLUNDER",			PROFESSION_plunder,			0 },	// �����Ӷ�
	{ "PROFESSION_TOXIN_WEAPON",	PROFESSION_toxin_weapon,	0 },	// ��������
	{ "PROFESSION_RESIST_FIRE",		PROFESSION_resist_fire,		0 },	// ����
	{ "PROFESSION_RESIST_ICE",		PROFESSION_resist_ice,		0 },	// ������
	{ "PROFESSION_RESIST_THUNDER",	PROFESSION_resist_thunder,	0 },	// �׿���
#ifdef _PROFESSION_ADDSKILL
    { "PROFESSION_RESIST_F_I_T",    PROFESSION_resist_f_i_t,    0 },    // ��Ȼ����
	{ "PROFESSION_CALL_NATURE",     PROFESSION_call_nature,     0 },    // ������Ȼ
	{ "PROFESSION_BOUNDARY",        PROFESSION_boundary,     0 },       // �����Խ��
#endif
	{ "PROFESSION_G_RESIST_FIRE",	PROFESSION_g_resist_fire,	0 },	// �������
	{ "PROFESSION_G_RESIST_ICE",	PROFESSION_g_resist_ice,	0 },	// ���������
	{ "PROFESSION_G_RESIST_THUNDER",PROFESSION_g_resist_thunder,0 },	// �����׿���
	{ "PROFESSION_ATTACK_WEAK",		PROFESSION_attack_weak,		0 },	// ���㹥��
	{ "PROFESSION_INSTIGATE",		PROFESSION_instigate,		0 },	// ����
	{ "PROFESSION_OBLIVION",		PROFESSION_oblivion,		0 },	// ����

// ��ͬ����
	{ "PROFESSION_FULL_MP",			PROFESSION_full_mp,			0 },	// ��������
	{ "PROFESSION_STRONG_BACK",		PROFESSION_strong_back,		0 },	// ��������
	{ "PROFESSION_STRENGTHEN",		PROFESSION_strengthen,		0 },	// ����ǿ��

};


static int profession_skill_num;				// ְҵ��������

// �� profession.txt ����
BOOL PROFESSION_initSkill( char *filename)
{

	FILE*   f;
    char    line[256];
    int     linenum=-1;
	int     skill_readlen=0;
	int		i, j;

	int max_skillid = 0;

    f = fopen(filename,"r");
    if( f == NULL ){
        print( "file open error\n");
        return FALSE;
    }

    profession_skill_num=0;

    while( fgets( line, sizeof( line ), f ) ){
		char    token[256];

        linenum ++;
        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;
        chomp( line );
        profession_skill_num++;
#ifdef _PROSKILL_OPTIMUM // Robin ȡ�����ְ��ID
		if( getStringFromIndexWithDelim( line, ",", 
				PROFESSION_SKILL_DATACHARNUM+PROFESSION_SKILL_ID+1,
				token, sizeof(token)) == FALSE )
			continue;
		max_skillid = max( atoi( token), max_skillid);
#endif
    }

    if( fseek( f, 0, SEEK_SET ) == -1 ){
        fprint( "Seek Error\n" );
        fclose(f);
        return FALSE;
    }

#ifdef _PROSKILL_OPTIMUM // ְ��Table��Ϊ �����ID +1
	profession_skill_num = max_skillid +1;
	print("���ְ��ID = %d ...", max_skillid);
#endif

	PROFESSION_skill = allocateMemory( sizeof(struct tagProfessionkill) * profession_skill_num + 1 );


	if( PROFESSION_skill == NULL ){
        fprint( "�޷������ڴ� %d\n", sizeof(struct tagProfessionkill)*profession_skill_num);
        fclose( f );
        return FALSE;
    }

	// init skill
    for( i = 0; i < profession_skill_num; i ++ ) {
    	for( j = 0; j < PROFESSION_SKILL_DATAINTNUM; j ++ )
    		PROFESSION_SKILL_setInt( i,j,-1);
    
    	for( j = 0; j < PROFESSION_SKILL_DATACHARNUM; j ++ ) 
    		PROFESSION_SKILL_setChar( i,j,"");    
    }

    linenum = -1;
    while( fgets( line, sizeof( line ), f ) ){
        linenum ++;
        if( line[0] == '#' )continue;
        if( line[0] == '\n' )continue;             
                
        chomp( line );

        replaceString( line, '\t' , ' ' );

		{
		    char    buf[256];
			for( i = 0; i < strlen( line); i ++) {
				if( line[i] != ' ' ) {
					break;
				}
				strcpysafe( buf,sizeof(buf), &line[i]);
			}
			
			if( i != 0 ) {
				strcpysafe( line,sizeof(line), buf);
			}
		}  
		{
			char    token[256];
			int     ret;

#ifdef _PROSKILL_OPTIMUM // ��ȡ����ְ����ID, ֱ����ְ��ID��Table index
			ret = getStringFromIndexWithDelim( line, ",", PROFESSION_SKILL_DATACHARNUM+PROFESSION_SKILL_ID+1, token, sizeof(token));
			if( ret==FALSE ){
				//fprint("Syntax Error file:%s line:%d\n",filename,linenum);
				break;
			}
			skill_readlen = atoi( token);
#endif
			for( i = 0; i < PROFESSION_SKILL_DATACHARNUM; i ++ ) {

			    ret = getStringFromIndexWithDelim( line,",", i + 1, token,sizeof(token));
				if( ret==FALSE ){
					//fprint("Syntax Error file:%s line:%d\n",filename,linenum);
					break;
				} 
				PROFESSION_SKILL_setChar( skill_readlen, i, token);
			} 

			for( i = 0; i < PROFESSION_SKILL_DATAINTNUM; i ++ ) {
				ret = getStringFromIndexWithDelim( line, ",", i+PROFESSION_SKILL_DATACHARNUM+1, token, sizeof(token));
				
				if( ret==FALSE ){
					//fprint("�ļ��﷨����:%s ��%d��\n",filename,linenum);
					break;
				} 
				if( strlen( token) != 0 ) {
					PROFESSION_SKILL_setInt( skill_readlen, i , atoi( token ));
				} 
			}         		
			skill_readlen ++;
		} 		
    } 
    fclose(f);

    profession_skill_num = skill_readlen;

	for( i = 0; i < arraysizeof( PROFESSION_SKILL_functbl); i ++ ){
		PROFESSION_SKILL_functbl[i].hash = hashpjw( PROFESSION_SKILL_functbl[i].functionname );
	}

    return TRUE;

}


// �������� profession.txt
void rePROFESSION_initSkill()
{
	freeMemory( PROFESSION_skill );
	PROFESSION_initSkill(getProfession());
}

INLINE BOOL PROFESSION_SKILL_CHECKINDEX( int index )
{
    if( profession_skill_num < index || index < 0 )return FALSE;
    return TRUE;
}

static INLINE BOOL PROFESSION_SKILL_CHECKINTDATAINDEX( int index)
{
	if( PROFESSION_SKILL_DATAINTNUM < index || index < 0 ) return FALSE;
	return TRUE;
}

static INLINE BOOL PROFESSION_SKILL_CHECKCHARDATAINDEX( int index)
{
	if( PROFESSION_SKILL_DATACHARNUM < index || index < 0 ) return FALSE;
	return TRUE;
}

INLINE int PROFESSION_SKILL_getInt( int index, PROFESSION_SKILL_DATAINT element)
{
	return PROFESSION_skill[index].data[element];
}

INLINE int PROFESSION_SKILL_setInt( int index, PROFESSION_SKILL_DATAINT element, int data)
{
	int buf;
	buf = PROFESSION_skill[index].data[element];
	PROFESSION_skill[index].data[element]=data;
	return buf;
}

INLINE char* PROFESSION_SKILL_getChar( int index, PROFESSION_SKILL_DATACHAR element)
{
	if( !PROFESSION_SKILL_CHECKINDEX( index)) return "\0";
	if( !PROFESSION_SKILL_CHECKCHARDATAINDEX( element)) return "\0";
	return PROFESSION_skill[index].string[element].string;
}

INLINE BOOL PROFESSION_SKILL_setChar( int index ,PROFESSION_SKILL_DATACHAR element, char* new )
{
    if(!PROFESSION_SKILL_CHECKINDEX(index))return FALSE;
    if(!PROFESSION_SKILL_CHECKCHARDATAINDEX(element))return FALSE;

    strcpysafe( PROFESSION_skill[index].string[element].string,
                sizeof(PROFESSION_skill[index].string[element].string),
                new );
    return TRUE;
}

int PROFESSION_SKILL_getskillNum( void)
{
	return profession_skill_num;
}


/*----------------------------------------------------------------------*/

int PROFESSION_SKILL_getskillArray( int skillid)
{
#ifdef _PROSKILL_OPTIMUM // ��ѯְ��ID��Ϊ����ʽ
	if( skillid >= 0 && skillid < profession_skill_num )
		return PROFESSION_skill[skillid].data[PROFESSION_SKILL_ID];
#else
	int		i;
	for( i = 0; i <= profession_skill_num; i ++ ) { // ���Ż�
		if( PROFESSION_skill[i].data[PROFESSION_SKILL_ID] == skillid ) 
			return i;
	}
#endif
	return -1;
}

PROFESSION_SKILL_CALLFUNC PROFESSION_SKILL_getskillFuncPointer(char* name )
{
    int i;
    int hash = hashpjw( name );
	
	for( i = 0; i< arraysizeof( PROFESSION_SKILL_functbl); i++ ){
		if( PROFESSION_SKILL_functbl[i].hash == hash ){
        	if( strcmp( PROFESSION_SKILL_functbl[i].functionname, name ) == 0 ){
	          return PROFESSION_SKILL_functbl[i].func;
			}
		}
	}

    return NULL;
}

int _PROFESSION_SKILL_GetArray( char *file, int line, int charaindex, int skill )
{
	int		skillid,array;

	skillid = CHAR_getCharSkill( charaindex, skill);

	if( skillid < 0 )	return -1;

	array = PROFESSION_SKILL_getskillArray( skillid );
		
	return array;
}


/*----------------------------------------------------------------------*/

// ʹ��ְҵ����
int PROFESSION_SKILL_Use( int charaindex, int skill, int toNo, char *data )
{
	int		Pskillid, skillid;
	int		ret=-1, skill_level;
	PROFESSION_SKILL_CALLFUNC func;
	CHAR_HaveSkill* hskill;
	int char_pskill, profession_skill;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	//print("\n skillid:%d Pskillid:%d ", skillid, Pskillid); // Robin log
	if( Pskillid == -1 ) {
//		printf("\n �ķ��??�Ҳ�����ְ��! ");
		return ret;
	}

#ifdef _PROSKILL_OPTIMUM	// Robin fix ���ְҵ�Ƿ����
	char_pskill = CHAR_getInt( charaindex, PROFESSION_CLASS );
	profession_skill = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);
	if( (char_pskill <=0) || (char_pskill != profession_skill) ) {
//		print("\n �ķ��??ְ����ְҵ����ȷ!:%d:%d ", char_pskill, profession_skill);
		return ret;
	}
#endif

	// ��Ѱ��Ӧ function
	func = PROFESSION_SKILL_getskillFuncPointer( PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME) );

	// �ж��Ƿ�Ϊ����
	if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER )	return ret;


	if( func ){
		// ���＼�ܵȼ�
		hskill = CHAR_getCharHaveSkill( charaindex, skill );
		skill_level = SKILL_getInt( &hskill->skill, SKILL_LEVEL);
	
		// ���� MP 
		if( PROFESSION_SKILL_DEC_COST_MP( charaindex, skill, Pskillid, skill_level ) != 1 )	{
//			print("\n Error:ְ��MP:%d:%d:%d ", skill, Pskillid, skill_level);
			return ret;
		}

		// ʹ��ְҵ����
		ret = func( charaindex, toNo, Pskillid, data, skill_level );
		if( ret == -1 ){
			int rands = rand()%10;
			//andy_log
//			print("ret = %d, rands:%d \n", ret, rands);
			if( rands > 5 ) return ret;
		}

		if( Pskillid == 57 //��ŭ���� ѡ������о���ֵ
			&& CHAR_getInt( BATTLE_No2Index( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEINDEX ), toNo ), CHAR_WHICHTYPE) != CHAR_TYPEPET ) 
			return ret;

		// һ�㼼������
		PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );
	}
	else {
		//print("\n ����!�Ҳ���ְ��func ");
	}

	return ret;
}

// ���⼼������
void PROFESSION_SKILL_LVEVEL_UP( int charaindex, char *name )
{
	int skill=-1;
	int skillid=0, Pskillid=0, i;
	char *skill_name="\0";
	CHAR_HaveSkill* hskill;

	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
			
		// ����ID
		skillid = CHAR_getCharSkill( charaindex, i);
		Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
		if( Pskillid < 0 ) continue;

		// ��������
		skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
		if( skill_name == "\0" ) continue;
			
		if( (strcmp( skill_name , name )) == 0 ){
			skill = i;		break;
		}
	}	

	if( (skill < 0) || (skill > CHAR_SKILLMAXHAVE) ) return;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	if( Pskillid == -1 )	return;

	// ���＼�ܵȼ�
	hskill = CHAR_getCharHaveSkill( charaindex, skill );
	
	// һ�㼼������
	PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );

	return;
}

// ��������������
void PROFESSION_SKILL_DUAL_WEAPON_LVEVEL_UP( int charaindex, char *name )
{
	int skill=-1, arm = 0, shield = 0;
	int skillid=0, Pskillid=0, i;
	char *skill_name="\0";
	CHAR_HaveSkill* hskill;

	// ���������װ��
	for( i = 0 ; i < CHAR_EQUIPPLACENUM ; i ++ ){
		int id = CHAR_getItemIndex(charaindex,i);
		if( ITEM_CHECKINDEX(id) ){
			if( i == CHAR_ARM ) arm = 1;
			else if( i == CHAR_EQSHIELD ) shield = 1;			
		}
	}

	if( arm != 1 || shield != 1 )	return;	

	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
		
		// ����ID
		skillid = CHAR_getCharSkill( charaindex, i);
		Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
		if( Pskillid < 0 ) continue;

		// ��������
		skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
		if( skill_name == "\0" ) continue;
			
		if( (strcmp( skill_name , name )) == 0 ){
			skill = i;		break;
		}
	}

	if( (skill < 0) || (skill > CHAR_SKILLMAXHAVE) ) return;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	if( Pskillid == -1 )	return;

	// ���＼�ܵȼ�
	hskill = CHAR_getCharHaveSkill( charaindex, skill );
	
	// һ�㼼������
	PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );

	return;
}


// ����ר����������
void PROFESSION_SKILL_WEAPON_FOCUS_LVEVEL_UP( int charaindex, char *name )
{
	int skill = -1;
	int skillid=0, Pskillid = 0, i;
	char *skill_name="\0", *pszOption="\0", *pszP, diff[20];
	int WeaponType = -1;
	CHAR_HaveSkill* hskill;

	memset( diff, -1, sizeof(diff) );
	// װ������
	WeaponType = BATTLE_GetWepon( charaindex );

	switch( WeaponType ){
		case ITEM_AXE:		
			sprintf( diff, "��" ); break;
		case ITEM_CLUB:		
			sprintf( diff, "��" ); break;
		case ITEM_SPEAR:	
			sprintf( diff, "ǹ" ); break;
		case ITEM_BOW:		
			sprintf( diff, "��" ); break;
		case ITEM_BOOMERANG:	// ������	
			sprintf( diff, "��" ); break;
		case ITEM_BOUNDTHROW:	// Ͷ����ͷ
			sprintf( diff, "Ͷ" ); break;
		case ITEM_BREAKTHROW:	// Ͷ��ʯ
			sprintf( diff, "ʯ" ); break;
		default: 
			sprintf( diff, "��" ); break;
	}

	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
		// ����ID
		skillid = CHAR_getCharSkill( charaindex, i);
		Pskillid = PROFESSION_SKILL_getskillArray( skillid);		
		if( Pskillid < 0 ) continue;

		// ��������
		skill_name = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_FUNCNAME);
		if( skill_name == "\0" ) continue;
	
		// ���ܲ���
		pszOption = PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_OPTION);
		if( pszOption == "\0" ) continue;
		
		if( (strcmp( skill_name , name )) == 0 ){
			if( (pszP=strstr( pszOption, diff ))!=NULL ){
				skill = i;		break;
			}
		}
	}	

	if( (skill < 0) || (skill > CHAR_SKILLMAXHAVE) ) return;

	skillid = CHAR_getCharSkill( charaindex, skill);
	Pskillid = PROFESSION_SKILL_getskillArray( skillid);
	if( Pskillid == -1 )	return;

	// ���＼�ܵȼ�
	hskill = CHAR_getCharHaveSkill( charaindex, skill );
	
	// һ�㼼������
	PROFESSION_NORMAL_SKILL_LEVLE_UP( &hskill->skill, Pskillid, charaindex );

	return;
}

// ���� MP 
int PROFESSION_SKILL_DEC_COST_MP( int charaindex, int skill, int Pskillid, int skill_level )
{	
	
	if(skill_level > 0){
		int	dec_mp;
		int old_mp = CHAR_getInt( charaindex, CHAR_MP );
		int mp=0;
		// ȡ������ MP 
		if( (dec_mp = PROFESSION_MAGIC_COST_MP( charaindex, skill )) == -1 )
			dec_mp = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_COST_MP );
		// ��� MP��			
		if( old_mp < dec_mp ) return -1;
		// ��Ѫ���� 
		if( Pskillid != 11
#ifdef _PROFESSION_ADDSKILL
		//	&& Pskillid != 2 //������Բ��� MP
#endif
			)
			if( dec_mp <= 0 ) return -1;
		// �۵����� MP
		mp = old_mp - dec_mp;
		if( mp < 0 ) mp = 0;
		CHAR_setInt( charaindex , CHAR_MP , mp );
		CHAR_send_P_StatusString( charaindex , CHAR_P_STRING_MP);
		return 1;
	}
	return -1;
}



// һ�㼼������
void PROFESSION_NORMAL_SKILL_LEVLE_UP(Skill *skill, int Pskillid, int charaindex )
{
	int skill_level = 0, up_fix_value = 0;
	int rand_num = RAND( 0, 10000 ), rand_num2 = 0;
	
	// ���ܵȼ��������޲�����
	skill_level = SKILL_getRealInt( skill, SKILL_LEVEL);
	if( skill_level >= PROFESSION_SKILL_MAX_LEVEL * 100 ) return;

	// ����������ֵ
	up_fix_value = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_FIX_VALUE) * 100;
	rand_num2 = RAND( 0, up_fix_value);

	// �����춨
	if( rand_num > skill_level + rand_num2){
		// ����������
		skill_level += PROFESSION_SKILL_ADD_POINT;	
		SKILL_setInt( skill, SKILL_LEVEL, skill_level );	

		if( ( skill_level % 100 ) == 0 ){
			char msg[256];
			sprintf( msg, "%s��������������Ϊ%d", 	PROFESSION_SKILL_getChar( Pskillid, PROFESSION_SKILL_NAME), skill_level/100 );
			CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
			CHAR_sendStatusString( charaindex , "S");	

			// ְҵ����ֵ����		
			PROFESSION_LEVEL_CHECK_UP( charaindex );
		}
	}
	return;
}


void PROFESSION_LEVEL_CHECK_UP( int charaindex )
{
	int skill_level_add = 0, skill_level_sum = 0, i = 0;
	int old_level = CHAR_getInt( charaindex, PROFESSION_LEVEL );
	int next_level_need_point = 9999999;
	CHAR_HaveSkill* hskill;

	// ��һ��ְҵ��������
	next_level_need_point = old_level * 70 * 100;
		
	for( i=0; i<CHAR_SKILLMAXHAVE; i++ ){
		int skillID = -1;

		// ���ܵȼ�
		hskill = CHAR_getCharHaveSkill( charaindex, i );
		skillID = SKILL_getInt( &hskill->skill, SKILL_IDENTITY);
		if( skillID <= 0 ) continue;

		// ��ͨ���������� 50
		if( (skillID == 63) || (skillID == 64) || (skillID == 65) ){
			skill_level_add = 50*100;	
		}else{
			skill_level_add = SKILL_getRealInt( &hskill->skill, SKILL_LEVEL);
		}
		
		skill_level_sum += skill_level_add;	
	}
	
//	print("\n�������������� ==> ��ǰ�����ȵ���(%d) ��һ����������������(%d)", skill_level_sum, next_level_need_point );

	// ���������ȴﵽ��һ��ְҵ��������ʱ��ְҵ�ȼ�����
	if(  skill_level_sum >= next_level_need_point ){
		int old_skill_point = CHAR_getInt( charaindex, PROFESSION_SKILL_POINT );
		int next_skill_point = old_skill_point+1;
		char msg[64];

//		print("\nְҵ�ȼ����� ==> ��ǰְҵ�ȼ�����(%d) ��һ��ְҵ�ȼ���������(%d)", old_skill_point, next_skill_point );
		
		CHAR_setInt( charaindex, PROFESSION_LEVEL, old_level + 1 );
		CHAR_setInt( charaindex, PROFESSION_SKILL_POINT, next_skill_point );
		CHAR_sendStatusString( charaindex , "P");
		
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( charaindex , CHAR_WORKOBJINDEX ));

		sprintf( msg, "ְҵ�ȼ�����Ϊ%d�������ܵ�����������", old_level + 1 );
		CHAR_talkToCli( charaindex, -1, msg, CHAR_COLORYELLOW);
	}
}

// ���Ӽ���
int PROFESSION_SKILL_ADDSK( int charaindex, int skill, int level )
{
	int i;

	CHAR_HaveSkill *pSkil;

	if( level > 100 )		level = 100;
	else if( level < 1 )	level = 1;

	for( i = 0; i < CHAR_SKILLMAXHAVE; i ++ ){
		int skill_id = -1;

		pSkil = CHAR_getCharHaveSkill( charaindex, i );
		skill_id = SKILL_getInt( &pSkil->skill, SKILL_IDENTITY);
		if( skill == skill_id && skill_id != -1 ){
			CHAR_talkToCli(charaindex,-1, "���Ѿ�ѧϰ���˼���", CHAR_COLORYELLOW);
			return -1;
		}

		if( pSkil == (CHAR_HaveSkill*)NULL )continue;
		if( pSkil->use != 0 )continue;
		break;
	}
	if( i >= CHAR_SKILLMAXHAVE ){
		CHAR_talkToCli(charaindex,-1, "���������Ѵ�����", CHAR_COLORYELLOW);
		return -1;
	}else{
		level *= 100;
		SKILL_makeSkillData( &pSkil->skill, skill, level);
		pSkil->use = TRUE;
	}

	CHAR_sendStatusString( charaindex , "S");
	

	return 1;
}

void profession_common_fun( int charaindex, int toNo, int skill_level, int array, int com1 )
{
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, com1 );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM2, toNo );
	CHAR_setWorkInt( charaindex, CHAR_WORKBATTLEMODE, BATTLE_CHARMODE_C_OK );
	CHAR_SETWORKINT_HIGH( charaindex, CHAR_WORKBATTLECOM3, skill_level);
	CHAR_SETWORKINT_LOW( charaindex, CHAR_WORKBATTLECOM3, array);
#ifdef _PROFESSION_ADDSKILL
	if( com1 == BATTLE_COM_S_DOOM 
		/*&& CHAR_getWorkInt( charaindex, CHAR_DOOMTIME) == 0*/ ){
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
		CHAR_setWorkInt( charaindex, CHAR_WORK_com1, com1 );
		CHAR_setWorkInt( charaindex, CHAR_WORK_toNo, toNo );
		CHAR_setWorkInt( charaindex, CHAR_WORK_mode, BATTLE_CHARMODE_C_OK );
		CHAR_setWorkInt( charaindex, CHAR_WORK_skill_level, skill_level );
		CHAR_setWorkInt( charaindex, CHAR_WORK_array, array );
		CHAR_setWorkInt( charaindex, CHAR_DOOMTIME, 3);
	}
	else if( com1 == BATTLE_COM_S_FIRE_SPEAR 
			/*&& CHAR_getWorkInt( charaindex, CHAR_DOOMTIME) == 0*/ ){
		CHAR_setWorkInt( charaindex, CHAR_WORKBATTLECOM1, BATTLE_COM_NONE );
		CHAR_setWorkInt( charaindex, CHAR_WORK_com1, com1 );
		CHAR_setWorkInt( charaindex, CHAR_WORK_toNo, toNo );
		CHAR_setWorkInt( charaindex, CHAR_WORK_mode, BATTLE_CHARMODE_C_OK );
		CHAR_setWorkInt( charaindex, CHAR_WORK_skill_level, skill_level );
		CHAR_setWorkInt( charaindex, CHAR_WORK_array, array );
		CHAR_setWorkInt( charaindex, CHAR_DOOMTIME, 2);
	}
#endif
}

/*----------------------------------------------------------------------*/
// ����Ϊְҵ����

// ��ʦ����-��ɽȪ
int PROFESSION_volcano_springs( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_VOLCANO_SPRINGS );		
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_fire_ball( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo<21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FIRE_BALL );		
	return TRUE;
}

// ��ʦ����-����ǹ
int PROFESSION_fire_spear( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FIRE_SPEAR );		
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_summon_thunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SUMMON_THUNDER );		
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_current( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CURRENT );		
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_storm( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_STORM );		
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_ice_arrow( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_ARROW );	
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_ice_crack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_CRACK );		
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_ice_mirror( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_MIRROR );		
	return TRUE;
}

// ��ʦ����-����ĩ��
int PROFESSION_doom( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DOOM );		
	return TRUE;
}

// ��ʦ����-��Ѫ����
int PROFESSION_blood( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BLOOD );		
	return TRUE;
}

// ��ʦ����-��Ѫ��
int PROFESSION_blood_worms( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BLOOD_WORMS );		
	return TRUE;
}

// ��ʦ����-һ���Ѫ
int PROFESSION_sign( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SIGN );		
	return TRUE;
}

// ��ʦ����-����
int PROFESSION_fire_enclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FIRE_ENCLOSE );		
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_ice_eclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ICE_ENCLOSE );		
	return TRUE;
}

// ��ʦ����-�׸���
int PROFESSION_thunder_enclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_THUNDER_ENCLOSE );		
	return TRUE;
}

// ��ʦ����-��ħ��������
int PROFESSION_fire_practice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// ��ʦ����-��ħ��������
int PROFESSION_ice_practice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// ��ʦ����-��ħ��������
int PROFESSION_thunder_practice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// ��ʦ����-������
int PROFESSION_enclose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENCLOSE );		
	return TRUE;
}

// ��ʦ����-���λ�λ
int PROFESSION_transpose( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_TRANSPOSE );		
	return TRUE;
}


/*----------------------------------------------------------------------*/

// ��ʿ����-����
int PROFESSION_brust( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BRUST );
	return TRUE;
}

// ��ʿ����-��������
int PROFESSION_chain_atk( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CHAIN_ATK );
	return TRUE;
}

// ��ʿ����-�ر�
int PROFESSION_avoid( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_AVOID );
	return TRUE;
}

// ��ʿ����-��Ѫ
int PROFESSION_recovery( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RECOVERY );
	return TRUE;
}

// ��ʿ����-����ר��
int PROFESSION_weapon_focus( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// ��ʿ����-״̬�ظ�
int PROFESSION_reback( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_REBACK );		
	return TRUE;
}

// ��ʿ����-˫�ع���
int PROFESSION_chain_atk_2( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CHAIN_ATK_2 );		
	return TRUE;
}

// ��ʿ����-����Ϊ��
int PROFESSION_scapegoat( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SCAPEGOAT );		
	return TRUE;
}

// ��ʿ����-��������
int PROFESSION_enrage( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENRAGE );		
	return TRUE;
}

// ��ʿ����-�����ۼ�
int PROFESSION_energy_collect( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_COLLECT );		
	return TRUE;
}

// ��ʿ����-רעս��
int PROFESSION_focus( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_FOCUS );		
	return TRUE;
}

// ��ʿ����-�ܻ�
int PROFESSION_shield_attack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_SHIELD_ATTACK );		
	return TRUE;
}

// ��ʿ����-������
int PROFESSION_dual_weapon( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// ��ʿ����-��
int PROFESSION_deflect( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DEFLECT );		
	return TRUE;
}

// ��ʿ����-�ᴩ����
int PROFESSION_through_attack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_THROUGH_ATTACK );		
	return TRUE;
}

// ��ʿ����-���﹥��
int PROFESSION_cavalry( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CAVALRY );		
	return TRUE;
}

// ��ʿ����-��������
int PROFESSION_dead_attack( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DEAD_ATTACK );		
	return TRUE;
}

// ��ʿ����-��������
int PROFESSION_convolute( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo<21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CONVOLUTE );		
	return TRUE;
}

// ��ʿ����-���ҹ���
int PROFESSION_chaos( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CHAOS );		
	return TRUE;
}

/*----------------------------------------------------------------------*/

// ���˼���-����
int PROFESSION_trap( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_TRAP );		
	return TRUE;
}

// ���˼���-ѱ������
int PROFESSION_docile( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DOCILE );		
	return TRUE;
}

// ���˼���-��ŭ����
int PROFESSION_enrage_pet( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENRAGE_PET );		
	return TRUE;
}

// ���˼���-���޵���
int PROFESSION_dragnet( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_DRAGNET );		
	return TRUE;
}

// ���˼���-��������
int PROFESSION_entwine( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ENTWINE );		
	return TRUE;
}

// ���˼���-�Ը�����
int PROFESSION_autarky( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

// ���˼���-�����Ӷ�
int PROFESSION_plunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_PLUNDER );	
	return TRUE;
}

// ���˼���-��������
int PROFESSION_toxin_weapon( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_TOXIN_WEAPON );	
	return TRUE;
}

// ���˼���-��������
int PROFESSION_resist_fire( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_FIRE );	
	return TRUE;
}

// ���˼���-����������
int PROFESSION_resist_ice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_ICE );	
	return TRUE;
}

// ���˼���-�׿�������
int PROFESSION_resist_thunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_THUNDER );	
	return TRUE;
}

#ifdef _PROFESSION_ADDSKILL
// ���˼���-��Ȼ����
int PROFESSION_resist_f_i_t( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_RESIST_F_I_T );	
	return TRUE;
}
// ���˼���-������Ȼ
int PROFESSION_call_nature( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_CALL_NATURE );	
	return TRUE;
}
// ���˼���-�����Խ��
int PROFESSION_boundary( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_BOUNDARY );	
	return TRUE;
}
#endif

// ���˼���-�����������
int PROFESSION_g_resist_fire( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_G_RESIST_FIRE );	
	return TRUE;
}

// ���˼���-�������������
int PROFESSION_g_resist_ice( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_G_RESIST_ICE );	
	return TRUE;
}

// ���˼���-�����׿�������
int PROFESSION_g_resist_thunder( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo!=20 && toNo!=21)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_G_RESIST_THUNDER );	
	return TRUE;
}

// ���˼���-���㹥��
int PROFESSION_attack_weak( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_ATTACK_WEAK );	
	return TRUE;
}

// ���˼���-����
int PROFESSION_instigate( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_INSTIGATE );	
	return TRUE;
}

// ���˼���-����
int PROFESSION_oblivion( int charaindex, int toNo, int array, char *data, int skill_level )
{
	if(toNo>19)	
		toNo=0;
	profession_common_fun( charaindex, toNo, skill_level, array, BATTLE_COM_S_OBLIVION );	
	return TRUE;
}


/*----------------------------------------------------------------------*/
// ��ս������

// ���˼���-׷Ѱ����
int PROFESSION_track( int charaindex, int toNo, int array, char *data, int skill_level )
{
	char *pszP = NULL, *pszOption = "\0", msg[20];
	int per=0, rate=0, img1, ret = 1;
	skill_level /= 10;

	if( CHAR_getWorkInt( charaindex, CHAR_ENCOUNT_NUM) >= (int)time(NULL) ){
		ret = -1;
	}

	// ���ܵĲ���
	pszOption = PROFESSION_SKILL_getChar( array, PROFESSION_SKILL_OPTION);

	// ���ܵȼ� rate
	if( (pszP = strstr( pszOption, "��%" ) ) != NULL )	sscanf( pszP+3, "%d", &rate );
	per = skill_level * rate;

	// ׼���ڵĶ���
	img1 = PROFESSION_SKILL_getInt( array, PROFESSION_SKILL_IMG_1 );

	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_FIX, per);		
	CHAR_talkToCli( charaindex, -1, "����������",  CHAR_COLORRED);
	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_NUM, (int)time(NULL) + (60 * 3) );

	sprintf( msg, "%d", img1);
	lssproto_BATTLESKILL_send ( getfdFromCharaIndex(charaindex), msg ) ; 

	return ret;
}

// ���˼���-�ر�ս��
int PROFESSION_escape( int charaindex, int toNo, int array, char *data, int skill_level )
{
	char *pszP = NULL, *pszOption = "\0", msg[20];
	int per=0, rate=0, img1, ret =1;
	skill_level /= 10;

	if( CHAR_getWorkInt( charaindex, CHAR_ENCOUNT_NUM) >= (int)time(NULL) ){
		ret = -1;
	}

	// ���ܵĲ���
	pszOption = PROFESSION_SKILL_getChar( array, PROFESSION_SKILL_OPTION);

	// ���ܵȼ� rate
	if( (pszP = strstr( pszOption, "��%" ) ) != NULL )	sscanf( pszP+3, "%d", &rate );
	per = skill_level * rate;

	// ׼���ڵĶ���
	img1 = PROFESSION_SKILL_getInt( array, PROFESSION_SKILL_IMG_1 );

	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_FIX, (-1) * per);
	CHAR_talkToCli( charaindex, -1, "�������½�",  CHAR_COLORGREEN);

	CHAR_setWorkInt( charaindex, CHAR_ENCOUNT_NUM, (int)time(NULL) + (60 * 3) );

	sprintf( msg, "%d", img1);
	lssproto_BATTLESKILL_send ( getfdFromCharaIndex(charaindex), msg ) ; 

	return ret;
}



// ��ͬ����-��������
int PROFESSION_full_mp( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}
// ��ͬ����-��������
int PROFESSION_strong_back( int charaindex, int toNo, int array, char *data, int skill_level )
{
	char *pszP = NULL, *pszOption = "\0";
	int rate=0;
	// ���ܵĲ���
	pszOption = PROFESSION_SKILL_getChar( array, PROFESSION_SKILL_OPTION);
	if( (pszP = strstr( pszOption, "��%" ) ) != NULL )	sscanf( pszP+3, "%d", &rate );
	int pile = CHAR_getWorkInt( charaindex, CHAR_WORKATTACHPILE);
	pile = pile * rate;
	CHAR_setWorkInt( charaindex, CHAR_WORKATTACHPILE,pile);
	return TRUE;
}
// ��ͬ����-����ǿ��
int PROFESSION_strengthen( int charaindex, int toNo, int array, char *data, int skill_level )
{
	return TRUE;
}

#endif

