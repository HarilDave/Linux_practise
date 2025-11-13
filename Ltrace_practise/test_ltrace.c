#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void greet(const char *name) {
    char message[100];
    snprintf(message, sizeof(message), "Hello, %s!\n", name);
    write(1, message, strlen(message));
}

int main() {
    char name[50];
    printf("Enter your name: ");
    scanf("%49s", name);
    greet(name);

    FILE *fp = fopen("ltrace_demo.txt", "w");
    if (!fp) {
        perror("File open failed");
        return 1;
    }
    fprintf(fp, "This file was created by %s\n", name);
    fclose(fp);

    char *text = strdup("Memory allocation check\n");
    printf("%s", text);
    free(text);

    sleep(1);
    printf("Done.\n");

    return 0;
}

