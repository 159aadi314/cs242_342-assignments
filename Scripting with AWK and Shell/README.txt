
Name: AADI AARYA CHANDRA
Roll No.- 200101002
CS242 Assignment-1 

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Q1
Files - Source code - 200101002_Assign01.awk, Input file - input_ques01.txt
Command to be typed into terminal (in present directory)- 
awk -f 200101002_Assign01.awk < input_ques01.txt 
Note: 1. Array in the source code is taken zero-indexed. 
2. In case the sum of the two column entries is equal, sorting is done based on 2nd column entry, if they are also equal, then according to name in alphabetical order

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Q2
Files - Source code - 200101002_Assign01.sh, Input file - input_ques02.txt
Commands to be typed into terminal (in present directory)- 
chmod +x 200101002_Assign01.sh
./200101002_Assign01.sh count length <input_ques02.txt (here count and length are integer values specified by user in their respective places and input_ques02.txt denotes file from which alphabet set is taken input)

Note: 1. After the above two commands, $1 will represent the value entered by user as count, $2 as length and then input will be redirected to input file of alphabet set (which in my case is currently input_ques02.txt . 
2. The array is 1-indexed as mentioned in comments of the source code file, unlike in question 1. So the first character in alphabet set in denoted by alphabet[1].
3. Please read source code with tab width set to 4 for better view of code.
4. Data validity checks have also been done for count (>1), length (>0) and alphabet set (see below).
5. Alphabet set is read from input file input_ques02.txt, and it can be modified.
6. I am assuming alphabet set is a continuous string of characters ranging in A-Z and a-z without any spaces in between characters, written in line 1 of file as given.
7. I am considering the case when alphabet set contains only one DISTINCT character as invalid if length>=count and prompting the user for the same. If length<count, I am giving a warning to user and recommending changing alphabet set but printing the required valid string also.
8. However, if alphabet set has repeated characters but there are more than one distinct characters, then it is considered a valid case and a valid string is generated.

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
