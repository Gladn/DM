#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <windows.h>
#include <direct.h>
#include "Simon.h"

void int_to_uint8(int num, uint8_t* p_uint8) {
    std::string str = std::to_string(num);
    size_t len = str.length();
    for (size_t i = 0; i < len; i++) {
        p_uint8[i] = str[i];
    }
}

int main(void) {

    

    
    uint8_t my_IV[] = { 0x32,0x14,0x76,0x58 };
    uint8_t my_counter[] = { 0x2F,0x3D,0x5C,0x7B };
    uint32_t result; 

   

    // Simon 128/128 Test
    // Key: 0f0e0d0c0b0a0908 0706050403020100 Plaintext: 6373656420737265 6c6c657661727420 Ciphertext: 49681b1e1e54fe3f 65aa832af84e0bbc
    printf("Test Simon 128/128\n");
    uint8_t simon128_128_key[] = { 0x00, 0x01, 0x02, 0x03,0x04, 0x05, 0x06,0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    uint8_t simon128_128_plain[] = { 0x20, 0x74, 0x72, 0x61, 0x76, 0x65, 0x6c, 0x6c, 0x65, 0x72, 0x73, 0x20, 0x64, 0x65, 0x73, 0x63 };
    uint8_t simon128_128_cipher[] = { 0xbc, 0x0b, 0x4e, 0xf8, 0x2a, 0x83, 0xaa, 0x65, 0x3f, 0xfe, 0x54, 0x1e, 0x1e, 0x1b, 0x68, 0x49 };

   
        
        uint8_t simon128_128_plain2[16];
        uint8_t simon128_128_plain3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        uint8_t simon128_128_key3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


        uint8_t plain[1];
        
        
        for (int i = 0; i < 16; i++) {
            simon128_128_plain2[i] = 0x00;
        }
        
        printf("\n");
        printf("text ");
        for (int i = 0; i < 16; i++) {
            printf("%02x", simon128_128_plain3[i]);
        }
        printf("\n");
        printf("text2 ");
        for (int i = 0; i < 1; i++) {
            plain[i] = 1;
            printf("%02x", plain[i]);
            printf("%d", plain[i]);
            printf("%c", plain[i]);
        }
        printf("\n");

        for (int j = 1; j <= 2000; j++)
        {     
            char filename[50];
            _mkdir("Simon32r");
            sprintf(filename, "Simon32r/Simon32r_%d.txt", j);
            FILE* fp = fopen(filename, "wb");
            Simon_Cipher my_cipher_simon = *(Simon_Cipher*)malloc(sizeof(Simon_Cipher));
            uint8_t ciphertext_buffer[16];
            uint8_t simon128_128_key2[16];
            srand(GetTickCount64());
            for (int i = 0; i < 16; i++) {
                simon128_128_key2[i] = rand() % 256;
            }

            for (int i = 1; i <= 2000; i++) {
                uint16_t value = (uint16_t)i;
                uint8_t value_bytes[16] = { 0 };
                value_bytes[0] = (uint8_t)(value & 0xFF);
                value_bytes[1] = (uint8_t)(value >> 8);
                //uint8_t p_plaintext = i % 256;
                result = Simon_Init(&my_cipher_simon, Simon_128_128, ECB, simon128_128_key2, my_IV, my_counter);
                Simon_Encrypt(my_cipher_simon, value_bytes, ciphertext_buffer);
                
                /*for (int i = 0; i < 16; i++) {
                    printf("%02x", simon128_128_key2[i]);
                }
                printf("\n");
                for (int i = 0; i < 16; i++) {
                    printf("%02x", value_bytes[i]);
                }
                printf("\n");

                for (int i = 0; i < 16; i++) {
                    printf("%02x", ciphertext_buffer[i]);
                }
                printf("\n");*/


                for (int i = 0; i < 15; i++) {
                    //printf("%c", ciphertext_buffer[i]);
                    fprintf(fp, "%c", ciphertext_buffer[i]);
                }
                //printf("\n");
            }
            printf("File created %d\n", j);
            fclose(fp);
        }       
        printf("END\n");
}