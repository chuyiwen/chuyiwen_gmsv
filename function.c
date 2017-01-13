#include "version.h"
#include <string.h>
#include "common.h"
#include "util.h"
#include "char_event.h"
#include "char.h"
#include "item_event.h"
#include "magic.h"
#include "pet_event.h"
#include "npc_townpeople.h"
#include "npc_Dengon.h"
#include "npc_door.h"
#include "npc_healer.h"
#include "npc_oldman.h"
#include "npc_warp.h"
#include "npc_storyteller.h"
#include "npc_msg.h"
#include "npc_npcenemy.h"
#include "npc_action.h"
#include "npc_windowman.h"
#include "npc_savepoint.h"
#include "npc_windowhealer.h"
#include "npc_itemshop.h"
#ifdef _VIGOR_SYS
#include "npc_vigorshop.h"
#endif
#include "npc_sysinfo.h"
#include "npc_duelranking.h"
#include "npc_petskillshop.h"
#ifdef	_PETSKILL_DEL_SHOP
#include "npc_petskilldelshop.h"
#endif
#include "npc_petshop.h"
#include "npc_signboard.h"
#include "npc_warpman.h"
#include "npc_exchangeman.h"
#include "petmail.h"
#include "npc_timeman.h"
#include "npc_bodylan.h"
#include "npc_mic.h"
#include "npc_luckyman.h"
#include "npc_bus.h"
#include "npc_charm.h"
#include "npc_poolitemshop.h"
#include "npc_quiz.h"
#include "npc_checkman.h"
#include "npc_janken.h"
#include "npc_transmigration.h"
#include "battle_event.h"
#include "enemy.h"
// Robin 0517
#include "npc_familyman.h"
#include "npc_bankman.h"
// add code by shan
#include "npc_fmdengon.h"
#include "npc_fmhealer.h"
#include "npc_petmaker.h"
#ifdef	_FMRANK_POINT
#include "npc_fmrank.h"
#endif
// CoolFish: Family 2001/6/4
#include "npc_fmwarpman.h"
#include "npc_fmpkman.h"
#include "npc_fmpkcallman.h"

// Arminius 7.7 Airplane
#include "npc_airplane.h"

// Arminius 7.13 Scheduleman
#include "npc_scheduleman.h"

// Arminius 7.24 manor scheduleman
#include "npc_manorsman.h"

// Robin 0725
#include "npc_riderman.h"
#include "npc_fmletter.h"

#ifdef _GAMBLE_BANK
#include "npc_gamblebank.h"
#endif
#ifdef _NEW_WARPMAN
#include "npc_newnpcman.h"
#endif

#ifdef _GAMBLE_ROULETTE 
#include "npc_gambleroulette.h"
#include "npc_gamblemaster.h"
#endif

#ifdef _TRANSER_MAN
#include "npc_transerman.h"
#endif

#ifdef _VIP_SHOP
#include "npc_vipshop.h"
//#include "npc_vippoint.h"
#include "npc_newvipshop.h"
#endif
#ifdef	_RMB_SYSTEM
#include "npc_rmbshop.h"
#endif
#ifdef _STU_SYS
#include "npc_stushop.h"
#endif
#ifdef	_NPC_ITEMUP
#include "npc_itemup.h"
#endif
#ifdef _AUTO_PK
#include "npc_autopk.h"
#endif
#ifdef _CFREE_petskill
#include "npc_freepetskillshop.h"
#endif

#ifdef _PETRACE
#include "npc_petracemaster.h"
#include "npc_petracepet.h"
#endif

#ifdef _ITEM_NPCCHANGE
#include "npc_itemchange.h"
#endif

#ifdef _NPC_MAKEPAIR
#include "npc_makepair.h"
#endif

#ifdef _NPC_FUSION
#include "npc_petfusion.h"
#endif

#ifdef _ALLDOMAN // (不可开) Syu ADD 排行榜NPC
#include "npc_alldoman.h"
#endif

#ifdef _NPC_WELFARE
#include "npc_welfare.h"
#endif
#ifdef _JZ_NEWSCRIPT_LUA
#include "npc_lua_interface.h"
#endif
#ifdef	_FM_NPC_LOOK_WAR
#include "npc_fmlookwar.h"
#endif
#ifdef	_FM_NPC_LOOK_WAR1
#include "npc_fmlookwar1.h"
#endif
//#define DEBUG
#include "npc_itemvippointshop.h"
typedef struct tagCorrespondStringAndFunctionTable
{
    STRING32    functionName;
    void*       functionPointer;
    int         hashcode;
}CorrespondStringAndFunctionTable;

