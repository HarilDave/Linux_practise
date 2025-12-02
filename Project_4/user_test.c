#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct stats_data {
    unsigned long open_count;
    unsigned long read_count;
    unsigned long write_count;
    char last_op[16];
    long last_time_sec;
    long last_time_nsec;
};

#define STATS_IOC_MAGIC 'S'
#define STATS_IOC_GET _IOR(STATS_IOC_MAGIC, 1, struct stats_data)

int main()
{
    int fd;
    struct stats_data stats;

    fd = open("/dev/stats_dev", O_RDWR);

    write(fd, "x", 1);
    read(fd, &stats, 0);

    ioctl(fd, STATS_IOC_GET, &stats);

    printf("opens=%lu reads=%lu writes=%lu last=%s\n",
           stats.open_count,
           stats.read_count,
           stats.write_count,
           stats.last_op);

    close(fd);
    return 0;
}

