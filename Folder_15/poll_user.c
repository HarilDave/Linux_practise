#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>

#define DEVICE_PATH "/dev/polldev"

int main()
{
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN | POLLOUT;   // We want to monitor read + write readiness

    printf("Calling poll() ... waiting up to 10 seconds for an event.\n");

    int ret = poll(&pfd, 1, 10000);   // timeout=10000 ms (10 seconds)

    if (ret < 0) {
        perror("poll");
        close(fd);
        return 1;
    }

    if (ret == 0) {
        printf("Timeout occurred! No events within 10 seconds.\n");
        close(fd);
        return 0;
    }

    printf("poll() returned: revents = 0x%x\n", pfd.revents);

    if (pfd.revents & POLLIN) {
        printf(" -> POLLIN: data is available, reading now...\n");

        char buf[256];
        int n = read(fd, buf, sizeof(buf));
        if (n < 0) {
            perror("read");
        } else {
            buf[n] = '\0';
            printf("Received: %s\n", buf);
        }
    }

    if (pfd.revents & POLLOUT) {
        printf(" -> POLLOUT: device ready to accept write.\n");
    }

    if (pfd.revents & POLLERR)
        printf(" -> POLLERR: Error condition.\n");

    if (pfd.revents & POLLHUP)
        printf(" -> POLLHUP: Hangup condition.\n");

    if (pfd.revents & POLLNVAL)
        printf(" -> POLLNVAL: Invalid FD.\n");

    close(fd);
    return 0;
}

