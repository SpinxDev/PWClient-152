#include "A3DTerrain.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DLight.h"
#include "A3DCameraBase.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DStream.h"
#include "A3DViewport.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"
#include "AFI.h"

//#define COLOR_TERRAIN

A3DTerrain::A3DTerrain()
{
	m_pA3DDevice			= NULL;
	m_pA3DCamera			= NULL;
	m_pA3DViewport			= NULL;

	m_bHWITerrain			= false;
	m_bNoLight				= false;

	ZeroMemory(m_pStreams, sizeof(A3DStream *) * MAXNUM_TEXTURE);
	ZeroMemory(m_pTextures, sizeof(A3DTexture *) * MAXNUM_TEXTURE);
	ZeroMemory(m_pVertexBuffers, sizeof(A3DLVERTEX *) * MAXNUM_TEXTURE);
	ZeroMemory(m_pIndexBuffers, sizeof(WORD *) * MAXNUM_TEXTURE);
	ZeroMemory(m_pExtraVertexBuffers, sizeof(A3DLVERTEX *) * MAXNUM_TEXTURE);
	ZeroMemory(m_pExtraIndexBuffers, sizeof(WORD *) * MAXNUM_TEXTURE);

	ZeroMemory(m_nMaxVerts, sizeof(int) * MAXNUM_TEXTURE);
	ZeroMemory(m_nMaxIndices, sizeof(int) * MAXNUM_TEXTURE);
	ZeroMemory(m_nMaxExtraVerts, sizeof(int) * MAXNUM_TEXTURE);
	ZeroMemory(m_nMaxExtraIndices, sizeof(int) * MAXNUM_TEXTURE);

	m_ppCellLevelTable = NULL;
	m_ppCellSquareErrorTable = NULL;

	m_pCellDistanceTable = NULL;

	m_pDetailTexture = NULL;
	m_pDetailVB = NULL;
	m_pDetailIndex = NULL;
	m_pDetailStream = NULL;

	m_pHeightBuffer = NULL;
	m_pTextureBuffer = NULL;

	m_pVertexColorBuffer = NULL;
	m_pFaceNormalBuffer = NULL;
	m_pVertexNormalBuffer = NULL;

	m_nNumTexture = 0;

	m_bShowWire = false;
	m_bShowTerrain = true;

	m_nVisibleBeginXOld = -100000;
	m_nVisibleBeginYOld = -100000;

	m_ptCamPosOld.x = -100000;
	m_ptCamPosOld.y = -100000;

	m_bRayTraceEnable	= true;
	m_bAABBTraceEnable	= true;

	m_pDirLight = NULL;
	m_vOptimizeStrength = 1.0f;

	m_bHasHollowRect = false;

	m_nDetailCellCount = 0;
}

A3DTerrain::~A3DTerrain()
{
}

bool A3DTerrain::Init(A3DDevice * pA3DDevice, TERRAINPARAM* pTerrainParam, char * szTexture, char * szHeightMap, char * szTextureMap)
{
	int		i, j;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		m_bHWITerrain = true;

	strcpy(m_szFolderName, "Terrain");

	m_pA3DDevice = pA3DDevice;
	m_nWidth = pTerrainParam->nWidth;
	m_nHeight = pTerrainParam->nHeight;

	if( pTerrainParam->nSightRange == 0 )
		m_nSightRange = 80;
	else
		m_nSightRange = pTerrainParam->nSightRange;
	
	if( pTerrainParam->nTextureCover == 0 )
		m_nTextureCover = 8;
	else
		m_nTextureCover = pTerrainParam->nTextureCover;

	if( pTerrainParam->vCellSize == 0.0f )
		m_vCellSize = 20.0f;
	else
		m_vCellSize = pTerrainParam->vCellSize;

	m_vOffset = pTerrainParam->vOffset;
	m_vScale = pTerrainParam->vScale;

	m_vecPos = A3DVECTOR3(-m_nWidth / 2 * m_vCellSize, 0.0f, m_nHeight / 2 * m_vCellSize);

	m_nNumTexture = pTerrainParam->nNumTexture;

	strncpy(m_szTextureBaseName, szTexture, MAX_PATH);
	strncpy(m_szHeightMapName, szHeightMap, MAX_PATH);
	strncpy(m_szTextureMapName, szTextureMap, MAX_PATH);

	// Height section;
	FILE * file;
	char szFullpath[MAX_PATH];
	af_GetFullPath(szFullpath, m_szFolderName, m_szHeightMapName);

	file = fopen(szFullpath, "rb");
	if( NULL == file )
	{
		g_A3DErrLog.Log("A3DTerrain::Init Can not open height map: %s", szFullpath);
		return false;
	}

	m_pHeightBuffer = (FLOAT *) malloc((m_nWidth + 1) * (m_nHeight + 1) * sizeof(FLOAT));
	if( NULL == m_pHeightBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");

		fclose(file);
		return false;
	}

	m_vMaxHeight = -10000.0f;
	m_vMinHeight = 100000.0f;

	//	Read the height map;
	if (af_CheckFileExt(m_szHeightMapName, ".h16", 4))
	{
		for(int y=0; y<m_nHeight + 1; y++)
		{
			for(int x=0; x<m_nWidth + 1; x++)
			{
				BYTE byteFirst, byteSecond;
				WORD wordHeight;
				FLOAT vHeight;
				fread(&byteFirst, 1, 1, file);
				fread(&byteSecond, 1, 1, file);
				wordHeight = byteFirst << 8 | byteSecond; 
				vHeight = m_vOffset + m_vScale * wordHeight;

				m_pHeightBuffer[y * (m_nWidth + 1) + x] = vHeight;

				if( m_vMaxHeight < vHeight )
					m_vMaxHeight = vHeight;
				if( m_vMinHeight > vHeight )
					m_vMinHeight = vHeight;
			}
		}
	}
	else if (af_CheckFileExt(m_szHeightMapName, ".bin", 4))
	{
		for(int y=0; y<m_nHeight + 1; y++)
		{
			for(int x=0; x<m_nWidth + 1; x++)
			{
				FLOAT vHeight;
				fread(&vHeight, sizeof(FLOAT), 1, file);
				
				m_pHeightBuffer[y * (m_nWidth + 1) + x] = vHeight;

				if( m_vMaxHeight < vHeight )
					m_vMaxHeight = vHeight;
				if( m_vMinHeight > vHeight )
					m_vMinHeight = vHeight;
			}
		}
	}
	else
	{
		for(int y=0; y<m_nHeight + 1; y++)
		{
			for(int x=0; x<m_nWidth + 1; x++)
			{
				BYTE byteHeight;
				FLOAT vHeight;
				fread(&byteHeight, 1, 1, file);
				vHeight = byteHeight * m_vScale + m_vOffset;

				m_pHeightBuffer[y * (m_nWidth + 1) + x] = vHeight;

				if( m_vMaxHeight < vHeight )
					m_vMaxHeight = vHeight;
				if( m_vMinHeight > vHeight )
					m_vMinHeight = vHeight;
			}
		}
	}

	/*
	for(int y=0; y<m_nHeight; y++)
	{
		for(int x=0; x<m_nWidth; x++)
		{
			WORD wordHeight;
			fread(&wordHeight, 1, 2, file);
			m_pHeightBuffer[y * (m_nWidth + 1) + x] = wordHeight * 0.1f;
		}
	}*/

	fclose(file);
	file = NULL;

	//Normal sections;
	//Now calculate normal information;
	m_pFaceNormalBuffer = (A3DVECTOR3 *) malloc(m_nWidth * m_nHeight * 2 * sizeof(A3DVECTOR3));
	if( NULL == m_pFaceNormalBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
		return false;
	}
	m_pVertexNormalBuffer = (A3DVECTOR3 *) malloc((m_nWidth + 1) * (m_nHeight + 1) * sizeof(A3DVECTOR3));
	if( NULL == m_pVertexNormalBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
		return false;
	}
	BuildNormals();

	// Load texture index;
	af_GetFullPath(szFullpath, m_szFolderName, m_szTextureMapName);
	file = fopen(szFullpath, "rb");
	if( NULL == file )
	{
		g_A3DErrLog.Log("A3DTerrain::Init Can not open texture map: %s", szFullpath);
		return false;
	}	
	
	m_pTextureBuffer = (LPBYTE) malloc((m_nWidth / m_nTextureCover) * (m_nHeight / m_nTextureCover) * sizeof(BYTE));
	if( NULL == m_pTextureBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");

		fclose(file);
		return false;
	}

	fread(m_pTextureBuffer, (m_nWidth / m_nTextureCover) * (m_nHeight / m_nTextureCover) * sizeof(BYTE), 1, file);
	fclose(file);
	file = NULL;

	for(i=0; i<m_nHeight / m_nTextureCover; i++)
	{
		for(j=0; j<m_nWidth / m_nTextureCover; j++)
		{
			if( m_pTextureBuffer[i * (m_nWidth / m_nTextureCover) + j] >= m_nNumTexture )
			{
				g_A3DErrLog.Log("A3DTerrain::Init The texture map contained more than m_nNumTexture index, pos[%d, %d]", j, i);
				return false;
			}
		}
	}

	if( m_bHWITerrain )
	{
	}
	else
	{
		// Init stage level table;
		switch(m_nTextureCover)
		{
		case 2:
			m_nCellStageCount = 1;
			break;
		case 4:
			m_nCellStageCount = 2;
			break;
		case 8:
			m_nCellStageCount = 3;
			break;
		case 16:
			m_nCellStageCount = 4;
			break;
		default:
			g_A3DErrLog.Log("Too large texture cover [%d] or it is not a power of 2! The max is 16!", m_nTextureCover);
			return false;
		}

		if( (m_nWidth % m_nTextureCover) || (m_nHeight % m_nTextureCover) )
		{
			g_A3DErrLog.Log("m_nWidth[%d] and m_nHeight[%d] must be dividable by m_nTextureCover[%d]!", m_nWidth, m_nHeight, m_nTextureCover);
			return false;
		}

		//Allocate the stage cell table;
		m_ppCellLevelTable = (CHAR **) malloc(sizeof(CHAR *) * m_nCellStageCount);
		if( NULL == m_ppCellLevelTable )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not enough memory!");
			return false;
		}
		m_ppCellSquareErrorTable = (FLOAT **) malloc(sizeof(FLOAT *) * m_nCellStageCount);
		if( NULL == m_ppCellSquareErrorTable )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not enough memory!");
			return false;
		}

		int nStageWidth = m_nWidth;
		int nStageHeight = m_nHeight;
		int nCover = 1;
		for(i=0; i<m_nCellStageCount; i++)
		{
			nStageWidth	/= 2;
			nStageHeight /= 2;
			nCover *= 2;

			m_nStageWidth[i] = nStageWidth;
			m_nStageHeight[i] = nStageHeight;
			m_nStageCover[i] = nCover;

			m_ppCellLevelTable[i] = (CHAR *) malloc(nStageWidth * nStageHeight * sizeof(CHAR));
			if( NULL == m_ppCellLevelTable[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not enough memory!");
				return false;
			}
			//First set all cell's level to be -1;
			memset(m_ppCellLevelTable[i], 0xff, nStageWidth * nStageHeight);

			m_ppCellSquareErrorTable[i] = (FLOAT *) malloc(nStageWidth * nStageHeight * sizeof(FLOAT));
			if( NULL == m_ppCellSquareErrorTable[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not enough memory!");
				return false;
			}
		}

		if( m_nSightRange % m_nTextureCover )
		{
			g_A3DErrLog.Log("A3DTerrain::Init m_nSightRange must be dividable by m_nTextureCover");
			return false;
		}

		char	szMapFile[MAX_PATH];

		for(i=0; i<m_nNumTexture; i++)
		{
			m_nMaxVerts[i] = m_nMaxExtraVerts[i] = 6 * 8 * 8;
			m_nMaxIndices[i] = m_nMaxExtraIndices[i] = 6 * 8 * 8;
			 
			m_pVertexBuffers[i] = (A3DLVERTEX *) malloc(m_nMaxVerts[i] * sizeof(A3DLVERTEX));
			if( NULL == m_pVertexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}
			m_pIndexBuffers[i] = (WORD *) malloc(m_nMaxIndices[i] * sizeof(WORD));
			if( NULL == m_pIndexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}
			m_pExtraVertexBuffers[i] = (A3DLVERTEX *) malloc(m_nMaxExtraVerts[i] * sizeof(A3DLVERTEX));
			if( NULL == m_pExtraVertexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}
			m_pExtraIndexBuffers[i] = (WORD *) malloc(m_nMaxExtraIndices[i] * sizeof(WORD));
			if( NULL == m_pIndexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}

			sprintf(szMapFile, "%s%02d.bmp", m_szTextureBaseName, i);
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szMapFile, "Textures\\Terrain", &m_pTextures[i]) )
				return false;
			
			/*
			m_pStreams[i] = new A3DStream();
			if( NULL == m_pStreams[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}
			if( !m_pStreams[i]->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxVertexCount, m_nMaxIndexCount, 0, true, true) )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Stream Init Fail!");
				return false;
			}*/

			m_nVisibleVerts[i] = m_nVisibleFaces[i] = 0;
		}

		//Allocate cell distance table;
		m_pCellDistanceTable = (FLOAT *) malloc(m_nWidth * m_nHeight * sizeof(FLOAT));
		if( NULL == m_pCellDistanceTable )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not enough memory!");
			return false;
		}

		CalculateDistanceTable();

		//Now calculate the square error of each cell;
		CalculateSquareError();

		//Now Prelight the terrain;
		m_pVertexColorBuffer = (A3DCOLOR *) malloc((m_nWidth + 1) * (m_nHeight + 1) * sizeof(A3DCOLOR));
		if( NULL == m_pVertexColorBuffer )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
			return false;
		}

		//Light the terrain;
		LightTerrain();

		//Init Detail Texture Sepcification Now!
		m_nDetailWidth = 8;
		m_nDetailHeight = 8;
		if( m_pA3DDevice->IsDetailMethodSupported() )
		{
			strcpy(m_szDetailTextureName, "DetailTexture.bmp");
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szDetailTextureName, "Textures\\Terrain", &m_pDetailTexture, A3DTF_DETAILMAP) )
				return false;

			m_nDetailVertCount = (m_nDetailWidth + 1) * (m_nDetailHeight + 1);
			m_nDetailFaceCount = 2 * m_nDetailWidth * m_nDetailHeight;

			m_pDetailVB = (A3DLVERTEX *) malloc(m_nDetailVertCount * sizeof(A3DLVERTEX));
			if( NULL == m_pDetailVB )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}
			m_pDetailIndex = (WORD *) malloc(m_nDetailFaceCount * 3 * sizeof(WORD));
			if( NULL == m_pDetailIndex )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}
			/*
			m_pDetailStream = new A3DStream();
			if( NULL == m_pDetailStream )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
				return false;
			}
			if( !m_pDetailStream->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nDetailVertCount, m_nDetailFaceCount * 3, 0, true, true) )
			{
				g_A3DErrLog.Log("A3DTerrain::Init Init Detail Stream Fail!");
				return false;
			}*/
		}
	}

	m_matWorld.Identity();
	return true;
}

