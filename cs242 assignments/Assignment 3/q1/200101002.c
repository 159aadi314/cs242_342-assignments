#include<stdio.h>					// including necessary libraries
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])					// main function taking arguments from command line
{
													// argc tells number of arguments passed. Syntax of argument passed from is ./200101002 <env_var_name> <env_var_value> <id>, none of the 4 arguments can be null
	if(argc < 4)										// we need exactly 4 arguments
	{
		printf("Error: Too few arguments\n");			// if argc < 4 then it is too few arguments
		exit(0);
	}
	
	else if(argc > 4)								// if argc > 4 it is too many arguments
	{
		printf("Error: Too many arguments\n");
		exit(0);
	}

	else
	{													// if we have correct number of arguments

		char* decide = (char*) malloc(256*sizeof(char));			// char pointers to store the arguments as per the above mentioned syntax
		strcpy(decide, argv[3]);
		
		char* name = (char*) malloc(256*sizeof(char));
		strcpy(name, argv[1]);
		
		char* value = (char*) malloc(256*sizeof(char));
		strcpy(value, argv[2]);
		
		char* old_value = (char*) malloc(256*sizeof(char));				// old_value pointer will store the value of env variable before this program is executed

		if(getenv(name) == NULL)
		{																			// if we use getenv and it returns null, it means the env variable of that name does not exist.
			printf("Error: Environment variable %s does not exist\n", name);		// in this case we print out an error message and exit
			exit(0);
		}

		else
		{															// if env variable of that name exists
		
			strcpy(old_value, getenv(name));							// copying old value from return value of getenv, now that we know getenv will return non null value, ie env variable exists
	
			if(strcmp(decide, "1") == 0)			// if the value of decide (id in makefile) is 1, it means we want to modify value of env variable to new value
			{

				if(setenv(name, value, 1) != 0)
				{															// else we use setenv to set env variable 'name' to value, printing error if it is unsuccessful
					printf("Error: Could not define environment variable\n");
					exit(0);
				}
				
				else
				{
					printf("Modified value of environment variable %s from %s to %s\n", name, old_value, value);		// printing result and exiting
					exit(0);
				}

			}
			
			else if(strcmp(decide, "0") == 0)					// if decide value (id in makefile) is 0, it means we have to retain the old value of env variable
			{
				printf("Retained value of environment variable %s as %s\n", name, old_value);		// so we print env variable with its old value and exit
				exit(0);
			}
			
			else
			{
				printf("Error: Invalid syntax. Please enter only 0 or 1 in the first prompt in command\n");		// else if decide value is set to anything else, it is syntax error, we print error message and exit
				exit(0);
			}

		}

	}

	return 0;
	
}					// end of main
