#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"
int main(int argc,char *argv[])
{
    //atleast 3 command line args and check
    
    
    if(check_operation_type(argv)==e_encode&&argc>=4){
        printf("Selected Encoding\n");
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,&encInfo)==e_success)
        {
            printf("Read and validate encode arg func is a success\n");
            if(do_encoding(&encInfo)==e_success)
            {
                printf("Done encoding successfully\n");
            }
            else{
                printf("Failed to do the encoding\n");
                return -1;
            }

        }

        else{
            printf("Read and validate encode arg func is a failure\n");
            return -1;
        }
    }
    else if(check_operation_type(argv)==e_decode && argc>=3){
        printf("Selected decoding\n");
        DecodeInfo decInfo;
        if(read_and_validate_decode_args(argv,&decInfo)==e_success)
        {
            printf("Read and validate decode arg func is a success\n");
            if(do_decoding(&decInfo)==e_success)
            {
                printf("Done decoding successfully\n");
            }
            else{
                printf("Failed to do the decoding\n");
                return -1;
            }

        }

        else{
            printf("Read and validate encode arg func is a failure\n");
            return -1;
        }
    }
    else{
        printf("Invalid input\nPlease pass\n For encoding");
    }

    
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0){
        return e_decode;
    }
    else{
        return e_unsupported;
    }
}

