#ifndef __CPPGEN_GNET_SKILL995
#define __CPPGEN_GNET_SKILL995
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill995:public Skill
    {
      public:
        enum
        { SKILL_ID = 995 };
          Skill995 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill995Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (30 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (90 + 32 * (skill->GetLevel () - 1));
                skill->SetFiredamage ((25 + (skill->GetLevel () - 1) * 30 + skill->GetT0 () + skill->GetT1 ()) * 3.1);
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
      Skill995Stub ():SkillStub (995)
        {
            cls = 258;
            name = L"·ÙÐÄÖä";
            nativename = "·ÙÐÄÖä";
            icon = "·ÙÐÄÖä.dds";
            max_level = 10;
            type = 1;
            apcost = 90032;
            arrowcost = 0;
            apgain = 0;
            attr = 5;
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
            effect = "·ÙÐÄÖä.sgc";
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
        virtual ~ Skill995Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (30 + (skill->GetLevel () - 1) * 3);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 10000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 2005, 2010, 2015, 2020, 2025, 2030, 2035, 2040, 2045, 2050 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 60, 240, 560, 1040, 1680, 2560, 3720, 5260, 7300, 10000 };
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
            return (float) (18);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               30 + (skill->GetLevel () - 1) * 3,
                               90 + 32 * (skill->GetLevel () - 1), (25 + (skill->GetLevel () - 1) * 30) * 3.1, skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (9000 + (skill->GetT0 () + skill->GetT1 ()) * 50);
            skill->GetVictim ()->SetRatio (skill->GetLevel () * 0.01 + skill->GetT0 () * 0.0005 + skill->GetT1 () * 0.0005);
            skill->GetVictim ()->SetSlowattackpray (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (20);
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
