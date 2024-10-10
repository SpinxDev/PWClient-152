#ifndef __CPPGEN_GNET_SKILL989
#define __CPPGEN_GNET_SKILL989
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill989:public Skill
    {
      public:
        enum
        { SKILL_ID = 989 };
          Skill989 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill989Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (85);
                skill->GetPlayer ()->SetDecelfap (500 + 18 * (skill->GetLevel () - 1));
                skill->SetWaterdamage (1);
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
      Skill989Stub ():SkillStub (989)
        {
            cls = 258;
            name = L"Õ½¶·²¨ÎÆ.äÎÃð";
            nativename = "Õ½¶·²¨ÎÆ.äÎÃð";
            icon = "Õ½¶·²¨ÎÆäÎÃð.dds";
            max_level = 10;
            type = 3;
            apcost = 500018;
            arrowcost = 0;
            apgain = 0;
            attr = 4;
            rank = 0;
            eventflag = 0;
            time_type = 1;
            showorder = 0;
            allow_land = 0;
            allow_air = 0;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "Õ½¶·²¨ÎÆäÎÃð.sgc";
            range.type = 2;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill989Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (85);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 20000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 80035, 80040, 80045, 80050, 80055, 80060, 80065, 80070, 80075, 80080 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 3720, 5260, 7300, 10000, 13620, 18480, 25000, 34000, 53200, 88000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (15);
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
            return (float) (15);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 500 + 18 * (skill->GetLevel () - 1), 20 + skill->GetLevel () * 2);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 10;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (9000);
            skill->GetVictim ()->SetRatio (0.2 + skill->GetLevel () * 0.02 + skill->GetT1 () * 0.004);
            skill->GetVictim ()->SetReducewater (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (9000);
            skill->GetVictim ()->SetRatio (0.2 + skill->GetLevel () * 0.02 + skill->GetT1 () * 0.004);
            skill->GetVictim ()->SetReducewater (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (18);
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
