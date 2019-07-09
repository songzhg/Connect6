#pragma once
#include "abstractai.h"
#include "VCST.h"
using namespace std;


typedef struct _OneHandInfo
{
	int x1;
	int y1;
	int x2;
	int y2;
	ChessPlaySide cps;
	double pn;
	int score;
	int childsize = 0;
	//bool childisfull = false;
	list<_OneHandInfo> children;

	bool operator < (struct _OneHandInfo & b)
	{
		return score > b.score;
	}
}OneHandInfo;

class Solver : public VCST
{
public:
	
	Solver(void);
	~Solver(void);
protected:
	int PlayoutBranchSzie = 30;
	int PlayoutDepth = 5;
	int PlayoutTimes = 10;

	void run(int& x1, int& y1, int& x2, int& y2);
	void generatePlayoutMoves(list<OneHandInfo>* pl, ChessPlaySide cps);
	bool generateMoves(OneHandInfo* pl, ChessPlaySide cps);
	bool playout(int depth);
	double getRate();
	void PPNS(OneHandInfo* p);
	int resX1;
	int resY1;
	int resX2;
	int resY2;
};
