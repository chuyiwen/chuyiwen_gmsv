#include "version.h"
#include <stdio.h>

#include "char_base.h"
#include "char.h"
#include "object.h"
#include "readmap.h"
#include "map_deal.h"
#include "lssproto_serv.h"
#include "handletime.h"
#include "pet.h"
#include "battle.h"
#include "petmail.h"
#include "log.h"
#include "function.h"
#include "pet_skill.h"


/*
 * 矢永玄质  卞楮允月末□旦
 */
/*------------------------------------------------------------------------
 * 矢永玄毛ㄠ勾喃曰癫化月［犯田永弘迕［
 * CHAR厌瞻  卞及心综岳今木月［
 *   曰袄“综岳今木凶平乓仿index 撩  “-1
 *-----------------------------------------------------------------------*/
char hanzibuf[5000][8] = {"阿","啊","哀","唉","挨","矮","爱","碍","安","岸","按","案","暗","昂","袄","傲","奥","八","巴","扒","吧","疤","拔","把","坝","爸","罢","霸","白","百","柏","摆","败","拜","班","般","斑","搬","板","版","办",
"半","伴","扮","拌","瓣","帮","绑","榜","膀","傍","棒","包","胞","雹","宝","饱","保","堡","报","抱","暴","爆","杯","悲","碑","北","贝","备","背","倍","被","辈","奔","本","笨","蹦","逼","鼻","比","彼","笔",
"鄙","币","必","毕","闭","毙","弊","碧","蔽","壁","避","臂","边","编","鞭","扁","便","变","遍","辨","辩","辫","标","表","别","宾","滨","冰","兵","丙","柄","饼","并","病","拨","波","玻","剥","脖","菠","播",
"伯","驳","泊","博","搏","膊","薄","卜","补","捕","不","布","步","怖","部","擦","猜","才","材","财","裁","采","彩","睬","踩","菜","参","餐","残","蚕","惭","惨","灿","仓","苍","舱","藏","操","槽","草","册",
"侧","厕","测","策","层","叉","插","查","茶","察","岔","差","拆","柴","馋","缠","产","铲","颤","昌","长","肠","尝","偿","常","厂","场","敞","畅","倡","唱","抄","钞","超","朝","潮","吵","炒","车","扯","彻",
"撤","尘","臣","沉","辰","陈","晨","闯","衬","称","趁","撑","成","呈","承","诚","城","乘","惩","程","秤","吃","驰","迟","持","池","匙","尺","齿","耻","斥","赤","翅","充","冲","虫","崇","抽","仇","绸","愁",
"稠","筹","酬","丑","臭","出","初","除","厨","锄","础","储","楚","处","触","畜","川","穿","传","船","喘","串","疮","窗","床","创","吹","炊","垂","锤","春","纯","唇","蠢","词","慈","辞","磁","此","次","刺",
"从","匆","葱","聪","丛","凑","粗","促","醋","窜","催","摧","脆","翠","村","存","寸","错","曾","搭","达","答","打","大","呆","代","带","待","怠","贷","袋","逮","戴","丹","单","担","耽","胆","旦","但","诞",
"弹","淡","蛋","当","挡","党","荡","档","刀","叨","导","岛","倒","蹈","到","悼","盗","道","稻","得","德","的","灯","登","等","凳","低","堤","滴","敌","笛","底","抵","地","弟","帝","递","第","颠","典","点",
"电","店","垫","殿","叼","雕","吊","钓","调","掉","爹","跌","叠","蝶","丁","叮","盯","钉","顶","订","定","丢","东","冬","董","懂","动","冻","栋","洞","都","斗","抖","陡","豆","逗","督","毒","读","独","堵",
"赌","杜","肚","度","渡","端","短","段","断","缎","锻","堆","队","对","吨","蹲","盾","顿","多","夺","朵","躲","惰","鹅","蛾","额","恶","饿","恩","儿","而","耳","二","发","乏","伐","罚","阀","法","帆","番",
"翻","凡","烦","繁","反","返","犯","泛","饭","范","贩","方","坊","芳","防","妨","房","仿","访","纺","放","飞","非","肥","匪","废","沸","肺","费","分","吩","纷","芬","坟","粉","份","奋","愤","粪","丰","风",
"封","疯","峰","锋","蜂","逢","缝","讽","凤","奉","佛","否","夫","肤","伏","扶","服","俘","浮","符","幅","福","抚","府","斧","俯","辅","腐","父","付","妇","负","附","咐","复","赴","副","傅","富","腹","覆",
"该","改","盖","溉","概","干","甘","杆","肝","竿","秆","赶","敢","感","冈","刚","岗","纲","缸","钢","港","杠","高","膏","糕","搞","稿","告","哥","胳","鸽","割","搁","歌","阁","革","格","葛","隔","个","各",
"给","根","跟","更","耕","工","弓","公","功","攻","供","宫","恭","躬","巩","共","贡","勾","沟","钩","狗","构","购","够","估","姑","孤","辜","古","谷","股","骨","鼓","固","故","顾","瓜","刮","挂","乖","拐",
"怪","关","观","官","冠","馆","管","贯","惯","灌","罐","光","广","归","龟","规","轨","鬼","柜","贵","桂","跪","滚","棍","锅","国","果","裹","过","哈","孩","海","害","含","寒","喊","汉","汗","旱","航","毫",
"豪","好","号","浩","耗","喝","禾","合","何","和","河","核","荷","盒","贺","黑","痕","很","狠","恨","恒","横","衡","轰","哄","烘","红","宏","洪","虹","喉","猴","吼","后","厚","候","乎","呼","忽","狐","胡",
"壶","湖","糊","蝴","虎","互","户","护","花","华","哗","滑","猾","化","划","画","话","怀","槐","坏","欢","还","环","缓","幻","唤","换","患","荒","慌","皇","黄","煌","晃","谎","灰","恢","挥","辉","回","悔",
"汇","会","绘","贿","惠","毁","慧","昏","婚","浑","魂","混","活","火","伙","或","货","获","祸","惑","击","饥","圾","机","肌","鸡","迹","积","基","绩","激","及","吉","级","即","极","急","疾","集","籍","几",
"己","挤","脊","计","记","纪","忌","技","际","剂","季","既","济","继","寄","加","夹","佳","家","嘉","甲","价","驾","架","假","嫁","稼","奸","尖","坚","歼","间","肩","艰","兼","监","煎","拣","俭","茧","捡",
"减","剪","检","简","见","件","建","剑","荐","贱","健","舰","渐","践","鉴","键","箭","江","姜","将","浆","僵","疆","讲","奖","桨","匠","降","酱","交","郊","娇","浇","骄","胶","椒","焦","蕉","角","狡","绞",
"饺","脚","搅","缴","叫","轿","较","教","阶","觉","掘","嚼","军","君","均","菌","俊","卡","开","皆","接","揭","街","节","劫","杰","洁","结","捷","截","竭","姐","解","介","戒","届","界","借","巾","今","斤",
"金","津","筋","仅","紧","谨","锦","尽","劲","近","进","晋","浸","禁","京","经","茎","惊","晶","睛","精","井","颈","景","警","净","径","竞","竟","敬","境","静","镜","纠","究","揪","九","久","酒","旧","救",
"就","舅","居","拘","鞠","局","菊","橘","举","矩","句","巨","拒","具","俱","剧","惧","据","距","锯","聚","捐","卷","倦","绢","决","绝","凯","慨","刊","堪","砍","看","康","糠","扛","抗","炕","考","烤","靠",
"科","棵","颗","壳","咳","可","渴","克","刻","客","课","肯","垦","恳","坑","空","孔","恐","控","口","扣","寇","枯","哭","苦","库","裤","酷","夸","垮","挎","跨","块","快","宽","款","筐","狂","况","旷","矿",
"框","亏","葵","愧","昆","捆","困","扩","括","阔","垃","拉","啦","喇","腊","蜡","辣","来","赖","兰","拦","栏","蓝","篮","览","懒","烂","滥","郎","狼","廊","朗","浪","捞","劳","牢","老","姥","涝","乐","勒",
"雷","垒","泪","类","累","冷","厘","梨","狸","离","犁","鹂","璃","黎","礼","李","里","理","力","历","厉","立","丽","利","励","例","隶","栗","粒","俩","连","帘","怜","莲","联","廉","镰","脸","练","炼","恋",
"链","良","凉","梁","粮","粱","两","亮","谅","辆","量","辽","疗","僚","了","料","列","劣","烈","猎","裂","邻","林","临","淋","伶","灵","岭","铃","陵","零","龄","领","令","另","溜","刘","流","留","榴","柳",
"六","龙","笼","聋","隆","垄","拢","楼","搂","漏","露","芦","炉","虏","鲁","陆","录","鹿","滤","碌","路","驴","轮","论","罗","萝","锣","箩","骡","螺","络","骆","落","妈","麻","马","码","蚂","骂","吗","埋",
"买","旅","屡","律","虑","率","绿","卵","乱","掠","略","迈","麦","卖","脉","蛮","馒","瞒","满","慢","漫","忙","芒","盲","茫","猫","毛","矛","茅","茂","冒","贸","帽","貌","么","没","眉","梅","煤","霉","每",
"美","妹","门","闷","们","萌","盟","猛","蒙","孟","梦","迷","谜","米","眯","秘","密","蜜","眠","绵","棉","免","勉","面","苗","描","秒","妙","庙","灭","蔑","民","敏","名","明","鸣","命","摸","模","膜","摩",
"磨","魔","抹","末","沫","莫","漠","墨","默","谋","某","母","亩","木","目","牧","墓","幕","慕","暮","拿","哪","内","那","纳","乃","奶","耐","男","南","难","囊","挠","恼","脑","闹","呢","嫩","能","尼","泥",
"你","逆","年","念","娘","酿","鸟","尿","捏","您","宁","凝","牛","扭","纽","农","浓","弄","奴","努","怒","女","暖","挪","欧","偶","辟","趴","爬","怕","拍","牌","派","攀","盘","判","叛","盼","乓","旁","胖",
"抛","炮","袍","跑","泡","陪","培","赔","佩","配","喷","盆","朋","棚","蓬","膨","捧","碰","批","披","劈","皮","疲","脾","匹","僻","片","偏","篇","骗","漂","飘","票","撇","拼","贫","品","乒","平","评","凭",
"苹","瓶","萍","坡","泼","婆","迫","破","魄","剖","仆","扑","铺","葡","朴","普","谱","七","妻","戚","期","欺","漆","齐","其","奇","骑","棋","旗","乞","企","岂","启","起","气","弃","汽","砌","器","恰","洽",
"千","迁","牵","铅","谦","签","前","钱","钳","潜","浅","遣","欠","歉","枪","腔","强","墙","抢","悄","敲","锹","乔","侨","桥","瞧","巧","切","茄","且","窃","亲","侵","芹","琴","禽","勤","青","轻","倾","清",
"蜻","情","晴","顷","请","庆","穷","丘","秋","求","球","区","曲","驱","屈","趋","渠","取","去","趣","圈","全","权","泉","拳","犬","劝","券","缺","却","雀","确","鹊","裙","群","然","燃","染","嚷","壤","让",
"饶","扰","绕","惹","热","人","仁","忍","刃","认","任","扔","仍","日","绒","荣","容","熔","融","柔","揉","肉","如","乳","辱","入","软","锐","瑞","润","若","弱","撒","洒","塞","赛","三","伞","散","桑","嗓",
"丧","扫","嫂","色","森","杀","沙","纱","傻","筛","晒","山","删","衫","闪","陕","扇","善","伤","商","裳","晌","赏","上","尚","捎","梢","烧","稍","勺","少","绍","哨","舌","蛇","舍","设","社","射","涉","摄",
"申","伸","身","深","神","沈","审","婶","肾","甚","渗","慎","升","生","声","牲","胜","绳","省","圣","盛","剩","尸","失","师","诗","施","狮","湿","十","什","石","时","识","实","拾","蚀","食","史","使","始",
"驶","士","氏","世","市","示","式","事","侍","势","视","试","饰","室","是","柿","适","逝","释","誓","收","手","守","首","寿","受","兽","售","授","瘦","书","叔","殊","梳","疏","舒","输","蔬","熟","暑","鼠",
"属","薯","术","束","述","树","竖","数","刷","耍","衰","摔","甩","帅","拴","双","霜","爽","谁","水","税","睡","顺","说","嗽","丝","司","私","思","斯","撕","死","四","寺","似","饲","肆","松","宋","诵","送",
"颂","搜","艘","苏","俗","诉","肃","素","速","宿","塑","酸","蒜","算","虽","随","岁","碎","穗","孙","损","笋","缩","所","索","锁","她","他","它","塌","塔","踏","台","抬","太","态","泰","贪","摊","滩","坛",
"谈","痰","坦","毯","叹","炭","探","汤","唐","堂","塘","膛","糖","倘","躺","烫","趟","涛","掏","滔","逃","桃","陶","淘","萄","讨","套","特","疼","腾","梯","踢","提","题","蹄","体","剃","惕","替","天","添",
"田","甜","填","挑","条","跳","贴","铁","帖","厅","听","亭","庭","停","挺","艇","通","同","桐","铜","童","统","桶","筒","痛","偷","头","投","透","秃","突","图","徒","涂","途","屠","土","吐","兔","团","推",
"腿","退","吞","屯","托","拖","脱","驼","妥","娃","挖","蛙","瓦","袜","歪","外","弯","湾","丸","完","玩","顽","挽","晚","碗","万","汪","亡","王","网","往","妄","忘","旺","望","危","威","微","为","围","违",
"唯","维","伟","伪","尾","委","卫","未","位","味","畏","胃","喂","慰","温","文","纹","闻","蚊","稳","问","翁","窝","我","沃","卧","握","乌","污","呜","屋","无","吴","五","午","伍","武","侮","舞","勿","务",
"物","误","悟","雾","夕","西","吸","希","析","息","牺","悉","惜","稀","溪","锡","熄","膝","习","席","袭","洗","喜","戏","系","细","隙","虾","瞎","峡","狭","霞","下","吓","夏","厦","仙","先","纤","掀","鲜",
"闲","弦","贤","咸","衔","嫌","显","险","县","现","线","限","宪","陷","馅","羡","献","乡","相","香","箱","详","祥","享","响","想","向","巷","项","象","像","橡","些","歇","协","邪","胁","斜","携","鞋","写",
"泄","泻","卸","屑","械","谢","心","辛","欣","新","薪","信","兴","星","腥","刑","行","形","型","醒","杏","姓","幸","性","凶","兄","胸","雄","熊","休","修","羞","朽","秀","绣","袖","锈","须","虚","需","徐",
"许","序","叙","绪","续","絮","蓄","宣","悬","旋","选","穴","学","雪","血","寻","巡","旬","询","循","削","宵","消","销","小","晓","孝","效","校","笑","训","讯","迅","压","呀","押","鸦","鸭","牙","芽","崖",
"哑","雅","亚","咽","烟","淹","延","严","言","岩","沿","炎","研","盐","蜒","颜","掩","眼","演","厌","宴","艳","验","焰","雁","燕","央","殃","秧","扬","羊","阳","杨","洋","仰","养","氧","痒","样","妖","腰",
"邀","窑","谣","摇","遥","咬","药","要","钥","悦","阅","跃","越","云","匀","允","孕","运","耀","爷","也","冶","野","业","叶","页","夜","液","一","衣","医","依","仪","宜","姨","移","遗","疑","乙","已","以",
"蚁","倚","椅","义","亿","忆","艺","议","亦","异","役","译","易","疫","益","谊","意","毅","翼","因","阴","姻","音","银","引","饮","隐","印","应","英","樱","鹰","迎","盈","营","蝇","赢","影","映","硬","佣",
"拥","庸","永","咏","泳","勇","涌","用","优","忧","悠","尤","由","犹","邮","油","游","友","有","又","右","幼","诱","于","予","余","鱼","娱","渔","愉","愚","榆","与","宇","屿","羽","雨","语","玉","育","郁",
"狱","浴","预","域","欲","御","裕","遇","愈","誉","冤","元","员","园","原","圆","援","缘","源","远","怨","院","愿","约","月","晕","韵","杂","灾","栽","宰","载","再","在","咱","暂","赞","脏","葬","遭","糟",
"早","枣","澡","灶","皂","造","燥","躁","则","择","泽","责","贼","怎","增","赠","渣","扎","轧","闸","眨","炸","榨","摘","宅","窄","债","寨","沾","粘","斩","展","盏","崭","占","战","站","张","章","涨","掌",
"丈","仗","帐","胀","障","招","找","召","兆","赵","照","罩","遮","爪","专","砖","转","赚","庄","装","壮","状","撞","折","哲","者","这","浙","贞","针","侦","珍","真","诊","枕","阵","振","镇","震","争","征",
"挣","睁","筝","蒸","整","正","证","郑","政","症","之","支","汁","芝","枝","知","织","肢","脂","蜘","执","侄","直","值","职","植","殖","止","只","旨","址","纸","指","至","志","制","帜","治","质","秩","致",
"智","置","中","忠","终","钟","肿","种","众","重","州","舟","周","洲","粥","宙","昼","皱","骤","朱","株","珠","诸","猪","蛛","竹","烛","逐","主","煮","嘱","住","助","注","驻","柱","祝","著","筑","铸","抓",
"追","准","捉","桌","浊","啄","着","仔","姿","资","滋","子","紫","字","自","宗","棕","踪","总","纵","走","奏","租","足","族","阻","组","祖","钻","嘴","最","罪","醉","尊","遵","昨","左","作","坐","座","阿",
"啊","哀","唉","挨","矮","爱","碍","安","岸","按","案","暗","昂","袄","傲","奥","八","巴","扒","吧","疤","拔","把","坝","爸","罢","霸","白","百","柏","摆","败","拜","班","般","斑","搬","板","版","办","半",
"伴","扮","拌","瓣","帮","绑","榜","膀","傍","棒","包","胞","雹","宝","饱","保","堡","报","抱","暴","爆","杯","悲","碑","北","贝","备","背","倍","被","辈","奔","本","笨","蹦","逼","鼻","比","彼","笔","鄙",
"币","必","毕","闭","毙","弊","碧","蔽","壁","避","臂","边","编","鞭","扁","便","变","遍","辨","辩","辫","标","表","别","宾","滨","冰","兵","丙","柄","饼","并","病","拨","波","玻","剥","脖","菠","播","伯",
"驳","泊","博","搏","膊","薄","卜","补","捕","不","布","步","怖","部","擦","猜","才","材","财","裁","采","彩","睬","踩","菜","参","餐","残","蚕","惭","惨","灿","仓","苍","舱","藏","操","槽","草","册","侧",
"厕","测","策","层","叉","插","查","茶","察","岔","差","拆","柴","馋","缠","产","铲","颤","昌","长","肠","尝","偿","常","厂","场","敞","畅","倡","唱","抄","钞","超","朝","潮","吵","炒","车","扯","彻","撤",
"尘","臣","沉","辰","陈","晨","闯","衬","称","趁","撑","成","呈","承","诚","城","乘","惩","程","秤","吃","驰","迟","持","虫","崇","抽","仇","绸","愁","稠","筹","酬","丑","臭","出","初","除","厨","锄","础",
"储","楚","处","触","畜","川","穿","传","船","喘","串","疮","窗","床","创","吹","炊","垂","锤","春","纯","唇","蠢","词","慈","辞","磁","此","次","刺","从","匆","葱","聪","丛","凑","粗","促","醋","窜","催",
"摧","脆","翠","村","存","寸","错","曾","搭","达","答","打","大","呆","代","带","待","怠","贷","袋","逮","戴","丹","单","担","耽","胆","旦","但","诞","弹","淡","蛋","当","挡","党","荡","档","刀","叨","导",
"岛","倒","蹈","到","悼","盗","道","稻","得","德","的","灯","登","等","凳","低","堤","滴","敌","笛","底","抵","地","弟","帝","递","第","颠","典","点","电","店","垫","殿","叼","雕","吊","钓","调","掉","爹",
"跌","叠","蝶","丁","叮","盯","钉","顶","订","定","丢","东","冬","董","懂","动","冻","栋","洞","都","斗","抖","陡","豆","逗","督","毒","读","独","堵","赌","杜","肚","度","渡","端","短","段","断","缎","锻",
"堆","队","对","吨","蹲","盾","顿","多","夺","朵","躲","惰","鹅","蛾","额","恶","饿","恩","儿","而","耳","二","发","乏","伐","罚","阀","法","帆","番","翻","凡","烦","繁","反","返","犯","泛","饭","范","贩",
"方","坊","芳","防","妨","房","仿","访","纺","放","飞","非","肥","匪","废","沸","肺","费","分","吩","纷","芬","坟","粉","份","奋","愤","粪","丰","风","封","疯","峰","锋","蜂","逢","缝","讽","凤","奉","佛",
"否","夫","肤","伏","扶","服","俘","浮","符","幅","福","抚","府","斧","俯","辅","腐","父","付","妇","负","附","咐","复","赴","副","傅","富","腹","覆","该","改","盖","溉","概","干","甘","杆","肝","竿","秆",
"赶","敢","感","冈","刚","岗","纲","缸","钢","港","杠","高","膏","糕","搞","稿","告","哥","胳","鸽","割","搁","歌","阁","革","格","葛","隔","个","各","给","根","跟","更","耕","工","弓","公","功","攻","供",
"宫","恭","躬","巩","共","贡","勾","沟","钩","狗","构","购","够","估","姑","孤","辜","古","谷","股","骨","鼓","固","故","顾","瓜","刮","挂","乖","拐","怪","关","观","官","冠","馆","管","贯","惯","灌","罐",
"光","广","归","龟","规","轨","鬼","柜","贵","桂","跪","滚","棍","锅","国","果","裹","过","哈","孩","海","害","含","寒","喊","汉","汗","旱","航","毫","豪","好","号","浩","耗","喝","禾","合","何","和","河",
"核","荷","盒","贺","黑","痕","很","狠","恨","恒","横","衡","轰","哄","烘","红","宏","洪","虹","喉","猴","吼","后","厚","候","乎","呼","忽","狐","胡","壶","湖","糊","蝴","虎","互","户","护","花","华","哗",
"滑","猾","化","划","画","话","怀","槐","坏","欢","还","环","缓","幻","唤","换","患","荒","慌","皇","黄","煌","晃","谎","灰","恢","挥","辉","回","悔","汇","会","绘","贿","惠","毁","慧","昏","婚","浑","魂",
"混","活","火","伙","或","货","获","祸","惑","击","饥","圾","机","肌","鸡","迹","积","基","绩","激","及","吉","级","即","极","急","疾","集","籍","几","己","挤","脊","计","记","纪","忌","技","际","剂","季",
"既","济","继","寄","加","夹","佳","家","嘉","甲","价","驾","架","假","嫁","稼","奸","尖","坚","歼","间","肩","艰","兼","监","煎","拣","俭","茧","捡","减","剪","检","简","见","件","建","剑","荐","贱","健",
"舰","渐","践","鉴","键","箭","江","姜","将","浆","僵","疆","讲","奖","桨","匠","降","酱","交","郊","娇","浇","骄","胶","椒","焦","蕉","角","狡","绞","饺","脚","搅","缴","叫","轿","较","教","阶","皆","接",
"揭","街","节","劫","杰","洁","结","捷","截","竭","姐","解","介","戒","届","界","借","巾","今","斤","金","津","筋","仅","紧","谨","锦","尽","劲","近","进","晋","浸","禁","京","经","茎","惊","晶","睛","精",
"井","颈","景","警","净","径","竞","竟","敬","境","静","镜","纠","究","揪","九","久","酒","旧","救","就","舅","居","拘","鞠","局","菊","橘","举","矩","句","巨","拒","具","俱","剧","惧","据","距","锯","聚",
"捐","卷","倦","绢","决","绝","觉","掘","嚼","军","君","均","菌","俊","卡","开","凯","慨","刊","堪","砍","看","康","糠","扛","抗","炕","考","烤","靠","科","棵","颗","壳","咳","可","渴","克","刻","客","课",
"肯","垦","恳","坑","空","孔","恐","控","口","扣","寇","枯","哭","苦","库","裤","酷","夸","垮","挎","跨","块","快","宽","款","筐","狂","况","旷","矿","框","亏","葵","愧","昆","捆","困","扩","括","阔","垃",
"拉","啦","喇","腊","蜡","辣","来","赖","兰","拦","栏","蓝","篮","览","懒","烂","滥","郎","狼","廊","朗","浪","捞","劳","牢","老","姥","涝","乐","勒","雷","垒","泪","类","僚","了","料","列","劣","烈","猎",
"裂","邻","林","累","冷","厘","梨","狸","离","犁","鹂","璃","黎","礼","李","里","理","力","历","厉","立","丽","利","励","例","隶","栗","粒","俩","连","帘","怜","莲","联","廉","镰","脸","练","炼","恋","链",
"良","凉","梁","粮","粱","两","亮","谅","辆","量","辽","疗","临","淋","伶","灵","岭","铃","陵","零","龄","领","令","另","溜","刘","流","留","榴","柳","六","龙","笼","聋","隆","垄","拢","楼","搂","漏","露",
"芦","炉","虏","鲁","陆","录","鹿","滤","碌","路","驴","轮","论","罗","萝","锣","箩","骡","螺","络","骆","旅","屡","律","虑","率","绿","卵","乱","掠","略","落","妈","麻","马","码","蚂","骂","吗","埋","买",
"迈","麦","卖","脉","蛮","馒","瞒","满","慢","漫","忙","芒","盲","茫","猫","毛","矛","茅","茂","冒","贸","帽","貌","么","没","眉","梅","煤","霉","每","美","妹","门","闷","们","萌","盟","猛","蒙","孟","梦",
"迷","谜","米","眯","秘","密","蜜","眠","绵","棉","免","勉","面","苗","描","秒","妙","庙","灭","蔑","民","敏","名","明","鸣","命","摸","模","膜","摩","磨","魔","抹","末","沫","莫","漠","墨","默","谋","某",
"母","亩","木","目","牧","墓","幕","慕","暮","拿","哪","内","那","纳","乃","奶","耐","男","南","难","囊","挠","恼","脑","闹","呢","嫩","能","尼","泥","你","逆","年","念","娘","酿","鸟","尿","捏","您","宁",
"凝","牛","扭","纽","农","浓","弄","奴","努","怒","女","暖","挪","欧","偶","辟","趴","爬","怕","拍","牌","派","攀","盘","判","叛","盼","乓","旁","胖","抛","炮","袍","跑","泡","陪","培","赔","佩","配","喷",
"盆","朋","棚","蓬","膨","捧","碰","批","披","劈","皮","疲","脾","匹","僻","片","偏","篇","骗","漂","飘","票","撇","拼","贫","品","乒","平","评","凭","苹","瓶","萍","坡","泼","婆","迫","破","魄","剖","仆",
"扑","铺","葡","朴","普","谱","七","妻","戚","期","欺","漆","齐","其","奇","骑","棋","旗","乞","企","岂","启","起","气","弃","汽","砌","器","恰","洽","千","迁","牵","铅","谦","签","前","钱","钳","潜","浅",
"遣","欠","歉","枪","腔","强","墙","抢","悄","敲","锹","乔","侨","桥","瞧","巧","切","茄","且","窃","亲","侵","芹","琴","禽","勤","青","轻","倾","清","蜻","情","晴","顷","请","庆","穷","丘","秋","求","球",
"区","曲","驱","屈","趋","渠","取","去","趣","圈","全","权","泉","拳","犬","劝","券","缺","却","雀","确","鹊","裙","群","然","燃","染","嚷","壤","让","饶","扰","绕","惹","热","人","仁","忍","刃","认","任",
"扔","仍","日","绒","荣","容","熔","融","柔","揉","肉","如","乳","辱","入","软","锐","瑞","润","若","弱","撒","洒","塞","赛","三","伞","散","桑","嗓","丧","扫","嫂","色","森","杀","沙","纱","傻","筛","晒",
"山","删","衫","闪","陕","扇","善","伤","商","裳","晌","赏","上","尚","捎","梢","烧","稍","勺","少","绍","哨","舌","蛇","舍","设","社","射","涉","摄","申","伸","身","深","神","沈","审","婶","肾","甚","渗",
"慎","升","生","声","牲","胜","绳","省","圣","盛","剩","尸","失","师","诗","施","狮","湿","十","什","石","时","识","实","拾","蚀","食","史","使","始","驶","士","氏","世","市","示","式","事","侍","势","视",
"试","饰","室","是","柿","适","逝","释","誓","池","匙","尺","齿","耻","斥","赤","翅","充","冲","收","手","守","首","寿","受","兽","售","授","瘦","书","叔","殊","梳","疏","舒","输","蔬","熟","暑","鼠","属",
"薯","术","束","述","树","竖","数","刷","耍","衰","摔","甩","帅","拴","双","霜","爽","谁","水","税","睡","顺","说","嗽","丝","司","私","思","斯","撕","死","四","寺","似","饲","肆","松","宋","诵","送","颂",
"搜","艘","苏","俗","诉","肃","素","穗","孙","损","笋","缩","所","索","锁","她","他","它","塌","塔","踏","台","抬","太","态","泰","贪","摊","滩","坛","谈","痰","坦","毯","叹","炭","探","汤","唐","堂","塘",
"膛","糖","倘","躺","烫","趟","涛","掏","滔","逃","桃","陶","淘","萄","讨","套","特","疼","腾","梯","踢","提","题","蹄","体","剃","惕","替","天","添","田","甜","填","挑","条","跳","贴","铁","帖","厅","听",
"亭","庭","停","挺","艇","通","同","桐","铜","童","统","桶","筒","痛","偷","头","投","透","秃","突","图","徒","涂","途","屠","土","吐","兔","团","推","腿","退","吞","屯","托","拖","脱","驼","妥","娃","挖",
"蛙","瓦","袜","歪","外","弯","湾","丸","完","玩","顽","挽","晚","碗","万","汪","亡","王","网","往","妄","忘","旺","望","危","威","微","为","围","违","唯","维","伟","伪","尾","委","卫","未","位","味","畏",
"胃","喂","慰","温","文","纹","闻","蚊","稳","问","翁","窝","我","沃","卧","握","乌","污","呜","屋","无","吴","五","午","伍","武","侮","舞","勿","务","物","误","悟","雾","夕","西","吸","希","析","息","牺",
"悉","惜","稀","溪","锡","熄","膝","习","席","袭","洗","喜","戏","系","细","隙","虾","瞎","峡","狭","霞","下","吓","夏","厦","仙","先","纤","掀","鲜","闲","弦","贤","咸","衔","嫌","显","险","县","现","线",
"限","宪","陷","馅","羡","献","乡","相","香","箱","详","祥","享","响","想","向","巷","项","象","像","橡","削","宵","消","销","小","晓","孝","效","校","笑","些","歇","协","邪","胁","斜","携","鞋","写","泄",
"泻","卸","屑","械","谢","心","辛","欣","新","薪","信","兴","星","腥","刑","行","形","型","醒","杏","姓","幸","性","凶","兄","胸","雄","熊","休","修","羞","朽","秀","绣","袖","锈","须","虚","需","徐","速",
"宿","塑","酸","蒜","算","虽","随","岁","碎","许","序","叙","绪","续","絮","蓄","宣","悬","旋","选","穴","学","雪","血","寻","巡","旬","询","循","训","讯","迅","压","呀","押","鸦","鸭","牙","芽","崖","哑",
"雅","亚","咽","烟","淹","延","严","言","岩","沿","炎","研","盐","蜒","颜","掩","眼","演","厌","宴","艳","验","焰","雁","燕","央","殃","秧","扬","羊","阳","杨","洋","仰","养","氧","痒","样","妖","腰","邀",
"窑","谣","摇","遥","咬","药","要","耀","爷","也","冶","野","业","叶","页","夜","液","一","衣","医","依","仪","宜","姨","移","遗","疑","乙","已","以","蚁","倚","椅","义","亿","忆","艺","议","亦","异","役",
"译","易","疫","益","谊","意","毅","翼","因","阴","姻","音","银","引","饮","隐","印","应","英","樱","鹰","迎","盈","营","蝇","赢","影","映","硬","佣","拥","庸","永","咏","泳","勇","涌","用","优","忧","悠",
"尤","由","犹","邮","油","游","友","有","又","右","幼","诱","于","予","余","鱼","娱","渔","愉","愚","榆","与","宇","屿","羽","雨","语","玉","育","郁","狱","浴","预","域","欲","御","裕","遇","愈","誉","冤",
"元","员","园","原","圆","援","缘","源","远","怨","院","愿","约","月","钥","悦","阅","跃","越","云","匀","允","孕","运","晕","韵","杂","灾","栽","宰","载","再","在","咱","暂","赞","脏","葬","遭","糟","早",
"枣","澡","灶","皂","造","燥","躁","则","择","泽","责","贼","怎","增","赠","渣","扎","轧","闸","眨","炸","榨","摘","宅","窄","债","寨","沾","粘","斩","展","盏","崭","占","战","站","张","章","涨","掌","丈",
"仗","帐","胀","障","招","找","召","兆","赵","照","罩","遮","折","哲","者","这","浙","贞","针","侦","珍","真","诊","枕","阵","振","镇","震","争","征","挣","睁","筝","蒸","整","正","证","郑","政","症","之",
"支","汁","芝","枝","知","织","肢","脂","蜘","执","侄","直","值","职","植","殖","止","只","旨","址","纸","指","至","志","制","帜","治","质","秩","致","智","置","中","忠","终","钟","肿","种","众","重","州",
"舟","周","洲","粥","宙","昼","皱","骤","朱","株","珠","诸","猪","蛛","竹","烛","逐","主","煮","嘱","住","助","注","驻","柱","祝","著","筑","铸","抓","爪","专","砖","转","赚","庄","装","壮","状","撞","追",
"准","捉","桌","浊","啄","着","仔","姿","资","滋","子","紫","字","自","宗","棕","踪","总","纵","走","奏","租","足","族","阻","组","祖","钻","嘴","最","罪","醉","尊","遵","昨","左","作","坐","座"};

