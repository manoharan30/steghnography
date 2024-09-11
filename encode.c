#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

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
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

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

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2],".bmp"))                          // checking whether given input file argv[2] is .bmp file or not
    {
        encInfo->src_image_fname = argv[2];          // storing argv[2] in fname
    }
    else
    {
        return e_failure;
    }
    if(strstr(argv[3],".txt"))                       // checking whether given input file argv[3] is .txt file or not
    {
        encInfo->secret_fname = argv[3];             // storing argv[3] in secret fname
        strcpy(encInfo->extn_secret_file,strstr(argv[3],".txt"));
    }
    else{
        return e_failure;
    }
    if(argv[4] != NULL)                              // checking  whether the file is passed or not , if yes
    {
        if(strstr(argv[4],".bmp"))                   // checking whether given input file argv[4] is .bmp file or not
        {
            encInfo->stego_image_fname = argv[4];    // storing argv[4] in structure
        }
        else{
            return e_failure;
        }
    }
    else{                                            //defualt name to the src_image_fname
        encInfo->stego_image_fname = "defualt.bmp";
    }
    return e_success;
    
}

/*function to check file capacity*/
Status check_capacity(EncodeInfo *encInfo){
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);        //storing total size of the image into structure
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (54+ (strlen(MAGIC_STRING)*8) + (strlen(strstr(encInfo->secret_fname,"."))*8) + (sizeof(int)*8) + (sizeof(int)*8) + (encInfo->size_secret_file*8) )){
        return e_success;               //validating whether the image has enough space to encode the secret file
    }
    else{
        return e_failure;
    }
}
uint get_file_size(FILE *fptr){
    fseek(fptr,0,SEEK_END);                 //function to calculate the size of the file
    return ftell(fptr);
}
Status do_encoding(EncodeInfo *encInfo){                                   //function to encode all data into image
    if((open_files(encInfo))==e_success){                                   //function call to open required files
        printf("INFO: open_files is success !!!\n");
        if((check_capacity(encInfo))==e_success){                           //function call to check capacity
            printf("INFO: check_capacity is success !!!\n");
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){  //function call to copy header bytes
                printf("INFO: copy_bmp_header is success !!!\n");
                if(encode_magic_string(MAGIC_STRING,encInfo)==e_success){           //function  call to encode magic string
                    printf("INFO: encode_magic_string is success !!!\n");
                    if(encode_secret_file_extn_size(encInfo) == e_success){         //function call to encode secret file extn size
                        printf("INFO: encode_secret_file_extn_size is success !!!\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success){      //function call to encode secret file extn size
                            printf("INFO: encode_secret_file_extn is success !!!\n");
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success){      //function call to encode secret file siize
                                printf("INFO: encode_secret_file_size is success !!!\n");
                                if(encode_secret_file_data(encInfo) == e_success){                  //function call to encode secret file data
                                  printf("INFO: encode_secret_file_data is success !!!\n");   
                                   if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success){        //function call to copy remaining data into stego image
                                        printf("INFO: copy_remaining_img_datais success\n");
                                        return e_success;
                                    }
									else
									{
									    printf("INFO: Encoding Secret File Remaining Copy Failure\n");
									    return e_failure;
									}
                                }
                                else{
                                    printf("INFO: Encoding Secret File Data Failure\n");
								    return e_failure;
                                }

                            }
                            else{
                                printf("INFO: Encoding Secret File Size Failure\n");
							    return e_failure;
                            }
                        }
                        else{
                            printf("INFO: Encoding Secret File Extension Failure\n");
						    return e_failure;
                        }
                    }
                    else{
                        printf("INFO: Encoding Secret File Extn Size Failure\n");
						return e_failure;
                    }    
                }
                else{
                    printf("INFO: Encoding Magic String Failure\n");
					return e_failure;
                }
            }
            else{
                printf("INFO: Failed\n");
				return e_failure;
            }
        }
        else{
            printf("INFO: Found Not\n");
			return e_failure;
        }
    }
    else{
        printf("INFO: Opening Files Failed\n");
	 	return e_failure;
    }
}

