#include "stdafx.h"
#include "AbstractAI.h"
#include <iostream>

AbstractAI::AbstractAI(void)
{
	initBoardInfo();	
	initRoadInfo();
	Zobrist.InitZobrist();  //Zobrist������� 2018-7-1
	zobr.InitZero();		//Zobrist�ṹ��¼��ǰ�����key 2018-7-1
	m_iSearchDepth = 1;
	//cout << "AI open" << endl;
}

AbstractAI::~AbstractAI(void)
{
	//cout << "AI close" << endl;
}

void AbstractAI::setSearchBranchThreshhold(int threshhold){
	m_iSearchBranchThreshhold=threshhold;
};

void AbstractAI::setTimeout(int timeout){
	m_iTimeout=timeout;
};

bool AbstractAI::loadInitLib()
{
	return false;
}

void AbstractAI::setSearchDepth(int searchDepth)
{
	this->m_iSearchDepth = searchDepth;
}


void AbstractAI::putStone(int x, int y, ChessPlaySide cps)
{
	setRoadInfos(x + 1, y + 1, cps);
}

void AbstractAI::unDoStone(int x, int y, ChessPlaySide cps)
{
	eraseRoadInfos(x + 1, y + 1, cps);
}

//*****ִ��������һ�� 2018-7-1
void AbstractAI::makeOneMove(int x1, int y1, int x2, int y2)
{
	putStone(x1, y1, nextChessPlaySide);
	putStone(x2, y2, nextChessPlaySide);
	int n = 19 * (y1 - 1) + x1 - 1;
	zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
	n = 19 * (y2 - 1) + x2 - 1;
	zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
	zobr.Xor(Zobrist.Player);
	changePlaySide();
}

//*****����������һ�� 2018-7-1
void AbstractAI::unDoOneMove(int x1, int y1, int x2, int y2)
{
	changePlaySide();
	unDoStone(x1, y1, nextChessPlaySide);
	unDoStone(x2, y2, nextChessPlaySide);
	int n = 19 * (y2 - 1) + x2 - 1;
	zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
	n = 19 * (y1 - 1) + x1 - 1;
	zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
	zobr.Xor(Zobrist.Player);
	
}

int AbstractAI::getScore(ChessPlaySide cps)
{
	return cps == SIDE_BLACK ? m_riRoadInfo.blackScore - m_riRoadInfo.whiteScore : m_riRoadInfo.whiteScore - m_riRoadInfo.blackScore;;
}

void AbstractAI::initBoardInfo()																//��ʼ�����������Ϣ
{
	
	memset(m_cbiChessBoard.board_of_own_stones, 0, 46);
	memset(m_cbiChessBoard.board_of_rival_stones, 0, 46);
	memset(m_charArrayBoard, 0, 361);
	memset(m_charArrayCanPutStone, 0, 361);
}

