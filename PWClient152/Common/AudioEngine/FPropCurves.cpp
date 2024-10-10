#include "FPropCurves.h"
#include "xml/xmlcommon.h"
#include <AFileImage.h>
#include <algorithm>

static const unsigned int version = 0x100001;

using namespace AudioEngine;

PropCurves::PropCurves(void)
{
	m_fMinY = 0.0f;
	m_fMaxY = 1.0f;
	m_fDefaultY = 0.0f;
}

PropCurves::~PropCurves(void)
{
	release();
}

void PropCurves::Init(float fDefaultY, float fMinY, float fMaxY)
{
	m_fDefaultY = fDefaultY;
	m_fMinY = fMinY;
	m_fMaxY = fMaxY;
}

void PropCurves::SetDefault(float fDefualt)
{
	m_fDefaultY = fDefualt;
}

float PropCurves::GetDefault() const
{
	return m_fDefaultY;
}

float PropCurves::GetMin() const
{
	return m_fMinY;
}

float PropCurves::GetMax() const
{
	return m_fMaxY;
}

int PropCurves::GetCurveNum() const
{
	return (int)m_arrCurve.size();
}

int PropCurves::GetPointNum() const
{
	return (int)m_arrPoint.size();
}

CVPOINT PropCurves::GetPointByIndex(int idx) const
{
	CVPOINT pt = m_arrPoint[idx];
	pt.y = pt.y * (m_fMaxY - m_fMinY) + m_fMinY;
	return pt;
}

void PropCurves::SetCurve(int idx, CURVE_TYPE type)
{
	m_arrCurve[idx] = type;
}

CURVE_TYPE PropCurves::GetCurveByIndex(int idx) const
{
	return m_arrCurve[idx];
}

void PropCurves::release()
{
	m_arrCurve.clear();
	m_arrPoint.clear();
}

bool PropCurves::AddPoint(CVPOINT pt)
{
	if(pt.y < m_fMinY || pt.y > m_fMaxY)
		return false;

	CVPOINT ptNL;
	ptNL.x = pt.x;
	ptNL.y = (pt.y-m_fMinY)/(m_fMaxY-m_fMinY);

	if(m_arrPoint.size() == 0)
	{
		CVPOINT pt0;
		pt0.x = 0.0f;
		pt0.y = (m_fDefaultY-m_fMinY)/(m_fMaxY-m_fMinY);
		m_arrPoint.push_back(pt0);
		
		m_arrPoint.push_back(ptNL);

		m_arrCurve.push_back(CURVE_SIN);
	}
	else
	{
		if(ptNL.x > m_arrPoint.back().x)
		{
			m_arrPoint.push_back(ptNL);
			m_arrCurve.push_back(CURVE_SIN);
		}
		else
		{
			for (size_t i=0; i<m_arrPoint.size()-1; ++i)
			{
				if(ptNL.x<=m_arrPoint[i+1].x && ptNL.x>=m_arrPoint[i].x)
				{
					m_arrPoint.insert(m_arrPoint.begin()+i+1, ptNL);
					m_arrCurve.insert(m_arrCurve.begin()+i, CURVE_SIN);
					break;
				}
			}
		}
	}
	return true;
}

float PropCurves::GetValue(float x) const
{
	if(x < 0)
		x = 0;
	if(x > 1)
		x = 1;
	if(m_arrPoint.size() == 0)
	{
		return m_fDefaultY;
	}
	else if(m_arrPoint.size() == 1)
	{
		float val = m_arrPoint[0].y;
		val = val * (m_fMaxY - m_fMinY) + m_fMinY;
		return val;
	}
	else
	{
		if(x > m_arrPoint.back().x)
		{
			float val = m_arrPoint.back().y;
			val = val * (m_fMaxY - m_fMinY) + m_fMinY;
			return val;
		}
		else
		{
			for (size_t i=0; i<m_arrPoint.size()-1; ++i)
			{
				if(x<m_arrPoint[i].x || x>m_arrPoint[i+1].x)
					continue;
				Curve curve;
				curve.SetType(m_arrCurve[i]);
				float val = curve.GetValue(m_arrPoint[i].x, m_arrPoint[i].y, m_arrPoint[i+1].x, m_arrPoint[i+1].y, x);
				val = val * (m_fMaxY - m_fMinY) + m_fMinY;
				return val;
			}
		}
	}
	return m_fDefaultY;
}

