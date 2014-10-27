#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/*
FILE *fopen(const char *filename, const char *mode)

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
	ptr -- This is the pointer to the array of elements to be written.
	size -- This is the size in bytes of each element to be written.
	nmemb -- This is the number of elements, each one with a size of size bytes.
	stream -- This is the pointer to a FILE object that specifies an output stream.
	
size_t strftime (char* ptr, size_t maxsize, const char* format, const struct tm* timeptr );

*/


static FILE* access_log_file;				// This is the CLF file
//static const FILE* general_log_file;	´	// This file could be used for other kind of logging, example crashes 

void accessLog(const char* adress, unsigned int size)				// Thread safe? I think this is okay if we use forking.
{
/*
	Some attempts to get the data from the buffer as needed, seems like it's easier to log just before returning data though.
*/
//	char *getCall, *ipAdress;
//	getCall = strcat(strtok(buffer, " \t\n"), strtok(buffer, " \t\n"));
//	getCall = strtok(NULL, " \t\n");
//	printf("\na:%s\n", getCall);

	// This works for the moment, but ONLY if runned as root, this is probably why we need chroot.
//	access_log_file = fopen("/var/log/testWServer.log", "a+"); 	// TODO undo hardcoded. 	
//	fwrite(buffer, 50, 1, access_log_file);	
	
	
}

// This is some code I found of the lighttpd project for doing the logging (http://www.lighttpd.net) 
// Might be useful for logging time correctly
/*
static int log_buffer_prepare(buffer *b, server *srv) {

	switch(srv->errorlog_mode) {
	case ERRORLOG_PIPE:
	case ERRORLOG_FILE:
	case ERRORLOG_FD:
		if (-1 == srv->errorlog_fd) return -1;
		// cache the generated timestamp 
		if (srv->cur_ts != srv->last_generated_debug_ts) {
			buffer_prepare_copy(srv->ts_debug_str, 255);
			strftime(srv->ts_debug_str->ptr, srv->ts_debug_str->size - 1, "%Y-%m-%d %H:%M:%S", localtime(&(srv->cur_ts)));
			srv->ts_debug_str->used = strlen(srv->ts_debug_str->ptr) + 1;

			srv->last_generated_debug_ts = srv->cur_ts;
		}

		buffer_copy_string_buffer(b, srv->ts_debug_str);
		buffer_append_string_len(b, CONST_STR_LEN(": ("));
		break;
	case ERRORLOG_SYSLOG:
		// syslog is generating its own timestamps 
		buffer_copy_string_len(b, CONST_STR_LEN("("));
		break;
	}

	buffer_append_string(b, filename);
	buffer_append_string_len(b, CONST_STR_LEN("."));
	buffer_append_long(b, line);
	buffer_append_string_len(b, CONST_STR_LEN(") "));

	return 0;
	
}
*/

