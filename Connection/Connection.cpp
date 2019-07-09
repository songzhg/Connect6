// SXU_Connect6.cpp : 定义应用程序的类行为。
//
#include "stdafx.h"
#include "afxwinappex.h"
#include "Connection.h"
#include "MainFrm.h"
#include "ConnectionDoc.h"
#include "ConnectionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CConnectionApp

BEGIN_MESSAGE_MAP(CConnectionApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CConnectionApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


CConnectionApp::CConnectionApp()
{
	// 将所有重要的初始化放置在 InitInstance 中
}


CConnectionApp theApp;


// CConnectionApp 初始化

BOOL CConnectionApp::InitInstance()
{
	
	CWinApp::InitInstance();
	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);

	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4); 

	CString path; 
    GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos) + _T("\\chessmanual");
	CString strFolderPath = path;
	//判断路径是否存在
	if (!PathIsDirectory(strFolderPath)) 
	{
		if (!CreateDirectory(strFolderPath, NULL))
		{
			CString str;
			str.LoadString(IDS_CREATECHESSMANUAL_FAIL);
			if (AfxMessageBox(_T("创建棋谱记录文件夹失败！是否继续"), MB_YESNO) == IDNO)
			{
				return FALSE; 
			}
		}
	}  


	// 注册应用程序的文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CConnectionDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CConnectionView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


void CConnectionApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
