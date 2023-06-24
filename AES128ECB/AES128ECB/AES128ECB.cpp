﻿#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>
#include <direct.h>

#include "AES.h"

AES::AES(const AESKeyLength keyLength) {
    switch (keyLength) {
    case AESKeyLength::AES_128:
        this->Nk = 4;
        this->Nr = 10;
        break;
    case AESKeyLength::AES_192:
        this->Nk = 6;
        this->Nr = 12;
        break;
    case AESKeyLength::AES_256:
        this->Nk = 8;
        this->Nr = 14;
        break;
    }
}

unsigned char* AES::EncryptECB(const unsigned char in[], unsigned int inLen,
    const unsigned char key[]) {
    CheckLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];
    KeyExpansion(key, roundKeys);
    for (unsigned int i = 0; i < inLen; i += blockBytesLen) {
        EncryptBlock(in + i, out + i, roundKeys);
    }

    delete[] roundKeys;

    return out;
}

unsigned char* AES::DecryptECB(const unsigned char in[], unsigned int inLen,
    const unsigned char key[]) {
    CheckLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];
    KeyExpansion(key, roundKeys);
    for (unsigned int i = 0; i < inLen; i += blockBytesLen) {
        DecryptBlock(in + i, out + i, roundKeys);
    }

    delete[] roundKeys;

    return out;
}

unsigned char* AES::EncryptCBC(const unsigned char in[], unsigned int inLen,
    const unsigned char key[],
    const unsigned char* iv) {
    CheckLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char block[blockBytesLen];
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];
    KeyExpansion(key, roundKeys);
    memcpy(block, iv, blockBytesLen);
    for (unsigned int i = 0; i < inLen; i += blockBytesLen) {
        XorBlocks(block, in + i, block, blockBytesLen);
        EncryptBlock(block, out + i, roundKeys);
        memcpy(block, out + i, blockBytesLen);
    }

    delete[] roundKeys;

    return out;
}

unsigned char* AES::DecryptCBC(const unsigned char in[], unsigned int inLen,
    const unsigned char key[],
    const unsigned char* iv) {
    CheckLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char block[blockBytesLen];
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];
    KeyExpansion(key, roundKeys);
    memcpy(block, iv, blockBytesLen);
    for (unsigned int i = 0; i < inLen; i += blockBytesLen) {
        DecryptBlock(in + i, out + i, roundKeys);
        XorBlocks(block, out + i, out + i, blockBytesLen);
        memcpy(block, in + i, blockBytesLen);
    }

    delete[] roundKeys;

    return out;
}

unsigned char* AES::EncryptCFB(const unsigned char in[], unsigned int inLen,
    const unsigned char key[],
    const unsigned char* iv) {
    CheckLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char block[blockBytesLen];
    unsigned char encryptedBlock[blockBytesLen];
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];
    KeyExpansion(key, roundKeys);
    memcpy(block, iv, blockBytesLen);
    for (unsigned int i = 0; i < inLen; i += blockBytesLen) {
        EncryptBlock(block, encryptedBlock, roundKeys);
        XorBlocks(in + i, encryptedBlock, out + i, blockBytesLen);
        memcpy(block, out + i, blockBytesLen);
    }

    delete[] roundKeys;

    return out;
}

unsigned char* AES::DecryptCFB(const unsigned char in[], unsigned int inLen,
    const unsigned char key[],
    const unsigned char* iv) {
    CheckLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char block[blockBytesLen];
    unsigned char encryptedBlock[blockBytesLen];
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];
    KeyExpansion(key, roundKeys);
    memcpy(block, iv, blockBytesLen);
    for (unsigned int i = 0; i < inLen; i += blockBytesLen) {
        EncryptBlock(block, encryptedBlock, roundKeys);
        XorBlocks(in + i, encryptedBlock, out + i, blockBytesLen);
        memcpy(block, in + i, blockBytesLen);
    }

    delete[] roundKeys;

    return out;
}

void AES::CheckLength(unsigned int len) {
    if (len % blockBytesLen != 0) {
        throw std::length_error("Plaintext length must be divisible by " +
            std::to_string(blockBytesLen));
    }
}

