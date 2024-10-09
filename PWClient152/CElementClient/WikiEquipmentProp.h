// Filename	: WikiEquipmentProp.h
// Creator	: Feng Ning
// Date		: 2010/04/26

#include "AAssist.h"

// use this interface to get all necessary properties
class WikiEquipmentProp
{
public:
	virtual ACString GetName() const = 0;
	virtual ACString GetTypeName() const = 0;
	virtual ACString GetDesc() const = 0;
	virtual AString GetIconFile() const = 0;

	virtual unsigned int GetID() const =0;
	virtual int GetGender() const =0;
	virtual unsigned int GetClass() const =0;
	virtual int GetLevel() const =0;
	virtual int GetLevelRequire() const =0;
	virtual int GetLevelupAddon() const =0;
	
	virtual unsigned int GetSplit() const =0;
	virtual int GetSplitNumber() const =0;
	
	virtual unsigned int GetDamaged() const =0;
	virtual int GetDamagedNumber() const =0;
	
	virtual unsigned int GetRandPropID(unsigned int i) const = 0;
	virtual unsigned int GetRandPropSize() const = 0;
	
	virtual ACString GetRandPropName(unsigned int i) const = 0;
	
	virtual ~WikiEquipmentProp() {};
	
	static WikiEquipmentProp* CreateProperty(unsigned int id);
	
protected:
	WikiEquipmentProp(){};
	WikiEquipmentProp(const WikiEquipmentProp&);
	WikiEquipmentProp& operator=(const WikiEquipmentProp&);
};