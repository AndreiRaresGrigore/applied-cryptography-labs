#!/bin/bash
# Encrypt-then-MAC: AES-128-CTR, then HMAC-SHA256 over the ciphertext.
# Run setup_keys_and_files.sh first. Keys and IV are derived from the master key with HMAC-SHA256
# and a different label for each purpose.
set -e

echo -n "aes-128-ctr-enc" | openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat masterkey.hex) -binary | head -c 16 | xxd -p -c 256 > criptkey.hex
echo -n "hmac-sha256-key" | openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat masterkey.hex) -binary | xxd -p -c 256 > hmackey.hex
echo -n "aes-iv-16bytes" | openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat masterkey.hex) -binary | head -c 16 | xxd -p -c 256 > ivkey.hex

time (openssl enc -aes-128-ctr -in 10MB.bin -out encrypted10.bin -K $(cat criptkey.hex) -iv $(cat ivkey.hex) && openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat hmackey.hex) -out hmac.bin encrypted10.bin)
time (openssl enc -aes-128-ctr -in 20MB.bin -out encrypted20.bin -K $(cat criptkey.hex) -iv $(cat ivkey.hex) && openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat hmackey.hex) -out hmac.bin encrypted20.bin)
time (openssl enc -aes-128-ctr -in 30MB.bin -out encrypted30.bin -K $(cat criptkey.hex) -iv $(cat ivkey.hex) && openssl dgst -sha256 -mac hmac -macopt hexkey:$(cat hmackey.hex) -out hmac.bin encrypted30.bin)
