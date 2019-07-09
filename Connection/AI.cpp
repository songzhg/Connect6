#include "stdafx.h"
#include "Solver.h"
#include "AI.h"
#include <iostream>

using namespace std;


AI::AI(void)
{
}

AI::~AI(void)
{
}

int AI::searchFull(int vlAlpha, int vlBeta, int nDepth)
{
	if (int hashvl = findHash(vlAlpha, vlBeta, nDepth) != - MAXVALUE - 100 ) return hashvl;

	// 1. 到达水平线，则调用静态搜索(注意：由于空步裁剪，深度可能小于零)

	if (VCDT::searchFull(VCDT::VCDT_DEPTH)) return MAXVALUE;

	//if (nDepth == MAXDEPTH - 1)
/*	{
		VCST::clearHash();
		if (VCST::searchFull(VCST::VCST_DEPTH))
		{
			recordHash(3, MAXVALUE, nDepth);
			return MAXVALUE;
		}
	}
	else*/
	if (nDepth == 0)
	{
		/*VCDT::clearHash();
		if (VCDT::searchFull(VCDT::VCDT_DEPTH))
		{
			recordHash(3, MAXVALUE, nDepth);
			return MAXVALUE;
		}*/
		recordHash(3, getScore(nextChessPlaySide), nDepth);
		return getScore(nextChessPlaySide);
	}
	/*else 
	{
		VCDT::clearHash();
		if (VCDT::searchFull(VCDT::VCDT_DEPTH))
		{
			recordHash(3, MAXVALUE, nDepth);
			return MAXVALUE;
		}	
	}*/

	int vlBest = -MAXVALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
	int vl;
	// 3. 初始化走法排序结构
	list<OneHandInfo> pl;
	generateMoves(&pl, nextChessPlaySide);

	list<OneHandInfo>::iterator itb = pl.begin(); 

	int nHashFlag = 1;
	int i = 0;
	for (list<OneHandInfo>::iterator it = pl.begin(); it != pl.end(); it++)
	{
		if (i >= 7) break;
		i++;
		/*if (vlBest != -MAXVALUE)
		{
			if (i >= 1) break;
			
		}
		i++;*/
		makeOneMove(it->x1, it->y1, it->x2, it->y2);

		/*for (int k = 0; k<19; k++)
		{
			for (int l = 0; l<19; l++)
			{
				if (m_charArrayBoard[k][l] == SIDE_BLACK)
					cout << "@ ";
				else if (m_charArrayBoard[k][l] == SIDE_WHITE)
					cout << "O ";
				else cout << ". ";
			}
			cout << "\n";
		}
		cout << "\n";*/

		if (vlBest == -MAXVALUE)
		{
			vl = -searchFull(-vlBeta, -vlAlpha, nDepth - 1);
		}
		else
		{
			vl = -searchFull(-vlAlpha - 1, -vlAlpha, nDepth - 1);
			if (vl > vlAlpha && vl < vlBeta)
			{
				vl = -searchFull(-vlBeta, -vlAlpha, nDepth - 1);
			}
		}
		unDoOneMove(it->x1, it->y1, it->x2, it->y2);

		// 5. 进行Alpha-Beta大小判断和截断
		if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
			vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
			if (vl >= vlBeta) { // 找到一个Beta走法
				nHashFlag = 2;
				itb = it;      // Beta走法要保存到历史表
				break;            // Beta截断
			}
			if (vl > vlAlpha) { // 找到一个PV走法
				nHashFlag = 3;
				itb = it;      // PV走法要保存到历史表
				vlAlpha = vl;     // 缩小Alpha-Beta边界
			}
		}
	}

	recordHash(nHashFlag, vlBest, nDepth);
	return vlBest;
}

