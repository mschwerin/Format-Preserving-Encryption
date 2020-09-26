#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <fpe.h>

static void hex2chars(unsigned char hex[], unsigned char result[])
{
    size_t len = strlen((char *) hex);
    unsigned char temp[3];
    temp[2] = 0x00;

    size_t j = 0;
    for (size_t i = 0; i < len; i += 2) {
        temp[0] = hex[i];
        temp[1] = hex[i + 1];
        result[j] = (unsigned char)strtol((char *) temp, NULL, 16);
        ++j;
    }
}

static void map_chars(unsigned char str[], unsigned int result[])
{
    size_t len = strlen((char *) str);

    for (size_t i = 0; i < len; ++i)
        if (str[i] >= 'a')
            result[i] = str[i] - 'a' + 10;
        else
            result[i] = str[i] - '0';
}

static void inverse_map_chars(unsigned result[], unsigned char str[], int len)
{
    for (int i = 0; i < len; ++i)
        if (result[i] < 10)
            str[i] = result[i] + '0';
        else 
            str[i] = result[i] - 10 + 'a';

    str[len] = 0x00;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Usage: %s <key> <tweak> <radix> <plaintext>\n", argv[0]);
        return 0;
    }

    unsigned char k[100],
                  t[100],
                  result[100];
    size_t xlen = strlen(argv[4]),
        klen = strlen(argv[1]) / 2,
        tlen = strlen(argv[2]) / 2;
    unsigned int radix = (unsigned int) atoi(argv[3]);
    unsigned int x[100],
                 y[xlen];

    hex2chars((unsigned char *) argv[1], k);
    hex2chars((unsigned char *) argv[2], t);
    map_chars((unsigned char *) argv[4], x);

    for (size_t i = 0; i < xlen; ++i)
        assert(x[i] < radix);

    FPE_KEY ff1, ff3;

    printf("key:");
    for (size_t i = 0; i < klen; ++i)    printf(" %02x", k[i]);
    puts("");
    if (tlen)    printf("tweak:");
    for (size_t i = 0; i < tlen; ++i)    printf(" %02x", t[i]);
    if (tlen)    puts("");

    FPE_set_ff1_key(k, klen * 8, t, tlen, radix, &ff1);
    FPE_set_ff3_key(k, klen * 8, t, radix, &ff3);

    printf("after map: ");
    for (size_t i = 0; i < xlen; ++i)    printf(" %d", x[i]);
    printf("\n\n");

    printf("========== FF1 ==========\n");
    FPE_ff1_encrypt(x, y, xlen, &ff1, FPE_ENCRYPT);

    printf("ciphertext(numeral string):");
    for (size_t i = 0; i < xlen; ++i)    printf(" %d", y[i]);
    printf("\n");

    inverse_map_chars(y, result, xlen);
    printf("ciphertext: %s\n\n", result);

    memset(x, 0, sizeof(x));
    FPE_ff1_encrypt(y, x, xlen, &ff1, FPE_DECRYPT);

    printf("plaintext:");
    for (size_t i = 0; i < xlen; ++i)    printf(" %d", x[i]);
    printf("\n\n");

    printf("========== FF3 ==========\n");
    FPE_ff3_encrypt(x, y, xlen, &ff3, FPE_ENCRYPT);

    printf("ciphertext(numeral string):");
    for (size_t i = 0; i < xlen; ++i)    printf(" %d", y[i]);
    printf("\n");

    inverse_map_chars(y, result, xlen);
    printf("ciphertext: %s\n\n", result);

    memset(x, 0, sizeof(x));
    FPE_ff3_encrypt(y, x, xlen, &ff3, FPE_DECRYPT);

    printf("plaintext:");
    for (size_t i = 0; i < xlen; ++i)    printf(" %d", x[i]);
    printf("\n");

    FPE_unset_ff1_key(&ff1);
    FPE_unset_ff3_key(&ff3);

    return 0;
}

