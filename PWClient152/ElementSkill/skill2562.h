#ifndef __CPPGEN_GNET_SKILL2562
#define __CPPGEN_GNET_SKILL2562
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2562:public Skill
    {
      public:
        enum
        { SKILL_ID = 2562 };
          Skill2562 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2562Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 67;
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
                skill->GetPlayer ()->SetDecmp (0.2 * 71.1);
                skill->GetPlayer ()->SetPray (1);
            }
            bool Interrupt (Skill * skill) const
            {
                return false;
            }
            bool Cancel (Skill * skill) const
            {
                return 1;
            }
            bool Skip (Skill * skill) const
            {
                return 0;
            }
        };
#endif
#ifdef _SKILL_SERVER
        class State2:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 501;
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
                skill->GetPlayer ()->SetDecmp (0.8 * 71.1);
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
#ifdef _SKILL_SERVER
        class State3:public SkillStub::State
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
      Skill2562Stub ():SkillStub (2562)
        {
            cls = 10;
            name = L"水月镜花";
            nativename = "水月镜花";
            icon = "水月镜花.dds";
            max_level = 1;
            type = 12;
            apcost = 0;
            arrowcost = 0;
            apgain = 20;
            attr = 1;
            rank = 8;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1024;
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
            effect = "夜影_水月镜花_击中.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            combosk_nobreak = 1;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2562Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (71.1);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 535;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 60000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 79 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 235410 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 46080 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 331980 };
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
            return (float) (20);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 71.1 + 0 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 100 * skill->GetLevel ();
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetValue (1);
            skill->GetVictim ()->SetHeal (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (25);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
        void ComboSkEndAction (Skill * skill) const
        {
            skill->GetPlayer ()->GetComboarg ()->SetValue (0, skill->GetTarget ()->GetCls ());
        }
#endif
    };
}
#endif
