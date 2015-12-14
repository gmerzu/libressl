/* =====================================================================
 * Author: Ignat Korchagin <ignat.korchagin@gmail.com>
 * This file is distributed under the same license as OpenSSL
 ==================================================================== */

#include "dstu_params.h"
#include "../gost/gost_locl.h"
#include <string.h>
#include <openssl/evp.h>

unsigned char default_sbox[64] = {
	0xa9, 0xd6, 0xeb, 0x45, 0xf1, 0x3c, 0x70, 0x82,
	0x80, 0xc4, 0x96, 0x7b, 0x23, 0x1f, 0x5e, 0xad,
	0xf6, 0x58, 0xeb, 0xa4, 0xc0, 0x37, 0x29, 0x1d,
	0x38, 0xd9, 0x6b, 0xf0, 0x25, 0xca, 0x4e, 0x17,
	0xf8, 0xe9, 0x72, 0x0d, 0xc6, 0x15, 0xb4, 0x3a,
	0x28, 0x97, 0x5f, 0x0b, 0xc1, 0xde, 0xa3, 0x64,
	0x38, 0xb5, 0x64, 0xea, 0x2c, 0x17, 0x9f, 0xd0,
	0x12, 0x3e, 0x6d, 0xb8, 0xfa, 0xc5, 0x79, 0x04
};

static void unpack_sbox(const unsigned char *packed_sbox, gost_subst_block * unpacked_sbox)
{
    int i;
    for (i = 0; i < 8; i++) {
        unpacked_sbox->k1[2 * i] = 0x0f & (packed_sbox[i] >> 4);
        unpacked_sbox->k1[(2 * i) + 1] = 0x0f & packed_sbox[i];

        unpacked_sbox->k2[2 * i] = 0x0f & (packed_sbox[i + 8] >> 4);
        unpacked_sbox->k2[(2 * i) + 1] = 0x0f & packed_sbox[i + 8];

        unpacked_sbox->k3[2 * i] = 0x0f & (packed_sbox[i + 16] >> 4);
        unpacked_sbox->k3[(2 * i) + 1] = 0x0f & packed_sbox[i + 16];

        unpacked_sbox->k4[2 * i] = 0x0f & (packed_sbox[i + 24] >> 4);
        unpacked_sbox->k4[(2 * i) + 1] = 0x0f & packed_sbox[i + 24];

        unpacked_sbox->k5[2 * i] = 0x0f & (packed_sbox[i + 32] >> 4);
        unpacked_sbox->k5[(2 * i) + 1] = 0x0f & packed_sbox[i + 32];

        unpacked_sbox->k6[2 * i] = 0x0f & (packed_sbox[i + 40] >> 4);
        unpacked_sbox->k6[(2 * i) + 1] = 0x0f & packed_sbox[i + 40];

        unpacked_sbox->k7[2 * i] = 0x0f & (packed_sbox[i + 48] >> 4);
        unpacked_sbox->k7[(2 * i) + 1] = 0x0f & packed_sbox[i + 48];

        unpacked_sbox->k8[2 * i] = 0x0f & (packed_sbox[i + 56] >> 4);
        unpacked_sbox->k8[(2 * i) + 1] = 0x0f & packed_sbox[i + 56];
    }
}

