/*
 * FILE: AVariant.cpp
 *
 * DESCRIPTION: ����AVariant���͵ķ���ʵ��
 *
 * CREATED BY: ����ӯ, 2002/8/7
 *
 * MODIFY BY: ����
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

// #include "stdafx.h"
#include "AVariant.h"


AVariant::AVariant()
{
	m_iType = AVT_INVALIDTYPE;		//��ʼ��Ϊ�Ƿ�����
	memset(&m_avData, 0, sizeof(AVARIANT));
}

AVariant::AVariant(const AVariant & v)
{
	/* ��Ϊ�ַ�������ʱ��Ҳ���뿽���ַ�����Modified by ���� Jun 22, 2004*/
	m_string = v.m_string;
	memcpy(&m_avData, &v.m_avData, sizeof(v.m_avData));
	m_iType = v.m_iType;
}

AVariant::AVariant(bool bV)
{
	m_iType = AVT_BOOL;
	m_avData.bV = bV;
}

AVariant::AVariant(char cV)
{
	m_iType = AVT_CHAR;
	m_avData.cV = cV;
}

AVariant::AVariant(unsigned char ucV)
{
	m_iType = AVT_UCHAR;
	m_avData.ucV = ucV;
}

AVariant::AVariant(short sV)
{
	m_iType = AVT_SHORT;
	m_avData.sV = sV;
}

AVariant::AVariant(unsigned short usV)
{
	m_iType = AVT_USHORT;
	m_avData.usV = usV;
}

AVariant::AVariant(int iV)
{
	m_iType = AVT_INT;
	m_avData.iV = iV;
}

AVariant::AVariant(unsigned int uiV)
{
	m_iType = AVT_UINT;
	m_avData.uiV = uiV;
}

AVariant::AVariant(long lV)
{
	m_iType = AVT_LONG;
	m_avData.lV = lV;
}

AVariant::AVariant(unsigned long ulV)
{
	m_iType = AVT_ULONG;
	m_avData.ulV = ulV;
}

AVariant::AVariant(__int64 i64V)
{
	m_iType = AVT_INT64;
	m_avData.i64V = i64V;
}

AVariant::AVariant(unsigned __int64 ui64V)
{
	m_iType = AVT_UINT64;
	m_avData.ui64V = ui64V;
}

AVariant::AVariant(float fV)
{
	m_iType = AVT_FLOAT;
	m_avData.fV = fV;
}

AVariant::AVariant(double dV)
{
	m_iType = AVT_DOUBLE;
	m_avData.dV = dV;
}

AVariant::AVariant(AObject & object)
{
	m_iType = AVT_POBJECT;
	m_avData.paV = & object;
}

AVariant::AVariant(AScriptCodeData & object)
{
	m_iType = AVT_PSCRIPT;
	m_avData.pascdV = & object;
}


AVariant::AVariant(A3DVECTOR3 & avec3)
{
	m_iType = AVT_A3DVECTOR3;
	memcpy(m_avData.a3dvV, &avec3, sizeof(m_avData.a3dvV));
	/*
		��Ϊm_avData.a3dvV�Ķ���Ϊ��char [sizeof(A3DVECTOR3)]�������á�memcpy�����ǡ�=����
	*/
}

AVariant::AVariant(A3DMATRIX4 & amat4)
{
	m_iType = AVT_A3DMATRIX4;
	memcpy(m_avData.a3dmV, &amat4, sizeof(m_avData.a3dmV));
	/*
		��Ϊm_avData.a3dmV�Ķ���Ϊ��char [sizeof(A3DMATRIX4)]�������á�memcpy�����ǡ�=����
	*/
}

AVariant::AVariant(AString & str)
{
	m_iType = AVT_STRING;
	m_string = str;
}

AVariant::AVariant(AClassInfo * paciV)
{
	m_iType = AVT_PACLASSINFO;
	m_avData.paciV = paciV;
}

/*
AVariant::AVariant(ASet * pSet)
{
	m_iType = AVT_PASET;
	m_avData.pasetV = pSet;
}
*/

AVariant::AVariant(A3DFontMan::FONTTYPE font)
{
	m_iType = AVT_FONT;
	memcpy(&m_avData.font, &font, sizeof(A3DFontMan::FONTTYPE));
}

static const char* _format_type		= "Type: %d";
static const char* _format_int		= "Int: %d";
static const char* _format_uint		= "Uint: %u";
static const char* _format_bool		= "Bool: %d";
static const char* _format_float	= "Float: %f";
static const char* _format_string	= "String: ";
static const char* _format_vector	= "X = %f, Y = %f, Z = %f";

void AVariant::Save(AFile* pFile)
{
	char	szLine[AFILE_LINEMAXLEN];

	sprintf(szLine, _format_type, m_iType);
	pFile->WriteLine(szLine);

	switch (m_iType)
	{
	case AVT_INVALIDTYPE:
		break;
	case AVT_BOOL:
		sprintf(szLine, _format_bool, m_avData.bV);
		pFile->WriteLine(szLine);
		break;
	case AVT_INT:
	case AVT_LONG:
		sprintf(szLine, _format_int, m_avData.iV);
		pFile->WriteLine(szLine);
		break;
	case AVT_UINT:
	case AVT_ULONG:
		sprintf(szLine, _format_uint, m_avData.uiV);
		pFile->WriteLine(szLine);
		break;
	case AVT_FLOAT:
	case AVT_DOUBLE:
		sprintf(szLine, _format_float, m_avData.fV);
		pFile->WriteLine(szLine);
		break;
	case AVT_STRING:
		strcpy(szLine, _format_string);
		strcat(szLine, m_string);
		pFile->WriteLine(szLine);
		break;
	case AVT_A3DVECTOR3: {
		A3DVECTOR3 vec = *this;
		sprintf(szLine, _format_vector, vec.x, vec.y, vec.z);
		pFile->WriteLine(szLine);
		break;
	}
	default:
		ASSERT(FALSE);
	}
}

void AVariant::Load(AFile* pFile)
{
	char szLine[AFILE_LINEMAXLEN];
	char szBuf[AFILE_LINEMAXLEN];
	int nBool;
	DWORD dwRead;

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_type, &m_iType);

	switch (m_iType)
	{
	case AVT_INVALIDTYPE:
		break;
	case AVT_BOOL:
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_bool, &nBool);
		m_avData.bV = (nBool != 0);
		break;
	case AVT_INT:
	case AVT_LONG:
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_int, &m_avData.iV);
		break;
	case AVT_UINT:
	case AVT_ULONG:
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_uint, &m_avData.uiV);
		break;
	case AVT_FLOAT:
	case AVT_DOUBLE:
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_float, &m_avData.fV);
		break;
	case AVT_STRING:
		szBuf[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		if (strncmp(szLine, _format_string, strlen(_format_string)) == 0)
			strcpy(szBuf, szLine + strlen(_format_string));
		m_string = szBuf;
		break;
	case AVT_A3DVECTOR3: {
		A3DVECTOR3 vec;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_vector, &vec.x, &vec.y, &vec.z);
		*this = vec;
		break;
	}
	default:
		ASSERT(FALSE);
	}
}