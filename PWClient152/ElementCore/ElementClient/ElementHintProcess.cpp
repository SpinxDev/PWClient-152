
#include "ElementHintProcess.h"
#include <Windows.h>
#include <WinBase.h>
#include "./GTransForm/groleinventory"
#include "./GTransForm/gwebtraderolebrief"
#include "./GTransForm/gwebtraderolebriefextend"
#include "./GTransForm/getitemdesc.hpp"
#include "./GTransForm/getitemdesc_re.hpp"
#include "./GTransForm/getencrypteditemdesc.hpp"
#include "./GTransForm/getencrypteditemdesc_re.hpp"
#include "./GTransForm/getroledesc.hpp"
#include "./GTransForm/getroledesc_re.hpp"
#include "./GTransForm/getroleequipmentdesc.hpp"
#include "./GTransForm/getroleequipmentdesc_re.hpp"
#include "./GTransForm/getrolepetcorraldesc.hpp"
#include "./GTransForm/getrolepetcorraldesc_re.hpp"
#include "./GTransForm/getroleskilldesc.hpp"
#include "./GTransForm/getroleskilldesc_re.hpp"
#include "./GTransForm/gpetcorral"
#include "./GTransForm/gpet"
#include "./GTransForm/snsroleequipment"
#include "./GTransForm/snsrolepetcorral"
#include "./GTransForm/snsroleskills"
#include "./GTransForm/gtransformclient.hpp"
#include "./GTransForm/gforcedatalist"
#include "./GTransForm/greincarnationrecord"
#include "./GTransForm/greincarnationdata"
#include "./GTransForm/gmeridiandata"
#include "./GTransForm/gtransformclient.hpp"
#include "EC_Global.h"
#include "../CElementClient/EC_RoleTypes.h"
#include "EC_GPDataType.h"
#include "EC_Skill.h"
#include "xmlcoder.h"
#include "auto_delete.h"
#include "ElementSkill.h"
#include "../CElementSkill/skillwrapper.h"
#include "EC_PetCorral.h"
#include "../CElementClient/EC_Algorithm.h"
#include "../CCommon/elementdataman.h"
#include "../CElementClient/EC_FixedMsg.h"
#include "EC_IvtrEquipMatter.h"
#include <AUICTranslate.h>

static CRITICAL_SECTION g_csProcessDemand;

//	���ø�ʽ
static ReturnCode GetEquipmentsDesc(const GNET::GRoleEquipment &role, XmlCoder &code);
static ReturnCode GetPetCorralDesc(const GNET::Octets &petcorral, XmlCoder &code);

typedef abase::vector<CECSkill *> AdditionalSkills;
static bool IsAdditionalSkill(CECSkill *pSkill, const AdditionalSkills *aSkills);
static ReturnCode GetSkillsDesc(const GNET::Octets &skill_data, XmlCoder &code, AdditionalSkills * pAdditoinalSkills=NULL);

//	Ѱ�������
static ReturnCode GetRoleBriefExtend(const GNET::GWebTradeRoleBrief &role, GNET::GWebTradeRoleBriefExtend &roleExtend);
static ReturnCode GetRoleMeridian(const GNET::GWebTradeRoleBriefExtend &roleExtend, GNET::GMeridianData &meridian);
static ReturnCode GetRoleReincarnation(const GNET::GWebTradeRoleBrief &role, int &reincarnation_count, int &reincarnation_exp, int &max_level, abase::vector<int> &reincarnation_levels);
static ReturnCode GetRoleRealm(const GNET::GWebTradeRoleBrief &role, int &realm_level, int &realm_exp);

