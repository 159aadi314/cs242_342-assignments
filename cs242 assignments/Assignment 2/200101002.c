#include <stdio.h>
#include <string.h>                              // please include all these libraries
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <curses.h>
#include <dirent.h>
#include <term.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"                    // colors defined for prompt
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"


char curr_dir[1000];                         // curr_dir stores current working directory, it may change only if cd is used. Declared as global variable
extern char **environ;                          // environ is external variable which gives us values of all env variables


int readInput(char* str)       // function to read input entered by used on command line
{
    char* buf;                              // declare a string buf

    buf = readline(NULL);              // use readline function to print prompt on screen and take the line entered by user as input into buf

    if (strlen(buf) != 0) {             // if the input is empty, we do nothing and return -1, otherwise we copy buf to str
        strcpy(str, buf);
        str[strlen(buf)] = '\0';
        return 1;
    }
    else{
        return 0;
    }
}

int help_cmd(char** args){                  // type help to get details of all commands allowed

    if(args[1] != NULL){                        // help <command> not allowed
        fprintf(stderr,"Help only lists all commands. Either arguments are invalid or see readme file for description of specific functions");
        return 0;
    }
    else{                                       // list of all commands implemented
        printf("Internal commands - cd, echo, history, pwd, exit, help\n");
        printf("External commands - all\n");
        printf("Env variables - all global envariables accessible, can declare local shell variables too\n");
        printf("Commands for env/shell variables: echo, printenv, setenv, getenv");
        return 1;
    }
}

int echo_cmd(char** args, int num_args){                            // implementation of echo as an internal command

    if(args[1] != NULL){                                    // if it is just echo , we do nothing, otherwise .. 

        for(int i = 1; i < num_args + 1; i++){

            if(args[i][0] == '$' || args[i][0] == '%'){             // using this if block, we check if argument is $name or %name form, 
                char *env = getenv(args[i] + 1);
                if(env != NULL){                                     // and if name is an env variable, we would print its value in place of it
                    args[i] = NULL;
                    args[i] = (char*)malloc(strlen(env));

                    strcpy(args[i],env);
                }
                else{
                    args[i][0] = '\0';
                }
            }
            if(i > 1){
                if(args[i][0] != '\0')                          // we print arguments one by one in for loop
                    printf(" %s",args[i]);
            }else{
                if(args[1][0] != '\0')
                    printf("%s",args[1]);
            }
        }
        printf("\n");
        return 2;                                           // return 2 is specifically for the echo case
    }
    else{
        return 1;
    }
}

int cd_cmd(char** args, char* arguments_str){                   // all variations in cd are allowed except directories with whitespaces in their names
    char* home = getenv("HOME");

    if(args[1] == NULL || strcmp(args[1],"~") == 0){
        chdir(home);                                            // in case we have cd ~ or just cd, we change directory to home
        getcwd(curr_dir,500);
        return 1;
    }
    else if(args[2] != NULL){                               // can't have more than one argument in cd
        fprintf(stderr,"%s: Invalid Syntax",arguments_str);
        return 0;
    }

    int d = chdir(args[1]);

    if(d == -1){                                                                // if chdir fails, we print error message to stderr
        fprintf(stderr,"%s: No such file or directory",arguments_str);
        return 0;
    }
    else{                                               // if chdir is successful we change variable storing our current directory to new value
        getcwd(curr_dir,500);
        return 1;
    }
}

int printenv_cmd(char** args){

    if(args[1] == NULL){                              // printenv without any arguments means printing all global env variables and also all shell variables in my shell (unlike bash)
        char **s = environ;                         // we use environ to get all env variables

        for(; *s; s++){
            
            printf("%s\n",*s);                          // print env variables in the format name=value line by line
        }
        return 1;
    }
    else if(args[2] == NULL){                               // if printenv <variable> is the command, then we print the value of that particular env variable 
            char* env = getenv(args[1]);
            if(env == NULL){
                fprintf(stderr,"Invalid Environment Variable");
                return 0; 
            }
            printf("%s",env);
            return 1;

    }else{                                                     // all other syntaxes of printenv are invalid
        fprintf(stderr,"Invalid Syntax. There should be no arguments or only 1 argument, ie the env variable to be printed.");
        return 0;
    }
}

int getenv_cmd(char** args){                    // getenv var prints var if is exists locally or globally

    if(args[1] == NULL){                // this means no variable has been specified

        fprintf(stderr,"Error: No variable specified for getenv");
        return 0;
    }  
    else if(args[2] != NULL){               // this means more than one argument which is not allowed

        fprintf(stderr,"Too many arguments for getenv");
        return 0;
    }
    else{
        char* env = getenv(args[1]);        // get the envariable using getenv
        
        if(env == NULL){
            fprintf(stderr,"Invalid Environment Variable");         // if var does not exist
            return 0; 
        }
        printf("%s",env);           // else print it
        return 1;
    }
}

