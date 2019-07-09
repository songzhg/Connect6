#pragma once
#include "abstractai.h"

const int HASH_SIZE = 1 << 20;

typedef struct VCDTNode
{
	int x1;
	int y1;
	int x2;
	int y2;
	ChessPlaySide cps;
	list<VCDTNode> DefenseMoves;
	bool singlet;
	int score;

	bool operator < (struct VCDTNode & b)
	{
		return score > b.score;
	}
};

// Trans±í
typedef struct HashItem {
	DWORD dwKey, dwLock0, dwLock1;
	int vl;
	int flag;
	int depth;
};

class VCDT : public AbstractAI
{
public:
	VCDT(void);
	~VCDT(void);
protected:
	int VCDT_BRANCH = 3;
	int VCDT_DEPTH = 25;
	HashItem HashTable[HASH_SIZE];
	void setBranch(int x);
	void setDepth(int x);
	bool recordHash(int depth);
	void clearHash();
	bool findHash();
	void run(int& x1, int& y1, int& x2, int& y2);
	bool searchIter(char ReleZone[][19], bool semi);
	bool searchFull(int nDepth);
	bool searchZone(int nDepth, char ReleZone[][19], char ReleBoard[][361], bool semi);
	void getReleZone(char ReleZone[][19], char ReleBoard[][361], bool semi, bool pre = false);
	void generateVCDTMoves(list<VCDTNode>* pl, ChessPlaySide cps);
	int resX1;
	int resY1;
	int resX2;
	int resY2;
};
