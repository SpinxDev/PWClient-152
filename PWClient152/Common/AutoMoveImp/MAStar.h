#pragma  once

#include <A3DTypes.h>\

#include "MapNode.h"

namespace AutoMove
{
class IMExpandLimit
{
public:
	virtual	bool Test(MapNode * n) = 0;
};

class MClose;
class MapNode;
class CBitImage;

class CMAStar
{
public:
	CMAStar();
	~CMAStar();
	
	float   Search(int iPfAlg, CBitImage * pRMap, const A3DPOINT2& ptStart, const A3DPOINT2& ptGoal, vector<A3DPOINT2>& path, IMExpandLimit * pExpandLmt = NULL);
private:
	double  _GeneratePath(MClose& closelist, MapNode& dest, vector<A3DPOINT2>& path);
protected:

	int   m_NodesExpanded;
	int   m_NodesTouched;
	int   m_State;
	int   m_MaxExpand;


};

}
