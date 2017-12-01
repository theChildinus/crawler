#ifndef CONNSERVER_H
#define CONNSERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "queue.h"
#include "common.h"

char* HttpHeadCreate(const char* strUrl);
char* getParamFromUrl(const char* strUrl);
char* getHostAddrFromUrl(const char* strUrl);

#endif // !

