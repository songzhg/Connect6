#include "stdafx.h"
#include "VCDT.h"
#include <iostream>
#include <string>

using namespace std;

VCDT::VCDT(void)
{
	clearHash();
}

VCDT::~VCDT(void)
{
}

void VCDT::setBranch(int x)
{
	VCDT_BRANCH = x;
}

void VCDT::setDepth(int x)
{
	VCDT_DEPTH = x;
}

bool VCDT::recordHash(int depth)
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

void  VCDT::clearHash()
{
	memset(HashTable, 0, HASH_SIZE * sizeof(HashItem));
}

bool VCDT::findHash()
{
	HashItem hsh;
	hsh = HashTable[zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.dwLock0 != zobr.dwLock0 || hsh.dwLock1 != zobr.dwLock1)
		return false;
	//vl = hsh.content;
	return true;
}

void VCDT::run(int& x1, int& y1, int& x2, int& y2)
{
	char ReleZone[19][19];
	if (searchFull(VCDT_DEPTH))
	{
		cout << "VCDT success"<<endl;
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
		x1 = resX1 + 1;
		y1 = resY1 + 1;
		x2 = resX2 + 1;
		y2 = resY2 + 1;
	}
}

bool VCDT::searchIter(char ReleZone[][19], bool semi)
{
	for (int i = 1; i <= VCDT_DEPTH; i++)
	{
		clearHash();
		memset(ReleZone, 0, 361 * sizeof(char));
		char ReleBoard[361][361] = { 0 };
		if (searchZone(i, ReleZone, ReleBoard, semi))
		{
			changePlaySide();
			getReleZone(ReleZone, ReleBoard, semi, true);
			changePlaySide();
			return true;
		}
	}
	return false;
}

