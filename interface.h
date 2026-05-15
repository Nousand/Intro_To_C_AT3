/*******************************************************************************
 * File: Interface.h
 * Author: Steven Fung
 *
 * Description:
 * This header file contains the function prototypes for the command line
 * interface module. The interface module allows the user to save, load, add,
 * view, and delete patient records. It connects user actions with the patient
 * database, compression module, and encryption module.
*******************************************************************************/

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "data.h"
#include "encrypt.h"
#include "serialize.h"

#define INTERFACE_MAX_PATIENTS 100
#define INTERFACE_MAX_SCANS 100
#define INTERFACE_INPUT_SIZE 256
#define INTERFACE_SERIAL_BUFFER_SIZE 65536
#define INTERFACE_PASSPHRASE "medical-data"
#define INTERFACE_DB_MAGIC "PATDB1"

/*******************************************************************************
 * Function prototypes for Interface.h
*******************************************************************************/

/*******************************************************************************
 * SavePatientData
 * saves patient data into a file after encryption and compression
 * -> char *filename    => string for filename of output ".db" file
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => number of patients in pat_db
 * -> scan_t *scan_db   => array of additional documents
 * -> int scan_amt      => number of additional documents
 * <- int               => error code (0 : success, 1 : fail)
*******************************************************************************/
int SavePatientData(char *filename, patient_t *pat_db, int pat_amt,
                    scan_t *scan_db, int scan_amt);


/*******************************************************************************
 * LoadPatientData
 * loads patient data from a file and performs decompression and decryption
 * -> char *filename     => string for filename of input ".db" file
 * -> patient_t **pat_db => pointer to patient array to be modified
 * -> scan_t **scan_db   => pointer to scan array to be modified
 * -> int *scan_amt      => pointer to number of scans loaded
 * <- int                => number of patients loaded (-1 : fail)
*******************************************************************************/
int LoadPatientData(char *filename, patient_t **pat_db, scan_t **scan_db,
                    int *scan_amt);


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
 * -> scan_t *scan_db   => array of attached documents
 * -> int scan_amt      => number of attached documents
 * <- void
*******************************************************************************/
void ViewPatientData(patient_t *pat_db, int pat_amt, scan_t *scan_db,
                     int scan_amt);


/*******************************************************************************
 * DeletePatientData
 * deletes a patient record from the database
 * -> patient_t *pat_db => array of patients
 * -> int *pat_amt      => pointer to number of patients
 * -> int index         => index of patient to delete
 * <- int               => error code (0 : success, 1 : fail)
*******************************************************************************/
int DeletePatientData(patient_t *pat_db, int *pat_amt, int index);

/*******************************************************************************
 * AttachAdditionalDocument
 * reads a local image/document file into scan_t.content for a patient
 * -> patient_t *pat_db => array of patients
 * -> int pat_amt       => number of patients
 * -> scan_t *scan_db   => array of additional documents
 * -> int *scan_amt     => pointer to number of additional documents
 * -> int index         => index of patient to update
 * <- int               => error code (0 : success, 1 : fail)
*******************************************************************************/
int AttachAdditionalDocument(patient_t *pat_db, int pat_amt, scan_t *scan_db,
                             int *scan_amt, int index);

/*******************************************************************************
 * RemovePatientDocuments
 * removes additional documents attached to a patient being deleted
*******************************************************************************/
void RemovePatientDocuments(patient_t *pat_db, scan_t *scan_db, int *scan_amt,
                            int patient_index);

/*******************************************************************************
 * FreeScanData
 * frees all attached document content
*******************************************************************************/
void FreeScanData(scan_t *scan_db, int scan_amt);

/*******************************************************************************
 * Helper function prototypes for Interface.h
*******************************************************************************/
static void InterfaceClearInputBuffer(void);
static void InterfaceReadLine(char str[], int size);
static void InterfaceCopyLimitedString(char destination[], const char source[],
                                       int size);
static int InterfaceReadInt(const char prompt[], int *value_p);
static int InterfaceIsLeapYear(int year);
static int InterfaceDaysInMonth(int month, int year);
static int InterfaceIsValidDate(date_t dob);
static void InterfaceFormatDate(char destination[], int size, date_t dob);

/*******************************************************************************
 * Helper function implementations
*******************************************************************************/
static void InterfaceClearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static void InterfaceReadLine(char str[], int size) {
    if (fgets(str, size, stdin) != NULL) {
        int length = (int)strlen(str);
        if (length > 0 && str[length - 1] == '\n') {
            str[length - 1] = '\0';
        } else {
            InterfaceClearInputBuffer();
        }
    } else {
        str[0] = '\0';
    }
}

