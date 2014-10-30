#include "utils.h"

#define BUFSIZE 4096
#define MAX_QUEUE 20

void start_server(struct sockaddr_in address, int *port, int *listener);
void wait_for_connection(struct sockaddr_in address, int *listener);
void handle_request(int new_socket);

int main(int argc, char* argv[])
{
	struct sockaddr_in address;
	char *wsroot = NULL, wsroot_folder[32];
	int port;
	int daemon = 0;
	char log_file[32] = "webserv.log";
	int listener;

	//Read values from config
	read_config_file(&port, wsroot_folder);
	//Get full directory path
	wsroot = get_full_path(wsroot_folder);
	//Creates preferred root dir, if it does not exist
	check_ws_root(wsroot);
	//Parsing arguments
	parse_arguments(argc, argv, &port, &daemon, log_file);
	//Open/create file for logging
	write_log(log_file, 0, NULL, NULL, NULL, 0, 0); 
	//Open supported.extensions file for later use
	get_content_type(NULL, NULL);
	//Set current dir and root it
	chdir(wsroot);
	//TODO This SHOULD work...
	/*if(chroot("/home/ironman/gitrepos/unix_programming/webserver/wsroot") != 0)
	{
		perror("chroot");
		exit(1);
	}
	setuid(1000);*/
	//If daemon flag is set, run as daemon
	if(daemon)
	{
		daemonize();
	}
	//Start server
	start_server(address, &port, &listener);
	//Wait for connections
	wait_for_connection(address, &listener);
	//Close listener socket
	close(listener);

	return 0;
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
			wait(NULL);
			close(new_socket);
		}
	}
}

void handle_request(int new_socket)
{
	char buffer[BUFSIZE];
	char *temp = malloc(BUFSIZE);
	char *temp2 = malloc(BUFSIZE);
	char *logger_line = NULL;
	char *type = NULL;
	char *uri = NULL;
	char *httpv = NULL;
	char *rp = NULL;
	int openfile;
	int bytes_sent = 0;

	recv(new_socket, buffer, BUFSIZE, 0);

	strcpy(temp, buffer);
	type = strtok(temp, " "); //Type of request, GET or HEAD are supported
	uri = resolve_path(strtok(NULL, " ")); //Path
	httpv = strtok(NULL, "\r"); //HTTP version

	strcpy(temp2, buffer);
	logger_line = strtok(temp2, "\r");

	//If it is not HTTP/1.0 or HTTP/1.1, 400
	if ((strcmp(httpv, "HTTP/1.0") != 0 && strcmp(httpv, "HTTP/1.1") != 0))
	{
		strcpy(buffer, "HTTP/1.0 400 Bad Request\r\n");
		bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
		write_log(NULL, new_socket, "-", "-", logger_line, 400, bytes_sent);
		goto closing_down;
	}

	rp = realpath(uri, NULL);
	if(rp == NULL)
	{
		//File does not exist, 404
		strcpy(buffer, "HTTP/1.0 404 Not Found\r\n");
		bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
		write_log(NULL, new_socket, "-", "-", logger_line, 404, bytes_sent);
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
			bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
			if((bytes_sent += sendfile(new_socket, openfile, NULL, /*filesize*/512)) == -1)
			{
				//Error sending file, 500
				strcpy(buffer,"HTTP/1.0 500 Internal Server Error\r\n");
				bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
				write_log(NULL, new_socket, "-", "-", logger_line, 500, bytes_sent);
			}
			else
			{
				write_log(NULL, new_socket, "-", "-", logger_line, 200, bytes_sent);
			}
		}
		else
		{
			//Can't open file, 403
			strcpy(buffer, "HTTP/1.0 403 Forbidden\r\n");
			bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
			write_log(NULL, new_socket, "-", "-", logger_line, 403, bytes_sent);
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
			bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
			write_log(NULL, new_socket, "-", "-", logger_line, 200, bytes_sent);
		}
		else
		{
			//Can't open file, 403
			strcpy(buffer, "HTTP/1.0 403 Forbidden\r\n");
			bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
			write_log(NULL, new_socket, "-", "-", logger_line, 403, bytes_sent);
		}
	}
	//All other requests
	else
	{
		//Not implemented, 501
		strcpy(buffer, "HTTP/1.0 501 Not Implemented\r\n");
		bytes_sent = send(new_socket, buffer, strlen(buffer), 0);
		write_log(NULL, new_socket, "-", "-", logger_line, 501, bytes_sent);
	}

	closing_down:
	free(rp);
	free(temp);
	free(temp2);
	close(openfile);
	close(new_socket);
}
