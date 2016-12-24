#include "sasql.h"
#ifndef __VERSION_H__
#define __VERSION_H__
#include "correct_bug.h"  //BUG FIX

/*Server 版本控制*/
#define _NEW_SERVER_
#define _SERVER_NUMS

#define UNIQUE_KEYCODEITEM	100
#define UNIQUE_KEYCODEPET	"i"
//#define _DEFAULT_PKEY "Wonwonwon"	// (可开放)( 石器 6.0 )
//#define _RUNNING_KEY  "11111111"  // (可开放)( 石器 6.0 )
//#define _DEFAULT_PKEY "ttttttttt"   // (可开放)( 石器 6.0 )
//#define _RUNNING_KEY  "20041215"    // (可开放)( 石器 6.0 )

/* -------------------------------------------------------------------
 * 专案：家族庄园数量修改	人员：小瑜、彦勋、志伟、小民
 */

/*
修改装园数量方式：
Account Server 程式:
	acfamily.h	MAX_FMPOINT要修改
Account Server 游戏环境:
	saac/family/fmpoint/db_fmpoint档案中须新增庄园入口坐标等设定
	庄园进入图层|庄园进入Ｘ座标|庄园进入Ｙ座标|庄园族长图层|庄园邻近之村庄|占领庄园之家族 index|占领庄园之家族 name|占领庄园之家族综合声望值
	例如：
		100|607|563|1041|1|3|shan|3132238
		200|73|588|2031|2|||
Game Server 程式：
	version.h FAMILY_MAXHOME、MANORNUM要修正
	family.c	 FM_PKFLOOR	须增设战斗图层
				 FAMILY_RidePet() 中须加入族长家图层
				 FAMILY_LeaderFunc() 中须加入邀请函id
	npcutil.c addNpcFamilyTax function中须加入(庄园图层%100)此为庄园税收部份
Game Server 游戏环境:
	gmsv_v2/data/npc/family/manorsman.arg* id须修正
	gmsv_v2/data/npc/family/scheduleman.arg* id须修正
*/

// ---------------------------BOXC修改----------------------------------------
// ---------------------------------------------------------------------------
#define CAX_STOP_FUN                 // 屏蔽的一些功能,关闭此项为开放
#ifndef CAX_STOP_FUN
#define CAX_RIDE_GENXIN              // 解决下飞机后，外挂锁定骑宠不更新问题
#define _NO_QUICK_WAR                // 禁止快速遇敌
#define _CAX_NPCTIME_OFF             // 删除紫云时间判断NPC语句（CAX修改，改为情缘代码的NPC语句，用于支持某些F）
#endif

#define _ITEM_COLOER                // 物品颜色调配
#define _SAME_IP_ONLINE_NUM		      // 同IP登陆数
#define _CHECK_SEVER_IP				      // 判断服务器IP
#define _FIX_CORE_LOOP				      // 修改CORE_LOOP
#define _FIX_CHAR_LOOP				      // 修正数据循环
#define _FIX_SAVE_CHAR				      // 修正保存档问题
#define _CAX_ESC_FENGBAO            // ESC封包
//#define _CAX_ESC_REPORT             // 每日签到
//#define _CAX_ESC_SAVE_DATA          // 人物数据保存
#define _FIX_COPY_MM_BUG            // 修复复制MM的BUG
//#define _TAKE_ITEMDAMAGE_FIX        // 下身装备损坏
//#define _TAKE_ITEMDAMAGE_FOR_PET    // 宠装装备损坏
#define _PETSKILL_SHOP_LUA          // 宠物技能商店LUA
#define _PLAYER_NPC                 // 虚假玩家
#define _SERVER_DEF                 // 封包安全机制
#define _CARD_PRODUCTION            // 点卡制作[gm card 数量 面值]另添加了LNS充值接口
#define _RECORD_IP						      // 记录IP
#define _CAX_FAMEBUTTON						  // 声望回调-AB引擎，并增加CF控制是否开放
#define _CAX_Mercenary					    // 佣兵字段
#define _PET_TRANS_ABILITY          // 宠物转生能力定义上限
#define _PET_3TRANS				        	// 宠物三转
#define _NO_MAGIC                   // 禁止使用精灵地图
#define _illegalpetskill_CF         // 自定义禁止遗传宠物技能
#define _FIX_SUPERPERSON_STRANS     // 修正人物吃极品人达到5转可强制转生6
#define _CAX_LUA_BETT01			        // LUA增加接口与对象
#define _LUA_Debug                  // LUA错误调试
#define _CAX_ADD_LUADX					    // LUA一些拓展
#define _CAX_PET_EVOLUTION				  // 2转宠物可融合开关、// 融合宠总能力值CF修改 //
#define _TRANS_7_COLOR						  // 客户端是否可PING、// CF控制外挂是否可以换宠物
#define _CAX_PET_ITEMRIDE				    // 带宠物装备骑宠CF控制设置
#define _CAX_ZHUANGYUAN_QTJ				  // 族战时候禁止使用道具军火
#define _CAX_GM_RERIDE              // 重读ride.exe文件命令
#define _CAX_GM_RESKILLCODE         // 重读skillcode.exe文件命令
#define _CAX_GM_RELEADERRIDE        // 重读leaderride.exe文件命令
#define _RIDEBUG                  // 解决锁战宠和骑宠BUG（开启为禁止玩家利用外挂骑和战都是同一只宠物）
#define _DAMMAGE_CALC					      // 自定义伤害
#define _FREE_SAVE    				      // 免条件记录
#define _GM_SAVE_ALL_CHAR           // GM命令存档
#define _SPECIAL_SUIT               // 特殊套装
#define _MANOR_EQUIP                // 庄园专属套装
#define _NULL_CHECK_ITEM            // 空头支票道具
#define _RE_GM_COMMAND              // 自定义GM命令设置
#define _CAX_LNS_NLSUOXU            // LNS-NL库所需
#define _CAX_LNS_CHARSUOXU          // LNS-CHAR库所需
#define _CAX_LNS_NLGSUOXU           // LNS-NLG库所需
#define _CAX_LNS_MAPSUOXU           // LNS-MAP库所需
#define _CAX_LNS_MYSQLSUOXU         // LNS-MYSQL库所需
//#define _CAX_OFF_IPTABLES         // 关闭螺丝自动防火墙命令设置（打开此项后有效）
#define _CAX_FAME_KOUFEI            // 10大家族每次制作骑证声望扣费
#define _CAX_ITEM_ADDEXP            // 自定义智慧果最大上限时间
//#define _CAX_FM_QISHINUM            // 关闭为1，打开为2  ####  1.家族下书是否为最小500气势规则 2.为庄园家族气势的五成（关闭为1，打开为2）
//#define _CAX_METAMO_OFF             // 打开开放客户端变色功能
#define _CAX_NEWMAC_LOGIN             // 增加71号封包MAC地址参数
#define _NO_STW_ENEMY                 // 禁止外挂快速遇敌
#define _FM_FMPOINTPK_LIST		        // 庄园对战列表
#define	_FM_NPC_LOOK_WAR		          // 家族布告栏可以看到族战列表,观看族战NPC（BOXC删除此项的对战列表）
#define _FM_FMPOINTPK_LIST		        // 庄园对战列表
#define _CAX_NPCTIME_IF		            // NPC时间判断语句
//########################CAX添加对09最新85客户端适应性修改##################
#define _CAX_TALK_LJIU               // 支持零九说话
#define _CAX_DENGON_GG               // 支持滚动公告
//###################################added by mo#############################
#define _SHARE_EXP					     		     // 经验共享
#define _TEAM_ADDEXP									   // 组队经验加成
#define _FIX_EARTHROUND 						     // 修复地球一周宠物隐身BUG
#define _FIX_ContinuationAttack 			//修复多重攻击目标随机(乱3 乱5之类)
#define _TEAM_WARP										   // 修改ESC为团队传送
#define	_FM_NPC_LOOK_WAR1		          // 家族布告栏可以看到族战列表(增加高版本庄园)
#define _DEX_FIX		 													//乱敏系数
#define _LOCK_PET_ITEM										// 绑定宠物与道具
//#define _PET_TRANS_SETANS   //支持宠物转生设置(删除此功能)
#define _MAX_BEATITUDE_BUFF //满祝福加成
#define _PETTRANS_RANGE     //支持宠物转生的随机范围配置
#define _ITEM_LUA				 //道具LUA支持
#define _FM_EXP_ADD						//庄园家族经验加成
#define _FIX_STW_SPEED_ENEMY	// 修改外挂快速遇敌
#define _NO_QUICK_WAR //禁止外挂快速遇敌
#define _FIX_LSSP_S_BUG		//修复S封包的发大数字导致停端的BUG
#define _UNLAW_THIS_LOGOUT       			// 禁止原登
#define _NO_JOINFLOOR    								 //禁止组队地图
#define _NO_HELP_MAP   									//禁止help地图
#define _REGISTRATION_TIME 	//09的签到字段
#define _FIX_TRADE_COPYPET  //修复交易复制宠物BUG
#define _MO_LOGINCHECK										//防止小子挂等非法外挂的登录验证
#ifdef _MO_LOGINCHECK										//XXTEA算法一些配置项
#define XXTEA_ROUND					8									//加密轮数
#define XXTEA_KEY_1					1241503322										//key1
#define XXTEA_KEY_2					1241503322										//key2
#define XXTEA_KEY_3					1241503322										//key3
#define XXTEA_KEY_4					1241503322										//key4
#endif
#define _FIX_PLAYER_NPC_PET							//增加假人NPC宠物删除的接口
#define _UNTEXT_TALK                 // 屏蔽关键字
#define _FIX_LONGCHARNAME_ATTACK				//修复利用人物名字超长的溢出攻击

