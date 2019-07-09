#pragma once

enum ChessPlaySide
{
	SIDE_BLACK = 1,
	SIDE_WHITE
};

struct OneMovePlayInfo
{
	ChessPlaySide chessPlaySide;
	int x;
	int y;
};