static void pack_sbox(const gost_subst_block * unpacked_sbox, unsigned char *packed_sbox)
{
    int i;
    for (i = 0; i < 8; i++) {
        packed_sbox[i] =
            ((unpacked_sbox->
              k1[2 * i] << 4) & 0xf0) | (unpacked_sbox->k1[(2 * i) +
                                                           1] & 0x0f);
        packed_sbox[i + 8] =
            ((unpacked_sbox->
              k2[2 * i] << 4) & 0xf0) | (unpacked_sbox->k2[(2 * i) +
                                                           1] & 0x0f);
        packed_sbox[i + 16] =
            ((unpacked_sbox->
              k3[2 * i] << 4) & 0xf0) | (unpacked_sbox->k3[(2 * i) +
                                                           1] & 0x0f);
        packed_sbox[i + 24] =
            ((unpacked_sbox->
              k4[2 * i] << 4) & 0xf0) | (unpacked_sbox->k4[(2 * i) +
                                                           1] & 0x0f);
        packed_sbox[i + 32] =
            ((unpacked_sbox->
              k5[2 * i] << 4) & 0xf0) | (unpacked_sbox->k5[(2 * i) +
                                                           1] & 0x0f);
        packed_sbox[i + 40] =
            ((unpacked_sbox->
              k6[2 * i] << 4) & 0xf0) | (unpacked_sbox->k6[(2 * i) +
                                                           1] & 0x0f);
        packed_sbox[i + 48] =
            ((unpacked_sbox->
              k7[2 * i] << 4) & 0xf0) | (unpacked_sbox->k7[(2 * i) +
                                                           1] & 0x0f);
        packed_sbox[i + 56] =
            ((unpacked_sbox->
              k8[2 * i] << 4) & 0xf0) | (unpacked_sbox->k8[(2 * i) +
                                                           1] & 0x0f);

    }
}

void dstu_set_sbox(GOST2814789_KEY *key, const unsigned char sbox[64])
{
	gost_subst_block unpacked_sbox;
	unsigned int t;
	int i;

	if (!sbox)
	{
		/* set default */
		sbox = default_sbox;
	}

	unpack_sbox(sbox, &unpacked_sbox);

	for (i = 0; i < 256; i++) {
		t = (unsigned int)(unpacked_sbox.k8[i >> 4] <<4 | unpacked_sbox.k7 [i & 15]) << 24;
		key->k87[i] = (t << 11) | (t >> 21);
		t = (unsigned int)(unpacked_sbox.k6[i >> 4] <<4 | unpacked_sbox.k5 [i & 15]) << 16;
		key->k65[i] = (t << 11) | (t >> 21);
		t = (unsigned int)(unpacked_sbox.k4[i >> 4] <<4 | unpacked_sbox.k3 [i & 15]) << 8;
		key->k43[i] = (t << 11) | (t >> 21);
		t = (unsigned int)(unpacked_sbox.k2[i >> 4] <<4 | unpacked_sbox.k1 [i & 15]) << 0;
		key->k21[i] = (t << 11) | (t >> 21);
	}
}

void dstu_get_sbox(const GOST2814789_KEY * ctx, unsigned char sbox[64])
{
	gost_subst_block unpacked_sbox;
    int i;

    for (i = 0; i < 256; i++) {
    	unpacked_sbox.k8[i >> 4] = (ctx->k87[i] >> 28) & 0xf;
    	unpacked_sbox.k7[i & 15] = (ctx->k87[i] >> 24) & 0xf;

    	unpacked_sbox.k6[i >> 4] = (ctx->k65[i] >> 20) & 0xf;
    	unpacked_sbox.k5[i & 15] = (ctx->k65[i] >> 16) & 0xf;

    	unpacked_sbox.k4[i >> 4] = (ctx->k43[i] >> 12) & 0xf;
    	unpacked_sbox.k3[i & 15] = (ctx->k43[i] >> 8) & 0xf;

    	unpacked_sbox.k2[i >> 4] = (ctx->k21[i] >> 4) & 0xf;
    	unpacked_sbox.k1[i & 15] = ctx->k21[i] & 0xf;
    }

    pack_sbox(&unpacked_sbox, sbox);
}

int bn_encode(const BIGNUM *bn, unsigned char *buffer, int length)
{
    int bn_size = BN_num_bytes(bn);
    if (length < bn_size)
        return 0;

    memset(buffer, 0, length - bn_size);
    return BN_bn2bin(bn, buffer + (length - bn_size));
}

