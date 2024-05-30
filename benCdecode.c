#include "include/benCdecoder.h"
#include "include/io.h"
#include <string.h>

/*forward declaring cuz decodeval function needs these two definition but they also need decodeval*/

ben_node *decodedict(char *str, size_t *index);
ben_node *decodeList(char *str, size_t *index);

benCode_t *BenCodeinit(torrent_t *torrent) {
    benCode_t *temp = malloc(sizeof(benCode_t));
    temp->index = 0;

    temp->torrent = torrent;
    return temp;
}

ben_node *decodeStr(char *str, size_t *index){
    ben_node *benString = malloc(sizeof(*benString));
    benString->type = BENCODE_STRING;
    char *endaddr;
    long length = strtol(&str[*index], &endaddr, 10);

    *index = endaddr - str;
    if(str[*index] == ':')  (*index)++; //consume ':'

    benString->val.s = malloc(length+1);
    memcpy(benString->val.s, &str[*index], length);    

    benString->val.s[length] = 0;
    *index += length;

    return benString;
}


ben_node *decodeLen(char *str, size_t *index){
    (*index)++; //consume "i"
    ben_node *benlen = malloc(sizeof(*benlen));
    benlen->type = BENCODE_INT;
    char *endptr;
    long length = strtol(&str[*index], &endptr, 10);

    benlen->val.i = length; //autocast no problem

    *index = endptr - str;
    if(str[*index] == 'e')  (*index)++; //consume "e"
    return benlen;
}

char *copy_str(char *src){
    size_t len = strlen(src);
    char *dest = malloc(len+1);
    strcpy(dest, src);
    return dest;
}

ben_node *decodeVal(char *str, size_t *index){
    switch(str[*index]) {
        case 'd':
            return decodedict(str, index);
        case 'i':
            return decodeLen(str, index);
        case '0' ... '9':
            return decodeStr(str, index);
        case 'l':
            return decodeList(str, index);
        default:
            return NULL;
    }
}

ben_node *decodeList(char *str, size_t *index){
    (*index)++; //consume 'l'
    ben_node *list = malloc(sizeof(*list));
    list->type = BENCODE_LIST;

    list->val.list.size = 0;
    list->val.list.items = NULL;
    while(str[*index] != 'e'){
        ben_node *item = decodeVal(str, index);
        if(list->val.list.size == 0){
            list->val.list.items = malloc(sizeof(ben_node));
        }else{
            list->val.list.items = realloc(list->val.list.items, (list->val.list.size+1) *sizeof(ben_node)); 
        }

        list->val.list.items[list->val.list.size++] = item;
    }

    (*index)++; //consume 'e'
    return list;
}

ben_node *decodedict(char *str, size_t *index){
    (*index)++; //consume 'd'
    ben_node *dict = malloc(sizeof(*dict));
    dict->type = BENCODE_DICT;
    dict->val.dic.size = 0;
    dict->val.dic.pairs = NULL;

    while(str[*index] != 'e'){
        ben_node *dictKey = decodeStr(str, index);
        if(dict->val.dic.size == 0){
            dict->val.dic.pairs = malloc(sizeof(ben_dict));
        }else{
            dict->val.dic.pairs = realloc(dict->val.dic.pairs, (dict->val.dic.size + 1)*sizeof(ben_dict));
        }
        dict->val.dic.pairs[dict->val.dic.size].key = copy_str(dictKey->val.s);
        dict->val.dic.pairs[dict->val.dic.size++].val = decodeVal(str, index);
        free(dictKey); 
    }
    (*index)++; //consume 'e'
    return dict;
}

void freeBencode(ben_node *benNode){
    if(benNode == NULL){
        return ;
    }

    switch((int)benNode->type){
        case BENCODE_STRING:
            free(benNode->val.s);
            break;
        case BENCODE_INT:
            break;
        case BENCODE_LIST:
            for(size_t i = 0; i < benNode->val.list.size; i++){
                freeBencode(benNode->val.list.items[i]);
            }
            free(benNode->val.list.items);
            break;
        case BENCODE_DICT:
            for(size_t i = 0; i < benNode->val.dic.size; i++){
                free(benNode->val.dic.pairs[i].key);
                freeBencode(benNode->val.dic.pairs[i].val);
            }
            free(benNode->val.dic.pairs);
            break;
    }
    free(benNode);
}

void displaybencode(ben_node *node){
    if(node == NULL)
        return ;
    switch((int)node->type){
        case BENCODE_STRING:
            printf("%s\n", node->val.s);
            break;
        case BENCODE_INT:
            printf("%llu\n", node->val.i);
            break;
        case BENCODE_LIST:
            for(size_t i = 0; i < node->val.list.size; i++){
                displaybencode(node->val.list.items[i]);
            }
            break;
        case BENCODE_DICT:
            for(size_t i = 0; i < node->val.dic.size; i++){
                printf("%s \t", node->val.dic.pairs[i].key);
                displaybencode(node->val.dic.pairs[i].val);
            }
            break;
        default:
            fprintf(stderr, "No such token or just putting this as a fail safe lol\n");
            break;
    }
}

void decode(benCode_t *bencode){
    ben_node **nodes = NULL;
    size_t size= 0;
    for(;bencode->index<bencode->torrent->length;){
        nodes = realloc(nodes, (size+1)*sizeof(ben_node *));
        nodes[size] = decodeVal(bencode->torrent->content, &bencode->index);
        size++;
    }

    for(size_t i=0; i<size; i++){
        displaybencode(nodes[i]);
    }

    for(size_t i=0; i<size; i++){
        freeBencode(nodes[i]);
    }
    free(nodes);
}