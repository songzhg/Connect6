#pragma once
#include "abstractai.h"

class AI : public Solver
{
public:
	AI(void);
	~AI(void);
protected:
	int MAXDEPTH = 5;
	int MAXBRANCH = 50;
	HashItem HashTable[HASH_SIZE];
	bool recordHash(int flag, int depth, int vl);
	void clearHash();
	int findHash(int vlAlpha, int vlBeta, int nDepth);

	int searchFull(int vlAlpha, int vlBeta, int nDepth);
	int searchRoot(int nDepth);
	void generateMoves(list<OneHandInfo>* pl, ChessPlaySide cps);
	int searchMain();
	void run(int& x1, int& y1, int& x2, int& y2);
private:
	int resX1;
	int resY1;
	int resX2;
	int resY2;
};
