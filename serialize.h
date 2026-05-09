/*******************************************************************************
 * Title: serialize.h
 * Author:
 * Description:
 * This header converts human readable, structured data e.g scan_t patient_t
 * and converts it into a single serialized database file
 *
 * interface functions (functions to be called from outside):
 * -> serialize      => when compressing raw data
 * -> deserialize    => decompress a compressed buffer
 * 
 * .db file format:
 * 
 *******************************************************************************/

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
 *  -> char* dest => compressed char buffer
 *  -> patient_t *pat_db => array of patients
 *  -> int pat_amt       => length of pat_db
 *  -> scan_t *scan_db   => array of scans
 *  -> int scan_amt      => length of scan_db
 * Output:
 *  <- int => length of written string
 *******************************************************************************/
int serialize(char* dest, patient_t *pat_db, int pat_amt, 
        scan_t *scan_db, int scan_amt);

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
int deserialize(char* dest, patient_t *pat_db, int* pat_amt, 
        scan_t *scan_db, int* scan_amt);


/*******************************************************************************
 * save_file
 * Creates a .db file from the input string
 * Input:
 *  -> char* filename => name of the file to be output eg "sample" -> sample.db
 *  -> char* src => the contents of the file to be written out 
 * Output:
 *  <- int => 0 for success -1 for error
 *******************************************************************************/
int save_file(char* filename, char* src);

/*******************************************************************************
 * Function Implementations
 *******************************************************************************/

int serialize(char* dest, patient_t *pat_db, int pat_amt, 
        scan_t *scan_db, int scan_amt){
    
    char* strptr = dest;

    int i;
    for (i = 0; i < pat_amt; i++) {

    }
}

#endif /* Serialize */
