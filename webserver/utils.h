/* This file contains functions supporting the web server. */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<syslog.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/mman.h>
#include<time.h>
#include<signal.h>

void read_config_file(int *port, char *wsroot);
char *get_full_path(char *folder);
void check_ws_root(char *wsroot);
void write_log(char *file_name, int sockfd, char *ident, char *auth, char *request_type, char *request_file, int status, int bytes);
void daemonize();
char *resolve_path(char *uri);
void create_ok_header(char *uri, char *buffer);
char *get_extension(char *path);
void *get_content_type(char *extension, char *content_type);
