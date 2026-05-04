#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include "data.h"
/*******************************************************************************
 * Function prototypes for encrypt.h
*******************************************************************************/

/*******************************************************************************
 * EncryptData
 * encrypts a byte buffer in place using a key
 * -> unsigned char *data => pointer to the raw data buffer to be encrypted
 * -> int len             => length of the data buffer in bytes
 * -> const char *key     => null-terminated string used as the encryption key
 * <- int                 => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int EncryptData(unsigned char *data, int len, const char *key);

/*******************************************************************************
 * DecryptData
 * encrypts a previously encrypted byte buffer in place using a key.
 * -> unsigned char *data => pointer to the encrypted data buffer to be decrypted
 * -> int len             => length of the data buffer in bytes
 * -> const char *key     => null-terminated string used as the decryption key
 * <- int                 => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int DecryptData(unsigned char *data, int len, const char *key);

/*******************************************************************************
 * GenerateKey
 * derives an encryption key from a user-supplied passphrase.
 * -> const char *passphrase => null-terminated string supplied by the user
 * -> char *key_out         => output buffer to store the generated key
 * -> int key_len           => desired length of the generated key in bytes
 * <- int                   => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int GenerateKey(const char *passphrase, char *key_out, int key_len);

/*******************************************************************************
 * ValidateKey
 * checks that a key is not-null and meets minimum length requirements.
 * -> const char *key => null-terminated string to validate
 * -> int min_len     => minimum acceptable key length in bytes
 * <- int             => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int ValidateKey(const char *key, int min_len);

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
int EncryptWithPassphrase(unsigned char *data, int len, const char *passphrase);

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
int DecryptWithPassphrase(unsigned char *data, int len, const char *passphrase);

#endif // ENCRYPT_H_






