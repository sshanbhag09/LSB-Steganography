#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Function Definitions */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo){
    if(strcmp(strstr(argv[2],"."),".bmp")==0)
    {
        encInfo -> src_image_fname = argv[2];

    }
    else
    {
        return e_failure;
    }
    if(strcmp(strstr(argv[3],"."),".txt")==0)
    {
        encInfo->secret_fname=argv[3];
    }
    else{
        return e_failure;
    }
    if(argv[4]==NULL){
        encInfo->stego_image_fname="stego.bmp";
    }
    else{
        encInfo->stego_image_fname=argv[4];
    }
    return e_success;
}
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status  check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo ->fptr_src_image);
    encInfo->size_secret_file =get_file_size(encInfo ->fptr_secret);
    if(encInfo ->image_capacity >(54+16+32+32+32+(encInfo ->size_secret_file*8))){
        return e_success;
    }
    else{
        return e_failure;
    }

}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char str[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(str,54,1,fptr_src_image);
    fwrite(str,54,1,fptr_dest_image);
    return e_success;
}
Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,strlen(magic_string),encInfo);
    return e_success;     
}
Status encode_data_to_image(char *data,int size,EncodeInfo *encInfo)
{
    for(int i=0;i<size;i++)
        {
            
            
            fread(encInfo ->image_data,8,1,encInfo->fptr_src_image); //8 bytes from beautiful.bmp
            encode_byte_to_lsb(data[i],encInfo->image_data);
            fwrite(encInfo ->image_data,8,1,encInfo->fptr_stego_image); // encodedd 8 bytes in stego.bmp
        }
}
Status encode_byte_to_lsb(char data,char *imageBuffer)
{
    for(int i=0;i<8;i++){
    imageBuffer[i]=(imageBuffer[i] & 0xFE) | ((data>>i) & 1);
    }
}
Status encode_secret_file_extn_size(int size ,EncodeInfo *encInfo)
{
    encode_size_to_lsb(size,encInfo);
    return e_success;
}
Status encode_size_to_lsb(int size,EncodeInfo *encInfo)
{
    char str[32];
    fread(str,32,1,encInfo->fptr_src_image);
    for(int i=0;i<32;i++){
        //1 byte = cleared lsb  | bit obtained from size
    str[i]=(str[i] & 0xFE) | ((size>>i) & 1);
    }
    fwrite(str,32,1,encInfo ->fptr_stego_image);
}
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo);
    return e_success;    
}
Status encode_secret_file_size(int file_size,EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{   //int n=;
    char str[encInfo ->size_secret_file];
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    fread(str,encInfo ->size_secret_file,1,encInfo ->fptr_secret);
    encode_data_to_image(str,encInfo ->size_secret_file,encInfo);
    return 0;
}
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
   
    int len=encInfo ->image_capacity +55 - ftell(encInfo ->fptr_src_image);
     printf("1");
    char ch[len];
    
    fread(ch,len,1,encInfo ->fptr_src_image);
    
    fwrite(ch,len,1,encInfo ->fptr_stego_image);
    

    return e_success;
}
//Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success){
        printf("openFiles is a success\n");
        if(check_capacity(encInfo)==e_success)
        {
            
            printf("Check Capacity is a success\n");
            if(copy_bmp_header(encInfo ->fptr_src_image,encInfo ->fptr_stego_image)==e_success)
            {
                printf("Copy bmp header is a success\n");
                if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
                {
                    printf("Encoded Magic String successfully\n");
                     //encInfo -> secret_fname =  strstr(encInfo -> secret_fname,".");
                    strcpy(encInfo -> extn_secret_file,strstr(encInfo ->secret_fname,"."));
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success)
                    {
                        printf("Encoded Secret file Extnsize\n");
                        if(encode_secret_file_extn(encInfo ->extn_secret_file,encInfo)==e_success)
                        {
                            printf("Encoded Secret file extn\n");
                            if(encode_secret_file_size(encInfo ->size_secret_file,encInfo)==e_success)
                            {
                                printf("Encoded secret file size\n");
                                if(encode_secret_file_data(encInfo) ==e_success)
                                {
                                    printf("Encoded secret file data\n");
                                    if(copy_remaining_img_data(encInfo)==e_success){
                                        printf("Copied remaining data\n");
                                    }
                                    else{
                                            printf("Failed to copy remaining data\n");
                                            
                                    }
                                    
                                }
                                else{
                                    printf("Failed to encode secret file data\n");
                                    return e_failure;
                                }

                            }
                            else{
                                printf("Failed to encode secret file size\n");
                                return e_failure;
                            }
                        }
                        else{
                            printf("Failed to encode secret file extn\n");
                            return e_failure;
                        }

                    }
                    else{
                        printf("Failed to encode secret file extn size\n");
                        return e_failure;
                    }
                }

		else{
			printf("Failed ailed to encode magic string\n");
		}
            }
            else{
                printf("Copy bmp header is a failure\n");
                return e_failure;
            }
        }
        else{
            printf("Check Capacity is a failure\n");
            return e_failure;
        }
    }
    else{
        printf("Open files is a failure\n");
        return e_failure;
    }
    return e_success;
}
