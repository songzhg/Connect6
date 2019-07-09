#include "stdafx.h"
#include "VCDT.h"
#include "VCST.h"
#include <iostream>
#include <math.h>

VCST::VCST(void)
{
	
}


VCST::~VCST(void)
{
	
}

void VCST::setBranch(int x)
{
	VCST_BRANCH = x;
}

void VCST::setDepth(int x)
{
	VCST_DEPTH  = x;
}

bool VCST::recordHash(int depth)
{
	HashItem hsh;
	hsh = HashTable[zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.depth > 0 && hsh.depth < depth) {
		return false;
	}
	hsh.depth = depth;
	hsh.dwLock0 = zobr.dwLock0;
	hsh.dwLock1 = zobr.dwLock1;
	HashTable[zobr.dwKey & (HASH_SIZE - 1)] = hsh;
	return true;
}

void  VCST::clearHash()
{
	memset(HashTable, 0, HASH_SIZE * sizeof(HashItem));
}

bool VCST::findHash()
{
	HashItem hsh;
	hsh = HashTable[zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.dwLock0 != zobr.dwLock0 || hsh.dwLock1 != zobr.dwLock1)
		return false;
	//vl = hsh.content;
	return true;
}

bool VCST::searchFull(int nDepth)
{
	if (nDepth == 0)
	{
		//cout << "limit depth" << endl;
		return false;
	}
	if (findHash())
	{
		//cout << "hash" << endl;
		return false;
	}

	list<VCDTNode> pl;
	generateVCSTMoves(&pl, nextChessPlaySide);

	bool res = false;
	for (list<VCDTNode>::iterator it = pl.begin(); it != pl.end(); it++)
	{
		if (it->score >= MAXVALUE)
		{
			resX1 = it->x1;
			resY1 = it->y1;
			resX2 = it->x2;
			resY2 = it->y2;

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

			return true;
		}

		makeOneMove(it->x1, it->y1, it->x2, it->y2);

		if (it->singlet)
		{
			for (list<VCDTNode>::iterator itd = it->DefenseMoves.begin(); itd != it->DefenseMoves.end(); itd++)
			{
				if (m_charArrayBoard[itd->y1][itd->x1] == 0)
				{
					putStone(itd->x1, itd->y1, nextChessPlaySide);
					int n = 19 * (itd->y1 - 1) + itd->x1 - 1;
					zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
				}
			}	
			
		}
		else
		{
			for (list<VCDTNode>::iterator itd = it->DefenseMoves.begin(); itd != it->DefenseMoves.end(); itd++)
			{
				if (m_charArrayBoard[itd->y1][itd->x1] == 0)
				{
					putStone(itd->x1, itd->y1, nextChessPlaySide);
					int n = 19 * (itd->y1 - 1) + itd->x1 - 1;
					zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
				}
				if (m_charArrayBoard[itd->y2][itd->x2] == 0)
				{
					putStone(itd->x2, itd->y2, nextChessPlaySide);
					int n = 19 * (itd->y2 - 1) + itd->x2 - 1;
					zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
				}
			}
		}
		
		zobr.Xor(Zobrist.Player);
		changePlaySide();

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

		if (it->singlet)
		{
			res = jugdeSingleThreatWin(nDepth - 1);
		//	if (!res) cout << "ST fails" << endl;
		}
		else
		{
			res = searchFull(nDepth - 1);
		}
		if (!res) recordHash(nDepth);

		changePlaySide();
		zobr.Xor(Zobrist.Player);

		if (it->singlet)
		{
			for (list<VCDTNode>::iterator itd = it->DefenseMoves.begin(); itd != it->DefenseMoves.end(); itd++)
			{
				if (m_charArrayBoard[itd->y1][itd->x1] != 0)
				{
					int n = 19 * (itd->y1 - 1) + itd->x1 - 1;
					zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
					unDoStone(itd->x1, itd->y1, nextChessPlaySide);
				}
			}
		}
		else
		{
			for (list<VCDTNode>::iterator itd = it->DefenseMoves.begin(); itd != it->DefenseMoves.end(); itd++)
			{
				if (m_charArrayBoard[itd->y2][itd->x2] != 0)
				{
					int n = 19 * (itd->y2 - 1) + itd->x2 - 1;
					zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
					unDoStone(itd->x2, itd->y2, nextChessPlaySide);
				}
				if (m_charArrayBoard[itd->y1][itd->x1] != 0)
				{
					int n = 19 * (itd->y1 - 1) + itd->x1 - 1;
					zobr.Xor(Zobrist.Table[nextChessPlaySide - 1][n]);
					unDoStone(itd->x1, itd->y1, nextChessPlaySide);
				}
			}
		}
		
		unDoOneMove(it->x1, it->y1, it->x2, it->y2);

		if (res)
		{
			resX1 = it->x1;
			resY1 = it->y1;
			resX2 = it->x2;
			resY2 = it->y2;
			return true;
		}
	}
	//cout << "no other choice" << endl;
	return false;
}

