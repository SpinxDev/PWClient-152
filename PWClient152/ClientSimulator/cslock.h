#pragma once

// ͬ��������
class CsMutex
{
public:
	// ��ʼ���ٽ���
    CsMutex()
    {
        ::InitializeCriticalSection(&mtx);
    }
	// ɾ���ٽ���
    ~CsMutex()
    {
        ::DeleteCriticalSection(&mtx);
    }
	// �����ٽ���
    void Lock()
    {
        ::EnterCriticalSection(&mtx);
    }
	// �뿪�ٽ���
    void Unlock()
    {
        ::LeaveCriticalSection(&mtx);
    }
	// ����ٽ�����ַ
	LPCRITICAL_SECTION GetMtx() { return &mtx; }
private:
    CsMutex(const CsMutex &);
    CsMutex & operator = (const CsMutex &);
    CRITICAL_SECTION mtx;
};

// �����ศ����
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
	// ����ָ��boolֵ������.
	typedef CsMutex * CsLockScoped::*unspecified_bool_type;
	operator unspecified_bool_type() const { return &CsLockScoped::m_pHost; }
private:
	CsLockScoped();
	CsLockScoped(const CsLockScoped&);
	CsLockScoped& operator=(const CsLockScoped&);
	CsMutex* m_pHost;
};