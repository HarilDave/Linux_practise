#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/ioctldev"
#define MAJOR_NUM 240

#define IOCTL_RESET_BUFFER _IO(MAJOR_NUM, 0)
#define IOCTL_GET_COUNT    _IOR(MAJOR_NUM, 1, int)
#define IOCTL_SET_DEBUG    _IOW(MAJOR_NUM, 2, int)

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int dbg = 1;
    ioctl(fd, IOCTL_SET_DEBUG, &dbg);   // ENABLE DEBUG MODE

    char msg[] = "Testing debug";
    write(fd, msg, strlen(msg));

    int count;
    ioctl(fd, IOCTL_GET_COUNT, &count);
    printf("Write count = %d\n", count);

    ioctl(fd, IOCTL_RESET_BUFFER);
    printf("Buffer reset.\n");

    dbg = 0;
    ioctl(fd, IOCTL_SET_DEBUG, &dbg);   // DISABLE DEBUG MODE

    close(fd);
    return 0;
}