int PET_DEBUG_initPetOne( int charaindex)
{
    Char    ch;
    int     havepetindex;
    int     index;
    /* 矢永玄毛  化月井譬屯月 */
    havepetindex = CHAR_getCharPetElement( charaindex) ;

    memset( &ch, 0, sizeof( ch));
    if( !CHAR_getDefaultChar( &ch,31010 ) )return -1;

    /*    飓  寞    */
    ch.data[CHAR_BASEBASEIMAGENUMBER]
        = ch.data[CHAR_BASEIMAGENUMBER] = 30008;
    ch.data[CHAR_WHICHTYPE] = CHAR_TYPEPET;
    /*    猾   */
    ch.workint[CHAR_WORKATTACKPOWER] = 100;
    /*  潮     */
    ch.workint[CHAR_WORKDEFENCEPOWER] = 50;
    /*  HP */
    ch.data[CHAR_HP] = 100;
    /*    蟆 */
    strcpysafe( ch.string[CHAR_NAME].string, 32, "宠物１" );

    /* CHAR卞喃曰癫化月 */
    index = PET_initCharOneArray( &ch);

    if( index < 0 ) return -1;

    /* 仍潜谛本永玄 */
    CHAR_setWorkInt( index, CHAR_WORKPLAYERINDEX, charaindex);
    CHAR_setWorkInt( index,CHAR_WORKOBJINDEX,-1);
    CHAR_setCharPet( charaindex, havepetindex, index);
    CHAR_setInt( index, CHAR_SLOT, 2);
    return havepetindex;
}

