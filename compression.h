/*******************************************************************************
 * Title: compression.h
 * Author: Rentaro Beattie
 * Description:
 * This file contains the compression functionality implementation
 * handles: compression and decompression
 * called by: interface.h
 *******************************************************************************/

#ifndef COMPRESSION_H_
#define COMPRESSION_H_

#include "data.h"
#include <cstdio>
#include <stdlib.h>
#include <string.h>

#define ERROR -1
#define SUCCESS 0

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
 * compress
 * uses LZW compression to compress the patients list and scans list
 * Input:
 *  -> char* filename     => string for filename of output ".db" file
 *                           eg. "sample" -> outputs "sample.db" files
 *  -> char* uncompressed => the input buffer to compress
 * Output:
 *  <- int                => error code for success (0 : success, 1 : fail)
 *******************************************************************************/
static int compress(char *filename, char *uncompressed);

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
 * -> char* filename     => string for filename of input ".db" file; eg.
 * "sample" -> reads "sample.db" file
 * -> patient_t **pat_db => pointer to a array of patient_t to be modified
 * -> scan_t **scan_db   => pointer to a array of scan_t to be modified
 * <- updated_t          => tupled return (make sure patient list and scans
 * lists have seperate length variables)
 *    -> int patient     => length of modified **pat_db
 *    -> int scan        => length of modified **scan_db
 *******************************************************************************/
typedef struct {
    int patient;
    int scan;
} updated_t;

updated_t decompress(char *filename, patient_t **pat_db, scan_t **scan_db);

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
    int ind = dict_find(dict, val);
    if (ind == ERROR)
        return SUCCESS;

    /* Dynamically resize dictionary at memory cap*/
    if (dict->len >= dict->cap) {
        dict->cap *= 2;
        dict->pats = (char **)realloc(dict->pats, dict->cap * sizeof(char *));
    }

    /* Copy and append new key */
    dict->pats[dict->len] = strdup(val);
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
    for (i = 0; i < dict->cap; i++)
        if (dict->pats[i])
            free(dict->pats[i]);

    /* Free array then struct itself */
    free(dict->pats);
    free(dict);
}

static int compress(char *filename, char *uncompressed) {
    dict_t *dict = dict_new(256);

    /* appends the first 256 basic ascii characters */
    int i;
    for (i = 0; i < 256; i++)
        sprintf(dict->pats[i], "%c", i);
    dict->len = 256;

    return SUCCESS;
}

#endif // COMPRESSION_H_
