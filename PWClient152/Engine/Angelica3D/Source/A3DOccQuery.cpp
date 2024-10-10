#include "A3DOccQuery.h"
#include "A3DDevice.h"
#include "A3DDevObject.h"

extern ALog g_A3DErrLog;

class A3DOccQueryImpl : public IA3DOccQueryImpl, public A3DDevObject
{
public:
    A3DOccQueryImpl();
    virtual ~A3DOccQueryImpl();

    bool Init(A3DDevice* pA3DDevice);

    // Inherited from IA3DOccQueryImpl
    virtual bool BeginQuery();
    virtual void EndQuery(LONG lID);
    virtual void GetData();

    // Inherited from IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID FAR& riid, LPVOID FAR* ppvObj) { return S_OK; }
    virtual ULONG STDMETHODCALLTYPE AddRef() { return S_OK; }
    virtual ULONG STDMETHODCALLTYPE Release();

    //	Before device reset
    virtual bool BeforeDeviceReset();
    //	After device reset
    virtual bool AfterDeviceReset();

public:
    IDirect3DQuery9* m_pD3DQuery;	
    A3DDevice* m_pDevice;
    LONG m_lQueryID;    // id of the latest query
    LONG m_lResultID;   // id of the latest result
    DWORD m_dwResult;     // the result
};


A3DOccQuery::A3DOccQuery()
{
    m_pDevice = NULL;
    m_pImpl = NULL;
}

A3DOccQuery::~A3DOccQuery()
{

}

bool A3DOccQuery::Init(A3DDevice* pA3DDevice)
{
    m_pDevice = pA3DDevice;
    A3DOccQueryImpl* pImpl = new A3DOccQueryImpl;
    if (!pImpl->Init(pA3DDevice))
    {
        pImpl->Release();
        return false;
    }
    m_pImpl = pImpl;
    return true;
}

bool A3DOccQuery::BeginQuery()
{
    if (m_pDevice->GetNeedResetFlag())
        return false;

    if (m_pImpl)
    {
        return m_pDevice->BeginOccQuery(m_pImpl);
    }
    else
    {
        return false;
    }
}

void A3DOccQuery::EndQuery()
{
    if (m_pDevice->GetNeedResetFlag())
        return;

    if (!m_pImpl)
        return;
    A3DOccQueryImpl* pImp = (A3DOccQueryImpl*)m_pImpl;
    InterlockedIncrement(&pImp->m_lQueryID);
    m_pDevice->EndOccQuery(pImp, pImp->m_lQueryID);
}

int A3DOccQuery::GetDataAsync()
{
    if (m_pDevice->GetNeedResetFlag())
        return -1;

    if (!m_pImpl)
        return -1;
    A3DOccQueryImpl* pImp = (A3DOccQueryImpl*)m_pImpl;
    
    if (pImp->m_lQueryID != pImp->m_lResultID)
    {
        m_pDevice->GetOccQueryData(pImp);
        return -1;
    }
    else
    {
        return pImp->m_dwResult;
    }
}

void A3DOccQuery::Release()
{
    if (m_pImpl && m_pDevice)
    {
        if (m_pDevice->GetNeedResetFlag())
        {
            m_pImpl->Release();
        }
        else
        {
            m_pDevice->ReleaseResource(m_pImpl);
        }
        m_pImpl = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
//
// A3DOccQueryImp
//////////////////////////////////////////////////////////////////////////

A3DOccQueryImpl::A3DOccQueryImpl()
{
    m_pDevice = NULL;
    m_lQueryID = 0;
    m_lResultID = 0;
    m_dwResult = 0;
    m_pD3DQuery = NULL;
}

A3DOccQueryImpl::~A3DOccQueryImpl()
{

}

bool A3DOccQueryImpl::Init(A3DDevice* pA3DDevice)
{
    m_pDevice = pA3DDevice;

    if (FAILED(m_pDevice->GetD3DDevice()->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_pD3DQuery)))
    {
        g_A3DErrLog.Log("A3DOccQueryImp::Init, Failed to create occlusion query");
        return false;
    }
    m_pDevice->AddUnmanagedDevObject(this);
    return true;
}

ULONG STDMETHODCALLTYPE A3DOccQueryImpl::Release()
{
    if (m_pD3DQuery)
    {
        m_pD3DQuery->Release();
        m_pD3DQuery = NULL;
    }
    m_pDevice->RemoveUnmanagedDevObject(this);
    delete this;
    return S_OK;
}

bool A3DOccQueryImpl::BeginQuery()
{
    if (!m_pD3DQuery)
        return false;

    if (FAILED(m_pD3DQuery->Issue(D3DISSUE_BEGIN)))
        return false;

    return true;
}

void A3DOccQueryImpl::EndQuery(LONG lID)
{
    if (!m_pD3DQuery)
        return;
    m_pD3DQuery->Issue(D3DISSUE_END);
    m_lResultID = lID - 1;
}

void A3DOccQueryImpl::GetData()
{
    // be sure to get the latest result
    if (m_lQueryID == m_lResultID + 1)
    {
        DWORD dwResult;
        if (S_OK == m_pD3DQuery->GetData(&dwResult, sizeof(DWORD), D3DGETDATA_FLUSH))
        {
            m_dwResult = dwResult;
            InterlockedIncrement(&m_lResultID);
        }
    }
}

bool A3DOccQueryImpl::BeforeDeviceReset()
{
    if (m_pD3DQuery)
    {
        m_pD3DQuery->Release();
        m_pD3DQuery = NULL;
    }
    return true;
}

bool A3DOccQueryImpl::AfterDeviceReset()
{
    if (FAILED(m_pDevice->GetD3DDevice()->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_pD3DQuery)))
    {
        g_A3DErrLog.Log("A3DOccQueryImp::AfterDeviceReset, Failed to create occlusion query");
        return false;
    }
    m_lQueryID = 0;
    m_lResultID = 0;

    return true;
}