from ctypes.wintypes import WORD
from BitVector import *
import time

WORD_LEN = 4 # 4 bytes 
AES_modulus = BitVector(bitstring='100011011')

Sbox = (
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16,
)

InvSbox = (
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D,
)

Mixer = [
    [BitVector(hexstring="02"), BitVector(hexstring="03"), BitVector(hexstring="01"), BitVector(hexstring="01")],
    [BitVector(hexstring="01"), BitVector(hexstring="02"), BitVector(hexstring="03"), BitVector(hexstring="01")],
    [BitVector(hexstring="01"), BitVector(hexstring="01"), BitVector(hexstring="02"), BitVector(hexstring="03")],
    [BitVector(hexstring="03"), BitVector(hexstring="01"), BitVector(hexstring="01"), BitVector(hexstring="02")]
]

InvMixer = [
    [BitVector(hexstring="0E"), BitVector(hexstring="0B"), BitVector(hexstring="0D"), BitVector(hexstring="09")],
    [BitVector(hexstring="09"), BitVector(hexstring="0E"), BitVector(hexstring="0B"), BitVector(hexstring="0D")],
    [BitVector(hexstring="0D"), BitVector(hexstring="09"), BitVector(hexstring="0E"), BitVector(hexstring="0B")],
    [BitVector(hexstring="0B"), BitVector(hexstring="0D"), BitVector(hexstring="09"), BitVector(hexstring="0E")]
]

roundkeys = []
key_scheduling_time = 0
encryption_time = 0
decryption_time = 0

# defining auxiliary functions
def input_key():
    key = 'Thats my Kung Fu'
    # key = input('Enter key: ')
    if len(key) > 16:
        key = key[0:16]
    elif len(key) < 16:
        key = key + ' ' * (16 - len(key))
    roundkeys.append(BitVector(textstring=key))

def input_plaintext():
    file = open('input.txt', 'r')
    plaintext = file.read()
    file.close()
    if len(plaintext) % 16 != 0:
        plaintext = plaintext + ' ' * (16 - len(plaintext) % 16)
    return BitVector(textstring=plaintext)

