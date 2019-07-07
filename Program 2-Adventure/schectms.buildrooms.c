/*
 * Shifra Schectman
 * CS344-Program 2
 * buildrooms
 * This program generates files of rooms for a text-based game
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

//creates room structure
struct room
{
	char* roomName;
	char* rT;
	int numberOfConnections;
	struct room* connectedRooms[6];
};

/*
 * Function Name: isGraphFull
 * Function Parameters: array of rooms
 * returns an integer value of 1 if the graph 
 * is full and 0 if not
 */
int IsGraphFull(struct room roomArray[7])
{
	int i;
	//loops through array and checks the number of Connections
	for(i=0;i<7;i++)
	{
		if(roomArray[i].numberOfConnections>=3)
			continue;
		else
			return 0;
	} 		
	return 1;
}

/*
 * Function Name: GetRandomRoom
 * Function Parameters: array of rooms
 * gets a random room from the array and returns it
 */
struct room* GetRandomRoom(struct room roomArray[7])
{
	int randomNumber=rand()%7;
	return &roomArray[randomNumber];
}

/*
 * Function Name: canAddConnection
 * Function Parameters: pointer to a room
 * returns an integer value of 1 if 
 * more connections can be made and 0
 * if not
 */
int canAddConnection(struct room* x)
{
	if (x->numberOfConnections>=6)
		return 0;
	else
		return 1;
}

/*
 * Function Name: IsSameRoom
 * Function Parameters: pointer to room x, and room y
 * returns an integer value of 1 if x and y are the same
 * and 0 if not
 */
int IsSameRoom(struct room* x, struct room* y)
{
	if (x->roomName==y->roomName)
		return 1;
	else
		return 0;
}

/*
 * Function Name: connectRoom
 * Function Parameters: pointer to room x and room y
 * connects rooms x and y and increments 
 * the number of connections
 */
void connectRoom(struct room* x, struct room* y)
{
	x->connectedRooms[x->numberOfConnections]=y;
	x->numberOfConnections++;
}

/*
 * Function Name: ConnectionAlreadyExists
 * Function Parameters: pointer to room x and room y
 * return an integer value of 1 if the connection 
 * already exists and 0 if not
 */
int ConnectionAlreadyExists(struct room* x, struct room* y)
{
	int i;
	//loops through array to check if connection is there
	for(i=0;i<x->numberOfConnections;i++)
	{
		if(x->connectedRooms[i]==y)	
			return 1;
		else
			continue;
	}
	return 0;
}

/*
 * Function Name: addRandomConnection
 * Function Parameters: array of rooms
 * adds a connection between 2 rooms
 */
void addRandomConnection(struct room roomArray[7] )
{
	struct room *A;
	struct room *B;

	while(1)
	{
		A=GetRandomRoom(roomArray);
		if(canAddConnection(A) == 1)
		break;
	}

	do
	{
		B=GetRandomRoom(roomArray);
	}
	while(
		canAddConnection(B) ==0 || 
		IsSameRoom(A, B) ==1 ||
		ConnectionAlreadyExists(A,B) == 1 
	);
	connectRoom(A, B);
	connectRoom(B, A);
}

//main function
int main()
{
	srand(time(NULL));
	int i=0;
	
	//hard-code names for rooms		
	char roomNames[10][9]={"Hall", "Library", "Kitchen", "Study", "Sunroom"
	, "Room6", "Dining", "Ballroom", "Lounge", "Bedroom"}; 	

	char temp [9];
	int j;
	
				
	for(i=9;i>0;--i)
	{
		j=rand()% (i+1);
		strcpy(temp,roomNames[j]);
		strcpy(roomNames[j], roomNames[i]);
                strcpy(roomNames[i], temp);
	}
			
	//initialize array of rooms
	struct room roomArray[7]=
	{
		{roomNames[0], "START_ROOM", 0, NULL} ,
		{roomNames[1], "MID_ROOM", 0, NULL},
		{roomNames[2], "MID_ROOM", 0, NULL},
		{roomNames[3], "MID_ROOM", 0, NULL},
		{roomNames[4], "MID_ROOM", 0, NULL},
		{roomNames[5], "MID_ROOM", 0, NULL},
		{roomNames[6], "END_ROOM", 0, NULL}
	};
	
	//form room connections	
	while(IsGraphFull(roomArray)==0)
	{
		addRandomConnection(roomArray);
	}

	//generate directory
	int pid=getpid();
        char *dirname = "schectms.rooms.";
        char fileName1[pathconf(dirname, _PC_NAME_MAX)];
        snprintf(fileName1, sizeof(fileName1), "%s%d", dirname, pid);
        mkdir(fileName1, 0755);
	
	//generate files and format with information
	FILE *file1;
	char fp[260];
	for(i=0;i<7;i++)
	{
		snprintf(fp, sizeof(fp), "%s%i/%s", dirname, pid, roomArray[i].roomName);
		file1=fopen(fp, "w+");
		fprintf(file1, "ROOM NAME: %s \n", roomArray[i].roomName);
		
		int j;
		for(j=0;j<roomArray[i].numberOfConnections;j++)
		{
			fprintf(file1, "CONNECTION %i : ", (j+1));
			fprintf(file1, roomArray[i].connectedRooms[j]->roomName);
			fprintf(file1, "\n");
		}
		fprintf(file1, "ROOM TYPE: %s", roomArray[i].rT);
		fclose(file1);
	}
}
