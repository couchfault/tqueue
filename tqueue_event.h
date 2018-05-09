//
// Created by Tesla on 5/9/18.
//

#ifndef TQUEUE_TQUEUE_EVENT_H
#define TQUEUE_TQUEUE_EVENT_H

#include <pthread.h>
#include "mapmemory.h"
#include "waitfile.h"

typedef void (*tqueue_handler_func)(void *buffer, size_t buffer_size, void *user);

struct tqueue_event_watcher {
    tqueue_handler_func handler;
    struct tqueue_mapping *mapping;
    int nrepeat;
    void *user;
    volatile int cancelled;
    pthread_t thread;
    struct tqueue_event_watcher *next;
};

void register_watcher(void*, size_t, tqueue_handler_func, int, void*);
void waitfor_watcher(void*);
void cancel_watcher(void*);
void destroy_watchers();

#endif //TQUEUE_TQUEUE_EVENT_H