static int _PET_dropPet( int charaindex, int havepetindex, int tofl, int tox, int toy)
{
    char szPet[128];
    int dirx[9],diry[9];
    int i, j;
    int objindex=-1;
    int floor,x,y;
    int petindex;
    int count_chara =0, count_item =0;

	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return FALSE;
    petindex = CHAR_getCharPet(charaindex,havepetindex);
    if( !CHAR_CHECKINDEX( petindex )) return FALSE;
    if( !CHAR_CHECKINDEX( charaindex ) )return FALSE;
    if( CHAR_CHECKINDEX( petindex) == FALSE ) return FALSE;

#ifdef _AVID_TRADETRYBUG //丢出宠物
	if( CHAR_getWorkInt( charaindex, CHAR_WORKTRADEMODE) == CHAR_TRADE_TRADING ){
		CHAR_talkToCli( charaindex, -1, "交易状态中无法丢出宠物。", CHAR_COLORYELLOW );
		return FALSE;
	}
#endif
    // CoolFish: Family 2001/6/13
    if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){
    	CHAR_talkToCli(charaindex, -1, "家族守护兽无法丢出！", CHAR_COLORYELLOW);
    	return	FALSE;
    }
    if (CHAR_getInt(charaindex, CHAR_RIDEPET) == havepetindex ){
    	CHAR_talkToCli(charaindex, -1, "骑乘中的宠物无法丢出！", CHAR_COLORYELLOW);
    	return	FALSE;
    }
    
    if( strlen(CHAR_getChar(petindex,CHAR_USERPETNAME))>0 && strstr(CHAR_getChar(petindex,CHAR_USERPETNAME),"*")!=NULL ){
    	CHAR_talkToCli(charaindex, -1, "宠物自定义名称中含有(*)，无法丢弃、卖店、交易、摆摊，请修改后操作。", CHAR_COLORYELLOW);
    	return	FALSE;
    }

