#ifndef INTERFACE_H
#define INTERFACE_H

#include "patient.h"

/*******************************************************************************
 * Function prototypes for interface.h
*******************************************************************************/


/*******************************************************************************
 * save patient data
 * uses LZF compression to compress the patients list and scans list
 * -> char* filename    => string for filename of output ".db" file; eg. "sample" -> outputs "sample.db" files
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => length of pat_db
 * -> scan_t *scan_db   => array of scans
 * -> int scan_amt      => length of scan_db
 * <- int               => error code for success (0 : success, 1 : fail)
*******************************************************************************/
int SavePatientData(patient_t *patients, int count, const char *filename);


/*
Function: LoadPatientData

Description:
Loads patient records from a file into memory.
The function will also handle decryption and decompression
before storing the data in the array.

Inputs:
  patients - array to store loaded patient data
  max_count - maximum capacity of the array
  filename - name of the file to load data from

Return:
  Number of records successfully loaded
  -1 if error occurs (e.g., file not found)

Post:
  Patient array is populated with decrypted and decompressed data.
*/
int LoadPatientData(patient_t *patients, int max_count, const char *filename);


/*
Function: AddPatientData

Description:
Adds a new patient record into the patient array.
User input is typically collected via CLI before calling this function.

Inputs:
  patients - array of patient_t structures
  count - pointer to current number of patients

Return:
  0 - success
  1 - failure (e.g., array is full)

Post:
  A new patient record is added and count is incremented.
*/
int AddPatientData(patient_t *patients, int *count);


/*
Function: ViewPatientData

Description:
Displays all patient records currently stored in memory
in a readable format via the command line.

Inputs:
  patients - array of patient_t structures
  count - number of patients stored

Return:
  None

Post:
  Patient data is printed to the terminal.
*/
void ViewPatientData(const patient_t *patients, int count);


/*
Function: DeletePatientData

Description:
Deletes a patient record from the array based on user input
(e.g., index or patient name).

Inputs:
  patients - array of patient_t structures
  count - pointer to number of patients
  index - position of patient to delete

Return:
  0 - success
  1 - failure (invalid index)

Post:
  Patient record is removed and array is shifted.
  Count is decremented.
*/
int DeletePatientData(patient_t *patients, int *count, int index);


#endif