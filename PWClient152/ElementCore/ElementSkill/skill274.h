#ifndef __CPPGEN_GNET_SKILL274
#define __CPPGEN_GNET_SKILL274
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill274:public Skill
    {
      public:
        enum
        { SKILL_ID = 274 };
          Skill274 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill274Stub:public SkillStub
    {
      public:
        Skill274Stub ():SkillStub (274)
        {
            cls = 6;
            name = L"飞行精通";
            nativename = "飞行精通";
            icon = "飞行精通.dds";
            max_level = 10;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 1;
            eventflag = 2;
            posdouble = 0;
            time_type = 0;
            showorder = 1322;
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
        virtual ~ Skill274Stub ()
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
            static int array[10] = { 9, 14, 19, 24, 29, 34, 39, 44, 49, 54 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 500, 1200, 2300, 3850, 5900, 8650, 12300, 17100, 23500, 32000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 110, 250, 440, 680, 1090, 1540, 1990, 2480, 2980, 3480 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 0.2 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetIncfeather (0.2 * skill->GetLevel ());
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
