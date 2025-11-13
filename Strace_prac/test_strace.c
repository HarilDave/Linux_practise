#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    FILE *fp;
    char buffer[50];
    int i;

    printf("Creating file...\n");
    fp = fopen("demo.txt", "w");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    for (i = 0; i < 5; i++) {
        fprintf(fp, "Line %d: Writing to file using fprintf()\n", i + 1);
        sleep(1);
    }

    fclose(fp);

    printf("Reading file...\n");
    fp = fopen("demo.txt", "r");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
        usleep(500000);
    }

    fclose(fp);
    printf("Done!\n");

    return 0;
}
