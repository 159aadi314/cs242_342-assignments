import random           # random module is used for getting random numbers in some places

print("Welcome to AADI'S TIC-TAC-TOE. X goes first, O goes second")                 # welcome message

print('Choose the difficulty level: Press 1 for easy, 2 for medium, 3 for hard')    # allowing user to choose difficulty level
level = input()
try: 
    level = int(level)      # store input in variable level
except:
    print('Please enter integer value for level')
    exit()

print('Press 1 to go first, 2 to go second')                # allowing user to choose to go first or second
user = input()
try: 
    user = int(user)        # stored input in variable user, if it can be converted to int (ie user entered a valid digit as input)
except:
    print('Please enter only integer value')
    exit()

cell = [' ']*9              # cell array stores contents of each of the 9 cells of the board
symbols = ['X','O']         # the two symbols are X and O in the game, stored in symbols array
if user == 1:               # if 2nd input was 1, it means user wants to go first, so we give user the number 0 and computer the number 1
    computer = 1
    user = 0
elif user == 2:             # otherwise user wants to go second, give user number 1, and computer number 0
    computer = 0
    user = 1
else:
    print('Invalid input')  # if user entered some other number, it was invalid and we exit
    exit()

def print_ref_board():      # function to print the reference board, showing the cell numbers of each cell of the board
    print(' ')
    print('Reference Board for cell numbers:')
    print(' ')
    print("\033[4m" + '1' + "\033[0m" + '|' + "\033[4m" + '2' + "\033[0m"+ '|' + "\033[4m" + '3' + "\033[0m")       # the escape characters are used to give the number an underline
    print("\033[4m" + '4' + "\033[0m" + '|' + "\033[4m" + '5' + "\033[0m" + '|' + "\033[4m" + '6' + "\033[0m")
    print('7' + '|' + '8' + '|' + '9')
    print(' ')

def print_board():          # function to print the board as it currently is
    print(' ')
    print('Current board situation:')
    print(' ')
    print("\033[4m" + cell[0] + "\033[0m" + '|' + "\033[4m" + cell[1] + "\033[0m"+ '|' + "\033[4m" + cell[2] + "\033[0m")   # cell[i] stores the symbol (X or O) in the board
    print("\033[4m" + cell[3] + "\033[0m" + '|' + "\033[4m" + cell[4] + "\033[0m" + '|' + "\033[4m" + cell[5] + "\033[0m")
    print(cell[6] + '|' + cell[7] + '|' + cell[8])
    print(' ')

def isGameOver():           # function to check if game is over, by looking at current board situation (the cell array)
    if (cell[0] != ' ' and cell[0]==cell[1] and cell[1] == cell[2]) or (cell[3] != ' ' and cell[3] == cell[4] and cell[4] == cell[5]) or (cell[6] != ' ' and cell[6] == cell[7] and cell[7] == cell[8]) or (cell[0] != ' ' and cell[0] == cell[3] and cell[3] == cell[6]) or (cell[1] != ' ' and cell[1] == cell[4] and cell[4] == cell[7]) or (cell[2] != ' ' and cell[2] == cell[5] and cell[5] == cell[8]) or (cell[0] != ' ' and cell[0] == cell[4] and cell[4] == cell[8]) or (cell[2] != ' ' and cell[2] == cell[4] and cell[4] == cell[6]):
        return True             # if one of the symbols is present in one whole row, or one column, or one diagonal, then that symbol's player is winner
    for i in range(0,9):    # otherwise, if no player has won, but some cells are still empty, the game is not over
        if cell[i] == ' ':
            return False
    return True             # otherwise, it's a draw