list<OneMovePlayInfo> AbstractAI::getNextMoveFromAI(list<OneMovePlayInfo> rivalLastMove)		//��AI��ȡ��һ��
{
	int x1, x2, y1, y2;
	list<OneMovePlayInfo> ret;
	OneMovePlayInfo ompi;
	if (m_listMovePlay.size() == 0 && rivalLastMove.size() == 0)								//������Ϊ�գ�˵���ߵ�һ���ڣ�������
	{
		m_myChessPlaySide = SIDE_BLACK;					//��������ɫ
		nextChessPlaySide = SIDE_BLACK;					//��һ������ɫ 2018-7-1
		ompi.x = 10;
		ompi.y = 10;
		ompi.chessPlaySide = SIDE_BLACK;
		ret.push_back(ompi);
		int n = 19 * (ompi.y - 1) + ompi.x - 1;
		zobr.Xor(Zobrist.Player);                        //hash for black to move 2018-7-1
		zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]);  //hash for black's move 2018-7-1
		m_cbiChessBoard.board_of_own_stones[n / 8] |= staticBitOpNumber[n % 8];
		m_charArrayBoard[ompi.y - 1][ompi.x - 1] = m_myChessPlaySide;
		x1 = ompi.x;
		y1 = ompi.y;
		setRoadInfos(x1, y1, SIDE_BLACK);
	}
	else if (rivalLastMove.size() > 0)
	{
		if (m_listMovePlay.size() == 0 && rivalLastMove.size() == 1)							//�������Ӽ�¼��Ϊ�գ��Է����Ӽ�¼��Ϊ1��˵���Լ��ߵڶ��������
		{
			m_myChessPlaySide = SIDE_WHITE;				//��������ɫ
			nextChessPlaySide = SIDE_WHITE;				//��һ������ɫ 2018-7-1
			ompi = rivalLastMove.front();
			x1 = ompi.x;
			y1 = ompi.y;
			x2 = 0;
			y2 = 0;
			int n = 19 * (ompi.y - 1) + ompi.x - 1;
			zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]); //hash for last move 2018-7-1
			m_cbiChessBoard.board_of_rival_stones[n / 8] |= staticBitOpNumber[n % 8];
			m_charArrayBoard[ompi.y - 1][ompi.x - 1] = ompi.chessPlaySide;
			setRoadInfos(x1, y1, SIDE_BLACK);
			m_listMovePlay.push_back(ompi);
			//���ٿ��������˷�ʱ��ֱ��ѡ����ʷ�Ͼ����ԽϺõ�Ӧ��
			int x = ompi.x;
			int y = ompi.y;
			if (x <= 10 && y <= 10)
			{
				/*x1 = x + 1;
				y1 = y + 2;
				x2 = x + 2;
				y2 = y + 1;*/
				x1 = x;
				y1 = y + 1;
				x2 = x + 1;
				y2 = y;
			}
			else if (x <= 10 && y > 10)
			{
				/*x1 = x + 1;
				y1 = y - 2;
				x2 = x + 2;
				y2 = y - 1;*/
				x1 = x ;
				y1 = y - 1;
				x2 = x + 1;
				y2 = y;
			}
			else if (x > 10 && y <= 10)
			{
				x1 = x ;
				y1 = y + 1;
				x2 = x - 1;
				y2 = y;
			}
			else
			{
				x1 = x;
				y1 = y - 1;
				x2 = x - 1;
				y2 = y;
			}
			ompi.x = x1;
			ompi.y = y1;
			ompi.chessPlaySide = m_myChessPlaySide;
			ret.push_back(ompi);
			m_listMovePlay.push_back(ompi);
			n = 19 * (ompi.y - 1) + ompi.x - 1;
			zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]);  //hash for this move 2018-7-1
			m_cbiChessBoard.board_of_own_stones[n / 8] |= staticBitOpNumber[n % 8];
			m_charArrayBoard[ompi.y - 1][ompi.x - 1] = m_myChessPlaySide;
			ompi.x = x2;
			ompi.y = y2;
			ompi.chessPlaySide = m_myChessPlaySide;
			ret.push_back(ompi);
			m_listMovePlay.push_back(ompi);
			n = 19 * (ompi.y - 1) + ompi.x - 1;
			zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]);  //hash for this move 2018-7-1
			m_cbiChessBoard.board_of_own_stones[n / 8] |= staticBitOpNumber[n % 8];
			m_charArrayBoard[ompi.y - 1][ompi.x - 1] = m_myChessPlaySide;
			setRoadInfos(x1, y1, m_myChessPlaySide);
			setRoadInfos(x2, y2, m_myChessPlaySide);

			/*cout << "White Zobrist Key: " << zobr.dwKey << ", ";
			cout << zobr.dwLock0 << ", ";
			cout << zobr.dwLock1 << endl;*/

			return ret;
		}
		else
		{
			list<OneMovePlayInfo>::iterator pIt = rivalLastMove.begin();
			ompi = *(++pIt);
			x1 = ompi.x;
			y1 = ompi.y;
			m_listMovePlay.push_back(ompi);
			int n = 19 * (ompi.y - 1) + ompi.x - 1;
			zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]); //hash for last move 2018-7-1
			m_cbiChessBoard.board_of_rival_stones[n / 8] |= staticBitOpNumber[n % 8];
			m_charArrayBoard[ompi.y - 1][ompi.x - 1] = ompi.chessPlaySide;
			ompi = rivalLastMove.front();
			x2 = ompi.x;
			y2 = ompi.y;
			m_listMovePlay.push_back(ompi);
			n = 19 * (ompi.y - 1) + ompi.x - 1;
			zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]); //hash for last move 2018-7-1
			m_cbiChessBoard.board_of_rival_stones[n / 8] |= staticBitOpNumber[n % 8];
			m_charArrayBoard[ompi.y - 1][ompi.x - 1] = ompi.chessPlaySide;
			setRoadInfos(x1, y1, ompi.chessPlaySide);
			setRoadInfos(x2, y2, ompi.chessPlaySide);
		}
		
		run(x1, y1, x2, y2);

		ompi.x = x1;
		ompi.y = y1;
		ompi.chessPlaySide = m_myChessPlaySide;
		ret.push_back(ompi);
		m_listMovePlay.push_back(ompi);
		int n = 19 * (ompi.y - 1) + ompi.x - 1;
		zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]); //hash for this move 2018-7-1
		m_cbiChessBoard.board_of_own_stones[n / 8] |= staticBitOpNumber[n % 8];
		m_charArrayBoard[ompi.y - 1][ompi.x - 1] = m_myChessPlaySide;
		ompi.x = x2;
		ompi.y = y2;
		ompi.chessPlaySide = m_myChessPlaySide;
		ret.push_back(ompi);
		m_listMovePlay.push_back(ompi);
		n = 19 * (ompi.y - 1) + ompi.x - 1;
		zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][n]); //hash for this move 2018-7-1
		m_cbiChessBoard.board_of_own_stones[n / 8] |= staticBitOpNumber[n % 8];
		m_charArrayBoard[ompi.y - 1][ompi.x - 1] = m_myChessPlaySide;
		setRoadInfos(x1, y1, m_myChessPlaySide);
		setRoadInfos(x2, y2, m_myChessPlaySide);
	}
	/*if (m_myChessPlaySide == SIDE_BLACK) cout << "Black ";
	else cout << "White ";
	cout << "Zobrist Key: " <<zobr.dwKey << ", ";
	cout << zobr.dwLock0 << ", ";
	cout << zobr.dwLock1 << endl;*/

	return ret;
}

