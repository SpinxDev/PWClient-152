#pragma once

class CPropertyInterface
{
public:
	CPropertyInterface(){}
	virtual ~CPropertyInterface(){}

	//	bIsSave: true - update REAL properties from the ADynPropertyObject object
	//			false - update ADynPropertyObject by REAL properties
	//	bIsCommit(valid only if bIsSave == true): 
	//			true - updating for the property comes to an end.
	//			false - update the object from property to the real data. but not ended yet.
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false) = 0;
};