int parse_command(char* str, int history_index){                                    // function to parse input command string (str), in case when there is no redirection or piping involved and then execute it also

    FILE* history_ptr = fopen("/home/aadi/Desktop/200101002/history.txt","a+");         // first we open history.txt file in append mode to write history of commands into it (specify path from your system here, instead of /home/aadi....)
    fseek(history_ptr,SEEK_END,0L);                                                 
    fprintf(history_ptr,"%-4d %s\n",history_index,str);                          // we append given command string str, to history file
    rewind(history_ptr);
    
    if(str == NULL){                                            // command string can't be empty, so return 0 in that case
        return 0;
    }
    int flag_success = 0;                                   // flag_success tells us whether command has been successfully executed or not. 1 if yes, 0 if no.

    char* whole_str = (char*)malloc(1000*sizeof(char));         // make a new variable whole_str to store str after removing initial spaces if any in str
    int j1 = 0;
    while(str[j1] == ' '){
        j1++;
    }
    str += j1;
    strcpy(whole_str,str);
    whole_str[strlen(str)] = '\0';                      // whole_str terminates at null character

    int i3 = 0;
    while(str[i3] != ' '&&str[i3] != '\0'){             // i3 is used to find the length of first argument, ie the command itself
        i3++;
    }

    char* command = (char*)malloc((i3 + 1)*sizeof(char));               // create variable command to store the first argument before whitespace of str
    strncpy(command,str,i3);
    command[i3] = '\0';

    int flag_empty_arg=0;
    str = str+i3;                                                 // flag_empty_arg tells us whether there are any more arguments after command
    if(str[0] == '\0'){
        flag_empty_arg = 1;
    }else{
        while(str[0] == ' '){
            str++;
        }
    }

    char* arguments_str = (char*)malloc(1000*sizeof(char));
    strcpy(arguments_str,str);                                    // arguments_str stores all arguments after the first one (which is the command) as a string
    arguments_str[strlen(str)] = '\0';

    char* delim = " ";                                        // we tokenise remaining str to get all arguments in an array of string called args
    char* token1 = strtok(str,delim);
    int i4 = 2;
    char* args[256];
    args[0] = command;
    args[1] = token1;

    while(token1 != NULL){
        token1 = strtok(NULL, delim);
        args[i4++] = token1;
    }
    int num_args = i4 - 2;                                // num_args tells how many arguments are there after the first one (command)

    
    int j = 0;                          // in case original str is of the form NAME=value (NO SPACES ALLOWED HERE!!), we say count is 1, otherwise it is 0. 
    int count=0;                        // if count is 1, we would later create a new shell env variable with value 'value'                       
    for(;j < strlen(command); j++){                          // another declaration method using setenv is given below
        if(command[j] == '='){
            count++;
            break;
        }
    }

    int found = 0;                              // found variable tells if command is one the below ones. These are all the internal commands that have been implemented, and the env variable related commands
    char* int_comm[10] = {"help", "quit", "exit", "history","cd","echo","printenv","setenv","getenv","pwd"};
    for(int i = 0;i < 10;i++){
        if(strcmp(command,int_comm[i]) == 0){       // check is command is one of the above ones
            found = 1;break;
        }
    }

    if(found||count){                   // if command is internal or we are declaring shell variable (in the form NAME=value)

            if(count != 0){                     // setting env variables if str was NAME=value
                if(j == 0){                                           // WARNING! dont set any global envariables using this shell
                    fprintf(stderr,"Invalid environment variable name");                // j == 0 means the name of env variable to be declared was not specified(see previous loop where count was set)
                    flag_success = 0;
                }
                else if(args[1] != NULL){
                    fprintf(stderr,"Invalid Syntax for declaring envariable");          // anything after NAME=value means syntax error
                    flag_success = 0;
                }
                else{
                    char *name = (char*)malloc(1000*sizeof(char));              // otherwise we assign the string before FIRST = sign as name of env variable and the string after it as its value
                    strncpy(name,command,j);
                    char *value = (char*)malloc(1000*sizeof(char));;
                    strcpy(value,command + j + 1);
                    setenv(name,value,1);                               // then we use setenv to set name variable to value and flag_success as 1
                    flag_success = 1;

                }
            }
            else if(strcmp(command,"exit") == 0){        // if the command typed is exit, we exit the shell, but terminal remains open
                exit(0);
            }
            else if(strcmp(command,"quit") == 0){                           // quit is not there, but it should be similar to exit, so this message
                fprintf(stderr,"Command quit not found. Did you mean: exit ?");
                flag_success=0;
            }
            else if(strcmp(command,"help") == 0){        // if the command typed is help, we display all the builtin commands and supported environmental variables
                
                if(help_cmd(args)){                     // flag_success is 1 if help command ran successfully, otherwise 0;
                    flag_success = 1;
                }
                else{
                    flag_success = 0;
                }
            }
            
            else if(strcmp(command,"echo") == 0){                    // implementation of echo: it prints anything as it is. However is $e is there is string where e is and env variable, then it prints $e in place of env variable
                int d;
                if((d = echo_cmd(args, num_args)))
                    flag_success = d;
            }
            else if(strcmp(command,"pwd") == 0){                    // pwd prints the current directory on screen
                printf("%s",curr_dir);
                flag_success = 1;
            }
            else if(strcmp(command,"cd") == 0){                     // implementation of cd: 1. cd .. takes you to parent directory of current directory
                if(cd_cmd(args,arguments_str) == 1){            // 2. cd ~ or just cd takes you to home directory
                    flag_success = 1;                           // cd <path> takes you to path
                }else{                                          // all other syntaxes of cd are not allowed, including where directory name has whitespaces in it (even with quotes this case does not work)
                    flag_success = 0;                           // finally cd changes curr_dir to current directory and that is printed in prompt
                }
            }
            else if(strcmp(command,"history") == 0){                // if just history is given, we use history pointer, set it to beginning of history.   txt and print out the history in stdout    
                if(args[1] == NULL){
                    int c;
                    while((c = fgetc(history_ptr)) != EOF){
                        printf("%c",c);
                    }
                    flag_success = 1;
                }else{                                          // all other syntaxes are considered invalid
                    fprintf(stderr,"history: Invalid Syntax");
                    flag_success = 0;
                }
            }
            else if(strcmp(command,"printenv")==0||strcmp(command,"getenv")==0){                 // to print the env variables, individually or all, local to shell env, or global
                
                if(strcmp(command,"getenv")==0){                           // getenv
                    if(getenv_cmd(args)){
                        flag_success=1;
                    }else{
                        flag_success=0;
                    }
                }
                else if(printenv_cmd(args))
                    flag_success = 1;                                                                     // syntaxes allowed are: 1. printenv and 2. printenv <command>
                else
                    flag_success = 0;
            }
            else if(strcmp(command,"setenv") == 0){            
                if(args[1] == NULL){                              // another way to set env variables. SPACED REQUIRED IN SYNTAX. can set the global env variables but only temporarily
                    
                    fprintf(stderr,"Error: specify environment variable's name");
                    flag_success= 0;
                }else if(args[4] != NULL || strcmp(args[2],"=") != 0){             // syntax is setenv NAME = value with spaces! all other syntaxes are considered invalid
                    
                    fprintf(stderr,"Invalid Syntax for setenv");
                    flag_success = 0;
                }
                else{   

                    setenv(args[1],args[3],1);                      // if syntax is valid set name (stored in arg[1]) to value (stored in arg[3]; args[2] is =)
                    flag_success = 1;
                }
            }
    }
    else{                                       // if it is not one of the env variable commands or internal commands, then it is either an external command or invalid command
        pid_t p;                            // we create a process id variable p and fork the current process. p stores process id of child process
        p = fork();

        if (p == -1){                         // p = -1 denotes forking was unsuccessful

            fprintf(stderr,"Forking failure");
            flag_success = 0;
    
        }
        else if(p == 0){                                    // p = 0 denotes child process. In child process we run execvp function, with arguments give as the command (args[0]),
            
            if (execvp(args[0], args) < 0) {                // and an array of the arguments including the command (args)
                
                fprintf(stderr,"%s: Invalid Command or command could not be executed", args[0]);            // when execvp unsuccessful (returns negative value)
                flag_success = 0;
            }
            else{                                                               
                flag_success = 1;               // when execvp successful
            }
            exit(0);                // exit child process
        }
        
        else{                       // parent process
            wait(NULL);                 // waiting for child to finish..
        }
    }

    free(command);                      // free all dynamically allocated pointers
    str[0]='\0';
    arguments_str[0]='\0';
    free(arguments_str);
    free(whole_str);

    return flag_success;                // return whether command was successfully run or not
}

