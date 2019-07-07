/*
*SHIFRA SCHECTMAN
*CS344-OPERATING SYSTEMS
*PROGRAM 3
*This program runs a shell script in C
*/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

int allowBackground = 1; //global variable to toggle foreground mode
int arrayPid[10];

/*
*C program to Replace a word in a text by another given word
*Source : https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
*/
char* replaceWord(const char* s, const char* oldW,
	const char* newW)
{
	char* result;
	int i, cnt = 0;
	int newWlen = strlen(newW);
	int oldWlen = strlen(oldW);

	// Counting the number of times old word 
	// occur in the string 
	for (i = 0; s[i] != '\0'; i++)
	{
		if (strstr(&s[i], oldW) == &s[i])
		{
			cnt++;

			// Jumping to index after the old word. 
			i += oldWlen - 1;
		}
	}

	// Making new string of enough length 
	result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1);

	i = 0;
	while (*s)
	{
		// compare the substring with the result 
		if (strstr(s, oldW) == s)
		{
			strcpy(&result[i], newW);
			i += newWlen;
			s += oldWlen;
		}
		else
			result[i++] = *s++;
	}

	result[i] = '\0';
	return result;
}


/*
*FUNCTION NAME : catchSIGSTP
*PARAMETERS: int
*This function sets CTRL-Z to toggle foreground mode
*/
void catchSIGSTP(int signo)
{
	//if the program is not in foreground mode
	if (allowBackground == 1)
	{
		//print message letting user know that foreground mode will be entered
		char* message = "Entering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, message, 49);
		fflush(stdout);
		allowBackground = 0; //toggles allowBackground variable
	}
	//if the program is in foreground mode
	else
	{
		//print message saying foreground mode will be exited
		char* message = "Exiting foreground-only mode\n";
		write(STDOUT_FILENO, message, 29);
		fflush(stdout);
		allowBackground = 1; //toggles allowBackground variable
	}


}

/*
*FUNCTION NAME : getInput
*This function gets input from user and returns it
*/
char* getInput()
{
	int numCharsEntered = -5; // How many chars we entered
	int currChar = -5; // Tracks where we are when we print out every char
	size_t bufferSize = 0; // Holds how large the allocated buffer is
	char* lineEntered = NULL; // Points to a buffer allocated by getline() that holds our entered string + \n + \0

	while (1)
	{
		// Get input from the user
		while (1)
		{
			printf(":");
			fflush(stdout);
			// Get a line from the user
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
			if (numCharsEntered == -1)
				clearerr(stdin);
			else
				break; // Exit the loop - we've got input
		}

		// Remove the trailing \n that getline adds
		lineEntered[strcspn(lineEntered, "\n")] = '\0';
		// Free the memory allocated by getline() or else memory leak

		return lineEntered;
		// free(lineEntered);
		// lineEntered = NULL;
	}
}

/*
*FUNCTION NAME : returnInfo
*PARAMETERS: character array of input
*This function takes the input from the user and stuffs it into an array
*/
char** returnInfo(char* input)
{
	int i = 0; //set counter variable to 0
	char* token = strtok(input, " "); //split input by spaces
	char** array = malloc(512 * sizeof(char*)); //allocate memory for array
	memset(array, 0, sizeof(array)); //initialize to null values
	for (i = 0; i < sizeof(array); i++)
	{
		array[i] = NULL;
	}
	i = 0;
	//loop through input and all tokens to array
	while (token != NULL)
	{
		array[i] = token;
		token = strtok(NULL, " ");
		i++;
	}

	return array;
}

/*
*FUNCTION NAME : returnCommand
*PARAMETERS: array of strings(character arrays)
*This function takes the array and returns the command(first element)
*/
char* returnCommand(char** array)
{
	return array[0];
}

/*
*FUNCTION NAME : returnInput
*PARAMETERS: array of strings(character arrays)
*This function takes the array and returns the input redirection variable
*/
char* returnInput(char** array)
{
	int i; //counter variable 
	//loop through array 
	for (i = 0; i < sizeof(array); i++)
	{
		if (array[i] != NULL) //check if element is null
		{
			if (strcmp(array[i], "<") == 0) //if the element is <
			{
				if (array[i + 1] != NULL) //check if following element null
				{
					return array[i + 1]; //returns the next element
				}
			}
		}
	}
	return NULL; //return null if no input 
}