int AI::searchRoot(int nDepth)
{

	int vlBest = -MAXVALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
	int vl;
	// 3. 初始化走法排序结构
	list<OneHandInfo> pl;
	generateMoves(&pl, nextChessPlaySide);
	//MAXBRANCH = 10;
	list<OneHandInfo>::iterator itb = pl.begin();       // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

	int i = 0;
	for (list<OneHandInfo>::iterator it = pl.begin(); it != pl.end(); it++)
	{
		if (vlBest != -MAXVALUE)
		{
			if (i >= 7) break;
			
		}
		i++;

		makeOneMove(it->x1, it->y1, it->x2, it->y2);


		/*for (int k = 0; k<19; k++)
		{
		for (int l = 0; l<19; l++)
		{
		if (m_charArrayBoard[k][l] == SIDE_BLACK)
		cout << "@ ";
		else if (m_charArrayBoard[k][l] == SIDE_WHITE)
		cout << "O ";
		else cout << ". ";
		}
		cout << "\n";
		}
		cout << "\n";*/

		if (vlBest == -MAXVALUE) {
			vl = -searchFull(-MAXVALUE, MAXVALUE, nDepth - 1);
		}
		else {
			vl = -searchFull(-vlBest - 1, -vlBest, nDepth - 1);
			if (vl > vlBest) {
				vl = -searchFull(-MAXVALUE, -vlBest, nDepth - 1);
			}
		}
		unDoOneMove(it->x1, it->y1, it->x2, it->y2);

		cout << vl << " ";

		if (vl == MAXVALUE)
		{
			itb = it;
			break;
		}

		// 5. 进行Alpha-Beta大小判断和截断
		if (vl > vlBest) {
			vlBest = vl;
			itb = it;
		}
	}
	cout << endl;

	resX1 = itb->x1;
	resY1 = itb->y1;
	resX2 = itb->x2;
	resY2 = itb->y2;

	return vlBest;
}