int parse_redir_command(char* str, int history_index){          // function to parse input command string (str), in case when there is redirection involved and then execute it also
        // IMPORTANT NOTE: SYNTAX REQUIRED FOR PROPER REDIRECTION: <command> <normal_arguments> <redirections>. redirections should NOT be put between other normal arguments, all only at the end
    
    FILE* history_ptr = fopen("/home/aadi/Desktop/200101002/history.txt","a+");             // first we open history.txt file (specify path from your system here, instead of /home/aadi....)
    fseek(history_ptr,SEEK_END,0L);
    fprintf(history_ptr,"%-4d %s\n",history_index,str);                      // we append given command string str, to history file
    rewind(history_ptr);

    int in = 0;                                     // declare three integers in out and append as file descriptors to be used for 
    int append = 1;                                 // input, output, and append redirection respectively
    int out = 1;

    char* complete_str = (char*)malloc(1000*sizeof(char));              // first store the complete str in complete_str, since str is going to be tokenised
    strcpy(complete_str,str);

    int i1 = 0;
    for(;i1 < strlen(str); i1++){                           // check for first occurrence of redirection, since we will split str into normal command and redirection part

        if(str[i1] == '<' || (i1 < strlen(str) - 1 && str[i1] == '>' && str[i1 + 1] == '>')){       
            break;
        }
        else if(str[i1] == '>'){
            break;
        }
    }                                                       // after running for loop i1 tells us where is the first redirection operator

    char* redir_str = (char*) malloc(1000*sizeof(char));                // all portion of str after the first redirection operator is put into redir_str
    strcpy(redir_str,str + i1);
    str[i1] = '\0';                                     // terminate str right here so that only the part of str before redirection is stored here now

    char* delim = " ";                                  // tokenise redir_str with spaces as delimiters
    char* token = strtok(redir_str,delim);
    int i2 = 1;
    char* redir_args[256];                              // create char** redir_args to store the tokens of redir_str
    redir_args[0] = token;
    while(token != NULL){
        token = strtok(NULL, delim);
        redir_args[i2++] = token;
    }
    int num_redir_args = i2 - 1;

    if(redir_args[0] != NULL){                              // this tells if any redirection is present or not  

        for(int i = 0; i < num_redir_args - 1; i++){                                // iterate through tokens of redir_args                    

            if(strcmp(redir_args[i],">>") == 0){                                // if a particular token is >> then the next token is the file to be opened in append mode.

                append = open(redir_args[i + 1],O_WRONLY | O_APPEND,0666);              // open file given as next token in append mode and assign append file descriptor to it

                if(append < 0){
                    fprintf(stderr,"Could not open output file %s",redir_args[i + 1]);          // if file could not be opened
                    return 0;
                }
                i++;                                    // skip one iteration of for loop since next one has already been taken care of above
            }
            else if(strcmp(redir_args[i],">") == 0){                // if a particular token is > then the next token is the file to be opened in output mode.

                out = creat(redir_args[i + 1],0666);                // open/create file given as next token in output mode and assign out file descriptor to it

                if(out < 0){
                    fprintf(stderr,"Could not open output file %s",redir_args[i + 1]);              // if file could not be opened
                    return 0;
                }
                i++;                                // skip one iteration of for loop since next one has already been taken care of above
            }
            else if(strcmp(redir_args[i],"<") == 0){                // if a particular token is < then the next token is the file to be opened in input mode.

                in = open(redir_args[i+1],O_RDONLY,0666);               // open file given as next token in input mode and assign in file descriptor to it

                if(in < 0){
                    fprintf(stderr,"Could not open input file %s",redir_args[i + 1]);               // if file could not be opened
                    return 0;
                }
                i++;                            // skip one iteration of for loop since next one has already been taken care of above
            }
        }
    }
    
    if(str == NULL){                 // command string can't be empty, so return 0 in that case
        return 0;
    }
    int flag_success = 0;                // flag_success tells us whether command has been successfully executed or not. 1 if yes, 0 if no.

    char* whole_str = (char*)malloc(1000*sizeof(char));     // make a new variable whole_str to store str after removing initial spaces if any in str and does not contain any redirections
    int j1 = 0;
    while(str[j1] == ' '){
        j1++;
    }
    str += j1;
    strcpy(whole_str,str);
    whole_str[strlen(str)] = '\0';                  // whole_str terminates at null character

    int i3 = 0;
    while(str[i3] != ' '&&str[i3] != '\0'){          // i3 is used to find the length of first argument, ie the command itself
        i3++;
    }

    char* command = (char*)malloc((i3 + 1)*sizeof(char));       // create variable command to store the first argument before whitespace of str
    strncpy(command,str,i3);
    command[i3] = '\0';

    int flag_empty_arg = 0;               // flag_empty_arg tells us whether there are any more arguments after command
    str += i3;
    if(str[0] == '\0'){
        flag_empty_arg = 1;
    }else{
        while(str[0] == ' '){
            str++;
        }
    }

    char* arguments_str = (char*)malloc(1000*sizeof(char));         // arguments_str stores all arguments after the first one (which is the command) as a string
    strcpy(arguments_str,str);
    arguments_str[strlen(str)] = '\0';

    char* token1 = strtok(str,delim);               // we tokenise remaining str to get all arguments in an array of string called args
    int i4 = 2;
    char* args[256];
    args[0] = command;
    args[1] = token1;
    while(token1 != NULL){
        token1 = strtok(NULL,delim);
        args[i4++] = token1;
    }

    int num_args = i4-2;                        // num_args tells how many arguments are there after the first one (command)

    int j = 0;                          // in case original str is of the form NAME=value (NO SPACES ALLOWED HERE!!), we say count is 1, otherwise it is 0. 
    int count=0;                        // if count is 1, we would later create a new shell env variable with value 'value'                       
    for(;j < strlen(command); j++){                          // another declaration method using setenv is given below
        if(command[j] == '='){
            count++;
            break;
        }
    }

    int found = 0;                              // found variable tells if command is one the below ones. These are all the internal commands that have been implemented, and the env variable related commands

    char* int_comm[10] = {"help", "quit", "exit", "history","cd","echo","printenv","setenv","getenv","pwd"};
    for(int i = 0;i < 10;i++){
        if(strcmp(command,int_comm[i]) == 0){       // check is command is one of the above ones
            found = 1;break;
        }
    }

    if(found || count){                 // if command is internal or we are declaring shell variable (in the form NAME=value)
        pid_t p;
        p = fork(); 
        if (p == -1){
            fprintf(stderr,"Forking failure");
            flag_success = 0;
        }
        else if(p == 0){                    // child process
            
            if(append != 1){                // if command has append redirection, we duplicate append and redirect stdout to append using dup2
                dup2(append,1);
            }
            if(out != 1){                   // if command has output redirection, we duplicate output and redirect stdout to output using dup2
                dup2(out,1);
            }
            if(in != 0){                     // if command has input redirection, we duplicate input and redirect stdin to input using dup2
                dup2(in,0);
            }
                                                    // now we execute internal commands!
            if(count != 0){                     // setting env variables if str was NAME=value
                if(j == 0){                                           // WARNING! dont set any global envariables using this shell
                    fprintf(stderr,"Invalid environment variable name");                // j == 0 means the name of env variable to be declared was not specified(see previous loop where count was set)
                    flag_success = 0;
                }
                else if(args[1] != NULL){
                    fprintf(stderr,"Invalid Syntax for declaring envariable");          // anything after NAME=value means syntax error
                    flag_success = 0;
                }
                else{
                    char *name = (char*)malloc(1000*sizeof(char));              // otherwise we assign the string before FIRST = sign as name of env variable and the string after it as its value
                    strncpy(name,command,j);
                    char *value = (char*)malloc(1000*sizeof(char));;
                    strcpy(value,command + j + 1);
                    setenv(name,value,1);                               // then we use setenv to set name variable to value and flag_success as 1
                    flag_success = 1;

                }
            }
            else if(strcmp(command,"exit") == 0){        // if the command typed is exit, we exit the shell, but terminal remains open
                FILE* clearing_fp1 = fopen("/home/aadi/Desktop/200101002/history.txt","w");              // clear history file, before exiting
                fclose(clearing_fp1);
                exit(0);
            }
            else if(strcmp(command,"quit") == 0){                           // quit is not there, but it should be similar to exit, so this message
                fprintf(stderr,"Command quit not found. Did you mean: exit ?");
                flag_success=0;
            }
            else if(strcmp(command,"help") == 0){        // if the command typed is help, we display all the builtin commands and supported environmental variables
                
                if(help_cmd(args)){                     // flag_success is 1 if help command ran successfully, otherwise 0;
                    flag_success = 1;
                }
                else{
                    flag_success = 0;
                }
            }
            
            else if(strcmp(command,"echo") == 0){                    // implementation of echo: it prints anything as it is. However is $e is there is string where e is and env variable, then it prints $e in place of env variable
                int d;
                if((d = echo_cmd(args, num_args)))
                    flag_success = d;
            }
            else if(strcmp(command,"pwd") == 0){                    // pwd prints the current directory on screen
                printf("%s",curr_dir);
                flag_success = 1;
            }
            else if(strcmp(command,"cd") == 0){                     // implementation of cd: 1. cd .. takes you to parent directory of current directory
                if(cd_cmd(args,arguments_str) == 1){            // 2. cd ~ or just cd takes you to home directory
                    flag_success = 1;                           // cd <path> takes you to path
                }else{                                          // all other syntaxes of cd are not allowed, including where directory name has whitespaces in it (even with quotes this case does not work)
                    flag_success = 0;                           // finally cd changes curr_dir to current directory and that is printed in prompt
                }
            }
            else if(strcmp(command,"history") == 0){                // if just history is given, we use history pointer, set it to beginning of history.   txt and print out the history in stdout    
                if(args[1] == NULL){
                    int c;
                    while((c = fgetc(history_ptr)) != EOF){
                        printf("%c",c);
                    }
                    flag_success = 1;
                }else{                                          // all other syntaxes are considered invalid
                    fprintf(stderr,"history: Invalid Syntax");
                    flag_success = 0;
                }
            }
            else if(strcmp(command,"printenv")==0||strcmp(command,"getenv")==0){                 // to print the env variables, individually or all, local to shell env, or global
                
                if(strcmp(command,"getenv")==0){                           // getenv var returns value of variable if it exists as local or global
                    if(getenv_cmd(args)){
                        flag_success=1;
                    }else{
                        flag_success=0;
                    }
                }
                if(printenv_cmd(args))
                    flag_success = 1;                                                                     // syntaxes allowed are: 1. printenv and 2. printenv <command>
                else
                    flag_success = 0;
            }
            else if(strcmp(command,"setenv") == 0){            
                if(args[1] == NULL){                              // another way to set env variables. SPACED REQUIRED IN SYNTAX. can set the global env variables but only temporarily
                    
                    fprintf(stderr,"Error: specify environment variable's name");
                    flag_success= 0;
                }else if(args[4] != NULL || strcmp(args[2],"=") != 0){             // syntax is setenv NAME = value with spaces! all other syntaxes are considered invalid
                    
                    fprintf(stderr,"Invalid Syntax for setenv");
                    flag_success = 0;
                }
                else{   
                    setenv(args[1],args[3],1);                      // if syntax is valid set name (stored in arg[1]) to value (stored in arg[3]; args[2] is =)
                    flag_success = 1;
                }
            }
            close(in);              // close in file descriptor, other 2 do not need closing
            exit(0);                // exit child process
        }
        else{                       // parent process
            wait(NULL);             // waiting for child to finish..
        }
    }
    else{                               // if it is not one of the env variable commands or internal commands, then it is either an external command or invalid command
        pid_t p;                         
        p = fork();                     // we create a process id variable p and fork the current process. p stores process id of child process

        if (p == -1){           // p = -1 denotes forking was unsuccessful

            fprintf(stderr,"Forking failure");
            flag_success = 0;
        }
        else if(p == 0){                            // p = 0 denotes child process. In child process we run execvp function, with arguments give as the command (args[0]),
            
            if(append != 1){                    // if command has append redirection, we duplicate append and redirect stdout to append using dup2
                dup2(append,1);
            }
            if(out != 1){           // if command has output redirection, we duplicate output and redirect stdout to output using dup2
                dup2(out,1);
            }
            if(in!=0){              // if command has input redirection, we duplicate input and redirect stdin to input using dup2
                dup2(in,0);
            }
            if (execvp(args[0], args) < 0) {                              // execute commands using execvp
                
                fprintf(stderr,"%s : Invalid Command or could not execute command",args[0]);            // when execvp unsuccessful
                flag_success = 0;
            }
            else{
                flag_success = 1;                               // when execvp successful
            }
            close(in);                  // close the in file descriptors, other 2 do not need closing
            exit(0);                        // exit child process
        }
        else{                  // parent process

            wait(NULL);         // waiting for child process to finish...

        }
    }
    free(command);
    str[0] = '\0';
    arguments_str[0] = '\0';
    free(arguments_str);            // free all dynamically allocated pointers
    free(whole_str);
    free(complete_str);

    return flag_success;            // return whether command was successfully run or not
}

