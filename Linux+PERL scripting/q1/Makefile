# Author: AADI AARYA CHANDRA
# Date and Time: Oct 13, 2021, 20:00
# Title: Makefile to update or retain an existing environment variable

# setting variable CC to command gcc
CC := gcc					

# oneshell helps to execute all commands in one subshell so that all variables can be declared and used properly
.ONESHELL :

# target is executable file with name as my roll number, it has dependency as .c file of same name. type make in terminal to run this since this is the default target
200101002 : 200101002.c

# first we ask for an ID value from user, to decide whether to modify the value of an existing env variable (1) or retain its current value (0). We input and store this value in variable ID
	@echo "Enter 1 if you want to modify the value of existing environment variable and 0 if you want to retain its existing value:"
	@read ID

# then we ask for the name of the env variable, input taken and stored in variable NAME
	@echo "Enter the name of the environment variable:"
	@read NAME

# then we ask for value to which we want to modify the env variable stored in NAME, in case ID was 1. (can be left blank by just pressing enter if id was 0)
	@if [ "$$ID" = "1" ]
	@then
	@echo "Enter the value of the environment variable to which it is to be modified (write any non-null value if you want to retain existing value (ie you typed 0 in first prompt)):"
	@read VALUE
# if id was not 1, and id was 0, we assign some random value to VALUE, since it is only required as argument later in running C program and has no other significance
	@else
	VALUE="abcd"
	@fi

# command for compilation of 200101002.c (name of dependency, denoted by macro $<) to create execute with target name 200101002 (denoted by macro name $@)
	${CC} $< -o  $@

# command for running the executable, giving command line arguments of NAME, VALUE and ID in that order. in case 
	./200101002 $$NAME $$VALUE $$ID

# declaring a phony target clean
.PHONY: clean

# clean is used to delete the executable file after work is done. type make clean in terminal to run this
clean:
	@rm 200101002

