#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<syslog.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/mman.h>

#define BUFSIZE 4096
#define MAX_QUEUE 20

struct mime_types
{
	char content_type[80];
	char ending[7][10];
};

void read_config_file(int *port, char *wsroot);
void check_ws_root(char *wsroot);
void parse_arguments(int argc, char **argv, int *port, int *daemon, char *log_file);
void start_server(struct sockaddr_in address, int *port, int *listener);
void wait_for_connection(struct sockaddr_in address, int *listener);
void handle_request(int new_socket);
char *resolve_path(char *uri);
void create_ok_header(char *uri, char *buffer);
char *get_extension(char *path);
void *get_content_type(char *extension, char *content_type);

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
	chdir(resolve_path(wsroot));
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
	if (stat(resolve_path(wsroot), &l_stat) == -1) // if the folder does not exist, create it
	{
		mkdir(resolve_path(wsroot), 0700);
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
	char *type = NULL;
	char *uri = NULL;
	char *httpv = NULL;
	char *resolved = NULL;
	int openfile;

	recv(new_socket, buffer, BUFSIZE, 0);

	temp = buffer;
	type = strsep(&temp, " \r\n"); //Type of request, GET or HEAD are supported
	uri = resolve_path(strsep(&temp, " \r\n")); //Path
	httpv = strsep(&temp, " \r\n"); //HTTP version

	//If it is not HTTP/1.0 or HTTP/1.1, 400
	if ((strcmp(httpv, "HTTP/1.0") != 0 && strcmp(httpv, "HTTP/1.1") != 0))
	{
		strcpy(buffer, "HTTP/1.0 400 Bad Request\r\n");
		send(new_socket, buffer, strlen(buffer), 0);
		goto closing_down;
	}

	resolved = realpath(uri, NULL);
	if(resolved == NULL)
	{
		//File does not exist, 404
		strcpy(buffer, "HTTP/1.0 404 Not Found\r\n");
		send(new_socket, buffer, strlen(buffer), 0);
		goto closing_down;
	}

	//GET requests
	if(strcmp(type, "GET") == 0)
	{
		//Opening requested file
		if((openfile = open(uri, O_RDONLY)) != -1)
		{
			//File opens, 200
			create_ok_header(uri, buffer);
			send(new_socket, buffer, strlen(buffer), 0);
			if(sendfile(new_socket, openfile, NULL, /*filesize*/244) == -1)
			{
				//Error sending file, 500
				strcpy(buffer,"HTTP/1.0 500 Internal Server Error\r\n");
				send(new_socket, buffer, strlen(buffer), 0);
			}
		}
		else
		{
			//Can't open file, 403
			strcpy(buffer, "HTTP/1.0 403 Forbidden\r\n");
			send(new_socket, buffer, strlen(buffer), 0);
		}		
	}
	//HEAD requests
	else if(strcmp(type, "HEAD") == 0)
	{
		//Opening requested file
		if((openfile = open(uri, O_RDONLY)) != -1)
		{
			//File opens, 200
			create_ok_header(uri, buffer);
			send(new_socket, buffer, strlen(buffer), 0);
		}
		else
		{
			//Can't open file, 403
			strcpy(buffer, "HTTP/1.0 403 Forbidden\r\n");
			send(new_socket, buffer, strlen(buffer), 0);
		}
	}
	//All other requests
	else
	{
		//Not implemented, 501
		strcpy(buffer, "HTTP/1.0 501 Not Implemented\r\n");
		send(new_socket,buffer,strlen(buffer),0);
	}
	closing_down:
	free(resolved);
	close(openfile);
	close(new_socket);
}

char *resolve_path(char *uri)
{
	if(strncmp(uri, "/\0", 2) == 0)
	{
		return "index.html";
	}
	else if(strncmp(uri, "/", 1) == 0)
	{
		return uri + 1;
	}
}

void create_ok_header(char *uri, char *buffer)
{
	char content_type[32];
	char *file_extension;
	file_extension = get_extension(uri);
	get_content_type(file_extension, content_type);
	strcpy(buffer,"HTTP/1.0 200 OK\r\nContent-Type: ");
	strcat(buffer, content_type);
	strcat(buffer, "\r\n\r\n");
}

char *get_extension(char *path)
{
	char *dot = strrchr(path, '.');
    if(!dot || dot == path)
	{
		return "";
	}
    return dot + 1;
}

void *get_content_type(char *extension, char *content_type)
{
	//TODO Path to file might be screwed up when chroot works...
	FILE *extension_file = fopen("../supported.extensions", "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	if(extension_file == NULL)
	{
		exit(1);
	}

	while((read = getline(&line, &len, extension_file)) != -1)
	{
		if(line[0] == '#')
		{
			continue;
		}

		if(strncmp(line, extension, strlen(extension)) == 0) //Extension found
		{
			sscanf(line, "%*s %s", content_type); //Get content-type
			break;
		}
	}

	fclose(extension_file);
	if(line)
	{
		free(line);
	}
}
