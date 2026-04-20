#include "Encryption.h"
#include <stdio.h>

BOOL AesEncrypt(AES_PARAMS* pAes) {
	BCRYPT_ALG_HANDLE hAlg          = NULL;
	BCRYPT_KEY_HANDLE hKey          = NULL;
	BOOL              success       = TRUE;
	NTSTATUS          status        = 0;
	ULONG             bytesWritten  = 0;

	//BCrypt internal buffer - size is determined at runtime
	DWORD             keyObjectSize = 0;
	unsigned char*    pKeyObject = NULL;

	// output buffer for encrypted data
	DWORD             cipherTextSize = 0;
	unsigned char*    pCipherText = NULL;

	//open aes algorithm provider
	status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
	if (!NT_SUCCESS(status)) {
		printf("[-] BCryptOpenAlgorithmProvider: 0x%0.8X\n", status);
		success = FALSE;
		goto Cleanup;
	}

	//determine the size of the key object
	status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
		(PBYTE)&keyObjectSize, sizeof(DWORD), &bytesWritten, 0);
	if (!NT_SUCCESS(status)) {
		printf("[-] BCryptGetProperty failed: 0x%0.8X\n", status);
		success = FALSE;
		goto Cleanup;
	}

	//allocate the key object buffer
	pKeyObject = (unsigned char*)HeapAlloc(GetProcessHeap(), 0, keyObjectSize);
	if (pKeyObject == NULL) {
		printf("[-] HeapAlloc failed\n");
		success = FALSE;
		goto Cleanup;
	}

	//CBC chaining mode, uses the IV to chain each block together,
	//identical plaintext blocks produce different ciphertext blocks
	status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, 
		(PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
	if (!NT_SUCCESS(status)) {
		printf("[-] BCryptSetProperty failed: 0x%0.8X\n", status);
		success = FALSE;
		goto Cleanup;
	}

	//generate the key from the supplied key bytes
	status = BCryptGenerateSymmetricKey(hAlg, &hKey, pKeyObject, keyObjectSize, 
		pAes->pKey, AESKEYSIZE, 0);
	if (!NT_SUCCESS(status)) {
		printf("[-] BCryptGenerateSymmetricKey failed: 0x%0.8X\n", status);
		success = FALSE;
		goto Cleanup;
	}

	//not encrypting yet, just determining the size of the ciphertext buffer needed
	status = BCryptEncrypt(hKey, pAes->pPlainText, pAes->dwPlainSize, NULL, 
		pAes->pIv, AESIVSIZE, NULL, 0, &cipherTextSize, BCRYPT_BLOCK_PADDING);
	if (!NT_SUCCESS(status)) {
		printf("[-] BCryptEncrypt (size determination) failed: 0x%0.8X\n", status);
		success = FALSE;
		goto Cleanup;
	}

	//allocate the ciphertext buffer now that we know the size needed
	pCipherText = (unsigned char*)HeapAlloc(GetProcessHeap(), 0, cipherTextSize);
	if (pCipherText == NULL) {
		printf("[-] HeapAlloc for cipher text failed\n");
		success = FALSE;
		goto Cleanup;
	}

	//second bcrypt call actually encrypts the data into the allocated buffer
	status = BCryptEncrypt(hKey, pAes->pPlainText, pAes->dwPlainSize, NULL,
		pAes->pIv, AESIVSIZE, pCipherText, cipherTextSize, &bytesWritten, BCRYPT_BLOCK_PADDING);
	if (!NT_SUCCESS(status)) {
		printf("[-] BCryptEncrypt failed: 0x%0.8X\n", status);
		success = FALSE;
		goto Cleanup;
	}

	//save results back into the struct
	pAes->pCipherText = pCipherText;
	pAes->dwCipherSize = bytesWritten;

Cleanup:
	if (hKey) BCryptDestroyKey(hKey);
	if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
	if (pKeyObject) HeapFree(GetProcessHeap(), 0, pKeyObject);

	//free ciphertext on failure, otherwise caller owns memory and is responsible for freeing it
	if (!success && pCipherText) {
		HeapFree(GetProcessHeap(), 0, pCipherText);
	}

	return success;
}

// so the caller does not need to build an AES_PARAMS struct manually
BOOL SimpleEncrypt(
	unsigned char* pPlainText,
	DWORD           plainSize,
	unsigned char* pKey,
	unsigned char* pIv,
	unsigned char** pCipherText,   // OUT - caller must HeapFree this
	DWORD* pCipherSize    // OUT
) {
	if (pPlainText == NULL || plainSize == 0 || pKey == NULL || pIv == NULL) {
		return FALSE;
	}

	AES_PARAMS aes = {
		.pPlainText = pPlainText,
		.dwPlainSize = plainSize,
		.pKey = pKey,
		.pIv = pIv
	};

	if (!AesEncrypt(&aes)) {
		return FALSE;
	}

	*pCipherText = aes.pCipherText;
	*pCipherSize = aes.dwCipherSize;

	return TRUE;
}