bool PropCurves::DeletePoint(int idx)
{
	if(idx <= 0 || idx >= (int)m_arrPoint.size())
		return false;
	m_arrPoint.erase(m_arrPoint.begin()+idx);
	m_arrCurve.erase(m_arrCurve.begin()+idx-1);
	return true;
}

void PropCurves::SetPoint(int idx, CVPOINT pt)
{
	if(idx < 0 || idx >= (int)m_arrPoint.size())
		return;
	pt.y = (pt.y-m_fMinY)/(m_fMaxY-m_fMinY);
	if(idx == 0)
		pt.x = 0;
	m_arrPoint[idx] = pt;
}

void PropCurves::MoveWholeCurve(float delta)
{
	if(m_arrPoint.size() == 0)
	{
		m_fDefaultY += delta;
		if(m_fDefaultY < m_fMinY)
			m_fDefaultY = m_fMinY;
		if(m_fDefaultY > m_fMaxY)
			m_fDefaultY = m_fMaxY;

		return;
	}
	delta = delta/(m_fMaxY-m_fMinY);
	float fmin = 1.0f;
	float fmax = 0.0f;
	for (size_t i=0; i<m_arrPoint.size(); ++i)
	{
		float y = m_arrPoint[i].y;
		if(y < fmin)
			fmin = y;
		if(y < fmax)
			fmax = y;
	}
	if(delta>0)
	{
		if(delta+fmax>1.0f)
			delta = 1.0f - fmax;
	}
	else
	{
		if(delta+fmin<0.0f)
			delta = -fmin;
	}

	for (size_t i=0; i<m_arrPoint.size(); ++i)
	{
		m_arrPoint[i].y += delta;
	}
}

void PropCurves::MoveCurve(int idx, float delta)
{	
	if(m_arrPoint.size() == 0)
	{
		m_fDefaultY += delta;
		if(m_fDefaultY < m_fMinY)
			m_fDefaultY = m_fMinY;
		if(m_fDefaultY > m_fMaxY)
			m_fDefaultY = m_fMaxY;

		return;
	}
	if(idx > (int)m_arrCurve.size())
		return;	
	delta = delta/(m_fMaxY-m_fMinY);
	if(idx == m_arrCurve.size())
	{
		m_arrPoint.back().y += delta;
		if(m_arrPoint.back().y<0)
			m_arrPoint.back().y = 0;
		if(m_arrPoint.back().y>1)
			m_arrPoint.back().y = 1;
	}
	else
	{
		float y0 = m_arrPoint[idx].y;
		float y1 = m_arrPoint[idx+1].y;
		if(y0 > y1)
		{
			float t = y0;
			y0 = y1;
			y1 = t;
		}
		if(delta>0)
		{
			if(delta+y1>1.0f)
				delta = 1.0f - y1;
		}
		else
		{
			if(delta+y0<0.0f)
				delta = -y0;
		}
		m_arrPoint[idx].y += delta;
		m_arrPoint[idx+1].y += delta;
	}
}

