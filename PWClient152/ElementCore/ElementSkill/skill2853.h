#ifndef __CPPGEN_GNET_SKILL2853
#define __CPPGEN_GNET_SKILL2853
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2853:public Skill
    {
      public:
        enum
        { SKILL_ID = 2853 };
          Skill2853 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2853Stub:public SkillStub
    {
      public:
        Skill2853Stub ():SkillStub (2853)
        {
            cls = 10;
            name = L"���ƺ�����Ч��2";
            nativename = "���ƺ�����Ч��2";
            icon = "��ʽ.dds";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "2����.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill2853Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (0);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1001;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 1000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 0, 5, 10, 15, 20, 25, 30, 35, 40, 45 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 0, 300, 1200, 2800, 5200, 8400, 12800, 18600, 26300, 36500 };
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
            skill->GetVictim ()->SetTime (7701);
            skill->GetVictim ()->SetRatio (1);
            skill->GetVictim ()->SetAmount (2);
            skill->GetVictim ()->SetValue (2553);
            skill->GetVictim ()->SetCooldownaward (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (7701);
            skill->GetVictim ()->SetRatio (1);
            skill->GetVictim ()->SetAmount (2);
            skill->GetVictim ()->SetValue (2740);
            skill->GetVictim ()->SetCooldownaward (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (7701);
            skill->GetVictim ()->SetRatio (1);
            skill->GetVictim ()->SetAmount (2);
            skill->GetVictim ()->SetValue (2741);
            skill->GetVictim ()->SetCooldownaward (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (7701);
            skill->GetVictim ()->SetValue (35);
            skill->GetVictim ()->SetIncsmite (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
