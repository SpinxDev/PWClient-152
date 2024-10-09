#ifndef __CPPGEN_GNET_SKILL2277
#define __CPPGEN_GNET_SKILL2277
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2277:public Skill
    {
      public:
        enum
        { SKILL_ID = 2277 };
          Skill2277 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2277Stub:public SkillStub
    {
      public:
        Skill2277Stub ():SkillStub (2277)
        {
            cls = 255;
            name = L"888";
            nativename = "888";
            icon = "";
            max_level = 1;
            type = 3;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 0;
            effect = "Ω£¡È_∞À∑ΩΩ£”∞.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill2277Stub ()
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
            return _snwprintf (buffer, length, format);
        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetValue (100);
            skill->GetVictim ()->SetGoldhurt (1);
            return true;
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
