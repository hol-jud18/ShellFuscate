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

char* GenerateIPv4(int b0, int b1, int b2, int b3) {
	char* result = (char*)malloc(64);
	if (!result) return NULL;

	// Format the bytes into an IPv4 string b0.b1.b2.b3
	sprintf(result, "%d.%d.%d.%d", b0, b1, b2, b3);
	return result;
}