#ifdef _DROPCHECK2
	
	floor = CHAR_getInt( charaindex, CHAR_FLOOR);
	x = CHAR_getInt( charaindex, CHAR_X);
	y = CHAR_getInt( charaindex, CHAR_Y);
	
	for( i = x-CHAR_DEFAULTSEESIZ/2 ; i <= x+CHAR_DEFAULTSEESIZ/2 ; i++ ){
		for( j = y-CHAR_DEFAULTSEESIZ/2 ; j <= y+CHAR_DEFAULTSEESIZ/2 ; j ++ ){
		OBJECT  object;
		for( object = MAP_getTopObj(floor,i,j); object ; object = NEXT_OBJECT(object ) ) {
			int objindex = GET_OBJINDEX(object);
			if( OBJECT_getType(objindex) == OBJTYPE_NOUSE ) continue;

			if( OBJECT_getType(objindex) == OBJTYPE_CHARA ) {
				count_chara++;
			}
			if( OBJECT_getType(objindex) == OBJTYPE_ITEM || OBJECT_getType(objindex) == OBJTYPE_GOLD ) {
				count_item++;
			}
		}
	}
		if( count_item > 80 || count_chara > 80 ) {
			CHAR_talkToCli( charaindex, -1, "这里已经太拥挤了，不能再丢了。", CHAR_COLORYELLOW );
			return FALSE;
		}
	}

#endif
	if( tofl == -1 ) {
	    for( i  = 0 ; i < 7 ; i  ++ ){
	        dirx[i+2] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
	        diry[i+2] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
	    }
	    dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
	    diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
	    dirx[1] = 0;
	    diry[1] = 0;
	    floor = CHAR_getInt( charaindex,CHAR_FLOOR );
	    for( i = 0 ; i < 9 ; i ++ ){
	        int x=CHAR_getInt(charaindex,CHAR_X)+dirx[i];
	        int y=CHAR_getInt(charaindex,CHAR_Y)+diry[i];
	        if( PET_isPutPoint( floor, x, y ) == TRUE ) {
	            break;
	        }
	    }
	    if( i == 9 ) i = 1;
	    x=CHAR_getInt(charaindex,CHAR_X)+dirx[i];
	    y=CHAR_getInt(charaindex,CHAR_Y)+diry[i];
	}else {
    	if( MAP_walkAbleFromPoint( tofl,tox,toy, FALSE ) == FALSE ) {
			print( "map walkable err %s:%d\n", __FILE__,__LINE__);
			return FALSE;
		}
		floor = tofl;
		x = tox;
		y = toy;
	}
    objindex = PET_dropPetAbsolute( petindex,floor,x,y, FALSE );
    if( objindex == -1 ) return FALSE;

    CHAR_setWorkInt( petindex,CHAR_WORKOBJINDEX,objindex );
    CHAR_setCharPet( charaindex, havepetindex, -1);
	CHAR_setInt( petindex, CHAR_FLOOR, floor);
	CHAR_setInt( petindex, CHAR_X, x);
	CHAR_setInt( petindex, CHAR_Y, y);
	CHAR_setInt( petindex, CHAR_PUTPETTIME, NowTime.tv_sec);
	if( havepetindex == CHAR_getInt( charaindex, CHAR_DEFAULTPET)) {
		int	fd;
		CHAR_setInt( charaindex, CHAR_DEFAULTPET, -1);
	    fd = getfdFromCharaIndex( charaindex);
		lssproto_KS_send( fd, havepetindex, 0);

	}
	CHAR_sendCToArroundCharacter( objindex);

    if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
