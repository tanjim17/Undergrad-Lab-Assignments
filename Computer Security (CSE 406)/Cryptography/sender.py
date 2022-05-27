import socket, os
from aes import *
from rsa import *

HOST = '127.0.0.1'
PORT = 12345

with socket.socket() as s:
    s.connect((HOST, PORT))
    while True:
        # file input
        filepath = input('Enter file path: ')
        basename = os.path.basename(filepath)
        filename, extension = basename.split('.')
        pad_count = 0
        if extension == 'txt':
            with open(filepath, 'r') as f:
                plaintext = f.read()
            if (len(plaintext) % 16) != 0:
                pad_count = 16 - len(plaintext) % 16
                plaintext = plaintext + '0' * pad_count
            plaintext_bv = BitVector(textstring=plaintext)
        else:
            with open(filepath, 'rb') as f:
                plaintext = bytes.hex(f.read())
            if (len(plaintext) % 32) != 0:
                pad_count = 32 - len(plaintext) % 32
                plaintext = plaintext + '0' * pad_count
            plaintext_bv = BitVector(hexstring=plaintext)

        # input key
        aes_key = input('Enter AES key: ')
        if len(aes_key) > 16:
            aes_key = aes_key[0:16]
        elif len(aes_key) < 16:
            aes_key = aes_key + ' ' * (16 - len(aes_key))
        key_bv = BitVector(textstring=aes_key)

        # input k for RSA
        k = int(input('Enter value of k for RSA: '))
        
        # plaintext encryption
        schedule_roundkeys(BitVector(textstring=aes_key))
        ciphertext_bv = BitVector(size=0)
        for i in range(plaintext_bv.length() // 128):
            ciphertext_bv += encrypt(plaintext_bv[i * 128: (i + 1) * 128])
            print('encrypted: {}B/{}B'.format((i + 1) * 16, plaintext_bv.length() // 8))

        # aes key encryption
        e, d, n = generate_key_pair(k)
        with open('./Don\'t Open This/privatekey.txt', 'w') as f:
            f.write(str([d, n]))
        ciphered_aes_key = []
        for p in aes_key:
            ciphered_aes_key.append(transform(ord(p), e, n))

        # send data
        s.sendall(ciphertext_bv.get_bitvector_in_ascii().encode())
        s.recv(8192)
        s.sendall(str(ciphered_aes_key).encode())
        s.recv(8192)
        s.sendall(str([e, n]).encode())
        s.recv(8192)
        s.sendall((str(pad_count) + ',' + extension).encode())
        break
print('File sent.')