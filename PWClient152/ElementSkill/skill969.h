#ifndef __CPPGEN_GNET_SKILL969
#define __CPPGEN_GNET_SKILL969
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill969:public Skill
    {
      public:
        enum
        { SKILL_ID = 969 };
          Skill969 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill969Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (70);
                skill->GetPlayer ()->SetDecelfap (200 + 15 * (skill->GetLevel () - 1));
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
      Skill969Stub ():SkillStub (969)
        {
            cls = 258;
            name = L"无常蛊";
            nativename = "无常蛊";
            icon = "无常蛊.dds";
            max_level = 10;
            type = 3;
            apcost = 200015;
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
            effect = "无常蛊.sgc";
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
        virtual ~ Skill969Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (70);
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
            static int array[10] = { 700030, 700035, 700040, 700045, 700050, 700055, 700060, 700065, 700070, 700075 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2560, 3720, 5260, 7300, 10000, 13620, 18480, 25000, 34000, 53200 };
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
                               200 + 15 * (skill->GetLevel () - 1),
                               25 + 10 * skill->GetLevel (),
                               3 * skill->GetLevel (), 2 * skill->GetLevel (), 10 * skill->GetLevel (), 3 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 10;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT2 () > 66 ? 100 : 0);
            skill->GetVictim ()->SetTime (9000);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetAmount (2 * skill->GetLevel () + skill->GetT0 ());
            skill->GetVictim ()->SetValue (1);
            skill->GetVictim ()->SetApleakcont (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT2 () > 33 ? (skill->GetT2 () < 67 ? 100 : 0) : 0);
            skill->GetVictim ()->SetTime (9000);
            skill->GetVictim ()->SetAmount (3 * skill->GetLevel () * skill->GetT0 ());
            skill->GetVictim ()->SetMagicleak (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT2 () < 34 ? 100 : 0);
            skill->GetVictim ()->SetTime (9000);
            skill->GetVictim ()->SetAmount (25 + 10 * skill->GetLevel () * skill->GetT1 ());
            skill->GetVictim ()->SetToxic (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (9000 + skill->GetT1 () * 50);
            skill->GetVictim ()->SetRatio (0.3);
            skill->GetVictim ()->SetSlow (1);
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
        float GetTalent2 (PlayerWrapper * player) const
        {
            return (float) (player->GetRand ());
        }
#endif
    };
}
#endif
