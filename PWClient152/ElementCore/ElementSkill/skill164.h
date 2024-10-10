#ifndef __CPPGEN_GNET_SKILL164
#define __CPPGEN_GNET_SKILL164
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill164:public Skill
    {
      public:
        enum
        { SKILL_ID = 164 };
          Skill164 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill164Stub:public SkillStub
    {
      public:
        Skill164Stub ():SkillStub (164)
        {
            cls = 255;
            name = L"飞行器精通";
            nativename = "飞行器精通";
            icon = "飞剑精通.dds";
            max_level = 6;
            type = 7;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 0;
            eventflag = 0;
            time_type = 0;
            showorder = 4001;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 0;
            effect = "";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill164Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (0);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 0;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 150, 150, 150, 150, 150, 150 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 3300, 14000, 48000, 160000, 185000, 200000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 4226, 4227, 4228, 4229, 4230, 4231 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 4200, 19000, 63000, 220000, 255000, 275000 };
            return array[skill->GetLevel () - 1];
        }
        int GetMaxAbility (Skill * skill) const
        {
            static int array[10] = { 10, 20, 30, 40, 50, 60 };
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
            return (float) (0);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), skill->GetAbility (), skill->GetMaxability ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (0);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
