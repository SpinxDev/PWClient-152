//////////////////////////////////////////////////////////////////////////
// this file defines some macros to ease the coding work
//////////////////////////////////////////////////////////////////////////


#ifndef _CODETEMPLATE_H
#define _CODETEMPLATE_H

#include <Windows.h>
#include <AAssist.h>

//////////////////////////////////////////////////////////////////////////
// some globe constant
//////////////////////////////////////////////////////////////////////////

#define	COLOR_BLACK			0xff000000
#define	COLOR_WHITE			0xffffffff
#define COLOR_LIGHT_GRAY	0x80c0c0c0
#define COLOR_DARK_GRAY		0x80404040

#define STYLE_NORMAL	0x00000000
#define STYLE_BOLD		0x00000001
#define STYLE_ITALIC	0x00000010

//////////////////////////////////////////////////////////////////////////
// code structure helper
//////////////////////////////////////////////////////////////////////////

#define SAFE_DELETE(p) {delete (p); (p) = NULL;}
#define SAFE_DELETE_ARRAY(p) {delete [] (p); (p) = NULL;}

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

#define CHECK_BREAK(p) {if (!(p)) break;}
#define CHECK_BREAK_LOG0(p, msg) \
{ \
	if (!(p)) \
	{ \
		FWLog::GetInstance()->Log(msg); \
		break; \
	} \
}
#define CHECK_BREAK_LOG1(p, msg, p0) \
{ \
	if (!(p)) \
	{ \
		FWLog::GetInstance()->Log(msg, p0); \
		break; \
	} \
}
#define CHECK_BREAK_LOG2(p, msg, p0, p1) \
{ \
	if (!(p)) \
	{ \
		FWLog::GetInstance()->Log(msg, p0, p1); \
		break; \
	} \
}
#define CHECK_BREAK_LOG3(p, msg, p0, p1, p2) \
{ \
	if (!(p)) \
	{ \
		FWLog::GetInstance()->Log(msg, p0, p1, p2); \
		break; \
	} \
}
#define CHECK_BREAK_LOG4(p, msg, p0, p1, p2, p3) \
{ \
	if (!(p)) \
	{ \
		FWLog::GetInstance()->Log(msg, p0, p1, p2, p3); \
		break; \
	} \
}
#define CHECK_BREAK_LOG5(p, msg, p0, p1, p2, p3, p4) \
{ \
	if (!(p)) \
	{ \
		FWLog::GetInstance()->Log(msg, p0, p1, p2, p3, p4); \
		break; \
	} \
}
#define CHECK_BREAK_LOG6(p, msg, p0, p1, p2, p3, p4, p5) \
{ \
	if (!(p)) \
	{ \
		FWLog::GetInstance()->Log(msg, p0, p1, p2, p3, p4, p5); \
		break; \
	} \
}

#define BEGIN_FAKE_WHILE	\
	bool bSucceed = false;  \
do {

#define END_FAKE_WHILE		\
	bSucceed = true;	\
} while (false);

#define RETURN_FAKE_WHILE_RESULT \
return bSucceed;

#define BEGIN_ON_FAIL_FAKE_WHILE	\
if (!bSucceed) { 

#define END_ON_FAIL_FAKE_WHILE	\
}

#ifdef UNICODE
#define S2AS WC2AS
#else
#define S2AS
#endif

#ifdef UNICODE
#define AS2S AS2WC
#else
#define AS2S
#endif

//////////////////////////////////////////////////////////////////////////
// property template
//////////////////////////////////////////////////////////////////////////

// bool
#define PROPERTY_DEFINE_BOOL(name)				bool m_b##name;
#define PROPERTY_GET_BOOL(name)					bool Get##name() const { return m_b##name; } 
#define PROPERTY_SET_BOOL(name)					void Set##name(bool val) { m_b##name = val; } 
#define PROPERTY_GET_SET_BOOL(name)				PROPERTY_GET_BOOL(name) PROPERTY_SET_BOOL(name)
#define PROPERTY_BOOL(name) \
	protected: \
	PROPERTY_DEFINE_BOOL(name) \
	public: \
