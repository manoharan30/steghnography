#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char** argv)
{
   if(argc == 1)                 //validating whether the total number of required arguments passed or not
    {
        printf("./a.out : Encoding: ./a.out -e <.bmp.file> <.txt.file> [output file]\n");
        printf("./a.out : Decoding: ./a.out -d <.bmp.file> [output file]\n");
        return 0;
    }
 int ret=check_operation_type(argv[1]);      //function call to check whether the user choosed to encode or decode
 if(ret==e_encode){
   if(argc>=4){                              //validating whether the total number of required arguments passed or not
    printf("Selected encoding");
    EncodeInfo encInfo;                      // creating a variable for encoding
    if(read_and_validate_encode_args(argv,&encInfo)==e_success){     //calling read and validate function
      printf("INFO: Read and Validate Success \n");
				if(do_encoding(&encInfo) == e_success)                   //function call to encode 
				{
					printf("## ======== Encoding Done Successfully ======== ##\n");
            }
            else{
               printf("## ======== Encoding Failed ======== ##\n");
            }
      }
      else{
         printf("INFO: Read and Validate Error\n");
      }
   }
   else{
      printf("./a.out : Encoding: ./a.out -e <.bmp.file> <.txt.file> [output file]\n");
      return 0;
   }
 }
 else if(ret==e_decode) {
   if(argc>=3){                           //validating whether the total number of required arguments passed or not
    printf("Selected decoding");
    DecodeInfo decInfo;                   // creating variable for decoding
    if(read_and_validate_decode_args(argv,&decInfo)==e_success){     //a function call to read and validate arguments
      if(do_decoding(&decInfo)==e_success){                          //calling do decoding function to decode secret message
         printf("## ======== decoding Done Successfully ======== ##\n");
      }
      else{
        printf("## ======== decoding Failed ======== ##\n");  
      }
    }
    else{
       printf("INFO: Read and Validate Error\n");
    }
 }
 else{
   printf("./a.out : Decoding: ./a.out -d <.bmp.file> [output file]\n");
   return 0;
 }
 }
 else{
   printf("./a.out : Encoding: ./a.out -e <.bmp.file> <.txt.file> [output file]\n");
   printf("./a.out : Decoding: ./a.out -d <.bmp.file> [output file]\n");
   return 0;
 }
}

/*function to determine whether the user wants to encode or decode*/
OperationType check_operation_type(char* argv){
    if(strcmp(argv,"-e")==0) return e_encode;
    else if(strcmp(argv,"-d")==0) return e_decode;
    else return e_unsupported;
}
