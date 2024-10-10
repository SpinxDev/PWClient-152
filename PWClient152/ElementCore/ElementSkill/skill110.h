#ifndef __CPPGEN_GNET_SKILL110
#define __CPPGEN_GNET_SKILL110
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill110:public Skill
    {
      public:
        enum
        { SKILL_ID = 110 };
          Skill110 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill110Stub:public SkillStub
    {
      public:
        Skill110Stub ():SkillStub (110)
        {
            cls = 4;
            name = L"＊";
            nativename = "＊";
            icon = "飞天神斧.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 3;
            eventflag = 0;
            time_type = 0;
            showorder = 140;
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
            effect = "飞天神斧.sgc";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(109, 1));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill110Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (25 + skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1500;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 8000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 55, 55, 55, 60, 60, 60, 65, 65, 65, 65 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 20000, 20000, 20000, 30000, 30000, 30000, 40000, 40000, 40000, 40000 };
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
            return (float) (1 - 0.0111111 * (20 + 2.5 * skill->GetLevel ()));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 4 + 0.3 * skill->GetLevel ());
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
            return (float) (30);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 0;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
