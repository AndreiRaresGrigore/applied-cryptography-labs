"""
Three CS-PRNGs / DRBGs generating the same number of bits from a common 32-byte seed:
Blum Blum Shub, CTR_DRBG (AES in counter mode) and Hash_DRBG (SHA-256 in counter mode).
Prints generation time, 0/1 balance and the first 50 bits of each generator.

Requires: pip install pycryptodome
"""
import hashlib
import os
import time

from Crypto.Cipher import AES
from Crypto.Util.number import GCD, getPrime

bits_num = 100000  # 100,000 bits, as in the measurements of the report
seed = os.urandom(32)
def generate_blum_prime(bits=512):
    while True:
        p = getPrime(bits)
        if p % 4 == 3:
            return p
p = generate_blum_prime()
q = generate_blum_prime()
n = p * q
s = int.from_bytes(seed, 'big') % n
while GCD(s, n) != 1:
    s = (s + 1) % n
xi = pow(s, 2, n)
t1 = time.time()
out1 = ""
j = bits_num
while j > 0:
    xi = pow(xi, 2, n)
    bit = xi & 1
    out1 += str(bit)
    j -= 1
t1 = time.time() - t1
num0_1 = out1.count('0')
num1_1 = out1.count('1')

k = hashlib.sha256(seed).digest()[:16]
v = 0
t2 = time.time()
out2 = ""
j = bits_num
while j > 0:
    v = (v + 1)%(2**128)
    aes = AES.new(k, AES.MODE_ECB)
    cbytes = v.to_bytes(16, 'big')
    cbytes_ecrypt = aes.encrypt(cbytes)
    for i in cbytes_ecrypt:
        out2 += f"{i:08b}"
        j -= 8
        if j <= 0:
            break
t2 = time.time() - t2
num0_2 = out2.count('0')
num1_2 = out2.count('1')

count = 0
t3 = time.time()
out3 = ""
j = bits_num
while j > 0:
    hashin = seed + count.to_bytes(8, 'big')
    hashout = hashlib.sha256(hashin).digest()
    for i in hashout:
        out3 += f"{i:08b}"
        j -= 8
        if j <= 0:
            break
    count += 1
t3 = time.time() - t3
num0_3 = out3.count('0')
num1_3 = out3.count('1')


def report(name, t, out):
    zeros, ones = out.count("0"), out.count("1")
    n = len(out)
    print(f"{name}: {t:.3f}s  0:{zeros / n:.1%}({zeros})  1:{ones / n:.1%}({ones})  first bits: {out[:50]}")


print("Results:")
report("BBS", t1, out1)
report("AES", t2, out2)
report("SHA", t3, out3)
