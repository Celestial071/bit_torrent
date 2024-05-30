#ifndef BENCDECODER_H
#define BENCDECODER_H

#include "io.h"
#include <stdbool.h>

typedef enum {
    BENCODE_INT,
    BENCODE_STRING,
    BENCODE_LIST,
    BENCODE_DICT,
}btype;


//forward declaration
struct ben_dict;
struct ben_node;

typedef struct ben_dict {
    char *key;
    struct ben_node *val;
}ben_dict;

typedef struct ben_node{
    btype type;
    union {
        char *s;
        long long i;
        struct{
            size_t size;
            struct ben_node **items;
        }list;
        struct{ 
            size_t size;
            struct ben_dict *pairs;
        }dic;
    }val;
}ben_node;

typedef struct beNCode{
    torrent_t *torrent;
    size_t index;
}benCode_t;

benCode_t *BenCodeinit(torrent_t *torrent);
void displaybencode(ben_node *benNode);
void freeBencode(ben_node *benNode);
void decode(benCode_t *bencode);
#endif
