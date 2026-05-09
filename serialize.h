/*******************************************************************************
 * Title: serialize.h
 * Author:
 * Description:
 * This header converts human readable, structured data e.g scan_t patient_t
 * and converts it into a single serialized database file
 *
 * interface functions (functions to be called from outside):
 * -> serialize      => pass in arrays of partient_t and scan_t to receive a
 *  raw char buffer form of the data
 * -> deserialize    => deserialize a serialized buffer
 *
 * .db file format:
 *
 *******************************************************************************/
#include "compression.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#define ERROR -1
#define SUCCESS 0

/*******************************************************************************
 * Function prototypes for serialize.h
 *******************************************************************************/

/*******************************************************************************
 * serialize
 * Serializes patient and scan data into a char buffer
 * Input:
 *  -> char* dest        => char buffer
 *  -> size_t dest_size  => size of the destination buffer
 *  -> patient_t *pat_db => array of patients
 *  -> size_t pat_amt    => length of pat_db
 *  -> scan_t *scan_db   => array of scans
 *  -> size_t scan_amt   => length of scan_db
 * Output:
 *  <- int => length of written string
 *******************************************************************************/
int serialize(char *dest, size_t dest_size, patient_t *pat_db, size_t pat_amt,
              scan_t *scan_db, size_t scan_amt);

/*******************************************************************************
 * deserialize
 * converts a serialized .db file into
 * Input:
 *  -> char* src => compressed char buffer
 *  -> patient_t *pat_db => array of patients
 *  -> int pat_amt       => length of pat_db
 *  -> scan_t *scan_db   => array of scans
 *  -> int scan_amt      => length of scan_db
 * Output:
 *  <- int => 0 for success -1 for error
 *******************************************************************************/
int deserialize(char *dest, patient_t *pat_db, int *pat_amt, scan_t *scan_db,
                int *scan_amt);

/*******************************************************************************
 * Function Implementations
 *******************************************************************************/

int serialize(char *dest, size_t dest_size, patient_t *pat_db, size_t pat_amt,
              scan_t *scan_db, size_t scan_amt) {

    size_t offset = 0;

    size_t i;
    for (i = 0; i < pat_amt; i++) {
        patient_t *key = &pat_db[i];

        /* example .db file => john,doe,1999-12-30,male\n
         *                  => jane,doe,2000,06,20,female\n
         *                  => first,sur,1960,01,01,other
         * Catch buffer overflow aswell */
        size_t rem = dest_size - offset;
        if (rem <= 0)
            return ERROR;

        int n = snprintf(dest + offset, rem, "%s,%s,%d-%02d-%02d,%s\n",
                         key->firstname, key->surname, key->dob.year,
                         key->dob.month, key->dob.day, key->gender);

        if (n < 0)
            return ERROR;
        if ((size_t)n >= dest_size - offset)
            return ERROR;
        offset += (size_t)n;
    }

    return SUCCESS;
}

int deserialize(char *dest, patient_t *pat_db, int *pat_amt, scan_t *scan_db,
                int *scan_amt) {
    return SUCCESS;
}

#endif /* Serialize */
