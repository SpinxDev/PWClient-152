#ifndef _A3DGFXGRIDANIMATION_H_
#define _A3DGFXGRIDANIMATION_H_


struct A3DGridDecalEx::GRID_VERTEX_DATA;

struct GRID_ANIM_KEY 
{
	int	 time;
	A3DGridDecalEx::GRID_VERTEX_DATA*		gridVertsData;

	GRID_ANIM_KEY() : time(0), gridVertsData(NULL) {}

	bool operator<(const GRID_ANIM_KEY& other) const
	{
		return time < other.time;
	}

	void Load(AFile* pFile, int size);
	void Save(AFile* pFile, int size);
};

class A3DGfxGridAnimation
{
public:
	A3DGfxGridAnimation();
	~A3DGfxGridAnimation();

	void Clear();
	bool AddKey(int time, A3DGridDecalEx::GRID_VERTEX_DATA*		gridVertsData, int size);
	int GetValue(int time, A3DGridDecalEx::GRID_VERTEX_DATA*		gridVertsData, int size);
	bool GetKeyData(int idx, A3DGridDecalEx::GRID_VERTEX_DATA*		gridVertsData, int size);

	int GetNumKeys() const { return (int)m_Keys.size(); }
	const GRID_ANIM_KEY* GetKey(int idx) const;

	static int nVertsLimit;

public:
	bool Load(AFile* pFile, DWORD dwVersion, int size);
	bool Save(AFile* pFile, int size);
	A3DGfxGridAnimation& operator = (const A3DGfxGridAnimation& src);

protected:
	abase::vector<GRID_ANIM_KEY>		m_Keys;
};


#endif