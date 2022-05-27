from BitVector import *
import time
from aes import *

def print_text(text_type, text_bv):
    print(text_type + ':')
    print(text_bv.get_bitvector_in_ascii(), '[In ASCII]')
    print(text_bv.get_bitvector_in_hex(), '[In HEX]\n')

# input plaintext
plaintext = 'CanTheyDoTheirFest'
# plaintext = input('Enter plaintext: ')
plaintext_bv = BitVector(textstring=plaintext)

# input key
key = 'BUET CSE17 Batch'
# key = input('Enter key: ')
if len(key) > 16:
    key = key[0:16]
elif len(key) < 16:
    key = key + ' ' * (16 - len(key))

# key scheduling
key_bv = BitVector(textstring=key)
key_scheduling_time = time.perf_counter()
schedule_roundkeys(key_bv)
key_scheduling_time = time.perf_counter() - key_scheduling_time

# encryption
ciphertext_bv = BitVector(size=0)
encryption_time = time.perf_counter()
for i in range(len(plaintext_bv) // 128):
    ciphertext_bv += encrypt(plaintext_bv[i * 128: i * 128 + 128])
encryption_time = time.perf_counter() - encryption_time

# decryption
deciphered_text_bv = BitVector(size=0)
decryption_time = time.perf_counter()
for i in range(len(ciphertext_bv) // 128):
    deciphered_text_bv += decrypt(ciphertext_bv[i * 128: i * 128 + 128])
decryption_time = time.perf_counter() - decryption_time

print_text('Plain Text', plaintext_bv)
print_text('Key', key_bv)
print_text('Cipher Text', ciphertext_bv)
print_text('Deciphered Text', deciphered_text_bv)
print('Execution Time:\nKey Scheduling:', key_scheduling_time, 'seconds')
print('Encryption:', encryption_time, 'seconds')
print('Decryption:', decryption_time, 'seconds')
