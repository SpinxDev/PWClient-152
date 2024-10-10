#ifndef __CPPGEN_GNET_SKILL998
#define __CPPGEN_GNET_SKILL998
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill998:public Skill
    {
      public:
        enum
        { SKILL_ID = 998 };
          Skill998 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill998Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (70);
                skill->GetPlayer ()->SetDecelfap (146 + 77 * (skill->GetLevel () - 1));
                skill->SetFiredamage ((24 + (skill->GetLevel () - 1) * 23 + skill->GetT1 ()) * 3.7);
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
      Skill998Stub ():SkillStub (998)
        {
            cls = 258;
            name = L"Àë»ðÉñ¾÷";
            nativename = "Àë»ðÉñ¾÷";
            icon = "Àë»ðÉñ¾÷.dds";
            max_level = 10;
            type = 1;
            apcost = 146077;
            arrowcost = 0;
            apgain = 0;
            attr = 5;
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
            effect = "Àë»ðÉñ¾÷.sgc";
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
        virtual ~ Skill998Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (70);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 10000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 6028, 6033, 6038, 6043, 6048, 6053, 6058, 6063, 6068, 6073 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2180, 3220, 4600, 6400, 8820, 12040, 16360, 22200, 30200, 43000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (10 + 0.2 * skill->GetLevel ());
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
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               10 + 0.2 * skill->GetLevel (), 146 + 77 * (skill->GetLevel () - 1), (24 + 23 * (skill->GetLevel () - 1)) * 3.7);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetTime (25000);
            skill->GetVictim ()->SetRatio (0.1 + skill->GetT1 () * 0.001);
            skill->GetVictim ()->SetValue (0);
            skill->GetVictim ()->SetBurningfeet (1);
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
