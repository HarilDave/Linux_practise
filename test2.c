#include <stdio.h>

int main() {
    int *p = NULL;
    *p = 10;   // intentional crash
    printf("Value = %d\n", *p);
    return 0;
}
