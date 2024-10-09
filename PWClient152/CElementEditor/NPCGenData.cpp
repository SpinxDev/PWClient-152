/*
 * FILE: NPCGenMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "npcgendata.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CNPCGenMan
//	
///////////////////////////////////////////////////////////////////////////

CNPCGenMan::CNPCGenMan()
{
}

CNPCGenMan::~CNPCGenMan()
{
}

//	Load data from file
bool CNPCGenMan::Load(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "rb");
	if (!fp)
		return false;
	
	size_t version;
	fread(&version, 1, sizeof (size_t), fp);
	if (version > AIGENFILE_VERSION)
		return false;

	NPCGENFILEHEADER7 Header;
	memset(&Header, 0, sizeof (Header));

	if (version < 6)
	{
		NPCGENFILEHEADER temp;
		fread(&temp, 1, sizeof (temp), fp);
		memcpy(&Header, &temp, sizeof (temp));
	}
	else if (version < 7)	//	version >= 6 && version < 7
	{
		NPCGENFILEHEADER6 temp;
		fread(&temp, 1, sizeof (temp), fp);
		memcpy(&Header, &temp, sizeof (temp));
	}
	else	//	version >= 7
	{
		fread(&Header, 1, sizeof (Header), fp);
	}
	
	int i;

	//	Read NPC generate areas
	for (i=0; i < Header.iNumAIGen; i++)
	{
		NPCGENFILEAREA7 SrcArea;

		if (version < 7)
		{
			NPCGENFILEAREA temp;
			fread(&temp, 1, sizeof (temp), fp);

			memcpy(&SrcArea, &temp, sizeof (temp));

			SrcArea.idCtrl		= 0;
			SrcArea.iLifeTime	= 0;
			SrcArea.iMaxNum		= 0;
		}
		else	//	version >= 7
			fread(&SrcArea, 1, sizeof (SrcArea), fp);
		
		AREA Area;
		Area.iType			= SrcArea.iType;
		Area.iNumGen		= SrcArea.iNumGen;
		Area.vPos[0]		= SrcArea.vPos[0];
		Area.vPos[1]		= SrcArea.vPos[1];
		Area.vPos[2]		= SrcArea.vPos[2];
		Area.vDir[0]		= SrcArea.vDir[0];
		Area.vDir[1]		= SrcArea.vDir[1];
		Area.vDir[2]		= SrcArea.vDir[2];
		Area.vExts[0]		= SrcArea.vExts[0];
		Area.vExts[1]		= SrcArea.vExts[1];
		Area.vExts[2]		= SrcArea.vExts[2];
		Area.iNPCType		= SrcArea.iNPCType;
		Area.iGroupType		= SrcArea.iGroupType;
		Area.bAutoRevive	= SrcArea.bAutoRevive;
		Area.idCtrl			= SrcArea.idCtrl;
		Area.iLifeTime		= SrcArea.iLifeTime;
		Area.iMaxNum		= SrcArea.iMaxNum;
		Area.iFirstGen		= m_aNPCGens.size();
		
		m_aAreas.push_back(Area);

		for (int j=0; j < SrcArea.iNumGen; j++)
		{
			NPCGENFILEAIGEN SrcGen;
			if(version < 11)
			{
				NPCGENFILEAIGEN10 oldGen;
				fread(&oldGen, 1, sizeof (oldGen), fp);
				memcpy(&SrcGen,&oldGen,sizeof(oldGen));
				SrcGen.iRefreshLower = 0;
				
			}else fread(&SrcGen, 1, sizeof (SrcGen), fp);

			NPCGEN Gen;
			Gen.iArea			= i;
			Gen.dwID			= SrcGen.dwID;
			Gen.dwNum			= SrcGen.dwNum;
			Gen.dwDiedTimes		= SrcGen.dwDiedTimes;
			Gen.dwAggressive	= SrcGen.dwAggressive;
			Gen.iRefresh		= SrcGen.iRefresh;
			Gen.iRefreshLower	= SrcGen.iRefreshLower;
			Gen.fOffsetTrn		= SrcGen.fOffsetTrn;
			Gen.fOffsetWater	= SrcGen.fOffsetWater;
			Gen.dwFaction		= SrcGen.dwFaction;
			Gen.dwFacHelper		= SrcGen.dwFacHelper;
			Gen.dwFacAccept		= SrcGen.dwFacAccept;
			Gen.bNeedHelp		= SrcGen.bNeedHelp;
			Gen.bDefFaction		= SrcGen.bDefFaction;
			Gen.bDefFacHelper	= SrcGen.bDefFacHelper;
			Gen.bDefFacAccept	= SrcGen.bDefFacAccept;
			Gen.iPathID			= SrcGen.iPathID;
			Gen.iLoopType		= SrcGen.iLoopType;
			Gen.iSpeedFlag		= SrcGen.iSpeedFlag;
			Gen.iDeadTime		= SrcGen.iDeadTime;

			m_aNPCGens.push_back(Gen);
		}
	}

	//	Read resources areas
	for (i=0; i < Header.iNumResArea; i++)
	{
		NPCGENFILERESAREA7 src;

		if (version < 6)
		{
			NPCGENFILERESAREA temp;
			fread(&temp, 1, sizeof (temp), fp);

			memcpy(&src, &temp, sizeof (temp));

			src.dir[0]	= 0;
			src.dir[1]	= 0;
			src.rad		= 0;

			src.idCtrl	= 0;
			src.iMaxNum	= 0;
		}
		else if (version < 7)	//	version >= 6 && version < 7
		{
			NPCGENFILERESAREA6 temp;
			fread(&temp, 1, sizeof (temp), fp);

			memcpy(&src, &temp, sizeof (temp));

			src.idCtrl	= 0;
			src.iMaxNum	= 0;
		}
		else	//	version >= 7
			fread(&src, 1, sizeof (src), fp);

		RESAREA dst;
		dst.vPos[0]		= src.vPos[0];
		dst.vPos[1]		= src.vPos[1];
		dst.vPos[2]		= src.vPos[2];
		dst.fExtX		= src.fExtX;
		dst.fExtZ		= src.fExtZ;
		dst.iResNum		= src.iNumRes;
		dst.bAutoRevive	= src.bAutoRevive;
		dst.idCtrl		= src.idCtrl;
		dst.iMaxNum		= src.iMaxNum;
		dst.dir[0]		= src.dir[0];
		dst.dir[1]		= src.dir[1];
		dst.rad			= src.rad;
		dst.iFirstRes	= m_aRes.size();

		m_aResAreas.push_back(dst);

		for (int j=0; j < src.iNumRes; j++)
		{
			NPCGENFILERES src_res;
			fread(&src_res, 1, sizeof (src_res), fp);

			RES dst_res;
			dst_res.iResType		= src_res.iResType;
			dst_res.idTemplate		= src_res.idTemplate;
			dst_res.dwRefreshTime	= src_res.dwRefreshTime;
			dst_res.dwNumber		= src_res.dwNumber;
			dst_res.fHeiOff			= src_res.fHeiOff;

			m_aRes.push_back(dst_res);
		}
	}

	//	Read dynamic objects
	for (i=0; i < Header.iNumDynObj; i++)
	{
		NPCGENFILEDYNOBJ10 src;

		if (version < 9)
		{
			NPCGENFILEDYNOBJ temp;
			fread(&temp, 1, sizeof (temp), fp);

			memcpy(&src, &temp, sizeof (temp));

			src.idController = 0;
			src.scale = 16;		//	16 means scale == 1.0f
		}
		else if (version < 10)
		{
			NPCGENFILEDYNOBJ9 temp;
			fread(&temp, 1, sizeof (temp), fp);

			memcpy(&src, &temp, sizeof (temp));

			src.scale = 16;
		}
		else
			fread(&src, 1, sizeof (src), fp);

		DYNOBJ dst;
		dst.dwDynObjID	= src.dwDynObjID;
		dst.idCtrl		= src.idController;
		dst.vPos[0]		= src.vPos[0];
		dst.vPos[1]		= src.vPos[1];
		dst.vPos[2]		= src.vPos[2];
		dst.dir[0]		= src.dir[0];
		dst.dir[1]		= src.dir[1];
		dst.rad			= src.rad;
		dst.scale		= src.scale;

		m_aDynObjs.push_back(dst);
	}

	//	Read NPC controller data table
	for (i=0; i < Header.iNumNPCCtrl; i++)
	{
		NPCGENFILECTRL8 src;

		if (version < 8)
		{
			NPCGENFILECTRL temp;
			fread(&temp, 1, sizeof (temp), fp);

			memcpy(&src, &temp, sizeof (temp));

			src.iActiveTimeRange = 0;
		}
		else	//	version >= 8
			fread(&src, 1, sizeof (src), fp);

		CONTROLLER dst;
		dst.id					= src.id;
		dst.iControllerID		= src.iControllerID;
		dst.bActived			= src.bActived;
		dst.iWaitTime			= src.iWaitTime;
		dst.iStopTime			= src.iStopTime;
		dst.bActiveTimeInvalid	= src.bActiveTimeInvalid;
		dst.bStopTimeInvalid	= src.bStopTimeInvalid;
		dst.ActiveTime			= src.ActiveTime;
		dst.StopTime			= src.StopTime;
		dst.iActiveTimeRange	= src.iActiveTimeRange;
		
		strncpy(dst.szName, src.szName, 128);

		m_aControllers.push_back(dst);
	}

	fclose(fp);
	return true;
}

bool CNPCGenMan::Save(const char* szFileName)
{
	FILE* fp = fopen(szFileName, "wb");
	if (!fp)
		return false;
	
	size_t version = AIGENFILE_VERSION;
	fwrite(&version, 1, sizeof (size_t), fp);


	NPCGENFILEHEADER7 Header;
	memset(&Header, 0, sizeof (Header));
	Header.iNumAIGen = m_aAreas.size();
	Header.iNumResArea = m_aResAreas.size();
	Header.iNumNPCCtrl = m_aControllers.size();
	Header.iNumDynObj = m_aDynObjs.size();
	fwrite(&Header, 1, sizeof (Header), fp);
	
	
	int i;

	//	Write NPC generate areas
	for (i=0; i < Header.iNumAIGen; i++)
	{
		AREA Area = m_aAreas[i];
		NPCGENFILEAREA7 SrcArea;
		SrcArea.iType=				Area.iType;		
		SrcArea.iNumGen=			Area.iNumGen;	
		SrcArea.vPos[0]=			Area.vPos[0];	
		SrcArea.vPos[1]=			Area.vPos[1];	
		SrcArea.vPos[2]=			Area.vPos[2];	
		SrcArea.vDir[0]=			Area.vDir[0];	
		SrcArea.vDir[1]=			Area.vDir[1];	
		SrcArea.vDir[2]=			Area.vDir[2];	
		SrcArea.vExts[0]=			Area.vExts[0];	
		SrcArea.vExts[1]=			Area.vExts[1];	
		SrcArea.vExts[2]=			Area.vExts[2];	
		SrcArea.iNPCType=			Area.iNPCType;	
		SrcArea.iGroupType=			Area.iGroupType;
		SrcArea.bAutoRevive=		Area.bAutoRevive;
		SrcArea.idCtrl=				Area.idCtrl;	
		SrcArea.iLifeTime=			Area.iLifeTime;	
		SrcArea.iMaxNum=			Area.iMaxNum;	
		fwrite(&SrcArea, 1, sizeof (SrcArea), fp);
		for (int j=0; j < SrcArea.iNumGen; j++)
		{
			NPCGEN Gen = m_aNPCGens[Area.iFirstGen + j];
			NPCGENFILEAIGEN SrcGen;
			SrcGen.dwID=Gen.dwID;			
			SrcGen.dwNum=Gen.dwNum;			
			SrcGen.dwDiedTimes=Gen.dwDiedTimes;	
			SrcGen.dwAggressive=Gen.dwAggressive;	
			SrcGen.iRefresh=Gen.iRefresh;
			SrcGen.iRefreshLower = Gen.iRefreshLower;
			SrcGen.fOffsetTrn=Gen.fOffsetTrn;		
			SrcGen.fOffsetWater=Gen.fOffsetWater;	
			SrcGen.dwFaction=Gen.dwFaction;		
			SrcGen.dwFacHelper=Gen.dwFacHelper;		
			SrcGen.dwFacAccept=Gen.dwFacAccept;		
			SrcGen.bNeedHelp=Gen.bNeedHelp;		
			SrcGen.bDefFaction=Gen.bDefFaction;		
			SrcGen.bDefFacHelper=Gen.bDefFacHelper;	
			SrcGen.bDefFacAccept=Gen.bDefFacAccept;	
			SrcGen.iPathID=Gen.iPathID;			
			SrcGen.iLoopType=Gen.iLoopType;		
			SrcGen.iSpeedFlag=Gen.iSpeedFlag;		
			SrcGen.iDeadTime=Gen.iDeadTime;	
			fwrite(&SrcGen, 1, sizeof (SrcGen), fp);
		}
	}

	//	Write resources areas
	for (i=0; i < Header.iNumResArea; i++)
	{
		NPCGENFILERESAREA7 src;
		RESAREA dst = m_aResAreas[i];
		src.vPos[0]=dst.vPos[0];		
		src.vPos[1]=dst.vPos[1];		
		src.vPos[2]=dst.vPos[2];		
		src.fExtX=dst.fExtX;		
		src.fExtZ=dst.fExtZ;		
		src.iNumRes=dst.iResNum;		
		src.bAutoRevive=dst.bAutoRevive;	
		src.idCtrl=dst.idCtrl;		
		src.iMaxNum=dst.iMaxNum;		
		src.dir[0]=dst.dir[0];		
		src.dir[1]=dst.dir[1];		
		src.rad=dst.rad;			
		fwrite(&src, 1, sizeof (src), fp);

		for (int j=0; j < src.iNumRes; j++)
		{
			NPCGENFILERES src_res;
			RES dst_res = m_aRes[dst.iFirstRes + j];
			src_res.iResType=dst_res.iResType;
			src_res.idTemplate=dst_res.idTemplate;
			src_res.dwRefreshTime=dst_res.dwRefreshTime;
			src_res.dwNumber=dst_res.dwNumber;
			src_res.fHeiOff=dst_res.fHeiOff;
			fwrite(&src_res, 1, sizeof (src_res), fp);
		}
	}

	//	Write dynamic objects
	for (i=0; i < Header.iNumDynObj; i++)
	{
		NPCGENFILEDYNOBJ10 src;
		
		DYNOBJ dst = m_aDynObjs[i];
		src.dwDynObjID	= dst.dwDynObjID;
		src.idController= dst.idCtrl;
		src.vPos[0]		= dst.vPos[0];
		src.vPos[1]		= dst.vPos[1];
		src.vPos[2]		= dst.vPos[2];
		src.dir[0]		= dst.dir[0];
		src.dir[1]		= dst.dir[1];
		src.rad			= dst.rad;
		src.scale		= dst.scale;

		fwrite(&src, 1, sizeof (src), fp);
	}

	//	Write NPC controller data table
	for (i=0; i < Header.iNumNPCCtrl; i++)
	{
		NPCGENFILECTRL8 src;
		CONTROLLER* dst = (CONTROLLER*)&m_aControllers[i];
		src.id					= dst->id;
		src.iControllerID		= dst->iControllerID;
		src.bActived			= dst->bActived;
		src.iWaitTime			= dst->iWaitTime;
		src.iStopTime			= dst->iStopTime;
		src.bActiveTimeInvalid	= dst->bActiveTimeInvalid;
		src.bStopTimeInvalid	= dst->bStopTimeInvalid;
		src.ActiveTime			= dst->ActiveTime;
		src.StopTime			= dst->StopTime;
		src.iActiveTimeRange	= dst->iActiveTimeRange;
		
		strncpy(src.szName, dst->szName, 128);
		fwrite(&src, 1, sizeof (src), fp);
	}

	fclose(fp);
	return true;
}



