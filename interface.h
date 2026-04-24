#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "data.h"

/*******************************************************************************
 * Function prototypes for Interface.h
*******************************************************************************/

/*******************************************************************************
 * SavePatientData
 * saves patient data into a file after encryption and compression
 * -> char *filename    => string for filename of output ".db" file
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => number of patients in pat_db
 * <- int               => error code (0 : success, 1 : fail)
*******************************************************************************/
int SavePatientData(char *filename, patient_t *pat_db, int pat_amt);


/*******************************************************************************
 * LoadPatientData
 * loads patient data from a file and performs decompression and decryption
 * -> char *filename     => string for filename of input ".db" file
 * -> patient_t **pat_db => pointer to patient array to be modified
 * <- int                => number of patients loaded (-1 : fail)
*******************************************************************************/
int LoadPatientData(char *filename, patient_t **pat_db);


/*******************************************************************************
 * AddPatientData
 * adds a new patient into the patient database
 * -> patient_t *pat_db => array of patients
 * -> int *pat_amt      => pointer to number of patients
 * <- int               => error code (0 : success, 1 : fail)
*******************************************************************************/
int AddPatientData(patient_t *pat_db, int *pat_amt);


/*******************************************************************************
 * ViewPatientData
 * displays all patient records in the database
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => number of patients
 * <- void
*******************************************************************************/
void ViewPatientData(patient_t *pat_db, int pat_amt);


/*******************************************************************************
 * DeletePatientData
 * deletes a patient record from the database
 * -> patient_t *pat_db => array of patients
 * -> int *pat_amt      => pointer to number of patients
 * -> int index         => index of patient to delete
 * <- int               => error code (0 : success, 1 : fail)
*******************************************************************************/
int DeletePatientData(patient_t *pat_db, int *pat_amt, int index);


#endif // INTERFACE_H_