/*******************************************************************************
 * Title: image.h
 * Author: Rentaro Beattie
 * Description:
 * Implements functionality for image related functionality
 *******************************************************************************/

#ifndef IMAGE_H_
#define IMAGE_H_

#include "compression.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR -1
#define SUCCESS 0
#define NAMESIZE 120

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
 * renames their file extensions to their original prior to serialization
 * Input:
 *  -> scan_t *scan_db => array of scans/image files
 *  -> size_t scan_amt => size of the array
 * Output:
 *  <- int => 0 for success and -1 for errors
 *******************************************************************************/

int bin2origin(scan_t *scan_db, size_t scan_amt);

/*******************************************************************************
 * Function implementations
 *******************************************************************************/

int to_rawimg(char *filepath, char *out_name) {
    /* Extract the filename without the preceding directory path */
    char *slash = strrchr(filepath, '/');
    char *filename = slash ? slash + 1 : filepath;
    strcpy(out_name, filename);
    char tname[NAMESIZE];
    strcpy(tname, filename);

    /* Null terminates the '.' to strip trailing format */
    char *dot = strstr(tname, ".");
    if (dot)
        *dot = '\0';

    char fname[NAMESIZE];
    sprintf(fname, "database/%s.bin", tname);
    FILE *dest = fopen(fname, "wb");
    if (!dest)
        return ERROR;

    FILE *src = fopen(filepath, "rb");
    if (!src) {
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

int bin2origin(scan_t *scan_db, size_t scan_amt) {
    char name[NAMESIZE];
    char nname[NAMESIZE];
    char pname[NAMESIZE];
    char *dot;
    int i;
    for (i = 0; i < scan_amt; i++) {
        strcpy(name, scan_db[i].content);
        dot = strstr(name, ".");
        if (dot)
            *dot = '\0';

        sprintf(pname, "database/%s.bin", name);
        sprintf(nname, "database/%s", scan_db[i].content);
        rename(pname, nname);
    }

    return SUCCESS;
}

int compress_image(scan_t *scan_db, size_t scan_amt) {
    int i;
    for (i = 0; i < scan_amt; i++) {
        scan_t *key = &scan_db[i];

        /* ensure file searchs for .bin files in database/ */
        char name[NAMESIZE] = "database/";
        strcat(name, key->content);
        char *dot = strstr(name, ".");
        if (dot)
            *dot = '\0';
        strcat(name, ".bin");

        /* absence of .bin files do not warrant raising errors thus continue */
        FILE *file = fopen(name, "rb");
        if (file == NULL) {
            continue;
        }

        /* Go to end of file to determine the file_len */
        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            continue;
        }

        size_t file_len = ftell(file);
        rewind(file);

        /* read file into buffer */
        char *buffer = (char *)malloc(file_len + 1);
        size_t bytes_read = fread(buffer, 1, file_len, file);
        if (bytes_read < file_len) {
            free(buffer);
            fclose(file);
            continue;
        }

        buffer[bytes_read] = '\0';

        /* close file here (to avoid repeating fclose(file) in err checks) */
        fclose(file);

        char *output = (char *)malloc(file_len + 1);

        int comp_len = compress(output, buffer);
        free(buffer);
        if (comp_len < 0) {
            free(output);
            continue;
        }

        size_t comp_bytes = (size_t)comp_len * sizeof(uint16_t);

        /* Write the compressed file to database */
        dot = strstr(name, ".");
        if (dot)
            *dot = '\0';
        strcat(name, ".cmp");

        FILE *comped = fopen(name, "wb");
        if (comped == NULL) {
            free(output);
            continue;
        }

        fwrite(output, 1, comp_bytes, comped);
        fclose(comped);
        free(output);
    }

    return SUCCESS;
}

int decompress_image(scan_t *scan_db, size_t scan_amt) {
    int i;
    for (i = 0; i < scan_amt; i++) {
        scan_t *key = &scan_db[i];

        /* Build .cmp path */
        char name[NAMESIZE] = "database/";
        strcat(name, key->content);
        char *dot = strstr(name, ".");
        if (dot)
            *dot = '\0';
        strcat(name, ".cmp");

        FILE *file = fopen(name, "rb");
        if (file == NULL)
            continue;

        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            continue;
        }
        size_t file_len = (size_t)ftell(file);
        rewind(file);

        char *buffer = (char *)malloc(file_len);
        if (!buffer) {
            fclose(file);
            continue;
        }
        size_t bytes_read = fread(buffer, 1, file_len, file);
        fclose(file);
        if (bytes_read < file_len) {
            free(buffer);
            continue;
        }

        /* Decompress — output will be larger than input, allocate generously */
        char *output = (char *)malloc(file_len * 8);
        if (!output) {
            free(buffer);
            continue;
        }

        if (decompress(output, buffer, file_len) < SUCCESS) {
            free(buffer);
            free(output);
            continue;
        }
        free(buffer);

        /* Write back to .bin */
        char binname[NAMESIZE] = "database/";
        strcat(binname, key->content);
        dot = strstr(binname, ".");
        if (dot)
            *dot = '\0';
        strcat(binname, ".bin");

        FILE *binfile = fopen(binname, "wb");
        if (binfile == NULL) {
            free(output);
            continue;
        }
        fwrite(output, 1, file_len * 8, binfile);
        fclose(binfile);
        free(output);
    }
    return SUCCESS;
}

#endif
