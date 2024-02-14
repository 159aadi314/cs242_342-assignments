#!/bin/bash

if [ "$1" != "tar" ] # if given command is not tar, we show error else we proceed
then

	echo "This is shell script for tar, please only use tar command"
	exit 0

else

	opt=$2					# define opt as the options string passed as third argument ($2)
	options=(c r x t f v)		# create 2 arrays, an options array showing the possible options and flag array showing which of these is present in opt (1 means present, 0 means absent)
	flag=(0 0 0 0 0 0 )

	for (( i=0; i<${#opt}; i++ ))				# iterate through characters of options string to update flag array
	do
													# the options string cannot have repeated options, also only one of c r x and t are allowed
		if [ "${opt:i:1}" == "${options[0]}" ] 		# if character is c
		then

			if [ "${flag[0]}" == "1" ] || [ "${flag[1]}" == "1" ] || [ "${flag[2]}" == "1" ] || [ "${flag[3]}" == "1" ]			# c can't be repeated and onyl one of c r x t is allowed
			then
				echo "Invalid options string, c can't be repeated and can have only one of c r x or t in options at a time"
				exit 0
			else
				flag[0]="1"					# update c flag
			fi

		elif [ "${opt:i:1}" == "${options[1]}" ]		# if character is r
		then

			if [ "${flag[0]}" == "1" ] || [ "${flag[1]}" == "1" ] || [ "${flag[2]}" == "1" ] || [ "${flag[3]}" == "1" ]			# r can't be repeated and onyl one of c r x t is allowed
			then
				echo "Invalid options string, r can't be repeated and can have only one of c r x or t in options at a time"
				exit 0
			else
				flag[1]="1"						# update r flag
			fi

		elif [ "${opt:i:1}" == "${options[2]}" ]		# if character is x
		then

			if [ "${flag[0]}" == "1" ] || [ "${flag[1]}" == "1" ] || [ "${flag[2]}" == "1" ] || [ "${flag[3]}" == "1" ]		# x can't be repeated and onyl one of c r x t is allowed
			then
				echo "Invalid options string, x can't be repeated and can have only one of c r x or t in options at a time"
				exit 0
			else
				flag[2]="1"								# update x flag
			fi

		elif [ "${opt:i:1}" == "${options[3]}" ]		# if character is t
		then

			if [ "${flag[0]}" == "1" ] || [ "${flag[1]}" == "1" ] || [ "${flag[2]}" == "1" ] || [ "${flag[3]}" == "1" ]			# t can't be repeated and onyl one of c r x t is allowed
			then
				echo "Invalid options string, t can't be repeated and can have only one of c r x or t in options at a time"
				exit 0
			else
				flag[3]="1"									# update t flag
			fi

		elif [ "${opt:i:1}" == "${options[4]}" ]		# if character is f
		then

			if [ "${flag[4]}" == "1" ]						# f can't be repeated
			then
				echo "Invalid options string, f repeated"
				exit 0
			else
				flag[4]="1"										# update f flag
			fi

		elif [ "${opt:i:1}" == "${options[5]}" ]			# if character is v
		then

			if [ "${flag[5]}" == "1" ]								# v can't be repeated
			then
				echo "Invalid options string, v repeated"
				exit 0
			else
				flag[5]="1"								# update v flag
			fi

		else
			echo "Invalid options string, invalid characters present"		# if some other character is present, it's invalid and we show error and exit
			exit 0
		fi

	done

	if [ "${flag[4]}" == "0" ]									# options string must contain f, if it doesn't we show error and exit
	then

		echo "Invalid options string, must contain f"
		exit 0

	elif [ "${flag[0]}" == "1" ]		# case when c is in the options string (ie user asks to create new archive)
	then

		if [ "$3" == "-" ]							# $3 denotes destination, if it is "-", it means output contents of archive to stdout
		then

			i=4									# i iterates through the sources files, count_arg counts number of sources files
			count_arg=0

			while [ "${!i}" ]
			do

				if [ -f "${!i}" ]							
				then
					count_arg=`expr $count_arg + 1`
					i=`expr $i + 1`
				else										# if one of the sources files doesn't exist, we show error and exit
					count_arg=0
					break
				fi

			done

			if [ "$count_arg" == "0" ]						# if no sources files are provided, we show error and exit
			then

				echo "No files or invalid files passed to be archived"
				exit 0

			else

				for (( i=4; i<`expr $count_arg + 4`; i++ )) 			# otherwise, we iterate through source files
				do													# and print file name, its details found using ls -lhad option
																		# ${!i} denotes argument number i+1 passed into program
					file=${!i}										# then print the contents of file using cat
					echo "$file"
					ls -lhad "$file"							# ls -lhad gives details of the file
					meta[$i]=$(ls -lhad "$file")
					echo "${meta[$i]}"
					cat "$file"					# copying contents of file into stdout

				done

			fi

		else									# if $3, ie destination is not "-", we create two files, one given by $3
												# which represents the archive file and a hidden file , to store its metadata
			touch "$3"
			touch ".$3"

			if [ "$?" != "0" ]			
			then
				exit 0
			else
				> "$3"					# since option is 'c', we overwrite these files if they already exist
				> ".$3"
			fi

			i=4							# i iterates through the sources files, count_arg counts number of sources files
			count_arg=0

			while [ "${!i}" ]
			do

				if [ -f "${!i}" ]
				then
					count_arg=`expr $count_arg + 1`
					i=`expr $i + 1`
				else						# if one of the sources files doesn't exist, we show error and exit
					count_arg=0
					break
				fi

			done

			if [ "$count_arg" == "0" ]				# if no sources files are provided, we show error and exit
			then
				echo "No files or invalid files passed to be archived"
				exit 0
			else

				for (( i=4; i<`expr $count_arg + 4`; i++ )) 	# otherwise, we iterate through source files
				do

					file=${!i}
					echo "$file" >> "$3"					# we first write separator info in archive file, which includes name of file and its details
					ls -lhad "$file" >> "$3"				# details given by ls -lhad option
					meta[$i]=$(ls -lhad "$file")			# we store these details in meta array
					head[$i]=$(stat -c%s "$3")				# head denotes the starting "bytes" of the contents of current source file
					cat "$file" >> "$3"						# then we print the contents of file using cat
					tail[$i]=$(stat -c%s "$3")				# tail denotes ending "bytes" of the contents of the file
					echo "$file ${head[$i]} ${tail[$i]} ${meta[$i]}" >> ".$3"		# we store head tail, file name and details of file in the hidden file .$3, which stores metadata of archive file $3

				done
			fi

		fi

		if [ "${flag[5]}" == "1" ]						# flag[5] == 1 denotes v option was there in options string,
		then

			for (( i=4; i<`expr $count_arg + 4`; i++ ))			# if v is there, we print names of source files that were archived
			do
				echo ${!i}
			done

		else
			exit 0									# else we exit
		fi

	elif [ "${flag[1]}" == "1" ]					# CASE OF r option (appending new files to existing archive)
	then

		if [ "$3" == "-" ]						# destination can't be "-" in this case, so we show error and exit if it is
		then

			echo "Invalid argument, cannot append files to stdout"
		else

			if [ ! -f ".$3" ]					# if archive file denoted by $3 does not exist, we show error and exit
			then								# we check if $3 is an archive file by finding if a hidden file .$3, which stores its metadata, exists or not
				echo "Given archive file does not exist"
				exit 0
			fi

			i=4					# otherwise we follow the same procedure as c option, but we don't overwrite the archive file or its hidden meta file
			count_arg=0

			while [ "${!i}" ]
			do

				if [ -f "${!i}" ]
				then
					count_arg=`expr $count_arg + 1`				# checking if all source files mentioned are existent or not
					i=`expr $i + 1`
				else
					count_arg=0
					break
				fi

			done

			if [ "$count_arg" == "0" ]					# if no source file mentioned we show error and exit
			then
				echo "No files or invalid files passed to be archived"
				exit 0
			else

				for (( i=4; i<`expr $count_arg + 4`; i++ )) 			# else by same procedure as before, we concatenate source files to archive files, with separator info
				do	

					file=${!i}											# and we update the metadata file .$3 also just like before
					echo "$file" >> "$3"
					ls -lhad "$file" >> "$3"						# copying separator info into archive file
					meta[$i]=$(ls -lhad "$file")			
					head[$i]=$(stat -c%s "$3")
					cat "$file" >> "$3"						# copying contents into archive file
					tail[$i]=$(stat -c%s "$3")
					echo "$file ${head[$i]} ${tail[$i]} ${meta[$i]}" >> ".$3"			# update hidden metadata file of archive file

				done
			fi

		fi

		if [ "${flag[5]}" == "1" ]							# if verbose option (v) is present, we print on stdout, the names of source files appended
		then

			for (( i=4; i<`expr $count_arg + 4`; i++ ))
			do
				echo ${!i}
			done

		else
			exit 0											# else we exit
		fi		

	elif [ "${flag[2]}" == "1" ]							# CASE of x option (extraction of existing archive file)
	then

		if [ "$4" ]								# if any more arguments are present other than archive file, we show error and exit
		then
			echo "Invalid syntax, only specify archive file as argument for x option"
			exit 0
		else

			if [ -f ".$3" ]					# else we check if given file $3 is an archive file or not, if not we show error and exit
			then								# if it exists, we extract it

				while read name head tail meta			# to extract archive, we use metadata info of .$3 file, 
													# we read the first column (containing source file name) into variable name, 
				do									# second column is read as head as it contains the byte number where current file contents start
													# third column is read as tail as it contains the byte number where current file contents end
					touch "$name"						# we create/overwrite file of name given by variable "name"
					> "$name"						
					head -c "$tail" "$3" | tail -c `expr $tail - $head` > "$name"		# then we copy first "tail" bytes of archive file using head command 
													# then we pass its output to tail command which reads the first "tail - head" bytes of this output. 
													# The output is precisely the bytes corresponding to current source file
				done < ".$3"				# using redirection to read name, meta head tail from hidden metadata file							

			else
				echo "Given file is not an archive file"			# in case given file is not an archive file
				exit 0
			fi
		fi

		if [ "${flag[5]}" == "1" ]					# if verbose option present, we print the file names by reading from metadata file .$3, stdout
		then

			while read name head tail meta
			do
				echo "$name"					# name of file echoed on terminal
			done < ".$3"

		else
			exit 0
		fi

	elif [ "${flag[3]}" == "1" ]				# CASE OF t option (table of contents of archive file needed)
	then

		if [ "$4" ]						# t option present means the only argument should be the archive file, in $3. 
		then							# If more arguments present, we show error and exit
			echo "Invalid syntax, only specify archive file as argument for t option"
			exit 0
		else						# else we first check if archive file of that name exists or not by same procedure as before

			if [ -f ".$3" ]
			then
														# if archive file exists, output depends on whether we have v option or not
				if [ "${flag[5]}" == "1" ]			# if v option present, we go through each line of metadata file .$3, and output "meta" (ie the details of source files) on the terminal
				then

					while read name head tail meta	
					do
						echo "$meta"				# verbose details of files echoed to terminal, if tvf
					done < ".$3"

				else

					while read name head tail meta			# if v is absent, we only print names of source files present in archive
					do
						echo "$name"
					done < ".$3"

				fi

			else
				echo "Given file is not an archive file"			# error and exit if archive file of that name does not exist
				exit 0
			fi

		fi

	else
		exit 0			
	fi

fi				# end