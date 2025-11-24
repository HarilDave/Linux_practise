#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd;
    char buffer[128];
    int n;

    const char *dev = "/dev/securedev";

    const char *msgs[] = {
        "First message\n",
        "Second message\n",
        "Third message\n"
    };

    int total = sizeof(msgs) / sizeof(msgs[0]);

    fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    for (int i = 0; i < total; i++) {
        write(fd, msgs[i], strlen(msgs[i]));
    }

    printf("\nReading from driver:\n");
    while (1) {
        n = read(fd, buffer, sizeof(buffer) - 1);
        if (n <= 0)
            break;

        buffer[n] = '\0';
        printf("Read: %s", buffer);
    }

    printf("No more messages.\n");

    close(fd);
    return 0;
}

