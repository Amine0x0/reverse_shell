#ifndef RADAR_H
#define RADAR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>
typedef struct {
    char *name;
}   eyes;

#define __state__ "running..."

#endif