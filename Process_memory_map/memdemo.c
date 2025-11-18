#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    printf("Program started!\n");

    // Get PID
    pid_t pid = getpid();
    printf("My PID = %d\n", pid);

    // Command to print memory map of this process
    printf("\n=== /proc/<PID>/maps output ===\n\n");

    char cmd[64];
    snprintf(cmd, sizeof(cmd), "cat /proc/%d/maps", pid);

    system(cmd);
    return 0;
}

