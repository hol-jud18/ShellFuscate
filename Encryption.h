#pragma once
#include <Windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

#define AESKEYSIZE 32 // 256 bits
#define AESIVSIZE 16  // 128 bits
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef struct {
	unsigned char* pPlainText;
	DWORD          dwPlainSize;
	unsigned char* pCipherText;
	DWORD          dwCipherSize;
	unsigned char* pKey;
	unsigned char* pIv;
} AES_PARAMS;

BOOL AesEncrypt(AES_PARAMS* pAes);
BOOL SimpleEncrypt(
	unsigned char* pPlainText,
	DWORD           plainSize,
	unsigned char* pKey,
	unsigned char* pIv,
	unsigned char** ppCipherText,
	DWORD*          pdwCipherSize
);

