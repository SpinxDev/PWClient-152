#include "skill.h"

namespace GNET
{
TeamStateImp::Map     TeamStateImp::stubmap;
VisibleStateImp::Map  VisibleStateImp::stubmap;

VisibleStateImp _stub_vstate1(1, L"ǿ��","������ǿ.gfx","");
VisibleStateImp _stub_vstate2(2, L"����","�����½�.gfx","");
VisibleStateImp _stub_vstate3(3, L"����","������.gfx","HH_Spine");
VisibleStateImp _stub_vstate4(4, L"����","����.gfx","");
VisibleStateImp _stub_vstate5(5, L"����","����.gfx","HH_Head");
VisibleStateImp _stub_vstate6(6, L"˯��","˯��.gfx","");
VisibleStateImp _stub_vstate7(7, L"����","����.gfx","");
VisibleStateImp _stub_vstate8(8, L"���","��ӡ.gfx","");
VisibleStateImp _stub_vstate9(9, L"����","���ⲻ��״̬.gfx","HH_Head");
VisibleStateImp _stub_vstate13(10,L"�׻�","������ϵ����.gfx","HH_Head");
VisibleStateImp _stub_vstate10(11,L"�ж�","�ж�.gfx","HH_Head");
VisibleStateImp _stub_vstate11(12,L"ȼ��","�����𹥻�.gfx","HH_Head");
VisibleStateImp _stub_vstate14(13,L"��ʯ","������ϵ�˺�.gfx","");
VisibleStateImp _stub_vstate12(14,L"��Ѫ","��Ѫ.gfx","HH_Head");
VisibleStateImp _stub_vstate15(15,L"ħ��ȼ��","ħ��ȼ��.gfx","HH_Head");
VisibleStateImp _stub_vstate16(16,L"˪��","˪��״̬.gfx","HH_RighthandWeapon");
VisibleStateImp _stub_vstate17(17,L"����","����Ч��״̬.gfx","HH_RighthandWeapon");
VisibleStateImp _stub_vstate18(18,L"������","����֮ʸ.gfx","HH_RighthandWeapon");
VisibleStateImp _stub_vstate19(19,L"�ʻ�1","����ܰ����.gfx","");
VisibleStateImp _stub_vstate20(20,L"�ʻ�2","�ٺϻ���.gfx","");
VisibleStateImp _stub_vstate21(21,L"�ʻ�3","�ջ�����.gfx","");
VisibleStateImp _stub_vstate22(22,L"�ʻ�4","õ�廷��.gfx","");
VisibleStateImp _stub_vstate23(23,L"����","������״̬.gfx","HH_Head");

// ��ʦ����
VisibleStateImp _stub_vstate24(24,L"���滤��","���滤��.gfx","HH_Spine");
VisibleStateImp _stub_vstate25(25,L"��������","��������.gfx","HH_Spine");
VisibleStateImp _stub_vstate26(26,L"��ʯ����","��ʯ����.gfx","HH_Spine");
VisibleStateImp _stub_vstate27(27,L"��ˮ��","��ˮ��.gfx","HH_Spine");
// ��������
VisibleStateImp _stub_vstate28(28,L"���","���.gfx","HH_Spine");
// ��â����
VisibleStateImp _stub_vstate29(29,L"���","���.gfx","HH_Spine");
VisibleStateImp _stub_vstate30(30,L"����֮ʸ","����֮ʸ.gfx","HH_LefthandWeapon");
// ��������
VisibleStateImp _stub_vstate31(31,L"������","ħ������.gfx","HH_Spine");
VisibleStateImp _stub_vstate32(32,L"������","��������״̬.gfx","HH_Spine");
// ��������
VisibleStateImp _stub_vstate33(33,L"Ŀä","�����䱻��.gfx","");

VisibleStateImp _stub_vstate34(34, L"��ӡ����","��ӡ����.gfx","");
VisibleStateImp _stub_vstate35(35, L"˯������","˯������.gfx","");
VisibleStateImp _stub_vstate36(36, L"��ϵ����","��ϵ����.gfx","HH_Spine");
VisibleStateImp _stub_vstate37(37, L"ˮϵ����","ˮϵ����.gfx","HH_Spine");
VisibleStateImp _stub_vstate38(38, L"��ϵ����","��ϵ����.gfx","HH_Spine");
VisibleStateImp _stub_vstate39(39, L"ľϵ����","ľϵ����.gfx","HH_Spine");
VisibleStateImp _stub_vstate40(40, L"��ϵ����","��ϵ����.gfx","HH_Spine");

VisibleStateImp _stub_vstate41(41, L"�����","�����.gfx","HH_Spine");
VisibleStateImp _stub_vstate42(42, L"������","������.gfx","HH_Spine");
VisibleStateImp _stub_vstate43(43, L"Ԫ������","Ԫ������״̬.gfx","");
VisibleStateImp _stub_vstate44(44, L"�˺���������","�˺���������.gfx","");
VisibleStateImp _stub_vstate45(45, L"���� ","����.gfx","HH_Spine");
VisibleStateImp _stub_vstate46(46, L"���","���.gfx","HH_Spine");
VisibleStateImp _stub_vstate47(47, L"ˮ�𻤼�","ˮ�𻤼�.gfx","HH_Spine");
VisibleStateImp _stub_vstate48(48, L"��ʦ֮��","��ʦ֮��.gfx","HH_Spine");
VisibleStateImp _stub_vstate49(49, L"�޵�","�޵�.gfx","HH_Spine");
VisibleStateImp _stub_vstate50(50, L"��������Ԫ��","��������Ԫ��.gfx","");

VisibleStateImp _stub_vstate51(51, L"��������Ԫ��","��������Ԫ��.gfx","");
VisibleStateImp _stub_vstate52(52, L"�����ȼ�����","�����ȼ�����.gfx","");
//VisibleStateImp _stub_vstate53(53, L"�����ȼ�����","�����ȼ�����.gfx","");
VisibleStateImp _stub_vstate54(54, L"���������˺�","���������˺�.gfx","HH_Spine");
VisibleStateImp _stub_vstate55(55, L"���շ����˺�","���շ����˺�.gfx","HH_Spine");
VisibleStateImp _stub_vstate56(56, L"������Ѫ","��Ѫ����.gfx","");
VisibleStateImp _stub_vstate57(57, L"���","���.gfx","HH_Spine");
VisibleStateImp _stub_vstate58(58, L"����","����.gfx","HH_Spine");
VisibleStateImp _stub_vstate59(59, L"����","����ͽ�.gfx","HH_Spine");
VisibleStateImp _stub_vstate60(60, L"��ӡ","�����ӡ.gfx","HH_Spine");
VisibleStateImp _stub_vstate61(61, L"����","������.gfx","HH_Spine");
VisibleStateImp _stub_vstate62(62, L"����","��������.gfx","HH_Spine");
VisibleStateImp _stub_vstate63(63, L"����","������.gfx","HH_Spine");
VisibleStateImp _stub_vstate64(64, L"����������","����������.gfx","HH_Spine");
VisibleStateImp _stub_vstate65(65, L"�����ӡ��","�����ӡ��.gfx","HH_Spine");
VisibleStateImp _stub_vstate66(66, L"������","������.gfx","HH_Spine");
VisibleStateImp _stub_vstate67(67, L"������","������.gfx","HH_Spine");
VisibleStateImp _stub_vstate68(68, L"��ȱ���","��ȱ���.gfx","HH_�Ҵ��");
VisibleStateImp _stub_vstate69(69, L"���佣��","���佣��.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate70(70, L"��Ѫ����","��Ѫ����.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate71(71, L"��������","��������.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate72(72, L"�˷���Ӱ","�˷���Ӱ.gfx","HH_Spine");		
VisibleStateImp _stub_vstate73(73, L"����","����.gfx","HH_righthandweapon");				
VisibleStateImp _stub_vstate74(74, L"����","������.gfx","HH_spine");		
VisibleStateImp _stub_vstate75(75, L"��Ѫ��","�ط����.gfx","HH_spine");		
VisibleStateImp _stub_vstate76(76, L"����ľˮ���","��ľ��.gfx","HH_spine");	
VisibleStateImp _stub_vstate77(77, L"����ļ���","������.gfx","HH_spine");		
VisibleStateImp _stub_vstate78(78, L"���Լ��ĳ��ﴫ��mp","�绡ͬ����֦.gfx","");			
VisibleStateImp _stub_vstate79(79, L"�ӿ��������ӷ���","��������.gfx","HH_spine");
VisibleStateImp _stub_vstate80(80, L"�����������","ħ���������.gfx","HH_spine");			
VisibleStateImp _stub_vstate81(81, L"���ӷ�������","����ĸ����.gfx","HH_spine");			
VisibleStateImp _stub_vstate82(82, L"��ʩ����(����)�����˺�","�ֵ��˺�.gfx","HH_spine");		
VisibleStateImp _stub_vstate83(83, L"���˺����﷨��","����.gfx","HH_spine");			
VisibleStateImp _stub_vstate84(84, L"��ľ�괺","��ľ�괺.gfx","HH_spine");		
VisibleStateImp _stub_vstate86(85, L"����������������Ч������","����_���.gfx","HH_Spine");
VisibleStateImp _stub_vstate85(86, L"���𽣾�","���𽣾�.gfx","HH_righthandweapon");			
VisibleStateImp _stub_vstate87(87, L"���޵��ù�Ч","���޵�_10.gfx","HH_head");
VisibleStateImp _stub_vstate88(88, L"���޵��ù�Ч","���޵�_50.gfx","HH_head");
VisibleStateImp _stub_vstate89(89, L"���޵��ù�Ч","���޵�_100.gfx","HH_head");
VisibleStateImp _stub_vstate90(90, L"���޵��ù�Ч","���޵�_300.gfx","HH_head");
VisibleStateImp _stub_vstate91(91, L"λ�û���","��ʦ_����֮��_״̬Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate92(92, L"��������","��ʦ_����֮��2_״̬Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate93(93, L"ǿ��ѡ��Ŀ��","�׻�_��սɨ��_����Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate94(94, L"��������","����_���ٷ�����Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate95(95, L"�ܵ��չ����˲���debuff","��ʦ_��֮����_����Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate96(96, L"���ͱ�������","����_��з�����Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate97(97, L"ת�Ƽ��ܹ�������","��ʦ_����֮�ꡪ������_״̬Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate98(98, L"�ܵ�����ʱ�����ѡ����ҷ��𹥻�","�̿�_����ӡ��״̬Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate99(99, L"��ֹ��ѡ��","��ʦ_ڤ����������Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate100(100, L"�ӳ���ϵ�˺�","������ϵ�˺�.gfx","HH_Spine");			
VisibleStateImp _stub_vstate101(101, L"���ԣ������ܵ��˺�����","��ʦ_�����������Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate102(102, L"����֮��2(����)","����_������.gfx","");						
VisibleStateImp _stub_vstate103(103, L"���˺�ʱ����","��ʦ_����_״̬Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate104(104, L"���˺�ʱ����������","����_��η_״̬Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate105(105, L"��ս����","�׻�_����֮������Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate106(106, L"����͵ȡ","����_����_����Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate107(107, L"����������","�̿�_Ʈ�����Ч��.gfx","HH_Spine");			
VisibleStateImp _stub_vstate108(108, L"���ӹ�������","��â_����֮��_Ч��״̬.gfx","HH_Spine");			
VisibleStateImp _stub_vstate109(109, L"�޵�5������debuf������Ѫ���ԣ�","��ʦ_����_��������.gfx","");	
VisibleStateImp _stub_vstate110(110, L"�����","����_�׻��_����.gfx","");	
VisibleStateImp _stub_vstate111(111, L"�������й⻷��Ч","����_��������.gfx","");			
VisibleStateImp _stub_vstate112(112, L"�����������","����_���֮������.gfx","HH_Spine");		
VisibleStateImp _stub_vstate113(113, L"��ʦ�����������","��ʦ_��������_�������.gfx","");
VisibleStateImp _stub_vstate114(114, L"ս�����Ĺ�Ч","����.gfx","HH_Head");
VisibleStateImp _stub_vstate115(115, L"�����ù�Ч","С��״̬.gfx","HH_feijian");			
VisibleStateImp _stub_vstate116(116, L"�����ù�Ч","С����״̬.gfx","HH_feijian");	
VisibleStateImp _stub_vstate117(117, L"̱����Ч","̱��.gfx","HH_Head");					
VisibleStateImp _stub_vstate118(118, L"�ض���������ʱ�併�͹�Ч","�ض���������ʱ�併��.gfx","HH_feijian");	
VisibleStateImp _stub_vstate119(119, L"�ض������˺����ӹ�Ч","�ض������˺�����.gfx","HH_feijian");	
VisibleStateImp _stub_vstate120(120, L"��Ѫ���ܹ�Ч","��Ѫ����.gfx","HH_bind");
VisibleStateImp _stub_vstate121(121, L"�����ٶ���ߣ��ɵ���ˢ�³���ʱ�䣩","���Ʋ�����.gfx","HH_Spine");
VisibleStateImp _stub_vstate122( 122, L"�ƶ���Ѫ","�ƶ���Ѫ_��Ч01.gfx","HH_feijian");
VisibleStateImp _stub_vstate123( 123, L"���ƶ��͵�Ѫ","���ƶ��͵�Ѫ01.gfx","HH_bind");
VisibleStateImp _stub_vstate125( 125, L"���ߴ�ϻ���","ħ����.gfx","HH_Spine");
VisibleStateImp _stub_vstate127( 127, L"����״̬","����״̬Ч��01.gfx","HH_feijian");
VisibleStateImp _stub_vstate128( 128, L"1��","����_����1��.gfx","HH_Spine");
VisibleStateImp _stub_vstate129( 129, L"2��","����_����2��.gfx","HH_Spine");
VisibleStateImp _stub_vstate130( 130, L"3��","����_����3��.gfx","HH_Spine");
VisibleStateImp _stub_vstate131( 131, L"1��","����_����1��.gfx","HH_Spine");
VisibleStateImp _stub_vstate132( 132, L"2��","����_����2��.gfx","HH_Spine");
VisibleStateImp _stub_vstate133( 133, L"3��","����_����3��.gfx","HH_Spine");
VisibleStateImp _stub_vstate134( 134, L"1��1��","����_1��1����.gfx","HH_Spine");
VisibleStateImp _stub_vstate135( 135, L"2��1��","����_1��2��.gfx","HH_Spine");
VisibleStateImp _stub_vstate136( 136, L"2��1��","����_2��1��.gfx","HH_Spine");
VisibleStateImp _stub_vstate137( 137, L"����״̬","����01.gfx","HH_bind");
VisibleStateImp _stub_vstate138( 138, L"��������ʩ������ȴ","��������ʩ������ȴʱ��01.gfx","HH_Spine");
VisibleStateImp _stub_vstate139( 139, L"�չ������˺�","�չ����Ӷ����˺�01.gfx","HH_shou03");
VisibleStateImp _stub_vstate140( 140, L"����CD�ı�","ҹӰ_Ӱ��_״̬Ч��01.gfx","HH_feijian");
VisibleStateImp _stub_vstate141( 141, L"Х��","Х��_ʩ����01.gfx","HH_feijian");


TeamStateImp _stub_tstate1( 1,L"����\r�޷������κ��ƶ��Ͳ���", "����.dds");
TeamStateImp _stub_tstate2( 2,L"˯��\r��˯״̬���ܹ�����ָ�", "˯��.dds");
TeamStateImp _stub_tstate3( 3,L"����\r�����ƶ��ٶ�", "����.dds");
TeamStateImp _stub_tstate4( 4,L"������\r�������н��������˺�", "����������������.dds");
TeamStateImp _stub_tstate5( 5,L"���\r������ֵ����һ���˺�", "���.dds");

TeamStateImp _stub_tstate6( 6,L"���滤��\r���ӻ������������ָ��ٶ�", "���滤��.dds");
TeamStateImp _stub_tstate7( 7,L"��������\r����ˮ�������ӷ����ָ��ٶ�", "��������.dds");
TeamStateImp _stub_tstate8( 8,L"����\r�޷��ƶ�", "����.dds");
TeamStateImp _stub_tstate9( 9,L"��ӡ\r�޷�ʹ���κ��չ��ͼ���", "���ܹ���.dds");
TeamStateImp _stub_tstate10( 10,L"�Ȼ�\rĿ������ɱ�������", "�����䱻��.dds");

TeamStateImp _stub_tstate11( 11,L"��ʯ����\r�������������", "��ʯ����.dds");
TeamStateImp _stub_tstate12( 12,L"�׻�\r�����ܵ���ϵ�˺�", "������ϵ����.dds");
TeamStateImp _stub_tstate13( 13,L"�ж�\r�����ܵ�ľϵ�˺�", "�ж�.dds");
TeamStateImp _stub_tstate14( 14,L"ȼ��\r�����ܵ���ϵ�˺�", "�����𹥻�.dds");
TeamStateImp _stub_tstate15( 15,L"ɳ��\r�����ܵ���ϵ�˺�", "������ϵ�˺�.dds");

TeamStateImp _stub_tstate16( 16,L"����ȼ��\r������������", "����MP����״̬.dds");
TeamStateImp _stub_tstate17( 17,L"��Ѫ\r�����ܵ������˺�", "��Ѫ.dds");
TeamStateImp _stub_tstate18( 18,L"�Ƽ�\r����������½�", "����������½�.dds");
TeamStateImp _stub_tstate19( 19,L"����\r�����������½�", "���з������½�.dds");
TeamStateImp _stub_tstate20( 20,L"ʴ��\r���������½�", "���������½�.dds");

TeamStateImp _stub_tstate21( 21,L"����\r�ܵ��˺�����", "�����ܵ��������˺��ӱ�.dds");
TeamStateImp _stub_tstate22( 22,L"�ٻ�\r�����ٶȼ���", "��ͨ��������ӳ�.dds");
TeamStateImp _stub_tstate23( 23,L"�ٶ�\r��������", "��������ʱ��ӳ�.dds");
TeamStateImp _stub_tstate24( 24,L"Ŀä\r����׼ȷ��", "׼ȷ���½�.dds");
TeamStateImp _stub_tstate25( 25,L"��׾\r�������½�", "�������½�.dds");

TeamStateImp _stub_tstate26( 26,L"�����ָ��ӿ�", "HP�ָ��ӿ�.dds");
TeamStateImp _stub_tstate27( 27,L"�����ָ��ӿ�", "MP�ָ��ӿ�.dds");
TeamStateImp _stub_tstate28( 28,L"������������", "����HP����.dds");
TeamStateImp _stub_tstate29( 29,L"������������", "���MP����.dds");
TeamStateImp _stub_tstate30( 30,L"���\r������������", "������������.dds");

TeamStateImp _stub_tstate31( 31,L"����\r�������������", "�������������.dds");
TeamStateImp _stub_tstate32( 32,L"����\r�����������", "�����������.dds");
TeamStateImp _stub_tstate33( 33,L"Ѹ��\r�����ٶȼӿ�", "�����ٶȼӿ�.dds");
TeamStateImp _stub_tstate34( 34,L"����\r��߶�����", "��߶�����.dds");
TeamStateImp _stub_tstate35( 35,L"ף��\r�ܵ��˺�����", "�����˺�����.dds");

TeamStateImp _stub_tstate36( 36,L"����\r���׼ȷ��", "��͸.dds");
TeamStateImp _stub_tstate37( 37,L"רע\r�ӿ������ٶ�", "�ӿ�����.dds");
TeamStateImp _stub_tstate38( 38,L"��������ˮ�˺�", "ʹ��ͨ��������ˮϵ�˺�.dds");
TeamStateImp _stub_tstate39( 39,L"�������ӻ��˺�", "ʹ��ͨ�������ӻ�ϵ�˺�.dds");
TeamStateImp _stub_tstate40( 40,L"��������ľ�˺�", "ʹ��ͨ��������ľϵ�˺�.dds");

TeamStateImp _stub_tstate41( 41,L"�����ָ�����", "�����ָ�HP.dds");
TeamStateImp _stub_tstate42( 42,L"�����ָ�����", "�����ָ�MP.dds");
TeamStateImp _stub_tstate43( 43,L"ɢ��\r��������������", "�����Ҳ��ܹ���.dds");
TeamStateImp _stub_tstate44( 44,L"�ƶ�����", "����.dds");
TeamStateImp _stub_tstate45( 45,L"�������޽���", "HP�����½�.dds");

TeamStateImp _stub_tstate46( 46,L"����\r�������������", "�������������.dds");
TeamStateImp _stub_tstate47( 47,L"�׻���", "�׻���.dds");
TeamStateImp _stub_tstate48( 48,L"��ϵ��������", "��ϵ��������.dds");
TeamStateImp _stub_tstate49( 49,L"ľϵ��������", "ľϵ��������.dds");
TeamStateImp _stub_tstate50( 50,L"ˮϵ��������", "ˮϵ��������.dds");

TeamStateImp _stub_tstate51( 51,L"��ϵ��������", "��ϵ��������.dds");
TeamStateImp _stub_tstate52( 52,L"��ϵ��������", "��ϵ��������.dds");
TeamStateImp _stub_tstate53( 53,L"��ϵ�����½�", "��ϵ�����½�.dds");
TeamStateImp _stub_tstate54( 54,L"ľϵ�����½�", "ľϵ�����½�.dds");
TeamStateImp _stub_tstate55( 55,L"ˮϵ�����½�", "ˮϵ�����½�.dds");

TeamStateImp _stub_tstate56( 56,L"��ϵ�����½�", "��ϵ�����½�.dds");
TeamStateImp _stub_tstate57( 57,L"��ϵ�����½�", "��ϵ�����½�.dds");
TeamStateImp _stub_tstate58( 58,L"��ˮ��\rˮ�в��ú������޷�ʹ�ü���", "��ˮ��.dds");
TeamStateImp _stub_tstate59( 59,L"�����ľ�\r���з�����������", "�����ľ�.dds");
TeamStateImp _stub_tstate60( 60,L"��Ԫ����\r��ȫ��Ԫ������������������", "��Ԫ����.dds");
TeamStateImp _stub_tstate61( 61,L"���辭\r�����������������", "���辭.dds");
TeamStateImp _stub_tstate62( 62,L"�׽\r�������������������", "�׽.dds");
TeamStateImp _stub_tstate63( 63,L"����֮ŭ\r�ܵ�����ʱ����Ԫ��", "����֮ŭ.dds");
TeamStateImp _stub_tstate64( 64,L"���\r������������", "��׷�.dds");
TeamStateImp _stub_tstate65( 65,L"������\r�������", "������.dds");
TeamStateImp _stub_tstate66( 66,L"��������\r���������", "��������.dds");
TeamStateImp _stub_tstate67( 67,L"��Ѫ��ħ\r����������������������", "��Ѫ��ħ.dds");
TeamStateImp _stub_tstate68( 68,L"����\r�������������", "������.dds");
TeamStateImp _stub_tstate69( 69,L"���\r����һ���˺����ظ�����", "���.dds");
TeamStateImp _stub_tstate70( 70,L"����֮ʸ\r��������һ�����˺�", "����֮ʸ.dds");
TeamStateImp _stub_tstate71( 71,L"������ӥ\r�����ܵ���ϵ�˺�������", "������ӥ.dds");
TeamStateImp _stub_tstate72( 72,L"���������ж��谭״̬", "׼�޵�.dds");
TeamStateImp _stub_tstate73( 73,L"�������˺�", "�������˺�.dds");
TeamStateImp _stub_tstate74( 74,L"����������", "����������.dds");

TeamStateImp _stub_tstate75( 75,L"������\r�������������������ֵ�½�", "�����.dds");
TeamStateImp _stub_tstate76( 76,L"�޵�", "�޵�.dds");
TeamStateImp _stub_tstate77( 77,L"������\r���������˺������ҷ��������˺�", "������.dds");
TeamStateImp _stub_tstate78( 78,L"������\r����������ֵֹͣ�ظ�", "������.dds");
TeamStateImp _stub_tstate79( 79,L"������\r�ܵ������˺�", "������.dds");
TeamStateImp _stub_tstate80( 80,L"������\r�ܵ�����ʧȥԪ��", "������.dds");
TeamStateImp _stub_tstate81( 81,L"�ɻ���Ӱ\r�ƶ�����", "��������.dds");
TeamStateImp _stub_tstate82( 82,L"��˼���", "��˼���.dds");
TeamStateImp _stub_tstate83( 83,L"���������", "�����������.dds");

TeamStateImp _stub_tstate84( 84,L"רע\r����רע�ľ������ʹ���ʩ���������š�\rĿ�����߷�ӡ", "רע.dds");
TeamStateImp _stub_tstate85( 85,L"����\r���ܵ����ǲ���Ҫ˯����\r����˯��״̬", "����.dds");
TeamStateImp _stub_tstate86( 86,L"����\rֻҪ����һ�����ɵ��ģ�\r��Ͳ��ᱻ���٣����߶���\r�����ƶ����ٺͻ���", "����.dds");
TeamStateImp _stub_tstate87( 87,L"��ǿ\r���۰�������Ѫ��\r��ǿ�����ܲ��ᱻ���", "��ǿ.dds");
TeamStateImp _stub_tstate88( 88,L"��ʥ\r�������и���״̬", "��ʥ.dds");
TeamStateImp _stub_tstate89( 89,L"����\r���������˺�", "����.dds");
TeamStateImp _stub_tstate90( 90,L"����\r���߻�ϵ�˺�", "����.dds");

TeamStateImp _stub_tstate91( 91,L"ˮ��\r����ˮϵ�˺�", "ˮ��.dds");
TeamStateImp _stub_tstate92( 92,L"����\r���߽�ϵ�˺�", "����.dds");
TeamStateImp _stub_tstate93( 93,L"ľ��\r����ľϵ�˺�", "ľ��.dds");
TeamStateImp _stub_tstate94( 94,L"����\r������ϵ�˺�", "����.dds");
TeamStateImp _stub_tstate95( 95,L"����\r���������˺�", "����.dds");
TeamStateImp _stub_tstate96( 96,L"����\r��������״̬", "����.dds");
TeamStateImp _stub_tstate97( 97,L"ˮ�м���\r��Ӿ����", "ˮ�м���.dds");
TeamStateImp _stub_tstate98( 98,L"ˮ�м���\r��Ӿ����", "ˮ�м���.dds");
TeamStateImp _stub_tstate99( 99,L"���м���\r���м���", "���м���.dds");
TeamStateImp _stub_tstate100( 100,L"���м���\r���м���", "���м���.dds");

TeamStateImp _stub_tstate101( 101,L"��˼���\r��˼���", "��˼���.dds");
TeamStateImp _stub_tstate102( 102,L"����\r��������Ԫ��", "����.dds");
TeamStateImp _stub_tstate103( 103,L"ƣ��\r��������Ԫ��", "ƣ��.dds");
TeamStateImp _stub_tstate104( 104,L"����֮��\r����ʱ��������С��������", "����֮��.dds");
TeamStateImp _stub_tstate105( 105,L"����֮��\r����ʱ��������С���������", "����֮��.dds");

TeamStateImp _stub_tstate106( 106,L"����\r��������ͷ��˿���,\r�Լ�Ҳͬʱ�ܵ��˷�ı�����\r���������������", "����.dds");
TeamStateImp _stub_tstate107( 107,L"Ԫ������\rĿ�������������,������������.", "Ԫ������.dds");
TeamStateImp _stub_tstate108( 108,L"�\rĿ�����˺�����20%��\r���Ч�����Ժ��������ȼ��ܵ���", "�.dds");
TeamStateImp _stub_tstate109( 109,L"����\r������ͬʱ�˺�����ͬʱ�����˺�", "����.dds");
TeamStateImp _stub_tstate110( 110,L"����ػ�\rÿ3��ָ�HP,�˺�����.����ͷ�����������", "����ػ�.dds");

TeamStateImp _stub_tstate111( 111,L"��\rĿ������ͷ�����������,ͬʱ�����ȼ�����", "��.dds");
TeamStateImp _stub_tstate112( 112,L"ɳ��\r���н���,ʩ���ٶȱ���", "ɳ��.dds");
TeamStateImp _stub_tstate113( 113,L"����\r�����������ظ��ٶ�����,�ƶ��ٶ�����", "����.dds");
TeamStateImp _stub_tstate114( 114,L"����\rˮϵ��������", "����.dds");
TeamStateImp _stub_tstate115( 115,L"����\r��������", "����.dds");
TeamStateImp _stub_tstate116( 116,L"���\r�����������", "���.dds");
TeamStateImp _stub_tstate117( 117,L"��\r��ϵ��������", "��.dds");
TeamStateImp _stub_tstate118( 118,L"�Ļ�\r����ͷ��ٹ����ٶȽ���", "�Ļ�.dds");
TeamStateImp _stub_tstate119( 119,L"����\r�������˺�", "����.dds");
TeamStateImp _stub_tstate120( 120,L"ȼ��\r��ϵ��������,�ƶ��ٶȽ���", "ȼ��.dds");

TeamStateImp _stub_tstate121( 121,L"���\r��������ͷ����˺�,���ҽ��������ƶ��ٶ�", "���.dds");
TeamStateImp _stub_tstate122( 122,L"����\r��ϵ��������", "����.dds");
TeamStateImp _stub_tstate123( 123,L"Ҷ��\r�ƶ��ٶȽ��Ͳ�����Ѫ", "Ҷ��.dds");
TeamStateImp _stub_tstate124( 124,L"�ս�\r�����ȼ��½�.�����ȼ��½�,\r�ƶ��ٶ���������", "�ս�.dds");
TeamStateImp _stub_tstate125( 125,L"���\r�ͷ��ٶȱ���,�ƶ��ٶȱ���.�ܵ��˺�����", "���.dds");

TeamStateImp _stub_tstate126( 126,L"ֹѪ\r������Ѫ״̬��", "ֹѪ.dds");
TeamStateImp _stub_tstate127( 127,L"��ղ���\r�����ܵ��������˺�", "��ղ���.dds");
TeamStateImp _stub_tstate128( 128,L"�����ޱ�\r�����ܵ������˺�", "�����ޱ�.dds");
TeamStateImp _stub_tstate129( 129,L"��������\r�����ܵ��ķ����˺�", "��������.dds");
TeamStateImp _stub_tstate130( 130,L"�����\r�ܵ��˺�����,�������ٶȺ���������", "�����.dds");

TeamStateImp _stub_tstate131( 131,L"������\rĿ�궨�����м��ʱ���ӡ", "������.dds");
TeamStateImp _stub_tstate132( 132,L"���\r��ӡ�������˺�����", "���.dds");
TeamStateImp _stub_tstate133( 133,L"����\r�����˺�����״̬��", "����.dds");
TeamStateImp _stub_tstate134( 134,L"����\r�޷��ж�", "����.dds");
TeamStateImp _stub_tstate135( 135,L"�����ȼ�����", "�����ȼ�����.dds");

TeamStateImp _stub_tstate136( 136,L"�����ȼ�����", "�����ȼ�����.dds");
TeamStateImp _stub_tstate137( 137,L"�����ȼ�����", "�����ȼ�����.dds");
TeamStateImp _stub_tstate138( 138,L"�����ȼ�����", "�����ȼ�����.dds");

TeamStateImp _stub_tstate139( 139,L"�����ܵ���ϵ�˺�", "�������˺�2.dds");
TeamStateImp _stub_tstate140( 140,L"��ӡ\r�޷�ʹ���κ��չ��ͼ���", "���ܹ���2.dds");

TeamStateImp _stub_tstate141( 141,L"����\r�޷��ƶ�", "����2.dds");
TeamStateImp _stub_tstate142( 142,L"ף��\r�ܵ��˺�����", "�����˺�����2.dds");
TeamStateImp _stub_tstate143( 143,L"����\r�ܵ��˺�����", "�˺�����2.dds");
TeamStateImp _stub_tstate144( 144,L"������������", "������������2.dds");
TeamStateImp _stub_tstate145( 145,L"����\r�����������", "����������2.dds");
TeamStateImp _stub_tstate146( 146,L"����\r�������������", "������������2.dds");
TeamStateImp _stub_tstate147( 147,L"רע\r�ӿ�����", "��������2.dds");
TeamStateImp _stub_tstate148( 148,L"�ƶ�����", "�ƶ�����2.dds");
TeamStateImp _stub_tstate149( 149 ,L"����\r�������״̬", "����.dds");
TeamStateImp _stub_tstate150( 150 ,L"���������ȼ�\r��߷����ȼ�", "���о���.dds");
TeamStateImp _stub_tstate151( 151 ,L"��Ѫ��\rÿ�ι���ʱ����������ɵ��˺��лָ�һ��������", "��Ѫ�⻷.dds");
TeamStateImp _stub_tstate152( 152 ,L"����ӡ\r��߱����˺�ֵ","�����˺�.dds");
TeamStateImp _stub_tstate153( 153 ,L"�����ӡ��\r������ʱ���м������ܹ������ܵ��˺���", "�˺�����.dds");
TeamStateImp _stub_tstate154( 154 ,L"����������\r������ʱ���м������ܹ������ܵ�Ч����", "״̬����.dds");
TeamStateImp _stub_tstate155( 155 ,L"����������\r���һ��������������ָ�����", "�عⷵ��.dds");
TeamStateImp _stub_tstate156( 156 ,L"�ͽ�\r�򹥻��߷����˺�", "����֮��.dds");
TeamStateImp _stub_tstate157( 157 ,L"��ӡ\rʹ�����������ӡ״̬", "��ӡ֮��.dds");
TeamStateImp _stub_tstate158( 158 ,L"����\r������ͬʱ�����似����ɵ��˺���״̬", "����֮��.dds");
TeamStateImp _stub_tstate159( 159 ,L"����\r�������߻���", "����֮��.dds");
TeamStateImp _stub_tstate160( 160 ,L"��������\r�����ɼ��ܺ�ҩƷ��õ������ָ�Ч���������ٺ����ȴʱ��", "ף������.dds");
TeamStateImp _stub_tstate161( 161 ,L"��������\r�����ɼ��ܺ�ҩƷ��õ������ָ�Ч�������ӳ������ȴʱ��", "ף������.dds");
TeamStateImp _stub_tstate162( 162 ,L"����\rÿ����ͨ������ʩ�ż���ʱ�������ܵ��˺�", "ʹ�ü���ʱ���˺�.dds");
TeamStateImp _stub_tstate163( 163 ,L"ʹ�ü���ʱ���ж�\rʹĿ��ÿ��ʩ�ż���ʱ�����ᱻ���", "����.dds");
TeamStateImp _stub_tstate164( 164 ,L"�����\r���������", "ħ����.dds");
TeamStateImp _stub_tstate165( 165 ,L"��ȱ���\r���͹��٣���߹����ȼ�", "����.dds");
TeamStateImp _stub_tstate166( 166 ,L"��ʦ��\r���������ȼ�", "��Ѫ.dds");
TeamStateImp _stub_tstate167( 167 ,L"��ս�ӡ\r��ϵ������ϵ�����˺������ӳ�Ч��", "��ս�ӡ.dds");			
TeamStateImp _stub_tstate168( 168 ,L"������ӡ\rľϵ��ˮϵ�����˺������ӳ�Ч��", "������ӡ.dds");			
TeamStateImp _stub_tstate169( 169 ,L"���Ľ�ӡ\r��ϵ����ϵ�����˺������ӳ�Ч��", "���Ľ�ӡ.dds");
TeamStateImp _stub_tstate170( 170 ,L"���佣��\r��Ч�˺��м��ʴ��������佣�ۡ�", "���佣��.dds");			
TeamStateImp _stub_tstate171( 171 ,L"��Ѫ����\r��Ч�˺��м��ʴ�������Ѫ���ۡ�", "��Ѫ����.dds");			
TeamStateImp _stub_tstate172( 172 ,L"��������\r��Ч�˺��м��ʴ������������ۡ�", "��������.dds");			
TeamStateImp _stub_tstate173( 173 ,L"���佣��\r�ض����ܼ���ý���", "���佣��.dds");			
TeamStateImp _stub_tstate174( 174 ,L"��Ѫ����\r�ض����ܼ���ý���", "��Ѫ����.dds");			
TeamStateImp _stub_tstate175( 175 ,L"��������\r�ض����ܼ���ý���", "��������.dds");			
TeamStateImp _stub_tstate176( 176 ,L"�˷���Ӱ\r����ΧĿ������˺�", "�˷���Ӱ.dds");	
TeamStateImp _stub_tstate177( 177 ,L"����\rѪ������50%ʱ���������˺�", "������.dds");
TeamStateImp _stub_tstate178( 178 ,L"��Ѫ��\r�ܵ��˺�����������ѪЧ��", "�ط����.dds");
TeamStateImp _stub_tstate179( 179 ,L"����ľˮ���\r���ľ��ˮ�������", "��ľ��.dds");
TeamStateImp _stub_tstate180( 180 ,L"����ļ���\r�������ܵ��������ǹ���ʱ�˺�����", "������.dds");
TeamStateImp _stub_tstate181( 181 ,L"�����������\r������������������", "ħ���������.dds");
TeamStateImp _stub_tstate182( 182 ,L"���ӷ�������\r������з���������������", "����ĸ����.dds");
TeamStateImp _stub_tstate183( 183 ,L"���Լ��ĳ��ﴫ��mp\r����������������ٻ���", "ͬ����֦.dds");
TeamStateImp _stub_tstate184( 184 ,L"��ʩ����(����)�����˺�\rʩ���߽�Ϊ״̬�����߷ֵ��˺�", "�ֵ��˺�.dds");
TeamStateImp _stub_tstate185( 185 ,L"���˺����﷨��\r������һ�����˺��һ��ܴ���ʱ��߷�����", "����.dds");
TeamStateImp _stub_tstate186( 186 ,L"�ӿ��������ӷ���\r��������ٶ���߷���", "��������.dds");
TeamStateImp _stub_tstate187( 187 ,L"�����Ļ�Ѫ\r��ֵ�����ĳ�����Ѫ", "����֮��.dds");
TeamStateImp _stub_tstate188( 188 ,L"�׼�\r�׼��⻷", "����.dds");
TeamStateImp _stub_tstate189( 189 ,L"��ľ�괺\r��������������", "��ľ�괺.dds");
TeamStateImp _stub_tstate190( 190 ,L"������Ʒ�˺�������\r�ܵ�����乥���˺���ǿ", "��������.dds");
TeamStateImp _stub_tstate191( 191 ,L"����������������Ч������", "��������.dds");	
TeamStateImp _stub_tstate192( 192 ,L"���𽣾�\r�м��ʴ���������", "���𽣾�.dds");			
TeamStateImp _stub_tstate193( 193 ,L"������\r�����⹥�����д���Ч��", "������.dds");			
TeamStateImp _stub_tstate194( 194 ,L"����ʧЧ", "����ʧЧ.dds");			
TeamStateImp _stub_tstate195( 195 ,L"���\r�����ܵ��˺�ʱ�ĳ��", "�䳰�ȷ�.dds");
TeamStateImp _stub_tstate196( 196 ,L"��ǿ���ܶ�npc���˺�\r���Ӽ��ܶ�NPC���˺�", "��ǿ�㼼�ܶ�NPC���˺�.dds");			
TeamStateImp _stub_tstate197( 197 ,L"�����Χ�������\r����Χ������λ��ӵ�������", "�����Χ��������.dds");			
TeamStateImp _stub_tstate198( 198 ,L"MP�ָ��ӿ�2\r������MP�ָ��ӿ�", "��������.dds");
TeamStateImp _stub_tstate199( 199 ,L"λ�û���\r������󱻴��ر��λ��", "����֮��.dds");			
TeamStateImp _stub_tstate200( 200 ,L"��������\r�������Ѫ����ر��Ѫ��", "����֮��2buff.dds");			
TeamStateImp _stub_tstate201( 201 ,L"��ֹ����\rʹ�����һ��ʱ�����޷�����", "����֮��buff.dds");			
TeamStateImp _stub_tstate202( 202 ,L"ǿ��ѡ��Ŀ��\rʹ�����һ��ʱ����ֻ��ѡ��ʩ����", "��սɨ��buff.dds");			
TeamStateImp _stub_tstate203( 203 ,L"��������\r����һ����������", "��������buff.dds");			
TeamStateImp _stub_tstate204( 204 ,L"�ܵ��չ����˲���debuff\r�ܹ���������", "��֮����.dds");			
TeamStateImp _stub_tstate205( 205 ,L"���ӱ�������\r�ܵ�����һ���ļ��ʽ���", "ɳ��buff.dds");			
TeamStateImp _stub_tstate206( 206 ,L"���ͱ�������\r�ܵ�����һ���ļ������", "��նbuff.dds");			
TeamStateImp _stub_tstate207( 207 ,L"ת�Ƽ��ܹ�������\r������ת�Ƹ�ʩ����", "����֮��buff.dds");
TeamStateImp _stub_tstate208( 208 ,L"������ʱ�����ѡ���������\r��������", "��������buff.dds");				
TeamStateImp _stub_tstate209( 209 ,L"�ܵ�����ʱ�����ѡ����ҷ��𹥻�\r�˺�����", "���ӡ��buff.dds");			
TeamStateImp _stub_tstate211( 211 ,L"��ֹ��ѡ��\rʹ�����޷�ֱ��", "ڤ������buff.dds");						
TeamStateImp _stub_tstate212( 212 ,L"��ǿ���ܶ�npc���˺�2\r���Ժ�1����", "��ɫ��buff.dds");			
TeamStateImp _stub_tstate213( 213 ,L"�ӳ���ϵ�˺�\r������������ϵ�˺�", "������buff.dds");			
TeamStateImp _stub_tstate214( 214 ,L"���ԣ������ܵ��˺�����\r�����⻷Ч��", "��������.dds");			
TeamStateImp _stub_tstate215( 215 ,L"����֮��2(����)\r������", "���buff.dds");					
TeamStateImp _stub_tstate216( 216 ,L"���˺�ʱ����\r�ܵ��˺�ʱ��һ�����ʶ���", "����.dds");	
TeamStateImp _stub_tstate217( 217 ,L"���˺�ʱ���Ԫ��\r���Ԫ��", "���Ԫ��buff.dds");
TeamStateImp _stub_tstate218( 218 ,L"���˺�ʱ����������\r����ʵ��Ч��ͼ��", "��ηbuff.dds");						
TeamStateImp _stub_tstate219( 219 ,L"���˺�ʱ����������\r��ս����", "��ηbuff.dds");			
TeamStateImp _stub_tstate220( 220 ,L"��ս����\r�������޵���Ѫ�����޼���99%", "��Хbuff.dds");			
TeamStateImp _stub_tstate221( 221 ,L"����͵ȡ\r���Ʊ�ת�Ƶ�ʩ����", "����buff.dds");			
TeamStateImp _stub_tstate222( 222 ,L"����������\r�ڴ�״̬�±�ɱ���������", "Ʈ��buff.dds");			
TeamStateImp _stub_tstate223( 223 ,L"���ӹ�������\r�����������", "��������buff.dds");			
TeamStateImp _stub_tstate224( 224 ,L"�޵�5������debuf������Ѫ���ԣ�\r����", "����buff.dds");
TeamStateImp _stub_tstate225( 225 ,L"�����\r�������", "�׻�׹buff.dds");	
TeamStateImp _stub_tstate226( 226 ,L"�������й⻷\r���й⻷ͼ��", "����.dds");					
TeamStateImp _stub_tstate227( 227 ,L"��������\r���������Чͼ��", "���֮��buff.dds");
TeamStateImp _stub_tstate229( 229 ,L"���ٽ�ս�չ��˺�\r�����Ż� ", "������.dds");	
TeamStateImp _stub_tstate230( 230 ,L"ȫ��������\r�ȼ����� ", "����֮��.dds");			
TeamStateImp _stub_tstate231( 231 ,L"��ս���ָ�\r��ս��״̬�ָ�HPMP", "�����ӡbuff.dds");			
TeamStateImp _stub_tstate232( 232 ,L"��ֵ����\r�̶�ֵ��HPMP����", "��з�.dds");
TeamStateImp _stub_tstate233( 233 ,L"�����ܵ��˺�����ɵ��ӿ�ˢ�£�", "����ն.dds");
TeamStateImp _stub_tstate234( 234 ,L"PK�޳ͷ�״̬", "ս��״̬.dds");
TeamStateImp _stub_tstate235( 235 ,L"�������������2", "�������������.dds");		
TeamStateImp _stub_tstate236( 236 ,L"ս������BUFF", "����BUFF.dds");
TeamStateImp _stub_tstate237( 237 ,L"�����ȼ�����", "�����ȼ�����.dds");
TeamStateImp _stub_tstate238( 238 ,L"�����﷨����hp", "����֮��.dds");
TeamStateImp _stub_tstate239( 239 ,L"��������", "��������.dds");
TeamStateImp _stub_tstate240( 240 ,L"������ħ��ħ�ȼ�\r�ٷֱ���߶Թ��﹥��", "��������.dds");
TeamStateImp _stub_tstate241( 241 ,L"����ֵ�������ȼ��������ȼ����", "������ս.dds");
TeamStateImp _stub_tstate242( 242 ,L"���ӹ����ȼ��������ȼ���3��", "GT�߼�.dds");
TeamStateImp _stub_tstate243( 243 ,L"���ӹ����ȼ��������ȼ���1��", "GT�ͼ�.dds");
TeamStateImp _stub_tstate244( 244 ,L"�˺�����3\r�ܵ���������ҵ��˺�����", "��������.dds");
TeamStateImp _stub_tstate245( 245 ,L"����ʱ���Լ����״̬��", "���ؼ汸.dds");	
TeamStateImp _stub_tstate246( 246 ,L"����ʱ��Ŀ�����״̬��", "��ʤ׷��.dds");
TeamStateImp _stub_tstate247( 247 ,L"���������˺�", "����2.dds");
TeamStateImp _stub_tstate248( 248 ,L"���߽�ϵ�˺�", "����2.dds");
TeamStateImp _stub_tstate249( 249 ,L"����ľϵ�˺�", "ľ��2.dds");
TeamStateImp _stub_tstate250( 250 ,L"����ˮϵ�˺�", "ˮ��2.dds");
TeamStateImp _stub_tstate251( 251 ,L"���߻�ϵ�˺�", "����2.dds");
TeamStateImp _stub_tstate252( 252 ,L"������ϵ�˺�", "����2.dds");
TeamStateImp _stub_tstate253( 253 ,L"������\r�������н��������˺�", "����������������.dds");
TeamStateImp _stub_tstate254( 254 ,L"���ӹ����ȼ��������ȼ�", "��Ѫ.dds");
TeamStateImp _stub_tstate255( 255,L"̱��", "̱��buff.dds");
TeamStateImp _stub_tstate256( 256,L"�ܵ��ļ����˺����", "�ܵ��ļ����˺����buff.dds");
TeamStateImp _stub_tstate257( 257,L"�ܵ��ļ��ܱ��������", "�ܵ��ļ��ܱ��������buff.dds");
TeamStateImp _stub_tstate258( 258,L"�ض���������ʱ�併��", "�ض���������ʱ�併��buff.dds");
TeamStateImp _stub_tstate259( 259,L"�ض������˺�����", "�ض������˺�����buff.dds");
TeamStateImp _stub_tstate260( 260,L"��������2", "��������.dds");
TeamStateImp _stub_tstate261( 261,L"���滤��2", "���滤��.dds");
TeamStateImp _stub_tstate262( 262,L"��Ѫ����", "��Ѫ����buff.dds");
TeamStateImp _stub_tstate263( 263,L"�����ٶ���ߣ��ɵ���ˢ�³���ʱ�䣩", "׼�޵�.dds");
TeamStateImp _stub_tstate264( 264,L"����ֵ���", "����ֵ���.dds");
TeamStateImp _stub_tstate265( 265,L"�����Ǽ�����", "����.dds");
TeamStateImp _stub_tstate266( 266,L"�ƶ���Ѫ", "����󸲽.dds");
TeamStateImp _stub_tstate267( 267,L"���ƶ��͵�Ѫ", "�ƶ���Ѫ.dds");
TeamStateImp _stub_tstate268( 268,L"���ƶ��ͻ�ü���", "���ƶ��͵�.dds");
TeamStateImp _stub_tstate269( 269,L"���ߴ�ϻ���", "���ߴ�ϻ���.dds");
TeamStateImp _stub_tstate270( 270,L"����̱��", "ʴ���¶�.dds");
TeamStateImp _stub_tstate271( 271,L"����", "�������.dds");
TeamStateImp _stub_tstate272( 272,L"�չ�����", "�㺮֮��.dds");
TeamStateImp _stub_tstate273( 273,L"����������ʩ���߼�����ȴʱ��", "������.dds");
TeamStateImp _stub_tstate274( 274,L"ָ��������ɶ��⼼��Ч��", "��ʳ.dds");
TeamStateImp _stub_tstate275( 275,L"��ͨ�������Ӷ����˺�", "������.dds");
TeamStateImp _stub_tstate276( 276,L"ָ������ʹ�ú󲻽�����ȴ", "���ƺ�.dds");
TeamStateImp _stub_tstate277( 277,L"����", "����.dds");
TeamStateImp _stub_tstate278( 278,L"ҹӰ����", "��ҹӰ.dds");
TeamStateImp _stub_tstate279( 279,L"���ɱ���", "������.dds");
TeamStateImp _stub_tstate280( 280,L"����˪��״̬", "˪����.dds");
TeamStateImp _stub_tstate281( 281,L"����֮��", "����֮��.dds");
TeamStateImp _stub_tstate282( 282,L"��ǿ���ܶ�npc���˺�3", "��ӽ.dds");
TeamStateImp _stub_tstate283( 283,L"�����ܵ��ı����˺�","�����ܵ��ı����˺�.dds");
TeamStateImp _stub_tstate284( 284,L"�����˺����","��������˺����2.dds");
TeamStateImp _stub_tstate285( 285,L"�����˺����","��ɷ����˺����2.dds");
TeamStateImp _stub_tstate286( 286,L"�ܵ��ļ����˺����2", "��������.dds");
TeamStateImp _stub_tstate287( 287,L"������ռλ״̬��", "");
TeamStateImp _stub_tstate288( 288,L"����CD�ı�","Ӱ��.dds");
TeamStateImp _stub_tstate289( 289,L"��������","��������.dds");
TeamStateImp _stub_tstate290( 290,L"�������Ӿ���ı�","��������.dds");
TeamStateImp _stub_tstate291( 291,L"˫�����","������.dds");
TeamStateImp _stub_tstate292( 292,L"�������3","���»���.dds");
TeamStateImp _stub_tstate293( 293,L"ˮ������3","���»�ˮ.dds");
TeamStateImp _stub_tstate294( 294,L"������Ѫ��Ԫ","̰����.dds");


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
