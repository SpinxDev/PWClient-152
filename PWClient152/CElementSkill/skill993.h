#ifndef __CPPGEN_GNET_SKILL993
#define __CPPGEN_GNET_SKILL993
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill993:public Skill
    {
      public:
        enum
        { SKILL_ID = 993 };
          Skill993 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill993Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (35 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (53 + 40 * (skill->GetLevel () - 1));
                skill->SetFiredamage ((7 + (skill->GetLevel () - 1) * 14 + skill->GetT1 ()) * 6.7);
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
      Skill993Stub ():SkillStub (993)
        {
            cls = 258;
            name = L"×ÓÄ¸À×»ðÖä";
            nativename = "×ÓÄ¸À×»ðÖä";
            icon = "×ÓÄ¸À×»ðÖä.dds";
            max_level = 10;
            type = 1;
            apcost = 53040;
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
            effect = "×ÓÄ¸»ðÀ×Öä.sgc";
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
        virtual ~ Skill993Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (35 + (skill->GetLevel () - 1) * 3);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 40000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 2008, 2013, 2018, 2023, 2028, 2033, 2038, 2043, 2048, 2053 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 160, 420, 820, 1400, 2180, 3220, 4600, 6400, 8820, 12040 };
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
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               35 + (skill->GetLevel () - 1) * 3,
                               53 + 40 * (skill->GetLevel () - 1), (7 + 14 * (skill->GetLevel () - 1)) * 6.7, skill->GetLevel () * 3);

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
            skill->GetVictim ()->SetTime (6000);
            skill->GetVictim ()->SetRatio (skill->GetLevel () * 0.03 + skill->GetT1 () * 0.007);
            skill->GetVictim ()->SetReducefire2 (1);
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
        float GetTalent2 (PlayerWrapper * player) const
        {
            return (float) (player->GetLevel ());
        }
#endif
    };
}
#endif