void AbstractAI::setHistoryMoves(list<OneMovePlayInfo> historyMoves, ChessPlaySide cps)									//����������ʷ��
{
	m_myChessPlaySide = cps;						//��������ɫ
	nextChessPlaySide = cps;						//��һ������ɫ 2018-7-1
	m_listMovePlay.clear();
	initBoardInfo();
	OneMovePlayInfo ompi;
	for (list<OneMovePlayInfo>::iterator pIt = historyMoves.begin(); pIt != historyMoves.end(); ++pIt)
	{
		ompi = *pIt;
		m_listMovePlay.push_front(ompi);
	}
	if (historyMoves.size() % 4 == 1 && cps == SIDE_WHITE || historyMoves.size() % 4 == 3 && cps == SIDE_BLACK)			//��ǰ�ֵ����ߣ��۳������������Ӽ�¼�����ظ��������������Ϊ������فE�����Ϣ������E
	{
		m_listMovePlay.pop_back();
		if (m_listMovePlay.size() > 0)
		{
			m_listMovePlay.pop_back();
		}
	}
	for (list<OneMovePlayInfo>::iterator pIt = m_listMovePlay.begin(); pIt != m_listMovePlay.end(); ++pIt)
	{
		ompi = *pIt;
		int nx = 19 * (ompi.y - 1) + ompi.x - 1;
		m_cbiChessBoard.board_of_rival_stones[nx / 8] |= staticBitOpNumber[nx % 8];
		m_charArrayBoard[ompi.y - 1][ompi.x - 1] = ompi.chessPlaySide;
		int x = ompi.x;
		int y = ompi.y;
		zobr.Xor(Zobrist.Table[ompi.chessPlaySide - 1][nx]);  //hash for history move 2018-7-1
		setCanPutStone(x, y);
	}
	//********��ʷ��Ϣ���������ᱻɾ����ɾ���Ĳ��ֻ���getNextMoveFromAI�������ԡ��з��ߵ��塱����ʽ���� 2018-7-1
	if(m_myChessPlaySide == SIDE_BLACK && historyMoves.size() != 0) zobr.Xor(Zobrist.Player);  //hash for Black to move 2018-7-1
	initRoadInfo();
}


