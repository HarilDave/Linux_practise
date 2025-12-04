#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

int main()
{
    int fd = open("/dev/gpiobtn", O_RDONLY);
    struct pollfd pfd = { .fd = fd, .events = POLLIN };

    printf("Waiting for button press...\n");

    while (1) {
        int ret = poll(&pfd, 1, 5000);

        if (ret == 0) {
            printf("No button press within 5 seconds\n");
            continue;
        }

        if (pfd.revents & POLLIN) {
            char buf[64];
            read(fd, buf, sizeof(buf));
            printf(">> %s", buf);
        }
    }

    close(fd);
    return 0;
}

