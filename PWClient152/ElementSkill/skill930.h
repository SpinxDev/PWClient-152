#ifndef __CPPGEN_GNET_SKILL930
#define __CPPGEN_GNET_SKILL930
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill930:public Skill
    {
      public:
        enum
        { SKILL_ID = 930 };
          Skill930 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill930Stub:public SkillStub
    {
      public:
        Skill930Stub ():SkillStub (930)
        {
            cls = 4;
            name = L"作废";
            nativename = "作废";
            icon = "";
            max_level = 1;
            type = 2;
            apcost = 100;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 22;
            eventflag = 0;
            time_type = 0;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (9);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "巨灵神力.sgc";
            range.type = 5;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill930Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (1);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 180000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 95 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (16);
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
            return _snwprintf (buffer, length, format);
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
            return (float) (5);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 15;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
#endif
    };
}
#endif