def print_result():         # function to print the result of the game
    print_board()           # print the board first
    if (cell[0] == symbols[0] and cell[0]==cell[1] and cell[1] == cell[2]) or (cell[3] == symbols[0] and cell[3]==cell[4] and cell[4] == cell[5]) or (cell[6] == symbols[0] and cell[6]==cell[7] and cell[7] == cell[8]) or (cell[0] == symbols[0] and cell[0]==cell[3] and cell[3] == cell[6]) or (cell[1] == symbols[0] and cell[1]==cell[4] and cell[4] == cell[7]) or (cell[2] == symbols[0] and cell[2]==cell[5] and cell[5] == cell[8]) or (cell[0] == symbols[0] and cell[0]==cell[4] and cell[4] == cell[8]) or (cell[2] == symbols[0] and cell[2]==cell[4] and cell[4] == cell[6]):    # if symbols[0], ie, X is present in one whole row, or column, or diagonal, then we check which player had X
        if user == 0:       # if user had X, user is winner
            
            print('You win! Congratulations on beating me! I will have to improve, I guess')
        else:                  # else user has lost
            
            print('You lose! Better luck next time!')

    elif (cell[0] == symbols[1] and cell[0]==cell[1] and cell[1] == cell[2]) or (cell[3] == symbols[1] and cell[3]==cell[4] and cell[4] == cell[5]) or (cell[6] == symbols[1] and cell[6]==cell[7] and cell[7] == cell[8]) or (cell[0] == symbols[1] and cell[0]==cell[3] and cell[3] == cell[6]) or (cell[1] == symbols[1] and cell[1]==cell[4] and cell[4] == cell[7]) or (cell[2] == symbols[1] and cell[2]==cell[5] and cell[5] == cell[8]) or (cell[0] == symbols[1] and cell[0]==cell[4] and cell[4] == cell[8]) or (cell[2] == symbols[1] and cell[2]==cell[4] and cell[4] == cell[6]):  # if O is present in one whole row, or column, or diagonal, then we check which player had O
        if user == 1:       # if user had O, user is winner
            
            print('You win! Congratulations on beating me! I will have to improve, I guess')
        else:               # else user has lost
            
            print('You lose! Better luck next time!')

    else:                   # if no player has won, either game is draw, or game not over. but we call this function after checking if game is over. So it must mean that game is a draw
        print('Draw! You are a tough opponent to beat')

def eval_board():           # function to evaluate a finished game board. If user has won, board has value -50, if computer has won, it has value 50, if draw, it has value 0
    if (cell[0] == symbols[0] and cell[0]==cell[1] and cell[1] == cell[2]) or (cell[3] == symbols[0] and cell[3]==cell[4] and cell[4] == cell[5]) or (cell[6] == symbols[0] and cell[6]==cell[7] and cell[7] == cell[8]) or (cell[0] == symbols[0] and cell[0]==cell[3] and cell[3] == cell[6]) or (cell[1] == symbols[0] and cell[1]==cell[4] and cell[4] == cell[7]) or (cell[2] == symbols[0] and cell[2]==cell[5] and cell[5] == cell[8]) or (cell[0] == symbols[0] and cell[0]==cell[4] and cell[4] == cell[8]) or (cell[2] == symbols[0] and cell[2]==cell[4] and cell[4] == cell[6]):    # using same criteria as last function, we check if user has won or computer has won or it's a draw
        if user == 0:
            return -50      # user won
        else:
            return 50       # user lost

    elif (cell[0] == symbols[1] and cell[0]==cell[1] and cell[1] == cell[2]) or (cell[3] == symbols[1] and cell[3]==cell[4] and cell[4] == cell[5]) or (cell[6] == symbols[1] and cell[6]==cell[7] and cell[7] == cell[8]) or (cell[0] == symbols[1] and cell[0]==cell[3] and cell[3] == cell[6]) or (cell[1] == symbols[1] and cell[1]==cell[4] and cell[4] == cell[7]) or (cell[2] == symbols[1] and cell[2]==cell[5] and cell[5] == cell[8]) or (cell[0] == symbols[1] and cell[0]==cell[4] and cell[4] == cell[8]) or (cell[2] == symbols[1] and cell[2]==cell[4] and cell[4] == cell[6]):
        if user == 1:
            return -50      # user won
        else:
            return 50       # user lost

    else:
        return 0 

