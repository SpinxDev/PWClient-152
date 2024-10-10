#ifndef __CPPGEN_GNET_SKILL1000
#define __CPPGEN_GNET_SKILL1000
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1000:public Skill
    {
      public:
        enum
        { SKILL_ID = 1000 };
          Skill1000 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1000Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (80 - skill->GetPlayer ()->GetElflevel () * 0.4);
                skill->GetPlayer ()->SetDecelfap (200);
                skill->SetWooddamage ((0.9 * (6 + (skill->GetLevel () - 1) * 12)) * (1 + skill->GetPlayer ()->GetElfstr () * 0.01) * 9.3);
                skill->SetGolddamage ((0.1 * (6 + (skill->GetLevel () - 1) * 12)) * (1 + skill->GetPlayer ()->GetElfstr () * 0.01) * 9.3);
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
      Skill1000Stub ():SkillStub (1000)
        {
            cls = 258;
            name = L"¶¾´Ì";
            nativename = "¶¾´Ì";
            icon = "¶¾´Ì.dds";
            max_level = 1;
            type = 3;
            apcost = 200000;
            arrowcost = 0;
            apgain = 0;
            attr = 3;
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
            effect = "¶¾´Ì.sgc";
            range.type = 0;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1000Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (80 - skill->GetPlayer ()->GetElflevel () * 0.4);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 15000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 0 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 0 };
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
            return _snwprintf (buffer, length, format, 80, 200, 20 + 2 * skill->GetLevel (), 20 + 2 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (5000 + skill->GetT1 () * 50);
            skill->GetVictim ()->SetRatio (0.2 + 0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetSlow (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (5000 + skill->GetT1 () * 50);
            skill->GetVictim ()->SetRatio (0.2 + 0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetSpeedup2 (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (30);
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
