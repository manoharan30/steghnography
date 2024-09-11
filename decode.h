//#ifndef DECODE_H
//#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char secret_fname[20];
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[150];
    int size_secret_file;
	int extn_size;

} DecodeInfo;

/*Decoding function prototype*/

/* read and validate decode arguments from argv*/
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

/*perform the decoding*/
Status do_decoding(DecodeInfo *decInfo);

/*get file pointers for i/p and o/p files*/
Status open_decode_files(DecodeInfo *decInfo);

/* store magic string*/
Status decode_magic_string(DecodeInfo *decInfo);

/*decode secret file extension*/
Status decode_secrt_file_extn(DecodeInfo *decInfo);

/*decode secrt file size*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/*decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/*decode scrt file extn size*/
Status decode_scrt_file_extn_size(DecodeInfo *decInfo);

/*decode lsb to byte*/
Status decode_lsb_to_byte(char *data,int i,char* image_buffer);

/*decode lsb to size*/
Status decode_lsb_to_size(int *data,char *image_buffer);