bool A3DTerrain::Release()
{
	int			i;

	if( m_pCellDistanceTable )
	{
		free(m_pCellDistanceTable);
		m_pCellDistanceTable = NULL;
	}

	if( m_ppCellLevelTable )
	{
		for(i=0; i<m_nCellStageCount; i++)
		{
			if( m_ppCellLevelTable[i] )
			{
				free(m_ppCellLevelTable[i]);
				m_ppCellLevelTable[i] = NULL;
			}
		}
		free(m_ppCellLevelTable);
		m_ppCellLevelTable = NULL;
	}

	if( m_ppCellSquareErrorTable )
	{
		for(i=0; i<m_nCellStageCount; i++)
		{
			if( m_ppCellSquareErrorTable[i] )
			{
				free(m_ppCellSquareErrorTable[i]);
				m_ppCellSquareErrorTable[i] = NULL;
			}
		}
		free(m_ppCellSquareErrorTable);
		m_ppCellSquareErrorTable = NULL;
	}

	if( m_pVertexColorBuffer )
	{
		free(m_pVertexColorBuffer);
		m_pVertexColorBuffer = NULL;
	}

	if( m_pVertexNormalBuffer )
	{
		free(m_pVertexNormalBuffer);
		m_pVertexNormalBuffer = NULL;
	}

	if( m_pFaceNormalBuffer )
	{
		free(m_pFaceNormalBuffer);
		m_pFaceNormalBuffer = NULL;
	}

	if( m_pDetailTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pDetailTexture);
		m_pDetailTexture = NULL;
	}

	if( m_pDetailStream )
	{
		m_pDetailStream->Release();
		delete m_pDetailStream;
		m_pDetailStream = NULL;
	}

	if( m_pDetailVB )
	{
		free(m_pDetailVB);
		m_pDetailVB = NULL;
	}

	if( m_pDetailIndex )
	{
		free(m_pDetailIndex);
		m_pDetailIndex = NULL;
	}

	for(i=0; i<MAXNUM_TEXTURE; i++)
	{
		if( m_pVertexBuffers[i] )
		{
			free(m_pVertexBuffers[i]);
			m_pVertexBuffers[i] = NULL;
		}
		if( m_pIndexBuffers[i] )
		{
			free(m_pIndexBuffers[i]);
			m_pIndexBuffers[i] = NULL;
		}
		if( m_pExtraVertexBuffers[i] )
		{
			free(m_pExtraVertexBuffers[i]);
			m_pExtraVertexBuffers[i] = NULL;
		}
		if( m_pExtraIndexBuffers[i] )
		{
			free(m_pExtraIndexBuffers[i]);
			m_pExtraIndexBuffers[i] = NULL;
		}
		if( m_pStreams[i] )
		{
			m_pStreams[i]->Release();
			delete m_pStreams[i];
			m_pStreams[i] = NULL;
		}
		if( m_pTextures[i] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextures[i]);
			m_pTextures[i] = NULL;
		}
	}

	if( m_pHeightBuffer )
	{
		free(m_pHeightBuffer);
		m_pHeightBuffer = NULL;
	}

	if( m_pTextureBuffer )
	{
		free(m_pTextureBuffer);
		m_pTextureBuffer = NULL;
	}

	return true;
}

bool A3DTerrain::SmoothConnection(int x, int y, int sx, int sy, int nTexIndex, int nOldVertCount, int nOldFaceCount)
{
	int nTexNBRight, nTexNBBottom, nTexNBRB;
	int nDiffTex[3];
	int nDiffCount = 0;

	//Start condition;
	if( x + m_nTextureCover < m_nWidth )
	{
		nTexNBRight = GetTextureIndex(x + m_nTextureCover, y);
		if( nTexIndex != nTexNBRight )
			nDiffTex[nDiffCount ++] = nTexNBRight;
	}
	else
		nTexNBRight = nTexIndex;

	if( y + m_nTextureCover < m_nHeight )
	{
		nTexNBBottom = GetTextureIndex(x, y + m_nTextureCover);
		if( nTexIndex != nTexNBBottom && nTexNBBottom != nTexNBRight )
			nDiffTex[nDiffCount ++] = nTexNBBottom;
	}
	else
		nTexNBBottom = nTexIndex;

	if( x + m_nTextureCover < m_nWidth && y + m_nTextureCover < m_nHeight )
	{
		nTexNBRB = GetTextureIndex(x + m_nTextureCover, y + m_nTextureCover);
		if( nTexIndex != nTexNBRB && nTexNBRB != nTexNBRight && nTexNBRB != nTexNBBottom )
			nDiffTex[nDiffCount ++] = nTexNBRB;
	}
	else
		nTexNBRB = nTexIndex;

	if( nDiffCount == 0 )
		return true; //They are the same;

	//First determine the adjacents;
	/*
		0---------1
		|	 	  |	right
		|	 	  |
		|	 	  |
		|	 	  |
		1---------1
		bottom		right-bottom
	*/
	BYTE			alpha[2][2];
	A3DLVERTEX		newVertex;
	A3DVECTOR3		vecOrg;

	vecOrg = GetVertexPos(x, y);
	for(int i=0; i<nDiffCount; i++)
	{
		int newTex = nDiffTex[i];
		int nStartVert = m_nExtraVisibleVerts[newTex];

		alpha[0][0] = 0;
		alpha[1][0] = newTex == nTexNBRight ? 1 : 0;
		alpha[0][1] = newTex == nTexNBBottom ? 1 :0;
		alpha[1][1] = newTex == nTexNBRB ? 1 : 0;

		// Check if enough buffer space available
		if( m_nExtraVisibleVerts[newTex] + 6 * 8 * 8 > m_nMaxExtraVerts[newTex] ||
			m_nExtraVisibleFaces[newTex] * 3 + 6 * 8 * 8 > m_nMaxExtraIndices[newTex] )
		{
			m_nMaxExtraVerts[newTex] += 6 * 8 * 8;
			m_nMaxExtraIndices[newTex] += 6 * 8 * 8;

			// Reallocate buffers;
			m_pExtraVertexBuffers[newTex] = (A3DLVERTEX *) realloc(m_pExtraVertexBuffers[newTex], m_nMaxExtraVerts[newTex] * sizeof(A3DLVERTEX));
			if( NULL == m_pExtraVertexBuffers[newTex] )
			{
				g_A3DErrLog.Log("A3DTerrain::SmoothConnection(), Not Enough Memory!");
				return false;
			}
			m_pExtraIndexBuffers[newTex] = (WORD *) realloc(m_pExtraIndexBuffers[newTex], m_nMaxExtraIndices[newTex] * sizeof(WORD));
			if( NULL == m_pExtraIndexBuffers[newTex] )
			{
				g_A3DErrLog.Log("A3DTerrain::SmoothConnection(), Not Enough Memory!");
				return false;
			}
		}

		for(int nVert=nOldVertCount; nVert<m_nVisibleVerts[nTexIndex]; nVert++)
		{
			int a0, a1, a2;
			int nAlpha;
			FLOAT vX, vY;

			//Calculate properate alpha value;
			newVertex = m_pVertexBuffers[nTexIndex][nVert];
			vX = (newVertex.x - vecOrg.x) / m_vCellSize / m_nTextureCover;
			vY = (vecOrg.z - newVertex.z) / m_vCellSize / m_nTextureCover;
			if( vX < 0.0f ) vX = 0.0f;
			if( vX > 1.0f ) vX = 1.0f;
			if( vY < 0.0f ) vX = 0.0f;
			if( vY > 1.0f ) vY = 1.0f;

			if( ((sx + sy)) % 2 == 0 )
			{
				// 0,0---1,0
				//	| \	  |
				//	|  \  |
				//	|   \ |
				// 0,1---1,1
				if( vX > vY )
				{
					//Use Right Triangle;
					a0 = alpha[1][0];
					a1 = alpha[0][0];
					a2 = alpha[1][1];
					vX = 1.0f - vX;
				}
				else
				{
					//Use Left Triangle;
					a0 = alpha[0][1];
					a1 = alpha[1][1];
					a2 = alpha[0][0];
					vY = 1.0f - vY;
				}
			}
			else
			{
				// 0,0---1,0
				//	|	/ |
				//	|  /  |
				//	| /	  |
				// 0,1---1,1
				if( vX + vY > 1.0f)
				{
					//Use Below Triangle;
					a0 = alpha[1][1];
					a1 = alpha[0][1];
					a2 = alpha[1][0];
					vX = 1.0f - vX;
					vY = 1.0f - vY;
				}
				else
				{
					//Use Up Triangle;
					a0 = alpha[0][0];
					a1 = alpha[1][0];
					a2 = alpha[0][1];
				}
			}

			nAlpha = (int)((a0 + (a1 - a0) * vX + (a2 - a0) * vY) * 255.0f);
			if( nAlpha > 255 ) nAlpha = 255;

			newVertex.diffuse = (newVertex.diffuse & 0x00ffffff) | (nAlpha << 24);
			m_pExtraVertexBuffers[newTex][m_nExtraVisibleVerts[newTex] ++] = newVertex;
		}
		for(int nFace=nOldFaceCount; nFace<m_nVisibleFaces[nTexIndex]; nFace++)
		{
			m_pExtraIndexBuffers[newTex][m_nExtraVisibleFaces[newTex] * 3 + 0] = 
				m_pIndexBuffers[nTexIndex][nFace * 3 + 0] - nOldVertCount + nStartVert;
			m_pExtraIndexBuffers[newTex][m_nExtraVisibleFaces[newTex] * 3 + 1] = 
				m_pIndexBuffers[nTexIndex][nFace * 3 + 1] - nOldVertCount + nStartVert;
			m_pExtraIndexBuffers[newTex][m_nExtraVisibleFaces[newTex] * 3 + 2] = 
				m_pIndexBuffers[nTexIndex][nFace * 3 + 2] - nOldVertCount + nStartVert;

			m_nExtraVisibleFaces[newTex] ++;
		}
	}	

	return true;
}

bool A3DTerrain::UpdateStream()
{
	if( NULL == m_pA3DCamera )
		return false;

	int				idx, idy, x, y;
	A3DVECTOR3		vec0, vec1, vec2, vec3;
	
	bool			bInSight;
	A3DVECTOR3		vecVert;

	//Update camera's position information;
	m_vecCamPos = m_pA3DCamera->GetPos();
	GetCellPos(m_vecCamPos, &x, &y);
	m_ptCamPos.x = x; m_ptCamPos.y = y;

	CalculateRenderRange();

	if( m_nVisibleBeginXOld != m_nVisibleBeginX || m_nVisibleBeginYOld != m_nVisibleBeginY || m_ptCamPos.x != m_ptCamPosOld.x || m_ptCamPos.y != m_ptCamPosOld.y )
	{
		ZeroMemory(m_nVisibleVerts, sizeof(int) * m_nNumTexture);
		ZeroMemory(m_nVisibleFaces, sizeof(int) * m_nNumTexture);
		ZeroMemory(m_nExtraVisibleVerts, sizeof(int) * m_nNumTexture);
		ZeroMemory(m_nExtraVisibleFaces, sizeof(int) * m_nNumTexture);

		//Reset all stage level information;
		for(int i=0; i<m_nCellStageCount; i++)
		{
			memset(m_ppCellLevelTable[i], 2, sizeof(CHAR) * m_nStageWidth[i] * m_nStageHeight[i]);
		}

		for(idy=m_nVisibleBeginY; idy<m_nVisibleBeginY + m_nSightRange; idy+=m_nTextureCover)
		{
			if( idy >= m_nHeight )
				break;
			for(idx=m_nVisibleBeginX; idx<m_nVisibleBeginX + m_nSightRange; idx+=m_nTextureCover)
			{   
				if( idx >= m_nWidth )
					break;

				int sx, sy;
				int stage = m_nCellStageCount - 1;

				sx = idx / m_nStageCover[stage];
				sy = idy / m_nStageCover[stage];

				if( 0 && GetCellLevel(m_nCellStageCount - 1, sx, sy) >= 0 )
				{
					vec0 = GetVertexPos(idx, idy);
					vec1 = GetVertexPos(idx + m_nTextureCover, idy);
					vec2 = GetVertexPos(idx, idy + m_nTextureCover);
					vec3 = GetVertexPos(idx + m_nTextureCover, idy + m_nTextureCover);
					
					A3DVECTOR3 vecScreen;

					if( m_pA3DViewport->Transform(vec0, vecScreen) &&
						m_pA3DViewport->Transform(vec1, vecScreen) &&
						m_pA3DViewport->Transform(vec2, vecScreen) &&
						m_pA3DViewport->Transform(vec3, vecScreen) )
						bInSight = false;
					else
						bInSight = true;
				}
				else
				{
					if( idx >= m_rectHollow.left && idx <= m_rectHollow.right &&
						idy >= m_rectHollow.top && idy <= m_rectHollow.bottom &&
						idx + m_nTextureCover <= m_rectHollow.right &&
						idy + m_nTextureCover <= m_rectHollow.bottom )
						bInSight = false;
					else
						bInSight = true;
				}
				
				// First update all stage's cell information;
				int nTexIndex = GetTextureIndex(idx, idy);
				int nOldVertCount = m_nVisibleVerts[nTexIndex];
				int nOldFaceCount = m_nVisibleFaces[nTexIndex];

				
				if( idx == m_nVisibleBeginX && idy == m_nVisibleBeginY )
					UpdateOneCellLevel(stage, sx, sy);
				if( idy == m_nVisibleBeginY )
					UpdateOneCellLevel(stage, sx + 1, sy);
				UpdateOneCellLevel(stage, sx, sy + 1);

				if( bInSight )
				{
					// Check if there is still enough buffer space
					if( m_nVisibleVerts[nTexIndex] + 6 * 8 * 8 > m_nMaxVerts[nTexIndex] ||
						m_nVisibleFaces[nTexIndex] * 3 + 6 * 8 * 8 > m_nMaxIndices[nTexIndex] )
					{
						// Reallocate the buffer;
						m_nMaxVerts[nTexIndex] += 6 * 8 * 8;
						m_nMaxIndices[nTexIndex] += 6 * 8 * 8;

						m_pVertexBuffers[nTexIndex] = (A3DLVERTEX *) realloc(m_pVertexBuffers[nTexIndex], m_nMaxVerts[nTexIndex] * sizeof(A3DLVERTEX));
						if( NULL == m_pVertexBuffers[nTexIndex] )
						{
							g_A3DErrLog.Log("A3DTerrain::UpdateStream(), Not Enough Memory!");
							return false;
						}
						m_pIndexBuffers[nTexIndex] = (WORD *) realloc(m_pIndexBuffers[nTexIndex], m_nMaxIndices[nTexIndex] * sizeof(WORD));
						if( NULL == m_pIndexBuffers[nTexIndex] )
						{
							g_A3DErrLog.Log("A3DTerrain::UpdateStream(), Not Enough Memory!");
							return false;
						}
					}
					UpdateCellStream(stage, sx, sy);

					SmoothConnection(idx, idy, sx, sy, nTexIndex, nOldVertCount, nOldFaceCount);
				}
			}
		}
	}

	if( m_pA3DDevice->IsDetailMethodSupported() && (m_ptCamPos.x != m_ptCamPosOld.x || m_ptCamPos.y != m_ptCamPosOld.y) )
	{
		int		i, j;
		FLOAT	vX, vY;

		GetCellPos(m_vecCamPos, &idx, &idy, &vX, &vY);

		FLOAT vDX = vX - idx;
		FLOAT vDY = vY - idy;

		//Detail Ratio multiply with m_nDetailWidth must be an integer;
		FLOAT vDetailRatio = 3.0f;

		idx -= m_nDetailWidth / 2;
		idy -= m_nDetailHeight / 2;

		//Now, fill the detail texture buffer;
		int sx, sy;
		A3DCOLOR color;
		for(sy=0; sy<=m_nDetailHeight; sy++)
		{
			for(sx=0; sx<=m_nDetailWidth; sx++)
			{
				A3DVECTOR3	vecVert;
				
				vecVert = GetVertexPos(idx + sx, idy + sy);
				
				color = A3DCOLORRGBA(255, 255, 255, 255);
				m_pDetailVB[sy * (m_nDetailWidth + 1) + sx] =  
					A3DLVERTEX(vecVert, color, A3DCOLORRGBA(0, 0, 0, 255), sx * vDetailRatio, sy * vDetailRatio);
			}
		}

		int nCellNum = 0;
		for(j=0; j<m_nDetailHeight; j++)
		{
			for(i=0; i<m_nDetailWidth; i++)
			{
				if( i + idx >= m_rectHollow.left && i + idx <= m_rectHollow.right &&
					j + idy >= m_rectHollow.top && j + idy <= m_rectHollow.bottom )
					continue;

				if( ((i + idx + j + idy) % 2) == 0 )
				{
					//	0(3)---4
					//	| \	  |
					//	|  \  |
					//	|   \ |
					//	2-----1(5)
					m_pDetailIndex[nCellNum * 6 + 0] = j * (m_nDetailWidth + 1) + i;
					m_pDetailIndex[nCellNum * 6 + 1] = (j + 1) * (m_nDetailWidth + 1) + i + 1;
					m_pDetailIndex[nCellNum * 6 + 2] = (j + 1) * (m_nDetailWidth + 1) + i;
					m_pDetailIndex[nCellNum * 6 + 3] = m_pDetailIndex[nCellNum * 6 + 0];
					m_pDetailIndex[nCellNum * 6 + 4] = j * (m_nDetailWidth + 1) + i + 1;
					m_pDetailIndex[nCellNum * 6 + 5] = m_pDetailIndex[nCellNum * 6 + 1];
				}
				else
				{
					//	0----1(3)
					//	|	/ |
					//	|  /  |
					//	| /	  |
					//	2(5)--4
					m_pDetailIndex[nCellNum * 6 + 0] = j * (m_nDetailWidth + 1) + i;
					m_pDetailIndex[nCellNum * 6 + 1] = j * (m_nDetailWidth + 1) + i + 1;
					m_pDetailIndex[nCellNum * 6 + 2] = (j + 1) * (m_nDetailWidth + 1) + i;
					m_pDetailIndex[nCellNum * 6 + 3] = m_pDetailIndex[nCellNum * 6 + 1];
					m_pDetailIndex[nCellNum * 6 + 4] = (j + 1) * (m_nDetailWidth + 1) + i + 1;
					m_pDetailIndex[nCellNum * 6 + 5] = m_pDetailIndex[nCellNum * 6 + 2];
				}

				nCellNum ++;
			}
		}
		m_nDetailCellCount = nCellNum;
		//m_pDetailStream->SetIndices(m_pDetailIndex, m_nDetailFaceCount * 3);
	}

	m_nVisibleBeginXOld = m_nVisibleBeginX;
	m_nVisibleBeginYOld = m_nVisibleBeginY;
	m_ptCamPosOld = m_ptCamPos;

	return true;
}

