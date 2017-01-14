#include "version.h"

static Char    player=
{
    FALSE,      /*  use     どうでもいい*/
    {
		0,		/*  CHAR_DATAPLACENUMBER */
        0,      /*  CHAR_BASEIMAGENUMBER */
        0,      /*  CHAR_BASEBASEIMAGENUMBER */
		0,		/*  CHAR_FACEIMAGENUMBER */
        0,      /*  CHAR_FLOOR  */
        0,      /*  CHAR_X */
        0,      /*  CHAR_Y  */
        5,      /*  CHAR_DIR 12時を0に時計周りに */
        1,      /*  CHAR_LV  */
        0,      /*  CHAR_GOLD    */
        1,      /*  CHAR_HP  */
        0,      /*  CHAR_MP  */

        0,      /*  CHAR_MAXMP   */

    	0,		/*  CHAR_VITAL	*/
        0,      /*  CHAR_STR */
        0,      /*  CHAR_TOUGH */
    	0,		/*  CHAR_DEX	*/

    	0,		/*  CHAR_CHARM	*/
    	0,		/*  CHAR_LUCK	*/
		0,		/* 地属性 */
		0,		/* 水属性 */
		0,		/* 火属性 */
		0,		/* 風属性 */


		0,		/* CHAR_SLOT*/
		0,		/* CHAR_CRITIAL */
		0,		/* CHAR_COUNTER */
		0,		/* CHAR_RARE */
        0,      /*  CHAR_RADARSTRLENGTH */
        0,      /*  CHAR_CHATVOLUME */
        MAKE2VALUE(100,20),  /*  CHAR_MERCHANTLEVEL  */
        0,      /*  CHAR_HEALERLEVEL    */
        0,      /*  CHAR_DETERMINEITEM  */

        -1,     /*  CHAR_INDEXOFEQTITLE  */


        0,      /*  CHAR_POISON  */
        0,      /*  CHAR_PARALYSIS  */
        0,      /*  CHAR_SILENCE    */
        0,      /*  CHAR_STONE  */
        0,      /*  CHAR_DARKNESS   */
        0,      /*  CHAR_CONFUSION  */

        0,      /*  CHAR_LOGINCOUNT */
        0,      /*  CHAR_DEADCOUNT */
        0,      /*  CHAR_WALKCOUNT */
        0,      /*  CHAR_TALKCOUNT */

    	0,		/*  CHAR_DAMAGECOUNT */
    	0,		/*  CHAR_GETPETCOUNT */
    	0,		/*  CHAR_KILLPETCOUNT */
    	0,		/*  CHAR_DEADPETCOUNT */
    	0,		/*  CHAR_SENDMAILCOUNT */
		0,		/*  CHAR_MERGEITEMCOUNT */

		0,		/*  CHAR_DUELBATTLECOUNT */
		0,		/*  CHAR_DUELWINCOUNT */
		0,		/*  CHAR_DUELLOSECOUNT */
		0,		/*  CHAR_DUELSTWINCOUNT */
		0,		/*  CHAR_DUELMAXSTWINCOUNT */

        CHAR_TYPEPLAYER,    /*  CHAR_WHICHTYPE  */
        1000,   /*  CHAR_WALKINTERVAL   */
        1000,   /*  CHAR_LOOPINTERVAL   */
#ifdef _NEWOPEN_MAXEXP
		0,	// CHAR_OLDEXP,
#endif
		0,	// CHAR_EXP,
        -1,     /*  CHAR_LASTTALKELDER*/
        0,      /*  CHAR_SKILLUPPOINT    */
    	0,		/*  CHAR_LEVELUPPOINT */

        0,      /*  CHAR_IMAGETYPE    */
		CHAR_COLORWHITE,	/* CHAR_NAMECOLOR */
		CHAR_COLORWHITE,	/* CHAR_POPUPNAMECOLOR */
		0,		/*  CHAR_LASTTIMESETLUCK */
		100,	/*  CHAR_DUELPOINT */
		0,	/*  CHAR_EVENT */
		0,	/*  CHAR_EVENT2 */
		0,	/*  CHAR_EVENT3 */
#ifdef _NEWEVENT
		0,	/*  CHAR_EVENT4 */
		0,	/*  CHAR_EVENT5 */
		0,	/*  CHAR_EVENT6 */
#endif
		0,	/*  CHAR_NOWEVENT */
		0,	/*  CHAR_NOWEVENT2 */
		0,	/*  CHAR_NOWEVENT3 */
#ifdef _NEWEVENT
		0,	/*  CHAR_NOWEVENT4 */
		0,	/*  CHAR_NOWEVENT5 */
		0,	/*  CHAR_NOWEVENT6 */
#endif
		0,  /* CHAR_TRANSMIGRATION */
		0,  /* CHAR_TRANSEQUATION */

        0,      /*  CHAR_INITDATA    */

//#ifdef _PETSKILL_BECOMEPIG
//        -1,
//#endif

		0,	//CHAR_SILENT,				/* char shutup time */    
		0,	//    CHAR_FMINDEX,				// 產壁 index
		0,	//    CHAR_FMLEADERFLAG,			
								/* 產壁Θ贺摸
    							 * FMMEMBER_NONE	:⊿Τヴ產壁
    							 * FMMEMBER_APPLY	:ビ叫產壁い
    							 * FMMEMBER_LEADER	:壁
    							 * FMMEMBER_MEMBER	:Θ
    							 * FMMEMBER_ELDER	:ρ    
    							 * FMMEMBER_INVITE	:步			// 既ぃノ
    							 * FMMEMBER_BAILEE	:癩叭			// 既ぃノ
    							 * FMMEMBER_VICELEADER  :捌壁		// 既ぃノ
    							*/
		0,	//    CHAR_FMSPRITE,		// 產壁臔弘艶

		0,	//    CHAR_BANKGOLD,
		0,	//    CHAR_RIDEPET,
		0,	//    CHAR_LEARNRIDE,
#ifdef _NEW_RIDEPETS
		0,	//	CHAR_LOWRIDEPETS,
#endif
 		0,	//   CHAR_LIMITLEVEL,
#ifdef _PET_FUSION
		0,	//	CHAR_FUSIONCODE,		//贺絪絏
		0,	//	CHAR_FUSIONINDEX,		//瑰て胐絪腹
		0,	//	CHAR_FUSIONRAISE,		//笼緄Ω计
		0,	//	CHAR_FUSIONBEIT,		//胐矹篨夹
		0,	//	CHAR_FUSIONTIMELIMIT,	//笼緄丁
#endif
#ifdef _PERSONAL_FAME	// Arminius 8.30: 產壁羘辨
		0,	//    CHAR_FAME,
#endif

#ifdef _NEWSAVE
		0,	//    CHAR_SAVEINDEXNUMBER,	/*  SaveFile .0.char or .1.char */
#endif
 
#ifdef _ATTACK_MAGIC
		0,	//		CHAR_EARTH_EXP,						// 產臸猭剪絤
		0,	//		CHAR_WATER_EXP,						// 產臸猭剪絤
		0,	//		CHAR_FIRE_EXP,						// 產臸猭剪絤
		0,	//		CHAR_WIND_EXP,						// 產臸猭剪絤
		0,	//		CHAR_EARTH_RESIST,					// 產臸猭к┦
		0,	//		CHAR_WATER_RESIST,					// 產臸猭к┦
		0,	//		CHAR_FIRE_RESIST,					// 產臸猭к┦
		0,	//		CHAR_WIND_RESIST,					// 產臸猭к┦
		0,	//		CHAR_EARTH_ATTMAGIC_EXP,			// 產臸猭剪絤竒喷
		0,	//		CHAR_WATER_ATTMAGIC_EXP,			// 產臸猭剪絤竒喷
		0,	//		CHAR_FIRE_ATTMAGIC_EXP,				// 產臸猭剪絤竒喷
		0,	//		CHAR_WIND_ATTMAGIC_EXP,				// 產臸猭剪絤竒喷
		0,	//	 	CHAR_EARTH_DEFMAGIC_EXP,			// 產臸猭к┦竒喷
		0,	//		CHAR_WATER_DEFMAGIC_EXP,			// 產臸猭к┦竒喷
		0,	//		CHAR_FIRE_DEFMAGIC_EXP,				// 產臸猭к┦竒喷
		0,	//		CHAR_WIND_DEFMAGIC_EXP,				// 產臸猭к┦竒喷
#endif


#ifdef _GAMBLE_BANK
		0,	//		CHAR_PERSONAGOLD,	//戒初蝗︽
#endif
#ifdef _DROPSTAKENEW
		0,	//		CHAR_GAMBLENUM,		//戒初縩だ
#endif
#ifdef _ADD_ACTION          //npc笆
		0,	//		CHAR_ACTIONSTYLE,
#endif
#ifdef _PET_EVOLUTION
		0,	//		CHAR_EVOLUTIONBASEVTL,
		0,	//		CHAR_EVOLUTIONBASESTR,	
		0,	//		CHAR_EVOLUTIONBASETGH,	
		0,	//		CHAR_EVOLUTIONBASEDEX,	
#endif

#ifdef _FM_JOINLIMIT
		0,	//		CHAR_FMTIMELIMIT,
#endif

#ifdef _CHAR_PROFESSION			// WON ADD 戮穨
		0,	//		PROFESSION_CLASS,			// 戮穨
		0,	//		PROFESSION_LEVEL,			// 戮穨单
//		0,	//		PROFESSION_EXP,				// 戮穨竒喷
		0,	//		PROFESSION_SKILL_POINT,		// м翴计
		0,	//		ATTACHPILE,					// 糤帮舼
		0,	//		PROFESSION_FIRE_P,			// 剪絤
		0,	//		PROFESSION_ICE_P,			// 剪絤		
		0,	//		PROFESSION_THUNDER_P,		// 筽剪絤
		0,	//		PROFESSION_FIRE_R,			// к┦
		0,	//		PROFESSION_ICE_R,			// к┦	
		0,	//		PROFESSION_THUNDER_R,		// 筽к┦
#endif
#ifdef _ALLDOMAN // (ぃ秨) Syu ADD 逼︽篯NPC
		0,	//		CHAR_HEROFLOOR, 
#endif
#ifdef _PETSKILL_BECOMEPIG
		-1,	//		CHAR_BECOMEPIG,
		100250, //CHAR_BECOMEPIG_BBI
#endif
		0,	//		CHAR_LASTLEAVETIME, // Robin add 程瞒絬丁

#ifdef _NEW_MANOR_LAW
		0,	//		CHAR_MOMENTUM,
#endif

#ifdef _ITEM_ADDEXP2
		0,	//		CHAR_ADDEXPPOWER,
		0,	//		CHAR_ADDEXPTIME,
#endif

#ifdef _VIP_BATTLE_EXP
	  0,	// CHAR_NVIPEXP,
	  0,	// CHAR_NVIPTIME,
#endif

#ifdef _ANGEL_SUMMON
		0,	//		CHAR_HEROCNT, // ЧΘ玦ヴ叭Ω计
#endif

#ifdef _TEACHER_SYSTEM
	  0,	// CHAR_TEACHER_FAME,	// 导师领导声望
#endif

#ifdef _GM_ITEM
	  0, // 玩家GM命令次数
#endif
#ifdef _VIP_SERVER
	  0, // 会员点数
#endif
#ifdef _VIP_RIDE
	  0, // 会员骑黑鸡
#endif
#ifdef _CAMEO_MONEY
	  0,
#endif
#ifdef _MISSION_TRAIN
	  0,
	  0,
#endif
#ifdef _PET_EVOLVE
	  0,
#endif
#ifdef _FLOOR_DIFI_FIELD
	  0,
#endif
#ifdef _MISSION_TIME
	  0,
#endif
#ifdef _PLAYER_TITLE
	  0,
	  0,
#endif
#ifdef _COWRIE
	  0,
#endif
#ifdef _ONLINE_TIME
	  0,
#endif
  0, //CHAR_LUASAVE_WORKINT_1
	0, //CHAR_LUASAVE_WORKINT_2
	0, //CHAR_LUASAVE_WORKINT_3
	0, //CHAR_LUASAVE_INT_1
	0, //CHAR_LUASAVE_INT_2
  0, //CHAR_LUASAVE_INT_3
#ifdef _CAX_ESC_REPORT
	0, //CHAR_REPORT     每日签到
#endif
#ifdef _CAX_Mercenary
	0, //CHAR_MERCENARY  佣兵字段
#endif
    },

    {
        {""},     /*  CHAR_NAME   */
        {""},     /*  CHAR_OWNTITLE   */
        {""},           /*  CHAR_ARGUMENT */
        {""},       /*  CHAR_OWNERCDKEY   */
        {""},       /*  CHAR_OWNERCHARANAME   */
    },
    {
        SETFLG(1,1,1,1,1,1,0,0 ),
        SETFLG(0,0,0,0,0,0,0,1 ),
    }
};

LevelUpPattern lvplayer00={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer10={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer20={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer30={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer01={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer11={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer21={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer31={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer02={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer12={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer22={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer32={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer03={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer13={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer23={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer33={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer04={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer14={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer24={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer34={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer05={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer15={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer25={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer35={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer06={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer16={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer26={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer36={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer07={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer17={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer27={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer37={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer08={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer18={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer28={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer38={ {{100,10},{200,10},{50,8}},9,11,10};

LevelUpPattern lvplayer09={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer19={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer29={ {{100,10},{200,10},{50,8}},9,11,10};
LevelUpPattern lvplayer39={ {{100,10},{200,10},{50,8}},9,11,10};


