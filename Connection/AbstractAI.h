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
	list<OneMovePlayInfo> getNextMoveFromAI(list<OneMovePlayInfo> rivalLastMove);				//从AI计算得到落子位置
	void setHistoryMoves(list<OneMovePlayInfo> historyMoves, ChessPlaySide cps);				//批量设置已落子信息
	virtual void unDoStone(int x, int y, ChessPlaySide cps);								    //悔一棋
	virtual void putStone(int x, int y, ChessPlaySide cps);                                     //落一子
	void makeOneMove(int x1, int y1, int x2, int y2);						//走一步
	void unDoOneMove(int x1, int y1, int x2, int y2);						//悔一步
	void setSearchDepth(int searchDepth);
	void setSearchBranchThreshhold(int threshhold);
	void setTimeout(int timeout);

private:
	void setCanPutStone(int x, int y);
	void eraseCanPutStone(int x, int y);

protected:
	virtual bool loadInitLib();																	//读取开局库
	ChessPlaySide m_myChessPlaySide;															//已方颜色
	ChessPlaySide nextChessPlaySide;												            //下一步的颜色
	ChessBoardInfo m_cbiChessBoard;																//位柄婢的棋盘信息
	list<OneMovePlayInfo> m_listMovePlay;														//落子信息list
	char m_charArrayBoard[19][19];																//二维数组保存的棋盘信息
	char m_charArrayCanPutStone[19][19];														//二维数组保存的搜索覆盖区
	int m_iSearchDepth;																			//搜索层数
	int m_iSearchBranchThreshhold;																//搜索分支数
	int m_iTimeout;																				//搜索时限
	list<ThreatRoadInfo> m_tpiBlackThreatRoadInfos;												//迫着路
	list<ThreatRoadInfo> m_tpiWhiteThreatRoadInfos;
	list<ThreatPointInfo> m_tpiBlackThreatPointInfos;											//解迫着点
	list<ThreatPointInfo> m_tpiWhiteThreatPointInfos;
	RoadInfo m_riRoadInfo;
	ZobristStruct zobr;
	ZobristTable Zobrist;
	int getScore(ChessPlaySide cps);

	virtual void run(int& x1, int& y1, int& x2, int& y2) = 0;									//计算落子位置过程
	void initBoardInfo();																		//初始化棋盘	
	void initRoadInfo(void);
	void changePlaySide();
	virtual void setRoadInfos(int x, int y, ChessPlaySide cps);					//可以在run中递归搜索时根据尝试的x,y设置路中相关信息
	virtual void eraseRoadInfos(int x, int y, ChessPlaySide cps);				//可以在run中递归搜索时根据尝试的x,y擦除路中相关信息
	virtual void setThreatInfo(char road, int road_no, ChessPlaySide cps);
	virtual void eraseThreatInfo(char road, int road_no, ChessPlaySide cps);
	static const int m_iArrayRoads[924][6];
	static const R_RoadInfo m_riArrayR_Roads[361];
	static const char staticBitOpNumber[8];	
	static int shapesScore[64];
	static int staticPositonValue[19][19];


};