/* 
	This function update the large cell's all stage level which covered by one texture
*/
bool A3DTerrain::UpdateOneCellLevel(int stage, int sx, int sy)
{
	FLOAT vDistance;
	FLOAT vSquareError;

	int nX, nY;
	int nLevel;
	
	if( sx >= m_nStageWidth[stage] || sy >= m_nStageHeight[stage] )
		return true;

	//Make nX, nY the center of this stage cell;
	nY = sy * m_nStageCover[stage] + m_nStageCover[stage] / 2;
	nX = sx * m_nStageCover[stage] + m_nStageCover[stage] / 2;
	
	vDistance = GetDistance(nX - m_ptCamPos.x, nY - m_ptCamPos.y);
	vSquareError = GetSquareError(stage, sx, sy);

	//My level will be determined only I have checked my left and top neighbour;
	nLevel = DetermineLevel(stage, vDistance, vSquareError);

	if( nLevel == -1 )
		goto DESCENT;
	SetCellLevel(stage, sx, sy, nLevel);
	return true;

DESCENT:
	if( stage == 0 )
	{
		SetCellLevel(stage, sx, sy, 0);
		return true;
	}

	//Check my four sub cell and then determine my level;
	UpdateOneCellLevel(stage - 1, sx * 2, sy * 2);
	UpdateOneCellLevel(stage - 1, sx * 2 + 1, sy * 2);
	UpdateOneCellLevel(stage - 1, sx * 2, sy * 2 + 1);
	UpdateOneCellLevel(stage - 1, sx * 2 + 1, sy * 2 + 1);
	if( GetCellLevel(stage - 1, sx * 2, sy * 2) != 2 ||	GetCellLevel(stage - 1, sx * 2 + 1, sy * 2) != 2 ||
		GetCellLevel(stage - 1, sx * 2, sy * 2 + 1) != 2 ||	GetCellLevel(stage - 1, sx * 2 + 1, sy * 2 + 1) != 2 )
	{
		SetCellLevel(stage, sx, sy, -1);
		return true;
	}
	
	SetCellLevel(stage, sx, sy, 0, false);
	return true;
}

/* 
	Construct a triagle and avoid aliasing between its neighbour
*/
bool A3DTerrain::ConstructTriangleHalf(int stage, int sx, int sy, int index, int idTex, bool bCallFromQuarter, FLOAT vCornerHeight)
{
	//The index of each half triangle's neighbour cell;
	static	int nx_half[8] = {-1, 0, 0, 1, 1, 0, 0, -1};
	static	int ny_half[8] = {0, -1, -1, 0, 0, 1, 1, 0};
	//The triangle vert offset list relative to the cell origin of the STAGE for each half triangle;
	static  int tx_half[24] = {0, 0, 1,		1, 0, 1,	0, 1, 0,	1, 1, 0,
							   1, 1, 0,		0, 1, 0,	1, 0, 1,	0, 0, 1};
	static  int ty_half[24] = {1, 0, 1,		0, 0, 1,	0, 0, 1,	1, 0, 1,	
							   0, 1, 0,		1, 1, 0,	1, 1, 0,	0, 1, 0};
	//The index conversion of one half triangle to one quarter triangle;
	static	int index_half_to_quarter[8] = {0, 1, 1, 2, 2, 3, 3, 0};

	//sub index which indicate whether it should inverse clockwise or not;
	static 	int sub_index[2][3] = {{0, 1, 2}, {1, 0, 2}}; 

	//Tempory index buffer;
	int		x[3], y[3];

	//Tempory vertex buffer;
	A3DLVERTEX vertex[5];

	//Neighbour's x, y
	int		nx, ny;
	//Extra vert's x, y
	int		ex, ey;
	
	int		si_order;

	int		i;

	int		nStartVert;

	nx = sx + nx_half[index];
	ny = sy + ny_half[index];
	si_order = index % 2;
	nStartVert = m_nVisibleVerts[idTex];

	//The 3 vertex which the half triangle has;
	for(i=0; i<3; i++)
	{
		x[i] = (sx + tx_half[index * 3 + i]) * m_nStageCover[stage];
		y[i] = (sy + ty_half[index * 3 + i]) * m_nStageCover[stage];
		vertex[i] = GetVertex(x[i], y[i]);
	}
	//If we have provide one height;
	if( bCallFromQuarter )
		vertex[2].y = vCornerHeight;

	if( nx > 0 && nx < m_nStageWidth[stage] && ny > 0 && ny < m_nStageHeight[stage] )
	{
		if(GetCellLevel(stage,	nx, ny) <= 0 )
		{
			//Add 2 extra vertex;
			//one on outside edge center;
			ex = (x[0] + x[1]) / 2;
			ey = (y[0] + y[1]) / 2;
			vertex[3] = GetVertex(ex, ey);
			//one is rectangle's center;
			ex = (x[1] + x[2]) / 2;
			ey = (y[1] + y[2]) / 2;
			vertex[4] = GetVertex(ex, ey);
			vertex[4].y = (vertex[1].y + vertex[2].y) / 2.0f;

			if( stage == 0 )
			{
				//We use all the five vertex;
				for(i=0; i<5; i++)
					m_pVertexBuffers[idTex][m_nVisibleVerts[idTex] ++] = vertex[i];

				//fill index here, 3 triangles, two small and one big;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][0]] = nStartVert;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][1]] = nStartVert + 3;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][2]] = nStartVert + 4;
				m_nVisibleFaces[idTex] ++;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][0]] = nStartVert + 3;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][1]] = nStartVert + 1;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][2]] = nStartVert + 4;
				m_nVisibleFaces[idTex] ++;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][0]] = nStartVert;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][1]] = nStartVert + 4;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][2]] = nStartVert + 2;
				m_nVisibleFaces[idTex] ++;
			}
			else
			{
				//We only use 3 of the 5 vertex;
				m_pVertexBuffers[idTex][m_nVisibleVerts[idTex] ++] = vertex[0];
				m_pVertexBuffers[idTex][m_nVisibleVerts[idTex] ++] = vertex[2];
				m_pVertexBuffers[idTex][m_nVisibleVerts[idTex] ++] = vertex[4];
				//Fill index;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][0]] = nStartVert + 1;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][1]] = nStartVert + 0;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][2]] = nStartVert + 2;
				m_nVisibleFaces[idTex] ++;

				ConstructTriangleQuarter(stage, sx, sy, index_half_to_quarter[index], idTex, true, vertex[4].y);
			}
			return true;
		}
	}
	//This triangle can match its neighbour; Just add it here;
	//The 3 vertex which the half triangle has;
	for(i=0; i<3; i++)
		m_pVertexBuffers[idTex][m_nVisibleVerts[idTex] ++] = vertex[i];

	m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][0]] = nStartVert;
	m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][1]] = nStartVert + 1;
	m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + sub_index[si_order][2]] = nStartVert + 2;
	m_nVisibleFaces[idTex] ++;
	return true;
}

/* 
	Construct a triagle and avoid aliasing between its neighbour
*/
bool A3DTerrain::ConstructTriangleQuarter(int stage, int sx, int sy, int index, int idTex, bool bCallFromHalf, FLOAT vCenterHeight)
{
	//The index of each quarter triangle's neighbour cell;
	static	int nx_quarter[4] = {-1, 0, 1, 0};
	static	int ny_quarter[4] = {0, -1, 0, 1};
	//The triangle vert offset list relative to the cell origin of the STAGE for each quarter triangle;
	static	int tx_quarter[12] = {0, 0, 1,		0, 2, 1,	2, 2, 1,	2, 0, 1}; //1 means half, 2 means 1;
	static	int ty_quarter[12] = {2, 0, 1,		0, 0, 1,	0, 2, 1,	2, 2, 1}; //1 means half, 2 means 1;
	//The index conversion of one quarter triangle to two half triangle;
	static	int index_quarter_to_half[8] = {7, 0, 1, 2, 3, 4, 5, 6}; //Each 1 convert to 2, so mulply with 2 of quarter's index
	//The new sx, sy if the quarter triangle is devided into two half triangle;
	static	int sx_quarter_to_half[8] = {0, 0, 0, 1, 1, 1, 1, 0};
	static	int sy_quarter_to_half[8] = {1, 0, 0, 0, 0, 1, 1, 1};

	int		x[3], y[3];
	A3DLVERTEX vertex[4];

	//Neighbour's x, y
	int		nx, ny;
	//Extra vert's x, y
	int		ex, ey;

	int		i;
	int		nStartVert;

	nx = sx + nx_quarter[index];
	ny = sy + ny_quarter[index];
	nStartVert = m_nVisibleVerts[idTex];

	for(i=0; i<3; i++)
	{
		x[i] = sx * m_nStageCover[stage] + tx_quarter[index * 3 + i] * m_nStageCover[stage] / 2;
		y[i] = sy * m_nStageCover[stage] + ty_quarter[index * 3 + i] * m_nStageCover[stage] / 2;
		vertex[i] = GetVertex(x[i], y[i]);
	}

	if( bCallFromHalf )
		vertex[2].y = vCenterHeight;
	else
		vCenterHeight = vertex[2].y;

	if( nx > 0 && nx < m_nStageWidth[stage] && ny > 0 && ny < m_nStageHeight[stage] )
	{
		if(GetCellLevel(stage,	nx, ny) <= 0 )
		{
			if( stage == 0 )
			{
				//Must create it here;
				//Add one more extra vert on the edge center;
				ex = (x[0] + x[1]) / 2;
				ey = (y[0] + y[1]) / 2;
				vertex[3] = GetVertex(ex, ey);

				for(i=0; i<4; i++)
					m_pVertexBuffers[idTex][m_nVisibleVerts[idTex] ++] = vertex[i];

				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = nStartVert;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = nStartVert + 3;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = nStartVert + 2;
				m_nVisibleFaces[idTex] ++;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = nStartVert + 3;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = nStartVert + 1;
				m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = nStartVert + 2;
				m_nVisibleFaces[idTex] ++;
			}
			else
			{
				//Devide it into two half triangle and then check it on the highest stage
				ConstructTriangleHalf(stage - 1, sx * 2 + sx_quarter_to_half[index * 2], sy * 2 + sy_quarter_to_half[index * 2], index_quarter_to_half[index * 2], idTex, true, vCenterHeight);
				ConstructTriangleHalf(stage - 1, sx * 2 + sx_quarter_to_half[index * 2 + 1], sy * 2 + sy_quarter_to_half[index * 2 + 1], index_quarter_to_half[index * 2 + 1], idTex, true, vCenterHeight);
			}
			return true;
		}
	}

	//This triangle can match its neighbour; Just add it here;
	for(i=0; i<3; i++)
		m_pVertexBuffers[idTex][m_nVisibleVerts[idTex] ++] = vertex[i];

	m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = nStartVert;
	m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = nStartVert + 1;
	m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = nStartVert + 2;
	m_nVisibleFaces[idTex] ++;
	return true;
}
/* 
	Construct the vertex and index buffer of the first level cell
*/
//----------------
//|\	 |	   / |
//|	 \	 |	 /	 |
//|	   \ | /	 |
//|--------------|
//|	   / | \	 |
//|	 /	 |	 \	 |
//|/	 |	   \ |
//----------------
bool A3DTerrain::ConstructFirstLevelCell(int idTex, int stage, int sx, int sy)
{
	int i, j, nbLeft, nbTop, nbRight, nbBottom;

	if( stage > 0 )
	{
		if( sx > 0 )
		{
			nbLeft = GetCellLevel(stage, sx - 1, sy);
			//If left is higher resolution,we must use triangle construct;
			if( nbLeft < 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sy > 0 )
		{
			nbTop = GetCellLevel(stage, sx, sy - 1);
			if( nbTop < 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sx < m_nStageWidth[stage] )
		{
			nbRight = GetCellLevel(stage, sx + 1, sy);
			if( nbRight < 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sy < m_nStageHeight[stage] )
		{
			nbBottom = GetCellLevel(stage, sx, sy + 1);
			if( nbBottom < 0 )
				goto TRIANGLECONSTRUCT;
		}
	}

	for(i=0; i<=2; i++)
	{
		for(j=0; j<=2; j++)
		{
			int			idx, idy;
			A3DVECTOR3	vecVert;

			idx = sx * m_nStageCover[stage] + j * m_nStageCover[stage] / 2;
			idy = sy * m_nStageCover[stage] + i * m_nStageCover[stage] / 2;
			vecVert = GetVertexPos(idx, idy);
			
			//Fill Vertex Buffer.
			A3DCOLOR color;
			color = GetVertexColor(idx, idy) | 0xff000000;
#ifdef COLOR_TERRAIN
			if( stage == 0 )
			{
				color = A3DCOLORRGBA(255, 0, 0, 255);
			}
			else if( stage == 1 )
			{
				color = A3DCOLORRGBA(0, 255, 0, 255);
			}
			else
			{
				color = A3DCOLORRGBA(255, 0, 255, 255);
			}
#endif

			m_pVertexBuffers[idTex][m_nVisibleVerts[idTex]] =  
				A3DLVERTEX(vecVert, color, A3DCOLORRGBA(0, 0, 0, 0/*GetFogData(GetDistance(idx - m_ptCamPos.x, idy - m_ptCamPos.y))*/), 
				idx * 1.0f / m_nTextureCover, idy * 1.0f / m_nTextureCover);

			if( i != 2 && j != 2 )
			{
				if( (i + j)	% 2 == 0 )
				{
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = m_nVisibleVerts[idTex];
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = m_nVisibleVerts[idTex] + 4;
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = m_nVisibleVerts[idTex] + 3;
					
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 3] = m_nVisibleVerts[idTex];
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 4] = m_nVisibleVerts[idTex] + 1;
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 5] = m_nVisibleVerts[idTex] + 4;
				}
				else
				{
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = m_nVisibleVerts[idTex];
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = m_nVisibleVerts[idTex] + 1;
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = m_nVisibleVerts[idTex] + 3;
					
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 3] = m_nVisibleVerts[idTex] + 1;
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 4] = m_nVisibleVerts[idTex] + 4;
					m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 5] = m_nVisibleVerts[idTex] + 3;
				}

				m_nVisibleFaces[idTex] += 2;
			}

			m_nVisibleVerts[idTex] ++;
		}
	}
	
	return true;