def hard_choice(depth,mx,alpha,beta):   # function to give the move to computer, when hard mode is selected by user
    if isGameOver() == True:        # if game is over, we simply evaluate the board first
        val = eval_board()
        if val == 50:
            val -= depth            # then if computer has won, we subtract depth, ie number of moves to reach this position from current position, from 50
            return val              # this is done so that the fastest win is chosen
        elif val == -50:            # else if user has won, we add depth to -50, so that among the losses, the slowest one is chosen
            val += depth
            return val
        else:                       # else if this position is drawn, we simply return val
            return val
    elif mx == True:                # otherwise, if game is not over in this position, we see if this currently it is computer's move or user's move.
        best = -500                 # if it's computer's move, we pick the best move to be played in the current situation, using alpha beta pruning to make the process more efficient
        for i in range(0,9):        # COMPUTER IS MAXIMISER
            if cell[i] == ' ':      # we assume i'th cell is the move made by computer, then see if it's better than what we already have, and update the variables best and alpha accordingly
                cell[i]=symbols[computer]   # we put computer's symbol in cell[i], after checking if this cell is empty
                val = hard_choice(depth+1,not mx,alpha,beta)    # then we find the best move in that situation
                cell[i] = ' '       # and then we reset the board to how it is actually right now
                best = max(val,best)    # we update variable best with the best score we have found so far
                alpha = max(best,alpha) # and also update alpha if needed
                if beta <= alpha:   # when beta <= alpha, it means we have found that this move is certainly not better for computer than what we have seen so far
                    break
                
        return best                 # finally, we return best move out of all available moves, calculated above
    else:                           # if it's the user's move, since user is minimiser, we find which move the user would play here that would minimise our score
        best = 500                  # this is done with same approach as last time
        for i in range(0,9):        # we assume ith cell is the move made by user, then see if it's better for the user than what we already have
            if cell[i] == ' ':
                cell[i]=symbols[user]
                val = hard_choice(depth+1,not mx,alpha,beta)   # alpha beta pruning is used like before
                cell[i] = ' '
                best = min(best,val)
                beta = min(best,beta)
                if beta <= alpha:           # when beta <= alpha, it means we have found that this move is certainly not better for user than what we have seen so far, so we break
                    break
                
        return best
            
def easy_choice(depth,mx,alpha,beta):   # function to make choice of move for computer when easy mode is selected
    if isGameOver() == True:            # if game is over, we simply evaluate the board first
        val = eval_board()
        if val == 50:
            val -= depth                # then if computer has won, we subtract depth, ie number of moves to reach this position from current position, from 50
            return val                  # this is done so that the fastest win is chosen
        elif val == -50:                # else if user has won, we add depth to -50, so that among the losses, the slowest one is chosen
            val += depth                
            return val
        else:                           # else if this position is drawn, we simply return val
            return val
    elif mx == False:                   # otherwise, if game is not over in this position, we see if this currently it is computer's move or user's move.
        best = -500                     # in easy mode, user and computer both are minimisers, and hence the code is same for both cases (mx=0 and mx=1)
        for i in range(0,9):
            if cell[i] == ' ':
                cell[i]=symbols[user]
                val = easy_choice(depth+1,not mx,alpha,beta)    # alpha beta pruning used
                best = min(val,best)            # user trying that move which minimises score of computer
                alpha = min(alpha,best)
                cell[i] = ' '
                if beta <= alpha:
                    break
        return best                 # returning best move for user to play
    else:
        best = 500                      # if it's computer's move, since computer is also minimiser is easy mode, it finds the worst move in given situation for itself
        for i in range(0,9):
            if cell[i] == ' ':
                cell[i]=symbols[computer]
                val = easy_choice(depth+1,not mx,alpha,beta)
                cell[i] = ' '
                best = min(best,val)            # finding worst move for computer, ie one which minimises computer's 'score'
                beta = min(best,beta)
                if beta <= alpha:
                    break
        return best
    