void AI::generateMoves(list<OneHandInfo>* pl, ChessPlaySide cps)
{
	char ReleBoard[19][19][19][19] = { 0 };
	OneHandInfo ohi;
	map<int, int>::iterator it;
	list<ThreatPointInfo>* pOwnThreat = cps == SIDE_BLACK ? &m_tpiBlackThreatPointInfos : &m_tpiWhiteThreatPointInfos;
	list<ThreatPointInfo>* pRivalThreat = cps == SIDE_BLACK ? &m_tpiWhiteThreatPointInfos : &m_tpiBlackThreatPointInfos;
	list<ThreatRoadInfo>* pOwnThreatRoad = cps == SIDE_BLACK ? &m_tpiBlackThreatRoadInfos : &m_tpiWhiteThreatRoadInfos;
	list<ThreatRoadInfo>* pRivalThreatRoad = cps == SIDE_BLACK ? &m_tpiWhiteThreatRoadInfos : &m_tpiBlackThreatRoadInfos;
	//map<int, int>* pRoads = cps == SIDE_BLACK ? &this->m_riRoadInfo.blackRoads : &this->m_riRoadInfo.whiteRoads;
	map<int, int>* pRoads;
	list<pair<int, int>> PosPoints1;
	list<pair<int, int>> PosPoints2;

	if (pOwnThreatRoad->size() > 0)																										//Threat
	{
		ThreatRoadInfo tri = pOwnThreatRoad->front();
		if (tri.threatPoints.size() > 0)
		{
			int index = tri.threatPoints.front();
			ohi.x1 = index % 19;
			ohi.y1 = index / 19;
		}
		if (tri.threatPoints.size() > 1)
		{
			list<int>::iterator it = tri.threatPoints.begin();
			it++;
			int index = *it;
			ohi.x2 = index % 19;
			ohi.y2 = index / 19;
			ohi.cps = cps;
			ohi.score = MAXVALUE;
			pl->push_back(ohi);
		}
		else if (tri.threatPoints.size() == 1)
		{
			ohi.x2 = -1;
			ohi.y2 = -1;
			ohi.cps = cps;
			ohi.score = MAXVALUE;
			pl->push_back(ohi);
		}
		return;
	}

	//********防守对方Threat 2018-7-3
	if (pRivalThreatRoad->size() > 0)
	{
		list<pair<int, int>> stonePairs;
		list<ThreatPointInfo>::iterator pIt;

		//********单子防守Threat 2018-7-3
		BOOL singleThreat = false;
		for (pIt = pRivalThreat->begin(); pIt != pRivalThreat->end(); ++pIt)
		{
			if (pIt->threatRoads.size() == pRivalThreatRoad->size())
			{
				stonePairs.push_back(pair<int, int>(pIt->threatPoint % 19, pIt->threatPoint / 19));
				singleThreat = true;
			}
		}

		//********单子防守Threat 2018-7-3
		if (singleThreat)
		{
			for (list<pair<int, int>>::iterator spIt = stonePairs.begin(); spIt != stonePairs.end(); ++spIt)
			{
				PosPoints2.clear();

				//********第一个子防守Threat 2018-7-3
				int x1 = spIt->first;
				int y1 = spIt->second;

				if (m_charArrayBoard[y1][x1] == 0)
				{
					putStone(x1, y1, cps);
					
					//********第二个子攻 2018-7-7
					pRoads = &this->m_riRoadInfo.blackRoads;
					for (it = pRoads->begin(); it != pRoads->end(); it++)
					{
						int road_no = it->first;
						//char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
						char road = m_riRoadInfo.roadItems[road_no].blackRoad;
						switch (road)
						{
						case 1:												//000001 1
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 2:												//000010 2
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 3:												//000011 3
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 4:												//000100 4
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 5:												//000101 5
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 6:												//000110 6
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 7:												//000111 7
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 8:												//001000 8
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 9:												//001001 9
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 10:											//001010 10
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 11:											//001011 11
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 12:											//001100 12
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 13:											//001101 13
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 14:											//001110 14
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 16:											//010000 16
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 17:											//010001 17
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 18:											//010010 18
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 19:											//010011 19
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 20:											//010100 20
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 21:											//010101 21
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 22:											//010110 22
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							break;
						case 24:											//011000 24
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 25:											//011001 25
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 26:											//011010 26
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							break;
						case 28:											//011100 28
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 32:											//100000 32
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 33:											//100001 33
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 34:											//100010 34
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 35:											//100011 35
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 36:											//100100 36
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 37:											//100101 37
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 38:											//100110 38
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 40:											//101000 40
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 41:											//101001 41
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 42:											//101010 42
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 44:											//101100 44
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 48:											//110000 48
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							break;
						case 49:											//110001 49
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 50:											//110010 50
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 52:											//110100 52
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 56:											//111000 56
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						}
					}
					

					//********第二个子守 2018-7-7
					pRoads = &this->m_riRoadInfo.whiteRoads;
					for (it = pRoads->begin(); it != pRoads->end(); it++)
					{
						int road_no = it->first;
						//char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
						char road = m_riRoadInfo.roadItems[road_no].whiteRoad;
						switch (road)
						{
						case 1:												//000001 1
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 2:												//000010 2
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 3:												//000011 3
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 4:												//000100 4
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 5:												//000101 5
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 6:												//000110 6
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 7:												//000111 7
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 8:												//001000 8
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 9:												//001001 9
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 10:											//001010 10
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 11:											//001011 11
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 12:											//001100 12
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 13:											//001101 13
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 14:											//001110 14
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 16:											//010000 16
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 17:											//010001 17
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 18:											//010010 18
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 19:											//010011 19
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 20:											//010100 20
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 21:											//010101 21
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 22:											//010110 22
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							break;
						case 24:											//011000 24
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
						case 25:											//011001 25
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 26:											//011010 26
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							break;
						case 28:											//011100 28
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 32:											//100000 32
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 33:											//100001 33
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 34:											//100010 34
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 35:											//100011 35
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 36:											//100100 36
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 37:											//100101 37
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 38:											//100110 38
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 40:											//101000 40
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
						case 41:											//101001 41
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 42:											//101010 42
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 44:											//101100 44
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 48:											//110000 48
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							break;
						case 49:											//110001 49
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							break;
						case 50:											//110010 50
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 52:											//110100 52
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						case 56:											//111000 56
																			//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							break;
						}
					}
				}

				for (list<pair<int, int>>::iterator it2 = PosPoints2.begin(); it2 != PosPoints2.end(); it2++)
				{
					int y2 = it2->first / 19;
					int x2 = it2->first % 19;
					if (m_charArrayBoard[y2][x2] == 0 && ReleBoard[y1][x1][y2][x2] == 0 && ReleBoard[y2][x2][y1][x1] == 0
						&& m_charArrayCanPutStone[y2][x2] > 0)
					{
						ReleBoard[y1][x1][y2][x2] = 1;
						ReleBoard[y2][x2][y1][x1] = 1;

						putStone(x2, y2, cps);

						ohi.x1 = x1;
						ohi.y1 = y1;
						ohi.x2 = x2;
						ohi.y2 = y2;
						ohi.cps = cps;
						ohi.score = getScore(cps);
						pl->push_back(ohi);

						unDoStone(x2, y2, cps);
					}
				}
				unDoStone(x1, y1, cps);
			}
			pl->sort();
			if (pl->size() > MAXBRANCH) pl->resize(MAXBRANCH);
			return;
		}
		else
		{
			//********双子防守Threat 2018-7-3
			stonePairs.clear();
			for (pIt = pRivalThreat->begin(); pIt != pRivalThreat->end(); ++pIt)													//DTDT
			{
				list<ThreatPointInfo>::iterator pIt2 = pIt;
				++pIt2;
				for (; pIt2 != pRivalThreat->end(); ++pIt2)
				{
					list<int>::iterator it3;
					list<int>::iterator it4;

					int count = 0;
					count = pIt->threatRoads.size();

					for (it3 = pIt2->threatRoads.begin(); it3 != pIt2->threatRoads.end(); ++it3)
					{
						for (it4 = pIt->threatRoads.begin(); it4 != pIt->threatRoads.end(); ++it4)
						{
							if (*it4 == *it3)
							{
								break;
							}
						}
						if (it4 == pIt->threatRoads.end())
						{
							count++;
						}
					}
					if (pRivalThreatRoad->size() == count)
					{
						stonePairs.push_back(pair<int, int>(pIt->threatPoint, pIt2->threatPoint));
					}
				}
			}

			if (stonePairs.size() != 0)
			{
				for (list<pair<int, int>>::iterator spIt = stonePairs.begin(); spIt != stonePairs.end(); ++spIt)
				{
					ohi.x1 = spIt->first % 19;
					ohi.y1 = spIt->first / 19;
					ohi.x2 = spIt->second % 19;
					ohi.y2 = spIt->second / 19;
					ohi.cps = cps;
					ohi.score = 0;
					pl->push_back(ohi);
				}
			}
			//********双子防守Threat 招法有限 不用排序 2018-7-3
			return;
		}
	}

	
	//********第一个子攻 2018-7-7
	pRoads = &this->m_riRoadInfo.blackRoads;
	for (it = pRoads->begin(); it != pRoads->end(); it++)
	{
		int road_no = it->first;
		//char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
		char road = m_riRoadInfo.roadItems[road_no].blackRoad;
		switch (road)
		{
		case 1:												//000001 1
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 2:												//000010 2
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 3:												//000011 3
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 4:												//000100 4
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 5:												//000101 5
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 6:												//000110 6
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 7:												//000111 7
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 8:												//001000 8
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 9:												//001001 9
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 10:											//001010 10
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 11:											//001011 11
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 12:											//001100 12
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 13:											//001101 13
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 14:											//001110 14
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 16:											//010000 16
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 17:											//010001 17
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 18:											//010010 18
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 19:											//010011 19
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 20:											//010100 20
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 21:											//010101 21
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 22:											//010110 22
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			break;
		case 24:											//011000 24
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 25:											//011001 25
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 26:											//011010 26
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			break;
		case 28:											//011100 28
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 32:											//100000 32
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 33:											//100001 33
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 34:											//100010 34
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 35:											//100011 35
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 36:											//100100 36
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 37:											//100101 37
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 38:											//100110 38
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 40:											//101000 40
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 41:											//101001 41
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 42:											//101010 42
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 44:											//101100 44
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 48:											//110000 48
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			break;
		case 49:											//110001 49
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 50:											//110010 50
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 52:											//110100 52
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 56:											//111000 56
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		}
	}
	
	//********第一个子守 2018-7-7
	pRoads = &this->m_riRoadInfo.whiteRoads;
	for (it = pRoads->begin(); it != pRoads->end(); it++)
	{
		int road_no = it->first;
		//char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
		char road = m_riRoadInfo.roadItems[road_no].whiteRoad;
		switch (road)
		{
		case 1:												//000001 1
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 2:												//000010 2
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 3:												//000011 3
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 4:												//000100 4
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 5:												//000101 5
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 6:												//000110 6
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 7:												//000111 7
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 8:												//001000 8
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 9:												//001001 9
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 10:											//001010 10
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 11:											//001011 11
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 12:											//001100 12
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 13:											//001101 13
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 14:											//001110 14
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 16:											//010000 16
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 17:											//010001 17
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 18:											//010010 18
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 19:											//010011 19
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 20:											//010100 20
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 21:											//010101 21
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 22:											//010110 22
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			break;
		case 24:											//011000 24
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 25:											//011001 25
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 26:											//011010 26
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			break;
		case 28:											//011100 28
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 32:											//100000 32
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 33:											//100001 33
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 34:											//100010 34
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 35:											//100011 35
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 36:											//100100 36
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 37:											//100101 37
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 38:											//100110 38
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 40:											//101000 40
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			break;
		case 41:											//101001 41
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 42:											//101010 42
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 44:											//101100 44
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 48:											//110000 48
															//	PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			break;
		case 49:											//110001 49
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			break;
		case 50:											//110010 50
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 52:											//110100 52
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		case 56:											//111000 56
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			break;
		}
	}

	
	for (list<pair<int, int>>::iterator it1 = PosPoints1.begin(); it1 != PosPoints1.end(); it1++)
	{
		PosPoints2.clear();

		int y1 = it1->first / 19;
		int x1 = it1->first % 19;

		if (m_charArrayBoard[y1][x1] == 0 && m_charArrayCanPutStone[y1][x1] > 0)
		{
			putStone(x1, y1, cps);

			//********第二个子攻 2018-7-7
			pRoads = &this->m_riRoadInfo.blackRoads;
			for (it = pRoads->begin(); it != pRoads->end(); it++)
			{
				int road_no = it->first;
				//char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
				char road = m_riRoadInfo.roadItems[road_no].blackRoad;
				switch (road)
				{
				case 1:												//000001 1
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 2:												//000010 2
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 3:												//000011 3
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 4:												//000100 4
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 5:												//000101 5
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 6:												//000110 6
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 7:												//000111 7
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 8:												//001000 8
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 9:												//001001 9
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 10:											//001010 10
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 11:											//001011 11
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 12:											//001100 12
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 13:											//001101 13
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 14:											//001110 14
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 16:											//010000 16
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 17:											//010001 17
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 18:											//010010 18
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 19:											//010011 19
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 20:											//010100 20
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 21:											//010101 21
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 22:											//010110 22
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					break;
				case 24:											//011000 24
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 25:											//011001 25
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 26:											//011010 26
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					break;
				case 28:											//011100 28
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 32:											//100000 32
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 33:											//100001 33
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 34:											//100010 34
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 35:											//100011 35
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 36:											//100100 36
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 37:											//100101 37
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 38:											//100110 38
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 40:											//101000 40
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 41:											//101001 41
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 42:											//101010 42
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 44:											//101100 44
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 48:											//110000 48
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					break;
				case 49:											//110001 49
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 50:											//110010 50
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 52:											//110100 52
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 56:											//111000 56
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				}
			}

			//********第二个子守 2018-7-7
			pRoads = &this->m_riRoadInfo.whiteRoads;
			for (it = pRoads->begin(); it != pRoads->end(); it++)
			{
				int road_no = it->first;
				//char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
				char road = m_riRoadInfo.roadItems[road_no].whiteRoad;
				switch (road)
				{
				case 1:												//000001 1
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 2:												//000010 2
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 3:												//000011 3
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 4:												//000100 4
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 5:												//000101 5
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 6:												//000110 6
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 7:												//000111 7
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 8:												//001000 8
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 9:												//001001 9
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 10:											//001010 10
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 11:											//001011 11
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 12:											//001100 12
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 13:											//001101 13
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 14:											//001110 14
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 16:											//010000 16
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 17:											//010001 17
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 18:											//010010 18
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 19:											//010011 19
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 20:											//010100 20
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 21:											//010101 21
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 22:											//010110 22
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					break;
				case 24:											//011000 24
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					break;
				case 25:											//011001 25
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 26:											//011010 26
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					break;
				case 28:											//011100 28
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 32:											//100000 32
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 33:											//100001 33
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 34:											//100010 34
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 35:											//100011 35
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 36:											//100100 36
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 37:											//100101 37
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 38:											//100110 38
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 40:											//101000 40
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					break;
				case 41:											//101001 41
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 42:											//101010 42
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 44:											//101100 44
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 48:											//110000 48
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					break;
				case 49:											//110001 49
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					break;
				case 50:											//110010 50
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 52:											//110100 52
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				case 56:											//111000 56
																	//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					break;
				}
			}
		}
		else continue;


		for (list<pair<int, int>>::iterator it2 = PosPoints2.begin(); it2 != PosPoints2.end(); it2++)
		{
			int y2 = it2->first / 19;
			int x2 = it2->first % 19;
			if (m_charArrayBoard[y2][x2] == 0 && ReleBoard[y1][x1][y2][x2] == 0 && ReleBoard[y2][x2][y1][x1] == 0
				&& m_charArrayCanPutStone[y2][x2] > 0)
			{
				ReleBoard[y1][x1][y2][x2] = 1;
				ReleBoard[y2][x2][y1][x1] = 1;

				putStone(x2, y2, cps);

				list<pair<int, int>> stonePairs;
				list<ThreatPointInfo>::iterator pIt;

				if (pRivalThreat->size() > 0)
				{
					unDoStone(x2, y2, cps);
					continue;
				}

				ohi.x1 = x1;
				ohi.y1 = y1;
				ohi.x2 = x2;
				ohi.y2 = y2;
				ohi.cps = cps;
				ohi.score = getScore(cps);
				pl->push_back(ohi);
				unDoStone(x2, y2, cps);
			}
		}
		unDoStone(x1, y1, cps);
	}
	pl->sort();
	if (pl->size() > MAXBRANCH) pl->resize(MAXBRANCH);
	return;
}