TRIANGLECONSTRUCT:
	ConstructTriangleHalf(stage - 1, sx * 2, sy * 2, 0, idTex);
	ConstructTriangleHalf(stage - 1, sx * 2, sy * 2, 1, idTex);
	ConstructTriangleHalf(stage - 1, sx * 2 + 1, sy * 2, 2, idTex);
	ConstructTriangleHalf(stage - 1, sx * 2 + 1, sy * 2, 3, idTex);
	ConstructTriangleHalf(stage - 1, sx * 2 + 1, sy * 2 + 1, 4, idTex);
	ConstructTriangleHalf(stage - 1, sx * 2 + 1, sy * 2 + 1, 5, idTex);
	ConstructTriangleHalf(stage - 1, sx * 2, sy * 2 + 1, 6, idTex);
	ConstructTriangleHalf(stage - 1, sx * 2, sy * 2 + 1, 7, idTex);
	return true;
}

/*
	Construct the vertex and index buffer of the second level cell
*/
//0--------------1
//|\	 	   / |
//|	 \	 	 /	 |
//|	   \ 2  /	 |
//|		 x	     |
//|	   /   \	 |
//|	 /	  	 \	 |
//|/	  	   \ |
//3---------------4
bool A3DTerrain::ConstructSecondLevelCell(int idTex, int stage, int sx, int sy)
{
	int			nbLeft, nbTop, nbRight, nbBottom;
	int			idx[5], idy[5], i;

	static int  index[4 * 3] = {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
	static int  nx[4] = {0, 1, 0, -1};
	static int  ny[4] = {-1, 0, 1, 0};
	static int  ex[4] = {1, 0, -1, 0};
	static int  ey[4] = {0, 1, 0, -1};
	//If we need check next stage's sub cell's level, we check this;
	static int  subx[8] = {0, 1, 0, 0, 0, 1, 1, 1};  
	static int  suby[8] = {1, 1, 0,	1, 0, 0, 0, 1};

	int			nIndexStart;
	bool		bNeedExtraVert;
	int			nExtraVert;
	int			nbX, nbY;

	A3DVECTOR3	vecVert;
	A3DCOLOR	color;

	//First we should check if this cell can match its neighbour;
	if( stage > 0 )
	{
		if( sx > 0 )
		{
			nbLeft = GetCellLevel(stage, sx - 1, sy);
			//If left is higher resolution,we must use triangle construct;
			if( nbLeft < 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sy > 0 )
		{
			nbTop = GetCellLevel(stage, sx, sy - 1);
			if( nbTop < 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sx < m_nStageWidth[stage] )
		{
			nbRight = GetCellLevel(stage, sx + 1, sy);
			if( nbRight < 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sy < m_nStageHeight[stage] )
		{
			nbBottom = GetCellLevel(stage, sx, sy + 1);
			if( nbBottom < 0 )
				goto TRIANGLECONSTRUCT;
		}
	}

	idx[0] = sx * m_nStageCover[stage];			idy[0] = sy * m_nStageCover[stage];
	idx[1] = idx[0] + m_nStageCover[stage];		idy[1] = idy[0];
	idx[2] = idx[1];							idy[2] = idy[0] + m_nStageCover[stage];
	idx[3] = idx[0];							idy[3] = idy[2];
	idx[4] = idx[0] + m_nStageCover[stage] / 2;	idy[4] = idy[0] + m_nStageCover[stage] / 2;

	//5 vertex and 4 triangle;
	nIndexStart = m_nVisibleVerts[idTex];
	for(i=0; i<5; i++)
	{
		//veterx 0;
		vecVert = GetVertexPos(idx[i], idy[i]);
		color = GetVertexColor(idx[i], idy[i]) | 0xff000000;
#ifdef COLOR_TERRAIN
		if( stage == 0 )
		{
			color = A3DCOLORRGBA(180, 0, 0, 255);
		}
		else if( stage == 1 )
		{
			color = A3DCOLORRGBA(0, 180, 0, 255);
		}
		else
		{
			color = A3DCOLORRGBA(180, 0, 180, 255);
		}
#endif
		m_pVertexBuffers[idTex][m_nVisibleVerts[idTex]] =  
			A3DLVERTEX(vecVert, color, A3DCOLORRGBA(0, 0, 0, 0/*GetFogData(GetDistance(idx[i] - m_ptCamPos.x, idy[i] - m_ptCamPos.y))*/), 
			idx[i] * 1.0f / m_nTextureCover, idy[i] * 1.0f / m_nTextureCover);
		m_nVisibleVerts[idTex] ++;
	}
	
	for(i=0; i<4; i++)
	{
		//First check my neighbour's level;
		bNeedExtraVert = false;
		nbX = sx + nx[i]; nbY = sy + ny[i];
		if( nbX >=0 && nbY >= 0 && nbX < m_nStageWidth[stage] && nbY < m_nStageHeight[stage] )
		{
			int nbLevel = GetCellLevel(stage, nbX, nbY);
			if( 0 == nbLevel )
			{
				bNeedExtraVert = true;
			}
			else if( nbLevel == -1 && stage > 0 )
			{
				//See its child's level info;
				//Check 2 horizontal sub cell;
				if( GetCellLevel(stage - 1, nbX * 2 + subx[i * 2], nbY * 2 + suby[i * 2]) >= 1 &&
					GetCellLevel(stage - 1, nbX * 2 + subx[i * 2 + 1], nbY * 2 + suby[i * 2 + 1]) >= 1)
				{
					bNeedExtraVert = true;
				}
			}
		}
		
		if( bNeedExtraVert )
		{
			//Need one extra vertex here;
			int extX = idx[i] + ex[i] * m_nStageCover[stage] / 2;
			int extY = idy[i] + ey[i] * m_nStageCover[stage] / 2;
			vecVert = GetVertexPos(extX, extY);
			color = GetVertexColor(extX, extY) | 0xff000000;
			m_pVertexBuffers[idTex][m_nVisibleVerts[idTex]] =  
				A3DLVERTEX(vecVert, color, A3DCOLORRGBA(0, 0, 0, 0/*GetFogData(GetDistance(idx[i] - m_ptCamPos.x, idy[i] - m_ptCamPos.y))*/), 
				extX * 1.0f / m_nTextureCover, extY * 1.0f / m_nTextureCover);
			nExtraVert = m_nVisibleVerts[idTex] ++;

			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = nIndexStart + index[i * 3 + 0];
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = nExtraVert;
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = nIndexStart + index[i * 3 + 2];
			m_nVisibleFaces[idTex] ++;

			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = nIndexStart + index[i * 3 + 2];
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = nExtraVert;
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = nIndexStart + index[i * 3 + 1];
			m_nVisibleFaces[idTex] ++;
		}
		else
		{
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 0] = nIndexStart + index[i * 3 + 0];
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 1] = nIndexStart + index[i * 3 + 1];
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + 2] = nIndexStart + index[i * 3 + 2];
			m_nVisibleFaces[idTex] ++;
		}
	}
	return true;

TRIANGLECONSTRUCT:
	ConstructTriangleQuarter(stage, sx, sy, 0, idTex);
	ConstructTriangleQuarter(stage, sx, sy, 1, idTex);
	ConstructTriangleQuarter(stage, sx, sy, 2, idTex);
	ConstructTriangleQuarter(stage, sx, sy, 3, idTex);
	return true;
}

/*
	Construct the vertex and index buffer of the third level cell
*/
//----------------
//|\	 	     |
//|	 \	 	 	 |
//|	   \    	 |
//|		 \	     |
//|	       \	 |
//|	  	  	 \	 |
//|  	  	   \ |
//----------------
//or
//----------------
//|	     	   / |
//|	 	 	 /	 |
//|	       /	 |
//|		 /	     |
//|	   /    	 |
//|	 / 	  	 	 |
//|/   	  	     |
//----------------
bool A3DTerrain::ConstructThirdLevelCell(int idTex, int stage, int sx, int sy)
{
	int			nbLeft, nbTop, nbRight, nbBottom;
	//4 vertex and 2 triangles;
	int			idx[4], idy[4], i;
	static int  indexEven[2 * 3] = {0, 3, 2, 0, 1, 3};
	static int  indexOdd[2 * 3] = {0, 1, 2, 2, 1, 3};
	int			nIndexStart;

	A3DVECTOR3	vecVert;
	A3DCOLOR	color;

	//First we should check if this cell can match its neighbour;
	if( 1 )
	{
		if( sx > 0 )
		{
			nbLeft = GetCellLevel(stage, sx - 1, sy);
			//If left is higher resolution,we must use triangle construct;
			if( nbLeft <= 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sy > 0 )
		{
			nbTop = GetCellLevel(stage, sx, sy - 1);
			if( nbTop <= 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sx < m_nStageWidth[stage] )
		{
			nbRight = GetCellLevel(stage, sx + 1, sy);
			if( nbRight <= 0 )
				goto TRIANGLECONSTRUCT;
		}
		if( sy < m_nStageHeight[stage] )
		{
			nbBottom = GetCellLevel(stage, sx, sy + 1);
			if( nbBottom <= 0 )
				goto TRIANGLECONSTRUCT;
		}
	}

	idx[0] = sx * m_nStageCover[stage];			idy[0] = sy * m_nStageCover[stage];
	idx[1] = idx[0] + m_nStageCover[stage];		idy[1] = idy[0];
	idx[2] = idx[0];							idy[2] = idy[0] + m_nStageCover[stage];
	idx[3] = idx[1];							idy[3] = idy[2];

	nIndexStart = m_nVisibleVerts[idTex];
	for(i=0; i<4; i++)
	{
		//veterx 0;
		vecVert = GetVertexPos(idx[i], idy[i]);
		color = GetVertexColor(idx[i], idy[i]) | 0xff000000;
#ifdef COLOR_TERRAIN
		if( stage == 0 )
		{
			color = A3DCOLORRGBA(80, 0, 0, 255);
		}
		else if( stage == 1 )
		{
			color = A3DCOLORRGBA(0, 80, 0, 255);
		}
		else
		{
			color = A3DCOLORRGBA(80, 0, 80, 255);
		}
#endif
		m_pVertexBuffers[idTex][m_nVisibleVerts[idTex]] =  
			A3DLVERTEX(vecVert, color, A3DCOLORRGBA(0, 0, 0, 0/*GetFogData(GetDistance(idx[i] - m_ptCamPos.x, idy[i] - m_ptCamPos.y))*/), 
			idx[i] * 1.0f / m_nTextureCover, idy[i] * 1.0f / m_nTextureCover);
		m_nVisibleVerts[idTex] ++;
	}
	
	if( (sx + sy) % 2 == 0 )
	{
		for(i=0; i<6; i++)
		{
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + i] = nIndexStart + indexEven[i];
		}
	}
	else
	{
		for(i=0; i<6; i++)
		{
			m_pIndexBuffers[idTex][m_nVisibleFaces[idTex] * 3 + i] = nIndexStart + indexOdd[i];
		}
	}
	m_nVisibleFaces[idTex] += 2;
	return true;

TRIANGLECONSTRUCT:
	ConstructSecondLevelCell(idTex, stage, sx, sy);
	return true;
}

/*
	This function contruct some vertex buffer and index buffer to represent
	the big stage cell;
*/
bool A3DTerrain::UpdateCellStream(int stage, int sx, int sy)
{
	int nLevel = GetCellLevel(stage, sx, sy);
	if( nLevel == -1 )
	{
		//Not determine yet all will be determine by my child;
		UpdateCellStream(stage - 1, sx * 2,		sy * 2);
		UpdateCellStream(stage - 1, sx * 2 + 1, sy * 2);
		UpdateCellStream(stage - 1, sx * 2,		sy * 2 + 1);
		UpdateCellStream(stage - 1, sx * 2 + 1, sy * 2 + 1);
		return true;
	}
	
	BYTE nTexIndex = GetTextureIndex(sx * m_nStageCover[stage], sy * m_nStageCover[stage]);
	switch(nLevel)
	{
	case 0:
		ConstructFirstLevelCell(nTexIndex, stage, sx, sy);
		break;
	case 1:
		ConstructSecondLevelCell(nTexIndex, stage, sx, sy);
		break;
	case 2:
		ConstructThirdLevelCell(nTexIndex, stage, sx, sy);
		break;
	}

	return true;
}

bool A3DTerrain::RenderExtraByTexture(int idTexture)
{
	if( m_nExtraVisibleVerts[idTexture] > 0 )
	{
		if( m_pTextures[idTexture] )
			m_pTextures[idTexture]->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		if( m_bShowTerrain )
		{
			if( !m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				m_nExtraVisibleVerts[idTexture], m_nExtraVisibleFaces[idTexture], m_pExtraIndexBuffers[idTexture], A3DFMT_INDEX16, 
				m_pExtraVertexBuffers[idTexture], sizeof(A3DLVERTEX)) )
			{
				g_A3DErrLog.Log("A3DTerrain::Render DrawIndexedPrimitive Normal Fail!");
				return false;
			}
		}

		if( m_pTextures[idTexture] )
			m_pTextures[idTexture]->Disappear(0);
	}

	return true;
}

bool A3DTerrain::RenderByTexture(int idTexture)
{
	if( m_nVisibleVerts[idTexture] > 0 )
	{
		if( m_pTextures[idTexture] )
			m_pTextures[idTexture]->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		if( m_bShowTerrain )
		{
			if( !m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				m_nVisibleVerts[idTexture], m_nVisibleFaces[idTexture], m_pIndexBuffers[idTexture], A3DFMT_INDEX16, 
				m_pVertexBuffers[idTexture], sizeof(A3DLVERTEX)) )
			{
				g_A3DErrLog.Log("A3DTerrain::Render DrawIndexedPrimitive Normal Fail!");
				return false;
			}
		}
		
		if( m_bShowWire )
		{
			A3DMATRIX4 matWorldBias = m_matWorld;
			matWorldBias._42 += 0.1f;

			m_pA3DDevice->ClearTexture(0);
			m_pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);
			m_pA3DDevice->SetWorldMatrix(matWorldBias);
			if( !m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				m_nVisibleVerts[idTexture], m_nVisibleFaces[idTexture], m_pIndexBuffers[idTexture], A3DFMT_INDEX16, 
				m_pVertexBuffers[idTexture], sizeof(A3DLVERTEX)) )
			{
				g_A3DErrLog.Log("A3DTerrain::Render DrawIndexedPrimitive Normal Fail!");
				return false;
			}
			m_pA3DDevice->SetFillMode(A3DFILL_SOLID);
			m_pA3DDevice->SetWorldMatrix(m_matWorld);
		}

		if( m_pTextures[idTexture] )
			m_pTextures[idTexture]->Disappear(0);
	}
					 
	return true;
}

