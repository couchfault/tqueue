//
// Created by Tesla on 5/9/18.
//

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/time.h>
#include <errno.h>
#include <mach/mach.h>
#include "mapmemory.h"

struct tqueue_mapping *map_head = NULL;


void insert_mapping(struct tqueue_mapping *new) {
    if (map_head == NULL) {
        map_head = new;
        return;
    }
    struct tqueue_mapping *tmp = map_head, *old = tmp;
    while (tmp != NULL) {
        old = tmp;
        tmp = tmp->next;
    }
    old->next = new;
}

int create_mapping(void *buf, size_t buf_size) {
    int fd;
    void *addr;
    void *buf_copy;
    struct tqueue_mapping *tmp;
    char template[] = "/Users/tesla/tmp/tqueue.fd.XXXXXX";
    if ((fd = mkstemp(template)) == -1) {
        return errno;
    }
    write(fd, buf, buf_size);
    // TODO: this currently removes all memory protections. find a way of detecting and preserving them.
    if ((addr = mmap(buf, buf_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED | MAP_FIXED | MAP_HASSEMAPHORE, fd, 0)) == MAP_FAILED) {
        close(fd);
        return errno;
    }
    if ((tmp = malloc(sizeof(struct tqueue_mapping))) == NULL) {
        munmap(buf, buf_size);
        close(fd);
        return errno;
    }
    memset(tmp, 0, sizeof(struct tqueue_mapping));
    tmp->addr = addr;
    tmp->size = buf_size;
    tmp->fd = fd;
    tmp->next = NULL;
    insert_mapping(tmp);
    return 0;
}

struct tqueue_mapping *lookup_mapping(void *addr) {
    struct tqueue_mapping *tmp = map_head;
    while (tmp != NULL) {
       if (tmp->addr == addr) {
           return tmp;
       }
    }
    return NULL;
}

void destroy_mapping(struct tqueue_mapping *mapping) {
    munmap(mapping->addr, mapping->size);
    close(mapping->fd);
    free(mapping);
}

void remove_mapping(void *addr) {
    struct tqueue_mapping *tmp = map_head, *old = tmp;
    if (map_head == NULL) {
        return;
    }
    if (map_head->addr == addr) {
        map_head = map_head->next;
        destroy_mapping(tmp);
        return;
    }
    while (tmp != NULL) {
        if (tmp->addr == addr) {
            old->next = tmp->next;
            destroy_mapping(tmp);
            return;
        }
        old = tmp;
        tmp = tmp->next;
    }
}

void destroy_mappings() {
    struct tqueue_mapping *tmp;
    while (map_head != NULL) {
        tmp = map_head;
        map_head = map_head->next;
        destroy_mapping(tmp);
    }
}