static unsigned char data163[] = {
     /*A*/ 0x01,

     /*B*/ 0x05, 0xFF, 0x61, 0x08, 0x46, 0x2A, 0x2D, 0xC8, 0x21, 0x0A, 0xB4,
    0x03,
    0x92, 0x5E, 0x63, 0x8A, 0x19, 0xC1, 0x45, 0x5D, 0x21,

     /*N*/ 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0xBE,
    0xC1, 0x2B, 0xE2, 0x26, 0x2D, 0x39, 0xBC, 0xF1, 0x4D,

    /*
     * Px
     */
    0x02, 0xE2, 0xF8, 0x5F, 0x5D, 0xD7, 0x4C, 0xE9, 0x83, 0xA5, 0xC4, 0x23,
    0x72, 0x29, 0xDA, 0xF8, 0xA3, 0xF3, 0x58, 0x23, 0xBE,

    /*
     * Py
     */
    0x03, 0x82, 0x6F, 0x00, 0x8A, 0x8C, 0x51, 0xD7, 0xB9, 0x52, 0x84, 0xD9,
    0xD0, 0x3F, 0xF0, 0xE0, 0x0C, 0xE2, 0xCD, 0x72, 0x3A
};

static unsigned char data167[] = {
     /*A*/ 0x01,

     /*B*/ 0x6E, 0xE3, 0xCE, 0xEB, 0x23, 0x08, 0x11, 0x75, 0x9F, 0x20, 0x51,
    0x8A,
    0x09, 0x30, 0xF1, 0xA4, 0x31, 0x5A, 0x82, 0x7D, 0xAC,

     /*N*/ 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xB1,
    0x2E, 0xBC, 0xC7, 0xD7, 0xF2, 0x9F, 0xF7, 0x70, 0x1F,

    /*
     * Px
     */
    0x7A, 0x1F, 0x66, 0x53, 0x78, 0x6A, 0x68, 0x19, 0x28, 0x03, 0x91, 0x0A,
    0x3D, 0x30, 0xB2, 0xA2, 0x01, 0x8B, 0x21, 0xCD, 0x54,

    /*
     * Py
     */
    0x5F, 0x49, 0xEB, 0x26, 0x78, 0x1C, 0x0E, 0xC6, 0xB8, 0x90, 0x91, 0x56,
    0xD9, 0x8E, 0xD4, 0x35, 0xE4, 0x5F, 0xD5, 0x99, 0x18
};

static unsigned char data173[] = {
     /*A*/ 0x00,

     /*B*/ 0x10, 0x85, 0x76, 0xC8, 0x04, 0x99, 0xDB, 0x2F, 0xC1, 0x6E, 0xDD,
    0xF6,
    0x85, 0x3B, 0xBB, 0x27, 0x8F, 0x6B, 0x6F, 0xB4, 0x37, 0xD9,

     /*N*/ 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18,
    0x9B, 0x4E, 0x67, 0x60, 0x6E, 0x38, 0x25, 0xBB, 0x28, 0x31,

    /*
     * Px
     */
    0x04, 0xD4, 0x1A, 0x61, 0x9B, 0xCC, 0x6E, 0xAD, 0xF0, 0x44, 0x8F, 0xA2,
    0x2F, 0xAD, 0x56, 0x7A, 0x91, 0x81, 0xD3, 0x73, 0x89, 0xCA,

    /*
     * Py
     */
    0x10, 0xB5, 0x1C, 0xC1, 0x28, 0x49, 0xB2, 0x34, 0xC7, 0x5E, 0x6D, 0xD2,
    0x02, 0x8B, 0xF7, 0xFF, 0x5C, 0x1C, 0xE0, 0xD9, 0x91, 0xA1
};