void AES::EncryptBlock(const unsigned char in[], unsigned char out[],
    unsigned char* roundKeys) {
    unsigned char state[4][Nb];
    unsigned int i, j, round;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            state[i][j] = in[i + 4 * j];
        }
    }

    AddRoundKey(state, roundKeys);

    for (round = 1; round <= Nr - 1; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, roundKeys + round * 4 * Nb);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, roundKeys + Nr * 4 * Nb);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            out[i + 4 * j] = state[i][j];
        }
    }
}

void AES::DecryptBlock(const unsigned char in[], unsigned char out[],
    unsigned char* roundKeys) {
    unsigned char state[4][Nb];
    unsigned int i, j, round;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            state[i][j] = in[i + 4 * j];
        }
    }

    AddRoundKey(state, roundKeys + Nr * 4 * Nb);

    for (round = Nr - 1; round >= 1; round--) {
        InvSubBytes(state);
        InvShiftRows(state);
        AddRoundKey(state, roundKeys + round * 4 * Nb);
        InvMixColumns(state);
    }

    InvSubBytes(state);
    InvShiftRows(state);
    AddRoundKey(state, roundKeys);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            out[i + 4 * j] = state[i][j];
        }
    }
}

void AES::SubBytes(unsigned char state[4][Nb]) {
    unsigned int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            t = state[i][j];
            state[i][j] = sbox[t / 16][t % 16];
        }
    }
}

void AES::ShiftRow(unsigned char state[4][Nb], unsigned int i,
    unsigned int n)  // shift row i on n positions
{
    unsigned char tmp[Nb];
    for (unsigned int j = 0; j < Nb; j++) {
        tmp[j] = state[i][(j + n) % Nb];
    }
    memcpy(state[i], tmp, Nb * sizeof(unsigned char));
}

void AES::ShiftRows(unsigned char state[4][Nb]) {
    ShiftRow(state, 1, 1);
    ShiftRow(state, 2, 2);
    ShiftRow(state, 3, 3);
}

unsigned char AES::xtime(unsigned char b)  // multiply on x
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}

void AES::MixColumns(unsigned char state[4][Nb]) {
    unsigned char temp_state[4][Nb];

    for (size_t i = 0; i < 4; ++i) {
        memset(temp_state[i], 0, 4);
    }

    for (size_t i = 0; i < 4; ++i) {
        for (size_t k = 0; k < 4; ++k) {
            for (size_t j = 0; j < 4; ++j) {
                if (CMDS[i][k] == 1)
                    temp_state[i][j] ^= state[k][j];
                else
                    temp_state[i][j] ^= GF_MUL_TABLE[CMDS[i][k]][state[k][j]];
            }
        }
    }

    for (size_t i = 0; i < 4; ++i) {
        memcpy(state[i], temp_state[i], 4);
    }
}

void AES::AddRoundKey(unsigned char state[4][Nb], unsigned char* key) {
    unsigned int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            state[i][j] = state[i][j] ^ key[i + 4 * j];
        }
    }
}

void AES::SubWord(unsigned char* a) {
    int i;
    for (i = 0; i < 4; i++) {
        a[i] = sbox[a[i] / 16][a[i] % 16];
    }
}

void AES::RotWord(unsigned char* a) {
    unsigned char c = a[0];
    a[0] = a[1];
    a[1] = a[2];
    a[2] = a[3];
    a[3] = c;
}

void AES::XorWords(unsigned char* a, unsigned char* b, unsigned char* c) {
    int i;
    for (i = 0; i < 4; i++) {
        c[i] = a[i] ^ b[i];
    }
}

void AES::Rcon(unsigned char* a, unsigned int n) {
    unsigned int i;
    unsigned char c = 1;
    for (i = 0; i < n - 1; i++) {
        c = xtime(c);
    }

    a[0] = c;
    a[1] = a[2] = a[3] = 0;
}

