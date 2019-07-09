#include "stdafx.h"
#include "VCST.h"
#include "Solver.h"
#include <iostream>
#include <string>
#define MAXVALUE 50000000000
using namespace std;


Solver::Solver(void)
{
	
}

Solver::~Solver(void)
{
	
}

void Solver::run(int& x1, int& y1, int& x2, int& y2)
{
	if (VCST::searchFull(VCST_DEPTH))
	{
		cout << "vsct success" << endl;
		x1 = VCST::resX1 + 1;
		y1 = VCST::resY1 + 1;
		x2 = VCST::resX2 + 1;
		y2 = VCST::resY2 + 1;
		return;
	}
	int i = 0;
	OneHandInfo p;
	p.pn = -1.0;
	while (i < 15)
	{
		PPNS(&p);
		cout <<"\nWinRate: "<< p.pn << endl;
		cout << "Branch ";
		for (list<OneHandInfo>::iterator it = p.children.begin(); it != p.children.end(); it++)
		{
			cout << it->pn << ", ";
		}
		cout << endl;
		cout << "______________________________________________________" << endl;
		//if (p.pn == 1.0) break;
		i++;
	}

	x1 = resX1 + 1;
	y1 = resY1 + 1;
	x2 = resX2 + 1;
	y2 = resY2 + 1;
}

