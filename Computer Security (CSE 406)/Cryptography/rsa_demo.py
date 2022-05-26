from rsa import *
import time

# input k
k = 16
# k = int(input('Enter value of k: '))

# input plaintext
plaintext = 'CanTheyDoTheirFest'
# plaintext = input('Enter plaintext: ')

# key generation
# e_bv, d_bv, n_bv = generate_key_pair(k)
key_generation_time = time.time()
e, d, n = generate_key_pair(k)
key_generation_time = time.time() - key_generation_time

# encryption
cipher = []
encryption_time = time.time()
for p in plaintext:
    cipher.append(transform(ord(p), e, n))
encryption_time = time.time() - encryption_time

# decryption
deciphered_text = ''
decryption_time = time.time()
for c in cipher:
    deciphered_text += chr(transform(c, d, n))
decryption_time = time.time() - decryption_time

print('Generated Keys:\nPublic: ({}, {})\nPrivate: ({}, {})\n'.format(e, n, d, n))
print('Plain Text:\n{}\n'.format(plaintext))
print('Cipher:\n{}\n'.format(cipher))
print('Deciphered Text:\n{}\n'.format(deciphered_text))
print('Execution Time:\nKey Generation:', key_generation_time, 'seconds')
print('Encryption:', encryption_time, 'seconds')
print('Decryption:', decryption_time, 'seconds')