static ReturnCode GetRoleDescBasic(const GNET::GWebTradeRoleBrief &role, AString &desc);
static ReturnCode GetRoleDescDetail(const GNET::GWebTradeRoleBrief &role, AString &desc);
static ReturnCode GetRoleDescDetailBasic(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailEquipment(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailInventory(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailStoreHouse(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailSkill(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailPet(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailItems(const GNET::GRoleInventoryVector &items, XmlCoder &code);
static ReturnCode GetRoleDescDetailForce(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailReincarnation(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailRealm(const GNET::GWebTradeRoleBrief &role, XmlCoder &code);
static ReturnCode GetRoleDescDetailMeridian(const GNET::GWebTradeRoleBrief &role, const GNET::GMeridianData &meridian, XmlCoder &code);
static ReturnCode GetRoleDescDetailGeneralCard(const GNET::GWebTradeRoleBriefExtend &roleExtend, XmlCoder &code);
static ReturnCode GetRoleDescDetailTitle(const GNET::GWebTradeRoleBriefExtend &roleExtend, XmlCoder &code);

//	SNS ���
static ReturnCode GetSNSRoleEquipmentDesc(const GNET::SNSRoleEquipment &sns_equip, AString &desc);
static ReturnCode GetSNSRolePetCorralDesc(const GNET::SNSRolePetCorral &sns_petcorral, AString &desc);
static ReturnCode GetSNSRoleSkillDesc(const GNET::SNSRoleSkills &sns_skills, AString &desc);

//----------------------------------------------------------
//	����Ϊ�ĵ���ʽ��������
//----------------------------------------------------------
const wchar_t * ReverseTranslate(const wchar_t *str)
{
	if( !str )
		return NULL;

	//	����ʹ�þ�̬��ȫ�ֱ���
	static AWString s_AWString;

	s_AWString = str;
	const wchar_t * tmp = str; 
	wchar_t * dest = new wchar_t[wcslen(tmp) * 2 + 1];
	wchar_t * target = dest;
	while(*tmp)
	{  
		switch(*tmp)
		{
			case '\r':
				if (*(tmp+1) == '\n')
				{
					*target ++= '\\';
					*target ++ = 'r';
					tmp ++;
				}
				break;
			default:
				*target ++ = *tmp;
				break;
		}
		tmp++;
	}
	*target = 0;
	s_AWString = dest;
	delete []dest;
	return s_AWString;
}

static AWString ToString(const abase::vector<int> &rhs)
{
	AWString ret;
	for (size_t i(0); i < rhs.size(); ++ i)
	{
		if (ret.IsEmpty()){
			ret.Format(_AL("%d"), rhs[i]);
		}else{
			ret += AWString().Format(_AL("/%d"), rhs[i]);
		}
	}
	if (ret.IsEmpty()){
		ret = g_pGame->GetStringFromTable(41);
	}
	return ret;
}

//----------------------------------------------------------
//	����Ϊͼ�긨������
//----------------------------------------------------------
static AString GetItemIcon(int idItem)
{
	//	��Ʒͼ�����Ƹ���idȷ��
	//
	AString strIcon;
	if (idItem == 3044)
	{
		//	������⴦����
		idItem = 0;
	}
	strIcon.Format("png/%d.png", idItem);
	return strIcon;
}

static AString GetSkillIcon(int idSkill)
{
	//	����ͼ���޸�Ϊ����idȷ��������Ʒͼ������
	//
	AString strIcon;
	strIcon.Format("png/skill/%d.png", idSkill);
	return strIcon;
}

static AString GetProfessionIcon(int idProfession)
{
	//	ְҵͼ�����idȷ��������Ʒͼ������
	//
	AString strIcon;
	strIcon.Format("png/profession/%d.png", idProfession);
	return strIcon;
}

//----------------------------------------------------------
//	����Ϊ���ܸ�������
//----------------------------------------------------------

static const wchar_t * GetSkillName(int idSkill)
{
	//	Ϊ�����ػ����⣬���ܵ����ƴ��� skillstr.txt �У�����ͨ�� CECSkill::GetName ��ȡԭδ���ػ�����
	//
	const wchar_t *szName = g_pGame->GetSkillDesc()->GetWideString(idSkill * 10);
	return szName ? szName : _AL("");
}

static AWString GetPetSkillDesc(int idSkill, int iLevel)
{
	//	���＼�ܶ�����ʱ�����������������������������⣬��˸��˴�����ͬ
	//
	CECSkill* pSkill = new CECSkill(idSkill, iLevel);
	auto_delete<CECSkill> tmp(pSkill);
	AWString str = pSkill->GetDesc();
	return str;
}

//----------------------------------------------------------
//	����Ϊxml �ַ������ɸ�������
//----------------------------------------------------------

template <typename T>
static AString GetUTF8String(const T &rhs);

template <>
static AString GetUTF8String(const AString &rhs)
{
	return rhs;
}

template <>
static AString GetUTF8String(const AWString &rhs)
{
	return UnicodeToUTF8(rhs);
}

template <>
static AString GetUTF8String(const wchar_t * const & rhs)
{
	return UnicodeToUTF8(rhs);
}

template <>
static AString GetUTF8String(const int &rhs)
{
	AString ret;
	ret.Format("%d", rhs);
	return ret;
}

template <>
static AString GetUTF8String(const unsigned int &rhs)
{
	AString ret;
	ret.Format("%u", rhs);
	return ret;
}

template <>
static AString GetUTF8String(const __int64 &rhs)
{
	AString ret;
	ret.Format("%I64d", rhs);
	return ret;
}

template <>
static AString GetUTF8String(const unsigned __int64 &rhs)
{
	AString ret;
	ret.Format("%I64u", rhs);
	return ret;
}

template <>
static AString GetUTF8String(const char &rhs)
{
	AString ret;
	ret.Format("%d", rhs);
	return ret;
}

template <>
static AString GetUTF8String(const unsigned char &rhs)
{
	AString ret;
	ret.Format("%d", rhs);
	return ret;
}

template <>
static AString GetUTF8String(const bool &rhs)
{
	return UnicodeToUTF8(g_pGame->GetStringFromTable(rhs ? 89 : 90));
}

template <>
static AString GetUTF8String(const abase::vector<int> &rhs)
{
	return UnicodeToUTF8(ToString(rhs));
}

struct  PropertyVectorMaker
{
	typedef XmlCoder::PropertyVector PropertyVector;
	PropertyVector _pv;

	const PropertyVector & get() { return _pv; }

	PropertyVectorMaker & clear(){ _pv.clear(); return *this;}

	template<typename T>
	PropertyVectorMaker & append(const char *szName, const T &value)
	{
		//	������ֵת��Ϊ utf-8 �ַ�������ת��Ϊ���� xml ��׼���ַ���
		_pv.push_back(XmlCoder::StringPair(szName,	XmlCoder::convert(GetUTF8String(value))));
		return *this;
	}
};

//	xml �����Ӹ�������
template<typename N, typename V>
static inline void AppendUTF8NameValueNode(XmlCoder &code, const char *szNodeName, N theName, V theValue)
{
	//	��ӵ��н�� <szNodeName name="theName" value="theValue"/>
	//	����, theName �� theValue ��ת��ΪUTF-8�ַ�������ת��Ϊ����XML��׼���ַ�����������ӡ����磺
	//	AppendUTF8NameValueNode(code, "cls", "profession, "jianling")	-->	<cls name="profession" value="jianling" />
	//	AppendUTF8NameValueNode(code, "level", L"�ȼ�, 50)					-->	<level name="�ȼ�" value="50" />
	//

	PropertyVectorMaker pvm;
	code.append_node(szNodeName, pvm.append("name", theName).append("value", theValue).get());
}

//----------------------------------------------------------
//	����Ϊͨ�õ�������Ϣ��ȡ����
//----------------------------------------------------------

static ReturnCode GetEquipmentsDesc(const GNET::GRoleEquipment &equipment, XmlCoder &code)
{
	//	���װ����Ϣ������Ϊ�գ�

	ReturnCode ret = RC_SUCCESS;
	const GRoleInventoryVector &items = equipment.inv;
	size_t count = const_cast<GRoleInventoryVector &>(items).size();

	const char *szEquip = "equipment";
	
	PropertyVectorMaker pvm;
	code.begin_node(szEquip, pvm.append("name", g_pGame->GetStringFromTable(77)).append("count", count).get());	
	ret = GetRoleDescDetailItems(items, code);	
	code.end_node(szEquip);

	return ret;
}

static ReturnCode GetPetCorralDesc(const GNET::Octets &petcorral, XmlCoder &code)
{
	//	��ӳ��������Ϣ������Ϊ�գ�
	ReturnCode ret = RC_SUCCESS;
	
	GPetCorral corral;
	while (true)
	{
		//	����������
		if (petcorral.size())
		{
			try
			{
				Marshal::OctetsStream os(petcorral);
				os >> corral;
			}
			catch (...)
			{
				ret = RC_PARSE_PET_STREAM;
				Log_Info("Exception when parsing GPetCorral! (retcode: %d)", ret);
				break;
			}
		}

		PropertyVectorMaker pvm;

		//	��ʼ������
		const char *szPets = "pets";
		code.begin_node(szPets, pvm.append("name", g_pGame->GetStringFromTable(84)).append("capacity", corral.capacity).append("count", corral.pets.size()).get());		

		for (size_t i = 0; i < corral.pets.size(); ++ i)
		{
			//	������������
			const GPet &pet = corral.pets[i];
			if (!pet.data.size())
			{
				ret = RC_INVALID_PET_DATA;
				Log_Info("Invalid pet data (empty, index = %d)! (retcode: %d)", i, ret);
				break;
			}
			
			const S2C::info_pet *info = (const S2C::info_pet *)pet.data.begin();

			CECPetData pd;
			if (!pd.Init(*info))
			{
				ret = RC_INVALID_PET_DATA;
				Log_Info("Invalid pet data (init failed, index = %d)! (retcode: %d)", i, ret);
				break;
			}

			//	������Ч������

			const char *szPet = "pet";

			//	��������й��������ƺ�ͼ������
			code.begin_node(szPet, pvm.clear().append("name", pd.GetName()).append("icon", GetItemIcon(pd.GetTemplateID())).get());

			//	����������Ҫ����
			if (pd.IsMountPet())
			{
				//	���
				AppendUTF8NameValueNode(code, "type", g_pGame->GetStringFromTable(50), g_pGame->GetStringFromTable(51));

				//	�������Դ�����ս��ͬ
				AppendUTF8NameValueNode(code, "level", g_pGame->GetStringFromTable(61), pd.GetLevel());
				AppendUTF8NameValueNode(code, "exp", g_pGame->GetStringFromTable(62), pd.GetExp());
				AppendUTF8NameValueNode(code, "food", g_pGame->GetStringFromTable(63), pd.GetFoodStr());
				AppendUTF8NameValueNode(code, "move_speed", g_pGame->GetStringFromTable(64), pd.GetMoveSpeedStr());
				AppendUTF8NameValueNode(code, "hungry", g_pGame->GetStringFromTable(65), pd.GetHungerStr());
				AppendUTF8NameValueNode(code, "loyalty", g_pGame->GetStringFromTable(66), pd.GetLoyaltyStr());

				//	�����������г���ͬ
				AppendUTF8NameValueNode(code, "level_requirement", g_pGame->GetStringFromTable(67), pd.GetLevelRequirement());
				if (pd.IsBind())
				{
					AppendUTF8NameValueNode(code, "bind", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_EQUIP_BIND), pd.IsBind());
					AppendUTF8NameValueNode(code, "webtrade", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_CAN_WEBTRADE), pd.CanWebTrade());
				}
				
				//	����Ϊ��������
				elementdataman* pDBMan = g_pGame->GetElementDataMan();
				DATA_TYPE DataType;
				PET_ESSENCE *pDB = (PET_ESSENCE*)pDBMan->get_data_ptr(pd.GetTemplateID(), ID_SPACE_ESSENCE, DataType);
				if (pDB && DataType == DT_PET_ESSENCE && pDB->require_dye_count > 0)
				{
					A3DCOLOR clr;
					if (!CECGame::GetColor(pd.GetColor(), clr))
						clr = CECGame::GetDefaultColor();
					
					ACString strTemp;
					strTemp.Format(_AL("^%02x%02x%02x"), A3DCOLOR_GETRED(clr), A3DCOLOR_GETGREEN(clr), A3DCOLOR_GETBLUE(clr));
					ACString strColor;
					strColor.Format(g_pGame->GetItemDesc()->GetWideString(ITEMDESC_COLORRECT), strTemp);

					AppendUTF8NameValueNode(code, "color", g_pGame->GetStringFromTable(88), strColor);
				}
			}
			else if (pd.IsCombatPet())
			{
				//	ս��
				AppendUTF8NameValueNode(code, "type", g_pGame->GetStringFromTable(50), g_pGame->GetStringFromTable(52));

				//	�������Դ��������ͬ
				AppendUTF8NameValueNode(code, "level", g_pGame->GetStringFromTable(61), pd.GetLevel());
				AppendUTF8NameValueNode(code, "exp", g_pGame->GetStringFromTable(62), pd.GetExp());
				AppendUTF8NameValueNode(code, "food", g_pGame->GetStringFromTable(63), pd.GetFoodStr());
				AppendUTF8NameValueNode(code, "move_speed", g_pGame->GetStringFromTable(64), pd.GetMoveSpeedStr());
				AppendUTF8NameValueNode(code, "hungry", g_pGame->GetStringFromTable(65), pd.GetHungerStr());
				AppendUTF8NameValueNode(code, "loyalty", g_pGame->GetStringFromTable(66), pd.GetLoyaltyStr());
				
				//	�����������г���ͬ
				AppendUTF8NameValueNode(code, "level_requirement", g_pGame->GetStringFromTable(67), pd.GetLevelRequirement());
				if (pd.IsBind())
				{
					AppendUTF8NameValueNode(code, "bind", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_EQUIP_BIND), pd.IsBind());
					AppendUTF8NameValueNode(code, "webtrade", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_CAN_WEBTRADE), pd.CanWebTrade());
				}

				//	����Ϊ��������
				AppendUTF8NameValueNode(code, "max_hp", g_pGame->GetStringFromTable(68), pd.GetMaxHP());
				AppendUTF8NameValueNode(code, "attack", g_pGame->GetStringFromTable(69), pd.GetAttack());
				AppendUTF8NameValueNode(code, "physic_defence", g_pGame->GetStringFromTable(70), pd.GetPhyicDefence());
				AppendUTF8NameValueNode(code, "magic_defence", g_pGame->GetStringFromTable(71), pd.GetMagicDefence());
				AppendUTF8NameValueNode(code, "definition", g_pGame->GetStringFromTable(72), pd.GetDefiniton());
				AppendUTF8NameValueNode(code, "evade", g_pGame->GetStringFromTable(73), pd.GetEvade());
				AppendUTF8NameValueNode(code, "attack_speed", g_pGame->GetStringFromTable(74), pd.GetAttackSpeedStr());
				AppendUTF8NameValueNode(code, "inhabit_type", g_pGame->GetStringFromTable(75), pd.GetInhabitTypeStr());

				const int nSkill = pd.GetSkillNum(CECPetData::EM_SKILL_NORMAL);
				if (nSkill > 0)
				{
					//	��Ӽ��ܽڵ�
					const char *szPetSkills = "skills";
					code.begin_node(szPetSkills, pvm.clear().append("name", g_pGame->GetStringFromTable(85)).append("count", nSkill).get());

					//	��Ӽ����ӽڵ�
					AUICTranslate trans;
					for (int k = 0; k < nSkill; ++ k)
					{
						const CECPetData::PETSKILL *pSkill = pd.GetSkill(CECPetData::EM_SKILL_NORMAL,k);
						if (pSkill && pSkill->idSkill !=0)
							code.append_node("skill", pvm.clear().append("id", pSkill->idSkill).append("name", GetSkillName(pSkill->idSkill)).append("icon", GetSkillIcon(pSkill->idSkill)).append("desc", trans.ReverseTranslate(GetPetSkillDesc(pSkill->idSkill, pSkill->iLevel))).get());
					}

					code.end_node(szPetSkills);
				}
			}
			else if (pd.IsFollowPet())
			{
				//	���ͳ�
				AppendUTF8NameValueNode(code, "type", g_pGame->GetStringFromTable(50), g_pGame->GetStringFromTable(53));
				AppendUTF8NameValueNode(code, "move_speed", g_pGame->GetStringFromTable(64), pd.GetMoveSpeedStr());
				AppendUTF8NameValueNode(code, "max_hp", g_pGame->GetStringFromTable(68), pd.GetMaxHP());
				
				//	�����������г���ͬ
				AppendUTF8NameValueNode(code, "level_requirement", g_pGame->GetStringFromTable(67), pd.GetLevelRequirement());
				if (pd.IsBind())
				{
					AppendUTF8NameValueNode(code, "bind", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_EQUIP_BIND), pd.IsBind());
					AppendUTF8NameValueNode(code, "webtrade", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_CAN_WEBTRADE), pd.CanWebTrade());
				}
			}
			else if (pd.IsEvolutionPet())
			{
				//	������
				AppendUTF8NameValueNode(code, "type", g_pGame->GetStringFromTable(50), g_pGame->GetStringFromTable(54));
				
				//	�������Դ��������ͬ
				AppendUTF8NameValueNode(code, "level", g_pGame->GetStringFromTable(61), pd.GetLevel());
				AppendUTF8NameValueNode(code, "exp", g_pGame->GetStringFromTable(62), pd.GetExp());
				AppendUTF8NameValueNode(code, "food", g_pGame->GetStringFromTable(63), pd.GetFoodStr());
				AppendUTF8NameValueNode(code, "move_speed", g_pGame->GetStringFromTable(64), pd.GetMoveSpeedStr());
				AppendUTF8NameValueNode(code, "hungry", g_pGame->GetStringFromTable(65), pd.GetHungerStr());
				AppendUTF8NameValueNode(code, "loyalty", g_pGame->GetStringFromTable(66), pd.GetLoyaltyStr());
				
				//	�����������г���ͬ
				AppendUTF8NameValueNode(code, "level_requirement", g_pGame->GetStringFromTable(67), pd.GetLevelRequirement());
				if (pd.IsBind())
				{
					AppendUTF8NameValueNode(code, "bind", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_EQUIP_BIND), pd.IsBind());
					AppendUTF8NameValueNode(code, "webtrade", g_pGame->GetItemDesc()->GetWideString(ITEMDESC_CAN_WEBTRADE), pd.CanWebTrade());
				}
				
				//	��������ͬս��
				AppendUTF8NameValueNode(code, "max_hp", g_pGame->GetStringFromTable(68), pd.GetMaxHP());
				AppendUTF8NameValueNode(code, "attack", g_pGame->GetStringFromTable(69), pd.GetAttack());
				AppendUTF8NameValueNode(code, "physic_defence", g_pGame->GetStringFromTable(70), pd.GetPhyicDefence());
				AppendUTF8NameValueNode(code, "magic_defence", g_pGame->GetStringFromTable(71), pd.GetMagicDefence());
				AppendUTF8NameValueNode(code, "definition", g_pGame->GetStringFromTable(72), pd.GetDefiniton());
				AppendUTF8NameValueNode(code, "evade", g_pGame->GetStringFromTable(73), pd.GetEvade());
				AppendUTF8NameValueNode(code, "attack_speed", g_pGame->GetStringFromTable(74), pd.GetAttackSpeedStr());
				AppendUTF8NameValueNode(code, "inhabit_type", g_pGame->GetStringFromTable(75), pd.GetInhabitTypeStr());

				const int nSkill = pd.GetSkillNum(CECPetData::EM_SKILL_DEFAULT);
				if (nSkill > 0)
				{
					//	��Ӽ��ܽڵ�
					const char *szPetSkills = "skills";
					code.begin_node(szPetSkills, pvm.clear().append("name", g_pGame->GetStringFromTable(85)).append("count", nSkill).get());
					
					//	��Ӽ����ӽڵ�
					AUICTranslate trans;
					for (int k = 0; k < nSkill; ++ k)
					{
						const CECPetData::PETSKILL *pSkill = pd.GetSkill(CECPetData::EM_SKILL_DEFAULT,k);
						if (pSkill && pSkill->idSkill !=0)
							code.append_node("skill", pvm.clear().append("id", pSkill->idSkill).append("name", GetSkillName(pSkill->idSkill)).append("icon", GetSkillIcon(pSkill->idSkill)).append("desc", trans.ReverseTranslate(GetPetSkillDesc(pSkill->idSkill, pSkill->iLevel))).get());
					}
					
					code.end_node(szPetSkills);
				}

				//	����Ϊ��������
				AppendUTF8NameValueNode(code, "nature", g_pGame->GetStringFromTable(100), pd.GetNature());

				{
					const char *szInherit = "inherited";
					code.begin_node(szInherit, pvm.clear().append("name", g_pGame->GetStringFromTable(106)).get());
					code.append_node("inherit_atk_ration", pvm.clear().append("name", g_pGame->GetStringFromTable(101)).append("cur_value", pd.GetAtkRation()).append("max_value", pd.GetMaxAtkRation()).get());
					code.append_node("inherit_def_ration", pvm.clear().append("name", g_pGame->GetStringFromTable(102)).append("cur_value", pd.GetDefRation()).append("max_value", pd.GetMaxDefRation()).get());
					code.append_node("inherit_hp_ration", pvm.clear().append("name", g_pGame->GetStringFromTable(103)).append("cur_value", pd.GetHpRation()).append("max_value", pd.GetMaxHpRation()).get());
					code.append_node("inherit_atk_lvl_ration", pvm.clear().append("name", g_pGame->GetStringFromTable(104)).append("cur_value", pd.GetAtkLvlRation()).append("max_value", pd.GetMaxAtkLvlRation()).get());
					code.append_node("inherit_def_lvl_ration", pvm.clear().append("name", g_pGame->GetStringFromTable(105)).append("cur_value", pd.GetDefLvlRation()).append("max_value", pd.GetMaxDefLvlRation()).get());
					code.end_node(szInherit);
				}
			}
			else
			{
				//	���������Ӧ�����������
				ret = RC_INVALID_PET_TYPE;
			}
			code.end_node(szPet);

			if (ret != RC_SUCCESS)
				break;
		}

		code.end_node(szPets);
		break;
	}
	return ret;
}
//----------------------------------------------------------

void  processOneDemand(const GNET::GetRoleDesc &in, GNET::GetRoleDesc_Re &out)
{
	MyCriticalSection dump1(g_csProcessDemand);
		
	// ��ӡ���յ���Э����Ϣ
	SYSTEMTIME st;
	GetLocalTime(&st);
	Log_Info("[%d:%02d:%02d-%02d:%02d:%02d] sn=%I64d, timestamp=%I64d, detail.size=%d, type=role.",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		in.sn, in.timestamp, in.detail.size());

	// �����ֽ�����Ӧ��Э������
	GWebTradeRoleBrief role;

	out.sn = in.sn;
	out.timestamp = in.timestamp;

	try
	{
		int id(0);					//	Ϊ�� GetItemDesc �ṹͳһ�����壬ֵΪ0������Ϸ����Ʒģ��ID����ͻ
		int category(0);		//	Ϊ�� GetItemDesc �ṹͳһ�����壬��ʾ��ʾ���࣬���ơ���Ϸ��	90048�����90048
		Marshal::OctetsStream(in.detail) >> id >> category >>  role;
	}catch (...)
	{
		// ����Э��������ʱ��������
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("Exception when parsing GWebTradeRoleBrief! (retcode: %d)", out.retcode);
		return;
	}

	//	����������ݵĺϷ���

	if (role.cls < 0 || role.cls >= NUM_PROFESSION)
	{
		//	ְҵid���Ϸ�������ԭ��Ϊ��Ϸ�������ְҵ����������δ��ʱ�޸ĵ���
		out.retcode = RC_INVALID_PROFESSION;
		Log_Info("Invalid profession (%d)! (retcode: %d)", role.cls, out.retcode);
		return;
	}

	if (role.property.size() != sizeof(ROLEEXTPROP))
	{
		//	������Բ��Ϸ�������Ϊ��
		out.retcode = RC_INVALID_ROLEEXTPROP;
		Log_Info("Invalid property size (%d, should be %d)! (retcode: %d)", role.property.size(), sizeof(ROLEEXTPROP), out.retcode);
		return;
	}

	// ��¼��ɫ�Ļ�����Ϣ
	Log_Info("role.cls = %d, role.gender = %d, role.level = %d, role.level2 = %d, role.exp = %d, role.sp = %d, role.pp = %d, role.reputation = %d", 
		role.cls, role.gender, role.level, role.level2, role.exp, role.sp, role.pp, role.reputation);

	//	����Э�����ݣ����ɴ�����

	//	��ɫ�ļ���ֵ��Ϣ
	out.cls = role.cls;
	out.gender = role.gender;
	out.level = role.level;

	AWString wstrName = g_pGame->GetProfName(role.cls);
	AString strName = UnicodeToUTF8(wstrName);
	out.name.replace((const void *)(const char *)strName, strName.GetLength());

	AString strIcon = GetProfessionIcon(role.cls);
	out.icon.replace((const void *)(const char *)strIcon, strIcon.GetLength());

	//	��ɫ�ļ�����
	AString strBasic;
	if ((out.retcode == GetRoleDescBasic(role, strBasic)) != RC_SUCCESS){
		return;
	}
	out.basic_desc.replace((const void *)(const char *)strBasic, strBasic.GetLength());

	//	��ɫ����ϸ����
	AString strDetail;
	if ((out.retcode = GetRoleDescDetail(role, strDetail)) != RC_SUCCESS){
		return;
	}
	out.detail_desc.replace((const void *)(const char *)strDetail, strDetail.GetLength());
#ifdef _DEBUG
	openOctet(out.detail_desc);
#endif
}

static ReturnCode GetRoleDescBasic(const GNET::GWebTradeRoleBrief &role, AString &desc)
{
	//	��ȡ��ɫ�Ļ�����Ϣ����
	ReturnCode rc = RC_SUCCESS;

	AWString strRet, strTemp;

	AWString strTitleColor = g_pGame->GetStringFromTable(13);
	AWString strValueColor = g_pGame->GetStringFromTable(14);

	//	ְҵ
	strTemp.Format(_AL("%s%s %s%s"), strTitleColor, g_pGame->GetStringFromTable(20), strValueColor, g_pGame->GetProfName(role.cls));
	strRet += strTemp;

	//	�Ա�
	strTemp.Format(_AL("%s%s %s%s"), strTitleColor, g_pGame->GetStringFromTable(21), strValueColor, g_pGame->GetStringFromTable(22+role.gender));
	strRet += _AL("\\r");
	strRet += strTemp;

	//	�ȼ�
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(24), strValueColor, role.level);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	ת��	
	int reincarnation_count(0), reincarnation_exp(0), max_level(0);
	abase::vector<int> reincarnation_levels;
	if ((rc = GetRoleReincarnation(role, reincarnation_count, reincarnation_exp, max_level, reincarnation_levels)) != RC_SUCCESS){
		return rc;
	}
	strTemp.Format(_AL("%s%s %s%s"), strTitleColor, g_pGame->GetStringFromTable(40), strValueColor, ToString(reincarnation_levels));
	strRet += _AL("\\r");
	strRet += strTemp;
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(39), strValueColor, max_level);
	strRet += _AL("\\r");
	strRet += strTemp;
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(35), strValueColor, reincarnation_count);
	strRet += _AL("\\r");
	strRet += strTemp;
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(36), strValueColor, reincarnation_exp);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	����	
	int realm_level(0), realm_exp(0);
	if ((rc = GetRoleRealm(role, realm_level, realm_exp)) != RC_SUCCESS){
		return rc;
	}
	strTemp.Format(_AL("%s%s %s%s"), strTitleColor, g_pGame->GetStringFromTable(37), strValueColor, g_pGame->GetRealmName(realm_level));
	strRet += _AL("\\r");
	strRet += strTemp;
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(38), strValueColor, realm_exp);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	����
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(34), strValueColor, role.reputation);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	��ǰ����
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(26), strValueColor, role.exp);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	����
	strTemp.Format(_AL("%s%s %s%s"), strTitleColor, g_pGame->GetStringFromTable(25), strValueColor, g_pGame->GetStringFromTable(1001+role.level2));
	strRet += _AL("\\r");
	strRet += strTemp;

	//	Ԫ��
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(28), strValueColor, role.sp);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	ʣ�����
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(29), strValueColor, role.pp);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	��������
	//	����
	ROLEEXTPROP &prop = *(ROLEEXTPROP *)role.property.begin();
	strRet += _AL("\\r");
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(30), strValueColor, prop.bs.vitality);
	strRet += strTemp;

	//	����
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(31), strValueColor, prop.bs.energy);
	strRet += _AL("\\r");
	strRet += strTemp;

	//	����
	strRet += _AL("\\r");
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(32), strValueColor, prop.bs.strength);
	strRet += strTemp;

	//	����
	strTemp.Format(_AL("%s%s %s%d"), strTitleColor, g_pGame->GetStringFromTable(33), strValueColor, prop.bs.agility);
	strRet += _AL("\\r");
	strRet += strTemp;

	desc = UnicodeToUTF8(strRet);
	return RC_SUCCESS;
}

