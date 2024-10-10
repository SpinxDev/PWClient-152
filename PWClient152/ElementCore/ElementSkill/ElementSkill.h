#ifndef __ELEMENT_SKILL_H
#define __ELEMENT_SKILL_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ELEMENTSKILL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ELEMENTSKILL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef ELEMENTSKILL_EXPORTS
#define ELEMENTSKILL_API __declspec(dllexport)
#else
#define ELEMENTSKILL_API __declspec(dllimport)
#endif

#include <vector>
#include <utility>

namespace GNET
{
enum skill_type{
    TYPE_ATTACK   = 1,  // ��������
    TYPE_BLESS    = 2,  // ����ף��
    TYPE_CURSE    = 3,  // ��������
    TYPE_SUMMON   = 4,  // ��Ʒ����
    TYPE_PASSIVE  = 5,  // ����
    TYPE_ENABLED  = 6,  // ��������
    TYPE_LIVE     = 7,  // ����
    TYPE_JUMP     = 8,  // ˲��
	TYPE_PRODUCE  = 9,  // ����(����)
	TYPE_BLESSPET = 10,  // ����ף��
	TYPE_NEUTRALBLESS = 11,	//����ף��
};

enum range_type{
	TYPE_POINT = 0,          // �� 
	TYPE_LINE  = 1,          // �� 
	TYPE_SELFSPHERE  = 2,    // ����Ϊ���ĵ���
	TYPE_TARGETSPHERE = 3,   // Ŀ��Ϊ���ĵ���
	TYPE_TAPER  = 4,         // Բ׶
	TYPE_SLEF  = 5,          // ����
};

enum form{
	FORM_MASK_HIGH	= 0xC0,
	FORM_NORMAL		= 0,	//��ͨ��̬
	FORM_CLASS		= 1,	//ְҵ����
	FORM_BEASTIE	= 2,	//��С����
};

struct LearnRequirement
{
	int level;		//��Ҽ���
	int sp;         //���ܵ�
	int money;      //��Ǯ
	int profession; //ְҵ
	int rank;       //���漶��
	int realm_level;//����ȼ�
};

struct PetRequirement
{
	int  level;		 //���Ｖ��
	int  sp;         //���˼��ܵ�
	int* list;       //���м����б�
	int  lsize;      //�б��С
};

struct GoblinRequirement
{
	int  genius[5];	  //С���鼼���츳��
	int  profession;  //ְҵ
	int  sp;		  //����Ԫ��
	int  money;		  //���˽�Ǯ
	int  level;		  //С����ȼ�
	int  mp;		  //С���鵱ǰ��������
};

struct ComboSkillState
{
	enum{ MAX_COMBO_ARG = 3 };
	unsigned int skillid;
	int arg[MAX_COMBO_ARG];
	ComboSkillState():skillid(0)
	{
		memset(arg, 0, sizeof(arg));
	}
};

struct UseRequirement
{
	int mp;            //ħ��
	int ap;            //ŭ��ֵ
	int form;          //����״̬
	int weapon;        //����
	int freepackage;   //������ʣ����Ŀ
	int arrow;         //װ����֧��Ŀ
	int move_env;	//�ƶ�����
	bool is_combat;   //�Ƿ�ս��״̬
	int hp;				//��ǰhp
	int max_hp;			//���hp
	ComboSkillState combo_state;	//����������
};

struct GoblinUseRequirement
{
	int mp;			//����(Ԫ��)
	int ap;			//����(����)
	int genius[5];	//С���鼼���츳��
	int profession; //����ְҵ
	int level;		//С����ȼ������ڼ�����������
	int move_env;	//���˵�ǰ�ƶ����� 
};

class SkillStub;

class ELEMENTSKILL_API SkillStr
{
public:
	virtual wchar_t* Find(int id) const {  return L""; }
};

class ELEMENTSKILL_API ElementSkill
{
public:
	enum {
		SKILL_PERFORM,
		SKILL_DONE,
	};
	typedef unsigned int ID;
	
	static ID NextSkill(ID id = 0);

