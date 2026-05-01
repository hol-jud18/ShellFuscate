#pragma once

// Functions from ShellFuscate.c
unsigned char* ReadShellcodeFile(const char* filePath, DWORD* pSize);
unsigned char* PadShellcode(unsigned char* pShellcode, DWORD* pSize, SIZE_T alignment);

// Functions from Obfuscation.c
// Obfuscate shellcode into UUID characters
BOOL ObfuscateUUID(unsigned char* pShellcode, SIZE_T ShellcodeSize);
// Obfuscate shellcode into MAC address characters
BOOL ObfuscateMAC(unsigned char* pShellcode, SIZE_T ShellcodeSize);
// Obfuscate shellcode into IPv6 address characters
BOOL ObfuscateIPv6(unsigned char* pShellcode, SIZE_T ShellcodeSize);
// Obfuscate shellcode into IPv4 address characters
BOOL ObfuscateIPv4(unsigned char* pShellcode, SIZE_T ShellcodeSize);