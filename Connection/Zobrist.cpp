#include "stdafx.h"
#include "Zobrist.h"

//Zobrist 2018-7-1

// �ÿ���Կ��ʼ��������������
void RC4Struct::InitZero(void) {
	int i, j;
	BYTE uc;

	x = y = j = 0;
	for (i = 0; i < 256; i++) {
		s[i] = i;
	}
	for (i = 0; i < 256; i++) {
		j = (j + s[i]) & 255;
		uc = s[i];
		s[i] = s[j];
		s[j] = uc;
	}
}

BYTE RC4Struct::NextByte(void) {  // ��������������һ���ֽ�
	BYTE uc;
	x = (x + 1) & 255;
	y = (y + s[x]) & 255;
	uc = s[x];
	s[x] = s[y];
	s[y] = uc;
	return s[(s[x] + s[y]) & 255];
}

DWORD RC4Struct::NextLong(void) { // ���������������ĸ��ֽ�
	BYTE uc0, uc1, uc2, uc3;
	uc0 = NextByte();
	uc1 = NextByte();
	uc2 = NextByte();
	uc3 = NextByte();
	return uc0 + (uc1 << 8) + (uc2 << 16) + (uc3 << 24);
}

void ZobristStruct::InitZero(void) {                 // �������Zobrist
	dwKey = dwLock0 = dwLock1 = 0;
}
void ZobristStruct::InitRC4(RC4Struct &rc4) {        // �����������Zobrist
	dwKey = rc4.NextLong();
	dwLock0 = rc4.NextLong();
	dwLock1 = rc4.NextLong();
}
void ZobristStruct::Xor(const ZobristStruct &zobr) { // ִ��XOR����
	dwKey ^= zobr.dwKey;
	dwLock0 ^= zobr.dwLock0;
	dwLock1 ^= zobr.dwLock1;
}
void ZobristStruct::Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
	dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
	dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
	dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
}

// ��ʼ��Zobrist��
void ZobristTable::InitZobrist(void) {
	int i, j;
	RC4Struct rc4;

	rc4.InitZero();
	Player.InitRC4(rc4);
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 361; j++) {
			Table[i][j].InitRC4(rc4);
		}
	}
}

