#!/bin/bash
# Walkthrough of the age examples in the guide: passphrase encryption, then public-key encryption.
set -e

# 1) Symmetric encryption with an auto-generated passphrase (scrypt + ChaCha20-Poly1305)
echo "document content 1" > document1.txt
age -p -o document1.txt.age document1.txt   # leave the prompt empty: age generates and prints a passphrase
head -n 5 document1.txt.age   # header: 'age-encryption.org/v1' + scrypt stanza
age -d -o decrypted1.txt document1.txt.age
cat decrypted1.txt

# 2) Public-key encryption (X25519 + ChaCha20-Poly1305)
echo "document content 2" > document2.txt
age-keygen -o identity.txt   # the file holds the PRIVATE key (identity); keep it secret
age -r "$(age-keygen -y identity.txt)" -o document2.txt.age document2.txt
head -n 5 document2.txt.age   # header: 'age-encryption.org/v1' + X25519 stanza
age -d -i identity.txt -o decrypted2.txt document2.txt.age
cat decrypted2.txt
