#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status do_decoding(DecodeInfo *decInfo){                                                //function to call all other functions in decoding
    if(open_decode_files(decInfo)==e_success){
        printf("INFO:files opened successfully\n");
        if(decode_magic_string(decInfo)==e_success){                                    //function call to decode magic string
            printf("INFO:Magic string decoded successfully\n");
            if(decode_scrt_file_extn_size(decInfo)==e_success){                         //function call to decode secret file extention size
                printf("INFO:Secret file extension size decoded successfully\n");
                if(decode_secrt_file_extn(decInfo)==e_success){                         //function call to  decode file extention
                    printf("INFO:Secret file extension decoded successfully\n");
                    if(decode_secret_file_size(decInfo)==e_success){                    //function call to decode secret file size
                        printf("INFO:Secret file size decoded successfully\n");
                        if(decode_secret_file_data(decInfo)==e_success){                //function call to decode secret file data
                            printf("INFO:decoding secret file data successfull\n");
                            return e_success;
                        }
                        else{
                            printf("INFO: decoding secret file data failed\n");
                            return e_failure;
                        }
                    }
                    else{
                        printf("INFO:Decoding secret file size failed\n");
                        return e_failure;
                    }
                }
                else{
                    printf("INFO:Decoding secret file extention failed\n");
                    return e_failure;
                }
            }
            else{
                printf("INFO:Decoding secret file extention size failed\n");
                return e_failure;
            }
        }
        else{
            printf("INFO:Decoding magic string failed\n");
            return e_failure;
        }
    }
    else{
        printf("INFO:Opening files failed\n");
        return e_failure;
    }
}

/*the below function is used to validate and read the CLA passed are relevent or not*/
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo){
    if(argv[3]!=NULL){                                  
        strcpy(decInfo->secret_fname,argv[3]);          //copying the output file into structure member
    }
    else{
        strcpy(decInfo->secret_fname,"default");

    }
    if((!strcmp(strchr(argv[2],'.'),".bmp"))){          
        decInfo->stego_image_fname=argv[2];             //storing the encoded image's file name into the structure
      
    }
    else if(strchr(argv[2],'.')==NULL){
        printf("Error:pass encoded file with extension\n");
        return e_failure;
    }
    else{
        printf("Error:stego file extension wrong\n");
    }
    return e_success;

}

/*the function below is used for opening the file pointer of encoded image*/
Status open_decode_files(DecodeInfo *decInfo){
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r"); //opening the encoded image file and storing it in a pointer
    if(decInfo->fptr_stego_image == NULL){
        printf("ERROR: opening file\n");
        return e_failure;
    }
    else return e_success;
}

/*the below function used to decode the magic string from the encoded image*/
Status decode_magic_string(DecodeInfo *decInfo){
    char str[20];
    printf("Enter the magic string:");
    scanf("%s",str);                                                //reading the magic string from the user
    char buf[6];
    char img_buf[8];
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);                 //changing the file pointer from 0th index to 54 because magic string is present in 54th byte
    for(int i=0;i<5;i++){
        fread(img_buf,8,1,decInfo->fptr_stego_image);               //reading 8 byte from encoded image
        decode_lsb_to_byte(buf,i,img_buf);                          //function call to decode lsb from each byte and store it in a char buffer
    }
    buf[6]='\n';                                                    // adding null character to the magic string
    if(strcmp(buf,str)==0) return e_success;                        // validating whether the user's i/p and magic string are matching or not
    else return e_failure;
}


/*the below function is used to decode secret file extention size*/
Status decode_secrt_file_extn(DecodeInfo *decInfo){
    char img_buf[8];
    for(int i=0;i<decInfo->extn_size;i++){
        fread(img_buf,8,1,decInfo->fptr_stego_image);               //reading 8 byte from encoded image
        decode_lsb_to_byte(decInfo->extn_secret_file, i, img_buf);  //function call to decode lsb from each byte and store it in a char buffer
    }
    strcat(decInfo->secret_fname,decInfo->extn_secret_file);        //adding extention to the output file using strcat function
    return e_success;
}

/*function to decode secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo){
    char img_buf[32];
    fread(img_buf,32,1,decInfo->fptr_stego_image);              //reading 32 byte from encoded image
    decode_lsb_to_size(&decInfo->size_secret_file,img_buf);     //function call to decode lsb from each byte and store it in a img buffer
    return e_success;
}

/*function to decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo){
    decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");      //opening file pointer for output

    if(decInfo->fptr_secret==NULL){                             // validating whether file has been opened or not
        printf("INFO:Error in opening file");
        return e_failure;
    }

    char arr[8];
    for(int i=0;i<decInfo->size_secret_file;i++){
        fread(arr,8,1,decInfo->fptr_stego_image);           // reading 8 byte at a time to decode from encoded image
        decode_lsb_to_byte(decInfo->secret_data,i,arr);     // function call to decode lsb from encoded image
    }

    fwrite(decInfo->secret_data,decInfo->size_secret_file,1,decInfo->fptr_secret);  //writing the array elements(secret message) into output file
    fclose(decInfo->fptr_secret);                                                   // closing file pointer
    return e_success;
}

/*function to decode secret file extention size*/
Status decode_scrt_file_extn_size(DecodeInfo *decInfo){
    char img_buf[32];
    fread(img_buf,32,1,decInfo->fptr_stego_image);              //reading 32 bytes from encoded image
    decode_lsb_to_size(&decInfo->extn_size,img_buf);            // function call to decode lsb of 32 bytes to find the extn size 
    return e_success;
}

/*function to decode lsb into byte(char)*/
Status decode_lsb_to_byte(char *data,int j,char* image_buffer){
    data[j]=0;
    for(int i=0;i<8;i++){
        data[j]=(((image_buffer[i]&(1))<<7-i)|data[j]);         //manipulating the bits and storing it in the array
    }
}

/*function to decode lsb into size(int)*/
Status decode_lsb_to_size(int *data,char *image_buffer){
    *data=0;
    for(int i=0;i<32;i++){
        *data=(((image_buffer[i]&(1))<<(31-i))|*data);      //manipulating bits and storing it in integer pointer
    }
}