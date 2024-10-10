#ifndef __CPPGEN_GNET_SKILL980
#define __CPPGEN_GNET_SKILL980
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill980:public Skill
    {
      public:
        enum
        { SKILL_ID = 980 };
          Skill980 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill980Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (85);
                skill->GetPlayer ()->SetDecelfap (500 + 99 * (skill->GetLevel () - 1));
                skill->SetEarthdamage ((120 + (skill->GetLevel () - 1) * 56 + skill->GetT1 () * 3) * 5.2);
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
      Skill980Stub ():SkillStub (980)
        {
            cls = 258;
            name = L"·çÐ¥¾÷";
            nativename = "·çÐ¥¾÷";
            icon = "·çÐ¥¾÷.dds";
            max_level = 10;
            type = 1;
            apcost = 500099;
            arrowcost = 0;
            apgain = 0;
            attr = 6;
            rank = 0;
            eventflag = 0;
            clslimit = 0;
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
            effect = "·çÐ¥¾÷.sgc";
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
        virtual ~ Skill980Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (85);
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
            static int array[10] = { 837, 842, 847, 852, 857, 862, 867, 872, 877, 882 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 4300, 6000, 8280, 11320, 15380, 20800, 28400, 38400, 65400, 106600 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (10);
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
            return (float) (8);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               500 + 99 * (skill->GetLevel () - 1), (120 + (skill->GetLevel () - 1) * 56) * 5.2, 10 + skill->GetLevel () * 2);

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
            skill->GetVictim ()->SetTime (5000);
            skill->GetVictim ()->SetRatio (0.1 + skill->GetLevel () * 0.02 + skill->GetT1 () * 0.0025);
            skill->GetVictim ()->SetSlow (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (10);
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
