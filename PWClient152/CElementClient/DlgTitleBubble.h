// Filename	: DlgTitleBubble.h
// Creator	: Han Guanghui
// Date		: 2013/5/16

#ifndef _DLGTITLEBUBBLE_H_ 
#define _DLGTITLEBUBBLE_H_

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include <deque>
class CDlgTitleBubble : public CDlgBase
{
public:
	CDlgTitleBubble();
	virtual ~CDlgTitleBubble();

	void				AddTitle(unsigned short id, int expiretime);

protected:
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	virtual void		OnTick();
	void				Update();
	void				ChangeState(int state, DWORD dwCount);
	std::deque<unsigned short> m_TitlesToBubble;
	enum{
		STATE_SHOW,
		STATE_FADE,
		STATE_HIDE,
		STATE_CLOSE,
	};
	int					m_iState;
	unsigned short		m_BubbleTitleID;
	DWORD				m_dwCount;
	PAUIOBJECT			m_pLabText;
	PAUIIMAGEPICTURE	m_pImgGfx;
};

#endif // _DLGTITLEBUBBLE_H_

