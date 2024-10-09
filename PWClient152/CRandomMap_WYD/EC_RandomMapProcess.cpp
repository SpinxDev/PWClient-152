#include "EC_RandomMapProcess.h"
#include <AMemory.h>
#include <ALog.h>
#include <io.h>
#include <string>
#include <DIRECT.H>
#include <math.h>
#include <A3DTerrain2.h>
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include <AFilePackage.h>


using namespace std;

#define new A_DEBUG_NEW

bool IsExistDir(const char* szDir)
{
	return -1 != _access(szDir,0);
}
bool glb_DeleteDir(const std::string& src)
{
	string src_backup = src;
	if (src_backup.size() > 0 && src_backup[src_backup.size() - 1] != '\\')
	{
		src_backup.append("\\");
	}
	
	string szDir = src_backup;
	szDir = szDir + "*.*";
	int handle;
	struct _finddata_t fileinfo;
	
	handle = _findfirst(szDir.c_str(),&fileinfo);
	if(-1==handle) return false;
	while(!_findnext(handle,&fileinfo))
	{
		if(strcmp(fileinfo.name,".")==0 || strcmp(fileinfo.name,"..")==0)
			continue;
		
		string tempSrc = src_backup + fileinfo.name; 
		
		if(fileinfo.attrib==_A_SUBDIR)
		{
			glb_DeleteDir(tempSrc);
			
			if(IsExistDir(tempSrc.c_str()))
				_rmdir(tempSrc.c_str());
		}
		else
		{
			string tempSrc = src_backup + fileinfo.name;
			remove(tempSrc.c_str());			
		}
	}
	if (src_backup.size() > 0 && src_backup[src_backup.size() - 1] == '/')
	{
		src_backup.erase(src_backup.size()-1,1);
	}
	_findclose(handle);
	
	return true;
}
bool glb_CreateDir(const char *path)
{
	if(_access(path, 0)!=-1) return true;
	
	BOOL ret = CreateDirectoryA(path, NULL);
	if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
	{
		return false;
	}
	return true;
}
void glb_MakeDir(const char* dir,int r)
{
	r--;
	while(r>0&&dir[r]!='/'&&dir[r]!='\\')
		r--;
	if(r==0)
		return;
	glb_MakeDir(dir,r);
	char t[512] = {0};
	strcpy(t,dir);
	t[r]='\0';
	glb_CreateDir(t);
}

