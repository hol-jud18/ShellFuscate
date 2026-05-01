# ShellFuscate

A Windows shellcode obfuscation tool written in C. Takes raw shellcode binaries and transforms them into human-readable string arrays — IPv4 addresses, IPv6 addresses, UUIDs, or MAC-like addresses — with optional AES-256-CBC or RC4 encryption before formatting. Output is ready-to-paste C code for use in shellcode loaders.

---

## What It Does

Static analysis tools and AV signatures often scan for byte patterns in PE files. Encoding shellcode as network-format strings disguises those patterns as plausible-looking data. ShellFuscate handles both the encoding step and the encryption step, producing a C array declaration that a loader can decode at runtime.

**Obfuscation formats**
| Format | Bytes per element | Example output |
|--------|-------------------|----------------|
| IPv4   | 4                 | `"144.215.40.13"` |
| IPv6   | 16                | `"FC4E:9001:2F35:8B09:..."` |
| UUID   | 16                | `"2801904F-C04E-359F-8B09-..."` |
| MAC    | 5                 | `"FC:4E:90:01:2F"` |

**Encryption**
- **AES-256-CBC** — via the Windows BCrypt API (`bcrypt.lib`). Randomly generates a 256-bit key and 128-bit IV, then prints both as byte arrays alongside the ciphertext.
- **RC4** — via `SystemFunction032` in `Advapi32.dll`, resolved at runtime with `GetProcAddress`. Avoids a static import entry for an encryption-related symbol; RC4's symmetric property means the same call encrypts and decrypts.

Both encryption modes can be combined with any obfuscation format.

---

## Building

Visual Studio 2022 project files are included (`ShellFuscate.slnx`, `ShellFuscate.vcxproj`). Open the solution and build, or use the Developer Command Prompt:

```
msbuild ShellFuscate.slnx /p:Configuration=Release /p:Platform=x64
```

**Dependencies:** Windows SDK (BCrypt, Advapi32 — both ship with the SDK).

---

## Usage

```
ShellFuscate -f <file> [options]

Required:
  -f <file>       Path to raw shellcode binary

Options:
  -fmt <format>   Obfuscation format: ipv4, ipv6, uuid, mac
  -enc <type>     Encryption: aes, rc4
  -h              Show this help message
```

**Size requirements** — each format encodes a fixed number of bytes per element. ShellFuscate automatically null-pads the shellcode buffer to the next valid multiple before encoding, so no manual padding is needed:
- IPv4: multiple of 4
- IPv6 / UUID: multiple of 16
- MAC: multiple of 5

### Examples

Encode as IPv4 addresses:
```
ShellFuscate.exe -f shell.bin -fmt ipv4
```
```c
char* IPv4Array[64] = {
    "252.72.131.228",
    "240.232.192.0",
    ...
    "104.1.0.0.0"
};

#define NumberOfElements 64
```

Encrypt with AES, then encode as UUIDs:
```
ShellFuscate.exe -f shell.bin -enc aes -fmt uuid
```
```c
unsigned char AesKey[] = { 0x3A, 0x1F, ... };
unsigned char AesIv[]  = { 0xC2, 0x44, ... };

char* UuidArray[16] = {
    "E3A12F4B-9C01-3D7F-A2B8-5F1E9C3D8A4E",
    ...
};

#define NumberOfElements 16
```

Encrypt with RC4 only (no format encoding):
```
ShellFuscate.exe -f shell.bin -enc rc4
```

---

## Implementation Notes

**AES via BCrypt**
The Windows BCrypt API (`BCryptOpenAlgorithmProvider` → `BCryptGenerateSymmetricKey` → `BCryptEncrypt`) is called with `BCRYPT_CHAIN_MODE_CBC` and `BCRYPT_BLOCK_PADDING`. Two calls to `BCryptEncrypt` are made: the first with a NULL output buffer to determine the required ciphertext size, the second to perform the actual encryption. Cleanup is handled with `goto Cleanup` to ensure handles are always closed.

**RC4 via undocumented `SystemFunction032`**
Rather than linking a third-party RC4 implementation, the tool uses `SystemFunction032` — an undocumented export in `Advapi32.dll` that performs RC4 in-place. The function accepts two `RC4_BLOB` structs (data and key). Loading it at runtime via `GetProcAddress` avoids a static import entry.

**UUID byte ordering**
UUIDs store the first 4-byte group in little-endian order. `GenerateUUID` reverses bytes 0–3 when formatting so that the UUID string round-trips correctly back to the original shellcode bytes when decoded with standard UUID parsing.

**Automatic padding**
`PadShellcode` null-pads the shellcode buffer to the next multiple of the format's element size before encoding. It uses `realloc` so the extension is done in-place where possible, and zeroes only the appended bytes with `memset`.

---

## Project Structure

```
ShellFuscate/
├── ShellFuscate.c   - Entry point, CLI argument parsing, file I/O
├── Obfuscation.c    - IPv4 / IPv6 / UUID / MAC encoding logic
├── aes.c            - AES-256-CBC encryption (BCrypt)
├── rc4.c            - RC4 encryption (SystemFunction032)
├── Encryption.h     - AES_PARAMS struct, function declarations
└── helpers.h        - Obfuscation function declarations
```

---