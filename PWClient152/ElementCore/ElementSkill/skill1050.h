#ifndef __CPPGEN_GNET_SKILL1050
#define __CPPGEN_GNET_SKILL1050
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1050:public Skill
    {
      public:
        enum
        { SKILL_ID = 1050 };
          Skill1050 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1050Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (85 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (780 + 99 * (skill->GetLevel () - 1));
                skill->SetGolddamage ((0.5 * (125 + (skill->GetLevel () - 1) * 22 + skill->GetT1 () * 3)) * 6.7);
                skill->SetDamage ((0.5 * (125 + (skill->GetLevel () - 1) * 22 + skill->GetT0 () * 3)) * 6.7);
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
      Skill1050Stub ():SkillStub (1050)
        {
            cls = 258;
            name = L"¿ÕÁÑ×¦";
            nativename = "¿ÕÁÑ×¦";
            icon = "¿ÕÁÑ×¦.dds";
            max_level = 10;
            type = 1;
            apcost = 780099;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
            rank = 0;
            eventflag = 0;
            posdouble = 0x02;
            clslimit = 0x08;
            time_type = 1;
            showorder = 0;
            allow_land = 1;
            allow_air = 0;
            allow_water = 0;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "¿ÕÁÑ×¦.sgc";
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
        virtual ~ Skill1050Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (85 + (skill->GetLevel () - 1) * 3);
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
            static int array[10] = { 400435, 400440, 400445, 400450, 400455, 400460, 400465, 400470, 400475, 400480 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 3720, 5260, 7300, 10000, 13620, 18480, 25000, 34000, 53200, 88000 };
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
            return (float) (25);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               85 + (skill->GetLevel () - 1) * 3,
                               780 + 99 * (skill->GetLevel () - 1),
                               0.5 * (125 + (skill->GetLevel () - 1) * 22) * 6.7, skill->GetLevel (), skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (9000);
            skill->GetVictim ()->SetRatio (0.2);
            skill->GetVictim ()->SetDecdefence2 (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (9000);
            skill->GetVictim ()->SetAmount (skill->GetLevel () * skill->GetT0 () * 1);
            skill->GetVictim ()->SetBleeding (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (26);
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
