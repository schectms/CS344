/*
 * Shifra Schectman
 * CS344-Program 2
 * adventure
 * runs text-based game from files generated in buildrooms
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

pthread_t thread;  //create thread
pthread_mutex_t mutex; //create mutex
pthread_cond_t wakeCondition; //create condition for mutex
int wakeUp = 0; //keeps track of condition
int counter = 1;

/*
Function Name:writeToFileInThread
writes the time to a file 
*/
void *writeToFileInThread()
{
	while (1)
	{

		pthread_mutex_lock(&mutex); //acquires lock

		// when the condition is not met, do not run
		while (wakeUp == 0)  
		{
			pthread_cond_wait(&wakeCondition, &mutex);
		}

		time_t rawtime;
		struct tm *info;
		char buffer[80];
		time(&rawtime);
		info = localtime(&rawtime);
		strftime(buffer, 80, "%l:%M%P, %A, %B %e, %Y ", info);

		FILE *fptr;
		fptr = fopen("currentTime.txt", "w"); //open file for writing		
		fprintf(fptr, "%s", buffer);
		//fprintf(fptr, "%d", counter); 
		//counter++;
		fclose(fptr); //closes file

		wakeUp = 0; //resets condition
		pthread_cond_signal(&wakeCondition); //signals that condition has been changed

		pthread_mutex_unlock(&mutex); //unlocks mutex
	}
}

/*
Function Name:readFromMainThread
reads the time file which was created
*/
void readFromMainThread()
{

	pthread_mutex_lock(&mutex); //acquires lock for mutex
	char ch;
	FILE *fptr;
	fptr = fopen("currentTime.txt", "r"); //opens file for reading
	//prints file contents
	while ((ch = getc(fptr)) != EOF)
	{
		printf("%c", ch);
	}
	fclose(fptr); //closes file
	printf("\n\n");
	pthread_mutex_unlock(&mutex); //unlocks mutex

}

//room structure to track file contents
struct room
{
	char roomName[9];
	int rt;
        int numberOfConnections;
        char connectedRooms[6][9];
};

/*
Function name: printRoom
Function Parameters: pointer to room instance
*/
void printRoom(struct room *r) 
{
	int i = 0;
	printf("CURRENT LOCATION: %s\n", r->roomName); //prints room name
	//prints connections
	printf("POSSIBLE CONNECTIONS: ");
	for (i; i < r->numberOfConnections; i++)
	{
		printf("%s", r->connectedRooms[i]);
		if (i != r->numberOfConnections - 1) 
		{
			printf(", ");
		}

	}
	printf(".\n");

}
	
/*
 * Function Name: readRoomInfo
 * Function Parameters: diretory name and file name
 * prints the cutrent location and connections
 */
struct room * readRoomInfo(char* newestDirName, char* name)
{
	
	FILE *file1;
	char fp[260];	
	snprintf(fp, sizeof(fp), "%s/%s", newestDirName, name);

	file1 = fopen(fp, "r"); //opens file for reading
	if (file1 != NULL) //checks validity of file
	{
		struct room *r = malloc(sizeof(struct room)); //allocates memory for room array
		memset(r, 0, sizeof(struct room)); //initilizes values

		char buf[100];
		
		while (fscanf(file1, "ROOM NAME:  %s\n", buf) == 1) 
		{
			
		    strcpy(r->roomName, buf); //reads out name
			
			while (fscanf(file1, "\nCONNECTION %*i :  %s", buf) == 1)
			{
				strcpy(r->connectedRooms[r->numberOfConnections++], buf); //reads out connections
				
			}
			//reads out room type
			fscanf(file1, "\nROOM TYPE: %s\n", buf);
			if (strcmp(buf, "START_ROOM") == 0)
				r->rt = 0;
			else if (strcmp(buf, "END_ROOM") == 0)
				r->rt = 2;
			else
				r->rt = 1;
		}
		fclose(file1); //close file
	    return  r; //return array of rooms
	}
    return NULL;
	fclose(file1);
}

/*
Function name: isConnected
Function paramters: 2 room pointers
checks if the rooms are connected to each other
*/
int isConnected(struct room * current, struct room * next)
{
	int i = 0;
	//loops through all connections of current room
	for (i; i < current->numberOfConnections; i++)
	{
		if (strcmp(current->connectedRooms[i], next->roomName) == 0) //checks connection
		{
			return 1;
		}
	}
	return 0;
}