void AbstractAI::initRoadInfo(void)																//��ʼ��·��Ϣ
{
	memset(&m_riRoadInfo, 0, sizeof(RoadInfo) - sizeof(map<int, int>) * 2);
	m_riRoadInfo.blackScore = 0;
	m_riRoadInfo.whiteScore = 0;
	m_riRoadInfo.blackRoads.clear();
	m_riRoadInfo.whiteRoads.clear();
	m_tpiBlackThreatRoadInfos.clear();
	m_tpiWhiteThreatRoadInfos.clear();
	m_tpiBlackThreatPointInfos.clear();
	m_tpiWhiteThreatPointInfos.clear();
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
			stone = m_charArrayBoard[y][x];
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
			m_riRoadInfo.blackRoads.insert(pair<int, int>(i, i));
			m_riRoadInfo.blackScore += shapesScore[blackRoad];
			setThreatInfo(blackRoad, i, SIDE_BLACK);
		}
		else if (blackRoad == 0 && whiteRoad != 0)
		{
			m_riRoadInfo.whiteRoads.insert(pair<int, int>(i, i));
			m_riRoadInfo.whiteScore += shapesScore[whiteRoad];
			setThreatInfo(whiteRoad, i, SIDE_WHITE);
		}
	}
}

void AbstractAI::changePlaySide()
{
	nextChessPlaySide = nextChessPlaySide == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
}

void AbstractAI::setCanPutStone(int x, int y)
{
	int i = y - 1 - 2;
	i = i >= 0 ? i : 0;
	int ii = y - 1 + 2;
	ii = ii <= 18 ? ii : 18;
	int j = x - 1 - 2;
	j = j >= 0 ? j : 0;
	int jj = x - 1 + 2;
	int jjj = j;
	jj = jj <= 18 ? jj : 18;
	for (; i <= ii; i++)
	{
		for (j = jjj; j <= jj; j++)
		{
			m_charArrayCanPutStone[i][j]++;														//����������������������
		}
	}
	
	//********** show the CanPutStoneArea 2018-6-30
	/*for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (m_charArrayCanPutStone[i][j] > 0)
			{
				cout << (char)(m_charArrayCanPutStone[i][j] + '0') << " ";
			}
			else
				cout << ". ";
		}
		cout << endl;
	}
	cout << endl;*/
}

void AbstractAI::eraseCanPutStone(int x, int y)
{
	int i = y - 1 - 2;
	i = i >= 0 ? i : 0;
	int ii = y - 1 + 2;
	ii = ii <= 18 ? ii : 18;
	int j = x - 1 - 2;
	j = j >= 0 ? j : 0;
	int jj = x - 1 + 2;
	jj = jj <= 18 ? jj : 18;
	int jjj = j;
	jj = jj <= 18 ? jj : 18;

	for (; i <= ii; i++)
	{
		for (j = jjj; j <= jj; j++)
		{
			m_charArrayCanPutStone[i][j]--;														//����������������������
			
		}
	}
}