bool A3DTerrain::Render(A3DViewport * pViewport)
{
	if( m_bHWITerrain )
		return true;

	m_pA3DViewport = pViewport;

	// Comment below to see stage cell level info;
	/*
	if( GetKeyState('I') & 0x8000 )
		ShowStageCellLevelInfo();*/

	//m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetLightingEnable(false);
	//m_pA3DDevice->SetFogEnable(false); // Let d3d do fogging
	//m_pA3DDevice->SetSpecularEnable(false);
	m_pA3DDevice->SetWorldMatrix(m_matWorld);
	m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);

	m_pA3DDevice->SetDeviceTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	
	// First update the streams;
	if( !UpdateStream() )
		return false;

	int i;
	for(i=0; i<m_nNumTexture; i++)
	{
		RenderByTexture(i);
	}

	//Show extra patches, which are used to smooth between neighbour cells;
	for(i=0; i<m_nNumTexture; i++)
	{
		RenderExtraByTexture(i);
	}

	//Now use detail texture to detail the terrain;
	if( m_pA3DDevice->IsDetailMethodSupported() && g_pA3DConfig->GetFlagDetailTerrain() )
	{
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_DESTCOLOR);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_SRCCOLOR);
		m_pDetailTexture->Appear(0);
		//m_pDetailStream->SetVerts((LPBYTE)m_pDetailVB, m_nDetailVertCount);
		//m_pDetailStream->Appear();
	
		if( m_bShowTerrain )
		{
			if( !m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
					m_nDetailVertCount, m_nDetailCellCount * 2, m_pDetailIndex, A3DFMT_INDEX16, 
					m_pDetailVB, sizeof(A3DLVERTEX)) )
			{
				g_A3DErrLog.Log("A3DTerrain::Render DrawIndexedPrimitive Normal Fail!");
				return false;
			}
		}

		m_pDetailTexture->Disappear(0);
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}
	
	m_pA3DDevice->SetLightingEnable(true);
	//m_pA3DDevice->SetFogEnable(true);
	//m_pA3DDevice->SetSpecularEnable(true);
	
	return true;
}

bool A3DTerrain::SetCamera(A3DCameraBase * pCamera)
{
	m_pA3DCamera = pCamera;
	return true;
}

A3DVECTOR3 A3DTerrain::GetVertexPos(int x, int y)
{
	A3DVECTOR3 ret = m_vecPos;
	
	ret.x += x * m_vCellSize;
	ret.z -= y * m_vCellSize;

	if( x < 0 ) x = 0;
	if( x > m_nWidth ) x = m_nWidth;
	if( y < 0 ) y = 0;
	if( y > m_nHeight ) y = m_nHeight;
	ret.y  = m_pHeightBuffer[y * (m_nWidth + 1) + x];
	return ret;
}

A3DVECTOR3 A3DTerrain::GetCellCenterPos(int x, int y)
{
	A3DVECTOR3 ret = m_vecPos;
	
	ret.x += x * m_vCellSize + m_vCellSize / 2.0f;
	ret.z -= y * m_vCellSize + m_vCellSize / 2.0f;

	if( x < 0 ) x = 0;
	if( x >= m_nWidth ) x = m_nWidth - 1;
	if( y < 0 ) y = 0;
	if( y >= m_nHeight ) y = m_nHeight - 1;

	if( (x + y) % 2 == 0 )
	{
		ret.y  = (GetVertexHeight(x, y) + GetVertexHeight(x + 1, y + 1)) * 0.5f;
	}
	else
	{
		ret.y  = (GetVertexHeight(x + 1, y) + GetVertexHeight(x, y + 1)) * 0.5f;
	}
	return ret;
}

bool A3DTerrain::GetCellAllVertex(int x, int y, A3DVECTOR3 * pVertexPos, WORD * pIndex)
{
	if( x < 0 ) x = 0;
	if( x >= m_nWidth ) x = m_nWidth - 1;
	if( y < 0 ) y = 0;
	if( y >= m_nHeight ) y = m_nHeight - 1;

	pVertexPos[0] = GetVertexPos(x	  , y	 );
	pVertexPos[1] = GetVertexPos(x + 1, y	 );
	pVertexPos[2] = GetVertexPos(x	  , y + 1);
	pVertexPos[3] = GetVertexPos(x + 1, y + 1);
	
	if( (x + y) % 2 == 0 )
	{
		//	0-----1
		//	| \	  |
		//	|  \  |
		//	|   \ |
		//	2-----3
		pIndex[0] = 0;
		pIndex[1] = 3;
		pIndex[2] = 2;
		pIndex[3] = 0;
		pIndex[4] = 1;
		pIndex[5] = 3;
	}
	else
	{
		//	0-----1
		//	| 	/ |
		//	|  /  |
		//	| /   |
		//	2-----3
		pIndex[0] = 0;
		pIndex[1] = 1;
		pIndex[2] = 2;
		pIndex[3] = 1;
		pIndex[4] = 3;
		pIndex[5] = 2;
	}
	return true;
}

BYTE A3DTerrain::GetTextureIndex(int x, int y)
{
	x /= m_nTextureCover;
	y /= m_nTextureCover;

	if( x < 0 ) x = 0;
	if( x >= m_nWidth / m_nTextureCover ) x = m_nWidth / m_nTextureCover - 1;
	if( y < 0 ) y = 0;
	if( y >= m_nHeight / m_nTextureCover ) y = m_nHeight / m_nTextureCover - 1;

	return m_pTextureBuffer[y * m_nWidth / m_nTextureCover + x];
}

void A3DTerrain::SetTextureIndex(int x, int y, BYTE index)
{
	x /= m_nTextureCover;
	y /= m_nTextureCover;

	if( x < 0 ) x = 0;
	if( x >= m_nWidth / m_nTextureCover ) x = m_nWidth / m_nTextureCover - 1;
	if( y < 0 ) y = 0;
	if( y >= m_nHeight / m_nTextureCover ) y = m_nHeight / m_nTextureCover - 1;

	m_pTextureBuffer[y * m_nWidth / m_nTextureCover + x] = index;
}

bool A3DTerrain::CalculateRenderRange()
{
	if( NULL == m_pA3DCamera )
		return false;

	A3DVECTOR3 vecCam = m_pA3DCamera->GetPos();
	A3DVECTOR3 vecDir = m_pA3DCamera->GetDir();
	//vecDir.y = 0.0f;
	vecDir = Normalize(vecDir);

	vecCam = vecCam - m_vecPos;
	A3DVECTOR3 vecCenter = vecCam + vecDir * ((m_nSightRange / 2 - m_nTextureCover) * m_vCellSize);

	m_nVisibleBeginX = (int)(vecCenter.x / m_vCellSize) - m_nSightRange / 2;
	m_nVisibleBeginY = (int)(-vecCenter.z / m_vCellSize) - m_nSightRange / 2;

	m_nVisibleBeginX = m_nVisibleBeginX / m_nTextureCover * m_nTextureCover;
	m_nVisibleBeginY = m_nVisibleBeginY / m_nTextureCover * m_nTextureCover;

	if( m_nVisibleBeginX < 0 )
		m_nVisibleBeginX = 0;
	if( m_nVisibleBeginY < 0 )
		m_nVisibleBeginY = 0;

	return true;
}

FLOAT A3DTerrain::GetHeight(A3DVECTOR3 vecPos)
{
	vecPos = vecPos - m_vecPos;
	
	int nCellsX, nCellsY;
    FLOAT vCellsX, vCellsY;
    FLOAT vVertHeight[4];
	FLOAT vDX, vDY;
	FLOAT h0, h1, h2;

    vCellsX = vecPos.x / m_vCellSize;
    vCellsY = -vecPos.z / m_vCellSize;

    nCellsX = (int)vCellsX;
    if( nCellsX < 0 || nCellsX >= m_nWidth ) return 0.0f;

    nCellsY = (int)vCellsY;
    if( nCellsY < 0 || nCellsY >= m_nHeight ) return 0.0f;

	vDX = vCellsX - nCellsX;
	vDY = vCellsY - nCellsY;
		
	//	0-----1
	//	| 	  |
	//	|     |
	//	|     |
	//	2-----3
    vVertHeight[0] = m_pHeightBuffer[nCellsY * (m_nWidth + 1) + nCellsX];
    vVertHeight[1] = m_pHeightBuffer[nCellsY * (m_nWidth + 1) + nCellsX + 1];
    vVertHeight[2] = m_pHeightBuffer[(nCellsY + 1) * (m_nWidth + 1) + nCellsX];
    vVertHeight[3] = m_pHeightBuffer[(nCellsY + 1) * (m_nWidth + 1) + nCellsX + 1];

	if( ((nCellsX + nCellsY) % 2) == 0 )
	{
		//	0-----1
		//	| \	  |
		//	|  \  |
		//	|   \ |
		//	2-----3
		if( vDX > vDY )
		{
			//Use Right Triangle;
			h0 = vVertHeight[1];
			h1 = vVertHeight[0];
			h2 = vVertHeight[3];
			vDX = 1.0f - vDX;
		}
		else
		{
			//Use Left Triangle;
			h0 = vVertHeight[2];
			h1 = vVertHeight[3];
			h2 = vVertHeight[0];
			vDY = 1.0f - vDY;
		}
	}
	else
	{
		//	0-----1
		//	|	/ |
		//	|  /  |
		//	| /	  |
		//	2-----3
		if( vDX + vDY > 1.0f)
		{
			//Use Below Triangle;
			h0 = vVertHeight[3];
			h1 = vVertHeight[2];
			h2 = vVertHeight[1];
			vDX = 1.0f - vDX;
			vDY = 1.0f - vDY;
		}
		else
		{
			//Use Up Triangle;
			h0 = vVertHeight[0];
			h1 = vVertHeight[1];
			h2 = vVertHeight[2];
		}
	}
        
    return h0 + (h1 - h0) * vDX + (h2 - h0) * vDY;
}

/*
	Convert a 3D vector pos to terrain's 2D grid cell coordinates;
*/
bool A3DTerrain::GetCellPos(A3DVECTOR3 vecPos, int * pX, int * pY, FLOAT * pvX, FLOAT * pvY)
{
	FLOAT vX, vY;

	vecPos = vecPos - m_vecPos;
	vX = vecPos.x / m_vCellSize;
	vY = -vecPos.z / m_vCellSize;

	*pX = (int)(vX);
	*pY = (int)(vY);

	if( pvX ) *pvX = vX;
	if( pvY ) *pvY = vY;
	return true;
}

A3DVECTOR3 A3DTerrain::GetVertexNormal(int x, int y)
{
	if( x < 0 ) x = 0;
	if( x > m_nWidth ) x = m_nWidth;
	if( y < 0 ) y = 0;
	if( y > m_nHeight ) y = m_nHeight;
	return m_pVertexNormalBuffer[y * (m_nWidth + 1) + x];
}

A3DVECTOR3 A3DTerrain::GetVertexNormal(A3DVECTOR3 vecPos)
{
	int x, y;

	GetCellPos(vecPos, &x, &y);

	if( x < 0 ) x = 0;
	if( x > m_nWidth ) x = m_nWidth;
	if( y < 0 ) y = 0;
	if( y > m_nHeight ) y = m_nHeight;
	return m_pVertexNormalBuffer[y * (m_nWidth + 1) + x];
}

A3DVECTOR3 A3DTerrain::GetFaceNormal(int x, int y, int nTriangleID)
{
	if( x < 0 ) x = 0;
	if( x > m_nWidth - 1 ) x = m_nWidth - 1;
	if( y < 0 ) y = 0;
	if( y > m_nHeight - 1 ) y = m_nHeight - 1;

	return m_pFaceNormalBuffer[y * (m_nWidth * 2) + x * 2 + nTriangleID];
}

A3DVECTOR3 A3DTerrain::GetFaceNormal(A3DVECTOR3 vecPos)
{
	int		x, y;
	FLOAT	vX, vY;
	
	GetCellPos(vecPos, &x, &y, &vX, &vY);
	return GetFaceNormal(x, y, GetTriangleID(x, y, vX - x, vY - y));
}

bool A3DTerrain::BuildNormals()
{
	int				x, y;
	A3DVECTOR3		v0, v1, v2, v3;
	A3DVECTOR3		e1, e2;
	
	//First calculate face normals of each triangle;
	for(y=0; y<m_nHeight; y++)
	{
		for(x=0; x<m_nWidth; x++)
		{
			int nCellNum = y * m_nWidth + x;
			v0 = GetVertexPos(x    , y    );
			v1 = GetVertexPos(x + 1, y    );
			v2 = GetVertexPos(x    , y + 1);
			v3 = GetVertexPos(x + 1, y + 1);

			if( ((x + y) % 2) == 0 )
			{
				//  v0----v1
				//	| \	1 |
				//	| 0\  |
				//	|   \ |
				//  v2----v3
				e1 = v2 - v3;
				e2 = v0 - v2;
				m_pFaceNormalBuffer[nCellNum * 2 + 0] = Normalize(CrossProduct(e1, e2));

				e1 = v1 - v0;
				e2 = v3 - v1;
				m_pFaceNormalBuffer[nCellNum * 2 + 1] = Normalize(CrossProduct(e1, e2));
			}
			else
			{
				//  v0----v1
				//	|0  / |
				//	|  /  |
				//	| /	1 |
				//  v2----v3
				e1 = v0 - v2;
				e2 = v1 - v0;
				m_pFaceNormalBuffer[nCellNum * 2 + 0] = Normalize(CrossProduct(e1, e2));

				e1 = v3 - v1;
				e2 = v2 - v3;
				m_pFaceNormalBuffer[nCellNum * 2 + 1] = Normalize(CrossProduct(e1, e2));
			}
		}
	}

	for(y=0; y<=m_nHeight; y++)
	{
		for(x=0; x<=m_nWidth; x++)
		{
			int nNum = 0;
			A3DVECTOR3 vecNormal = A3DVECTOR3(0.0f);
			
			if( (x + y) % 2 == 0 )
			{
				//We should check sum 8 face normals up;
				if( x > 0 && y < m_nHeight )
				{
					vecNormal = vecNormal + GetFaceNormal(x - 1, y, 0) + GetFaceNormal(x - 1, y, 1);
					nNum += 2;
				}
				if( y > 0 && x < m_nWidth )
				{
					vecNormal = vecNormal + GetFaceNormal(x, y - 1, 0) + GetFaceNormal(x, y - 1, 1);
					nNum += 2;
				}
				if( x > 0 && y > 0 )
				{
					vecNormal = vecNormal + GetFaceNormal(x - 1, y - 1, 0) + GetFaceNormal(x - 1, y - 1, 1);
					nNum += 2;
				}
				if( x < m_nWidth && y < m_nHeight )
				{
					vecNormal = vecNormal + GetFaceNormal(x, y, 0) + GetFaceNormal(x, y, 1);
					nNum += 2;
				}
			}
			else
			{
				if( x > 0 && y < m_nHeight )
				{
					vecNormal = vecNormal + GetFaceNormal(x - 1, y, 1);
					nNum ++;
				}
				if( y > 0 && x < m_nWidth )
				{
					vecNormal = vecNormal + GetFaceNormal(x, y - 1, 0);
					nNum ++;
				}
				if( x > 0 && y > 0 )
				{
					vecNormal = vecNormal + GetFaceNormal(x - 1, y - 1, 1);
					nNum ++;
				}
				if( x < m_nWidth && y < m_nHeight )
				{
					vecNormal = vecNormal + GetFaceNormal(x, y, 0);
					nNum ++;
				}
			}
			
			m_pVertexNormalBuffer[y * (m_nWidth + 1) + x] = Normalize(vecNormal / (FLOAT)(nNum));
		}
	}

	return true;
}