bool VCDT::searchFull(int nDepth)
{
	if (nDepth == 0)
	{
		return false;
	}
	if (findHash())
	{
		//cout << "hash" << endl;
		return false;
	}

	list<VCDTNode> pl;
	generateVCDTMoves(&pl, nextChessPlaySide);

	bool res = false;
	for (list<VCDTNode>::iterator it = pl.begin(); it != pl.end(); it++)
	{
		if (it->score >= MAXVALUE)
		{
			resX1 = it->x1;
			resY1 = it->y1;
			resX2 = it->x2;
			resY2 = it->y2;
			return true;
		}

		makeOneMove(it->x1, it->y1, it->x2, it->y2);

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

		/*if (nextChessPlaySide == SIDE_BLACK) cout << "Black ";
		else cout << "White ";
		cout << "Zobrist Key: " << zobr.dwKey << ", ";
		cout << zobr.dwLock0 << ", ";
		cout << zobr.dwLock1 << endl;*/

		res = searchFull(nDepth - 1);

		if (!res) recordHash(nDepth);

		/*if (nextChessPlaySide == SIDE_BLACK) cout << "VCDT Black ";
		else cout << "VCDT White ";
		cout << "Zobrist Key: " << zobr.dwKey << ", ";
		cout << zobr.dwLock0 << ", ";
		cout << zobr.dwLock1 << endl;*/

		changePlaySide();
		zobr.Xor(Zobrist.Player);
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
	return false;
}

bool VCDT::searchZone(int nDepth, char ReleZone[][19], char ReleBoard[][361], bool semi)
{
	if (nDepth == 0)
	{
		return false;
	}
	if (findHash())
	{
		//cout << "hash" << endl;
		return false;
	}

	list<VCDTNode> pl;
	generateVCDTMoves(&pl, nextChessPlaySide);

	bool res = false;
	for (list<VCDTNode>::iterator it = pl.begin(); it != pl.end(); it++)
	{
		ReleZone[it->y2][it->x2] = 1;
		ReleZone[it->y1][it->x1] = 1;

		if (it->score >= MAXVALUE)
		{
			resX1 = it->x1;
			resY1 = it->y1;
			resX2 = it->x2;
			resY2 = it->y2;

			
			changePlaySide();
			//makeOneMove(it->x1, it->y1, it->x2, it->y2);
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

			getReleZone(ReleZone, ReleBoard, semi);

			//unDoOneMove(it->x1, it->y1, it->x2, it->y2);
			changePlaySide();
			
			return true;
		}

		makeOneMove(it->x1, it->y1, it->x2, it->y2);

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

			ReleBoard[(itd->y1) * 19 + itd->x1][(itd->y2) * 19 + itd->x2] = 1;
			ReleBoard[(itd->y2) * 19 + itd->x2][(itd->y1) * 19 + itd->x1] = 1;
			ReleZone[itd->y2][itd->x2] = nDepth + 2;
			ReleZone[itd->y1][itd->x1] = nDepth + 2;
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

		/*if (nextChessPlaySide == SIDE_BLACK) cout << "Black ";
		else cout << "White ";
		cout << "Zobrist Key: " << zobr.dwKey << ", ";
		cout << zobr.dwLock0 << ", ";
		cout << zobr.dwLock1 << endl;*/

		res = searchZone(nDepth - 1, ReleZone, ReleBoard, semi);

		if (!res) recordHash(nDepth);

		/*if (nextChessPlaySide == SIDE_BLACK) cout << "VCDT Black ";
		else cout << "VCDT White ";
		cout << "Zobrist Key: " << zobr.dwKey << ", ";
		cout << zobr.dwLock0 << ", ";
		cout << zobr.dwLock1 << endl;*/

		changePlaySide();
		zobr.Xor(Zobrist.Player);
		for (list<VCDTNode>::iterator itd = it->DefenseMoves.begin(); itd != it->DefenseMoves.end(); itd++)
		{
			if (!res)
			{
				ReleBoard[(itd->y1) * 19 + itd->x1][(itd->y2) * 19 + itd->x2] = 0;
				ReleBoard[(itd->y2) * 19 + itd->x2][(itd->y1) * 19 + itd->x1] = 0;
				ReleZone[itd->y2][itd->x2] = 0;
				ReleZone[itd->y1][itd->x1] = 0;
			}
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
		unDoOneMove(it->x1, it->y1, it->x2, it->y2);

		if (res)
		{
			resX1 = it->x1;
			resY1 = it->y1;
			resX2 = it->x2;
			resY2 = it->y2;
			return true;
		}
		else
		{
			ReleZone[it->y2][it->x2] = 0;
			ReleZone[it->y1][it->x1] = 0;
		}
	}
	return false;
}

void VCDT::getReleZone(char ReleZone[][19], char ReleBoard[][361], bool semi, bool pre)
{
	list<ThreatPointInfo>* pOwnThreat = nextChessPlaySide == SIDE_BLACK ? &m_tpiBlackThreatPointInfos : &m_tpiWhiteThreatPointInfos;
	list<ThreatPointInfo>* pRivalThreat = nextChessPlaySide == SIDE_BLACK ? &m_tpiWhiteThreatPointInfos : &m_tpiBlackThreatPointInfos;
	list<ThreatRoadInfo>* pOwnThreatRoad = nextChessPlaySide == SIDE_BLACK ? &m_tpiBlackThreatRoadInfos : &m_tpiWhiteThreatRoadInfos;
	list<ThreatRoadInfo>* pRivalThreatRoad = nextChessPlaySide == SIDE_BLACK ? &m_tpiWhiteThreatRoadInfos : &m_tpiBlackThreatRoadInfos;
	map<int, int>* pRoads = nextChessPlaySide == SIDE_BLACK ? &this->m_riRoadInfo.blackRoads: &this->m_riRoadInfo.whiteRoads;


	map<int, int>::iterator it;
	int n, m, i, j;
	int x, y;
	if (!semi)
	{
		for (it = pRoads->begin(); it != pRoads->end(); it++)
		{
			int road_no = it->first;
			char road = nextChessPlaySide == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
			switch (road)
			{
			case 3:												//000011 3
				if (pre || ReleBoard[m_iArrayRoads[road_no][0]][m_iArrayRoads[road_no][1]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19]
					!= ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				}
				break;
			case 5:												//000101 5
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 6:												//000110 6
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 7:												//000111 7
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 9:												//001001 9
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 10:											//001010 10
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 11:											//001011 11
				if (pre || ReleBoard[m_iArrayRoads[road_no][0]][m_iArrayRoads[road_no][1]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19]
					!= ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				}
				break;
			case 12:											//001100 12
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 13:											//001101 13
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 14:											//001110 14
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 17:											//010001 17
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 18:											//010010 18
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 19:											//010011 19
				if (pre || ReleBoard[m_iArrayRoads[road_no][0]][m_iArrayRoads[road_no][1]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19]
					!= ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				}
				break;
			case 20:											//010100 20
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 21:											//010101 21
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 22:											//010110 22
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 24:											//011000 24
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 25:											//011001 25
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 26:											//011010 26
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 28:											//011100 28
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 33:											//100001 33
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 34:											//100010 34
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 35:											//100011 35
				if (pre || ReleBoard[m_iArrayRoads[road_no][0]][m_iArrayRoads[road_no][1]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19]
					!= ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				}
				break;
			case 36:											//100100 36
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 37:											//100101 37
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 38:											//100110 38
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 40:											//101000 40
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 41:											//101001 41
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 42:											//101010 42
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 44:											//101100 44
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 48:											//110000 48
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				}
				break;
			case 49:											//110001 49
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				}
				break;
			case 50:											//110010 50
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				}
				break;
			case 52:											//110100 52
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				}
				break;
			case 56:											//111000 56
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				break;
			}
		}
	}
	else
	{
		for (it = pRoads->begin(); it != pRoads->end(); it++)
		{
			int road_no = it->first;
			char road = nextChessPlaySide == SIDE_BLACK ? m_riRoadInfo.roadItems[road_no].blackRoad : m_riRoadInfo.roadItems[road_no].whiteRoad;
			switch (road)
			{
			case 7:												//000111 7
				if (pre || ReleBoard[m_iArrayRoads[road_no][0]][m_iArrayRoads[road_no][1]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19]
					!= ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				}
				break;
			case 11:											//001011 11
				if (pre || ReleBoard[m_iArrayRoads[road_no][0]][m_iArrayRoads[road_no][1]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19]
					!= ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				}
				break;
			case 13:											//001101 13
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 14:											//001110 14
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 19:											//010011 19
				if (pre || ReleBoard[m_iArrayRoads[road_no][0]][m_iArrayRoads[road_no][1]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19]
					!= ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				}
				break;
			case 21:											//010101 21
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 22:											//010110 22
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 25:											//011001 25
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 26:											//011010 26
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 28:											//011100 28
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19] = 1;
				break;
			case 35:											//100011 35
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 37:											//100101 37
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 38:											//100110 38
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 41:											//101001 41
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 42:											//101010 42
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 44:											//101100 44
				ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
				ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19] = 1;
				break;
			case 49:											//110001 49
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				}
				break;
			case 50:											//110010 50
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				}
				break;
			case 52:											//110100 52
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][3] / 19][m_iArrayRoads[road_no][3] % 19] = 1;
				}
				break;
			case 56:											//111000 56
				if (pre || ReleBoard[m_iArrayRoads[road_no][4]][m_iArrayRoads[road_no][5]] == 1 ||
					ReleZone[m_iArrayRoads[road_no][4] / 19][m_iArrayRoads[road_no][4] % 19]
					!= ReleZone[m_iArrayRoads[road_no][5] / 19][m_iArrayRoads[road_no][5] % 19])
				{
					ReleZone[m_iArrayRoads[road_no][0] / 19][m_iArrayRoads[road_no][0] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][1] / 19][m_iArrayRoads[road_no][1] % 19] = 1;
					ReleZone[m_iArrayRoads[road_no][2] / 19][m_iArrayRoads[road_no][2] % 19] = 1;
				}
				break;
			}
		}
	}

	list<ThreatPointInfo>::iterator pIt;
	for (pIt = pRivalThreat->begin(); pIt != pRivalThreat->end(); ++pIt)
	{
		ReleZone[pIt->threatPoint / 19][pIt->threatPoint % 19] = 1;
	}

	for (pIt = pOwnThreat->begin(); pIt != pOwnThreat->end(); ++pIt)
	{
		ReleBoard[pIt->threatPoint / 19][pIt->threatPoint % 19] = 1;
	}

}