void AbstractAI::setRoadInfos(int x, int y, ChessPlaySide cps)									//�����������޸�·��Ϣ
{
	m_charArrayBoard[y - 1][x - 1] = cps;

	setCanPutStone(x, y);

	int roadIndex = (y - 1) * 19 + (x - 1);
	for (int i = 0; i < m_riArrayR_Roads[roadIndex].count; i++)
	{
		const R_RoadItem* pRri = &m_riArrayR_Roads[roadIndex].r_RoadItems[i];
		RoadItem* pRi = &m_riRoadInfo.roadItems[pRri->road_no];
		int flag = 0;
		if (pRi->blackRoad != 0 && pRi->whiteRoad == 0)											//�жϸ�·��������ɫ
		{
			eraseThreatInfo(pRi->blackRoad, pRri->road_no, SIDE_BLACK);
			m_riRoadInfo.blackScore -= shapesScore[pRi->blackRoad];								//��·���ͽ������ı䣬�ȴ��ܷ��н�ԭ�����ͷ�ֵ����
			m_riRoadInfo.blackRoads.erase(pRri->road_no);										//��·���ͽ������ı䣬��ȷ����·�����ʲôɫ���ȴ�map��ɾ��
			flag = 1;																			//��¼��·δ�޸�ǰ����ɫ״̬
		}
		else if (pRi->blackRoad == 0 && pRi->whiteRoad != 0)
		{
			eraseThreatInfo(pRi->whiteRoad, pRri->road_no, SIDE_WHITE);
			m_riRoadInfo.whiteScore -= shapesScore[pRi->whiteRoad];
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
			flag = 2;
		}
		if (cps == SIDE_BLACK)
		{
			pRi->blackRoad |= staticBitOpNumber[pRri->at_pos];									//���ӣ��޸�·����
		}
		else if (cps == SIDE_WHITE)
		{
			pRi->whiteRoad |= staticBitOpNumber[pRri->at_pos];
		}
		if (pRi->blackRoad != 0 && pRi->whiteRoad == 0)
		{
			m_riRoadInfo.blackRoads.insert(pair<int, int>(pRri->road_no, pRri->road_no));		//���Ӻ󣬸�·Ϊ��ɫ·������£���ȁE���?
			m_riRoadInfo.blackScore += shapesScore[pRi->blackRoad];								//���Ӻ󣬸�·Ϊ��ɫ·������£��ּܷ��������͵ķ�ֵ
			setThreatInfo(pRi->blackRoad, pRri->road_no, SIDE_BLACK);							//���Ӻ󣬸�·Ϊ��ɫ·������£���������·
		}
		else if (pRi->blackRoad == 0 && pRi->whiteRoad != 0)
		{
			m_riRoadInfo.whiteRoads.insert(pair<int, int>(pRri->road_no, pRri->road_no));
			m_riRoadInfo.whiteScore += shapesScore[pRi->whiteRoad];
			setThreatInfo(pRi->whiteRoad, pRri->road_no, SIDE_WHITE);
		}
		else if (flag > 0 && pRi->blackRoad != 0 && pRi->whiteRoad != 0)
		{
			if (flag == 1)
			{
				eraseThreatInfo(pRi->blackRoad, pRri->road_no, SIDE_BLACK);						//����޸ĸ�·���ͺ�·�ӵ�ɫ��Ϊ˫ɫ����ɾ������·��Ϣ
			}
			else if (flag == 2)
			{
				eraseThreatInfo(pRi->whiteRoad, pRri->road_no, SIDE_WHITE);
			}
		}
	}

}


void AbstractAI::eraseRoadInfos(int x, int y, ChessPlaySide cps)								//����ɾ�������޸�·��Ϣ
{
	m_charArrayBoard[y - 1][x - 1] = 0;

	//*************should add a function eraseCanPutStone(int,int) 2018-6-30
	eraseCanPutStone(x, y);

	int roadIndex = (y - 1) * 19 + (x - 1);
	for (int i = 0; i < m_riArrayR_Roads[roadIndex].count; i++)
	{
		const R_RoadItem* pRri = &m_riArrayR_Roads[roadIndex].r_RoadItems[i];
		RoadItem* pRi = &m_riRoadInfo.roadItems[pRri->road_no];
		int flag = 0;
		if (pRi->blackRoad != 0 && pRi->whiteRoad == 0)											//����ǵ�ɫ·
		{
			eraseThreatInfo(pRi->blackRoad, pRri->road_no, SIDE_BLACK);							//ɾ����·������Ϣ
			m_riRoadInfo.blackScore -= shapesScore[pRi->blackRoad];
			m_riRoadInfo.blackRoads.erase(pRri->road_no);
		}
		else if (pRi->blackRoad == 0 && pRi->whiteRoad != 0)
		{
			eraseThreatInfo(pRi->whiteRoad, pRri->road_no, SIDE_WHITE);
			m_riRoadInfo.whiteScore -= shapesScore[pRi->whiteRoad];
			m_riRoadInfo.whiteRoads.erase(pRri->road_no);
		}
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
			m_riRoadInfo.blackScore += shapesScore[pRi->blackRoad];
			setThreatInfo(pRi->blackRoad, pRri->road_no, SIDE_BLACK);							//ɾ�����Ӻ������·Ϊ��ɫ״̬����������������Ϣ
			m_riRoadInfo.blackRoads.insert(pair<int, int>(pRri->road_no, pRri->road_no));		
		}
		else if (pRi->blackRoad == 0 && pRi->whiteRoad != 0)
		{
			m_riRoadInfo.whiteScore += shapesScore[pRi->whiteRoad];
			setThreatInfo(pRi->whiteRoad, pRri->road_no, SIDE_WHITE);
			m_riRoadInfo.whiteRoads.insert(pair<int, int>(pRri->road_no, pRri->road_no));
		}
	}
}

