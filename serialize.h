/*******************************************************************************
 * Title: serialize.h
 * Author: Rentaro Beattie
 * Description:
 * This header converts human readable, structured data e.g scan_t patient_t
 * and converts it into a single serialized char buffer (string)
 * images are then stored locally as .rawimg files for easy deserialization
 *
 * USAGE:
 * this should be called before the compression and encryption stages to convert
 * data into format those functions can read (char *)
 *
 * read function prototypes for parameters necessary
 *
 * interface functions (functions to be called from outside):
 * -> serialize      => pass in arrays of partient_t and scan_t to receive a
 *  raw char buffer form of the data
 * -> deserialize    => deserialize a serialized buffer
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
int deserialize(char *src, patient_t *pat_db, size_t *pat_amt, scan_t *scan_db,
                size_t *scan_amt);

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
        if (rem == 0)
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

    if (dest_size - offset < 3)
        return ERROR;
    memcpy(dest + offset, "^^\n", 3);
    offset += 3;

    /* For patient scans, images are stored in scan.rawimg files locally */
    for (i = 0; i < scan_amt; i++) {
        scan_t *key = &scan_db[i];

        char filename[64];
        snprintf(filename, sizeof(filename), "scan_%zu.rawimg", i);
        FILE *f = fopen(filename, "wb");
        if (!f)
            return ERROR;
        fwrite(key->content, 1, key->len, f);
        fclose(f);

        /* after image is made, write the metadata */
        size_t rem = dest_size - offset;
        if (rem == 0)
            return ERROR;
        int n =
            snprintf(dest + offset, rem, "%d,%s,%zu\n",
                     (int)(key->patient - pat_db), filename, (size_t)key->len);
        if (n < 0 || (size_t)n >= rem)
            return ERROR;
        offset += (size_t)n;
    }

    return SUCCESS;
}

int deserialize(char *src, patient_t *pat_db, size_t *pat_amt, scan_t *scan_db,
                size_t *scan_amt) {
    size_t pcount = 0;
    size_t scount = 0;
    char *line = src;
    size_t scanmode = 0;

    while (*line != '\0') {
        char *newline = strchr(line, '\n');
        size_t linelen = newline ? (size_t)(newline - line) : strlen(line);

        /* Check for divider */
        if (linelen == 2 && memcmp(line, "^^", 2) == 0) {
            scanmode = 1;
            line = newline ? newline + 1 : line + linelen;
            continue;
        }

        char tmp[512];
        if (linelen >= sizeof(tmp))
            return ERROR;
        memcpy(tmp, line, linelen);
        tmp[linelen] = '\0';

        if (!scanmode) {
            /* firstname,surname,YYYY-MM-DD,gender */
            patient_t *pat = &pat_db[pcount];

            char *firstname = strtok(tmp, ",");
            char *surname = strtok(NULL, ",");
            char *dob_str = strtok(NULL, ",");
            char *gender = strtok(NULL, ",");

            if (!firstname || !surname || !dob_str || !gender)
                return ERROR;

            if (sscanf(dob_str, "%d-%d-%d", &pat->dob.year, &pat->dob.month,
                       &pat->dob.day) != 3)
                return ERROR;

            strncpy(pat->firstname, firstname, sizeof(pat->firstname) - 1);
            pat->firstname[sizeof(pat->firstname) - 1] = '\0';

            strncpy(pat->surname, surname, sizeof(pat->surname) - 1);
            pat->surname[sizeof(pat->surname) - 1] = '\0';

            strncpy(pat->gender, gender, sizeof(pat->gender) - 1);
            pat->gender[sizeof(pat->gender) - 1] = '\0';

            pcount++;
        } else {
            scan_t *scan = &scan_db[scount];

            char *idxstr = strtok(tmp, ",");
            char *filenamestr = strtok(NULL, ",");
            char *lenstr = strtok(NULL, ",");

            if (!idxstr || !filenamestr || !lenstr)
                return ERROR;

            scan->len = (size_t)atoi(lenstr);
            scan->patient = &pat_db[atoi(idxstr)];

            scan->content = (char *)malloc(scan->len);
            if (!scan->content)
                return ERROR;

            FILE *f = fopen(filenamestr, "rb");
            if (!f) {
                free(scan->content);
                return ERROR;
            }
            fread(scan->content, 1, scan->len, f);
            fclose(f);

            scount++;
        }

        line = newline ? newline + 1 : line + linelen;
        if (!newline)
            break;
    }

    *pat_amt = pcount;
    *scan_amt = scount;
    return SUCCESS;
}

#endif /* Serialize */
