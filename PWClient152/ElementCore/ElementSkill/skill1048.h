#ifndef __CPPGEN_GNET_SKILL1048
#define __CPPGEN_GNET_SKILL1048
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1048:public Skill
    {
      public:
        enum
        { SKILL_ID = 1048 };
          Skill1048 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1048Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 0;
            }
            bool Quit (Skill * skill) const
            {
                return false;
            }
            bool Loop (Skill * skill) const
            {
                return false;
            }
            bool Bypass (Skill * skill) const
            {
                return false;
            }
            void Calculate (Skill * skill) const
            {
                skill->GetPlayer ()->SetDecelfmp (125);
                skill->GetPlayer ()->SetDecelfap (59 + 24 * (skill->GetLevel () - 1));
                skill->SetDamage (0 + (skill->GetLevel () - 1) * 39 * (1 + skill->GetPlayer ()->GetElfstr () * 0.01));
                skill->GetPlayer ()->SetPerform (1);
            }
            bool Interrupt (Skill * skill) const
            {
                return false;
            }
            bool Cancel (Skill * skill) const
            {
                return 0;
            }
            bool Skip (Skill * skill) const
            {
                return 0;
            }
        };
#endif
      Skill1048Stub ():SkillStub (1048)
        {
            cls = 258;
            name = L"����";
            nativename = "����";
            icon = "����.dds";
            max_level = 10;
            type = 11;
            apcost = 59024;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
            rank = 0;
            eventflag = 0;
            time_type = 1;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "����.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1048Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (125);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 1000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 5060052, 5060057, 5060062, 5060067, 5060072, 5060077, 5060082, 5060087, 5060092, 5060097 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 11320, 15380, 20800, 28400, 38400, 65400, 106600, 169400, 263600, 359000 };
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
            return (float) (20);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 59 + 24 * (skill->GetLevel () - 1), 4 + skill->GetLevel () * 0.5);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000 + skill->GetLevel () * 500 + skill->GetT1 () * 25);
            skill->GetVictim ()->SetSealed (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000 + skill->GetLevel () * 500 + skill->GetT1 () * 25);
            skill->GetVictim ()->SetImmunephysical (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (21);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 20;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetElfstr ());
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) (player->GetElfagi ());
        }
#endif
    };
}
#endif