def easy(user_choice):      # function called when easy mode is selected
    if user_choice !=0 and user_choice != 1:    # first evaluation check for input by user, it should be 0 if user wants to go first, or 1 if wants to go second (after updating this value in lines 23-28) 
        print('Invalid input for user to go first or second')
        exit()
    else:                   # if input is valid, we maintain a counter j, whose parity tells us whose move it is right now. even parity means the one who played first move, odd means the one who played second
        j=0
        while isGameOver() == False:    # while the game is not over, we let user and computer make moves
            if j == user_choice:        # if it's user's turn
                while True:                 # we run an infinite loop, until user makes a valid move
                    print_board()       # first we print the board
                    print("It's your turn, enter the cell number in which you want to make your move. Press P to get the reference board")
                    k = input()         # prompt the user and ask him to make move or ask for reference board to refer to the cell numbers of various cells of the board
                    if k == 'P':        # if entered value is P, user wants reference board, so we print it
                        print_ref_board()
                    else:               # else if user didn't enter an integer, we show error message and prompt again
                        try:
                            k = int(k)
                        except: 
                            print('Invalid cell number')
                        if k >=1 and k <=9:         # if user's input was a valid integer between 1 and 9, we check if that cell is empty or not. if yes, we make the move and break from loop
                            if cell[k-1] == ' ':
                                cell[k-1] = symbols[user_choice]
                                print('Your move has been made')
                                break
                            else:                   # else we show error message
                                print('This cell has already been filled, choose some other cell')
                        else:                       # if cell number was invalid, we show error message
                            print('Invalid cell number')
            
            else:                   # if it's computer's turn
                print_board()       # print the current board situation
                p = random.randint(1,10000)             # use a random integer between 1 and 10000, and find its remainder with 1000
                p %= 10
                if p == 0:                  # if remainder is 0, which is rare, we let computer choose a random (valid) move to make, else computer makes the worst move in current situation
                    c = -1
                    l = []
                    for i in range(0,9):
                        if cell[i] == ' ':  # all valid cell numbers are added to list l
                            l.append(i+1)
                    c = random.choice(l)  # random cell number is chosen from list l, then assign it to c
                else:                       # else if remainder is 1, which is very likely, computer makes the worst possible move in the current position
                    c = -1
                    best = 500
                    for i in range(0,9):        # we go through each cell in which move can be made
                        if cell[i] == ' ':      # we temporarily make a move in this cell
                            cell[i] = symbols[computer]     # we evaluate current board situation with the temporary move made
                            val = easy_choice(0,0,-1000,1000)       # easy choice function is called for the same
                            if val <= best:
                                best = val
                                c = i+1
                            cell[i] = ' '       # then we rest the temporary move
                cell[c-1]=symbols[computer]     # finally c contains cell number of cell in which required move is to be made, so we make this move, and inform the user
                print('I have made my move in cell ',c)
            
            j += 1      # increment j
            j %= 2
        print_result()  # print the result when the game gets over and we come out of while loop
    

