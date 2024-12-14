#ifndef _HEADERS_
#define _HEADERS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <error.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include "helper.h"
#include "messages.h"
#include "game.h"

#ifdef USE_TCP
#include "tcp.h"
#endif

#ifdef USE_UDP
#include "udp.h"
#endif


#endif