#ifdef _PET_ITEM
			CHAR_sendPetItemEmpty(charaindex,havepetindex);
#endif
		snprintf( szPet, sizeof( szPet ), "K%d", havepetindex );
		CHAR_sendStatusString( charaindex, szPet );
    }
    return TRUE;
}


/*------------------------------------------------------------
 *     泫  矢永玄毛  仁
 * 娄醒
 *  itemindex       int         失奶  丞奶件犯永弁旦
 *  floor           int         白夫失ID
 *  x               int         x甄  
 *  y               int         y甄  
 *  net             BOOL        生永玄伐□弁及仇午毛允月井升丹井
 * 忒曰袄
 *  岳      objindex
 *  撩      -1
 ------------------------------------------------------------*/
int PET_dropPetAbsolute( int petindex, int floor, int x, int y,BOOL net)
{
    Object object;
    int objindex;

    if( !CHAR_CHECKINDEX(petindex) )return FALSE;

    object.type = OBJTYPE_CHARA;
    object.index = petindex;
    object.x = x;
    object.y = y;
    object.floor = floor;

    /*  左皮斥尼弁玄瓒  允月    */
    objindex = initObjectOne( &object );

    /* 生永玄伐□弁白仿弘互凶匀化中月午五反允月  by ringo*/
    if( net )
        CHAR_sendWatchEvent( objindex,CHAR_ACTSTAND,NULL,0,TRUE);

    return objindex;
}

BOOL PET_isPutPoint( int fl,int x, int y)
{
    OBJECT  object;
    if( MAP_walkAbleFromPoint( fl,x,y, FALSE ) == FALSE )
        return FALSE;
    for( object=MAP_getTopObj(fl,x,y) ;
         object ;
         object = NEXT_OBJECT(object ) )
    {
        int objindex = GET_OBJINDEX(object);
        switch( OBJECT_getType(objindex) ){
          case OBJTYPE_NOUSE:
            break;
          case OBJTYPE_ITEM:
          case OBJTYPE_GOLD:
          case OBJTYPE_CHARA:
            return FALSE;
            break;
          default:
            break;
        }
    }
    return TRUE;
}

