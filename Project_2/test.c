// user_test.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define KP_MAGIC 'K'
#define KP_CLEAR_QUEUE _IO(KP_MAGIC, 1)

int main()
{
    int fd, key;

    fd = open("/dev/keypad", O_RDWR);

    key = 5;
    write(fd, &key, sizeof(int));

    read(fd, &key, sizeof(int));
    printf("Key read: %d\n", key);

    ioctl(fd, KP_CLEAR_QUEUE);

    close(fd);
    return 0;
}

