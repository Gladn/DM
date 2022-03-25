#include <iostream>
#include <string>
#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"
#include <rc5.h>
//
typedef unsigned char WORD;       /* Should be 32-bit = 4 bytes
*/
#define w        32             /* word size in bits                 */
#define r        12             /* number of rounds                  */
#define b        16             /* number of bytes in key            */
#define c         4             /* number  words in key = ceil(8*b/w)*/
#define t        26             /* size of table S = 2*(r+1) words   */
WORD S[t];                      /* expanded key table                */
WORD P = 0xb7e15163, Q = 0x9e3779b9;  /* magic constants             */
/* Rotation operators. x must be unsigned, to get logical right shift*/
#define ROTL(x,y) (((x)<<(y&(w-1))) | ((x)>>(w-(y&(w-1)))))
#define ROTR(x,y) (((x)>>(y&(w-1))) | ((x)<<(w-(y&(w-1)))))

void RC5_ENCRYPT(WORD* pt, WORD* ct) /* 2 WORD input pt/output ct    */
{
    WORD i, A = pt[0] + S[0], B = pt[1] + S[1];
    for (i = 1; i <= r; i++)
    {
        A = ROTL(A ^ B, B) + S[2 * i];
        B = ROTL(B ^ A, A) + S[2 * i + 1];
    }
    ct[0] = A; ct[1] = B;
}

void RC5_DECRYPT(WORD* ct, WORD* pt) /* 2 WORD input ct/output pt    */
{
    WORD i, B = ct[1], A = ct[0];
    for (i = r; i > 0; i--)
    {
        B = ROTR(B - S[2 * i + 1], A) ^ A;
        A = ROTR(A - S[2 * i], B) ^ B;
    }
    pt[1] = B - S[1]; pt[0] = A - S[0];
}

void RC5_SETUP(unsigned char* K) /* secret input key K[0...b-1]      */
{
    WORD i, j, k, u = w / 8, A, B, L[c];
    /* Initialize L, then S, then mix key into S */
    for (i = b - 1, L[c - 1] = 0; i != -1; i--) L[i / u] = (L[i / u] << 8) + K[i];
    for (S[0] = P, i = 1; i < t; i++) S[i] = S[i - 1] + Q;
    for (A = B = i = j = k = 0; k < 3 * t; k++, i = (i + 1) % t, j = (j + 1) % c)   /* 3*t > 3*c */
    {
        A = S[i] = ROTL(S[i] + (A + B), 3);
        B = L[j] = ROTL(L[j] + (A + B), (A + B));
    }
}


int main(int argc, char* argv[])
{

    using namespace CryptoPP;

    AutoSeededRandomPool prng;

    SecByteBlock key(RC5::DEFAULT_KEYLENGTH);
    prng.GenerateBlock(key, key.size());

    byte iv[RC5::BLOCKSIZE];
    prng.GenerateBlock(iv, sizeof(iv));

    std::string plain = "1234567CBC Mode Test";
    std::string cipher, encoded, recovered;


    /*********************************\
    \*********************************/
    std::cout << "key: ";
    for (int i = 0; i < key.size(); i++) {
        printf("%.2X", key[i]);
    } std::cout << "\n";


    try
    {
        std::cout << "plain text: " << plain << std::endl;

        CBC_Mode< RC5 >::Encryption e;
        e.SetKeyWithIV(key, key.size(), iv);

        // The StreamTransformationFilter adds padding
        //  as required. ECB and CBC Mode must be padded
        //  to the block size of the cipher.
        StringSource(plain, true,
            new StreamTransformationFilter(e,
                new StringSink(cipher)
            ) // StreamTransformationFilter
        ); // StringSource
    }
    catch (const CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    /*********************************\
    \*********************************/

    // Pretty print
    StringSource(cipher, true,
        new HexEncoder(
            new StringSink(encoded)
        ) // HexEncoder
    ); // StringSource

    std::cout << "cipher text: " << encoded << std::endl;

    /*********************************\
    \*********************************/

    try
    {
        CBC_Mode< RC5 >::Decryption d;
        d.SetKeyWithIV(key, key.size(), iv);

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource s(cipher, true,
            new StreamTransformationFilter(d,
                new StringSink(recovered)
            ) // StreamTransformationFilter
        ); // StringSource

        std::cout << "recovered text: " << recovered << std::endl;
    }
    catch (const CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}