#ifndef H_ENC_CRYPT__
#define H_ENC_CRYPT__

NC_LIBEXPORT(VOID) ncEncrypt_1(unsigned char* outbuf, int len, unsigned char* key, int klen);
NC_LIBEXPORT(VOID) ncDecrypt_1(unsigned char* outbuf, int len, unsigned char* key, int klen);

#endif