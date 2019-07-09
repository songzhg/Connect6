#pragma once
#include <windows.h>

//Zobrist 2018-7-1

// RC4密码流生成器
typedef struct RC4Struct {
	BYTE s[256];
	int x, y;

	void InitZero(void);   // 用空密钥初始化密码流生成器
	BYTE NextByte(void);   // 生成密码流的下一个字节
	DWORD NextLong(void);  // 生成密码流的下四个字节
};


// Zobrist结构
typedef struct ZobristStruct {
	DWORD dwKey, dwLock0, dwLock1;

	void InitZero(void);                 // 用零填充Zobrist

	void InitRC4(RC4Struct &rc4);        // 用密码流填充Zobrist
		
	void Xor(const ZobristStruct &zobr); // 执行XOR操作
		
	void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2);
};

// Zobrist表
typedef struct ZobristTable{
	ZobristStruct Player;
	ZobristStruct Table[2][361];

	// 初始化Zobrist表
	void InitZobrist(void);
};


