// SXU_Connect6.cpp : ����Ӧ�ó��������Ϊ��
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
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


CConnectionApp theApp;


// CConnectionApp ��ʼ��

BOOL CConnectionApp::InitInstance()
{
	
	CWinApp::InitInstance();
	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);

	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4); 

	CString path; 
    GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos) + _T("\\chessmanual");
	CString strFolderPath = path;
	//�ж�·���Ƿ����
	if (!PathIsDirectory(strFolderPath)) 
	{
		if (!CreateDirectory(strFolderPath, NULL))
		{
			CString str;
			str.LoadString(IDS_CREATECHESSMANUAL_FAIL);
			if (AfxMessageBox(_T("�������׼�¼�ļ���ʧ�ܣ��Ƿ����"), MB_YESNO) == IDNO)
			{
				return FALSE; 
			}
		}
	}  


	// ע��Ӧ�ó�����ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
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

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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
