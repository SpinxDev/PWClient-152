
#include "sevbezier.h"
#include "a3dmatrix.h"

#define DEG2RAD(deg) ((deg) * 3.1415926535f / 180.0f)
#define RAD2DEG(rad) ((rad) * 180.0f / 3.1415926535f)

//	Returns vector with same direction and unit length
static A3DVECTOR3 _Normalize(const A3DVECTOR3& v)
{
	float mag = v.Magnitude();
	if (mag < 1e-12	&& mag > -1e-12)
		return A3DVECTOR3(0.0f);
	else
		return v / mag;
}

static A3DMATRIX4 _RotateAxis(const A3DVECTOR3& vRotAxis, float vRad)
{
	/*
		The derivation of this algorithm can be seen in rotation.pdf in my ÎÄµµ directory.
		The basic idea is to divide the original vector to two part: paralell to axis and 
		perpendicular to axis, then only perpendicular part can be affected by this rotation.
		Now divide the rotation part onto the main axis on the rotation plane then it will
		be clear to see what composes the rotated vector. Finally we can get the transform
		matrix from a set of 3 equations.
	*/
	A3DVECTOR3 vecAxis = _Normalize(vRotAxis);

	A3DMATRIX4 ret;
	float xx, xy, xz, yy, yz, zz, cosine, sine, one_cs, xsine, ysine, zsine;

	xx = vecAxis.x * vecAxis.x;
	xy = vecAxis.x * vecAxis.y;
	xz = vecAxis.x * vecAxis.z;
	yy = vecAxis.y * vecAxis.y;
	yz = vecAxis.y * vecAxis.z;
	zz = vecAxis.z * vecAxis.z;

	cosine = (float)cos(vRad);
	sine = (float)sin(vRad);
	one_cs = 1.0f - cosine;

	xsine = vecAxis.x * sine;
	ysine = vecAxis.y * sine;
	zsine = vecAxis.z * sine;
	
	ret._11 = xx + cosine * (1.0f - xx);
	ret._12 = xy * one_cs + zsine;
	ret._13 = xz * one_cs - ysine;
	ret._14 = 0.0f;

	ret._21 = xy * one_cs - zsine;
	ret._22 = yy + cosine * (1.0f - yy);
	ret._23 = yz * one_cs + xsine;
	ret._24 = 0.0f;

	ret._31 = xz * one_cs + ysine;
	ret._32 = yz * one_cs - xsine;
	ret._33 = zz + cosine * (1.0f - zz);
	ret._34 = 0.0f;

	ret._41 = 0.0f;
	ret._42 = 0.0f;
	ret._43 = 0.0f;
	ret._44 = 1.0f;
	
	//D3DXMatrixRotationAxis((D3DXMATRIX *)&ret, (D3DXVECTOR3*)&vecAxis, vRad);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////
// class CSevBezierPoint
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
// class CSevBezierSeg
///////////////////////////////////////////////////////////////////////////////////////

CSevBezierSeg::CSevBezierSeg()
{
}

CSevBezierSeg::~CSevBezierSeg()
{
}

void CSevBezierSeg::Init( CSevBezierPoint *pListPt)
{
	m_pListPoint = pListPt;
	assert(m_pListPoint);
}

A3DVECTOR3	CSevBezierSeg::Bezier( float u ,bool bForward)
{
	A3DVECTOR3 pt1,pt2,control1,control2;
	
	if(bForward)
	{
		pt1			= m_pListPoint[m_nHeadPoint].GetPos();
		control1	= m_vAnchorHead; 
		control2	= m_vAnchorTail; 
		pt2			= m_pListPoint[m_nTailPoint].GetPos();
	}else
	{
		pt1			= m_pListPoint[m_nTailPoint].GetPos();
		control1	= m_vAnchorTail; 
		control2	= m_vAnchorHead; 
		pt2			= m_pListPoint[m_nHeadPoint].GetPos();
	}


	float a[3], b[3], c[3];
	int i;
	for(i = 0; i < 3;i ++)
	{
		c[i] = 3 * (control1.m[i] - pt1.m[i]);
		b[i] = 3 * (control2.m[i] - control1.m[i]) - c[i];
		a[i] = pt2.m[i] - pt1.m[i] - c[i] - b[i];
	}

	A3DVECTOR3 pos;
	for(i = 0; i < 3; i ++)
	{
		pos.m[i] = (a[i] * u * u * u) + (b[i] * u * u) + (c[i] * u) + pt1.m[i];
	}

	return pos;
}

A3DVECTOR3	CSevBezierSeg::Vector( float u ,bool bForward)
{
	A3DVECTOR3 headspot,tailspot;
	
	if(bForward)
	{
		headspot = m_pListPoint[m_nHeadPoint].GetDir();
		tailspot = m_pListPoint[m_nTailPoint].GetDir();
	}else
	{
		headspot = m_pListPoint[m_nTailPoint].GetDir();
		tailspot = m_pListPoint[m_nHeadPoint].GetDir();
	}
	//return headspot + (tailspot - headspot)*u;
	
	if(u <= 0.0f) return headspot;
	if(u >= 1.0f) return tailspot;

	float m1 = headspot.Magnitude();
	float m2 = tailspot.Magnitude();
	float dot = DotProduct(_Normalize(headspot), _Normalize(tailspot));

	if(dot > 1.0f)
	{
		return headspot;
	}

	if(dot < -1.0f) dot = -1.0f;

	float r = (float)acos(dot);
	r = r * u;

	A3DVECTOR3 dir = CrossProduct(headspot, tailspot);
	A3DMATRIX4 matrix = _RotateAxis(dir, r);
	headspot = matrix * headspot;
	if(bForward)
		return _Normalize(headspot) * (m1 + (m2 - m1) * u);
	else return -_Normalize(headspot) * (m1 + (m2 - m1) * u);
	
}

///////////////////////////////////////////////////////////////////////////////////////
// class CSevBezier
///////////////////////////////////////////////////////////////////////////////////////

CSevBezier::CSevBezier()
{
	m_pListPoint	= NULL;
	m_pListSeg		= NULL;
	m_nNumPoint		= 0;
	m_nNumSeg		= 0;
	m_iGlobalID		= -1;
	m_iNextGlobalID	= -1;
	m_nObjectID		= 0;
};

CSevBezier::~CSevBezier()
{
};

void CSevBezier::Release()
{
	if(m_pListSeg){ delete []m_pListSeg; m_pListSeg = NULL; m_nNumSeg = 0; }
	if(m_pListPoint){ delete []m_pListPoint; m_pListPoint = NULL; m_pListPoint = 0;}
}

/* Bezier export data struct
 * DWORD      verion	
 * int		  object	id
 * -----------------------------------
 * int		  point		num
 * A3DVECTOR3 point		pos
 * A3DVECTOR3 point		dir
 *------------------------------------
 * int        segment	num
 * A3DVECTOR3 segment	AnchorHead
 * A3DVECTOR3 segment   AnchorTail
 * int        segment   HeadPointIndex
 * int        segment   TailPointIndex
 * float      segment   Length
 */
bool CSevBezier::Load(FILE* pFile)
{
	unsigned long dwVersion;
	if(0==fread(&dwVersion,sizeof(unsigned long),1,pFile)) return false;
	if(0==fread(&m_nObjectID,sizeof(int),1,pFile)) return false;
	
	if (dwVersion >= 2)
	{
		if(0==fread(&m_iGlobalID,sizeof(int),1,pFile)) return false;
		if(0==fread(&m_iNextGlobalID,sizeof(int),1,pFile)) return false;
	}

	int ptNum,segNum,i;
	A3DVECTOR3 vAnchorHead,vAnchorTail;
	int headIndex,TailIndex;
	float lenght;
	
	//Read bezier point
	A3DVECTOR3 vPos,vDir;
	if(0==fread(&ptNum,sizeof(int),1,pFile))
		return false;

	m_pListPoint = new CSevBezierPoint[ptNum];
	if(m_pListPoint==NULL) return false;
	m_nNumPoint = ptNum;
	for(i = 0; i < ptNum; i++)
	{
		if(0==fread(&vPos,sizeof(A3DVECTOR3),1,pFile)) goto FAIL;
		if(0==fread(&vDir,sizeof(A3DVECTOR3),1,pFile)) goto FAIL;
		m_pListPoint[i].SetPos(vPos);
		m_pListPoint[i].SetDir(vDir);
	}

	//Read bezier segment
	if(0==fread(&segNum,sizeof(int),1,pFile))
		goto FAIL;

	m_pListSeg = new CSevBezierSeg[segNum];
	if(m_pListPoint==NULL) goto FAIL;
	m_nNumSeg = segNum;
	for(i = 0; i < segNum; i++)
	{
		if(0==fread(&vAnchorHead,sizeof(A3DVECTOR3),1,pFile)) goto FAIL;
		if(0==fread(&vAnchorTail,sizeof(A3DVECTOR3),1,pFile)) goto FAIL;

		if(0==fread(&headIndex,sizeof(int),1,pFile)) goto FAIL;
		if(0==fread(&TailIndex,sizeof(int),1,pFile)) goto FAIL;
		
		if(0==fread(&lenght,sizeof(float),1,pFile)) goto FAIL;

		m_pListSeg[i].Init(m_pListPoint);
		m_pListSeg[i].SetAnchorHead(vAnchorHead);
		m_pListSeg[i].SetAnchorTail(vAnchorTail);
		m_pListSeg[i].SetHeadPoint(headIndex);
		m_pListSeg[i].SetTailPoint(TailIndex);
		m_pListSeg[i].SetSegLenght(lenght);
	}

	return true;

FAIL:

	if(m_pListPoint) delete []m_pListPoint;
	if(m_pListSeg) delete []m_pListSeg;
	m_nNumSeg = 0;
	m_nNumPoint = 0;
	m_pListPoint = 0;
	m_pListSeg = 0;
	return false;	
}

void CSevBezier::Assign(int nNumPt,int nNumSeg)
{
	Release();
	m_nNumPoint = nNumPt;
	m_nNumSeg = nNumSeg;

	m_pListPoint = new CSevBezierPoint[m_nNumPoint];
	m_pListSeg = new CSevBezierSeg[m_nNumSeg];
	
}

void CSevBezier::AddBezierPoint( CSevBezierPoint *pt , int i)
{
	if(pt==NULL) return;
	
	if(i>=0 && i< m_nNumPoint)
	{
		m_pListPoint[i].SetPos(pt->GetPos());
		m_pListPoint[i].SetDir(pt->GetDir());
	}
}

void CSevBezier::AddBezierSeg( CSevBezierSeg *seg , int i)
{
	if(seg==NULL) return;
	
	if(i>=0 && i< m_nNumSeg)
	{
		m_pListSeg[i].Init(m_pListPoint);
		m_pListSeg[i].SetAnchorHead(seg->GetAnchorHead());
		m_pListSeg[i].SetAnchorTail(seg->GetAnchorTail());
		m_pListSeg[i].SetHeadPoint(seg->GetHeadPoint());
		m_pListSeg[i].SetTailPoint(seg->GetTailPoint());
		m_pListSeg[i].SetSegLenght(seg->GetSegLength());
	}
}

void CSevBezier::SetOffset(A3DVECTOR3 vOffset)
{
	int i;
	for(i = 0; i < m_nNumPoint; i++ )
	{
		A3DVECTOR3 pos = m_pListPoint[i].GetPos();
		pos += vOffset;
		m_pListPoint[i].SetPos(pos);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class CSevBezierWalker
//	
///////////////////////////////////////////////////////////////////////////

CSevBezierWalker::CSevBezierWalker()
{
	m_pBezier		= NULL;
	m_fSpeed		= 1.0f;
	m_iTotalTime	= 0;
	m_iTimeCnt		= 0;
	m_bForward		= true;
	m_iCurSeg		= 0;
	m_iCurSegTime	= 0;
	m_iPassSegTime	= 0;
	m_bLoop			= true;
	m_bWalking		= false;
	m_bPause		= false;
	m_bForwardStop	= false;
}

CSevBezierWalker::~CSevBezierWalker()
{
}

//	Bind bezier route
bool CSevBezierWalker::BindBezier(CSevBezier* pBezier)
{
	m_pBezier = pBezier;

	//	Stop at default position
	m_iTimeCnt		= 0;
	m_iCurSeg		= 0;
	m_iPassSegTime	= 0;
	m_bForwardStop	= false;
	m_bWalking		= false;
	m_bPause		= false;

	return true;
}

//	Start walk
bool CSevBezierWalker::StartWalk(bool bLoop, bool bForward)
{
	if (!m_pBezier)
		return false;

	m_bLoop			= bLoop;
	m_bForward		= bForward;
	m_iTimeCnt		= 0;
	m_iCurSeg		= bForward ? 0 : m_pBezier->GetSegmentNum()-1;
	m_iCurSegTime	= (int)(m_pBezier->GetSegment(m_iCurSeg)->GetSegLength() * 1000.0f / m_fSpeed);
	m_iPassSegTime	= 0;
	m_bWalking		= true;
	m_bPause		= false;

	return true;
}

//	Pause walk
void CSevBezierWalker::Pause(bool bPause)
{
	if (bPause)
	{
		if (m_bWalking)
		{
			m_bPause	= true;
			m_bWalking	= false;
		}
	}
	else
	{
		if (m_bPause)
		{
			m_bPause	= false;
			m_bWalking	= true;
		}
	}
}

//	Move speed
void CSevBezierWalker::SetSpeed(float fSpeed)
{
	assert(fSpeed > 0.0f);
	m_fSpeed = fSpeed;

	//	Calcualte total time
	if (m_pBezier)
	{
		int i, iNumSeg = m_pBezier->GetSegmentNum();
		float fInvSpeed = 1000.0f / m_fSpeed;

		m_iTotalTime = 0;
		for (i=0; i < iNumSeg; i++)
			m_iTotalTime += (int)(m_pBezier->GetSegment(i)->GetSegLength() * fInvSpeed);
	}
}

void CSevBezierWalker::SetForwardFlag(bool bForward)
{
	if (!m_bWalking && !m_bPause)
	{
		m_bForward = bForward;
		return;
	}

	int iDeltaTime	= m_iPassSegTime + m_iCurSegTime - m_iTimeCnt;
	m_iPassSegTime	= m_iTotalTime - (m_iPassSegTime + m_iCurSegTime);
	m_iTimeCnt		= m_iPassSegTime + iDeltaTime;

	m_bForward = bForward;
}

//	Tick routine
bool CSevBezierWalker::Tick(int iDeltaTime)
{
	if (!m_pBezier || !m_bWalking)
		return true;

	int iNumSeg = m_pBezier->GetSegmentNum();
	float fInvSpeed = 1000.0f / m_fSpeed;

	iDeltaTime %= m_iTotalTime;
	m_iTimeCnt += iDeltaTime;

	if (m_bForward)
	{
		while (m_iTimeCnt >= m_iPassSegTime + m_iCurSegTime)
		{
			if (m_iCurSeg + 1 >= iNumSeg)
			{
				if (m_bLoop)
				{
					m_iCurSeg		= 0;
					m_iPassSegTime	= 0;
					m_iTimeCnt	   -= m_iTotalTime;

					if (m_iTimeCnt < 0)
						m_iTimeCnt = 0;
				}
				else
				{
					m_iTimeCnt = m_iPassSegTime + m_iCurSegTime;
					m_bWalking = false;
					m_bForwardStop = true;
					break;
				}
			}
			else
			{
				m_iCurSeg++;
				m_iPassSegTime += m_iCurSegTime;
			}

			CSevBezierSeg* pSeg = m_pBezier->GetSegment(m_iCurSeg);
			m_iCurSegTime = (int)(pSeg->GetSegLength() * fInvSpeed);
		}
	}
	else
	{
		while (m_iTimeCnt >= m_iPassSegTime + m_iCurSegTime)
		{
			if (m_iCurSeg - 1 < 0)
			{
				if (m_bLoop)
				{
					m_iCurSeg		= iNumSeg - 1;
					m_iPassSegTime	= 0;
					m_iTimeCnt	   -= m_iTotalTime;

					if (m_iTimeCnt < 0)
						m_iTimeCnt = 0;
				}
				else
				{
					m_iTimeCnt = m_iPassSegTime + m_iCurSegTime;
					m_bWalking = false;
					m_bForwardStop = false;
					break;
				}
			}
			else
			{
				m_iCurSeg--;
				m_iPassSegTime += m_iCurSegTime;
			}

			CSevBezierSeg* pSeg = m_pBezier->GetSegment(m_iCurSeg);
			m_iCurSegTime = (int)(pSeg->GetSegLength() * fInvSpeed);
		}
	}
	
	return true;
}

//	Get current position
A3DVECTOR3 CSevBezierWalker::GetPos()
{
	if (!m_pBezier)
	{
		assert(0);
		return A3DVECTOR3(0.0f);
	}

	int iSeg;
	float f;
	bool bForward;

	if (m_bWalking || m_bPause)
	{
		f = (float)(m_iTimeCnt - m_iPassSegTime) / m_iCurSegTime;
		bForward = m_bForward;
		iSeg = m_iCurSeg;
	}
	else
	{
		bForward = true;

		if (m_bForwardStop)
		{
			f = 1.0f;
			iSeg = m_pBezier->GetSegmentNum() - 1;
		}
		else
		{
			f = 0.0f;
			iSeg = 0;
		}
	}

	return m_pBezier->GetSegment(iSeg)->Bezier(f, bForward);
}

//	Get current direction
A3DVECTOR3 CSevBezierWalker::GetDir()
{
	if (!m_pBezier)
	{
		assert(0);
		return A3DVECTOR3(0.0f, 0.0f, 1.0f);
	}

	int iSeg;
	float f;
	bool bForward;

	if (m_bWalking || m_bPause)
	{
		f = (float)(m_iTimeCnt - m_iPassSegTime) / m_iCurSegTime;
		bForward = m_bForward;
		iSeg = m_iCurSeg;
	}
	else
	{
		bForward = true;

		if (m_bForwardStop)
		{
			f = 1.0f;
			iSeg = m_pBezier->GetSegmentNum() - 1;
		}
		else
		{
			f = 0.0f;
			iSeg = 0;
		}
	}

	return m_pBezier->GetSegment(iSeg)->Vector(f, bForward);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CSevBezierMan
//	
///////////////////////////////////////////////////////////////////////////

CSevBezierMan::CSevBezierMan() : m_BezierTab(128)
{
}

CSevBezierMan::~CSevBezierMan()
{
	BezierTable::iterator it = m_BezierTab.begin();
	for (; it != m_BezierTab.end(); ++it)
	{
		CSevBezier* pBezier = *it.value();
		pBezier->Release();
		delete pBezier;
	}
}

//	Load bezier data from file
bool CSevBezierMan::Load(const char* szFile)
{
	FILE* fp = fopen(szFile, "rb");
	if (!fp)
		return false;

	//	Read file header
	SEVBEZIERFILEHEADER Header;
	fread(&Header, 1, sizeof (Header), fp);

	if (Header.iVersion > SEVBEZIERFILE_VERSION)
		return false;

	int i;
	for (i=0; i < Header.iNumBezier; i++)
	{
		CSevBezier* pBezier = new CSevBezier;
		if (!pBezier->Load(fp))
			return false;

		m_BezierTab.put(pBezier->GetObjectID(), pBezier);
	}

	fclose(fp);
	return true;
}

//	Get bezier route by it's object ID
CSevBezier* CSevBezierMan::GetBezier(int iObjectID)
{
	BezierTable::pair_type Pair = m_BezierTab.get(iObjectID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this bezier route

	return *Pair.first;
}

