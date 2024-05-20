#include "include/io.h"


torrent_t *readTorrentfile(char *path) {
    torrent_t *torrentFile = malloc(sizeof(torrent_t));
    int fd = open(path, O_RDONLY);

    if(fd == -1) {
        perror("File opening failed");
        return NULL;
    }

    struct stat fileinfo = {0};

    if(fstat(fd, &fileinfo) == -1) {
        close(fd);
        perror("Fstat failed");
        return NULL;
    }

    off_t fileSize = fileinfo.st_size;
    torrentFile->length = fileinfo.st_size;
    torrentFile->content = (char *) mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);

    if(torrentFile->content == MAP_FAILED) {
        close(fd);
        perror("Failed to allocate enough memory");
        return NULL;
    }

    return torrentFile;
}
