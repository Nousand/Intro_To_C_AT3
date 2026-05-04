/*******************************************************************************
 * Title: compression.h
 * Author:
 * Description:
 * This file contains the compression functionality implementation
 * handles: compression and decompression
 * called by: interface.h
 *
 * interface functions (functions to be called from outside):
 * -> compress      => when compressing raw data
 * -> decompress    => decompress a compressed buffer
 *  NOTE: both input and output strings
 *******************************************************************************/

#ifndef COMPRESSION_H_
#define COMPRESSION_H_

#include "data.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdlib.h>
#include <string.h>

#define ERROR -1
#define SUCCESS 0
/* Propagate errors upwards if function fails */
#define TRY(FUNC, CATCH)                                                       \
    do {                                                                       \
        int TRYRESULT = (FUNC);                                                \
        if (TRYRESULT < 0) {                                                   \
            CATCH;                                                             \
            return -1;                                                         \
        }                                                                      \
    } while (0)

#define MAXPATTERNSIZE 256
/*******************************************************************************
 * Function prototypes for Compression.h
 *******************************************************************************/

/*******************************************************************************
 * dict_t
 * a pseudo dictionary to hold the key(patterns) and value(code) pairs
 * <- char** pats => An array of strings, each a pattern for LZW
 * <- size_t len => size of array entry
 * <- size_t cap => maximum size of array entry
 *******************************************************************************/
typedef struct {
    char **pats;
    size_t len;
    size_t cap;
} dict_t;

/*******************************************************************************
 * dict_new
 * creates the pseudo dictionary
 * Input:
 *  -> int size => size of the array portion of the dictionary
 * Output:
 *  <- dict_t* => pointer to the newly initialised dictiionary
 *  TODO use a faster dictionary ie hashmap
 *******************************************************************************/

static dict_t *dict_new(int size);

/*******************************************************************************
 * dict_find
 * Linearly search over the array in reverses to find the matching pattern
 * Input:
 *  -> dict_t* dict => the dictionary
 *  -> const char* trg => target pattern to search the dict for
 * Output:
 *  <- int => index of pattern [0..->len] (doubles as code) or -1 for error
 *******************************************************************************/

static int dict_find(dict_t *dict, const char *trg);

/*******************************************************************************
 * dict_append
 * Dynamically appends an pattern to the dictionary, resizing when necessary
 * Input:
 *  -> dict_t* dict => the dictionary
 *  -> const char* trg => string to append to list
 * Output:
 *  <- int => 0 = Success, else Error
 *******************************************************************************/

static int dict_append(dict_t *dict, const char *val);

/*******************************************************************************
 * dict_free
 * free the dictionary and all patterns from memory
 * Input:
 *  -> dict_t* dict => the dictionary
 *******************************************************************************/

static void dict_free(dict_t *dict);

/*******************************************************************************
 * compress_raw
 * Uses LZW compression to compress an char buffer
 * Takes an uncompressed string to compress and its destination
 * Input:
 *  -> char* dest         => the destination buffer of the compressed data
 *  -> char* uncompressed => the input buffer (null-terminated) to compress
 * Output:
 *  <- int                => error code for success (0 : success, -1 : fail)
 *******************************************************************************/
static int compress(char *dest, char *uncompressed);

/*******************************************************************************
 * serialize
 * Serializes patient and scan data into a char buffer
 * Input:
 *  -> patient_t *pat_db => array of patients
 *  -> int pat_amt       => length of pat_db
 *  -> scan_t *scan_db   => array of scans
 *  -> int scan_amt      => length of scan_db
 * Output:
 *  <- char* => compressed char buffer
 *******************************************************************************/
char *serialize(patient_t *pat_db, int pat_amt, scan_t *scan_db, int scan_amt);

/*******************************************************************************
 * decompress
 * decompresses a compressed buffer into a readable string
 * Input:
 *  -> char* dest       => the destination buffer of the compressed data
 *  -> char* compressed => the input buffer (null-terminated) to decompress
 * Output:
 *  <- int              => error code for success (0 : success, -1 : fail)
 *******************************************************************************/
static int decompress(char *dest, char *compressed);

/*******************************************************************************
 * Function Implementations
 *******************************************************************************/

