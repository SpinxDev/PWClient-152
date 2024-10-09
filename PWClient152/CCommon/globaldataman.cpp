/*
 * FILE: globaldataman.cpp
 *
 * DESCRIPTION: global data loader and manager
 *
 * CREATED BY: Hedi, 2005/7/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include <A3DTypes.h>
#include <vector.h>
#include <globaldataman.h>
#include <AWScriptFile.h>
#include <AChar.h>
#include <ALog.h>
#include <AFileImage.h>

using namespace abase;

static vector<TRANS_TARGET>			global_trans_targets;
static vector<TRANS_TARGET_SERV>	global_trans_targets_server;
static vector<DOMAIN_INFO>			global_domain_infos;
static vector<DOMAIN_INFO_SERV>		global_domain_infos_server;

static vector<GSHOP_ITEM>			global_gshop_items;
static vector<GSHOP_MAIN_TYPE>		global_gshop_maintypes;
static DWORD						global_gshop_timestamp;

static vector<GSHOP_ITEM>			global_gshop_items2;
static vector<GSHOP_MAIN_TYPE>		global_gshop_maintypes2;
static DWORD						global_gshop_timestamp2;

struct BattleTime { int nDay, nHour, nMin; };
static vector<BattleTime> l_vecTime;
static int l_nTimeMax = 0;

struct Domain2 
{
	vector<DOMAIN2_INFO>	info;
	DWORD					timestamp;
	vector<BattleTime>		vecTime;
	int						nTimeMax;	
	float					speed;

	Domain2() : nTimeMax(0), speed(1.0f), timestamp(0) {}
	void Clear(){
		info.clear();
		vecTime.clear();
	}
};
static Domain2	global_domain2_infos[2];

struct Domain2_Sev
{
	vector<DOMAIN2_INFO_SERV>	info;
	DWORD					timestamp;
	vector<BattleTime>		vecTime;
	int						nTimeMax;
	
	Domain2_Sev() : nTimeMax(0), timestamp(0) {}
	void Clear(){
		info.clear();
		vecTime.clear();
	}
};
static Domain2_Sev	global_domain2_infos_server;

vector<TRANS_TARGET> * globaldata_gettranstargets()
{
	return &global_trans_targets;
}

vector<TRANS_TARGET_SERV> * globaldata_gettranstargetsserver()
{
	return &global_trans_targets_server;
}

vector<GSHOP_ITEM> * globaldata_getgshopitems()
{
	return &global_gshop_items;
}

vector<GSHOP_MAIN_TYPE> * globaldata_getgshopmaintypes()
{
	return &global_gshop_maintypes;
}

DWORD globaldata_getgshop_timestamp()
{
	return global_gshop_timestamp;
}

vector<GSHOP_ITEM> * globaldata_getgshopitems2()
{
	return &global_gshop_items2;
}

vector<GSHOP_MAIN_TYPE> * globaldata_getgshopmaintypes2()
{
	return &global_gshop_maintypes2;
}

DWORD globaldata_getgshop_timestamp2()
{
	return global_gshop_timestamp2;
}

vector<DOMAIN_INFO> * globaldata_getdomaininfos()
{
	return &global_domain_infos;
}

vector<DOMAIN_INFO_SERV> * globaldata_getdomaininfosserver()
{
	return &global_domain_infos_server;
}

vector<DOMAIN2_INFO> * globaldata_getdomain2infos(int type)
{
	return (type >= 0 && type < DOMAIN2_TYPE_NUM) ? &global_domain2_infos[type].info : NULL;
}

DWORD globaldata_getdomain2_timestamp(int type)
{
	return (type >= 0 && type < DOMAIN2_TYPE_NUM) ? global_domain2_infos[type].timestamp : 0;
}

vector<DOMAIN2_INFO_SERV> * globaldata_getdomain2infosserver()
{
	return &global_domain2_infos_server.info;
}

static int	calc_domain2_move_time(const A3DPOINT2 &p, const A3DPOINT2 &q, float speed)
{
	float dist = static_cast<float>(sqrt(pow(p.x-q.x, 2) + pow(p.y-q.y, 2)));
	float t = dist / speed;
	int nsec = max(1, (int)ceil(t));
	return nsec;
}

static bool saveimpl_domain2_data(const char *szPath, int index)
{
	FILE * file = fopen(szPath, "wb");
	if (!file) return false;

	const Domain2 &data = global_domain2_infos[index];
	const abase::vector<DOMAIN2_INFO> &infos = data.info;

	fwrite(&data.timestamp, 1, sizeof(data.timestamp), file);
	fwrite(&data.speed, 1, sizeof(data.speed), file);
	
	size_t j(0);
	size_t num = infos.size();
	fwrite(&num, 1, sizeof(int), file);
	for(size_t i = 0; i < infos.size(); i++ )
	{
		const DOMAIN2_INFO &data = infos[i];
		fwrite(&data.name, 1, sizeof(data.name), file);
		fwrite(&data.id, 1, sizeof(data.id), file);
		fwrite(&data.point, 1, sizeof(data.point), file);
		fwrite(&data.wartype, 1, sizeof(data.wartype), file);
		fwrite(&data.countryid, 1, sizeof(data.countryid), file);
		fwrite(&data.iscapital, 1, sizeof(data.iscapital), file);
		fwrite(&data.mappos, 1, sizeof(data.mappos), file);
		fwrite(&data.center, 1, sizeof(data.center), file);

		num = data.verts.size();
		fwrite(&num, 1, sizeof(int), file);
		for(j = 0; j < data.verts.size(); j++)
			fwrite(&data.verts[j], 1, sizeof(A3DPOINT2), file);

		num = data.faces.size()/3;
		fwrite(&num, 1, sizeof(int), file);
		for(j = 0; j < data.faces.size(); j++)
			fwrite(&data.faces[j], 1, sizeof(int), file);

		num = data.neighbours.size();
		fwrite(&num, 1, sizeof(int), file);
		for(j = 0; j < data.neighbours.size(); j++)
			fwrite(&data.neighbours[j], 1, sizeof(int), file);
	}
	num = data.vecTime.size();
	fwrite(&num, 1, sizeof(int), file);
	for(j = 0; j < data.vecTime.size(); j++)
		fwrite(&data.vecTime[j], 1, sizeof(BattleTime), file);
	fwrite(&data.nTimeMax, 1, sizeof(int), file);
	fflush(file);
	fclose(file);

	return true;
}

static bool loadimpl_domain2_data(const char *szPath, int index)
{
	Domain2 &data = global_domain2_infos[index];
	data.Clear();

	AFileImage domainFile;
	if( !domainFile.Open(szPath, AFILE_OPENEXIST | AFILE_BINARY) )
	{
		return false;
	}
	
	bool bRet(false);
	while (true)
	{
		DWORD dwReadLen;
		if (!domainFile.Read(&data.timestamp, sizeof(data.timestamp), &dwReadLen))
			break;
		if (!domainFile.Read(&data.speed, sizeof(data.speed), &dwReadLen))
			break;
		a_ClampFloor(data.speed, 0.001f);

		int nNumDomains;
		if( !domainFile.Read(&nNumDomains, sizeof(int), &dwReadLen) ||
			nNumDomains < 0)
			break;
		
		int i(0), j(0), num(0);
		for(i=0; i<nNumDomains; i++)
		{
			DOMAIN2_INFO		theDomain;
			memset(&theDomain, 0, sizeof(DOMAIN2_INFO));

			if( !domainFile.Read(&theDomain.name, sizeof(theDomain.name), &dwReadLen) ||
				!domainFile.Read(&theDomain.id, sizeof(theDomain.id), &dwReadLen) ||
				!domainFile.Read(&theDomain.point, sizeof(theDomain.point), &dwReadLen) ||
				!domainFile.Read(&theDomain.wartype, sizeof(theDomain.wartype), &dwReadLen) ||
				!domainFile.Read(&theDomain.countryid, sizeof(theDomain.countryid), &dwReadLen) ||
				!domainFile.Read(&theDomain.iscapital, sizeof(theDomain.iscapital), &dwReadLen) ||
				!domainFile.Read(&theDomain.mappos, sizeof(theDomain.mappos), &dwReadLen) ||
				!domainFile.Read(&theDomain.center, sizeof(A3DPOINT2), &dwReadLen) ||
				!domainFile.Read(&num, sizeof(int), &dwReadLen) )
				break;

			for(j=0; j<num; j++)
			{
				A3DPOINT2 vert;
				if( !domainFile.Read(&vert, sizeof(A3DPOINT2), &dwReadLen) )
					break;
				theDomain.verts.push_back(vert);
			}
			if (j<num) break;
			
			if( !domainFile.Read(&num, sizeof(int), &dwReadLen) )
				break;
			for(j=0; j<num * 3; j++)
			{
				int		index;
				if( !domainFile.Read(&index, sizeof(int), &dwReadLen) )
					break;
				theDomain.faces.push_back(index);
			}
			if (j < num) break;
			
			if( !domainFile.Read(&num, sizeof(int), &dwReadLen) )
				break;
			for(j=0; j<num; j++)
			{
				int		id;
				if( !domainFile.Read(&id, sizeof(int), &dwReadLen) )
					break;
				theDomain.neighbours.push_back(id);
			}
			if (j < num) break;

			data.info.push_back(theDomain);
		}
		if (i < nNumDomains) break;

		if( !domainFile.Read(&num, sizeof(int), &dwReadLen) )
			break;
		for(i=0; i<num; i++)
		{
			BattleTime theTime;
			if( !domainFile.Read(&theTime, sizeof(BattleTime), &dwReadLen) )
				break;
			data.vecTime.push_back(theTime);
		}
		if ( i < num) break;

		if( !domainFile.Read(&data.nTimeMax, sizeof(int), &dwReadLen) )
			break;		
		
		//	计算各领土到达邻居的时间
		for(i=0; i<nNumDomains; i++)
		{
			DOMAIN2_INFO &domain_info = data.info[i];
			for(j=0; j<domain_info.neighbours.size(); j++)
			{
				int neighbour = domain_info.neighbours[j];
				DOMAIN2_INFO * pNeighbour = data.info.begin();
				for (; pNeighbour != data.info.end(); ++pNeighbour)
				{
					if (pNeighbour->id == neighbour) break;
				}
				if (pNeighbour == data.info.end())
					break;
				int nsec = calc_domain2_move_time(domain_info.center, pNeighbour->center, data.speed);
				domain_info.time_neighbours.push_back(nsec);
			}
			if (j < domain_info.neighbours.size())
				break;
		}
		if (i < nNumDomains) break;

		bRet = true;
		break;
	}
	domainFile.Close();
	return bRet;
}


static bool saveimpl_domain2_data_sev(const char *szPath, int index)
{
	FILE *fpServer = fopen(szPath, "wb");
	if( NULL == fpServer ) return false;

	const Domain2 &data = global_domain2_infos[index];
	const abase::vector<DOMAIN2_INFO> &infos = data.info;

	bool bRet(false);
	while (true)
	{
		int i(0), j(0);

		fwrite(&data.timestamp, 1, sizeof(data.timestamp), fpServer);
		int nCount = infos.size();
		fwrite(&nCount, 1, sizeof(int), fpServer);
		for(i=0; i<nCount; i++)
		{
			const DOMAIN2_INFO &domain_info = infos[i];
			fwrite(&domain_info.id, 1, sizeof(domain_info.id), fpServer);
			fwrite(&domain_info.point, 1, sizeof(domain_info.point), fpServer);
			fwrite(&domain_info.wartype, 1, sizeof(domain_info.wartype), fpServer);
			fwrite(&domain_info.countryid, 1, sizeof(domain_info.countryid), fpServer);
			fwrite(&domain_info.iscapital, 1, sizeof(domain_info.iscapital), fpServer);
			fwrite(&domain_info.mappos, 1, sizeof(domain_info.mappos), fpServer);

			int nNumNeighbours = domain_info.neighbours.size();

			fwrite(&nNumNeighbours, 1, sizeof(int), fpServer);
			for(j=0; j<nNumNeighbours; j++)
				fwrite(&domain_info.neighbours[j], 1, sizeof(int), fpServer);
			for(j=0; j<nNumNeighbours; j++)
				fwrite(&domain_info.time_neighbours[j], 1, sizeof(int), fpServer);
		}
		
		nCount = data.vecTime.size();
		fwrite(&nCount, sizeof(int), 1, fpServer);

		for(i=0; i<nCount; i++)
			fwrite(&data.vecTime[i], sizeof(BattleTime), 1, fpServer);

		fwrite(&data.nTimeMax, sizeof(int), 1, fpServer);

		bRet = true;
		break;
	}
	
	fclose(fpServer);
	return bRet;
}

static bool loadimpl_domain2_data_sev(const char *szPath)
{
	FILE *fpServer = fopen(szPath, "rb");
	if( !fpServer )	return false;

	Domain2_Sev &data = global_domain2_infos_server;
	data.Clear();

	abase::vector<DOMAIN2_INFO_SERV> &infos = data.info;
	fread(&data.timestamp, 1, sizeof(data.timestamp), fpServer);

	int i(0), j(0);
	int nCount;
	fread(&nCount, 1, sizeof(int), fpServer);
	if (nCount < 0) return false;
	for(i=0; i<nCount; i++)
	{
		DOMAIN2_INFO_SERV domain_info;
		memset(&domain_info, 0, sizeof(domain_info));

		fread(&domain_info.id, 1, sizeof(domain_info.id), fpServer);
		fread(&domain_info.point, 1, sizeof(domain_info.point), fpServer);
		fread(&domain_info.wartype, 1, sizeof(domain_info.wartype), fpServer);
		fread(&domain_info.countryid, 1, sizeof(domain_info.countryid), fpServer);
		fread(&domain_info.iscapital, 1, sizeof(domain_info.iscapital), fpServer);
		fread(&domain_info.mappos, 1, sizeof(domain_info.mappos), fpServer);

		int nNumNeighbours;
		fread(&nNumNeighbours, 1, sizeof(int), fpServer);
		for(j=0; j<nNumNeighbours; j++)
		{
			int idNeighbour;
			fread(&idNeighbour, 1, sizeof(idNeighbour), fpServer);
			domain_info.neighbours.push_back(idNeighbour);
		}
		for(j=0; j<nNumNeighbours; j++)
		{
			int nsec;
			fread(&nsec, 1, sizeof(nsec), fpServer);
			domain_info.time_neighbours.push_back(nsec);
		}
		infos.push_back(domain_info);
	}
	int num(0);
	fread(&num, 1, sizeof(int), fpServer);
	if (num < 0) return false;
	for(i=0; i<num; i++)
	{
		BattleTime theTime;
		fread(&theTime, 1, sizeof(BattleTime), fpServer);
		data.vecTime.push_back(theTime);
	}
	fread(&data.nTimeMax, 1, sizeof(int), fpServer);

	fclose(fpServer);
	return true;
}

bool globaldata_load()
{
	//	Open font description file
	AWScriptFile ScriptFile;
	if (!ScriptFile.Open("maps\\world\\world_targets.txt"))
	{
		a_LogOutput(1, "GlobalData_Load(), Failed to open file world_targets");
		return false;
	}

	//	Read configs
	while (ScriptFile.GetNextToken(true))
	{
		TRANS_TARGET trans_target;

		trans_target.id = _wtoi(ScriptFile.m_szToken);

		ScriptFile.GetNextToken(false);
		wcscpy(trans_target.name, ScriptFile.m_szToken);

		ScriptFile.GetNextToken(false);
		trans_target.world_id = _wtoi(ScriptFile.m_szToken);

		ScriptFile.GetNextToken(false);
		swscanf(ScriptFile.m_szToken, L"%f,%f,%f", &trans_target.vecPos.x, &trans_target.vecPos.y, &trans_target.vecPos.z);

		ScriptFile.GetNextToken(false);
		trans_target.domain_id = _wtoi(ScriptFile.m_szToken);

		global_trans_targets.push_back(trans_target);
	}
	ScriptFile.Close();

	// now load domain info file
	AFileImage domainFile;
	if( !domainFile.Open("Data\\domain.data", AFILE_OPENEXIST | AFILE_BINARY) )
	{
		a_LogOutput(1, "GlobalData_Load(), Failed to open domain data file");
		return false;
	}
	
	DWORD dwReadLen;
	int nNumDomains;
	if( !domainFile.Read(&nNumDomains, sizeof(int), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
		return false;
	}

	int i, j;
	for(i=0; i<nNumDomains; i++)
	{
		DOMAIN_INFO		theDomain;
		int				num;

		memset(&theDomain, 0, sizeof(DOMAIN_INFO));
		if( !domainFile.Read(&theDomain.name, 16 * sizeof(WORD), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}

		if( !domainFile.Read(&theDomain.id, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}
		if( !domainFile.Read(&theDomain.type, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}
		if( !domainFile.Read(&theDomain.reward, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}

		if( !domainFile.Read(&theDomain.center, sizeof(A3DPOINT2), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}

		if( !domainFile.Read(&num, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}
		for(j=0; j<num; j++)
		{
			A3DPOINT2 vert;
			if( !domainFile.Read(&vert, sizeof(A3DPOINT2), &dwReadLen) )
			{
				a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
				return false;
			}
			theDomain.verts.push_back(vert);
		}

		if( !domainFile.Read(&num, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}
		for(j=0; j<num * 3; j++)
		{
			int		index;
			if( !domainFile.Read(&index, sizeof(int), &dwReadLen) )
			{
				a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
				return false;
			}
			theDomain.faces.push_back(index);
		}

		if( !domainFile.Read(&num, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}
		for(j=0; j<num; j++)
		{
			int		id;
			if( !domainFile.Read(&id, sizeof(int), &dwReadLen) )
			{
				a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
				return false;
			}
			theDomain.neighbours.push_back(id);
		}
		global_domain_infos.push_back(theDomain);
	}
	// load time list
	int num;
	if( !domainFile.Read(&num, sizeof(int), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
		return false;
	}
	for(i=0; i<num; i++)
	{
		BattleTime theTime;
		if( !domainFile.Read(&theTime, sizeof(BattleTime), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
			return false;
		}
		l_vecTime.push_back(theTime);
	}
	if( !domainFile.Read(&l_nTimeMax, sizeof(int), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted domain data file!");
		return false;
	}
	domainFile.Close();

	if (!loadimpl_domain2_data("Data\\domain2.data", 0))
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted domain2.data file!");
		return false;
	}
	if (!loadimpl_domain2_data("Data\\domain2_cross.data", 1))
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted domain2_cross.data file!");
		return false;
	}

	// now load gshop items.
	AFileImage gshopFile;
	if( !gshopFile.Open("Data\\gshop.data", AFILE_OPENEXIST | AFILE_BINARY) )
	{
		a_LogOutput(1, "GlobalData_Load(), Failed to open domain data file");
		return false;
	}
	if( !gshopFile.Read(&global_gshop_timestamp, sizeof(DWORD), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
		return false;
	}
	int size;
	if( !gshopFile.Read(&size, sizeof(int), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
		return false;
	}
	for(i=0; i<size; i++)
	{
		GSHOP_ITEM item;
		if( !gshopFile.Read(&item, sizeof(item), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
			return false;
		}
		global_gshop_items.push_back(item);
	}
	for(i=0; i<8; i++)
	{
		GSHOP_MAIN_TYPE main_type;
		if( !gshopFile.Read(&main_type.szName, sizeof(WORD) * 64, &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
			return false;
		}
		int num_sub;
		if( !gshopFile.Read(&num_sub, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
			return false;
		}
		for(j=0; j<num_sub; j++)
		{
			WORD szSubName[64];
			if( !gshopFile.Read(&szSubName, sizeof(WORD) * 64, &dwReadLen) )
			{
				a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
				return false;
			}
			main_type.sub_types.push_back((WCHAR*)szSubName);
		}
		global_gshop_maintypes.push_back(main_type);
	}
	gshopFile.Close();

	// now load gshop2 items.
	if( !gshopFile.Open("Data\\gshop1.data", AFILE_OPENEXIST | AFILE_BINARY) )
	{
		a_LogOutput(1, "GlobalData_Load(), Failed to open gshop1 file");
		return false;
	}
	if( !gshopFile.Read(&global_gshop_timestamp2, sizeof(DWORD), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop1 file!");
		return false;
	}
	if( !gshopFile.Read(&size, sizeof(int), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
		return false;
	}
	for(i=0; i<size; i++)
	{
		GSHOP_ITEM item;
		if( !gshopFile.Read(&item, sizeof(item), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
			return false;
		}
		global_gshop_items2.push_back(item);
	}
	for(i=0; i<8; i++)
	{
		GSHOP_MAIN_TYPE main_type;
		if( !gshopFile.Read(&main_type.szName, sizeof(WORD) * 64, &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
			return false;
		}
		int num_sub;
		if( !gshopFile.Read(&num_sub, sizeof(int), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
			return false;
		}
		for(j=0; j<num_sub; j++)
		{
			WORD szSubName[64];
			if( !gshopFile.Read(&szSubName, sizeof(WORD) * 64, &dwReadLen) )
			{
				a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
				return false;
			}
			main_type.sub_types.push_back((WCHAR*)szSubName);
		}
		global_gshop_maintypes2.push_back(main_type);
	}
	gshopFile.Close();



	if( strstr(GetCommandLineA(), "globalsave") )
	{
		int	nCount;
		int i;

		FILE * fpServer = fopen("world_targets.sev", "wb");
		if( NULL == fpServer ) return false;
		nCount = global_trans_targets.size();
		fwrite(&nCount, 1, sizeof(int), fpServer);
		// now output a trans target list for server usage
		for(i=0; i<nCount; i++)
		{
			TRANS_TARGET target = global_trans_targets[i];
			fwrite(&target.id, 1, sizeof(int), fpServer);
			fwrite(&target.world_id, 1, sizeof(int), fpServer);
			fwrite(&target.vecPos, 1, sizeof(A3DVECTOR3), fpServer);
			fwrite(&target.domain_id, 1, sizeof(int), fpServer);
		}
		fclose(fpServer);

		fpServer = fopen("domain.sev", "wb");
		if( NULL == fpServer ) return false;
		nCount = global_domain_infos.size();
		fwrite(&nCount, 1, sizeof(int), fpServer);
		// now output server side domain info
		for(i=0; i<nCount; i++)
		{
			DOMAIN_INFO domain_info = global_domain_infos[i];
			fwrite(&domain_info.id, 1, sizeof(int), fpServer);
			fwrite(&domain_info.type, 1, sizeof(int), fpServer);
			fwrite(&domain_info.reward, 1, sizeof(int), fpServer);
			int nNumNeighbours = domain_info.neighbours.size();
			fwrite(&nNumNeighbours, 1, sizeof(int), fpServer);
			for(int j=0; j<nNumNeighbours; j++)
				fwrite(&domain_info.neighbours[j], 1, sizeof(int), fpServer);
		}

		// output time list
		nCount = l_vecTime.size();
		fwrite(&nCount, sizeof(int), 1, fpServer);
		for(i=0; i<nCount; i++)
			fwrite(&l_vecTime[i], sizeof(BattleTime), 1, fpServer);
		fwrite(&l_nTimeMax, sizeof(int), 1, fpServer);

		fclose(fpServer);

		if (!saveimpl_domain2_data_sev("domain2.sev", 0) ||
			!saveimpl_domain2_data_sev("domain2_cross.sev", 1))
			return false;

		if( !globaldata_loadserver() )
			return false;
		globaldata_releaseserver();
	}

	return true;
}

bool globaldata_save_to(const char * szPath)
{
	FILE * file;
	size_t i, j;
	char szFullPath[MAX_PATH];
	
	//	save world target file
	sprintf(szFullPath, "%s%s", szPath, "maps\\world\\world_targets.txt");
	DWORD flag = GetFileAttributesA(szFullPath);
	if (flag & FILE_ATTRIBUTE_READONLY) 
	{
		flag ^= FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szFullPath,flag);
	}
			
	
	file = fopen(szFullPath, "wb");
	if( NULL == file )
		return false;
	
	WORD tag = 0xfeff;
	fwrite(&tag, 1, sizeof(WORD), file);
	for(i=0; i<global_trans_targets.size(); i++)
	{
		const TRANS_TARGET& trans_target = global_trans_targets[i];
		fwprintf(file, L"%d\t\"%s\"\t%d\t\"%.3f,%.3f,%.3f\"\t%d\r\n", trans_target.id, trans_target.name, trans_target.world_id, 
			trans_target.vecPos.x, trans_target.vecPos.y, trans_target.vecPos.z, trans_target.domain_id);
	}
	fclose(file);

	// now save domain info file
	sprintf(szFullPath, "%s%s", szPath, "data\\domain.data");
	flag = GetFileAttributesA(szFullPath);
	if (flag & FILE_ATTRIBUTE_READONLY) 
	{
		flag ^= FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szFullPath,flag);
	}
	file = fopen(szFullPath, "wb");
	if( NULL == file )
		return false;

	DWORD moxb = 0x42584F4D;
	fwrite(&moxb, 1, sizeof(DWORD), file);

	size_t nNumDomains = global_domain_infos.size();
	fwrite(&nNumDomains, 1, sizeof(int), file);
	for(i=0; i<nNumDomains; i++)
	{
		const DOMAIN_INFO& theDomain = global_domain_infos[i];
		size_t num_verts = theDomain.verts.size();
		size_t num_faces = theDomain.faces.size() / 3;
		size_t num_neighbours =	theDomain.neighbours.size();

		fwrite(theDomain.name, 1, 16 * sizeof(WORD), file);
		fwrite(&theDomain.id, 1, sizeof(int), file);
		fwrite(&theDomain.type, 1, sizeof(int), file);
		fwrite(&theDomain.reward, 1, sizeof(int), file);
		fwrite(&theDomain.center, 1, sizeof(A3DPOINT2), file);

		fwrite(&num_verts, 1, sizeof(int), file);
		for(j=0; j<num_verts; j++)
		{
			A3DPOINT2 vert = theDomain.verts[j];
			fwrite(&vert, 1, sizeof(A3DPOINT2), file);
		}

		fwrite(&num_faces, 1, sizeof(int), file);
		for(j=0; j<num_faces * 3; j++)
		{
			int	index = theDomain.faces[j];
			fwrite(&index, 1, sizeof(int), file);
		}

		fwrite(&num_neighbours, 1, sizeof(int), file);
		for(j=0; j<num_neighbours; j++)
		{
			int id = theDomain.neighbours[j];
			fwrite(&id, 1, sizeof(int), file);
		}
	}

	size_t num_time = l_vecTime.size();
	fwrite(&num_time, 1, sizeof(int), file);
	for(i=0; i<num_time; i++)
	{
		BattleTime theTime = l_vecTime[i];
		fwrite(&theTime, 1, sizeof(BattleTime), file);
	}
	fwrite(&l_nTimeMax, 1, sizeof(int), file);
	fclose(file);
	
	// now save domain2 info file
	sprintf(szFullPath, "%s%s", szPath, "data\\domain2.data");
	flag = GetFileAttributesA(szFullPath);
	if (flag & FILE_ATTRIBUTE_READONLY) 
	{
		flag ^= FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szFullPath,flag);
	}
	if (!saveimpl_domain2_data(szFullPath, 0))
		return false;

	sprintf(szFullPath, "%s%s", szPath, "data\\domain2_cross.data");
	flag = GetFileAttributesA(szFullPath);
	if (flag & FILE_ATTRIBUTE_READONLY) 
	{
		flag ^= FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szFullPath,flag);
	}
	if (!saveimpl_domain2_data(szFullPath, 1))
		return false;

	// now save gshop items.
	sprintf(szFullPath, "%s%s", szPath, "data\\gshop.data");
	flag = GetFileAttributesA(szFullPath);
	if (flag & FILE_ATTRIBUTE_READONLY) 
	{
		flag ^= FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szFullPath,flag);
	}
	file = fopen(szFullPath, "wb");
	if( NULL == file )
		return false;

	fwrite(&global_gshop_timestamp, 1, sizeof(DWORD), file);
	size_t size = global_gshop_items.size();
	fwrite(&size, 1, sizeof(DWORD), file);
	for(i=0; i<size; i++)
	{
		GSHOP_ITEM item = global_gshop_items[i];
		fwrite(&item, 1, sizeof(item), file);
	}
	for(i=0; i<8; i++)
	{
		const GSHOP_MAIN_TYPE& main_type = global_gshop_maintypes[i];
		fwrite(main_type.szName, sizeof(WORD) * 64, 1, file);
		size_t num_sub = main_type.sub_types.size();
		fwrite(&num_sub, sizeof(int), 1, file);
		for(j=0; j<num_sub; j++)
		{
			wchar_t szSubName[64];
			wcscpy(szSubName, main_type.sub_types[j]);
			fwrite(szSubName, sizeof(WORD) * 64, 1, file);
		}
	}
	fclose(file);

	// now save gshop2 items.
	sprintf(szFullPath, "%s%s", szPath, "data\\gshop1.data");
	flag = GetFileAttributesA(szFullPath);
	if (flag & FILE_ATTRIBUTE_READONLY) 
	{
		flag ^= FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szFullPath,flag);
	}
	file = fopen(szFullPath, "wb");
	if( NULL == file )
		return false;

	fwrite(&global_gshop_timestamp2, 1, sizeof(DWORD), file);
	size = global_gshop_items2.size();
	fwrite(&size, 1, sizeof(DWORD), file);
	for(i=0; i<size; i++)
	{
		GSHOP_ITEM item = global_gshop_items2[i];
		fwrite(&item, 1, sizeof(item), file);
	}
	for(i=0; i<8; i++)
	{
		const GSHOP_MAIN_TYPE& main_type = global_gshop_maintypes2[i];
		fwrite(main_type.szName, sizeof(WORD) * 64, 1, file);
		size_t num_sub = main_type.sub_types.size();
		fwrite(&num_sub, sizeof(int), 1, file);
		for(j=0; j<num_sub; j++)
		{
			wchar_t szSubName[64];
			wcscpy(szSubName, main_type.sub_types[j]);
			fwrite(szSubName, sizeof(WORD) * 64, 1, file);
		}
	}
	fclose(file);

	return true;
}

bool globaldata_release()
{
	global_trans_targets.clear();
	global_gshop_items.clear();
	global_domain_infos.clear();
	global_domain2_infos[0].Clear();
	global_domain2_infos[1].Clear();
	global_gshop_maintypes.clear();
	global_gshop_items2.clear();
	global_gshop_maintypes2.clear();
	return true;
}

bool globaldata_loadserver()
{
	FILE * fpServer = fopen("world_targets.sev", "rb");
	if( !fpServer )	return false;
	
	int nCount, i;
	fread(&nCount, 1, sizeof(int), fpServer);
	for(i=0; i<nCount; i++)
	{
		TRANS_TARGET_SERV target;
		fread(&target.id, 1, sizeof(int), fpServer);
		fread(&target.world_id, 1, sizeof(int), fpServer);
		fread(&target.vecPos, 1, sizeof(A3DVECTOR3), fpServer);
		fread(&target.domain_id, 1, sizeof(int), fpServer);
		global_trans_targets_server.push_back(target);
	}
	fclose(fpServer);

	fpServer = fopen("domain.sev", "rb");
	if( !fpServer )	return false;
	fread(&nCount, 1, sizeof(int), fpServer);
	for(i=0; i<nCount; i++)
	{
		DOMAIN_INFO_SERV domain_info;
		fread(&domain_info.id, 1, sizeof(int), fpServer);
		fread(&domain_info.type, 1, sizeof(int), fpServer);
		fread(&domain_info.reward, 1, sizeof(int), fpServer);
		int nNumNeighbours;
		fread(&nNumNeighbours, 1, sizeof(int), fpServer);
		for(int j=0; j<nNumNeighbours; j++)
		{
			int idNeighbour;
			fread(&idNeighbour, 1, sizeof(int), fpServer);
			domain_info.neighbours.push_back(idNeighbour);
		}
		global_domain_infos_server.push_back(domain_info);
	}

	if (!loadimpl_domain2_data_sev("domain2.sev") ||
		!loadimpl_domain2_data_sev("domain2_cross.sev"))
		return false;

	DWORD dwReadLen;
	AFileImage gshopFile;
	// gshop
	if( !gshopFile.Open("gshopsev.data", AFILE_OPENEXIST | AFILE_BINARY) )
	{
		a_LogOutput(1, "GlobalData_Load(), Failed to open domain data file");
		return false;
	}
	if( !gshopFile.Read(&global_gshop_timestamp, sizeof(DWORD), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
		return false;
	}
	int size;
	if( !gshopFile.Read(&size, sizeof(int), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
		return false;
	}
	for(i=0; i<size; i++)
	{
		GSHOP_ITEM item;
		if( !gshopFile.Read(&item, sizeof(item), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop file!");
			return false;
		}
		global_gshop_items.push_back(item);
	}	
	gshopFile.Close();

	// gshop1
	if( !gshopFile.Open("gshopsev1.data", AFILE_OPENEXIST | AFILE_BINARY) )
	{
		a_LogOutput(1, "GlobalData_Load(), Failed to open gshopsev1.data file");
		return false;
	}
	if( !gshopFile.Read(&global_gshop_timestamp2, sizeof(DWORD), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop1 file!");
		return false;
	}
	if( !gshopFile.Read(&size, sizeof(int), &dwReadLen) )
	{
		a_LogOutput(1, "GlobalData_Load(), corrupted gshop1 file!");
		return false;
	}
	for(i=0; i<size; i++)
	{
		GSHOP_ITEM item;
		if( !gshopFile.Read(&item, sizeof(item), &dwReadLen) )
		{
			a_LogOutput(1, "GlobalData_Load(), corrupted gshop1 file!");
			return false;
		}
		global_gshop_items2.push_back(item);
	}	
	gshopFile.Close();
	
	return true;
}

bool globaldata_releaseserver()
{
	global_trans_targets_server.clear();
	global_domain_infos_server.clear();
	global_domain2_infos_server.Clear();
	global_gshop_items.clear();
	global_gshop_items2.clear();
	return true;
}
