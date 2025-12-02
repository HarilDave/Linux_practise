// user_test.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Same IOCTL definitions as in driver
#define TEMP_IOC_MAGIC      'T'
#define TEMP_IOC_SET_HIGH   _IOW(TEMP_IOC_MAGIC, 1, int)
#define TEMP_IOC_SET_LOW    _IOW(TEMP_IOC_MAGIC, 2, int)

int main(void)
{
    int fd;
    int high = 60;
    int low  = 30;
    char buf[128];
    ssize_t n;

    fd = open("/dev/temp_sensor", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Set thresholds using ioctl
    if (ioctl(fd, TEMP_IOC_SET_HIGH, &high) < 0) {
        perror("ioctl set high");
    }

    if (ioctl(fd, TEMP_IOC_SET_LOW, &low) < 0) {
        perror("ioctl set low");
    }

    // Read temperature
    n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0) {
        perror("read");
        close(fd);
        return 1;
    }

    buf[n] = '\0';
    printf("User read: %s", buf);

    close(fd);
    return 0;
}

