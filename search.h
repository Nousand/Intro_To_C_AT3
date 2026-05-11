/*******************************************************************************
 * Title: search.h
 * Author: Marc Corrente
 * Student Number: 13901626
 * Description:
 * this file contains all of the search functions that will be used in the
 * program.
*******************************************************************************/
#ifndef SEARCH_H_
#define SEARCH_H_

#include "data.h"

/*******************************************************************************
 * Function prototypes for Search.h
*******************************************************************************/

int SearchByName(patient_t *pat_db, int pat_amt, const char *name);
int SearchByDOB(patient_t *pat_db, int pat_amt, date_t dob);
void SortByName(patient_t *pat_db, int pat_amt);
void SortByDOB(patient_t *pat_db, int pat_amt);
int FilterByGender(patient_t *pat_db, int pat_amt, const char *gender, patient_t *result_db);

#ifdef SEARCH_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * SearchByName
 * searches the patient database for a patient matching the given name.
 * compares the search string against both firstname and surname of each
 * patient using a case-sensitive linear search.
 * -> patient_t *pat_db  => array of patients
 * -> int pat_amt        => number of patients in pat_db
 * -> const char *name   => null-terminated string to search for (firstname or 
                            surname)
 * <- int                => index of the matching patient (-1 : not found)
*******************************************************************************/
int SearchByName(patient_t *pat_db, int pat_amt, const char *name){
    int i;

    if(pat_db == NULL || name == NULL || pat_amt <= 0){
        return -1;
    }

    for(i = 0; i < pat_amt; i++){
        if (strcmp(pat_db[i].firstname, name) == 0 ||
            strcmp(pat_db[i].surname,   name) == 0){
            return i;
        }
    }

    return -1;
}

/*******************************************************************************
 * SearchByDOB
 * searches the patient database for a patient matching the given date of birth.
 * compares day, month, and year fields of each patient using a linear search.
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => number of patients in pat_db
 * -> date_t dob        => date of birth to search for
 * <- int               => index of the matching patient (-1 : not found)
*******************************************************************************/
int SearchByDOB(patient_t *pat_db, int pat_amt, date_t dob){
    int i;

    if(pat_db == NULL || pat_amt <= 0){
        return -1;
    }

    for(i = 0; i < pat_amt; i++){
        if (pat_db[i].dob.day   == dob.day  && pat_db[i].dob.month == dob.month 
            && pat_db[i].dob.year  == dob.year){
            return i;
        }
    }

    return -1;
}

/*******************************************************************************
 * compareByName
 * internal helper for SortByName — used as the comparator for qsort.
 * compares two patients first by surname, then by firstname if surnames match.
 * -> const void *a => pointer to first patient_t
 * -> const void *b => pointer to second patient_t
 * <- int           => negative if a < b, 0 if equal, positive if a > b
*******************************************************************************/
static int compareByName(const void *a, const void *b){
    const patient_t *pa = (const patient_t *)a;
    const patient_t *pb = (const patient_t *)b;
    int surname_cmp = strcmp(pa->surname, pb->surname);

    if(surname_cmp != 0){
        return surname_cmp;
    }

    return strcmp(pa->firstname, pb->firstname);
}

/*******************************************************************************
 * SortByName
 * sorts the patient database alphabetically by surname, then by firstname,
 * using the C standard library qsort with compareByName as the comparator.
 * -> patient_t *pat_db => array of patients to be sorted in place
 * -> int pat_amt       => number of patients in pat_db
 * <- void
*******************************************************************************/
void SortByName(patient_t *pat_db, int pat_amt){
    if(pat_db == NULL || pat_amt <= 0){
        return;
    }

    qsort(pat_db, pat_amt, sizeof(patient_t), compareByName);
}

/*******************************************************************************
 * compareByDOB
 * internal helper for SortByDOB — used as the comparator for qsort.
 * compares two patients chronologically by year, then month, then day.
 * -> const void *a => pointer to first patient_t
 * -> const void *b => pointer to second patient_t
 * <- int           => negative if a < b, 0 if equal, positive if a > b
*******************************************************************************/
static int compareByDOB(const void *a, const void *b){
    const patient_t *pa = (const patient_t *)a;
    const patient_t *pb = (const patient_t *)b;

    if(pa->dob.year != pb->dob.year){
        return pa->dob.year - pb->dob.year;
    }
    if(pa->dob.month != pb->dob.month){
        return pa->dob.month - pb->dob.month;
    }
    return pa->dob.day - pb->dob.day;
}

/*******************************************************************************
 * SortByDOB
 * sorts the patient database in ascending chronological order by date of birth,
 * using the C standard library qsort with compareByDOB as the comparator.
 * -> patient_t *pat_db => array of patients to be sorted in place
 * -> int pat_amt       => number of patients in pat_db
 * <- void
*******************************************************************************/
void SortByDOB(patient_t *pat_db, int pat_amt){
    if(pat_db == NULL || pat_amt <= 0){
        return;
    }

    qsort(pat_db, pat_amt, sizeof(patient_t), compareByDOB);
}

/*******************************************************************************
 * FilterByGender
 * iterates through pat_db and copies any patient whose gender field matches
 * the given gender string into result_db. the caller is responsible for
 * ensuring result_db is large enough to hold all patients (pat_amt is safe).
 * -> patient_t *pat_db      => array of patients to filter from
 * -> int pat_amt            => number of patients in pat_db
 * -> const char *gender     => null-terminated gender string to match; eg. "male"
 * -> patient_t *result_db   => output array to store matching patients
 * <- int                    => number of patients written to result_db (-1 : fail)
*******************************************************************************/
int FilterByGender(patient_t *pat_db, int pat_amt, const char *gender, patient_t *result_db){
    if(pat_db == NULL || gender == NULL || result_db == NULL || pat_amt <= 0){
        return -1;
    }
    int count = 0;
    int i;
    for(i = 0; i < pat_amt; i++){
        if(strcmp(pat_db[i].gender, gender) == 0){
            result_db[count] = pat_db[i];
            count++;
        }
    }

    return count;
}

#endif /* SEARCH_IMPLEMENTATION */

#endif /* SEARCH_H_ */
