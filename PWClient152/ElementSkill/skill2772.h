#ifndef __CPPGEN_GNET_SKILL2772
#define __CPPGEN_GNET_SKILL2772
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2772:public Skill
    {
      public:
        enum
        { SKILL_ID = 2772 };
          Skill2772 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2772Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (134);
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
      Skill2772Stub ():SkillStub (2772)
        {
            cls = 10;
            name = L"啸月";
            nativename = "啸月";
            icon = "啸月.dds";
            max_level = 1;
            type = 2;
            apcost = 30;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 8;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1001;
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
            effect = "夜影_啸月_击中.sgc";
            range.type = 2;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill2772Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (134);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 90000;
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
            static int array[10] = { 46327 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1000000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (12);
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
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (11000);
            skill->GetVictim ()->SetRatio (1);
            skill->GetVictim ()->SetSpeedup (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetPlayer ()->GetMaxhp () ==
                                                 skill->GetT0 ()? (skill->GetPlayer ()->GetMaxmp () == skill->GetT1 ()? 100 : 0) : 0);
            skill->GetVictim ()->SetTime (11000);
            skill->GetVictim ()->SetRatio (0.2);
            skill->GetVictim ()->SetDechurt (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetTime (3000);
            skill->GetVictim ()->SetRatio (0);
            skill->GetVictim ()->SetAmount (141);
            skill->GetVictim ()->SetValue (2);
            skill->GetVictim ()->SetInsertvstate (1);
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
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetMaxhp ());
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) (player->GetMaxmp ());
        }
#endif
    };
}
#endif
