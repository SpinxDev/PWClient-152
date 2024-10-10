#ifndef __CPPGEN_GNET_SKILL1009
#define __CPPGEN_GNET_SKILL1009
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1009:public Skill
    {
      public:
        enum
        { SKILL_ID = 1009 };
          Skill1009 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1009Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (45 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (200 + 14 * (skill->GetLevel () - 1));
                skill->SetGolddamage ((0.6 * (6 + (skill->GetLevel () - 1) * 15)) * (1 + skill->GetPlayer ()->GetElfstr () * 0.01) * 6.2);
                skill->SetFiredamage ((0.4 * (6 + (skill->GetLevel () - 1) * 15)) * (1 + skill->GetPlayer ()->GetElfstr () * 0.01) * 6.2);
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
      Skill1009Stub ():SkillStub (1009)
        {
            cls = 258;
            name = L"À×»ðÊõ";
            nativename = "À×»ðÊõ";
            icon = "À×»ðÊõ.dds";
            max_level = 10;
            type = 1;
            apcost = 200014;
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
            effect = "À×»ðÊõ.sgc";
            range.type = 3;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1009Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (45 + (skill->GetLevel () - 1) * 3);
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
            static int array[10] = { 1101010, 1101015, 1101020, 1101025, 1101030, 1101035, 1101040, 1101045, 1101050, 1101055 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 240, 560, 1040, 1680, 2560, 3720, 5260, 7300, 10000, 13620 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (8);
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
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               45 + (skill->GetLevel () - 1) * 3,
                               200 + 14 * (skill->GetLevel () - 1),
                               0.6 * (6 + 15 * (skill->GetLevel () - 1)) * 6.2,
                               0.4 * (6 + 15 * (skill->GetLevel () - 1)) * 6.2, 15 + skill->GetLevel () * 3);

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
            skill->GetVictim ()->SetTime (6000 + skill->GetT1 () * 50);
            skill->GetVictim ()->SetRatio (0.15 + skill->GetLevel () * 0.03);
            skill->GetVictim ()->SetSlow (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (22);
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
