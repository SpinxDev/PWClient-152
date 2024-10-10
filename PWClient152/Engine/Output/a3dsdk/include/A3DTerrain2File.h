/*
 * FILE: A3DTerrain2File.h
 *
 * DESCRIPTION: File data structure used by A3DTerrain2LoaderA
 *
 * CREATED BY: Duyuxin, 2004/7/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2FILE_H_
#define _A3DTERRAIN2FILE_H_

#include "ABaseDef.h"

///////////////////////////////////////////////////////////////////////////
//	
//	A3D terrain2 general information file format description
//	
///////////////////////////////////////////////////////////////////////////

//	Identity and version of terrain2 general information file
const DWORD TRN2FILE_IDENTIFY	= (('T'<<24) | ('R'<<16) | ('N'<<8) | '2');
const DWORD TRN2FILE_VERSION	= 1;

//	Identity and version of terrain2 light map file
const DWORD T2LMFILE_IDENTIFY	= (('T'<<24) | ('2'<<16) | ('L'<<8) | 'M');
const DWORD T2LMFILE_VERSION	= 2;

/*	Terrain 2 general description file format:

		TRN2FILEIDVER					| file ID and version
		--------------------------------|
		TRN2FILEHEADER					| terrain file header
		--------------------------------|
		TRN2FILESUBTRN #0				| sub-terrain #0 information
		...								|
		TRN2FILESUBTRN #n				| sub-terrain #n information
		--------------------------------|
		texture #0 file name			| texture file name ...
		...								|
		texture #n file name			|
		--------------------------------|
		Loader data						| this block is parsed by loader
		--------------------------------|

  ====================================================================

	Terrain 2 light map file format:

	Version 1:

		TRN2FILEIDVER					| file ID and version
		--------------------------------|
		T2LMFILEHEADER					| light map file header
		--------------------------------|
		iNumMaskArea * DWORD			| each mask area's light map data offset in file
		--------------------------------|
		mask area #0 light map data		| Data of each mask area.
		...								| Note: Don't assume light map data is
		mask area #n light map data		|   ordered by mask area indices.
		--------------------------------|
*/

//	Terrain file ID and cersion
struct TRN2FILEIDVER
{
	DWORD	dwIdentify;		//	Identity, must be TRN2FILE_IDENTIFY
	DWORD	dwVersion;		//	Version, must be TRN2FILE_VERSION
};

//	File header when TRN2FILE_VERSION == 1
struct TRN2FILEHEADER
{
	float	fTerrainWid;	//	Terrain width in meters
	float	fTerrainHei;	//	Terrain height in meters
	float	fBlockSize;		//	Block size in meters
	int		iBlockGrid;		//	Block grid row and column number, each block has (grid x grid) terrain grids
	int		iNumBlock;		//	Number of block
	int		iTrnBlkRow;		//	Block row number in whole terrain
	int		iTrnBlkCol;		//	Block column number in whole terrain
	int		iNumTexture;	//	Number of texture
	int		iNumMaskArea;	//	Number of mask area
	int		iMaskBlock;		//	Each mask area has iMaskBlock * iMaskBlock blocks
	int		iMaskGrid;		//	Mask grid row and column number, each mask area has (grid x grid) terrain grids
	int		iNumSubTrn;		//	Number of sub-terrain
	int		iSubTrnRow;		//	Row number of sub-terrain
	int		iSubTrnCol;		//	Column number of sub-terrain
	int		iSubTrnGrid;	//	Each sub-terrain has iSubTrnGrid * iSubTrnGrid grids
	float	fGridSize;		//	Terrain grid size in meters
	DWORD	dwTexOffset;	//	Texture file name list's offset in file
	int		iLoaderID;		//	Terrain loader ID
	DWORD	dwLoaderOff;	//	Offset of loader's data in file
	BYTE	aReserved[56];	//	Reserved
};

//	Sub-terrain information
struct TRN2FILESUBTRN
{
	float	fMinHei;		//	Minimum height
	float	fMaxHei;		//	Maximum height
};

//	Block information
struct T2BKFILEBLOCK
{
	int		iMaskArea;		//	Index of mask area this block blongs to
	WORD	iRowInTrn;		//	Row of this block in whole terrain
	WORD	iColInTrn;		//	Column of this block in whole terrain
	float	fLODScale;		//	LOD scale factor
	DWORD	dwLayerFlags;	//	Effect layer flags
};

