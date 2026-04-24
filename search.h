#ifndef SEARCH_H_
#define SEARCH_H_

#include "data.h"

/*******************************************************************************
 * Function prototypes for search.h
*******************************************************************************/

/*******************************************************************************
 * SearchByName
 * searchs the patient database for a patient matching the given name
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => number of patients in pat_db
 * -> const char *name  => null-terminated string to search for (firstname or surname)
 * <- int               => index of the matching patient(-1 : not found)
*******************************************************************************/
int SearchByName(patient_t *pat_db, int pat_amt, const char *name);

/*******************************************************************************
 * SearchByDOB
 * searchs the patient database for a patient matching the given date of birth
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => number of patients in pat_db
 * -> date_t dob        => date of birth to search for
 * <- int               => index of the matching patient(-1 : not found)
*******************************************************************************/
int SearchByDOB(patient_t *pat_db, int pat_amt, date_t dob);

/*******************************************************************************
 * SortByName
 * sorts the patient database alphabetically by surname, then by firstname
 * -> patient_t *pat_db => array of patients to be sorted in place
 * -> int pat_amt       => number of patients in pat_db
 * <- void
*******************************************************************************/
void SortByName(patient_t *pat_db, int pat_amt);

/*******************************************************************************
 * SortByDOB
 * sorts the patient database in ascending chronological order by date of birth
 * -> patient_t *pat_db => array of patients to be sorted in place
 * -> int pat_amt       => number of patients in pat_db
 * <- void
*******************************************************************************/
void SortByDOB(patient_t *pat_db, int pat_amt);

/*******************************************************************************
 * FilterByGender
 * returns a subset of patients matching the given gender string
 * -> patient_t *pat_db    => array of patients to filter from
 * -> int pat_amt          => number of patients in pat_db
 * -> const char *gender   => null-terminated string to match gender
 * -> patient_t *result_db => output array to store matching patients
 * <- int                  => number of patients written to result_db (-1 : fail)
*******************************************************************************/
int FilterByGender(patient_t *pat_db, int pat_amt, const char *gender, patient_t *result_db);

#endif // SEARCH_H_
