// Filename	: EC_FashionModel.h
// Creator	: Xu Wenbin
// Date		: 2014/5/30

#ifndef _ELEMENTCLIENT_EC_FASHIONMODEL_H_
#define _ELEMENTCLIENT_EC_FASHIONMODEL_H_

#include <ABaseDef.h>
#include "EC_IvtrTypes.h"

//	前置声明
class CECLoginPlayer;
struct FASHION_ESSENCE;

class CECComputeMoveParamFunction;

//	class CECFashionModel
class CECFashionModel
{	
public:
	CECFashionModel();
	~CECFashionModel();

	//	更新
	void Tick();
	
	//	创建
	bool CreatePlayer(int profession, int gender);

	//	查询接口
	int	 GetProfession()const;
	int  GetGender()const;
	bool CanFit(int tid)const;
	bool CanChangeColor()const;
	bool GetFashionColor(int equipSlot, unsigned short &color)const;
	bool GetFashionBestColor(int equipSlot, unsigned short &color)const;
	bool HasFashionOn()const;
	bool IsCameraDefault()const;

	CECLoginPlayer * GetPlayer();
	
	static bool FindBestColorFor(int tid, unsigned short &bestColor);
	static int  GetFashionEquipSlot(int tid);

	//	修改
	bool Fit(int tid);
	bool ChangeFashionColor(int equipSlot, unsigned short newColor);
	void ClearFashion();
	void ClearFashion(int equipSlot);
	void SetMoveParamFunction(CECComputeMoveParamFunction * pFunction);

	//	相机操纵
	bool OnEventLButtonDown(int x, int y);
	bool OnEventLButtonUp(int x, int y);
	bool OnEventRButtonDown(int x, int y);
	bool OnEventRButtonUp(int x, int y);
	bool OnEventMouseMove(int x, int y);
	bool OnEventMouseWheel(int zDelta);

	void ResetCamera();
	int	 GetAngle()const;
	int  ComposeCameraParameter()const;
	
private:
	//	禁用
	CECFashionModel(const CECFashionModel &);
	CECFashionModel & operator=(const CECFashionModel &);

	void ClampCamera();
	void GenerateEquipsFromHostPlayer(int aEquips[SIZE_ALL_EQUIPIVTR])const;
	bool IsLikeHostPlayer(int profession, int gender)const;
	bool IsLikeHostPlayer()const;
	static DWORD GetColoredFashion(unsigned short color, int id);
	static int	 GetIDFromColoredFashion(DWORD coloredID);
	static unsigned short GetColorFromColoredFashion(DWORD coloredID);
	static bool	 CanShowFashionWeapon(int fashionWeaponID, int weaponID);
	static bool  GetFashionWeaponType(int tid, unsigned int &weaponType);
	static bool  GetWeaponType(int tid, unsigned int &weaponType);
	static bool  IsFashionWeapon(const FASHION_ESSENCE *pEssence);
	
	CECLoginPlayer	*	m_pPlayer;						//	试衣间 Player
	int					m_aEquips[SIZE_ALL_EQUIPIVTR];	//	试衣间 Player 当前时装装备

	int					m_nAngle;				//	角色旋转角度
	int					m_nCameraDistanceChange;//	远近调整
	int					m_nCameraHeightChange;	//	高度调整
	int					m_nCameraLeftRightChange;//	左右调整
	
	int					m_mouseDownPositionX;	//	模特显示控件上左键、右键按下时的位置
	int					m_mouseDownPositionY;
	bool				m_bCharLButtonDown;		//	模特显示左键摁下位置
	bool				m_bCharRButtonDown;		//	模特显示右键摁下位置
	int					m_nAngleAtButtonDown;	//	鼠标键按下时角色旋转角度（用于相机交互）
	int					m_nCameraHeightChangeAtButtonDown;	//	鼠标键按下时相机高度（用于相机交互）
	int					m_nCameraLeftRightChangeAtButtonDown;//	鼠标键按下时相机X轴位置（用于相机交互）

	CECComputeMoveParamFunction * m_pComputeMoveFunction;
};

// 根据鼠标移动计算位移参数的类
class CECComputeMoveParamFunction {
public:
	virtual void ComputeMoveParam(int screenDx, int screenDy, int &cameraLeftRightChange, int &cameraHeightChange) = 0;
	virtual ~CECComputeMoveParamFunction() {}
};

// 根据鼠标移动计算位移参数的通用类，与EC_Player.h的PlayerRenderDemonstration函数匹配
class CECGeneralComputeMoveParamFunction : public CECComputeMoveParamFunction {
public:
	CECGeneralComputeMoveParamFunction(int viewportWidth, int viewportHeight, bool isMale, CECFashionModel* pModel);
	void ComputeMoveParam(int screenDx, int screenDy, int &cameraLeftRightChange, int &cameraHeightChange);
private:
	int m_iViewportWidth;
	int m_iViewportHeight;
	CECFashionModel* m_pModel;
	bool m_isMale;
};

#endif	//	_ELEMENTCLIENT_EC_FASHIONMODEL_H_