static unsigned char data179[] = {
     /*A*/ 0x01,

     /*B*/ 0x04, 0xA6, 0xE0, 0x85, 0x65, 0x26, 0x43, 0x6F, 0x2F, 0x88, 0xDD,
    0x07,
    0xA3, 0x41, 0xE3, 0x2D, 0x04, 0x18, 0x45, 0x72, 0xBE, 0xB7, 0x10,

     /*N*/ 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF,
    0xB9, 0x81, 0x96, 0x04, 0x35, 0xFE, 0x5A, 0xB6, 0x42, 0x36, 0xEF,

    /*
     * Px
     */
    0x06, 0xBA, 0x06, 0xFE, 0x51, 0x46, 0x4B, 0x2B, 0xD2, 0x6D, 0xC5, 0x7F,
    0x48, 0x81, 0x9B, 0xA9, 0x95, 0x46, 0x67, 0x02, 0x2C, 0x7D, 0x03,

    /*
     * Py
     */
    0x02, 0x5F, 0xBC, 0x36, 0x35, 0x82, 0xDC, 0xEC, 0x06, 0x50, 0x80, 0xCA,
    0x82, 0x87, 0xAA, 0xFF, 0x09, 0x78, 0x8A, 0x66, 0xDC, 0x3A, 0x9E
};

static unsigned char data191[] = {
     /*A*/ 0x01,

     /*B*/ 0x7B, 0xC8, 0x6E, 0x21, 0x02, 0x90, 0x2E, 0xC4, 0xD5, 0x89, 0x0E,
    0x8B,
    0x6B, 0x49, 0x81, 0xff, 0x27, 0xE0, 0x48, 0x27, 0x50, 0xFE, 0xFC, 0x03,

     /*N*/ 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,
    0x69, 0xA7, 0x79, 0xCA, 0xC1, 0xDA, 0xBC, 0x67, 0x88, 0xF7, 0x47, 0x4F,

    /*
     * Px
     */
    0x71, 0x41, 0x14, 0xB7, 0x62, 0xF2, 0xFF, 0x4A, 0x79, 0x12, 0xA6, 0xD2,
    0xAC, 0x58, 0xB9, 0xB5, 0xC2, 0xFC, 0xFE, 0x76, 0xDA, 0xEB, 0x71, 0x29,

    /*
     * Py
     */
    0x29, 0xC4, 0x1E, 0x56, 0x8B, 0x77, 0xC6, 0x17, 0xEF, 0xE5, 0x90, 0x2F,
    0x11, 0xDB, 0x96, 0xFA, 0x96, 0x13, 0xCD, 0x8D, 0x03, 0xDB, 0x08, 0xDA
};

static unsigned char data233[] = {
     /*A*/ 0x01,

     /*B*/ 0x00, 0x69, 0x73, 0xB1, 0x50, 0x95, 0x67, 0x55, 0x34, 0xC7, 0xCF,
    0x7E,
    0x64, 0xA2, 0x1B, 0xD5, 0x4E, 0xF5, 0xDD, 0x3B, 0x8A, 0x03, 0x26, 0xAA,
    0x93, 0x6E, 0xCE, 0x45, 0x4D, 0x2C,

     /*N*/ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,
    0x00, 0x00, 0x00, 0x13, 0xE9, 0x74, 0xE7, 0x2F, 0x8A, 0x69, 0x22, 0x03,
    0x1D, 0x26, 0x03, 0xCF, 0xE0, 0xD7,

    /*
     * Px
     */
    0x00, 0x3F, 0xCD, 0xA5, 0x26, 0xB6, 0xCD, 0xF8, 0x3B, 0xA1, 0x11, 0x8D,
    0xF3, 0x5B, 0x3C, 0x31, 0x76, 0x1D, 0x35, 0x45, 0xF3, 0x27, 0x28, 0xD0,
    0x03, 0xEE, 0xB2, 0x5E, 0xFE, 0x96,

    /*
     * Py
     */
    0x00, 0x9C, 0xA8, 0xB5, 0x7A, 0x93, 0x4C, 0x54, 0xDE, 0xED, 0xA9, 0xE5,
    0x4A, 0x7B, 0xBA, 0xD9, 0x5E, 0x3B, 0x2E, 0x91, 0xC5, 0x4D, 0x32, 0xBE,
    0x0B, 0x9D, 0xF9, 0x6D, 0x8D, 0x35
};

