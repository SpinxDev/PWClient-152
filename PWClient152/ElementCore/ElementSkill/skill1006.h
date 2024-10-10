#ifndef __CPPGEN_GNET_SKILL1006
#define __CPPGEN_GNET_SKILL1006
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1006:public Skill
    {
      public:
        enum
        { SKILL_ID = 1006 };
          Skill1006 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1006Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (70 + (skill->GetLevel () - 1) * 5);
                skill->GetPlayer ()->SetDecelfap (700);
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
      Skill1006Stub ():SkillStub (1006)
        {
            cls = 258;
            name = L"守护";
            nativename = "守护";
            icon = "守护.dds";
            max_level = 10;
            type = 2;
            apcost = 700000;
            arrowcost = 0;
            apgain = 0;
            attr = 4;
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
            effect = "守护.sgc";
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
        virtual ~ Skill1006Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (70 + (skill->GetLevel () - 1) * 5);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 60000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 340031, 340036, 340041, 340046, 340051, 340056, 340061, 340066, 340071, 340076 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2780, 4000, 5620, 7780, 10640, 14480, 19660, 26600, 36200, 59000 };
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
            return (float) (30);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (), 70 + (skill->GetLevel () - 1) * 5, 10 + skill->GetLevel () * 2, skill->GetLevel () * 2);

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
            skill->GetVictim ()->SetTime (5000 + skill->GetT0 () * 17);
            skill->GetVictim ()->SetRatio (0.1 + skill->GetLevel () * 0.02 + skill->GetT0 () * 0.005);
            skill->GetVictim ()->SetShowicon (1);
            skill->GetVictim ()->SetInchp2 (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (6000);
            skill->GetVictim ()->SetValue (skill->GetPlayer ()->GetMaxhp () * (skill->GetLevel () * 0.04 + skill->GetT1 () * 0.0025));
            skill->GetVictim ()->SetHpgen (1);
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