int PET_dropPet( int charaindex, int havepetindex)
{
	if( CHAR_getWorkInt( charaindex, CHAR_WORKSAFEMODE)==0)
		{
			CHAR_SafePassword(charaindex);
			return;
		}
	int petindex;
	petindex = CHAR_getCharPet(charaindex,havepetindex);
    if( !CHAR_CHECKINDEX( petindex )) return FALSE;
#ifdef _PET_AMOI__E
		//if( PET_NOT_DropTradeStreetVandor == EVOLUTION_getPetENABLETRADE( CHAR_getInt( petindex, CHAR_PETID)) ||
		//	PET_NOT_Drop == EVOLUTION_getPetENABLETRADE( CHAR_getInt( petindex, CHAR_PETID)) ){
		if( (PET_NOT_DROP) & EVOLUTION_getPetENABLETRADE( CHAR_getInt( petindex, CHAR_PETID))){
			char buf[256];
			sprintf( buf, "%s无法丢弃!~",CHAR_getChar( petindex, CHAR_NAME) );
			CHAR_talkToCli(charaindex,-1,buf,CHAR_COLORRED);
			return FALSE;
		}
#endif    	
#ifdef	_PETSKILL_BINDING
	int i,skillid=-1,skillarray;
	for(i=0;i<7;i++){
		skillid=CHAR_getPetSkill(petindex,i);
		skillarray = PETSKILL_getPetskillArray( skillid);
		if( PETSKILL_CHECKINDEX( skillarray) == FALSE ) continue;
		if(strstr(PETSKILL_getChar( skillarray, PETSKILL_NAME),"★")){
			CHAR_talkToCli(charaindex, -1, "此宠物为绑定宠物，无法丢弃", CHAR_COLORRED);
			return;
		}
	}
#endif
#ifdef	_PET_BINDING
				if(strstr(CHAR_getChar(petindex,CHAR_NAME),"*") || strstr(CHAR_getChar(petindex,CHAR_NAME),"+")){
					CHAR_talkToCli(charaindex, -1, "此宠物为绑定宠物，无法丢弃", CHAR_COLORRED);
					return;
				}
#endif
#ifdef	_MM_NO_JIAOBEN
	if(getNoMMJiaoben()==1){
		if(CHAR_getInt(petindex,CHAR_PETID)==718 || CHAR_getInt( petindex, CHAR_PETID) == 401){
			if(getMmType()==0){
				CHAR_talkToCli(charaindex, -1, getMmMsg(), CHAR_COLORRED);
				return;
			}
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			timep = mktime(p);
			char *randtypetime = CHAR_getChar(charaindex,CHAR_RANDTYPE);
			char tempbuff[64];
			int randtype = 0;
			int randtime = 0;
			int randwrong = 0;
			if(strlen(randtypetime)>2){
				if(getStringFromIndexWithDelim(randtypetime,"-", 3, tempbuff, sizeof(tempbuff)) != FALSE){
					if(getStringFromIndexWithDelim(randtypetime,"-", 1, tempbuff, sizeof(tempbuff))== FALSE)
						return 0;
					randwrong = atoi(tempbuff);
					if(getStringFromIndexWithDelim(randtypetime,"-", 2, tempbuff, sizeof(tempbuff))== FALSE)
						return 0;
					randtype = atoi(tempbuff);
					if(getStringFromIndexWithDelim(randtypetime,"-", 3, tempbuff, sizeof(tempbuff))== FALSE)
						return 0;
					randtime = atoi(tempbuff);
					if(timep - randtime < getEditBaseTime()*randwrong){
						char errbuf[256];
						sprintf(errbuf,"您还有%d秒才可以继续此操作！",getEditBaseTime()*randwrong-(timep - randtime));
						CHAR_talkToCli(charaindex, -1, errbuf, CHAR_COLORRED);
						return 0;
					}
				}
			}
//			if(timep-CHAR_getWorkInt(charaindex,CHAR_WORKRANDEDITBASETIME)>=getEditBaseTime())
//				CHAR_setWorkInt(charaindex,CHAR_WORKRANDEDITBASETIME,timep);
//			else{
//				char errbuf[256];
//				sprintf(errbuf,"您还有%d秒才可以继续此操作！",getEditBaseTime()-(timep-CHAR_getWorkInt(charaindex,CHAR_WORKRANDEDITBASETIME)));
//				CHAR_talkToCli(charaindex, -1, errbuf, CHAR_COLORRED);
//				return;
//			}
			int fd = getfdFromCharaIndex( charaindex);
			char arg[255];
			char buf[128];
			char jiabuf[][5]={"加","╋","╉","╈","╂","十","╃"};
			char jianbuf[][5]={"减","一","-","—","-","—","━"};
			char chengbuf[][5]={"乘","*","x","X","Ｘ","ｘ","＊"};
			char numbuf[][5]={"０","１","２","３","４","５","６","７","８","９"};
			int i,j,k,l,m;
			i = RAND(30,70);
			j = RAND(1,29);
			l = RAND(1,7);
			m = RAND(1,3);
			char *typebuf;
//			if(randtype == 0){
//				randtype = RAND(1,10);
//				if(randtype<=5){
//					randtype = RAND(1,10);
//					if(randtype<=3) randtype=1;
//					else randtype=2;
//				}else{
//					randtype = RAND(1,10);
//					if(randtype<=3) randtype=3;
//					else randtype=4;
//				}
//			}
			randtype = 4;
			if(randtype==1){
				if(m==1){
					typebuf=jiabuf[l-1];
					k = i+j;
				}else{
					typebuf=jianbuf[l-1];
					k = i-j;
				}
				char ibuf[10],jbuf[10];
				if(i<10){
					sprintf(ibuf,"%s",numbuf[i]);
				}else{
					int ia,ib;
					ia = i/10;
					ib = i - ia*10;
					sprintf(ibuf,"%s",numbuf[ia]);
					strcat(ibuf,numbuf[ib]);
				}
				if(j<10){
					sprintf(jbuf,"%s",numbuf[j]);
				}else{
					int ja,jb;
					ja = j/10;
					jb = j - ja*10;
					sprintf(jbuf,"%s",numbuf[ja]);
					strcat(jbuf,numbuf[jb]);
				}
				sprintf(arg,"%d|%d",havepetindex,k);
				CHAR_setWorkChar(charaindex, CHAR_WORKDROPMM, arg);
				sprintf(buf,"请输入这个算术公式的结果(%s%s%s=?)：",ibuf,typebuf,jbuf);
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_RAND1_DROPMM,
					-1,
					buf);
			}else if(randtype==2){
				if(m==1){
					typebuf=jiabuf[l-1];
					k = i+j;
				}else if(m==2){
					typebuf=jianbuf[l-1];
					k = i-j;
				}else{
					typebuf=chengbuf[l-1];
					k = i*j;
				}
				char ibuf[10],jbuf[10];
				if(i<10){
					sprintf(ibuf,"%s",numbuf[i]);
				}else{
					int ia,ib;
					ia = i/10;
					ib = i - ia*10;
					sprintf(ibuf,"%s",numbuf[ia]);
					strcat(ibuf,numbuf[ib]);
				}
				if(j<10){
					sprintf(jbuf,"%s",numbuf[j]);
				}else{
					int ja,jb;
					ja = j/10;
					jb = j - ja*10;
					sprintf(jbuf,"%s",numbuf[ja]);
					strcat(jbuf,numbuf[jb]);
				}
				int n;
				int randnum1;
				char tempret[5][32];
				randnum1 = RAND(1,5);
				int randnumbuf[4];
				int tmpi=0;
				while(tmpi<100){
					randnumbuf[0] = RAND(1,100);
					randnumbuf[1] = RAND(1,100);
					randnumbuf[2] = RAND(1,100);
					randnumbuf[3] = RAND(1,100);
					if(randnumbuf[0]!=randnumbuf[1]!=randnumbuf[2]!=randnumbuf[3]!=k){
						break;
					}
					tmpi++;
				}
				tmpi=0;
				for(n=1;n<=5;n++){
					if(n==randnum1){
						sprintf(tempret[n-1],"%d",k);
					}else if(tmpi<4){
						sprintf(tempret[n-1],"%d",randnumbuf[tmpi]);
						tmpi++;
					}
				}
				sprintf(arg,"%d|%d",havepetindex,randnum1);
				CHAR_setWorkChar(charaindex, CHAR_WORKDROPMM, arg);
				sprintf(buf,"3\n请选择这个算术公式的结果(%s%s%s=?)：\n \n \n选项一：%s\n选项二：%s\n选项三：%s\n选项四：%s\n选项五：%s\n",ibuf,typebuf,jbuf,tempret[0],tempret[1],tempret[2],tempret[3],tempret[4]);
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT, 
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_RAND2_DROPMM,
					-1,
					buf);
			}else if(randtype==3){
				sprintf(arg,"%s",sasql_rand_buf());
				if(strcmp(arg,"err")==0) return;
				char tempbuf[64];
				char randquestion[64];
				char randrightanswer[64];
				if(getStringFromIndexWithDelim(arg,"|", 1, tempbuf, sizeof(tempbuf)) == FALSE)
					return;
				strcpy(randquestion,tempbuf);
				if(getStringFromIndexWithDelim(arg,"|", 2, tempbuf, sizeof(tempbuf)) == FALSE)
					return;
				strcpy(randrightanswer,tempbuf);
				sprintf(arg,"%d|%s",havepetindex,randrightanswer);
				CHAR_setWorkChar(charaindex, CHAR_WORKDROPMM, arg);
				sprintf(buf,"%s:",randquestion);
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_RAND3_DROPMM,
					-1,
					buf);
			}else{
//				sprintf(arg,"%s",sasql_rand_buf());
//				if(strcmp(arg,"err")==0) return;
				char tempbuf[64];
				char randquestion[64];
				char randrightanswer[64];
				char randwronganswer1[64];
				char randwronganswer2[64];
				char randwronganswer3[64];
				char randwronganswer4[64];
//				if(getStringFromIndexWithDelim(arg,"|", 1, tempbuf, sizeof(tempbuf)) == FALSE)
//					return;
//				strcpy(randquestion,tempbuf);
//				if(getStringFromIndexWithDelim(arg,"|", 2, tempbuf, sizeof(tempbuf)) == FALSE)
//					return;
//				strcpy(randrightanswer,tempbuf);
//				if(getStringFromIndexWithDelim(arg,"|", 3, tempbuf, sizeof(tempbuf)) == FALSE)
//					return;
//				strcpy(randwronganswer1,tempbuf);
//				if(getStringFromIndexWithDelim(arg,"|", 4, tempbuf, sizeof(tempbuf)) == FALSE)
//					return;
//				strcpy(randwronganswer2,tempbuf);
//				if(getStringFromIndexWithDelim(arg,"|", 5, tempbuf, sizeof(tempbuf)) == FALSE)
//					return;
//				strcpy(randwronganswer3,tempbuf);
//				if(getStringFromIndexWithDelim(arg,"|", 6, tempbuf, sizeof(tempbuf)) == FALSE)
//					return;
//				strcpy(randwronganswer4,tempbuf);
				memset(tempbuf, 0, sizeof(tempbuf));
				memset(randquestion, 0, sizeof(randquestion));
				memset(randrightanswer, 0, sizeof(randrightanswer));
				memset(randwronganswer1, 0, sizeof(randwronganswer1));
				memset(randwronganswer2, 0, sizeof(randwronganswer2));
				memset(randwronganswer3, 0, sizeof(randwronganswer3));
				memset(randwronganswer4, 0, sizeof(randwronganswer4));
				if(RAND(1,100)<=75){
					char randtmpbuf[4][10];
					sprintf(randtmpbuf[0],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randtmpbuf[1],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randtmpbuf[2],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randtmpbuf[3],"%s",hanzibuf[RAND(1,5000)-1]);
					sprintf(randquestion,"%s",str_hanzi_ganrao(randtmpbuf[0],randtmpbuf[1],randtmpbuf[2],randtmpbuf[3],0,0,0,0,0,0));
					strcat(randquestion,"（请找出相同词组）");
					int newrand1 = RAND(1,100)-1;
					int newrand2 = RAND(1,100)-1;
					int newrand3 = RAND(1,100)-1;
					int newrand4 = RAND(1,100)-1;
					int newrand5 = RAND(1,100)-1;
					sprintf(randrightanswer,"%s",str_hanzi_ganrao(randtmpbuf[0],randtmpbuf[1],randtmpbuf[2],randtmpbuf[3],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer1,"%s",str_hanzi_ganrao(randtmpbuf[0],randtmpbuf[1],randtmpbuf[3],randtmpbuf[2],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer2,"%s",str_hanzi_ganrao(randtmpbuf[3],randtmpbuf[1],randtmpbuf[2],randtmpbuf[0],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer3,"%s",str_hanzi_ganrao(randtmpbuf[1],randtmpbuf[0],randtmpbuf[2],randtmpbuf[3],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					sprintf(randwronganswer4,"%s",str_hanzi_ganrao(randtmpbuf[2],randtmpbuf[3],randtmpbuf[0],randtmpbuf[1],1,newrand1,newrand2,newrand3,newrand4,newrand5));
					
				}else{
					int tii;
					char timubuf[36][5] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9"};
					for(tii=0;tii<5;tii++){
						if(tii==0)
							strcpy(tempbuf,timubuf[RAND(1,36)-1]);
						else{
							int timunum = 0;
							while(1){
								timunum = RAND(1,36)-1;
								if(strstr(tempbuf,timubuf[timunum])==NULL)
									break;
							}
							strcat(tempbuf,timubuf[timunum]);
						}
					}
					sprintf(randwronganswer1,"[%c%c%c%c%c]",tempbuf[0],tempbuf[2],tempbuf[1],tempbuf[3],tempbuf[4]);
					sprintf(randwronganswer2,"[%c%c%c%c%c]",tempbuf[0],tempbuf[1],tempbuf[4],tempbuf[2],tempbuf[3]);
					sprintf(randwronganswer3,"[%c%c%c%c%c]",tempbuf[4],tempbuf[1],tempbuf[2],tempbuf[0],tempbuf[3]);
					sprintf(randwronganswer4,"[%c%c%c%c%c]",tempbuf[1],tempbuf[0],tempbuf[2],tempbuf[3],tempbuf[4]);
					sprintf(randquestion,"[%s]（请找出相同词组）",tempbuf);
					sprintf(randrightanswer,"[%s]",tempbuf);
					char* randstr = NULL;
					randstr = str_ganrao(randquestion);
					strcpy(randquestion,randstr);
					randstr = str_ganraoan(randrightanswer);
					strcpy(randrightanswer,randstr);
					randstr = str_ganraoan(randwronganswer1);
					strcpy(randwronganswer1,randstr);
					randstr = str_ganraoan(randwronganswer2);
					strcpy(randwronganswer2,randstr);
					randstr = str_ganraoan(randwronganswer3);
					strcpy(randwronganswer3,randstr);
					randstr = str_ganraoan(randwronganswer4);
					strcpy(randwronganswer4,randstr);
				}
				int n;
				int randnum1;
				m=1;
				char tempret[5][64];
				randnum1 = RAND(1,5);
				int rightnum,wrongnum1,wrongnum2,wrongnum3,wrongnum4;
				rightnum = RAND(getRandMin(),getRandMax());
				wrongnum1 = RAND(getRandMin(),getRandMax());
				wrongnum2 = RAND(getRandMin(),getRandMax());
				wrongnum3 = RAND(getRandMin(),getRandMax());
				wrongnum4 = RAND(getRandMin(),getRandMax());
				//char *tempbuff2 = "ABCDE";
				int qianhounum = RAND(1,100);
				for(n=1;n<=5;n++){
					if(n==randnum1){
						if(qianhounum<=50)
							sprintf(tempret[n-1],"选项( %d %s )",rightnum,randrightanswer);
						else
							sprintf(tempret[n-1],"选项( %s %d )",randrightanswer,rightnum);
					}else{
						if(m==1){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum1,randwronganswer1);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer1,wrongnum1);
							m = 2;
						}else if(m==2){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum2,randwronganswer2);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer2,wrongnum2);
							m = 3;
						}else if(m==3){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum3,randwronganswer3);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer3,wrongnum3);
							m = 4;
						}else if(m==4){
							if(qianhounum<=50)
								sprintf(tempret[n-1],"选项( %d %s )",wrongnum4,randwronganswer4);
							else
								sprintf(tempret[n-1],"选项( %s %d )",randwronganswer4,wrongnum4);
							m = 5;
						}
					}
				}
				int windowtype = 0;
				int selecttype = RAND(1,100);
				if(selecttype<=30){
					sprintf(arg,"%d|%d|%d",havepetindex,rightnum);
					windowtype = WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT;
				}else{
					sprintf(arg,"%d|%d|%d",havepetindex,randnum1);
					windowtype = WINDOW_MESSAGETYPE_SELECT;
				}
				CHAR_setWorkChar(charaindex, CHAR_WORKDROPMM, arg);
				sprintf(buf,"%s\n%s\n%s\n%s\n%s\n%s\n输入正确答案括号内的5位数字或者点选答案",randquestion,tempret[0],tempret[1],tempret[2],tempret[3],tempret[4]);
				lssproto_WN_send( fd, windowtype, 
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_RAND4_DROPMM,
					-1,
					buf);
				CHAR_setWorkInt(charaindex,CHAR_WORKLUANPCINT,0);
			}
			if(randwrong<=0){
				sprintf(tempbuff,"1-%d-%d",randtype,timep);
			}else{
				sprintf(tempbuff,"%d-%d-%d",randwrong*3,randtype,timep);
			}
			CHAR_setChar(charaindex,CHAR_RANDTYPE,tempbuff);
			return 0;
		}
}
#endif    
	if( _PET_dropPet( charaindex, havepetindex, -1,-1,-1) == TRUE ){
		char tmpbuf[256];
		sprintf(tmpbuf,"丢弃 %s (宠物消失时间%d秒)。",CHAR_getChar( petindex, CHAR_NAME),getPetdeletetime());
		CHAR_talkToCli( charaindex, -1, tmpbuf, CHAR_COLORYELLOW );
		LogPet(
			CHAR_getChar( charaindex, CHAR_NAME ), /* 平乓仿   */
			CHAR_getChar( charaindex, CHAR_CDKEY ),
			CHAR_getChar( petindex, CHAR_NAME),
			CHAR_getInt( petindex, CHAR_LV),
			"Drop(丢宠)",
			CHAR_getInt( charaindex,CHAR_FLOOR),
			CHAR_getInt( charaindex,CHAR_X ),
			CHAR_getInt( charaindex,CHAR_Y ),
			CHAR_getChar( petindex, CHAR_UNIQUECODE)   // shan 2001/12/14
		);
		return 1;
	}
	return 0;
}

