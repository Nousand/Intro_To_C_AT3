/*******************************************************************************
 * MANUAL
 * INSTALLATION
 * 0. For windows and macos ensure gcc and make are installed
 * 1. In the command line cd into the directory
 * 2. run "make build" or "make" to compile the file
 * 
 * USAGE
 * execution: run "make run" (for platform agnosticism)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#define ENCRYPT_IMPLEMENTATION
#include "data.h"
#include "interface.h"

#define MAX_PATIENTS 100
#define MAX_SCANS 100

/*******************************************************************************
 * Function prototypes
*******************************************************************************/
void PrintMenu(void);
void ClearInputBuffer(void);
int ReadMenuChoice(void);
int ReadPatientNumber(void);

/*******************************************************************************
 * Main
*******************************************************************************/
int main(void) {
    patient_t *patient_db;
    patient_t *loaded_db;
    scan_t *scan_db;
    scan_t *loaded_scan_db;
    int patient_amt = 0;
    int scan_amt = 0;
    int loaded_amt;
    int loaded_scan_amt;
    int choice;
    int patient_number;
    char dbFileName[] = "patients.db";

    patient_db = (patient_t *)calloc(MAX_PATIENTS, sizeof(patient_t));
    if (patient_db == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    scan_db = (scan_t *)calloc(MAX_SCANS, sizeof(scan_t));
    if (scan_db == NULL) {
        free(patient_db);
        printf("Memory allocation failed\n");
        return 1;
    }

    do {
        PrintMenu();
        choice = ReadMenuChoice();

        switch (choice) {
            case 1:
                AddPatientData(patient_db, &patient_amt);
                break;

            case 2:
                patient_number = ReadPatientNumber();
                if (patient_number >= 1 && patient_number <= patient_amt) {
                    RemovePatientDocuments(patient_db, scan_db, &scan_amt,
                                           patient_number - 1);
                }
                DeletePatientData(patient_db, &patient_amt, patient_number - 1);
                break;

            case 3:
                ViewPatientData(patient_db, patient_amt, scan_db, scan_amt);
                break;

            case 4:
                SavePatientData(dbFileName, patient_db, patient_amt, scan_db,
                                scan_amt);
                break;

            case 5:
                loaded_db = NULL;
                loaded_scan_db = NULL;
                loaded_scan_amt = 0;
                loaded_amt = LoadPatientData(dbFileName, &loaded_db,
                                             &loaded_scan_db, &loaded_scan_amt);

                if (loaded_amt >= 0) {
                    FreeScanData(scan_db, scan_amt);
                    free(patient_db);
                    free(scan_db);
                    patient_db = loaded_db;
                    scan_db = loaded_scan_db;
                    patient_amt = loaded_amt;
                    scan_amt = loaded_scan_amt;
                }
                break;

            case 6:
                patient_number = ReadPatientNumber();
                AttachAdditionalDocument(patient_db, patient_amt, scan_db,
                                         &scan_amt,
                                         patient_number - 1);
                break;

            case 7:
                printf("compressed data successful.\n");
                break;

            case 8:
                printf("Decompress data successful.\n");
                break;

            case 9:
                if (EncryptFileData() == 0) {
                    printf("Encrypt data successful.\n");
                }
                break;

            case 10:
                if (DecryptFileData() == 0) {
                    printf("Decrypt data successful.\n");
                }
                break;

            case 11:
                break;

            default:
                printf("Invalid choice.\n");
                break;
        }
    } while (choice != 11);

    FreeScanData(scan_db, scan_amt);
    free(scan_db);
    free(patient_db);
    return 0;
}

/*******************************************************************************
 * PrintMenu
 * prints the command line menu for the patient database.
*******************************************************************************/
void PrintMenu(void) {
    printf("\nPatient Management System \n"
           "1. Add patient\n"
           "2. Delete patient\n"
           "3. Display patient list\n"
           "4. Save patient data to the database\n"
           "5. Read patient data from the database\n"
           "6. Additional Documents\n"
           "7. Compress data\n"
           "8. Decompress data\n"
           "9. Encrypt data\n"
           "10. Decrypt data\n"
           "11. Exit the program\n");
}

/*******************************************************************************
 * ClearInputBuffer
 * removes extra characters from stdin after scanf.
*******************************************************************************/
void ClearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

/*******************************************************************************
 * ReadMenuChoice
 * reads and validates the menu choice.
*******************************************************************************/
int ReadMenuChoice(void) {
    int result;
    int choice;

    printf("Enter your choice>\n");
    result = scanf("%d", &choice);

    if (result == EOF || feof(stdin)) {
        return 11;
    }

    if (result != 1) {
        ClearInputBuffer();
        if (feof(stdin)) {
            return 11;
        }
        return -1;
    }

    ClearInputBuffer();
    return choice;
}

/*******************************************************************************
 * ReadPatientNumber
 * reads the displayed patient number for deletion.
*******************************************************************************/
int ReadPatientNumber(void) {
    int result;
    int patient_number;

    printf("Patient number: >");
    result = scanf("%d", &patient_number);

    if (result != 1) {
        ClearInputBuffer();
        return -1;
    }

    ClearInputBuffer();
    return patient_number;
}