//	Light map file header when T2LMFILE_VERSION < 2
struct T2LMFILEHEADER
{
	int		iNumMaskArea;	//	Number of mask area
	int		iMaskLMSize;	//	Each mask area's light map size (in pixels)
};

//	Lightmap file flags
enum
{
	T2LMFILE_DDS	= 0x0001,
};

//	Light map file header when T2LMFILE_VERSION >= 2
struct T2LMFILEHEADER2
{
	int		iNumMaskArea;	//	Number of mask area
	int		iMaskLMSize;	//	Each mask area's light map size (in pixels)
	DWORD	dwFlags;		//	Flags
	int		iLMDataSize;	//	Lightmap data length (in bytes) of each mask area
};

///////////////////////////////////////////////////////////////////////////
//	
//	A3D terrain2 A3DTerrain2LoaderA file format description
//	
///////////////////////////////////////////////////////////////////////////

namespace A3DTRN2LOADERA
{
/*	Terrain 2 block data file format:
	Note: Block data file for A3DTerrain2LoaderA is just the general
		information file, they are the same file.

	Version 1:
	
		TRN2FILEIDVER					| file ID and version
		--------------------------------|
		TRN2FILEHEADER					| terrain file header
		--------------------------------|
		TRN2FILESUBTRN #0				| sub-terrain #0 information
		...								|
		TRN2FILESUBTRN #n				| sub-terrain #n information
		--------------------------------|
		iNumBlock * DWORD				| each block data's offset in file
		--------------------------------|
		T2BKFILEBLOCK #0				| Data of each block.
		block #0 data					| Note: Don't assume block data is ordered
		...								|   by their indices.
		T2BKFILEBLOCK #n				|
		block #n data					|
		--------------------------------|
		texture #0 file name			| texture file name ...
		...								|
		texture #n file name			|
		--------------------------------|

  ====================================================================

	Terrain 2 mask map file format:

	Version 1:

		TRN2FILEIDVER					| file ID and version
		--------------------------------|
		T2MKFILEHEADER					| mask file header
		--------------------------------|
		iNumMaskArea * DWORD			| each mask area data's offset in file
		--------------------------------|
		TRN2FILEMASKAREA #0				| Data of each mask area.
		mask area #0 data				| Note: Don't assume mask area data is
		...								|   ordered by their indices.
		TRN2FILEMASKAREA #n				|
		mask area #n data				|
		--------------------------------|
		int stream						| texture index lump
		--------------------------------|
		int stream						| mask map image size lump
		--------------------------------|
		DWORD stream					| mask map image data offset lump
		--------------------------------|
		BYTE stream						| projection axis lump
		--------------------------------|
		T2MKFILEUVSCALE stream			| uv scale lump
		--------------------------------|
		BYTE stream						| layer weight lump
		--------------------------------|
*/

//	Identity and version of terrain2 mask map file
const DWORD T2MKFILE_IDENTIFY	= (('T'<<24) | ('2'<<16) | ('M'<<8) | 'K');
const DWORD T2MKFILE_VERSION	= 3;

//	Lump in mask map file
enum
{
	T2MKLUMP_TEXTURE = 0,	//	Texture index lump
	T2MKLUMP_SPECUMAP,		//	Specular map index lump
	T2MKLUMP_MASKSIZE,		//	Mask map size lump
	T2MKLUMP_OFFSET,		//	Mask map image data offset
	T2MKLUMP_PROJAXIS,		//	Layer project axis
	T2MKLUMP_UVSCALE,		//	UV scale
	T2MKLUMP_WEIGHT,		//	Layer weight
	NUM_T2MKLUMP,
};

//	Block data file header
struct T2BKFILEHEADER
{
};

//	Lump data in mask file
struct T2MKFILELUMP
{
	DWORD	dwOff;			//	Offset from file header to lump's data
	DWORD	dwSize;			//	Size of this lump
};

//	Mask map file header 
struct T2MKFILEHEADER
{
	int		iNumMaskArea;	//	Number of mask area

	T2MKFILELUMP	aLumps[NUM_T2MKLUMP];	//	Lump infromation
};

//	Mask area information
struct T2MKFILEMASKAREA
{
	int		iNumLayer;		//	Number of layer in this area
	int		iFirstIndex;	//	First index of mask area's data in thest lumps:
							//		texture, mask size, offset, project axis and uv-scale.
};

//	UV scale lump in mask area file
struct T2MKFILEUVSCALE
{
	float	su;
	float	sv;
};

}	//	A3DTRN2LOADERA

