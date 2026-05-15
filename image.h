/*******************************************************************************
 * Title: image.h
 * Author:
 * Description:
 * Implements functionality for image related functionality
 *******************************************************************************/

#ifndef IMAGE_H_
#define IMAGE_H_

#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR -1
#define SUCCESS 0

/*******************************************************************************
 * Function prototypes
 *******************************************************************************/

/*******************************************************************************
 * to_rawimg
 * searchs the filepath of a image and 
 * Input:
 *  -> char* filepath => the path to the image file
 *  -> char* out_name => name of the file saved locally
 * Output:
 *  <- int => 0 for success and -1 for errors
 *******************************************************************************/

int to_rawimg(char *filepath, char *out_name);

/*******************************************************************************
 * bin2origin
 * it iterates through all .bin files in the local database/ directory and
 * renames them file extensions to their original
 * Input:
 *  -> char* filepath => the path to the image file
 *  -> char* out_name => name of the file saved locally
 * Output:
 *  <- int => 0 for success and -1 for errors
 *******************************************************************************/

int bin2origin(scan_t *scan_db, size_t scan_amt);

/*******************************************************************************
 * Function implementations
 *******************************************************************************/

int to_rawimg(char *filepath, char *out_name) {
    /* Extract the filename without the preceding directory path */
    char *path = strdup(filepath);

    char *slash;
    int i = 0;
    slash = strstr(path, "/");
    while(slash) {
        i = strlen(slash) + 1;
        path = &path[strlen(path)-i+2];
        slash = strstr(path, "/");
    }

    strcpy(out_name, path);
    char tname[120];
    strcpy(tname, path);
    free(path);

    /* Null terminates the '.' to strip trailing format */
    char *dot = strstr(tname, ".");
    if (dot)
        *dot = '\0';

    char fname[120];
    sprintf(fname, "database/%s.bin", tname);
    FILE *dest = fopen(fname, "wb");
    if (!dest)
        return ERROR;

    FILE *src = fopen(filepath, "rb");
    if (!src){
        fclose(dest);
        return ERROR;
    }

    /* Copy input file to a local .bin file */
    char buff[4096];
    size_t n;
    while ((n = fread(buff, 1, sizeof(buff), src)) > 0)
        fwrite(buff, 1, n, dest);

    fclose(dest);
    fclose(src);

    return SUCCESS;
}

int bin2origin(scan_t *scan_db, size_t scan_amt){
    char name[120];
    char nname[120];
    char pname[120];
    char *dot;
    int i;
    for (i = 0; i < scan_amt; i++){
        strcpy(name, scan_db[i]->content);
        dot = strstr(pname, ".");
        if (dot)
            *dot = '\0';

        sprintf(pname, "database/%s.bin", name);
        sprintf(fname, "database/%s", scan_db[i]->content);
        rename(pname, fname);
    }

    return SUCCESS;
}

#endif