static CorrespondStringAndFunctionTable
correspondStringAndFunctionTable[]=
{
    { {"core_PreWalk"},  			CHAR_allprewalk,   		0 },
    { {"core_PostWalk"},  			CHAR_allpostwalk,  		0 },
    { {"core_Loop"},				CHAR_loopFunc, 			0 },
    { {"core_Dying"},				CHAR_die, 				0 },
    { {"core_PlayerWatch"},   		CHAR_playerWatchfunc, 	0 },
    { {"core_PlayerTalked"},		CHAR_playerTalkedfunc, 	0 },


    { {"MedicineInit"},   			ITEM_MedicineInit, 		0 },
    { {"MedicineUsed"},   			ITEM_MedicineUsed, 		0 },
    { {"SandClockDetach"},			ITEM_SandClockDetach, 	0 },
    { {"addTitleAttach"}, 			ITEM_addTitleAttach, 	0 },
    { {"delTitleDetach"}, 			ITEM_delTitleDetach, 	0 },
    { {"ITEM_DeleteByWatched"}, 	ITEM_DeleteByWatched, 	0 },
    { {"ITEM_DeleteTimeWatched"}, 	ITEM_DeleteTimeWatched, 0 },
    { {"ITEM_useEffectTohelos"}, 	ITEM_useEffectTohelos, 	0 },

//    { {"ITEM_useHpRecovery"}, 	ITEM_useHpRecovery, 	0 },
    { {"ITEM_useRecovery"}, 		ITEM_useRecovery, 		0 },
#ifdef _ITEM_MAGICRECOVERY
  	{ {"ITEM_useMRecovery"},	ITEM_useMRecovery,	0 },
#endif
#ifdef _ITEM_USEMAGIC
  	{ {"ITEM_useMagic"},	ITEM_useMagic,	0 },
#endif
    { {"ITEM_useStatusChange"}, 	ITEM_useStatusChange, 	0 },
    { {"ITEM_useStatusRecovery"}, 	ITEM_useStatusRecovery, 0 },
    { {"ITEM_useMagicDef"}, 		ITEM_useMagicDef, 		0 },
    { {"ITEM_useParamChange"}, 		ITEM_useParamChange, 	0 },
    { {"ITEM_useFieldChange"}, 		ITEM_useFieldChange, 	0 },
    { {"ITEM_useAttReverse"}, 		ITEM_useAttReverse, 	0 },
    { {"ITEM_useRessurect"}, 		ITEM_useRessurect, 		0 },
    { {"ITEM_useMic"}, 				ITEM_useMic, 			0 },
    { {"ITEM_dropMic"}, 			ITEM_dropMic, 			0 },
    { {"ITEM_useCaptureUp"}, 		ITEM_useCaptureUp, 		0 },
    { {"ITEM_useRenameItem"}, 		ITEM_useRenameItem,		0 },
    { {"ITEM_pickupDice"}, 			ITEM_pickupDice,		0 },
    { {"ITEM_dropDice"}, 			ITEM_dropDice,			0 },
    { {"ITEM_initLottery"}, 		ITEM_initLottery,			0 },
    { {"ITEM_useLottery"}, 			ITEM_useLottery,			0 },
    { {"ITEM_useWarp"},	    ITEM_useWarp, 0 },
    { {"ITEM_petFollow"},   ITEM_petFollow, 0 },
    { {"ITEM_useSkup"},     ITEM_useSkup, 0 }, // Nuke 0624: Hero's bless
    { {"ITEM_useNoenemy"},  ITEM_useNoenemy, 0 }, // Nuke 0626: Dragon's help
    { {"ITEM_equipNoenemy"},ITEM_equipNoenemy, 0 }, // Arminius 7.2 Ra's amulet
    { {"ITEM_remNoenemy"},  ITEM_remNoenemy, 0 },   // Arminius 7.2 Ra's amulet
    { {"ITEM_useEncounter"}, ITEM_useEncounter, 0},     // Arminius 7.31 cursed stone

  	{ {"ITEM_AddPRSkillPoint"}, ITEM_AddPRSkillPoint, 0},
  	{ {"ITEM_AddPRSkillPercent"}, ITEM_AddPRSkillPercent, 0},

#ifdef _ITEM_METAMO
	  { {"ITEM_metamo"},   ITEM_metamo, 0 },
  	{ {"ITEM_ColorMetamo"},   ITEM_ColorMetamo, 0 },
  	{ {"ITEM_CharaMetamo"},   ITEM_CharaMetamo, 0 },
  	{ {"ITEM_SexMetamo"},   ITEM_SexMetamo, 0 },	
#endif

#ifdef _USEWARP_FORNUM
  	{ {"ITEM_useWarpForNum"},	    ITEM_useWarpForNum, 0 },
#endif

#ifdef _IMPRECATE_ITEM
	  { {"ITEM_useImprecate"},	    ITEM_useImprecate, 0 },
#endif
#ifdef _ITEM_FIRECRACKER	//Terry add 2001/12/21
		{ {"ITEM_firecracker"}, ITEM_firecracker, 0 },
#endif
#ifdef _ITEM_CRACKER	//vincent 拉炮
		{ {"ITEM_Cracker"}, ITEM_Cracker, 0 },
#endif
#ifdef _ITEM_ADDEXP	//vincent 经验提升
		{ {"ITEM_Addexp"}, ITEM_Addexp, 0 },
#endif
#ifdef _ITEM_REFRESH //vincent 解除异常状态道具
		{ {"ITEM_Refresh"}, ITEM_Refresh, 0 },
#endif

  	{ {"ITEM_WearEquip"}, ITEM_WearEquip, 0 },
  	{ {"ITEM_ReWearEquip"}, ITEM_ReWearEquip, 0 },

#ifdef _ITEM_CONSTITUTION
  	{ {"ITEM_Constitution"}, ITEM_Constitution, 0 },
#endif

#ifdef _Item_ReLifeAct
  	{ {"ITEM_DIErelife"}, ITEM_DIErelife, 0 },
#endif

#ifdef _ITEM_ORNAMENTS
  	{ {"ITEM_PutOrnaments"}, ITEM_PutOrnaments, 0},
#endif
#ifdef _CHIKULA_STONE
  	{ {"ITEM_ChikulaStone"}, ITEM_ChikulaStone, 0},
#endif

#ifdef _THROWITEM_ITEMS
  	{ {"ITEM_ThrowItemBox"}, ITEM_ThrowItemBox, 0},
#endif

#ifdef _ITEM_WATERWORDSTATUS
  	{ {"ITEM_WaterWordStatus"}, ITEM_WaterWordStatus, 0},
#endif

#ifdef _ITEM_LOVERPARTY
  	{ {"ITEM_LoverSelectUser"}, ITEM_LoverSelectUser, 0},
#endif

#ifdef _Item_MoonAct
	  { {"ITEM_randEnemyEquipOne"}, ITEM_randEnemyEquipOne, 0 },
  
  	{ {"ITEM_randEnemyEquip"}, ITEM_randEnemyEquip, 0 },
  	{ {"ITEM_RerandEnemyEquip"}, ITEM_RerandEnemyEquip, 0},
#endif

#ifdef _SUIT_ITEM
  	{ {"ITEM_suitEquip"},	ITEM_suitEquip,		0 },
  	{ {"ITEM_ResuitEquip"},	ITEM_ResuitEquip,	0 },
#endif

#ifdef _Item_DeathAct
  	{ {"ITEM_useDeathcounter"}, ITEM_UseDeathCounter, 0 },
#endif
#ifdef _FEV_ADD_NEW_ITEM			// FEV ADD 增加复活守精
  	{ {"ITEM_ResAndDef"} ,   ITEM_ResAndDef,        0 },
#endif

#ifdef _CHRISTMAS_REDSOCKS
  	{ {"ITEM_useMaxRedSocks"}, ITEM_useMaxRedSocks, 0 },
#endif

#ifdef _CHRISTMAS_REDSOCKS_NEW
  	{ {"ITEM_useMaxRedSocksNew"}, ITEM_useMaxRedSocksNew, 0 },
#endif

#ifdef _PETSKILL_CANNEDFOOD
	  { {"ITEM_useSkillCanned"}, ITEM_useSkillCanned, 0},
#endif

#ifdef _NEW_RIDEPETS
  	{ {"ITEM_useLearnRideCode"}, ITEM_useLearnRideCode, 0 },
#endif

#ifdef _EQUIT_DEFMAGIC
  	{ {"ITEM_MagicEquitWear"}, ITEM_MagicEquitWear, 0 },
  	{ {"ITEM_MagicEquitReWear"}, ITEM_MagicEquitReWear, 0 },
#endif
#ifdef _EQUIT_RESIST
	  { {"ITEM_MagicResist"}, ITEM_MagicResist, 0 },
  	{ {"ITEM_MagicReResist"}, ITEM_MagicReResist, 0 },
#endif

#ifdef _MAGIC_RESIST_EQUIT			// WON ADD 职业抗性装备
  	{ {"ITEM_P_MagicEquitWear"},		ITEM_P_MagicEquitWear, 0 },
	  { {"ITEM_P_MagicEquitReWear"},	ITEM_P_MagicEquitReWear, 0 },
#endif

#ifdef _ANGEL_SUMMON	
  	{ {"ITEM_AngelToken"}, ITEM_AngelToken, 0 },
  	{ {"ITEM_HeroToken"}, ITEM_HeroToken, 0 },
#endif
#ifdef _HALLOWEEN_EFFECT
  	{ {"ITEM_MapEffect"}, ITEM_MapEffect, 0 },
#endif

   	{ {"ITEM_changePetOwner"}, 		ITEM_changePetOwner, 		0 },
   	
   	{ {"ITEM_useDelRenameItem"}, 		ITEM_useDelRenameItem, 		0 },
   	
   	{ {"ITEM_DANMM"}, 		ITEM_DANMM, 		0 },

  	{ {"core_PetWatch"},   			PET_Watchfunc, 			0 },
    { {"PETMAIL_Loop"},   			PETMAIL_Loopfunc, 		0 },
#ifdef _USER_CHARLOOPS
  	{ {"CHAR_BattleStayLoop"},		CHAR_BattleStayLoop,	0 },
  	{ {"PET_CheckIncubateLoop"},	PET_CheckIncubate,		0 },
#endif

#ifdef _PETSKILL_PROPERTY
  	{ {"PET_PetskillPropertyEvent"},	PET_PetskillPropertyEvent,		0 },
#endif
    { {"core_PetTalk"}, PET_Talkfunc, 0},	// Arminius 8.14 pet talk


	  /* warp */
    { {"WarpInit"},					NPC_WarpInit, 			0 },
    { {"WarpPostOver"},				NPC_WarpPostOver, 		0 },
    { {"WarpWatch"},				NPC_WarpWatch,			0 },

     /* Dengon */
    { {"DengonInit"}, 				NPC_DengonInit,			0 },
    { {"DengonWindowTalked"}, 		NPC_DengonWindowTalked, 0 },
    { {"DengonLooked"}, 			NPC_DengonLooked, 		0 },
    
     /* FmDengon add code by shan */
    { {"FmDengonInit"}, 			NPC_FmDengonInit,         0 },
    { {"FmDengonWindowTalked"}, 		NPC_FmDengonWindowTalked, 0 },
    { {"FmDengonLooked"}, 			NPC_FmDengonLooked, 	  0 },

    /* Healer */
    { {"HealerInit"} ,				NPC_HealerInit,			0 },
    { {"HealerTalked"} ,			NPC_HealerTalked , 		0 },
    
    /* FMHealer add code by shan */
    { {"FmHealerInit"} ,			NPC_FmHealerInit,	0 },
    { {"FmHealerTalked"} ,			NPC_FmHealerTalked , 	0 },
    
    /* petmaker add code by shan */
    { {"PetMakerInit"} ,			NPC_PetMakerInit,	0 },
    { {"PetMakerTalked"} ,			NPC_PetMakerTalked , 	0 },    
    
    /* TownPeople */
    { {"TownPeopleTalked"},			NPC_TownPeopleTalked, 	0 },
    { {"TownPeopleInit"},			NPC_TownPeopleInit, 	0 },

    /* TownPeople */
    { {"MsgLooked"},				NPC_MsgLooked , 		0 },
    { {"MsgInit"},					NPC_MsgInit, 			0 },

    /* Oldman */
    { {"OldmanInit"} , 				NPC_OldmanInit , 		0 },
    { {"OldmanTalked"} ,			NPC_OldmanTalked ,		0 },

    /* SavePOint */
    { {"SavePointInit"} ,			NPC_SavePointInit , 	0 },
    { {"SavePointTalked"} ,			NPC_SavePointTalked , 	0 },
    { {"SavePointWindowTalked"},	NPC_SavePointWindowTalked, 0 },

    /* StoryTeller */
    { {"StoryTellerInit"} , 		NPC_StoryTellerInit , 	0 },
    { {"StoryTellerTalked"} , 		NPC_StoryTellerTalked , 0 },

    /* NPCEnemy */
    { {"NPCEnemyInit"} ,			NPC_NPCEnemyInit ,		0 },
    { {"NPCEnemyTalked"} ,			NPC_NPCEnemyTalked ,	0 },
    { {"NPCEnemyWatch"} ,			NPC_NPCEnemyWatch ,		0 },
	  { {"NPCEnemyLoop"},				NPC_NPCEnemyLoop,		0 },
    { {"NPCEnemyWindowTalked"}, 	NPC_NPCEnemyWindowTalked, 0 },

    /* 失弁扑亦件楝 */
    { {"ActionInit"} ,				NPC_ActionInit ,		0 },
    { {"ActionTalked"} ,			NPC_ActionTalked ,		0 },
    { {"ActionWatch"} ,				NPC_ActionWatch ,		0 },

    /* 它奴件玉它楝 */
    { {"WindowmanInit"} , 			NPC_WindowmanInit ,		0 },
    { {"WindowmanTalked"} , 		NPC_WindowmanTalked ,	0 },
    { {"WindowmanLooked"}, 			NPC_WindowmanLooked,	0 },
    { {"WindowmanWindowTalked"}, 	NPC_WindowmanWindowTalked, 0 },

    /* 它奴件玉它甲□仿□ */
    { {"WindowHealerInit"} , 		NPC_WindowHealerInit ,	0 },
    { {"WindowHealerTalked"} , 		NPC_WindowHealerTalked,	0 },
    { {"WindowHealerLooked"} , 		NPC_WindowHealerLooked,	0 },
    { {"WindowHealerWindowTalked"}, NPC_WindowHealerWindowTalked, 0 },

    { {"ItemShopInit"} ,			NPC_ItemShopInit ,		0 },
    { {"ItemShopTalked"} ,			NPC_ItemShopTalked ,	0 },
    { {"ItemShopWindowTalked"},		NPC_ItemShopWindowTalked, 0 },
    
    { {"ItemVippointShopInit"} ,			NPC_ItemVippointShopInit ,		0 },
    { {"ItemVippointShopTalked"} ,			NPC_ItemVippointShopTalked ,	0 },
    { {"ItemVippointShopWindowTalked"},		NPC_ItemVippointShopWindowTalked, 0 },

    /* Sysinfo */
    { {"SysinfoInit"},				NPC_SysinfoInit, 		0 },
    { {"SysinfoLoop"},				NPC_SysinfoLoop,		0 },
    { {"SysinfoTalked"} ,			NPC_SysinfoTalked ,		0 },

    { {"DuelrankingInit"} ,			NPC_DuelrankingInit ,	0 },
    { {"DuelrankingLooked"},		NPC_DuelrankingLooked,	0 },
    { {"DuelrankingWindowTalked"}, 	NPC_DuelrankingWindowTalked, 0 },
    
    { {"PetSkillShopInit"} ,		NPC_PetSkillShopInit ,	0 },
    { {"PetSkillShopTalked"} ,		NPC_PetSkillShopTalked,	0 },
    { {"PetSkillShopLooked"} ,		NPC_PetSkillShopLooked,	0 },
    { {"PetSkillShopWindowTalked"},	NPC_PetSkillShopWindowTalked, 0 },
#ifdef	_PETSKILL_DEL_SHOP    
    { {"PetSkillDelShopInit"} ,		NPC_PetSkillDelShopInit ,	0 },
    { {"PetSkillDelShopTalked"} ,		NPC_PetSkillDelShopTalked,	0 },
    { {"PetSkillDelShopLooked"} ,		NPC_PetSkillDelShopLooked,	0 },
    { {"PetSkillDelShopWindowTalked"},	NPC_PetSkillDelShopWindowTalked, 0 },
#endif
    { {"PetShopInit"} ,				NPC_PetShopInit, 		0 },
    { {"PetShopTalked"} ,			NPC_PetShopTalked,		0 },
    { {"PetShopLooked"} ,			NPC_PetShopLooked,		0 },
    { {"PetShopWindowTalked"},		NPC_PetShopWindowTalked,0 },

    { {"SignBoardInit"} ,			NPC_SignBoardInit,		0 },
    { {"SignBoardLooked"} ,			NPC_SignBoardLooked,	0 },
    { {"SignBoardWindowTalked"},	NPC_SignBoardWindowTalked, 0 },

    /*伐□皿穴件 */
    { {"WarpManInit"},				NPC_WarpManInit,		0 },
    { {"WarpManTalked"},			NPC_WarpManTalked,		0 },
  	{ {"WarpManWatch"},				NPC_WarpManWatch,		0 },
	  { {"WarpManLoop"} ,				NPC_WarpManLoop,		0 },
    { {"WarpManWindowTalked"},		NPC_WarpManWindowTalked,0 },


    { {"ExChangeManInit"},				NPC_ExChangeManInit,		0 },
    { {"ExChangeManTalked"},			NPC_ExChangeManTalked,		0 },
    { {"ExChangeManWindowTalked"},		NPC_ExChangeManWindowTalked,0 },

    /*正奶丞穴件 */
    { {"TimeManInit"},				NPC_TimeManInit,		0 },
    { {"TimeManTalked"},			NPC_TimeManTalked,		0 },
    { {"TimeManWatch"} ,			NPC_TimeManWatch ,		0 },

    /* 示犯奴仿件必□斥 */
    { {"BodyLanInit"},				NPC_BodyLanInit,		0 },
    { {"BodyLanTalked"},			NPC_BodyLanTalked,		0 },
    { {"BodyLanWatch"} ,			NPC_BodyLanWatch ,		0 },
    { {"BodyLanWindowTalked"} ,		NPC_BodyLanWindowTalked,0 },

    /* 穴奶弁 */
    { {"MicInit"},				NPC_MicInit,		0 },
    { {"MicTalked"},			NPC_MicTalked,		0 },

    /* 仿永平□穴件 */
    { {"LuckyManInit"} , 		NPC_LuckyManInit ,	0 },
    { {"LuckyManTalked"} , 		NPC_LuckyManTalked,	0 },
    { {"LuckyManWindowTalked"}, NPC_LuckyManWindowTalked, 0 },

    /* 楝 */
    { {"BusInit"} ,				NPC_BusInit ,		0 },
    { {"BusTalked"} ,			NPC_BusTalked ,		0 },
    { {"BusLoop"} ,				NPC_BusLoop ,		0 },

    /* 加美航空 */      // Arminius 7.7 Airplane
    { {"AirInit"} ,     NPC_AirInit ,           0 },
    { {"AirTalked"} ,   NPC_AirTalked ,         0 },
    { {"AirLoop"} ,     NPC_AirLoop ,           0 },

    { {"CharmInit"} , 		NPC_CharmInit ,	0 },
    { {"CharmTalked"} , 		NPC_CharmTalked,	0 },
    { {"CharmWindowTalked"}, NPC_CharmWindowTalked, 0 },
	
    { {"PoolItemShopInit"} ,			NPC_PoolItemShopInit ,		0 },
  	{ {"PoolItemShopLoop"} ,			NPC_PoolItemShopLoop ,		0 },
    { {"PoolItemShopTalked"} ,			NPC_PoolItemShopTalked ,	0 },
    { {"PoolItemShopWindowTalked"},		NPC_PoolItemShopWindowTalked, 0 },

    { {"QuizInit"} , 		NPC_QuizInit ,	0 },
    { {"QuizTalked"} , 		NPC_QuizTalked,	0 },
    { {"QuizWindowTalked"}, NPC_QuizWindowTalked, 0 },
	

    /* 切之匀仁引氏 */
    { {"CheckManInit"} , 		NPC_CheckManInit ,	0 },
    { {"CheckManTalked"} , 		NPC_CheckManTalked,	0 },
    { {"CheckManWindowTalked"}, NPC_CheckManWindowTalked, 0 },

    /* 元扎氏仃氏楝 */
    { {"JankenInit"} , 		NPC_JankenInit ,	0 },
    { {"JankenTalked"} , 		NPC_JankenTalked,	0 },
    { {"JankenWindowTalked"}, NPC_JankenWindowTalked, 0 },

    /* 鳖戏谛 */
    { {"TransmigrationInit"} , 		NPC_TransmigrationInit ,	0 },
    { {"TransmigrationTalked"} , 		NPC_TransmigrationTalked,	0 },
    { {"TransmigrationWindowTalked"}, NPC_TransmigrationWindowTalked, 0 },

    /* Family man */
    { {"FamilymanInit"} ,          NPC_FamilymanInit ,        0 },
    { {"FamilymanTalked"} ,                NPC_FamilymanTalked,       0 },
    { {"FamilymanLooked"},	NPC_FamilymanLooked,0 },    
    { {"FamilymanWindowTalked"}, NPC_FamilymanWindowTalked, 0 },
                
    /* CoolFish: Family WarpMan 2001/6/6 */
    { {"FMWarpManInit"},		NPC_FMWarpManInit,		0 },
    { {"FMWarpManTalked"},		NPC_FMWarpManTalked,		0 },
    { {"FMWarpManLoop"} ,		NPC_FMWarpManLoop,		0 },
    { {"FMWarpManWindowTalked"},	NPC_FMWarpManWindowTalked,	0 },

    /* CoolFish: Family PKMan 2001/7/4 */
    { {"FMPKManInit"},		NPC_FMPKManInit,		0 },
    { {"FMPKManTalked"},		NPC_FMPKManTalked,		0 },
    { {"FMPKManWindowTalked"},	NPC_FMPKManWindowTalked,	0 },

    /* CoolFish: Family PKCallMan 2001/7/13 */
    { {"FMPKCallManInit"},		NPC_FMPKCallManInit,		0 },
    { {"FMPKCallManTalked"},		NPC_FMPKCallManTalked,		0 },
    { {"FMPKCallManWindowTalked"},	NPC_FMPKCallManWindowTalked,	0 },
    
    /* Bank man */
    { {"BankmanInit"} ,          NPC_BankmanInit ,        0 },
    { {"BankmanTalked"} ,                NPC_BankmanTalked,       0 },
    { {"BankmanLooked"},	NPC_BankmanLooked,0 },
    { {"BankmanWindowTalked"}, NPC_BankmanWindowTalked, 0 },

    /* Arminius 7.13 scheduleman */
    { {"SchedulemanInit"},	NPC_SchedulemanInit,	0},
    { {"SchedulemanTalked"},	NPC_SchedulemanTalked,	0},
    { {"SchedulemanWindowTalked"},	NPC_SchedulemanWindowTalked,	0},
    { {"SchedulemanLoop"},	NPC_SchedulemanLoop,	0},

    /* Arminius 7.24 manor scheduleman */
    { {"ManorSmanInit"},	NPC_ManorSmanInit,	0},
    { {"ManorSmanTalked"},	NPC_ManorSmanTalked,	0},
    { {"ManorSmanWindowTalked"},	NPC_ManorSmanWindowTalked,	0},
    { {"ManorSmanLoop"},	NPC_ManorSmanLoop,	0},

    /* Rider man */
    { {"RidermanInit"} ,          NPC_RidermanInit ,        0 },
    { {"RidermanTalked"} ,                NPC_RidermanTalked,       0 },
    { {"RidermanLooked"},	NPC_RidermanLooked,0 },    
    { {"RidermanWindowTalked"}, NPC_RidermanWindowTalked, 0 },

    /* FmLetter man */
    { {"FmLetterInit"} ,          NPC_FmLetterInit ,        0 },
    { {"FmLetterTalked"} ,                NPC_FmLetterTalked,       0 },
    { {"FmLetterLooked"},	NPC_FmLetterLooked,0 },    
    { {"FmLetterWindowTalked"}, NPC_FmLetterWindowTalked, 0 },


#ifdef _GAMBLE_BANK	//银行
  	{ {"GambleBankInit"},			NPC_GambleBankInit, 0},
  	{ {"GambleBankLoop"},			NPC_GambleBankLoop,0},
    { {"GambleBankTalked"},			NPC_GambleBankTalked,0},
    { {"GambleBankWindowTalked"},	NPC_GambleBankWindowTalked,0},
#endif


#ifdef _PET_LIMITLEVEL
	{ {"ITEM_useOtherEditBase"}, ITEM_useOtherEditBase, 0},
#endif
#ifdef _ITEM_EDITBASES
	{ {"ITEM_useFusionEditBase"}, ITEM_useFusionEditBase, 0},
#endif
#ifdef _GAMBLE_ROULETTE //赌场轮盘
	{ {"GambleRouletteInit"},			NPC_Gamble_RouletteInit, 0},
	{ {"GambleRouletteLoop"},			NPC_Gamble_RouletteLoop, 0},
    { {"GambleRouletteTalked"},		NPC_Gamble_RouletteTalked, 0},
	{ {"GambleRouletteWindowTalked"}, NPC_Gamble_RouletteWindowTalked, 0},

	{ {"GambleMasterInit"},			NPC_Gamble_MasterInit, 0},
	{ {"GambleMasterLoop"},			NPC_Gamble_MasterLoop, 0},
    { {"GambleMasterTalked"},		NPC_Gamble_MasterTalked, 0},
	{ {"GambleMasterWindowTalked"}, NPC_Gamble_MasterWindowTalked, 0},
#endif

#ifdef _TRANSER_MAN
    { {"TranserManInit"},			NPC_TranserManInit,			0 },
    { {"TranserManTalked"},			NPC_TranserManTalked,		0 },
	  { {"TranserManLoop"} ,			NPC_TranserManLoop,			0 },
    { {"TranserManWindowTalked"},	NPC_TranserManWindowTalked,	0 },
#endif
#ifdef	_FM_NPC_LOOK_WAR
		{ {"FmLookWarManInit"},			NPC_FmLookWarManInit,			0 },
    { {"FmLookWarManTalked"},			NPC_FmLookWarManTalked,		0 },
	  { {"FmLookWarManLoop"} ,			NPC_FmLookWarManLoop,			0 },
    { {"FmLookWarManWindowTalked"},	NPC_FmLookWarManWindowTalked,	0 },
#endif
#ifdef	_FM_NPC_LOOK_WAR1
		{ {"FmLookWarMan1Init"},			NPC_FmLookWarMan1Init,			0 },
    { {"FmLookWarMan1Talked"},			NPC_FmLookWarMan1Talked,		0 },
	  { {"FmLookWarMan1Loop"} ,			NPC_FmLookWarMan1Loop,			0 },
    { {"FmLookWarMan1WindowTalked"},	NPC_FmLookWarMan1WindowTalked,	0 },
#endif
#ifdef _VIP_SHOP
    { {"VipShopInit"},			NPC_VipshopInit,			0 },
    { {"VipShopTalked"},			NPC_VipshopTalked,		0 },
		{ {"VipShopLoop"} ,			NPC_VipshopLoop,			0 },
    { {"VipShopWindowTalked"},	NPC_VipshopWindowTalked,	0 },
#endif

#ifdef _VIP_SHOP
    { {"NewVipShopInit"},			NPC_NewVipshopInit,			0 },
    { {"NewVipShopTalked"},			NPC_NewVipshopTalked,		0 },
		{ {"NewVipShopLoop"} ,			NPC_NewVipshopLoop,			0 },
    { {"NewVipShopWindowTalked"},	NPC_NewVipshopWindowTalked,	0 },
#endif

#ifdef _RMB_SYSTEM
    { {"RmbShopInit"},			NPC_RmbshopInit,			0 },
    { {"RmbShopTalked"},			NPC_RmbshopTalked,		0 },
		{ {"RmbShopLoop"} ,			NPC_RmbshopLoop,			0 },
    { {"RmbShopWindowTalked"},	NPC_RmbshopWindowTalked,	0 },
#endif

#ifdef _STU_SYS
    { {"StuShopInit"},			NPC_StushopInit,			0 },
    { {"StuShopTalked"},			NPC_StushopTalked,		0 },
		{ {"StuShopLoop"} ,			NPC_StushopLoop,			0 },
    { {"StuShopWindowTalked"},	NPC_StushopWindowTalked,	0 },
#endif

#ifdef _AUTO_PK
    { {"AutoPkInit"},			NPC_AutoPkInit,			0 },
    { {"AutoPkTalked"},			NPC_AutoPkTalked,		0 },
		{ {"AutoPkLoop"} ,			NPC_AutoPkLoop,			0 },
    { {"AutoPkWindowTalked"},	NPC_AutoPkWindowTalked,	0 },
#endif
#ifdef	_FMRANK_POINT
		{ {"FmRankInit"},			NPC_FmRankInit,			0 },
    { {"FmRankTalked"},			NPC_FmRankTalked,		0 },
		{ {"FmRankLoop"} ,			NPC_FmRankLoop,			0 },
    { {"FmRankWindowTalked"},	NPC_FmRankWindowTalked,	0 },
#endif
#ifdef _NPC_MAKEPAIR
	{ {"MakePairManInit"},			NPC_MakePairManInit,		0 },
    { {"MakePairManTalked"},		NPC_MakePairManTalked,		0 },
	{ {"MakePairManLoop"} ,			NPC_MakePairManLoop,		0 },
    { {"MakePairManWindowTalked"},	NPC_MakePairManWindowTalked,0 },
#endif
#ifdef _NPC_FUSION
  	{ {"PetFusionManInit"},			NPC_PetFusionManInit,		0 },
    { {"PetFusionManTalked"},		NPC_PetFusionManTalked,		0 },
  	{ {"PetFusionManLoop"} ,		NPC_PetFusionManLoop,		0 },
    { {"PetFusionManWindowTalked"},	NPC_PetFusionManWindowTalked,0 },
#endif
#ifdef _ITEM_NPCCHANGE
    { {"ItemchangeManInit"},		NPC_ItemchangeManInit,			0 },
    { {"ItemchangeManTalked"},		NPC_ItemchangeManTalked,		0 },
	{ {"ItemchangeManLoop"} ,		NPC_ItemchangeManLoop,			0 },
    { {"ItemchangeManWindowTalked"},		NPC_ItemchangeManWindowTalked,	0 },
#endif
#ifdef	_NPC_ITEMUP
		{ {"ItemupManInit"},		NPC_ItemupManInit,			0 },
    { {"ItemupManTalked"},		NPC_ItemupManTalked,		0 },
	{ {"ItemupManLoop"} ,		NPC_ItemupManLoop,			0 },
    { {"ItemupManWindowTalked"},		NPC_ItemupManWindowTalked,	0 },
#endif
#ifdef _CFREE_petskill
    { {"FreePetSkillInit"} ,		NPC_FreePetSkillShopInit,	0 },
    { {"FreePetSkillTalked"} ,		NPC_FreePetSkillShopTalked,	0 },
    { {"FreePetSkillWindowTalked"},	NPC_FreePetSkillShopWindowTalked, 0 },
#endif

#ifdef _PETRACE // 宠物竞速
	{ {"PetRaceMasterInit"},			NPC_PetRaceMasterInit, 0},
	{ {"PetRaceMasterLoop"},			NPC_PetRaceMasterLoop, 0},
    { {"PetRaceMasterTalked"},		NPC_PetRaceMasterTalked, 0},
	{ {"PetRaceMasterWindowTalked"}, NPC_PetRaceMasterWindowTalked, 0},

	{ {"PetRacePetInit"},	NPC_PetRacePetInit, 0},
	{ {"PetRacePetLoop"},	NPC_PetRacePetLoop, 0},
	{ {"PetRacePetTalked"},	NPC_PetRacePetTalked, 0},
#endif

#ifdef _NEW_WARPMAN
	{ {"NewNpcManInit"},			NPC_NewNpcManInit, 0},
	{ {"NewNpcManLoop"},			NPC_NewNpcManLoop, 0},
    { {"NewNpcManTalked"},			NPC_NewNpcManTalked, 0},
    { {"NewNpcManWindowTalked"},	NPC_NewNpcManWindowTalked, 0},
#endif

#ifdef _ALLDOMAN   // (不可开) Syu ADD 排行榜NPC
    { {"AlldomanInit"} ,                        NPC_AlldomanInit,        0 },
    { {"AlldomanTalked"} ,                      NPC_AlldomanTalked ,     0 },
    { {"AlldomanWindowTalked"},                 NPC_AlldomanWindowTalked , 0},
#endif

#ifdef _NPC_WELFARE
    { {"WelfareInit"} ,                        NPC_WelfareInit,        0 },
    { {"WelfareTalked"} ,                      NPC_WelfareTalked ,     0 },
    { {"WelfareWindowTalked"},                 NPC_WelfareWindowTalked , 0},
#endif
#ifdef _VIGOR_SYS
    { {"ItemVigorShopInit"} ,			NPC_ItemVigorShopInit ,		0 },
    { {"ItemVigorShopTalked"} ,			NPC_ItemVigorShopTalked ,	0 },
    { {"ItemVigorShopWindowTalked"},		NPC_ItemVigorShopWindowTalked, 0 },
#endif
#ifdef _TIME_TICKET
  	{ {"ITEM_timeticket"}, ITEM_timeticket, 0},
#endif

#ifdef _ITEM_SETLOVER	// 结婚物品
    { {"ITEM_SetLoverUser"}, ITEM_SetLoverUser, 0 },
    { {"ITEM_LoverWarp"}, ITEM_LoverWarp, 0 },
    { {"ITEM_LoverUnmarry"}, ITEM_LoverUnmarry, 0 },
    { {"ITEM_LoverForceUnmarry"}, ITEM_LoverForceUnmarry, 0 },        //强制离婚
#endif

#ifdef _GM_ITEM	// GM命令物品
		{ {"ITEM_GMFUNCTION"}, ITEM_GMFUNCTION, 0 },
#endif

#ifdef _VIP_SERVER	// 会员服务
		{ {"ITEM_AddMemberPoint"}, ITEM_AddMemberPoint, 0 },
		{ {"ITEM_AddVipPoint"}, ITEM_AddVipPoint, 0 },
#endif
#ifdef _VIP_RIDE
		{ {"ITEM_VipRide"}, ITEM_VipRide, 0 },
#endif
#ifdef _NEW_NAME     //自定义称号
		{ {"ITEM_NewName"}, ITEM_NewName, 0 },
#endif
#ifdef _FM_ITEM
		{ {"ITEM_AddFame"}, ITEM_AddFame, 0 },
#endif
#ifdef _LUCK_ITEM
		{ {"ITEM_Luck"}, ITEM_Luck, 0 },
#endif
#ifdef _FM_METAMO
		{ {"ITEM_MetamoTime"}, ITEM_MetamoTime, 0 },
#endif
#ifdef _ITEM_GOLD
		{ {"ITEM_Gold"}, ITEM_Gold, 0 },
#endif
#ifdef _MYSTERIOUS_GIFT
		{ {"ITEM_MysteriousGift"}, ITEM_MysteriousGift, 0 },
#endif
#ifdef _BATTLE_PK
		{ {"ITEM_BattlePK"}, ITEM_BattlePK, 0 },
#endif	
#ifdef _SILENTZERO
		{ {"ITEM_SetSilentZero"}, ITEM_SetSilentZero, 0 },
#endif
#ifdef _PET_LEVEL_ITEM
		{ {"ITEM_PetLevelItem"}, ITEM_PetLevelItem, 0 },
#endif

#ifdef _ITEM_EFMETAMO
		{ {"ITEM_efMetamo"}, ITEM_efMetamo, 0 },
#endif
#ifdef _PET_BEATITUDE
		{ {"PET_BEATITUDE"}, PET_BEATITUDE, 0 },
#endif
#ifdef _GET_MULTI_ITEM
		{ {"ITEM_GetMultiItem"}, ITEM_GetMultiItem, 0 },
#endif
		{ {"ITEM_OldToNew"}, ITEM_OldToNew, 0 },
#ifdef _SAFE_PASSWORD
		{ {"ITEM_ItemPetLocked"}, ITEM_ItemPetLocked, 0 },
#endif
#ifdef _POINT_CARD
		{ {"ITEM_OnlineCost"}, ITEM_OnlineCost, 0 },
#endif
#ifdef _SUPER_MAN_ITEM
		{ {"ITEM_SuperManItem"}, ITEM_SuperManItem, 0 },
#endif
		{ {"ITEM_CharSave"}, ITEM_CharSave, 0 },
		{ {"ITEM_NoDelWarp"},	    ITEM_NoDelWarp, 0 },
		{ {"ITEM_RandWarp"},	    ITEM_RandWarp, 0 },
		{ {"ITEM_RandMsg"},	    ITEM_RandMsg, 0 },
#ifdef _SQL_BUY_FUNC
    { {"ITEM_OnlineBuy_recv"}, ITEM_OnlineBuy_recv, 0 },
#endif
#ifdef _JOB_AUTOPK
		{ {"ITEM_JobPk"}, ITEM_JobPk, 0 },
#endif
		{ {"ITEM_SeeEquip"}, ITEM_SeeEquip, 0 },
#ifdef _VIGOR_SYS
		{ {"ITEM_VigorSave"}, ITEM_VigorSave, 0 },
#endif
#ifdef _EV_NUM
		{ {"ITEM_EvItem"}, ITEM_EvItem, 0 },
#endif
#ifdef _OFFLINE_SYSTEM
		{ {"ITEM_OFFLINE"}, ITEM_OFFLINE, 0 },
#endif
		{ {"ITEM_AddProfessionLevel"}, ITEM_AddProfessionLevel, 0 },
#ifdef	_ZHIPIAO_SYSTEM
		{ {"ITEM_ZhiPiao"}, ITEM_ZhiPiao, 0 },
#endif
		{ {"ITEM_useRandEditBase"}, ITEM_useRandEditBase, 0 },
#ifdef	_DP_ZHIPIAO_ITEM
		{ {"ITEM_DpZhiPiao"}, ITEM_DpZhiPiao, 0 },
#endif
		{ {"ITEM_UpPointResetItem"}, ITEM_UpPointResetItem, 0 },
		{ {"ITEM_MM"}, ITEM_MM, 0 },
#ifdef _XC_CANGKU		
		{ {"ITEM_ICK"}, ITEM_ICK, 0 },		
		{ {"ITEM_PCK"}, ITEM_PCK, 0 },		
#endif
#ifdef _SPECIAL_SUIT
  	{ {"ITEM_SpecialSuitEquip"},	ITEM_SpecialSuitEquip,		0 },
  	{ {"ITEM_SpecialResuitEquip"},	ITEM_SpecialResuitEquip,	0 },
#endif	
#ifdef _NULL_CHECK_ITEM
		{ {"ITEM_NullCheck"}, ITEM_NullCheck, 0 },
#endif
#ifdef _MANOR_EQUIP
  	{ {"ITEM_ManorEquip"},	ITEM_ManorEquip,		0 },
  	{ {"ITEM_ReManorEquip"},	ITEM_ReManorEquip,	0 },
#endif	
		{ {"ITEM_MakePet"}, ITEM_MakePet, 0 },
		{ {"PET_OTHERBEATITUDE"}, PET_OTHERBEATITUDE, 0 },
		{ {"PET_MYBEATITUDE"}, PET_MYBEATITUDE, 0 },
		{ {"ITEM_useExpLv"}, ITEM_useExpLv, 0 },
#ifdef _ITEM_LUA
		{ {"ITEM_Lua"}, ITEM_Lua, 0 },
#endif
#ifdef _JZ_NEWSCRIPT_LUA
	{ {FUNCNAME_INITCALLBACK} ,					NPC_Lua_InitCallBack,					0 },
	{ {FUNCNAME_WALKPRECALLBACK} ,				NPC_Lua_WalkPreCallBack,			0 },
	{ {FUNCNAME_WALKPOSTCALLBACK} ,			NPC_Lua_WalkPostCallBack,			0 },
	{ {FUNCNAME_PREOVERCALLBACK} ,				NPC_Lua_PreOverCallBack,			0 },
	{ {FUNCNAME_POSTOVERCALLBACK} ,			NPC_Lua_PostOverCallBack,			0 },
	{ {FUNCNAME_WATCHCALLBACK} ,					NPC_Lua_WatchCallBack,				0 },
	{ {FUNCNAME_LOOPCALLBACK} ,					NPC_Lua_LoopCallBack,					0 },
	{ {FUNCNAME_TALKEDCALLBACK} ,				NPC_Lua_TalkedCallBack,				0 },
	{ {FUNCNAME_OFFCALLBACK} ,						NPC_Lua_OFFCallBack,					0 },
	{ {FUNCNAME_LOOKEDCALLBACK} ,				NPC_Lua_LookedCallBack,				0 },
	{ {FUNCNAME_ITEMPUTCALLBACK} ,				NPC_Lua_ItemPutCallBack,			0 },
	{ {FUNCNAME_WINDOWTALKEDCALLBACK} ,	NPC_Lua_WindowTalkedCallBack,	0 },
#ifdef _USER_CHARLOOPS
	{ {FUNCNAME_CHARLOOPSCALLBACK} ,	NPC_Lua_CharLoopsCallBack,	0 },
	{ {FUNCNAME_BATTLEPROPERTYCALLBACK} ,	NPC_Lua_BattleProPertyCallBack,	0 },
#endif
	
	{ {FUNCNAME_ITEMPEROVERCALLBACK} ,	NPC_Lua_ItemPerOverCallBack,	0 },
	{ {FUNCNAME_ITEMPOSTOVERCALLBACK} ,	NPC_Lua_ItemPostOverCallBack,	0 },
	{ {FUNCNAME_ITEMWATCHCALLBACK} ,	NPC_Lua_ItemWatchCallBack,	0 },
	{ {FUNCNAME_ITEMUSECALLBACK} ,	NPC_Lua_ItemUseCallBack,	0 },
	{ {FUNCNAME_ITEMATTACHCALLBACK} ,	NPC_Lua_ItemAttachCallBack,	0 },
	{ {FUNCNAME_ITEMDETACHCALLBACK} ,	NPC_Lua_ItemDetachCallBack,	0 },
	{ {FUNCNAME_ITEMDROPCALLBACK} ,	NPC_Lua_ItemDropCallBack,	0 },
	{ {FUNCNAME_ITEMPICKUPCALLBACK} ,	NPC_Lua_ItemPickUPCallBack,	0 },
#ifdef _Item_ReLifeAct
	{ {FUNCNAME_ITEMDIERELIFECALLBACK} ,	NPC_Lua_ItemDieReLifeCallBack,	0 },
#endif

	{ {FUNCNAME_BATTLEWINCALLBACK} ,	NPC_Lua_BattleWinCallBack,	0 },
	{ {FUNCNAME_BATTLEENDCALLBACK} ,	NPC_Lua_BattleEndCallBack,	0 },
#endif	//_JZ_NEWSCRIPT_LUA
};

