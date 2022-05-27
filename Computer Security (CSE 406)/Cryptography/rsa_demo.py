from rsa import *
import time
from prettytable import PrettyTable

time_info = PrettyTable(['K', 'Key Generation', 'Encryption', 'Decryption'])

# input plaintext
plaintext = input('Enter plaintext: ')

# time count
for k in [16, 32, 64, 128]:
    # key generation
    key_generation_time = time.perf_counter()
    e, d, n = generate_key_pair(k)
    key_generation_time = time.perf_counter() - key_generation_time

    # encryption
    cipher = []
    encryption_time = time.perf_counter()
    for p in plaintext:
        cipher.append(transform(ord(p), e, n))
    encryption_time = time.perf_counter() - encryption_time

    # decryption
    deciphered_text = ''
    decryption_time = time.perf_counter()
    for c in cipher:
        deciphered_text += chr(transform(c, d, n))
    decryption_time = time.perf_counter() - decryption_time

    print('k =', k)
    print('Public Key: ({}, {})'.format(e, n))
    print('Private Key: ({}, {})'.format(d, n))
    print('Plain Text:', plaintext)
    print('Cipher:', cipher)
    print('Deciphered Text:', deciphered_text, '\n')
    time_info.add_row([k, '{:.5e}'.format(key_generation_time), '{:.5e}'.format(encryption_time),
        '{:.5e}'.format(decryption_time)])

print('Time Info (in seconds):')
print(time_info)
