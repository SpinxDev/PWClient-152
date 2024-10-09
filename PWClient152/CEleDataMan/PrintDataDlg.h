#if !defined(AFX_TESTGENERATEITEMDLG_H__B75BF659_55B8_442F_ABB2_2A145E69EA84__INCLUDED_)
#define AFX_TESTGENERATEITEMDLG_H__B75BF659_55B8_442F_ABB2_2A145E69EA84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintDataDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintDataDlg dialog

class CPrintDataDlg : public CDialog
{
// Construction
	enum Type
	{
		TEMPLATE_DATA = 0,
		GENERATE_DATA = 1,
	};

	enum GenerateStyle
	{
		RANDOM = 0,			
		FORSALE = 1,
		PRODUCE = 2,
		SPEC_GEN = 3,
	};

public:
	CPrintDataDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintDataDlg)
	enum { IDD = IDD_PRINT_DIALOG };
	CComboBox	m_cmbGenerateStyle;
	CComboBox	m_cmbType;
	CComboBox	m_cmbIDSpace;
	CSpinButtonCtrl	m_spnID;
	UINT		m_nID;
	ID_SPACE	m_eIDSpace;
	CString		m_strItemInfo;
	Type		m_eDataType;
	GenerateStyle m_eGenerateStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// generated data
	void PrintItem(const void * item, DATA_TYPE datatype);
	void PrintEquipDataHeader(const void ** item);
	void PrintItemDataHeader(const void ** item);
	void PrintItemHoleAndAddon(const void ** item);
	
	void PrintWeapon(const void * item);
	void PrintArmor(const void * item);
	void PrintProjectile(const void * item);
	void PrintDecoration(const void * item);
	void PrintStone(const void * item);

	// template data
	void PrintTemplateData(const void * data, DATA_TYPE datatype);

	void Print_EQUIPMENT_ADDON(const void * data);
	void Print_WEAPON_MAJOR_TYPE(const void * data);
	void Print_WEAPON_SUB_TYPE(const void * data);
	void Print_WEAPON_ESSENCE(const void * data);
	void Print_ARMOR_MAJOR_TYPE(const void * data);

	void Print_ARMOR_SUB_TYPE(const void * data);
	void Print_ARMOR_ESSENCE(const void * data);
	void Print_DECORATION_MAJOR_TYPE(const void * data);
	void Print_DECORATION_SUB_TYPE(const void * data);
	void Print_DECORATION_ESSENCE(const void * data);

	void Print_MEDICINE_MAJOR_TYPE(const void * data);
	void Print_MEDICINE_SUB_TYPE(const void * data);
	void Print_MEDICINE_ESSENCE(const void * data);
	void Print_MATERIAL_MAJOR_TYPE(const void * data);
	void Print_MATERIAL_SUB_TYPE(const void * data);

	void Print_MATERIAL_ESSENCE(const void * data);
	void Print_DAMAGERUNE_SUB_TYPE(const void * data);
	void Print_DAMAGERUNE_ESSENCE(const void * data);
	void Print_ARMORRUNE_SUB_TYPE(const void * data);
	void Print_ARMORRUNE_ESSENCE(const void * data);

	void Print_SKILLTOME_SUB_TYPE(const void * data);	
	void Print_SKILLTOME_ESSENCE(const void * data);
	void Print_FLYSWORD_ESSENCE(const void * data);
	void Print_WINGMANWING_ESSENCE(const void * data);
	void Print_TOWNSCROLL_ESSENCE(const void * data);
	void Print_UNIONSCROLL_ESSENCE(const void * data);

	void Print_REVIVESCROLL_ESSENCE(const void * data);	
	void Print_ELEMENT_ESSENCE(const void * data);
	void Print_TASKMATTER_ESSENCE(const void * data);
	void Print_TOSSMATTER_ESSENCE(const void * data);
	void Print_PROJECTILE_TYPE(const void * data);

	void Print_PROJECTILE_ESSENCE(const void * data);
	void Print_QUIVER_SUB_TYPE(const void * data);
	void Print_QUIVER_ESSENCE(const void * data);
	void Print_STONE_SUB_TYPE(const void * data);
	void Print_STONE_ESSENCE(const void * data);
	
	void Print_MONSTER_ADDON(const void * data);
	void Print_MONSTER_TYPE(const void * data);
	void Print_MONSTER_ESSENCE(const void * data);
	void Print_NPC_TALK_SERVICE(const void * data);
	void Print_NPC_SELL_SERVICE(const void * data);

	void Print_NPC_BUY_SERVICE(const void * data);
	void Print_NPC_REPAIR_SERVICE(const void * data);
	void Print_NPC_INSTALL_SERVICE(const void * data);
	void Print_NPC_UNINSTALL_SERVICE(const void * data);
	void Print_NPC_TASK_OUT_SERVICE(const void * data);

	void Print_NPC_TASK_IN_SERVICE(const void * data);
	void Print_NPC_TASK_MATTER_SERVICE(const void * data);
	void Print_NPC_SKILL_SERVICE(const void * data);
	void Print_NPC_HEAL_SERVICE(const void * data);
	void Print_NPC_TRANSMIT_SERVICE(const void * data);

	void Print_NPC_TRANSPORT_SERVICE(const void * data);
	void Print_NPC_PROXY_SERVICE(const void * data);
	void Print_NPC_STORAGE_SERVICE(const void * data);
	void Print_NPC_MAKE_SERVICE(const void * data);
	void Print_NPC_DECOMPOSE_SERVICE(const void * data);
	
	void Print_NPC_TYPE(const void * data);
	void Print_NPC_ESSENCE(const void * data);

	void Print_RECIPE_MAJOR_TYPE(const void * data);
	void Print_RECIPE_SUB_TYPE(const void * data);
	void Print_RECIPE_ESSENCE(const void * data);
	
	void Print_FACE_TEXTURE_ESSENCE(const void * data);
	void Print_FACE_SHAPE_ESSENCE(const void * data);
	void Print_FACE_EMOTION_TYPE(const void * data);
	void Print_FACE_EXPRESSION_ESSENCE(const void * data);
	void Print_FACE_HAIR_ESSENCE(const void * data);
	void Print_FACE_MOUSTACHE_ESSENCE(const void * data);
	void Print_COLORPICKER_ESSENCE(const void * data);
	void Print_CUSTOMIZEDATA_ESSENCE(const void* data);
	
	void Print_ENEMY_FACTION_CONFIG(const void * data);
	void Print_CHARRACTER_CLASS_CONFIG(const void * data);
	void Print_PARAM_ADJUST_CONFIG(const void * data);
	void Print_PLAYER_ACTION_INFO_CONFIG(const void * data);
	void Print_TASKDICE_ESSENCE(const void * data);

	void Print_TASKNORMALMATTER_ESSENCE(const void * data);

	