static ReturnCode GetRoleDescDetail(const GNET::GWebTradeRoleBrief &role, AString &desc)
{
	//	��ȡ��ɫ����ϸ��Ϣ����

	ReturnCode ret = RC_SUCCESS;

	XmlCoder code;

	//	��������Ӱ汾��Ϣ���Ա�����޸�
	//	<role version="1.0">
	while (true)
	{
		const char *szRoot = "role";
		code.begin_node(szRoot, "version", "1.0");
		
		GNET::GWebTradeRoleBriefExtend roleExtend;
		GMeridianData meridian;
		if ((ret = GetRoleBriefExtend(role, roleExtend)) != RC_SUCCESS ||
			(ret = GetRoleMeridian(roleExtend, meridian)) != RC_SUCCESS)
			break;
		
		//	������Ϣ
		if ((ret = GetRoleDescDetailBasic(role, code)) != RC_SUCCESS)
			break;
		
		//	װ����Ϣ
		if ((ret = GetRoleDescDetailEquipment(role, code)) != RC_SUCCESS)
			break;
		
		//	������Ϣ
		if ((ret = GetRoleDescDetailInventory(role, code)) != RC_SUCCESS)
			break;
		
		//	�ֿ���Ϣ
		if ((ret = GetRoleDescDetailStoreHouse(role, code)) != RC_SUCCESS)
			break;
		
		//	������Ϣ
		if ((ret = GetRoleDescDetailSkill(role, code)) != RC_SUCCESS)
			break;
		
		//	������Ϣ
		if ((ret = GetRoleDescDetailPet(role, code)) != RC_SUCCESS)
			break;
		
		//	������Ϣ
		if ((ret = GetRoleDescDetailForce(role, code)) != RC_SUCCESS)
			break;
		
		//	������Ϣ
		if ((ret = GetRoleDescDetailMeridian(role, meridian, code)) != RC_SUCCESS)
			break;
		
		//	ս����Ϣ
		if ((ret = GetRoleDescDetailGeneralCard(roleExtend, code)) != RC_SUCCESS)
			break;
		
		//	�ƺ���Ϣ
		if ((ret = GetRoleDescDetailTitle(roleExtend, code)) != RC_SUCCESS)
			break;
		
		//	</role>
		code.end_node(szRoot);		
		desc = code.c_str();
		break;
	}
	return ret;
}

