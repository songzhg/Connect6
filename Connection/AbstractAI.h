#include "OneMovePlayInfo.h"
#pragma once
#include <list>
#include <vector>
#include <map>
#include <string>
#include "Zobrist.h"

#define MAXVALUE 10000000

using namespace std;

typedef struct _ChessBoardInfo
{
	char board_of_own_stones[46];
	char board_of_rival_stones[46];
}ChessBoardInfo;

typedef struct _RoadItem
{
	char blackRoad;
	char whiteRoad;
}RoadItem;

typedef struct _RoadInfo
{
	RoadItem roadItems[924];
	int blackScore;
	int whiteScore;
	map<int, int> blackRoads;
	map<int, int> whiteRoads;
}RoadInfo;

typedef struct _R_RoadItem
{
	int road_no;
	int at_pos;
}R_RoadItem;

typedef struct _R_RoadInfo
{
	int count;
	R_RoadItem r_RoadItems[24];
}R_RoadInfo;

typedef struct _ThreatRoadInfo
{
	int threatRoad;
	list<int> threatPoints;

}ThreatRoadInfo;

typedef struct _ThreatPointInfo
{
	int threatPoint;
	list<int> threatRoads;
}ThreatPointInfo;

typedef struct _Threat
{
	list<int> threatRoads;
	list<int> threatPoints;
}Threat;

class AbstractAI
{

public:
	AbstractAI(void);
	virtual ~AbstractAI(void);
	list<OneMovePlayInfo> getNextMoveFromAI(list<OneMovePlayInfo> rivalLastMove);				//��AI����õ�����λ��
	void setHistoryMoves(list<OneMovePlayInfo> historyMoves, ChessPlaySide cps);				//����������������Ϣ
	virtual void unDoStone(int x, int y, ChessPlaySide cps);								    //��һ��
	virtual void putStone(int x, int y, ChessPlaySide cps);                                     //��һ��
	void makeOneMove(int x1, int y1, int x2, int y2);						//��һ��
	void unDoOneMove(int x1, int y1, int x2, int y2);						//��һ��
	void setSearchDepth(int searchDepth);
	void setSearchBranchThreshhold(int threshhold);
	void setTimeout(int timeout);

private:
	void setCanPutStone(int x, int y);
	void eraseCanPutStone(int x, int y);

protected:
	virtual bool loadInitLib();																	//��ȡ���ֿ�
	ChessPlaySide m_myChessPlaySide;															//�ѷ���ɫ
	ChessPlaySide nextChessPlaySide;												            //��һ������ɫ
	ChessBoardInfo m_cbiChessBoard;																//λ��澵�������Ϣ
	list<OneMovePlayInfo> m_listMovePlay;														//������Ϣlist
	char m_charArrayBoard[19][19];																//��ά���鱣���������Ϣ
	char m_charArrayCanPutStone[19][19];														//��ά���鱣�������������
	int m_iSearchDepth;																			//��������
	int m_iSearchBranchThreshhold;																//������֧��
	int m_iTimeout;																				//����ʱ��
	list<ThreatRoadInfo> m_tpiBlackThreatRoadInfos;												//����·
	list<ThreatRoadInfo> m_tpiWhiteThreatRoadInfos;
	list<ThreatPointInfo> m_tpiBlackThreatPointInfos;											//�����ŵ�
	list<ThreatPointInfo> m_tpiWhiteThreatPointInfos;
	RoadInfo m_riRoadInfo;
	ZobristStruct zobr;
	ZobristTable Zobrist;
	int getScore(ChessPlaySide cps);

	virtual void run(int& x1, int& y1, int& x2, int& y2) = 0;									//��������λ�ù���
	void initBoardInfo();																		//��ʼ������	
	void initRoadInfo(void);
	void changePlaySide();
	virtual void setRoadInfos(int x, int y, ChessPlaySide cps);					//������run�еݹ�����ʱ���ݳ��Ե�x,y����·�������Ϣ
	virtual void eraseRoadInfos(int x, int y, ChessPlaySide cps);				//������run�еݹ�����ʱ���ݳ��Ե�x,y����·�������Ϣ
	virtual void setThreatInfo(char road, int road_no, ChessPlaySide cps);
	virtual void eraseThreatInfo(char road, int road_no, ChessPlaySide cps);
	static const int m_iArrayRoads[924][6];
	static const R_RoadInfo m_riArrayR_Roads[361];
	static const char staticBitOpNumber[8];	
	static int shapesScore[64];
	static int staticPositonValue[19][19];


};
