#include "skill.h"

namespace GNET
{
TeamStateImp::Map     TeamStateImp::stubmap;
VisibleStateImp::Map  VisibleStateImp::stubmap;

VisibleStateImp _stub_vstate1(1, L"强化","属性增强.gfx","");
VisibleStateImp _stub_vstate2(2, L"弱化","属性下降.gfx","");
VisibleStateImp _stub_vstate3(3, L"反震","物理反震.gfx","HH_Spine");
VisibleStateImp _stub_vstate4(4, L"迟滞","迟滞.gfx","");
VisibleStateImp _stub_vstate5(5, L"昏迷","震晕.gfx","HH_Head");
VisibleStateImp _stub_vstate6(6, L"睡眠","睡眠.gfx","");
VisibleStateImp _stub_vstate7(7, L"定身","定身.gfx","");
VisibleStateImp _stub_vstate8(8, L"麻痹","封印.gfx","");
VisibleStateImp _stub_vstate9(9, L"受诅","特殊不利状态.gfx","HH_Head");
VisibleStateImp _stub_vstate13(10,L"雷击","持续金系攻击.gfx","HH_Head");
VisibleStateImp _stub_vstate10(11,L"中毒","中毒.gfx","HH_Head");
VisibleStateImp _stub_vstate11(12,L"燃烧","持续火攻击.gfx","HH_Head");
VisibleStateImp _stub_vstate14(13,L"落石","持续土系伤害.gfx","");
VisibleStateImp _stub_vstate12(14,L"放血","放血.gfx","HH_Head");
VisibleStateImp _stub_vstate15(15,L"魔力燃烧","魔力燃烧.gfx","HH_Head");
VisibleStateImp _stub_vstate16(16,L"霜刃","霜刃状态.gfx","HH_RighthandWeapon");
VisibleStateImp _stub_vstate17(17,L"爆气","爆气效果状态.gfx","HH_RighthandWeapon");
VisibleStateImp _stub_vstate18(18,L"附火伤","烈焰之矢.gfx","HH_RighthandWeapon");
VisibleStateImp _stub_vstate19(19,L"鲜花1","康乃馨环绕.gfx","");
VisibleStateImp _stub_vstate20(20,L"鲜花2","百合环绕.gfx","");
VisibleStateImp _stub_vstate21(21,L"鲜花3","菊花环绕.gfx","");
VisibleStateImp _stub_vstate22(22,L"鲜花4","玫瑰环绕.gfx","");
VisibleStateImp _stub_vstate23(23,L"弱魂","弱魂咒状态.gfx","HH_Head");

// 法师特有
VisibleStateImp _stub_vstate24(24,L"烈焰护甲","烈焰护甲.gfx","HH_Spine");
VisibleStateImp _stub_vstate25(25,L"寒冰护甲","寒冰护甲.gfx","HH_Spine");
VisibleStateImp _stub_vstate26(26,L"磐石护甲","磐石护甲.gfx","HH_Spine");
VisibleStateImp _stub_vstate27(27,L"避水诀","避水诀.gfx","HH_Spine");
// 羽灵特有
VisibleStateImp _stub_vstate28(28,L"羽盾","羽盾.gfx","HH_Spine");
// 羽芒特有
VisibleStateImp _stub_vstate29(29,L"翼盾","翼盾.gfx","HH_Spine");
VisibleStateImp _stub_vstate30(30,L"烈焰之矢","烈焰之矢.gfx","HH_LefthandWeapon");
// 妖精特有
VisibleStateImp _stub_vstate31(31,L"荆棘阵","魔法反震.gfx","HH_Spine");
VisibleStateImp _stub_vstate32(32,L"妖狐变","妖狐附体状态.gfx","HH_Spine");
// 怪物特有
VisibleStateImp _stub_vstate33(33,L"目盲","主动变被动.gfx","");

VisibleStateImp _stub_vstate34(34, L"封印免疫","封印免疫.gfx","");
VisibleStateImp _stub_vstate35(35, L"睡眠免疫","睡眠免疫.gfx","");
VisibleStateImp _stub_vstate36(36, L"火系免疫","火系免疫.gfx","HH_Spine");
VisibleStateImp _stub_vstate37(37, L"水系免疫","水系免疫.gfx","HH_Spine");
VisibleStateImp _stub_vstate38(38, L"金系免疫","金系免疫.gfx","HH_Spine");
VisibleStateImp _stub_vstate39(39, L"木系免疫","木系免疫.gfx","HH_Spine");
VisibleStateImp _stub_vstate40(40, L"土系免疫","土系免疫.gfx","HH_Spine");

VisibleStateImp _stub_vstate41(41, L"风盾术","风盾术.gfx","HH_Spine");
VisibleStateImp _stub_vstate42(42, L"气流锁","气流锁.gfx","HH_Spine");
VisibleStateImp _stub_vstate43(43, L"元素虚弱","元素虚弱状态.gfx","");
VisibleStateImp _stub_vstate44(44, L"伤害加深免疫","伤害加深免疫.gfx","");
VisibleStateImp _stub_vstate45(45, L"冻结 ","冻结.gfx","HH_Spine");
VisibleStateImp _stub_vstate46(46, L"真空","真空.gfx","HH_Spine");
VisibleStateImp _stub_vstate47(47, L"水火护甲","水火护甲.gfx","HH_Spine");
VisibleStateImp _stub_vstate48(48, L"法师之星","法师之星.gfx","HH_Spine");
VisibleStateImp _stub_vstate49(49, L"无敌","无敌.gfx","HH_Spine");
VisibleStateImp _stub_vstate50(50, L"持续增加元气","持续增加元气.gfx","");

VisibleStateImp _stub_vstate51(51, L"持续降低元气","持续降低元气.gfx","");
VisibleStateImp _stub_vstate52(52, L"攻击等级增加","攻击等级增加.gfx","");
//VisibleStateImp _stub_vstate53(53, L"防御等级增加","防御等级增加.gfx","");
VisibleStateImp _stub_vstate54(54, L"吸收物理伤害","吸收物理伤害.gfx","HH_Spine");
VisibleStateImp _stub_vstate55(55, L"吸收法术伤害","吸收法术伤害.gfx","HH_Spine");
VisibleStateImp _stub_vstate56(56, L"免疫流血","流血免疫.gfx","");
VisibleStateImp _stub_vstate57(57, L"坚壁","坚壁.gfx","HH_Spine");
VisibleStateImp _stub_vstate58(58, L"遁隐","遁隐.gfx","HH_Spine");
VisibleStateImp _stub_vstate59(59, L"反弹","复仇惩戒.gfx","HH_Spine");
VisibleStateImp _stub_vstate60(60, L"封印","复仇封印.gfx","HH_Spine");
VisibleStateImp _stub_vstate61(61, L"反击","复仇镜像.gfx","HH_Spine");
VisibleStateImp _stub_vstate62(62, L"击晕","复仇雷霆.gfx","HH_Spine");
VisibleStateImp _stub_vstate63(63, L"反噬","复仇反噬.gfx","HH_Spine");
VisibleStateImp _stub_vstate64(64, L"三世明王咒","三世明王咒.gfx","HH_Spine");
VisibleStateImp _stub_vstate65(65, L"金刚轮印咒","金刚轮印咒.gfx","HH_Spine");
VisibleStateImp _stub_vstate66(66, L"黑巫咒","黑巫咒.gfx","HH_Spine");
VisibleStateImp _stub_vstate67(67, L"白巫咒","白巫咒.gfx","HH_Spine");
VisibleStateImp _stub_vstate68(68, L"深度冰刺","深度冰刺.gfx","HH_右大臂");
VisibleStateImp _stub_vstate69(69, L"玄武剑诀","玄武剑诀.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate70(70, L"饮血剑诀","饮血剑诀.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate71(71, L"磐龙剑诀","磐龙剑诀.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate72(72, L"八方剑影","八方剑影.gfx","HH_Spine");		
VisibleStateImp _stub_vstate73(73, L"火神","火神.gfx","HH_righthandweapon");				
VisibleStateImp _stub_vstate74(74, L"毒种","碧云术.gfx","HH_spine");		
VisibleStateImp _stub_vstate75(75, L"回血种","回风拂柳.gfx","HH_spine");		
VisibleStateImp _stub_vstate76(76, L"增加木水物防","铁木衫.gfx","HH_spine");	
VisibleStateImp _stub_vstate77(77, L"特殊的减速","腐蔓咒.gfx","HH_spine");		
VisibleStateImp _stub_vstate78(78, L"向自己的宠物传输mp","电弧同气连枝.gfx","");			
VisibleStateImp _stub_vstate79(79, L"加快吟唱增加法攻","风竹秋韵.gfx","HH_spine");
VisibleStateImp _stub_vstate80(80, L"增加物防暴击","魔神蚩尤牺牲.gfx","HH_spine");			
VisibleStateImp _stub_vstate81(81, L"增加法防法攻","金光电母牺牲.gfx","HH_spine");			
VisibleStateImp _stub_vstate82(82, L"向施放者(宠物)传输伤害","分担伤害.gfx","HH_spine");		
VisibleStateImp _stub_vstate83(83, L"吸伤害加物法防","护盾.gfx","HH_spine");			
VisibleStateImp _stub_vstate84(84, L"枯木逢春","枯木逢春.gfx","HH_spine");		
VisibleStateImp _stub_vstate86(85, L"提升暴击（限制生效次数）","魅灵_灵机.gfx","HH_Spine");
VisibleStateImp _stub_vstate85(86, L"九尊剑诀","九尊剑诀.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate87(87, L"修罗道用光效","修罗道_10.gfx","HH_head");
VisibleStateImp _stub_vstate88(88, L"修罗道用光效","修罗道_50.gfx","HH_head");
VisibleStateImp _stub_vstate89(89, L"修罗道用光效","修罗道_100.gfx","HH_head");
VisibleStateImp _stub_vstate90(90, L"修罗道用光效","修罗道_300.gfx","HH_head");
VisibleStateImp _stub_vstate91(91, L"位置回溯","法师_静谧之术_状态效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate92(92, L"生命回溯","法师_静谧之术2_状态效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate93(93, L"强制选择目标","白虎_奋战扫击_持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate94(94, L"吸收治疗","羽灵_天瀑符持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate95(95, L"受到普攻后退并清debuff","法师_法之奥义_持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate96(96, L"降低暴击抗性","羽灵_五感符持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate97(97, L"转移技能攻击诅咒","巫师_复仇之魂——涅槃_状态效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate98(98, L"受到攻击时额外对选定玩家发起攻击","刺客_魂葬印记状态效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate99(99, L"禁止被选中","巫师_冥王乐土持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate100(100, L"延迟土系伤害","持续土系伤害.gfx","HH_Spine");			
VisibleStateImp _stub_vstate101(101, L"昏迷，自身受到伤害加深","法师_冰晶世界持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate102(102, L"反弹之魂2(比例)","妖精_灵火持续.gfx","");						
VisibleStateImp _stub_vstate103(103, L"受伤害时定身","法师_灸焰_状态效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate104(104, L"受伤害时提升攻击力","妖兽_无畏_状态效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate105(105, L"死战到底","白虎_无冕之王持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate106(106, L"治疗偷取","魅灵_繁星_持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate107(107, L"死亡掉落金币","刺客_飘零持续效果.gfx","HH_Spine");			
VisibleStateImp _stub_vstate108(108, L"增加攻击距离","羽芒_回旋之翼_效果状态.gfx","HH_Spine");			
VisibleStateImp _stub_vstate109(109, L"无敌5（驱除debuf持续回血昏迷）","巫师_流殇_持续冰块.gfx","");	
VisibleStateImp _stub_vstate110(110, L"雷神变","羽灵_雷火堕_持续.gfx","");	
VisibleStateImp _stub_vstate111(111, L"武侠剑刃光环光效","武侠_霸气持续.gfx","");			
VisibleStateImp _stub_vstate112(112, L"妖精共舞调用","妖精_火狐之术持续.gfx","HH_Spine");		
VisibleStateImp _stub_vstate113(113, L"法师冰晶世界调用","法师_冰晶世界_冰雾持续.gfx","");
VisibleStateImp _stub_vstate114(114, L"战场旗帜光效","旗子.gfx","HH_Head");
VisibleStateImp _stub_vstate115(115, L"任务用光效","小狗状态.gfx","HH_feijian");			
VisibleStateImp _stub_vstate116(116, L"任务用光效","小白蛇状态.gfx","HH_feijian");	
VisibleStateImp _stub_vstate117(117, L"瘫痪光效","瘫痪.gfx","HH_Head");					
VisibleStateImp _stub_vstate118(118, L"特定技能吟唱时间降低光效","特定技能吟唱时间降低.gfx","HH_feijian");	
VisibleStateImp _stub_vstate119(119, L"特定技能伤害增加光效","特定技能伤害增加.gfx","HH_feijian");	
VisibleStateImp _stub_vstate120(120, L"回血护盾光效","回血护盾.gfx","HH_bind");
VisibleStateImp _stub_vstate121(121, L"飞行速度提高（可叠加刷新持续时间）","疾云步击中.gfx","HH_Spine");
VisibleStateImp _stub_vstate122( 122, L"移动掉血","移动掉血_光效01.gfx","HH_feijian");
VisibleStateImp _stub_vstate123( 123, L"不移动就掉血","不移动就掉血01.gfx","HH_bind");
VisibleStateImp _stub_vstate125( 125, L"免疫打断护盾","魔法盾.gfx","HH_Spine");
VisibleStateImp _stub_vstate127( 127, L"猎命状态","猎命状态效果01.gfx","HH_feijian");
VisibleStateImp _stub_vstate128( 128, L"1白","月仙_冰球1个.gfx","HH_Spine");
VisibleStateImp _stub_vstate129( 129, L"2白","月仙_冰球2个.gfx","HH_Spine");
VisibleStateImp _stub_vstate130( 130, L"3白","月仙_冰球3个.gfx","HH_Spine");
VisibleStateImp _stub_vstate131( 131, L"1黑","月仙_雷球1个.gfx","HH_Spine");
VisibleStateImp _stub_vstate132( 132, L"2黑","月仙_雷球2个.gfx","HH_Spine");
VisibleStateImp _stub_vstate133( 133, L"3黑","月仙_雷球3个.gfx","HH_Spine");
VisibleStateImp _stub_vstate134( 134, L"1黑1白","月仙_1冰1雷球.gfx","HH_Spine");
VisibleStateImp _stub_vstate135( 135, L"2黑1白","月仙_1冰2雷.gfx","HH_Spine");
VisibleStateImp _stub_vstate136( 136, L"2白1黑","月仙_2冰1雷.gfx","HH_Spine");
VisibleStateImp _stub_vstate137( 137, L"内伤状态","内伤01.gfx","HH_bind");
VisibleStateImp _stub_vstate138( 138, L"死亡重置施法者冷却","死亡重置施法者冷却时间01.gfx","HH_Spine");
VisibleStateImp _stub_vstate139( 139, L"普攻附加伤害","普攻附加额外伤害01.gfx","HH_shou03");
VisibleStateImp _stub_vstate140( 140, L"技能CD改变","夜影_影噬_状态效果01.gfx","HH_feijian");
VisibleStateImp _stub_vstate141( 141, L"啸月","啸月_施放起01.gfx","HH_feijian");


TeamStateImp _stub_tstate1( 1,L"昏迷\r无法进行任何移动和操作", "致晕.dds");
TeamStateImp _stub_tstate2( 2,L"睡眠\r沉睡状态，受攻击会恢复", "睡眠.dds");
TeamStateImp _stub_tstate3( 3,L"迟滞\r降低移动速度", "迟滞.dds");
TeamStateImp _stub_tstate4( 4,L"物理反震\r反弹所有近身物理伤害", "对物理攻击产生反震.dds");
TeamStateImp _stub_tstate5( 5,L"羽盾\r用真气值吸收一部伤害", "羽盾.dds");

TeamStateImp _stub_tstate6( 6,L"烈焰护甲\r增加火防并提高生命恢复速度", "烈焰护甲.dds");
TeamStateImp _stub_tstate7( 7,L"寒冰护甲\r增加水防并增加法力恢复速度", "寒冰护甲.dds");
TeamStateImp _stub_tstate8( 8,L"定身\r无法移动", "定身.dds");
TeamStateImp _stub_tstate9( 9,L"封印\r无法使用任何普攻和技能", "不能攻击.dds");
TeamStateImp _stub_tstate10( 10,L"魅惑\r目标怪物变成被动攻击", "主动变被动.dds");

TeamStateImp _stub_tstate11( 11,L"磐石护甲\r增加土防和物防", "磐石护甲.dds");
TeamStateImp _stub_tstate12( 12,L"雷击\r持续受到金系伤害", "持续金系攻击.dds");
TeamStateImp _stub_tstate13( 13,L"中毒\r持续受到木系伤害", "中毒.dds");
TeamStateImp _stub_tstate14( 14,L"燃烧\r持续受到火系伤害", "持续火攻击.dds");
TeamStateImp _stub_tstate15( 15,L"沙漩\r持续受到土系伤害", "持续土系伤害.dds");

TeamStateImp _stub_tstate16( 16,L"法力燃烧\r真气持续减少", "持续MP减少状态.dds");
TeamStateImp _stub_tstate17( 17,L"流血\r持续受到物理伤害", "放血.dds");
TeamStateImp _stub_tstate18( 18,L"破甲\r物理防御力下降", "物理防御力下降.dds");
TeamStateImp _stub_tstate19( 19,L"虚弱\r法术防御力下降", "五行防御力下降.dds");
TeamStateImp _stub_tstate20( 20,L"蚀刃\r物理攻击力下降", "物理攻击力下降.dds");

TeamStateImp _stub_tstate21( 21,L"诅咒\r受到伤害提升", "自身受到的物理伤害加倍.dds");
TeamStateImp _stub_tstate22( 22,L"迟缓\r攻击速度减慢", "普通攻击间隔加长.dds");
TeamStateImp _stub_tstate23( 23,L"迟钝\r减慢吟唱", "技能吟唱时间加长.dds");
TeamStateImp _stub_tstate24( 24,L"目盲\r降低准确度", "准确度下降.dds");
TeamStateImp _stub_tstate25( 25,L"笨拙\r躲闪度下降", "躲闪度下降.dds");

TeamStateImp _stub_tstate26( 26,L"生命恢复加快", "HP恢复加快.dds");
TeamStateImp _stub_tstate27( 27,L"真气恢复加快", "MP恢复加快.dds");
TeamStateImp _stub_tstate28( 28,L"提升生命上限", "提升HP上限.dds");
TeamStateImp _stub_tstate29( 29,L"提升真气上限", "提高MP上限.dds");
TeamStateImp _stub_tstate30( 30,L"坚甲\r物理防御力提高", "物理防御力提高.dds");

TeamStateImp _stub_tstate31( 31,L"聚神\r法术防御力提高", "法术防御力提高.dds");
TeamStateImp _stub_tstate32( 32,L"利刃\r物理攻击力提高", "物理攻击力提高.dds");
TeamStateImp _stub_tstate33( 33,L"迅捷\r攻击速度加快", "攻击速度加快.dds");
TeamStateImp _stub_tstate34( 34,L"灵敏\r提高躲闪率", "提高躲闪度.dds");
TeamStateImp _stub_tstate35( 35,L"祝福\r受到伤害减少", "自身伤害减半.dds");

TeamStateImp _stub_tstate36( 36,L"灵巧\r提高准确度", "穿透.dds");
TeamStateImp _stub_tstate37( 37,L"专注\r加快吟唱速度", "加快吟唱.dds");
TeamStateImp _stub_tstate38( 38,L"攻击附加水伤害", "使普通攻击附加水系伤害.dds");
TeamStateImp _stub_tstate39( 39,L"攻击附加火伤害", "使普通攻击附加火系伤害.dds");
TeamStateImp _stub_tstate40( 40,L"攻击附加木伤害", "使普通攻击附近木系伤害.dds");

TeamStateImp _stub_tstate41( 41,L"持续恢复生命", "持续恢复HP.dds");
TeamStateImp _stub_tstate42( 42,L"持续恢复真气", "持续恢复MP.dds");
TeamStateImp _stub_tstate43( 43,L"散神\r法术攻击力降低", "定身且不能攻击.dds");
TeamStateImp _stub_tstate44( 44,L"移动加速", "加速.dds");
TeamStateImp _stub_tstate45( 45,L"生命上限降低", "HP上限下降.dds");

TeamStateImp _stub_tstate46( 46,L"灵助\r法术攻击力提高", "法术攻击力提高.dds");
TeamStateImp _stub_tstate47( 47,L"白虎变", "白虎变.dds");
TeamStateImp _stub_tstate48( 48,L"金系防御上升", "金系防御上升.dds");
TeamStateImp _stub_tstate49( 49,L"木系防御上升", "木系防御上升.dds");
TeamStateImp _stub_tstate50( 50,L"水系防御上升", "水系防御上升.dds");

TeamStateImp _stub_tstate51( 51,L"火系防御上升", "火系防御上升.dds");
TeamStateImp _stub_tstate52( 52,L"土系防御上升", "土系防御上升.dds");
TeamStateImp _stub_tstate53( 53,L"金系防御下降", "金系防御下降.dds");
TeamStateImp _stub_tstate54( 54,L"木系防御下降", "木系防御下降.dds");
TeamStateImp _stub_tstate55( 55,L"水系防御下降", "水系防御下降.dds");

TeamStateImp _stub_tstate56( 56,L"火系防御下降", "火系防御下降.dds");
TeamStateImp _stub_tstate57( 57,L"土系防御下降", "土系防御下降.dds");
TeamStateImp _stub_tstate58( 58,L"避水诀\r水中不用呼吸，无法使用技能", "避水诀.dds");
TeamStateImp _stub_tstate59( 59,L"般若心经\r所有法术忽略吟唱", "般若心经.dds");
TeamStateImp _stub_tstate60( 60,L"真元爆发\r将全身元气爆发提升自身能力", "真元爆发.dds");
TeamStateImp _stub_tstate61( 61,L"易髓经\r降低自身法防提升物防", "易髓经.dds");
TeamStateImp _stub_tstate62( 62,L"易筋经\r降低自身物防提升法防", "易筋经.dds");
TeamStateImp _stub_tstate63( 63,L"兽王之怒\r受到攻击时增加元气", "兽王之怒.dds");
TeamStateImp _stub_tstate64( 64,L"坚甲\r物理防御力提高", "坚甲符.dds");
TeamStateImp _stub_tstate65( 65,L"金钟罩\r提升物防", "金钟罩.dds");
TeamStateImp _stub_tstate66( 66,L"巨灵神力\r提高物理攻击", "巨灵神力.dds");
TeamStateImp _stub_tstate67( 67,L"化血成魔\r降低生命上限提升命中率", "化血成魔.dds");
TeamStateImp _stub_tstate68( 68,L"灵助\r法术攻击力提高", "灵助符.dds");
TeamStateImp _stub_tstate69( 69,L"翼盾\r吸收一定伤害并回复真气", "翼盾.dds");
TeamStateImp _stub_tstate70( 70,L"烈焰之矢\r攻击附加一定火伤害", "烈焰之矢.dds");
TeamStateImp _stub_tstate71( 71,L"狂雷天鹰\r持续受到金系伤害并减速", "狂雷天鹰.dds");
TeamStateImp _stub_tstate72( 72,L"无视所有行动阻碍状态", "准无敌.dds");
TeamStateImp _stub_tstate73( 73,L"持续冰伤害", "持续冰伤害.dds");
TeamStateImp _stub_tstate74( 74,L"暴击率提升", "爆击率提升.dds");

TeamStateImp _stub_tstate75( 75,L"妖狐变\r物防、命中提升，真气值下降", "灵狐变.dds");
TeamStateImp _stub_tstate76( 76,L"无敌", "无敌.dds");
TeamStateImp _stub_tstate77( 77,L"荆棘阵\r大量降低伤害，并且反弹物理伤害", "荆棘阵.dds");
TeamStateImp _stub_tstate78( 78,L"弱魂咒\r生命和真气值停止回复", "弱魂咒.dds");
TeamStateImp _stub_tstate79( 79,L"残体咒\r受到加深伤害", "残体咒.dds");
TeamStateImp _stub_tstate80( 80,L"碎灵咒\r受到攻击失去元气", "碎灵咒.dds");
TeamStateImp _stub_tstate81( 81,L"飞花遁影\r移动加速", "妖狐附体.dds");
TeamStateImp _stub_tstate82( 82,L"骑乘加速", "骑乘加速.dds");
TeamStateImp _stub_tstate83( 83,L"物理攻击提高", "物理攻击力提高.dds");

TeamStateImp _stub_tstate84( 84,L"专注\r提升专注的精神可以使你的施法不被干扰。\r目标免疫封印", "专注.dds");
TeamStateImp _stub_tstate85( 85,L"亢奋\r亢奋的人是不需要睡觉的\r免疫睡眠状态", "亢奋.dds");
TeamStateImp _stub_tstate86( 86,L"自由\r只要你有一颗自由的心，\r你就不会被减速，或者定身。\r免疫移动减速和击晕", "自由.dds");
TeamStateImp _stub_tstate87( 87,L"坚强\r免役按比例掉血。\r坚强的人总不会被打垮", "坚强.dds");
TeamStateImp _stub_tstate88( 88,L"神圣\r免疫所有负面状态", "神圣.dds");
TeamStateImp _stub_tstate89( 89,L"霸体\r免疫物理伤害", "霸体.dds");
TeamStateImp _stub_tstate90( 90,L"火免\r免疫火系伤害", "火免.dds");

TeamStateImp _stub_tstate91( 91,L"水免\r免役水系伤害", "水免.dds");
TeamStateImp _stub_tstate92( 92,L"金免\r免疫金系伤害", "金免.dds");
TeamStateImp _stub_tstate93( 93,L"木免\r免疫木系伤害", "木免.dds");
TeamStateImp _stub_tstate94( 94,L"土免\r免疫土系伤害", "土免.dds");
TeamStateImp _stub_tstate95( 95,L"虚无\r免疫五行伤害", "虚无.dds");
TeamStateImp _stub_tstate96( 96,L"狂妄\r免疫所有状态", "狂妄.dds");
TeamStateImp _stub_tstate97( 97,L"水中减速\r游泳减速", "水中减速.dds");
TeamStateImp _stub_tstate98( 98,L"水中加速\r游泳加速", "水中加速.dds");
TeamStateImp _stub_tstate99( 99,L"空中减速\r飞行减速", "空中减速.dds");
TeamStateImp _stub_tstate100( 100,L"空中加速\r飞行加速", "空中加速.dds");

TeamStateImp _stub_tstate101( 101,L"骑乘减速\r骑乘减速", "骑乘减速.dds");
TeamStateImp _stub_tstate102( 102,L"爆发\r持续增加元气", "爆发.dds");
TeamStateImp _stub_tstate103( 103,L"疲乏\r持续减少元气", "疲乏.dds");
TeamStateImp _stub_tstate104( 104,L"精灵之力\r持续时间内增加小精灵力量", "精灵之力.dds");
TeamStateImp _stub_tstate105( 105,L"精灵之敏\r持续时间内增加小精灵的敏捷", "精灵之敏.dds");

TeamStateImp _stub_tstate106( 106,L"空刃\r你向敌人释放了空刃,\r自己也同时受到了风的保护。\r增加自身物理防御", "空刃.dds");
TeamStateImp _stub_tstate107( 107,L"元素虚弱\r目标物理防御增加,法术防御降低.", "元素虚弱.dds");
TeamStateImp _stub_tstate108( 108,L"深毒\r目标受伤害增加20%，\r这个效果可以和天火狂龙等技能叠加", "深毒.dds");
TeamStateImp _stub_tstate109( 109,L"扎根\r自身定身同时伤害减少同时反弹伤害", "扎根.dds");
TeamStateImp _stub_tstate110( 110,L"大地守护\r每3秒恢复HP,伤害减少.物理和法术攻击降低", "大地守护.dds");

TeamStateImp _stub_tstate111( 111,L"狂暴\r目标防御和法术防御降低,同时攻击等级增加", "狂暴.dds");
TeamStateImp _stub_tstate112( 112,L"沙暴\r命中降低,施法速度变慢", "沙暴.dds");
TeamStateImp _stub_tstate113( 113,L"乡情\r生命和真气回复速度增加,移动速度增加", "乡情.dds");
TeamStateImp _stub_tstate114( 114,L"湮灭\r水系防御降低", "湮灭.dds");
TeamStateImp _stub_tstate115( 115,L"幸运\r暴击增加", "幸运.dds");
TeamStateImp _stub_tstate116( 116,L"冲击\r物理防御降低", "冲击.dds");
TeamStateImp _stub_tstate117( 117,L"火花\r火系防御降低", "火花.dds");
TeamStateImp _stub_tstate118( 118,L"心火\r物理和法速攻击速度降低", "心火.dds");
TeamStateImp _stub_tstate119( 119,L"火种\r持续火伤害", "火种.dds");
TeamStateImp _stub_tstate120( 120,L"燃足\r火系防御降低,移动速度降低", "燃足.dds");

TeamStateImp _stub_tstate121( 121,L"坚壁\r吸收物理和法术伤害,并且降低自身移动速度", "坚壁.dds");
TeamStateImp _stub_tstate122( 122,L"刃切\r金系防御降低", "刃切.dds");
TeamStateImp _stub_tstate123( 123,L"叶舞\r移动速度降低并且流血", "叶舞.dds");
TeamStateImp _stub_tstate124( 124,L"烧焦\r防御等级下降.攻击等级下降,\r移动速度少量增加", "烧焦.dds");
TeamStateImp _stub_tstate125( 125,L"真空\r释放速度变慢,移动速度变慢.受到伤害减少", "真空.dds");

TeamStateImp _stub_tstate126( 126,L"止血\r免疫流血状态。", "止血.dds");
TeamStateImp _stub_tstate127( 127,L"金刚不坏\r吸收受到的物理伤害", "金刚不坏.dds");
TeamStateImp _stub_tstate128( 128,L"法力无边\r吸收受到法术伤害", "法力无边.dds");
TeamStateImp _stub_tstate129( 129,L"以牙还牙\r反弹受到的法术伤害", "以牙还牙.dds");
TeamStateImp _stub_tstate130( 130,L"风盾术\r受到伤害减少,物理攻击速度和闪避提升", "风盾术.dds");

TeamStateImp _stub_tstate131( 131,L"气流锁\r目标定身且有几率被封印", "气流锁.dds");
TeamStateImp _stub_tstate132( 132,L"封闭\r封印且物理伤害免疫", "封闭.dds");
TeamStateImp _stub_tstate133( 133,L"无心\r免疫伤害加深状态。", "无心.dds");
TeamStateImp _stub_tstate134( 134,L"冻结\r无法行动", "冻结.dds");
TeamStateImp _stub_tstate135( 135,L"攻击等级增加", "攻击等级增加.dds");

TeamStateImp _stub_tstate136( 136,L"攻击等级降低", "攻击等级降低.dds");
TeamStateImp _stub_tstate137( 137,L"防御等级增加", "防御等级增加.dds");
TeamStateImp _stub_tstate138( 138,L"防御等级降低", "防御等级降低.dds");

TeamStateImp _stub_tstate139( 139,L"持续受到土系伤害", "持续土伤害2.dds");
TeamStateImp _stub_tstate140( 140,L"封印\r无法使用任何普攻和技能", "不能攻击2.dds");

TeamStateImp _stub_tstate141( 141,L"定身\r无法移动", "定身2.dds");
TeamStateImp _stub_tstate142( 142,L"祝福\r受到伤害减少", "物理伤害减少2.dds");
TeamStateImp _stub_tstate143( 143,L"诅咒\r受到伤害提升", "伤害加深2.dds");
TeamStateImp _stub_tstate144( 144,L"提升生命上限", "生命上限增加2.dds");
TeamStateImp _stub_tstate145( 145,L"利刃\r物理攻击力提高", "攻击力增加2.dds");
TeamStateImp _stub_tstate146( 146,L"灵助\r法术攻击力提高", "法术攻击增加2.dds");
TeamStateImp _stub_tstate147( 147,L"专注\r加快吟唱", "吟唱加速2.dds");
TeamStateImp _stub_tstate148( 148,L"移动加速", "移动加速2.dds");
TeamStateImp _stub_tstate149( 149 ,L"遁隐\r进入遁隐状态", "隐身.dds");
TeamStateImp _stub_tstate150( 150 ,L"提升反隐等级\r提高反隐等级", "集中精神.dds");
TeamStateImp _stub_tstate151( 151 ,L"嗜血咒\r每次攻击时，从自身造成的伤害中恢复一定生命。", "吸血光环.dds");
TeamStateImp _stub_tstate152( 152 ,L"狂狼印\r提高暴击伤害值","暴击伤害.dds");
TeamStateImp _stub_tstate153( 153 ,L"金刚轮印咒\r被攻击时，有几率闪避攻击技能的伤害。", "伤害闪避.dds");
TeamStateImp _stub_tstate154( 154 ,L"三世明王咒\r被攻击时，有几率闪避攻击技能的效果。", "状态闪避.dds");
TeamStateImp _stub_tstate155( 155 ,L"如来天心咒\r免除一次致死打击，并恢复生命", "回光返照.dds");
TeamStateImp _stub_tstate156( 156 ,L"惩戒\r向攻击者反弹伤害", "反弹之魂.dds");
TeamStateImp _stub_tstate157( 157 ,L"封印\r使攻击者陷入封印状态", "封印之魂.dds");
TeamStateImp _stub_tstate158( 158 ,L"镜像\r攻击者同时承受其技能造成的伤害和状态", "反击之魂.dds");
TeamStateImp _stub_tstate159( 159 ,L"震慑\r将攻击者击晕", "击晕之魂.dds");
TeamStateImp _stub_tstate160( 160 ,L"生命绽放\r增加由技能和药品获得的生命恢复效果，并减少红符冷却时间", "祝福加深.dds");
TeamStateImp _stub_tstate161( 161 ,L"生命禁锢\r降低由技能和药品获得的生命恢复效果，并延长红符冷却时间", "祝福削弱.dds");
TeamStateImp _stub_tstate162( 162 ,L"反噬\r每次普通攻击或施放技能时，都会受到伤害", "使用技能时受伤害.dds");
TeamStateImp _stub_tstate163( 163 ,L"使用技能时被中断\r使目标每次施放技能时，都会被打断", "爆发.dds");
TeamStateImp _stub_tstate164( 164 ,L"人鱼变\r变身成人鱼", "魔法盾.dds");
TeamStateImp _stub_tstate165( 165 ,L"深度冰刺\r降低攻速，提高攻击等级", "冰冻.dds");
TeamStateImp _stub_tstate166( 166 ,L"巫师咒\r调整攻防等级", "热血.dds");
TeamStateImp _stub_tstate167( 167 ,L"金刚剑印\r金系及物理系技能伤害产生加成效果", "金刚剑印.dds");			
TeamStateImp _stub_tstate168( 168 ,L"寒潮剑印\r木系及水系技能伤害产生加成效果", "寒潮剑印.dds");			
TeamStateImp _stub_tstate169( 169 ,L"焚心剑印\r土系及火系技能伤害产生加成效果", "焚心剑印.dds");
TeamStateImp _stub_tstate170( 170 ,L"玄武剑诀\r有效伤害有几率触发“玄武剑眼”", "玄武剑诀.dds");			
TeamStateImp _stub_tstate171( 171 ,L"饮血剑诀\r有效伤害有几率触发“饮血剑眼”", "饮血剑诀.dds");			
TeamStateImp _stub_tstate172( 172 ,L"磐龙剑诀\r有效伤害有几率触发“磐龙剑眼”", "磐龙剑诀.dds");			
TeamStateImp _stub_tstate173( 173 ,L"玄武剑眼\r特定技能激活该剑眼", "玄武剑眼.dds");			
TeamStateImp _stub_tstate174( 174 ,L"饮血剑眼\r特定技能激活该剑眼", "饮血剑眼.dds");			
TeamStateImp _stub_tstate175( 175 ,L"磐龙剑眼\r特定技能激活该剑眼", "磐龙剑眼.dds");			
TeamStateImp _stub_tstate176( 176 ,L"八方剑影\r对周围目标造成伤害", "八方剑影.dds");	
TeamStateImp _stub_tstate177( 177 ,L"毒种\r血量低于50%时触发持续伤害", "碧云咒.dds");
TeamStateImp _stub_tstate178( 178 ,L"回血种\r受到伤害触发持续回血效果", "回风拂柳.dds");
TeamStateImp _stub_tstate179( 179 ,L"增加木水物防\r提高木防水防物防”", "铁木衫.dds");
TeamStateImp _stub_tstate180( 180 ,L"特殊的减速\r减速且受到勾魂摄魄攻击时伤害加深", "腐蔓咒.dds");
TeamStateImp _stub_tstate181( 181 ,L"增加物防暴击\r提高物理防御，暴击率", "魔神蚩尤牺牲.dds");
TeamStateImp _stub_tstate182( 182 ,L"增加法防法攻\r提高五行防御，法术攻击力", "金光电母牺牲.dds");
TeamStateImp _stub_tstate183( 183 ,L"向自己的宠物传输mp\r将本体真气传输给召唤物", "同气连枝.dds");
TeamStateImp _stub_tstate184( 184 ,L"向施放者(宠物)传输伤害\r施法者将为状态所有者分担伤害", "分担伤害.dds");
TeamStateImp _stub_tstate185( 185 ,L"吸伤害加物法防\r可吸收一定的伤害且护盾存在时提高防御抗", "护盾.dds");
TeamStateImp _stub_tstate186( 186 ,L"加快吟唱增加法攻\r提高吟唱速度提高法攻", "风竹秋韵.dds");
TeamStateImp _stub_tstate187( 187 ,L"渐增的回血\r数值渐增的持续回血", "生命之树.dds");
TeamStateImp _stub_tstate188( 188 ,L"献祭\r献祭光环", "火神.dds");
TeamStateImp _stub_tstate189( 189 ,L"枯木逢春\r死亡后立即复活", "枯木逢春.dds");
TeamStateImp _stub_tstate190( 190 ,L"特殊物品伤害触发器\r受到摄魂咒攻击伤害加强", "勾魂摄魄.dds");
TeamStateImp _stub_tstate191( 191 ,L"提升暴击（限制生效次数）", "暴击提升.dds");	
TeamStateImp _stub_tstate192( 192 ,L"九尊剑诀\r有几率触发九尊剑眼", "九尊剑诀.dds");			
TeamStateImp _stub_tstate193( 193 ,L"九尊剑眼\r被特殊攻击击中触发效果", "九尊剑眼.dds");			
TeamStateImp _stub_tstate194( 194 ,L"武器失效", "武器失效.dds");			
TeamStateImp _stub_tstate195( 195 ,L"仇恨\r增加受到伤害时的仇恨", "冷嘲热讽.dds");
TeamStateImp _stub_tstate196( 196 ,L"增强技能对npc的伤害\r增加技能对NPC的伤害", "增强点技能对NPC的伤害.dds");			
TeamStateImp _stub_tstate197( 197 ,L"侦测周围隐身玩家\r将周围的隐身单位添加到界面上", "侦查周围隐身的玩家.dds");			
TeamStateImp _stub_tstate198( 198 ,L"MP恢复加快2\r场景内MP恢复加快", "场景回蓝.dds");
TeamStateImp _stub_tstate199( 199 ,L"位置回溯\r若干秒后被传回标记位置", "静谧之术.dds");			
TeamStateImp _stub_tstate200( 200 ,L"生命回溯\r若干秒后血量变回标记血量", "静谧之术2buff.dds");			
TeamStateImp _stub_tstate201( 201 ,L"禁止飞行\r使玩家在一段时间内无法飞行", "兽灵之力buff.dds");			
TeamStateImp _stub_tstate202( 202 ,L"强制选择目标\r使玩家在一段时间内只能选择施法者", "奋战扫击buff.dds");			
TeamStateImp _stub_tstate203( 203 ,L"吸收治疗\r吸收一定量的治疗", "吸收治疗buff.dds");			
TeamStateImp _stub_tstate204( 204 ,L"受到普攻后退并清debuff\r受攻击被弹开", "法之奥义.dds");			
TeamStateImp _stub_tstate205( 205 ,L"增加爆击抗性\r受到致命一击的几率降低", "沙暴buff.dds");			
TeamStateImp _stub_tstate206( 206 ,L"降低爆击抗性\r受到致命一击的几率提高", "羽斩buff.dds");			
TeamStateImp _stub_tstate207( 207 ,L"转移技能攻击诅咒\r将攻击转移给施法者", "复仇之魂buff.dds");
TeamStateImp _stub_tstate208( 208 ,L"被治疗时额外给选定玩家治疗\r额外治疗", "额外治疗buff.dds");				
TeamStateImp _stub_tstate209( 209 ,L"受到攻击时额外对选定玩家发起攻击\r伤害链接", "葬魂印记buff.dds");			
TeamStateImp _stub_tstate211( 211 ,L"禁止被选中\r使敌人无法直视", "冥王乐土buff.dds");						
TeamStateImp _stub_tstate212( 212 ,L"增强技能对npc的伤害2\r可以和1叠加", "迷色万花buff.dds");			
TeamStateImp _stub_tstate213( 213 ,L"延迟土系伤害\r若干秒后造成土系伤害", "风岩葬buff.dds");			
TeamStateImp _stub_tstate214( 214 ,L"昏迷，自身受到伤害加深\r冰冻光环效果", "冰晶世界.dds");			
TeamStateImp _stub_tstate215( 215 ,L"反弹之魂2(比例)\r荆棘术", "灵火buff.dds");					
TeamStateImp _stub_tstate216( 216 ,L"受伤害时定身\r受到伤害时有一定几率定身", "炙焰.dds");	
TeamStateImp _stub_tstate217( 217 ,L"受伤害时获得元气\r获得元气", "获得元气buff.dds");
TeamStateImp _stub_tstate218( 218 ,L"受伤害时提升攻击力\r触发实际效果图标", "无畏buff.dds");						
TeamStateImp _stub_tstate219( 219 ,L"受伤害时提升攻击力\r愈战愈勇", "无畏buff.dds");			
TeamStateImp _stub_tstate220( 220 ,L"死战到底\r濒死后无敌且血量上限减少99%", "虎啸buff.dds");			
TeamStateImp _stub_tstate221( 221 ,L"治疗偷取\r治疗被转移到施法者", "流星buff.dds");			
TeamStateImp _stub_tstate222( 222 ,L"死亡掉落金币\r在此状态下被杀死会掉落金币", "飘零buff.dds");			
TeamStateImp _stub_tstate223( 223 ,L"增加攻击距离\r攻击距离提高", "擎天雷鸣buff.dds");			
TeamStateImp _stub_tstate224( 224 ,L"无敌5（驱除debuf持续回血昏迷）\r放逐", "流殇buff.dds");
TeamStateImp _stub_tstate225( 225 ,L"雷神变\r羽灵变身", "雷火坠buff.dds");	
TeamStateImp _stub_tstate226( 226 ,L"武侠剑刃光环\r剑刃光环图标", "霸气.dds");					
TeamStateImp _stub_tstate227( 227 ,L"妖精共舞\r妖精共舞光效图标", "火狐之术buff.dds");
TeamStateImp _stub_tstate229( 229 ,L"减少近战普攻伤害\r防御优化 ", "桎梏诀.dds");	
TeamStateImp _stub_tstate230( 230 ,L"全属性提升\r等级奖励 ", "回旋之翼.dds");			
TeamStateImp _stub_tstate231( 231 ,L"非战斗恢复\r非战斗状态恢复HPMP", "玄武刻印buff.dds");			
TeamStateImp _stub_tstate232( 232 ,L"定值提升\r固定值加HPMP上限", "五感符.dds");
TeamStateImp _stub_tstate233( 233 ,L"自身受到伤害加深（可叠加可刷新）", "旋风斩.dds");
TeamStateImp _stub_tstate234( 234 ,L"PK无惩罚状态", "战场状态.dds");
TeamStateImp _stub_tstate235( 235 ,L"法术防御力提高2", "法术防御力提高.dds");		
TeamStateImp _stub_tstate236( 236 ,L"战场旗帜BUFF", "旗帜BUFF.dds");
TeamStateImp _stub_tstate237( 237 ,L"防御等级降低", "防御等级降低.dds");
TeamStateImp _stub_tstate238( 238 ,L"增加物法攻防hp", "回旋之翼.dds");
TeamStateImp _stub_tstate239( 239 ,L"暴击增加", "暴击提升.dds");
TeamStateImp _stub_tstate240( 240 ,L"提升御魔猎魔等级\r百分比提高对怪物攻防", "暴击提升.dds");
TeamStateImp _stub_tstate241( 241 ,L"生命值、攻击等级、防御等级提高", "北美国战.dds");
TeamStateImp _stub_tstate242( 242 ,L"增加攻击等级、防御等级各3点", "GT高级.dds");
TeamStateImp _stub_tstate243( 243 ,L"增加攻击等级、防御等级各1点", "GT低级.dds");
TeamStateImp _stub_tstate244( 244 ,L"伤害减免3\r受到来自于玩家的伤害减少", "固若金汤.dds");
TeamStateImp _stub_tstate245( 245 ,L"攻击时给自己添加状态包", "攻守兼备.dds");	
TeamStateImp _stub_tstate246( 246 ,L"攻击时给目标添加状态包", "乘胜追击.dds");
TeamStateImp _stub_tstate247( 247 ,L"免疫物理伤害", "物免2.dds");
TeamStateImp _stub_tstate248( 248 ,L"免疫金系伤害", "金免2.dds");
TeamStateImp _stub_tstate249( 249 ,L"免疫木系伤害", "木免2.dds");
TeamStateImp _stub_tstate250( 250 ,L"免疫水系伤害", "水免2.dds");
TeamStateImp _stub_tstate251( 251 ,L"免疫火系伤害", "火免2.dds");
TeamStateImp _stub_tstate252( 252 ,L"免疫土系伤害", "土免2.dds");
TeamStateImp _stub_tstate253( 253 ,L"物理反震\r反弹所有近身物理伤害", "对物理攻击产生反震.dds");
TeamStateImp _stub_tstate254( 254 ,L"增加攻击等级、防御等级", "热血.dds");
TeamStateImp _stub_tstate255( 255,L"瘫痪", "瘫痪buff.dds");
TeamStateImp _stub_tstate256( 256,L"受到的技能伤害提高", "受到的技能伤害提高buff.dds");
TeamStateImp _stub_tstate257( 257,L"受到的技能暴击率提高", "受到的技能暴击率提高buff.dds");
TeamStateImp _stub_tstate258( 258,L"特定技能吟唱时间降低", "特定技能吟唱时间降低buff.dds");
TeamStateImp _stub_tstate259( 259,L"特定技能伤害增加", "特定技能伤害增加buff.dds");
TeamStateImp _stub_tstate260( 260,L"寒冰护甲2", "寒冰护甲.dds");
TeamStateImp _stub_tstate261( 261,L"烈焰护甲2", "烈焰护甲.dds");
TeamStateImp _stub_tstate262( 262,L"回血护盾", "回血护盾buff.dds");
TeamStateImp _stub_tstate263( 263,L"飞行速度提高（可叠加刷新持续时间）", "准无敌.dds");
TeamStateImp _stub_tstate264( 264,L"气魄值提高", "气魄值提高.dds");
TeamStateImp _stub_tstate265( 265,L"加气魄减暴伤", "暗炎.dds");
TeamStateImp _stub_tstate266( 266,L"移动扣血", "寒宫蟾步.dds");
TeamStateImp _stub_tstate267( 267,L"不移动就掉血", "移动扣血.dds");
TeamStateImp _stub_tstate268( 268,L"不移动就获得技能", "不移动就掉.dds");
TeamStateImp _stub_tstate269( 269,L"免疫打断护盾", "免疫打断护盾.dds");
TeamStateImp _stub_tstate270( 270,L"持续瘫痪", "蚀骨月毒.dds");
TeamStateImp _stub_tstate271( 271,L"内伤", "雷霆万钧.dds");
TeamStateImp _stub_tstate272( 272,L"普攻减伤", "广寒之护.dds");
TeamStateImp _stub_tstate273( 273,L"死亡后重置施法者技能冷却时间", "无情游.dds");
TeamStateImp _stub_tstate274( 274,L"指定技能造成额外技能效果", "月食.dds");
TeamStateImp _stub_tstate275( 275,L"普通攻击附加额外伤害", "狱龙牙.dds");
TeamStateImp _stub_tstate276( 276,L"指定技能使用后不进入冷却", "邈云汉.dds");
TeamStateImp _stub_tstate277( 277,L"猎命", "猎命.dds");
TeamStateImp _stub_tstate278( 278,L"夜影变身", "胧夜影.dds");
TeamStateImp _stub_tstate279( 279,L"月仙变身", "胧月仙.dds");
TeamStateImp _stub_tstate280( 280,L"附加霜雷状态", "霜雷术.dds");
TeamStateImp _stub_tstate281( 281,L"月神之体", "月神之体.dds");
TeamStateImp _stub_tstate282( 282,L"增强技能对npc的伤害3", "月咏.dds");
TeamStateImp _stub_tstate283( 283,L"减少受到的暴击伤害","减少受到的暴击伤害.dds");
TeamStateImp _stub_tstate284( 284,L"物理伤害提高","造成物理伤害提高2.dds");
TeamStateImp _stub_tstate285( 285,L"五行伤害提高","造成法术伤害提高2.dds");
TeamStateImp _stub_tstate286( 286,L"受到的技能伤害提高2", "晓月银光.dds");
TeamStateImp _stub_tstate287( 287,L"冰雷球占位状态包", "");
TeamStateImp _stub_tstate288( 288,L"技能CD改变","影噬.dds");
TeamStateImp _stub_tstate289( 289,L"免疫驱逐","彩云障月.dds");
TeamStateImp _stub_tstate290( 290,L"吟唱附加距离改变","神镰伐桂.dds");
TeamStateImp _stub_tstate291( 291,L"双防提高","祭月礼.dds");
TeamStateImp _stub_tstate292( 292,L"金防降低3","落月击金.dds");
TeamStateImp _stub_tstate293( 293,L"水防降低3","落月击水.dds");
TeamStateImp _stub_tstate294( 294,L"暴击回血回元","贪狼刃.dds");


SkillStub::Map& SkillStub::GetMap()
{ 
	static Map map; 
	return map; 
}

SkillStub::ID_LIST & SkillStub::GetInherentSkillList(int cls)
{
	static std::map<int, ID_LIST> map;
	return map[cls];
}

SkillStub::COMBOSK_MAP & SkillStub::GetComboSkMap()
{
	static COMBOSK_MAP map;
	return map;
}

SkillStub::~SkillStub()
{
}

const VisibleState* VisibleStateImp::Query(int prof, int id)
{
	Map::iterator it = stubmap.find(id);
	return it == stubmap.end() ? NULL : it->second;
}

const TeamState* TeamStateImp::Query(int id)
{
	Map::iterator it = stubmap.find(id);
	return it == stubmap.end() ? NULL : it->second;
}


}
