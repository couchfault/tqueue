//
// Created by Tesla on 5/9/18.
//

#include <errno.h>
#include <stdio.h>
#include <sys/_types/_timespec.h>
#include "waitfile.h"

int kqueue_waitfd(int fd, long timeout_nsec) {
    int kq;
    if ((kq = kqueue()) < 0) {
//        perror("failed to create kqueue");
        return 1;
    }
    struct timespec xtimeout;
    xtimeout.tv_sec = (time_t)(timeout_nsec / 1e9);
    xtimeout.tv_nsec = (timeout_nsec % (int)1e9);
    struct timespec *timeout;
    if (timeout_nsec <= 0) {
        timeout = NULL;
    } else {
        timeout = &xtimeout;
    }
    struct kevent monitor_events[1], event_data[1];
    EV_SET(&monitor_events[0], fd, EVFILT_VNODE, (EV_ADD | EV_CLEAR), NOTE_WRITE | NOTE_EXTEND | NOTE_REVOKE | NOTE_LINK | NOTE_FUNLOCK, 0, NULL);
    if (kevent(kq, monitor_events, 1, event_data, 1, timeout) < 0 || event_data[0].flags == EV_ERROR) {
        perror("kevent failure");
        return 1;
    }
    if (event_data[0].fflags & NOTE_WRITE) {
        return  0;
    }
    return 1;
}

int inotify_waitfd(int fd, long timeout_nsec) {
    return 0;
}