void VCDT::generateVCDTMoves(list<VCDTNode>* pl, ChessPlaySide cps)
{
	char ReleBoard[19][19][19][19] = { 0 };
	VCDTNode ohi;
	map<int, int>::iterator it;
	list<ThreatPointInfo>* pOwnThreat = cps == SIDE_BLACK ? &m_tpiBlackThreatPointInfos : &m_tpiWhiteThreatPointInfos;
	list<ThreatPointInfo>* pRivalThreat = cps == SIDE_BLACK ? &m_tpiWhiteThreatPointInfos : &m_tpiBlackThreatPointInfos;
	list<ThreatRoadInfo>* pOwnThreatRoad = cps == SIDE_BLACK ? &m_tpiBlackThreatRoadInfos : &m_tpiWhiteThreatRoadInfos;
	list<ThreatRoadInfo>* pRivalThreatRoad = cps == SIDE_BLACK ? &m_tpiWhiteThreatRoadInfos : &m_tpiBlackThreatRoadInfos;
	map<int, int>* pRoads =  cps == SIDE_BLACK ?&this->m_riRoadInfo.blackRoads: &this->m_riRoadInfo.whiteRoads;

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
						//case 7:												//000111 7
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//break;
							//case 8:												//001000 8
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
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
						//case 11:											//001011 11
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//break;
							//case 12:											//001100 12
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
						//case 13:											//001101 13
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//break;
						case 14:											//001110 14
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
							break;
							//case 16:											//010000 16
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
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
						//case 19:											//010011 19
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						//	break;
							//case 20:											//010100 20
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
						//case 21:											//010101 21
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						//	break;
						//case 22:											//010110 22
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	break;
							//case 24:											//011000 24
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							//	break;
						//case 25:											//011001 25
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	break;
						case 26:											//011010 26
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
							PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
							break;
						case 28:											//011100 28
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
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
						//case 35:											//100011 35
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	break;
						//case 36:											//100100 36
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	break;
						//case 37:											//100101 37
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	break;
						//case 38:											//100110 38
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	break;
						//case 40:											//101000 40
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	break;
						//case 41:											//101001 41
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	break;
						//case 42:											//101010 42
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	break;
						//case 44:											//101100 44
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	break;
						//case 48:											//110000 48
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	break;
						//case 49:											//110001 49
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	break;
						//case 50:											//110010 50
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	break;
						//case 52:											//110100 52
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	break;
						//case 56:											//111000 56
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
						//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						//	break;
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

						if (pRivalThreat->size() > 0 || pOwnThreatRoad->size() < 2)
						{
							unDoStone(x2, y2, cps);
							continue;
						}

						BOOL singleThreat = false;
						for (pIt = pOwnThreat->begin(); pIt != pOwnThreat->end(); ++pIt)
						{
							if (pIt->threatRoads.size() == pOwnThreatRoad->size())
							{
								unDoStone(x2, y2, cps);
								singleThreat = true;
								break;
							}
						}
						if (singleThreat) continue;

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

						ohi.x1 = x1;
						ohi.y1 = y1;
						ohi.x2 = x2;
						ohi.y2 = y2;
						ohi.cps = cps;
						ohi.score = getScore(cps);
						ohi.DefenseMoves.clear();

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
						pl->push_back(ohi);
						unDoStone(x2, y2, cps);
					}
				}
				unDoStone(x1, y1, cps);
			}
			pl->sort();
			if (pl->size() > VCDT_BRANCH) pl->resize(VCDT_BRANCH);
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
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 2:												//000010 2
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
			break;
		case 3:												//000011 3
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][3], it->first));
			PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][4], it->first));
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][5], it->first));
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
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
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
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
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
	//		PosPoints1.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
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

			if (pOwnThreatRoad->size() >= 2)
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
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][2], it->first));
						PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][1], it->first));
					//	PosPoints2.push_back(pair<int, int>(m_iArrayRoads[road_no][0], it->first));
						break;
					}
				}
			}
			else if (pOwnThreatRoad->size() < 2)
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

				if (pRivalThreat->size() > 0 || pOwnThreatRoad->size() < 2)
				{
					unDoStone(x2, y2, cps);
					continue;
				}

				BOOL singleThreat = false;
				for (pIt = pOwnThreat->begin(); pIt != pOwnThreat->end(); ++pIt)
				{
					if (pIt->threatRoads.size() == pOwnThreatRoad->size())
					{
						unDoStone(x2, y2, cps);
						singleThreat = true;
						break;
					}
				}
				if (singleThreat) continue;

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

				ohi.x1 = x1;
				ohi.y1 = y1;
				ohi.x2 = x2;
				ohi.y2 = y2;
				ohi.cps = cps;
				ohi.score = getScore(cps);
				ohi.DefenseMoves.clear();

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
				pl->push_back(ohi);
				unDoStone(x2, y2, cps);
			}
		}
		unDoStone(x1, y1, cps);
	}
	pl->sort();
	if (pl->size() > VCDT_BRANCH) pl->resize(VCDT_BRANCH);
}
