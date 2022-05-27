from ctypes.wintypes import CHAR
from pydoc import pager
from BitVector import *

def generate_primes(k):
    bv = BitVector(intVal = 0)
    primes = set()
    while True:
        bv = bv.gen_random_bits(k//2)
        if bv.test_for_primality() != 0:
            primes.add(int(bv))
            if len(primes) == 2:
                break
    return primes

def generate_key_pair(k):
    p, q = generate_primes(k)
    # p, q = int(p_bv), int(q_bv)
    n = p * q
    phi_n = (p - 1) * (q - 1)
    phi_n_bv = BitVector(intVal=phi_n)

    for e in range(2, phi_n):
        e_bv = BitVector(intVal=e)
        if e_bv.gcd(phi_n_bv) == BitVector(intVal=1):
            break
    d_bv = e_bv.multiplicative_inverse(phi_n_bv)
    # return e_bv, d_bv, BitVector(intVal=n)
    return e, int(d_bv), n

def transform(char, key, n):
    return pow(char, key, n)