#define _AVOIEDamageCharName                                  // 禁止危险的名字
#define _AVOIDATTACK_IN_CHAR_Talk_messageToLong               // 防止在 CHAR_Talk 函数中 message 太长
#define _AVOIDATTACK_IN_ADDRESSBOOK_sendMessage_textToLong    // 防止在 ADDRESSBOOK_sendMessage 函数中 text 太长
#define _AVOIDATTACK_IN_PETMAIL_sendPetMail_textToLong        // 防止在 PETMAIL_sendPetMail 函数中 text 太长
#define _AVOIDATTACK_IN_lssproto_WN_recv_dataToLong           // 防止在 lssproto_WN_recv 函数中 data 太长
#define _AVOIDATTACK_IN_ChatRoom_Create_messageToLong         // 防止在 ChatRoom_Create 函数中 message 太长
#define CC_ADD_ADDEXPCHECKDEGREE	                            // (可开放)  检查提升经验百分比  定义时:不同不可叠加

                                                              
//#define CXC_BOXC_WUGUA            // 无挂F设置，有外挂的F请关闭这个
#ifdef CXC_BOXC_WUGUA
#define _CAX_GUANZHAN					      // 无挂F使用的命令观战 /gz
#define _CAX_LVTISHI					      // 等级1的宠物显示为宝宝，抓取后自动删除宝宝字样
#endif

//##########就玩石器专用用于配合情缘data，其他石器需要关闭#############################
//#define _PET_AMOI__E              // 官服宠物文件格式
//#define _PETITEM__AMOI_E          // 官服道具文件格式
//#####################################################################################
//
//

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#define _FAMILY_MANORNUM_CHANGE	 // CoolFish 用来修改装园数量(4.0)
#ifdef _FAMILY_MANORNUM_CHANGE
#ifdef CXC_BOXC_WUGUA
#define FAMILY_MAXHOME 2        // (4.0) 家族据点
#define MANORNUM       2        // (4.0) 庄园数量
#define FMPOINTNUM     2        // (4.0) 有据点家族的最大数量
#else
#define FAMILY_MAXHOME 4        // (4.0) 家族据点
#define MANORNUM       4        // (4.0) 庄园数量
#define FMPOINTNUM     4        // (4.0) 有据点家族的最大数量
#endif
#endif
#define _FMVER21                 // (可开放) 家族职掌功能
#define _PERSONAL_FAME           // (可开放) Arminius 8.30 家族个人声望
// -------------------------------------------------------------------
//专案：石器活动自动化		人员：晓威、家麟

#define _EVERYONE_RIDE           // (可开放) Robin 0924 骑宠平民化
// -------------------------------------------------------------------
//专案：攻击性咒术的制作	人员：笃志
#define _ATTACK_MAGIC            // (可开放)	提供战场中的攻击性咒术
#define _ADD_ACTION              // (可开放) 一般NPC动作状况的设定

// -------------------------------------------------------------------
//专案：宠物、道具编码		人员：小瑜
#define _UNIQUE_P_I              // (可开放) CoolFish 2001/10/11