void Solver::generatePlayoutMoves(list<OneHandInfo>* pl, ChessPlaySide cps)
{
	char ReleBoard[19][19][19][19] = { 0 };
	OneHandInfo ohi;
	map<int, int>::iterator it;
	list<ThreatPointInfo>* pOwnThreat = cps == SIDE_BLACK ? &m_tpiBlackThreatPointInfos : &m_tpiWhiteThreatPointInfos;
	list<ThreatPointInfo>* pRivalThreat = cps == SIDE_BLACK ? &m_tpiWhiteThreatPointInfos : &m_tpiBlackThreatPointInfos;
	list<ThreatRoadInfo>* pOwnThreatRoad = cps == SIDE_BLACK ? &m_tpiBlackThreatRoadInfos : &m_tpiWhiteThreatRoadInfos;
	list<ThreatRoadInfo>* pRivalThreatRoad = cps == SIDE_BLACK ? &m_tpiWhiteThreatRoadInfos : &m_tpiBlackThreatRoadInfos;
	map<int, int>* pRoads = cps == SIDE_BLACK ? &this->m_riRoadInfo.blackRoads : &this->m_riRoadInfo.whiteRoads;

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
					//********第二个子进攻 2018-7-3
					for (it = pRoads->begin(); it != pRoads->end(); it++)
					{
						int road_no = it->first;
						char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
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

				//********第二个子进攻 2018-7-3
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
			if (pl->size() > PlayoutBranchSzie) pl->resize(PlayoutBranchSzie);
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

	for (it = pRoads->begin(); it != pRoads->end(); it++)
	{
		int road_no = it->first;
		char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
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
			for (it = pRoads->begin(); it != pRoads->end(); it++)
			{
				int road_no = it->first;
				char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
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
	if (pl->size() > PlayoutBranchSzie) pl->resize(PlayoutBranchSzie);
	return;
}

bool Solver::generateMoves(OneHandInfo* p, ChessPlaySide cps)
{
	list<OneHandInfo> plt;
	char ReleBoard[19][19][19][19] = { 0 };
	OneHandInfo ohi;
	map<int, int>::iterator it;
	list<ThreatPointInfo>* pOwnThreat = cps == SIDE_BLACK ? &m_tpiBlackThreatPointInfos : &m_tpiWhiteThreatPointInfos;
	list<ThreatPointInfo>* pRivalThreat = cps == SIDE_BLACK ? &m_tpiWhiteThreatPointInfos : &m_tpiBlackThreatPointInfos;
	list<ThreatRoadInfo>* pOwnThreatRoad = cps == SIDE_BLACK ? &m_tpiBlackThreatRoadInfos : &m_tpiWhiteThreatRoadInfos;
	list<ThreatRoadInfo>* pRivalThreatRoad = cps == SIDE_BLACK ? &m_tpiWhiteThreatRoadInfos : &m_tpiBlackThreatRoadInfos;
	map<int, int>* pRoads = cps == SIDE_BLACK ? &this->m_riRoadInfo.blackRoads : &this->m_riRoadInfo.whiteRoads;

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
			ohi.score =  MAXVALUE;
			ohi.pn = m_myChessPlaySide == cps? 1.0: 0.0;
			p->children.push_back(ohi);
		}
		else if (tri.threatPoints.size() == 1)
		{
			ohi.x2 = -1;
			ohi.y2 = -1;
			ohi.cps = cps;
			ohi.score = MAXVALUE;
			ohi.pn = m_myChessPlaySide == cps ? 1.0 : 0.0;
			p->children.push_back(ohi);
		}
		p->childsize = 1;
		return true;
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
					//********第二个子进攻 2018-7-3
					for (it = pRoads->begin(); it != pRoads->end(); it++)
					{
						int road_no = it->first;
						char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
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

				//********第二个子进攻 2018-7-3
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
						plt.push_back(ohi);

						unDoStone(x2, y2, cps);
					}
				}
				unDoStone(x1, y1, cps);
			}

			p->childsize = plt.size();
			if (plt.size() > 0)
			{
				plt.sort();
				int i = 0;
				list<OneHandInfo>::iterator itm;
				for (itm = plt.begin(); itm != plt.end(); itm++)
				{
					if (i == p->children.size())
					{
						makeOneMove(itm->x1, itm->y1, itm->x2, itm->y2);
						itm->pn = getRate();
						unDoOneMove(itm->x1, itm->y1, itm->x2, itm->y2);
						p->children.push_back(*itm);
						return true;
					}
					i++;
				}
			}
			return false;
		}
		else
		{
			//********双子防守Threat 2018-7-3
			stonePairs.clear();
			for (pIt = pRivalThreat->begin(); pIt != pRivalThreat->end(); ++pIt)													
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
					makeOneMove(ohi.x1, ohi.y1, ohi.x2, ohi.y2);
					ohi.score = getScore(cps);
					unDoOneMove(ohi.x1, ohi.y1, ohi.x2, ohi.y2);
					plt.push_back(ohi);
				}
			}
			//********双子防守Threat 招法有限 不用排序 2018-7-3

			p->childsize = plt.size();
			if (plt.size() >  0)
			{
				plt.sort();
				int i = 0;
				list<OneHandInfo>::iterator itm;
				for (itm = plt.begin(); itm != plt.end(); itm++)
				{
					if (i == p->children.size())
					{
						makeOneMove(itm->x1, itm->y1, itm->x2, itm->y2);
						itm->pn = getRate();
						unDoOneMove(itm->x1, itm->y1, itm->x2, itm->y2);
						p->children.push_back(*itm);
						return true;
					}
					i++;
				}
			}
			return false;
		}
	}

	for (it = pRoads->begin(); it != pRoads->end(); it++)
	{
		int road_no = it->first;
		char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
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
			for (it = pRoads->begin(); it != pRoads->end(); it++)
			{
				int road_no = it->first;
				char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
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
				plt.push_back(ohi);
				unDoStone(x2, y2, cps);
			}
		}
		unDoStone(x1, y1, cps);
	}
	
	p->childsize = plt.size();
	if (plt.size() >  p->children.size())
	{
		plt.sort();
		int i = 0;
		list<OneHandInfo>::iterator itm;
		for (itm = plt.begin(); itm != plt.end(); itm++)
		{
			if (i == p->children.size())
			{
				makeOneMove(itm->x1, itm->y1, itm->x2, itm->y2);
				itm->pn = getRate();
				unDoOneMove(itm->x1, itm->y1, itm->x2, itm->y2);
				p->children.push_back(*itm);
				return true;
			}
			i++;
		}
	}
	return false;
}

bool Solver::playout(int depth)
{
	
	VCDT::clearHash();
	if (depth != PlayoutDepth && VCDT::searchFull(VCDT_DEPTH))
	{
		if (nextChessPlaySide == m_myChessPlaySide)
			return true;
		else return false;
	}
	
	if (depth == 0) return false;

	list<OneHandInfo> pl;
	generatePlayoutMoves(&pl, nextChessPlaySide);

	if (pl.size() == 0)
	{
		if (nextChessPlaySide == m_myChessPlaySide)
			return false;
		else return true;
	}

	int i = rand() % pl.size();
	list<OneHandInfo>::iterator it = pl.begin();
	std::advance(it, i);

	makeOneMove(it->x1, it->y1, it->x2, it->y2);
	bool res = playout(depth - 1);
	unDoOneMove(it->x1, it->y1, it->x2, it->y2);

	return res;
}

