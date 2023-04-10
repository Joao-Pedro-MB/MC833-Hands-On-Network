#include <stdio.h>

int main() {
    FILE *file;
    char buffer[100];

    // open the file for writing
    file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // write to the file
    fputs("Hello World", file);

    // close the file
    fclose(file);

    // open the file for reading
    file = fopen("output.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // read from the file
    fgets(buffer, 100, file);

    // print the content to the console
    printf("%s\n", buffer);

    // close the file
    fclose(file);

    return 0;
}