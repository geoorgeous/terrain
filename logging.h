#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#define ENABLE_LOGGING
#ifdef ENABLE_LOGGING
#define LOG(fmt, ...) fprintf(stdout, fmt"\n", ##__VA_ARGS__)
#define LOGFATAL(fmt, ...) do { LOG(fmt, ##__VA_ARGS__); getchar(); exit(-1); } while(0)
#else
#define LOG(fmt, ...)
#define LOGFATAL(fmt, ...)
#endif

#endif