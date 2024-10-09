#ifndef __CPPGEN_GNET_SKILL2688
#define __CPPGEN_GNET_SKILL2688
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2688:public Skill
    {
      public:
        enum
        { SKILL_ID = 2688 };
          Skill2688 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2688Stub:public SkillStub
    {
      public:
        Skill2688Stub ():SkillStub (2688)
        {
            cls = 10;
            name = L"ëÊ×åÑªÂö";
            nativename = "ëÊ×åÑªÂö";
            icon = "Ô¶³Ì¼õÉË.dds";
            max_level = 10;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 6;
            eventflag = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1102;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
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
        virtual ~ Skill2688Stub ()
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
            static int array[10] = { 59, 62, 65, 68, 71, 74, 77, 80, 83, 86 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 53070, 60320, 68750, 78520, 92310, 129060, 186390, 264000, 369180, 510180 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 3980, 8980, 14980, 41980, 91980, 151980, 251980, 371980, 611980, 851980 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), (30 + 5 * skill->GetLevel ()) * (1.0 * 8 / 15));

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetIncfarnormaldmgreduce (0.3 + 0.05 * skill->GetLevel ());
            skill->GetPlayer ()->SetIncfarskilldmgreduce (0.3 + 0.05 * skill->GetLevel ());
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