int exec_pipe (int in, int out, char* this_command){                // function to execute this_command, reading from in and writing to out. WARNING: piping not supported on certain commands, check readme file for the same
    
    if(this_command==NULL){                         // this_command can't be null, if it is return 0;
        fprintf(stderr,"Blank Command");
        return 0;
    }

    pid_t pid;                              // create a process id variable pid for 

    int j1 = 0;                                                                 // store this_command into a new variable whole_str, after removing any extra whitespaces in the beginning
    char* whole_str = (char*)malloc(1000*sizeof(char));                             // similar to parse_command function, see better explanation there
    while(this_command[j1] == ' '){
        j1++;
    }
    this_command += j1;
    strcpy(whole_str, this_command);
    whole_str[strlen(this_command)] = '\0';

    int i = 0;                                                          // finding the command in this_command string and storing it in variable command
    while(this_command[i] != ' ' && this_command[i] != '\0'){
        i++;                                                            // similar to parse_command function, see better explanation there
    }
    char* command = (char*)malloc((i + 1)*sizeof(char));
    strncpy(command,this_command,i);                                    // similar to parse_command function, see better explanation there
    command[i] = '\0';

    int found = 0;              // found is 1 if command in this_command string is an external command out of the below 27 ones, otherwise 0
    char* ext_comm[27] = {"printenv","echo","man","ls","cat","cal","which","wc","passwd","touch","cp","mv","rm","rmdir","mkdir","sleep","clear","script","grep","chsh","date","chmod","more","less","alias","unalias","find"};
    for(int i = 0; i < 27; i++){                    // finding if command in this_command string is an external command out of the above 27 ones
        if(strcmp(command,ext_comm[i]) == 0){
            found = 1;
            break;
        }
    }
    if(found == 0){                                   // if command is not one of above, we print error message
        fprintf(stderr,"%s: command not found",command);
        return 0;
    }
    else if(found == 1){                                        // if found, we first tokenise this_command and create an array args to store the arguments of the command
        
        char* delim = " ";
        char* token = strtok(this_command, delim);               // similar to parse_command function, see better explanation there
        int i1 = 1;
        char* args[256];
        args[0] = token;
        while(token != NULL){
            token = strtok(NULL, delim);
            args[i1++] = token;
        }

        pid = fork();                           // now we fork given process into child process and parent process, and we execute given command in child process

        if (pid == -1){                         // pid = -1 means forking was unsuccessful
            return 0;
        }
        else if(pid == 0){                  // pid = 0 means child process
            
            if(in != 0){                    // first we redirect stdin and stdout to in and out respectively and close in and out
                dup2(in, 0);
                close (in);
            }
            if(out != 1){
                dup2 (out, 1);
                close(out);
            }
            if(execvp(args[0],args)<0){                         // execute command with arguments using execvp
                return 0;                               // if execvp unsuccessful
            }
            return 1;                           // if execvp unsucessful
        }
        else{                                                               // parent process
            wait(NULL);                     // waiting for child to finish
        }
    }
}


