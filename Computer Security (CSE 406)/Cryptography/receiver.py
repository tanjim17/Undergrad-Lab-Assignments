import socket
from BitVector import *
from aes import *
from rsa import *

HOST = '127.0.0.1'
PORT = 12345

def convert_str_to_int_list(s):
    s = s[1:-1].split(', ')
    return [int(x) for x in s] 

with socket.socket() as s:
    s.bind((HOST, PORT))
    s.listen()
    print('Waiting for connection...')
    conn, _ = s.accept()
    with conn:
        while True:
            data = conn.recv(8192)
            ciphertext_bv = BitVector(textstring=data.decode())
            conn.send(b'ack')

            data = conn.recv(8192)
            ciphered_aes_key = convert_str_to_int_list(data.decode())
            conn.send(b'ack')

            data = conn.recv(8192)
            publickey = convert_str_to_int_list(data.decode())
            conn.send(b'ack')

            data = conn.recv(8192)
            pad_count, extension = data.decode().split(',')
            pad_count = int(pad_count)

            # decrypt aes key
            with open('./Don\'t Open This/privatekey.txt', 'r') as f:
                d, n = convert_str_to_int_list(f.read())
            deciphered_aes_key = ''
            for c in ciphered_aes_key:
                deciphered_aes_key += chr(transform(c, d, n))

            # decrypt file
            schedule_roundkeys(BitVector(textstring=deciphered_aes_key))
            deciphered_text_bv = BitVector(size=0)
            for i in range(ciphertext_bv.length() // 128):
                deciphered_text_bv += decrypt(ciphertext_bv[i * 128: (i + 1) * 128])
                print('decrypted: {}B/{}B'.format((i + 1) * 16, ciphertext_bv.length() // 8))

            # output file
            if extension == 'txt':
                with open('./output/' + 'new.txt', 'w') as f:
                    f.write(deciphered_text_bv.get_bitvector_in_ascii()[:len(
                        deciphered_text_bv.get_bitvector_in_ascii()) - pad_count])
            else:
                with open('./output/' + 'new.' + extension, 'wb') as f:
                    f.write(bytes.fromhex(deciphered_text_bv.get_bitvector_in_hex()[:len(
                        deciphered_text_bv.get_bitvector_in_hex()) - pad_count]))
            break
    print('File received.\nConnection closed.')
