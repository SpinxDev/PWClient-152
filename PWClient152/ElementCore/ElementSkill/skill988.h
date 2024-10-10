#ifndef __CPPGEN_GNET_SKILL988
#define __CPPGEN_GNET_SKILL988
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill988:public Skill
    {
      public:
        enum
        { SKILL_ID = 988 };
          Skill988 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill988Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (60 + (skill->GetLevel () - 1) * 2);
                skill->GetPlayer ()->SetDecelfap (175 + 99 * (skill->GetLevel () - 1));
                skill->SetWaterdamage ((24 + (skill->GetLevel () - 1) * 22 + skill->GetT1 ()) * 5.2);
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
      Skill988Stub ():SkillStub (988)
        {
            cls = 258;
            name = L"岕鍾機";
            nativename = "岕鍾機";
            icon = "岕鍾機.dds";
            max_level = 10;
            type = 1;
            apcost = 175099;
            arrowcost = 0;
            apgain = 0;
            attr = 4;
            rank = 0;
            eventflag = 0;
            posdouble = 0x04;
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
            effect = "岕鍾機.sgc";
            range.type = 2;
            doenchant = false;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill988Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (60 + (skill->GetLevel () - 1) * 2);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 15000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 60025, 60030, 60035, 60040, 60045, 60050, 60055, 60060, 60065, 60070 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1680, 2560, 3720, 5260, 7300, 10000, 13620, 18480, 25000, 34000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (18);
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
            return (float) (18);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               60 + (skill->GetLevel () - 1) * 2, 175 + 99 * (skill->GetLevel () - 1), (24 + (skill->GetLevel () - 1) * 22) * 5.2);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetValue (skill->GetPlayer ()->GetMaxhp () * (0.01 + skill->GetT1 () * 0.01 * 0.1));
            skill->GetVictim ()->SetHeal (1);
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
