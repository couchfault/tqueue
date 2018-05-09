//
// Created by Tesla on 5/9/18.
//

#include <pthread.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include "tqueue_event.h"


struct tqueue_event_watcher *watcher_head = NULL;

void *run_handler(void *data) {
    struct tqueue_event_watcher *watcher = (struct tqueue_event_watcher*)data;
    for (int i = 0; (i < watcher->nrepeat || watcher->nrepeat <= 0) && !watcher->cancelled;  ++i) {
        if (waitfd(watcher->mapping->fd, 5e5/*500 ms*/) == 0) {
            watcher->handler(watcher->mapping->addr, watcher->mapping->size, watcher->user);
        } else {
            msync(watcher->mapping->addr, watcher->mapping->size, MS_SYNC);
            usleep(5e5);
        }
    }
    return NULL;
}

void add_watcher(struct tqueue_event_watcher *new) {
    if (watcher_head == NULL) {
        watcher_head = new;
        return;
    }
    struct tqueue_event_watcher *tmp = watcher_head, *old = tmp;
    while (tmp != NULL) {
        old = tmp;
        tmp = tmp->next;
    }
    old->next = new;
}

void register_watcher(void *data, size_t data_size, tqueue_handler_func handler, int nrepeat, void *user) {
    if (create_mapping(data, data_size)) {
        return;
    }
    struct tqueue_mapping *mapping;
    if ((mapping = lookup_mapping(data)) == NULL) {
        return;
    }
    struct tqueue_event_watcher *watcher;
    if ((watcher = malloc(sizeof(struct tqueue_event_watcher))) == NULL) {
        remove_mapping(data);
        return;
    }
    memset(watcher, 0, sizeof(struct tqueue_event_watcher));
    watcher->cancelled = 0;
    watcher->user = user;
    watcher->mapping = mapping;
    watcher->handler = handler;
    watcher->nrepeat = nrepeat;
    watcher->next = NULL;
    add_watcher(watcher);
    pthread_create(&watcher->thread, NULL, run_handler, (void*)watcher);
}

void remove_node(struct tqueue_event_watcher *node) {
    struct tqueue_event_watcher *tmp = watcher_head;
    if (node == watcher_head) {
        watcher_head = watcher_head->next;
        free(node);
        return;
    }
    while (tmp->next != NULL && tmp->next != node) {
        tmp = tmp->next;
    }
    tmp->next = node->next;
    free(node);
}

void waitfor_watcher(void *data) {
    struct tqueue_event_watcher *tmp = watcher_head, *old = tmp;
    while (tmp != NULL && tmp->mapping->addr != data) {
        old = tmp;
        tmp = tmp->next;
    }
    if (old != NULL) {
        pthread_join(old->thread, NULL);
        remove_mapping(old->mapping);
        old->cancelled = 1;
    }
    remove_node(old);
}

void cancel_watcher(void *data) {
    struct tqueue_event_watcher *tmp = watcher_head, *old = NULL;
    while (tmp != NULL && tmp->mapping->addr != data) {
        old = tmp;
        tmp = tmp->next;
    }
    if (old != NULL) {
        remove_mapping(old->mapping);
        old->cancelled = 1;
        pthread_join(old->thread, NULL);
    }
    remove_node(old);
}

void destroy_watchers() {
    struct tqueue_event_watcher *tmp;
    while (watcher_head != NULL) {
        tmp = watcher_head;
        watcher_head = watcher_head->next;
        remove_mapping(tmp->mapping);
        tmp->cancelled = 1;
        pthread_cancel(tmp->thread);
        free(tmp);
    }
}
