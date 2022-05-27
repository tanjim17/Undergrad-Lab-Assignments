import socket, os
from aes import *
from rsa import *

HOST = '127.0.0.1'
PORT = 12345

with socket.socket() as s:
    s.connect((HOST, PORT))
    while True:
        # filepath = input('Enter file path: ')
        filepath = 'input.txt'
        basename = os.path.basename(filepath)
        filename, extension = basename.split('.')
        pad_count = 0
        if extension == 'txt':
            with open(filepath, 'r') as f:
                plaintext = f.read()
            if (len(plaintext) % 16) != 0:
                pad_count = 16 - len(plaintext) % 16
                plaintext = plaintext + ' ' * pad_count
                plaintext_bv = BitVector(textstring=plaintext)
        else:
            with open(filepath, 'rb') as f:
                plaintext = bytes.hex(f.read())
            if (len(plaintext) % 32) != 0:
                pad_count = 32 - len(plaintext) % 32
                plaintext = plaintext + ' ' * pad_count
                plaintext_bv = BitVector(hexstring=plaintext)

        aes_key = 'BUET CSE17 Batch'
        schedule_roundkeys(BitVector(textstring=aes_key))

        # plaintext encryption
        ciphertext_bv = BitVector(size=0)
        for i in range(len(plaintext_bv) // 128):
            print('a', i)
            ciphertext_bv += encrypt(plaintext_bv[i * 128: i * 128 + 128])

        # aes key encryption
        e, d, n = generate_key_pair(16)
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

        # # decryption
        # deciphered_text_bv = BitVector(size=0)
        # for i in range(len(ciphertext_bv) // 128):
        #     print('b', i)
        #     deciphered_text_bv += decrypt(ciphertext_bv[i * 128: i * 128 + 128])

        # # output non-text file
        # if extension != 'txt':
        #     file = open('./output/' + basename, "wb+")
        #     file.write(bytes.fromhex(deciphered_text_bv.get_bitvector_in_hex()[:len(
        #         deciphered_text_bv.get_bitvector_in_hex()) - pad_count]))
        #     file.close()

        #print(deciphered_text_bv.get_bitvector_in_ascii())
        break