static ReturnCode GetRoleDescDetailBasic(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	//	��ӽ�ɫ������Ϣ
	//

	ReturnCode rc = RC_SUCCESS;

	const char *szBasic = "basic";

	PropertyVectorMaker pvm;
	code.begin_node(szBasic, pvm.append("name", g_pGame->GetStringFromTable(76)).get());

	//	������Ϣ
	AppendUTF8NameValueNode(code,	"cls",			g_pGame->GetStringFromTable(20),	g_pGame->GetProfName(role.cls));			//	ְҵ
	AppendUTF8NameValueNode(code,	"gender",		g_pGame->GetStringFromTable(21),	g_pGame->GetStringFromTable(22+role.gender));	//	�Ա�
	AppendUTF8NameValueNode(code,	"level",		g_pGame->GetStringFromTable(24),	role.level);								//	�ȼ�
	if ((rc = GetRoleDescDetailReincarnation(role, code)) != RC_SUCCESS ||
		(rc = GetRoleDescDetailRealm(role, code)) != RC_SUCCESS){
		return rc;
	}
	AppendUTF8NameValueNode(code,	"reputation",	g_pGame->GetStringFromTable(34),	role.reputation);								//	����
	AppendUTF8NameValueNode(code,	"exp",			g_pGame->GetStringFromTable(26),	role.exp);										//	��ǰ����
	AppendUTF8NameValueNode(code,	"level2",		g_pGame->GetStringFromTable(25),	g_pGame->GetStringFromTable(1001+role.level2));	//	����
	AppendUTF8NameValueNode(code,	"sp",			g_pGame->GetStringFromTable(28),	role.sp);

	ROLEEXTPROP &prop = *(ROLEEXTPROP *)role.property.begin();
	AppendUTF8NameValueNode(code,	"pp",			g_pGame->GetStringFromTable(29),	role.pp);										//	ʣ�����
	AppendUTF8NameValueNode(code,	"vitality",		g_pGame->GetStringFromTable(30),	prop.bs.vitality);								//	����
	AppendUTF8NameValueNode(code,	"energy",		g_pGame->GetStringFromTable(31),	prop.bs.energy);								//	����
	AppendUTF8NameValueNode(code,	"strength",		g_pGame->GetStringFromTable(32),	prop.bs.strength);								//	����
	AppendUTF8NameValueNode(code,	"agility",		g_pGame->GetStringFromTable(33),	prop.bs.agility);

	code.end_node(szBasic);
	return RC_SUCCESS;
}

static ReturnCode GetRoleDescDetailItems(const GNET::GRoleInventoryVector &items, XmlCoder &code)
{
	ReturnCode ret = RC_SUCCESS;
	for (size_t i = 0; i < const_cast<GRoleInventoryVector &>(items).size(); ++ i)
	{
		const GRoleInventory &item = const_cast<GRoleInventoryVector &>(items)[i];
		{
			CECIvtrItem *pItem = CECIvtrItem::CreateItem(item.id, item.expire_date, item.count);
			if (!pItem)
			{
				// ��Ʒ����ʧ��
				ret = RC_ITEM_NOT_EXIST;
				Log_Info("Cannot create item %d from GRoleInventory (pos = %d)! (retcode: %d)", item.id, item.pos, ret);
				break;
			}

			//	�Զ�ɾ����Ʒ
			auto_delete<CECIvtrItem> tmp(pItem);

			pItem->SetProcType(item.proctype);
			if (item.data.begin() && item.data.size())
				pItem->SetItemInfo((BYTE *)item.data.begin(), item.data.size());
			else
				pItem->SetItemInfo(NULL, 0);
			
			// �����Ʒ�� xml

			pItem->SetPriceScale(CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);
			const wchar_t *pszHint = pItem->GetDesc(CECIvtrItem::DESC_NORMAL, NULL);

			PropertyVectorMaker pvm;
			code.append_node("item", pvm.append("id", pItem->GetTemplateID()).append("pos", item.pos).append("equipmask", pItem->GetEquipMask()).append("level", pItem->GetItemLevel()).append("name", pItem->GetName()).append("icon", GetItemIcon(pItem->GetTemplateID())).append("count", item.count).append("desc", ReverseTranslate(pszHint)).get());
		}
	}

	return ret;
}

static ReturnCode GetRoleDescDetailEquipment(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	return GetEquipmentsDesc(role.equipment, code);
}

static ReturnCode GetRoleDescDetailInventory(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	//	��Ӱ�����Ϣ������Ϊ�գ�

	ReturnCode ret = RC_SUCCESS;

	const GRolePocket &inventory = role.inventory;
	const GRoleInventoryVector &items = inventory.items;
	size_t count = const_cast<GRoleInventoryVector &>(items).size();

	const char *szPack = "inventory";

	PropertyVectorMaker pvm;
	code.begin_node(szPack, pvm.append("name", g_pGame->GetStringFromTable(78)).append("capacity", inventory.capacity).append("count", count).append("money", inventory.money).get());
	ret = GetRoleDescDetailItems(items, code);
	code.end_node(szPack);

	return ret;
}

static ReturnCode GetRoleDescDetailStoreHouse(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	//	��Ӳֿ���Ϣ
	ReturnCode ret = RC_SUCCESS;

	const GRoleStorehouse &storehouse = role.storehouse;

	PropertyVectorMaker pvm;

	const char *szStore = "storehouse";
	code.begin_node(szStore, pvm.append("name", g_pGame->GetStringFromTable(79)).append("money", storehouse.money).get());

	while (true)
	{
		{
			const GRoleInventoryVector &items = storehouse.items;
			size_t items_count = const_cast<GRoleInventoryVector &>(items).size();
			
			const char *szItems = "items";

			code.begin_node(szItems, pvm.clear().append("name", g_pGame->GetStringFromTable(80)).append("capacity", storehouse.capacity).append("count", items_count).get());
			ret = GetRoleDescDetailItems(items, code);
			code.end_node(szItems);
			if (ret != RC_SUCCESS)
				break;
		}
		
		{
			const GRoleInventoryVector &material = storehouse.material;
			size_t material_count = const_cast<GRoleInventoryVector &>(material).size();
			
			const char *szMaterial = "material";

			code.begin_node(szMaterial, pvm.clear().append("name", g_pGame->GetStringFromTable(82)).append("capacity", storehouse.size1).append("count", material_count).get());
			ret = GetRoleDescDetailItems(material, code);
			code.end_node(szMaterial);
			if (ret != RC_SUCCESS)
				break;
		}
		
		{
			const GRoleInventoryVector &dress = storehouse.dress;
			size_t dress_count = const_cast<GRoleInventoryVector &>(dress).size();
			
			const char *szDress = "dress";

			code.begin_node(szDress, pvm.clear().append("name", g_pGame->GetStringFromTable(81)).append("capacity", storehouse.size2).append("count", dress_count).get());
			ret = GetRoleDescDetailItems(dress, code);
			code.end_node(szDress);
			if (ret != RC_SUCCESS)
				break;
		}
		
		{
			const GRoleInventoryVector &generalcard = storehouse.generalcard;
			size_t generalcard_count = const_cast<GRoleInventoryVector &>(generalcard).size();
			
			const char *szGeneralCard = "generalcard";
			
			code.begin_node(szGeneralCard, pvm.clear().append("name", g_pGame->GetStringFromTable(88)).append("capacity", storehouse.size3).append("count", generalcard_count).get());
			ret = GetRoleDescDetailItems(generalcard, code);
			code.end_node(szGeneralCard);
			if (ret != RC_SUCCESS)
				break;
		}

		break;
	}

	code.end_node(szStore);
	return ret;
}

//	������ڴ�����ת���� Octets ��/
class OctetsMaker : public GNET::Octets
{
public:
	template <typename T>
	OctetsMaker & operator << (const T &rhs)
	{
		insert(end(), &rhs, sizeof(rhs));
		return *this;
	}
};

//	������ʾ����ʱ�ȽϺ���
class CompareCECSkillShowOrder
{
public:
	bool operator ()(CECSkill * p1, CECSkill * p2)
	{
		return p1->GetShowOrder() < p2->GetShowOrder();
	}
};

static ReturnCode GetRoleDescDetailSkill(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	ReturnCode ret(RC_SUCCESS);

	//	�����װ���л�ȡ�ļ���
	AdditionalSkills aEquipSkills;

	std::set<int> addSkillIDs;
	const GRoleInventoryVector &items = role.equipment.inv;
	for (size_t i = 0; i < const_cast<GRoleInventoryVector &>(items).size(); ++ i)
	{
		const GRoleInventory &item = const_cast<GRoleInventoryVector &>(items)[i];

		DATA_TYPE DataType = g_pGame->GetElementDataMan()->get_data_type(item.id, ID_SPACE_ESSENCE);
		if (DataType != DT_DYNSKILLEQUIP_ESSENCE)
			continue;

		CECIvtrItem *pItem = CECIvtrItem::CreateItem(item.id, item.expire_date, item.count);
		if (!pItem)	continue;

		//	�Զ�ɾ����Ʒ
		auto_delete<CECIvtrItem> tmp(pItem);

		CECIvtrDynSkillEquip *pDynSkillEquip = dynamic_cast<CECIvtrDynSkillEquip*>(pItem);
		if (!pDynSkillEquip)	continue;

		for (int k(0); k < pDynSkillEquip->GetSkillCount(); ++ k)
		{
			CECSkill *pSkill = pDynSkillEquip->GetSkill(k);
			if (pSkill && pSkill->IsGeneralSkill() && pSkill->IsPositiveSkill())
				addSkillIDs.insert(pSkill->GetSkillID());
		}
	}
	if (!addSkillIDs.empty())
	{
		for (std::set<int>::iterator it=addSkillIDs.begin(); it != addSkillIDs.end(); ++ it)
		{
			int idSkill = *it;
			CECSkill* pSkill = new CECSkill(idSkill, 1);
			aEquipSkills.push_back(pSkill);
		}
	}

	//	��ȡ����������Ϣ
	ret = GetSkillsDesc(role.skills, code, &aEquipSkills);

	//	ɾ����װ���л�ȡ�ļ���
	if (!aEquipSkills.empty())
	{
		for (AdditionalSkills::iterator it = aEquipSkills.begin(); it != aEquipSkills.end(); ++ it)
			delete *it;
		aEquipSkills.clear();
	}

	return ret;
}

