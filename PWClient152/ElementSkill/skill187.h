#ifndef __CPPGEN_GNET_SKILL187
#define __CPPGEN_GNET_SKILL187
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill187:public Skill
    {
      public:
        enum
        { SKILL_ID = 187 };
          Skill187 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill187Stub:public SkillStub
    {
      public:
        Skill187Stub ():SkillStub (187)
        {
            cls = 0;
            name = L"2233";
            nativename = "2233";
            icon = "ÎÞÐ§.ddsdd";
            max_level = 12;
            type = 10;
            apcost = 2233;
            arrowcost = 31;
            apgain = 3233;
            attr = 6;
            rank = 1;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 3;
            showorder = 4233;
            allow_land = 0;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 1;
            restrict_corpse = 2;
            allow_forms = 15;
            effect = "3233";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 2));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill187Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (1033);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1000333;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 30002;
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 42333 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 42333 };
            return array[skill->GetLevel () - 1];
        }
        int GetMaxAbility (Skill * skill) const
        {
            static int array[10] = { 42333 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (0);
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (0);
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (0));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (1533);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format);
        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 33344;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (8.5211);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 64;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1233);
        }
#endif
    };
}
#endif
