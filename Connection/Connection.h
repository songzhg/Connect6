
// SXU_Connect6.h : SXU_Connect6 Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CConnectionApp:

class CConnectionApp : public CWinApp
{
public:
	CConnectionApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CConnectionApp theApp;