/*function to copy first 54 bytes (header)*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
    rewind(fptr_src_image);         //using rewind to fit file pointer at correct position
    char buffer[54];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);        //writing header into output file
    if(ftell(fptr_src_image)==54) return e_success;     // validation
    else return e_failure;
}

/*function to  encode magic string*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo){
    char arr[8];
    int temp=54+(strlen(magic_string)*8);                               //creating a temporary variable for validation purpose
    for(int i=0;i<strlen(magic_string);i++){
        fread(arr,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i],arr);                        //function call to encode lsb to byte
        fwrite(arr,8,1,encInfo->fptr_stego_image);
    }
    if((ftell(encInfo->fptr_stego_image))==temp) return e_success;      //validation
    else return e_failure;
}

/*function to encode secret file extn size*/
Status encode_secret_file_extn_size(EncodeInfo *encInfo){
    int temp=ftell(encInfo->fptr_stego_image)+32;               // temporary variable for validation
    char arr[32];
    int n=strlen(encInfo->extn_secret_file);                    // finding lenght of secret file 
    fread(arr,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(n,arr);                                  // encoding secret file extn size into stego image
    fwrite(arr,32,1,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_stego_image)==temp) return e_success;    //validation
    else return e_failure;
}

/*function to encode secret file extn*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo){
    char arr[8];
    int temp=ftell(encInfo->fptr_stego_image)+(strlen(file_extn)*8);        //temporary variable for validation
    for(int i=0;i<strlen(file_extn);i++){
        fread(arr,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],arr);                               // function call to encode secret file extn
        fwrite(arr,8,1,encInfo->fptr_stego_image);
    }
    if(ftell(encInfo->fptr_stego_image)==temp) return e_success;            // validation
    else return e_failure;
}

/*function to encode secret file size*/
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo){
    int temp=ftell(encInfo->fptr_stego_image)+32;                       //temporary variable for validation
    char arr[32];
    fread(arr,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,arr);                                  //function call to encode secret file size into lsb
    fwrite(arr,32,1,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_stego_image)==temp) return e_success;        //validation
    else return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo){
    int temp=ftell(encInfo->fptr_stego_image)+((encInfo->size_secret_file)*8);           // Calculate expected end position after encoding secret file data
    char arr[(encInfo->size_secret_file)+1];                                            // Array to hold secret file data
    rewind(encInfo -> fptr_secret);                                                     // Rewind secret file pointer to beginning
    fread(arr,encInfo->size_secret_file,1,encInfo->fptr_secret);                        // Read secret file data into arr
    char arr1[8];                                                                          // Temporary array to hold image data
    for(int i=0;i<encInfo->size_secret_file;i++){
        fread(arr1,8,1,encInfo->fptr_src_image);               
        encode_byte_to_lsb(arr[i],arr1);                                        // Encode secret file byte into LSB of image byte
        fwrite(arr1,8,1,encInfo->fptr_stego_image);                             // Write encoded image byte to stego image
    }
    if(ftell(encInfo->fptr_stego_image)==temp) return e_success;                // Check if encoding completed successfully
    else return e_failure;                                                          // Return failure if encoding failed
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    char ch;
    while((fread(&ch,1,1,fptr_src)) > 0){                                           // Read each byte from source file until EOF
        fwrite(&ch,1,1,fptr_dest);                                                  // Write each byte to destination file
    }
    return e_success;                                                               // Return success after copying
}

Status encode_byte_to_lsb(char data, char *image_buffer){
    for(int i=0;i<8;i++){ 
        image_buffer[i]=(image_buffer[i]&(~1))|(unsigned)(data&(1<<(7-i)))>>(7-i);          // Encode data byte into LSB of image buffer byte
    }
}  

Status encode_size_to_lsb(int data, char *image_buffer){
    for(int i=0;i<32;i++){ 
        image_buffer[i]=(image_buffer[i]&(~1))|(unsigned)(data&(1<<(31-i)))>>(31-i);        // Encode integer into LSB of image buffer
    } 
}
