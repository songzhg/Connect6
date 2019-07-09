// SXU_Connect6View.cpp : CConnectionView 类的实现
//

#include "stdafx.h"
#include "Connection.h"
#include "ConnectionDoc.h"
#include "ConnectionView.h"
#include "OneMovePlayInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CConnectionView, CView)

BEGIN_MESSAGE_MAP(CConnectionView, CView)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_NEW, &CConnectionView::OnFileNew)
	ON_COMMAND(ID_FILE_SETTING, &CConnectionView::OnFileSetting)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_FILE_STOP, &CConnectionView::OnFileStop)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_OPEN, &CConnectionView::OnFileOpen)
	ON_COMMAND(ID_FILE_CONTINUE, &CConnectionView::OnFileContinue)
	ON_COMMAND(ID_FILE_PAUSE, &CConnectionView::OnFilePause)
	ON_COMMAND(ID_CHESS_BACK, &CConnectionView::OnChessBack)
	ON_UPDATE_COMMAND_UI(ID_SHOW_NUMBER, &CConnectionView::OnUpdateShowNumber)
	ON_UPDATE_COMMAND_UI(ID_SHOW_LINE, &CConnectionView::OnUpdateShowLine)
	ON_COMMAND(ID_SHOW_LINE, &CConnectionView::OnShowLine)
	ON_COMMAND(ID_SHOW_NUMBER, &CConnectionView::OnShowNumber)
	ON_COMMAND(ID_FILE_SAVE, &CConnectionView::OnReserve)
END_MESSAGE_MAP()


CConnectionView::CConnectionView()
: m_iShowNumber(0)
, m_iShowLine(0)
{
	//m_pAbstractAI = NULL;
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP1);
	m_hBrush = CreatePatternBrush((HBITMAP)bmp);
	bmp.DeleteObject();
	m_bitmapBmps[0].LoadBitmap(IDB_BITMAP2);
	m_bitmapBmps[1].LoadBitmap(IDB_BITMAP3);
	m_bitmapBmps[2].LoadBitmap(IDB_BITMAP4);
	m_bitmapBmps[3].LoadBitmap(IDB_BITMAP5);
	m_bitmapBmps[4].LoadBitmap(IDB_BITMAP6);
	m_bitmapBmps[5].LoadBitmap(IDB_BITMAP7);
	initBoard();
	m_iState = INIT;
	m_cpcChessPlayColor = SIDE_BLACK;
	m_listMoves.clear();
	m_pkMode = P2P;
	m_strAIName = "Connection";
	m_strAIName2 = "Connection";
	m_pAbstractAI = NULL;
	m_pAbstractAI2 = NULL;
	m_thread1 = NULL;
	m_thread2 = NULL;
	m_pChessManualFile = NULL;
	m_tsCPU1_UsedTime = 0;
	m_tsCPU2_UsedTime = 0;
	m_rectTimer1.left = 60;
	m_rectTimer1.top = 620;
	m_rectTimer1.right = 220;
	m_rectTimer1.bottom = 670;
	m_rectTimer2.left = 60;
	m_rectTimer2.top = 620;
	m_rectTimer2.right = 220;
	m_rectTimer2.bottom = 670;
	m_iBlackThreshhold = 550;
	m_iWhiteThreshhold = 550;
}

CConnectionView::~CConnectionView()
{
	fclose(stdout);
	FreeConsole();

	if (m_pAbstractAI != NULL)
	{
		delete m_pAbstractAI;
		m_pAbstractAI = NULL;
	}
	if (m_pAbstractAI2 != NULL)
	{
		delete m_pAbstractAI2;
		m_pAbstractAI2 = NULL;
	}
	closeChessManualFile();
	for (int i = 0; i < 6; i++)
	{
		m_bitmapBmps[i].DeleteObject();
	}
}

BOOL CConnectionView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


typedef struct _LineDrawInfo
{
	int x1;
	int y1;
	int x2;
	int y2;
	int type;
}LineDrawInfo;


void CConnectionView::OnDraw(CDC* pDC)
{
	CConnectionDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	POINT points;
	CDC dcmem;BITMAP bm;
	dcmem.CreateCompatibleDC(pDC);
	int i = 0;
	for(list<OneMovePlayInfo>::reverse_iterator pIt=m_listMoves.rbegin(); pIt!=m_listMoves.rend(); ++pIt,++i)
	{
		points.x = 38 + (28 * (pIt->x - 1));
		points.y = 38 + (28 * (pIt->y - 1));
		CBitmap *pbmp;
		if (i == m_listMoves.size() - 1)
		{
			if ((*pIt).chessPlaySide == SIDE_BLACK)
			{
				pbmp = &m_bitmapBmps[0];
			}
			else if ((*pIt).chessPlaySide == SIDE_WHITE)
			{
				pbmp = &m_bitmapBmps[4];
			}
		}
		else if (i == m_listMoves.size() - 2)
		{
			if ((*pIt).chessPlaySide == SIDE_BLACK)
			{
				pbmp = &m_bitmapBmps[2];
			}
			else if ((*pIt).chessPlaySide == SIDE_WHITE)
			{
				pbmp = &m_bitmapBmps[1];
			}
		}
		else
		{
			if ((*pIt).chessPlaySide == SIDE_BLACK)
			{
				pbmp = &m_bitmapBmps[3];
			}
			else if ((*pIt).chessPlaySide == SIDE_WHITE)
			{
				pbmp = &m_bitmapBmps[5];
			}
		}
		dcmem.SelectObject(pbmp);
		GetObjectA(*pbmp, sizeof(BITMAP), (LPVOID)&bm);
		pDC->BitBlt(points.x, points.y, bm.bmWidth, bm.bmHeight, &dcmem, 0, 0, SRCCOPY);
		if(m_iShowNumber != 0)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255, 0, 0));
			CString number;
			number.Format(_T("%d"), i + 1);
			if (i + 1 < 10)
			{
				pDC->TextOut(points.x + 10, points.y + 5, number, 1);
			}
			else if (i < 100)
			{
				pDC->TextOut(points.x + 6, points.y + 5, number, 2);
			}
			else
			{
				pDC->TextOut(points.x, points.y + 5, number, 3);
			}
			pDC->SetTextColor(RGB(0, 0, 0));
		}
	}
	if (m_iShowLine != 0)
	{
		map<int, int>::iterator it;
		list<LineDrawInfo> l2;
		list<LineDrawInfo> l3;
		list<LineDrawInfo> l4;
		list<LineDrawInfo> l5;
		list<LineDrawInfo> l6;
		for (it = m_riRoadInfo.blackRoads.begin(); it != m_riRoadInfo.blackRoads.end(); it++)
		{
			if (it->second > 1)
			{
				LineDrawInfo ldi;
				ldi.y1 = m_iArrayRoads[it->first][0] / 19;
				ldi.x1 = m_iArrayRoads[it->first][0] % 19;
				ldi.y2 = m_iArrayRoads[it->first][5] / 19;
				ldi.x2 = m_iArrayRoads[it->first][5] % 19;
				switch (it->second)
				{
				case 2:
					ldi.type = 2;
					l2.push_back(ldi);
					break;
				case 3:
					ldi.type = 3;
					l3.push_back(ldi);
					break;
				case 4:
					ldi.type = 4;
					l4.push_back(ldi);
					break;
				case 5:
					ldi.type = 5;
					l5.push_back(ldi);
					break;
				case 6:
					ldi.type = 6;
					l6.push_back(ldi);
					break;
				}
			}
		}
		for (it = m_riRoadInfo.whiteRoads.begin(); it != m_riRoadInfo.whiteRoads.end(); it++)
		{
			if (it->second > 1)
			{
				LineDrawInfo ldi;
				ldi.y1 = m_iArrayRoads[it->first][0] / 19;
				ldi.x1 = m_iArrayRoads[it->first][0] % 19;
				ldi.y2 = m_iArrayRoads[it->first][5] / 19;
				ldi.x2 = m_iArrayRoads[it->first][5] % 19;
				switch (it->second)
				{
				case 2:
					ldi.type = 2;
					l2.push_back(ldi);
					break;
				case 3:
					ldi.type = 3;
					l3.push_back(ldi);
					break;
				case 4:
					ldi.type = 4;
					l4.push_back(ldi);
					break;
				case 5:
					ldi.type = 5;
					l5.push_back(ldi);
					break;
				case 6:
					ldi.type = 6;
					l6.push_back(ldi);
					break;
				}
			}
		}
		l6.insert(l6.end(), l5.begin(), l5.end());
		l6.insert(l6.end(), l4.begin(), l4.end());
		l6.insert(l6.end(), l3.begin(), l3.end());
		l6.insert(l6.end(), l2.begin(), l2.end());
		list<LineDrawInfo>::reverse_iterator itx;
		for (itx = l6.rbegin(); itx != l6.rend(); itx++)
		{
				CPen pen; 
				LOGPEN lp;
				lp.lopnStyle = PS_SOLID; 
				switch (itx->type)
				{
				case 2:
					lp.lopnWidth.x = 2; 
					lp.lopnColor = RGB(50, 0, 100);
					break;
				case 3:
					lp.lopnWidth.x = 3; 
					lp.lopnColor = RGB(100, 150, 0);
					break;
				case 4:
					lp.lopnWidth.x = 4; 
					lp.lopnColor = RGB(125, 0, 0);
					break;
				case 5:
					lp.lopnWidth.x = 4; 
					lp.lopnColor = RGB(255, 0, 0);
					break;
				case 6:
					lp.lopnWidth.x = 5; 
					lp.lopnColor = RGB(0, 255, 0);
					break;
				} 
				pen.CreatePenIndirect(&lp);  
				CPen *pOldPen = pDC->SelectObject(&pen);
				int sx = 51 + (28 * itx->x1);
				int sy = 51 + (28 * itx->y1);
				int dx = 51 + (28 * itx->x2);
				int dy = 51 + (28 * itx->y2);
				pDC->MoveTo(sx, sy);
				pDC->LineTo(dx, dy);
				pDC->SelectObject(pOldPen);
		}
	}
	if (m_iState == CPUTHINKING || m_iState == RUNNING || m_iState == STOP)
	{
		CString str, str2;
		if (m_pkMode == C2P || m_pkMode == C2C)
		{
			str.LoadString(IDS_CPU1_THINKING);
			long ticks = m_tsCPU1_UsedTime;
			long sec = ticks % 60;
			long hour = ticks / 3600;
			long min = ticks % 3600 / 60;
			str2.Format(_T("%.2d:%.2d:%.2d"), hour, min, sec);
			str += str2;
			pDC->DrawText(str, str.GetLength(), &m_rectTimer1, DT_CENTER);
		}
		if (m_pkMode == P2C || m_pkMode == C2C)
		{
			str.LoadString(IDS_CPU2_THINKING);
			long ticks = m_tsCPU2_UsedTime;
			long sec = ticks % 60;
			long hour = ticks / 3600;
			long min = ticks % 3600 / 60;
			str2.Format(_T("%.2d:%.2d:%.2d"), hour, min, sec);
			str += str2;
			pDC->DrawText(str, str.GetLength(), &m_rectTimer2, DT_CENTER);
		}
	}
}

#ifdef _DEBUG
void CConnectionView::AssertValid() const
{
	CView::AssertValid();
}

void CConnectionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CConnectionDoc* CConnectionView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CConnectionDoc)));
	return (CConnectionDoc*)m_pDocument;
}
#endif //_DEBUG


