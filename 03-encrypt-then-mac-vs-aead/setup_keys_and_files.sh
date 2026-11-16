#!/bin/bash
# Master key (32 random bytes) + test files of 10, 20 and 30 MB
set -e
openssl rand 32 > master.key
xxd -p master.key | tr -d '\n' > masterkey.hex
dd if=/dev/urandom of=10MB.bin bs=1M count=10
dd if=/dev/urandom of=20MB.bin bs=1M count=20
dd if=/dev/urandom of=30MB.bin bs=1M count=30
