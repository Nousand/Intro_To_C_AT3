#ifndef COMPRESSION_H_
#define COMPRESSION_H_

#include "data.h"
/*******************************************************************************
 * Function prototypes for Compression.h
*******************************************************************************/

/*******************************************************************************
 * compress
 * uses LZF compression to compress the patients list and scans list
 * -> char* filename    => string for filename of output ".db" file; eg. "sample" -> outputs "sample.db" files
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => length of pat_db
 * -> scan_t *scan_db   => array of scans
 * -> int scan_amt      => length of scan_db
 * <- int               => error code for success (0 : success, 1 : fail)
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

#endif // COMPRESSION_H_
