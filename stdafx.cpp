
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Es.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


//�ӳ���Ϣ
//����SPD-3�еĺ���

void DelayMessage() 
{
	MSG message;
	while(::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}
