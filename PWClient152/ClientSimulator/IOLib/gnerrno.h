
#ifndef __GNET_ERRCODE_H
#define __GNET_ERRCODE_H

namespace GNET
{
	enum ErrCode {
		ERR_SUCCESS = 0,			// �ɹ�
		ERR_TOBECONTINUE = 1,		// �ɹ������һ��и�������δ�����꣬Ŀǰδ��

		ERR_INVALID_ACCOUNT = 2,	// �ʺŲ�����
		ERR_INVALID_PASSWORD = 3,	// �������
		ERR_TIMEOUT = 4,			// ��ʱ
		ERR_INVALID_ARGUMENT = 5,	// ��������
		ERR_FRIEND_SYNCHRONIZE = 6,	// ������Ϣ���浽���ݿ�ʱ�޷�ͬ��
		ERR_SERVERNOTSUPPLY = 7,	// �÷�������֧�ָ�����
		ERR_COMMUNICATION=8,		// ����ͨѶ����
		ERR_ACCOUNTLOCKED=9,		// ����ظ���½����ǰ�û���һ����½���ڱ�������������״̬
		ERR_MULTILOGIN	=10,		// ����ظ���½�����û�ѡ���Զ�����
		// keyexchange		
		ERR_INVALID_NONCE,      //��Ч��nonceֵ
		//add friend user
		ERR_ADDFRD_REFUSE,		//�ܾ���Ϊ����
		ERR_ADDFRD_AGREE ,		//ͬ���Ϊ����
		ERR_ADDFRD_AGREEANDADD,	//ͬ�Ⲣϣ�����Է���Ϊ����
		// deliver use
		ERR_DELIVER_SEND = 1001,	// ת��ʧ��
		ERR_DELIVER_TIMEOUT = 1002,	// ת����ʱ
		//player login
		ERR_LOGINFAIL	=	2001,	//��½��Ϸʧ��
		ERR_KICKOUT		=	2002,	//��������
		ERR_CREATEROLE	=	2003,	//������ɫʧ��
		ERR_DELETEROLE	=	2004,	//ɾ����ɫʧ��
		ERR_ROLELIST	=	2005	//��ý�ɫ�б�ʧ��
	};
	
};

#endif