void VCST::generateVCSTMoves(list<VCDTNode>* pl, ChessPlaySide cps)
{
	char ReleBoard[19][19][19][19] = { 0 };
	VCDTNode ohi;
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
							//case 1:												//000001 1
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
								//	break;
							//case 2:												//000010 2
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
								//	break;
							//case 3:												//000011 3
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
								//	break;
							//case 4:												//000100 4
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
								//	break;
							//case 5:												//000101 5
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
								//	break;
							//case 6:												//000110 6
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
								//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
								//	break;
						case 7:												//000111 7
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
							//case 8:												//001000 8
							//													//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
							//case 9:												//001001 9
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
							//case 10:											//001010 10
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
						case 11:											//001011 11
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
							//case 12:											//001100 12
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
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
							//case 16:											//010000 16
							//													//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//													//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
							//case 17:											//010001 17
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
							//case 18:											//010010 18
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
						case 19:											//010011 19
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							break;
							//case 20:											//010100 20
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
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
							//case 24:											//011000 24
							//													//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
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
							//case 32:											//100000 32
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	break;
							//case 33:											//100001 33
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	break;
							//case 34:											//100010 34
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	break;
						case 35:											//100011 35
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
							//case 36:											//100100 36
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	break;
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
							//case 40:											//101000 40
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	break;
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
							//case 48:											//110000 48
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	break;
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
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
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

						list<pair<int, int>> stonePairs;
						list<ThreatPointInfo>::iterator pIt;

						if (pRivalThreat->size() > 0 || pOwnThreatRoad->size() < 1)
						{
							unDoStone(x2, y2, cps);
							continue;
						}

						BOOL singleThreat = false;
						for (pIt = pOwnThreat->begin(); pIt != pOwnThreat->end(); ++pIt)
						{
							if (pIt->threatRoads.size() == pOwnThreatRoad->size())
							{
								stonePairs.push_back(pair<int, int>(pIt->threatPoint % 19, pIt->threatPoint / 19));
								singleThreat = true;
							}
						}

						ohi.x1 = x1;
						ohi.y1 = y1;
						ohi.x2 = x2;
						ohi.y2 = y2;
						ohi.cps = cps;
						ohi.score = getScore(cps);
						ohi.DefenseMoves.clear();

						if (singleThreat)
						{
							ohi.singlet = true;
							VCDTNode defence;

							for (list<pair<int, int>>::iterator spIt = stonePairs.begin(); spIt != stonePairs.end(); ++spIt)
							{
								defence.x1 = spIt->first;
								defence.y1 = spIt->second;
								defence.cps = cps == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
								ohi.DefenseMoves.push_back(defence);
							}
						}
						else
						{
							ohi.singlet = false;
							stonePairs.clear();
							for (pIt = pOwnThreat->begin(); pIt != pOwnThreat->end(); ++pIt)													//DTDT
							{
								list<ThreatPointInfo>::iterator pIt2 = pIt;
								++pIt2;
								for (; pIt2 != pOwnThreat->end(); ++pIt2)
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
									if (pOwnThreatRoad->size() == count)
									{
										stonePairs.push_back(pair<int, int>(pIt->threatPoint, pIt2->threatPoint));
									}
								}
							}

							VCDTNode defence;
							if (stonePairs.size() == 0)
							{
								ohi.score = MAXVALUE;
							}
							else {
								for (list<pair<int, int>>::iterator spIt = stonePairs.begin(); spIt != stonePairs.end(); ++spIt)
								{
									defence.x1 = spIt->first % 19;
									defence.y1 = spIt->first / 19;
									defence.x2 = spIt->second % 19;
									defence.y2 = spIt->second / 19;
									defence.cps = cps == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
									ohi.DefenseMoves.push_back(defence);
								}
							}
						}

						pl->push_back(ohi);
						unDoStone(x2, y2, cps);
					}
				}
				unDoStone(x1, y1, cps);
			}
			pl->sort();
			if (pl->size() > VCST_BRANCH) pl->resize(VCST_BRANCH);
		}
		return;
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

		if (m_charArrayBoard[y1][x1] == 0)
		{
			putStone(x1, y1, cps);

			if (pOwnThreatRoad->size() >= 1)
			{
				for (it = pRoads->begin(); it != pRoads->end(); it++)
				{
					int road_no = it->first;
					char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
					switch (road)
					{
					case 1:												//000001 1
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 2:												//000010 2
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 3:												//000011 3
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 4:												//000100 4
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 5:												//000101 5
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 6:												//000110 6
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 7:												//000111 7
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 8:												//001000 8
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						break;
					}
				}
			}
			else if (pOwnThreatRoad->size() < 1)
			{
				for (it = pRoads->begin(); it != pRoads->end(); it++)
				{
					int road_no = it->first;
					char road = cps == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
					switch (road)
					{
					case 7:												//000111 7
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					case 11:											//001011 11
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						break;
					//case 12:											//001100 12
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
					//	break;
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
					case 19:											//010011 19
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
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
					case 35:											//100011 35
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						break;
					}
				}
			}
		}

		for (list<pair<int, int>>::iterator it2 = PosPoints2.begin(); it2 != PosPoints2.end(); it2++)
		{
			int y2 = it2->first / 19;
			int x2 = it2->first % 19;
			if (m_charArrayBoard[y2][x2] == 0 && ReleBoard[y1][x1][y2][x2] == 0 && ReleBoard[y2][x2][y1][x1] == 0)
			{
				ReleBoard[y1][x1][y2][x2] = 1;
				ReleBoard[y2][x2][y1][x1] = 1;

				putStone(x2, y2, cps);

				list<pair<int, int>> stonePairs;
				list<ThreatPointInfo>::iterator pIt;

				if (pRivalThreat->size() > 0 || pOwnThreatRoad->size() < 1)
				{
					unDoStone(x2, y2, cps);
					continue;
				}

				BOOL singleThreat = false;
				for (pIt = pOwnThreat->begin(); pIt != pOwnThreat->end(); ++pIt)
				{
					if (pIt->threatRoads.size() == pOwnThreatRoad->size())
					{
						stonePairs.push_back(pair<int, int>(pIt->threatPoint % 19, pIt->threatPoint / 19));
						singleThreat = true;
					}
				}

				ohi.x1 = x1;
				ohi.y1 = y1;
				ohi.x2 = x2;
				ohi.y2 = y2;
				ohi.cps = cps;
				ohi.score = getScore(cps);
				ohi.DefenseMoves.clear();

				if (singleThreat)
				{
					ohi.singlet = true;
					VCDTNode defence;
					
					for (list<pair<int, int>>::iterator spIt = stonePairs.begin(); spIt != stonePairs.end(); ++spIt)
					{
						defence.x1 = spIt->first;
						defence.y1 = spIt->second;
						defence.cps = cps == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
						ohi.DefenseMoves.push_back(defence);
					}
				}
				else
				{
					ohi.singlet = false;
					stonePairs.clear();
					for (pIt = pOwnThreat->begin(); pIt != pOwnThreat->end(); ++pIt)													//DTDT
					{
						list<ThreatPointInfo>::iterator pIt2 = pIt;
						++pIt2;
						for (; pIt2 != pOwnThreat->end(); ++pIt2)
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
							if (pOwnThreatRoad->size() == count)
							{
								stonePairs.push_back(pair<int, int>(pIt->threatPoint, pIt2->threatPoint));
							}
						}
					}

					VCDTNode defence;
					if (stonePairs.size() == 0)
					{
						ohi.score = MAXVALUE;
					}
					else {
						for (list<pair<int, int>>::iterator spIt = stonePairs.begin(); spIt != stonePairs.end(); ++spIt)
						{
							defence.x1 = spIt->first % 19;
							defence.y1 = spIt->first / 19;
							defence.x2 = spIt->second % 19;
							defence.y2 = spIt->second / 19;
							defence.cps = cps == SIDE_BLACK ? SIDE_WHITE : SIDE_BLACK;
							ohi.DefenseMoves.push_back(defence);
						}
					}
				}

				pl->push_back(ohi);
				unDoStone(x2, y2, cps);
			}
		}
		unDoStone(x1, y1, cps);
	}
	pl->sort();
	if (pl->size() > VCST_BRANCH) pl->resize(VCST_BRANCH);
}

