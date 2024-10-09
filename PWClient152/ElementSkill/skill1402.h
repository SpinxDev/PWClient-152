#ifndef __CPPGEN_GNET_SKILL1402
#define __CPPGEN_GNET_SKILL1402
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1402:public Skill
    {
      public:
        enum
        { SKILL_ID = 1402 };
          Skill1402 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1402Stub:public SkillStub
    {
      public:
        Skill1402Stub ():SkillStub (1402)
        {
            cls = 255;
            name = L"打磨精通";
            nativename = "打磨精通";
            icon = "打磨精通.dds";
            max_level = 10;
            type = 9;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 3010;
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
        virtual ~ Skill1402Stub ()
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
            static int array[10] = { 60, 70, 80, 90, 95, 99, 100, 105, 105, 105 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 30000, 50000, 100000, 150000, 200000, 500000, 1000000, 1000000, 1000000, 1000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 28941, 28942, 28943, 28944, 28945, 28946, 28947, 28948, 28949, 28950 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 0, 50000, 100000, 200000, 500000, 1000000, 2000000, 3500000, 6000000, 10000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetMaxAbility (Skill * skill) const
        {
            static int array[10] = { 10, 20, 30, 45, 60, 80, 100, 120, 150, 200 };
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
