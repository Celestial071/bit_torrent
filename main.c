#include  <stdio.h>
#include "include/io.h"


int main() {
    torrent_t *torrent = readTorrentfile("debian-edu-12.5.0-amd64-netinst.iso.torrent");
    if(!torrent) fprintf(stderr, "Error With File reading\n");
    printf("content:-\n%s\nSize:-%zu\n", torrent->content, torrent->length);
    FREETORRENTFILE(torrent->content, torrent->length);
    free(torrent);
    return 0;

}
