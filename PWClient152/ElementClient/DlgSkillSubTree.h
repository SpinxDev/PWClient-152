// Filename	: DlgSkillSubTree.h
// Creator	: zhangyitian
// Date		: 2014/7/2

#ifndef _ELEMENTCLIENT_DLGSKILLSUBTREE_H_
#define _ELEMENTCLIENT_DLGSKILLSUBTREE_H_

#include "DlgBase.h"
#include "APoint.h"

class AUIImagePicture;
class CDlgSkillSubList;
class FWFlatCollector;

// ��ʾ���ܸ��ǹ�ϵ���������ڼ��ܶԻ����Ҳ�

class CDlgSkillSubTree : public CDlgBase {

	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
	
public:
	class SkillTreeNode {
	public:
		SkillTreeNode(int skillID, int xPos, int yPos, int imgWidth, int imgHeight, int zoneLeft, int zoneRight, int verticalDistanceToChild);
		int GetSkillID() const;
		int GetCenterX() const;
		int GetCenterY() const;
		int GetTop() const;
		int GetBottom() const;
		int GetLeft() const;
		int GetRight() const;
		SkillTreeNode GetChild(int index) const;
		int GetChildrenCount() const;
		int GetMidYCoordWithChild() const;
	private:
		int m_skillID;
		int m_xPos;
		int m_yPos;
		int m_imgWidth;
		int m_imgHeight;
		int m_zoneLeft;
		int m_zoneRight;
		int m_verticalDistanceToChild;
		int m_childrenCount;

	};

public:
	CDlgSkillSubTree();
	virtual ~CDlgSkillSubTree();
	virtual bool Render();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
	virtual bool Release();

	bool OnChangeLayoutBegin();			// ׼�����Ľ���

	void OnCommand_TreeClose(const char *pszCommand);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void ShowSkillTree(int rootID);

protected:
	virtual bool OnInitDialog();

private:
	abase::vector<AUIImagePicture*> m_imgPics;		// ���п��õļ���ͼ��
	int m_usedImgPicCount;							// ��ǰ���õļ���ͼ�����
	AUIImagePicture* AllocImgPic();					// ����һ�����õ�ͼ��

	// �ݹ����һ�ż�����
	void DrawSkillTree(const SkillTreeNode& rootNode);

	void SetImagePicture(int skillID, AUIImagePicture* imgPic);

	const char* GetGfxName(int skillID);

private:
	AUIImagePicture* m_templateImgPic;		// ͼ��ؼ���ģ��
	int m_imgPicWidth;						// ͼ��ؼ��Ŀ��
	int m_imgPicHeight;						// ͼ��ؼ��ĸ߶�
	
	FWFlatCollector * m_pFlatCollector;

	std::vector<std::pair<APointI, APointI> > m_linesToDraw;	// ��Ҫ������

	int m_rootID;					// ���ĸ���id
};

#endif