// int, unsigned int, size_t
template<typename T>
void print_data(CString name, T** ptr, size_t count = 1)
{
	bool numsuffix = count > 1;
	CString str, strNo;	
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" : ");
		m_strItemInfo += str;
		str.Format(_T("%d\r\n"), *(*ptr)); (*ptr)++;
		m_strItemInfo += str;
	}
}

// float
template<>
void print_data(CString name, float** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;	
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("%f\r\n"), *(*ptr)); (*ptr)++;
		m_strItemInfo += str;
	}
}

#define CODE_PAGE	CP_ACP

class CSafeString
{
	LPSTR m_szBuf;
	LPWSTR m_wszBuf;

public:
	CSafeString(LPCSTR lpsz) : m_wszBuf(NULL)
	{
		ASSERT(lpsz);
		int n = strlen(lpsz);
		m_szBuf = new char[n+1];
		strcpy(m_szBuf, lpsz);
	}
	
	CSafeString(LPCWSTR lpwsz) : m_szBuf(NULL)
	{
		ASSERT(lpwsz);
		int n = wcslen(lpwsz);
		m_wszBuf = new wchar_t[n+1];
		wcscpy(m_wszBuf, lpwsz);
	}

	operator LPCSTR() { return GetAnsi();}
	operator LPCWSTR() { return GetUnicode(); }
	
	CSafeString& operator= (const CSafeString& str)
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;

		m_szBuf = NULL;
		m_wszBuf = NULL;

		if (str.m_szBuf)
		{
			m_szBuf = new char[strlen(str.m_szBuf)+1];
			strcpy(m_szBuf, str.m_szBuf);
		}

		if (str.m_wszBuf)
		{
			m_wszBuf = new wchar_t[wcslen(str.m_wszBuf)+1];
			wcscpy(m_wszBuf, str.m_wszBuf);
		}
		return *this;
	}

	LPCSTR GetAnsi()
	{
		if (m_szBuf) return m_szBuf;
		ASSERT(m_wszBuf);
		
		int nCount = WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			NULL,
			0,
			NULL,
			NULL);
	
		m_szBuf = new char[nCount];

		WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			m_szBuf,
			nCount,
			NULL,
			NULL);
		return m_szBuf;
	}
	
	LPCWSTR GetUnicode()
	{
		if (m_wszBuf) return m_wszBuf;
		ASSERT(m_szBuf);
		
		int nCount = MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			NULL,
			0);

		m_wszBuf = new wchar_t[nCount];

		MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			m_wszBuf,
			nCount);

		return m_wszBuf;
	}

	virtual ~CSafeString() 
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;
	}
};