BOOL CConnectionView::OnEraseBkgnd(CDC* pDC)
{
	CRect   rect;   
	this->GetClientRect(&rect);   
	CRect rect1=rect;
	SelectObject(pDC->GetSafeHdc(),m_hBrush);
	pDC->Rectangle(0,0,631,615);
	if(rect.right>631)
	{
		rect1.left=rect.left+630;
		pDC->FillSolidRect(&rect1,RGB(200,200,200));
	}
	if(rect.bottom>615)
	{
		rect1=rect;
		rect1.top=614;
		pDC->FillSolidRect(&rect1,RGB(200,200,200));
	}
	return true;
}



void CConnectionView::OnFileNew()
{
	startGame(TRUE);
}

void CConnectionView::OnFileSetting()
{
	if (IDOK == m_dlgSetting.DoModal())
	{
		if (m_dlgSetting.m_iPlayer1_Type == 0 && m_dlgSetting.m_iPlayer2_Type == 0)
		{
			m_pkMode = P2P;
		}
		else if (m_dlgSetting.m_iPlayer1_Type == 0 && m_dlgSetting.m_iPlayer2_Type == 1)
		{
			m_pkMode = P2C;
		}
		else if (m_dlgSetting.m_iPlayer1_Type == 1 && m_dlgSetting.m_iPlayer2_Type == 0)
		{
			m_pkMode = C2P;
		}
		else if (m_dlgSetting.m_iPlayer1_Type == 1 && m_dlgSetting.m_iPlayer2_Type == 1)
		{
			m_pkMode = C2C;
		}
		this->m_strAIName = m_dlgSetting.m_strPlayer1_AI_Name;
		this->m_strAIName2 = m_dlgSetting.m_strPlayer2_AI_Name;
		this->m_iBlackDepth = m_dlgSetting.m_iBlackDepth;
		this->m_iWhiteDepth = m_dlgSetting.m_iWhiteDepth;
		this->m_iBlackTimeout = m_dlgSetting.m_iBlackTimeout;
		this->m_iWhiteTimeout = m_dlgSetting.m_iWhiteTimeout;
		this->m_iBlackThreshhold = m_dlgSetting.m_iSearchBranchThreshholds[m_dlgSetting.m_iPlayer1_SearchBranchThreshhold];
		this->m_iWhiteThreshhold =  m_dlgSetting.m_iSearchBranchThreshholds[m_dlgSetting.m_iPlayer2_SearchBranchThreshhold];
	}
}

void CConnectionView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_iState == RUNNING)
	{
		int x;
		int y;
		if(point.x >= 36 && point.x <= 569 && point.y >= 36 && point.y <= 569)
		{
			x = ((point.x - 22) / 28) + ((point.x - 22) % 28) / 14;
			y = ((point.y - 22) / 28) + ((point.y - 22) % 28) / 14;
			if(m_arrayBoard[y - 1][x - 1] == 0)
			{
				OneMovePlayInfo omd;
				omd.x = x;
				omd.y = y;
				omd.chessPlaySide = m_cpcChessPlayColor;
				m_listMoves.push_front(omd);
				setRoadInfos(m_listMoves.front().x, m_listMoves.front().y, m_listMoves.front().chessPlaySide);
				m_arrayBoard[y - 1][x - 1] = m_cpcChessPlayColor;
				Invalidate();
				if (isWin())
				{
					CString str;
					m_cpcChessPlayColor == SIDE_BLACK ? str.LoadString(IDS_BLACK_WIN) : str.LoadString(IDS_WHITE_WIN);
					MessageBox(str);
					OnFileStop();
					return;
				}
				if (m_listMoves.size() % 2 == 1)
				{
					if (m_listMoves.size() > 1)
					{
						list<OneMovePlayInfo>::iterator pIt = m_listMoves.begin();
						pIt++;
					}
					OneMovePlayInfo ompi = m_listMoves.front();
					m_cpcChessPlayColor = m_cpcChessPlayColor == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
					if (m_pkMode != P2P)
					{
						m_iState = CPUTHINKING;
						computerThink(SIDE_WHITE);
					}
				}
			}
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CConnectionView::initBoard(void)
{
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			m_arrayBoard[i][j] = 0;
		}
	}
}

void CConnectionView::newGame(void)
{
	initBoard();
	m_cpcChessPlayColor = SIDE_BLACK;
	m_listMoves.clear();
	Invalidate();
}

void CConnectionView::getAI(AbstractAI** ppAbstractAI, CString strName)
{
	AIFactory aIFactory;
	if (*ppAbstractAI != NULL)
	{
		delete *ppAbstractAI;
		*ppAbstractAI = NULL;
	}
	*ppAbstractAI = aIFactory.get_AI_Instance(strName);
	if (*ppAbstractAI == NULL)
	{
		MessageBox(_T("Fail to new AI instance"));
		OnFileStop();
	}
}

