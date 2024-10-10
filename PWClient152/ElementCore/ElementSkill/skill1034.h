#ifndef __CPPGEN_GNET_SKILL1034
#define __CPPGEN_GNET_SKILL1034
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1034:public Skill
    {
      public:
        enum
        { SKILL_ID = 1034 };
          Skill1034 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1034Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (120);
                skill->GetPlayer ()->SetDecelfap (999);
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
      Skill1034Stub ():SkillStub (1034)
        {
            cls = 258;
            name = L"潜能激发";
            nativename = "潜能激发";
            icon = "潜能激发.dds";
            max_level = 10;
            type = 2;
            apcost = 999000;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
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
            effect = "潜能激发.sgc";
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
        virtual ~ Skill1034Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (120);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 1000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 5545, 5550, 5555, 5560, 5565, 5570, 5575, 5580, 5585, 5590 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 7300, 10000, 13620, 18480, 25000, 34000, 53200, 88000, 141000, 221200 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 2 + skill->GetLevel () * 0.2, 3 + skill->GetLevel () * 0.4);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 10 + skill->GetT0 () * 0.5);
            skill->GetVictim ()->SetTime (2000 + skill->GetLevel () * 200);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetImmunesealed (1);
            skill->GetVictim ()->SetProbability (1.0 * 10 + skill->GetT0 () * 0.5);
            skill->GetVictim ()->SetTime (3000 + skill->GetLevel () * 200);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetImmunesleep (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (3000 + skill->GetLevel () * 400);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetImmuneweak (1);
            skill->GetVictim ()->SetProbability (1.0 * 10 + skill->GetT1 () * 0.5);
            skill->GetVictim ()->SetTime (3000 + skill->GetLevel () * 200);
            skill->GetVictim ()->SetImmuneslowdizzy (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (27);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 0;
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
