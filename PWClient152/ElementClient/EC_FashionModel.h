// Filename	: EC_FashionModel.h
// Creator	: Xu Wenbin
// Date		: 2014/5/30

#ifndef _ELEMENTCLIENT_EC_FASHIONMODEL_H_
#define _ELEMENTCLIENT_EC_FASHIONMODEL_H_

#include <ABaseDef.h>
#include "EC_IvtrTypes.h"

//	ǰ������
class CECLoginPlayer;
struct FASHION_ESSENCE;

class CECComputeMoveParamFunction;

//	class CECFashionModel
class CECFashionModel
{	
public:
	CECFashionModel();
	~CECFashionModel();

	//	����
	void Tick();
	
	//	����
	bool CreatePlayer(int profession, int gender);

	//	��ѯ�ӿ�
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

	//	�޸�
	bool Fit(int tid);
	bool ChangeFashionColor(int equipSlot, unsigned short newColor);
	void ClearFashion();
	void ClearFashion(int equipSlot);
	void SetMoveParamFunction(CECComputeMoveParamFunction * pFunction);

	//	�������
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
	//	����
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
	
	CECLoginPlayer	*	m_pPlayer;						//	���¼� Player
	int					m_aEquips[SIZE_ALL_EQUIPIVTR];	//	���¼� Player ��ǰʱװװ��

	int					m_nAngle;				//	��ɫ��ת�Ƕ�
	int					m_nCameraDistanceChange;//	Զ������
	int					m_nCameraHeightChange;	//	�߶ȵ���
	int					m_nCameraLeftRightChange;//	���ҵ���
	
	int					m_mouseDownPositionX;	//	ģ����ʾ�ؼ���������Ҽ�����ʱ��λ��
	int					m_mouseDownPositionY;
	bool				m_bCharLButtonDown;		//	ģ����ʾ�������λ��
	bool				m_bCharRButtonDown;		//	ģ����ʾ�Ҽ�����λ��
	int					m_nAngleAtButtonDown;	//	��������ʱ��ɫ��ת�Ƕȣ��������������
	int					m_nCameraHeightChangeAtButtonDown;	//	��������ʱ����߶ȣ��������������
	int					m_nCameraLeftRightChangeAtButtonDown;//	��������ʱ���X��λ�ã��������������

	CECComputeMoveParamFunction * m_pComputeMoveFunction;
};

// ��������ƶ�����λ�Ʋ�������
class CECComputeMoveParamFunction {
public:
	virtual void ComputeMoveParam(int screenDx, int screenDy, int &cameraLeftRightChange, int &cameraHeightChange) = 0;
	virtual ~CECComputeMoveParamFunction() {}
};

// ��������ƶ�����λ�Ʋ�����ͨ���࣬��EC_Player.h��PlayerRenderDemonstration����ƥ��
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