//namechar
template<>
void print_data(CString name, namechar** ptr, size_t count)
{
	CString str;
	str = _T(name)+_T(" :  ");
	m_strItemInfo += str;
	str.Format(_T("%s\r\n"), CSafeString(*ptr).GetAnsi()); (*ptr)+=count;
	m_strItemInfo += str;
}

template<>
void print_data(CString name, char** ptr, size_t count)
{
	CString str;
	str = _T(name)+_T(" :  ");
	m_strItemInfo += str;
	str.Format(_T("%s\r\n"), (*ptr)); (*ptr)+=count;
	m_strItemInfo += str;
}

	struct addon
	{
		unsigned int	id_addon;				// 附加属性的类型ID
		float			probability_addon;		// 附加属性出现的概率
	};
template<>
void print_data(CString name, addon** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id = %d \t prob = %f \r\n"), (*ptr)->id_addon, (*ptr)->probability_addon);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct aggro_strategy
	{
		unsigned int	id;						// 仇恨度策略，可以是：普通，杀低级，杀血少之一
		float			probability;			// 仇恨度策略出现概率
	};
template<>
void print_data(CString name, aggro_strategy** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);		
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id = %d \t prob = %f \r\n"), (*ptr)->id, (*ptr)->probability);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct drop_matter
	{
		unsigned int	id;						// 掉落的物品
		float			probability;			// 掉落的概率
	};
template<>
void print_data(CString name, drop_matter** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id = %d \t prob = %f \r\n"), (*ptr)->id, (*ptr)->probability);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct magic_defence{
		int			low;						// 法术防御力最小值
		int			high;						// 法术防御力最大值
	};	// 共5个是因为分金木水火土五系

template<>
void print_data(CString name, magic_defence** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("low = %d \t high = %d \r\n"), (*ptr)->low, (*ptr)->high);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct magic_damages_ext
	{
		int				damage_min;				// 物理攻击附带的五系魔法伤害值: 最小
		int				damage_max;				// 物理攻击附带的五系魔法伤害值: 最大
	};
template<>
void print_data(CString name, magic_damages_ext** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("damage_min = %d \t damage_max = %d \r\n"), (*ptr)->damage_min, (*ptr)->damage_max);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct skill_hp75
	{
		unsigned int	id_skill;				// 怪物的条件技能类型ID	(HP<75%时使用的技能)
		int				level;					// 技能的级别
		float			probability;			// 该条件技能出现的概率
	};

template<>
void print_data(CString name, skill_hp75** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id_skill = %d \t level = %d \r\n"), (*ptr)->id_skill, (*ptr)->level);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct skill_hp50
	{
		unsigned int	id_skill;				// 怪物的条件技能类型ID	(HP<50%时使用的技能)
		int				level;					// 技能的级别
		float			probability;			// 该条件技能出现的概率
	} ;
template<>
void print_data(CString name, skill_hp50** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id_skill = %d \t level = %d \r\n"), (*ptr)->id_skill, (*ptr)->level);
		(*ptr)++;
		m_strItemInfo += str;
	}
}
	struct skill_hp25
	{
		unsigned int	id_skill;				// 怪物的条件技能类型ID	(HP<25%时使用的技能)
		int				level;					// 技能的级别
		float			probability;			// 该条件技能出现的概率
	};
template<>
void print_data(CString name, skill_hp25** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id_skill = %d \t level = %d \r\n"), (*ptr)->id_skill, (*ptr)->level);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct skill
	{
		unsigned int	id_skill;				// 怪物的普通技能类型ID
		int				level;					// 技能的级别
	};	
template<>
void print_data(CString name, skill** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id_skill = %d \t level = %d \r\n"), (*ptr)->id_skill, (*ptr)->level);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct task
	{
		unsigned int	id_task;				// 可以处理的任务列表
		struct taks_matter
		{
		unsigned int	id_matter;				// 任务相关物品id
		int				num_matter;				// 任务相关物品数目
		} tasks_matters[4];
	};
template<>
void print_data(CString name, task** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id_task = %d \r\n \
						tasks_matters0: id_matter = %d \t num_matter = %d \r\n \
						tasks_matters1: id_matter = %d \t num_matter = %d \r\n \
						tasks_matters2: id_matter = %d \t num_matter = %d \r\n \
						tasks_matters3: id_matter = %d \t num_matter = %d \r\n"), 
						(*ptr)->id_task, 
						(*ptr)->tasks_matters[0].id_matter, (*ptr)->tasks_matters[0].num_matter,
						(*ptr)->tasks_matters[1].id_matter, (*ptr)->tasks_matters[1].num_matter,
						(*ptr)->tasks_matters[2].id_matter, (*ptr)->tasks_matters[2].num_matter,
						(*ptr)->tasks_matters[3].id_matter, (*ptr)->tasks_matters[3].num_matter);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct target
	{
		float		x;							// 目标点位置x坐标
		float		y;							// 目标点位置y坐标
		float		z;							// 目标点位置z坐标
		int			fee;						// 费用
	};
template<>
void print_data(CString name, target** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("x = %f \t y = %f \t z = %f \t fee = %d \r\n"),
				(*ptr)->x, (*ptr)->y, (*ptr)->z, (*ptr)->fee);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct route
	{
		unsigned int	id;						// 路线ID
		unsigned int	fee;					// 费用

	};
template<>
void print_data(CString name, route** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id = %d \t fee = %d \r\n"), (*ptr)->id, (*ptr)->fee);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct material
	{
		unsigned int	id;						// 原料id
		int				num;					// 原料所需数目

	};

	struct tasklist
	{
		unsigned int	id;						// 任务id
		float			probability;			// 概率
	};

template<>
void print_data(CString name, material** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id = %d \t fee = %d \r\n"), (*ptr)->id, (*ptr)->num);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

template<>
void print_data(CString name, tasklist** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("id = %d \t probability = %f \r\n"), (*ptr)->id, (*ptr)->probability);
		(*ptr)++;
		m_strItemInfo += str;
	}
}

	struct level_diff_adjust_t
	{
		int			level_diff;				// Player和怪物的级别差(怪物-Player)(当级别差为99999时，表示此条失效)
		float		adjust_exp;				// 对杀怪获得经验的修正系数(0.0~1.0)
		float		adjust_sp;				// 对杀怪获得SP的修正系数(0.0~1.0)
		float		adjust_money;			// 对杀怪获得金钱掉落数目的修正系数(0.0~1.0)
		float		adjust_matter;			// 对杀怪掉落物品的概率的修正(0.0~1.0)
		float		adjust_attack;
	};  										// 级别差对杀怪计算的修正
template<>
void print_data(CString name, level_diff_adjust_t** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("level_diff = %d \t adjust_exp = %f \t adjust_sp = %f \t   \
			adjust_money = %f \t adjust_matter = %f adjust_attack = %f\r\n"),                      \
			(*ptr)->level_diff, (*ptr)->adjust_exp, (*ptr)->adjust_sp, (*ptr)->adjust_money,
			(*ptr)->adjust_matter,(*ptr)->adjust_attack);
		(*ptr)++;
		m_strItemInfo += str;
	}
}	

	struct team_adjust_t
	{
		float		adjust_exp;				// 对杀怪获得经验的修正系数(0.0~1.0)
		float		adjust_sp;				// 对杀怪获得SP的修正系数(0.0~1.0)
	};										// 组队杀怪时的计算修正
template<>
void print_data(CString name, team_adjust_t** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("adjust_exp = %f \t adjust_sp = %f \r\n"), 
			(*ptr)->adjust_exp, (*ptr)->adjust_sp);
		(*ptr)++;
		m_strItemInfo += str;
	}
}	

	struct action_weapon_suffix_t
	{
		char suffix[32];
	};
template<>
void print_data(CString name, action_weapon_suffix_t** ptr, size_t count)
{
	bool numsuffix = count > 1;
	CString str, strNo;
	for(size_t i=0; i<count; i++)
	{
		if(numsuffix)	strNo.Format(_T("%d"), i);
		str = _T(name)+strNo+_T(" :  ");
		m_strItemInfo += str;
		str.Format(_T("suffix = %s \r\n"), (*ptr)->suffix);
		(*ptr)++;
		m_strItemInfo += str;
	}
}	

	// Generated message map functions
	//{{AFX_MSG(CPrintDataDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboIdspace();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinId(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditId();
	afx_msg void OnButtonBatchTest();
	afx_msg void OnSelchangeComboType();
	afx_msg void OnSelchangeComboGenerateStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTGENERATEITEMDLG_H__B75BF659_55B8_442F_ABB2_2A145E69EA84__INCLUDED_)
