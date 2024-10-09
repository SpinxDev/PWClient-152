// Filename	: EC_RandomMapPreProcessor.h
// Creator	: WYD
// Date		: 2014/01/14

#pragma once

#include "EC_WorldFile.h"

#include <A3DTerrain2File.h>
#include <AString.h>
#include <vector.h>
#include <ABaseDef.h>
#include <AArray.h>
#include <A3DVector.h>
#include <BITSET>
#include <AAssist.h>

#include <hashmap.h>

class CECRandomMapProcess;
class A3DSurface;


class CECPreprocessor
{
public:	
	CECPreprocessor(CECRandomMapProcess* asset):m_pAsset(asset){}
	virtual ~CECPreprocessor(){Release();}	
	virtual void DoProcess(const char* szMapName) = 0;
	virtual void Release(){};
protected:
	CECRandomMapProcess* m_pAsset;
private:
};

// 处理 地形trn2文件
class CECTerrainPreprocessor : public CECPreprocessor
{
public:
	CECTerrainPreprocessor(CECRandomMapProcess* asset):CECPreprocessor(asset){}
	void DoProcess(const char* szMapName);
	void Release();
protected:
	bool LoadFile(const char* szMapName);
	void SaveFile(const char* szMapName);	

	bool WriteStringLine(FILE* fp, const char* str);

protected:

	// trn2 文件按如下信息组织内容
	TRN2FILEIDVER fileVer;
	TRN2FILEHEADER fileHeader;
	abase::vector<TRN2FILESUBTRN> subTernInfo;
	abase::vector<AString> textureFile;
	A3DTRN2LOADERB::TRN2FILELOADERINFO loaderInfo;
	abase::vector<AString> subTernName;
};

// 处理 t2bk文件
class CECTernBlockPreprocessor : public CECPreprocessor
{
public:
	CECTernBlockPreprocessor(CECRandomMapProcess* asset):CECPreprocessor(asset){ }
	void DoProcess(const char* szMapName);
	void Release();

protected:
	bool ProcessFile(const char* szMapName,int fileIndex);
	void ConvertMaskAreaIdx(int iMaskArea, int& iSubTrn, int& iIdxInSubTrn);
	
protected:
	TRN2FILEIDVER fileVer;
	A3DTRN2LOADERB::T2BKFILEHEADER5 fileHeader;
	abase::vector<DWORD> blockOffset;
};

//////////////////////////////////////////////////////////////////////////
// 处理基本地形块中block的缝合

class CECBlockSticher
{
public:
	struct BKINFO
	{
		BKINFO() { memset(this,0,sizeof(*this));}

		int		iMaskArea;		//	Index of mask area this block blongs to
		WORD	iRowInTrn;		//	Row of this block in whole terrain
		WORD	iColInTrn;		//	Column of this block in whole terrain
		float	fLODScale;		//	LOD scale factor
		DWORD	dwLayerFlags;	//	Effect layer flags
		DWORD	dwHeiSize;		//	Height data size
		DWORD	dwNormalSize;	//	Normal data size
		DWORD	dwDiffSize;		//	Diffuse (day light) data size
		DWORD	dwDiffSize1;	//	Diffuse (night light) data size
		DWORD	dwBlkFlags;	

		float* pHeiData;
		BYTE* pNormal;
		BYTE* pDiffuse;
		BYTE* pDiffuse1;
	};
	
	enum
	{
		LEFT = 0,
		RIGHT,
		TOP,
		BOTTOM,
		NUM_BORDER,
	};
	enum
	{
		LEFT_TOP = 0,
		RIGHT_TOP,
		RIGHT_BOTTOM,
		LEFT_BOTTOM,
		NUM_CORNER,
	};
	public:
		CECBlockSticher(CECRandomMapProcess* asset):m_pAsset(asset),m_byBlockComp(0){ }
		~CECBlockSticher(){Release();}
		bool LoadFile(const char* szMapName,int fileIndex);
		void SaveFile();
		void Release();

		
		int GetBorderBlockCount(int border) { return borderBlock[border].size();}		
		
		BKINFO GetBlockData(int i) { return blockData[i];}
		int GetBorderBlockIndex(int border,int i) { return borderBlock[border][i];}
		
		int GetCornerBlockIndex(int corner) { return cornerBlock[corner];}
		
		void StitchRightBorder(CECBlockSticher* pNeighbor);
		void StitchLeftBorder(CECBlockSticher* pNeighbor);
		void StitchTopBorder(CECBlockSticher* pNeighbor);
		void StitchBottomBorder(CECBlockSticher* pNeighbor);
		
		void StitchCorner(int corner,CECBlockSticher* pNeighbor);
		
		bool GetBlockDataByRowCol(int r,int c,CECBlockSticher::BKINFO& info);
		
		void AddBlockInfo(BKINFO info) { blockData.push_back(info);}
		void SetCorner(int corner,int index) { cornerBlock[corner] = index;}
		void AddBorder(int border,int index) {borderBlock[border].push_back(index);}
		BYTE GetCompressFlag() const { return m_byBlockComp;}
		
		CECRandomMapProcess* GetAssetMan() { return m_pAsset;}
protected:
		TRN2FILEIDVER fileVer;
		A3DTRN2LOADERB::T2BKFILEHEADER5 fileHeader;
		abase::vector<DWORD> blockOffset;
		abase::vector<BKINFO> blockData;
		
