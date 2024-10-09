#ifndef __GNET_FACTIONDATA_H
#define __GNET_FACTIONDATA_H
#include "gnmarshal.h"
namespace GNET
{	
	struct testsync_param_ct
	{
		int padding;
		testsync_param_ct(int _padding=0) : padding(_padding) { }
		testsync_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct testsync_param_st
	{
		int padding;
		testsync_param_st(int _padding=0) : padding(_padding) { }
		testsync_param_st(const testsync_param_ct& _ct){
			padding=_ct.padding;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct create_param_ct
	{
		Octets faction_name;
		Octets proclaim;
		create_param_ct(const Octets& _faction_name=Octets(),const Octets& _proclaim=Octets()) : faction_name(_faction_name),proclaim(_proclaim) { }
		create_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> faction_name;
			os >> proclaim;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<faction_name<<proclaim;
		}
	};
	struct create_param_st
	{
		int level;
		unsigned int money;
		int sp;
		Octets faction_name;
		Octets proclaim;
		create_param_st(int _level=0,unsigned int _money=0,int _sp=0,const Octets& _faction_name=Octets(),const Octets& _proclaim=Octets()) : level(_level),money(_money),sp(_sp),faction_name(_faction_name),proclaim(_proclaim) { }
		create_param_st(int _level,unsigned int _money,int _sp,const create_param_ct& _ct) : level(_level),money(_money),sp(_sp){
			faction_name=_ct.faction_name;
			proclaim=_ct.proclaim;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> level;
			os >> money;
			os >> sp;
			os >> faction_name;
			os >> proclaim;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<level<<money<<sp<<faction_name<<proclaim;
		}
	};
	struct changeproclaim_param_ct
	{
		Octets proclaim;
		changeproclaim_param_ct(const Octets& _proclaim=Octets()) : proclaim(_proclaim) { }
		changeproclaim_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> proclaim;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<proclaim;
		}
	};
	struct changeproclaim_param_st
	{
		Octets proclaim;
		changeproclaim_param_st(const Octets& _proclaim=Octets()) : proclaim(_proclaim) { }
		changeproclaim_param_st(const changeproclaim_param_ct& _ct){
			proclaim=_ct.proclaim;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> proclaim;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<proclaim;
		}
	};
	struct acceptjoin_param_ct
	{
		int applicant;
		char blAgree;
		acceptjoin_param_ct(int _applicant=-1,char _blAgree=0) : applicant(_applicant),blAgree(_blAgree) { }
		acceptjoin_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> applicant;
			os >> blAgree;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<applicant<<blAgree;
		}
	};
	struct acceptjoin_param_st
	{
		int applicant;
		char blAgree;
		acceptjoin_param_st(int _applicant=-1,char _blAgree=0) : applicant(_applicant),blAgree(_blAgree) { }
		acceptjoin_param_st(const acceptjoin_param_ct& _ct){
			applicant=_ct.applicant;
			blAgree=_ct.blAgree;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> applicant;
			os >> blAgree;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<applicant<<blAgree;
		}
	};
	struct expelmember_param_ct
	{
		int memberid;
		expelmember_param_ct(int _memberid=-1) : memberid(_memberid) { }
		expelmember_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> memberid;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<memberid;
		}
	};
	struct expelmember_param_st
	{
		int memberid;
		expelmember_param_st(int _memberid=-1) : memberid(_memberid) { }
		expelmember_param_st(const expelmember_param_ct& _ct){
			memberid=_ct.memberid;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> memberid;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<memberid;
		}
	};
	struct appoint_param_ct
	{
		int memberid;
		char newoccup;
		appoint_param_ct(int _memberid=-1,char _newoccup=-1) : memberid(_memberid),newoccup(_newoccup) { }
		appoint_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> memberid;
			os >> newoccup;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<memberid<<newoccup;
		}
	};
	struct appoint_param_st
	{
		int memberid;
		char newoccup;
		appoint_param_st(int _memberid=-1,char _newoccup=-1) : memberid(_memberid),newoccup(_newoccup) { }
		appoint_param_st(const appoint_param_ct& _ct){
			memberid=_ct.memberid;
			newoccup=_ct.newoccup;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> memberid;
			os >> newoccup;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<memberid<<newoccup;
		}
	};
	struct masterresign_param_ct
	{
		int newmaster;
		masterresign_param_ct(int _newmaster=-1) : newmaster(_newmaster) { }
		masterresign_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> newmaster;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<newmaster;
		}
	};
	struct masterresign_param_st
	{
		int newmaster;
		masterresign_param_st(int _newmaster=-1) : newmaster(_newmaster) { }
		masterresign_param_st(const masterresign_param_ct& _ct){
			newmaster=_ct.newmaster;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> newmaster;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<newmaster;
		}
	};
	struct resign_param_ct
	{
		int padding;
		resign_param_ct(int _padding=0) : padding(_padding) { }
		resign_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct resign_param_st
	{
		int padding;
		resign_param_st(int _padding=0) : padding(_padding) { }
		resign_param_st(const resign_param_ct& _ct){
			padding=_ct.padding;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct leave_param_ct
	{
		char paddings;
		leave_param_ct(char _paddings=0) : paddings(_paddings) { }
		leave_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> paddings;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<paddings;
		}
	};
	struct leave_param_st
	{
		char paddings;
		leave_param_st(char _paddings=0) : paddings(_paddings) { }
		leave_param_st(const leave_param_ct& _ct){
			paddings=_ct.paddings;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> paddings;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<paddings;
		}
	};
	struct broadcast_param_ct
	{
		Octets msg;
		broadcast_param_ct(const Octets& _msg=Octets()) : msg(_msg) { }
		broadcast_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> msg;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<msg;
		}
	};
	struct broadcast_param_st
	{
		Octets msg;
		broadcast_param_st(const Octets& _msg=Octets()) : msg(_msg) { }
		broadcast_param_st(const broadcast_param_ct& _ct){
			msg=_ct.msg;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> msg;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<msg;
		}
	};
	struct dismiss_param_ct
	{
		int padding;
		dismiss_param_ct(int _padding=0) : padding(_padding) { }
		dismiss_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct dismiss_param_st
	{
		int padding;
		dismiss_param_st(int _padding=0) : padding(_padding) { }
		dismiss_param_st(const dismiss_param_ct& _ct){
			padding=_ct.padding;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct upgrade_param_ct
	{
		int padding;
		upgrade_param_ct(int _padding=0) : padding(_padding) { }
		upgrade_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct upgrade_param_st
	{
		unsigned int money;
		int padding;
		upgrade_param_st(unsigned int _money=0,int _padding=0) : money(_money),padding(_padding) { }
		upgrade_param_st(unsigned int _money,const upgrade_param_ct& _ct) : money(_money){
			padding=_ct.padding;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> money;
			os >> padding;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<money<<padding;
		}
	};
	struct degrade_param_ct
	{
		int padding;
		degrade_param_ct(int _padding=0) : padding(_padding) { }
		degrade_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct degrade_param_st
	{
		int padding;
		degrade_param_st(int _padding=0) : padding(_padding) { }
		degrade_param_st(const degrade_param_ct& _ct){
			padding=_ct.padding;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct listmember_param_ct
	{
		int handle;
		listmember_param_ct(int _handle=-1) : handle(_handle) { }
		listmember_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> handle;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<handle;
		}
	};
	struct listmember_param_st
	{
		int handle;
		listmember_param_st(int _handle=-1) : handle(_handle) { }
		listmember_param_st(const listmember_param_ct& _ct){
			handle=_ct.handle;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> handle;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<handle;
		}
	};
	struct rename_param_ct
	{
		int dst_roleid;
		Octets new_name;
		rename_param_ct(int _dst_roleid=-1,Octets _new_name=Octets()) : dst_roleid(_dst_roleid),new_name(_new_name) { }
		rename_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_roleid;
			os >> new_name;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_roleid<<new_name;
		}
	};
	struct rename_param_st
	{
		int dst_roleid;
		Octets new_name;
		rename_param_st(int _dst_roleid=-1,Octets _new_name=Octets()) : dst_roleid(_dst_roleid),new_name(_new_name) { }
		rename_param_st(const rename_param_ct& _ct){
			dst_roleid=_ct.dst_roleid;
			new_name=_ct.new_name;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_roleid;
			os >> new_name;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_roleid<<new_name;
		}
	};
	struct allianceapply_param_ct
	{
		int dst_fid;
		allianceapply_param_ct(int _dst_fid=0) : dst_fid(_dst_fid) { }
		allianceapply_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid;
		}
	};
	struct allianceapply_param_st
	{
		int dst_fid;
		allianceapply_param_st(int _dst_fid=0) : dst_fid(_dst_fid) { }
		allianceapply_param_st(const allianceapply_param_ct& _ct){
			dst_fid=_ct.dst_fid;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid;
		}
	};
	struct alliancereply_param_ct
	{
		int dst_fid;
		char agree;
		alliancereply_param_ct(int _dst_fid=0,char _agree=0) : dst_fid(_dst_fid),agree(_agree) { }
		alliancereply_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> agree;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<agree;
		}
	};
	struct alliancereply_param_st
	{
		int dst_fid;
		char agree;
		alliancereply_param_st(int _dst_fid=0,char _agree=0) : dst_fid(_dst_fid),agree(_agree) { }
		alliancereply_param_st(const alliancereply_param_ct& _ct){
			dst_fid=_ct.dst_fid;
			agree=_ct.agree;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> agree;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<agree;
		}
	};
	struct hostileapply_param_ct
	{
		int dst_fid;
		hostileapply_param_ct(int _dst_fid=0) : dst_fid(_dst_fid) { }
		hostileapply_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid;
		}
	};
	struct hostileapply_param_st
	{
		int dst_fid;
		hostileapply_param_st(int _dst_fid=0) : dst_fid(_dst_fid) { }
		hostileapply_param_st(const hostileapply_param_ct& _ct){
			dst_fid=_ct.dst_fid;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid;
		}
	};
	struct hostilereply_param_ct
	{
		int dst_fid;
		char agree;
		hostilereply_param_ct(int _dst_fid=0,char _agree=0) : dst_fid(_dst_fid),agree(_agree) { }
		hostilereply_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> agree;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<agree;
		}
	};
	struct hostilereply_param_st
	{
		int dst_fid;
		char agree;
		hostilereply_param_st(int _dst_fid=0,char _agree=0) : dst_fid(_dst_fid),agree(_agree) { }
		hostilereply_param_st(const hostilereply_param_ct& _ct){
			dst_fid=_ct.dst_fid;
			agree=_ct.agree;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> agree;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<agree;
		}
	};
	struct removerelationapply_param_ct
	{
		int dst_fid;
		char force;
		removerelationapply_param_ct(int _dst_fid=0,char _force=0) : dst_fid(_dst_fid),force(_force) { }
		removerelationapply_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> force;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<force;
		}
	};
	struct removerelationapply_param_st
	{
		int dst_fid;
		char force;
		removerelationapply_param_st(int _dst_fid=0,char _force=0) : dst_fid(_dst_fid),force(_force) { }
		removerelationapply_param_st(const removerelationapply_param_ct& _ct){
			dst_fid=_ct.dst_fid;
			force=_ct.force;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> force;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<force;
		}
	};
	struct removerelationreply_param_ct
	{
		int dst_fid;
		char agree;
		removerelationreply_param_ct(int _dst_fid=0,char _agree=0) : dst_fid(_dst_fid),agree(_agree) { }
		removerelationreply_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> agree;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<agree;
		}
	};
	struct removerelationreply_param_st
	{
		int dst_fid;
		char agree;
		removerelationreply_param_st(int _dst_fid=0,char _agree=0) : dst_fid(_dst_fid),agree(_agree) { }
		removerelationreply_param_st(const removerelationreply_param_ct& _ct){
			dst_fid=_ct.dst_fid;
			agree=_ct.agree;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> dst_fid;
			os >> agree;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<dst_fid<<agree;
		}
	};
	struct listrelation_param_ct
	{
		int padding;
		listrelation_param_ct(int _padding=0) : padding(_padding) { }
		listrelation_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct listrelation_param_st
	{
		int padding;
		listrelation_param_st(int _padding=0) : padding(_padding) { }
		listrelation_param_st(const listrelation_param_ct& _ct){
			padding=_ct.padding;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> padding;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<padding;
		}
	};
	struct cancelexpelschedule_param_ct
	{
		int memberid;
		cancelexpelschedule_param_ct(int _memberid=-1) : memberid(_memberid) { }
		cancelexpelschedule_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> memberid;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<memberid;
		}
	};
	struct cancelexpelschedule_param_st
	{
		int memberid;
		cancelexpelschedule_param_st(int _memberid=-1) : memberid(_memberid) { }
		cancelexpelschedule_param_st(const cancelexpelschedule_param_ct& _ct){
			memberid=_ct.memberid;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> memberid;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<memberid;
		}
	};
	struct accelerateexpelschedule_param_ct
	{
		int wastetime;
		accelerateexpelschedule_param_ct(int _wastetime=0) : wastetime(_wastetime) { }
		accelerateexpelschedule_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> wastetime;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<wastetime;
		}
	};
	struct accelerateexpelschedule_param_st
	{
		int wastetime;
		accelerateexpelschedule_param_st(int _wastetime=0) : wastetime(_wastetime) { }
		accelerateexpelschedule_param_st(const accelerateexpelschedule_param_ct& _ct){
			wastetime=_ct.wastetime;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> wastetime;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<wastetime;
		}
	};
	struct factionrename_param_ct
	{
		Octets faction_name;
		factionrename_param_ct(const Octets& _faction_name=Octets()) : faction_name(_faction_name) { }
		factionrename_param_ct& Create(const Marshal::OctetsStream& os) 
		{
			os >> faction_name;
			return *this;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<faction_name;
		}
	};
	struct factionrename_param_st
	{
		Octets faction_name;
		factionrename_param_st(const Octets& _faction_name=Octets()) : faction_name(_faction_name) { }
		factionrename_param_st(const factionrename_param_ct& _ct){
			faction_name=_ct.faction_name;
		}
		void Create(const Marshal::OctetsStream& os) 
		{
			os >> faction_name;
		}
		Marshal::OctetsStream marshal()
		{
			return Marshal::OctetsStream()<<faction_name;
		}
	};
}; //end of namespace
#endif	
