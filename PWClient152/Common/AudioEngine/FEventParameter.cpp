#include "FEventParameter.h"
#include "xml/xmlcommon.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

EventParameter::EventParameter(void)
: m_fValue(0)
, m_pEvent(0)
, m_bSeek(false)
, m_fSeekValue(0)
, m_bForwardSeek(true)
{	
	m_EventParamProperty.fMinValue = 0.0f;
	m_EventParamProperty.fMaxValue = 1.0f;
	m_EventParamProperty.fVelocity = 0.0f;
	m_EventParamProperty.fSeekSpeed = 0.0f;
	m_EventParamProperty.velocityLoopMode = VELOCITY_ONSHOT;	
}

EventParameter::~EventParameter(void)
{
}

void EventParameter::AddListener(EventParameterListener* pListener)
{
	if(!pListener)
		return;
	m_setListener.insert(pListener);
}

void EventParameter::RemoveListener(EventParameterListener* pListener)
{
	ListenerSet::iterator it = m_setListener.find(pListener);
	if(it != m_setListener.end())
		m_setListener.erase(m_setListener.find(pListener));
}

bool EventParameter::listenerValueChanged()
{
	ListenerSet setListener = m_setListener;
	ListenerSet::const_iterator it = setListener.begin();
	for (; it!=setListener.end(); ++it)
	{
		if(!(*it)->OnValueChanged(this))
			continue;
	}
	return true;
}

void EventParameter::SetValue(float value, bool bSeek /*= true*/)
{	
	if(bSeek && m_EventParamProperty.fSeekSpeed != 0)
	{
		m_bSeek = bSeek;
		m_fSeekValue = value;
		if(m_fSeekValue >= m_fValue)
			m_bForwardSeek = true;
		else
			m_bForwardSeek = false;
	}
	else
	{
		m_fValue = value;
		listenerValueChanged();
	}	
}

void EventParameter::FinishSeek()
{
	m_bSeek = false;
}

void EventParameter::Reset()
{
	m_fValue = m_EventParamProperty.fMinValue;
	m_bSeek = false;
	m_fSeekValue = m_fValue;
}

bool EventParameter::Load(AFileImage* pFile)
{
	DWORD dwReadLen = 0;
	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iNameLen = 0;
	pFile->Read(&iNameLen, sizeof(iNameLen), &dwReadLen);
	char szName[1024] = {0};
	pFile->Read(szName, iNameLen, &dwReadLen);
	m_strName = szName;

	pFile->Read(&m_EventParamProperty, sizeof(m_EventParamProperty), &dwReadLen);
	pFile->Read(&m_fValue, sizeof(m_fValue), &dwReadLen);
	return true;
}

bool EventParameter::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	pFile->Write(&m_EventParamProperty, sizeof(m_EventParamProperty), &dwWriteLen);
	pFile->Write(&m_fValue, sizeof(m_fValue), &dwWriteLen);
	return true;
}

bool EventParameter::LoadXML(TiXmlElement* root)
{
	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "name", m_strName);

	TiXmlNode* pNode = root->FirstChild("EventParameterProperty");
	if (!pNode)
		return false;

	TiXmlElement* pParamPropertyNode = pNode->ToElement();
	QueryElement(pParamPropertyNode, "minValue", m_EventParamProperty.fMinValue);
	QueryElement(pParamPropertyNode, "maxValue", m_EventParamProperty.fMaxValue);
	QueryElement(pParamPropertyNode, "velocity", m_EventParamProperty.fVelocity);
	int iVelocityLoopMode = 0;
	QueryElement(pParamPropertyNode, "velocityLoopMode", iVelocityLoopMode);
	m_EventParamProperty.velocityLoopMode = (VELOCIY_LOOP_MODE)iVelocityLoopMode;
	QueryElement(pParamPropertyNode, "seekSpeed", m_EventParamProperty.fSeekSpeed);
	return true;
}

bool EventParameter::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("EventParameter");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "name", m_strName);

	TiXmlElement* pParamPropertyNode = new TiXmlElement("EventParameterProperty");
	root->LinkEndChild(pParamPropertyNode);
	AddElement(pParamPropertyNode, "minValue", m_EventParamProperty.fMinValue);
	AddElement(pParamPropertyNode, "maxValue", m_EventParamProperty.fMaxValue);
	AddElement(pParamPropertyNode, "velocity", m_EventParamProperty.fVelocity);
	AddElement(pParamPropertyNode, "velocityLoopMode", (int)m_EventParamProperty.velocityLoopMode);
	AddElement(pParamPropertyNode, "seekSpeed", m_EventParamProperty.fSeekSpeed);

	return true;
}
