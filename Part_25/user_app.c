#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define MY_SIG 44
#define MY_IOCTL_REG_TASK _IOW('a','a',int32_t*)

void sig_handler(int signo)
{
    printf("Received signal from kernel: %d\n", signo);
}

int main()
{
    int fd;

    signal(MY_SIG, sig_handler);

    fd = open("/dev/mydevice_sig", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    ioctl(fd, MY_IOCTL_REG_TASK, NULL);

    printf("Waiting for signal...\n");

    while (1)
        pause();

    close(fd);
    return 0;
}

