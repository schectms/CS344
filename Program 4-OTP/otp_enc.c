/*
* SHIFRA SCHECTMAN
* CS344-Program 4
* OTP_ENC
*This program takes 2 files and a port as arguments and sends
* information to the server
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 

/*
* FUNCTION_NAME: validChars
* PARAMETERS: string variable
* This program checks a string variable to ensure
* it is made up of only uppercase letters, spaces and
* new lines
*/
int validChars(char* s)
{
	int i; //initialize i 
	//loop through string and check each character
	for (i = 0; i < strlen(s); i++)
	{
		if (!(s[i] >= 'A' && s[i] <= 'Z' || s[i] == ' ' || s[i] == '\n'))
			return 0; //return 0 if not  all chars are valid
	}

	return 1; //otherwise return 1
}

void error(const char* msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char* argv[])
{
	//initializes variables
	int socketFD, portNumber, charsWritten, charsRead;
	uint32_t dataSize = 0;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char* plaintext = 0;
	char* key = 0;
	long length;
	long sizePT;
	char buffer[140000];

	//Source code for reading from file 
	//https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c

	// Get plaintext from file
	FILE* f = fopen(argv[1], "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		plaintext = malloc(length);
		if (plaintext)
		{
			fread(plaintext, 1, length, f);
		}	
		sizePT = strlen(plaintext);
		fclose(f);
	}

	//error if file does not exist
	else
	{
		fprintf(stderr, "%s", "file is invalid");
		exit(1);
	}

	// Get key from file
	f = fopen(argv[2], "rb");
	if (f)
	{
		fseek(f, 0L, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		key = malloc(length);
		if (key)
		{
			fread(key, 1, length, f);
		}
		fclose(f);
		
	}

	//error if file does not exist
	else
	{
		fprintf(stderr, "%s", "file is invalid");
		exit(1);
	}

	//checks the content of plaintext and key file
	if (validChars(plaintext) == 0 || validChars(key) == 0)
	{
		fprintf(stderr, "%s", "file is invalid");
		exit(1);
	}

	//makes sure that the key is not shorter than plaintext
	if (strlen(key) < strlen(plaintext))
	{
		fprintf(stderr, "%s%s%s", "Error: ", argv[2], " is too short\n");
		exit(1);
	}
	else
	{
		// Check usage & args
		if (argc < 3)
		{
			fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
			exit(0);
		}

		// Set up the server address struct
		memset((char*)& serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
		portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
		serverAddress.sin_family = AF_INET; // Create a network-capable socket
		serverAddress.sin_port = htons(portNumber); // Store the port number
		serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
		if (serverHostInfo == NULL)
		{
			fprintf(stderr, "CLIENT: ERROR, no such host\n");
			exit(0);
		}
		memcpy((char*)& serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

		// Set up the socket
		socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
		if (socketFD < 0)
		{
			error("CLIENT: ERROR opening socket");
		}

		// Connect to server
		if (connect(socketFD, (struct sockaddr*) & serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		{
			error("CLIENT: ERROR connecting");
		}
			   
		long enc=1; // variable that tracks if client is enc
		long validConn; //variable to track if connection is valid
				
		// Send the enc variable over
		send(socketFD, &enc, sizeof(enc), 0);
		
		//receive back validConnection variable
		charsWritten = recv(socketFD, &validConn, sizeof(validConn), 0);
	
		//if the connection is valid
		if (validConn == 1)
		{
			// Send the size of plainText over
			dataSize = htonl(sizePT);
			charsWritten = send(socketFD, &dataSize, sizeof(dataSize), 0);
	
			if (charsWritten < 0)
			{
				error("CLIENT: ERROR writing to socket");
			}

			// Send the actual plainText
			int bytesSent = 0;
			bytesSent += send(socketFD, plaintext, sizePT, 0);
				
			if (charsWritten < 0)
			{
				error("CLIENT: ERROR writing to socket");
			}

			//send key
			bytesSent = 0;
			bytesSent += send(socketFD, key, sizePT, 0);

			if (charsWritten < 0)
			{
				error("CLIENT: ERROR writing to socket");
			}

			memset(buffer, '\0', sizeof(buffer)); //reset buffer
			int i = 0; 

			// Get return message from server
			while (i < (sizePT-1))
			{
				i += recv(socketFD, buffer + i, sizePT-i, 0);
			}
			
			printf("%s\n", buffer); //prints ciphertext to output			

			close(socketFD); // Close the socket
			free(plaintext);
			free(key);
			exit(0);
			return 0;
		}

		//if connection is invalid, exit and error
		else
		{
			fprintf(stderr, "%s%d", "error cannot connect to otp_enc_d on port \n", portNumber);
			close(socketFD); // Close the socket
			free(plaintext);
			free(key);
			exit(2);

		}
	}
}
