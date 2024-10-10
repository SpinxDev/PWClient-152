#ifndef __CPPGEN_GNET_SKILL2443
#define __CPPGEN_GNET_SKILL2443
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2443:public Skill
    {
      public:
        enum
        { SKILL_ID = 2443 };
          Skill2443 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2443Stub:public SkillStub
    {
      public:
        Skill2443Stub ():SkillStub (2443)
        {
            cls = 255;
            name = L"不周千风";
            nativename = "不周千风";
            icon = "技能伤害提高.dds";
            max_level = 10;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 8;
            eventflag = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 2001;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 0;
            effect = "";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill2443Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (0);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 0;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 42257, 42257, 42257, 42257, 42257, 42257, 42257, 42257, 42257, 42257 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredRealmLevel (Skill * skill) const
        {
            static int array[10] = { 1, 4, 7, 10, 13, 16, 19, 22, 25, 28 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 2 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetAddskilldamage (0.02 * skill->GetLevel ());
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
