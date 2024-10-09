// Filename	: EC_CustomizePolicy.h
// Creator	: Xu Wenbin
// Date		: 2015/1/21

#pragma once

class CECCustomizeMgr;
class CECCustomizePolicy{
public:
	virtual ~CECCustomizePolicy(){}
	virtual bool ShouldHideAllDialog()const=0;
	virtual bool ShouldShowOKCancel()const=0;
	virtual void OnOK(CECCustomizeMgr &manager)=0;
	virtual void OnCancel(CECCustomizeMgr &manager)=0;
};

class CECCustomizeCreateCharacterPolicy : public CECCustomizePolicy{
public:
	static CECCustomizeCreateCharacterPolicy &Instance();
	virtual bool ShouldHideAllDialog()const;
	virtual bool ShouldShowOKCancel()const;
	virtual void OnOK(CECCustomizeMgr &manager);
	virtual void OnCancel(CECCustomizeMgr &manager);
};

class CECCustomizeLoginCharacterPolicy : public CECCustomizePolicy{
public:
	static CECCustomizeLoginCharacterPolicy &Instance();
	virtual bool ShouldHideAllDialog()const;
	virtual bool ShouldShowOKCancel()const;
	virtual void OnOK(CECCustomizeMgr &manager);
	virtual void OnCancel(CECCustomizeMgr &manager);
};

class CECCustomizeHostPolicy : public CECCustomizePolicy{
public:
	static CECCustomizeHostPolicy &Instance();
	virtual bool ShouldHideAllDialog()const;
	virtual bool ShouldShowOKCancel()const;
	virtual void OnOK(CECCustomizeMgr &manager);
	virtual void OnCancel(CECCustomizeMgr &manager);
};