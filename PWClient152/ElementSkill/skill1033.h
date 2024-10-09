#ifndef __CPPGEN_GNET_SKILL1033
#define __CPPGEN_GNET_SKILL1033
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1033:public Skill
    {
      public:
        enum
        { SKILL_ID = 1033 };
          Skill1033 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1033Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (108);
                skill->GetPlayer ()->SetDecelfap (480);
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
      Skill1033Stub ():SkillStub (1033)
        {
            cls = 258;
            name = L"法师之星";
            nativename = "法师之星";
            icon = "法师之星.dds";
            max_level = 10;
            type = 2;
            apcost = 480000;
            arrowcost = 0;
            apgain = 0;
            attr = 5;
            rank = 0;
            eventflag = 0;
            clslimit = 0x02;
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
            effect = "法师之星.sgc";
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
        virtual ~ Skill1033Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (108);
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
            static int array[10] = { 33341, 33346, 33351, 33356, 33361, 33366, 33371, 33376, 33381, 33386 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 5620, 7780, 10640, 14480, 19660, 26600, 36200, 59000, 96800, 154600 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 5 + skill->GetLevel () * 0.5);

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
            skill->GetVictim ()->SetTime (5000 + skill->GetLevel () * 500 + skill->GetT1 () * 62.5);
            skill->GetVictim ()->SetValue (50);
            skill->GetVictim ()->SetFastmpgen (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (5000 + skill->GetLevel () * 500 + skill->GetT1 () * 62.5);
            skill->GetVictim ()->SetValue (50);
            skill->GetVictim ()->SetFasthpgen (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (5000 + skill->GetLevel () * 500 + skill->GetT1 () * 62.5);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetAmount (1000 + skill->GetT1 () * 10);
            skill->GetVictim ()->SetValue (0);
            skill->GetVictim ()->SetAbsorbmagicdamage (1);
            skill->GetVictim ()->SetTime (5000 + skill->GetLevel () * 500 + skill->GetT1 () * 62.5);
            skill->GetVictim ()->SetRatio (0);
            skill->GetVictim ()->SetAmount (48);
            skill->GetVictim ()->SetValue (1);
            skill->GetVictim ()->SetInsertvstate (1);
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