bool CConnectionView::isWin(void)
{
	if (m_listMoves.size() > 5)
	{
		OneMovePlayInfo omd = m_listMoves.front();
		int x = omd.x;
		int y = omd.y;
		ChessPlaySide cpc = omd.chessPlaySide;
		int k = 0;
		int i = max(0, x - 6);
		while (i <= min(x + 4, 18))
		{
			if ((ChessPlaySide)m_arrayBoard[y - 1][i] == cpc)
			{
				k++;
				if (k == 6)
				{
					return true;
				}
			}
			else
			{
				k = 0;
			}
			i++;
		}
		k = 0;
		i = max(0, y - 6);
		while (i <= min(y + 4, 18))
		{
			if ((ChessPlaySide)m_arrayBoard[i][x - 1] == cpc)
			{
				k++;
				if (k == 6)
				{
					return true;
				}
			}
			else
			{
				k = 0;
			}
			i++;
		}
		k = 0;
		int j = 0;
		if (x + y < 21)
		{
			i = max(0, x - 6);
			j = y + x - i - 2;
			while (i <= min(x + 4, 18) && j >= max(0, y - 6))
			{
				if ((ChessPlaySide)m_arrayBoard[j][i] == cpc)
				{
					k++;				
					if (k == 6)
					{
						return true;
					}
				}
				else
				{
					k = 0;
				}
				i++;
				j--;
			}
		}
		else
		{
			i = min(x + 4, 17);
			j = y + x - i - 2;
			while (i >= max(1, x - 6) && j <= min(y + 4, 18))
			{
				if ((ChessPlaySide)m_arrayBoard[j][i] == cpc)
				{
					k++;
					if (k == 6)
					{
						return true;
					}
				}
				else
				{
					k = 0;
				}
				i--;
				j++;
			}
		}
		k = 0;
		j = 0;
		if (x - y <= 0)
		{
			i = max(0, x - 6);
			j = y + i - x;
			while (i <= min(x + 4, 18) && j <= min(y + 4, 18))
			{
				if ((ChessPlaySide)m_arrayBoard[j][i] == cpc)
				{
					k++;
					if (k == 6)
					{
						return true;
					}
				}
				else
				{
					k = 0;
				}
				i++;
				j++;
			}
		}
		else
		{
			i = min(x + 4, 17);
			j = y + i - x;
			while (i >= max(1, x - 6) && j >= max(0, y - 6))
			{
				if ((ChessPlaySide)m_arrayBoard[j][i] == cpc)
				{
					k++;
					if (k == 6)
					{
						return true;
					}
				}
				else
				{
					k = 0;
				}
				i--;
				j--;
			}
		}
	}
	return false;
}

