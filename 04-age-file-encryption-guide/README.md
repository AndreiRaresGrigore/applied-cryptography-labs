# 04 · age File-Encryption Guide

A practical guide to [**age**](https://github.com/FiloSottile/age) ("Actually Good Encryption"), a modern and minimal alternative to GPG/OpenSSL for encrypting files.

📄 [Guide (PDF)](report.pdf)

## Cryptography under the hood

| Mode | Key setup | Encryption |
|---|---|---|
| Passphrase | **scrypt**, a memory-hard KDF that slows down GPU/ASIC brute force | **ChaCha20-Poly1305** (AEAD) |
| Public key | **X25519** elliptic-curve Diffie-Hellman between a fresh ephemeral key and the recipient's public key | **ChaCha20-Poly1305** (AEAD) |

In both cases a random **file key** encrypts the data. The header only carries that file key, wrapped for each recipient (one "stanza" per recipient), followed by a MAC over the header.

```
age-encryption.org/v1
-> X25519 <ephemeral public key>
<file key encrypted for this recipient>
--- <header MAC>
<binary payload: ChaCha20-Poly1305 chunks>
```

## Walkthrough

```bash
./age_demo.sh
```

The script covers:

1. **Passphrase encryption** with an auto-generated passphrase (`age -p`), inspecting the scrypt header, then decryption.
2. **Key pair generation** (`age-keygen`), encryption to the public key (`age -r`), inspecting the X25519 header, then decryption with the identity file (`age -d -i`).

> The identity file created by `age-keygen` contains the **private key**. Keep it secret and never commit it.

## Why age

- **No configuration choices that can be got wrong:** no cipher, mode or padding to pick.
- **Authenticated by default.** Tampering with the file makes decryption fail.
- **Short, copy-pasteable keys** (`age1…`), and support for multiple recipients.
