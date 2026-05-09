/*******************************************************************************
 * Title: encrypt.c
 * Author: Marc Corrente
 * Student Number: 13901626
 * Description:
 * this file contains all of the encryption functions that will be used in the
 * program.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "encrypt.h"

#define KEY_LEN 32     /* length of the generated key in bytes */
#define MIN_PASS_LEN 4 /* minimum acceptable passphrase length */
/*******************************************************************************
 * Functions for encrypt.c
 * This file contains an XOR cipher to encrypt and decrypt a file as well as an
 * internal hashing helper that uses djb2 algorithm to turn a passphrase into 
 * an unsigned long.
*******************************************************************************/

/*******************************************************************************
 * djb2_hash
 * internal helper that hashes a passphrase string into an unsigned long
 * using the djb2 algorithm.
 * -> const char *str => null-terminated passphrase string
 * <- unsigned long   => resulting hash value
*******************************************************************************/
static unsigned long djb2_hash(const char *str){
    unsigned long hash = 5381;
    int c;
    while((c = (unsigned char)*str++)){
        hash = ((hash << 5) + hash) ^ c;
    }
    return hash;
}

/*******************************************************************************
 * GenerateKey
 * derives an encryption key from a user-supplied passphrase using djb hashing.
 * the hash is used as a seed to fill key_out with pseudorandom bytes, ensuring
 * the key length is not limited by the passphrase length
 * -> const char *passphrase => null-terminated string supplied by the user
 * -> char *key_out         => output buffer to store the generated key
 * -> int key_len           => desired length of the generated key in bytes
 * <- int                   => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int GenerateKey(const char *passphrase, char *key_out, int key_len){
    if(passphrase == NULL || key_out == NULL || key_len <= 0){
        return 1;
    }
    unsigned long hash = djb2_hash(passphrase);

    /* fill key_out by mixing the hash with the index, 
       each byte of the key is derived from the passphrase */
    int i;
    for(i = 0; i < key_len; i++){
        hash = ((hash << 5) + hash) ^ (unsigned long)i;
        key_out[i] = (char)(hash & 0xFF);
    }
    return 0;
}

/*******************************************************************************
 * ValidateKey
 * checks that a key is not-null and meets minimum length requirements.
 * -> const char *key => null-terminated string to validate
 * -> int min_len     => minimum acceptable key length in bytes
 * <- int             => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int ValidateKey(const char *key, int min_len){
    if(key == NULL || min_len <= 0){
        return 1;
    }
    if((int)strlen(key) < min_len){
        return 1;
    }
    return 0;
}

/*******************************************************************************
 * EncryptData
 * encrypts a byte buffer in place using a repeating XOR cipher.
 * each byte of data is XOR'd against the corresponding byte of the key.
 * cycling through the key if the data is longer than the key.
 * -> unsigned char *data => pointer to the raw data buffer to be encrypted
 * -> int len             => length of the data buffer in bytes
 * -> const char *key     => null-terminated string used as the encryption key
 * <- int                 => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int EncryptData(unsigned char *data, int len, const char *key){
    if(data == NULL || key == NULL || len <= 0){
        return 1;
    }

    int key_len = (int)strlen(key);
    if(key_len == 0){
        return 1;
    }

    int i;
    for(i = 0; i < len; i++){
        data[i] ^= (unsigned char)key[i % key_len];
    }
    return 0;
}

/*******************************************************************************
 * DecryptData
 * decrypts a previously encrypted byte buffer in place using a repeating XOR
 * cipher. XOR is symmetric, so this is identical to EncryptData. XOR'ing the 
 * encrypted data with the same key restores the original bytes.
 * -> unsigned char *data => pointer to the encrypted data buffer to be decrypted
 * -> int len             => length of the data buffer in bytes
 * -> const char *key     => null-terminated string used as the decryption key
 * <- int                 => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int DecryptData(unsigned char *data, int len, const char *key){
    /* XOR is symmetric: decryption is identical to encryption */
    return EncryptData(data, len, key);
}

/*******************************************************************************
 * EncryptWithPassphrase
 * Wrapper function - validates the passphrase, generates a key and encrypts the 
 * data buffer in one call. This is the recommended function to use from main or
 * any other module that needs to encrypt data.
 * -> unsigned char *data    => pointer to the raw data buffer to be encrypted
 * -> int len                => length of the data buffer in bytes
 * -> const char *passphrase => null-terminated passphrase string eg. "hello"
 * <- int                    => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int EncryptWithPassphrase(unsigned char *data, int len, const char *passphrase){
    if(ValidateKey(passphrase, MIN_PASS_LEN) != 0){
        printf("ERROR: passphrase must be at least %d characters.\n", MIN_PASS_LEN);
        return 1;
    }

    char key[KEY_LEN];
    if(GenerateKey(passphrase, key, KEY_LEN) != 0){
        printf("ERROR: key generattion failed.\n");
        return 1;
    }

    if(EncryptData(data, len, key) != 0){
        printf("ERROR: encryption failed.\n");
        return 1;
    }
    return 0;
}

/*******************************************************************************
 * DecryptWithPassphrase
 * Wrapper function - validates the passphrase, generates a key and decrypts the 
 * data buffer in one call. This is the recommended function to use from main or
 * any other module that needs to decrypt data.
 * -> unsigned char *data    => pointer to the encrypted data buffer to be decrypted
 * -> int len                => length of the data buffer in bytes
 * -> const char *passphrase => null-terminated passphrase string eg. "hello"
 * <- int                    => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int DecryptWithPassphrase(unsigned char *data, int len, const char *passphrase){
    if(ValidateKey(passphrase, MIN_PASS_LEN) != 0){
        printf("ERROR: passphrase must be at least %d characters.\n", MIN_PASS_LEN);
        return 1;
    }

    char key[KEY_LEN];
    if(GenerateKey(passphrase, key, KEY_LEN) != 0){
        printf("ERROR: key generattion failed.\n");
        return 1;
    }

    if(DecryptData(data, len, key) != 0){
        printf("ERROR: encryption failed.\n");
        return 1;
    }
    return 0;
}




