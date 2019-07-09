
// SXU_Connect6.h : SXU_Connect6 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CConnectionApp:

class CConnectionApp : public CWinApp
{
public:
	CConnectionApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CConnectionApp theApp;