def convert_bitvector_to_matrix(bitvector):
    state_matrix = []
    for i in range(WORD_LEN):
        row = []
        for j in range(bitvector.length() // (8 * WORD_LEN)):
            row.append(bitvector[(i * 8 + j * 32): (i * 8 + j * 32) + 8])
        state_matrix.append(row)
    return state_matrix

def convert_matrix_to_bitvector(state_matrix):
    bitvector = BitVector(size=0)
    for j in range(len(state_matrix[0])):
        for i in range(len(state_matrix)):
            bitvector += state_matrix[i][j]
    return bitvector

def g(word, rc):
    word = word << 8
    word = sub_bytes(word, Sbox)
    word = word ^ rc
    return word

def schedule_roundkeys():
    global AES_modulus
    rc = BitVector(hexstring="01")
    for i in range(10):
        rcon = rc
        rcon += BitVector(hexstring="000000")

        w0 = roundkeys[i][0: 32] ^ g(roundkeys[i][96:128], rcon)
        w1 = w0 ^ roundkeys[i][32: 64]
        w2 = w1 ^ roundkeys[i][64: 96]
        w3 = w2 ^ roundkeys[i][96: 128]

        new_roundkey = w0
        new_roundkey += w1
        new_roundkey += w2
        new_roundkey += w3
        roundkeys.append(new_roundkey)
        
        # prev_rc = rc
        # rc = BitVector(hexstring="02").gf_multiply(prev_rc)
        # if prev_rc >= BitVector(hexstring='80'):
        #     rc ^= BitVector(hexstring='11b')
        rc = BitVector(hexstring='02').gf_multiply_modular(rc, AES_modulus, 8)

def sub_bytes(bitvector, sBox):
    new_bitvector = BitVector(size=0)
    for i in range(0, bitvector.length(), 8):
        new_bitvector += BitVector(intVal=sBox[bitvector[i: i+8].intValue()], size=8)
    return new_bitvector

def shift_rows(bitvector, isInverse):
    state_matrix = convert_bitvector_to_matrix(bitvector)
    for i in range(WORD_LEN):
        shift_idx = len(state_matrix[i]) - i if isInverse else i 
        state_matrix[i] = state_matrix[i][shift_idx:] + state_matrix[i][:shift_idx]
    return convert_matrix_to_bitvector(state_matrix)

def mix_col(bitvector, mixer):
    state = convert_bitvector_to_matrix(bitvector)
    new_state = []
    for i in range(len(mixer)):
        row = []
        for j in range(len(state[0])):
            entry = BitVector(hexstring='00')
            for k in range(len(state)):
                entry ^= mixer[i][k].gf_multiply_modular(state[k][j], AES_modulus, 8)
            row.append(entry)
        new_state.append(row)
    return convert_matrix_to_bitvector(new_state)

def encrypt(bitvector, is_debug):
    # round 0
    bitvector = bitvector ^ roundkeys[0]
    if is_debug:
        print("AES output after round {}: {}".format(0, bitvector.get_bitvector_in_hex()))

    # round 1-9
    for i in range(1, 10):
        bitvector = mix_col(shift_rows(sub_bytes(bitvector, Sbox), False), Mixer) ^ roundkeys[i]
        if is_debug:
            print("AES output after round {}: {}".format(i, bitvector.get_bitvector_in_hex()))

    # round 10
    bitvector = shift_rows(sub_bytes(bitvector, Sbox), False) ^ roundkeys[10]
    if is_debug:
        print("AES output after round {}: {}".format(10, bitvector.get_bitvector_in_hex()))

    return bitvector

def decrypt(bitvector, is_debug):
    # round 0
    bitvector = bitvector ^ roundkeys[10]
    if is_debug:
        print("Decryption output after round {}: {}".format(0, bitvector.get_bitvector_in_hex()))

    # round 1-9
    for i in range(1, 10):
        bitvector = mix_col(sub_bytes(shift_rows(bitvector, True), InvSbox) ^ roundkeys[10 - i], InvMixer)
        if is_debug:
            print("Decryption output after round {}: {}".format(i, bitvector.get_bitvector_in_hex()))

    # round 10
    bitvector = sub_bytes(shift_rows(bitvector, True), InvSbox) ^ roundkeys[0]
    if is_debug:
        print("Decryption output after round {}: {}".format(10, bitvector.get_bitvector_in_hex()))

    return bitvector

is_debug = True

# key scheduling
input_key()
key_scheduling_time = time.time()
schedule_roundkeys()
key_scheduling_time = time.time() - key_scheduling_time
# print("Scheduled roundkeys:")
# for i in range(len(roundkeys)):
#     print("Roundkey {}: {}".format(i, roundkeys[i].get_bitvector_in_hex()))

# encryption
plaintext = input_plaintext()
ciphertext = BitVector(size=0)
encryption_time = time.time()
for i in range(len(plaintext) // 128):
    ciphertext += encrypt(plaintext[i * 128: i * 128 + 128], is_debug)
encryption_time = time.time() - encryption_time

# decryption
deciphered_output = BitVector(size=0)
decryption_time = time.time()
for i in range(len(ciphertext) // 128):
    deciphered_output += decrypt(ciphertext[i * 128: i * 128 + 128], is_debug)
decryption_time = time.time() - decryption_time        
if is_debug:
    print("Deciphered output: {}".format(deciphered_output.get_bitvector_in_ascii()))

# def generate_report(is_text_input):
#     # reporting initial roundkey
#     print("\nInitial roundkey [in ASCII]: {}".format(roundkeys[0].get_bitvector_in_ascii()))
#     print("Initial roundkey [in Hex]: {}\n".format(roundkeys[0].get_bitvector_in_hex()))

#     # reporting plaintext
#     if is_text_input:
#         print("Plaintext [in ASCII]: {}".format(AES_input.get_bitvector_in_ascii()[: len(AES_input.get_bitvector_in_ascii()) - pad_count_input]))
#         print("Plaintext [in Hex]: {}\n".format(AES_input.get_bitvector_in_hex()[: len(AES_input.get_bitvector_in_hex()) - 2*pad_count_input]))

#     # reporting ciphertext
#     if is_text_input:
#         print("Ciphertext [in ASCII]: {}".format(AES_output.get_bitvector_in_ascii()))
#         print("Ciphertext [in Hex]: {}\n".format(AES_output.get_bitvector_in_hex()))

#     # reporting deciphertext
#     if is_text_input:
#         print("Deciphered text [in ASCII]: {}".format(decipher_output.get_bitvector_in_ascii()[: len(decipher_output.get_bitvector_in_ascii()) - pad_count_input]))
#         print("Deciphered text [in Hex]: {}\n".format(decipher_output.get_bitvector_in_hex()[: len(decipher_output.get_bitvector_in_hex()) - 2*pad_count_input]))

#     # reporting execution time
#     print("<Execution time>")
#     print("Key scheduling time: {} seconds".format(key_scheduling_time))
#     print("Encryption time: {} seconds".format(encryption_time))
#     print("Decryption time: {} seconds".format(decryption_time))