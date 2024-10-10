#ifndef __CPPGEN_GNET_SKILL2550
#define __CPPGEN_GNET_SKILL2550
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2550:public Skill
    {
      public:
        enum
        { SKILL_ID = 2550 };
          Skill2550 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2550Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (85.5 + 4.5 * skill->GetLevel ());
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
      Skill2550Stub ():SkillStub (2550)
        {
            cls = 10;
            name = L"月食";
            nativename = "月食";
            icon = "月食.dds";
            max_level = 10;
            type = 2;
            apcost = 100;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 6;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1010;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (44878);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "2寸力.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2560, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill2550Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (85.5 + 4.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 150000 - 6000 * skill->GetLevel ();
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 60, 63, 66, 69, 72, 75, 78, 81, 84, 87 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 55440, 63270, 71820, 81600, 99840, 146300, 209380, 295240, 411520, 567490 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 4980, 10980, 22980, 51980, 111980, 171980, 291980, 451980, 691980, 931980 };
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
            return (float) (skill->GetPlayer ()->GetRange ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (), 85.5 + 4.5 * skill->GetLevel (), 150 - 6.0 * skill->GetLevel (), 21 + skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (4000);
            skill->GetVictim ()->SetRatio (0);
            skill->GetVictim ()->SetAmount (0);
            skill->GetVictim ()->SetValue (21 + skill->GetLevel ());
            skill->GetVictim ()->SetInvisible (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000);
            skill->GetVictim ()->SetRatio (3);
            skill->GetVictim ()->SetSpeedup (1);
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
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