static void InterfaceCopyLimitedString(char destination[], const char source[],
                                       int size) {
    int i;
    for (i = 0; i < size - 1 && source[i] != '\0'; i++) {
        destination[i] = source[i];
    }
    destination[i] = '\0';
}

static int InterfaceReadInt(const char prompt[], int *value_p) {
    int result;

    printf("%s", prompt);
    result = scanf("%d", value_p);

    if (result != 1) {
        InterfaceClearInputBuffer();
        return 1;
    }

    InterfaceClearInputBuffer();
    return 0;
}

static int InterfaceIsLeapYear(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

static int InterfaceDaysInMonth(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month < 1 || month > 12) {
        return 0;
    }

    if (month == 2 && InterfaceIsLeapYear(year)) {
        return 29;
    }

    return days[month - 1];
}

static int InterfaceIsValidDate(date_t dob) {
    if (dob.year < 1900 || dob.year > 2026) {
        return 0;
    }

    if (dob.month < 1 || dob.month > 12) {
        return 0;
    }

    return dob.day >= 1 &&
           dob.day <= InterfaceDaysInMonth(dob.month, dob.year);
}

static void InterfaceFormatDate(char destination[], int size, date_t dob) {
    snprintf(destination, (size_t)size, "%02d-%02d-%04d", dob.day, dob.month,
             dob.year);
}

/*******************************************************************************
 * SavePatientData
 * serializes, compresses, encrypts, and writes patient records to a file.
*******************************************************************************/
int SavePatientData(char *filename, patient_t *pat_db, int pat_amt,
                    scan_t *scan_db, int scan_amt) {
    FILE *fp;
    char serialized[INTERFACE_SERIAL_BUFFER_SIZE] = "";
    char *compressed;
    int serialized_len;
    int compressed_len;

    if (filename == NULL || pat_db == NULL || pat_amt < 0 ||
        scan_amt < 0) {
        printf("Save error\n");
        return 1;
    }

    serialized_len = serialize(serialized, sizeof(serialized), pat_db,
                               (size_t)pat_amt, scan_db, (size_t)scan_amt);
    if (serialized_len != SUCCESS) {
        printf("Save error\n");
        return 1;
    }

    serialized_len = (int)strlen(serialized);
    compressed_len = serialized_len * (int)sizeof(uint16_t);
    if (compressed_len <= 0) {
        compressed_len = (int)sizeof(uint16_t);
    }

    compressed = (char *)calloc((size_t)compressed_len, 1);
    if (compressed == NULL) {
        printf("Save error\n");
        return 1;
    }

    if (compress(compressed, serialized) != SUCCESS) {
        free(compressed);
        printf("Save error\n");
        return 1;
    }

    if (EncryptWithPassphrase((unsigned char *)compressed, compressed_len,
                              INTERFACE_PASSPHRASE) != 0) {
        free(compressed);
        printf("Save error\n");
        return 1;
    }

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        free(compressed);
        printf("Cannot open file.\n");
        return 1;
    }

    fprintf(fp, "%s\n%d\n", INTERFACE_DB_MAGIC, compressed_len);
    if (fwrite(compressed, 1, (size_t)compressed_len, fp) !=
        (size_t)compressed_len) {
        fclose(fp);
        free(compressed);
        printf("Save error\n");
        return 1;
    }

    fclose(fp);
    free(compressed);
    return 0;
}

