#include "Encryption.h"
#include <stdio.h>


// SystemFunction032 expects data passed in this specific struct
// Length        - how many bytes are actually used
// MaximumLength - how big the buffer is (same as Length for our purposes)
// Buffer        - pointer to the actual data
typedef struct {
    DWORD   Length;
    DWORD   MaximumLength;
    void* Buffer;
} RC4_BLOB;

// function pointer type matching SystemFunction032's signature
// we need this because we're loading it manually at runtime
typedef NTSTATUS(NTAPI* fnSystemFunction032)(RC4_BLOB* data, RC4_BLOB* key);


BOOL Rc4Encrypt(unsigned char* pKey, DWORD keySize, unsigned char* pData, DWORD dataSize) {

    if (pKey == NULL || keySize == 0 || pData == NULL || dataSize == 0) {
        return FALSE;
    }

    // wrap key and data in the struct format SystemFunction032 expects
    RC4_BLOB Key = { .Buffer = pKey,  .Length = keySize,  .MaximumLength = keySize };
    RC4_BLOB Data = { .Buffer = pData, .Length = dataSize, .MaximumLength = dataSize };

    // load Advapi32.dll into our process at runtime
    // then get the address of SystemFunction032 from its export table
    // we do it this way because SystemFunction032 is undocumented and has no header
    HMODULE hAdvapi32 = LoadLibraryA("Advapi32.dll");
    if (hAdvapi32 == NULL) {
        printf("[-] Failed to load Advapi32.dll\n");
        return FALSE;
    }

    fnSystemFunction032 SystemFunction032 = (fnSystemFunction032)GetProcAddress(
        hAdvapi32,
        "SystemFunction032"
    );
    if (SystemFunction032 == NULL) {
        printf("[-] Failed to find SystemFunction032\n");
        return FALSE;
    }

    // RC4 is symmetric - the same operation encrypts and decrypts
    // call it on plaintext to encrypt, call it again on ciphertext to decrypt
    NTSTATUS status = SystemFunction032(&Data, &Key);
    if (!NT_SUCCESS(status)) {
        printf("[-] SystemFunction032 failed: 0x%0.8X\n", status);
        return FALSE;
    }

    return TRUE;
}