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
#include <stdlib>

#define ERROR -1
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
    char** pats;
    size_t len;
    size_t cap;
}dict_t;

/*******************************************************************************
 * dict_find
 * creates the pseudo dictionary 
 * Input:
 *  -> int size => size of the array portion of the dictionary
 * Output:
 *  <- dict_t* => pointer to the newly initialised dictiionary
*******************************************************************************/

static dict_t* dict_new(int size);

/*******************************************************************************
 * dict_find
 * Linearly search over the array in reverses to find the matching pattern
 * Input:
 *  -> dict_t* dict => the dictionary
 *  -> const char* trg => target pattern to search the dict for
 * Output:
 *  <- int => index of pattern [0..->len] (doubles as code) or -1 for error
*******************************************************************************/

static int dict_find(dict_t* dict, const char* trg);

/*******************************************************************************
 * dict_append
 * Dynamically appends an pattern to the dictionary, resizing when necessary
 * Input:
 *  -> dict_t* dict => the dictionary
 *  -> const char* trg => string to append to list
 * Output:
 *  <- int => 0 = Success, else Error
*******************************************************************************/

static int dict_append(dict_t* dict, const char* val);

/*******************************************************************************
 * dict_free
 * free the dictionary and all patterns from memory
 * Input:
 *  -> dict_t* dict => the dictionary
*******************************************************************************/

static void dict_free(dict_t* dict);

/*******************************************************************************
 * compress
 * uses LZW compression to compress the patients list and scans list
 * Input:
 *  -> char* filename    => string for filename of output ".db" file
 *                          eg. "sample" -> outputs "sample.db" files
 *  -> patient_t *pat_db => array of patients
 *  -> int pat_amt       => length of pat_db
 *  -> scan_t *scan_db   => array of scans
 *  -> int scan_amt      => length of scan_db
 * Output:
*   <- int               => error code for success (0 :success, 1 or -1 :fail)
*******************************************************************************/
int compress(char *filename, patient_t* pat_db, int pat_amt, scan_t* scan_db, int scan_amt);

/*******************************************************************************
 * decompress
 * -> char* filename     => string for filename of input ".db" file; eg. "sample" -> reads "sample.db" file
 * -> patient_t **pat_db => pointer to a array of patient_t to be modified
 * -> scan_t **scan_db   => pointer to a array of scan_t to be modified
 * <- updated_t          => tupled return (make sure patient list and scans lists have seperate length variables)
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

static dict_t* dict_new(int size) {
    /* Allocate struct */
    dict_t* dict;
    dict = malloc(sizeof(dict_t));

    /* Allocate array and length fields */
    dict->len = size;
    dict->cap = size;
    dict->pats = malloc(size * sizeof(char*));

    /* Allocate strings within the array */
    for (int i = 0; i < size; i++)
        dict->pats[i] = malloc(MAXPATTERNSIZE);

    return dict;
}

int compress(char *filename, patient_t* pat_db, int pat_amt, scan_t* scan_db, int scan_amt) {
}

#endif // COMPRESSION_H_
