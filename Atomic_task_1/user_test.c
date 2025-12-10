#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Load the module using: sudo insmod light_controller.ko\n");
    printf("Check dmesg for output.\n");
    printf("Remove using: sudo rmmod light_controller\n");
    return 0;
}
