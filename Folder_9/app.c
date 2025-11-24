#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd;
    char read_buf[100];

    // Open the device
    fd = open("/dev/mychardev", O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return 1;
    }

    // Write to device
    char msg[] = "Testing from app";
    write(fd, msg, strlen(msg));

    // Read response
    int ret = read(fd, read_buf, sizeof(read_buf));
    read_buf[ret] = '\0';

    printf("Driver says: %s", read_buf);

    close(fd);
    return 0;
}

