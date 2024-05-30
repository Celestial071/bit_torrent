#include  <stdio.h>
#include "include/benCdecoder.h"
#include "include/io.h"


int main() {
    torrent_t *torrent = readTorrentfile("torrentfiles/debian-edu-12.5.0-amd64-netinst.iso.torrent");
    if(!torrent) fprintf(stderr, "Error With File reading\n");
    //printf("\n%s\nSize:-%zu\n", torrent->content, torrent->length);
    
    benCode_t *BenCode = BenCodeinit(torrent);
    
    decode(BenCode);
    FREETORRENTFILE(torrent->content, torrent->length);
    free(torrent);
    free(BenCode);
    return 0;
}
