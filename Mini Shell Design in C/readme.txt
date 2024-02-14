Name: AADI AARYA CHANDRA

Roll Number: 200101002

Programming Language Used: C

Operating Systems Used: LINUX (Ubuntu 20.04.3 LTS)

Files in the folder: readme.txt 200101002.c history.txt

Instructions for executing shell:
Step1: Install required readline libraries: sudo apt-get install libreadline-dev. // VERY IMPORTANT STEP
Step2: sudo apt-get upgrade

Step3: Go to 200101002.c, go to first line of parse_command function and                     // VERY IMPORTANT STEP
change the path written inside fopen from "/home/aadi/Desktop/200101002/history.txt" 
to the ABSOLUTE path of my history.txt file (present in this folder) in your system. (line 171)

Step4: Repeat Step3 for first line of parse_redir_command function, first line of parse_piped function and for first line of main function. (lines 381, 799, 833 in VSCode or gedit) 
Step5: Compilation: gcc 200101002.c -L/usr/include -lreadline -o 200101002  (200101002.c and 200101002 denote filename)
Step6: Running the shell - ./200101002

On doing above, a welcome message appears with a colorful (blue and yellow) prompt which tell username, hostname, and present directory and now we can type commands.

Troubleshooting: 
1. Ensure that all libraries given at the beginning of c code are present in your system
2. Use the above exact commands to compile and run shell
3. Preferable use VSCode on Linux in case of problem on other editors.
4. If segmentation fault, ensure that the path of history.txt file has been changed in all 3 places according to step 3 and 4.

Important points to note about my shell - 

1. For any command (internal or external or env variable), arguments can only be space separated, 
   and cannot be joined by quotes. Hence, cd 'a b' will not consider a b as one argument, 
   instead it will consider 'a and b' as two separate arguments. Hence do not use arguments which have quotes in them.
   As a result, cd cannot change directory to directory whose name has spaces in it, even if you put quotes around it.
   Similarly, grep "c" doesn't work but grep c works.

2. Piping is only allowed for the following external commands- 
    printenv, echo, man,ls,cat, cal,which,wc,passwd,touch,cp, 
    mv,rm,rmdir,mkdir,sleep,clear,script,grep,chsh,date,chmod,more,less,alias,unalias,find.
    Internal Commands and printenv setenv getenv cannot be used in pipes and 
    no other external command other than those given above support piping in my shell
    To compensate for this, I have done something extra, I have implemented multi-level piping for above commands.
    So you can use upto 255 pipes in one command as long as all commands are one of those mentioned above

3. Using backspace may make prompt disappear sometimes, just press enter to get prompt back
4. I am using return 0 to mean error and return 1 to mean success (except in chdir where 0 means success)
5. Redirection has some syntax limitations, please see below manual point 5.
6. Assuming hostname, username cannot change during shell usage
7. Proper Indentation and commenting has been done, better viewed in vscode for linux, may not look good on gedit or other IDEs.
8. Do not run on windows.


Manual of the shell (Description of various features of shell):
1. Internal Commands - help, exit, echo, cd, history.
    a. help - Lists down all commands which can be used. 
                Syntax - help (no arguments after help)
    b. echo - Prints env variables and whatever is passed as argument to it
                Syntax - echo <whatever_is_to_be_printed> (if blank then newline printed)
                Another syntax - echo $name (or echo %name) - to print the value of env variable 'name', if it is defined by user or if its a global env variable
    c. cd -   Changes directory to what is mentioned after it
                Syntax - cd <directory_name>
                Other allowed variations - cd (blank) - changes to home directory
                                           cd ~ - changes to home directory
                                           cd .. - changes to parent directory of current directory
                The change in current directory is visible in the printed prompt.
                Cannot change to directory whose name has spaces in it.
    d. exit - exits the shell, but not the terminal
    e. history - prints commands with their index (1-based indexing) in the order in which they were executed in the current run of the shell
                 Syntax - history (blank) 
                 My shell erases history file before and after execution of the shell program only if we exit shell using exit command. if ctrl+c is used history will remain in the file
                 Repitition in history happens unlike in BASH
                 history always shows history of commands entered in this shell, even if directory changed (unlike bash)
    Internal commands do not support piping.
    Implemented by creating explicit functions and doing validation checks

2. Features related to environment variables (also are part of internal commands) - echo $var, echo %var, printenv, getenv, setenv, declaring using NAME=value
    All environment variables that are present in linux can be accessed from my shell.
    Moreover, local env variables (local to shell) can also be created in my shell.

    a. echo $var - prints the value of env variable var if it exists
    b. echo %var - same as a.
    c. printenv - syntaxes - i. printenv (blank) - prints all env variables, global as well as local
                             ii. printenv var (without $ or %) - prints the value of var if it exists as a global or local env variable.
    d. getenv - syntax - getenv var (without $ or %) - prints the value of var if it exists as a global or local env variable.
    e. Declaration of local env variables without using setenv - 
            syntax - NAME=VALUE (no spaces!) - Create a local env variable called NAME and sets its value to VALUE.
                                               Can be printed using echo getenv and setenv as described above.
    f. setenv - syntax - setenv NAME = VALUE (take care of spaces!) - same as e.
    Implemented by creating explicit functions and doing validation checks

3. External Linux Commands - 
    All external commands present in Linux can be used as long as there is no piping.
    If there is piping then only the ones mentioned above in important points can be used.
    Implemented using execvp and forking.
    Please see the above important points before manual for more information.

4. Piping - 
    upto 255 pipes are allowed as long as all commands are certain Linux external commands as listed above in important points.
    Implemented by creating pipes and forking and using file descriptors

5. Redirection - Supported redirections - 
    1. > (single) - single input redirection 
    2. < (single) - single output redirection
    3. >> (single) - single append redirection
    4. <, >> together (single) - input and append redirection
    5. >, < together (single) - input and output redirection
    Hence, more than one redirection of any particular type is not allowed and > and >> are not both allowed simultaneously (exactly like BASH).
    Syntax - command <normal_arguments> <redirections> 
    The above syntax has to be strictly followed - there must be spaces between commands, normal_arguments, redirection operators and filenames.
    Hence first type command then all the arguments for it and write all the redirections required only at the end.
    For example - 
    a. cat input.txt < input1.txt >> append.txt is allowed
    b. cat < input1.txt input.txt >> append.txt is not allowed since input.txt is a normal argument in between redirections.
    c. cat <input1.txt is not allowed since space should be there between < and input1.txt
    Redirection is supported for all commands.
    Redirection is implemented using forking and file descriptors and functions like open and creat to open/create files


