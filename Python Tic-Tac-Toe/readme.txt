CS242 Assignment 4
Name - AADI AARYA CHANDRA
Roll No. - 200101002

Files in zip folder 200101002.zip: readme.txt, 2 folders: q1: 200101002.py
                                                          q2: 200101002_page1.tex, 200101002_page2.tex

----------------------------------------------------------------------------------------------------------------------------
Question 1: folder - q1, files - 200101002.py

Instructions for running program:
1. Check if the version of python installed on your system is 3.8.10 or later. If not, please update it.
2. Type this command in terminal: python3 200101002.py
3. Answer the prompts of the program as requested, they are self-explanatory
4. The only module used is the module random. it is usually present when python is installed on system. If not, please check before trying the program

Some important points about the program:
1.  The game can be played in 3 levels: easy, medium and hard. User has to tell his choice in the beginning for this.
2.  The user can choose to play first or second, user has to answer a prompt for the same, after choosing the level of difficulty.
3.  Before each move (of user or computer), the current board situation is printed. 
    Each of the 9 cells of the board are given a special number, 
    which has to be entered by user to make his move into that cell.
    cell numbers:           1 | 2 | 3
                            -   -   -
                            4 | 5 | 6
                            -   -   -
                            7 | 8 | 9

4.  If it's the user's turn, he is allowed to type P on the prompt, to see the the above reference board. 
    Then he has to make a move into a particular cell, by typing its cell number. 
    If it's a valid move (ie, integer typed by user is between 1 and 9 and that cell is not filled yet), the move is made, else error is printed until user enters a valid move.
5.  When it's the computer's turn, it simply makes its move and informs the user which cell he has made the move in.
6.  When the game ends, the result of the game, along with the final board is printed.
7.  The algorithm to choose moves for the computer is different for different difficulty modes.
8.  In hard mode, the computer makes the best move possible in the current situation with 90% chance 
    and a random move with 10% chance (this chance is ensured using random number generator).
    In easy mode, the computer makes the worst move possible with 90% chance and a random move possible with 10% chance.
    In medium mode, the computer makes the worst move possible with 50% chance, and the best move possible with 50% chance. No other move is made, hence there would be limited configurationsof board possible in this mode.
    The score is some positive number for computer's win, and some negative number for user's win and 0 for draw.
9.  Making the best move possible (hard choice function): This is done using a standard minimax algorithm with alpha beta pruning for increased efficiency. 
    Here computer is maximiser, and user is minimiser of score. 
10. Making the worst move possible (easy choice function): This is done using a modified minimax algorithm, with alpha beta pruning, 
    where both the user and the computer are minimisers of score, hence user may try to minimise the score by playing in his own favour, 
    and the computer makes bad moves which also favours the user.
11. When worst move had to be played, we call the easy choice function, when best move, we call hard choice function.
12. The first player always gets X and second one always gets O

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Question 2: folder - q2, files - 200101002_page1.tex, 200101002_page2.tex

Instructions for running program - 
1. Install texlive distribution of latex, if it is not already installed (this is the standard latex compiler in linux, but in case it's not on your system, please install using below command)
   sudo apt install texlive (distro of TeX used)
2. Compile the tex files for both pages using - latex 200101002_page1.tex, and latex 200101002_page2.tex
3. Use this command to convert the formed dvi files to pdf: dvipdf 200101002_page1.dvi and dvipdf 200101002_page2.dvi

Some important points about the program:
1. The exact spacing between lines and characters in some places may not be exactly the same as given in the attachments,
   but they may be modified to ensure good readability without changing meaning of any equation/formula.
2. 2, 3, 4, 5 are made blue on first page but no hyperlink is references as sir said it was not required.
   In case the program does not work, try downloading the xcolor package. 
   If not, please remove line 2 and replace line 22 of page 1 tex file with the following: 
   Equations 2, 3, 4, and 5 are some of the most important in Physics.\\