static unsigned char data257[] = {
     /*A*/ 0x00,

     /*B*/ 0x01, 0xCE, 0xF4, 0x94, 0x72, 0x01, 0x15, 0x65, 0x7E, 0x18, 0xF9,
    0x38,
    0xD7, 0xA7, 0x94, 0x23, 0x94, 0xFF, 0x94, 0x25, 0xC1, 0x45, 0x8C, 0x57,
    0x86, 0x1F, 0x9E, 0xEA, 0x6A, 0xDB, 0xE3, 0xBE, 0x10,

     /*N*/ 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x59, 0x21, 0x3A, 0xF1, 0x82, 0xE9,
    0x87, 0xD3, 0xE1, 0x77, 0x14, 0x90, 0x7D, 0x47, 0x0D,

    /*
     * Px
     */
    0x00, 0x2A, 0x29, 0xEF, 0x20, 0x7D, 0x0E, 0x9B, 0x6C, 0x55, 0xCD, 0x26,
    0x0B, 0x30, 0x6C, 0x7E, 0x00, 0x7A, 0xC4, 0x91, 0xCA, 0x1B, 0x10, 0xC6,
    0x23, 0x34, 0xA9, 0xE8, 0xDC, 0xD8, 0xD2, 0x0F, 0xB7,

    /*
     * Py
     */
    0x01, 0x06, 0x86, 0xD4, 0x1F, 0xF7, 0x44, 0xD4, 0x44, 0x9F, 0xCC, 0xF6,
    0xD8, 0xEE, 0xA0, 0x31, 0x02, 0xE6, 0x81, 0x2C, 0x93, 0xA9, 0xD6, 0x0B,
    0x97, 0x8B, 0x70, 0x2C, 0xF1, 0x56, 0xD8, 0x14, 0xEF
};

static unsigned char data307[] = {
     /*A*/ 0x01,

     /*B*/ 0x03, 0x93, 0xC7, 0xF7, 0xD5, 0x36, 0x66, 0xB5, 0x05, 0x4B, 0x5E,
    0x6C,
    0x6D, 0x3D, 0xE9, 0x4F, 0x42, 0x96, 0xC0, 0xC5, 0x99, 0xE2, 0xE2, 0xE2,
    0x41, 0x05, 0x0D, 0xF1, 0x8B, 0x60, 0x90, 0xBD, 0xC9, 0x01, 0x86, 0x90,
    0x49, 0x68, 0xBB,

     /*N*/ 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x79, 0xC2, 0xF3,
    0x82, 0x5D, 0xA7, 0x0D, 0x39, 0x0F, 0xBB, 0xA5, 0x88, 0xD4, 0x60, 0x40,
    0x22, 0xB7, 0xB7,

    /*
     * Px
     */
    0x02, 0x16, 0xEE, 0x8B, 0x18, 0x9D, 0x29, 0x1A, 0x02, 0x24, 0x98, 0x4C,
    0x1E, 0x92, 0xF1, 0xD1, 0x6B, 0xF7, 0x5C, 0xCD, 0x82, 0x5A, 0x08, 0x7A,
    0x23, 0x9B, 0x27, 0x6D, 0x31, 0x67, 0x74, 0x3C, 0x52, 0xC0, 0x2D, 0x6E,
    0x72, 0x32, 0xAA,

    /*
     * Py
     */
    0x05, 0xD9, 0x30, 0x6B, 0xAC, 0xD2, 0x2B, 0x7F, 0xAE, 0xB0, 0x9D, 0x2E,
    0x04, 0x9C, 0x6E, 0x28, 0x66, 0xC5, 0xD1, 0x67, 0x77, 0x62, 0xA8, 0xF2,
    0xF2, 0xDC, 0x9A, 0x11, 0xC7, 0xF7, 0xBE, 0x83, 0x40, 0xAB, 0x22, 0x37,
    0xC7, 0xF2, 0xA0
};