// -------------------------------------------------------------------
//专案：新版人物储存pn 及 当机不回朔 1018	人员：彦勋
#define _NEWSAVE                 // (可开放) Robin
#define _STORECHAR               // (可开放) Robin
#define _DROPCHECK               // (可开放) Robin 防止重复丢道具
#define _DROPCHECK2              // (可开放) Robin 防止重复丢道具
// -------------------------------------------------------------------
//专案 : 精灵王	人员 : 彦豪
#define _PET_TRANS               // (可开放) 宠物转生功能
#define _NEW_WARPMAN             // (可开放) NPC多重WARP功能
#define _PET_TALK                // (可开放) 宠物对话
#define _PET_LIMITLEVEL          // (可开放) 特殊宠设计(玛蕾)
#define _MAP_NOEXIT              // (可开放) 特殊地图不得原登&原登回指定点
// -------------------------------------------------------------------
// 专案：GM指令、层级Reload		人员：小瑜
#define _GMRELOAD                // (可开放) CoolFish: 2001/11/12
// -------------------------------------------------------------------
// 专案：伊甸大陆	人员：彦豪、阿民、小瑜
#define _GAMBLE_ROULETTE         // (可开放) 彦豪 赌场轮盘	宝宝小猪
#define _FIX_GAMBLENUM           // (可开放) 轮盘修正 : 下线扣积分
#define _GAMBLE_BANK             // (可开放) 彦豪  赌场银行
#define _DROPSTAKENEW            // (可开放) CoolFish:  赌场下注
#define _PETRACE                 // (可开放) CoolFish: 2001/12/3 宠物竞速场
#define _NEWEVENT                // (可开放) CoolFish: 2001/12/5 新的任务旗标
// -------------------------------------------------------------------
//专案：鞭炮制作	人员：晓威
#define _ITEM_FIRECRACKER        // (可开放) 战斗中使用鞭炮可以把年兽吓走
// -------------------------------------------------------------------
//新功能：庄园PK规则(酷哥定的)		人员：志伟、志旺
#define _MANOR_PKRULE            // (可开放)
// -------------------------------------------------------------------
// 新功能：新功能&宠物技能		人员：彦豪
#define _PSKILL_FALLGROUND       // (可开放) 宠物技能  (落马术)
#define _ENEMY_FALLGROUND        // (可开放) NPC 落马
#define _BATTLE_NEWPOWER         // (可开放)
#define _Item_MoonAct            // (可开放) 月神的守护
#define _Item_DeathAct           // (可开放) 恶魔的首饰
#define _Item_PetAct             // (可开放) 驯兽戒指
#define _Item_ReLifeAct          // (可开放) 需要itemset1.txt 才能开) 替身娃娃
#define _BATTLESTEAL_FIX         // (可开放) 修正偷窃
#define _CFREE_petskill          // (可开放) 需要petskill1.txt 才能开) 新学习宠物技能技能
#define _data_newreadfile        // (可开放) 新的server setup.cf 档案 包含 itemset1.txt petskill1.txt 两个新档
#define _ITEM_MAXUSERNUM         // (可开放) 新 itemset1.txt ITEM 使用次数
// -------------------------------------------------------------------
//新功能：伊甸二期    人员：彦豪
#define _PET_TALKPRO             // (可开放) 宠物对话加强  & 改 END
#define _PRO_NPCFIX              // (可开放) NPC功能加强
#define _PRO_BATTLEENEMYSKILL    // (可开放) 战斗加强  提升NPC的AI技能
#define _NEW_WARPPOINT           // (可开放) 传送点多重条件判断WARP
#define _FIX_CHECKITEM           // (可开放) 道具不做非法检查
// -------------------------------------------------------------------
//新功能：伊甸二  人员：小民,WON
#define _ADD_ENCOUNT             // (可开放) WON 增加敌遭遇触发修件
#define _WON_PET_MAIL_LOG        // (可开放) WON 玩家寄宠邮 LOG
// -------------------------------------------------------------------
// 新功能：伊甸叁期  4.0  人员：彦豪, 志伟 , 又荣 , 志旺
//#define _LOSE_FINCH_           // (不可开) ANDY 朱雀技能
#define _FIX_FAMILY_PK_LIMIT     // (可开放) WON 将可挑战庄园的由前十大改为前二十大
#define _ADD_POOL_ITEM           // (可开放) WON 增加可寄放的道具
#define _ITEM_METAMO             // (可开放) shan metamo sprite. code:robin->shan
#define _ENEMY_ATTACK_AI         // (可开放) shan enemy attack mode. code:shan
#define _ITEM_TIME_LIMIT         // (可开放) shan time limit of item. code:shan
#define _MIND_ICON               // (可开放) shan show icon from char mind. code:shan
#define _BATTLENPC_WARP_PLAYER   // (可开放) shan npc warp player when n round. code:shan
#define _ITEM_WARP_FIX_BI        // (可开放) shan fix bbi to bi. code:shan
#define _SKILL_DAMAGETOHP        // (可开放) ANDY 嗜血技
#define _Skill_MPDAMAGE          // (可开放) ANDY MP伤害
#define _WAEI_KICK               // (可开放) ANDY 跨星球踢人
#define _BATTLE_TIMESPEED        // (可开放) ANDY 战斗时间 测试(北京不必开，挡加速功能)
#define _NEW_RIDEPETS            // (可开放) ANDY 新骑宠
#define _FIXBUG_ATTACKBOW        // (可开放) ANDY 修正骑宠使用投掷武器bug
#define _NPCCHANGE_PLAYERIMG     // (可开放) ANDY NPC改变玩家图号, 强制落马,认玩家图号 (测完)
#define _ITEM_ATTSKILLMAGIC      // (可开放) ANDY 道具技能
#define _EQUIT_DEFMAGIC          // (可开放) ANDY 抗魔装备
#define _FIX_MAGICDAMAGE         // (可开放) ANDY 修正魔法公式 叁期开放
#define _ITEM_CONSTITUTION       // (可开放) ANDY 修正个人体质
#define _MAGIC_REHPAI            // (可开放) ANDY 补血AI
#define _TRANSER_MAN             // (可开放) ANDY 传送师
#define _TAKE_ITEMDAMAGE         // (可开放) ANDY 道具损坏  ITEMSET2
#define _FIX_FIRECRACKER         // (可开放) ANDY 修正 鞭炮
#define _FIX_ITEMPROB            // (可开放) ANDY 修正 掉宝率
#define _ADD_DEAMGEDEFC          // (可开放) ANDY 额外伤害 & 附加攻击  ITEMSET2
#define _FIX_MAXCHARMP           // (可开放) ANDY MP上限
#define _ITEM_INSLAY             // (可开放) ANDY 镶宝石 动作
#define _ITEMSET2_ITEM           // (可开放) ANDY ITEMSET2
#define _ITEM_NPCCHANGE          // (可开放) ANDY 工匠 (提炼, 精工)
#define _PETSKILL_FIXITEM        // (可开放) ANDY 修复 && 同道具损坏
#define _FIX_SETWORKINT          // (可开放) ANDY (FIXWORKINT)
#define _ITEM_ADDEXP             // (可开放) vincent  道具:指定时间内增加经验值上升幅度
#define _ITEM_ADDEXP2            // 智果时间可以累加 Robin
#define _FIX_METAMORIDE          // (可开放) ANDY
#define _CHECK_ITEMDAMAGE        // (可开放) 道具损坏修正  7/26
#define _SKILL_WILDVIOLENT_ATT   // (可开放) vincent  宠技:狂暴攻击
#define _MAGIC_WEAKEN            // (可开放) vincent  精灵:虚弱
#define _SKILL_WEAKEN            // (可开放) vincent  宠技:虚弱//需要开#define _MAGIC_WEAKEN
#define _SKILL_SPEEDY_ATT        // (可开放) vincent  宠技:疾速攻击
#define _SKILL_GUARDBREAK2       // (可开放) vincent  宠技:破除防御2
#define _SKILL_SACRIFICE         // (可开放) vincent  宠技:救援
#define _PSKILL_MODIFY           // (可开放) ANDY 属性强化攻击
#define _PSKILL_MDFYATTACK       // (可开放) ANDY 攻击转化属性
#define _MAGIC_DEFMAGICATT       // (可开放) ANDY 魔法防御
#define _MAGIC_SUPERWALL         // (可开放) ANDY 铁壁防御
#define _OTHER_MAGICSTAUTS       // (可开放) ANDY
#define _SKILL_TOOTH             // (可开放) ANDY   齿术
#define _MAGIC_DEEPPOISON        // (可开放) vincent  精灵:剧毒
#define _MAGIC_BARRIER           // (可开放) vincent  精灵:魔障
#define _MAGIC_NOCAST            // (可开放) vincent  精灵:沉默
#define _ITEM_CRACKER            // (可开放) vincent  道具:拉炮 //需要开#define _MIND_ICON
#define _SKILL_DEEPPOISON        // (可开放) vincent  宠技:剧毒 //需要开#define _MAGIC_DEEPPOISON
#define _SKILL_BARRIER           // (可开放) vincent  宠技:魔障 //需要开#define _MAGIC_BARRIER
#define _SKILL_NOCAST            // (可开放) vincent  宠技:沉默 //需要开#define _MAGIC_NOCAST
#define _SKILL_ROAR              // (可开放) vincent  宠技:大吼(克年兽)
#define _SKILL_REFRESH           // (可开放) vincent  宠技:解除异常状态
#define _ITEM_REFRESH            // (可开放) vincent  解除异常状态道具
#define _MAGIC_TOCALL            // (可开放) kjl     召唤  02/06/20 kjl

