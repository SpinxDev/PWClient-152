#ifndef __CPPGEN_GNET_SKILL1020
#define __CPPGEN_GNET_SKILL1020
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1020:public Skill
    {
      public:
        enum
        { SKILL_ID = 1020 };
          Skill1020 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1020Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (70 + (skill->GetLevel () - 1) * 2);
                skill->GetPlayer ()->SetDecelfap (120);
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
      Skill1020Stub ():SkillStub (1020)
        {
            cls = 258;
            name = L"´óµØ²¨ÎÆ.·ßÅ­";
            nativename = "´óµØ²¨ÎÆ.·ßÅ­";
            icon = "´óµØ²¨ÎÆ·ßÅ­.dds";
            max_level = 10;
            type = 2;
            apcost = 120000;
            arrowcost = 0;
            apgain = 0;
            attr = 5;
            rank = 0;
            eventflag = 0;
            posdouble = 1;
            clslimit = 0x10;
            time_type = 1;
            showorder = 0;
            allow_land = 1;
            allow_air = 0;
            allow_water = 0;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "´óµØ²¨ÎÆ·ßÅ­.sgc";
            range.type = 2;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1020Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (70 + (skill->GetLevel () - 1) * 2);
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
            static int array[10] = { 3310, 3315, 3320, 3325, 3330, 3335, 3340, 3345, 3350, 3355 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 240, 560, 1040, 1680, 2560, 3720, 5260, 7300, 10000, 13620 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (5 + skill->GetLevel () * 1);
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
            return (float) (15);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 70 + (skill->GetLevel () - 1) * 2, 5 + skill->GetLevel () * 1);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetTime (5000 + skill->GetT0 () * 250);
            skill->GetVictim ()->SetValue (3 + skill->GetT1 () * 0.014);
            skill->GetVictim ()->SetApgen (1);
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