int PET_dropPetFLXY( int charaindex, int havepetindex, int fl, int x, int y)
{
	return _PET_dropPet( charaindex, havepetindex, fl,x,y);
}

/*------------------------------------------------------------
 * 矢永玄迕及奶矛件玄楮醒毛本永玄仄化支月
 ------------------------------------------------------------*/
int PET_initCharOneArray( Char *ch)
{
    int i;
    char *tmp[CHAR_FUNCTABLENUM] = {
        "",                 /*  CHAR_INITFUNC */
        "",     			/*  CHAR_WALKPREFUNC    */
        "",    				/*  CHAR_WALKPOSTFUNC   */
        "",                 /*  CHAR_PREOVERFUNC    */
        "",                 /*  CHAR_PREOVERFUNC    */
        "core_PetWatch", 	/*  CHAR_WATCHFUNC  */
        "",        			/*  CHAR_LOOPFUNC */
        "",       			/*  CHAR_DYINGFUNC */
        "core_PetTalk",		/*  CHAR_TALKEDFUNC */
        "",                 /*  CHAR_PREATTACKEDFUNC    */
        "",                 /*  CHAR_POSTATTACKEDFUNC    */
        "",                 /*  CHAR_OFFFUNC    */
        "",                 /*  CHAR_LOOKEDFUNC */
        "",                 /*  CHAR_ITEMPUTFUNC    */
        "",                 /*  CHAR_SPECIALTALKEDFUNC    */
        "",                 /*  CHAR_WINDOWTALKEDFUNC    */
#ifdef _USER_CHARLOOPS
		"",					// CHAR_LOOPFUNCTEMP1,
		"",					// CHAR_LOOPFUNCTEMP2,
		"",					//CHAR_BATTLEPROPERTY,
#endif
    };
    for( i = 0; i < CHAR_FUNCTABLENUM; i ++ ) {
        strcpysafe( ch->charfunctable[i].string,
                    sizeof( ch->charfunctable[i]),
                    tmp[i]);
    }
	if( ch->data[CHAR_MAILMODE] != CHAR_PETMAIL_NONE ) {
		strcpysafe( ch->charfunctable[CHAR_LOOPFUNC].string,
					sizeof( ch->charfunctable[CHAR_LOOPFUNC]), "PETMAIL_Loop");

	}
#ifdef _USER_CHARLOOPS
	if( ch->data[CHAR_FUSIONBEIT] == 1 &&
		ch->data[CHAR_FUSIONRAISE] > 0 ) {
		//andy_log
//		print("init CHAR_LOOPFUNCTEMP1:%s \n", "PET_CheckIncubateLoop");

		strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
				sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "PET_CheckIncubateLoop");
		ch->data[CHAR_LOOPINTERVAL] = 60000;

		ch->functable[CHAR_LOOPFUNCTEMP1]
            = getFunctionPointerFromName( "PET_CheckIncubateLoop");

//		CHAR_constructFunctable( petindex);
	}
#endif
    return( CHAR_initCharOneArray( ch));
}

int PET_createPetFromCharaIndex( int charaindex, int enemyindex)
{
    Char    CharNew;
    int     newindex;
    int     havepetelement;
	char	szPet[128];
	int		i;

    havepetelement = CHAR_getCharPetElement( charaindex);
    if( havepetelement < 0 ) return -1;
    memset( &CharNew, 0, sizeof( Char ) );
    if( !CHAR_getDefaultChar( &CharNew,31010 ) )return -1;
    CharNew.data[CHAR_BASEBASEIMAGENUMBER]
        = CharNew.data[CHAR_BASEIMAGENUMBER] = CHAR_getInt(enemyindex,CHAR_BASEIMAGENUMBER);
    CharNew.data[CHAR_WHICHTYPE] = CHAR_TYPEPET;
    CharNew.data[CHAR_HP]       = CHAR_getInt(enemyindex, CHAR_HP);
    CharNew.data[CHAR_MP]       = CHAR_getInt(enemyindex, CHAR_MP);
    CharNew.data[CHAR_MAXMP]    = CHAR_getInt(enemyindex, CHAR_MAXMP);
    CharNew.data[CHAR_VITAL]    = CHAR_getInt(enemyindex, CHAR_VITAL);
    CharNew.data[CHAR_STR]      = CHAR_getInt(enemyindex, CHAR_STR);
    CharNew.data[CHAR_TOUGH]    = CHAR_getInt(enemyindex, CHAR_TOUGH);
    CharNew.data[CHAR_DEX]      = CHAR_getInt(enemyindex, CHAR_DEX);
    CharNew.data[CHAR_LUCK]     = CHAR_getInt(enemyindex, CHAR_LUCK);
    CharNew.data[CHAR_FIREAT]   = CHAR_getInt(enemyindex, CHAR_FIREAT);
    CharNew.data[CHAR_WATERAT]  = CHAR_getInt(enemyindex, CHAR_WATERAT);
    CharNew.data[CHAR_EARTHAT]  = CHAR_getInt(enemyindex, CHAR_EARTHAT);
    CharNew.data[CHAR_WINDAT]   = CHAR_getInt(enemyindex, CHAR_WINDAT);
    //CharNew.data[CHAR_EXP]      = CHAR_getInt(enemyindex, CHAR_EXP);

    CharNew.data[CHAR_SLOT]     = CHAR_getInt(enemyindex, CHAR_SLOT);
    CharNew.data[CHAR_MODAI]    = CHAR_getInt(enemyindex, CHAR_MODAI);
    CharNew.data[CHAR_LV]       = CHAR_getInt(enemyindex, CHAR_LV);
    CharNew.data[CHAR_POISON]   = CHAR_getInt(enemyindex, CHAR_POISON);
    CharNew.data[CHAR_PARALYSIS]= CHAR_getInt(enemyindex, CHAR_PARALYSIS);
    CharNew.data[CHAR_SLEEP]	= CHAR_getInt(enemyindex, CHAR_SLEEP);
    CharNew.data[CHAR_STONE]	= CHAR_getInt(enemyindex, CHAR_STONE);
    CharNew.data[CHAR_DRUNK]	= CHAR_getInt(enemyindex, CHAR_DRUNK);
    CharNew.data[CHAR_CONFUSION]= CHAR_getInt(enemyindex, CHAR_CONFUSION);
    CharNew.data[CHAR_RARE]     = CHAR_getInt(enemyindex, CHAR_RARE);
    CharNew.data[CHAR_PETRANK]  = CHAR_getInt(enemyindex, CHAR_PETRANK);
	CharNew.data[CHAR_PETID]	= CHAR_getInt(enemyindex, CHAR_PETID);
	CharNew.data[CHAR_CRITIAL]	= CHAR_getInt(enemyindex, CHAR_CRITIAL);
	CharNew.data[CHAR_COUNTER]	= CHAR_getInt(enemyindex, CHAR_COUNTER);
	CharNew.data[CHAR_PETMAILEFFECT] = RAND(0, PETMAIL_EFFECTMAX); 
	CharNew.data[CHAR_PETENEMYID]	= CHAR_getInt(enemyindex, CHAR_PETENEMYID);

	for( i = 0; i < CHAR_MAXPETSKILLHAVE; i ++ ) {
		CharNew.unionTable.indexOfPetskill[i] = CHAR_getPetSkill( enemyindex, i);
	}
    CharNew.data[CHAR_ALLOCPOINT] = CHAR_getInt(enemyindex, CHAR_ALLOCPOINT);
    strcpysafe( CharNew.string[CHAR_NAME].string,
                sizeof(CharNew.string[CHAR_NAME].string),
                CHAR_getChar( enemyindex, CHAR_NAME) );
    newindex = PET_initCharOneArray( &CharNew );
#ifdef _CAX_LVTISHI       
    if (CharNew.data[CHAR_LV] == 1){//如果捕捉到的宠物是1级
    	int namelen=strlen(CHAR_getChar(newindex, CHAR_NAME));//获得宠物名称的长度
    	char petname[namelen-3];//因为要去掉宝宝这四个字符，所以重新设置名称的话,直接取消掉后面的四个字符
	  snprintf( petname, sizeof( petname),"%s",CHAR_getChar( newindex, CHAR_NAME));
    CHAR_setChar( newindex, CHAR_NAME, petname);
    }
#endif
    if( newindex < 0 ){
        return -1;
    }

	CHAR_setMaxExpFromLevel( newindex, CHAR_getInt( enemyindex, CHAR_LV ));
    CHAR_complianceParameter( newindex );
    CHAR_setWorkInt( newindex, CHAR_WORKPLAYERINDEX, charaindex);
    CHAR_setCharPet( charaindex, havepetelement, newindex);
	CHAR_setChar( newindex, CHAR_OWNERCDKEY,
						CHAR_getChar( charaindex, CHAR_CDKEY));
	CHAR_setChar( newindex, CHAR_OWNERCHARANAME,
						CHAR_getChar( charaindex, CHAR_NAME));
	snprintf( szPet, sizeof( szPet ), "K%d", havepetelement );
	CHAR_sendStatusString( charaindex, szPet );
	snprintf( szPet, sizeof( szPet ), "W%d", havepetelement );
	CHAR_sendStatusString( charaindex, szPet );

    return newindex;

}
BOOL PET_SelectBattleEntryPet( int charaindex, int petarray)
{
	int		pindex;
	/* 爵    反轮仁   仿弘匹仇木卞娄匀井井月第  岭丐曰  */
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return FALSE;
	/* -1及桦宁反］-1卞仄化本永玄仄化蔽歹曰［*/
	if( petarray == -1 ) {
		CHAR_setInt( charaindex, CHAR_DEFAULTPET, -1 );
		return TRUE;
	}
	if( !CHAR_CHECKPETINDEX( petarray)) return FALSE;
	pindex = CHAR_getCharPet( charaindex, petarray );
	if( !CHAR_CHECKINDEX( pindex)) return FALSE;

	if( CHAR_getFlg( pindex, CHAR_ISDIE )) return FALSE;
#ifdef _RIDEBUG
	//判断是否为骑宠
	if(CHAR_getInt( charaindex, CHAR_RIDEPET) == petarray) return FALSE;
#endif
	CHAR_setInt( charaindex, CHAR_DEFAULTPET, petarray );

	return TRUE;
}