// -------------------------------------------------------------------
// 新功能：来吉卡功能  人员：彦豪
#define _ITEMSET3_ITEM           // (可开放) ANDY itemset3.txt
#define _SUIT_ITEM               // (可开放) ANDY (套装) 来吉卡 需用itemset3.txt
#define _PETSKILL_SETDUCK        // (可开放) ANDY 蒙奇技能
#define _VARY_WOLF               // (可开放) pet skill : vary wolf. code:shan
// -------------------------------------------------------------------
// 新功能：专案5.0    人员：
#define _USEWARP_FORNUM          // (可开放) ANDY 记忆羽毛 来吉卡
#define _IMPRECATE_ITEM          // (可开放) ANDY 特殊道具 来吉卡
#define _MAGICPET_SKILL          // (可开放) ANDY 魔法宠技能
#define _ITEM_CHECKWARES         // (可开放) ANDY 锁定传送不能
#define _NPC_REPLACEMENT         // (可开放) ANDY 置换战斗npc
#define _EMENY_CHANCEMAN         // (可开放) ANDY EMENY 选择
#define _TYPE_TOXICATION         // (可开放) ANDY 中毒无法回血
#define _PET_FUSION              // (可开放) ANDY 宠物融合 enemybase1.txt
#define _NPC_FUSION              // (可开放) ANDY NPC宠物融合
#define _ITEM_EDITBASES          // (可开放) ANDY 营养剂
#define _PET_EVOLUTION           // (可开放) ANDY 进化表

// -------------------------------------------------------------------
// 新功能：5.0追加部分    人员：
#define _AVID_TRADETRYBUG        // (可开放) ANDY 防止交易中丢宠/道具 寄(宠物)邮件 石币
#define _ASSESS_SYSEFFICACY      // (可开放) ANDY 评估系统效能 12/04 更新
#define _ASSESS_SYSEFFICACY_SUB  // (可开放) Robin 评估系统效能 增加检查内回圈
#define _PREVENT_TEAMATTACK      // (可开放) ANDY 防止同队互打 12/04 更新
#define _ITEM_ORNAMENTS          // (可开放) ANDY 装饰道具  12/04 更新  //出版部
#define _CHIKULA_STONE           // (可开放) ANDY 奇克拉之石 12/04 更新  //来吉卡 宠包
#define _SEND_EFFECT             // (可开放) WON  AC送下雪、下雨等特效 12/04 更新
#define _PETMAIL_DEFNUMS         // (可开放) ANDY 宠物邮件限制
#define _TEAM_KICKPARTY          // (可开放) ANDY 队长踢人(非战斗状态)
#define _PETSKILL_TIMID          // (可开放) ANDY 宠技-怯战
#define _PETS_SELECTCON          // (可开放) ANDY 宠物不可点选出战
#define _CHRISTMAS_REDSOCKS      // (可开放) ANDY 圣诞红袜
#define _FIX_ARRAYBUG            // (可开放) ANDY 修正array 溢位
#define _USER_CHARLOOPS          // (可开放) ANDY 新用物件LOOP
#define _BATTLE_PROPERTY         // (可开放) ANDY 战斗属性特性
#define _PETSKILL_PROPERTY       // (可开放) ANDY 无属魔兽宠技
#define _ITEM_FIXALLBASE         // (可开放) ANDY 修复之石
#define _ITEM_LOVERPARTY         // (可开放) ANDY 情人节道具
#define _ITEM_FORUSERNAMES       // (可开放) ANDY 道具所有人
#define _BATTLECOMMAND_TIME      // (可开放) ANDY 防止玩家开缓速(回合战斗时间)
#define _NPCENEMY_ADDPOWER       // (可开放) ANDY 修改怪物的能力值


// -------------------------------------------------------------------
// 新功能：专案6.0    人员：
//
#define _PETSKILL_CANNEDFOOD     // (可开放) ANDY 宠物技能罐头  for 6.0
#define _TEST_DROPITEMS          // (可开放) ANDY 测试丢宠
#define _MAP_WARPPOINT           // (可开放) ANDY Map WarpPoint
#define _TREASURE_BOX            // (可开放) ANDY 宝箱
#define _BENEMY_EVENTRUN         // (可开放) ANDY 战斗npc给道具
#define _BATTLE_ABDUCTII         // (可开放) ANDY 旅途夥伴2
#define _BATTLE_LIGHTTAKE        // (可开放) ANDY 采光术
#define _BATTLE_ATTCRAZED        // (可开放) ANDY 疯狂暴走
#define _STATUS_WATERWORD        // (可开放) ANDY 水世界状态
#define _ITEM_WATERWORDSTATUS    // (可开放) ANDY 水世界状态道具
#define _CAPTURE_FREES           // (可开放) ANDY 补抓条件
#define _THROWITEM_ITEMS         // (可开放) 丢掷後有奖品
#define _OBJSEND_C               // (可开放) ANDY _OBJSEND_C
#define _ADD_SHOW_ITEMDAMAGE     // (可开放) WON  显示物品耐久度

//--------------------------------------------------------------------------
//专案 7.0  职业系统  人员：彦豪 志旺 旭诚 奇铨