/*******************************************************************************
 * LoadPatientData
 * reads, decrypts, decompresses, and deserializes patient records from a file.
*******************************************************************************/
int LoadPatientData(char *filename, patient_t **pat_db, scan_t **scan_db,
                    int *scan_amt) {
    FILE *fp;
    char magic[INTERFACE_INPUT_SIZE];
    char *compressed;
    char *serialized;
    int compressed_len;
    size_t pat_amt = 0;
    size_t loaded_scan_amt = 0;

    if (filename == NULL || pat_db == NULL || scan_db == NULL ||
        scan_amt == NULL) {
        printf("Read error\n");
        return -1;
    }

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Read error\n");
        return -1;
    }

    if (fgets(magic, sizeof(magic), fp) == NULL) {
        fclose(fp);
        printf("Read error\n");
        return -1;
    }
    magic[strcspn(magic, "\n")] = '\0';

    if (strcmp(magic, INTERFACE_DB_MAGIC) != 0 ||
        fscanf(fp, "%d", &compressed_len) != 1 || compressed_len <= 0 ||
        fgetc(fp) != '\n') {
        fclose(fp);
        printf("Read error\n");
        return -1;
    }

    compressed = (char *)malloc((size_t)compressed_len);
    serialized = (char *)calloc(INTERFACE_SERIAL_BUFFER_SIZE, 1);
    *pat_db = (patient_t *)calloc(INTERFACE_MAX_PATIENTS, sizeof(patient_t));
    *scan_db = (scan_t *)calloc(INTERFACE_MAX_SCANS, sizeof(scan_t));

    if (compressed == NULL || serialized == NULL || *pat_db == NULL ||
        *scan_db == NULL) {
        free(compressed);
        free(serialized);
        free(*pat_db);
        free(*scan_db);
        *pat_db = NULL;
        *scan_db = NULL;
        fclose(fp);
        printf("Read error\n");
        return -1;
    }

    if (fread(compressed, 1, (size_t)compressed_len, fp) !=
        (size_t)compressed_len) {
        free(compressed);
        free(serialized);
        free(*pat_db);
        free(*scan_db);
        *pat_db = NULL;
        *scan_db = NULL;
        fclose(fp);
        printf("Read error\n");
        return -1;
    }
    fclose(fp);

    if (DecryptWithPassphrase((unsigned char *)compressed, compressed_len,
                              INTERFACE_PASSPHRASE) != 0 ||
        decompress(serialized, compressed, (size_t)compressed_len) != SUCCESS ||
        deserialize(serialized, *pat_db, &pat_amt, *scan_db,
                    &loaded_scan_amt) !=
            SUCCESS) {
        free(compressed);
        free(serialized);
        free(*pat_db);
        FreeScanData(*scan_db, (int)loaded_scan_amt);
        free(*scan_db);
        *pat_db = NULL;
        *scan_db = NULL;
        printf("Read error\n");
        return -1;
    }

    free(compressed);
    free(serialized);
    *scan_amt = (int)loaded_scan_amt;
    return (int)pat_amt;
}

/*******************************************************************************
 * AddPatientData
 * asks for one patient record and appends it to the database.
*******************************************************************************/
int AddPatientData(patient_t *pat_db, int *pat_amt) {
    patient_t new_patient;
    char buffer[INTERFACE_INPUT_SIZE];

    if (pat_db == NULL || pat_amt == NULL || *pat_amt < 0) {
        printf("Add error\n");
        return 1;
    }

    if (*pat_amt >= INTERFACE_MAX_PATIENTS) {
        printf("List is full\n");
        return 1;
    }

    printf("First name: >");
    InterfaceReadLine(buffer, INTERFACE_INPUT_SIZE);
    InterfaceCopyLimitedString(new_patient.firstname, buffer, MAX_NAME_SIZE);

    printf("Surname: >");
    InterfaceReadLine(buffer, INTERFACE_INPUT_SIZE);
    InterfaceCopyLimitedString(new_patient.surname, buffer, MAX_NAME_SIZE);

    do {
        if (InterfaceReadInt("Date of birth(day): >",
                             &new_patient.dob.day) != 0) {
            printf("Invalid day. ");
        } else if (InterfaceReadInt("Date of birth(month): >",
                                    &new_patient.dob.month) != 0) {
            printf("Invalid month. ");
        } else if (InterfaceReadInt("Date of birth(year): >",
                                    &new_patient.dob.year) != 0) {
            printf("Invalid year. ");
        } else if (!InterfaceIsValidDate(new_patient.dob)) {
            printf("Invalid date. ");
        } else {
            break;
        }
    } while (1);

    printf("Gender: >");
    InterfaceReadLine(buffer, INTERFACE_INPUT_SIZE);
    InterfaceCopyLimitedString(new_patient.gender, buffer, MAX_GENDER_SIZE);

    pat_db[*pat_amt] = new_patient;
    *pat_amt = *pat_amt + 1;
    return 0;
}

/*******************************************************************************
 * ViewPatientData
 * displays all patient records in a fixed-width table.
*******************************************************************************/
void ViewPatientData(patient_t *pat_db, int pat_amt, scan_t *scan_db,
                     int scan_amt) {
    int i;
    int j;
    int document_count;
    char datestr[11];

    if (pat_db == NULL || pat_amt <= 0) {
        printf("List is empty\n");
        return;
    }

    printf("%-5s%-20s%-20s%-12s%-12s%s\n", "No.", "First Name", "Surname",
           "DOB", "Gender", "Documents");
    printf("%-5s%-20s%-20s%-12s%-12s%s\n", "---", "----------", "----------",
           "----------", "--------", "---------");

    for (i = 0; i < pat_amt; i++) {
        document_count = 0;
        for (j = 0; j < scan_amt; j++) {
            if (scan_db[j].patient == &pat_db[i]) {
                document_count++;
            }
        }

        InterfaceFormatDate(datestr, sizeof(datestr), pat_db[i].dob);
        printf("%-5d%-20s%-20s%-12s%-12s%d\n", i + 1, pat_db[i].firstname,
               pat_db[i].surname, datestr, pat_db[i].gender, document_count);
    }
}

