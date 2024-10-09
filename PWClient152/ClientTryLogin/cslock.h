#pragma once

// 同步互斥类
class CsMutex
{
public:
	// 初始化临界区
    CsMutex()
    {
        ::InitializeCriticalSection(&mtx);
    }
	// 删除临界区
    ~CsMutex()
    {
        ::DeleteCriticalSection(&mtx);
    }
	// 进入临界区
    void Lock()
    {
        ::EnterCriticalSection(&mtx);
    }
	// 离开临界区
    void Unlock()
    {
        ::LeaveCriticalSection(&mtx);
    }
	// 获得临界区地址
	LPCRITICAL_SECTION GetMtx() { return &mtx; }
private:
    CsMutex(const CsMutex &);
    CsMutex & operator = (const CsMutex &);
    CRITICAL_SECTION mtx;
};

// 互斥类辅助类
class CsLockScoped
{
public:
	explicit CsLockScoped(CsMutex& host) : m_pHost(&host)
	{
		m_pHost->Lock();
	}
	explicit CsLockScoped(CsMutex* pHost) : m_pHost(pHost)
	{
		m_pHost->Lock();
	}
	~CsLockScoped()
	{
		m_pHost->Unlock();
	}
	// 重载指针bool值操作符.
	typedef CsMutex * CsLockScoped::*unspecified_bool_type;
	operator unspecified_bool_type() const { return &CsLockScoped::m_pHost; }
private:
	CsLockScoped();
	CsLockScoped(const CsLockScoped&);
	CsLockScoped& operator=(const CsLockScoped&);
	CsMutex* m_pHost;
};