/*
*FUNCTION NAME : returnOutput
*PARAMETERS: array of strings(character arrays)
*This function takes the array and returns the output redirection variable
*/
char* returnOutput(char** array)
{
	int i; //counter variable 
	//loop through array 
	for (i = 0; i < sizeof(array); i++)
	{
		if (array[i] != NULL) //check if element is null
		{
			if (strcmp(array[i], ">") == 0) //if the element is >
			{
				if (array[i + 1] != NULL) //check if following element null
				{
					return array[i + 1]; //returns the next element
				}
			}
		}
	}
	return NULL; //return null if no output 
}

/*
*FUNCTION NAME : returnArgs
*PARAMETERS: array of strings(character arrays)
*This function takes the array and returns the arguments
*/
char** returnArgs(char** array)
{
	int i; //counter variable
	char** args = malloc(512 * sizeof(char*)); // allocate memory for array
	memset(args, 0, sizeof(args)); //initialize values to 0 
	for (i = 0; i < sizeof(args); i++)
	{
		args[i] = NULL;
	}
	i = 0;

	//loop through array
	for (i = 0; i < sizeof(array); i++)
	{
		if (array[i] != NULL) //check if element is null
		{
			if (strcmp(array[i], "<") == 0 || strcmp(array[i], ">") == 0 || strcmp(array[i], "&") == 0) //check if value is < > or &
			{

				return args; //return args
			}
			args[i] = array[i];
		}
	}
	return args; //return args
}

/*
*FUNCTION NAME : returnBackground
*PARAMETERS: character array
*This function takes the string and returns whether & is the last value
*/
int returnBackground(char* input)
{
	if (input[(strlen(input) - 1)] == '&')
		return 1;
	else
		return 0;
}

/*
*FUNCTION NAME : runCD
*PARAMETERS: array of strings(character arrays)
*This function changes the directory
*/
int runCD(char** args)
{
	//if there are more than 1 element after cd, error
	if (args[2] != NULL)
	{
		fprintf(stdout, "%s", "Too many arguments");
		fflush(stdout);
	}
	//if there are no element after cd, change to home dir
	else if (args[1] == NULL)
	{
		chdir(getenv("HOME"));
	}
	else
	{
		if (chdir(args[1]) != 0) //try to change into the directory input
		{
			fprintf(stdout, "%s", "Directory does not exist");
			fflush(stdout);
		}
	}
	return 1;

}

/*
*FUNCTION NAME : printStatus
*PARAMETERS: integer variable
*This function prints the status of the last foreground command
*/
void printStatus(int childExitMethod)
{
	//if the process  exited normally, print exit status
	if (WIFEXITED(childExitMethod))
	{
		int exitStatus = WEXITSTATUS(childExitMethod);
		fprintf(stdout, "%s%d\n", "exit value ", exitStatus);
		fflush(stdout);
	}

	//if the process exited with a signal, display signal
	else if (WIFSIGNALED(childExitMethod))
	{
	
		int termSignal = WTERMSIG(childExitMethod);
		fprintf(stdout, "%s%d\n", "terminated by signal ", termSignal);
		fflush(stdout);
	}

}

