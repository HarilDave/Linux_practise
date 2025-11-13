#include <stdio.h>

int add(int a, int b) {
    int sum = a + b;
    return sum;
}

int main() {
    int x = 5, y = 7;
    int result = add(x, y);
    printf("Result = %d\n", result);
    return 0;
}