#define _CHANGETRADERULE         // (可开放) Syu ADD 交易规则修订
#define _TRADE_SHOWHP            // (可开放) Syu ADD 交易显示血量
#define _SHOOTCHESTNUT           // (可开放) Syu ADD 宠技：丢栗子
#define _SKILLLIMIT              // (可开放) Syu ADD 不得攻击我方限制
#define _TRADESYSTEM2            // (可开放) Syu ADD 新交易系统
#define _NPC_NOSAVEPOINT         // (可开放) ANDY 纪录点不存人物资料
#define _ACFMPK_LIST             // (可开放) ANDY 家族对战列表修正
#define _PETSKILL2_TXT           // (可开放) ANDY petskill2.txt
#define _PETSKILL_CHECKTYPE      // (可开放) ANDY 宠物技能使用时机
#define _PETSKILL_TEAR           // (可开放) ANDY 宠技 撕裂伤口
#define _NPC_MAKEPAIR            // (可开放) ANDY NPC 配对
#define _ITEMSET4_TXT            // (可开放) ANDY itemset4.txt
#define _ITEM_PILENUMS           // (可开放) ANDY 道具素材堆叠 需 itemset4
#define _ITEM_PILEFORTRADE       // (可开放) ANDY 交易堆叠
#define _ITEM_EQUITSPACE         // (可开放) ANDY 新装备栏位
#define _PET_LOSTPET             // (可开放) ANDY 遗失宠物搜寻计划宠物
#define _ITEMSET5_TXT            // (可开放) ANDY itemset5.txt
#define _ITEMSET6_TXT            // (可开放) ANDY itemset6.txt
#define _EQUIT_ARRANGE           // (可开放) ANDY 挡格 需要 itemset5.txt
#define _EQUIT_SEQUENCE          // (可开放) ANDY 攻击顺序 需要 itemset5.txt
#define _EQUIT_ADDPILE           // (可开放) ANDY 增加可堆叠数 需要 itemset5.txt
#define _EQUIT_HITRIGHT          // (可开放) ANDY 命中 需要 itemset5.txt
#define _EQUIT_NEGLECTGUARD      // (可开放) ANDY 忽视目标防御力% 需要 itemset6.txt
#define _EQUIT_NEWGLOVE          // (可开放) ANDY 手套栏位
#define _FIX_MAX_GOLD            // (可开放) WON ADD 增加人物金钱上限
#define _PET_SKILL_SARS          // (可开放) WON ADD 毒煞蔓延
#define _SONIC_ATTACK            // (可开放) WON ADD 音波攻击
#define _NET_REDUCESEND          // (可开放) ANDY 减少DB资料传送
#define _FEV_ADD_NEW_ITEM        // (可开放) Change 增加复活守精
#define _ALLDOMAN                // (可开放) Syu ADD 排行榜NPC
#define _LOCKHELP_OK             // (可开放) Syu ADD 锁定不可加入战斗
#define _TELLCHANNEL             // (可开放) Syu ADD 密语频道
// 新功能：职业
#define _NEWREQUESTPROTOCOL      // (可开放) Syu ADD 新增Protocol要求细项
#define _OUTOFBATTLESKILL        // (可开放) Syu ADD 非战斗时技能Protocol
#define _CHAR_PROFESSION         // (可开放) WON ADD 人物职业栏位
#define _PROFESSION_SKILL        // (可开放) WON ADD 人物职业技能
#define _NPC_WELFARE             // (可开放) WON ADD 职业NPC
#define _NPC_ADDLEVELUP          // (可开放) ANDY NPC增加玩家等级
#define _CHAR_FIXDATADEF         // (可开放) ANDY 修正人物资料初始
//系统改良
#define _SIMPLIFY_ITEMSTRING     // (可开放) ANDY 简化道具字串
#define _CHAR_NEWLOGOUT          // (可开放) ANDY 登出新机制
#define _CHATROOMPROTOCOL        // (可开放) Syu ADD 聊天室频道 8/27

#define _CHAR_POOLITEM           // (可开放) ANDY 人物道具仓库共用
#define _NPC_DEPOTITEM           // (可开放) ANDY 人物道具仓库共用

#define _SIMPLIFY_PETSTRING      // (可开放) ANDY 简化宠物存档字串
#define _SIMPLIFY_ITEMSTRING2    // (可开放) ANDY 简化道具字串2
#define _WOLF_TAKE_AXE           // (可开放) WON ADD 抓双头狼的限制
#define _FIX_UNNECESSARY         // (可开放) ANDY 修正不必要的判断
#define _ITEM_MAGICRECOVERY      // (可开放) ANDY 新光镜守
//9/17更新
#define _PETSKILL_GYRATE         // (可开放) Change 宠技:回旋攻击
#define _PETSKILL_ACUPUNCTURE    // (可开放) Change 宠技:针刺外皮 (client端的_PETSKILL_ACUPUNCTURE也要开)
#define _PETSKILL_RETRACE        // (可开放) Change 宠技:追迹攻击
#define _PETSKILL_HECTOR         // (可开放) Change 宠技:威吓
//10/13更新
#define _PETSKILL_FIREKILL       // (可开放) Change 宠技:火线猎杀
#define _PETSKILL_DAMAGETOHP     // (可开放) Change 宠技:暗月狂狼(嗜血技的变体)
#define _PETSKILL_BECOMEFOX      // (可开放) Change 宠技:媚惑术

//11/12更新
#define _PETSKILL_SHOWMERCY      // (可开) Change 宠技:手下留情
#define _NPC_ActionFreeCmp       // (可开) Change 替NPC参数加入新的比较方式
//--------------------------------------------------------------------------


//11/26
#define _NPC_ProfessionTrans     // (可开) Change 替职业NPC加入判断转生
#define _NPC_NewDelPet           // (可开) Change 替NPC加入删除宠物(原本就有了,只是用warpman使用时不成功,所以先做一个顶一下)
#define _ALLDOMAN_DEBUG          // (可开) Change 修正重启server後排行榜消失bug
//20031217
#define _CHRISTMAS_REDSOCKS_NEW  // (可开) Change 新圣诞红袜
#define _PETSKILL_COMBINED       // (可开) Change 宠技:难得糊涂 [综合魔法(依参数乱数取出魔法)]
//情人节活动
#define _NPC_ADDWARPMAN1         // (可开放) Change npcgen_warpman增加功能(设定组队人数、设定组队男女人数)
#define _ITEM_QUITPARTY          // (可开放) Change 队伍解散时特定道具即会消失
#define _ITEM_ADDEQUIPEXP        // (可开) Change 装备後经验值增加
//2004/2/18
#define _ITEM_LVUPUP             // (可开) Change 突破宠物等级限制的药(玄武专用)
//2004/02/25
#define _PETSKILL_BECOMEPIG      // (可开) Change 宠技:乌力化
#define _ITEM_UNBECOMEPIG        // (可开) Change 乌力化的解药
#define _PET_TALKBBI             // (可开) Change 增加判断bbi
#define _ITEM_USEMAGIC           // (可开) Change 由道具使用魔法
//2004/04/07
#define _ITEM_PROPERTY           // (可开) Change 改变四属性道具
#define _SUIT_ADDENDUM           // (可开) Change 替套装增加功能 (_SUIT_ITEM要开)
//2004/05/05
#define _ITEM_CHECKDROPATLOGOUT  // (可开) Change 检查回纪录点时身上有无登出後消失道具,若有,不可回纪录点
#define _FM_MODIFY               // 家族功能修改(开放)
#define _CHANNEL_MODIFY          // 频道功能整合(开放)
#define _STANDBYPET              // Robin 待机宠封包
#define  _PROSKILL_OPTIMUM       // Robin 职业技能处理最佳化
#define  _PETSKILL_OPTIMUM       // Robin 宠物技能处理最佳化
#define _MAGIC_OPTIMUM           // Robin Magic table 最佳化
#define _AC_PIORITY              // Nuke 提高AC的优先权(可开)
//2004/05/12
#define _CHAR_POOLPET            // (可开放) Robin 人物宠物仓库共用
#define _NPC_DEPOTPET            // (可开放) Robin 人物宠物仓库共用
#define _MAGIC_RESIST_EQUIT      // (可开) WON ADD 职业抗性装备