/*
*FUNCTION NAME : execute
*PARAMETERS: character array with command, array of strings(char array), char arry for input, char array for output, int background, exitstatus, signal
*This function runs built-in commands
*/
int execute(char* command, char** args, char* inputRedir, char* outputRedir, int background, int exitStatus, struct sigaction SIGINT_action)
{

	int sourceFD, targetFD, result; //declare variables
	pid_t spawnPid = -5; //initialize spawnPid to garbage variable
	spawnPid = fork(); //fork
	int childExitStatus = exitStatus;

	switch (spawnPid)
	{
		//if spawnPid fails, print error
	case -1:;
		perror("Hull Breach!\n");
		fflush(stdout);
		exit(1);
		break;

	case 0:;
		SIGINT_action.sa_handler = SIG_DFL;
		sigaction(SIGINT, &SIGINT_action, NULL);

		if (inputRedir != NULL && strcmp(inputRedir, ""))
		{
			sourceFD = open(inputRedir, O_RDONLY);
			if (sourceFD == -1)
			{
				fprintf(stdout, "%s%s%s\n", "cannot open ", inputRedir, "for input");
				fflush(stdout);
				exit(1);
			}
			result = dup2(sourceFD, 0);
			if (result == -1)
			{
				perror("Cannot assign input\n");
				fflush(stdout);
				exit(2);
			}
			fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
		}

		if (outputRedir != NULL && strcmp(outputRedir, ""))
		{
			targetFD = open(outputRedir, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (targetFD == -1)
			{
				fprintf(stdout, "%s%s%s\n", "cannot open ", outputRedir, "for output");
				fflush(stdout);
				exit(1);
			}
			result = dup2(targetFD, 1);
			if (result == -1)
			{
				perror("Cannot assign output\n");
				fflush(stdout);
				exit(2);
			}
			fcntl(targetFD, F_SETFD, FD_CLOEXEC);
		}


		execvp(command, args);
		fprintf(stdout, "%s%s\n", command, " does not exist");
		fflush(stdout);
		exit(2);
		break;

	default:;

		//int *array[10];
		int j = 0;

		if (background == 1 && allowBackground == 1)
		{
			//pid_t actualPid = array[j] =waitpid(spawnPid, &childExitStatus, WNOHANG);			
			arrayPid[j] = spawnPid;
			j++;
			fprintf(stdout, "%s%d\n", "background pid is: ", spawnPid);
			fflush(stdout);
		}

		else
		{
			pid_t actualPid = waitpid(spawnPid, &childExitStatus, 0);
		}

		return childExitStatus;

	};


}




int main()
{


	// Redirect ctrlZ to our own function
	struct sigaction SIGSTP_action = { 0 };
	SIGSTP_action.sa_handler = catchSIGSTP;
	sigfillset(&SIGSTP_action.sa_mask);
	SIGSTP_action.sa_flags = 0;
	sigaction(SIGTSTP, &SIGSTP_action, NULL);

	// We want to Ignore ctrlC
	struct sigaction SIGINT_action = { 0 };
	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);

	int pid = getpid(); //initialize pid

	//convert pid to string
	int length = snprintf(NULL, 0, "%d", pid);
	char* str1 = malloc(length + 1);
	snprintf(str1, length + 1, "%d", pid);

	int exitStatus = 0; //initialize exitStatus
	int i = 0; //initialize i
	int es = 0;

	//loop until user exits
	do
	{
		//loop to check for terminated background processess
		int i;
		for (i = 0; i < sizeof(arrayPid); i++)
		{
			if (arrayPid[i] != '\0')
			{
				if (waitpid(arrayPid[i], &exitStatus, WNOHANG) > 0)
				{
					printf("%s%d %s", "background pid ", arrayPid[i], "is done\n");
					arrayPid[i] = '\0';
					printStatus(exitStatus);
				}
			}
		}

		char* input = getInput(); //getInput from user
		char* result = NULL;
		char c[] = "$$";
		result = replaceWord(input, c, str1);


		char** array = returnInfo(result); //stuff input into an array

		char** args = returnArgs(array); //get args
		char* command = returnCommand(array); //get command
		char* inputRedir = returnInput(array); //get input
		char* outputRedir = returnOutput(array); //get output

		int background = 0; //set background to 0

		//if user enters blank value or # , reprompt
		if (command == NULL || command[0] == '#')
		{
			continue;
		}
		//if user inputs cd, change the dir
		else if (strcmp(command, "cd") == 0)
		{
			runCD(args);
		}
		//if user enters exit, program exits
		else if (strcmp(command, "exit") == 0)
		{
			return 0;
		}
		//if user enters status, print status of last foreground command
		else if (strcmp(command, "status") == 0)
		{
			printStatus(es);
		}
		//otherwise, run built in command using execute
		else
		{
			background = returnBackground(input);
			es = execute(command, args, inputRedir, outputRedir, background, exitStatus, SIGINT_action);
		}


		//free(args);
		//free(array);
		//free(str1);
		//free(input);


	} while (1);



}
