#ifndef __CPPGEN_GNET_SKILL1816
#define __CPPGEN_GNET_SKILL1816
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1816:public Skill
    {
      public:
        enum
        { SKILL_ID = 1816 };
          Skill1816 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1816Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 400;
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
                skill->GetPlayer ()->SetDecmp (99);
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
                return 1700;
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
                skill->GetPlayer ()->SetDecmp (396);
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
      Skill1816Stub ():SkillStub (1816)
        {
            cls = 0;
            name = L"Ì«Ðé¡¤ÖïÉñ¾÷";
            nativename = "Ì«Ðé¡¤ÖïÉñ¾÷";
            icon = "¿ñ·ç.dds";
            max_level = 1;
            type = 2;
            apcost = 300;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 8;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 2905;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (1);
            }
            {
                restrict_weapons.push_back (5);
            }
            {
                restrict_weapons.push_back (9);
            }
            {
                restrict_weapons.push_back (182);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "ÎäÏÀ_¿ñ·ç.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(1871, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill1816Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (495);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1700;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 180000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 100 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 5000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 33713 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 5000000 };
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
            return (float) (skill->GetPlayer ()->GetRange ());
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
            return 100;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (12000);
            skill->GetVictim ()->SetRatio (10);
            skill->GetVictim ()->SetAmount (1);
            skill->GetVictim ()->SetValue (1868);
            skill->GetVictim ()->SetAuracurse (1);
            skill->GetVictim ()->SetTime (12000);
            skill->GetVictim ()->SetRatio (0.5);
            skill->GetVictim ()->SetAmount (204);
            skill->GetVictim ()->SetFreemoveapgen (1);
            skill->GetVictim ()->SetTime (12000);
            skill->GetVictim ()->SetRatio (226);
            skill->GetVictim ()->SetAmount (111);
            skill->GetVictim ()->SetValue (2);
            skill->GetVictim ()->SetInsertvstate (1);
            skill->GetVictim ()->SetTime (12000);
            skill->GetVictim ()->SetDenyattackcmd (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (20);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 9;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetAttackdegree ());
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) ((player->GetPureattack () + 4.5 * player->GetWeapondamage () + 12000 +
                             (player->GetSoulpower () - player->GetLevel () * player->GetLevel () - 50 * player->GetLevel ()) * 0.2));
        }
        float GetTalent2 (PlayerWrapper * player) const
        {
            return (float) (player->GetRand () < player->GetCrit () * 2 ? 15 : 10);
        }
#endif
    };
}
#endif
