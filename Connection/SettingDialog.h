#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Resource.h"


// SettingDialog

class SettingDialog : public CDialog
{
	DECLARE_DYNAMIC(SettingDialog)

public:
	SettingDialog(CWnd* pParent = NULL);  
	virtual ~SettingDialog();

// 对话框数据
	enum { IDD = IDD_SETTINGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboPlayer1Type();
	afx_msg void OnCbnSelchangeComboPlayer2Type();
	afx_msg void OnBnClickedOk();
	int m_iPlayer1_Type;
	int m_iPlayer2_Type;
	int m_iPlayer1_AI_Index;
	int m_iPlayer2_AI_Index;
	int m_iBlackDepth;
	int m_iWhiteDepth;
	int m_iPlayer1_SearchBranchThreshhold;
	int m_iPlayer2_SearchBranchThreshhold;
	CString m_strPlayer1_AI_Name;
	CString m_strPlayer2_AI_Name;
	CStringList m_listStringAINames;	
	CSliderCtrl m_sliderBlackDepth;
	CSliderCtrl m_sliderWhiteDepth;
	int m_iSearchBranchThreshholdsCount;
	int m_iSearchBranchThreshholds[100];
private:
	BOOL isInit;
public:
	CEdit m_editBlackTimeout;
	CEdit m_editWhiteTimeout;
	int m_iBlackTimeout;
	int m_iWhiteTimeout;
	afx_msg void OnNMCustomdrawSliderBlackDepth(NMHDR *pNMHDR, LRESULT *pResult);
};
