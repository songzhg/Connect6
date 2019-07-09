#pragma once
#include <windows.h>

//Zobrist 2018-7-1

// RC4������������
typedef struct RC4Struct {
	BYTE s[256];
	int x, y;

	void InitZero(void);   // �ÿ���Կ��ʼ��������������
	BYTE NextByte(void);   // ��������������һ���ֽ�
	DWORD NextLong(void);  // ���������������ĸ��ֽ�
};


// Zobrist�ṹ
typedef struct ZobristStruct {
	DWORD dwKey, dwLock0, dwLock1;

	void InitZero(void);                 // �������Zobrist

	void InitRC4(RC4Struct &rc4);        // �����������Zobrist
		
	void Xor(const ZobristStruct &zobr); // ִ��XOR����
		
	void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2);
};

// Zobrist��
typedef struct ZobristTable{
	ZobristStruct Player;
	ZobristStruct Table[2][361];

	// ��ʼ��Zobrist��
	void InitZobrist(void);
};


