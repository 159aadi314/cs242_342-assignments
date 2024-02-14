# Shell script of a random string generator
# Author: AADI AARYA CHANDRA
# Date and Time : 20/8/21 19:45

echo                                                                              # start.
echo "Welcome to random string generator!"
count=$1

if [ $count -lt 2 ]                            														# count must be greater than or equal to 2 otherwise it is invalid (VALIDATION CHECK FOR COUNT)
	then
	echo "Invalid Input. Count cannot be $count. Please enter a integer value of count (1st argument) greater than 2"
else

	length=$2
	
	if [ $length -le 0 ]																			# length must be greater than 0 otherwise it is invalid (VALIDATION CHECK FOR LENGTH)
		then
		echo "Invalid Input. Length cannot be $length. Please enter an integer value of length (2nd argument) greater than 0."															
	else
																										# if input of count and length is valid, we take alphabet set input from file input_ques02.txt
																										
		it=1																							# iterator for loop that takes alphabet set input into an array is set to 1 BECAUSE WE ASSUME 																											  1-BASED INDEXING instead of zero-based indexing
		
		while IFS= read -n1 alphabet[$it]									# taking the alphabet set as input from file input_ques02.txt.txt using input redirection after command typed in terminal
		do																								# while IFS= read -n1 allows taking input character by character till we reach EOF
			it=`expr $it + 1`
																										# we input the characters into an array called alphabet.
		done

		total_letters=`expr $it - 2`														     # since i starts from 1 and last element of alphabet array is EOF, total_letters taken input is it-2
		
		if [ "$total_letters" == "0" ]																	# atleast one letter must be present in our alphabet set (VALIDAITON CHECK FOR ALPHABET SET)
			then
			echo "Empty alphabet set"
		else
			it1=$total_letters
			while [ true ]                                                        # while loop to check if alphabet set contains only 1 distinct letter (SECOND VALIDATION CHECK FOR ALPHABET SET)
			do
				if [ "${alphabet[$total_letters]}" == "${alphabet[$it1]}" ]
				then
					it1=`expr $it1 - 1 `
					continue
				else
					break
				fi
			done						
			if [ "$it1" == "0" ]               				# it1 == 0 represents condition when all letters in alphabet set are same
				then
				check_validity=1
					if [ $length -ge $count ]             # if alphabet set contains only 1 distinct letter and length >= count, then we can't generate a random string of length = $length, so we exit
						then
						echo "Alphabet set contains only 1 distinct alphabet and required length of string is less than count. Please change alphabet set or change value of length/count. Latter will give a non-random string."
						exit
					else								  # Otherwise we give a warning of non-random string and recommend changing alphabet set
					echo "Warning: Alphabet set contians only 1 distinct alphabet. Please change alphabet set otherwise the following final string is always generated and is not random"
					fi
			fi                                                                                   # ALL VALIDATION CHECKS DONE AND VALID INPUT SCANNED
			echo "Processing..."
			echo
		
			s=""																					# string s denotes final string after below loop terminates. s is initialised to empty string.
		
		
			while [ ${#s} -lt $length ]																# using while loop to append characters to make string, till string length is less than length
			do
				rd2=0																				# rd2 is second random number generated, it shows how many times the character will be repeated
				
				while [ true ]																			# loop to find second random number
				do
					rd2=`expr $RANDOM % $count ` 					            # random number generated using while loop, which breaks when valid random number is found.
					if [ $rd2 -gt 0 ]										# we use $RANDOM to generate a random number, then take its modulo count, so that rd2 will now be between 0 and
						then													# count-1 if rd2 is greater than 0, then it is valid, otherwise run loop again till valid rd2 found
						break
					fi
				done
				
				while [ true ]																		# loop to find first random number, rd1, which denotes index of character to be repeated
				do
					rd1=`expr $RANDOM % $total_letters `					# generate random number using $RANDOM, take modulo total letters so that rd1 is between 0 and total_letters-1, then 
					rd1=`expr $rd1 + 1 `									# increment by 1 so that rd1 will now be between 1 and total_letters (this is done because we want 1-based indexing).
																				# eg: if $RANDOM is 57 and total_letters is 23, then rd1 is 57%23 + 1 = 12, which if we take given alphabet set, 																					  gives alphabet[12] which is 'L'. This satisfies the bounds on rd1.
					if [ ${#s} -gt 0 ]						# if s is as yet empty, we have no problem. Otherwise we have to check if letter denoted by p is same as the last letter chosen or not.
						then
						last_char=`expr ${#s} - 1 `    														# last_char stores last character of s
						
						if [ "${s:last_char:1}" != "${alphabet[$rd1]}" ]									# check if letter denoted by p is same as the last letter chosen or not.
							then
							break
						else
						
							already_count=1				# in case last_char and alphabet[p] are same, run a while loop to find the number of times that character has appeared already at end of s
							posn=1							# this value is stored in already_count. posn tells distance from end of string.
							
							while [ ` expr $last_char - $posn ` -ge 0  ]                               		# while loop runs till we reach beginning of string, or may break in-between as below
							do
								if [ "${alphabet[rd1]}" == "${s:` expr $last_char - $posn ` : 1}" ]  		# we check if character at the index pointed by last_char-posn is same as alphabet[p].
									then																	# not, we break from loop. Otherwise increment already_count.
									
									already_count=` expr $already_count + 1`
									posn=`expr $posn + 1`
									
								else
									break
								fi
							done

							if [ ` expr $already_count + $rd2` -lt `expr $count ` ]					# if number of times the letter has come already at end of string (= already_count) plus rd2
								then
								break																# (ie number of times) we plan to add the same character) is less than count, then it is fine.
							else
								if [ ` expr $count - 1 - $already_count ` -gt 0 ]					# Otherwise, we check if (count-1) - already_count is greater than 0 or not. If yes, we change
									then														# rd2 to that value, so as to keep the number of times that character is repeated less than count
									rd2=` expr $count - 1 - $already_count `
									break
									
								else								# Otherwise, if there was only one distinct character in alphabet set, we have already considered that case. (line 56)
								 	continue						# Else, we must find another set of rd1 and rd2, so we continue the loop
								fi	
							fi

						fi

					else 
						break
					fi

				done                                                                                      # both random numbers have been found now

			if [ `expr ${#s} + $rd2` -gt $length ]				# check if new length of string with current values of random numbers would be greater than length or not. if yes, we have to 																		  reduce rd2 so as to keep the length of new string upto $length
				then
				rd2=` expr $length - ${#s} `
			fi
			t=""						# declaring empty string t which after below while loop becomes the substring generated using above 2 random numbers, which is then to be appended to s
			it2=0												# declaring iterator for below while loop
			
			while [ $it2 -lt $rd2 ]								# using while loop to append character denoted by alphabet[rd1] one by one rd2 number of times to the end of string t
			do
				t="$t${alphabet[$rd1]}"
				
				it2=`expr $it2 + 1`
			done

			s="$s$t"											# concatenation of t to the end of s
			echo $s												# resulting new string s

			done
			echo
			echo "Final random string generated of length $length and no same letter substring longer than `expr $count - 1` : $s"         # printing the final string
			echo
		fi
	fi
fi 																			# end


