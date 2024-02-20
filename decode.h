
#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include <stdio.h>
#include "common.h"
/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

/* Read and validate Encode args from argv */
typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    int size_secret_file;//long

    /* Decode txt file Info */
    char *stego_decode_fname;
    FILE *fptr_decode_txt;

} DecodeInfo;
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Store Magic String */
// Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);
/*Encode secret file extn size*/
Status decode_output_file_extn_size(int size,DecodeInfo *decInfo);
/* Encode secret file extenstion */
Status decode_output_file_extn( char *file_extn, DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_output_file_size(int file_size, DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_output_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real decoding G */
Status decode_image_to_data(char *data, int size, DecodeInfo *decodeInfo);

/* Encode a byte into LSB of image data array */
char decode_lsb_to_byte(char *image_buffer);

/*Generic function to encode int to the lsb*/
Status decode_size_to_lsb(int *size,char *buffer);

/* Copy remaining image bytes from src to stego image after encoding */
//Status copy_remaining_img_data(DecodeInfo *decInfo);

#endif