static unsigned char data367[] = {
     /*A*/ 0x01,

     /*B*/ 0x43, 0xFC, 0x8A, 0xD2, 0x42, 0xB0, 0xB7, 0xA6, 0xF3, 0xD1, 0x62,
    0x7A,
    0xD5, 0x65, 0x44, 0x47, 0x55, 0x6B, 0x47, 0xBF, 0x6A, 0xA4, 0xA6, 0x4B,
    0x0C, 0x2A, 0xFE, 0x42, 0xCA, 0xDA, 0xB8, 0xF9, 0x3D, 0x92, 0x39, 0x4C,
    0x79, 0xA7, 0x97, 0x55, 0x43, 0x7B, 0x56, 0x99, 0x51, 0x36,

     /*N*/ 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C,
    0x30, 0x0B, 0x75, 0xA3, 0xFA, 0x82, 0x4F, 0x22, 0x42, 0x8F, 0xD2, 0x8C,
    0xE8, 0x81, 0x22, 0x45, 0xEF, 0x44, 0x04, 0x9B, 0x2D, 0x49,

    /*
     * Px
     */
    0x32, 0x4A, 0x6E, 0xDD, 0xD5, 0x12, 0xF0, 0x8C, 0x49, 0xA9, 0x9A, 0xE0,
    0xD3, 0xF9, 0x61, 0x19, 0x7A, 0x76, 0x41, 0x3E, 0x7B, 0xE8, 0x1A, 0x40,
    0x0C, 0xA6, 0x81, 0xE0, 0x96, 0x39, 0xB5, 0xFE, 0x12, 0xE5, 0x9A, 0x10,
    0x9F, 0x78, 0xBF, 0x4A, 0x37, 0x35, 0x41, 0xB3, 0xB9, 0xA1,

    /*
     * Py
     */
    0x01, 0xAB, 0x59, 0x7A, 0x5B, 0x44, 0x77, 0xF5, 0x9E, 0x39, 0x53, 0x90,
    0x07, 0xC7, 0xF9, 0x77, 0xD1, 0xA5, 0x67, 0xB9, 0x2B, 0x04, 0x3A, 0x49,
    0xC6, 0xB6, 0x19, 0x84, 0xC3, 0xFE, 0x34, 0x81, 0xAA, 0xF4, 0x54, 0xCD,
    0x41, 0xBA, 0x1F, 0x05, 0x16, 0x26, 0x44, 0x2B, 0x3C, 0x10
};

static unsigned char data431[] = {
     /*A*/ 0x01,

     /*B*/ 0x03, 0xCE, 0x10, 0x49, 0x0F, 0x6A, 0x70, 0x8F, 0xC2, 0x6D, 0xFE,
    0x8C,
    0x3D, 0x27, 0xC4, 0xF9, 0x4E, 0x69, 0x01, 0x34, 0xD5, 0xBF, 0xF9, 0x88,
    0xD8, 0xD2, 0x8A, 0xAE, 0xAE, 0xDE, 0x97, 0x59, 0x36, 0xC6, 0x6B, 0xAC,
    0x53, 0x6B, 0x18, 0xAE, 0x2D, 0xC3, 0x12, 0xCA, 0x49, 0x31, 0x17, 0xDA,
    0xA4, 0x69, 0xC6, 0x40, 0xCA, 0xF3,

     /*N*/ 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xBA, 0x31, 0x75, 0x45, 0x80, 0x09, 0xA8, 0xC0, 0xA7,
    0x24, 0xF0, 0x2F, 0x81, 0xAA, 0x8A, 0x1F, 0xCB, 0xAF, 0x80, 0xD9, 0x0C,
    0x7A, 0x95, 0x11, 0x05, 0x04, 0xCF,

    /*
     * Px
     */
    0x1A, 0x62, 0xBA, 0x79, 0xD9, 0x81, 0x33, 0xA1, 0x6B, 0xBA, 0xE7, 0xED,
    0x9A, 0x8E, 0x03, 0xC3, 0x2E, 0x08, 0x24, 0xD5, 0x7A, 0xEF, 0x72, 0xF8,
    0x89, 0x86, 0x87, 0x4E, 0x5A, 0xAE, 0x49, 0xC2, 0x7B, 0xED, 0x49, 0xA2,
    0xA9, 0x50, 0x58, 0x06, 0x84, 0x26, 0xC2, 0x17, 0x1E, 0x99, 0xFD, 0x3B,
    0x43, 0xC5, 0x94, 0x7C, 0x85, 0x7D,

    /*
     * Py
     */
    0x70, 0xB5, 0xE1, 0xE1, 0x40, 0x31, 0xC1, 0xF7, 0x0B, 0xBE, 0xFE, 0x96,
    0xBD, 0xDE, 0x66, 0xF4, 0x51, 0x75, 0x4B, 0x4C, 0xA5, 0xF4, 0x8D, 0xA2,
    0x41, 0xF3, 0x31, 0xAA, 0x39, 0x6B, 0x8D, 0x18, 0x39, 0xA8, 0x55, 0xC1,
    0x76, 0x9B, 0x1E, 0xA1, 0x4B, 0xA5, 0x33, 0x08, 0xB5, 0xE2, 0x72, 0x37,
    0x24, 0xE0, 0x90, 0xE0, 0x2D, 0xB9
};

