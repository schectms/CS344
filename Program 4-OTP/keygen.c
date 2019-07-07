/*
* SHIFRA SCHECTMAN
* CS344-Program 4
* KEYGEN
*This program takes an number as an argument and
generates a string of that length
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int * main(int argc, char** argv)
{
	if (argc > 1 && argc < 3 ) //checks that only 1 argument is passed in 
	{
		const int length=atoi(argv[1]); //converts the int passed in to an int length
		char key[length+1]; //key string
	
		srand(time(NULL)); //seed ransom
		int i;
		for (i = 0; i < length; i++)
		{
			char c = rand() % (90-65) + 65; //generates randon letter
			printf("%c", c); //prints
		}
		printf("%c", '\n'); //prints new line
	}
	else
	{
		fprintf(stderr, "%s", "Cannot run keygen with given input\n"); //error
	}
}
