#ifndef _CTRIANGLE_H_
#define _CTRIANGLE_H_

#include <AArray.h>

class CTriangle  
{
public:
	union
	{
		struct  
		{
			int Index0, Index1, Index2;
		};
		int Index[3];
	};

	CTriangle(int index0 = 0, int index1 = 1, int index2 = 2);
	CTriangle(int index[3]);
	CTriangle(const CTriangle& src);
	~CTriangle();

	CTriangle & operator = (const CTriangle & src);
	bool operator == (const CTriangle & src);
};

typedef AArray<CTriangle, CTriangle&> ATriangleArray;


#endif 