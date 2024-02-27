//
//  main.c
//  Kecik
//
//  Created by Muhammad Reza Z'aba on 14/11/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "api.h"
#include "crypto_aead.h"
//#include "skinny_reference.h"

#include <inttypes.h>

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

//#include "lcd.h"
#include "avr-tick-counter.h"

#include <avr/io.h>
//#define F_CPU 8000000UL
#include <util/delay.h>
#define USART_BAUD 2400UL
#define USART_UBBR_VALUE ((F_CPU / (USART_BAUD <<  4)) - 1)
// bit
#define bit(n) (1 <<  n)
// Set bit
#define sbit(v, n) v |= (1 <<  n)
// Clear bit
#define cbit(v, n) v &= !(1 <<  n)
void usart_init() {
        UBRR0H = (uint8_t) (USART_UBBR_VALUE >> 8);
        UBRR0L = (uint8_t) USART_UBBR_VALUE;
        UCSR0A = 0x00;
        UCSR0B = 0x08;
        UCSR0C = 0x06;
        sbit(DDRD, 1);
}
void usart_send_byte(uint8_t byte) {
        while((UCSR0A & bit(5)) == 0);
        UDR0 = byte;
}
void usart_send_string(char* string) {
        int i;
        for(i = 0; string[i] != '\0'; i++) {
                usart_send_byte(string[i]);
        }
}



int main() {
    unsigned char c[48];
    unsigned long long clen = 0;

    /*
    const unsigned char m[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    */
    // test vector value
    const unsigned char m[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

    //unsigned char *m_dec;
    unsigned char m_dec[32];

    //unsigned long long mlen = BYTERATE+8-1;
    unsigned long long mlen = 32;
    unsigned long long mlen_dec;

    //const unsigned char ad[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    // test vector value
    const unsigned char ad[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

    //unsigned long long adlen = BYTERATE;
    unsigned long long adlen = 32;

    //const unsigned char npub[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    // test vector value
    const unsigned char npub[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

    //const unsigned char k[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    // test vector value
    const unsigned char k[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                   0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

    int i;

    unsigned char cycle[16];
    StartTickCounter();
    crypto_aead_encrypt(c, &clen, m, mlen, ad, adlen, NULL, npub, k);
    StopTickCounter();
    int x;
    sprintf(cycle, "cycles: %u", GetTicks());
    sbit(DDRD,1);
    for(x=0;x<16;x++)
       usart_send_byte(cycle[x]);
    

    printf("clen = %llu\n", clen);

    printf("\nENCRYPTION\n");

    printf("\nPlaintext  : ");
    for (i = 0; i < mlen; ++i) {
        if ((i>0) && ((i%16)==0)) printf(" ");
        printf("%02x", m[i]);
    }

    printf("\nKey        : ");
    for (i = 0; i < CRYPTO_KEYBYTES; ++i) {
        printf("%02x", k[i]);
    }

    printf("\nNonce      : ");
    for (i = 0; i < CRYPTO_NPUBBYTES; ++i) {
        printf("%02x", npub[i]);
    }

    printf("\nAD         : ");
    for (i = 0; i < adlen; ++i) {
        if ((i>0) && ((i%16)==0)) printf(" ");
        printf("%02x", ad[i]);
    }

    printf("\nCiphertext : ");
    for (i = 0; i < (clen - CRYPTO_ABYTES); ++i) {
        if ((i>0) && ((i%16)==0)) printf(" ");
        printf("%02x", c[i]);
    }

    printf("\nTag        : ");
    for (i = 0; i < CRYPTO_ABYTES; ++i) {
        printf("%02x", c[(clen - CRYPTO_ABYTES)+i]);
    }


    printf("\n\nDECRYPTION\n");

    printf("\nCiphertext : ");
    for (i = 0; i < (clen - CRYPTO_ABYTES); ++i) {
        if ((i>0) && ((i%16)==0)) printf(" ");
        printf("%02x", c[i]);
    }

    printf("\nKey        : ");
    for (i = 0; i < CRYPTO_KEYBYTES; ++i) {
        printf("%02x", k[i]);
    }

    printf("\nNonce      : ");
    for (i = 0; i <CRYPTO_NPUBBYTES; ++i) {
        printf("%02x", npub[i]);
    }

    // tamper
    //c[0] ^=1;
    if (crypto_aead_decrypt(m_dec, &mlen_dec, NULL, c, clen, ad, adlen, npub, k) == 0) {
        printf("\nPlaintext  : ");
        for (i = 0; i < mlen_dec; ++i) {
            if ((i>0) && ((i%16)==0)) printf(" ");
            printf("%02x", m_dec[i]);
        }

        printf("\nAD         : ");
        for (i = 0; i < adlen; ++i) {
            if ((i>0) && ((i%16)==0)) printf(" ");
            printf("%02x", ad[i]);
        }
        printf("\nDecryption SUCCESSFUL!\n");
    }
    else {
        printf("\nDecryption failed\n");
    }

    printf("\nOriginal Plaintext  : ");
    for (i = 0; i < mlen; ++i) {
        if ((i>0) && ((i%16)==0)) printf(" ");
        printf("%02x", m[i]);
    }

    printf("\n");

    return 0;
}
