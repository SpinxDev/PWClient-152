#ifndef __CPPGEN_GNET_SKILL976
#define __CPPGEN_GNET_SKILL976
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill976:public Skill
    {
      public:
        enum
        { SKILL_ID = 976 };
          Skill976 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill976Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (55 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (600);
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
      Skill976Stub ():SkillStub (976)
        {
            cls = 258;
            name = L"明王咒";
            nativename = "明王咒";
            icon = "明王咒.dds";
            max_level = 10;
            type = 2;
            apcost = 600000;
            arrowcost = 0;
            apgain = 0;
            attr = 5;
            rank = 0;
            eventflag = 0;
            posdouble = 1;
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
            effect = "明王咒.sgc";
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
        virtual ~ Skill976Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (55 + (skill->GetLevel () - 1) * 3);
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
            static int array[10] = { 500021, 500026, 500031, 500036, 500041, 500046, 500051, 500056, 500061, 500066 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1160, 1840, 2780, 4000, 5620, 7780, 10640, 14480, 19660, 26600 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 55 + (skill->GetLevel () - 1) * 3, 100 - skill->GetLevel () * 5);

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
            skill->GetVictim ()->SetTime (3000 + skill->GetT1 () * 34);
            skill->GetVictim ()->SetImmuneslowdizzy (1);
            skill->GetVictim ()->SetProbability (1.0 * 100 - skill->GetLevel () * 5);
            skill->GetVictim ()->SetTime (8000);
            skill->GetVictim ()->SetRatio (0.5);
            skill->GetVictim ()->SetDecresist (1);
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
