#include <stdio.h>

int main()
{
    printf("Load module: sudo insmod temp_sensor.ko\n");
    printf("Check alerts in: dmesg | tail\n");
    printf("Unload: sudo rmmod temp_sensor\n");
    return 0;
}
