#ifndef __CPPGEN_GNET_SKILL981
#define __CPPGEN_GNET_SKILL981
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill981:public Skill
    {
      public:
        enum
        { SKILL_ID = 981 };
          Skill981 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill981Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (45 + (skill->GetLevel () - 1) * 1);
                skill->GetPlayer ()->SetDecelfap (150 + skill->GetLevel () * 8);
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
      Skill981Stub ():SkillStub (981)
        {
            cls = 258;
            name = L"太极";
            nativename = "太极";
            icon = "太极.dds";
            max_level = 10;
            type = 2;
            apcost = 158008;
            arrowcost = 0;
            apgain = 0;
            attr = 6;
            rank = 0;
            eventflag = 0;
            time_type = 1;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "太级.sgc";
            range.type = 2;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill981Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (45 + (skill->GetLevel () - 1) * 1);
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
            static int array[10] = { 415, 420, 425, 430, 435, 440, 445, 450, 455, 460 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 560, 1040, 1680, 2560, 3720, 5260, 7300, 10000, 13620, 18480 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (20);
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
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               45 + (skill->GetLevel () - 1) * 1,
                               150 + skill->GetLevel () * 8, 5 + skill->GetLevel (), 5 + skill->GetLevel () * 1, 5 + skill->GetLevel () * 1);

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
            skill->GetVictim ()->SetTime (5000 + skill->GetLevel () * 1000);
            skill->GetVictim ()->SetRatio (0.05 + skill->GetLevel () * 0.01 + skill->GetT0 () * 0.0025);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetSpeedup2 (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (5000 + skill->GetLevel () * 1000);
            skill->GetVictim ()->SetValue (50);
            skill->GetVictim ()->SetFasthpgen (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (40);
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
