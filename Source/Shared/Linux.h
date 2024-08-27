#pragma once

#ifndef WIN32
#include <unistd.h>
#include <limits.h>
#include <dlfcn.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_PATH PATH_MAX
#define Sleep(ms) usleep((ms) * 1000)
#define ExitProcess(code) _exit(code)

#define THREAD_PRIORITY_NORMAL 0

#define stricmp strcasecmp
#endif
