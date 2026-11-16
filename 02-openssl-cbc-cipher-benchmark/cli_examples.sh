#!/bin/bash
# Command-line examples from the report: test files, then CBC encryption/decryption
# with AES, Camellia and SM4 (OpenSSL asks for a passphrase).
set -e

# Test files: 16 bytes, 20 KB, 3 MB
dd if=/dev/urandom of=16byte.bin bs=16 count=1
dd if=/dev/urandom of=20KB.bin bs=1024 count=20
dd if=/dev/urandom of=3MB.bin bs=1M count=3

# AES-128-CBC
time openssl enc -aes-128-cbc -in 16byte.bin -out 16byte.enc
time openssl enc -d -aes-128-cbc -in 16byte.enc -out 16byte.dec

# Camellia-128-CBC
time openssl enc -camellia-128-cbc -in 20KB.bin -out 20KB.enc
time openssl enc -d -camellia-128-cbc -in 20KB.enc -out 20KB.dec

# SM4-CBC
time openssl enc -sm4-cbc -in 3MB.bin -out 3MB.enc
time openssl enc -d -sm4-cbc -in 3MB.enc -out 3MB.dec