		DWORD blockOffsetAddr;

		int m_iFileIndex;
		AString m_strMapName;
		
		abase::vector<int> borderBlock[NUM_BORDER]; // 边界block
		
		int cornerBlock[NUM_CORNER];
		BYTE m_byBlockComp;
		CECRandomMapProcess* m_pAsset;
};

// 处理基本地形块的缝合
class CECStichSubTern
{
public:
	CECStichSubTern(CECRandomMapProcess* asset,int iSubTern);
	~CECStichSubTern(){Release();}
	void Stich();
	void Release();
protected:
	bool GetProjectListInfor(int currentMapIndex, int mapCount,int pNeiIndices[]);
	void ConnectHeightMap();
	bool LoadHeightMap(int pNeighborMap[]);
	void LoadProjMap(int i,int pSubMap[]);
protected:
	CECBlockSticher currentSubTern;
	CECRandomMapProcess* m_pAsset;
	CECBlockSticher* m_pNeighbourMap[9];
	int m_iSubTern;
	int m_iAlignType;// 对齐方式
};
class CECHostPlayer;

class CECRandomMapFogMask
{
public:
	enum
	{
		FOGMASK_RESOLUTION = 1024,
		MAP_SIZE = 1024,
	};
	enum
	{
		FOGRADIUS = 24,
		FOGCIRCLERADIUS = FOGRADIUS * FOGRADIUS,
	};
public:
	CECRandomMapFogMask() ;
	void UpdateByPos(CECHostPlayer* pHost);
	void UpdateByPos(const A3DVECTOR3& pos);
	bool GetTileVisbile(int idx) const;

	int GetCellW() const { return m_iCellW;}
	int GetCellH() const { return m_iCellH;}
protected:
	std::bitset<FOGMASK_RESOLUTION*FOGMASK_RESOLUTION> m_tileVisbleFlag;

	int m_iCellW;
	int m_iCellH;
	int m_iMapW;
	int m_iMapH;

//	abase::hash_map<int,A3DVECTOR3> m_teamMemLastPos; // 队友的上次位置
};


class CRandMapProp;

// 地形相关文件预处理管理类
class CECRandomMapProcess
{
	enum
	{
		PROC_TRN2 = 0,
		PROC_T2BK,
		NUM_PROCESSOR,
	};
	struct RepeatSubTrn
	{
		int trn;
		int index;
	};
public:
	CECRandomMapProcess():m_pMapProp(NULL){}
	~CECRandomMapProcess(){Release();}

public:
	void Init(int r,int c,int* pData);
	void Release(bool bOnlyReleaseProcessor = false);
	void DoProcess(const char* szMap);

	int GetSevSubTernIndex(int oldIndex);
	int GetNewSubTernIndexByOldIndex(int oldIndex) ;
	int GetOldSubTernIndexByNewIndex(int index);

	int GetBlockRowCountInSubTern() const { return trn2Info.iSubTrnRow == 0 ? 0 : trn2Info.iTrnBlkRow / trn2Info.iSubTrnRow;}
	int GetBlockColCountInSubTern() const { return trn2Info.iSubTrnCol == 0 ? 0 : trn2Info.iTrnBlkCol / trn2Info.iSubTrnCol;}

	int GetColCountSubTern() const { return trn2Info.iSubTrnCol;} 

	void GetNewBlockRowColInWorld(int iSubTrn,WORD& row,WORD& col);

	int GetSubTernCount() const { ASSERT((int)newSubTernOrder.size() == trn2Info.iSubTrnRow * trn2Info.iSubTrnCol); return newSubTernOrder.size();}

	void SetTrn2Info(const TRN2FILEHEADER& r) {trn2Info = r;};
	TRN2FILEHEADER GetTrn2Info() const { return trn2Info;}
	const AString& GetRealMapName() { return m_strMapName;}
	AString GetMapName();

	void ProcessRepeatSubTern();

	void ClearMapName() { m_strMapName.Empty();}
	
	void GenerateImage(abase::vector<A3DSurface*>& srcImgVec, const AString& path, const AString& fileName);

	void GenerateWorldMap();
	void GenerateMiniMap();

	void Tick(DWORD dt);

	const CECRandomMapFogMask& GetRandomMapFogMask() { return m_worldmapFogMask;}

	bool HaveClearMapFogItem();

	bool GetRegionName(const A3DVECTOR3& pos,ACString& name);
	void GenerateWorldMapBorder(const AString& srcpath,const AString& path, const AString& fileName);
	bool EncodeMap(const char* file,unsigned char*& pData,int& iLen);
	void SaveMapToFile(const char* desfile,unsigned char* pData,int iLen);

	static void DeleteRandomMapData(const char* szMap);
	static void DeleteAllRandomMapDataForSingleUser();
	static AString GetRandomMapName(const AString& strMap);

protected:
	bool GetRegionName(int idx,ACString& name);

protected:
	CECPreprocessor*		m_processor[NUM_PROCESSOR];

	abase::vector<int>		newSubTernOrder;
	abase::vector<int>		sevOrder;

//	int						m_iBlockRowInSubTern;
//	int						m_iBlockColInSubTern;

	TRN2FILEHEADER			trn2Info;

	AString					m_strMapName;
	int						m_iInstanceID;

	CECRandomMapFogMask		m_worldmapFogMask;
	CRandMapProp*			m_pMapProp;

private:
};


///////////////////////////////////////////////////////////////////////////////////