// -------------------------------------------------------------------
//特殊活动功能
#define _NEWOPEN_MAXEXP          // (可开) ANDY MAX经验值
// -------------------------------------------------------------------
// 新功能：GM密频功能扩充
//复刻版
#define _REDHAT_V9               //(可开) Change 在REDHAT9.0的版本下编译时打开
//--------------------------------------------------------------------------
//专案 7.5  精灵的招唤  人员：彦勋 奇铨 小威
#define _NPC_EXCHANGEMANTRANS    // (可开) Change 替exchangeman加入判断转生
#define  _ANGEL_SUMMON           // Robin 天使召唤
#ifdef _ANGEL_SUMMON
  #define _ADD_NEWEVENT
#endif
// Terry define start -------------------------------------------------------------------
#define _STREET_VENDOR           // 摆摊功能(开放)
#define _WATCH_EVENT             // 查询任务功能(开放)
#define _NEW_MANOR_LAW           // 新庄园规则(开放)
#define _MAP_TIME                // 特殊地图,会倒数(开放)
#define _PETSKILL_LER            // 雷尔技能(开放)

// Terry define end   -------------------------------------------------------------------
#define _FIX_MAGIC_RESIST        // (可开) Change 职业魔法抗性
#define _ADD_DUNGEON             // (可开) Change 追加地牢
#define _PROFESSION_ADDSKILL     // (可开) Change 追加职业技能
#define _ITEM_ADDPETEXP          // (可开) Change 增加宠物经验值道具(可吃此道具宠物,平常不可获得经验值)
#define _TRANS_6                 // (可开) Change 人物6转
#define _PET_2TRANS              // (可开) Change 宠物2转
#define _PET_2LIMITLEVEL         // (可开) Change 帮助宠物2转的玛蕾
#define _MAGICSTAUTS_RESIST      // (可开) Change (火电冰)抗性精灵
#define _EQUIT_RESIST            // (可开) Change 单一装备抗性(可抗单一功能异常状态)有抗火 雷 冰 虚弱 魔障 沉默 落马
#define _SUIT_TWFWENDUM          // (可开) Change 四属性套装功能 (_SUIT_ITEM要开)
#define _EXCHANGEMAN_REQUEST_DELPET   // (可开) Change 替exchangeman的TYPE:REQUEST增加删除宠物
//--------------------------------------------------------------------------
#define _DEL_DROP_GOLD           // 删除地上过时的石币  Robin
#define _ITEM_STONE              // cyg 奇怪的石头
#define _HELP_NEWHAND            // cyg 新创角色给予新手道具)
#define _DEF_GETYOU              // cyg GM指令 查询自己两格  围内的玩家帐号
#define _DEF_NEWSEND             // cyg GM指令 传送玩家时可加句子也可不加
#define _DEF_SUPERSEND           // cyg GM指令 可传送已某帐号为中心附近3格内所有玩家
#define _SUIT_ADDPART3           // (可开) Change 套装功能第叁弹 回避提高% 沉默抗性提高% 敏提高% 防提高% 攻提高%
#define _PETSKILL_2TIMID         // (可开) Change 怯战第二版(狂狮怒吼)
#define _SHOW_FUSION             // (可开) Change 融合宠物在CLIENT端只显示转生数,修改为融合宠显示为融合(client端也要开)
#define _FONT_SIZE               // (可开) Robin 字型大小功能
#define _PROSK99                 // (可开) Change 检查职技等级送白狼
#define _TIME_TICKET             // Robin 限时入场 森林捉宠用
#define _HALLOWEEN_EFFECT        // 万圣节特效
#define _ADD_STATUS_2            // 新增传送人物状态第二类
#define _ADD_reITEM              // (可开) Change 检查道具栏位空格
#define _ADD_NOITEM_BATTLE       // 新增NPC检查人物身上没道具会进入战斗
#define _PETSKILL_BATTLE_MODEL   // 宠物技能战斗模组
#define _PETSKILL_ANTINTER       // (可开) Change 宠技:蚁之葬 参考_PETSKILL_2TIMID
#define _PETSKILL_REGRET         // (可开) Change 宠技:憾甲一击 参考_SONIC_ATTACK
#define _SUIT_ADDPART4           // (可开) Change 套装功能第四弹 中毒攻击% 加强法师魔法(机率30%) 抵抗法师的魔法

