char request_ok_header[] = "HTTP/1.0 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

char bad_request_header[] = "HTTP/1.0 400 Bad Request\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

char bad_request[] = "HTTP/1.0 400 Bad Request\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<doctype !html><html><head><title>Bad Request</title></head>"
"<body><h1>400 Bad Request</h1></body></html>\r\n";

char forbidden[] = "HTTP/1.0 403 Forbidden\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<doctype !html><html><head><title>Forbidden</title></head>"
"<body><h1>403 Forbidden</h1></body></html>\r\n";

char not_found_header[] = "HTTP/1.0 404 Not Found\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

char not_found[] = "HTTP/1.0 404 Not Found\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<doctype !html><html><head><title>Not Found</title></head>"
"<body><h1>404 Not Found</h1></body></html>\r\n";

char internal_error[] = "HTTP/1.0 500 Internal Server Error\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<doctype !html><html><head><title>Internal Server Error</title></head>"
"<body><h1>500 Internal Server Error</h1></body></html>\r\n";

char not_implemented[] = "HTTP/1.0 501 Not Implemented\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<doctype !html><html><head><title>Not Implemented</title></head>"
"<body><h1>404 Not Implemented</h1></body></html>\r\n";
