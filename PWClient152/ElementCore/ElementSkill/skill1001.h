#ifndef __CPPGEN_GNET_SKILL1001
#define __CPPGEN_GNET_SKILL1001
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1001:public Skill
    {
      public:
        enum
        { SKILL_ID = 1001 };
          Skill1001 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1001Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (35 + (skill->GetLevel () - 1) * 1);
                skill->GetPlayer ()->SetDecelfap (300);
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
      Skill1001Stub ():SkillStub (1001)
        {
            cls = 258;
            name = L"×¾»ð¶¨";
            nativename = "×¾»ð¶¨";
            icon = "×¾»ð¶¨.dds";
            max_level = 10;
            type = 2;
            apcost = 300000;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
            rank = 0;
            eventflag = 0;
            clslimit = 0x10;
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
            effect = "×¾»ð¶¨.sgc";
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
        virtual ~ Skill1001Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (35 + (skill->GetLevel () - 1) * 1);
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
            static int array[10] = { 100109, 100114, 100119, 100124, 100129, 100134, 100139, 100144, 100149, 100154 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 200, 480, 920, 1540, 2360, 3460, 4920, 6840, 9400, 12800 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 35 + (skill->GetLevel () - 1) * 1, 5 + skill->GetLevel () * 3);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetTime (10000 + skill->GetT0 () * 40);
            skill->GetVictim ()->SetRatio (0.05 + skill->GetLevel () * 0.03 + skill->GetT0 () * 0.0025);
            skill->GetVictim ()->SetValue (0.5);
            skill->GetVictim ()->SetHardenskin (1);
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
