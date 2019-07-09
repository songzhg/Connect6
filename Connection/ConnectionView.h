#pragma once

#include "ConnectionDoc.h"
#include "AIFactory.h"
#include "SettingDialog.h"
#include "atltypes.h"

enum PKMode
{
	P2P,	
	P2C,
	C2P,
	C2C
};

enum State
{
	INIT,
	RUNNING,
	CPUTHINKING,
	PAUSE,
	STOP
};

class CConnectionView : public CView
{
protected: // 仅从序列化创建
	CConnectionView();
	DECLARE_DYNCREATE(CConnectionView)

// 属性
public:
	CConnectionDoc* GetDocument() const;

// 操讈E
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CConnectionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	HBRUSH m_hBrush;
	CBitmap m_bitmapBmps[6];
	State m_iState;
	ChessPlaySide m_cpcChessPlayColor;
	int m_iBlackDepth;
	int m_iWhiteDepth;
	int m_iBlackThreshhold;
	int m_iWhiteThreshhold;
	int m_iBlackTimeout;
	int m_iWhiteTimeout;
	CString m_strAIName;
	CString m_strAIName2;
	AbstractAI* m_pAbstractAI;
	AbstractAI* m_pAbstractAI2;
	list<OneMovePlayInfo> m_listMoves;
	char m_arrayBoard[19][19];
	PKMode m_pkMode;
	SettingDialog m_dlgSetting;
public:
	afx_msg void OnFileNew();
	afx_msg void OnFileSetting();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFileOpen();
	afx_msg void OnFileStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	void initBoard(void);
	void newGame(void);
	void getAI(AbstractAI** ppAbstractAI, CString strName);
	void computerThink(ChessPlaySide chessPlayColor);
private:
	bool isWin(void);
	static UINT thinkThread(LPVOID lpParam);
private:
	CRect m_rectTimer1;
	CRect m_rectTimer2;
	long m_tsCPU1_UsedTime;
	long m_tsCPU2_UsedTime;
	CWinThread* m_thread1;
	CWinThread* m_thread2;
	CStdioFile* m_pChessManualFile;
	void closeChessManualFile(void);
	void writeChessManual(int x, int y);
	BOOL createNewChessManulFile(CStdioFile** sf);
	CString getModuleDirectory(void);
	CString getChessManualDirectory(void);
	RoadInfo m_riRoadInfo;
	static const int m_iArrayRoads[924][6];
	static const R_RoadInfo m_riArrayR_Roads[361];
	static const char staticBitOpNumber[8];	
public:
	void startGame(BOOL isNewGame);
	afx_msg void OnFileContinue();
	afx_msg void OnFilePause();
	afx_msg void OnChessBack();
private:
	void gotoPause(void);
public:
	afx_msg void OnUpdateShowNumber(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShowLine(CCmdUI *pCmdUI);
private:
	int m_iShowNumber;
	int m_iShowLine;
public:
	afx_msg void OnShowLine();
	afx_msg void OnShowNumber();
private:
	void setRoadInfos(int x, int y, ChessPlaySide cps);
	void eraseRoadInfos(int x, int y, ChessPlaySide cps);
	int getConnectCount(char shape);
	void initRoadInfo(void);
public:
	afx_msg void OnReserve();
};

#ifndef _DEBUG  // SXU_Connect6View.cpp 中的调试版本
inline CConnectionDoc* CConnectionView::GetDocument() const
   { return reinterpret_cast<CConnectionDoc*>(m_pDocument); }
#endif