/*******************************************************************************
 * AttachAdditionalDocument
 * reads a local image/document file into scan_t.content for a patient.
*******************************************************************************/
int AttachAdditionalDocument(patient_t *pat_db, int pat_amt, scan_t *scan_db,
                             int *scan_amt, int index) {
    char buffer[INTERFACE_INPUT_SIZE];
    FILE *fp;
    long file_len;
    char *content;

    if (pat_db == NULL || scan_db == NULL || scan_amt == NULL ||
        pat_amt <= 0) {
        printf("List is empty\n");
        return 1;
    }

    if (*scan_amt >= INTERFACE_MAX_SCANS) {
        printf("Document list is full\n");
        return 1;
    }

    if (index < 0 || index >= pat_amt) {
        printf("Invalid patient number\n");
        return 1;
    }

    printf("Local image/document path: >");
    InterfaceReadLine(buffer, INTERFACE_INPUT_SIZE);

    if (buffer[0] == '\0') {
        printf("Additional document unchanged.\n");
        return 1;
    }

    fp = fopen(buffer, "rb");
    if (fp == NULL) {
        printf("Cannot open document.\n");
        return 1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        printf("Cannot read document.\n");
        return 1;
    }

    file_len = ftell(fp);
    if (file_len <= 0 || file_len > 2147483647L) {
        fclose(fp);
        printf("Cannot read document.\n");
        return 1;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        printf("Cannot read document.\n");
        return 1;
    }

    content = (char *)malloc((size_t)file_len);
    if (content == NULL) {
        fclose(fp);
        printf("Memory allocation failed\n");
        return 1;
    }

    if (fread(content, 1, (size_t)file_len, fp) != (size_t)file_len) {
        free(content);
        fclose(fp);
        printf("Cannot read document.\n");
        return 1;
    }
    fclose(fp);

    scan_db[*scan_amt].patient = &pat_db[index];
    scan_db[*scan_amt].content = content;
    scan_db[*scan_amt].len = (int)file_len;
    *scan_amt = *scan_amt + 1;

    printf("Additional document attached.\n");
    return 0;
}

/*******************************************************************************
 * RemovePatientDocuments
 * frees documents for a deleted patient and keeps other patient links correct.
*******************************************************************************/
void RemovePatientDocuments(patient_t *pat_db, scan_t *scan_db, int *scan_amt,
                            int patient_index) {
    int i = 0;
    int scan_patient_index;

    if (pat_db == NULL || scan_db == NULL || scan_amt == NULL) {
        return;
    }

    while (i < *scan_amt) {
        scan_patient_index = (int)(scan_db[i].patient - pat_db);

        if (scan_patient_index == patient_index) {
            free(scan_db[i].content);
            scan_db[i] = scan_db[*scan_amt - 1];
            *scan_amt = *scan_amt - 1;
        } else {
            if (scan_patient_index > patient_index) {
                scan_db[i].patient = &pat_db[scan_patient_index - 1];
            }
            i++;
        }
    }
}

/*******************************************************************************
 * FreeScanData
 * frees all attached document content.
*******************************************************************************/
void FreeScanData(scan_t *scan_db, int scan_amt) {
    int i;

    if (scan_db == NULL) {
        return;
    }

    for (i = 0; i < scan_amt; i++) {
        free(scan_db[i].content);
        scan_db[i].content = NULL;
        scan_db[i].len = 0;
        scan_db[i].patient = NULL;
    }
}

/*******************************************************************************
 * DeletePatientData
 * deletes the patient at the given zero-based index.
*******************************************************************************/
int DeletePatientData(patient_t *pat_db, int *pat_amt, int index) {
    int i;

    if (pat_db == NULL || pat_amt == NULL || *pat_amt <= 0) {
        printf("List is empty\n");
        return 1;
    }

    if (index < 0 || index >= *pat_amt) {
        printf("Invalid patient number\n");
        return 1;
    }

    for (i = index; i < *pat_amt - 1; i++) {
        pat_db[i] = pat_db[i + 1];
    }

    *pat_amt = *pat_amt - 1;
    return 0;
}

#endif // INTERFACE_H_
