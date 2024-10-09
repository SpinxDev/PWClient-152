#ifndef __SKILL_PLAYERWRAPPER_H
#define __SKILL_PLAYERWRAPPER_H


namespace GNET
{
struct PlayerInfo;
struct ComboArg
{
	enum{ MAX_COMBO_ARG = 3 };
    int arg[MAX_COMBO_ARG];
    ComboArg()
	{ 
		memset(arg, 0, sizeof(arg));
	}
    int GetValue(unsigned int index)
	{
		return index < MAX_COMBO_ARG ? arg[index] : 0;
	}
    void SetValue(unsigned int index, int value)
	{
		if(index < MAX_COMBO_ARG) arg[index] = value;
	}
};
class PlayerWrapper
{
public:
	typedef unsigned int	SID;
	typedef unsigned int	PID;

	float range;
	float pray_range_plus;
	int elf_level;
	ComboArg comboarg;
private:
	PlayerInfo * object;

public:
	PlayerWrapper() : range(0) ,pray_range_plus(0), elf_level(0) { }
	PlayerWrapper(PlayerInfo * o) : range(0),pray_range_plus(0), elf_level(0), object(o) { }

	int GetLevel() { return 50; }
	void SetRange(float r) { range = r; }
	void SetPrayrangeplus(float p) { pray_range_plus = p; }

	int GetMP()
	{
		return 10000;
	}

	int GetMp()
	{
		return 0;
	}

	int GetSp( )
	{
		return 10000;
	}

	bool IsRiding()
	{
		return false;
	}

	bool IsUsingWeapon(int weapon)
	{
		return true;
	}

	bool HasSkillBook( SID id )
	{
		return true;
	}

	bool SetDecsp(int sp)
	{
		return true;
	}

	bool SetDistance(float d)
	{
		return true;
	}

	float GetWeapondistance()
	{
		return -1;
	}

	bool SetDecmp(int m)
	{
		return true;
	}

	bool SetPray(bool)
	{
		return true;
	}

	bool SetInform(bool)
	{
		return true;
	}

	
	bool SetPerform(bool)
	{
		return true;
	}

	bool SetDamage(int)
	{
		return true;
	}
	int GetPerform()
	{
		return 1;
	}

	int GetDamage()
	{
		return 1;
	}
	float GetRange()
	{
		return range;
	}
	float GetPrayrangeplus() 
	{ 
		return pray_range_plus; 
	}
	bool SetCheckbook(int i) { return true;}
    int GetCheckbook() { return 0;}
    bool SetCheckmoney(int m) { return true; }
    int GetCheckmoney() { return 0;}
    bool SetUsebook(int i) {return true; }
    int GetUsebook() { return 0;}
    bool SetUsemoney(int m) { return true; }
    int GetUsemoney() { return 0;}

	void SetElflevel(int iLevel) { elf_level = iLevel; };
	int GetElflevel() { return elf_level; }

	ComboArg * GetComboarg() { return &comboarg; }
	int GetForm(){ return 0; }	
};

class TargetWrapper
{
public:
	int id;
	TargetWrapper(int i) : id(i) {}

	bool IsValid() { return -1 != id; }
};


}

#endif