PROPERTY_GET_SET_BOOL(name)
#define PROPERTY_INIT_BOOL(name, val)			m_b##name(val)


// float
#define PROPERTY_DEFINE_FLOAT(name)				float m_f##name;
#define PROPERTY_GET_FLOAT(name)				float Get##name() const { return m_f##name; } 
#define PROPERTY_SET_FLOAT(name)				void Set##name(float val) { m_f##name = val; } 
#define PROPERTY_GET_SET_FLOAT(name)			PROPERTY_GET_FLOAT(name) PROPERTY_SET_FLOAT(name)
#define PROPERTY_FLOAT(name) \
	protected: \
		PROPERTY_DEFINE_FLOAT(name) \
	public: \
		PROPERTY_GET_SET_FLOAT(name)
#define PROPERTY_INIT_FLOAT(name, val)			m_f##name(val)

// DWORD
#define PROPERTY_DEFINE_DWORD(name)				DWORD m_dw##name;
#define PROPERTY_GET_DWORD(name)				DWORD Get##name() const { return m_dw##name; } 
#define PROPERTY_SET_DWORD(name)				void Set##name(DWORD val) { m_dw##name = val; } 
#define PROPERTY_GET_SET_DWORD(name)			PROPERTY_GET_DWORD(name) PROPERTY_SET_DWORD(name)
#define PROPERTY_DWORD(name) \
	protected: \
		PROPERTY_DEFINE_DWORD(name) \
	public: \
		PROPERTY_GET_SET_DWORD(name)
#define PROPERTY_INIT_DWORD(name, val)			m_dw##name(val)

// int
#define PROPERTY_DEFINE_INT(name)				int m_n##name;
#define PROPERTY_GET_INT(name)					int Get##name() const { return m_n##name; } 
#define PROPERTY_SET_INT(name)					void Set##name(int val) { m_n##name = val; } 
#define PROPERTY_GET_SET_INT(name)				PROPERTY_GET_INT(name) PROPERTY_SET_INT(name)
#define PROPERTY_INT(name) \
	protected: \
		PROPERTY_DEFINE_INT(name) \
	public: \
		PROPERTY_GET_SET_INT(name)
#define PROPERTY_INIT_INT(name, val)			m_n##name(val)

// ACString
#define PROPERTY_DEFINE_STRING(name)			ACString m_str##name;
#define PROPERTY_GET_STRING(name)				const ACString & Get##name() const { return m_str##name; } 
#define PROPERTY_SET_STRING(name) \
void Set##name(const ACString & val) { m_str##name = val; } \
void Set##name(const ACHAR * val) { m_str##name = ACString(val); }

#define PROPERTY_GET_SET_STRING(name)			PROPERTY_GET_STRING(name) PROPERTY_SET_STRING(name)
#define PROPERTY_STRING(name) \
	protected: \
		PROPERTY_DEFINE_STRING(name) \
	public: \
		PROPERTY_GET_SET_STRING(name)
#define PROPERTY_INIT_STRING(name, val)			m_str##name(val)

// AString
#define PROPERTY_DEFINE_ASTRING(name)			AString m_str##name;
#define PROPERTY_GET_ASTRING(name)				const AString & Get##name() const { return m_str##name; } 
#define PROPERTY_SET_ASTRING(name)	\
void Set##name(const AString & val) { m_str##name = val; } \
void Set##name(const char * val) { m_str##name = AString(val); } 

#define PROPERTY_GET_SET_ASTRING(name)			PROPERTY_GET_ASTRING(name) PROPERTY_SET_ASTRING(name)
#define PROPERTY_ASTRING(name) \
	protected: \
	PROPERTY_DEFINE_ASTRING(name) \
	public: \
PROPERTY_GET_SET_ASTRING(name)
#define PROPERTY_INIT_ASTRING(name, val)			m_str##name(val)