bool glb_CopyFiles(string src,string des)
{
	if (src.size() > 0 && src[src.size() - 1] != '\\')
	{
		src.append("\\");
	}
	if (des.size() > 0 && des[des.size() - 1] != '\\')
	{
		des.append("\\");
	}
	
	string szDir = src;
	szDir = szDir + "*.*";
	int handle;
	struct _finddata_t fileinfo;
	
	handle = _findfirst(szDir.c_str(),&fileinfo);
	if(-1==handle) return false;
	while(!_findnext(handle,&fileinfo))
	{
		if(strcmp(fileinfo.name,".")==0 || strcmp(fileinfo.name,"..")==0)
			continue;
		
		string tempSrc = src + fileinfo.name;
		
		string tempDes = tempSrc;
		int pos = tempDes.find(src,0);
		tempDes.replace(pos,src.size(),des.c_str());
		
		if(fileinfo.attrib==_A_SUBDIR)
		{			
			glb_CreateDir(tempDes.c_str());
			glb_CopyFiles(tempSrc,tempDes);
		}
		else
		{
			CopyFileA(tempSrc.c_str(),tempDes.c_str(),true);
		}
	}
	_findclose(handle);
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

void CECTerrainPreprocessor::Release()
{
	memset(&fileVer,0,sizeof(fileVer));
	memset(&fileHeader,0,sizeof(fileHeader));
	memset(&loaderInfo,0,sizeof(loaderInfo));
	
	subTernInfo.clear();
	textureFile.clear();
	subTernName.clear();
}
void CECTerrainPreprocessor::DoProcess(const char* szMapName)
{
	LoadFile(szMapName);
	SaveFile(szMapName);
}
bool CECTerrainPreprocessor::LoadFile(const char* szMapName)
{
	// clear info
	Release();
	
	//Create terrain description file
	char szFile[MAX_PATH] = {0};
	sprintf(szFile, "maps\\%s\\%s.trn2", szMapName,szMapName);
	
	FILE* fpTerrain = NULL;
	
	if (!(fpTerrain = fopen(szFile, "rb")))
	{
		a_LogOutput(1,"CECTerrainPreprocessor::LoadTrn2, Failed to open terrain file %s", szFile);
		return false;
	}
	
	try
	{
		if (fread(&fileVer,1,sizeof(TRN2FILEIDVER),fpTerrain) != sizeof(TRN2FILEIDVER))
			throw 1;
		if (fread(&fileHeader,1,sizeof(TRN2FILEHEADER),fpTerrain) != sizeof(TRN2FILEHEADER))
			throw 1;
		
		m_pAsset->SetBlockRowCountInSubTern(fileHeader.iTrnBlkRow / fileHeader.iSubTrnRow);
		m_pAsset->SetBlockColCountInSubTern(fileHeader.iTrnBlkCol / fileHeader.iSubTrnCol);
		
	//	m_pAsset->SetRowCountSubTern(fileHeader.iSubTrnRow);
//		m_pAsset->SetColCountSubTern(fileHeader.iSubTrnCol);
		
		m_pAsset->SetTrn2Info(fileHeader);
		
		subTernInfo.reserve(fileHeader.iNumSubTrn);
		for (int i=0;i<fileHeader.iNumSubTrn;i++)
		{
			TRN2FILESUBTRN trn2;
			if (fread(&trn2,1,sizeof(TRN2FILESUBTRN),fpTerrain) != sizeof(TRN2FILESUBTRN))
				throw 1;
			subTernInfo.push_back(trn2);  
		}
		
		
		textureFile.reserve(fileHeader.iNumTexture);
		
		for (i=0;i<fileHeader.iNumTexture;i++)
		{
			int len = 0;
			char path[MAX_PATH] = {0};
			fread(&len,1,sizeof(int),fpTerrain);
			if(len)
			{
				fread(path,len,sizeof(char),fpTerrain);
				textureFile.push_back(path);
			}
		}
		
		if (fread(&loaderInfo,1,sizeof(A3DTRN2LOADERB::TRN2FILELOADERINFO),fpTerrain) != sizeof(A3DTRN2LOADERB::TRN2FILELOADERINFO))
			throw 1;
		
		
		subTernName.reserve(fileHeader.iNumSubTrn);
		
		for (i=0;i<fileHeader.iNumSubTrn;i++)
		{
			int len = 0;
			char path[MAX_PATH] = {0};
			fread(&len,1,sizeof(int),fpTerrain);
			if(len)
			{
				fread(path,len,sizeof(char),fpTerrain);
				subTernName.push_back(path);
			}
		}
		
		fclose((fpTerrain));
		
		return true;
	}
	catch (...)
	{
		ASSERT(FALSE);
		fclose(fpTerrain);
		a_LogOutput(1,"CECTerrainPreprocessor::LoadTrn2, Failed to load terrain file %s", szFile);
		Release();
		return false;
	}	
}

//	Write string to file
bool CECTerrainPreprocessor::WriteStringLine(FILE* fp, const char* str)
{
	ASSERT(str);
	
	//	Write length of string
	int iLen = strlen(str);
	fwrite(&iLen, 1, sizeof (int), fp);
	
	//	Write string data
	if (iLen)
		fwrite(str, 1, iLen, fp);
	
	return true;
}

void CECTerrainPreprocessor::SaveFile(const char* szMapName)
{
	ASSERT((int)subTernInfo.size() == fileHeader.iNumSubTrn);
	if((int)subTernInfo.size() != fileHeader.iNumSubTrn)
	{
		a_LogOutput(1,"CECTerrainPreprocessor::SaveFile，%s 子地图个数错误", szMapName);
		return;
	}
	
	//Create terrain description file
	//	char szFile[MAX_PATH] = {0};
	char szFileTemp[MAX_PATH] = {0};
	//	sprintf(szFile, "maps\\%s\\%s.trn2", szMapName,szMapName);

	sprintf(szFileTemp, "temp\\maps\\%s\\%s.trn2",szMapName, szMapName);
	
	FILE* fpTerrain = NULL;
	
	if (!(fpTerrain = fopen(szFileTemp, "wb+")))
	{
		a_LogOutput(1,"CECTerrainPreprocessor::SaveTrn2, Failed to open terrain file %s", szFileTemp);
		//	return false;
	}
	
	fwrite(&fileVer,1,sizeof(TRN2FILEIDVER),fpTerrain);
	fwrite(&fileHeader,1,sizeof(TRN2FILEHEADER),fpTerrain);
	ASSERT((int)subTernInfo.size()==fileHeader.iNumSubTrn);
	for (int i=0;i<fileHeader.iNumSubTrn;i++)
	{
		fwrite(&subTernInfo[m_pAsset->GetNewSubTernIndex(i)],1,sizeof(TRN2FILESUBTRN),fpTerrain);
	}
	
	for (i=0;i<fileHeader.iNumTexture;i++)
	{
		WriteStringLine(fpTerrain,textureFile[i]);
	}
	fwrite(&loaderInfo,1,sizeof(A3DTRN2LOADERB::TRN2FILELOADERINFO),fpTerrain);
	
	for (i=0;i<fileHeader.iNumSubTrn;i++)
	{
		WriteStringLine(fpTerrain,subTernName[m_pAsset->GetNewSubTernIndex(i)]);
		//	WriteStringLine(fpTerrain,subTernName[i]);
	}
	
	fclose(fpTerrain);
}

//////////////////////////////////////////////////////////////////////////

void CECTernBlockPreprocessor::ConvertMaskAreaIdx(int iMaskArea, int& iSubTrn, int& iIdxInSubTrn)
{
	//	ASSERT(iMaskArea >= 0 && iMaskArea < m_pt->m_iNumMaskArea);
	
	//	Mask area row and column in whole terrain
	int iMaskPitch = m_pAsset->GetTrn2Info().iTrnBlkCol * m_pAsset->GetTrn2Info().iBlockGrid / m_pAsset->GetTrn2Info().iMaskGrid;
	int r = iMaskArea / iMaskPitch;
	int c = iMaskArea % iMaskPitch;
	
	//	Mask area row and column in sub-terrain
	int iSubTrnMask = m_pAsset->GetTrn2Info().iSubTrnGrid / m_pAsset->GetTrn2Info().iMaskGrid;
	int iSubRow = r % iSubTrnMask;
	int iSubCol = c % iSubTrnMask;
	
	//	Which sub-terrain this block is in ?
	int iRow = r / iSubTrnMask;
	int iCol = c / iSubTrnMask;
	iSubTrn = iRow * (m_pAsset->GetTrn2Info().iTrnBlkCol * m_pAsset->GetTrn2Info().iBlockGrid / m_pAsset->GetTrn2Info().iSubTrnGrid) + iCol;
	
	iIdxInSubTrn = iSubRow * iSubTrnMask + iSubCol;
}
bool CECTernBlockPreprocessor::ProcessFile(const char* szMapName,int fileIndex)
{
	Release();
	
	//	char szSrcFile[MAX_PATH] = {0};
	char szFile[MAX_PATH] = {0};
	//sprintf(szSrcFile, "maps\\%s\\%s_%d.t2bk", szMapName,szMapName,fileIndex);
	sprintf(szFile, "temp\\maps\\%s\\%s_%d.t2bk", szMapName,szMapName,fileIndex+1);
	
	
	FILE* fpBlock = NULL;
	
	try
	{
		if (!(fpBlock = fopen(szFile, "rb+")))
		{
			a_LogOutput(1,"CECTernBlockPreprocessor::LoadFile, Failed to open block file %s", szFile);
			return false;
		}
		
		if (fread(&fileVer,1,sizeof(TRN2FILEIDVER),fpBlock) != sizeof(TRN2FILEIDVER))
			throw 1;
		if (fread(&fileHeader,1,sizeof(A3DTRN2LOADERB::T2BKFILEHEADER5),fpBlock) != sizeof(A3DTRN2LOADERB::T2BKFILEHEADER5))
			throw 1;
		
		blockOffset.reserve(fileHeader.iNumBlock);
		for (int i=0;i<fileHeader.iNumBlock;i++)
		{
			DWORD off;
			if(fread(&off,1,sizeof(DWORD),fpBlock)!= sizeof(DWORD))
				throw 1;
			
			blockOffset.push_back(off);
		}		
		
		int iMaskBlock = m_pAsset->GetTrn2Info().iMaskGrid / m_pAsset->GetTrn2Info().iBlockGrid;
		
		//	Calculate index of the mask area which this block blongs to
		int iMaskPitch = m_pAsset->GetTrn2Info().iTrnBlkCol * m_pAsset->GetTrn2Info().iBlockGrid / m_pAsset->GetTrn2Info().iMaskGrid;

		for (i=0;i<fileHeader.iNumBlock;i++)
		{
			fseek(fpBlock, blockOffset[i], SEEK_SET);
			A3DTRN2LOADERB::T2BKFILEBLOCK6 bk;
			if(fread(&bk,1,sizeof(bk),fpBlock)!= sizeof(bk))
				throw 1;
	//		int a,b;
	//		ConvertMaskAreaIdx(bk.iMaskArea,a,b);
			
	//		int oldSub,newSub;
	//		int oldRow = bk.iRowInTrn;
	//		int oldCol = bk.iColInTrn;
			int iDummy;
		
	////		m_pAsset->GetNewRowColInWorld(bk.iRowInTrn,bk.iColInTrn,iDummy);
			m_pAsset->GetNewBlockRowColInWorld(fileIndex,bk.iRowInTrn,bk.iColInTrn);
					
			int iMaskRow = bk.iRowInTrn / iMaskBlock;
			int iMaskCol = bk.iColInTrn / iMaskBlock;		
			
			bk.iMaskArea = iMaskRow * iMaskPitch + iMaskCol;
			
	//		ConvertMaskAreaIdx(bk.iMaskArea,a,b);
			
//			ASSERT(oldSub == fileIndex);
		//	ASSERT(newSub == m_pAsset->GetNewSubTernIndexByOldIndex(fileIndex));
			
	//		fprintf(fp,"%d %d %d %d %d %d\n",fileIndex,newSub,oldRow,oldCol,bk.iRowInTrn,bk.iColInTrn);
			fseek(fpBlock, blockOffset[i], SEEK_SET);
			if(fwrite(&bk,1,sizeof(bk),fpBlock)!=sizeof(bk))
				throw 1;
		}
		
		fclose(fpBlock);
		
		return true;
	}
	catch(...)
	{
		ASSERT(FALSE);
		fclose(fpBlock);
	}
	
	return false;
}

void CECTernBlockPreprocessor::Release()
{
	memset(&fileVer,0,sizeof(fileVer));
	memset(&fileHeader,0,sizeof(fileHeader));
	blockOffset.clear();
}
void CECTernBlockPreprocessor::DoProcess(const char* szMapName)
{	
	for (int i=0;i<m_pAsset->GetSubTernCount();i++)
	{
		ProcessFile(szMapName,i);
	}
	
	for (i=0;i<m_pAsset->GetSubTernCount();i++)
	{
		CECStichSubTern sticher(m_pAsset,i);
		sticher.Stich();
	}	
}
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
void CECWorldFilePreprocessor::Release()
{
	memset(&fileHeader,0,sizeof(ECWDFILEHEADER));
	blockOffset.RemoveAll();

	blockInSubTrn = 0;				
	subTrnRowCount = 0;
	subTrnColCount = 0;

	tempOffset.RemoveAll();
}
void CECWorldFilePreprocessor::DoProcess(const char* szMapName)
{
	return;
	LoadFile(szMapName);
}
int CECWorldFilePreprocessor::GetNewBlockIndexInWorldByOldIndex(int index)
{
	//	row = index / GetColCountSubTern(); 
	//	col = index % GetColCountSubTern();	
	int rowInWhole = index / fileHeader.iWorldBlkCol;
	int colInWhole = index % fileHeader.iWorldBlkCol;

	int rowInSub = rowInWhole % blockInSubTrn;
	int colInSub = colInWhole % blockInSubTrn;

	int rowSubTrn = rowInWhole / blockInSubTrn;
	int colSubTrn = colInWhole / blockInSubTrn;

	int oldSubTernIndex = rowSubTrn * subTrnColCount + colSubTrn;
	int newSubTernIndex = m_pAsset->GetNewSubTernIndexByOldIndex(oldSubTernIndex);

	rowSubTrn = newSubTernIndex / subTrnColCount;
	colSubTrn = newSubTernIndex % subTrnColCount;

	
	rowInWhole = rowInSub + rowSubTrn * blockInSubTrn;
	colInWhole = colInSub + colSubTrn * blockInSubTrn;

	return rowInWhole * fileHeader.iWorldBlkCol + colInWhole;
}

bool CECWorldFilePreprocessor::LoadFile(const char* szMapName)
{
	// clear info
	Release();
	
	//Create terrain description file
	char szFile[MAX_PATH] = {0};
	sprintf(szFile, "maps\\%s\\%s.ecwld", szMapName,szMapName);
	
	FILE* fpWorld = NULL;
	
	if (!(fpWorld = fopen(szFile, "rb+")))
	{
		a_LogOutput(1,"CECTerrainPreprocessor::loadECWLD, Failed to open world file %s", szFile);
		return false;
	}
	
	try
	{
		if(fread(&fileHeader,1,sizeof(ECWDFILEHEADER),fpWorld)!=sizeof(ECWDFILEHEADER))
			throw 1;
		
		blockInSubTrn = m_pAsset->GetTrn2Info().iSubTrnGrid * m_pAsset->GetTrn2Info().fGridSize / fileHeader.fBlockSize;
				
		subTrnRowCount = fileHeader.iWorldBlkRow / blockInSubTrn;
		subTrnColCount = fileHeader.iWorldBlkCol / blockInSubTrn;

		int pos = ftell(fpWorld);

		blockOffset.SetSize(fileHeader.iNumBlock, 100);
		fread(blockOffset.GetData(), fileHeader.iNumBlock ,sizeof (DWORD), fpWorld);		
		
		tempOffset.SetSize(fileHeader.iNumBlock,100);
		memset(tempOffset.GetData(),0,sizeof(DWORD)*tempOffset.GetSize());

		for (int i=0;i<fileHeader.iNumBlock;i++)
		{
			int newIdx = GetNewBlockIndexInWorldByOldIndex(i);
			ASSERT(newIdx<fileHeader.iNumBlock);
			ASSERT(tempOffset[newIdx]==0);
			tempOffset[newIdx] = blockOffset[i];
		}
		for (i=0;i<fileHeader.iNumBlock;i++)
		{
			ASSERT(tempOffset[i]);
		}
		fseek(fpWorld,pos,SEEK_SET);

		ASSERT(tempOffset.GetSize()==fileHeader.iNumBlock);

		fwrite(tempOffset.GetData(),fileHeader.iNumBlock,sizeof(DWORD),fpWorld);

		tempOffset.RemoveAll(false);

		//
	
		fclose(fpWorld);
		
		return true;
	}
	catch(...)
	{
		ASSERT(FALSE);
		fclose(fpWorld);
		return false;
	}
}
float CECWorldFilePreprocessor::GetSubTernSize()
{
	return m_pAsset->GetTrn2Info().iSubTrnGrid * m_pAsset->GetTrn2Info().fGridSize;
}
void CECWorldFilePreprocessor::GetOffsetByRowCol(int row,int col,float& cx,float& cz)
{
	GetSubTernByBlockRowCol(row,col);
	float subTrnSize = GetSubTernSize();
	cx = -m_pAsset->GetTrn2Info().fTerrainWid * 0.5f + col * subTrnSize + subTrnSize * 0.5f;
	cz = m_pAsset->GetTrn2Info().fTerrainHei * 0.5f - row * subTrnSize - subTrnSize * 0.5f;
}
int CECWorldFilePreprocessor::GetSubTernColCount()
{
	return m_pAsset->GetTrn2Info().iTrnBlkCol / GetBlockCountInSubTern();
}
int CECWorldFilePreprocessor::GetSubTernRowCount()
{
	return m_pAsset->GetTrn2Info().iTrnBlkRow / GetBlockCountInSubTern();
}
void CECWorldFilePreprocessor::GetSubTernByBlockRowCol(int& r,int& c)
{
	int blockCountInSubTern = GetBlockCountInSubTern();
	
	r = r / blockCountInSubTern;
	c = c / blockCountInSubTern;
}
int CECWorldFilePreprocessor::GetBlockCountInSubTern()
{
	float l = GetSubTernSize() / m_pAsset->GetTrn2Info().fBlockSize;
	return (int)l;
}
int CECWorldFilePreprocessor::GetNewSubTernIndexByOldIndex(int old)
{
	return m_pAsset->GetNewSubTernIndexByOldIndex(old);
}
void CECWorldFilePreprocessor::LoadTree(FILE* fpWorld)
{
	//	Read block information
	int iTrees = 0;
	int iRowInWorld,iColInWorld;

	if (fileHeader.dwVersion < 4)
	{
		ECWDFILEBLOCK Info;
	
		fread(&Info,1,sizeof(Info),fpWorld);
		iTrees = Info.iNumTree;
		iRowInWorld = Info.iRowInWorld;
		iColInWorld = Info.iColInWorld;
	}
	else if (fileHeader.dwVersion < 7)
	{
		ECWDFILEBLOCK4 Info;
		fread(&Info,1,sizeof(Info),fpWorld);
		iTrees = Info.iNumTree;
		iRowInWorld = Info.iRowInWorld;
		iColInWorld = Info.iColInWorld;
	}
	else if (fileHeader.dwVersion < 9)
	{
		ECWDFILEBLOCK7 Info;
		fread(&Info,1,sizeof(Info),fpWorld);
		iTrees = Info.iNumTree;
		iRowInWorld = Info.iRowInWorld;
		iColInWorld = Info.iColInWorld;
	}
	else	//	m_pScene->GetSceneFileVersion() >= 9
	{
		ECWDFILEBLOCK9 Info;
		fread(&Info,1,sizeof(Info),fpWorld);
		iTrees = Info.iNumTree;
		iRowInWorld = Info.iRowInWorld;
		iColInWorld = Info.iColInWorld;
	}

	float m_fOffsetXForRandomMap,m_fOffsetZForRandomMap;

	GetNewPosOffset(iRowInWorld,iColInWorld,m_fOffsetXForRandomMap,m_fOffsetZForRandomMap);

	A3DVECTOR3 vPos;

	if (fileHeader.dwVersion < 5)
	{
		ECWDFILETREE TreeInfo;

		for (int i=0; i < iTrees; i++)
		{

			fread(&TreeInfo,1,sizeof(TreeInfo),fpWorld);

			//	Add scene offset to tree's position
			
			vPos.x	= TreeInfo.vPos[0] + m_fOffsetXForRandomMap;
			vPos.z	= TreeInfo.vPos[1] + m_fOffsetZForRandomMap;
			vPos.y	= 0;
			
		
		}
	}
	else	//	Version >= 5
	{
		ECWDFILETREE5 TreeInfo;

		for (int i=0; i < iTrees; i++)
		{
			fread(&TreeInfo,1,sizeof(TreeInfo),fpWorld);

			//	Add scene offset to tree's position
			vPos.x	= TreeInfo.vPos[0] + m_fOffsetXForRandomMap;
			vPos.y	= TreeInfo.vPos[1];
			vPos.z	= TreeInfo.vPos[2] + m_fOffsetZForRandomMap;		
		}
	}

}
//////////////////////////////////////////////////////////////////////////


bool CECBlockSticher::GetBlockDataByRowCol(int r,int c,CECBlockSticher::BKINFO& info)
{
	for (int i=0;i<blockData.size();i++)
	{
		if (blockData[i].iRowInTrn == r && blockData[i].iColInTrn == c)
		{
			info = blockData[i];
			return true;
		}
	}
	return false;
}
void CECBlockSticher::StitchBottomBorder(CECBlockSticher* pNeighbor)
{
	for (int i=0;i<GetBorderBlockCount(BOTTOM);i++)
	{
		BKINFO info = blockData[GetBorderBlockIndex(BOTTOM,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn+1,info.iColInTrn,neighbourBK);
		assert(bFind);

		if (bFind)
		{
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)
			{
				info.pHeiData[(s - 1)*s + j] = neighbourBK.pHeiData[j];				
			}
		}
	}
}
void CECBlockSticher::StitchRightBorder(CECBlockSticher* pNeighbor)
{
	for (int i=0;i<GetBorderBlockCount(RIGHT);i++)
	{
		BKINFO info = blockData[GetBorderBlockIndex(RIGHT,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn,info.iColInTrn+1,neighbourBK);
		assert(bFind);
		
		if (bFind)
		{
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)
			{
				info.pHeiData[j*s + s - 1] = neighbourBK.pHeiData[j*s];	
			}
		}
	}
}
void CECBlockSticher::StitchLeftBorder(CECBlockSticher* pNeighbor)
{
	for (int i=0;i<GetBorderBlockCount(LEFT);i++)
	{
		BKINFO info = blockData[GetBorderBlockIndex(LEFT,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn,info.iColInTrn-1,neighbourBK);
		assert(bFind);

		if (bFind)
		{
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)
			{
				info.pHeiData[j * s] = neighbourBK.pHeiData[j*s+s-1];	
			}
		}
	}
}
void CECBlockSticher::StitchTopBorder(CECBlockSticher* pNeighbor)
{
	for (int i=0;i<GetBorderBlockCount(TOP);i++)
	{
		BKINFO info = blockData[GetBorderBlockIndex(TOP,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn-1,info.iColInTrn,neighbourBK);
		assert(bFind);
		
		if (bFind)
		{
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)
			{
				info.pHeiData[j] = neighbourBK.pHeiData[(s-1)*s+j];		
			}
		}
	}
}
void CECBlockSticher::StitchCorner(int corner,CECBlockSticher* pNeighbor)
{
	BKINFO info = blockData[GetCornerBlockIndex(corner)];
	BKINFO neighbourBK;
	switch (corner)
	{
	case LEFT_TOP:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn-1,info.iColInTrn-1,neighbourBK);
			assert(bFind);
			if (bFind)
			{
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[0] = neighbourBK.pHeiData[s*s - 1];
			}
		}
		break;
	case RIGHT_TOP:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn-1,info.iColInTrn+1,neighbourBK);
			assert(bFind);
			if (bFind)
			{
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[s-1] = neighbourBK.pHeiData[(s - 1)*s];
			}
		}
		break;
	case RIGHT_BOTTOM:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn+1,info.iColInTrn+1,neighbourBK);
			assert(bFind);
			if (bFind)
			{
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[s*s-1] = neighbourBK.pHeiData[0];
			}
		}
		break;
	case LEFT_BOTTOM:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn+1,info.iColInTrn-1,neighbourBK);
			assert(bFind);
			if (bFind)
			{
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[(s - 1)*s] = neighbourBK.pHeiData[s - 1];
			}
		}
		break;
	}
}
template<typename T>
void BlockHeightDataCompress(T& FileBlock,BYTE blockCompFlag,int iNumVert,const CECBlockSticher::BKINFO& bkInfo,BYTE* pOutBuf,FILE* fpBlock)
{
//	A3DTRN2LOADERB::T2BKFILEBLOCK6 FileBlock = blockData[i].bk;
	//	Compress height data ----------------------------
	if ((blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP) || (blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_INC))
	{
		if (!(blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_INC))
		{	//	ZIP compress only
			DWORD dwSrcSize = iNumVert * sizeof (float);
			int v = AFilePackage::Compress((BYTE*)bkInfo.pHeiData, dwSrcSize, pOutBuf, &dwSrcSize);
			ASSERT(v==0);
		//	ASSERT(FileBlock.dwHeiSize == dwSrcSize);
			FileBlock.dwHeiSize = dwSrcSize;
		}
		else if (!(blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP))
		{	//	Incremental compress only
			DWORD dwSrcSize;
			bool v = A3DTerrain2::CompressHeightMap(bkInfo.pHeiData, iNumVert, pOutBuf, &dwSrcSize);
			ASSERT(v);
		//	ASSERT(FileBlock.dwHeiSize == dwSrcSize);
			FileBlock.dwHeiSize = dwSrcSize;
		}
		else
		{	//	Incremental compress and then ZIP compress
			DWORD dwSrcSize;
			BYTE* pTempHei = new BYTE [iNumVert * 2 * sizeof (float)];
			bool v = A3DTerrain2::CompressHeightMap(bkInfo.pHeiData, iNumVert, pTempHei, &dwSrcSize);
			ASSERT(v);
			int c = AFilePackage::Compress(pTempHei, dwSrcSize, pOutBuf, &dwSrcSize);
			ASSERT(c==0);
		//	ASSERT(FileBlock.dwHeiSize == dwSrcSize);
			FileBlock.dwHeiSize = dwSrcSize;
			delete [] pTempHei;	
		}
	}
	else	//	No compression
	{
		FileBlock.dwHeiSize = iNumVert * sizeof (float);
		memcpy(pOutBuf, bkInfo.pHeiData, FileBlock.dwHeiSize);
	}
	
//	fwrite(&FileBlock, 1, sizeof (FileBlock), fpBlock);
//	fwrite(pOutBuf, 1, FileBlock.dwHeiSize, fpBlock);
}
void CECBlockSticher::SaveFile()
{
	char szFile[MAX_PATH] = {0};

	sprintf(szFile, "temp\\maps\\%s\\%s_%d.t2bk", m_strMapName,m_strMapName,m_iFileIndex+1);
	
	FILE* fpBlock = NULL;
	if (!(fpBlock = fopen(szFile, "rb+")))
	{
		a_LogOutput(1,"CECStichBolckProcessor::SaveFile, Failed to open block file %s", szFile);
		ASSERT(FALSE);
		return;
	}

	int iNumVert = (m_pAsset->GetTrn2Info().iBlockGrid + 1) * (m_pAsset->GetTrn2Info().iBlockGrid + 1);
	

	BYTE* m_pCompHei = new BYTE [iNumVert * sizeof (float) * 2];
	
	abase::vector<DWORD> newOffset;

	fseek(fpBlock, blockOffset[0], SEEK_SET);

	for (int i=0;i<fileHeader.iNumBlock;i++)
	{
		memset(m_pCompHei,0,sizeof(BYTE)*(iNumVert * sizeof (float) * 2));

		newOffset.push_back(ftell(fpBlock));

		if (fileVer.dwVersion<3)
		{
			A3DTRN2LOADERB::T2BKFILEBLOCK2 bk;
			
			bk.iColInTrn = blockData[i].iColInTrn;
			bk.iRowInTrn = blockData[i].iRowInTrn;
			bk.iMaskArea = blockData[i].iMaskArea;
			bk.fLODScale = blockData[i].fLODScale;
			bk.dwLayerFlags = blockData[i].dwLayerFlags;
			bk.dwHeiSize = blockData[i].dwHeiSize;
			bk.dwNormalSize = blockData[i].dwNormalSize;
			
			BlockHeightDataCompress(bk,m_byBlockComp,iNumVert,blockData[i],m_pCompHei,fpBlock);

			fwrite(&bk, 1, sizeof(bk), fpBlock);
			fwrite(m_pCompHei, 1, bk.dwHeiSize, fpBlock);
			fwrite(blockData[i].pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);		
			
		}
		else if (fileVer.dwVersion<4)
		{
			A3DTRN2LOADERB::T2BKFILEBLOCK3 bk;
			
			bk.iColInTrn = blockData[i].iColInTrn;
			bk.iRowInTrn = blockData[i].iRowInTrn;
			bk.iMaskArea = blockData[i].iMaskArea;
			bk.fLODScale = blockData[i].fLODScale;
			bk.dwLayerFlags = blockData[i].dwLayerFlags;
			bk.dwHeiSize = blockData[i].dwHeiSize;
			bk.dwNormalSize = blockData[i].dwNormalSize;
			bk.dwDiffSize = blockData[i].dwDiffSize;			

			BlockHeightDataCompress(bk,m_byBlockComp,iNumVert,blockData[i],m_pCompHei,fpBlock);

			fwrite(&bk, 1, sizeof(bk), fpBlock);
			fwrite(m_pCompHei, 1, bk.dwHeiSize, fpBlock);
			fwrite(blockData[i].pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);
			fwrite(blockData[i].pDiffuse,bk.dwDiffSize,sizeof(BYTE),fpBlock);

		}
		else if (fileVer.dwVersion<6)
		{
			A3DTRN2LOADERB::T2BKFILEBLOCK4 bk;
			bk.iColInTrn = blockData[i].iColInTrn;
			bk.iRowInTrn = blockData[i].iRowInTrn;
			bk.iMaskArea = blockData[i].iMaskArea;
			bk.fLODScale = blockData[i].fLODScale;
			bk.dwLayerFlags = blockData[i].dwLayerFlags;
			bk.dwHeiSize = blockData[i].dwHeiSize;
			bk.dwNormalSize = blockData[i].dwNormalSize;
			bk.dwDiffSize = blockData[i].dwDiffSize;
			bk.dwDiffSize1 = blockData[i].dwDiffSize1;

			BlockHeightDataCompress(bk,m_byBlockComp,iNumVert,blockData[i],m_pCompHei,fpBlock);

			fwrite(&bk, 1, sizeof(bk), fpBlock);
			fwrite(m_pCompHei, 1, bk.dwHeiSize, fpBlock);
			fwrite(blockData[i].pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);
			fwrite(blockData[i].pDiffuse,bk.dwDiffSize,sizeof(BYTE),fpBlock);
			fwrite(blockData[i].pDiffuse1,bk.dwDiffSize1,sizeof(BYTE),fpBlock);
		}
		else
		{
			A3DTRN2LOADERB::T2BKFILEBLOCK6 bk;
		
			bk.iColInTrn = blockData[i].iColInTrn;
			bk.iRowInTrn = blockData[i].iRowInTrn;
			bk.iMaskArea = blockData[i].iMaskArea;
			bk.fLODScale = blockData[i].fLODScale;
			bk.dwLayerFlags = blockData[i].dwLayerFlags;
			bk.dwHeiSize = blockData[i].dwHeiSize;
			bk.dwNormalSize = blockData[i].dwNormalSize;
			bk.dwDiffSize = blockData[i].dwDiffSize;
			bk.dwDiffSize1 = blockData[i].dwDiffSize1;
			bk.dwBlkFlags = blockData[i].dwBlkFlags;

			BlockHeightDataCompress(bk,m_byBlockComp,iNumVert,blockData[i],m_pCompHei,fpBlock);

			fwrite(&bk, 1, sizeof(bk), fpBlock);
			fwrite(m_pCompHei, 1, bk.dwHeiSize, fpBlock);
			fwrite(blockData[i].pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);
			fwrite(blockData[i].pDiffuse,bk.dwDiffSize,sizeof(BYTE),fpBlock);
			fwrite(blockData[i].pDiffuse1,bk.dwDiffSize1,sizeof(BYTE),fpBlock);
		}	
	}

	delete [] m_pCompHei;

	ASSERT(newOffset.size() == blockOffset.size());

	fseek(fpBlock,blockOffsetAddr,SEEK_SET);
	fwrite(newOffset.begin(),newOffset.size(),sizeof(DWORD),fpBlock);
	
	fclose(fpBlock);
}
template<typename T>
bool ProcessBlock(T& bk, int i,int iNumVert,CECBlockSticher* pBlockProc,FILE* fpBlock, float*& pOutBuf)
{
	BYTE m_byBlockComp = pBlockProc->GetCompressFlag();

	if(fread(&bk,1,sizeof(bk),fpBlock)!= sizeof(bk))
	{
		ASSERT(FALSE);
		return false;			
	}
	
	// read height data
	BYTE *pHeiData=NULL, *pHeiData2=NULL;
	if (m_byBlockComp)
	{
		pHeiData = new BYTE[iNumVert * sizeof (float) * 2];			
		pHeiData2 = new BYTE[iNumVert * sizeof (float)];				
		ASSERT(pHeiData && pHeiData2);
	}
	
	float * aTempHei = NULL;
	if (!(aTempHei = new float[iNumVert]))
	{
		ASSERT(FALSE);
		return false;
	}
	
	//	Load block height data ...
	if ((m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP) || (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_INC))
	{
		//	Load block height data
		fread(pHeiData, bk.dwHeiSize, sizeof(BYTE),fpBlock);
		
		if (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_INC)
		{
			if (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP)
			{
				DWORD dwSize = iNumVert * sizeof (float);
				AFilePackage::Uncompress(pHeiData, bk.dwHeiSize, (BYTE*)pHeiData2, &dwSize);
				dwSize = A3DTerrain2::DecompressHeightMap(pHeiData2, aTempHei);
				ASSERT(dwSize == iNumVert * sizeof (float));
			}
			else
			{
				DWORD dwSize = A3DTerrain2::DecompressHeightMap(pHeiData, aTempHei);
				ASSERT(dwSize == iNumVert * sizeof (float));						
			}
		}
		else 
		{
			ASSERT( (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP) && !(m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_INC));
			DWORD dwSize = iNumVert * sizeof (float);
			int v = AFilePackage::Uncompress(pHeiData, bk.dwHeiSize, (BYTE*)aTempHei, &dwSize);
			ASSERT(v==0);
			ASSERT(dwSize == iNumVert * sizeof (float));
			
		}
	}
	else	//	No compression
	{
		//	Load block height data
		DWORD dwSize = fread(aTempHei, bk.dwHeiSize, sizeof(BYTE),fpBlock);
		ASSERT(dwSize == iNumVert * sizeof (float));
	}
	
	int iRowBlockPerSubTern = pBlockProc->GetAssetMan()->GetBlockRowCountInSubTern();
	int iColBlockPerSubTern = pBlockProc->GetAssetMan()->GetBlockColCountInSubTern();
	
	bool isTop = bk.iRowInTrn % iRowBlockPerSubTern == 0;
	bool isBottom = (bk.iRowInTrn+1) % iRowBlockPerSubTern == 0;
	bool isLeft = bk.iColInTrn % iColBlockPerSubTern == 0;
	bool isRight = (bk.iColInTrn+1) % iColBlockPerSubTern == 0;
	
	if(isTop)
		pBlockProc->AddBorder(CECBlockSticher::TOP,i);
	if(isBottom)
		pBlockProc->AddBorder(CECBlockSticher::BOTTOM,i);
	if(isLeft)
		pBlockProc->AddBorder(CECBlockSticher::LEFT,i);
	if(isRight)
		pBlockProc->AddBorder(CECBlockSticher::RIGHT,i);
	
	if (isLeft && isTop)
		pBlockProc->SetCorner(CECBlockSticher::LEFT_TOP,i);		
	else if (isRight && isTop)
		pBlockProc->SetCorner(CECBlockSticher::RIGHT_TOP,i);		
	else if (isRight && isBottom)
		pBlockProc->SetCorner(CECBlockSticher::RIGHT_BOTTOM,i);		
	else if (isLeft && isBottom)
		pBlockProc->SetCorner(CECBlockSticher::LEFT_BOTTOM,i);		
	
	if(pHeiData)
		delete [] pHeiData;
	if(pHeiData2)
		delete [] pHeiData2;
	//
	
/*	CECBlockSticher::BKINFO info;
	info.iColInTrn = bk.iColInTrn;
	info.iRowInTrn = bk.iRowInTrn;
	info.pHeiData = aTempHei;
	pBlockProc->AddBlockInfo(info);*/
	pOutBuf = aTempHei;

	return true;
}
bool CECBlockSticher::LoadFile(const char* szMapName,int fileIndex)
{
	Release();
	
	char szFile[MAX_PATH] = {0};
	sprintf(szFile, "temp\\maps\\%s\\%s_%d.t2bk", szMapName,szMapName,fileIndex+1);
	
	m_iFileIndex = fileIndex;
	m_strMapName = szMapName;
	
	FILE* fpBlock = NULL;
	
	try
	{
		if (!(fpBlock = fopen(szFile, "rb+")))
		{
			a_LogOutput(1,"CECBlockSticher::LoadFile, Failed to open block file %s", szFile);
			return false;
		}
		
		if (fread(&fileVer,1,sizeof(TRN2FILEIDVER),fpBlock) != sizeof(TRN2FILEIDVER))
			throw 1;

		if (fileVer.dwVersion < 2)
		{
			//	Read file header
			A3DTRN2LOADERB::T2BKFILEHEADER Header;
			fread(&Header,1,sizeof(Header),fpBlock);
			
			//iNumBlock		= Header.iNumBlock;
			m_byBlockComp	= 0;

			fileHeader.byCompressed = m_byBlockComp;
			fileHeader.iNumBlock = Header.iNumBlock;
		}
		else if (fileVer.dwVersion < 5)	//	IdVer.dwVersion >= 2 && < 5
		{
			//	Read file header
			A3DTRN2LOADERB::T2BKFILEHEADER2 Header;
			fread(&Header,1,sizeof(Header),fpBlock);
						
		//	iNumBlock		= Header.iNumBlock;
			m_byBlockComp	= Header.bCompressed ? (A3DTRN2LOADERB::T2BKCOMP_COL_ZIP | A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP | A3DTRN2LOADERB::T2BKCOMP_NOR_ZIP) : 0;

			fileHeader.byCompressed = m_byBlockComp;
			fileHeader.iNumBlock = Header.iNumBlock;
		}
		else	//	IdVer.dwVersion >= 5
		{
			//	Read file header
			fread(&fileHeader,1,sizeof(A3DTRN2LOADERB::T2BKFILEHEADER5),fpBlock);			
		//	iNumBlock		= Header.iNumBlock;
			m_byBlockComp	= fileHeader.byCompressed;
		}
		
	//	blockOffset.reserve(fileHeader.iNumBlock);
		blockOffsetAddr = ftell(fpBlock);

		for (int i=0;i<fileHeader.iNumBlock;i++)
		{
			DWORD off;
			if(fread(&off,1,sizeof(DWORD),fpBlock)!= sizeof(DWORD))
				throw 1;
			
			blockOffset.push_back(off);
		}

		int iNumVert = (m_pAsset->GetTrn2Info().iBlockGrid + 1) * (m_pAsset->GetTrn2Info().iBlockGrid + 1);

		for (i=0;i<fileHeader.iNumBlock;i++)
		{
			fseek(fpBlock, blockOffset[i], SEEK_SET);

			float* pOut;
			if (fileVer.dwVersion<3)
			{
				A3DTRN2LOADERB::T2BKFILEBLOCK2 bk;
				ProcessBlock(bk,i,iNumVert,this,fpBlock,pOut);

				BKINFO info;
				info.iColInTrn = bk.iColInTrn;
				info.iRowInTrn = bk.iRowInTrn;
				info.iMaskArea = bk.iMaskArea;
				info.fLODScale = bk.fLODScale;
				info.dwLayerFlags = bk.dwLayerFlags;
				info.dwHeiSize = bk.dwHeiSize;
				info.dwNormalSize = bk.dwNormalSize;
				info.dwDiffSize = 0;
				info.dwDiffSize1 = 0;
				info.dwBlkFlags = 0;	

				info.pHeiData = pOut;

				info.pNormal = new BYTE[bk.dwNormalSize];
				fread(info.pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);

				AddBlockInfo(info);
			}
			else if (fileVer.dwVersion<4)
			{
				A3DTRN2LOADERB::T2BKFILEBLOCK3 bk;
				ProcessBlock( bk,i,iNumVert,this,fpBlock,pOut);

				BKINFO info;
				info.iColInTrn = bk.iColInTrn;
				info.iRowInTrn = bk.iRowInTrn;
				info.iMaskArea = bk.iMaskArea;
				info.fLODScale = bk.fLODScale;
				info.dwLayerFlags = bk.dwLayerFlags;
				info.dwHeiSize = bk.dwHeiSize;
				info.dwNormalSize = bk.dwNormalSize;
				info.dwDiffSize = bk.dwDiffSize;
				info.dwDiffSize1 = 0;
				info.dwBlkFlags = 0;	


				info.pHeiData = pOut;
				
				info.pNormal = new BYTE[bk.dwNormalSize];
				fread(info.pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);

				info.pDiffuse = new BYTE[bk.dwDiffSize];
				fread(info.pDiffuse,bk.dwDiffSize,sizeof(BYTE),fpBlock);

				AddBlockInfo(info);
			}
			else if (fileVer.dwVersion<6)
			{
				A3DTRN2LOADERB::T2BKFILEBLOCK4 bk;
				ProcessBlock( bk,i,iNumVert,this,fpBlock,pOut);

				BKINFO info;
				info.iColInTrn = bk.iColInTrn;
				info.iRowInTrn = bk.iRowInTrn;
				info.iMaskArea = bk.iMaskArea;
				info.fLODScale = bk.fLODScale;
				info.dwLayerFlags = bk.dwLayerFlags;
				info.dwHeiSize = bk.dwHeiSize;
				info.dwNormalSize = bk.dwNormalSize;
				info.dwDiffSize = bk.dwDiffSize;
				info.dwDiffSize1 = bk.dwDiffSize1;
				info.dwBlkFlags = 0;	

				info.pHeiData = pOut;
				
				info.pNormal = new BYTE[bk.dwNormalSize];
				fread(info.pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);
				
				info.pDiffuse = new BYTE[bk.dwDiffSize];
				fread(info.pDiffuse,bk.dwDiffSize,sizeof(BYTE),fpBlock);

				info.pDiffuse1 = new BYTE[bk.dwDiffSize1];
				fread(info.pDiffuse1,bk.dwDiffSize1,sizeof(BYTE),fpBlock);
				
				AddBlockInfo(info);
			}
			else
			{
				A3DTRN2LOADERB::T2BKFILEBLOCK6 bk;
				ProcessBlock( bk,i,iNumVert,this,fpBlock,pOut);

				BKINFO info;
				info.iColInTrn = bk.iColInTrn;
				info.iRowInTrn = bk.iRowInTrn;
				info.iMaskArea = bk.iMaskArea;
				info.fLODScale = bk.fLODScale;
				info.dwLayerFlags = bk.dwLayerFlags;
				info.dwHeiSize = bk.dwHeiSize;
				info.dwNormalSize = bk.dwNormalSize;
				info.dwDiffSize = bk.dwDiffSize;
				info.dwDiffSize1 = bk.dwDiffSize1;
				info.dwBlkFlags = bk.dwBlkFlags;	

				info.pHeiData = pOut;
				
				info.pNormal = new BYTE[bk.dwNormalSize];
				fread(info.pNormal,bk.dwNormalSize,sizeof(BYTE),fpBlock);
				
				info.pDiffuse = new BYTE[bk.dwDiffSize];
				fread(info.pDiffuse,bk.dwDiffSize,sizeof(BYTE),fpBlock);

				info.pDiffuse1 = new BYTE[bk.dwDiffSize1];
				fread(info.pDiffuse1,bk.dwDiffSize1,sizeof(BYTE),fpBlock);
				
				AddBlockInfo(info);
			}	
		}
		
		fclose(fpBlock);
		
		return true;
	}
	catch(...)
	{
		ASSERT(FALSE);
		fclose(fpBlock);
	}
	
	return false;
}