void AbstractAI::setThreatInfo(char road, int road_no, ChessPlaySide cps)
{
	map<int, int> posPoints;																		//Ѱ�Ҳ���������·
	switch (road)
	{
	case 15://001111 15
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 23://010111 23
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 27://011011 27
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 29://011101 29
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 30://011110 30
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 31://011111 31
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 39://100111 39
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 43://101011 43
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 45://101101 45
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 46://101110 46
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 47://101111 46
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 51://110011 51
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 53://110101 53
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 54://110110 54
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 55://110111 55
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 57://111001 57
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		break;
	case 58://111010 58
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		break;
	case 59://111011 59
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		break;
	case 60://111100 60
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		break;
	case 61://111101 61
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		break;
	case 62://111110 62
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		break;
	default:
		return;
	}
	list<ThreatPointInfo>* pThreatPointInfos = NULL;
	list<ThreatRoadInfo>* pThreatRoadInfos = NULL;
	if (cps == SIDE_BLACK)
	{
		pThreatPointInfos = &this->m_tpiBlackThreatPointInfos;
		pThreatRoadInfos = &this->m_tpiBlackThreatRoadInfos;
	}
	else if (cps == SIDE_WHITE)
	{
		pThreatPointInfos = &this->m_tpiWhiteThreatPointInfos;
		pThreatRoadInfos = &this->m_tpiWhiteThreatRoadInfos;
	}	
	map<int, int>::iterator itp;
	for (itp = posPoints.begin(); itp != posPoints.end(); ++itp)								//�������ŵ��Ƿ������������Ϣ����
	{
		list<ThreatPointInfo>::iterator it;
		for (it = pThreatPointInfos->begin(); it != pThreatPointInfos->end(); ++it)
		{
			if (it->threatPoint == itp->first)													//�ҵ����ŵ��Ѿ�������������Ϣ����
			{
				list<int>::iterator pIt;
				for (pIt = it->threatRoads.begin(); pIt != it->threatRoads.end(); ++pIt)		//���Ҳ��������ŵ��·�Ƿ�����ڸ����ŵ�Ӱ���·��
				{
					if (*pIt == road_no)														//����Ѿ����ڣ�������
					{
						break;
					}
				}
				if (pIt == it->threatRoads.end())												//��������ڣ��ͽ����������ŵ��·�ӵ������ŵ�ӰӰ���·��
				{
					it->threatRoads.push_back(road_no);
				}
				break;
			}
		}
		if (it == pThreatPointInfos->end())														//������ŵ㲻������������Ϣ���оͽ������ŵ���Ϣ�ӵ�������Ϣ����
		{
			ThreatPointInfo tpi;
			tpi.threatPoint = itp->first;
			tpi.threatRoads.push_back(road_no);
			pThreatPointInfos->push_back(tpi);
		}
		list<ThreatRoadInfo>::iterator itThreatRoads;
		for (itThreatRoads = pThreatRoadInfos->begin(); itThreatRoads != pThreatRoadInfos->end(); ++itThreatRoads)
		{
			if (road_no == itThreatRoads->threatRoad)											//��������·��
			{
				list<int>::iterator itThreatPoints;
				for (itThreatPoints = itThreatRoads->threatPoints.begin(); itThreatPoints != itThreatRoads->threatPoints.end(); ++itThreatPoints)
				{
					if (*itThreatPoints == itp->first)											
					{
						break;
					}
				}
				if (itThreatPoints == itThreatRoads->threatPoints.end())						//��û�м�¼�������ŵ㣬��Ҫ����
				{
					itThreatRoads->threatPoints.push_back(itp->first);
				}
				break;
			}
		}
		if (itThreatRoads == pThreatRoadInfos->end())
		{
			ThreatRoadInfo tri;
			tri.threatRoad = road_no;
			tri.threatPoints.push_back(itp->first);
			pThreatRoadInfos->push_back(tri);
		}
	}	
}