void AES::KeyExpansion(const unsigned char key[], unsigned char w[]) {
    unsigned char temp[4];
    unsigned char rcon[4];

    unsigned int i = 0;
    while (i < 4 * Nk) {
        w[i] = key[i];
        i++;
    }

    i = 4 * Nk;
    while (i < 4 * Nb * (Nr + 1)) {
        temp[0] = w[i - 4 + 0];
        temp[1] = w[i - 4 + 1];
        temp[2] = w[i - 4 + 2];
        temp[3] = w[i - 4 + 3];

        if (i / 4 % Nk == 0) {
            RotWord(temp);
            SubWord(temp);
            Rcon(rcon, i / (Nk * 4));
            XorWords(temp, rcon, temp);
        }
        else if (Nk > 6 && i / 4 % Nk == 4) {
            SubWord(temp);
        }

        w[i + 0] = w[i - 4 * Nk] ^ temp[0];
        w[i + 1] = w[i + 1 - 4 * Nk] ^ temp[1];
        w[i + 2] = w[i + 2 - 4 * Nk] ^ temp[2];
        w[i + 3] = w[i + 3 - 4 * Nk] ^ temp[3];
        i += 4;
    }
}

void AES::InvSubBytes(unsigned char state[4][Nb]) {
    unsigned int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            t = state[i][j];
            state[i][j] = inv_sbox[t / 16][t % 16];
        }
    }
}

void AES::InvMixColumns(unsigned char state[4][Nb]) {
    unsigned char temp_state[4][Nb];

    for (size_t i = 0; i < 4; ++i) {
        memset(temp_state[i], 0, 4);
    }

    for (size_t i = 0; i < 4; ++i) {
        for (size_t k = 0; k < 4; ++k) {
            for (size_t j = 0; j < 4; ++j) {
                temp_state[i][j] ^= GF_MUL_TABLE[INV_CMDS[i][k]][state[k][j]];
            }
        }
    }

    for (size_t i = 0; i < 4; ++i) {
        memcpy(state[i], temp_state[i], 4);
    }
}

void AES::InvShiftRows(unsigned char state[4][Nb]) {
    ShiftRow(state, 1, Nb - 1);
    ShiftRow(state, 2, Nb - 2);
    ShiftRow(state, 3, Nb - 3);
}

void AES::XorBlocks(const unsigned char* a, const unsigned char* b,
    unsigned char* c, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        c[i] = a[i] ^ b[i];
    }
}

void AES::printHexArray(unsigned char a[], unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        printf("%02x ", a[i]);
    }
}

void AES::printHexVector(std::vector<unsigned char> a) {
    for (unsigned int i = 0; i < a.size(); i++) {
        printf("%02x ", a[i]);
    }
}

std::vector<unsigned char> AES::ArrayToVector(unsigned char* a,
    unsigned int len) {
    std::vector<unsigned char> v(a, a + len * sizeof(unsigned char));
    return v;
}

unsigned char* AES::VectorToArray(std::vector<unsigned char>& a) {
    return a.data();
}

std::vector<unsigned char> AES::EncryptECB(std::vector<unsigned char> in,
    std::vector<unsigned char> key) {
    unsigned char* out = EncryptECB(VectorToArray(in), (unsigned int)in.size(),
        VectorToArray(key));
    std::vector<unsigned char> v = ArrayToVector(out, in.size());
    delete[] out;
    return v;
}

std::vector<unsigned char> AES::DecryptECB(std::vector<unsigned char> in,
    std::vector<unsigned char> key) {
    unsigned char* out = DecryptECB(VectorToArray(in), (unsigned int)in.size(),
        VectorToArray(key));
    std::vector<unsigned char> v = ArrayToVector(out, (unsigned int)in.size());
    delete[] out;
    return v;
}

std::vector<unsigned char> AES::EncryptCBC(std::vector<unsigned char> in,
    std::vector<unsigned char> key,
    std::vector<unsigned char> iv) {
    unsigned char* out = EncryptCBC(VectorToArray(in), (unsigned int)in.size(),
        VectorToArray(key), VectorToArray(iv));
    std::vector<unsigned char> v = ArrayToVector(out, in.size());
    delete[] out;
    return v;
}

std::vector<unsigned char> AES::DecryptCBC(std::vector<unsigned char> in,
    std::vector<unsigned char> key,
    std::vector<unsigned char> iv) {
    unsigned char* out = DecryptCBC(VectorToArray(in), (unsigned int)in.size(),
        VectorToArray(key), VectorToArray(iv));
    std::vector<unsigned char> v = ArrayToVector(out, (unsigned int)in.size());
    delete[] out;
    return v;
}

