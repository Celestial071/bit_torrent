#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define FREETORRENTFILE(file, size) do{munmap(file, size);}while(0)

typedef struct Torrent{
    char *content;
    size_t length;
}torrent_t;

torrent_t *readTorrentfile(char *path);
#endif