DSTU_NAMED_CURVE dstu_curves[] = {
    {
     NID_uacurve0,
     {
      163, 7, 6, 3, 0, -1}, data163},
    {
     NID_uacurve1,
     {
      167, 6, 0, -1, 0, 0}, data167},
    {
     NID_uacurve2,
     {
      173, 10, 2, 1, 0, -1}, data173},
    {
     NID_uacurve3,
     {
      179, 4, 2, 1, 0, -1}, data179},
    {
     NID_uacurve4,
     {
      191, 9, 0, -1, 0, 0}, data191},
    {
     NID_uacurve5,
     {
      233, 9, 4, 1, 0, -1}, data233},
    {
     NID_uacurve6,
     {
      257, 12, 0, -1, 0, 0}, data257},
    {
     NID_uacurve7,
     {
      307, 8, 4, 2, 0, -1}, data307},
    {
     NID_uacurve8,
     {
      367, 21, 0, -1, 0, 0}, data367},
    {
     NID_uacurve9,
     {
      431, 5, 3, 1, 0, -1}, data431}
};

int curve_nid_from_group(const EC_GROUP *group)
{
    int m = EC_GROUP_get_degree(group), i, nid = NID_undef;
    EC_GROUP *std_group = NULL;

    for (i = 0; i < (sizeof(dstu_curves) / sizeof(DSTU_NAMED_CURVE)); i++) {
        if (m == dstu_curves[i].poly[0])
            break;
    }

    if (i < (sizeof(dstu_curves) / sizeof(DSTU_NAMED_CURVE))) {
        std_group = group_from_named_curve(i);

        if (std_group) {
            if (!EC_GROUP_cmp(group, std_group, NULL))
                nid = dstu_curves[i].nid;
        }
    }

    if (std_group)
        EC_GROUP_free(std_group);

    return nid;
}