def medium(user_choice):        # if medium mode is selected by user
    if user_choice !=0 and user_choice != 1:        # validation check for user input
        print('Invalid input for user to go first or second')
        return -1
    else:       # user input is valid
        j=0
        while isGameOver() == False:   # while game is not over
            if j == user_choice:        # if it's user's choice, we let user make move just like before
                while True:
                    print_board()
                    print("It's your turn, enter the cell number in which you want to make your move. Press P to get the reference board")
                    k = input()
                    if k == 'P':
                        print_ref_board()
                    else:           # same as in last function
                        try:
                            k = int(k)
                        except: 
                            print('Invalid cell number')
                        if k >=1 and k <=9:
                            if cell[k-1] == ' ':            # same as in last function
                                cell[k-1] = symbols[user_choice]
                                print('Your move has been made')
                                break
                            else:
                                print('This cell has already been filled, choose some other cell')
                        else:
                            print('Invalid cell number')        # same as in last function
            
            else:                   # if it's computer's move, we do almost same thing as in easy mode
                print_board()
                p = random.randint(1,10000) # get a random number between 1 and 10000
                p %= 2                          # check if it's even or odd
                if p == 0:          # if number is even (50% chance), then we are going to make the worst move in this situation
                    c = -1
                    best = -500
                else:               # if p is odd, we make the best move in this situation
                    c = -1
                    best = 500
                for i in range(0,9):        # same as in last function
                    if cell[i] == ' ':
                        cell[i] = symbols[computer]
                        if p == 0:
                            val = hard_choice(0,0,-1000,1000)   # if p is 0, computer finds the best move in current situation
                            if val >= best:
                                best = val
                                c = i+1
                        else:
                            val = easy_choice(0,0,-1000,1000)   # else it finds the worst move possible in current situation
                            if val <= best:
                                best = val
                                c = i+1
                        cell[i] = ' '
                cell[c-1]=symbols[computer]         # make the computer's move after it has been chosen
                print('I have made my move in cell ',c)
            
            j += 1
            j %= 2
        print_result()          # print result when game is over

def hard(user_choice):          # function to play game when hard mode is selected by user
    if user_choice !=0 and user_choice != 1:            # validation checks for user input
        print('Invalid input for user to go first or second')
    else:
        j=0
        while isGameOver() == False:    # while the game is not over
            if j == user_choice:    # if it's the user's move, we ask user to make a move, just like in last 2 functions
                while True:
                    print_board()           # same as in last function
                    print("It's your turn, enter the cell number in which you want to make your move. Press P to get the reference board")
                    k = input()
                    if k == 'P':
                        print_ref_board()           # same as in last function
                    else:
                        try:
                            k = int(k)
                        except: 
                            print('Invalid cell number')    # same as in last function
                        if k >=1 and k <=9:
                            if cell[k-1] == ' ':
                                cell[k-1] = symbols[user_choice]
                                print('Your move has been made')    # same as in last function
                                break
                            else:
                                print('This cell has already been filled, choose some other cell')
                        else:
                            print('Invalid cell number')    # same as in last function
            
            else:                   # else if it is computer's move
                print_board()
                p = random.randint(1,10000)     # we take a random number between 1 and 10000
                p %= 10              # and find its remainder with 1000
                if p == 0:              # if in the rare occasion that this remainder is 0, we make a random move in this situation
                    c = -1
                    l = []
                    for i in range(0,9):
                        if cell[i] == ' ':  # all valid cell numbers are added to list l
                            l.append(i+1)
                    c = random.choice(l)  # random cell number is chosen from list l, then assign it to c    
                else:
                    c = -1          # else we will make the best move in this situation
                    best = -500
                    for i in range(0,9):        # same as in easy function
                        if cell[i] == ' ':
                            cell[i] = symbols[computer]
                            val = hard_choice(0,0,-1000,1000)       # we use hard choice function to make the best move in this situation for the computer
                            if val >= best:
                                best = val
                                c = i+1
                            cell[i] = ' '
                cell[c-1]=symbols[computer]     # finally make the required move for computer
                print('I have made my move in cell ',c)

            j += 1
            j %= 2
        print_result()          # finally print the result of the game when it gets over

print_ref_board()               # main function, first we print reference board
if level == 1:      # then if level is 1, we go to easy function to play the game in easy mode
    easy(user)
elif level == 2:       # else if level is 2, we go to medium function for medium mode
    medium(user)
elif level == 3:    # else if level is 3, we go to hard function for hard mode
    hard(user)
else:
    print('Invalid level')  # any other value of level is invalid, so we print error message and exit
    exit()