BOOL initFunctionTable( void )
{

    int     i;
    {
        char*   strings[arraysizeof(correspondStringAndFunctionTable)];
        int     stringnum=0;
        for( i=0 ; i<arraysizeof(correspondStringAndFunctionTable) ; i++ )
            strings[stringnum++] =
                correspondStringAndFunctionTable[i].functionName.string;
        if( ! checkStringsUnique( strings, stringnum , 1 ) ){
            fprint("Function Name Table is overlapped.\n" );
            fprint("It is not allowed\n");
            return FALSE;
        }
    }

    for(i = 0; i < arraysizeof(correspondStringAndFunctionTable) ; i ++ ){
        correspondStringAndFunctionTable[i].hashcode =
            hashpjw( correspondStringAndFunctionTable[i].
                     functionName.string);
    }
    return TRUE;
}

void* getFunctionPointerFromName( char* funcname )
{
    int     i;
    int     hashcode;
    if( funcname == NULL || funcname[0] == '\0' ){
		return NULL;
	}
    hashcode = hashpjw( funcname );
    for( i=0 ; i<arraysizeof(correspondStringAndFunctionTable) ; i++ )
        if( correspondStringAndFunctionTable[i].hashcode == hashcode )
            if( strcmp( correspondStringAndFunctionTable[i].functionName.string,funcname ) == 0 ){
              return correspondStringAndFunctionTable[i].functionPointer;
            }

#ifdef DEBUG
    print("No such Function: %s\n" ,funcname );
#endif
    return NULL;
}
