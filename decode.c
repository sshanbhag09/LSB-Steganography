#include "decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include <string.h>
#include "common.h"
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp(strstr(argv[2],"."),".bmp")==0)
    {
        decInfo -> src_image_fname = argv[2];

    }
    else
    {
        return e_failure;
    }
    
    if(argv[3]==NULL){
        decInfo->stego_decode_fname="decode.txt";
    }
    else{
        decInfo->stego_decode_fname=argv[3];
    }

    
    return e_success;
}
Status decode_magic_string(char *magic_string,DecodeInfo *decInfo)
{
    //printf("!");
    char magic_data[10]={0},ch;
    fseek(decInfo->fptr_src_image,54,SEEK_SET);
    //decode_image_to_data(magic_string,strlen(magic_string),decInfo);
    for(int i=0;i<2;i++)
    {
    fread(decInfo->image_data, 8, 1, decInfo->fptr_src_image);
    ch=decode_lsb_to_byte(decInfo->image_data);
    
    magic_data[i]=ch;

    }
    printf("The magic data is %.2s\n",magic_data);
    //fwrite(magic_data,sizeof(magic_data),1,decInfo->fptr_decode_txt);
    //printf("%ld",ftell(decInfo->fptr_src_image));
        
    
    if(strcmp(magic_string, magic_data) == 0)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }

    
        
}

char decode_lsb_to_byte(char *image_buffer)
{

    char data = 0;

    for(int i = 0;i < 8;i++)
    {
	data |= (image_buffer[i] & 1) << i;
    }
    return data;
    
}
Status decode_output_file_extn_size(int size,DecodeInfo *decInfo)
{
    char str[32];
    int length;

 
    fread(str, 32, sizeof(char), decInfo->fptr_src_image);

    decode_size_to_lsb(&length,str);
    printf("size of extn %d\n",length);
    //printf("ftell %ld",ftell(decInfo->fptr_src_image));
    //fwrite(&length,sizeof(length),1,decInfo->fptr_decode_txt);
    if (length == size)
        return e_success;
    else
        return e_failure;
}
Status decode_size_to_lsb(int *size,char *buffer)
{
    int j = 31;
    int num = 0x00;
    for (int i = 0; i < 32; i++)
    {
        num = ((buffer[i] & 0x01) << i) | num;
    }
    *size = num;
}
Status open_decode_files(DecodeInfo *decInfo)
{
    //1.open file of image and decode.txt destination
    
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    //2.check the src file
    if (decInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);

    	return e_failure;
    }
    //3.open decode .txt
    decInfo ->fptr_decode_txt = fopen(decInfo ->stego_decode_fname,"w");
    
    //4.check decode.txt
    if (decInfo->fptr_decode_txt == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo ->stego_decode_fname);

    	return e_failure;
    }

    return e_success;
}
/*Status decode_size_to_lsb(int size,DecodeInfo *decInfo)
{
    int decoded_int=0;
    //char str[32];
    //fread(str,32,1,decInfo->fptr_src_image);
    for(int i=0;i<4;i++){
        
        char scan_char = 0;//Read and store each character
				for (uint i = 0; i < 8; i++)//8 times inner iteration
				{
						
						fread(&scan_char, sizeof(scan_char), 1, decInfo->fptr_src_image);
						if (ferror(decInfo->fptr_src_image))//Error handling
						{
								printf("ERROR: Error in reading from image file.\n\n");
								exit(e_success);
						}
						scan_char &= 01;//Obtain the least significant bit
						decoded_int <<= 1;//Left shift by 1 bit to store obtained least significant bit
						decoded_int |= (uint) scan_char;//Store the obtained least significant bit
				}
    
    }
    return decoded_int;
}*/
Status decode_output_file_extn(char *file_extn, DecodeInfo *decInfo)
{
    file_extn = ".txt";
    int size = strlen(file_extn);

    char ch,extn[4];
    for (int i = 0; i < size; i++)
    {
        fread(decInfo->src_image_fname, 8, 1, decInfo->fptr_src_image);
        ch=decode_lsb_to_byte(decInfo->src_image_fname);
	extn[i]=ch;
    }
	printf("The extension of secret file is %s\n",extn);
    //fwrite(extn,sizeof(extn),1,decInfo->fptr_decode_txt);
    decInfo->extn_secret_file[size] = '\0';

    if (strcmp(decInfo->extn_secret_file, file_extn) == 0)
        return e_success;
    else
        return e_failure;
}
Status decode_output_file_size(int filesize, DecodeInfo *decInfo)
{
    char str[32];
    

 
    fread(str, 32, sizeof(char), decInfo->fptr_src_image);

    decode_size_to_lsb(&decInfo->size_secret_file,str);
    printf("The decoded secret file sze is %d\n",decInfo->size_secret_file);
    //fwrite(str,sizeof(str),1,decInfo->fptr_decode_txt);
    if (filesize==decInfo->size_secret_file)
        return e_success;
    else
        return e_failure;
}
int file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}
Status decode_output_file_data(DecodeInfo *decInfo)
{
	char str[decInfo->size_secret_file];
	char ch;	
	for(int i=0;i<decInfo->size_secret_file;i++)
	{
		fread(decInfo->src_image_fname, 8, 1, decInfo->fptr_src_image);
        	ch=decode_lsb_to_byte(decInfo->src_image_fname);
		str[i]=ch;
		
	}
	printf("The decoded secret data is %s\n",str);
	str[decInfo->size_secret_file] = '\0';
        fwrite(str,sizeof(str),1,decInfo->fptr_decode_txt);

	return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
     
     if(open_decode_files(decInfo)==e_success){
        printf("open decode Files is a success\n");
        int filesize = file_size(decInfo->fptr_src_image);
        fseek(decInfo->fptr_src_image,0,SEEK_SET);
        if(decode_magic_string(MAGIC_STRING,decInfo)==e_success)
            {
                printf("Decoded Magic String successfully\n");
		if(decode_output_file_extn_size(MAX_FILE_SUFFIX,decInfo)==e_success)
                {
                    printf("decoded file extn size\n");
			 
			if(decode_output_file_extn(decInfo -> extn_secret_file,decInfo))
			{
				printf("Decoded file extn size successfully\n");
				
				if(decode_output_file_size(filesize, decInfo))
				{
					printf("Decoded secret file size\n");
					if(decode_output_file_data(decInfo))
					{
						printf("Decoded output file data\n");
					}
				}
				else
				{
					printf("Decoded secret file size unsuccessfully\n");
				}
			}
			else
			{
				printf("Decoded file extn size unsuccessfully\n");
			}
                }
                else{
                    printf("Failed to decode file extn size\n");
                }
            }
        else
        {
            printf("Decoded Magic String unsuccessfully\n");
        }
     }
     else{
        printf("Open decode files is a failure\n");
        return e_failure;
    }
    return e_success;
}