bool PropCurves::Load(AFileImage* pFile)
{
	release();

	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);
	pFile->Read(&m_fDefaultY, sizeof(m_fDefaultY), &dwReadLen);
	pFile->Read(&m_fMinY, sizeof(m_fMinY), &dwReadLen);
	pFile->Read(&m_fMaxY, sizeof(m_fMaxY), &dwReadLen);
	Init(m_fDefaultY, m_fMinY, m_fMaxY);
	int iPointNum = 0;
	pFile->Read(&iPointNum, sizeof(iPointNum), &dwReadLen);
	for (int i=0; i<iPointNum; ++i)
	{
		CVPOINT pt;
		pFile->Read(&pt.x, sizeof(pt.x), &dwReadLen);
		pFile->Read(&pt.y, sizeof(pt.y), &dwReadLen);
		m_arrPoint.push_back(pt);
	}
	int iCurveNum = 0;
	for (int i=0; i<iCurveNum; ++i)
	{
		CURVE_TYPE type = CURVE_SIN;
		pFile->Read(&type, sizeof(type), &dwReadLen);
		m_arrCurve.push_back(type);
	}

	return true;
}

bool PropCurves::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;

	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);
	pFile->Write(&m_fDefaultY, sizeof(m_fDefaultY), &dwWriteLen);
	pFile->Write(&m_fMinY, sizeof(m_fMinY), &dwWriteLen);
	pFile->Write(&m_fMaxY, sizeof(m_fMaxY), &dwWriteLen);
	int iPointNum = GetPointNum();
	pFile->Write(&iPointNum, sizeof(iPointNum), &dwWriteLen);
	for (size_t i=0; i<m_arrPoint.size(); ++i)
	{
		float x = m_arrPoint[i].x;
		float y = m_arrPoint[i].y;
		pFile->Write(&x, sizeof(x), &dwWriteLen);
		pFile->Write(&y, sizeof(y), &dwWriteLen);
	}
	int iCurveNum = GetCurveNum();
	for (size_t i=0; i<m_arrCurve.size(); ++i)
	{
		CURVE_TYPE type = m_arrCurve[i];
		pFile->Write(&type, sizeof(type), &dwWriteLen);
	}
	return true;
}

bool PropCurves::LoadXML(TiXmlElement* root)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "Default", m_fDefaultY);
	QueryElement(root, "Min", m_fMinY);
	QueryElement(root, "Max", m_fMaxY);
	Init(m_fDefaultY, m_fMinY, m_fMaxY);
	int iPointNum = 0;
	QueryElement(root, "PointNum", iPointNum);
	TiXmlNode* pPointNode = root->FirstChild("Point");
	for (int i=0; i<iPointNum; ++i)
	{
		CVPOINT pt;
		QueryElement(pPointNode, "x", pt.x);
		QueryElement(pPointNode, "y", pt.y);
		m_arrPoint.push_back(pt);
		pPointNode = pPointNode->NextSibling("Point");
	}

	int iCurveNum = 0;
	QueryElement(root, "CurveNum", iCurveNum);
	TiXmlNode* pCurveTypeNode = root->FirstChild("CurveType");
	for (int i=0; i<iCurveNum; ++i)
	{
		int iCurveType = 0;
		pCurveTypeNode->ToElement()->QueryIntAttribute("value", &iCurveType);
		m_arrCurve.push_back((CURVE_TYPE)iCurveType);
		pCurveTypeNode = pCurveTypeNode->NextSibling("CurveType");
	}
	return true;
}

bool PropCurves::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("PropCurves");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "Default", m_fDefaultY);
	AddElement(root, "Min", m_fMinY);
	AddElement(root, "Max", m_fMaxY);
	AddElement(root, "PointNum", (int)GetPointNum());
	for (size_t i=0; i<m_arrPoint.size(); ++i)
	{
		TiXmlElement* ele = new TiXmlElement("Point");
		root->LinkEndChild(ele);
		AddElement(ele, "x", m_arrPoint[i].x);
		AddElement(ele, "y", m_arrPoint[i].y);
	}

	AddElement(root, "CurveNum", (int)GetCurveNum());
	for (size_t i=0; i<m_arrCurve.size(); ++i)
	{
		AddElement(root, "CurveType", (int)m_arrCurve[i]);
	}
	return true;
}