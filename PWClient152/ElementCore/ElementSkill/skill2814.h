#ifndef __CPPGEN_GNET_SKILL2814
#define __CPPGEN_GNET_SKILL2814
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2814:public Skill
    {
      public:
        enum
        { SKILL_ID = 2814 };
          Skill2814 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2814Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1034;
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
                return 50;
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
                skill->GetPlayer ()->SetPerform (2);
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
                return 1268;
            }
            bool Quit (Skill * skill) const
            {
                return false;
            }
            bool Loop (Skill * skill) const
            {
                return skill->GetPlayer ()->GetMp () > 168 + 9 * skill->GetLevel ();
            }
            bool Bypass (Skill * skill) const
            {
                return false;
            }
            void Calculate (Skill * skill) const
            {
                skill->GetPlayer ()->SetDecmp (297);
                skill->SetPlus (8855);
                skill->SetRatio (1.5);
                skill->SetWaterdamage (0.22 * skill->GetMagicattack ());
                skill->SetGolddamage (0.22 * skill->GetMagicattack ());
                skill->GetPlayer ()->SetCombat (1);
                skill->GetPlayer ()->SetPerform (0);
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
        class State4:public SkillStub::State
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
      Skill2814Stub ():SkillStub (2814)
        {
            cls = 11;
            name = L"狂·晓月银光";
            nativename = "狂·晓月银光";
            icon = "魔晓月银光.dds";
            max_level = 1;
            type = 1;
            apcost = 150;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
            rank = 31;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 2;
            showorder = 1025;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (44879);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "月仙_晓月银光_击中.sgc";
            range.type = 0;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2592, 10));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
            statestub.push_back (new State4 ());
#endif
        }
        virtual ~ Skill2814Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (297);
        }
        int GetExecutetime (Skill * skill) const
        {
            return -1;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 5000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 99 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 46377 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 2000000 };
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
            return (float) (27 + skill->GetPlayer ()->GetPrayrangeplus ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format);
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
            skill->GetVictim ()->SetRatio (0.8);
            skill->GetVictim ()->SetSlow (1);
            skill->GetVictim ()->SetProbability (1.0 * -1);
            skill->GetVictim ()->SetTime (5000);
            skill->GetVictim ()->SetRatio (0.3);
            skill->GetVictim ()->SetSlowfly (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000);
            skill->GetVictim ()->SetRatio (0.01);
            skill->GetVictim ()->SetAmount (15);
            skill->GetVictim ()->SetValue (2);
            skill->GetVictim ()->SetInchurt5 (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetRatio (0.03);
            skill->GetVictim ()->SetScaleinchp (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (36.5 + skill->GetPlayer ()->GetPrayrangeplus ());
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
