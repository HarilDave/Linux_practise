#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define LED_ON  _IO('G', 0)
#define LED_OFF _IO('G', 1)

int main()
{
    int fd = open("/dev/gpiobtn", O_RDWR);
    char buf[64];

    read(fd, buf, sizeof(buf));
    printf("%s", buf);

    write(fd, "1", 1);      // toggle LED
    ioctl(fd, LED_ON);       // LED ON
    ioctl(fd, LED_OFF);      // LED OFF

    close(fd);
    return 0;
}

