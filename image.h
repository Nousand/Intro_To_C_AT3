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
#include "encrypt.h"
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

typedef enum {
    MODE_COMPRESS,
    MODE_DECOMPRESS,
    MODE_ENCRYPT,
    MODE_DECRYPT
} process_mode_t;

typedef struct {
    process_mode_t mode;
    const char *in_ext;
    const char *out_ext;
    const char *passphrase;
    size_t out_scale;
} image_processor_t;

int process_images(scan_t *scan_db, size_t scan_amt, image_processor_t *proc) {
    for (size_t i = 0; i < scan_amt; i++) {
        scan_t *key = &scan_db[i];

        char inname[NAMESIZE];
        snprintf(inname, sizeof(inname), "database/%s", key->content);

        char *dot = strrchr(inname, '.');
        if (dot)
            *dot = '\0';
        strncat(inname, proc->in_ext, sizeof(inname) - strlen(inname) - 1);

        FILE *file = fopen(inname, "rb");
        if (file == NULL)
            continue;

        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            continue;
        }

        long told_len = ftell(file);
        rewind(file);
        if (told_len <= 0) {
            fclose(file);
            continue;
        }
        size_t file_len = (size_t)told_len;

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

        size_t alloc_scale = (proc->mode == MODE_DECOMPRESS) ? 8 : 2;
        char *output = (char *)malloc(file_len * alloc_scale);
        if (!output) {
            free(buffer);
            continue;
        }

        int result;
        switch (proc->mode) {
        case MODE_COMPRESS:
            result = compress(output, buffer, file_len);
            break;
        case MODE_DECOMPRESS:
            result = decompress(output, buffer, file_len);
            break;
        case MODE_ENCRYPT:
            strcpy(output, buffer);
            result = EncryptWithPassphrase((unsigned char *)output,
                                           (int)file_len, proc->passphrase);
            result = result ? ERROR : file_len;
            break;
        case MODE_DECRYPT:
            strcpy(output, buffer);
            result = DecryptWithPassphrase((unsigned char *)output,
                                           (int)file_len, proc->passphrase);
            result = result ? ERROR : file_len;
            break;
        default:
            free(buffer);
            free(output);
            continue;
        }
        free(buffer);

        if (result < 0) {
            free(output);
            continue;
        }

        char outname[NAMESIZE];
        snprintf(outname, sizeof(outname), "database/%s", key->content);
        dot = strrchr(outname, '.');
        if (dot)
            *dot = '\0';
        strncat(outname, proc->out_ext, sizeof(outname) - strlen(outname) - 1);

        FILE *outfile = fopen(outname, "wb");
        if (outfile == NULL) {
            free(output);
            continue;
        }

        size_t out_bytes = (size_t)result * proc->out_scale;
        fwrite(output, 1, out_bytes, outfile);
        fclose(outfile);
        free(output);
    }
    return SUCCESS;
}

int image_adapter(scan_t *scan_db, size_t scan_amt, process_mode_t mode) {
    image_processor_t proc;
    proc.mode = mode;
    switch (mode) {
    case MODE_COMPRESS:
        proc.in_ext = ".bin";
        proc.out_ext = ".cmp";
        proc.out_scale = sizeof(uint16_t);
        break;
    case MODE_DECOMPRESS:
        proc.in_ext = ".cmp";
        proc.out_ext = ".bin";
        proc.out_scale = 1;
        break;
    case MODE_ENCRYPT:
        proc.in_ext = ".bin";
        proc.out_ext = ".enc";
        proc.out_scale = 1;
        break;
    case MODE_DECRYPT:
        proc.in_ext = ".enc";
        proc.out_ext = ".bin";
        proc.out_scale = 1;
        break;
    default:
        return ERROR;
    }
    return process_images(scan_db, scan_amt, &proc);
}
#endif
