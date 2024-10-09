#ifndef _EXCEL_CONVERTER_H_
#define _EXCEL_CONVERTER_H_

#include "..\TaskExcelTool\TaskConverter.h"
#include "BaseDataIDMan.h"

class DataIDManForExcel : public BaseDataIDManBase
{
public:
	DataIDManForExcel(){}
	virtual ~DataIDManForExcel(){}

	virtual unsigned long CreateID(const AString& strPath);
};
class CExcelConverter : public TaskConverter
{
public:
	CExcelConverter(){};
	virtual ~CExcelConverter();
	void						ConvertExcel(const char* filename);
	
	typedef std::vector<ATaskTempl*> NewTaskTempls;
	const NewTaskTempls&		GetTempls(){return m_vecNewTempls;}

protected:
	virtual ATaskTempl*			DoConvert( const StringVector& data );
	// �����Ʒ�Ƿ�������Ʒ
	virtual bool				IsTaskMatter( int tid );
	// �������ģ���¼
	bool						AddTaskTempl( int task_id,const String& templName ){return true;}
	bool						Init();
	void						ConvertExcelEnd(bool bSucc);
	virtual bool				ExtractNPC( ATaskTempl* pTask,const String& str,const String& str2 );
	bool						AddNpcTask(bool bDelv, unsigned long ulNPC, unsigned long ulTask);
	std::vector<ATaskTempl*>	m_vecNewTempls;
	struct STR_FILES  
	{
		CString path;
		CString name;
	};
	std::vector<STR_FILES>		m_vecFilesToCheckIn;
	FILE*						m_pLog;
};






#endif