//
// Created by Tesla on 5/9/18.
//

#ifndef TQUEUE_MAPMEMORY_H
#define TQUEUE_MAPMEMORY_H

struct tqueue_mapping {
    void *addr;
    size_t size;
    int fd;
    struct tqueue_mapping *next;
};

int create_mapping(void*, size_t);
struct tqueue_mapping *lookup_mapping(void*);
void remove_mapping(void*);
void destroy_mappings();

#endif //TQUEUE_MAPMEMORY_H
