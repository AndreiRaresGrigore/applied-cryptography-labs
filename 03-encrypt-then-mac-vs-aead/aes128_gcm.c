#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/kdf.h>
#include <openssl/params.h>
#include <string.h>
const int master_key_size = 32;
const int key_size = 16;
const int iv_size = 12;
const int tag_size = 16;
double sec() {
    struct timespec t;
    timespec_get(&t, TIME_UTC);
    return (t.tv_sec+t.tv_nsec) / 1000000000.0;
}
// Encryption using GCM
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv,int iv_len, unsigned char *ciphertext, unsigned char *tag) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    // Create and initialise the context
    ctx = EVP_CIPHER_CTX_new();
    // Initialise the encryption operation.
    EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    // Set IV length
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL);
    // Initialise key and IV
    EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv);
    // Provide the message to be encrypted, and obtain the encrypted output.
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;
    // Finalise the encryption
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;
    // Get the tag
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag);
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}
// Decryption using GCM
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv,int iv_len, unsigned char *plaintext, unsigned char *tag) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    // Create and initialise the context
    ctx = EVP_CIPHER_CTX_new();
    // Initialise the decryption operation.
    EVP_DecryptInit_ex(ctx,EVP_aes_128_gcm(), NULL, NULL, NULL);
    // Set IV length.
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL);
    // Initialise key and IV
    EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv);
    // Provide the message to be decrypted, and obtain the plaintext output.
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;
    // Set expected tag value.
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag);
    // Finalise the decryption.
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;
    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}
int main() {
    unsigned char master_key[master_key_size], derived_key[key_size], derived_iv[iv_size], tag[tag_size];
    char *m_key = "master.key";
    FILE *key_open = fopen(m_key, "rb");
    fread(master_key, 1, master_key_size, key_open);
    // HKDF KEY DERIVATION
    EVP_KDF *kdf;
    EVP_KDF_CTX *kctx = NULL;
    unsigned char derived[key_size + iv_size]; // Key + IV
    OSSL_PARAM params[5], *p = params;
    // Find and allocate a context for the HKDF algorithm
    kdf = EVP_KDF_fetch(NULL, "hkdf", NULL);
    kctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf); // The kctx keeps a reference so this is safe
    // Build up the parameters for the derivation
    *p++ = OSSL_PARAM_construct_utf8_string("digest", "sha256", (size_t)7);
    *p++ = OSSL_PARAM_construct_octet_string("salt", "aessalt", (size_t)6);
    *p++ = OSSL_PARAM_construct_octet_string("key", master_key, (size_t)master_key_size);
    *p++ = OSSL_PARAM_construct_octet_string("info", "aeskeyiv", (size_t)8);
    *p = OSSL_PARAM_construct_end();
    if (EVP_KDF_CTX_set_params(kctx, params) <= 0) {
        printf("ERRORE");
    }
    /* Do the derivation */
    if (EVP_KDF_derive(kctx, derived, sizeof(derived), NULL) <= 0) {
        printf("ERRORE");
    }
    EVP_KDF_CTX_free(kctx);
    memcpy(derived_key, derived + 0, key_size);    // first 16 bytes = key
    memcpy(derived_iv, derived + key_size, iv_size); // Next 12 bytes = iv
    char *file = "10MB.bin";
    printf("%s\n", file);
    FILE *f = fopen(file, "rb");
    fseek(f, 0, SEEK_END);
    int file_dim = ftell(f);
    unsigned char *plain = malloc(file_dim);
    fread(plain, 1, file_dim, f);
    unsigned char *encr = malloc(file_dim + EVP_MAX_BLOCK_LENGTH);
    unsigned char *decr = malloc(file_dim + EVP_MAX_BLOCK_LENGTH);
    double t1 = sec();
    int enc_len = encrypt(plain, file_dim, derived_key, derived_iv,iv_size, encr, tag);
    double t2 = sec();
    int dec_len = decrypt(encr, enc_len, derived_key, derived_iv,iv_size, decr, tag);
    double t3 = sec();
    printf("\nAES_128_GCM\n\nENCRYPT->%f\nDECRYPT->%f\n", (t2 - t1), (t3 - t2));
    printf("\n\n");
}
