/* This file contains functions supporting the web server. */
#include "utils.h"

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

char *get_full_path(char *folder)
{
	char *executing_directory = getenv("PWD");
	return strcat(executing_directory, folder);
}

void check_ws_root(char *wsroot)
{
	struct stat l_stat = {0};
	if (stat(wsroot, &l_stat) == -1) // if the folder does not exist, create it
	{
		mkdir(wsroot, 0700);
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

void write_log(char *file_name, int sockfd, char *ident, char *auth, char *request, int status, int bytes) 
{
	static FILE *file = NULL;
	if(file == NULL) 
	{
		file = fopen(file_name, "a+");
	} 

	if(sockfd == 0) 
	{
		return;
	}
	
	struct sockaddr_in client;
	int c_len = sizeof(client);
	char buf[80];	

	//Getting time stamp
	time_t result;
	result = time(NULL);
	struct tm* brokentime = localtime(&result);
	char now[32];

	getpeername(sockfd, (struct sockaddr*)&client, &c_len);
	
	inet_ntop(AF_INET, (struct sockaddr*)&client.sin_addr, buf, sizeof(buf));
	
	//Writing data to file
	strftime(now, 32, "%d/%b/%Y:%T %z", brokentime);
	fprintf(file, "%s %s %s [%s] \"%s\" %d %d \n", buf, ident, auth, now, request, status, bytes);
	fflush(file);	
}

void daemonize()
{
	//http://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
	pid_t pid;

    //Fork off the parent process
    if((pid = fork()) < 0)
	{
        exit(1);
	}
    else if(pid > 0)
	{
		//If parent, terminate
        exit(0);
	}

	//Create new session
    setsid();

    //Fork again, to make sure session leader is gone.
    if((pid = fork()) < 0)
	{
		exit(1);
	}
	else if(pid > 0)
	{
		exit(0);
	}

	//Set file permissions for daemon
    umask(0);

	//Close all open file descriptors
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>0; x--)
    {
        close (x);
    }
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

void get_content_type(char *extension, char *content_type)
{
	static FILE *extension_file;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	printf("Inside get_content_type...\n");

	if(extension == NULL)
	{
		printf("extension == null\n");
		extension_file = fopen("supported.extensions", "r");
	}
	else
	{
		printf("extension != null\n");
		while((read = getline(&line, &len, extension_file)) != -1)
		{
			printf("while!\n");
			if(line[0] == '#')
			{
				continue;
			}

			printf("extension: %s\n", extension);
			printf("line: %s\n", line);

			if(strncmp(line, extension, strlen(extension)) == 0) //Extension found
			{
				printf("Found match!\n");
				sscanf(line, "%*s %s", content_type); //Get content-type
				break;
			}
		}
	}
	rewind(extension_file);
	free(line);
}