void AbstractAI::eraseThreatInfo(char road, int road_no, ChessPlaySide cps)
{	
	map<int, int> posPoints;																		//Ѱ�Ҳ���������·
	switch (road)
	{
	case 15://001111 15
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 23://010111 23
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 27://011011 27
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 29://011101 29
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 30://011110 30
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 31://011111 31
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][5], m_iArrayRoads[road_no][5]));
		break;
	case 39://100111 39
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 43://101011 43
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 45://101101 45
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 46://101110 46
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 47://101111 46
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][4], m_iArrayRoads[road_no][4]));
		break;
	case 51://110011 51
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 53://110101 53
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 54://110110 54
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 55://110111 55
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][3], m_iArrayRoads[road_no][3]));
		break;
	case 57://111001 57
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		break;
	case 58://111010 58
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		break;
	case 59://111011 59
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][2], m_iArrayRoads[road_no][2]));
		break;
	case 60://111100 60
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		break;
	case 61://111101 61
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][1], m_iArrayRoads[road_no][1]));
		break;
	case 62://111110 62
		posPoints.insert(pair<int, int>(m_iArrayRoads[road_no][0], m_iArrayRoads[road_no][0]));
		break;
	default:
		return;
	}
	list<ThreatPointInfo>* pThreatPointInfos = NULL;
	list<ThreatRoadInfo>* pThreatRoadInfos = NULL;
	if (cps == SIDE_BLACK)
	{
		pThreatPointInfos = &this->m_tpiBlackThreatPointInfos;
		pThreatRoadInfos = &this->m_tpiBlackThreatRoadInfos;
	}
	else if (cps == SIDE_WHITE)
	{
		pThreatPointInfos = &this->m_tpiWhiteThreatPointInfos;
		pThreatRoadInfos = &this->m_tpiWhiteThreatRoadInfos;
	}
	map<int, int>::iterator itp;
	for (itp = posPoints.begin(); itp != posPoints.end(); ++itp)								//�������ŵ��Ƿ������������Ϣ��
	{
		list<ThreatPointInfo>::iterator it;
		for (it = pThreatPointInfos->begin(); it != pThreatPointInfos->end(); ++it)
		{
			if (it->threatPoint == itp->first)													//�ҵ����ŵ��Ѿ�������������Ϣ��
			{
				list<int>::iterator pIt;
				for (pIt = it->threatRoads.begin(); pIt != it->threatRoads.end(); ++pIt)
				{
					if (*pIt == road_no)														//�ҵ������ŵ�Ӱ�������·
					{
						it->threatRoads.erase(pIt);												//�ڸ����ŵ�Ӱ�������·
						break;
					}
				}
				if (it->threatRoads.size() == 0)												//��������ŵ�Ӱ�������·��Ϊ0��˵������ɾ�������ŵ���Ϣ
				{
					pThreatPointInfos->erase(it);
					break;
				}
			}
		}
		list<ThreatRoadInfo>::iterator itThreatRoads;
		for (itThreatRoads = pThreatRoadInfos->begin(); itThreatRoads != pThreatRoadInfos->end(); ++itThreatRoads)
		{
			if (road_no == itThreatRoads->threatRoad)											//��������·��
			{
				list<int>::iterator itThreatPoints;
				for (itThreatPoints = itThreatRoads->threatPoints.begin(); itThreatPoints != itThreatRoads->threatPoints.end(); ++itThreatPoints)
				{
					if (*itThreatPoints == itp->first)											
					{
						itThreatRoads->threatPoints.erase(itThreatPoints);
						break;
					}
				}
				if (itThreatRoads->threatPoints.size() == 0)									//���������·�����ŵ������ˣ���ô˵��������·����ɾ����
				{
					pThreatRoadInfos->erase(itThreatRoads);
				}
				break;
			}
		}
	}
}

//_int64 AbstractAI::rand64()
//{
//	return rand() ^ ((_int64)rand() << 15) ^ ((_int64)rand() << 30) ^ ((_int64)rand() << 45) ^ ((_int64)rand() << 60);
//}