// Robin 0707 petFollow <charaindex>
#if 1
int PET_dropPetFollow( int charaindex, int havepetindex, int tofl, int tox, int toy)
{
	char szPet[128];
    int dirx[9],diry[9];
    int i;
    int objindex=-1;
    int floor,x,y;
    int petindex;

	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return FALSE;
    petindex = CHAR_getCharPet(charaindex,havepetindex);
    if( petindex == -1  ) return FALSE;
    if( !CHAR_CHECKINDEX( charaindex ) )return FALSE;
    if( CHAR_CHECKINDEX( petindex) == FALSE ) return FALSE;

	if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1){
     	CHAR_talkToCli(charaindex, -1, "家族守护兽无法丢出！", CHAR_COLORYELLOW);
    	return	FALSE;  
    }
    if (CHAR_getInt(charaindex, CHAR_RIDEPET) == havepetindex){
    	CHAR_talkToCli(charaindex, -1, "骑乘中的宠物无法跟随！", CHAR_COLORYELLOW);
    	return	FALSE;
    }
	if( tofl == -1 ) {
	    for( i  = 0 ; i < 7 ; i  ++ ){
	        dirx[i+2] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
	        diry[i+2] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR) + i+1);
	    }
	    dirx[0] = CHAR_getDX(CHAR_getInt(charaindex,CHAR_DIR));
	    diry[0] = CHAR_getDY(CHAR_getInt(charaindex,CHAR_DIR));
	    dirx[1] = 0;
	    diry[1] = 0;

	    floor = CHAR_getInt( charaindex,CHAR_FLOOR );
	    for( i = 0 ; i < 9 ; i ++ ){
	        int x=CHAR_getInt(charaindex,CHAR_X)+dirx[i];
	        int y=CHAR_getInt(charaindex,CHAR_Y)+diry[i];
	        if( PET_isPutPoint( floor, x, y ) == TRUE ) {
	            break;
	        }
	    }
	    if( i == 9 ) i = 1;

	    x=CHAR_getInt(charaindex,CHAR_X)+dirx[i];
	    y=CHAR_getInt(charaindex,CHAR_Y)+diry[i];
	}
	else {
    	if( MAP_walkAbleFromPoint( tofl,tox,toy, FALSE ) == FALSE ) {
			print( "map walkable err %s:%d\n", __FILE__,__LINE__);
			return FALSE;
		}
		floor = tofl;
		x = tox;
		y = toy;
	}

  objindex = PET_dropPetAbsolute( petindex,floor,x,y, FALSE );
  if( objindex == -1 ) return FALSE;
  
  CHAR_setWorkInt( petindex,CHAR_WORKOBJINDEX,objindex );
  CHAR_setCharPet( charaindex, havepetindex, -1);
	CHAR_setInt( petindex, CHAR_FLOOR, floor);
	CHAR_setInt( petindex, CHAR_X, x);
	CHAR_setInt( petindex, CHAR_Y, y);
	CHAR_setInt( petindex, CHAR_PUTPETTIME, NowTime.tv_sec);
	if( havepetindex == CHAR_getInt( charaindex, CHAR_DEFAULTPET)) {
		int	fd;
		CHAR_setInt( charaindex, CHAR_DEFAULTPET, -1);
	    fd = getfdFromCharaIndex( charaindex);
		lssproto_KS_send( fd, havepetindex, 0);

	}
	CHAR_sendCToArroundCharacter( objindex);
    if( CHAR_getInt( charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
#ifdef _PET_ITEM
			CHAR_sendPetItemData( charaindex, havepetindex);
#endif		
			snprintf( szPet, sizeof( szPet ), "K%d", havepetindex );
			CHAR_sendStatusString( charaindex, szPet );
    }

	CHAR_setWorkInt( charaindex, CHAR_WORKPETFOLLOW, petindex);
	CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWMODE, CHAR_PETFOLLOW_NOW);
	CHAR_setWorkInt( petindex, CHAR_WORKPETFOLLOWCOUNT, 0);
	CHAR_setInt( petindex, CHAR_PUTPETTIME, (int)(NowTime.tv_sec));
	CHAR_setInt( petindex, CHAR_WORKPLAYERINDEX, charaindex);
	LogPet(
		CHAR_getChar( charaindex, CHAR_NAME ),
		CHAR_getChar( charaindex, CHAR_CDKEY ),
		CHAR_getChar( petindex, CHAR_NAME),
		CHAR_getInt( petindex, CHAR_LV),
		"Follow(溜宠)",
		CHAR_getInt( charaindex,CHAR_FLOOR),
		CHAR_getInt( charaindex,CHAR_X ),
		CHAR_getInt( charaindex,CHAR_Y ),
		CHAR_getChar( petindex, CHAR_UNIQUECODE)
	);
    return 1;
}
#endif

BOOL PET_getBaseForAllocpoint( int toindex, int *work)
{
	int LevelUpPoint=0;
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
		return FALSE;
	LevelUpPoint = CHAR_getInt( toindex, CHAR_ALLOCPOINT );
	work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
	work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
	work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
	work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
	return TRUE;
}

void PET_showEditBaseMsg( int charaindex, int toindex, int itemindex, int *work)
{
	int i, maxnums = 6000;
	char buf1[256];
	char buf2[][32]={"腕力成长率","耐久力成长率","速度成长率","体力成长率","能力"};
	char buf3[][32]={"大幅提高","略为提高","略为减少"};
	
	if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPET)
		return;
	memset( buf1, 0, sizeof( buf1));

	for( i=0; i<4; i++)	{
		int type = ITEM_getInt( itemindex, (ITEM_MODIFYATTACK + i));
//		print(" 喂[%d]%d+%d ", i, work[i], type);
		work[i] += type;
		strcpy( buf1,"\0");
		if( work[i] > maxnums )	{
			sprintf(buf1,"%s 已经达到最高了。", buf2[i]);
			work[i] = maxnums;
		}else if( work[i] < 0 )	{
			sprintf(buf1,"%s 已经为零了。", buf2[i]);
			work[i] = 0;
		}else	{
			if( type > 0 )	{
				if( type > 2 )	{
					sprintf(buf1,"%s %s %s", buf2[i], buf3[0], "。");
				}else	{
					sprintf(buf1,"%s %s %s", buf2[i], buf3[1], "。");
				}
			}else if( type < 0 ){
				sprintf(buf1,"%s %s %s", buf2[i], buf3[2], "。");	
			}
		}
		if( strcmp( buf1, "\0"))	{
			CHAR_talkToCli( charaindex, toindex, buf1, CHAR_COLORYELLOW);
		}
	}
}

#ifdef _PET_EVOLUTION
BOOL PET_getBaseAndSkill( int charaindex, int baseindex, int *base, int *skill, int flg)
{
	int i;
	if( !CHAR_CHECKINDEX( baseindex)) return FALSE;
	if( base != NULL )	{
		int levelup = CHAR_getInt( baseindex, CHAR_ALLOCPOINT);
		base[0] = ((levelup>>24) & 0xFF);
		base[1]   = ((levelup>>16) & 0xFF);
		base[2]   = ((levelup>> 8) & 0xFF);
		base[3]   = ((levelup>> 0) & 0xFF);
	}

	if( skill != NULL )	{
		for( i=0; i<CHAR_MAXPETSKILLHAVE; i++)	{
			skill[i] = CHAR_getPetSkill( baseindex, i);
		}
	}

	return TRUE;
}

#endif
BOOL CHAR_DelPetForIndex( int charaindex, int petindex)
{
	int i;
	for( i=0; i<CHAR_MAXPETHAVE; i++)	{
		int pindex = CHAR_getCharPet( charaindex, i);
		if( !CHAR_CHECKINDEX( pindex) ) continue;
		if( pindex == petindex )
			break;
	}

	if( i >= CHAR_MAXPETHAVE){
		return FALSE;
	}else	{
		char szPet[256];
		char msgbuf[256];
		CHAR_setCharPet( charaindex, i, -1);
		snprintf( szPet, sizeof( szPet ), "K%d", i);
		CHAR_sendStatusString( charaindex, szPet );

		snprintf( msgbuf,sizeof( msgbuf), "交出%s。", CHAR_getChar( petindex, CHAR_NAME));
		CHAR_talkToCli( charaindex, -1, msgbuf,  CHAR_COLORYELLOW);
		CHAR_endCharOneArray( petindex );
	}
	return TRUE;
}


