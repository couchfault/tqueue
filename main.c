#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <sys/mman.h>
#include "tqueue_event.h"

void callback(void *buf, size_t buf_size, void *user) {
    printf("buf = %s\n", (char*)buf);
}

int main(int argc, char *argv[]) {
    char *buf;
    size_t buf_size = 4096;
    if ((buf = malloc(buf_size)) == NULL) {
        perror("Malloc failure");
        return 1;
    }
    memset(buf, 0, buf_size);
    register_watcher(buf, buf_size, callback, 0, NULL);
    memcpy(buf, "hello world", strlen("hello world"));
    msync(buf, buf_size, MS_SYNC);
    sleep(3);
    memcpy(buf, "some other test string", strlen("some other test string"));
    msync(buf, buf_size, MS_SYNC);
    for (int i = 0; i < 60; ++i) {
        sleep(1);
        strcat(buf, "yello");
    }
    sleep(5);
    cancel_watcher(buf);
//    waitfor_watcher(buf);
    return 0;
}