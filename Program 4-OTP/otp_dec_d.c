/*
* SHIFRA SCHECTMAN
* CS344-Program 4
* OTP_DEC_D
*This program takes a port as an argument and runs a server which will
* decrypt a message received from client and send it back
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(const char* msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char* argv[])
{
	//initialize variables
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead=0;
	socklen_t sizeOfClientInfo;
	char plaintext[70000];
	char key[70000];
	struct sockaddr_in serverAddress, clientAddress;

	// Check usage & args
	if (argc < 2)
	{
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	}

	memset((char*)& serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0)
	{
		error("ERROR opening socket");
	}

	//allow more than one send/receive
	int val = 1;
	setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr*) & serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
	{
		error("ERROR on binding");
	}
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	uint32_t plainTextSize;
	long keySize;

	while (1)
	{
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr*) & clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0)
		{
			error("ERROR on accept");
		}

		pid_t pid = -5; //initialize spawnPid to garbage variable
		pid = fork(); //fork process

		switch (pid)
		{
			//if the fork fails
		case -1:;
			perror("Hull Breach!\n");
			fflush(stdout);
			exit(1);
			break;
			//child case
		case 0:;
			int i = 0;
			int length1;
			long enc=0; //tracks if enc/dec is trying to connect
			recv(establishedConnectionFD, &enc, sizeof(enc), 0);
			
			//if the client connecting is dec
			if (enc == 0)
			{
				enc = 1; //send back 1-indicating proper connection was formed
				send(establishedConnectionFD, &enc, sizeof(enc), 0);
			}
			//if the client connecting is enc
			else
			{
				enc = 0; //send back 0, indicating conbnection was not good
				send(establishedConnectionFD, &enc, sizeof(enc), 0); //it is not ok;
				//close connectioin
				close(establishedConnectionFD);
				close(listenSocketFD);
				exit(2);
			}

			// Get the plaintext size from the client
			charsRead += recv(establishedConnectionFD, &plainTextSize, sizeof(plainTextSize), 0);
			plainTextSize = ntohl(plainTextSize);
			bzero(plaintext, sizeof(plaintext));
			bzero(key, sizeof(key));
			

			// Get the actual plaintext and key from client
			int bytesRead = 0;
			while (bytesRead < (plainTextSize))
			{
				bytesRead += recv(establishedConnectionFD, plaintext + bytesRead, plainTextSize - bytesRead, 0);
			}
			bytesRead = 0;
			while (bytesRead < (plainTextSize))
			{
				size_t x = recv(establishedConnectionFD, key + bytesRead, plainTextSize - bytesRead, 0);
				if (x == -1) {
					break;
				}
				bytesRead += x;
			}

			int readIn = 0; //tracks number of chars read from buffer
			i = 0;

			int j = 0;//for assigning ciphertext
			int numPT = 0; //for ascii number of p/t
			int numK = 0; //for ascii number of key

			char ciphertext[70000];
			//loops through and decrypts
			for (i=0; i < plainTextSize - 1; i++)
			{
				if (plaintext[i] != '\0' && key[i] !='\0')
				{
					if (plaintext[i] != ' ')
					{
						numPT = plaintext[i] - 64;
					}
					else
					{
						numPT = plaintext[i] - 5;
					}

					if (key[i] != ' ')
					{
						numK = key[i] - 64;
					}
					else
					{
						numK = key[i] - 5;
					}
					int num = (numPT - numK);
					if (num < 0)
					{
						num += 27;
					}

					if (num != 27)
					{
						num += 64;
					}
					else
					{
						num += 5;
					}


					char c = num;
					if (c == '@')
					{
						c -= ' ';
					}
					ciphertext[j] = c;
					j++;

				}
			}
			


			// Send a Success message back to the client

			charsRead = send(establishedConnectionFD, ciphertext, plainTextSize, 0); // Write to the server

			if (charsRead < 0) error("CLIENT: ERROR writing to socket");
			if (charsRead < (plainTextSize - 1)) printf("CLIENT: WARNING: Not all data written to socket!\n");

			close(establishedConnectionFD);
			close(listenSocketFD); // Close the listening socket

			exit(0);
			break;
			return 0;
			//parent
		default:
			waitpid(pid, NULL, WNOHANG);
		}


	}
}