int fork_pipes (int num_commands, char** commands){                     // function to create pipes. WARNING: piping not supported on certain commands, check readme file for the same

  int i;                    // i is iterator for for loop which runs and does the interprocess communication through making pipes

  int in, fd [2];               // in denotes file descriptor of read end of pipe form  where the current command should read. fd stores file descriptors of current pipe, fd[0] - read end, fd[1] - write end        
  in = 0;                                  // The first process should get its input from the 0, ie stdin

  for (i = 0; i < num_commands - 1; i++){                           // for loop which runs and does the interprocess communication through making pipes

      if(pipe (fd) < 0){                        // we create a pipe with file descriptors stored in fd. If unsucessfull, we print error message and return 0;
          fprintf(stderr,"piping error");
          return 0;
      }
      int d = exec_pipe (in, fd [1], commands[i]);                 // exec_pipe function executes a command and takes input from first argument and sends output to second argument
                                                                  // in ith iteration we execute ith command, ie commands[i], it should take input from in, which is read end of previous pipe or stdin, and it should write to fd[1], write end of current pipe
      if(d == 0){                                       // exec_pipe return 0 if error, 1 if successful
          return 0;
      }
      close (fd [1]);                                       // now we dont need write end of current pipe so we close it
      in = fd [0];                                         // in is updated to read end of current pipe
    }
  if(exec_pipe(in,1,commands[i]) == 0){                     // after the last pipe, the last command has to be executed out of the for loop, with reading from last pipe's read end and writing to stdout, ie 1
    return 0;                                       // return 0 if unsuccessful
  }
    return 1;                                       // return 1 if successful
}



