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

int main(int argc, char* argv[])
{
	char wsroot[256];
	char port[6];

	//Read values from config
	read_config_file(port, wsroot);

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
