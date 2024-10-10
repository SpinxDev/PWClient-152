#ifndef __CPPGEN_GNET_SKILL1084
#define __CPPGEN_GNET_SKILL1084
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1084:public Skill
    {
      public:
        enum
        { SKILL_ID = 1084 };
          Skill1084 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1084Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfap (800);
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
      Skill1084Stub ():SkillStub (1084)
        {
            cls = 258;
            name = L"平衡";
            nativename = "平衡";
            icon = "平衡.dds";
            max_level = 10;
            type = 2;
            apcost = 800000;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
            rank = 0;
            eventflag = 0;
            clslimit = 0x01;
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
            effect = "平衡.sgc";
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
        virtual ~ Skill1084Stub ()
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
            return 60000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 4005041, 4005046, 4005051, 4005056, 4005061, 4005066, 4005071, 4005076, 4005081, 4005086 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), skill->GetLevel () * 0.1);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 *
                                                 (skill->GetPlayer ()->GetEarthresist () + skill->GetPlayer ()->GetFireresist () +
                                                  skill->GetPlayer ()->GetWaterresist () + skill->GetPlayer ()->GetWoodresist () +
                                                  skill->GetPlayer ()->GetGoldresist ()) * 0.2 > skill->GetPlayer ()->GetDefense ()? 100 : 0);
            skill->GetVictim ()->SetTime (6000 + skill->GetT0 () * 62.5);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->
                SetAmount ((skill->GetPlayer ()->GetEarthresist () + skill->GetPlayer ()->GetFireresist () + skill->GetPlayer ()->GetWaterresist () +
                            skill->GetPlayer ()->GetWoodresist () + skill->GetPlayer ()->GetGoldresist () -
                            5 * skill->GetPlayer ()->GetDefense ()) * 0.2 * 0.1 * skill->GetLevel ());
            skill->GetVictim ()->SetValue (1);
            skill->GetVictim ()->SetAbsorbphysicdamage (1);
            skill->GetVictim ()->SetProbability (1.0 *
                                                 (skill->GetPlayer ()->GetEarthresist () + skill->GetPlayer ()->GetFireresist () +
                                                  skill->GetPlayer ()->GetWaterresist () + skill->GetPlayer ()->GetWoodresist () +
                                                  skill->GetPlayer ()->GetGoldresist ()) * 0.2 > skill->GetPlayer ()->GetDefense ()? 0 : 100);
            skill->GetVictim ()->SetTime (6000 + skill->GetT0 () * 62.5);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->
                SetAmount ((5 * skill->GetPlayer ()->GetDefense () - skill->GetPlayer ()->GetEarthresist () - skill->GetPlayer ()->GetFireresist () -
                            skill->GetPlayer ()->GetWaterresist () - skill->GetPlayer ()->GetWoodresist () -
                            skill->GetPlayer ()->GetGoldresist ()) * 0.2 * 0.1 * skill->GetLevel ());
            skill->GetVictim ()->SetValue (1);
            skill->GetVictim ()->SetAbsorbmagicdamage (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (0);
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
