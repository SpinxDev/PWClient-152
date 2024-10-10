#ifndef __CPPGEN_GNET_SKILL531
#define __CPPGEN_GNET_SKILL531
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill531:public Skill
    {
      public:
        enum
        { SKILL_ID = 531 };
          Skill531 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill531Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (50);
                skill->GetPlayer ()->SetPerform (1);
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
      Skill531Stub ():SkillStub (531)
        {
            cls = 4;
            name = L"狂·兽王之怒";
            nativename = "狂·兽王之怒";
            icon = "天崩地裂2.dds";
            max_level = 1;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 30;
            eventflag = 1;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1526;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (9);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "怪物治疗术.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(188, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill531Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (50);
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
            static int array[10] = { 89 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 9602 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1000000 };
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
            skill->GetVictim ()->SetTime (15000);
            skill->GetVictim ()->SetValue (8);
            skill->GetVictim ()->SetApgen (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetPlayer ()->GetHp () > skill->GetPlayer ()->GetMaxhp () * 0.5 ? 0 : 100);
            skill->GetVictim ()->SetTime (10000);
            skill->GetVictim ()->SetShowicon (1);
            skill->GetVictim ()->SetAmount (4000);
            skill->GetVictim ()->SetValue (0);
            skill->GetVictim ()->SetAbsorbmagicdamage (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (8.5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