void CConnectionView::OnFileStop()
{
	m_iState = STOP;
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(0, MF_BYPOSITION | MF_ENABLED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(6, MF_BYPOSITION | MF_ENABLED);	
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(3, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(4, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetMainWnd()->DrawMenuBar();
	KillTimer(1);
	KillTimer(2);	
	if (m_thread1 != NULL)
	{
		TerminateThread(m_thread1->m_hThread, TRUE);
		m_thread1 = NULL;
	}
	if (m_thread2 != NULL)
	{
		TerminateThread(m_thread2->m_hThread, TRUE);
		m_thread2 = NULL;
	}
}

void CConnectionView::computerThink(ChessPlaySide chessPlaySide)
{
	if (chessPlaySide == SIDE_BLACK)
	{
		m_thread1 = AfxBeginThread(thinkThread, this);
	}
	else if (chessPlaySide == SIDE_WHITE)
	{
		m_thread2 = AfxBeginThread(thinkThread, this);
	}
}

UINT CConnectionView::thinkThread(LPVOID lpParam)
{
	UINT ret = TRUE;
	CConnectionView* pThis = (CConnectionView*)lpParam;
	ChessPlaySide cpc = pThis->m_cpcChessPlayColor;
	if (pThis->m_iState == RUNNING || pThis->m_iState == CPUTHINKING)
	{
		AbstractAI* pAbstractAI = pThis->m_cpcChessPlayColor == SIDE_BLACK ? pThis->m_pAbstractAI : pThis->m_pAbstractAI2;
		UINT_PTR nTimer = pThis->m_cpcChessPlayColor == SIDE_BLACK ? 1 : 2;
		list<OneMovePlayInfo> listPlayInfos;
		if (pThis->m_listMoves.size() > 0)
		{
			OneMovePlayInfo ompi;
			if (pThis->m_listMoves.size() > 1)
			{
				list<OneMovePlayInfo>::iterator pOmpiIt = pThis->m_listMoves.begin();
				ompi = *(++pOmpiIt);
				listPlayInfos.push_front(ompi);
			}
			ompi = pThis->m_listMoves.front();
			listPlayInfos.push_front(ompi);
		}

		long startTick = ::GetTickCount();
		pThis->SetTimer(nTimer, 1000, NULL);
		list<OneMovePlayInfo> listOneMovePlayInfo = pAbstractAI->getNextMoveFromAI(listPlayInfos);
		pThis->KillTimer(nTimer);

		if (listOneMovePlayInfo.size() > 2	||
			listOneMovePlayInfo.size() <= 0 || 
			pThis->m_listMoves.size() == 0 && listOneMovePlayInfo.size() > 1 ||
			pThis->m_listMoves.size() > 0 && listOneMovePlayInfo.size() != 2 ||
			listOneMovePlayInfo.front().chessPlaySide != pThis->m_cpcChessPlayColor)
		{
			ret = FALSE;
		}
		else
		{
			list<OneMovePlayInfo>::iterator pIt = listOneMovePlayInfo.begin();
			if (listOneMovePlayInfo.size() == 1)
			{
				OneMovePlayInfo ompi1 = *pIt;
				if (ompi1.x == 0 && ompi1.y == 0)
				{
					ret = TRUE;
				}
				else if (ompi1.x < 1 || ompi1.x > 19 || ompi1.y < 1 || ompi1.y > 19)
				{
					ret = FALSE;
				}
			}
			else if (listOneMovePlayInfo.size() == 2)
			{
				OneMovePlayInfo ompi1 = *pIt;
				OneMovePlayInfo ompi2 = *(++pIt);
				if (ompi1.x == 0 && ompi1.y == 0 && ompi2.x == 0 && ompi2.y == 0)
				{
					ret = TRUE;
				}
				else if (ompi1.x == ompi2.x && ompi1.y == ompi2.y || 
					ompi1.chessPlaySide != ompi2.chessPlaySide ||
					ompi1.x < 1 || ompi1.x > 19 ||
					ompi2.x < 1 || ompi2.x > 19 ||
					ompi1.y < 1 || ompi1.y > 19 ||
					ompi2.y < 1 || ompi2.y > 19
					)
				{
					ret = FALSE;
				}
			}
			if (ret == TRUE)
			{
				for (pIt = listOneMovePlayInfo.begin(); pIt != listOneMovePlayInfo.end(); ++pIt)
				{
					if (pIt->x == 0 && pIt->y == 0)
					{
						break;
					}
					if (pThis->m_arrayBoard[pIt->y - 1][pIt->x - 1] > 0)
					{
						ret = FALSE;
						break;
					}					
					pThis->setRoadInfos(pIt->x, pIt->y, pIt->chessPlaySide);
					pThis->m_listMoves.push_front(*pIt);
					pThis->m_arrayBoard[pIt->y - 1][pIt->x - 1] = pIt->chessPlaySide;
					pThis->Invalidate();
					if (pThis->isWin())
					{
						CString str;
						pThis->m_cpcChessPlayColor == SIDE_BLACK ? str.LoadString(IDS_BLACK_WIN) : str.LoadString(IDS_WHITE_WIN);
						pThis->MessageBox(str);
						pThis->OnFileStop();
						ret = TRUE;
					}
				}
			}
		}
		if (ret == FALSE)
		{
			CString tstr;
			tstr.LoadString(IDS_ERROR_AI);
			pThis->MessageBox(tstr);
			pThis->OnFileStop();
		}
		else
		{
			pThis->m_cpcChessPlayColor = pThis->m_cpcChessPlayColor == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
			switch (pThis->m_pkMode)
			{
			case P2C:
			case C2P:
				AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED & ~MF_GRAYED);
				pThis->m_iState = RUNNING;
				break;
			case C2C:				
				AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
				pThis->m_iState = CPUTHINKING;
				pThis->computerThink(pThis->m_cpcChessPlayColor);
				break;
			}
		}
	}
	else
	{
		pThis->OnFileStop();
	}
	if (cpc == SIDE_BLACK)
	{
		pThis->m_thread1 = NULL;
	}
	else
	{
		pThis->m_thread2 = NULL;
	}
	return ret;
}


void CConnectionView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{		
		InvalidateRect(&m_rectTimer1);
		m_tsCPU1_UsedTime++;
	}
	else if (nIDEvent == 2)
	{
		InvalidateRect(&m_rectTimer2);
		m_tsCPU2_UsedTime++;
	}
	CView::OnTimer(nIDEvent);
}

void CConnectionView::closeChessManualFile(void)
{
	if (m_pChessManualFile != NULL)
	{
		m_pChessManualFile->Close();
		delete m_pChessManualFile;
		m_pChessManualFile = NULL;
	}
}

void CConnectionView::writeChessManual(int x, int y)
{
	CString str;
	str.Format(_T("\n%d %d"), x, y);
	m_pChessManualFile->WriteString(str);
	m_pChessManualFile->Flush();
}

void CConnectionView::OnFileOpen()
{
	CString szFilterFDlg = _T("棋谱文件 (*.cm)|*.cm|所有文件 (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, szFilterFDlg, NULL);
	CString s = getChessManualDirectory();
	fileDlg.m_pOFN->lpstrInitialDir = s;
	if(fileDlg.DoModal() != IDOK)
	{
		closeChessManualFile();
		return;
	}
	CString filepath = fileDlg.GetPathName();
	m_pChessManualFile = new CStdioFile(filepath, CFile::modeReadWrite);
	initBoard();
	initRoadInfo();
	m_listMoves.clear();
	int x, y;
	CString str;
	int n = 0;
	while(m_pChessManualFile->ReadString(str))
	{
		if (str.IsEmpty())
		{
			continue;
		}
		USES_CONVERSION;
		char* p = W2A(str);
		sscanf(p,"%d%d", &x, &y);
		if(x > 0 && x < 20 && y > 0 && y < 20 && m_arrayBoard[y - 1][x - 1] == 0)
		{
			OneMovePlayInfo ompi;
			ompi.x = x;
			ompi.y = y;
			m_cpcChessPlayColor = (ChessPlaySide)(((n + 1) / 2) % 2 + 1);			
			ompi.chessPlaySide = m_cpcChessPlayColor;
			m_listMoves.push_front(ompi);
			m_arrayBoard[y - 1][x - 1] = m_cpcChessPlayColor;
			setRoadInfos(x, y, m_cpcChessPlayColor);
			Invalidate();
			if (isWin())
			{
				CString str;
				m_cpcChessPlayColor == SIDE_BLACK ? str.LoadString(IDS_BLACK_WIN) : str.LoadString(IDS_WHITE_WIN);
				MessageBox(str);
				OnFileStop();
				closeChessManualFile();
				return;
			}
		}
		else
		{
			MessageBox(_T("Wrong value in position file"));
			OnFileStop();
			closeChessManualFile();
			return;
		}
		n++;
	}
	if (n % 2 == 0)
	{
		MessageBox(_T("Wrong format in position file"));
		OnFileStop();
		closeChessManualFile();
		return;
	}
	if (n > 0)
	{
		m_cpcChessPlayColor = m_cpcChessPlayColor == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
	}
	closeChessManualFile();
	gotoPause();
	m_iState = INIT;
} 
BOOL CConnectionView::createNewChessManulFile(CStdioFile** sf)
{
	CTime time = CTime::GetCurrentTime();
	CString tstr = time.Format(_T("%Y_%m_%d %H_%M_%S %W_%A")) + ".cm";
	CString path = getModuleDirectory(); 
    path += _T("\\chessmanual");
	*sf = new CStdioFile(path + "\\" + tstr, CFile::modeWrite | CFile::modeCreate);	
	if (*sf == NULL)
	{
		MessageBox(_T("Connect6"));
		OnFileStop();
		return FALSE;
	}
	return TRUE;
}

CString CConnectionView::getModuleDirectory(void)
{
	CString path; 
    GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos);
	return path;
}

CString CConnectionView::getChessManualDirectory(void)
{
	CString str = getModuleDirectory();
	return str + _T("\\chessmanual");
}

void CConnectionView::startGame(BOOL isNewGame)
{
	if (isNewGame)
	{
		initRoadInfo();
	}
	else if (isWin())
	{
		CString str;
		m_cpcChessPlayColor == SIDE_BLACK ? str.LoadString(IDS_BLACK_WIN) : str.LoadString(IDS_WHITE_WIN);
		MessageBox(str);
		OnFileStop();
		return;
	}
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(2, MF_BYPOSITION | MF_ENABLED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(4, MF_BYPOSITION | MF_ENABLED);
	AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED & ~MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(0, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(3, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(6, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetMainWnd()->DrawMenuBar();
	switch (m_pkMode)
	{
	case P2P:
		m_iState = RUNNING;
		AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED & ~MF_GRAYED);
		if (isNewGame)
		{			
			newGame();
		}
		break;
	case P2C:
		m_tsCPU2_UsedTime = 0;
		getAI(&m_pAbstractAI2, m_strAIName2);
		if (m_pAbstractAI2 != NULL)
		{
			m_pAbstractAI2->setSearchDepth(this->m_iWhiteDepth);
			m_pAbstractAI2->setSearchBranchThreshhold(this->m_iWhiteThreshhold);
			m_pAbstractAI2->setTimeout(this->m_iWhiteTimeout);
			m_iState = CPUTHINKING;
			if (isNewGame)
			{
				AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED & ~MF_GRAYED);
				m_iState = RUNNING;
				newGame();
			}
			else
			{
				m_pAbstractAI2->setHistoryMoves(m_listMoves, SIDE_WHITE);
				//******* change this for "undo" operation, because m_listMoves could be empty 2018-6-30
				//if(m_listMoves.front().chessPlaySide == SIDE_BLACK)

				
				if (m_cpcChessPlayColor == SIDE_WHITE)
				{
					//********add this to sovle semi move problem 2018-7-1
					if (m_listMoves.front().chessPlaySide == SIDE_WHITE) m_listMoves.pop_front();
					//****************************************************

					AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
					computerThink(SIDE_WHITE);
				}
				else
				{
					AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED & ~MF_GRAYED);
					m_iState = RUNNING;
				}

			}
		}
		break;
	case C2P:
		m_tsCPU1_UsedTime = 0;
		getAI(&m_pAbstractAI, m_strAIName);
		if (m_pAbstractAI != NULL)
		{
			m_pAbstractAI->setSearchDepth(this->m_iBlackDepth);
			m_pAbstractAI->setSearchBranchThreshhold(this->m_iBlackThreshhold);
			m_pAbstractAI->setTimeout(m_iBlackTimeout);
			m_iState = CPUTHINKING;
			if (isNewGame)
			{
				newGame();
				AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
				computerThink(SIDE_BLACK);
			}
			else
			{
				m_pAbstractAI->setHistoryMoves(m_listMoves, SIDE_BLACK);
				//******* change this for "undo" operation, because m_listMoves could be empty 2018-6-30
				//if(m_listMoves.front().chessPlaySide == SIDE_WHITE)

				if (m_cpcChessPlayColor == SIDE_BLACK)
				{
					//********add this to sovle semi move problem 2018-7-1
					if (m_listMoves.front().chessPlaySide == SIDE_BLACK) m_listMoves.pop_front();
					//****************************************************

					AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
					computerThink(SIDE_BLACK);
				}
				else
				{
					AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED & ~MF_GRAYED);
					m_iState = RUNNING;
				}
			}
		}
		break;
	case C2C:
		m_tsCPU1_UsedTime = 0;
		m_tsCPU2_UsedTime = 0;
		getAI(&m_pAbstractAI, m_strAIName);
		getAI(&m_pAbstractAI2, m_strAIName2);
		if (m_pAbstractAI != NULL && m_pAbstractAI2 != NULL)
		{
			m_pAbstractAI->setSearchDepth(this->m_iBlackDepth);
			m_pAbstractAI2->setSearchDepth(this->m_iWhiteDepth);
			m_pAbstractAI->setSearchBranchThreshhold(this->m_iBlackThreshhold);
			m_pAbstractAI2->setSearchBranchThreshhold(this->m_iWhiteThreshhold);
			m_pAbstractAI->setTimeout(m_iBlackTimeout);
			m_pAbstractAI2->setTimeout(m_iWhiteTimeout);
			m_iState = CPUTHINKING;
			AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
			if (isNewGame)
			{
				newGame();
				computerThink(SIDE_BLACK);
			}
			else
			{
				//*********** the recursion could have potential risk of storage problem. 2018-6-30
				m_pAbstractAI->setHistoryMoves(m_listMoves, SIDE_BLACK);
				m_pAbstractAI2->setHistoryMoves(m_listMoves, SIDE_WHITE);
				if (m_listMoves.front().chessPlaySide == SIDE_WHITE)
				{
					computerThink(SIDE_BLACK);
				}
				else
				{
					computerThink(SIDE_WHITE);
				}
			}
		}
		break;
	}
}

void CConnectionView::OnFileContinue()
{
	if(m_listMoves.size() == 0) // *****如果继续的时候历史表为0，说明悔棋撤销了所有记录，因此就重新开局
		startGame(TRUE);
	else
		startGame(FALSE);
}

void CConnectionView::OnFilePause()
{
	gotoPause();
}

void CConnectionView::OnChessBack()
{
	if ((m_iState == RUNNING || m_iState == PAUSE) && m_listMoves.size() > 0)
	{
		
		if (m_listMoves.size() % 2 == 0 || m_listMoves.size() == 1)
		{
			m_arrayBoard[m_listMoves.front().y - 1][m_listMoves.front().x - 1] = 0;
			eraseRoadInfos(m_listMoves.front().x, m_listMoves.front().y, m_listMoves.front().chessPlaySide);
			m_listMoves.pop_front();
		}
		else
		{
			for (int i = 0; i < 2; i++)
			{
				m_arrayBoard[m_listMoves.front().y - 1][m_listMoves.front().x - 1] = 0;
				eraseRoadInfos(m_listMoves.front().x, m_listMoves.front().y, m_listMoves.front().chessPlaySide);

				if (m_pkMode == C2P && m_pAbstractAI != NULL)
				{
					m_pAbstractAI->unDoStone(m_listMoves.front().x, m_listMoves.front().y, SIDE_BLACK);
				}
				else if (m_pkMode == P2C && m_pAbstractAI2 != NULL)
				{
					m_pAbstractAI2->unDoStone(m_listMoves.front().x, m_listMoves.front().y, SIDE_WHITE);
				}
				m_listMoves.pop_front();
			}
		}
		m_cpcChessPlayColor = m_cpcChessPlayColor == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
		Invalidate();
		//RedrawWindow(NULL, NULL, RDW_UPDATENOW);
		gotoPause();
		//*********** do not need to revise the manual in real time, only save the manual when click "save files". 2018-6-30
		/*if (m_pChessManualFile != NULL)
		{
			CString strFilePath = m_pChessManualFile->GetFilePath();
			m_pChessManualFile->Close();
			m_pChessManualFile->Open(strFilePath, CFile::modeWrite | CFile::modeCreate);
			for (list<OneMovePlayInfo>::reverse_iterator pIt = m_listMoves.rbegin(); pIt != m_listMoves.rend(); ++pIt)
			{
				writeChessManual(pIt->x, pIt->y);
			}
			m_pChessManualFile->Flush();
		}	*/
	}
}

void CConnectionView::gotoPause(void)
{
	if (m_thread1 != NULL)
	{
		::TerminateThread(m_thread1->m_hThread, -1);
		Sleep(2000);
		
		m_thread1 = NULL;
	}
	if (m_thread2 != NULL)
	{
		::TerminateThread(m_thread2->m_hThread, -1);
		Sleep(2000);
		m_thread2 = NULL;
	}	
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(3, MF_BYPOSITION | MF_ENABLED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(4, MF_BYPOSITION | MF_ENABLED);	
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(0, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu(0)->EnableMenuItem(6, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	//AfxGetApp()->m_pMainWnd->GetMenu()->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	AfxGetMainWnd()->DrawMenuBar();
	m_iState = PAUSE;
}

void CConnectionView::OnReserve()
{
	if (m_pChessManualFile != NULL)
	{
		m_pChessManualFile->Close();
		m_pChessManualFile = NULL;
	}

	CTime time = CTime::GetCurrentTime();
	CString tstr = time.Format(_T("%Y_%m_%d %H_%M_%S %W_%A")) + ".cm";
	CString path = getModuleDirectory();
	path += _T("\\chessmanual");
	m_pChessManualFile = new CStdioFile(path + "\\" + tstr, CFile::modeWrite | CFile::modeCreate);
	if (*m_pChessManualFile == NULL)
	{
		MessageBox(_T("Connect6"));
		OnFileStop();
	}

	for (list<OneMovePlayInfo>::reverse_iterator pIt = m_listMoves.rbegin(); pIt != m_listMoves.rend(); ++pIt)
	{
		writeChessManual(pIt->x, pIt->y);
	}
	m_pChessManualFile->Flush();
	Invalidate();
	m_pChessManualFile->Close();
}


void CConnectionView::OnUpdateShowNumber(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_iShowNumber);
}

void CConnectionView::OnUpdateShowLine(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_iShowLine);
}

void CConnectionView::OnShowLine()
{
	m_iShowLine = 1 - m_iShowLine;
	this->Invalidate();
}

void CConnectionView::OnShowNumber()
{
	m_iShowNumber = 1 - m_iShowNumber;
	this->Invalidate();
}

int CConnectionView::getConnectCount(char shape)
{
	switch (shape)
	{
	case 0:												//000000 0		0
		return 0;
	case 1:												//000001 1		1
	case 2:												//000010 2		1
	case 4:												//000100 4		1
	case 8:												//001000 8		1
	case 16:											//010000 16
	case 32:											//100000 32
		return 1;
	case 3:												//000011 3	
	case 5:												//000101 5
	case 6:												//000110 6	
	case 9:												//001001 9		2m
	case 10:											//001010 10		2
	case 12:											//001100 12
	case 17:											//010001 17
	case 18:											//010010 18
	case 20:											//010100 20
	case 24:											//011000 24
	case 33:											//100001 33
	case 34:											//100010 34
	case 36:											//100100 36
	case 40:											//101000 40
	case 48:											//110000 48
		return 2;	
	case 7:												//000111 7		3
	case 11:											//001011 11
	case 13:											//001101 13
	case 14:											//001110 14
	case 19:											//010011 19	
	case 21:											//010101 21
	case 22:											//010110 22	
	case 25:											//011001 25
	case 26:											//011010 26
	case 28:											//011100 28
	case 35:											//100011 35
	case 37:											//100101 37
	case 38:											//100110 38
	case 41:											//101001 41
	case 42:											//101010 42
	case 44:											//101100 44
	case 49:											//110001 49
	case 50:											//110010 50
	case 52:											//110100 52
	case 56:											//111000 56
		return 3;
	case 15:											//001111 15
	case 23:											//010111 23
	case 27:											//011011 27
	case 29:											//011101 29
	case 30:											//011110 30
	case 39:											//100111 39
	case 43:											//101011 43
	case 45:											//101101 45
	case 46:											//101110 46
	case 51:											//110011 51
	case 53:											//110101 53
	case 54:											//110110 54
	case 57:											//111001 57
	case 58:											//111010 58
	case 60:											//111100 60
		return 4;
	case 31:											//011111 31
	case 47:											//101111 47
	case 55:											//110111 55
	case 59:											//111011 59
	case 61:											//111101 61
	case 62:											//111110 62
		return 5;
	case 63:											//111111 63
		return 6;
	}
	return 0;
}

void CConnectionView::setRoadInfos(int x, int y, ChessPlaySide cps)
{
	int roadIndex = (y - 1) * 19 + (x - 1);
	for (int i = 0; i < m_riArrayR_Roads[roadIndex].count; i++)
	{
		const R_RoadItem* pRri = &m_riArrayR_Roads[roadIndex].r_RoadItems[i];
		RoadItem* pRi = &m_riRoadInfo.roadItems[pRri->road_no];
		int flag = 0;
		if (cps == SIDE_BLACK)
		{
			pRi->blackRoad |= staticBitOpNumber[pRri->at_pos];			
		}
		else if (cps == SIDE_WHITE)
		{
			pRi->whiteRoad |= staticBitOpNumber[pRri->at_pos];
		}
		if (pRi->blackRoad != 0 && pRi->whiteRoad == 0)
		{
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
			m_riRoadInfo.blackRoads.erase(pRri->road_no);
			m_riRoadInfo.blackRoads.insert(pair<int, int>(pRri->road_no, getConnectCount(pRi->blackRoad)));	
		}
		else if (pRi->blackRoad == 0 && pRi->whiteRoad != 0)
		{
			m_riRoadInfo.blackRoads.erase(pRri->road_no);
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
			m_riRoadInfo.whiteRoads.insert(pair<int, int>(pRri->road_no, getConnectCount(pRi->whiteRoad)));
		}
		else if (pRi->blackRoad != 0 && pRi->whiteRoad != 0)
		{
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
			m_riRoadInfo.blackRoads.erase(pRri->road_no);
		}
	}
}

void CConnectionView::eraseRoadInfos(int x, int y, ChessPlaySide cps)
{
	int roadIndex = (y - 1) * 19 + (x - 1);
	for (int i = 0; i < m_riArrayR_Roads[roadIndex].count; i++)
	{
		const R_RoadItem* pRri = &m_riArrayR_Roads[roadIndex].r_RoadItems[i];
		RoadItem* pRi = &m_riRoadInfo.roadItems[pRri->road_no];
		int flag = 0;
		if (cps == SIDE_BLACK)
		{
			pRi->blackRoad &= ~staticBitOpNumber[pRri->at_pos];			
		}
		else if (cps == SIDE_WHITE)
		{
			pRi->whiteRoad &= ~staticBitOpNumber[pRri->at_pos];
		}
		if (pRi->blackRoad != 0 && pRi->whiteRoad == 0)
		{
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
			m_riRoadInfo.blackRoads.erase(pRri->road_no);
			m_riRoadInfo.blackRoads.insert(pair<int, int>(pRri->road_no, getConnectCount(pRi->blackRoad)));	
		}
		else if (pRi->blackRoad == 0 && pRi->whiteRoad != 0)
		{
			m_riRoadInfo.blackRoads.erase(pRri->road_no);
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
			m_riRoadInfo.whiteRoads.insert(pair<int, int>(pRri->road_no, getConnectCount(pRi->whiteRoad)));
		}
		else if (pRi->blackRoad == 0 && pRi->whiteRoad == 0)
		{
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
			m_riRoadInfo.blackRoads.erase(pRri->road_no);
		}
	}
}



void CConnectionView::initRoadInfo(void)																		//初始化路信息
{
	memset(m_arrayBoard, 0, 361);
	memset(&m_riRoadInfo, 0, sizeof(RoadInfo) - sizeof(map<int, int>) * 2);
	m_riRoadInfo.blackRoads.clear();
	m_riRoadInfo.whiteRoads.clear();
	m_riRoadInfo.blackScore = 0;
	m_riRoadInfo.whiteScore = 0;
	for (int i = 0; i < 924; i++)
	{
		int x, y;
		char blackRoad = 0;
		char whiteRoad = 0;
		char stone;
		int j;
		for (j = 0; j < 6; j++)
		{
			y = m_iArrayRoads[i][j] / 19;
			x = m_iArrayRoads[i][j] % 19;
			stone = m_arrayBoard[y][x];			
			if (stone != 0)
			{
				if ((ChessPlaySide)stone == SIDE_BLACK)
				{
					blackRoad |= staticBitOpNumber[j];
				}
				else if ((ChessPlaySide)stone == SIDE_WHITE)
				{
					whiteRoad |= staticBitOpNumber[j];
				}
			}
		}
		m_riRoadInfo.roadItems[i].blackRoad = blackRoad;
		m_riRoadInfo.roadItems[i].whiteRoad = whiteRoad;
		if (blackRoad != 0 && whiteRoad == 0)
		{
			m_riRoadInfo.blackRoads.insert(pair<int, int>(i, getConnectCount(blackRoad)));
		}
		else if (blackRoad == 0 && whiteRoad != 0)
		{
			m_riRoadInfo.whiteRoads.insert(pair<int, int>(i, getConnectCount(whiteRoad)));
		}
		else if (blackRoad != 0 && whiteRoad != 0)
		{
			m_riRoadInfo.whiteRoads.erase(i);
			m_riRoadInfo.blackRoads.erase(i);
		}
	}
}




