#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

int main()
{
    int fd;
    fd_set rfds;

    fd = open("/dev/mydevice", O_RDWR);

    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        printf("Waiting on select...\n");

        select(fd + 1, &rfds, NULL, NULL, NULL);

        if (FD_ISSET(fd, &rfds))
        {
            char buf[20];
            read(fd, buf, sizeof(buf));
            printf("Read ready: %s\n", buf);
        }
    }
}