bool A3DTerrain::LightTerrain()
{
	if( m_bHWITerrain ) return true;

	int		x, y;
	int		r, g, b;
	int		lR, lG, lB;
	int		aR, aG, aB;
	A3DVECTOR3 vecLight;
	
	if( m_pDirLight )
		vecLight = -Normalize(m_pDirLight->GetLightparam().Direction);
	else
		vecLight = Normalize(A3DVECTOR3(1.0f, -0.5f, -1.0f));

	if( m_pDirLight )
	{
		lR = int(255 * m_pDirLight->GetLightparam().Diffuse.r);
		lG = int(255 * m_pDirLight->GetLightparam().Diffuse.g);
		lB = int(255 * m_pDirLight->GetLightparam().Diffuse.b);
	}
	else
		lR = lB = lG = 255;

	aR = A3DCOLOR_GETRED(m_pA3DDevice->GetAmbientColor());
	aG = A3DCOLOR_GETGREEN(m_pA3DDevice->GetAmbientColor());
	aB = A3DCOLOR_GETBLUE(m_pA3DDevice->GetAmbientColor());

	for(y=0; y<=m_nHeight; y++)
	{
		for(x=0; x<=m_nWidth; x++)
		{
			A3DVECTOR3	vecPos = GetVertexPos(x, y);
			FLOAT		vDis = 410.0f;
			A3DVECTOR3	vecHitNormal;
			RAYTRACERT	rayTrace;

			r = aR;
			g = aG;
			b = aB;

			if( 0 )//RayTrace(vecPos + vecLight * vDis, -vecLight, vDis - 1.0f, &rayTrace) )
			{
				int a = 0;
				r = 255;
				g = 0;
				b = 0;
			}
			else
			{
				A3DVECTOR3 vecNormal = GetVertexNormal(x, y);
				FLOAT vDot = DotProduct(vecNormal, vecLight);
				if( vDot < 0.0f )
					goto FILL;

				r += (int)(lR * vDot);
				if( r > 255 ) r = 255;
				g += (int)(lG * vDot);
				if( g > 255 ) g = 255;
				b += (int)(lB * vDot);
				if( b > 255 ) b = 255;
			}
FILL:
			m_pVertexColorBuffer[y * (m_nWidth + 1) + x] = A3DCOLORRGBA(r, g, b, 0);
		}
	}

	return true;
}

A3DCOLOR A3DTerrain::GetVertexColor(int x, int y)
{
	if( m_bNoLight )
		return 0xffffffff;

	if( x < 0 ) x = 0;
	if( x > m_nWidth ) x = m_nWidth;
	if( y < 0 ) y = 0;
	if( y > m_nHeight ) y = m_nHeight;
	return m_pVertexColorBuffer[y * (m_nWidth + 1) + x];
}

/*
	Try to find a ray intersection on the terrain;
	Input vecStart and vecEnd indicates where the ray starts and ends;
	Return true if such a point is found and the vDis and vecHitNormal is the distance and point normal where the ray hits;
	Return false if no collision detected;
*/
bool A3DTerrain::RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecVelocity, FLOAT vTime, RAYTRACERT* pRayTrace)
{
	if( !GetRayTraceEnable() )
		return false;

	//First we move on the horizontal plane;
	A3DVECTOR3	vecEnd, vecDir;
	FLOAT		vVelocity, vDis;

	//Maybe we should clamps the vecStart and vecEnd in the terrain's height range;
	vVelocity = vecVelocity.Magnitude();
	vDis = vVelocity * vTime;

	if( vDis < 1e-6 )
		return false;

	vecEnd		= vecStart + vecVelocity * vTime;
	vecDir		= vecVelocity / vVelocity;
	
	int			nStartX, nStartY;
	int			nEndX, nEndY;
	FLOAT		vStartX, vStartY;
	FLOAT		vEndX, vEndY;
	int			nStep;
	
	GetCellPos(vecStart, &nStartX, &nStartY, &vStartX, &vStartY);
	GetCellPos(vecEnd, &nEndX, &nEndY, &vEndX, &vEndY);
	
	//First check my cell;
	if( RayPickInCell(nStartX, nStartY, vecStart, vecDir, vDis, pRayTrace) )
		return true;

	if( nStartX == nEndX && nStartY == nEndY )
	{
		//In same cell, but this cell has been checked and no intersects occurs;
		return false;
	}
	else if( abs(nEndX - nStartX) == 1 && abs(nEndY - nStartY) == 1 )
	{
		//Force check the four neighbour cell;
		if( RayPickInCell(nStartX, nEndY, vecStart, vecDir, vDis, pRayTrace) )
			return true;
		if( RayPickInCell(nEndX, nStartY, vecStart, vecDir, vDis, pRayTrace) )
			return true;
		if( RayPickInCell(nEndX, nEndY, vecStart, vecDir, vDis, pRayTrace) )
			return true;
	}
	else if( (abs(nEndX - nStartX) == 1 && nStartY == nEndY) || (abs(nEndY - nStartY) == 1 && nStartX == nEndX) )
	{
		if( RayPickInCell(nEndX, nEndY, vecStart, vecDir, vDis, pRayTrace) )
			return true;
	}
	else if(fabs(vStartX - vEndX) > fabs(vStartY - vEndY))
	{
		//Then move along x axis;
		FLOAT r = (vEndY - vStartY) / (vEndX - vStartX);
		FLOAT vY;
		int   nY, nYOld;
		int	  nDeltaX, nDeltaY;

		nStep = nStartX < nEndX ? 1 : -1;

		if( r > 0.0f)
		{
			if( nStep == 1 )
			{
				nDeltaX = -1;
				nDeltaY = 0;
			}
			else
			{
				nDeltaX = 0;
				nDeltaY = 1;
			}
		}
		else
		{
			if( nStep == 1 )
			{
				nDeltaX = -1;
				nDeltaY = 0;
			}
			else
			{
				nDeltaX = 0;
				nDeltaY = -1;
			}
		}
		
		//Move to next cell to start the algorithm;
		nYOld = nStartY;
		if( nStep == 1 )
			vY = vStartY + r * (1.0f - vStartX + nStartX);
		else
		{
			vY = vStartY - r * (vStartX - nStartX);
			nY = int(vY);
			if( nY != nStartY )
			{
				if( RayPickInCell(nStartX, nY, vecStart, vecDir, vDis, pRayTrace) )
					return true;			
			}
			nYOld = nY;
			vY = vY - r;
		}
		
		for(int nX=nStartX+nStep; nX!=nEndX+nStep; nX+=nStep)
		{
			nY = int(vY);
			if( nY != nYOld )
			{
				nYOld = nY;
				if( RayPickInCell(nX + nDeltaX, nY + nDeltaY, vecStart, vecDir, vDis, pRayTrace) )
					return true;
			}
			if( RayPickInCell(nX, nY, vecStart, vecDir, vDis, pRayTrace) )
				return true;
			vY += nStep * r;
		}
	}
	else
	{
		//Move along y axis;
		FLOAT r = (vEndX - vStartX) / (vEndY - vStartY);
		FLOAT vX;
		int   nX, nXOld;
		int	  nDeltaX, nDeltaY;

		nStep = nStartY < nEndY ? 1 : -1;

		if( r > 0.0f)
		{
			if( nStep == 1 )
			{
				nDeltaX = 0;
				nDeltaY = -1;
			}
			else
			{
				nDeltaX = 1;
				nDeltaY = 0;
			}
		}
		else
		{
			if( nStep == 1 )
			{
				nDeltaX = 0;
				nDeltaY = -1;
			}
			else
			{
				nDeltaX = -1;
				nDeltaY = 0;
			}
		}

		nXOld = nStartX;
		if( nStep == 1 )
			vX = vStartX + r * (1.0f - vStartY + nStartY);
		else
		{
			vX = vStartX - r * (vStartY - nStartY);
			nX = int(vX);
			if( nX != nStartX )
			{
				if( RayPickInCell(nX, nStartY, vecStart, vecDir, vDis, pRayTrace) )
					return true;			
			}
			nXOld = nX;
			vX = vX - r;
		}
		
		for(int nY=nStartY + nStep; nY!=nEndY+nStep; nY+=nStep)
		{
			nX = int(vX);
			if( nX != nXOld )
			{
				if( RayPickInCell(nX + nDeltaX, nY + nDeltaY, vecStart, vecDir, vDis, pRayTrace) )
					return true;
			}
			if( RayPickInCell(nX, nY, vecStart, vecDir, vDis, pRayTrace) )
				return true;
			vX += nStep * r;
			nXOld = nX;
		}
	}

	return false;
}

/*
	Try to find the ray intersection point in a terrain cell
	Input vecStart and vecEnd indicates where the ray starts and ends;
	Return true if such a point is found and the vDis and vecHitNormal is the distance and point normal where the ray hits;
	Return false if no collision detected;

	IN:
		x					cell's x coordinate
		y					cell's y coordinate
		vecStart			the ray's start position
		vecDir				the ray's direction
		vDis				the ray's length

	OUT:
		pvFraction			the 0~1 parameter which specify the intersection point;
		pvecHitNormal			the face normal of the intersected face;
	RETURN:
		true				if the ray intersect with the terrain;
		false				not intersect;
*/
bool A3DTerrain::RayPickInCell(int x, int y, const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDir, FLOAT vDis, RAYTRACERT* pRayTrace)
{
	FLOAT t, u, v;
	A3DVECTOR3 v0, v1, v2;
	int nCheckCount = 0;

#define ERR_T	1e-6f

	if( x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight )
		return false;

	if( (x + y)	% 2 == 0 )
	{
		//	0-----1
		//	| \	  |
		//	|  \  |
		//	|   \ |
		//	2-----3
		v0 = GetVertexPos(x, y + 1);
		v1 = GetVertexPos(x, y);
		v2 = GetVertexPos(x + 1, y + 1);
	}
	else
	{
		//	0-----1
		//	|	/ |
		//	|  /  |
		//	| /	  |
		//	2-----3
		v0 = GetVertexPos(x, y);
		v1 = GetVertexPos(x + 1, y);
		v2 = GetVertexPos(x, y + 1);
	}

	while( 1 ) 
	{
		if( nCheckCount == 1 )
		{
			//Check right bottom triangle;
			if( (x + y) % 2 == 0 )
			{
				v0 = v1;
				v1 = GetVertexPos(x + 1, y);
			}
			else
			{
				v0 = v1;
				v1 = GetVertexPos(x + 1, y + 1);
			}
		}
		else if( nCheckCount == 2 )
			return false;
		
		nCheckCount ++;
		// Find vectors for two edges sharing vert0
		A3DVECTOR3 edge1 = v1 - v0;
		A3DVECTOR3 edge2 = v2 - v0;

		// Begin calculating determinant - also used to calculate U parameter
		A3DVECTOR3 pvec;
		pvec = CrossProduct(vecDir, edge2);

		// If determinant is near zero, ray lies in plane of triangle
		FLOAT det = DotProduct(edge1, pvec);
		if( det < ERR_T )
			continue;

		// Calculate distance from vert0 to ray origin
		A3DVECTOR3 tvec = vecStart - v0;

		// Calculate U parameter and test bounds
		u = DotProduct(tvec, pvec);
		if( u < -ERR_T || u > det + ERR_T )
			continue;

		// Prepare to test V parameter
		A3DVECTOR3 qvec;
		qvec = CrossProduct(tvec, edge1);

		// Calculate V parameter and test bounds
		v = DotProduct(vecDir, qvec);
		if( v < -ERR_T || u + v > det + ERR_T)
			continue;

		// Calculate t, scale parameters, ray intersects triangle
		t = DotProduct(edge2, qvec);
		FLOAT fInvDet = 1.0f / det;
		t *= fInvDet;
		u *= fInvDet;
		v *= fInvDet;

		//If the line is short for touch that line;
		if( vDis < t )
			return false;

		float fFraction = (t - 0.01f) / vDis;
		float fHitFrac	= t / vDis;

		pRayTrace->fFraction	= fFraction > 0.0f ? fFraction : 0.0f;
		pRayTrace->vNormal		= GetFaceNormal(x, y, nCheckCount);
		pRayTrace->vPoint		= vecStart + vecDir * (pRayTrace->fFraction * vDis);
		pRayTrace->vHitPos		= vecStart + vecDir * (fHitFrac * vDis);
		return true;
	}

	return false;
}

/*
	This function calculate the square error of the height values in each cell
*/
bool A3DTerrain::CalculateSquareError()
{
	if( m_bHWITerrain ) return true;

	int i, j, m, n, s;
	int nX, nY;

	FLOAT		vAvgHeight;
	FLOAT		vSquareError;
	A3DVECTOR3	vecAvgNormal;

	for(s=0; s<m_nCellStageCount; s++)
	{
		for(i=0; i<m_nStageHeight[s]; i++)
		{
			for(j=0; j<m_nStageWidth[s]; j++)
			{
				if( s == 0 )
				{
					vAvgHeight = 0.0f;
					vecAvgNormal = A3DVECTOR3(0.0f);
					for(m=0; m<=m_nStageCover[s]; m++)
					{
						for(n=0; n<=m_nStageCover[s]; n++)
						{
							nY = i * m_nStageCover[s] + m;
							nX = j * m_nStageCover[s] + n;
							vAvgHeight += m_pHeightBuffer[nY * (m_nWidth + 1) + nX];
							if( m != m_nStageCover[s] && n != m_nStageCover[s] )
								vecAvgNormal = vecAvgNormal + GetFaceNormal(nX, nY, 0) + GetFaceNormal(nX, nY, 1);
						}
					}
					vAvgHeight = vAvgHeight / (m_nStageCover[s] + 1) / (m_nStageCover[s] + 1);
					vecAvgNormal = vecAvgNormal / FLOAT(m_nStageCover[s] * m_nStageCover[s] * 2);
					
					vSquareError = 0.0f;
					for(m=0; m<=m_nStageCover[s]; m++)
					{
						for(n=0; n<=m_nStageCover[s]; n++)
						{
							nY = i * m_nStageCover[s] + m;
							nX = j * m_nStageCover[s] + n;
							vSquareError += (m_pHeightBuffer[nY * (m_nWidth + 1) + nX] * vecAvgNormal.y - vAvgHeight * vecAvgNormal.y) *
								(m_pHeightBuffer[nY * (m_nWidth + 1) + nX] * vecAvgNormal.y - vAvgHeight * vecAvgNormal.y);
							vSquareError += a3d_Magnitude(GetFaceNormal(nX, nY, 0) - vecAvgNormal) * a3d_Magnitude(GetFaceNormal(nX, nY, 1) - vecAvgNormal) * 400.0f;
						}
					}
					vSquareError = (FLOAT)(vSquareError);
				}
				else
				{
					vSquareError = GetSquareError(s - 1, j * 2, i * 2) + GetSquareError(s - 1, j * 2 + 1, i * 2) + 
						GetSquareError(s - 1, j * 2, i * 2 + 1) + GetSquareError(s - 1, j * 2 + 1, i * 2 + 1);
				}
				m_ppCellSquareErrorTable[s][i * m_nStageWidth[s] + j] = vSquareError;
			}
		}
	}

	return true;
}

bool A3DTerrain::CalculateDistanceTable()
{
	if( m_bHWITerrain ) return true;

	A3DVECTOR3 vecOrgin = GetVertexPos(0, 0);
	A3DVECTOR3 vecDelta;
	A3DVECTOR3 vecCellPos;
	
	for(int y=0; y<m_nHeight; y++)
	{
		for(int x=0; x<m_nWidth; x++)
		{
			vecCellPos = GetVertexPos(x, y);
			vecDelta = vecCellPos - vecOrgin;
			vecDelta.y = 0.0f;
			m_pCellDistanceTable[y * m_nWidth + x] = vecDelta.Magnitude() / m_vCellSize;
		}
	}

	return true;
}

int A3DTerrain::DetermineLevel(int nStage, FLOAT vDistance, FLOAT vSquareError)
{
	static FLOAT s_levelThreshHold[3] = {90.0f, 45.0f, 20.0f};
	static FLOAT LOOSEN_FACTOR = 20.0f / m_vCellSize;

	FLOAT vRatio = vSquareError / (vDistance + 1e-6f);
	FLOAT vLevelThreshHold[3];

	for(int i=0; i<3; i++)
	{
		vLevelThreshHold[i] = s_levelThreshHold[i] * m_vOptimizeStrength;
	}
	
	if( GetKeyState(VK_F7) & 0x8000 )
	{
		if( nStage == 0 )
			return 0;
		else
			return -1;
	}

	//First we let nearest m_nDetailWidth * m_nDetailHeight cell to be first level;
	if( nStage == 2 && vDistance < GetDistance(m_nDetailWidth + m_nDetailWidth / 2 / 2, m_nDetailHeight + m_nDetailHeight / 2 / 2) )
		return -1;
	else if( nStage == 1 && vDistance < GetDistance(m_nDetailWidth + m_nDetailWidth / 4 / 2, m_nDetailHeight + m_nDetailHeight / 4 / 2) )
		return -1;
	else if( nStage == 0 && vDistance < GetDistance(m_nDetailWidth, m_nDetailHeight / 2) )
		return 0;
	else if( vRatio > vLevelThreshHold[0] / LOOSEN_FACTOR)
	{
		if( nStage == 0 ) return 0;
		else return -1;
	}
	else if( vRatio > vLevelThreshHold[1] / LOOSEN_FACTOR )
	{
		return 0;
	}
	else if( vRatio > vLevelThreshHold[2] / LOOSEN_FACTOR )
		return 1;
	else
		return 2;
}	