void CECBlockSticher::Release()
{
	memset(&fileVer,0,sizeof(fileVer));
	memset(&fileHeader,0,sizeof(fileHeader));
	blockOffset.clear();
	for (unsigned int i=0;i<blockData.size();i++)
	{
		delete [] blockData[i].pHeiData;
		delete [] blockData[i].pNormal;
		delete [] blockData[i].pDiffuse;
		delete [] blockData[i].pDiffuse1;
	}
	blockData.clear();	
}

CECStichSubTern::CECStichSubTern(CECRandomMapProcess* asset,int iSubTern):m_iSubTern(iSubTern),m_pAsset(asset),currentSubTern(asset)
{
	for (int i=0;i<9;i++)
	{
		m_pNeighbourMap[i] = NULL;
	}
	m_iAlignType = 1;
}
void CECStichSubTern::Release()
{
	for (int i=0;i<9;i++)
	{
		if(m_pNeighbourMap[i])
		{
			m_pNeighbourMap[i]->Release();
			delete m_pNeighbourMap[i];
		}
	}
}
// Get 8 neighbor information of project list /XQF
bool CECStichSubTern::GetProjectListInfor(int currentMapIndex, int mapCount,int pNeiIndices[])
{
	DWORD dwNumCol = m_pAsset->GetTrn2Info().iSubTrnCol;
	int   nCurrent = currentMapIndex;
	
	// 0 1 2
	// 3 4 5
	// 6 7 8
	if((nCurrent+1)/dwNumCol <= 0 || (nCurrent+1)%dwNumCol==1) pNeiIndices[0] = -1;
	else pNeiIndices[0] = (nCurrent - dwNumCol) - 1;
	
	if((nCurrent+1)/dwNumCol <= 0) pNeiIndices[1] = -1;
	else pNeiIndices[1] = (nCurrent - dwNumCol);
	
	if((nCurrent+1)%dwNumCol==0 || (nCurrent+1)/dwNumCol <= 0) pNeiIndices[2] = -1;
	else pNeiIndices[2] = (nCurrent - dwNumCol) + 1;
	
	if((nCurrent+1)%dwNumCol==1) pNeiIndices[3] = -1;
	else pNeiIndices[3] = (nCurrent) - 1;
	
	pNeiIndices[4] = (nCurrent);
	
	if((nCurrent+1)%dwNumCol==0) pNeiIndices[5] = -1;
	else pNeiIndices[5] = (nCurrent) + 1;
	
	if((nCurrent+1)%dwNumCol==1 || (nCurrent+1)/dwNumCol >= mapCount/dwNumCol) pNeiIndices[6] = -1;
	else pNeiIndices[6] = (nCurrent + dwNumCol) - 1;
	
	if((nCurrent+1)/dwNumCol >= mapCount/dwNumCol) pNeiIndices[7] = -1;
	else pNeiIndices[7] = (nCurrent + dwNumCol);
	
	if((nCurrent+1)%dwNumCol==0 || (nCurrent+1)/dwNumCol >= mapCount/dwNumCol) pNeiIndices[8] = -1;
	else pNeiIndices[8] = (nCurrent + dwNumCol) + 1;
	
	for(int i = 0; i < 9; i++)
	{
		if(pNeiIndices[i] < 0 || pNeiIndices[i] >= mapCount)
			pNeiIndices[i] = -1;
		
		int index = pNeiIndices[i];
		
		if(index==-1) continue;
	}	
	return true;
}