bool IsAdditionalSkill(CECSkill *pSkill, const AdditionalSkills *aSkills)
{
	return aSkills && std::find(aSkills->begin(), aSkills->end(), pSkill) != aSkills->end();
}

static ReturnCode GetSkillsDesc(const GNET::Octets &skills_data, XmlCoder &code, AdditionalSkills * pAdditoinalSkills/* = NULL*/)
{
	//	��Ӽ�����Ϣ������Ϊ�գ�
	ReturnCode ret = RC_SUCCESS;
	
	//	����Э�����ݽṹ
	struct server_skill_data
	{
		size_t count;
		struct data
		{
			int	id;
			int	ability;
			int	level;
		}list[1];
	};
	
	abase::vector<CECSkill *> skills;		//	��������
	if (skills_data.size())
	{
		const server_skill_data *pData = (const server_skill_data *)skills_data.begin();
		if (pData->count)
		{
			size_t i(0);
			
			//	������ͻ�����Ҫ�� cmd_skill_data �ṹ
			OctetsMaker local;
			local << pData->count;
			for (i = 0; i < pData->count; ++ i)
			{
				const server_skill_data::data &cur = pData->list[i];
				local << (short)cur.id << (char)cur.level << (short)cur.ability;
			}
			
			//	���� ElementSkill �����ת����ļ������ݣ���ȡ����������Ϣ���룬��Ҫ�����������ȵĲ�ѯ��
			ElementSkill::LoadSkillData(local.begin());
			
			//	�ӷ������˼������ݹ��챾����Ч���ݣ����������Overridden���ܣ�
			for (i = 0; i < pData->count; ++ i)
			{
				const server_skill_data::data &cur = pData->list[i];
				if (!ElementSkill::IsOverridden(cur.id))
					skills.push_back(new CECSkill(cur.id, cur.level));
			}

			//	���Ӹ��Ӽ���
			if (pAdditoinalSkills)
			{
				for (AdditionalSkills::iterator it = pAdditoinalSkills->begin(); it != pAdditoinalSkills->end(); ++ it)
					skills.push_back(*it);
			}
			
			//	�Լ��ܽ�����������Ϸ�ͻ�����ʾһ��
			BubbleSort(skills.begin(), skills.end(), CompareCECSkillShowOrder());
		}
	}
	
	//	��Ӽ�����������
	PropertyVectorMaker pvm;
	size_t i(0);
	AUICTranslate trans;
	
	//	��Ӽ��ܸ����
	const char *szSkill = "skills";
	code.begin_node(szSkill, pvm.append("name", g_pGame->GetStringFromTable(83)).get());
	
	//	������������ӽ��
	size_t positive_count(0);
	for (i = 0; i < skills.size(); ++ i)
	{
		CECSkill *pSkill = skills[i];
		if (pSkill->IsPositiveSkill())
			positive_count ++;
	}
	
	const char *szPositiveSkills = "positive";
	
	code.begin_node(szPositiveSkills, pvm.clear().append("name", g_pGame->GetStringFromTable(86)).append("count", positive_count).get());				
	for (i = 0; i < skills.size(); ++ i)
	{
		CECSkill *pSkill = skills[i];
		if (pSkill->IsPositiveSkill())
		{
			pvm.clear().append("id", pSkill->GetSkillID()).append("name", GetSkillName(pSkill->GetSkillID())).append("icon", GetSkillIcon(pSkill->GetSkillID())).append("desc", trans.ReverseTranslate(pSkill->GetDesc()));
			if (IsAdditionalSkill(pSkill, pAdditoinalSkills))
				pvm.append("additional", 1);
			code.append_node("skill", pvm.get());
		}
	}				
	code.end_node(szPositiveSkills);
	
	//	��ӱ��������ӽ��
	size_t passive_count = skills.size() - positive_count;
	
	const char *szPassiveSkills = "passive";
	
	code.begin_node(szPassiveSkills, pvm.clear().append("name", g_pGame->GetStringFromTable(87)).append("count", passive_count).get());				
	for (i = 0; i < skills.size(); ++ i)
	{
		CECSkill *pSkill = skills[i];
		if (!pSkill->IsPositiveSkill())
		{
			pvm.clear().append("id", pSkill->GetSkillID()).append("name", GetSkillName(pSkill->GetSkillID())).append("icon", GetSkillIcon(pSkill->GetSkillID())).append("desc", trans.ReverseTranslate(pSkill->GetDesc()));
			code.append_node("skill", pvm.get());
		}
	}				
	code.end_node(szPassiveSkills);
	
	//	��ɼ������
	code.end_node(szSkill);
	
	//	�����������
	for (i = 0; i < skills.size(); ++ i)
	{
		CECSkill *pSkill = skills[i];
		if (!IsAdditionalSkill(pSkill, pAdditoinalSkills))
		{
			//	��ɾ����紫��� skills
			delete pSkill;
		}
	}
	skills.clear();
	
	cmd_skill_data dummy;
	dummy.count = 0;
	ElementSkill::LoadSkillData(&dummy);

	return ret;
}

static ReturnCode GetRoleDescDetailPet(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	return GetPetCorralDesc(role.petcorral, code);
}