double Solver::getRate()
{
	VCST::clearHash();
	if (VCST::searchFull(VCST_DEPTH))
	{
		cout << "vsct success" << endl;
		if(nextChessPlaySide == m_myChessPlaySide)
			return 1.0;
		else
			return 0.0;
	}
	
	int i = 0;
	int winnum = 0;
	while (i < PlayoutTimes)
	{
		if(playout(PlayoutDepth)) winnum ++;
		i++;
	}
	if (winnum == 0) return 0.05;
	else if (winnum == PlayoutTimes) return 0.95;
	else return (double) winnum / PlayoutTimes;
}

void Solver::PPNS(OneHandInfo* p)
{
	if (p->children.size() == 0)
	{
		if (generateMoves(p, nextChessPlaySide))
		{
			/*if (p->children.back().pn == 1.0) p->pn = 0.95;
			else if(p->children.back().pn == 0.0) p->pn = 0.05;
			else p->pn = p->children.back().pn;*/
			p->pn = p->children.back().pn;

			makeOneMove(p->children.back().x1, p->children.back().y1, p->children.back().x2, p->children.back().y2);
			for (int k = 0; k<19; k++)
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
			cout << "\n";
			unDoOneMove(p->children.back().x1, p->children.back().y1, p->children.back().x2, p->children.back().y2);
		}
		else
			p->pn = nextChessPlaySide == m_myChessPlaySide ? 0.0 : 1.0;

		cout << "child ";
		for (list<OneHandInfo>::iterator it = p->children.begin(); it != p->children.end(); it++)
		{
			cout <<it->pn << ", ";
		}
		cout << endl;
		return;
	}
	
	list<OneHandInfo>::iterator itm = p->children.begin();

	if (nextChessPlaySide == m_myChessPlaySide)
	{
		double maxpn = -MAXVALUE;
		for (list<OneHandInfo>::iterator it = p->children.begin(); it != p->children.end(); it++)
		{
			if (it->pn >= maxpn)
			{
				maxpn = it->pn;
				itm = it;
			}
		}
	}
	else
	{
		double minpn = MAXVALUE;
		for (list<OneHandInfo>::iterator it = p->children.begin(); it != p->children.end(); it++)
		{
			if (it->pn <= minpn)
			{
				minpn = it->pn;
				itm = it;
			}
		}
	}
	
	OneHandInfo* pm = &(*itm);
	itm++;
	if (itm == p->children.end())
	{
		if (p->children.size() < p->childsize)
		{
			generateMoves(p, nextChessPlaySide);
			cout<<"sibling: "<<p->children.back().pn << " ("<<p->children.back().x1 << ", " << p->children.back().y1 << ", " << p->children.back().x2 << ", " << p->children.back().y2 <<")"<< endl;
			
			/*if (nextChessPlaySide == m_myChessPlaySide && p->children.back().pn > itm->pn) pm = &p->children.back();
			if (nextChessPlaySide != m_myChessPlaySide && p->children.back().pn < itm->pn) pm = &p->children.back();*/
		}	
	}

	makeOneMove(pm->x1, pm->y1, pm->x2, pm->y2);

	for (int k = 0; k<19; k++)
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
	cout << "\n";

	PPNS(pm);
	unDoOneMove(pm->x1, pm->y1, pm->x2, pm->y2);
	
	double andp = 1.0;
	double orp = 1.0;
	double maxpn = -MAXVALUE;

	for (list<OneHandInfo>::iterator it = p->children.begin(); it != p->children.end(); it++)
	{
		andp *= it->pn;
		orp *= 1.0 - it->pn;

		if (it->pn > maxpn)
		{
			maxpn = it->pn;

			resX1 = it->x1;
			resY1 = it->y1;
			resX2 = it->x2;
			resY2 = it->y2;
		}
	}

	if (nextChessPlaySide == m_myChessPlaySide)
	{
		p->pn = 1.0 - orp;
	}
	else
		p->pn = andp;
		
	return;
}