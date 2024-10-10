#ifndef __CPPGEN_GNET_SKILL1043
#define __CPPGEN_GNET_SKILL1043
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1043:public Skill
    {
      public:
        enum
        { SKILL_ID = 1043 };
          Skill1043 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1043Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (160 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (113 + 30 * (skill->GetLevel () - 1));
                skill->SetWaterdamage (0 + (skill->GetLevel () - 1) * 96 * (1 + skill->GetPlayer ()->GetElfstr () * 0.01));
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
      Skill1043Stub ():SkillStub (1043)
        {
            cls = 258;
            name = L"±ù±©";
            nativename = "±ù±©";
            icon = "±ù±©.dds";
            max_level = 10;
            type = 3;
            apcost = 113030;
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
            effect = "±ù±©.sgc";
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
        virtual ~ Skill1043Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (160 + (skill->GetLevel () - 1) * 3);
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
            static int array[10] = { 707060, 707065, 707070, 707075, 707080, 707085, 707090, 707095, 707099, 707099 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 18480, 25000, 34000, 53200, 88000, 141000, 221200, 317200, 500000, 500000 };
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
                               160 + (skill->GetLevel () - 1) * 3, 113 + 30 * (skill->GetLevel () - 1), 3 + skill->GetLevel () * 0.3);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 10;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (5000);
            skill->GetVictim ()->SetRatio (0.03 + 0.003 * skill->GetLevel () + skill->GetT1 () * 0.000625);
            skill->GetVictim ()->SetDechp (1);
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