int parse_piped_command(char *str,int history_index){               // command to parse commands with pipes. WARNING: piping not supported on certain commands, check readme file for the same

    FILE* history_ptr = fopen("/home/aadi/Desktop/200101002/history.txt","a+");         // first we open history.txt file in append mode to write history of commands into it (specify path from your system here, instead of /home/aadi....)
    fseek(history_ptr,SEEK_END,0L);                                                 
    fprintf(history_ptr,"%-4d %s\n",history_index,str);                          // we append given command string str, to history file
    rewind(history_ptr);

    char* whole_str = (char*)malloc(1000*sizeof(char));             // whole_str stores the whole command string str, after removing any spaces if any
    int j1 = 0;                     
    while(str[j1] == ' '){  
        j1++;
    }
    str += j1;                      // j1 denotes first non-whitespace character of str
    strcpy(whole_str,str);                  

    char* delim = "|";                      // we use pipe as delimiter to tokenise the string into an array of commands 'commands'
    char* token = strtok(str,delim);
    int i = 1;
    char* commands[256];
    commands[0] = token;
    while(token!=NULL){
        token = strtok(NULL,delim);
        commands[i++] = token;
    }
    int num_commands = i - 1;                       // tells number of commands to be executed

    if(fork_pipes(num_commands,commands) == 0){                 // after parsing we pass commands array along with its size to fork_pipes function which created pipes and sets up reading and writing ends
        return 0;                                       // if fork_pipes is unsuccessful, return 0;
    }
    else{
        return 1;               // otherwise return 1;
    }
}

