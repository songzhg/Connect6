// SettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Connection.h"
#include "SettingDialog.h"
#include "iostream"

using namespace std;

// SettingDialog 对话縼E
void Tokenize(CString s, CString delimits, CStringList* pList, BOOL trim=FALSE, CString nullSubst=_T(""))
{
 ASSERT( !s.IsEmpty() && !delimits.IsEmpty() );

 s += delimits[0];
 for( long index=-1; (index=s.FindOneOf((LPCTSTR)delimits))!=-1; )
 {
  if(index != 0) pList->AddTail( s.Left(index) );
  else if(!trim) pList->AddTail(nullSubst);
  s = s.Right(s.GetLength()-index-1);
 }
}

IMPLEMENT_DYNAMIC(SettingDialog, CDialog)

SettingDialog::SettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SettingDialog::IDD, pParent)
	, m_iBlackTimeout(0)
	, m_iWhiteTimeout(0)
{
	m_iPlayer1_Type = 1;
	m_iPlayer2_Type = 0;
	m_iPlayer1_AI_Index = 0;
	m_iPlayer2_AI_Index = 0;
	m_iBlackDepth = 0;
	m_iWhiteDepth = 0;
	m_iBlackTimeout = 200;
	m_iWhiteTimeout = 200;
	m_iPlayer1_SearchBranchThreshhold = 5;
	m_iPlayer2_SearchBranchThreshhold = 5;
	CString path; 
    GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos); 
	//wcout << path.GetString() << endl;
	WCHAR ainames[MAX_PATH];
	GetPrivateProfileString(_T("Connection"), _T("AI_Class_Names"), _T("Connection"), ainames, MAX_PATH, path + _T("\\Connection.ini"));
	CString str(ainames);
	Tokenize(str, _T("|"), &m_listStringAINames);
	GetPrivateProfileString(_T("Connection"), _T("AI_SearchBranch_Threshholds"), _T("5"), ainames, MAX_PATH, path + _T("\\Connection.ini"));
	CString str2(ainames);
	CStringList csl;
	Tokenize(str2, _T("|"), &csl);
	m_iSearchBranchThreshholdsCount = csl.GetCount();
	for (int i = 0; i < m_iSearchBranchThreshholdsCount; i++)
	{
		POSITION pos = csl.FindIndex(i);
		CString tstr = csl.GetAt(pos);
		m_iSearchBranchThreshholds[i] = _ttoi(tstr);
	}
}

SettingDialog::~SettingDialog()
{
}

void SettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_BLACK_DEPTH, m_sliderBlackDepth);
	DDX_Control(pDX, IDC_SLIDER_WHITE_DEPTH, m_sliderWhiteDepth);
	DDX_Control(pDX, IDC_EDIT_BLACK_SEARCH_TIMEOUT2, m_editBlackTimeout);
	DDX_Control(pDX, IDC_EDIT_WHITE_SEARCH_TIMEOUT, m_editWhiteTimeout);
	DDX_Text(pDX, IDC_EDIT_BLACK_SEARCH_TIMEOUT2, m_iBlackTimeout);
	DDV_MinMaxInt(pDX, m_iBlackTimeout, 0, 600);
	DDX_Text(pDX, IDC_EDIT_WHITE_SEARCH_TIMEOUT, m_iWhiteTimeout);
	DDV_MinMaxInt(pDX, m_iWhiteTimeout, 0, 600);
}


BEGIN_MESSAGE_MAP(SettingDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_Player1_Type, &SettingDialog::OnCbnSelchangeComboPlayer1Type)
	ON_CBN_SELCHANGE(IDC_COMBO_Player2_Type, &SettingDialog::OnCbnSelchangeComboPlayer2Type)
	ON_BN_CLICKED(IDOK, &SettingDialog::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BLACK_DEPTH, &SettingDialog::OnNMCustomdrawSliderBlackDepth)
END_MESSAGE_MAP()