/*
Function name: getRoomByName
Function Parameters: pointer to room array, array of chars
takes the name of the room and returns a room pointer
*/
struct room * getRoomByName(struct room ** rooms, char *name)
{
	int i = 0;
	//loops through array
	for (i; i < 7; i++)
	{
		if (strcmp(rooms[i]->roomName, name) == 0)  //checks the name
		{
			return rooms[i];
		}
	}
	return NULL;
}

//main function
int main()
{
	pthread_cond_init(&wakeCondition, NULL); //initializes condition
	pthread_mutex_init(&mutex, NULL); //initializes mutex
	pthread_create(&thread, NULL, &writeToFileInThread, NULL); //initializes thread

	// all the rooms we have
	struct room **rooms = malloc( sizeof(struct room *) * 7);
	// the current room
	struct room * current = NULL;

	int newestDirTime = -1; // Modified timestamp of newest subdir examined
	char targetDirPrefix[32] = "schectms.rooms."; // Prefix we're looking for
	char newestDirName[256]; // Holds the name of the newest dir that contains prefix
	memset(newestDirName, '\0', sizeof(newestDirName));

	DIR* dirToCheck; // Holds the directory we're starting in
	struct dirent *fileInDir; // Holds the current subdir of the starting dir
	struct stat dirAttributes; // Holds information we've gained about subdir

	dirToCheck = opendir("."); // Open up the directory this program was run in

	if (dirToCheck > 0) // Make sure the current directory could be opened
	{
		while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
    	{
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
      			{
			        stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

        			if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
        			{
				         newestDirTime = (int)dirAttributes.st_mtime;
 				         memset(newestDirName, '\0', sizeof(newestDirName));
				         strcpy(newestDirName, fileInDir->d_name);
        			}
    			}
		}
 	}

	closedir(dirToCheck); // Close the directory we opened

	DIR *d;
	struct dirent *dir;
	int i=0;
	//read out all file names from directory
	d=opendir(newestDirName);
	if(d)
	{
		while((dir=readdir(d)) !=NULL )
		{
			if((strcmp(dir->d_name, ".") !=0)&& (strcmp(dir->d_name, "..")!=0))
			{
				rooms[i] = readRoomInfo(newestDirName, dir->d_name);
				i++;
			}
		}
		closedir(d);//closes directory
	}


	//loop through files and prints info of starting room
	for(i=0; i<7 ; i++)
	{
		if(rooms[i]->rt==0)
		{
			printRoom(rooms[i]);
			current = rooms[i];
		}
	}

	int steps=0; //keeps track of steps
	char str[10]; //var for user input
	strcpy(str, current->roomName);
	char path[100][100]; //holds path take


	while(current->rt!=2)//do	
	{
		//takes user input
		printf("WHERE TO? >");
  	 	scanf("%s", str);
		printf("\n");
		//if imput is time, run second thread
		if (strcmp(str, "time") == 0)
		{
			wakeUp = 1;
			pthread_cond_signal(&wakeCondition);
			readFromMainThread();
		}
		//otherwise , continue in main
		else
		{

			struct room* next = getRoomByName(rooms, str); //returns room for chosen input
			if (next != NULL)
			{
				if (isConnected(current, next))
				{
					strcpy(path[steps++], str); //adds this location to path
					current = next;
					if(current->rt==2)break;	
					printRoom(next); //prints info of current room
				}
				//if rooms are not connected, display error
				else
				{
					printf("HUH I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
					printRoom(current);
				}
			}
			//if file is invalid, dipaly error
			else 
			{
				printf("HUH I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
				printRoom(current);
			}
		}
	}//while( current->rt !=2 ); //while the toom is not the end room
	strcpy(path[steps+1], current->roomName);//copy final room to path
	printf("YOU HAVE FOUND THE END ROOM.\nCONGRATULATIONS! ");
	printf("YOU TOOK %d STEPS. ", steps);
	printf("YOUR PATH TO VICTORY WAS :\n");
	for(i=0;i < steps;i++)
	{	
		printf("%s\n", path[i]);
	};
	//free individual rooms in array
	for (i = 0; i < 7; i++)
	{
		free(rooms[i]);
	}
	//free array
	free(rooms);
	return 0;	
}