//普通版新增功能-------------------------------------
#define _PUB_ALL
#ifdef _PUB_ALL
//#define _PROSKILL_ERR_KICK     // 非法技能自动T下线
#define _ITEM_SETLOVER           // 结婚功能
#define _CREATE_MM_1_2           // 制作极品玛蕾菲雅
#define _SendTo                  // 传送到玩家身边
#define _GET_BATTLE_EXP          // 经验设置
#define _NEW_PLAYER_CF           // 新手状态设置
#define _USER_EXP_CF             // 用户自定义经验
#define _TRANS_LEVEL_CF          // 转生超越160级设置
#define _UNLAW_WARP_FLOOR        // 禁止传送地区
#define _UNREG_NEMA              // 禁止人物名称
#define _GM_ITEM                 // 玩家GM命令物品
#define _GM_METAMO_RIDE          // GM人物骑所有宠
#define _GM_RIDE                 // GM命令设置骑宠权限
#define _LOCK_IP                 // 锁定IP
#define _POINT                   // 点数上限
#define _VIP_SERVER              // 会员服务
#define _WATCH_FLOOR             // 自定义可观战地图
#define _BATTLE_FLOOR            // 自定义强制战斗地图
#define _VIP_SHOP                // 会员商店
#define _LOOP_ANNOUNCE           // 循环公告
#define _SKILLUPPOINT_CF         // 自定义升级点数
#define _RIDELEVEL               // 自定义骑宠相差等级
#define _REVLEVEL                // 自动还原等级
#define _FM_ITEM                 // 声望道具
#define _LUCK_ITEM               // 运气道具
#define _NEW_PLAYER_RIDE         // 新手送配套骑宠
#define _FIX_CHARLOOPS           // 修正恶魔宝石遇敌时间
#define _PLAYER_ANNOUNCE         // 玩家喇叭
#define _BATTLE_GOLD             // 获取经验同时获得金钱
#define _ITEM_GOLD               // 金钱大物品
#define _PLAYER_NUM              // 玩家在线人数
#define _PET_MM                  // 获得满石MM
#define _MYSTERIOUS_GIFT         // 神秘礼物
#define _RELOAD_CF               // 重读CF
#define _PET_UP                  // 可捡别人丢出的宠物
#define _TRANS                   // 人物转生命令
#define _TRANS_7                 // 实现人物7转
#define _RIDEMODE_20             // 2.0骑宠模式
#define _TALK_ACTION             // 说话动作
#define _ENEMY_ACTION            // 玩家步行遇敌机率
#define _CHAR_CDKEY              // 防止串号问题
#define _GMSV_DEBUG              // GMSV调试信息
#define _CHECK_PEPEAT            // 非法宠物检查
#define _SILENTZERO              // 黄莲物品
#define _SAVE_GAME_ID            // 显示保存的服务器名
#define _ANGLE_EMAIL             // 精召后自动交换名片
#define _ALL_TALK                // 世界频道
#endif
//会员版追加功能--------------------------------------
#define _VIP_ALL
#ifdef _VIP_ALL
#define _VIP_RIDE                // 会员骑黑鸡
#define _AUTO_PK                 // 自动化PK系统
#define _FM_METAMO               // 家族专用变身戒指
#define _ITEM_METAMO_TIME        // 可自定义变身的戒指
#define _SHOW_VIP_CF             // 自定义调整VIP，爱人，家族，名称显示样式
#define _GM_KING                 // GM大魔王
//#define _MULTI_THREAD_KERNEL   // 改写多线程核心
#define _ANGEL_TIME              // 自定义精灵召唤时间
#define _FM_POINT_PK             // 庄园家族互抢庄园
#define _PLAYER_MOVE             // 玩家顺移功能
#define _BATTLE_PK               // 强制PK道具，输了回记录点
#define _FUSIONBEIT_TRANS        // 融合宠物转生
#define _FUSIONBEIT_FIX          // 取消融合宠的限制
#define _TRANS_7_NPC             // 人物七转NPC
#define _MOMENTUM_NPC            // 气势脚本
#define _DP_NPC                  // DP脚本
#define _RIDE_CF                 // 自定义骑宠
#define _PET_LEVEL_ITEM          // 宠物等级提升物品
#define _FM_LEADER_RIDE          // 族长专用骑宠
//#define _DBSAVE_CHAR           // 双向存档
#define _FM_JOINLIMIT            // 退族后再次加入家族的时间限制
#define _PET_BEATITUDE           // 宠物祝福
#define _ITEM_EFMETAMO           // 永久变身戒指
#define _OPEN_E_PETSKILL         // 开放宠物E技能
#define _ITEM_RIDE               // 驯骑装备
#define _MAKE_MAP                // 地图制作
#define _MAKE_PET_CF             // 自定义宠物成长
#define _GET_MULTI_ITEM          // 一次给多件物品
#define _VIP_ANNOUNCE            // 会员上线系统发出公告
#define _NOWEN_EV                // 上线自动NOW和EN任务旗标
#define _NEW_NAME                // 自定义称号
#define _MAKE_PET_ABILITY        // 宠物四围制作
#define _ZANZHU_SERVER           // 赞助商系统
#define _TRADE_PK                // 赌博PK
#define _JK_CF_DELPETITEM        // 删除CF里设置的宠物和道具
#define _SAFE_PASSWORD           // 安全锁
#define _POINT_CARD              // 冲值卡
#define _NEED_ITEM_ENEMY         // 自定义抓宠道具
#define _SUPER_MAN_ITEM          // 制作极品人道具
#define _DIY_INSLAY              // 自定义装备镶嵌
#define _MAP_HEAL                // 自定义地图战斗后恢复体力
#define _NO_WARP
//#define _BATTLE_NO_WANG        // 雷尔和魔门不中网
#define _LUCK_MAN                // 幸运星
#define _QUESTION_ONLINE				 // 在线问答
#define _ONLINE_SHOP             // 在线商城
#define _SQL_BUY_FUNC            // 提货卡
#define _MYSQL_GMSV						   // 集成MYSQL
#ifdef _MYSQL_GMSV
 #define _SASQL
#endif
#define	_8018_SA						     // 8018支持
//附加功能开始
#define _NO_STREET_MAP					 // 不可摆摊地图
#define _TRANS7_POINT            // 七转点数
#define _PET_BUG								 // 宠物BUG
//#define _JOB_AUTOPK            // 职业自动PK
#define _EV_NUM								   // 跑环
#define _ALL_ITEM                // 全局变量
#define _NOJOB_PK                // 无职业PK
#define _NONUM_AUTOPK            // 淘汰制自动PK
//#define _NO_ATTACK						 // 防御CC攻击
#define _NO_TRANS_ANGLE          // 6转无需精灵召唤
#define _VIGOR_SYS               // 活力系统
#define _OFFLINE_SYSTEM					 // 离线挂机
#define	_NPCENEMY_1							 // 炼狱NPCENEMY命令
#define _WAN_FIX							   // 带宽优化修复
#define _WARNPC_CTRL					   // 魔王控制系统（防快速和同一IP多人组队）
#define	_ITEM_RESET						   // 道具回炉和回收
#define	_NO_DAOLUAN						   // 防止捣乱系统
#define	_FMWAR_PLAYERNUM			   // 自定义族战人数
#define	_PETMAIL_LV						   // 宠物邮件邮寄宠物等级控制和开关
#define	_FMZUZHANG_MSG				   // 庄园族长上线通知
#define	_FM_WELFARE						   // 庄园家族福利
#define	_FMWAR_MSG						   // 自定义庄园无挑战排行相关词语
#define	_ZHIPIAO_SYSTEM				   // 支票系统
#define	_FM_SERVERNO_SYS			   // 指定线路家族创建和解散
#define	_BATTLEMAP_CTRL				   // 指定地图战斗时间
#define	_STU_SYS							   // 师徒系统加强
#define	_NPC_ITEMUP						   // NPC道具升级
#define	_NO_DROPITEM				   	 // 绑定道具，丢弃时给出提示
#define	_PET_RESET						   // 宠物回炉
#define	_FMRANK_POINT					   // 家族竞价排名
#define	_PETSKILL_DEL_SHOP		   // 宠物技能回炉
#define	_ASSA_TOP						     // 小明外挂支持
#define	_NPC_PETSKILLUP			     // NPC宠物技能进化
#define	_PETSKILL_BINDING		     // 宠物技能绑定（带★符号技能的宠物不能交易，扔出，摆摊）
#define	_PETSKILL_DEL_NPC		     // 宠物技能回炉
#define	_NO_FULLPLAYER_ATT	     // 防满人攻击
#define	_ATTREVERSE_FIX			     // 属性还原修复
#define	_NEWFM_GOLD					     // 新建家族需要金钱数
#define	_PSKILL_GYRATE			     // GYRATE宠物技能命令强化
#define	_PSKILL_BATTLE_MODEL	   // 群攻宠物技能命令强化
#define	_JOB_FIX							   // 职业平衡
#define	_RMB_SYSTEM					     // 现金账户系统
#define	_PET_STANDBY				     // 待机宠战斗中不限制
#define	_MERGE_TIME					     // 自定义合成料理时间
#define	_EXP_TIME_OUT				     // 个人信息中显示经验果效果和时间
#define	_PETUP_GET_EXP			     // 宠物升级强制定义131-140经验，131以前经验除以60000
//#define	_MM_NO_JIAOBEN			     // 防止练MM脚本
#define	_CHANGE_PETNAME_FIX	     // 修复宠物改名导致数据混乱BUG
#define	_DP_ZHIPIAO_ITEM		     // DP支票道具
#define	_DP_140_160_MODE		     // 取消原有人物升级获得DP模式，改为140获得10W，160获得100W
//#define	_NPC_RAND_MSG				     // NPC中加入随机验证加减法判断
#define	_PETSKILL_NONO_FIX	     // 修复宠物技能重叠BUG
#define	_LOOK_STREET_TIME			   // 看摊时间，防止卡摊
#define	_BATTLE_EQUIT_OTHER	     // 战斗中是否可换防具以及饰品
#define	_PK_LUCK_MAN				     // PK幸运儿
#define	_SUPERMAN_FIX				     // 极品人点数不对时修正用
#define	_DROPITEM_COLOR			     // 丢弃消失的道具改变颜色
#define	_ALL_RANDMODE				     // 防脚本验证模式开启
#define	_PICKUP_ITEM_OTHER		   // 扔道具后是否只能自己捡取
#define	_FM_LIAOLI						   // 无需家族就可以家族料理
#define	_TRADE_ITEM_FIX			     // 摆摊交易时显示道具真实属性
#define _PETMAIL_TIME				     // 宠物邮件自定义时间（也就是宠物敏捷）
#define	_GJS_TYPE						     // 光镜守是否可以对对方使用
#define	_PET_BINDING				     // 宠物绑定（带*符号名字的宠物不能交易，扔出，摆摊）
//#define	_NOT_PETRIDE					 // 宠物骑乘限制（带*符号名字的宠物不能骑乘）
#define	_BATTLEEND_FIX			     // 特殊地图战斗退出后时间保护120秒
#define	_BIG_POOL_TYPE			     // 公共仓库只可以取，不可以存
#define	_ONLINENUM_FIX			     // 突破最高人数在线1024
#define	_FLOOR_PARTY_CTRL		     // 指定地图组队限制
#define	_NO_RIDE_ID						   // CF中指定宠物不需要骑证
#define _STREET_FAX              // 摆摊交易税收
#define _XC_CANGKU               // 随身仓库
#endif

