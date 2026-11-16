#!/bin/bash
# Encrypt-then-MAC: ChaCha20, then HMAC-SHA256 over the ciphertext.
# Run setup_keys_and_files.sh first.
set -e

echo -n "chacha20-enc-key" | openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat masterkey.hex) -binary | head -c 32 | xxd -p -c 256 > criptkey.hex
echo -n "hmac-sha256-key" | openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat masterkey.hex) -binary | xxd -p -c 256 > hmackey.hex
echo -n "chacha20-iv-16b" | openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat masterkey.hex) -binary | head -c 16 | xxd -p -c 256 > ivkey.hex

time (openssl enc -chacha20 -in 10MB.bin -out encrypted10.bin -K $(cat criptkey.hex) -iv $(cat ivkey.hex) && openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat hmackey.hex) -out hmac10.bin encrypted10.bin)
time (openssl enc -chacha20 -in 20MB.bin -out encrypted20.bin -K $(cat criptkey.hex) -iv $(cat ivkey.hex) && openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat hmackey.hex) -out hmac20.bin encrypted20.bin)
time (openssl enc -chacha20 -in 30MB.bin -out encrypted30.bin -K $(cat criptkey.hex) -iv $(cat ivkey.hex) && openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat hmackey.hex) -out hmac30.bin encrypted30.bin)