void CECStichSubTern::LoadProjMap(int i,int pSubMap[])
{
	int index = pSubMap[i];

	if (index>=0)
	{
		CECBlockSticher* p = new CECBlockSticher(m_pAsset);
		
		p->LoadFile(m_pAsset->GetMapName(),index);
		
		m_pNeighbourMap[i] = p;
	}	
}
bool CECStichSubTern::LoadHeightMap(int pNeighborMap[])
{
	switch(m_iAlignType)
	{
	case 0:// not init
		break;
	case 1:// map 1,2,5
		LoadProjMap(1,pNeighborMap);
		LoadProjMap(2,pNeighborMap);
		LoadProjMap(5,pNeighborMap);
		break;
	case 2:// map 0,1,3
		LoadProjMap(0,pNeighborMap);
		LoadProjMap(1,pNeighborMap);
		LoadProjMap(3,pNeighborMap);
		break;
	case 3:// map 3,6,7
		LoadProjMap(3,pNeighborMap);
		LoadProjMap(6,pNeighborMap);
		LoadProjMap(7,pNeighborMap);
		break;
	case 4:// map 5,7,8
		LoadProjMap(5,pNeighborMap);
		LoadProjMap(7,pNeighborMap);
		LoadProjMap(8,pNeighborMap);
		break;
	}
	return true;
}