int main(){                             // main

    FILE* clearing_fp = fopen("/home/aadi/Desktop/200101002/history.txt","w");              // first we open history.txt file in write mode to clear the file up (specify path from your system here, instead of /home/aadi....)
    fclose(clearing_fp);                                                                    // then close it

    
    int len = 1000;              // maximum length allowed for command. PLEASE FOLLOW THIS LIMIT
    char com[len];              // com string contains what user enters as input till pressing enter

    getcwd(curr_dir,500);            // put current directory path into global variable curr_dir. It can change only by cd
    
    char host[1000];                 // host stores hostname
    gethostname(host,1000);

    int history_index = 0;              // history_index stores the number of the current command being executed in current run of the shell, this is printed along command in history file (in history file it will be 1-index as in bash). it resets after you exit the shell

    char* user = getlogin();        // user stores username, ASSUMING it never changes during execution
    
    printf("Welcome to AADI'S SHELL\nType help to list all builtin commands executed by me\n\n");                   // welcome message

    while(1){                                // while loop runs till exit command or ctrl+c is typed. if ctrl+c, then shell closes, terminal still remains open

        printf(YEL "%s@%s:" BLU "%s"WHT"$ ",user,host,curr_dir);         // printing prompt everytime, with colors
        
        if(readInput(com)){          // read input into com and prompt the path and if the read input is not empty, we go further

            history_index++;                                            // for all commands

            int length_com = strlen(com);               // to store length of command string

            int flag_pipe = 0;                     // flag whose value = 0 means command has no pipes, and 1 means it does
            int flag_redir_i = 0;       // flag whose value = 0 means command does not have input redirection operations, otherwise tells how many input redirections there are
            int flag_redir_o = 0;       // flag whose value = 0 means command does not have output redirection operations, otherwise tells how many output redirections there are
            int flag_redir_a = 0;       // flag whose value = 0 means command does not have append redirection operations, otherwise tells how many append redirections there are
            int flag_redir = 0;     // flag whose value = 0 means command does not have redirection operations, and 1 means it does

            for(int i = 0; i < length_com; i++){            // iterate through command string to determine whether it has
                                                            // piping or redirection or not. if redirection, then which type
                if(com[i] == '|')
                {
                    flag_pipe = 1;                //  | denotes pipe
                }

                if(com[i] == '<' || (i < length_com - 1 && com[i] == '>' && com[i + 1] == '>'))
                {
                    flag_redir = 1;

                    if(com[i] == '<'){
                        flag_redir_i++;                         // if redirection is < it is input redirection
                    }
                    else{
                        flag_redir_a++;                 // if redirection is >> it is append redirection
                    }
                }else if(i > 0 && com[i] == '>'&& (i < length_com-1) && com[i+1] != '>'&& com[ i - 1] != '>'){              // if redirection is > it is output redirection
                    flag_redir = 1;
                    flag_redir_o++;
                }
            }
            if(flag_pipe && flag_redir){                    // my shell does not support both piping and redirection simultaneously
                printf("Both piping and redirection are not simultaneously supported");
            }

            else if(flag_pipe){                             // if there is piping, we go to function parse_piped_command. WARNING: piping not supported on certain commands, check readme file for the same

                parse_piped_command(com,history_index);             // return 1 if successful, otherwise 0

                printf("\n");
            }

            else if(flag_redir_o && flag_redir_a){                                // if both  > and >> are there. it is not supported in BASH. Neither is it supported in my shell
                printf("Both > and >> are not simultaneously supported\n");
            }

            else if(flag_redir_a >= 2 || flag_redir_i >= 2 || flag_redir_o >= 2){           // we can't have two or more redirections of same type in my shell
                printf("Two files can't be opened in same mode\n");
            }

            else if(flag_redir==1){                         // all other cases of redirection are valid

                int d;
                d = parse_redir_command(com,history_index);               // we parse and execute command with redirections in this function,  return 1 or 2 if successful otherwise 0
                printf("\n");
            }
            else{                       // if there are no redirections and no piping, we use parse_command to parse it and execute it
                int d;
                d = parse_command(com,history_index);               // return 1 or 2 if successful, otherwise 0

                if(d != 2){                       // d = 2 denotes the case when command is echo (blank), and in that case we have to print a newline
                    printf("\n");
                }
            }
        }
        else{
            printf("\n");                   // if readline couldn't read anything, we just print a newline and a new prompt
        }
    }
    return 0;                                       // end of main
}
