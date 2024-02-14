CS242 Assignment 3
Name - AADI AARYA CHANDRA
Roll No. - 200101002

Files in zip folder 200101002.zip: readme.txt 3 folders: q1: 200101002.c, Makefile
                                                         q2: 200101002.sh
                                                         q3: 200101002.pl, dictionary_words.txt, linux_dictionary.txt, input.txt, output.txt
(please don't change names of any of the above files) (for q2, archive file and source files can be given any name)
--------------

Question 1:
Files: 200101002.c (c file containing program), Makefile (to compile and run C file)

Commands to be executed to run program:
Step 1: sudo apt install make (if make is not installed), sudo apt install gcc (if gcc not installed)
Step 1': (OPTIONAL, do this step ONLY if step 2 doesn't work without it) type this command in terminal: chmod 777 -R <path of Makefile's parent directory>
Step 2: type make in terminal
Step 3: 3 self-explanatory prompts will come in series, their description is mentioned in comments of c file and makefile. 
        After answering the prompts, the desired result will be shown.
Step 4: type make clean in terminal to remove executable file 200101002

Important Points to be noted-
1. Ensure that make clean command is used to remove executable file before using make again. 
   Otherwise it will show "make: '200101002' is up to date."
2. All the asked arguments (id, name of variable and value of variable (value wont be asked if id is 0)) have to be specified, 
   and cannot be left empty, otherwise error message of too few arguments will come.
3. Working of my program - User is asked to specify (using id) whether it is required to modify env variable or retain its original value.
                           If variable of that name does not exist or name is not specified or id is not specified, we print error message.
                           else if id is 0, it means it should retain original value. It prints the variable name with its retained value.
                           else if id is 1, it means it will modify the variable value, and print variable name with old and new values.
                           else if id is any other value other than 0 or 1, error message is printed.
4. The values of the environment variables as modified (if so) in this program, 
   are modified only within context of the C file, as evident from the printed results.
   These changes are lost when we exit the C program or the shell instance in which it is run.
5. Ensure that version of GNU make is 3.82 or later.

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Question 2:
Files: 200101002.sh (shell script of tar replica)

Command to be executed to run program:
Step 1: chmod +x 200101002.sh
Step 2: ./200101002.sh tar [options string] [destination] [sources] (see below for explanation)

Important Points to be noted:
1. Syntax of command: Options string can only be one of the following: cvf (or any permutation of it like vcf, cfv etc)
                                                                       cf (-do-)
                                                                       xvf (-do-)
                                                                       xf (-do-)
                                                                       tvf (-do-)
                                                                       tf (-do-)
                                                                       rvf (-do-)
                                                                       rf (-do-)
    Clearly, f option is must, and v option may/may not be there
    All other options strings ("-cvf", "avf", etc) are considered invalid
    Destination is the path to the archive file. if options are xvf, xf, tvf, tf, rvf, or rf 
    then it must be a txt file in some directory 
    (eg: /home/archive.txt if home is not current dir, or ./archive.txt, or just archive.txt if it's in current directory)
    For the other 2 options, same format must be followed but the archive file may not exist, it will be created/overwritten.
    Destination can be "-" also, meaning that the contents of archive created will be printed on stdout 
    (this is allowed in actual tar command also but with piping to cat; here piping with cat is not needed)
    Sources are the files in some directories (eg /home/file.txt or 3.jpg) which are to be archived. 
    Can be multiple in number but all must exist. Cannot be "-", 
2. Destination must be a txt file, sources can be any file having text or image also (not checked for video files)
3. Error messages are customised, not exactly same as tar
4. Hidden files of the name .archive_file_name.txt are created to store metadata of archive file, 
   user can delete them after evaluating this question, but not before as it is needed for extraction.
5. cvf/cf can create an archive file only in the current directory, directories can't be passed as sources but files in other directories as sources are allowed.
6. tvf/tf show table of contents about the given archive file either verbosely (if v) or just a list
7. rvf/rf appends files to an existing archive
8. xvf/xf extracts given archive exactly in the locations where they were stored before archiving. 
   This is done because tar shows unusual behaviour involving directories in this scenario 
   and we were told not to deal with the directories issues in this assignment.
9. metadata of any file that is archived includes its name, its details (permissions, size, etc) found using ls -l option, 
   and the range of bytes it covers in the archive file (head: where its data starts, tail: where it ends).
   Name and details of the file are used as separator information, printed before contents of that file in the archive file (slightly different from bash, but similar)
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Question 3:
Files: input.txt (where text is to be written), output.txt (where text will be copied from input.txt after making corrections specified by user in terminal), 
       200101002.pl (PERL script), linux_dictionary.txt (a dictionary of linux commands and options), 
       dictionary_words (a dictionary of normal English words)

Steps to run program:
1. Install cpan on your system: cpan App::cpanminus (then confirm with yes then enter sudo when prompted for 'approach') (may need to enter your login password)
2. Install Text::Fuzzy module of cpan: cpanm --sudo Text::Fuzzy (would need to enter your login password)
3. Write text (with typos) in input.txt
4. Run perl script: perl 200101002.pl

Important points to be noted:
1. The text is to be written in the input.txt file provided, its name should not be changed. 
2. It is advisable NOT to have punctuation marks in text as it will be considered as typo.
3. Any number of spaces/tabs in text will be reduced to single space in the final output. The text will be case sensitive.
4. Upon running script, you will be prompted appropriately as soon as any typo is detected in the text. 
   The words of the line till the typo, line number of typo, along with word suggestions and linux command suggestions are then provided.
   A self explanatory message tells you to press some keys to choose to retain the typo, 
   or select a word suggestion to replace it, or select a linux command suggestion to replace it.
   This process is repeated for every typo and finally, 
   the number of typos detected and number of corrections made is printed on terminal, and corrected text is printed in output.txt
5. The suggestions provided are sorted in increasing order of their edit distances from typo.
6. threshold of edit distance is set at 2, because given the large size of dictionary, 
   edit distance of 3 or more gives too many suggestions, 
   and most commonly occurring typos are within the threshold of edit distance of 2
7. if needed, user can increase this threshold by just changing the value in line 14, without affecting rest of the program
8. past tense of words, future tense, etc are also considered typos if they are not present in the dictionary