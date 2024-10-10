#ifndef _ECMODELHOOK_H_
#define _ECMODELHOOK_H_


//	This class is used by client
class CECModelHook
{
public:

	CECModelHook();
	~CECModelHook();

	//	Save / Load ECModelHook
	bool Save(AFile* pFileToSave);
	bool Load(AFile* pFileToLoad, DWORD dwVersion);

	//	Set / Get Name
	void SetName(const char* szName) { m_strName = szName; }
	const char* GetName() const { return m_strName; }

	//	Set a custom ID, which could be used by the game client
	void SetID(int nID) { m_nID = nID; }
	int GetID() const { return m_nID; }

	//	Set / Get scale infomation (used for gfx playing)
	void SetScaleFactor(float fScale) { m_fScale = fScale; }
	float GetScaleFactor() const { return m_fScale; }

	//	Set / Get relative matrix (relative to ecmodel's a3dskinmodel's origin pos)
	void SetRelativeMatrix(const A3DMATRIX4& matRel) { m_matRelative = matRel; }
	const A3DMATRIX4& GetRelativeMatrix() const { return m_matRelative; }
 
private:
	
	AString	m_strName;
	int m_nID;
	float m_fScale;
	A3DMATRIX4 m_matRelative;
};



#endif