static dict_t *dict_new(int size) {
    /* Allocate struct */
    dict_t *dict;
    dict = (dict_t *)malloc(sizeof(dict_t));
    if (!dict)
        return NULL;

    /* Allocate array and length fields */
    dict->len = 0;
    dict->cap = size;
    dict->pats = (char **)calloc(size, sizeof(char *));

    return dict;
}

static int dict_append(dict_t *dict, const char *val) {
    /* Exits if key already exists */
    /*
    int ind = dict_find(dict, val);
    if (ind == ERROR)
        return SUCCESS;
        */

    /* Dynamically resize dictionary at memory cap*/
    if (dict->len >= dict->cap) {
        dict->cap *= 2;
        char **tmp = (char **)realloc(dict->pats, dict->cap * sizeof(char *));
        if (!tmp)
            return ERROR;
        dict->pats = tmp;

        /* Zero out memory so free is safe */
        memset(dict->pats + dict->len, 0,
               (dict->cap - dict->len) * sizeof(char *));
    }

    /* Copy and append new key */
    dict->pats[dict->len] = strdup(val);
    if (!dict->pats[dict->len])
        return ERROR;
    dict->len++;

    return SUCCESS;
}

static int dict_find(dict_t *dict, const char *trg) {

    /* Linearly searches keys for match */
    int i = 0;
    for (i = 0; i < dict->len; i++)
        if (!strcmp(dict->pats[i], trg))
            return i;

    return ERROR;
}

static void dict_free(dict_t *dict) {

    /* Free all strings if allocated */
    int i;
    for (i = 0; i < dict->len; i++)
        if (dict->pats[i])
            free(dict->pats[i]);

    /* Free array then struct itself */
    free(dict->pats);
    free(dict);
}

static int compress(char *dest, char *uncompressed) {
    dict_t *dict = dict_new(256);
    if (!dict)
        return ERROR;

    /* appends the first 256 basic ascii characters */
    size_t i;
    char safechr[2] = ""; /* Ensures chars are null-terminated before copy */
    for (i = 0; i < 256; i++) {
        safechr[0] = char(i);
        TRY(dict_append(dict, safechr), dict_free(dict));
    }

    dict->len = 256;

    size_t trglen = strlen(uncompressed);
    size_t *result = (size_t *)malloc(trglen * sizeof(size_t));
    if (!result) {
        dict_free(dict);
        return ERROR;
    }
    size_t reslen = 0;              /* length of results array             */
    char buff[MAXPATTERNSIZE] = ""; /* buffer for pattern string           */
    char next[MAXPATTERNSIZE];      /* tmp buffer to find matching keys    */
    char ch;                        /* singular character buffer           */

    for (i = 0; i < trglen; i++) {
        /* patterns expand until they no longer match any key in dictionary */
        ch = uncompressed[i];
        if (snprintf(next, sizeof(next), "%s%c", buff, ch) >=
            (int)sizeof(next)) {
            /* Prevent buffer overflow */
            free(result);
            dict_free(dict);
            return ERROR;
        }
        if (dict_find(dict, next) != ERROR)
            strcpy(buff, next);
        else {
            int code = dict_find(dict, buff);
            if (code == ERROR) {
                free(result);
                dict_free(dict);
                return ERROR;
            }

            /* append newly concatenated pattern to result */
            result[reslen++] = (size_t)code;
            if (dict_append(dict, next) == ERROR) {
                free(result);
                dict_free(dict);
                return ERROR;
            }

            /* pattern restarts from final char */
            safechr[0] = ch;
            strcpy(buff, safechr);
        }
    }

    /* match leftover pattern */
    if (strlen(buff) > 0) {
        int code = dict_find(dict, buff);
        if (code == ERROR) {
            free(result);
            dict_free(dict);
            return ERROR;
        }
        result[reslen++] = (size_t)code;
    }

    /* Convert an array of size_t to uint16_t (writes asif dest is uint16_t*) */
    uint16_t *out = (uint16_t *)dest;
    for (i = 0; i < reslen; i++) {
        out[i] = (uint16_t)result[i];
    }

    free(result);
    dict_free(dict);
    return SUCCESS;
}

#endif // COMPRESSION_H_