bool VCST::jugdeSingleThreatWin(int nDepth)
{
	char ReleZone[19][19] = { 0 };
	if (VCDT::searchIter(ReleZone, true))
	{
		/*for (int k = 0; k<19; k++)
		{
			for (int l = 0; l<19; l++)
			{
				if (ReleZone[k][l] > 0)
					cout << "@ ";
				else cout << ". ";
			}
			cout << "\n";
		}
		cout << "\n";*/

		bool res = true;
		for (int k = 0; k < 19; k++)
		{
			for (int l = 0; l < 19; l++)
			{
				if (ReleZone[k][l] > 0)
				{
					changePlaySide();
					putStone(l, k, nextChessPlaySide);
					changePlaySide();

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
					VCDT::clearHash();
					if (!VCDT::searchFull(VCDT_DEPTH))
					//if (!VCST::searchFull(VCST_DEPTH))
					{
						res = false;
						changePlaySide();
						unDoStone(l, k, nextChessPlaySide);
						changePlaySide();
						break;
					}
					changePlaySide();
					unDoStone(l, k, nextChessPlaySide);
					changePlaySide();
				}
			}
		}
		return res;
	}
	else return false;
}

void VCST::run(int& x1, int& y1, int& x2, int& y2)
{
	if (searchFull(VCST_DEPTH))
	{
		cout << "VCST success" << endl;
		x1 = resX1 + 1;
		y1 = resY1 + 1;
		x2 = resX2 + 1;
		y2 = resY2 + 1;
	}
}
