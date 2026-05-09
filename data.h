/*******************************************************************************
 * Title: data.h
 * Author: Rentaro Beattie
 * Description:
 * This file aggregates the common data structures used across the program
 * into one header file that can be easily accessed by other files
 *******************************************************************************/

#ifndef DATA_H_
#define DATA_H_

#define MAX_NAME_SIZE 20
#define MAX_GENDER_SIZE 10

/*******************************************************************************
 * date_t
 *******************************************************************************/

typedef struct {
    int day;
    int month;
    int year;
} date_t;

/*******************************************************************************
 * patient_t
 * -> char firstname[MAX_NAME_SIZE]
 * -> char surname[MAX_NAME_SIZE]
 * -> date_t dob                   => date of birth
 * -> char gender[MAX_GENDER_SIZE] => i.e "male" "female" "other"
 *******************************************************************************/
typedef struct {
    char firstname[MAX_NAME_SIZE];
    char surname[MAX_NAME_SIZE];
    date_t dob;
    char gender[MAX_GENDER_SIZE];
} patient_t;

/*******************************************************************************
 * scan_t
 * -> patient_t *patient => associated patient
 * -> char *content      => Head of the raw image data itself (not null
 * terminated)
 * -> int len            => length of file
 *******************************************************************************/
typedef struct {
    patient_t *patient;
    char *content;
    int len;
} scan_t;

#endif /* DATA_H_ */