std::vector<unsigned char> AES::EncryptCFB(std::vector<unsigned char> in,
    std::vector<unsigned char> key,
    std::vector<unsigned char> iv) {
    unsigned char* out = EncryptCFB(VectorToArray(in), (unsigned int)in.size(),
        VectorToArray(key), VectorToArray(iv));
    std::vector<unsigned char> v = ArrayToVector(out, in.size());
    delete[] out;
    return v;
}

std::vector<unsigned char> AES::DecryptCFB(std::vector<unsigned char> in,
    std::vector<unsigned char> key,
    std::vector<unsigned char> iv) {
    unsigned char* out = DecryptCFB(VectorToArray(in), (unsigned int)in.size(),
        VectorToArray(key), VectorToArray(iv));
    std::vector<unsigned char> v = ArrayToVector(out, (unsigned int)in.size());
    delete[] out;
    return v;
}

using namespace std;

void print_ascii(const unsigned char* s, int len)
{
    int i;
    for (i = 0; i < len; ++i) {
        printf("%c", s[i]);
    }
    printf("\n");
}

int main() {
    unsigned char key[16];
    ///srand(time(NULL));
    for (int i = 0; i < 16; ++i) {
        key[i] = rand() % 256;
    }
    unsigned char in[16] = { 0 };
    unsigned char key2[16] = { 0 };
    uint8_t my_IV[] = { 0x32,0x14,0x76,0x58 };
    //unsigned char* encrypted, * decrypted;
    //AES aes(AESKeyLength::AES_128);

    //encrypted = aes.EncryptECB(in, 16, key2);
    //cout << "Encrypted: ";
    //for (int i = 0; i < 16; ++i) {
    //    printf("%02x", encrypted[i]);
    //}
    //cout << endl;
    //for (int i = 0; i < 16; ++i) {
    //    printf("%c", encrypted[i]);
    //}
    //cout << endl;
    ////print_ascii((unsigned char*)encrypted, 16);

    //decrypted = aes.DecryptECB(encrypted, 16, key2);
    //cout << "Decrypted: ";
    //for (int i = 0; i < 16; ++i) {
    //    printf("%02x", decrypted[i]);
    //}
    //cout << endl;
    //
    //cout << endl;

    //delete[] encrypted;
    //delete[] decrypted;

    for (int j = 1; j <= 5000; j++)
    {
        char filename[50];
        _mkdir("AES-EXTRA10r");
        sprintf(filename, "AES-EXTRA10r/AES-EXTRA10r_%d.txt", j);
        FILE* fp = fopen(filename, "wb");
        srand(GetTickCount64());
        unsigned char key3[16];
        for (int i = 0; i < 16; i++) {           
            key3[i] = rand() % 256;
        }
        for (int i = 0; i < 16; ++i) {
            printf("%02x", key3[i]);
        }
        for (int i = 1; i <= 10036; i++) {
            uint16_t value = (uint16_t)i;
            unsigned char value_bytes[16] = { 0 };
            value_bytes[0] = (unsigned char)(value & 0xFF);
            value_bytes[1] = (unsigned char)(value >> 8);
            //unsigned char p_plaintext = i % 256;
            //result = Simon_Init(&my_cipher_simon, Simon_128_128, ECB, simon128_128_key2, NULL, NULL);
            //Simon_Encrypt(my_cipher_simon, value_bytes, ciphertext_buffer);
            unsigned char* encrypted, * decrypted;
            
            //cout << endl;
            AES aes(AESKeyLength::AES_128);
            for (int i = 0; i < 16; ++i) {
               // printf("%02x", value_bytes[i]);
            }
            //cout << endl;
            encrypted = aes.EncryptCBC(value_bytes, 16, key3, my_IV);
            //cout << "Encrypted: ";
            for (int i = 0; i < 16; ++i) {
                //printf("%02x", encrypted[i]);
            }
            //cout << endl;
            for (int i = 0; i < 15; i++) {
                //printf("%c", encrypted[i]);
                fprintf(fp, "%c", encrypted[i]);
            }
            delete[] encrypted;
            //cout << endl;

        }
        printf("File created %d\n", j);
        fclose(fp);
    }


    return 0;
}