	// ��������
	virtual const wchar_t* GetName() { return NULL;}
	virtual const char* GetNativeName() { return NULL;}
	// �������,��skill_type
	virtual char GetType() const { return 1; }        
	// ����ͼ��
	virtual const char* GetIcon() { return NULL; }
	// ����˵��
	virtual const wchar_t* GetIntroduction(wchar_t* buf,int len,const SkillStr& table) { return L""; }
	// ����ְҵ����
	virtual int GetCls() const { return -1; }
	// ������ȴʱ�䣬��λ����
	virtual int GetCoolingTime() { return 5000; }
	// ����ִ��ʱ�䣬��λ����
	virtual int GetExecuteTime() { return 1000; }
	// Ŀ����������, 0:����Ŀ�꣬1:��ҪĿ�꣬2:Ŀ�������ʬ��, 3:Ŀ�����Ϊ����, 4:Ŀ�����Ϊ���� 
	virtual int GetTargetType(){return 0;}
	// ������Ч�ͷž���: <-0.001����Ŀ�� -0.001-0.001 Ĭ�Ϲ������룬>0.001 �ͷž���
	virtual float GetPrayRange(float range, float prayplus) { return 0; }  


	// �������漶��
	virtual int GetRank() { return 0; }
	// ѧϰn������Ҫ�����Ҽ���
	virtual int GetRequiredLevel() { return 0;}
	// ѧϰn��������Ҫ�ļ��ܵ�
	virtual int GetRequiredSp() { return 0;}
	// ѧϰn����Ҫ�ļ�����
	virtual int GetRequiredBook() { return 0;}
	// ѧϰ��Ҫ��Ǯ
	virtual int GetRequiredMoney() { return 0;}
	// ѧϰ���󾳽�ȼ�
	virtual int GetRequiredRealmLevel() { return 0; }
	// ǰ�Ἴ��
	virtual const std::vector<std::pair<unsigned int, int> > & GetRequiredSkill() = 0;
	// ��ʾ˳��
	virtual int GetShowOrder() { return 0; }
	// ���ü��ܼ���
	virtual int SetLevel(int) { return 0; } 
	// ������󼶱�
	virtual int GetMaxLevel() { return 0; }

	// �Ƿ���������
	virtual bool IsWarmup() { return false; }
	// ʹ�ú��Ƿ��Զ�����
	virtual bool IsAutoAttack(){return false;}
	// ˲������
	virtual bool IsInstant(){return false;}
	// �Ƿ��������
	virtual bool IsDurative() { return false; }
	// ɱ�˷�Χ������
	virtual int  GetRangeType() { return 0; }
	// �ͷŻ��������С����桢ˮ��
	virtual int  GetCastEnv() { return 0; }
	// ��ȡС����ѧϰ���ͷż��������츳��
	virtual int* GetRequiredGenius(int idSkill) { return NULL; }

	// Ч���ļ���
	virtual const char* GetEffect() { return NULL;}
	virtual const char* GetElseEffect() { return NULL;}

	// ʹ����ҪMP
	virtual int GetMpCost(){return 1;}

	// ʹ����ҪAP
	virtual int GetApCost(){return 0;}

	// ��֧����
	virtual int GetArrowCost(){return 0;}

	// ���������ж�
	virtual bool ValidWeapon(int w) const { return true; }
	// 0, �ɹ���1��������ƥ�䣻2, mp���㣻3��λ�����������㣻4��������������㣻5������ID, 6��δѡ��Ŀ��
	int Condition(UseRequirement& info);

	virtual bool IsAllowLand(){ return true; }
	virtual bool IsAllowWater(){ return true; }
	virtual bool IsAllowAir(){ return true; }
	virtual bool GetNotuseInCombat(){ return false; }
	//�Ƿ��ƶ�ʩ��
	virtual bool IsMovingSkill() { return false; }
	// �����ܷ��ڵ�ǰ����״̬��ʹ��
	bool IsValidForm(char form);
	virtual char GetAllowForms() { return 0; }
	virtual bool Interrupt() { return true; }
	int GetAbilityPercent();

	// ��ȡ���Ӧ�ĳ�������
	virtual std::vector<std::pair<unsigned int, int> > GetJunior() { return std::vector<std::pair<unsigned int, int> >(); };

	// ��ȡС���鼼��ְҵ����
	virtual int GetClsLimit() { return 0; }
	
	//��ȡ������ȴmask bit0-4 ������ȴ0-4 bit5-9 ��Ʒ��ȴ0-4
	virtual int GetCommonCoolDown() { return 0; }

	//��ȡ������ȴʱ�䣬��λ����
	virtual int GetCommonCoolDownTime() { return 0; }

	//��ȡ����������Ʒ
	virtual int GetItemCost() { return 0; }

	//��ȡ������ʩ��ǰ��
	virtual int GetComboSkPreSkill(){ return 0; }

	//���ʩ��ʱhp����
	virtual bool CheckHpCondition(int hp, int max_hp) { return true; }

	//�����������������
	virtual bool CheckComboSkExtraCondition(){ return true; } 
	
