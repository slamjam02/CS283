#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here

void print_help(){
    printf("This is a Text Line Processor application.\n\n");
    printf("$ stringfun -[h|c|r|w|x] \"sample string\" [other_options]\n\n");
    printf("Arguments:\n");
    printf("-c    counts of the number of words in the \"sample string\"\n");
    printf("-r    reverses the characters (in place) in \"sample string\"\n");
    printf("-w    prints the individual words and their length in the \"sample string\"\n");
    printf("-x    takes sample string and 2 other strings, replaces the first with second\n");
    printf("\n");
}


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int currentBufferPos = 0;
    int currentUserPos = 0;

    while(user_str[currentUserPos] != '\0'){
        // "Boolean" variable to confirm whether 
        int addChar = 1;

        if (currentUserPos > len){
            return -1;
        }

        // Do not add a character if the current buffer character is a space
        // and the current user string character is a whitespace character.
        if(!(buff[currentBufferPos - 1] == ' ' && ((user_str[currentUserPos] == ' ') || (user_str[currentUserPos] == '\t') || (user_str[currentUserPos] == '\n')))){

            // If char is a tab or nl, add a space to buff instead.
            if(user_str[currentUserPos] == '\t' || user_str[currentUserPos] == '\n'){
                buff[currentBufferPos] = ' ';
                currentBufferPos++;
            }

            // Otherwise, just add the character.
            else{
                buff[currentBufferPos] = user_str[currentUserPos];
                currentBufferPos++;
            }   
        }

        currentUserPos++;
    }

    while(currentBufferPos < len){
        buff[currentBufferPos] = '.';
        currentBufferPos++;
    }

    return currentUserPos + 1;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    return 0;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    
    // If argv[1] does not exist, that means there was no input given with the program
    // call. Thus, the program doesn't have anything to work with, and should quit with
    // exit code 1, telling the user the program format.
    
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        print_help();

        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below

    // Much like the previous case, or there are less than 3 arguments (including the program
    // call), that means that you are not telling the program what to do with your input string.
    // It cannot do anything, and must exit after telling the user the proper signature.

    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    // Allocate BUFFER_SZ bytes for the buff char pointer.
    buff = malloc(BUFFER_SZ);
    if(buff == NULL){
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);  
       //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE


