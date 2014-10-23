#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <limits.h>
#include <fcntl.h>

#define BACKLOG 20		// TODO eh?
#define BUFSIZE 1024	// eh?
#ifndef MYDEBUG
//#define MYDEBUG
#endif


// TODO do something about this
extern char request_ok[];
extern char bad_request[];
extern char forbidden[];
extern char not_found[];
extern char internal_error[];
extern char not_implemented[];

FILE* config_file;

int sock;

// Pre thingymagicks
void start_server();
void handle_request(int);
void read_config_file();
void set_ws_root_directory();
void parse_arguments();
void wait_for_connection();

char* executing_directory;
char* ws_root_directory;		// TODO path_max // Webserver root directory
char* listening_port;

int main(int argc, char* argv[])
{
	// Allocate stuff
	ws_root_directory = malloc(PATH_MAX);	// TODO path_max
	listening_port = malloc(5);

	// Default values 
	executing_directory = getenv("PWD");				// Get current directory

	read_config_file();
	set_ws_root_directory();
	parse_arguments(argc, argv);

	printf("\nInitializing server\n");
	printf("Port: %s\n", listening_port);
	printf("Root directory: %s\n", ws_root_directory);
	printf("\nStarting server\n");
	
	// Start the server
	start_server();	
	printf("Ready for connections...\n");
	wait_for_connection();

	return 0;
}