	// 0:�ɹ�            1:SP����            2:���𲻹�
	// 3:����            4:���岻ƥ��        5:����ID
	// 6:��Ǯ����        7:���漶������    8:û�м�����
	// 9:û��ǰ�Ἴ��    10:�����Ȳ���       11:�Ѿ�ѧ���˸߼�����
	// 12:��������������
	static int LearnCondition(ID id, LearnRequirement& info, int level ); 

	// 0:�ɹ�            1:������ƥ��        2:mp����
	// 3:λ������������  4:�������������    5:����ID
	// 6:δѡ��Ŀ��      7:����״̬����      8:ŭ��ֵ����
	// 9:��֧����        10:������û��ʣ��λ�� 11: ������ս����ʹ��
	// 12:HP����������   13:������ǰ�᲻����
	static int Condition(ID id, UseRequirement& performer, int level );

	// 0:�ɹ�            1:����SP����        2:���𲻹�
	// 3:����            9:ǰ�Ἴ�ܼ��𲻹�  5:����ID
	static int PetLearn(ID id, PetRequirement& info, int level); 

	// 0:�ɹ�            1:����SP����        2:�����츳�㲻��
	// 3:����            4:���ܸ�������		 5:����ID
	// 6:��Ǯ����		 7:��С���鼼��		 8:û�м�����
	// 9:�ȼ�����       10:�������޲���     11:ְҵ��ƥ��
    // 12:�������޲��㣬��ְҵ��ƥ��
	static int GoblinLearn(ID id, GoblinRequirement& info, int level); 

	// 0:�ɹ�            1:ְҵ��ƥ��        2:mp����
	// 3:AP����			 4:�츳�㲻��		 5:����ID
	// 6:δѡ��Ŀ��      7:��С���鼼��      8:�ƶ�����������
	static int GoblinCondition(ID id, GoblinUseRequirement& info, int level );

	//��ȡְҵ��������
	static const std::vector<ID>& GetInherentSkills(int cls);

	//��ȡ������������б�
	static void GetComboSkActivated(const ComboSkillState & combo_state, std::vector<std::pair<ID, int> >& list);
	//�Ƿ��ƶ�ʩ��
	static bool IsMovingSkill(ID id);

	static char GetType(ID id);  
	static const char* GetIcon(ID id);
	static const wchar_t* GetName(ID id);
	static const char* GetNativeName(ID id);
	static const char* GetEffect(ID id);
	static const char* GetElseEffect(ID id);
	static const wchar_t* GetIntroduction(ID id, int level, wchar_t* buf, unsigned int len, const SkillStr& table);
	static int GetRequiredLevel(ID id, int level);
	static int GetRequiredSp(ID id, int level);
	static int GetRequiredBook(ID id, int level);
	static int GetRequiredMoney(ID id, int level);
	static int GetRequiredRealmLevel(ID id, int level);
	static bool IsValidForm(ID id, char form);
	static int GetExecuteTime(ID id, int level);
	static int GetAbilityPercent(ID id);
	static std::vector<std::pair<unsigned int, int> > GetJunior(ID id);
	static int GetCommonCoolDown(ID id);
	static int GetCommonCoolDownTime(ID id);
	static int GetItemCost(ID id);
	
	static bool IsGoblinSkill(ID id);
	static int GetComboSkPreSkill(ID id);
	
	//���Ǽ���������  <0û��ѧ��˼��� >=0 ������
	static int GetAbility(ID id); 
	static int SetAbility(ID id, int ability);
	static int SetLevel(ID id, int level);
	static int GetLevel(ID id);
	static bool IsOverridden(ID id);
	static int GetMaxAbility(ID id, int level);

	static void LoadSkillData(void * data);

	static ElementSkill* Create(ID id, int level);
	void Destroy();

	static int GetVersion();
	
	//
	// ��ʼ�������ڲ���̬���ݣ���ʹ�ÿ�֮ǰ���ã�����
	//
	static void InitStaticData();
};

class ELEMENTSKILL_API VisibleState
{
public:
	virtual const int GetID() const { return 0; }
	virtual const wchar_t* GetName() const {  return NULL; }
	virtual const char* GetHH() const { return NULL; }
	virtual const char* GetEffect() const {  return NULL; }
	static const VisibleState* Query(int profession, int id);
};

class ELEMENTSKILL_API TeamState
{
public:
	virtual const int GetID() const { return 0; }
	virtual const wchar_t* GetName() const {  return NULL; }
	virtual const char* GetIcon() const { return NULL; }
	static const TeamState* Query(int id);
};

}

#endif
