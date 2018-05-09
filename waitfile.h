//
// Created by Tesla on 5/9/18.
//

#ifndef TQUEUE_WAITFILE_H
#define TQUEUE_WAITFILE_H

int inotify_waitfd(int, long);
int kqueue_waitfd(int, long);

#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/event.h>
#define waitfd kqueue_waitfd
#elif defined(__linux__)
// inotify includes
#define waitfd inotify_waitfd
#else
#error "Unsupported platform"
#endif

#endif //TQUEUE_WAITFILE_H