EC_GROUP *group_from_named_curve(int curve_num)
{
    int bytesize = ((dstu_curves[curve_num].poly[0]) + 7) / 8;
    BIGNUM *p, *a, *b, *Px, *Py, *N;
    BN_CTX *ctx = NULL;
    EC_GROUP *group = NULL, *ret = NULL;
    EC_POINT *P = NULL;
    unsigned char *data = dstu_curves[curve_num].data;

    ctx = BN_CTX_new();
    if (!ctx)
        return NULL;

    BN_CTX_start(ctx);

    p = BN_CTX_get(ctx);
    a = BN_CTX_get(ctx);
    b = BN_CTX_get(ctx);
    N = BN_CTX_get(ctx);
    Px = BN_CTX_get(ctx);
    Py = BN_CTX_get(ctx);

    if (!Py)
        goto err;

    if (!BN_GF2m_arr2poly(dstu_curves[curve_num].poly, p))
        goto err;

    if (*data)
        BN_one(a);
    else
        BN_zero(a);

    data++;

    if (!BN_bin2bn(data, bytesize, b))
        goto err;
    data += bytesize;

    group = EC_GROUP_new_curve_GF2m(p, a, b, ctx);
    if (!group)
        goto err;

    if (!BN_bin2bn(data, bytesize, N))
        goto err;
    data += bytesize;

    P = EC_POINT_new(group);
    if (!P)
        goto err;

    if (!BN_bin2bn(data, bytesize, Px))
        goto err;
    data += bytesize;

    if (!BN_bin2bn(data, bytesize, Py))
        goto err;

    if (!EC_POINT_set_affine_coordinates_GF2m(group, P, Px, Py, ctx))
        goto err;

    if (!EC_GROUP_set_generator(group, P, N, BN_value_one()))
        goto err;

    ret = group;
    group = NULL;

 err:if (P)
        EC_POINT_free(P);
    if (group)
        EC_GROUP_free(group);

    if (ctx) {
        BN_CTX_end(ctx);
        BN_CTX_free(ctx);
    }

    return ret;
}

EC_GROUP *group_from_nid(int nid)
{
    int i;

    for (i = 0; i < (sizeof(dstu_curves) / sizeof(DSTU_NAMED_CURVE)); i++) {
        if (nid == dstu_curves[i].nid)
            return group_from_named_curve(i);
    }

    return NULL;
}

int dstu_generate_key(EC_KEY *key)
{
    const EC_GROUP *group = EC_KEY_get0_group(key);
    BIGNUM *order, *prk;
    BN_CTX *ctx = NULL;
    int ret = 0;

    if (!group)
        return 0;

    ctx = BN_CTX_new();
    if (!ctx)
        return 0;

    BN_CTX_start(ctx);

    order = BN_CTX_get(ctx);
    prk = BN_CTX_get(ctx);

    if (!prk)
        goto err;

    if (!EC_GROUP_get_order(group, order, NULL))
        goto err;

    do {
        if (!BN_rand_range(prk, order))
            goto err;
    }
    while (BN_is_zero(prk));

    if (!EC_KEY_set_private_key(key, prk))
        goto err;

    ret = dstu_add_public_key(key);

 err:if (ctx) {
        BN_CTX_end(ctx);
        BN_CTX_free(ctx);
    }
    return ret;
}

int dstu_add_public_key(EC_KEY *key)
{
    EC_POINT *pbk = NULL;
    const EC_GROUP *group = EC_KEY_get0_group(key);
    const BIGNUM *prk = EC_KEY_get0_private_key(key);
    int ret = 0;

    if (!group || !prk)
        return 0;

    pbk = EC_POINT_new(group);
    if (!pbk)
        return 0;

    if (!EC_POINT_mul(group, pbk, prk, NULL, NULL, NULL))
        goto err;

    if (!EC_POINT_invert(group, pbk, NULL))
        goto err;

    if (!EC_KEY_set_public_key(key, pbk))
        goto err;

    ret = 1;

 err:if (pbk)
        EC_POINT_free(pbk);
    return ret;
}

void reverse_bytes(void *mem, int size)
{
    unsigned char *bytes = mem;
    unsigned char tmp;
    int i;

    for (i = 0; i < (size / 2); i++) {
        tmp = bytes[i];
        bytes[i] = bytes[size - 1 - i];
        bytes[size - 1 - i] = tmp;
    }
}

void reverse_bytes_copy(void *dst, const void *src, int size)
{
    unsigned char *to = dst;
    const unsigned char *from = src;
    int i;

    for (i = 0; i < size; i++)
        to[i] = from[size - 1 - i];
}

unsigned char *copy_sbox(const unsigned char sbox[64])
{
    unsigned char *copy = malloc(sizeof(default_sbox));

    if (copy)
        memcpy(copy, sbox, sizeof(default_sbox));

    return copy;
}

int is_default_sbox(const unsigned char sbox[64])
{
    return !memcmp(default_sbox, sbox, sizeof(default_sbox));
}