// A3DVECTOR3
#define PROPERTY_DEFINE_VEC3(name)				A3DVECTOR3 m_vec##name;
#define PROPERTY_GET_VEC3(name)					const A3DVECTOR3 & Get##name() const { return m_vec##name; } 
#define PROPERTY_SET_VEC3(name)					void Set##name(const A3DVECTOR3 & val) { m_vec##name = val; } 
#define PROPERTY_GET_SET_VEC3(name)				PROPERTY_GET_VEC3(name) PROPERTY_SET_VEC3(name)
#define PROPERTY_VEC3(name) \
	protected: \
		PROPERTY_DEFINE_VEC3(name) \
	public: \
		PROPERTY_GET_SET_VEC3(name)
#define PROPERTY_INIT_VEC3(name, val)			m_vec##name(val)

// pointer
#define PROPERTY_DEFINE_POINTER(type, name)		type m_p##name;
#define PROPERTY_GET_POINTER(type, name)		type Get##name() const { return m_p##name; } 
#define PROPERTY_SET_POINTER(type, name)		void Set##name(type val) { m_p##name = val; } 
#define PROPERTY_GET_SET_POINTER(type, name)	PROPERTY_GET_POINTER(type, name) PROPERTY_SET_POINTER(type, name)
#define PROPERTY_POINTER(type, name) \
	protected: \
		PROPERTY_DEFINE_POINTER(type, name) \
	public: \
		PROPERTY_GET_SET_POINTER(type, name)
#define PROPERTY_INIT_POINTER(name, val)		m_p##name(val)


// common
#define PROPERTY_READ(owner, name)		((owner)->Get##name())
#define PROPERTY_WRITE(owner, name, val)	{(owner)->Set##name(val);}

//////////////////////////////////////////////////////////////////////////
// clone mechanism implement helper
//////////////////////////////////////////////////////////////////////////

#define DECLARE_CLONE(mostbase) \
virtual mostbase * Clone() const;

#define IMPLEMENT_CLONE(type, mostbase) \
mostbase * type::Clone() const\
{ \
	type *pNewObject = new type; \
	return &(*pNewObject = *this); \
}

#define IMPLEMENT_CLONE_INLINE(type, mostbase) \
virtual mostbase * Clone() const\
{ \
	type *pNewObject = new type; \
	return &(*pNewObject = *this); \
}

#define IMPLEMENT_EMPTY_OPERATER_EQUAL_INLINE(type, base) \
type & operator = (const type & src) \
{ \
	base::operator = (src); \
	return *this; \
}



//////////////////////////////////////////////////////////////////////////
// event handle helper for AUI
//////////////////////////////////////////////////////////////////////////

class FWDialogBase;
typedef void (FWDialogBase::*ON_EVENT_FUNC_TYPE_FW)(WPARAM, LPARAM);

struct EVENT_MAP_ENTRY_FW
{ 
	const char * szObjectName; 
	UINT uMessage; 
	ON_EVENT_FUNC_TYPE_FW pFunc; 
}; 

struct EVENT_MAP_TYPE_FW
{
	const EVENT_MAP_TYPE_FW * pBaseMap;
	const EVENT_MAP_ENTRY_FW * pMapEntries;
};


#define DECLARE_EVENT_MAP_FW() \
private: \
	static const EVENT_MAP_ENTRY_FW _eventMapEntries[]; \
protected: \
	static const EVENT_MAP_TYPE_FW _eventMap; \
	virtual const EVENT_MAP_TYPE_FW * GetEventMap() const; 

#define BEGIN_EVENT_MAP_FW(theClass, baseClass) \
const EVENT_MAP_TYPE_FW * theClass::GetEventMap() const \
{ return &theClass::_eventMap; } \
const EVENT_MAP_TYPE_FW theClass::_eventMap = \
{ &baseClass::_eventMap, theClass::_eventMapEntries}; \
const EVENT_MAP_ENTRY_FW theClass::_eventMapEntries[] = \
{

#define ON_EVENT_FW(objname, message, func) \
	{(objname), (message), (ON_EVENT_FUNC_TYPE_FW)(&func)},

#define END_EVENT_MAP_FW() \
	{0, 0, 0} \
};