void CECStichSubTern::ConnectHeightMap()
{	
	switch(m_iAlignType)
	{
	case 0:// not init
		break;
	case 1:// map 1,2,5
		if(m_pNeighbourMap[1])
		{
			m_pNeighbourMap[1]->StitchBottomBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[2])
		{
			m_pNeighbourMap[2]->StitchCorner(CECBlockSticher::LEFT_BOTTOM,&currentSubTern);
		}
		
		if(m_pNeighbourMap[5])
		{
			m_pNeighbourMap[5]->StitchLeftBorder(&currentSubTern);
		}
		break;
	case 2:// map 0,1,3
		if(m_pNeighbourMap[0])
		{
			m_pNeighbourMap[0]->StitchCorner(CECBlockSticher::RIGHT_BOTTOM,&currentSubTern);
		}
		
		if(m_pNeighbourMap[1])
		{
			m_pNeighbourMap[1]->StitchBottomBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[3])
		{
			m_pNeighbourMap[3]->StitchRightBorder(&currentSubTern);				
		}
		break;
	case 3:// map 3,6,7
		
		if(m_pNeighbourMap[3])
		{
			m_pNeighbourMap[3]->StitchRightBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[6])
		{			
			m_pNeighbourMap[6]->StitchCorner(CECBlockSticher::RIGHT_TOP,&currentSubTern);
		}
		
		if(m_pNeighbourMap[7])
		{
			m_pNeighbourMap[7]->StitchTopBorder(&currentSubTern);
		}
		break;
	case 4:// map 5,7,8
		
		if(m_pNeighbourMap[5])
		{
			m_pNeighbourMap[5]->StitchLeftBorder(&currentSubTern);				
		}
		
		if(m_pNeighbourMap[7])
		{
			m_pNeighbourMap[7]->StitchTopBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[8])
		{
			m_pNeighbourMap[8]->StitchCorner(CECBlockSticher::LEFT_TOP,&currentSubTern);
		}
		
		break;
	}
}
void CECStichSubTern::Stich()
{
	Release();
	
	int iFileIndex = m_iSubTern;
	int pNeigbor[9];
	currentSubTern.LoadFile(m_pAsset->GetMapName(),iFileIndex);

	GetProjectListInfor(m_pAsset->GetNewSubTernIndexByOldIndex(iFileIndex),m_pAsset->GetTrn2Info().iNumSubTrn,pNeigbor);
	int pNewNeigbor[9];
	for (int i=0;i<9;i++)
	{
		pNewNeigbor[i] = m_pAsset->GetNewSubTernIndex(pNeigbor[i]);
	}
	LoadHeightMap(pNewNeigbor);
	ConnectHeightMap();
	for (i=0;i<9;i++)
	{
		if(m_pNeighbourMap[i])
			m_pNeighbourMap[i]->SaveFile();
	}
}