void start_server()
{
	struct addrinfo hints, *res, *p;
	int optval = 1;
	int result = -1;

	memset(&hints, 0, sizeof(hints));	// zero all the memory for hints
	hints.ai_family 	= AF_INET;
	hints.ai_socktype 	= SOCK_STREAM;
	hints.ai_flags 		= AI_PASSIVE;	
	
	/* int getaddrinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **res);
		hostname	= 
		service		=
		hints		=
		res			=
	*/
	result = getaddrinfo(NULL, listening_port, &hints, &res);
	if(result != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}

	for(p = res; p != NULL; p = p->ai_next)
	{
		/* socket(int socket_family, int socket_type, int protocol);
		socket_family	=  
		socket_type		= 
		protocol		= 
		*/
		sock = socket(p->ai_family, p->ai_socktype, 0);		// Create socket
		if(sock == -1)
		{
			// TODO log something here
		}
		
		/*int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
			s 			= socket
			level 		= should be SOL_SOCKET
			optname 	= SO_REUSEADDR, allows other sockets to bind() to this same port
			optval 		= 1 for true, 0 for false
			socklen_t 	= sizeof(optval)
		*/
		result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
		if(result == -1)									
		{
			perror("setsockopt");
			exit(1);
		}

		/* int bind(int socket_descriptor, struct sockaddr *local_address, int address_length);
			socket_descriptor 	= 
			sockaddr			= 
			address_length		= 
		*/
		result = bind(sock, p->ai_addr, p->ai_addrlen);		// Bind socket
		if(result == 0)
		{
			break;
		}
	}

	if(p == NULL)
	{
		perror("socket/bind");
		exit(1);
	}

	freeaddrinfo(res);

	if(listen(sock, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}
}

void handle_request(int sock_current)
{
	int reqlen, i, open_file, bytes_read;
	char *msg_buffer = malloc(BUFSIZE);
	char send_buffer[BUFSIZE];
	char *reqtype, *reqpath, *reqver;
	char *path;
	char rpbuf[PATH_MAX];

	reqlen = recv(sock_current, msg_buffer, BUFSIZE, 0);

	if(reqlen == -1 || reqlen == 0)
	{
		//LOG error
		exit(1);
	}
	else
	{
		reqtype = strtok(msg_buffer, " \t\n");
		
		//If request is not GET, close socket and exit child process
		if(strncmp(reqtype, "GET\0", 4) == 0 || strncmp(reqtype, "get\0", 4) == 0)
		{
			//Split string into tokens http://www.cplusplus.com/reference/cstring/strtok/
			reqpath = strtok(NULL, " \t");
			reqver = strtok(NULL, " \t\n");
			
			// Compare characters of two strings http://www.cplusplus.com/reference/cstring/strncmp/
			if((strncmp(reqver, "HTTP/1.0", 8) != 0 && strncmp(reqver, "HTTP/1.1", 8) != 0))
			{
				//Request not valid, return 400
				if(send(sock_current, bad_request, strlen(bad_request), 0) == -1)
				{
					perror("send, 400");
				}
			}
			else
			{
				if(strncmp(reqpath, "/\0", 2) == 0)
				{
					reqpath = "/index.html";
				}
				
				path = malloc(strlen(ws_root_directory) + strlen(reqpath));
				strcpy(path, ws_root_directory);
				strcat(path, reqpath);
				
				if((open_file = open(path, O_RDONLY)) != -1)
				{
					//File found, return 200 and file
					if(send(sock_current, request_ok, strlen(request_ok), 0) == -1)
					{
						perror("send, 200");
					}
					while((bytes_read = read(open_file, send_buffer, BUFSIZE)) > 0)
					{
						send(sock_current, send_buffer, bytes_read, 0);
					}
				}
				else
				{
					//File not found, return 404
					if(send(sock_current, not_found, strlen(not_found), 0) == -1)
					{
						perror("send, 404");
					}
				}
			}
		}
		else if(strncmp(reqtype, "HEAD\0", 5) == 0 || strncmp(reqtype, "head\0", 5) == 0)
		{
		
		}
		else
		{
			//Not implemented, return 501
			if(send(sock_current, not_implemented, strlen(not_implemented), 0) == -1)
			{
				perror("send, 501");
			}
			//LOG the fact that this wasn't GET or HEAD
			exit(1);
		}
	}
}


void read_config_file()
{
	char file_buffer[BUFSIZE];
	// Read config file
	config_file = fopen(".lab3-config", "r");			// r is flag for reading, file must exist
	if(config_file != NULL)
	{
		while(!feof(config_file))
		{
			fgets(file_buffer, BUFSIZE, config_file);
			if(strncmp(file_buffer, "port=", 5) == 0)
			{
				//TODO clean up the buffer/string
				strcpy(listening_port, file_buffer + 5);
			}
			else if(strncmp(file_buffer, "root=", 5) == 0)
			{
				//TODO clean up the buffer/string
				strcpy(ws_root_directory, executing_directory);
				strcat(ws_root_directory, file_buffer + 5);
				break;
			}
		}
	}
	else
	{
		perror("could not open file");
	}
	fclose(config_file);
}

void set_ws_root_directory()
{
	struct stat l_stat = {0};
	if (stat("wsroot", &l_stat) == -1) 	// if the folder does not exist, create it
	{
		mkdir("wsroot", 0700);
	}
	strcpy(ws_root_directory, executing_directory);		// Set root directory to executing_directory
	strcat(ws_root_directory, "/wsroot");				// Append root folder to the current directory 
}

void parse_arguments(int argc, char* argv[])
{
	char option;
	while((option = getopt(argc, argv, "p:dl:")) != -1)
	{
		switch(option)
		{
			case 'p':
				strcpy(listening_port, optarg);
				break;
			case 'd':
				printf("Starting server as daemon.\n");
				break;
			case 'l':
				printf("Logging to logfile: %s\n", optarg);
				break;
		}
	}
}


void wait_for_connection()
{
	int sock_current;
	struct sockaddr_in client_address, server_address;
	struct sigaction sa;
	int addrlen;
	pid_t pid;
	while(1)
	{
		addrlen = sizeof(client_address);
		if((sock_current = accept(sock, (struct sockaddr*) &client_address, &addrlen)) == -1)
		{
			perror("accept");	// TODO should maybe specify more?
		}
		
		printf("Request from %s\n", inet_ntoa(client_address.sin_addr));

		if((pid = fork()) < 0)						//
		{
			perror("Failed to create process");		//
			close(sock_current);					//
		}
		else if(pid == 0)
		{
			// This is the child process
			//Close parent socket and handle the request
			close(sock);
			handle_request(sock_current);
			close(sock_current);
			exit(0);
		}
		else
		{
			// This is the parent process
			// Kill child process and close child socket
			wait(NULL);
			close(sock_current);
		}
	}
}