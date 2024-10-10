#ifndef __CPPGEN_GNET_SKILL14
#define __CPPGEN_GNET_SKILL14
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill14:public Skill
    {
      public:
        enum
        { SKILL_ID = 14 };
          Skill14 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill14Stub:public SkillStub
    {
      public:
        Skill14Stub ():SkillStub (14)
        {
            cls = 4;
            name = L"£ª";
            nativename = "£ª";
            icon = "ÊÉÑª.dds";
            max_level = 3;
            type = 3;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 0;
            eventflag = 0;
            time_type = 0;
            showorder = 143;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 1;
            allow_forms = 1;
            {
                restrict_weapons.push_back (9);
            }
            effect = "ÊÉÑª.sgc";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill14Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (10 + 5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 3000;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 200002;
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
            return (float) (433);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 333;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