#define BEGIN_EVENT_MAP_FW_BASE(theClass) \
const EVENT_MAP_TYPE_FW * theClass::GetEventMap() const \
{ return &theClass::_eventMap; } \
const EVENT_MAP_TYPE_FW theClass::_eventMap = \
{ NULL, theClass::_eventMapEntries}; \
const EVENT_MAP_ENTRY_FW theClass::_eventMapEntries[] = \
{


//////////////////////////////////////////////////////////////////////////
// command handle helper for AUI
//////////////////////////////////////////////////////////////////////////

class FWDialogBase;
typedef void (FWDialogBase::*ON_COMMAND_FUNC_TYPE_FW)();

struct COMMAND_MAP_ENTRY_FW
{ 
	const char * szCommand; 
	ON_COMMAND_FUNC_TYPE_FW pFunc; 
}; 

struct COMMAND_MAP_TYPE_FW
{
	const COMMAND_MAP_TYPE_FW * pBaseMap;
	const COMMAND_MAP_ENTRY_FW * pMapEntries;
};


#define DECLARE_COMMAND_MAP_FW() \
private: \
	static const COMMAND_MAP_ENTRY_FW _commandMapEntries[]; \
protected: \
	static const COMMAND_MAP_TYPE_FW _commandMap; \
	virtual const COMMAND_MAP_TYPE_FW * GetCommandMap() const; 

#define BEGIN_COMMAND_MAP_FW(theClass, baseClass) \
const COMMAND_MAP_TYPE_FW * theClass::GetCommandMap() const \
{ return &theClass::_commandMap; } \
const COMMAND_MAP_TYPE_FW theClass::_commandMap = \
{ &baseClass::_commandMap, theClass::_commandMapEntries}; \
const COMMAND_MAP_ENTRY_FW theClass::_commandMapEntries[] = \
{

#define ON_COMMAND_FW(command, func) \
	{(command), (ON_COMMAND_FUNC_TYPE_FW)(&func)},

#define END_COMMAND_MAP_FW() \
	{0, 0} \
};


#define BEGIN_COMMAND_MAP_FW_BASE(theClass) \
const COMMAND_MAP_TYPE_FW * theClass::GetCommandMap() const \
{ return &theClass::_commandMap; } \
const COMMAND_MAP_TYPE_FW theClass::_commandMap = \
{ NULL, theClass::_commandMapEntries}; \
const COMMAND_MAP_ENTRY_FW theClass::_commandMapEntries[] = \
{

//////////////////////////////////////////////////////////////////////////
// singleton patten implement helper
//////////////////////////////////////////////////////////////////////////

class FWCriticalSection
{ 
protected: 
	CRITICAL_SECTION cs;
public:
	FWCriticalSection() { ::InitializeCriticalSection (&cs); }; 
	~FWCriticalSection() { ::DeleteCriticalSection(&cs); }
	void EnterCriticalSection() { ::EnterCriticalSection(&cs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(&cs); }
}; 

#define DECLARE_SINGLETON(theClass) \
protected: \
	static theClass * s_pInstance; \
	static FWCriticalSection s_CSForInstance; \
public: \
	static theClass * GetInstance(); \
	static void DestroyInstance();

#define IMPLEMENT_SINGLETON(theClass) \
FWCriticalSection theClass::s_CSForInstance; \
theClass * theClass::s_pInstance = NULL; \
theClass * theClass::GetInstance() \
{ \
	if (!s_pInstance) \
	{ \
		theClass::s_CSForInstance.EnterCriticalSection(); \
		if (!s_pInstance) \
			s_pInstance = new theClass; \
		theClass::s_CSForInstance.LeaveCriticalSection(); \
	} \
	return s_pInstance; \
} \
void theClass::DestroyInstance() \
{ \
	SAFE_DELETE(s_pInstance); \
}

//////////////////////////////////////////////////////////////////////////
// miscellaneous
//////////////////////////////////////////////////////////////////////////

// for convenient when name is a identifier : TO_STRING and name
// are both supported by visual assist, it is much easier to use
// them than to type a string
#define TO_STRING(name) (#name)

// make call to GetFWDialog convenient : you can use class name it self
// as the parameter
#define GetFWDialog2(className) (static_cast<className *>(GetFWDialog(#className)))

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#endif