BOOL SettingDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
		str.LoadString(IDS_PERSON);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_Type))->InsertString(0, str);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_Type))->InsertString(0, str);
		str.LoadString(IDS_COMPUTER);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_Type))->InsertString(1, str);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_Type))->InsertString(1, str);

		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_Type))->SetCurSel(m_iPlayer1_Type);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_Type))->SetCurSel(m_iPlayer2_Type);

		m_editBlackTimeout.SetLimitText(3);
		m_editWhiteTimeout.SetLimitText(3);

		
		for (int i = 0; i < m_listStringAINames.GetCount(); i++)
		{
			POSITION pos = m_listStringAINames.FindIndex(i);       
			CString tstr = m_listStringAINames.GetAt(pos);
			((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->InsertString(i, tstr);
			((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->InsertString(i, tstr);
		}
		for (int i = 0; i < m_iSearchBranchThreshholdsCount; i++)
		{
			CString tstr;
			tstr.Format(_T("%d"), m_iSearchBranchThreshholds[i]);
			((CComboBox*)GetDlgItem(IDC_COMBO_Player1_SearchBranchThreshhold))->InsertString(i, tstr);
			((CComboBox*)GetDlgItem(IDC_COMBO_Player2_SearchBranchThreshhold))->InsertString(i, tstr);
		}

		BOOL b1 = m_iPlayer1_Type == 0 ? FALSE : TRUE;
		BOOL b2 = m_iPlayer2_Type == 0 ? FALSE : TRUE;
		
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->EnableWindow(b1);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_SearchBranchThreshhold))->EnableWindow(b1);
		m_editBlackTimeout.EnableWindow(b1);
		this->m_sliderBlackDepth.EnableWindow(b1);
		if (b1)
		{
			((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->SetCurSel(m_iPlayer1_AI_Index);
			((CComboBox*)GetDlgItem(IDC_COMBO_Player1_SearchBranchThreshhold))->SetCurSel(m_iPlayer1_SearchBranchThreshhold);
		}
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->EnableWindow(b2);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_SearchBranchThreshhold))->EnableWindow(b2);
		m_editWhiteTimeout.EnableWindow(b2);
		this->m_sliderWhiteDepth.EnableWindow(b2);
		if (b2)
		{
			((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->SetCurSel(m_iPlayer2_AI_Index);
			((CComboBox*)GetDlgItem(IDC_COMBO_Player2_SearchBranchThreshhold))->SetCurSel(m_iPlayer2_SearchBranchThreshhold);
		}

		m_sliderBlackDepth.SetRange(1, 5);
		m_sliderBlackDepth.SetTicFreq(1);
		m_sliderBlackDepth.SetPos(m_iBlackDepth);
		m_sliderWhiteDepth.SetRange(1, 5);
		m_sliderWhiteDepth.SetTicFreq(1);
		m_sliderWhiteDepth.SetPos(m_iWhiteDepth);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void SettingDialog::OnCbnSelchangeComboPlayer1Type()
{
	int i = ((CComboBox*)GetDlgItem(IDC_COMBO_Player1_Type))->GetCurSel();
	if (i == 0)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->EnableWindow(FALSE);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_SearchBranchThreshhold))->EnableWindow(FALSE);
		m_editBlackTimeout.EnableWindow(FALSE);
		this->m_sliderBlackDepth.EnableWindow(FALSE);
	}
	else if (i == 1)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->EnableWindow(TRUE);		
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->SetCurSel(0);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_SearchBranchThreshhold))->EnableWindow(TRUE);		
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_SearchBranchThreshhold))->SetCurSel(m_iPlayer1_SearchBranchThreshhold);
		m_editBlackTimeout.EnableWindow(TRUE);
		this->m_sliderBlackDepth.EnableWindow(TRUE);
	}
}

void SettingDialog::OnCbnSelchangeComboPlayer2Type()
{

	int i = ((CComboBox*)GetDlgItem(IDC_COMBO_Player2_Type))->GetCurSel();
	if (i == 0)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->EnableWindow(FALSE);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_SearchBranchThreshhold))->EnableWindow(FALSE);
		m_editWhiteTimeout.EnableWindow(FALSE);
		this->m_sliderWhiteDepth.EnableWindow(FALSE);
	}
	else if (i == 1)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->SetCurSel(0);		
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_SearchBranchThreshhold))->EnableWindow(TRUE);		
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_SearchBranchThreshhold))->SetCurSel(m_iPlayer2_SearchBranchThreshhold);
		m_editWhiteTimeout.EnableWindow(TRUE);
		this->m_sliderWhiteDepth.EnableWindow(TRUE);
	}
}

void SettingDialog::OnBnClickedOk()
{
	m_iPlayer1_Type = ((CComboBox*)GetDlgItem(IDC_COMBO_Player1_Type))->GetCurSel();
	m_iPlayer2_Type = ((CComboBox*)GetDlgItem(IDC_COMBO_Player2_Type))->GetCurSel();
	m_iPlayer1_AI_Index = ((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->GetCurSel();
	m_iPlayer2_AI_Index = ((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->GetCurSel();
	m_iPlayer1_SearchBranchThreshhold = ((CComboBox*)GetDlgItem(IDC_COMBO_Player1_SearchBranchThreshhold))->GetCurSel();
	m_iPlayer2_SearchBranchThreshhold = ((CComboBox*)GetDlgItem(IDC_COMBO_Player2_SearchBranchThreshhold))->GetCurSel();
	m_iBlackDepth = m_sliderBlackDepth.GetPos();
	m_iWhiteDepth = m_sliderWhiteDepth.GetPos();
	if(m_iPlayer1_AI_Index != -1)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_Player1_AI))->GetLBText(m_iPlayer1_AI_Index, m_strPlayer1_AI_Name);
	}
	if(m_iPlayer2_AI_Index != -1)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_Player2_AI))->GetLBText(m_iPlayer2_AI_Index, m_strPlayer2_AI_Name);
	}
	UpdateData();
	OnOK();
}


void SettingDialog::OnNMCustomdrawSliderBlackDepth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
