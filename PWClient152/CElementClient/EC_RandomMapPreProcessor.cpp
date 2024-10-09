// Filename	: EC_RandomMapPreProcessor.h
// Creator	: WYD
// Date		: 2014/01/14

#include "EC_RandomMapPreProcessor.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_Inventory.h"
#include "EL_RandomMapInfo.h"
#include "EC_Utility.h"
#include "EC_GameSession.h"
#include "EC_UIConfigs.h"
#include "EC_Instance.h"
#include "EC_Team.h"

#include <io.h>
#include <string>
#include <DIRECT.H>
#include <math.h>

#include <A3DTerrain2.h>
#include <AFilePackage.h>
#include <AMemory.h>
#include <ALog.h>

#include <A3DSurface.h>

#include <A3DSurfaceMan.h>
#include <AFI.h>
#include <A3DMacros.h>

using namespace std;

#define new A_DEBUG_NEW

typedef BOOL (WINAPI* PGETDISKFREESPACEEX)(LPCSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);


BOOL SafeGetDiskFreeSpace(LPCSTR pszDrive){
	PGETDISKFREESPACEEX pGetDiskFreeSpaceExFunc;

	__int64 uiFreeBytesAvailableToCaller,uiTotoalNumberOfBytes,uiTotoalNumberOfFreeBytes;
	
	DWORD dwSectionPerCluster,dwBytesPerSect,dwFreeClusters,dwTotalClusters;

	BOOL bResult;
	pGetDiskFreeSpaceExFunc = (PGETDISKFREESPACEEX)GetProcAddress(GetModuleHandleA("kernel32.dll"),"GetDiskFreeSpaceExA");

	__int64 freeByte = 0;

	if (pGetDiskFreeSpaceExFunc){
		bResult = pGetDiskFreeSpaceExFunc(pszDrive,(PULARGE_INTEGER)&uiFreeBytesAvailableToCaller,(PULARGE_INTEGER)&uiTotoalNumberOfBytes,(PULARGE_INTEGER)&uiTotoalNumberOfFreeBytes);
		if(bResult)
			freeByte = uiTotoalNumberOfFreeBytes;		
	}
	else{
		bResult = GetDiskFreeSpaceA(pszDrive,&dwSectionPerCluster,&dwBytesPerSect,&dwFreeClusters,&dwTotalClusters);
		if(bResult)
			freeByte = dwFreeClusters*dwSectionPerCluster*dwBytesPerSect;
	}

	if(freeByte < 50 * 1024 * 1024){
		MessageBoxA(NULL,"硬盘空间不足，无法运行随机副本!","错误",MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	return TRUE;
}

bool IsExistDir(const char* szDir){
	return -1 != _access(szDir,0);
}
bool glb_DeleteDir(const std::string& src){
	string src_backup = src;
	if (src_backup.size() > 0 && src_backup[src_backup.size() - 1] != '\\')	{
		src_backup.append("\\");
	}
	
	string szDir = src_backup;
	szDir = szDir + "*.*";
	int handle;
	struct _finddata_t fileinfo;
	
	handle = _findfirst(szDir.c_str(),&fileinfo);
	if(-1==handle) return false;
	while(!_findnext(handle,&fileinfo))	{
		if(strcmp(fileinfo.name,".")==0 || strcmp(fileinfo.name,"..")==0)
			continue;
		
		string tempSrc = src_backup + fileinfo.name; 
		
		if(fileinfo.attrib==_A_SUBDIR)	{
			glb_DeleteDir(tempSrc);
			
			int ret =0;
			if(IsExistDir(tempSrc.c_str()))
				ret = _rmdir(tempSrc.c_str());
			if(ret !=0)
				ASSERT(FALSE);
		}else {
			string tempSrc = src_backup + fileinfo.name;
			int ret = remove(tempSrc.c_str());			
			if(ret != 0)
				ASSERT(FALSE);
		}
	}
	if (src_backup.size() > 0 && src_backup[src_backup.size() - 1] == '\\')	{
		src_backup.erase(src_backup.size()-1,1);
	//	 _rmdir(src_backup.c_str());
	}
	_findclose(handle);
	
	return true;
}
bool glb_CreateDir(const char *path){
	if(_access(path, 0)!=-1) return true;
	
	BOOL ret = CreateDirectoryA(path, NULL);
	if (!ret && ERROR_ALREADY_EXISTS != GetLastError())	{
		return false;
	}
	return true;
}
void glb_MakeDir(const char* dir,int r){
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

bool glb_CopyFiles(string src,string des){
	if (src.size() > 0 && src[src.size() - 1] != '\\')	{
		src.append("\\");
	}
	if (des.size() > 0 && des[des.size() - 1] != '\\')	{
		des.append("\\");
	}
	
	string szDir = src;
	szDir = szDir + "*.*";
	int handle;
	struct _finddata_t fileinfo;
	
	handle = _findfirst(szDir.c_str(),&fileinfo);
	if(-1==handle) return false;
	while(!_findnext(handle,&fileinfo))	{
		if(strcmp(fileinfo.name,".")==0 || strcmp(fileinfo.name,"..")==0)
			continue;
		
		string tempSrc = src + fileinfo.name;
		
		string tempDes = tempSrc;
		int pos = tempDes.find(src,0);
		tempDes.replace(pos,src.size(),des.c_str());
		
		if(fileinfo.attrib==_A_SUBDIR)		{			
			glb_CreateDir(tempDes.c_str());
			glb_CopyFiles(tempSrc,tempDes);
		} else{
			CopyFileA(tempSrc.c_str(),tempDes.c_str(),FALSE);
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

void CECTerrainPreprocessor::Release(){
	memset(&fileVer,0,sizeof(fileVer));
	memset(&fileHeader,0,sizeof(fileHeader));
	memset(&loaderInfo,0,sizeof(loaderInfo));
	
	subTernInfo.clear();
	textureFile.clear();
	subTernName.clear();
}
void CECTerrainPreprocessor::DoProcess(const char* szMapName){
	LoadFile(szMapName);
	SaveFile(szMapName);
}
bool CECTerrainPreprocessor::LoadFile(const char* szMapName){
	// clear info
	Release();
	
	//Create terrain description file
	char szFile[MAX_PATH] = {0};
	sprintf(szFile, "maps\\%s\\%s.trn2", szMapName,szMapName);
	
	FILE* fpTerrain = NULL;
	
	if (!(fpTerrain = fopen(szFile, "rb")))	{
		a_LogOutput(1,"CECTerrainPreprocessor::LoadTrn2, Failed to open terrain file %s", szFile);
		return false;
	}
	
	try	{
		if (fread(&fileVer,1,sizeof(TRN2FILEIDVER),fpTerrain) != sizeof(TRN2FILEIDVER))
			throw 1;
		if (fread(&fileHeader,1,sizeof(TRN2FILEHEADER),fpTerrain) != sizeof(TRN2FILEHEADER))
			throw 1;
		
		m_pAsset->SetTrn2Info(fileHeader);
		
		subTernInfo.reserve(fileHeader.iNumSubTrn);
		for (int i=0;i<fileHeader.iNumSubTrn;i++){
			TRN2FILESUBTRN trn2;
			if (fread(&trn2,1,sizeof(TRN2FILESUBTRN),fpTerrain) != sizeof(TRN2FILESUBTRN))
				throw 1;
			subTernInfo.push_back(trn2);  
		}
		
		
		textureFile.reserve(fileHeader.iNumTexture);
		
		for (i=0;i<fileHeader.iNumTexture;i++)	{
			int len = 0;
			char path[MAX_PATH] = {0};
			fread(&len,1,sizeof(int),fpTerrain);
			if(len)	{
				fread(path,len,sizeof(char),fpTerrain);
				textureFile.push_back(path);
			}
		}
		
		if (fread(&loaderInfo,1,sizeof(A3DTRN2LOADERB::TRN2FILELOADERINFO),fpTerrain) != sizeof(A3DTRN2LOADERB::TRN2FILELOADERINFO))
			throw 1;
				
		subTernName.reserve(fileHeader.iNumSubTrn);
		
		for (i=0;i<fileHeader.iNumSubTrn;i++){
			int len = 0;
			char path[MAX_PATH] = {0};
			fread(&len,1,sizeof(int),fpTerrain);
			if(len)	{
				fread(path,len,sizeof(char),fpTerrain);
				subTernName.push_back(path);
			}
		}
		
		fclose((fpTerrain));
		
		return true;
	}
	catch (...)	{
		ASSERT(FALSE);
		fclose(fpTerrain);
		a_LogOutput(1,"CECTerrainPreprocessor::LoadTrn2, Failed to load terrain file %s", szFile);
		Release();
		return false;
	}	
}

//	Write string to file
bool CECTerrainPreprocessor::WriteStringLine(FILE* fp, const char* str){
	ASSERT(str);
	
	//	Write length of string
	int iLen = strlen(str);
	fwrite(&iLen, 1, sizeof (int), fp);
	
	//	Write string data
	if (iLen)
		fwrite(str, 1, iLen, fp);
	
	return true;
}

void CECTerrainPreprocessor::SaveFile(const char* szMapName){
	ASSERT((int)subTernInfo.size() == fileHeader.iNumSubTrn);
	if((int)subTernInfo.size() != fileHeader.iNumSubTrn){
		a_LogOutput(1,"CECTerrainPreprocessor::SaveFile，%s 子地图个数错误", szMapName);
		return;
	}
	
	//Create terrain description file
	char szFileTemp[MAX_PATH] = {0};

	sprintf(szFileTemp, "temp\\maps\\%s\\%s.trn2",m_pAsset->GetMapName(), szMapName);
	
	FILE* fpTerrain = NULL;
	
	if (!(fpTerrain = fopen(szFileTemp, "wb+"))){
		a_LogOutput(1,"CECTerrainPreprocessor::SaveTrn2, Failed to open terrain file %s", szFileTemp);
		//	return false;
	}
	
	fwrite(&fileVer,1,sizeof(TRN2FILEIDVER),fpTerrain);
	fwrite(&fileHeader,1,sizeof(TRN2FILEHEADER),fpTerrain);
	ASSERT((int)subTernInfo.size()==fileHeader.iNumSubTrn);
	for (int i=0;i<fileHeader.iNumSubTrn;i++)	{
		fwrite(&subTernInfo[m_pAsset->GetOldSubTernIndexByNewIndex(i)],1,sizeof(TRN2FILESUBTRN),fpTerrain);
	}
	
	for (i=0;i<fileHeader.iNumTexture;i++)	{
		WriteStringLine(fpTerrain,textureFile[i]);
	}
	fwrite(&loaderInfo,1,sizeof(A3DTRN2LOADERB::TRN2FILELOADERINFO),fpTerrain);
	
	for (i=0;i<fileHeader.iNumSubTrn;i++){
		WriteStringLine(fpTerrain,subTernName[m_pAsset->GetOldSubTernIndexByNewIndex(i)]);
		//	WriteStringLine(fpTerrain,subTernName[i]);
	}
	
	fclose(fpTerrain);
}

//////////////////////////////////////////////////////////////////////////

void CECTernBlockPreprocessor::ConvertMaskAreaIdx(int iMaskArea, int& iSubTrn, int& iIdxInSubTrn){
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
bool CECTernBlockPreprocessor::ProcessFile(const char* szMapName,int fileIndex){
	Release();
	
	//	char szSrcFile[MAX_PATH] = {0};
	char szFile[MAX_PATH] = {0};
	//sprintf(szSrcFile, "maps\\%s\\%s_%d.t2bk", szMapName,szMapName,fileIndex);
	sprintf(szFile, "temp\\maps\\%s\\%s_%d.t2bk", m_pAsset->GetMapName(),szMapName,fileIndex+1);	
	
	FILE* fpBlock = NULL;
	
	try	{
		if (!(fpBlock = fopen(szFile, "rb+")))		{
			a_LogOutput(1,"CECTernBlockPreprocessor::LoadFile, Failed to open block file %s", szFile);
			return false;
		}
		
		if (fread(&fileVer,1,sizeof(TRN2FILEIDVER),fpBlock) != sizeof(TRN2FILEIDVER))
			throw 1;
		if (fread(&fileHeader,1,sizeof(A3DTRN2LOADERB::T2BKFILEHEADER5),fpBlock) != sizeof(A3DTRN2LOADERB::T2BKFILEHEADER5))
			throw 1;
		
		blockOffset.reserve(fileHeader.iNumBlock);
		for (int i=0;i<fileHeader.iNumBlock;i++){
			DWORD off;
			if(fread(&off,1,sizeof(DWORD),fpBlock)!= sizeof(DWORD))
				throw 1;
			
			blockOffset.push_back(off);
		}		
		
		int iMaskBlock = m_pAsset->GetTrn2Info().iMaskGrid / m_pAsset->GetTrn2Info().iBlockGrid;
		
		//	Calculate index of the mask area which this block belongs to
		int iMaskPitch = m_pAsset->GetTrn2Info().iTrnBlkCol * m_pAsset->GetTrn2Info().iBlockGrid / m_pAsset->GetTrn2Info().iMaskGrid;

		for (i=0;i<fileHeader.iNumBlock;i++){
			fseek(fpBlock, blockOffset[i], SEEK_SET);
			A3DTRN2LOADERB::T2BKFILEBLOCK6 bk;
			if(fread(&bk,1,sizeof(bk),fpBlock)!= sizeof(bk))
				throw 1;
		
			m_pAsset->GetNewBlockRowColInWorld(fileIndex,bk.iRowInTrn,bk.iColInTrn);
					
			int iMaskRow = bk.iRowInTrn / iMaskBlock;
			int iMaskCol = bk.iColInTrn / iMaskBlock;		
			
			bk.iMaskArea = iMaskRow * iMaskPitch + iMaskCol;
			
			fseek(fpBlock, blockOffset[i], SEEK_SET);
			if(fwrite(&bk,1,sizeof(bk),fpBlock)!=sizeof(bk))
				throw 1;
		}
		
		fclose(fpBlock);
		
		return true;
	}
	catch(...)	{
		ASSERT(FALSE);
		fclose(fpBlock);
	}
	
	return false;
}

void CECTernBlockPreprocessor::Release(){
	memset(&fileVer,0,sizeof(fileVer));
	memset(&fileHeader,0,sizeof(fileHeader));
	blockOffset.clear();
}
void CECTernBlockPreprocessor::DoProcess(const char* szMapName){	
	for (int i=0;i<m_pAsset->GetSubTernCount();i++)	{
		ProcessFile(szMapName,i);
	}
	
	for (i=0;i<m_pAsset->GetSubTernCount();i++)	{
		CECStichSubTern sticher(m_pAsset,i);
		sticher.Stich();
	}	
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

bool CECBlockSticher::GetBlockDataByRowCol(int r,int c,CECBlockSticher::BKINFO& info)
{
	for (int i=0;i<(int)blockData.size();i++){
		if (blockData[i].iRowInTrn == r && blockData[i].iColInTrn == c)	{
			info = blockData[i];
			return true;
		}
	}
	return false;
}
void CECBlockSticher::StitchBottomBorder(CECBlockSticher* pNeighbor){
	for (int i=0;i<GetBorderBlockCount(BOTTOM);i++)	{
		BKINFO info = blockData[GetBorderBlockIndex(BOTTOM,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn+1,info.iColInTrn,neighbourBK);
		assert(bFind);

		if (bFind)		{
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)	{
				info.pHeiData[(s - 1)*s + j] = neighbourBK.pHeiData[j];				
			}
		}
	}
}
void CECBlockSticher::StitchRightBorder(CECBlockSticher* pNeighbor){
	for (int i=0;i<GetBorderBlockCount(RIGHT);i++){
		BKINFO info = blockData[GetBorderBlockIndex(RIGHT,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn,info.iColInTrn+1,neighbourBK);
		assert(bFind);
		
		if (bFind){
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)	{
				info.pHeiData[j*s + s - 1] = neighbourBK.pHeiData[j*s];	
			}
		}
	}
}
void CECBlockSticher::StitchLeftBorder(CECBlockSticher* pNeighbor){
	for (int i=0;i<GetBorderBlockCount(LEFT);i++)	{
		BKINFO info = blockData[GetBorderBlockIndex(LEFT,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn,info.iColInTrn-1,neighbourBK);
		assert(bFind);

		if (bFind){
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)	{
				info.pHeiData[j * s] = neighbourBK.pHeiData[j*s+s-1];	
			}
		}
	}
}
void CECBlockSticher::StitchTopBorder(CECBlockSticher* pNeighbor){
	for (int i=0;i<GetBorderBlockCount(TOP);i++){
		BKINFO info = blockData[GetBorderBlockIndex(TOP,i)];
		BKINFO neighbourBK;
		bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn-1,info.iColInTrn,neighbourBK);
		assert(bFind);
		
		if (bFind)	{
			int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
			for (int j = 0;j<s;j++)	{
				info.pHeiData[j] = neighbourBK.pHeiData[(s-1)*s+j];		
			}
		}
	}
}
void CECBlockSticher::StitchCorner(int corner,CECBlockSticher* pNeighbor){
	BKINFO info = blockData[GetCornerBlockIndex(corner)];
	BKINFO neighbourBK;
	switch (corner)
	{
	case LEFT_TOP:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn-1,info.iColInTrn-1,neighbourBK);
			assert(bFind);
			if (bFind)	{
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[0] = neighbourBK.pHeiData[s*s - 1];
			}
		}
		break;
	case RIGHT_TOP:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn-1,info.iColInTrn+1,neighbourBK);
			assert(bFind);
			if (bFind){
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[s-1] = neighbourBK.pHeiData[(s - 1)*s];
			}
		}
		break;
	case RIGHT_BOTTOM:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn+1,info.iColInTrn+1,neighbourBK);
			assert(bFind);
			if (bFind)	{
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[s*s-1] = neighbourBK.pHeiData[0];
			}
		}
		break;
	case LEFT_BOTTOM:
		{
			bool bFind = pNeighbor->GetBlockDataByRowCol(info.iRowInTrn+1,info.iColInTrn-1,neighbourBK);
			assert(bFind);
			if (bFind)	{
				int s = m_pAsset->GetTrn2Info().iBlockGrid + 1;
				info.pHeiData[(s - 1)*s] = neighbourBK.pHeiData[s - 1];
			}
		}
		break;
	}
}
template<typename T>
void BlockHeightDataCompress(T& FileBlock,BYTE blockCompFlag,int iNumVert,const CECBlockSticher::BKINFO& bkInfo,BYTE* pOutBuf,FILE* fpBlock){
//	A3DTRN2LOADERB::T2BKFILEBLOCK6 FileBlock = blockData[i].bk;
	//	Compress height data ----------------------------
	if ((blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP) || (blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_INC))	{
		if (!(blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_INC)){	//	ZIP compress only
			DWORD dwSrcSize = iNumVert * sizeof (float);
			int v = AFilePackage::Compress((BYTE*)bkInfo.pHeiData, dwSrcSize, pOutBuf, &dwSrcSize);
			ASSERT(v==0);
		//	ASSERT(FileBlock.dwHeiSize == dwSrcSize);
			FileBlock.dwHeiSize = dwSrcSize;
		}
		else if (!(blockCompFlag & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP)){	//	Incremental compress only
			DWORD dwSrcSize;
			bool v = A3DTerrain2::CompressHeightMap(bkInfo.pHeiData, iNumVert, pOutBuf, &dwSrcSize);
			ASSERT(v);
		//	ASSERT(FileBlock.dwHeiSize == dwSrcSize);
			FileBlock.dwHeiSize = dwSrcSize;
		}
		else{	//	Incremental compress and then ZIP compress
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
	}else{	//	No compression
	
		FileBlock.dwHeiSize = iNumVert * sizeof (float);
		memcpy(pOutBuf, bkInfo.pHeiData, FileBlock.dwHeiSize);
	}
	
//	fwrite(&FileBlock, 1, sizeof (FileBlock), fpBlock);
//	fwrite(pOutBuf, 1, FileBlock.dwHeiSize, fpBlock);
}
void CECBlockSticher::SaveFile(){
	char szFile[MAX_PATH] = {0};

	sprintf(szFile, "temp\\maps\\%s\\%s_%d.t2bk", m_pAsset->GetMapName(),m_strMapName,m_iFileIndex+1);
	
	FILE* fpBlock = NULL;
	if (!(fpBlock = fopen(szFile, "rb+"))){
		a_LogOutput(1,"CECStichBolckProcessor::SaveFile, Failed to open block file %s", szFile);
		ASSERT(FALSE);
		return;
	}

	int iNumVert = (m_pAsset->GetTrn2Info().iBlockGrid + 1) * (m_pAsset->GetTrn2Info().iBlockGrid + 1);	

	BYTE* m_pCompHei = new BYTE [iNumVert * sizeof (float) * 2];
	
	abase::vector<DWORD> newOffset;

	fseek(fpBlock, blockOffset[0], SEEK_SET);

	for (int i=0;i<fileHeader.iNumBlock;i++){
		memset(m_pCompHei,0,sizeof(BYTE)*(iNumVert * sizeof (float) * 2));
		newOffset.push_back(ftell(fpBlock));

		if (fileVer.dwVersion<3){
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
		else if (fileVer.dwVersion<4){
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
		else if (fileVer.dwVersion<6){
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
		else{
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
bool ProcessBlock(T& bk, int i,int iNumVert,CECBlockSticher* pBlockProc,FILE* fpBlock, float*& pOutBuf){
	BYTE m_byBlockComp = pBlockProc->GetCompressFlag();

	if(fread(&bk,1,sizeof(bk),fpBlock)!= sizeof(bk)){
		ASSERT(FALSE);
		return false;			
	}
	
	// read height data
	BYTE *pHeiData=NULL, *pHeiData2=NULL;
	if (m_byBlockComp){
		pHeiData = new BYTE[iNumVert * sizeof (float) * 2];			
		pHeiData2 = new BYTE[iNumVert * sizeof (float)];				
		ASSERT(pHeiData && pHeiData2);
	}
	
	float * aTempHei = NULL;
	if (!(aTempHei = new float[iNumVert])){
		ASSERT(FALSE);
		return false;
	}
	
	//	Load block height data ...
	if ((m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP) || (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_INC)){
		//	Load block height data
		fread(pHeiData, bk.dwHeiSize, sizeof(BYTE),fpBlock);
		
		if (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_INC){
			if (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP){
				DWORD dwSize = iNumVert * sizeof (float);
				AFilePackage::Uncompress(pHeiData, bk.dwHeiSize, (BYTE*)pHeiData2, &dwSize);
				dwSize = A3DTerrain2::DecompressHeightMap(pHeiData2, aTempHei);
				ASSERT(dwSize == iNumVert * sizeof (float));
			}
			else{
				DWORD dwSize = A3DTerrain2::DecompressHeightMap(pHeiData, aTempHei);
				ASSERT(dwSize == iNumVert * sizeof (float));						
			}
		}
		else {
			ASSERT( (m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP) && !(m_byBlockComp & A3DTRN2LOADERB::T2BKCOMP_HEI_INC));
			DWORD dwSize = iNumVert * sizeof (float);
			int v = AFilePackage::Uncompress(pHeiData, bk.dwHeiSize, (BYTE*)aTempHei, &dwSize);
			ASSERT(v==0);
			ASSERT(dwSize == iNumVert * sizeof (float));
			
		}
	}
	else{	//	No compression
	
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
bool CECBlockSticher::LoadFile(const char* szMapName,int fileIndex){
	Release();
	
	char szFile[MAX_PATH] = {0};
	sprintf(szFile, "temp\\maps\\%s\\%s_%d.t2bk", m_pAsset->GetMapName(),szMapName,fileIndex+1);
	
	m_iFileIndex = fileIndex;
	m_strMapName = szMapName;
	
	FILE* fpBlock = NULL;
	
	try	{
		if (!(fpBlock = fopen(szFile, "rb+")))		{
			a_LogOutput(1,"CECBlockSticher::LoadFile, Failed to open block file %s", szFile);
			return false;
		}
		
		if (fread(&fileVer,1,sizeof(TRN2FILEIDVER),fpBlock) != sizeof(TRN2FILEIDVER))
			throw 1;

		if (fileVer.dwVersion < 2)	{
			//	Read file header
			A3DTRN2LOADERB::T2BKFILEHEADER Header;
			fread(&Header,1,sizeof(Header),fpBlock);
			
			//iNumBlock		= Header.iNumBlock;
			m_byBlockComp	= 0;

			fileHeader.byCompressed = m_byBlockComp;
			fileHeader.iNumBlock = Header.iNumBlock;
		}
		else if (fileVer.dwVersion < 5){	//	IdVer.dwVersion >= 2 && < 5		
			//	Read file header
			A3DTRN2LOADERB::T2BKFILEHEADER2 Header;
			fread(&Header,1,sizeof(Header),fpBlock);
						
		//	iNumBlock		= Header.iNumBlock;
			m_byBlockComp	= Header.bCompressed ? (A3DTRN2LOADERB::T2BKCOMP_COL_ZIP | A3DTRN2LOADERB::T2BKCOMP_HEI_ZIP | A3DTRN2LOADERB::T2BKCOMP_NOR_ZIP) : 0;

			fileHeader.byCompressed = m_byBlockComp;
			fileHeader.iNumBlock = Header.iNumBlock;
		}
		else{	//	IdVer.dwVersion >= 5
		
			//	Read file header
			fread(&fileHeader,1,sizeof(A3DTRN2LOADERB::T2BKFILEHEADER5),fpBlock);			
		//	iNumBlock		= Header.iNumBlock;
			m_byBlockComp	= fileHeader.byCompressed;
		}
		
	//	blockOffset.reserve(fileHeader.iNumBlock);
		blockOffsetAddr = ftell(fpBlock);

		for (int i=0;i<fileHeader.iNumBlock;i++){
			DWORD off;
			if(fread(&off,1,sizeof(DWORD),fpBlock)!= sizeof(DWORD))
				throw 1;
			
			blockOffset.push_back(off);
		}

		int iNumVert = (m_pAsset->GetTrn2Info().iBlockGrid + 1) * (m_pAsset->GetTrn2Info().iBlockGrid + 1);

		for (i=0;i<fileHeader.iNumBlock;i++){
			fseek(fpBlock, blockOffset[i], SEEK_SET);

			float* pOut;
			if (fileVer.dwVersion<3){
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
			else if (fileVer.dwVersion<4){
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
			else if (fileVer.dwVersion<6){
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
			else{
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
	catch(...){
		ASSERT(FALSE);
		fclose(fpBlock);
	}
	
	return false;
}

void CECBlockSticher::Release(){
	memset(&fileVer,0,sizeof(fileVer));
	memset(&fileHeader,0,sizeof(fileHeader));
	blockOffset.clear();
	for (unsigned int i=0;i<blockData.size();i++){
		delete [] blockData[i].pHeiData;
		delete [] blockData[i].pNormal;
		delete [] blockData[i].pDiffuse;
		delete [] blockData[i].pDiffuse1;
	}
	blockData.clear();	
}

CECStichSubTern::CECStichSubTern(CECRandomMapProcess* asset,int iSubTern):m_iSubTern(iSubTern),m_pAsset(asset),currentSubTern(asset){
	for (int i=0;i<9;i++)	{
		m_pNeighbourMap[i] = NULL;
	}
	m_iAlignType = 2;
}
void CECStichSubTern::Release(){
	for (int i=0;i<9;i++){
		if(m_pNeighbourMap[i]){
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
	
	for(int i = 0; i < 9; i++){
		if(pNeiIndices[i] < 0 || pNeiIndices[i] >= mapCount)
			pNeiIndices[i] = -1;
		
		int index = pNeiIndices[i];		
		if(index==-1) continue;
	}	
	return true;
}

void CECStichSubTern::LoadProjMap(int i,int pSubMap[]){
	int index = pSubMap[i];
	if (index>=0){
		CECBlockSticher* p = new CECBlockSticher(m_pAsset);
		
		p->LoadFile(m_pAsset->GetRealMapName(),index);
		
		m_pNeighbourMap[i] = p;
	}	
}
bool CECStichSubTern::LoadHeightMap(int pNeighborMap[]){
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

void CECStichSubTern::ConnectHeightMap(){	
	switch(m_iAlignType)
	{
	case 0:// not init
		break;
	case 1:// map 1,2,5
		if(m_pNeighbourMap[1]){
			m_pNeighbourMap[1]->StitchBottomBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[2]){
			m_pNeighbourMap[2]->StitchCorner(CECBlockSticher::LEFT_BOTTOM,&currentSubTern);
		}
		
		if(m_pNeighbourMap[5]){
			m_pNeighbourMap[5]->StitchLeftBorder(&currentSubTern);
		}
		break;
	case 2:// map 0,1,3
		if(m_pNeighbourMap[0]){
			m_pNeighbourMap[0]->StitchCorner(CECBlockSticher::RIGHT_BOTTOM,&currentSubTern);
		}
		
		if(m_pNeighbourMap[1]){
			m_pNeighbourMap[1]->StitchBottomBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[3]){
			m_pNeighbourMap[3]->StitchRightBorder(&currentSubTern);				
		}
		break;
	case 3:// map 3,6,7
		
		if(m_pNeighbourMap[3]){
			m_pNeighbourMap[3]->StitchRightBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[6]){			
			m_pNeighbourMap[6]->StitchCorner(CECBlockSticher::RIGHT_TOP,&currentSubTern);
		}
		
		if(m_pNeighbourMap[7]){
			m_pNeighbourMap[7]->StitchTopBorder(&currentSubTern);
		}
		break;
	case 4:// map 5,7,8
		
		if(m_pNeighbourMap[5]){
			m_pNeighbourMap[5]->StitchLeftBorder(&currentSubTern);				
		}
		
		if(m_pNeighbourMap[7]){
			m_pNeighbourMap[7]->StitchTopBorder(&currentSubTern);
		}
		
		if(m_pNeighbourMap[8]){
			m_pNeighbourMap[8]->StitchCorner(CECBlockSticher::LEFT_TOP,&currentSubTern);
		}
		
		break;
	}
}
void CECStichSubTern::Stich(){
	Release();
	
	int iFileIndex = m_iSubTern;
	int pNeigbor[9];
	currentSubTern.LoadFile(m_pAsset->GetRealMapName(),iFileIndex);

	GetProjectListInfor(m_pAsset->GetNewSubTernIndexByOldIndex(iFileIndex),m_pAsset->GetTrn2Info().iNumSubTrn,pNeigbor);
	int pNewNeigbor[9];
	for (int i=0;i<9;i++){
		pNewNeigbor[i] = m_pAsset->GetOldSubTernIndexByNewIndex(pNeigbor[i]);
	}
	LoadHeightMap(pNewNeigbor);
	ConnectHeightMap();
	for (i=0;i<9;i++){
		if(m_pNeighbourMap[i])
			m_pNeighbourMap[i]->SaveFile();
	}
}


//////////////////////////////////////////////////////////////////////////
bool CECRandomMapProcess::GetRegionName(const A3DVECTOR3& pos,ACString& name){
	int x,y;
	
	x = (int)(trn2Info.fTerrainWid * 0.5f + pos.x);
	y = (int)(trn2Info.fTerrainHei * 0.5f - pos.z);
	a_Clamp(x,0,(int)trn2Info.fTerrainWid);
	a_Clamp(y,0,(int)trn2Info.fTerrainHei);

	x /= 128;
	y /= 128;

	int idx = y * 8 + x;
	a_Clamp(idx,0,(int)sevOrder.size()-1);
	idx = sevOrder[idx];

	return GetRegionName(idx,name);
}

bool CECRandomMapProcess::GetRegionName(int idx,ACString& name){
	CRandMapProp::MAP_INFO info;
	if (m_pMapProp && m_pMapProp->GetGirdProp(idx,info)) {
		name = info.name;
		return true;
	}

	return false;
}
void CECRandomMapProcess::Init(int r,int c,int* pData){
	memset(m_processor,0,sizeof(CECPreprocessor*)*NUM_PROCESSOR);
	m_processor[PROC_TRN2] = new CECTerrainPreprocessor(this);
	m_processor[PROC_T2BK] = new CECTernBlockPreprocessor(this);
	
	newSubTernOrder.clear();
	sevOrder.clear();

	if (pData && r*c){
		for (int i=0;i<r*c;i++)
			newSubTernOrder.push_back(*pData++);
		
		sevOrder = newSubTernOrder;
	}
}
void CECRandomMapProcess::Release(bool bOnlyReleaseProcessor){
	for (int i=0;i<NUM_PROCESSOR;i++)	{
		if(!m_processor[i]) continue;

		m_processor[i]->Release();
		delete m_processor[i];
		m_processor[i] = NULL;
	}

	if (!bOnlyReleaseProcessor){
		delete m_pMapProp;
		m_pMapProp = NULL;
		newSubTernOrder.clear();
	}
}
int CECRandomMapProcess::GetNewSubTernIndexByOldIndex(int oldIndex){ // 原来的第几块，现在变成第几块

	for (unsigned int i=0;i<newSubTernOrder.size();i++)	{
		if(newSubTernOrder[i] == oldIndex)
			return i;
	}
	
	return -1;
}
int CECRandomMapProcess::GetOldSubTernIndexByNewIndex(int oldIndex){ // 现在的第几块，是原来的第几块

	if(oldIndex>= (int)newSubTernOrder.size()||oldIndex<0) return -1;	
	return newSubTernOrder[oldIndex];
}

int CECRandomMapProcess::GetSevSubTernIndex(int oldIndex){

	if(oldIndex>= (int)sevOrder.size()||oldIndex<0) return -1;	
	return sevOrder[oldIndex];
}
AString CECRandomMapProcess::GetMapName(){
	return CECRandomMapProcess::GetRandomMapName(m_strMapName);
}
void CECRandomMapProcess::DoProcess(const char* szMap){
	if( SafeGetDiskFreeSpace(NULL) == FALSE) {
		g_dwFatalErrorFlag = FATAL_ERROR_LOAD_BUILDING;
		return;
	}

	if(newSubTernOrder.size()<=0) return;
	
	m_strMapName = szMap;

	AString src,des;
	src.Format("maps\\%s\\",m_strMapName);
	des.Format("temp\\maps\\%s\\",GetMapName());
	glb_DeleteDir(des.GetBuffer(des.GetLength()));
	
	glb_MakeDir(des.GetBuffer(des.GetLength()),des.GetLength());	
	glb_CopyFiles(src.GetBuffer(src.GetLength()),des.GetBuffer(des.GetLength()));	

	ProcessRepeatSubTern();

	for (int i=0;i<NUM_PROCESSOR;i++){
		m_processor[i]->DoProcess(szMap);
	}

	GenerateWorldMap();

	GenerateMiniMap();

	if (m_pMapProp)
		delete m_pMapProp;
	
	AString cltFile = "Maps\\" + m_strMapName + "\\map_desp.clt";
	m_pMapProp = new CRandMapProp;
	if(!m_pMapProp->LoadClt(cltFile))
		a_LogOutput(1,"CECRandomMapProcess::DoProcess, fail to load %s",cltFile);

	Release(true);
}

void CECRandomMapProcess::GetNewBlockRowColInWorld(int iSubTrn,WORD& row,WORD& col){
	int rowInSub,colInSub;
	rowInSub = row % GetBlockRowCountInSubTern();
	colInSub = col % GetBlockColCountInSubTern();

	int newSubTernIndex = GetNewSubTernIndexByOldIndex(iSubTrn);	
	int newSubTernRow = newSubTernIndex / GetColCountSubTern();
	int newSubTernCol = newSubTernIndex % GetColCountSubTern();
	
	row = rowInSub + newSubTernRow * GetBlockRowCountInSubTern();
	col = colInSub + newSubTernCol * GetBlockColCountInSubTern();
}

void CECRandomMapProcess::ProcessRepeatSubTern(){
	unsigned int c = newSubTernOrder.size();
	abase::vector<int> oldSub; // 原来有，随机后没有的序号.

	unsigned int j=0;
	for (unsigned int i=0;i<c;i++){
		j=0;
		for ( j=0;j<c;j++){
			if (i==(unsigned int)newSubTernOrder[j])
				break;
		}
		if (j == c)
			oldSub.push_back(i);
	}

	abase::vector<RepeatSubTrn> newSub; // 随机后重复的序号：从第二次出现开始记录信息
	for (i=1;i<c;i++){
		for (j=0;j<i;j++){
			if (newSubTernOrder[i] == newSubTernOrder[j]){
				RepeatSubTrn s;
				s.trn = newSubTernOrder[i];
				s.index = i;
				newSub.push_back(s);
				break;
			}
		}
	}
	ASSERT(newSub.size() == oldSub.size());

	for (i=0;i<(int)newSub.size();i++){
		char src[MAX_PATH],des[MAX_PATH];
		int  from = newSub[i].trn+1;
		int to = oldSub[i]+1;
		sprintf(src,"maps\\%s\\%s_%d.t2bk",GetRealMapName(),GetRealMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2bk",GetMapName(),GetRealMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\%s_%d.t2mk",GetRealMapName(),GetRealMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2mk",GetMapName(),GetRealMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\%s_%d.t2lm",GetRealMapName(),GetRealMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2lm",GetMapName(),GetRealMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\%s_%d.t2lm1",GetRealMapName(),GetRealMapName(),from);
		sprintf(des,"temp\\maps\\%s\\%s_%d.t2lm1",GetMapName(),GetRealMapName(),to);
		CopyFileA(src,des,FALSE);

		sprintf(src,"maps\\%s\\bsdata\\%d.dat",GetRealMapName(),from);
		sprintf(des,"temp\\maps\\%s\\bsdata\\%d.dat",GetMapName(),to);
		CopyFileA(src,des,FALSE);

		newSubTernOrder[newSub[i].index] = oldSub[i];
	}
}
// AString CECRandomMapProcess::GetMapName()
// {
// 	return m_strMapName ;//+ g_pGame->GetGameSession()->GetUserID();
// }
void CECRandomMapProcess::GenerateMiniMap(){
	A3DSurface *pA3DSurface;
	A3DSurfaceMan *pMan = g_pGame->GetA3DEngine()->GetA3DSurfaceMan();
	
	const int miniMapWCount = 2;
	const int miniMapHCount = 2;
	const int imgWCount = 4;
	const int imgHCount = 4;

	for (int i=0;i<miniMapHCount;i++){
		int baseRow = i * imgHCount;
		for (int j=0;j<miniMapWCount;j++){
			int baseCol = j * imgWCount;

			AString miniMap;
			miniMap.Format("%02d%02d.dds",i,j);

			abase::vector<A3DSurface *> vecSrcImg;

			for (int r = baseRow;r < baseRow + imgHCount;r++){
				for (int c = baseCol;c < baseCol + imgWCount;c++){
					int index = r * miniMapWCount*imgWCount + c;

					AString fileName;
					fileName.Format("minimaps\\%s\\%02d.dds",m_strMapName,sevOrder[index]);
					bool bval = pMan->LoadSurfaceFromFile(fileName, 0, &pA3DSurface);
					if (bval)					
						vecSrcImg.push_back(pA3DSurface);					
				}
			}
			AString path = "temp\\surfaces\\minimaps\\" + GetMapName() + "\\";
			GenerateImage(vecSrcImg,path,miniMap);
		}
	}	
}
void CECRandomMapProcess::GenerateImage(abase::vector<A3DSurface*>& srcImgVec, const AString& path, const AString& fileName){
	if(srcImgVec.size()<1) return;

	A3DSurfaceMan *pMan = g_pGame->GetA3DEngine()->GetA3DSurfaceMan();	
	
	int i, j, nIndex;
	int W0 = srcImgVec[0]->GetWidth();
	int H0 = srcImgVec[0]->GetHeight();
	
	int nCols = int(sqrt(srcImgVec.size()));
	int W = W0 * nCols;
/*	n = 1;
	while( n < W ) n *= 2;
	W = n;
	nCols = W / W0;
	*/
	int nRows = (srcImgVec.size() + nCols - 1) / nCols;
	int H = H0 * nRows;
	
/*	n = 1;
	while( n < H ) n *= 2;
	H = n;
	*/
	A3DSurface* pDestSurface = new A3DSurface;
	A3DFORMAT nFormat = srcImgVec[0]->GetFormat();
	pDestSurface->Create(g_pGame->GetA3DDevice(), W, H, nFormat);
	
	POINT a_pt[1];
	RECT a_rc[1] = { { 0, 0, W0, H0 } };
	for( i = 0; i < nRows; i++ ){
		for( j = 0; j < nCols; j++ ){
			nIndex = i * nCols + j;
			if( nIndex < (int)srcImgVec.size() ){
				if( srcImgVec[nIndex]->GetFormat() != nFormat )				
					return;				
				
				a_pt[0].x = W0 * j;
				a_pt[0].y = H0 * i;
				pDestSurface->CopyRects(srcImgVec[nIndex], a_rc, 1, a_pt);
			}
		}
	}
	
	char szFile[256];
	
	sprintf(szFile, "%s\\%s",path,fileName);
	glb_MakeDir(szFile,strlen(szFile));
	
	if( nFormat == A3DFMT_A8R8G8B8 || nFormat == A3DFMT_R8G8B8 ){
		A3DSurface *pA3DSurface = new A3DSurface;
		pA3DSurface->Create(g_pGame->GetA3DDevice(), W, H, A3DFMT_DXT3);
		D3DXLoadSurfaceFromSurface(pA3DSurface->GetDXSurface(), NULL, NULL, pDestSurface->GetDXSurface(), NULL, NULL, D3DX_FILTER_NONE, 0);
		pA3DSurface->SaveToFile(szFile);
		A3DRELEASE(pA3DSurface)
	}
	else
		pDestSurface->SaveToFile(szFile);
	
	A3DRELEASE(pDestSurface);

	for (i =0;i<(int)srcImgVec.size();i++)
		A3DRELEASE(srcImgVec[i]);	

	unsigned char* pData = NULL;
	int iLen = 0;
	if(EncodeMap(szFile,pData,iLen))
	{
		SaveMapToFile(path + fileName,pData,iLen);
		delete [] pData;
	}
}
bool CECRandomMapProcess::EncodeMap(const char* file,unsigned char*& pData,int& iLen)
{
	FILE* fp = fopen(file,"rb");
	if(!fp) return NULL;

	fseek(fp,0,SEEK_END);
	iLen = ftell(fp);
	if(iLen<1) {
		fclose(fp);
		return false;
	}

	pData = new unsigned char[iLen];
	int dwLen;
	fseek(fp,0,SEEK_SET);
	dwLen = fread(pData,sizeof(unsigned char),iLen,fp);
	fclose(fp);

	if(dwLen != iLen)
	{
		delete [] pData;
		pData = NULL;
		return false;
	}

	for (int i=0;i<iLen;i++){
		pData[i] ^= 5;
	}

	return true;
}
void CECRandomMapProcess::SaveMapToFile(const char* desfile,unsigned char* pData,int iLen)
{
	if(!pData) return;
	FILE* fp = fopen(desfile,"wb");
	if(!fp) return;

	fwrite(pData,sizeof(unsigned char),iLen,fp);

	fclose(fp);	
}
void CECRandomMapProcess::GenerateWorldMapBorder(const AString& srcpath,const AString& path, const AString& fileName){	

	A3DSurfaceMan *pMan = g_pGame->GetA3DEngine()->GetA3DSurfaceMan();	
	
	AString mapfileName = path + fileName;

	unsigned char* pData = NULL;
	int iLen = 0;

	if(!EncodeMap(mapfileName,pData,iLen)) 
		return;

	A3DSurface* pTargetSurface;

//	bool bval = pMan->LoadSurfaceFromFile(mapfileName, 0, &pTargetSurface);
	bool bval = pMan->LoadSurfaceFromFileInMemory(mapfileName,pData,iLen,0,&pTargetSurface);

	delete [] pData;

	if(!bval || !pTargetSurface)
		return;

	A3DSurface* pBorder[4]; // left,top,right,bottom
	for (int i=0;i<4;i++)
	{
		AString border;
		border.Format("%s\\border_%d.dds",srcpath,i); // 0: left, 1: top,2:right, 3:bottom

		bval = pMan->LoadSurfaceFromFile(border,0,&pBorder[i]);
		ASSERT(bval);
		if(!bval || !pBorder[i])
			return;
	}
	
	int W_TOP = 1024;
	int H_TOP = 32;
	int W_LEFT = 32;
	int H_LEFT = 960;

	int W = 960;
	int H = 960;
	
	A3DSurface* pDestSurface = new A3DSurface;
	A3DFORMAT nFormat = pTargetSurface->GetFormat();
	pDestSurface->Create(g_pGame->GetA3DDevice(), W + W_LEFT + W_LEFT, H + H_TOP + H_TOP, nFormat);
	
	
	POINT a_pt[1];
	
	// map
	RECT a_rc[1] = { { W_LEFT, H_TOP, W + W_LEFT, H + H_TOP} };
	a_pt[0].x = W_LEFT;
	a_pt[0].y = H_TOP;
	bool val = pDestSurface->CopyRects(pTargetSurface, a_rc, 1, a_pt);
	ASSERT(val);

	// left border
	RECT a_rc_left[1] = {{0,0,W_LEFT,H_LEFT}};
	a_pt[0].x = 0;
	a_pt[0].y = H_TOP;
	val = pDestSurface->CopyRects(pBorder[0], a_rc_left, 1, a_pt);
	ASSERT(val);

	// top border
	RECT a_rc_top[1] = {{0,0,W_TOP,H_TOP}};
	a_pt[0].x = 0;
	a_pt[0].y = 0;
	val = pDestSurface->CopyRects(pBorder[1], a_rc_top, 1, a_pt);
	ASSERT(val);

	// right border
	a_pt[0].x = W_TOP-W_LEFT;
	a_pt[0].y = H_TOP;
	val = pDestSurface->CopyRects(pBorder[2], a_rc_left, 1, a_pt);	
	ASSERT(val);
	
	// bottom border
	a_pt[0].x = 0;
	a_pt[0].y = H_LEFT + H_TOP;
	pDestSurface->CopyRects(pBorder[3], a_rc_top, 1, a_pt);	

	char szFile[256];
	
	sprintf(szFile, "%s\\%s",path,fileName);
	glb_MakeDir(szFile,strlen(szFile));
	
	if( nFormat == A3DFMT_A8R8G8B8 || nFormat == A3DFMT_R8G8B8 ){
		A3DSurface *pA3DSurface = new A3DSurface;
		pA3DSurface->Create(g_pGame->GetA3DDevice(), W, H, A3DFMT_DXT3);
		D3DXLoadSurfaceFromSurface(pA3DSurface->GetDXSurface(), NULL, NULL, pDestSurface->GetDXSurface(), NULL, NULL, D3DX_FILTER_NONE, 0);
		pA3DSurface->SaveToFile(szFile);
		A3DRELEASE(pA3DSurface)
	}
	else
		pDestSurface->SaveToFile(szFile);
	
	A3DRELEASE(pDestSurface);

	for (i=0;i<4;i++)
		A3DRELEASE(pBorder[i]);

	pData = NULL;
	iLen = 0;
	if(EncodeMap(szFile,pData,iLen))
	{
		SaveMapToFile(path + fileName,pData,iLen);
		delete [] pData;
	}		
}
void CECRandomMapProcess::GenerateWorldMap(){
	A3DSurface *pA3DSurface;
	A3DSurfaceMan *pMan = g_pGame->GetA3DEngine()->GetA3DSurfaceMan();
	
	abase::vector<A3DSurface *> vecA3DSurface;
	
 
	for (unsigned int c=0;c<sevOrder.size();c++){
		AString fileName;
		fileName.Format("ingame\\%s\\%02d.dds",m_strMapName,sevOrder[c]);
		bool bval = pMan->LoadSurfaceFromFile(fileName, 0, &pA3DSurface);
		if (bval)		
			vecA3DSurface.push_back(pA3DSurface);		
	}

	AString fileName = GetMapName() + ".dds";
	AString path = "temp\\surfaces\\ingame\\";
	GenerateImage(vecA3DSurface,path,fileName);

//	AString srcPath = "ingame\\" + m_strMapName;
//	GenerateWorldMapBorder(srcPath,path,fileName);	
}

void CECRandomMapProcess::Tick(DWORD dt){
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if(pHost){
		m_worldmapFogMask.UpdateByPos(pHost);
	}
}

bool CECRandomMapProcess::HaveClearMapFogItem(){
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int worldID = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	if(pHost){
	//	CECUIConfig::RandomMapItem itemInfo;
	//	if (!CECUIConfig::Instance().GetGameUI().GetRandomMapItemInfo(worldID,itemInfo))	
	//		itemInfo = CECUIConfig::Instance().GetGameUI().DefaultRandomMapItem;		
		
	//	return pHost->GetPack()->GetItemTotalNum(itemInfo.itemID) >= itemInfo.count;
		return pHost->GetPack()->FindItem(45742) >=0;
	}

	return false;
}

void CECRandomMapProcess::DeleteAllRandomMapDataForSingleUser(){
	for (int i=0;i<CECUIConfig::Instance().GetGameUI().GetRandomMapCount();i++)	{
		int id = CECUIConfig::Instance().GetGameUI().GetRandomMapID(i);
		CECInstance* pInst = g_pGame->GetGameRun()->GetInstance(id);
		if (pInst)
			DeleteRandomMapData(pInst->GetPath());
	}
}
void CECRandomMapProcess::DeleteRandomMapData(const char* szMap){
	AString des;
	des.Format("temp\\maps\\%s\\",GetRandomMapName(szMap));
	glb_DeleteDir(des.GetBuffer(des.GetLength()));

	des.Format("temp\\surfaces\\minimaps\\%s\\",GetRandomMapName(szMap));
	glb_DeleteDir(des.GetBuffer(des.GetLength()));

	des.Format("temp\\surfaces\\ingame\\%s.dds",GetRandomMapName(szMap));
	remove(des.GetBuffer(des.GetLength())); // remove file
}
AString CECRandomMapProcess::GetRandomMapName(const AString& strMap){
	AString ret;
	ret.Format("%s_%d_%d",strMap,g_pGame->GetGameSession()->GetZoneID(),g_pGame->GetGameSession()->GetCharacterID());
	
	return ret;
}
////////////////////////////////////////////////////

CECRandomMapFogMask::CECRandomMapFogMask(){
	m_tileVisbleFlag.reset();
	m_iMapW = MAP_SIZE;
	m_iMapH = MAP_SIZE;
	m_iCellW = m_iMapW / FOGMASK_RESOLUTION;
	m_iCellH = m_iMapH / FOGMASK_RESOLUTION;
}
void CECRandomMapFogMask::UpdateByPos(CECHostPlayer* pHost){
	if(pHost){
		CECTeam* pTeam = pHost->GetTeam();
		if(pTeam){
			for (int i=0;i<pTeam->GetMemberNum();i++){
				CECTeamMember* pMemb = pTeam->GetMemberByIndex(i);
				if(pMemb && pMemb->IsSameInstanceTeamMember() && pMemb->IsInfoReady()){
					A3DVECTOR3 pos = pMemb->GetPos();
				/*	if (m_teamMemLastPos.find(pMemb->GetCharacterID()) == m_teamMemLastPos.end()){						
						pos = pMemb->GetPos();
					}
					else{
						A3DVECTOR3 lastPos = m_teamMemLastPos[pMemb->GetCharacterID()];
						A3DVECTOR3 delta = pMemb->GetPos() - lastPos;
						float fDist = delta.MagnitudeH();
						// 在比较小的跳跃范围内插值，如果太远就不处理了。
						if ( fDist > 1.5 * FOGRADIUS && fDist < 3 * FOGRADIUS){
							pos = (pMemb->GetPos() + lastPos);
							pos *= 0.5;
							UpdateByPos(pos);
						}						
						pos = pMemb->GetPos();
					}*/
					UpdateByPos(pos);
				//	m_teamMemLastPos[pMemb->GetCharacterID()] = pMemb->GetPos();
				}
			}
		}
		
		UpdateByPos(pHost->GetPos());
	}
}
void CECRandomMapFogMask::UpdateByPos(const A3DVECTOR3& pos){
	int x,y;

	x = (int)(m_iMapW * 0.5f + pos.x);
	y = (int)(m_iMapH * 0.5f - pos.z);
	a_Clamp(x,0,m_iMapW);
	a_Clamp(y,0,m_iMapH);

	x /= m_iCellW;
	y /= m_iCellH;

	int c1 = max(0,x - FOGRADIUS);
	int c2 = min(FOGMASK_RESOLUTION,x + FOGRADIUS);
	int r1 = max(0,y - FOGRADIUS);
	int r2 = min(FOGMASK_RESOLUTION,y + FOGRADIUS);
	for (int r=r1;r<r2;r++) {
		int base = r * FOGMASK_RESOLUTION;
		for (int c=c1;c<c2;c++)	{
			int tr = fabs(r - y);
			int tc = fabs(c - x);
			if(tr*tr+tc*tc<= FOGCIRCLERADIUS)
				m_tileVisbleFlag.set(base + c,true);
		}
	}
}
bool CECRandomMapFogMask::GetTileVisbile(int idx) const{
	if(idx>= m_tileVisbleFlag.size()) return false;
	return m_tileVisbleFlag.test(idx);
}

//////////////////////////////////////////////////////////