HDC hdc;
int nFaceCount = 0;

bool A3DTerrain::ShowOneCellLevelInfo(int stage, int sx, int sy)
{
	int nLevel = GetCellLevel(stage, sx, sy);
	if( nLevel == -1 )
	{
		if( stage == 0 )
			return true;
		ShowOneCellLevelInfo(stage - 1, sx * 2,		sy * 2);
		ShowOneCellLevelInfo(stage - 1, sx * 2 + 1, sy * 2);
		ShowOneCellLevelInfo(stage - 1, sx * 2,		sy * 2 + 1);
		ShowOneCellLevelInfo(stage - 1, sx * 2 + 1, sy * 2 + 1);
		return true;
	}
	
	HBRUSH hbr;
	BYTE color = 20 * (3 - nLevel) * 3;
	if( stage == 0 )
	{
		hbr = CreateSolidBrush(RGB(color, 0, 0));
	}
	else if( stage == 1 )
	{
		hbr = CreateSolidBrush(RGB(0, color, 0));
	}
	else if( stage == 2 )
	{
		hbr = CreateSolidBrush(RGB(0, 0, color));
	}

	nFaceCount += m_nStageCover[2 - nLevel];

	RECT rect;
	rect.left = sx * m_nStageCover[stage];
	rect.top = sy * m_nStageCover[stage];
	rect.right = rect.left + m_nStageCover[stage];
	rect.bottom = rect.top + m_nStageCover[stage];

	FillRect(hdc, &rect, hbr);

	DeleteObject(hbr);
	
	return true;
}

bool A3DTerrain::ShowStageCellLevelInfo()
{
	int x, y;

	nFaceCount = 0;
	hdc = ::GetDC(NULL);

	for(y=0; y<m_nHeight; y+=m_nTextureCover)
	{
		for(x=0; x<m_nWidth; x+=m_nTextureCover)
		{
			ShowOneCellLevelInfo(m_nCellStageCount - 1, x / m_nStageCover[m_nCellStageCount - 1], 
				y / m_nStageCover[m_nCellStageCount - 1]);
		}
	}

	SetPixel(hdc, m_ptCamPos.x, m_ptCamPos.y, RGB(255, 255, 255));

	char szInfo[256];

	sprintf(szInfo, "face: %d", nFaceCount);
	TextOutA(hdc, 300, 0, szInfo, strlen(szInfo));
	::ReleaseDC(NULL, hdc);
	return true;
}

BYTE A3DTerrain::GetFogData(FLOAT vDistance)
{
	return 0;
	FLOAT vFog, vFogStart, vFogEnd;
	
	vDistance *= m_vCellSize;

	vFogStart = m_pA3DDevice->GetFogStart();
	vFogEnd	= m_pA3DDevice->GetFogEnd();

	if( vDistance < vFogStart )
		vFog = 1.0f;
	else
		vFog = 1.0f - (vDistance - vFogStart) / (vFogEnd - vFogStart);
	int nFog = int(255 * vFog);
	if( nFog < 0 ) return 0;
	if( nFog > 255 ) nFog = 255;
	return (BYTE)nFog;
}

void A3DTerrain::SetCellLevel(int stage, int sx, int sy, CHAR nLevel, bool bFillSub)
{
	m_ppCellLevelTable[stage][sy * m_nStageWidth[stage] + sx] = nLevel;

	if( stage > 0 && nLevel != -1 && bFillSub )
	{
		//We must set my child level in case maybe it will be referenced;
		for(int i=0; i<2; i++)
		{
			for(int j=0; j<2; j++)
			{
				SetCellLevel(stage - 1, sx * 2 + j, sy * 2 + i, nLevel + 2);
			}
		}
	}
}

int A3DTerrain::GetTotalFaceCount()
{
	int nCount = 0;

	for(int i=0; i<m_nNumTexture; i++)
	{
		nCount += m_nVisibleFaces[i];
		nCount += m_nExtraVisibleFaces[i];
	}
	return nCount;
}

int A3DTerrain::GetTotalVertCount()
{
	int nCount = 0;

	for(int i=0; i<m_nNumTexture; i++)
	{
		nCount += m_nVisibleVerts[i];
		nCount += m_nExtraVisibleVerts[i];
	}
	return nCount;
}

// Scene Editor Specified Section;
bool A3DTerrain::Create(A3DDevice * pA3DDevice, int nNumTexture, int nWidth, int nHeight)
{
	int		i;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		m_bHWITerrain = true;

	strcpy(m_szFolderName, "Terrain");

	m_pA3DDevice = pA3DDevice;
	m_nNumTexture = nNumTexture;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nSightRange = 80;
	m_nTextureCover = 8;
	m_vCellSize = 20.0f;

	m_vecPos = A3DVECTOR3(-m_nWidth / 2 * m_vCellSize, 0.0f, m_nHeight / 2 * m_vCellSize);

	m_pHeightBuffer = (FLOAT *) malloc((m_nWidth + 1) * (m_nHeight + 1) * sizeof(FLOAT));
	if( NULL == m_pHeightBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
		return false;
	}

	ZeroMemory(m_pHeightBuffer, sizeof(FLOAT) * (m_nWidth + 1) * (m_nHeight + 1));
	m_vMaxHeight = 0.0f;
	m_vMinHeight = 0.0f;

	m_pTextureBuffer = (LPBYTE) malloc((m_nWidth / m_nTextureCover) * (m_nHeight / m_nTextureCover) * sizeof(BYTE));
	if( NULL == m_pTextureBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
		return false;
	}

	ZeroMemory(m_pTextureBuffer, sizeof(BYTE) * (m_nWidth / m_nTextureCover) * (m_nHeight / m_nTextureCover));
	
	//Now calculate normal information;
	m_pFaceNormalBuffer = (A3DVECTOR3 *) malloc(m_nWidth * m_nHeight * 2 * sizeof(A3DVECTOR3));
	if( NULL == m_pFaceNormalBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
		return false;
	}
	m_pVertexNormalBuffer = (A3DVECTOR3 *) malloc((m_nWidth + 1) * (m_nHeight + 1) * sizeof(A3DVECTOR3));
	if( NULL == m_pVertexNormalBuffer )
	{
		g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
		return false;
	}

	BuildNormals();

	if( m_bHWITerrain )
	{
	}
	else
	{
		// Init stage level table;
		switch(m_nTextureCover)
		{
		case 2:
			m_nCellStageCount = 1;
			break;
		case 4:
			m_nCellStageCount = 2;
			break;
		case 8:
			m_nCellStageCount = 3;
			break;
		case 16:
			m_nCellStageCount = 4;
			break;
		default:
			g_A3DErrLog.Log("Too large texture cover [%d]! The max is 16!", m_nTextureCover);
			return false;
		}

		if( (m_nWidth % m_nTextureCover) || (m_nHeight % m_nTextureCover) )
		{
			g_A3DErrLog.Log("m_nWidth[%d] and m_nHeight[%d] must be dividable by m_nTextureCover[%d]!", m_nWidth, m_nHeight, m_nTextureCover);
			return false;
		}

		//Allocate the stage cell table;
		m_ppCellLevelTable = (CHAR **) malloc(sizeof(CHAR *) * m_nCellStageCount);
		if( NULL == m_ppCellLevelTable )
		{
			g_A3DErrLog.Log("A3DTerrain::Create Not enough memory!");
			return false;
		}
		m_ppCellSquareErrorTable = (FLOAT **) malloc(sizeof(FLOAT *) * m_nCellStageCount);
		if( NULL == m_ppCellSquareErrorTable )
		{
			g_A3DErrLog.Log("A3DTerrain::Create Not enough memory!");
			return false;
		}

		int nStageWidth = m_nWidth;
		int nStageHeight = m_nHeight;
		int nCover = 1;
		for(i=0; i<m_nCellStageCount; i++)
		{
			nStageWidth	/= 2;
			nStageHeight /= 2;
			nCover *= 2;

			m_nStageWidth[i] = nStageWidth;
			m_nStageHeight[i] = nStageHeight;
			m_nStageCover[i] = nCover;

			m_ppCellLevelTable[i] = (CHAR *) malloc(nStageWidth * nStageHeight * sizeof(CHAR));
			if( NULL == m_ppCellLevelTable[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not enough memory!");
				return false;
			}
			//First set all cell's level to be -1;
			memset(m_ppCellLevelTable[i], -1, nStageWidth * nStageHeight);

			m_ppCellSquareErrorTable[i] = (FLOAT *) malloc(nStageWidth * nStageHeight * sizeof(FLOAT));
			if( NULL == m_ppCellSquareErrorTable[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not enough memory!");
				return false;
			}
		}

		if( m_nSightRange % m_nTextureCover )
		{
			g_A3DErrLog.Log("A3DTerrain::Create m_nSightRange must be dividable by m_nTextureCover");
			return false;
		}
		
		for(i=0; i<m_nNumTexture; i++)
		{
			m_nMaxVerts[i] = m_nMaxExtraVerts[i] = 6 * 8 * 8;
			m_nMaxIndices[i] = m_nMaxExtraIndices[i] = 6 * 8 * 8;
			 
			m_pVertexBuffers[i] = (A3DLVERTEX *) malloc(m_nMaxVerts[i] * sizeof(A3DLVERTEX));
			if( NULL == m_pVertexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			m_pIndexBuffers[i] = (WORD *) malloc(m_nMaxIndices[i] * sizeof(WORD));
			if( NULL == m_pIndexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			m_pExtraVertexBuffers[i] = (A3DLVERTEX *) malloc(m_nMaxExtraVerts[i] * sizeof(A3DLVERTEX));
			if( NULL == m_pExtraVertexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			m_pExtraIndexBuffers[i] = (WORD *) malloc(m_nMaxExtraIndices[i] * sizeof(WORD));
			if( NULL == m_pIndexBuffers[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			
			strcpy(m_szTextureMap[i], "");
			
			/*
			m_pStreams[i] = new A3DStream();
			if( NULL == m_pStreams[i] )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			if( !m_pStreams[i]->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxVertexCount, m_nMaxIndexCount, 0, true, true) )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Stream Init Fail!");
				return false;
			}*/

			m_nVisibleVerts[i] = m_nVisibleFaces[i] = 0;
		}
	
		//Allocate cell distance table;
		m_pCellDistanceTable = (FLOAT *) malloc(m_nWidth * m_nHeight * sizeof(FLOAT));
		if( NULL == m_pCellDistanceTable )
		{
			g_A3DErrLog.Log("A3DTerrain::Create Not enough memory!");
			return false;
		}

		CalculateDistanceTable();

		//Now calculate the square error of each cell;
		CalculateSquareError();

		//Now Prelight the terrain;
		m_pVertexColorBuffer = (A3DCOLOR *) malloc((m_nWidth + 1) * (m_nHeight + 1) * sizeof(A3DCOLOR));
		if( NULL == m_pVertexColorBuffer )
		{
			g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
			return false;
		}

		//Light the terrain;
		LightTerrain();

		//Init Detail Texture Sepcification Now!
		m_nDetailWidth = 8;
		m_nDetailHeight = 8;
		if( m_pA3DDevice->IsDetailMethodSupported() )
		{
			strcpy(m_szDetailTextureName, "DetailTexture.bmp");
			if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szDetailTextureName, "Textures\\Terrain", &m_pDetailTexture, true) )
				return false;
		
			m_nDetailVertCount = (m_nDetailWidth + 1) * (m_nDetailHeight + 1);
			m_nDetailFaceCount = 2 * m_nDetailWidth * m_nDetailHeight;

			m_pDetailVB = (A3DLVERTEX *) malloc(m_nDetailVertCount * sizeof(A3DLVERTEX));
			if( NULL == m_pDetailVB )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			m_pDetailIndex = (WORD *) malloc(m_nDetailFaceCount * 3 * sizeof(WORD));
			if( NULL == m_pDetailIndex )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			/*
			m_pDetailStream = new A3DStream();
			if( NULL == m_pDetailStream )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Not Enough Memory!");
				return false;
			}
			if( !m_pDetailStream->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nDetailVertCount, m_nDetailFaceCount * 3, 0, true, true) )
			{
				g_A3DErrLog.Log("A3DTerrain::Create Init Detail Stream Fail!");
				return false;
			}*/
		}
	}

	m_matWorld.Identity();
	return true;
}

bool A3DTerrain::IncreaseTextureNum(int nTextureIncrease)
{
	int nNewNumTexture = m_nNumTexture + nTextureIncrease;

	if( nNewNumTexture > MAXNUM_TEXTURE )
	{
		return false;
	}

	for(int i=m_nNumTexture; i<nNewNumTexture; i++)
	{
		//Alloc nessessary memory;
		m_nMaxVerts[i] = m_nMaxExtraVerts[i] = 6 * 10 * 10;
		m_nMaxIndices[i] = m_nMaxExtraIndices[i] = 6 * 10 * 10;

		m_pVertexBuffers[i] = (A3DLVERTEX *) malloc(m_nMaxVerts[i] * sizeof(A3DLVERTEX));
		if( NULL == m_pVertexBuffers[i] )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
			return false;
		}
		m_pIndexBuffers[i] = (WORD *) malloc(m_nMaxIndices[i] * sizeof(WORD));
		if( NULL == m_pIndexBuffers[i] )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
			return false;
		}
		m_pExtraVertexBuffers[i] = (A3DLVERTEX *) malloc(m_nMaxExtraVerts[i] * sizeof(A3DLVERTEX));
		if( NULL == m_pExtraVertexBuffers[i] )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
			return false;
		}
		m_pExtraIndexBuffers[i] = (WORD *) malloc(m_nMaxExtraIndices[i] * sizeof(WORD));
		if( NULL == m_pIndexBuffers[i] )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
			return false;
		}
		m_pTextures[i] = new A3DTexture();
		if( NULL == m_pTextures[i] )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
			return false;
		}
		if( !m_pTextures[i]->Init(m_pA3DDevice) )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Textures Init Fail!");
			return false;
		}
		/*
		m_pStreams[i] = new A3DStream();
		if( NULL == m_pStreams[i] )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Not Enough Memory!");
			return false;
		}
		if( !m_pStreams[i]->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxVertexCount, m_nMaxIndexCount, 0, true, true) )
		{
			g_A3DErrLog.Log("A3DTerrain::Init Stream Init Fail!");
			return false;
		}*/

		m_nVisibleVerts[i] = m_nVisibleFaces[i] = 0;
	}

	return true;
}

