#ifndef __CPPGEN_GNET_SKILL2444
#define __CPPGEN_GNET_SKILL2444
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2444:public Skill
    {
      public:
        enum
        { SKILL_ID = 2444 };
          Skill2444 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2444Stub:public SkillStub
    {
      public:
        Skill2444Stub ():SkillStub (2444)
        {
            cls = 255;
            name = L"重霄雷劫";
            nativename = "重霄雷劫";
            icon = "暴击率提高.dds";
            max_level = 10;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 8;
            eventflag = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 2003;
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
        virtual ~ Skill2444Stub ()
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
            static int array[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 42258, 42258, 42258, 42258, 42258, 42258, 42258, 42258, 42258, 42258 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredRealmLevel (Skill * skill) const
        {
            static int array[10] = { 3, 6, 9, 12, 15, 18, 21, 24, 27, 30 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 1 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetInccrit (1 * skill->GetLevel ());
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
