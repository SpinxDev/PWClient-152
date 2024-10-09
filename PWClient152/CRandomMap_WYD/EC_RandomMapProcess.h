// Filename	: EC_WorldTernPreprocess.h
// Creator	: WYD
// Date		: 2014/01/14

#pragma once

#include <A3DTerrain2File.h>
#include <AString.h>
#include <vector.h>
#include <ABaseDef.h>
#include <AArray.h>
#include "EC_WorldFile.h"


class CECRandomMapProcess;


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

class CECWorldFilePreprocessor : public CECPreprocessor
{
public:
	CECWorldFilePreprocessor(CECRandomMapProcess* asset):CECPreprocessor(asset){}
	void DoProcess(const char* szMapName);
	void Release();

protected:
	bool LoadFile(const char* szMapName);
	
	int GetNewBlockIndexInWorldByOldIndex(int index);

	void LoadTree(FILE* fpWorld);
protected:
	ECWDFILEHEADER fileHeader;

	int blockInSubTrn;	
	int subTrnRowCount;
	int subTrnColCount;

	AArray<DWORD,DWORD> blockOffset;
	AArray<DWORD,DWORD> tempOffset;
private:

	float GetSubTernSize();
	void GetNewPosOffset(int r,int c,float& x,float& z)
	{
		float x1,z1,x2,z2;
		GetOffsetByRowCol(r,c,x1,z1);
		
		GetNewBlockRowCol(r,c);
		GetOffsetByRowCol(r,c,x2,z2);
		
		x = x2 - x1;
		z = z2 - z1;
	}
	void GetNewBlockRowCol(int& row,int& col)
	{
		int rowInSub,colInSub;
		rowInSub = row % GetBlockCountInSubTern();
		colInSub = col % GetBlockCountInSubTern();
		int subTernRow = row / GetBlockCountInSubTern(); 
		int subTernCol = col / GetBlockCountInSubTern();
		
		int oldSubTernIndex = subTernRow * GetSubTernColCount() + subTernCol;
		int newSubTernIndex = GetNewSubTernIndexByOldIndex(oldSubTernIndex);
		
		int newSubTernRow = newSubTernIndex / GetSubTernColCount();
		int newSubTernCol = newSubTernIndex % GetSubTernColCount();
		
		row = rowInSub + newSubTernRow * GetBlockCountInSubTern();
		col = colInSub + newSubTernCol * GetBlockCountInSubTern();
		
	}
	int GetNewSubTernIndexByOldIndex(int old);
	void GetOffsetByRowCol(int row,int col,float& cx,float& cz);
	int GetSubTernColCount();
	int GetSubTernRowCount();
	void GetSubTernByBlockRowCol(int& r,int& c);
	int GetBlockCountInSubTern();

};

//////////////////////////////////////////////////////////////////////////

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

class CECRandomMapProcess
{
	enum
	{
		PROC_TRN2 = 0,
		PROC_WORLD,
		PROC_T2BK,
		NUM_PROCESSOR,
	};
	struct RepeatSubTrn
	{
		int trn;
		int index;
	};
public:
	CECRandomMapProcess(){}
	~CECRandomMapProcess(){Release();}
	void Init();
	void Release();
	void DoProcess(const char* szMap);
	void SetNewSubTernMap(const abase::vector<int>& newSubMap) { newSubTernOrder = newSubMap;}

	int GetNewSubTernIndexByOldIndex(int oldIndex) ;
	int GetNewSubTernIndex(int index) ;
	void SetBlockRowCountInSubTern(int r) { m_iBlockRowInSubTern = r;}
	void SetBlockColCountInSubTern(int c) { m_iBlockColInSubTern = c;}
	int GetBlockRowCountInSubTern() const { return m_iBlockRowInSubTern;}
	int GetBlockColCountInSubTern() const { return m_iBlockColInSubTern;}

//	void SetRowCountSubTern(int r) { m_iRowCountSubTern = r;}
//	void SetColCountSubTern(int c) { m_iColCountSubTern = c;}
//	int GetRowCountSubTern() const { return m_iRowCountSubTern;}
	int GetColCountSubTern() const { return trn2Info.iSubTrnCol;} 

//	void GetNewRowColInWorld(WORD& row,WORD& col,int& index); // for block
	void GetNewBlockRowColInWorld(int iSubTrn,WORD& row,WORD& col);
	void GetSubTrnRowColInWorldByIndex(int index,int& row,int& col);
	void GetNewBlockIndexInWorldByOldIndex(int& index);


	int GetSubTernCount() const { ASSERT((int)newSubTernOrder.size() == trn2Info.iSubTrnRow * trn2Info.iSubTrnCol); return newSubTernOrder.size();}
	void SetTrn2Info(const TRN2FILEHEADER& r) {trn2Info = r;};
	TRN2FILEHEADER GetTrn2Info() const { return trn2Info;}
	const AString& GetMapName() { return m_strMapName;}

	void ProcessRepeatSubTern();

	bool IsRandomMap() { return m_strMapName.Find("a21")>=0;}

protected:
	CECPreprocessor* m_processor[NUM_PROCESSOR];
	abase::vector<int> newSubTernOrder;

	int m_iBlockRowInSubTern;
	int m_iBlockColInSubTern;
//	int m_iRowCountSubTern;
//	int m_iColCountSubTern;

	TRN2FILEHEADER trn2Info;

	AString m_strMapName;
private:
};