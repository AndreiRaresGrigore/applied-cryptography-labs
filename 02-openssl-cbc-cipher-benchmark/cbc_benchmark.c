#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
const int key_size = 16;
const int iv_size = 16;
double sec() {
    struct timespec t;
    timespec_get(&t, TIME_UTC);
    return (t.tv_sec+t.tv_nsec) / 1000000000.0;
}
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, const EVP_CIPHER *cipher) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    if (!(ctx = EVP_CIPHER_CTX_new()))
        printf("ERRORE");
    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv))
        printf("ERRORE");
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        printf("ERRORE");
    ciphertext_len = len;
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        printf("ERRORE");
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext, const EVP_CIPHER *cipher) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    if (!(ctx = EVP_CIPHER_CTX_new()))
        printf("ERRORE");
    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv))
        printf("ERRORE");
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        printf("ERRORE");
    plaintext_len = len;
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        printf("ERRORE");
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}
int main() {
    char key[key_size], iv[iv_size];
    RAND_bytes(key, key_size);
    RAND_bytes(iv, iv_size);
    char *file[] = {"16byte.bin", "20KB.bin", "3MB.bin"};
    const EVP_CIPHER *ciphers[] = {EVP_aes_256_cbc(), EVP_camellia_256_cbc(), EVP_sm4_cbc()};
    for (int i = 0; i < 3; i++) {
        printf("%s\n\n", file[i]);
        FILE *f = fopen(file[i], "rb");
        fseek(f, 0, SEEK_END);
        int file_dim = ftell(f);
        char *plain = malloc(file_dim);
        fread(plain, 1, file_dim, f);
        char *encr = malloc(file_dim + EVP_MAX_BLOCK_LENGTH);
        char *decr = malloc(file_dim + EVP_MAX_BLOCK_LENGTH);
        for (int j = 0; j < 3; j++) {
            char* name = "AES";
            double t1 = sec();
            int enc_len = encrypt(plain, file_dim, key, iv, encr, ciphers[j]);
            double t2 = sec();
            int dec_len = decrypt(encr, enc_len, key, iv, decr, ciphers[j]);
            double t3 = sec();
            if (j==1){
                name = "CAMELLIA";
            }
            if (j==2){
                name = "SM4";
            }
            printf("%s\nENCRYPT->%f  DECRYPT->%f\n", name, (t2 - t1), (t3 - t2));
        }
        printf("\n\n");
    }
}
