#ifndef __CPPGEN_GNET_SKILL145
#define __CPPGEN_GNET_SKILL145
namespace GNET
{
class Skill145:public Skill
{
public:
enum	{ SKILL_ID = 145 };
Skill145():Skill(SKILL_ID){}
};
class Skill145Stub:public SkillStub
{
public:
class State1:public SkillStub::State
{
public:
int GetTime(Skill * skill) const{return 3000;}
#ifdef _SKILL_SERVER
bool Quit(Skill * skill) const{return false;}
#endif
#ifdef _SKILL_SERVER
bool Loop(Skill * skill) const{return false;}
#endif
#ifdef _SKILL_SERVER
bool Bypass(Skill * skill) const{return false;}
#endif
#ifdef _SKILL_SERVER
void Calculate(Skill * skill) const{}
#endif
#ifdef _SKILL_SERVER
bool Interrupt(Skill * skill) const{return false;}
#endif
bool Cancel(Skill * skill) const{return 0;}
bool Skip(Skill * skill) const{return 0;}
};
Skill145Stub():SkillStub(Skill145::SKILL_ID){cls = 255;
name = L"Ä§·¨·âÓ¡";
icon = "";
max_level = 10;
type = 1;
attr = 1;
practice_level = 1;
eventflag = 2;
pre_id = 0;
skillbook = -1;
allow_land = 1;
allow_air = 0;
allow_water = 0;
allow_ride = 0;
restrict_corpse = 0;
allow_changestatus = 0;
effect = "";
range.type = 0;
has_stateattack = false;
statestub.push_back(new State1());
}
virtual ~Skill145Stub(){}
bool LearnCondition1(Skill* skill) const{return true;}
bool LearnCondition2(Skill* skill) const{return true;}
bool LearnCondition3(Skill* skill) const{return true;}
bool LearnCondition4(Skill* skill) const{return true;}
bool LearnCondition5(Skill* skill) const{return true;}
bool LearnCondition6(Skill* skill) const{return true;}
bool LearnCondition7(Skill* skill) const{return true;}
bool LearnCondition8(Skill* skill) const{return true;}
bool LearnCondition9(Skill* skill) const{return true;}
bool LearnCondition10(Skill* skill) const{return true;}
#ifdef _SKILL_SERVER
bool Learn1(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn2(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn3(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn4(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn5(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn6(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn7(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn8(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn9(Skill* skill) const{return true;}
#endif
#ifdef _SKILL_SERVER
bool Learn10(Skill* skill) const{return true;}
#endif
float GetMpcost(Skill* skill) const{return (float)( 0);}
int GetExecutetime(Skill* skill) const{return 0;}
int GetCoolingtime(Skill* skill) const{return 0;}
#ifdef _SKILL_SERVER
bool StateAttack(Skill* skill) const{;return true;}
#endif
int GetEnmity(Skill* skill) const{return 0;}
#ifdef _SKILL_SERVER
bool TakeEffect(Skill* skill) const{;return true;}
#endif
int GetRequiredLevel(Skill* skill) const{int a[10]={0};return a[skill->GetLevel()];}
int GetRequiredSp(Skill* skill) const{int a[10]={0};return a[skill->GetLevel()];}
float GetRadius(Skill* skill) const{return (float)(0);}
float GetAttackdistance(Skill* skill) const{return (float)(0);}
float GetAngle(Skill* skill) const{return (float)(1-0.0111111*(0));}
float GetPraydistance(Skill* skill) const{return (float)(0);}
#ifdef _SKILL_SERVER
float GetEffectdistance(Skill* skill) const{return (float)(0);}
#endif
int GetInvadertime(Skill* skill) const{return 0;}
int GetShoworder(Skill* skill) const{return 0;}
};
}
#endif