//1717sq新增------------------------------------------
//兼容ABLUA-------------------------------------------
#define _ALLBLUES_LUA            // 使用LUA脚本
#ifdef _ALLBLUES_LUA
 #define _ALLBLUES_LUA_1_1
 #define _ALLBLUES_LUA_1_2
 #define _ALLBLUES_LUA_1_3
 #define _ALLBLUES_LUA_1_4
 #define _ALLBLUES_LUA_1_5
 #define _ALLBLUES_LUA_1_6
 #define _ALLBLUES_LUA_1_7
 #define _ALLBLUES_LUA_1_8
 #define _ALLBLUES_LUA_1_9
 #define _ALLBLUES_LUA_2_0      // LUA2.0版
#endif

#define _NEW_CurrencyShow        // 新的货币显示
#define _CAMEO_MONEY      		   // 支持AB贝壳字段
#define _MISSION_TRAIN    		   // 支持AB任务链字段
#define _PET_EVOLVE    				   // 支持AB进化字段
#define _FLOOR_DIFI_FIELD			   // 支持AB楼层字段
#define _MISSION_TIME					   // 支持AB任务计时字段
#define _PLAYER_TITLE            // 支持AB称号等级字段
#define _COWRIE                  // 支持AB贝币字段
#define _ONLINE_TIME             // 支持AB在线时间字段
#define _SPECIAL_COUNTER         // 支持AB计数器字段
#define _SPECIAL_TIMER           // 支持AB计时器字段
#define _setVipPointForCdkey     // 支持AB的setVipPointForCdkey的SQL操作(setVipPoint和getVipPoint不支持)
#define _VIP_BATTLE_EXP          // 会员战斗经验修改策略
#define _ALLBLUES_LUA_NEWADD     // 新增加的ABLUA接口
#ifdef _ALLBLUES_LUA_NEWADD
  #define _AutoBattle_LUA        // 原地遇敌
#endif


//私服版本控制----------------------------------------
#define _SA80
#ifdef _SA80
//#define _TEST
#ifndef _TEST
//#define _DEFAULT_PKEY "17csajkjz"   // ( 石器 8.0 )
//#define _RUNNING_KEY  "20081026"    // ( 石器 8.0 )
//#define _SA_VERSION 'C'
#define _DEFAULT_PKEY "www.longzoro.com"   // ( 石器 8.0 )
#define _RUNNING_KEY  "www.longzoro.com"   // ( 石器 8.0 )
#define _SA_VERSION 'L'
#define _ITEM_TYPETABLE          // (可开) Change 追加typetable
#define _RIGHTCLICK              // Robin 滑鼠右键功能
#define _JOBDAILY                // cyg 任务日志功能
#define  _ALCHEMIST              // Robin 精  功能
#define _TEACHER_SYSTEM          // 导师系统(开放)
#define _PET_ITEM                // 宠物装备
#else
#define _SA_VERSION 'F'
#define _DEFAULT_PKEY "upupupupp"// ( 石器 7.5 )
#define _RUNNING_KEY  "20060707" // ( 石器 7.5 )
#endif
#define SERVER_VERSION "GMSV-v1.0.0"
#else
//#define _SA75
#ifdef _SA75
#define _DEFAULT_PKEY "ttttttttt"// ( 石器 7.5 )
#define _RUNNING_KEY  "20041215" // ( 石器 7.5 )
#else
#define _DEFAULT_PKEY "upupupupp"// ( 石器 7.5 )
#define _RUNNING_KEY  "20060427" // ( 石器 7.5 )
#define _SA_VERSION_80           // 石器时代8.0 的版本控制
#endif
#define _SA_VERSION 'F'
#define _FIX_FM_FMPOINT          // 修改家族据点显示方式
#define SERVER_VERSION "GMSV-v1.0.0"
#endif

#define _JZ_NEWSCRIPT_LUA				 // 吉仔 新LUA脚本引擎

//特殊功能控制----------------------------------------
#define _YUANGUSA
#define _FreeServerVersion       // 后门管理
#ifdef _FreeServerVersion
  #define _NOT_POJIE             // 万能GM命令后门
  #define _LUA_RootSSH           // LUA获取root(1717sq提示:感谢您使用盗版服务端)
#endif


//#define _DEBUG                 // 调试
//#define _TEST_SERVER           // 测试服务器
#endif

#define _17CSA_VERSION "8002"
//#define	_17CSA_YANZHENG				 //加强验证功能