int AI::searchMain()
{
	for (int i = 0; i < MAXDEPTH; i++)
	{
		int vl = searchRoot(i);
		if (vl >= MAXVALUE || vl <= -MAXVALUE) break;
	}
	return 0;
}

bool AI::recordHash(int flag, int depth, int vl)
{
	HashItem hsh;
	hsh = HashTable[zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.depth > 0 && hsh.depth < depth) {
		return false;
	}
	hsh.flag = flag;
	hsh.depth = depth;
	hsh.dwLock0 = zobr.dwLock0;
	hsh.dwLock1 = zobr.dwLock1;
	HashTable[zobr.dwKey & (HASH_SIZE - 1)] = hsh;
	hsh.vl = vl;
	return true;
}

void AI::clearHash()
{
	memset(HashTable, 0, HASH_SIZE * sizeof(HashItem));
}

int AI::findHash(int vlAlpha, int vlBeta, int nDepth)
{
	HashItem hsh;
	hsh = HashTable[zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.dwLock0 != zobr.dwLock0 || hsh.dwLock1 != zobr.dwLock1)
		return -MAXVALUE - 100;

	if (hsh.depth <= nDepth) {
		if (hsh.flag == 2) {
			return (hsh.vl >= vlBeta ? hsh.vl : - MAXVALUE - 100);
		}
		else if (hsh.flag == 1) {
			return (hsh.vl <= vlAlpha ? hsh.vl : -MAXVALUE - 100);
		}
		return hsh.vl;
	}
	return -MAXVALUE - 100;
}

void AI::run(int& x1, int& y1, int& x2, int& y2)
{
	VCST::clearHash();
	if (VCST::searchFull(VCST_DEPTH))
	{
		cout << "vsct success" << endl;
		x1 = VCST::resX1 + 1;
		y1 = VCST::resY1 + 1;
		x2 = VCST::resX2 + 1;
		y2 = VCST::resY2 + 1;
		return;
	}

	clearHash();
	cout << "begin: " << endl;
	searchRoot(MAXDEPTH);

	x1 = resX1 + 1;
	y1 = resY1 + 1;
	x2 = resX2 + 1;
	y2 = resY2 + 1;

}

