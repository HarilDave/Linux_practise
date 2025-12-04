#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

int main()
{
    int fd = open("/dev/gpiobtn", O_RDONLY | O_NONBLOCK);
    int epfd = epoll_create1(0);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    printf("Waiting for events...\n");

    while (1) {
        struct epoll_event events[1];
        epoll_wait(epfd, events, 1, -1);

        char buf[32];
        read(fd, buf, sizeof(buf));
        printf(">> %s", buf);
    }
}

