#include <Windows.h>
#include <stdio.h>
#include "helpers.h"
#include "Encryption.h"

// Prints usage instructions for the program
void PrintUsage(const char* programName) {
    fprintf(stderr, "\nShellFuscate - Shellcode obfuscation tool\n\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s -f <file> [options]\n\n", programName);
    fprintf(stderr, "Required:\n");
    fprintf(stderr, "  -f <file>       Path to raw shellcode binary\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -fmt <format>   Obfuscation format: ipv4, ipv6, uuid, mac\n");
    fprintf(stderr, "  -enc <type>     Encryption: aes, rc4\n");
    fprintf(stderr, "  -h              Show this help message\n\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s -f shell.bin -fmt ipv4\n", programName);
    fprintf(stderr, "  %s -f shell.bin -enc aes\n", programName);
    fprintf(stderr, "  %s -f shell.bin -fmt uuid -enc aes\n\n", programName);
}

//Reads the contents of a shellcode file into a buffer and returns it
unsigned char* ReadShellcodeFile(const char* filePath, DWORD* pSize) {

    FILE* f = fopen(filePath, "rb");
    if (f == NULL) {
        fprintf(stderr, "[-] Failed to open file: %s\n", filePath);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    DWORD size = (DWORD)ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size == 0) {
        fprintf(stderr, "[-] File is empty: %s\n", filePath);
        fclose(f);
        return NULL;
    }

    unsigned char* pBuffer = (unsigned char*)malloc(size);
    if (pBuffer == NULL) {
        fprintf(stderr, "[-] Failed to allocate buffer\n");
        fclose(f);
        return NULL;
    }

    fread(pBuffer, 1, size, f);
    fclose(f);

    *pSize = size;
    return pBuffer;
}