//////////////////////////////////////////////////////////////////////////

#include <algorithm>
void CECRandomMapProcess::Init()
{
	memset(m_processor,0,sizeof(CECPreprocessor*)*NUM_PROCESSOR);
	m_processor[PROC_TRN2] = new CECTerrainPreprocessor(this);
	m_processor[PROC_WORLD] = new CECWorldFilePreprocessor(this);
	m_processor[PROC_T2BK] = new CECTernBlockPreprocessor(this);
	
	newSubTernOrder.clear();
	m_iBlockRowInSubTern = 0;
	m_iBlockColInSubTern = 0;
//	m_iRowCountSubTern = 0;
//	m_iColCountSubTern = 0;
	
	/*   for (int i=0;i<9;i++)
   	{
	newSubTernOrder.push_back(i);
}*/
	//  newSubTernOrder[33] = 32;
	//  newSubTernOrder[32] = 33;
	//	std::random_shuffle(newSubTernOrder.begin(),newSubTernOrder.end());
	if (false)
	{
		newSubTernOrder.push_back(8);
		newSubTernOrder.push_back(3);
		newSubTernOrder.push_back(4);
		newSubTernOrder.push_back(1);
		newSubTernOrder.push_back(5);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(7);
		newSubTernOrder.push_back(2);
	newSubTernOrder.push_back(6);
	}
	else
	{
	/*	newSubTernOrder.push_back(5);
		newSubTernOrder.push_back(5);
		newSubTernOrder.push_back(1);
		newSubTernOrder.push_back(2);
		newSubTernOrder.push_back(5);
		newSubTernOrder.push_back(8);
		newSubTernOrder.push_back(7);
		newSubTernOrder.push_back(2);
	newSubTernOrder.push_back(6);
*/
		newSubTernOrder.push_back(1);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(9);
		newSubTernOrder.push_back(10);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(5);
		newSubTernOrder.push_back(6);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);
		newSubTernOrder.push_back(0);

// 	for (int i=0;i<16;i++)
//    	{
// 		newSubTernOrder.push_back(i);
// 	}
	//  newSubTernOrder[33] = 32;
	//  newSubTernOrder[32] = 33;
	//	std::random_shuffle(newSubTernOrder.begin(),newSubTernOrder.end());
	}

}
void CECRandomMapProcess::Release()
{
	for (int i=0;i<NUM_PROCESSOR;i++)
	{
		if(!m_processor[i]) continue;

		m_processor[i]->Release();
		delete m_processor[i];
		m_processor[i] = NULL;
	}
//	newSubTernOrder.clear();
}
int CECRandomMapProcess::GetNewSubTernIndexByOldIndex(int oldIndex)
{
	for (unsigned int i=0;i<newSubTernOrder.size();i++)
	{
		if(newSubTernOrder[i] == oldIndex)
			return i;
	}
	
	return -1;
}
int CECRandomMapProcess::GetNewSubTernIndex(int oldIndex)
{
	if(oldIndex>= (int)newSubTernOrder.size()||oldIndex<0) return -1;
	
	return newSubTernOrder[oldIndex];
}
void CECRandomMapProcess::DoProcess(const char* szMap)
{
	if(newSubTernOrder.size()<=0) return;
	
	AString src,des;
	src.Format("maps\\%s\\",szMap);
	des.Format("temp\\maps\\%s\\",szMap);
	glb_DeleteDir(des.GetBuffer(des.GetLength()));
	
	glb_MakeDir(des.GetBuffer(des.GetLength()),des.GetLength());
	
	glb_CopyFiles(src.GetBuffer(src.GetLength()),des.GetBuffer(des.GetLength()));
	
	m_strMapName = szMap;

	ProcessRepeatSubTern();

	for (int i=0;i<NUM_PROCESSOR;i++)
	{
		m_processor[i]->DoProcess(szMap);
	}
}
/*
void CECRandomMapProcess::GetNewRowColInWorld(WORD& row,WORD& col,int& index)
{
	int rowInSub,colInSub;
	rowInSub = row % GetBlockRowCountInSubTern();
	colInSub = col % GetBlockColCountInSubTern();
	int subTernRow = row / GetBlockRowCountInSubTern(); 
	int subTernCol = col / GetBlockColCountInSubTern();
	
	int oldSubTernIndex = subTernRow * GetColCountSubTern() + subTernCol;
	int newSubTernIndex = GetNewSubTernIndexByOldIndex(oldSubTernIndex);
	
	int newSubTernRow = newSubTernIndex / GetColCountSubTern();
	int newSubTernCol = newSubTernIndex % GetColCountSubTern();
	
	row = rowInSub + newSubTernRow * GetBlockRowCountInSubTern();
	col = colInSub + newSubTernCol * GetBlockColCountInSubTern();

	index = row * GetBlockColCountInSubTern()* subTernCol + col;
}*/
void CECRandomMapProcess::GetNewBlockRowColInWorld(int iSubTrn,WORD& row,WORD& col)
{
	int rowInSub,colInSub;
	rowInSub = row % GetBlockRowCountInSubTern();
	colInSub = col % GetBlockColCountInSubTern();

	int newSubTernIndex = GetNewSubTernIndexByOldIndex(iSubTrn);
	
	int newSubTernRow = newSubTernIndex / GetColCountSubTern();
	int newSubTernCol = newSubTernIndex % GetColCountSubTern();
	
	row = rowInSub + newSubTernRow * GetBlockRowCountInSubTern();
	col = colInSub + newSubTernCol * GetBlockColCountInSubTern();
}
void CECRandomMapProcess::GetSubTrnRowColInWorldByIndex(int index,int& row,int& col)
{
	row = index / GetColCountSubTern(); 
	col = index % GetColCountSubTern();	
}
void CECRandomMapProcess::ProcessRepeatSubTern()
{
	unsigned int c = newSubTernOrder.size();
	abase::vector<int> oldSub; // 原来有，随机后没有的序号.

	unsigned int j=0;
	for (int i=0;i<c;i++)
	{
		j=0;
		for ( j=0;j<c;j++)
		{
			if (i==newSubTernOrder[j])
				break;
		}
		if (j == c)
			oldSub.push_back(i);
	}

	abase::vector<RepeatSubTrn> newSub; // 随机后重复的序号
	for (i=1;i<c;i++)
	{
		for (j=0;j<i;j++)
		{
			if (newSubTernOrder[i] == newSubTernOrder[j])
			{
				RepeatSubTrn s;
				s.trn = newSubTernOrder[i];
				s.index = i;
				newSub.push_back(s);
				break;
			}
		}
	}
	ASSERT(newSub.size() == oldSub.size());

	for (i=0;i<newSub.size();i++)
	{
		char src[MAX_PATH],des[MAX_PATH];
		int  from = newSub[i].trn+1;
		int to = oldSub[i]+1;
		sprintf(src,"maps\\%s\\%s_%d.t2bk",GetMapName(),GetMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2bk",GetMapName(),GetMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\%s_%d.t2mk",GetMapName(),GetMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2mk",GetMapName(),GetMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\%s_%d.t2lm",GetMapName(),GetMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2lm",GetMapName(),GetMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\%s_%d.t2lm1",GetMapName(),GetMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2lm1",GetMapName(),GetMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\bsdata\\%d.dat",GetMapName(),from);
		sprintf(des,"temp\\maps\\%s\\bsdata\\%d.dat",GetMapName(),to);
		CopyFileA(src,des,FALSE);

		newSubTernOrder[newSub[i].index] = oldSub[i];
	}
}