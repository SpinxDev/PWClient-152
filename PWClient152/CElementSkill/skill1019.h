#ifndef __CPPGEN_GNET_SKILL1019
#define __CPPGEN_GNET_SKILL1019
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1019:public Skill
    {
      public:
        enum
        { SKILL_ID = 1019 };
          Skill1019 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1019Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (50 + (skill->GetLevel () - 1) * 10);
                skill->GetPlayer ()->SetDecelfap (180);
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
      Skill1019Stub ():SkillStub (1019)
        {
            cls = 258;
            name = L"战歌";
            nativename = "战歌";
            icon = "战歌.dds";
            max_level = 10;
            type = 2;
            apcost = 180000;
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
            effect = "战歌.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1019Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (50 + (skill->GetLevel () - 1) * 10);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 30000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 201223, 201228, 201233, 201238, 201243, 201248, 201253, 201258, 201263, 201268 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1400, 2180, 3220, 4600, 6400, 8820, 12040, 16360, 22200, 30200 };
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
            return (float) (0);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 50 + (skill->GetLevel () - 1) * 10, 3 + 0.5 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (3000 + 500 * skill->GetLevel ());
            skill->GetVictim ()->SetRatio (0.05 + skill->GetT0 () * 0.0025);
            skill->GetVictim ()->SetFastattack (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (3000 + 500 * skill->GetLevel ());
            skill->GetVictim ()->SetRatio (0.05 + skill->GetT1 () * 0.00067);
            skill->GetVictim ()->SetFastpray (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (3000 + 500 * skill->GetLevel ());
            skill->GetVictim ()->SetRatio (0.05 + skill->GetT0 () * 0.0025);
            skill->GetVictim ()->SetSpeedup (1);
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