///////////////////////////////////////////////////////////////////////////
//	
//	A3D terrain2 A3DTerrain2LoaderB file format description
//	
///////////////////////////////////////////////////////////////////////////

namespace A3DTRN2LOADERB
{
/*	Terrain 2 block data file format:
	Note: Block data file for A3DTerrain2LoaderA is just the general
		information file, they are the same file.

	Version 1:
	
		TRN2FILEIDVER					| file ID and version
		--------------------------------|
		T2BKFILEHEADER					| terrain file header
		--------------------------------|
		iNumBlock * DWORD				| each block data's offset in file
		--------------------------------|
		T2BKFILEBLOCK #0				| Data of each block.
		block #0 data					| Note: Don't assume block data is ordered
		...								|   by their indices.
		T2BKFILEBLOCK #n				|
		block #n data					|
		--------------------------------|
		texture #0 file name			| texture file name ...
		...								|
		texture #n file name			|
		--------------------------------|

  ====================================================================

	Terrain 2 mask map file format:

	Version 1:

		TRN2FILEIDVER					| file ID and version
		--------------------------------|
		T2MKFILEHEADER					| mask file header
		--------------------------------|
		iNumMaskArea * DWORD			| each mask area data's offset in file
		--------------------------------|
		T2MKFILELAYER #0				| layer information
		...								|
		T2MKFILELAYER #n				|
		--------------------------------|
		TRN2FILEMASKAREA #0				| Data of each mask area.
		mask area #0 data				| Note: Don't assume mask area data is
		...								|   ordered by their indices.
		TRN2FILEMASKAREA #n				|
		mask area #n data				|
		--------------------------------|
*/

//	Identity and version of terrain2 block data file
const DWORD T2BKFILE_IDENTIFY	= (('T'<<24) | ('B'<<16) | ('K'<<8) | 'B');
const DWORD T2BKFILE_VERSION	= 6;

//	Identity and version of terrain2 mask map file
const DWORD T2MKFILE_IDENTIFY	= (('T'<<24) | ('M'<<16) | ('K'<<8) | 'B');
const DWORD T2MKFILE_VERSION	= 3;

//	Block data compress flags
enum
{
	T2BKCOMP_COL_ZIP	= 0x01,		//	ZIP compression for color
//	T2BKCOMP_COL_16B	= 0x02,		//	16-bit compression for color
	T2BKCOMP_HEI_ZIP	= 0x04,		//	ZIP compression for vertex height
	T2BKCOMP_HEI_INC	= 0x08,		//	Increamental compression for vertex height
	T2BKCOMP_NOR_ZIP	= 0x10,		//	ZIP compression for normal
	T2BKCOMP_NOR_DEG	= 0x20,		//	Degree compression for normal
};

//	Block flags & masks
enum
{
	T2BKFLAG_DEFAULT			= 0x00,		//	The block flag default value
	T2BKFLAG_NORENDER			= 0x01,		//	The whole block is not rendered
	T2BKFLAG_NOTRACE			= 0x02,		//	The whole block is out of RayTrace
	T2BKFLAG_NOPOSHEIGHT		= 0x04,		//	The whole block is unable to GetPosHeight
	T2BKFLAG_NORENDERWITHWATER	= 0x08,		//	The whole block is not rendered with water
};

//	Loader data section in description file
struct TRN2FILELOADERINFO
{
	int		iMaskMapSize;	//	Mask map size
};

//	Block data file header, T2BKFILE_VERSION < 2
struct T2BKFILEHEADER
{
	int		iNumBlock;		//	Number of block
};

//	Block data file header, T2BKFILE_VERSION >= 2
struct T2BKFILEHEADER2
{
	int		iNumBlock;		//	Number of block
	bool	bCompressed;	//	Data compression flag
};

//	Block data file header, T2BKFILE_VERSION >= 5
struct T2BKFILEHEADER5
{
	int		iNumBlock;		//	Number of block
	BYTE	byCompressed;	//	Compression flag
};

//	Block information, T2BKFILE_VERSION >= 2
struct T2BKFILEBLOCK2
{
	int		iMaskArea;		//	Index of mask area this block blongs to
	WORD	iRowInTrn;		//	Row of this block in whole terrain
	WORD	iColInTrn;		//	Column of this block in whole terrain
	float	fLODScale;		//	LOD scale factor
	DWORD	dwLayerFlags;	//	Effect layer flags
	DWORD	dwHeiSize;		//	Height data size
	DWORD	dwNormalSize;	//	Normal data size
};

//	Block information, T2BKFILE_VERSION >= 3
struct T2BKFILEBLOCK3
{
	int		iMaskArea;		//	Index of mask area this block blongs to
	WORD	iRowInTrn;		//	Row of this block in whole terrain
	WORD	iColInTrn;		//	Column of this block in whole terrain
	float	fLODScale;		//	LOD scale factor
	DWORD	dwLayerFlags;	//	Effect layer flags
	DWORD	dwHeiSize;		//	Height data size
	DWORD	dwNormalSize;	//	Normal data size
	DWORD	dwDiffSize;		//	Diffuse data size
};

//	Block information, T2BKFILE_VERSION >= 4
struct T2BKFILEBLOCK4
{
	int		iMaskArea;		//	Index of mask area this block blongs to
	WORD	iRowInTrn;		//	Row of this block in whole terrain
	WORD	iColInTrn;		//	Column of this block in whole terrain
	float	fLODScale;		//	LOD scale factor
	DWORD	dwLayerFlags;	//	Effect layer flags
	DWORD	dwHeiSize;		//	Height data size
	DWORD	dwNormalSize;	//	Normal data size
	DWORD	dwDiffSize;		//	Diffuse (day light) data size
	DWORD	dwDiffSize1;	//	Diffuse (night light) data size
};

//	Block information, T2BKFILE_VERSION >= 6
struct T2BKFILEBLOCK6
{
	int		iMaskArea;		//	Index of mask area this block blongs to
	WORD	iRowInTrn;		//	Row of this block in whole terrain
	WORD	iColInTrn;		//	Column of this block in whole terrain
	float	fLODScale;		//	LOD scale factor
	DWORD	dwLayerFlags;	//	Effect layer flags
	DWORD	dwHeiSize;		//	Height data size
	DWORD	dwNormalSize;	//	Normal data size
	DWORD	dwDiffSize;		//	Diffuse (day light) data size
	DWORD	dwDiffSize1;	//	Diffuse (night light) data size
	DWORD	dwBlkFlags;		/*  
								dwBlkFlags & T2BKFLAG_NORENDER		The whole block is not rendered
								dwBlkFlags & T2BKFLAG_NOTRACE		The whole block is out of RayTrace
								dwBlkFlags & T2BKFLAG_NOPOSHEIGHT	The whole block is unable to GetPosHeight
							*/
};

//	Mask map file header, T2MKFILE_VERSION < 2
struct T2MKFILEHEADER
{
	int		iNumMaskArea;	//	Number of mask area
	int		iNumLayer;		//	Number of layer
};

//	Mask map file header, T2MKFILE_VERSION >= 2
struct T2MKFILEHEADER2
{
	int		iNumMaskArea;	//	Number of mask area
	int		iNumLayer;		//	Number of layer
	bool	bCompressed;	//	true, data is compressed
};

//	Layer information
struct T2MKFILELAYER
{
	int		iTextureIdx;	//	Texture index
	int		iSpecMapIdx;	//	Specular map index
	int		iMaskMapSize;	//	Mask map size
	BYTE	byProjAxis;		//	Texture project axis
	float	fUScale;		//	U scale of texture
	float	fVScale;		//	V scale of texture
	BYTE	byWeight;		//	Layer weight
};

//	Mask area information
struct T2MKFILEMASKAREA
{
	int		iNumLayer;		//	Number of layer in this area
};

//	Layer information of each mask area, T2MKFILE_VERSION < 2
struct T2MKFILEMASKLAYER
{
	int		iLayer;			//	Layer index in whole terrain
	DWORD	dwMapDataOff;	//	Layer mask map data offset in file
};

//	Layer information of each mask area, T2MKFILE_VERSION >= 2
struct T2MKFILEMASKLAYER2
{
	int		iLayer;			//	Layer index in whole terrain
	DWORD	dwMapDataOff;	//	Layer mask map data offset in file
	DWORD	dwDataSize;		//	Layer mask map data size in file
};

}

#endif	//	_A3DTERRAIN2FILE_H_

