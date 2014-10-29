#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<syslog.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/mman.h>

#include<errno.h>

#define BUFSIZE 4096
#define MAX_QUEUE 20

void read_config_file(int *port, char *wsroot);
void check_ws_root(char *wsroot);
void parse_arguments(int argc, char **argv, int *port, int *daemon, char *log_file);
void start_server(struct sockaddr_in address, int *port, int *listener);
void wait_for_connection(struct sockaddr_in address, int *listener);
void handle_request(int new_socket);

int main(int argc, char* argv[])
{
	struct sockaddr_in address;
	char wsroot[32];
	int port;
	int daemon = 0;
	char log_file[32] = "webserv.log";
	int listener;

	//Read values from config
	read_config_file(&port, wsroot);
	//Creates preferred root dir, if it does not exist
	check_ws_root(wsroot);
	//Parsing arguments
	parse_arguments(argc, argv, &port, &daemon, log_file);
	
	//Set current dir
	//TODO, fix this + 1 crap...
	chdir(wsroot + 1);
	//TODO Set chroot...

	// TODO Add daemon stuff

	//Start server
	start_server(address, &port, &listener);
	//Wait for connections
	wait_for_connection(address, &listener);
	//Close listener socket
	close(listener);

	return 0;
}

void read_config_file(int *port, char *wsroot)
{
	FILE *config_file = fopen(".lab3-config", "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	if(config_file == NULL)
	{
		//TODO Log
		exit(1);
	}

	while((read = getline(&line, &len, config_file)) != -1)
	{
		if(line[0] == '#')
		{
			continue;
		}

		if(strncmp(line, "port", 4) == 0) //Store port number
		{
			sscanf(line, "%*s %d", port);
		}
		else if(strncmp(line, "root", 4) == 0) //Store root-folder
		{
			sscanf(line, "%*s %s", wsroot);
		}
	}
	
	fclose(config_file);
	if(line)
	{
		free(line);
	}
}

void check_ws_root(char *wsroot)
{
	char *executing_directory;
	struct stat l_stat = {0};
	if (stat(wsroot + 1, &l_stat) == -1) // if the folder does not exist, create it
	{
		mkdir(wsroot + 1, 0700);
	}
}

void parse_arguments(int argc, char **argv, int *port, int *daemon, char *log_file)
{
	int opt;
	while((opt = getopt(argc, argv, "p:dl:")) != -1)
	{
		switch(opt)
		{
			case 'p':
				*port = atoi(optarg);
				break;
			case 'd':
				*daemon = 1;
				break;
			case 'l':
				strcpy(log_file, optarg);
				break;
		}
	}
}

void start_server(struct sockaddr_in address, int *port, int *listener)
{
	int opt = 1;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(*port);
	
	*listener = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(*listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	bind(*listener, (struct sockaddr *) &address, sizeof(address));
	
	listen(*listener, MAX_QUEUE);
}

void wait_for_connection(struct sockaddr_in address, int *listener)
{
	int new_socket;
	pid_t pid;

	int addrlen = sizeof(address);
	while(1)
	{
		new_socket = accept(*listener, (struct sockaddr*) &address, &addrlen);
		
		if((pid = fork()) < 0)
		{
			//Forking failed
			close(new_socket);
		}
		else if(pid == 0)
		{
			// This is the child process
			//Close parent socket and handle the request
			close(*listener);
			handle_request(new_socket);
			close(new_socket);
			exit(0);
		}
		else
		{
			// This is the parent process
			// Kill child process and close child socket
			//TODO Don't wait, just keep accepting new connections.
			wait(NULL);
			close(new_socket);
		}
	}
}

void handle_request(int new_socket)
{
	char buffer[BUFSIZE];
	char *temp = NULL;
	char *uri = NULL;
	char *httpv = NULL;
	char *resolved = NULL;
	int openfile;

	recv(new_socket, buffer, BUFSIZE, 0);

	if(strncmp(buffer, "GET", 3) == 0)
	{
		temp = buffer;
		uri = strsep(&temp, " \r\n"); //First part is "GET", we want the path
		uri = strsep(&temp, " \r\n"); //Path
		httpv = strsep(&temp, " \r\n"); //HTTP version

		//If it is not HTTP/1.0 or HTTP/1.1, send Bad Request
		if ((strcmp(httpv, "HTTP/1.0") != 0 && strcmp(httpv, "HTTP/1.1") != 0))
		{
			strcpy(buffer, "HTTP/1.0 400 Bad Request\r\n");
			send(new_socket, buffer, strlen(buffer), 0);
		}
		
		//TODO REALPATH hates ALL teh paths! <3
		/*resolved = realpath(uri, NULL);
		printf("uri: %s REALPATH: %s\n", uri, resolved);*/
		/*if(resolved == NULL)
		{
			strcpy(buffer, "HTTP/1.0 403 Forbidden\r\n");
			send(new_socket, buffer, strlen(buffer), 0);
		}*/

		if(strcmp(uri, "/") == 0)
		{
			uri = "index.html";
		}
		
		//Check if the requested file exists
		//It does, 200
		if((openfile = open(uri, O_RDONLY)) != -1)
		{
			strcpy(buffer, "HTTP/1.0 200 OK\r\n\r\n");
			send(new_socket, buffer, strlen(buffer), 0);
			sendfile(new_socket, openfile, NULL, /*filesize*/244);
		}
		//It does not exist, 404
		else
		{
			strcpy(buffer, "HTTP/1.0 404 Not Found\r\n");
			send(new_socket, buffer, strlen(buffer), 0);
		}		
	}
	else if(strncmp(buffer, "HEAD", 4) == 0)
	{
		printf("HEAD request...\n");
	}
	else
	{
		strcpy(buffer, "HTTP/1.0 501 Not Implemented\r\n");
		send(new_socket,buffer,strlen(buffer),0);
	}
}
