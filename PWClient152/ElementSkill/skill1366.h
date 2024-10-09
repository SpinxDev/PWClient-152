#ifndef __CPPGEN_GNET_SKILL1366
#define __CPPGEN_GNET_SKILL1366
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1366:public Skill
    {
      public:
        enum
        { SKILL_ID = 1366 };
          Skill1366 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1366Stub:public SkillStub
    {
      public:
        Skill1366Stub ():SkillStub (1366)
        {
            cls = 8;
            name = L"ÒûÑª½£ÑÛ£¨Ð§¹û×¨ÓÃ£©";
            nativename = "ÒûÑª½£ÑÛ£¨Ð§¹û×¨ÓÃ£©";
            icon = "";
            max_level = 10;
            type = 1;
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
            allow_air = 0;
            allow_water = 0;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 1;
            restrict_corpse = 0;
            allow_forms = 0;
            effect = "½£Áé_ÒûÑª½£¾÷.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill1366Stub ()
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
            skill->GetVictim ()->SetTime (20000);
            skill->GetVictim ()->SetRatio (1354);
            skill->GetVictim ()->SetShowicon (1);
            skill->GetVictim ()->SetAmount (skill->GetLevel ());
            skill->GetVictim ()->SetValue (1369);
            skill->GetVictim ()->SetBeattackattachstate2 (1);
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
