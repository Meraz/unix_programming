#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<time.h>
#include<syslog.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/mman.h>

void read_config_file(char *port, char *wsroot);
void set_ws_root_directory(char *path, char *wsroot);
void parse_arguments(int argc, char **argv, char *port, int *daemon, char *log_file);

int main(int argc, char* argv[])
{
	char ws_root_path[256];
	char wsroot[32];
	char port[6];
	int daemon = 0;
	char log_file[32] = "webserv.log";

	//Read values from config
	read_config_file(port, wsroot);
	//Setting root directory
	set_ws_root_directory(ws_root_path, wsroot);
	//Parsing arguments
	parse_arguments(argc, argv, port, &daemon, log_file);

	return 0;
}

void read_config_file(char *port, char *wsroot)
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
			sscanf(line, "%*s %s", port);
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

void set_ws_root_directory(char *path, char *wsroot)
{
	char *executing_directory;
	struct stat l_stat = {0};
	if (stat(wsroot + 1, &l_stat) == -1) 	// if the folder does not exist, create it
	{
		mkdir(wsroot + 1, 0700);
	}

	executing_directory = getenv("PWD");
	strcpy(path, executing_directory);		// Set root directory to executing_directory
	strcat(path, wsroot);				// Append root folder to the current directory 
}

void parse_arguments(int argc, char **argv, char *port, int *daemon, char *log_file)
{
	int opt;
	while((opt = getopt(argc, argv, "p:dl:")) != -1)
	{
		switch(opt)
		{
			case 'p':
				strcpy(port, optarg);
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
