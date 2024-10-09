#include "EC_Bezier.h"
#include "a3d.h"

///////////////////////////////////////////////////////////////////////////////////////
// class CECBezierPoint
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
// class CECBezierSeg
///////////////////////////////////////////////////////////////////////////////////////

CECBezierSeg::CECBezierSeg()
{
};

CECBezierSeg::~CECBezierSeg()
{
};

void CECBezierSeg::Init( CECBezierPoint *pListPt)
{
	m_pListPoint = pListPt;
	ASSERT(m_pListPoint);
}

A3DVECTOR3	CECBezierSeg::Bezier( float u ,bool bForward)
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

A3DVECTOR3	CECBezierSeg::Vector( float u ,bool bForward)
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

	float m1 = Magnitude(headspot),
		  m2 = Magnitude(tailspot);

	float dot = DotProduct(Normalize(headspot), Normalize(tailspot));

	if(dot > 1.0f)
	{
		return headspot;
	}

	if(dot < -1.0f) dot = -1.0f;

	float r = (float)acos(dot);
	r = r * u;

	A3DVECTOR3 dir = CrossProduct(headspot, tailspot);
	A3DMATRIX4 matrix = RotateAxis(dir, r);
	headspot = matrix * headspot;
	if(bForward)
		return Normalize(headspot) * (m1 + (m2 - m1) * u);
	else return -Normalize(headspot) * (m1 + (m2 - m1) * u);
	
}

///////////////////////////////////////////////////////////////////////////////////////
// class CECBezier
///////////////////////////////////////////////////////////////////////////////////////

CECBezier::CECBezier() : CECSceneObject(TYPE_BEZIER)
{
	m_pListPoint	= NULL;
	m_pListSeg		= NULL;
	m_nNumPoint		= 0;
	m_nNumSeg		= 0;
	m_iGlobalID		= -1;
	m_iNextGlobalID	= -1;
	m_nObjectID		= 0;
};

CECBezier::~CECBezier()
{
};

void CECBezier::Release()
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
bool CECBezier::Load(AFile* pFile)
{
	DWORD dwVersion;
	DWORD dwRr;
	if(!pFile->Read(&dwVersion,sizeof(DWORD),&dwRr)) return false;
	if(!pFile->Read(&m_nObjectID,sizeof(int),&dwRr)) return false;

	if (dwVersion >= 2)
	{
		if(!pFile->Read(&m_iGlobalID,sizeof(int),&dwRr)) return false;
		if(!pFile->Read(&m_iNextGlobalID,sizeof(int),&dwRr)) return false;
	}

	if (dwVersion >= 3)
	{
		if (!pFile->ReadString(m_strActName))
			return false;
	}

	int ptNum,segNum,i;
	A3DVECTOR3 vAnchorHead,vAnchorTail;
	int headIndex,TailIndex;
	float lenght;
	
	//Read bezier point
	A3DVECTOR3 vPos,vDir;
	if(!pFile->Read(&ptNum,sizeof(int),&dwRr)) return false;
	m_pListPoint = new CECBezierPoint[ptNum];
	if(m_pListPoint==NULL) return false;
	m_nNumPoint = ptNum;
	for(i = 0; i < ptNum; i++)
	{
		if(!pFile->Read(&vPos,sizeof(A3DVECTOR3),&dwRr)) goto FAIL;
		if(!pFile->Read(&vDir,sizeof(A3DVECTOR3),&dwRr)) goto FAIL;
		m_pListPoint[i].SetPos(vPos);
		m_pListPoint[i].SetDir(vDir);
	}

	//Read bezier segment
	if(!pFile->Read(&segNum,sizeof(int),&dwRr)) goto FAIL;
	if( segNum > 0 )
	{
		m_pListSeg = new CECBezierSeg[segNum];
		if(m_pListPoint==NULL) goto FAIL;
		m_nNumSeg = segNum;
		for(i = 0; i < segNum; i++)
		{
			if(!pFile->Read(&vAnchorHead,sizeof(A3DVECTOR3),&dwRr)) goto FAIL;
			if(!pFile->Read(&vAnchorTail,sizeof(A3DVECTOR3),&dwRr)) goto FAIL;

			if(!pFile->Read(&headIndex,sizeof(int),&dwRr)) goto FAIL;
			if(!pFile->Read(&TailIndex,sizeof(int),&dwRr)) goto FAIL;

			if(!pFile->Read(&lenght,sizeof(float),&dwRr)) goto FAIL;

			m_pListSeg[i].Init(m_pListPoint);
			m_pListSeg[i].SetAnchorHead(vAnchorHead);
			m_pListSeg[i].SetAnchorTail(vAnchorTail);
			m_pListSeg[i].SetHeadPoint(headIndex);
			m_pListSeg[i].SetTailPoint(TailIndex);
			m_pListSeg[i].SetSegLenght(lenght);
		}
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

void CECBezier::Assign(int nNumPt,int nNumSeg)
{
	Release();
	m_nNumPoint = nNumPt;
	m_nNumSeg = nNumSeg;

	m_pListPoint = new CECBezierPoint[m_nNumPoint];
	m_pListSeg = new CECBezierSeg[m_nNumSeg];
	
}

void CECBezier::AddBezierPoint( CECBezierPoint *pt , int i)
{
	if(pt==NULL) return;
	
	if(i>=0 && i< m_nNumPoint)
	{
		m_pListPoint[i].SetPos(pt->GetPos());
		m_pListPoint[i].SetDir(pt->GetDir());
	}
}

void CECBezier::AddBezierSeg( CECBezierSeg *seg , int i)
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

void CECBezier::SetOffset(A3DVECTOR3 vOffset)
{
	int i;
	for(i = 0; i < m_nNumPoint; i++ )
	{
			A3DVECTOR3 pos = m_pListPoint[i].GetPos();
			pos +=vOffset;
			m_pListPoint[i].SetPos(pos);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECBezierWalker
//	
///////////////////////////////////////////////////////////////////////////

CECBezierWalker::CECBezierWalker()
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

CECBezierWalker::~CECBezierWalker()
{
}

//	Bind bezier route
bool CECBezierWalker::BindBezier(CECBezier* pBezier)
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
bool CECBezierWalker::StartWalk(bool bLoop, bool bForward)
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
void CECBezierWalker::Pause(bool bPause)
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
void CECBezierWalker::SetSpeed(float fSpeed)
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

void CECBezierWalker::SetForwardFlag(bool bForward)
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
bool CECBezierWalker::Tick(int iDeltaTime)
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

			CECBezierSeg* pSeg = m_pBezier->GetSegment(m_iCurSeg);
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

			CECBezierSeg* pSeg = m_pBezier->GetSegment(m_iCurSeg);
			m_iCurSegTime = (int)(pSeg->GetSegLength() * fInvSpeed);
		}
	}
	
	return true;
}

//	Get current position
A3DVECTOR3 CECBezierWalker::GetPos()
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
A3DVECTOR3 CECBezierWalker::GetDir()
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