bool A3DTerrain::SetTextureMap(int nTexIndex, char * szTextureMap, int nNewIndex)
{
	//We can only set used texture;
	if( nTexIndex > m_nNumTexture )
		return false;

	int x, y;
	if( strlen(szTextureMap) == 0 )
	{
		//Remove the texture;
		if( m_pTextures[nTexIndex] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextures[nTexIndex]);
			m_pTextures[nTexIndex] = NULL;
		}

		//We have to clean the texture index map;
		for(y=0; y<m_nHeight; y+=m_nTextureCover)
		{
			for(x=0; x<m_nWidth; x+=m_nTextureCover)
			{
				if( GetTextureIndex(x, y) == nTexIndex )
				{
					SetTextureIndex(x, y, nNewIndex);
				}
			}
		}

		//Now move the texture records ahead one delta;
		for(int i=nTexIndex+1; i<m_nNumTexture; i++)
		{
			strncpy(m_szTextureMap[i - 1], m_szTextureMap[i], MAX_PATH);
			m_pTextures[i - 1] = m_pTextures[i];
			for(y=0; y<m_nHeight; y+=m_nTextureCover)
			{
				for(x=0; x<m_nWidth; x+=m_nTextureCover)
				{
					if( GetTextureIndex(x, y) == i )
					{
						SetTextureIndex(x, y, i - 1);
					}
				}
			}
		}

		//Free unused resources;
		m_nNumTexture --;
		if( m_pVertexBuffers[m_nNumTexture] )
		{
			free(m_pVertexBuffers[m_nNumTexture]);
			m_pVertexBuffers[m_nNumTexture] = NULL;
		}
		if( m_pIndexBuffers[m_nNumTexture] )
		{
			free(m_pIndexBuffers[m_nNumTexture]);
			m_pIndexBuffers[m_nNumTexture] = NULL;
		}
		if( m_pExtraVertexBuffers[m_nNumTexture] )
		{
			free(m_pExtraVertexBuffers[m_nNumTexture]);
			m_pExtraVertexBuffers[m_nNumTexture] = NULL;
		}
		if( m_pExtraIndexBuffers[m_nNumTexture] )
		{
			free(m_pExtraIndexBuffers[m_nNumTexture]);
			m_pExtraIndexBuffers[m_nNumTexture] = NULL;
		}
		if( m_pStreams[m_nNumTexture] )
		{
			m_pStreams[m_nNumTexture]->Release();
			delete m_pStreams[m_nNumTexture];
			m_pStreams[m_nNumTexture] = NULL;
		}
		m_szTextureMap[m_nNumTexture][0] = '\0';
	}
	else
	{
		if( strlen(m_szTextureMap[nTexIndex]) )
		{
			//Reset one texture;
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTextures[nTexIndex]);
			m_pTextures[nTexIndex] = NULL;			
		}

		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureMap, "Textures\\Terrain", &m_pTextures[nTexIndex]) )
			return false;
		strcpy(m_szTextureMap[nTexIndex], szTextureMap);
	}
	return true;
}

bool A3DTerrain::AdjustVertexHeight(int x, int y, FLOAT vDeltaHeight)
{
	if( x < 0 || y < 0 || x > m_nWidth || y > m_nHeight )
		return false;

	m_pHeightBuffer[y * (m_nWidth + 1) + x] += vDeltaHeight;
	return true;
}

FLOAT A3DTerrain::GetVertexHeight(int x, int y)
{
	if( x < 0 || y < 0 || x > m_nWidth || y > m_nHeight )
		return 0.0f;

	return m_pHeightBuffer[y * (m_nWidth + 1) + x];
}

bool A3DTerrain::SetHeight(int nWidth, int nHeight, FLOAT * pHeightBuffer)
{
	if( nWidth != m_nWidth || nHeight != m_nHeight )
	{
		return false;
	}

	memcpy(m_pHeightBuffer, pHeightBuffer, sizeof(FLOAT) * (m_nWidth + 1) * (m_nHeight + 1));
	return true;
}

bool A3DTerrain::GetTerrainColor(const A3DVECTOR3& vecPos, A3DCOLOR * pDiffuse, A3DCOLOR * pSpecular, BYTE byteAlpha)
{
	int x, y;
	FLOAT vX, vY;

	GetCellPos(vecPos, &x, &y, &vX, &vY);

	if( x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight )
	{	
		*pDiffuse = A3DCOLORRGBA(255, 255, 255, 0);
		*pSpecular = A3DCOLORRGBA(0, 0, 0, 255);
	}
	else
	{
		A3DCOLOR color[4];
		BYTE r0, g0, b0, r1, g1, b1, r2, g2, b2;
		BYTE r, g, b;

		vX = vX - x;
		vY = vY - y;

		color[0] = m_pVertexColorBuffer[y * (m_nWidth + 1) + x];
		color[1] = m_pVertexColorBuffer[y * (m_nWidth + 1) + x + 1];
		color[2] = m_pVertexColorBuffer[(y + 1) * (m_nWidth + 1) + x];
		color[3] = m_pVertexColorBuffer[(y + 1) * (m_nWidth + 1) + x + 1];

		if( ((x + y) % 2) == 0 )
		{
			//	0-----1
			//	| \	  |
			//	|  \  |
			//	|   \ |
			//	2-----3
			if( vX > vY )
			{
				//Use Right Triangle;
				a3d_DecompressColor(color[1], &r0, &g0, &b0);
				a3d_DecompressColor(color[0], &r1, &g1, &b1);
				a3d_DecompressColor(color[3], &r2, &g2, &b2);
				vX = 1.0f - vX;
			}
			else
			{
				//Use Left Triangle;
				a3d_DecompressColor(color[2], &r0, &g0, &b0);
				a3d_DecompressColor(color[3], &r1, &g1, &b1);
				a3d_DecompressColor(color[0], &r2, &g2, &b2);
				vY = 1.0f - vY;
			}
		}
		else
		{
			//	0-----1
			//	|	/ |
			//	|  /  |
			//	| /	  |
			//	2-----3
			if( vX + vY > 1.0f)
			{
				//Use Below Triangle;
				a3d_DecompressColor(color[3], &r0, &g0, &b0);
				a3d_DecompressColor(color[2], &r1, &g1, &b1);
				a3d_DecompressColor(color[1], &r2, &g2, &b2);
				vX = 1.0f - vX;
				vY = 1.0f - vY;
			}
			else
			{
				//Use Up Triangle;
				a3d_DecompressColor(color[0], &r0, &g0, &b0);
				a3d_DecompressColor(color[1], &r1, &g1, &b1);
				a3d_DecompressColor(color[2], &r2, &g2, &b2);
			}
		}
        
		r = (BYTE)(r0 + (r1 - r0) * vX + (r2 - r0) * vY);
		g = (BYTE)(g0 + (g1 - g0) * vX + (g2 - g0) * vY);
		b = (BYTE)(b0 + (b1 - b0) * vX + (b2 - b0) * vY);

 		*pDiffuse = A3DCOLORRGBA(r, g, b, byteAlpha);
		*pSpecular = A3DCOLORRGBA(0, 0, 0, 255);		
	}
	return true;
}

bool A3DTerrain::UpdateAllChanges(bool bAdjustHeightOnly)
{
	if( !bAdjustHeightOnly )
	{
		BuildNormals();
		CalculateSquareError();
		LightTerrain();
	}

	m_nVisibleBeginXOld = -1000000;
	m_nVisibleBeginYOld = -1000000;
	return true;
}

bool A3DTerrain::SetDetailMap(char * szTextureMap)
{
	if( m_pDetailTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pDetailTexture);
		m_pDetailTexture = NULL;
	}
	
	strcpy(m_szDetailTextureName, szTextureMap);
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szDetailTextureName, "Textures\\Terrain", &m_pDetailTexture, A3DTF_DETAILMAP) )
		return false;
	return true;
}

void A3DTerrain::SetOptimizeStrength(FLOAT vOptimizeStrength)
{
	m_vOptimizeStrength = vOptimizeStrength;
	m_nVisibleBeginXOld = -1000000;
	m_nVisibleBeginYOld = -1000000;
}

bool A3DTerrain::SplitMark(const A3DAABB& aabb, A3DLVERTEX* aVerts, WORD* aIndices, bool bJog, 
		int* piNumVert, int* piNumIdx)
{
	int nStartX, nStartY;
	int nEndX, nEndY;

	GetCellPos(aabb.Mins, &nStartX, &nEndY);
	GetCellPos(aabb.Maxs, &nEndX, &nStartY);

	TERRAIN_MARKSPLIT markSplit;
	markSplit.pVerts		= aVerts;
	markSplit.pIndices		= aIndices;
	markSplit.nNumVert		= 0;
	markSplit.nNumIndices	= 0;

	if (nStartX < 0 ) nStartX = 0;
	if (nStartX >= m_nWidth ) nStartX = m_nWidth - 1;
	if (nStartY < 0 ) nStartY = 0;
	if (nStartY >= m_nHeight ) nStartY = m_nHeight - 1;

	if (nEndX < 0 ) nEndX = 0;
	if (nEndX >= m_nWidth ) nEndX = m_nWidth - 1;
	if (nEndY < 0 ) nEndY = 0;
	if (nEndY >= m_nHeight ) nEndY = m_nHeight - 1;

	for (int x=nStartX; x<=nEndX; x++)
	{
		for (int y=nStartY; y<=nEndY; y++)
		{
			if (!ClipInCell(aabb, x, y, &markSplit))
				goto EXIT;
		}
	}

EXIT:
	*piNumVert = markSplit.nNumVert;
	*piNumIdx = markSplit.nNumIndices;
	return true;
}

//clip in one cell;
bool A3DTerrain::ClipInCell(const A3DAABB& aabb, int nCellX, int nCellY, TERRAIN_MARKSPLIT * pMarkSplit)
{
	A3DVECTOR3			aVerts[4];
	WORD				aIndices[6];
	A3DVECTOR3			vNormal;

	if( !GetCellAllVertex(nCellX, nCellY, aVerts, aIndices) )
		return false;

	// Push all verts a litter upper, to avoid the seam caused by jog operation;
	for(int i=0; i<4; i++)
		aVerts[i].y += 0.1f;

	for(int idTri=0; idTri<2; idTri++)
	{
		vNormal = GetFaceNormal(nCellX, nCellY, idTri);

		int i;
		float fDist;

		//	Check whether cell is in affected area
		fDist = GetHeight(aabb.Center) - aabb.Center.y;

		//fDist = DotProduct(aabb.Center - aVerts[0], vNormal);
		if (fDist < -aabb.Extents.y || fDist > aabb.Extents.y)
			return true;

		//	Use axis y as the main axis of side plane
		int t0, t1, t2;
		float fBd1, fBd2, fBd3, fBd4;

		t0 = 1;	t1 = 0; t2 = 2;
		fBd1 = aabb.Center.x + aabb.Extents.x;
		fBd2 = aabb.Center.z + aabb.Extents.z;
		fBd3 = aabb.Center.x - aabb.Extents.x;
		fBd4 = aabb.Center.z - aabb.Extents.z;
			
		A3DVECTOR3 aProj1[16], aProj2[16];	//	Vertices projection on main plane
		A3DVECTOR3 *v1, *v2, *aOld, *aNew;
		bool bIn1, bIn2;
		int iNumOld, iNumNew;
		float d, d1, d2;

		//	Copy side's vertex from mesh vertex array
		for (i=idTri*3; i<idTri*3+3; i++)
			aProj2[i-idTri*3] = aVerts[aIndices[i]];

		//	Clip side using the first border plane
		v1		= &aProj2[2];
		bIn1	= ((d1 = v1->m[t1] - fBd1) <= 0);
		iNumNew = 0;
		aNew	= aProj1;

		for (i=0; i < 3; i++)
		{
			v2	 = &aProj2[i];
			bIn2 = ((d2 = v2->m[t1] - fBd1) <= 0);
		
			if (bIn1)
			{
				aNew[iNumNew] = *v1;
				iNumNew++;

				if (!bIn2)
				{
					d = d1 / (d1 - d2);
					aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
					aNew[iNumNew].m[t1] = fBd1;
					aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
					iNumNew++;
				}
			}
			else if (bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = fBd1;
				aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
				iNumNew++;
			}

			bIn1 = bIn2;
			v1	 = v2;
			d1	 = d2;
		}

		if ((iNumOld = iNumNew) < 3)
			continue;

		//	Clip side using next border plane
		aOld	= aNew;
		aNew	= aProj2;
		v1		= &aOld[iNumOld-1];
		bIn1	= ((d1 = v1->m[t2] - fBd2) <= 0);
		iNumNew = 0;

		for (i=0; i < iNumOld; i++)
		{
			v2	 = &aOld[i];
			bIn2 = ((d2 = v2->m[t2] - fBd2) <= 0);
		
			if (bIn1)
			{
				aNew[iNumNew] = *v1;
				iNumNew++;

				if (!bIn2)
				{
					d = d1 / (d1 - d2);
					aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
					aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
					aNew[iNumNew].m[t2] = fBd2;
					iNumNew++;
				}
			}
			else if (bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
				aNew[iNumNew].m[t2] = fBd2;
				iNumNew++;
			}

			bIn1 = bIn2;
			v1	 = v2;
			d1	 = d2;
		}

		if ((iNumOld = iNumNew) < 3)
			continue;

		//	Clip side using next border plane
		aOld	= aNew;
		aNew	= aProj1;
		v1		= &aOld[iNumOld-1];
		bIn1	= ((d1 = v1->m[t1] - fBd3) >= 0);
		iNumNew = 0;

		for (i=0; i < iNumOld; i++)
		{
			v2	 = &aOld[i];
			bIn2 = ((d2 = v2->m[t1] - fBd3) >= 0);
		
			if (bIn1)
			{
				aNew[iNumNew] = *v1;
				iNumNew++;

				if (!bIn2)
				{
					d = d1 / (d1 - d2);
					aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
					aNew[iNumNew].m[t1] = fBd3;
					aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
					iNumNew++;
				}
			}
			else if (bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = fBd3;
				aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
				iNumNew++;
			}

			bIn1 = bIn2;
			v1	 = v2;
			d1	 = d2;
		}

		if ((iNumOld = iNumNew) < 3)
			continue;

		//	Clip side using the last border plane
		aOld	= aNew;
		aNew	= aProj2;
		v1		= &aOld[iNumOld-1];
		bIn1	= ((d1 = v1->m[t2] - fBd4) >= 0);
		iNumNew = 0;

		for (i=0; i < iNumOld; i++)
		{
			v2	 = &aOld[i];
			bIn2 = ((d2 = v2->m[t2] - fBd4) >= 0);
		
			if (bIn1)
			{
				aNew[iNumNew] = *v1;
				iNumNew++;

				if (!bIn2)
				{
					d = d1 / (d1 - d2);
					aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
					aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
					aNew[iNumNew].m[t2] = fBd4;
					iNumNew++;
				}
			}
			else if (bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
				aNew[iNumNew].m[t2] = fBd4;
				iNumNew++;
			}

			bIn1 = bIn2;
			v1	 = v2;
			d1	 = d2;
		}

		if (iNumNew < 3)
			continue;

		//	Check whether buffer has been full
		if (pMarkSplit->nNumVert + iNumNew > MAXNUM_MARKVERT ||
			pMarkSplit->nNumIndices + (iNumNew - 2) * 3 > MAXNUM_MARKINDEX)
			return false;

		//	Calculate texture coordinates for left vertex
		A3DLVERTEX* pVert = &pMarkSplit->pVerts[pMarkSplit->nNumVert];
		
		d1 = 1.0f / (fBd1 - fBd3);
		d2 = 1.0f / (fBd2 - fBd4);

		for (i=0; i < iNumNew; i++, pVert++)
		{
			pVert->x	= aNew[i].x;
			pVert->y	= aNew[i].y;
			pVert->z	= aNew[i].z;
			pVert->tu	= (aNew[i].m[t1] - fBd3) * d1;
			pVert->tv	= (aNew[i].m[t2] - fBd4) * d2;
		}

		//	Build triangle list
		WORD* aIndices = &pMarkSplit->pIndices[pMarkSplit->nNumIndices];
		
		iNumOld = 0;
		for (i=2; i < iNumNew; i++, aIndices+=3, iNumOld+=3)
		{
			aIndices[0] = pMarkSplit->nNumVert;
			aIndices[1] = pMarkSplit->nNumVert + i - 1;
			aIndices[2] = pMarkSplit->nNumVert + i;
		}

		pMarkSplit->nNumVert += iNumNew;
		pMarkSplit->nNumIndices += iNumOld;
	}
	return true;
}