typedef abase::hash_map<int, const FORCE_CONFIG *> ForceMap;
static void GetAllForceConfig(ForceMap & forces)
{
	forces.clear();

	DATA_TYPE DataType;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	unsigned int id = pDataMan->get_first_data_id(ID_SPACE_CONFIG, DataType);
	while( id )
	{
		if( DataType == DT_FORCE_CONFIG )
		{
			const FORCE_CONFIG *pConfig = (const FORCE_CONFIG *)pDataMan->get_data_ptr(id, ID_SPACE_CONFIG, DataType);
			if (!pConfig) continue;
			forces[id] = pConfig;
		}

		id = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
}

static const GForceData * SearchForceData(const std::vector<GForceData> &l, int force_id)
{
	const GForceData *pInfo = NULL;
	for (size_t i(0); i < l.size(); ++ i)
	{
		const GForceData &f  = l[i];
		if (f.force_id == force_id)
		{
			pInfo = &f;
			break;
		}
	}
	return pInfo;
}

static ReturnCode GetRoleDescDetailForce(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	//	���������Ϣ������Ϊ�գ�
	ReturnCode ret = RC_SUCCESS;

	GForceDataList force;
	while (true)
	{
		//	����������
		if (role.force_data.size())
		{
			try
			{
				Marshal::OctetsStream os(role.force_data);
				os >> force;
			}
			catch (...)
			{
				ret = RC_INVALID_ROLEFORCE;
				Log_Info("Exception when parsing GForceDataList! (retcode: %d)", ret);
				break;
			}
		}

		//	��֤���� id �ڱ��������д���
		ForceMap forceMap;
		GetAllForceConfig(forceMap);

		if (force.cur_force_id != 0 && forceMap.find(force.cur_force_id) == forceMap.end())
		{
			ret = RC_FORCE_NOT_EXIST;
			Log_Info("Force %d not exist! (retcode: %d)", force.cur_force_id, ret);
			break;
		}
		for (size_t i = 0; i < force.list.size(); ++ i)
		{
			const GForceData &f = force.list[i];
			if (forceMap.find(f.force_id) == forceMap.end())
			{
				ret = RC_FORCE_NOT_EXIST;
				Log_Info("Force %d not exist! (retcode: %d)", f.force_id, ret);
				break;
			}
		}
		if (ret != RC_SUCCESS)
			break;

		PropertyVectorMaker pvm;

		//	��ʼ�����ڵ�
		const char *szForces = "forces";

		code.begin_node(szForces, pvm.append("name", g_pGame->GetStringFromTable(91)).append("count", forceMap.size()).get());

		//	��ǰ����
		const FORCE_CONFIG *pCurrentForce = force.cur_force_id ? forceMap[force.cur_force_id] : NULL;
		const wchar_t *pForceName = pCurrentForce ? pCurrentForce->name : L"";
		AppendUTF8NameValueNode(code, "current_force_name", g_pGame->GetStringFromTable(92), pForceName);

		//	����������
		for (ForceMap::iterator it = forceMap.begin(); it != forceMap.end(); ++ it)
		{
			const FORCE_CONFIG *pConfig = it->second;
			const GForceData *pInfo = SearchForceData(force.list, pConfig->id);

			const char *szForce = "force";
			code.begin_node(szForce, pvm.clear().append("name", (const wchar_t *)(pConfig->name)).get());

			AString strValue;

			strValue.Format("%d/%d", pInfo ? pInfo->reputation : 0, pConfig->reputation_max);
			AppendUTF8NameValueNode(code, "reputation", g_pGame->GetStringFromTable(93), strValue);

			strValue.Format("%d/%d", pInfo ? pInfo->contribution : 0, pConfig->contribution_max);
			AppendUTF8NameValueNode(code, "contribution", g_pGame->GetStringFromTable(94), strValue);

			code.end_node(szForce);
		}

		//	�����������
		code.end_node(szForces);
		break;
	}

	return ret;
}

static ReturnCode GetRoleBriefExtend(const GNET::GWebTradeRoleBrief &role, GNET::GWebTradeRoleBriefExtend &roleExtend)
{	
	ReturnCode ret = RC_SUCCESS;
	
	//	����������
	if (role.extend_data.size()){
		try
		{
			Marshal::OctetsStream os(role.extend_data);
			os >> roleExtend;
		}
		catch (...)
		{
			ret = RC_INVALID_ROLEBRIEFEXTEND;
			Log_Info("Exception when parsing GWebTradeRoleBriefExtend! (retcode: %d)", ret);
		}
	}
	return ret;
}

static ReturnCode GetRoleMeridian(const GNET::GWebTradeRoleBriefExtend &roleExtend, GNET::GMeridianData &meridian)
{
	ReturnCode ret = RC_SUCCESS;
	if (roleExtend.meridian_data.size()){
		try
		{
			Marshal::OctetsStream os(roleExtend.meridian_data);
			os >> meridian;
		}
		catch (...)
		{
			ret = RC_INVALID_ROLEMERIDIAN;
			Log_Info("Exception when parsing GMeridianData! (retcode: %d)", ret);
		}
	}
	return ret;
}


static ReturnCode GetRoleReincarnation(const GNET::GWebTradeRoleBrief &role, int &reincarnation_count, int &reincarnation_exp, int &max_level, abase::vector<int> &reincarnation_levels)
{
	max_level = role.level;
	reincarnation_count = 0;
	reincarnation_exp = 0;
	reincarnation_levels.clear();
	if (role.reincarnation_data.size() > 0){
		try
		{
			GReincarnationData reincarnation_data;
			Marshal::OctetsStream os(role.reincarnation_data);
			os >> reincarnation_data;
			reincarnation_count = (int)reincarnation_data.records.size();
			reincarnation_exp = reincarnation_data.tome_exp;
			for (int i(0); i < reincarnation_count; ++ i)
			{
				const GReincarnationRecord &record = reincarnation_data.records[i];
				if (max_level < record.level){
					max_level = record.level;
				}
				reincarnation_levels.push_back(record.level);
			}
		}
		catch (...)
		{
			return RC_INVALID_REINCARNATIONDATA;
		}
	}
	return RC_SUCCESS;
}

static ReturnCode GetRoleRealm(const GNET::GWebTradeRoleBrief &role, int &realm_level, int &realm_exp)
{
	realm_level = 0;
	realm_exp = 0;
	if (role.realm_data.size() > 0){
		try
		{
			Marshal::OctetsStream(role.realm_data) >> realm_level >> realm_exp;
		}
		catch (...)
		{
			return RC_INVALID_REALMDATA;
		}
	}
	return RC_SUCCESS;
}

static ReturnCode GetRoleDescDetailMeridian(const GNET::GWebTradeRoleBrief &role, const GNET::GMeridianData &meridian, XmlCoder &code)
{
	//	���������Ϣ
	ReturnCode ret = RC_SUCCESS;
	
	while (true)
	{
		const char *szMeridian = "meridians";		
		PropertyVectorMaker pvm;
		code.begin_node(szMeridian, pvm.append("name", g_pGame->GetStringFromTable(110)).get());

		int layer = meridian.meridian_level/CECGame::MERIDIAN_LEVEL_COUNT;
		int levelInLayer = meridian.meridian_level % CECGame::MERIDIAN_LEVEL_COUNT;

		if (layer < 0 || layer > CECGame::MeridiansLevelLayer){
			ret = RC_INVALID_ROLEMERIDIAN_LAYER;
			Log_Info("Invalid GMeridianData layer=%d ! (retcode: %d)", layer, ret);
			break;
		}

		//	��������
		AppendUTF8NameValueNode(code, "layer", g_pGame->GetStringFromTable(130), g_pGame->GetStringFromTable(131+layer));
		
		//	��Ѩλ���״̬
		{
			const char *szAcupoints = "acupoints";
			int finished = (layer == CECGame::MeridiansLevelLayer ? 1 : 0);	//	�Ƿ������
			code.begin_node(szAcupoints, pvm.clear().append("finished", finished).get());
			for (int i = 0; i < CECGame::MERIDIAN_LEVEL_COUNT; ++ i)	//	i ��ʾѨλ
			{
				static const int ACUPOINT_ORDER[CECGame::MeridiansLevelLayer][CECGame::MERIDIAN_LEVEL_COUNT] = {
					//	Ѩλ�ڸ��������е�˳���±�
					{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
					{0, 1, 2, 3, 4, 5, 6, 7, 8, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 19},
					{18, 17, 16, 15, 14, 13, 12, 11, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 19},
					{1, 3, 5, 7, 9, 11, 13, 15, 17, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0, 19},
				};
				int state = 0;
				if (layer >= CECGame::MeridiansLevelLayer){
					state = 2;		//	�ѳ�
				}else{
					int order = ACUPOINT_ORDER[layer][i];
					if (order < levelInLayer){
						state = 2;	//	�ѳ�
					}else if (order == levelInLayer){
						state = 1;	//	����
					}else{
						state = 0;	//	δ��
					}
				}
				code.append_node("acupoint", pvm.clear().append("pos", i).append("state", state).get());
			}
			code.end_node(szAcupoints);
		}

		//	�������
		{
			int iProp[5];
			if (!g_pGame->GetLevelPropBonus(role.cls, meridian.meridian_level, iProp[0],iProp[1],iProp[2],iProp[3],iProp[4])){
				ret = RC_INVALID_ROLEMERIDIAN_CONFIG;
				Log_Info("GetRoleDescDetailMeridian::Failed to call GetLevelPropBonus(professon=%d,meridian_level=%d) !(retcode: %d)", role.cls, meridian.meridian_level, ret);
				break;
			}
			const char *szAttribute = "attribute";
			code.begin_node(szAttribute, pvm.clear().append("name", g_pGame->GetStringFromTable(140)).get());
			AppendUTF8NameValueNode(code, "hp", g_pGame->GetStringFromTable(141), iProp[0]);
			AppendUTF8NameValueNode(code, "phy_damage", g_pGame->GetStringFromTable(144), iProp[3]);
			AppendUTF8NameValueNode(code, "magic_damage", g_pGame->GetStringFromTable(145), iProp[4]);
			AppendUTF8NameValueNode(code, "phy_defence", g_pGame->GetStringFromTable(142), iProp[1]);
			AppendUTF8NameValueNode(code, "magic_defence", g_pGame->GetStringFromTable(143), iProp[2]);
			code.end_node(szAttribute);
		}

		code.end_node(szMeridian);
		break;
	}

	return ret;
}

struct FateRing 
{
	int	level;
	int	power;
	FateRing():level(0), power(0){}
};
static ReturnCode GetRoleDescDetailGeneralCard(const GNET::GWebTradeRoleBriefExtend &roleExtend, XmlCoder &code)
{
	//	���ս����Ϣ
	ReturnCode ret = RC_SUCCESS;
	
	while (true)
	{
		int leadership(0);
		if (roleExtend.card_leadership.size()){
			try
			{
				Marshal::OctetsStream os(roleExtend.card_leadership);
				os >> leadership;
			}
			catch (...)
			{
				ret = RC_INVALID_ROLE_CARD_LEADERSHIP;
				Log_Info("Exception when parsing leadership! (retcode: %d)", ret);
				break;
			}
		}
		abase::vector<FateRing>	fateRings;
		if (roleExtend.fate_ring_data.size()){
#pragma pack(1)
			struct Dummy
			{
				int		next_update_time;
				int		gain_times;
				size_t	size;					
				struct _entry
				{
					int level;
					int exp;
				}entries[1];
			};
#pragma pack()
			if (roleExtend.fate_ring_data.size() < sizeof(Dummy) - sizeof(Dummy::_entry)){
				ret = RC_INVALID_ROLE_FATE_RING;
				Log_Info("Invalid fate ring data(fate_ring_data.size()=%u)! (retcode: %d)", roleExtend.fate_ring_data.size(), ret);
				break;
			}
			const Dummy *pData = (const Dummy *)(roleExtend.fate_ring_data.begin());
			fateRings.reserve(pData->size);
			FateRing tmp;
			for (int i(0); i < (int)pData->size; ++ i)
			{
				tmp.level = pData->entries[i].level;
				tmp.power = pData->entries[i].exp;
				fateRings.push_back(tmp);
			}
		}
		PropertyVectorMaker pvm;
		const char *szCard = "card";
		code.begin_node(szCard, pvm.clear().append("name", g_pGame->GetStringFromTable(150)).get());

		AppendUTF8NameValueNode(code, "leadership", g_pGame->GetStringFromTable(151), leadership);

		if (fateRings.size()){
			const char *szFateRings = "faterings";
			code.begin_node(szFateRings, pvm.clear().append("name", g_pGame->GetStringFromTable(152)).append("count", fateRings.size()).get());
			for (size_t i(0); i < fateRings.size(); ++ i)
			{
				const PLAYER_SPIRIT_CONFIG *pConfig = g_pGame->GetPlayerSpiritConfig(i);
				if (!pConfig){
					ret = RC_INVALID_PLAYER_SPIRIT_CONFIG;
					Log_Info("Failed to get PLAYER_SPIRIT_CONFIG with type = %d! (retcode: %d)", i, ret);
					return ret;
				}
				const FateRing & tmp = fateRings[i];
				const char *szFateRing = "fatering";
				code.begin_node(szFateRing, pvm.clear().append("name", g_pGame->GetStringFromTable(360+i)).append("pos", i).get());
				AppendUTF8NameValueNode(code, "level", g_pGame->GetStringFromTable(153), tmp.level);
				
				AString strText;
				int levelup_exp = tmp.level < CECGame::CARD_HOLDER::max_holder_level ? pConfig->list[tmp.level].require_power : 0;
				if (levelup_exp == 0) strText.Format("-");
				else strText.Format("%d/%d", tmp.power, levelup_exp);
				AppendUTF8NameValueNode(code, "exp", g_pGame->GetStringFromTable(154), strText);

				code.end_node(szFateRing);
			}
			code.end_node(szFateRings);
		}

		code.end_node(szCard);
		break;
	}
	return ret;
}

static ReturnCode GetRoleDescDetailTitle(const GNET::GWebTradeRoleBriefExtend &roleExtend, XmlCoder &code)
{
	//	��ӳƺ���Ϣ
	ReturnCode ret = RC_SUCCESS;

	while (true)
	{
		typedef unsigned short TITLE_ID;
		abase::vector<TITLE_ID> titles;
		if (roleExtend.title_data.size()){
#pragma pack(1)
			struct Dummy
			{
				TITLE_ID	current_title;
				size_t		size;
				TITLE_ID	titles[1];
			};
#pragma pack()
			if (roleExtend.title_data.size() < sizeof(Dummy) - sizeof(TITLE_ID)){
				ret = RC_INVALID_ROLE_TITLE;
				Log_Info("Invalid title data (title_data.size()=%u)! (retcode: %d)", roleExtend.title_data.size(), ret);
				break;
			}
			const Dummy * pData = (const Dummy *)roleExtend.title_data.begin();
			titles.reserve(pData->size);
			for (size_t i(0); i < pData->size; ++ i)
			{
				titles.push_back(pData->titles[i]);
			}
		}
		
		int	phy_damage(0);					//	�����﹥
		int	magic_damage(0);				//	���ӷ���
		int	phy_defence(0);					//	�������
		int	magic_defence(0);				//	���ӷ���
		int	attack(0);						//	��������
		int	armor(0);						//	��������
		elementdataman* pDBMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		for (size_t i(0); i < titles.size(); ++ i)
		{
			if (titles[i] == 0){
				continue;
			}
			TITLE_CONFIG *pDB = (TITLE_CONFIG*)pDBMan->get_data_ptr(titles[i], ID_SPACE_CONFIG, DataType);				
			if (DataType != DT_TITLE_CONFIG && DataType != DT_COMPLEX_TITLE_CONFIG){
				ret = RC_INVALID_ROLE_TITLE_CONFIG;
				Log_Info("Failed to find title(%u)! (retcode: %d)", titles[i], ret);
				return ret;
			}
			phy_damage += pDB->phy_damage;
			magic_damage += pDB->magic_damage;
			phy_defence += pDB->phy_defence;
			magic_defence += pDB->magic_defence;
			attack += pDB->attack;
			armor += pDB->armor;
		}
		
		PropertyVectorMaker pvm;
		const char *szTitles = "titles";
		code.begin_node(szTitles, pvm.clear().append("name", g_pGame->GetStringFromTable(120)).get());
		AppendUTF8NameValueNode(code, "phy_damage", g_pGame->GetStringFromTable(121), phy_damage);
		AppendUTF8NameValueNode(code, "magic_damage", g_pGame->GetStringFromTable(122), magic_damage);
		AppendUTF8NameValueNode(code, "phy_defence", g_pGame->GetStringFromTable(123), phy_defence);
		AppendUTF8NameValueNode(code, "magic_defence", g_pGame->GetStringFromTable(124), magic_defence);
		AppendUTF8NameValueNode(code, "attack", g_pGame->GetStringFromTable(125), attack);
		AppendUTF8NameValueNode(code, "armor", g_pGame->GetStringFromTable(126), armor);
		code.end_node(szTitles);

		break;
	}
	return ret;
}

static ReturnCode GetRoleDescDetailReincarnation(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{
	//	���ת����Ϣ������Ϊ�գ�
	ReturnCode rc = RC_SUCCESS;
	int reincarnation_count(0), reincarnation_exp(0), max_level(0);
	abase::vector<int> reincarnation_levels;
	if ((rc = GetRoleReincarnation(role, reincarnation_count, reincarnation_exp, max_level, reincarnation_levels)) == RC_SUCCESS){
		AppendUTF8NameValueNode(code, "reincarnation_levels", g_pGame->GetStringFromTable(40), reincarnation_levels);
		AppendUTF8NameValueNode(code, "max_level", g_pGame->GetStringFromTable(39), max_level);
		AppendUTF8NameValueNode(code, "reincarnation_count", g_pGame->GetStringFromTable(35), reincarnation_count);
		AppendUTF8NameValueNode(code, "reincarnation_exp", g_pGame->GetStringFromTable(36), reincarnation_exp);
	}
	return rc;
}

static ReturnCode GetRoleDescDetailRealm(const GNET::GWebTradeRoleBrief &role, XmlCoder &code)
{	
	//	���硢���羭��
	ReturnCode rc = RC_SUCCESS;
	int realm_level(0), realm_exp(0);
	if ((rc = GetRoleRealm(role, realm_level, realm_exp)) == RC_SUCCESS){
		AppendUTF8NameValueNode(code, "realm_level", g_pGame->GetStringFromTable(37), g_pGame->GetRealmName(realm_level));
		AppendUTF8NameValueNode(code, "realm_exp", g_pGame->GetStringFromTable(38), realm_exp);
	}
	return rc;
}

void  processOneDemand(const GNET::GetItemDesc &in, GNET::GetItemDesc_Re &out)
{
	MyCriticalSection dump1(g_csProcessDemand);
		
	// ��ӡ���յ���Э����Ϣ
	SYSTEMTIME st;
	GetLocalTime(&st);
	Log_Info("[%d:%02d:%02d-%02d:%02d:%02d] sn=%I64d, timestamp=%I64d, detail.size=%d, type=item.",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		in.sn, in.timestamp, in.detail.size());

	// �����ֽ�����Ӧ��Э������
	GRoleInventory item;

	out.sn = in.sn;
	out.timestamp = in.timestamp;

	try
	{
		Marshal::OctetsStream(in.detail) >> item;
	}catch (...)
	{
		// ����Э��������ʱ��������
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("Exception when parsing GRoleInventory! (retcode: %d)", out.retcode);
		return;
	}

	// ��¼��Ʒ�Ļ�����Ϣ
	Log_Info("item.id = %d, item.data.size = %d", item.id, item.data.size());
	
	// ����Э�����ݣ�������Ӧ��Ʒ

	if (item.id == 3044)
	{
		// ����Ϸ�н�Ǯ�����⴦��
		//
		out.retcode = RC_SUCCESS;

		out.id = item.id;
		out.level = 0;

		AString strName = UnicodeToUTF8(g_pGame->GetStringFromTable(11));
		out.name.replace((const void *)(const char *)strName, strName.GetLength());

		AString strIcon = GetItemIcon(0);
		out.icon.replace((const void *)(const char *)strIcon, strIcon.GetLength());

		AString strHint = UnicodeToUTF8(g_pGame->GetStringFromTable(12));
		out.desc.replace((const void *)(const char *)strHint, strHint.GetLength());

		return;
	}

	CECIvtrItem *pItem = CECIvtrItem::CreateItem(item.id, item.expire_date, item.count);
	if (!pItem)
	{
		// ��Ʒ����ʧ��
		out.retcode = RC_ITEM_NOT_EXIST;
		Log_Info("Cannot create item %d from GRoleInventory! (retcode: %d)", item.id, out.retcode);
		return;
	}
	
	//	�Զ�ɾ����Ʒ
	auto_delete<CECIvtrItem> tmp(pItem);
	
	pItem->SetProcType(item.proctype);
	if (item.data.begin() && item.data.size())
		pItem->SetItemInfo((BYTE *)item.data.begin(), item.data.size());
	else
		pItem->SetItemInfo(NULL, 0);
	
	// �Ӵ�����Ʒ����ʾ��Ϣ�����
	out.retcode = RC_SUCCESS;
	
	out.id = pItem->GetTemplateID();
	out.level = pItem->GetItemLevel();
	
	AString strName = UnicodeToUTF8(pItem->GetName());
	out.name.replace((const void *)(const char *)strName, strName.GetLength());
	
	AString strIcon = GetItemIcon(pItem->GetTemplateID());
	out.icon.replace((const void *)(const char *)strIcon, strIcon.GetLength());
	
	pItem->SetPriceScale(CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);
	const wchar_t *pszHint = pItem->GetDesc(CECIvtrItem::DESC_NORMAL, NULL);
	AString strHint = UnicodeToUTF8(ReverseTranslate(pszHint));
	out.desc.replace((const void *)(const char *)strHint, strHint.GetLength());
}

AString UnicodeToUTF8(const wchar_t *str)
{
	// �� Unicode ���ַ� wchar_t ת��Ϊ UTF8 ���ֽ���
	
	AString result;
	int textlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	if (textlen>0)
	{
		char *buffer = new char[textlen+1];
		memset(buffer, 0, sizeof(char)*(textlen+1));
		WideCharToMultiByte(CP_UTF8, 0, str, -1, buffer, textlen, NULL, NULL);
		result = buffer;
		delete []buffer;
	}
	return result;
}

AWString UTF8ToUnicode(const char *str)
{
	int len = strlen(str);
	int wlen = ::MultiByteToWideChar(CP_UTF8, NULL, str, len, NULL, 0);
	wchar_t *wStr = new wchar_t[wlen+1];
	::MultiByteToWideChar(CP_UTF8, NULL, str, len, wStr, wlen);
	wStr[wlen] = L'\0';
	AWString ret = wStr;
	delete []wStr;
	return ret;
}

void initMultiThreadAccess()
{
	InitializeCriticalSection(&g_csException);
	InitializeCriticalSection(&g_csProcessDemand);
	InitializeCriticalSection(&g_csLog);
}

void destroyMultiThreadAccess()
{
	DeleteCriticalSection(&g_csException);
	DeleteCriticalSection(&g_csLog);
	DeleteCriticalSection(&g_csProcessDemand);
}

static ReturnCode GetSNSRoleEquipmentDesc(const GNET::SNSRoleEquipment &sns_equip, AString &desc)
{
	//	��ȡ��ɫ����ϸ��Ϣ����

	ReturnCode ret = RC_SUCCESS;

	XmlCoder code;

	//	��������Ӱ汾��Ϣ���Ա�����޸�
	//	<role version="1.0">
	while (true)
	{
		const char *szRoot = "root";
		code.begin_node(szRoot, "version", "1.0");
		
		//	װ����Ϣ
		if ((ret = GetEquipmentsDesc(sns_equip.equipment, code)) != RC_SUCCESS)
			break;
		
		//	</role>
		code.end_node(szRoot);		
		desc = code.c_str();
		break;
	}
	return ret;
}

void processOneDemand(const GNET::GetRoleEquipmentDesc &in, GNET::GetRoleEquipmentDesc_Re &out)
{
	MyCriticalSection dump1(g_csProcessDemand);
		
	// ��ӡ���յ���Э����Ϣ
	SYSTEMTIME st;
	GetLocalTime(&st);
	Log_Info("[%d:%02d:%02d-%02d:%02d:%02d] id=%I64d, detail.size=%d, type=sns_equipments.",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		in.id, in.detail.size());
	
	// �����ֽ�����Ӧ��Э������
	SNSRoleEquipment sns_equip;

	out.id = in.id;
	out.zoneid = in.zoneid;

	try
	{
		Marshal::OctetsStream(in.detail) >> sns_equip;
	}catch (...)
	{
		// ����Э��������ʱ��������
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("Exception when parsing SNSRoleEquipment! (retcode: %d)", out.retcode);
		return;
	}

	// ��¼������Ϣ
	Log_Info("crc = %d, equipment.size = %d", sns_equip.crc, sns_equip.equipment.inv.size());
	
	//	����Э�����ݣ����ɴ�����
	out.crc = sns_equip.crc;

	AString strDesc;
	if ((out.retcode = GetSNSRoleEquipmentDesc(sns_equip, strDesc)) == RC_SUCCESS)
		out.desc.replace((const void *)(const char *)strDesc, strDesc.GetLength());
}

ReturnCode GetSNSRolePetCorralDesc(const GNET::SNSRolePetCorral &sns_petcorral, AString &desc)
{
	//	��ȡ��ɫ����ϸ��Ϣ����

	ReturnCode ret = RC_SUCCESS;

	XmlCoder code;

	//	��������Ӱ汾��Ϣ���Ա�����޸�
	//	<role version="1.0">
	while (true)
	{
		const char *szRoot = "root";
		code.begin_node(szRoot, "version", "1.0");
		
		//	��������Ϣ
		if ((ret = GetPetCorralDesc(sns_petcorral.petcorral, code)) != RC_SUCCESS)
			break;
		
		//	</role>
		code.end_node(szRoot);		
		desc = code.c_str();
		break;
	}
	return ret;
}

void processOneDemand(const GNET::GetRolePetCorralDesc &in, GNET::GetRolePetCorralDesc_Re &out)
{
	MyCriticalSection dump1(g_csProcessDemand);
		
	// ��ӡ���յ���Э����Ϣ
	SYSTEMTIME st;
	GetLocalTime(&st);
	Log_Info("[%d:%02d:%02d-%02d:%02d:%02d] id=%I64d, detail.size=%d, type=sns_petcorral.",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		in.id, in.detail.size());
	
	// �����ֽ�����Ӧ��Э������
	SNSRolePetCorral sns_petcorral;

	out.id = in.id;
	out.zoneid = in.zoneid;

	try
	{
		Marshal::OctetsStream(in.detail) >> sns_petcorral;
	}catch (...)
	{
		// ����Э��������ʱ��������
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("Exception when parsing SNSRolePetCorral! (retcode: %d)", out.retcode);
		return;
	}

	// ��¼������Ϣ
	Log_Info("crc = %d, octets.size = %d", sns_petcorral.crc, sns_petcorral.petcorral.size());
	
	//	����Э�����ݣ����ɴ�����
	out.crc = sns_petcorral.crc;

	AString strDesc;
	if ((out.retcode = GetSNSRolePetCorralDesc(sns_petcorral, strDesc)) == RC_SUCCESS)
		out.desc.replace((const void *)(const char *)strDesc, strDesc.GetLength());
}

ReturnCode GetSNSRoleSkillDesc(const GNET::SNSRoleSkills &sns_skills, AString &desc)
{
	//	��ȡ��ɫ����ϸ��Ϣ����

	ReturnCode ret = RC_SUCCESS;

	XmlCoder code;

	//	��������Ӱ汾��Ϣ���Ա�����޸�
	//	<role version="1.0">
	while (true)
	{
		const char *szRoot = "root";
		code.begin_node(szRoot, "version", "1.0");
		
		//	��������Ϣ
		if ((ret = GetSkillsDesc(sns_skills.skills, code)) != RC_SUCCESS)
			break;
		
		//	</role>
		code.end_node(szRoot);		
		desc = code.c_str();
		break;
	}
	return ret;
}

void processOneDemand(const GNET::GetRoleSkillDesc &in, GNET::GetRoleSkillDesc_Re &out)
{
	MyCriticalSection dump1(g_csProcessDemand);
		
	// ��ӡ���յ���Э����Ϣ
	SYSTEMTIME st;
	GetLocalTime(&st);
	Log_Info("[%d:%02d:%02d-%02d:%02d:%02d] id=%I64d, detail.size=%d, type=sns_skills.",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		in.id, in.detail.size());
	
	// �����ֽ�����Ӧ��Э������
	SNSRoleSkills sns_skills;

	out.id = in.id;
	out.zoneid = in.zoneid;

	try
	{
		Marshal::OctetsStream(in.detail) >> sns_skills;
	}catch (...)
	{
		// ����Э��������ʱ��������
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("Exception when parsing SNSRoleSkills! (retcode: %d)", out.retcode);
		return;
	}

	// ��¼������Ϣ
	Log_Info("crc = %d, octets.size = %d", sns_skills.crc, sns_skills.skills.size());
	
	//	����Э�����ݣ����ɴ�����
	out.crc = sns_skills.crc;

	AString strDesc;
	if ((out.retcode = GetSNSRoleSkillDesc(sns_skills, strDesc)) == RC_SUCCESS)
		out.desc.replace((const void *)(const char *)strDesc, strDesc.GetLength());
}

static inline bool getHexCharValue(char c, byte &b)
{
	//	��ȡ����16�����ַ��������16����ֵ
	bool bSuccess(false);
	if (c >= '0' && c <= '9')
	{
		b = c-'0';
		bSuccess = true;
	}
	else if (c >= 'a' && c <= 'f')
	{
		b = c-'a'+10;
		bSuccess = true;
	}
	else if (c >= 'A' && c <= 'F')
	{
		b = c-'A'+10;
		bSuccess = true;
	}
	return bSuccess;
}

static inline bool getByteValue(char c1, char c2, byte &b)
{
	//	��ȡ����16�����ַ���������ֽ�ֵ
	bool bSuccess(false);

	byte b1(0), b2(0);
	if (getHexCharValue(c1, b1) &&
		getHexCharValue(c2, b2))
	{
		b = (b1<<4)+b2;
		bSuccess = true;
	}

	return bSuccess;
}

static bool convertOctetsAsHexChars(const GNET::Octets &in, GNET::Octets &out)
{
	//	�� Octets ����ȡ 16 �����ֽ���
	bool bSuccess(false);

	size_t in_size = in.size();
	while (in_size && (in_size%2 == 0))
	{
		size_t out_size = in_size/2;
		out.resize(out_size);

		const char *p = NULL;
		byte *q = NULL;
		byte b = NULL;
		for (p = (const char *)in.begin(), q=(byte *)out.begin(); p != in.end(); p+=2, ++q)
		{
			if (!getByteValue(*p, *(p+1), b))
				break;
			*q = b;
		}

		if (p != in.end())
		{
			//	in �����ַ����󣬲��� 16 �����ַ�ֵ
			break;
		}

		bSuccess = true;
		break;
	}

	return bSuccess;
}

void  processOneDemand(const GNET::GetEncryptedItemDesc &in, GNET::GetEncryptedItemDesc_Re &out)
{
	MyCriticalSection dump1(g_csProcessDemand);

	// ��ӡ���յ���Э����Ϣ
	SYSTEMTIME st;
	GetLocalTime(&st);
	Log_Info("[%d:%02d:%02d-%02d:%02d:%02d] tid=%I64d, encrypted_item.size=%d, checksum.size=%d, type=encrypteditem.",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		in.tid, in.encrypted_item.size(), in.checksum.size());

	// �����ֽ�����Ӧ��Э������

	out.tid = in.tid;

	GNET::Octets encryptedGRoleInventory, checksum;
	if (!convertOctetsAsHexChars(in.encrypted_item, encryptedGRoleInventory) ||
		!convertOctetsAsHexChars(in.checksum, checksum))
	{
		// �������ı���ʾ�� Octets ����ȡ�ֽ���ʱ��������
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("Invalid octets format! (retcode: %d)", out.retcode);
		return;
	}

	byte key_buf[128] = {
		0xbf, 0x61, 0x09, 0x72, 0x1d, 0xbb, 0x6e, 0xe9, 0x9a, 0x01, 0x3d, 0x47, 0x0a, 0x11, 0x60, 0xbe, 
		0xf8, 0x48, 0x36, 0x3f, 0xdf, 0xf5, 0x9a, 0xc5, 0xca, 0x1c, 0x79, 0xcb, 0xbb, 0x90, 0x96, 0xde, 
		0x58, 0xff, 0x60, 0xbe, 0x8d, 0x4e, 0xd8, 0xe1, 0x83, 0x47, 0xd0, 0x57, 0x6d, 0x62, 0x49, 0x66, 
		0x71, 0x0d, 0x6a, 0xfe, 0x3b, 0xdc, 0xd2, 0x33, 0xd7, 0x8f, 0x5c, 0x5e, 0x4b, 0x86, 0x25, 0xd0, 
		0x6f, 0xd2, 0xd0, 0x8c, 0x53, 0x46, 0xd0, 0xc4, 0x3a, 0x10, 0xae, 0xcf, 0x75, 0xcd, 0xfd, 0x74,
		0xe8, 0xfe, 0x80, 0x0b, 0x14, 0xf9, 0x94, 0x66, 0xcc, 0x19, 0x7d, 0xda, 0x89, 0x27, 0x91, 0xab, 
		0x5c, 0x73, 0xb0, 0x78, 0xc9, 0x1c, 0x48, 0xd9, 0xe2, 0xc3, 0xfc, 0xfa, 0x3e, 0x74, 0xb9, 0xec, 
		0x81, 0x66, 0x5f, 0x5b, 0x3e, 0x73, 0x1e, 0xd2, 0x31, 0x99, 0xc7, 0xb5, 0xe7, 0x3e, 0xbc, 0x02,
	};
	ARCFourSecurity arc4;
	arc4.SetParameter(Octets(key_buf, sizeof(key_buf)/sizeof(key_buf[0])));
	GNET::Octets decryptedGRoleInventory = arc4.Update(encryptedGRoleInventory);

	GNET::Octets md5 = GNET::MD5Hash::Digest(decryptedGRoleInventory);
	if (md5 != checksum)
	{
		//	�������ݺ� md5 ��֤����
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("MD5  mismatch! (retcode: %d)", out.retcode);
		return;
	}

	GRoleInventory item;
	try
	{
		Marshal::OctetsStream(decryptedGRoleInventory) >> item;
	}catch (...)
	{
		// ����Э��������ʱ��������
		out.retcode = RC_PARSE_PROTOCOL_STREAM;
		Log_Info("Exception when parsing GRoleInventory! (retcode: %d)", out.retcode);
		return;
	}

	// ��¼��Ʒ�Ļ�����Ϣ
	Log_Info("item.id = %d, item.data.size = %d", item.id, item.data.size());

	// ����Э�����ݣ�������Ӧ��Ʒ

	if (item.id == 3044)
	{
		// ����Ϸ�н�Ǯ�����⴦��
		//
		out.retcode = RC_SUCCESS;

		AString strHint = UnicodeToUTF8(g_pGame->GetStringFromTable(12));
		out.desc.replace((const void *)(const char *)strHint, strHint.GetLength());

		return;
	}

	CECIvtrItem *pItem = CECIvtrItem::CreateItem(item.id, item.expire_date, item.count);
	if (!pItem)
	{
		// ��Ʒ����ʧ��
		out.retcode = RC_ITEM_NOT_EXIST;
		Log_Info("Cannot create item %d from GRoleInventory! (retcode: %d)", item.id, out.retcode);
		return;
	}

	//	�Զ�ɾ����Ʒ
	auto_delete<CECIvtrItem> tmp(pItem);

	pItem->SetProcType(item.proctype);
	if (item.data.begin() && item.data.size())
		pItem->SetItemInfo((BYTE *)item.data.begin(), item.data.size());
	else
		pItem->SetItemInfo(NULL, 0);

	// �Ӵ�����Ʒ����ʾ��Ϣ�����
	out.retcode = RC_SUCCESS;

	pItem->SetPriceScale(CECIvtrItem::SCALE_SELL, PLAYER_PRICE_SCALE);
	const wchar_t *pszHint = pItem->GetDesc(CECIvtrItem::DESC_NORMAL, NULL);
	AString strHint = UnicodeToUTF8(ReverseTranslate(pszHint));
	out.desc.replace((const void *)(const char *)strHint, strHint.GetLength());
}

void	outputToFile(const char *szProtocolDesc, char outputReason, __int64 id, const GNET::Octets &details)
{
	static bool s_output = false;
	static bool s_initialized = false;
	if (!s_initialized)
	{
		s_initialized = true;
		
		AString strNetworkConfig = g_szWorkDir;
		strNetworkConfig += "\\client.conf";
		if (_access(strNetworkConfig, R_OK) == 0)
		{
			Conf *conf = Conf::GetInstance(strNetworkConfig);
			GTransformClient *manager = GTransformClient::GetInstance();
			s_output = atoi(conf->find(manager->Identification(), "sns_err_file").c_str()) != 0;
		}
	}

	if (!s_output)	return;

	AString strOutputDir = g_szWorkDir;
	strOutputDir += "\\logs";
	
	//	�����������������ļ�����
	AString strFilePath;
	int count = 0;
	while (true)
	{
		if (count == 0)
		{
			//	����Ҫʱ����ʹ�ö����������׺
			strFilePath.Format("%s\\err_%s_%d_%I64d", strOutputDir, szProtocolDesc, outputReason, id);
		}
		else
		{
			//	�ж��ǰ׺�ظ��ļ����������ֺ�׺
			strFilePath.Format("%s\\err_%s_%d_%I64d_%d", strOutputDir, szProtocolDesc, outputReason, id, count);
		}
		if (_access(strFilePath, 0) == -1)
		{
			//	�ļ������ڣ��������ڱ���д������
			break;
		}

		//	������ţ������´γ���
		++count;
	}

	//	���ļ�д��
	bool bSuccess(false);
	FILE *file = fopen(strFilePath, "w");
	if (file != NULL)
	{
		fwrite(details.begin(), 1, details.size(), file);
		fflush(file);
		fclose(file);

		bSuccess = true;
	}

	Log_Info("write data to file %s...%s", strFilePath, bSuccess ? "success":"fail");
}