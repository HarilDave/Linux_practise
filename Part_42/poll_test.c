// poll_test.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int main()
{
    int fd;
    struct pollfd pfd;

    fd = open("/dev/mydevice", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    pfd.fd = fd;
    pfd.events = POLLIN | POLLOUT;

    printf("Waiting on poll...\n");

    poll(&pfd, 1, -1);

    if (pfd.revents & POLLIN)
        printf("POLLIN triggered\n");

    if (pfd.revents & POLLOUT)
        printf("POLLOUT triggered\n");

    close(fd);
    return 0;
}

