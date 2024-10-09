// Filename	: WikiItemProp.h
// Creator	: Feng Ning
// Date		: 2010/07/20

#include "AAssist.h"

// use this interface to get all necessary properties
class WikiItemProp
{
public:
	virtual ACString GetName() const = 0;
	virtual ACString GetTypeName() const = 0;
	virtual ACString GetDesc() const = 0;
	virtual AString GetIconFile() const = 0;
	virtual unsigned int GetID() const =0;
	
	virtual ~WikiItemProp() {};
	
	static WikiItemProp* CreateProperty(unsigned int id, bool noEquip = true);
	
protected:
	WikiItemProp(){};
	WikiItemProp(const WikiItemProp&);
	WikiItemProp& operator=(const WikiItemProp&);
};