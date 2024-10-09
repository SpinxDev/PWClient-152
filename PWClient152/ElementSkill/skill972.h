#ifndef __CPPGEN_GNET_SKILL972
#define __CPPGEN_GNET_SKILL972
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill972:public Skill
    {
      public:
        enum
        { SKILL_ID = 972 };
          Skill972 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill972Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (50);
                skill->GetPlayer ()->SetDecelfap (100 + skill->GetLevel () * 50);
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
      Skill972Stub ():SkillStub (972)
        {
            cls = 258;
            name = L"ÃðÆÇÕæ¾÷";
            nativename = "ÃðÆÇÕæ¾÷";
            icon = "ÃðÆÇÕæ¾÷.dds";
            max_level = 10;
            type = 3;
            apcost = 150050;
            arrowcost = 0;
            apgain = 0;
            attr = 3;
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
            effect = "ÃðÆÇÕæ¾÷.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill972Stub ()
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
            return 1000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 400015, 400020, 400025, 400030, 400035, 400040, 400045, 400050, 400055, 400060 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 560, 1040, 1680, 2560, 3720, 5260, 7300, 10000, 13620, 18480 };
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
            return (float) (25);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 50, 100 + skill->GetLevel () * 50, 1 + skill->GetLevel () * 0.5);

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
            skill->GetVictim ()->SetTime (1000 + skill->GetLevel () * 500 + skill->GetT1 () * 100);
            skill->GetVictim ()->SetRatio (0.2);
            skill->GetVictim ()->SetDeeppoision (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (28);
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
