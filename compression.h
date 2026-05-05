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
#include <stdio.h>
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

#define MAXPATTERNSIZE 65535 /* max size of a 16 bit unsigned integer */

/* Reimplementation of uint16_t from the <stdint.h> library
 * as it's usage is not permitted as per AT3 specs */
typedef unsigned short uint16_t;
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
 * allocates memory and creates the pseudo dictionary
 * Input:
 *  -> int size => size of the array portion of the dictionary
 * Output:
 *  <- dict_t* => pointer to the newly initialised dictiionary
 *  TODO use a faster lookup dictionary ie hashmap
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
 * Takes an uncompressed string to compress and its destination
 *
 * Uses LZW compression to compress an char buffer.
 * 1. LZW compression builds a dictionary of patterns as it encodes;
 * 2. Then it reads chars and expands until the pattern no longer matches any
 *    in the dictionary
 * 3. it then emits the index of the pattern and adds itself to dictionary
 * 4. restart from current char
 * LZW does not need the dictionary to decompress
 *
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
 *  -> char* compressed => the input buffer to decompress
 *  -> size_t inputlen  => the length of the compressed input buffer
 * Output:
 *  <- int              => error code for success (0 : success, -1 : fail)
 *******************************************************************************/
static int decompress(char *dest, char *compressed, size_t inputlen);

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

    /* results are stored in size_t to ensure size compatibility with the
     * dictionary array.
     * This will later be converted to uint16_t for better compression */
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

static int decompress(char *dest, char *compressed, size_t compressedlen) {

    /* Same initial logic as compress */
    dict_t *dict = dict_new(256);
    if (!dict)
        return ERROR;

    size_t i;
    char safechr[2] = "";
    for (i = 0; i < 256; i++) {
        safechr[0] = char(i);
        TRY(dict_append(dict, safechr), dict_free(dict));
    }

    /* cast input buffer as uint16 */
    uint16_t *inbuff = (uint16_t *)compressed;
    size_t inlen = compressedlen / sizeof(uint16_t);

    /* index to position in destination */
    size_t destind = 0;

    /* read write the first code (guaranteed to be one char) */
    int prevcode = inbuff[0];
    char *prevstr = dict->pats[prevcode]; /* string content of previous code */
    size_t prevlen = strlen(prevstr);
    memcpy(dest + destind, prevstr, prevlen);
    destind += prevlen;

    char tmppat[MAXPATTERNSIZE];

    /* Translate each code to their equivalent string form
     * note index from 1 */
    for (i = 1; i < inlen; i++) {
        int code = inbuff[i];
        char *curstr;

        if (code < (int)dict->len) {
            /* code already in dictionary */
            curstr = dict->pats[code];
        } else if (code == (int)dict->len) {
            /* if code not yet in dict =>
             * entry is prevstr + first char of prevstr */
            if (snprintf(tmppat, sizeof(tmppat), "%s%c", prevstr, prevstr[0]) >=
                (int)sizeof(tmppat)) {
                dict_free(dict);
                return ERROR;
            }
            curstr = tmppat;
        } else {
            /* unknown code */
            dict_free(dict);
            return ERROR;
        }

        /* Write string to output */
        prevlen = strlen(curstr);
        memcpy(dest + destind, curstr, prevlen);
        destind += prevlen;

        /* add prevstr + first char of currstr to dictionary */
        if (snprintf(tmppat, sizeof(tmppat), "%s%c", prevstr, curstr[0]) >=
            (int)sizeof(tmppat)) {
            dict_free(dict);
            return ERROR;
        }
        /* Allocation failure case */
        if (dict_append(dict, tmppat) == ERROR) {
            dict_free(dict);
            return ERROR;
        }

        if (code < (int)dict->len)
            prevstr = dict->pats[code];
        else
            prevstr = dict->pats[dict->len - 1];
        prevcode = code;
    }

    /* ensure output is readable as string */
    dest[destind] = '\0';
    dict_free(dict);
    return SUCCESS;
}

#endif // COMPRESSION_H_
