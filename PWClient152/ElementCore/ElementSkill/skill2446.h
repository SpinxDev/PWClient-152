#ifndef __CPPGEN_GNET_SKILL2446
#define __CPPGEN_GNET_SKILL2446
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2446:public Skill
    {
      public:
        enum
        { SKILL_ID = 2446 };
          Skill2446 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2446Stub:public SkillStub
    {
      public:
        Skill2446Stub ():SkillStub (2446)
        {
            cls = 255;
            name = L"五德终始";
            nativename = "五德终始";
            icon = "全系防御提高.dds";
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
            showorder = 2002;
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
        virtual ~ Skill2446Stub ()
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
            static int array[10] = { 42259, 42259, 42259, 42259, 42259, 42259, 42259, 42259, 42259, 42259 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredRealmLevel (Skill * skill) const
        {
            static int array[10] = { 2, 5, 8, 11, 14, 17, 20, 23, 26, 29 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 8 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetAddresistance (0.08 * skill->GetLevel ());
            skill->GetPlayer ()->SetAdddefence (0.08 * skill->GetLevel ());
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
