#pragma once
#include "VCDT.h"

class VCST : public VCDT
{
public:
	VCST(void);
	~VCST(void);
protected:
	int VCST_BRANCH = 7;
	int VCST_DEPTH = 7;
	HashItem HashTable[HASH_SIZE];
	bool recordHash(int depth);
	void clearHash();
	bool findHash();
	void setBranch(int x);
	void setDepth(int x);
	bool searchFull(int nDepth);
	void generateVCSTMoves(list<VCDTNode>* pl, ChessPlaySide cps);
	bool jugdeSingleThreatWin(int nDepth);
	void run(int& x1, int& y1, int& x2, int& y2);
	int resX1;
	int resY1;
	int resX2;
	int resY2;
};
