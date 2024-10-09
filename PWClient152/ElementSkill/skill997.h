#ifndef __CPPGEN_GNET_SKILL997
#define __CPPGEN_GNET_SKILL997
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill997:public Skill
    {
      public:
        enum
        { SKILL_ID = 997 };
          Skill997 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill997Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfap (32 + 43 * (skill->GetLevel () - 1));
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
      Skill997Stub ():SkillStub (997)
        {
            cls = 258;
            name = L"Ìì»ðÖä";
            nativename = "Ìì»ðÖä";
            icon = "Ìì»ðÖä.dds";
            max_level = 10;
            type = 3;
            apcost = 32043;
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
            effect = "Ìì»ðÖä.sgc";
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
        virtual ~ Skill997Stub ()
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
            return 8000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 3010, 3015, 3020, 3025, 3030, 3035, 3040, 3045, 3050, 3055 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 240, 560, 1040, 1680, 2560, 3720, 5260, 7300, 10000, 13620 };
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
                               35 + (skill->GetLevel () - 1) * 3, 32 + 43 * (skill->GetLevel () - 1), (8 + 35 * (skill->GetLevel () - 1)) * 8);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 4;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 50 + 5 * skill->GetLevel ());
            skill->GetVictim ()->SetTime (3000);
            skill->GetVictim ()->SetAmount ((8 + (skill->GetLevel () - 1) * 35 + skill->GetT1 ()) * 8 *
                                            (skill->GetPlayer ()->GetWaterresist () + 3000) / (skill->GetPlayer ()->GetFireresist () + 3000));
            skill->GetVictim ()->SetBurning (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (24);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 16;
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
