#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int main()
{
    // Open the JSON file for reading
    FILE *fp = fopen("example.json", "r");
    if (fp == NULL) {
        printf("Error: unable to open file.\n");
        exit(1);
    }

    // Read the contents of the file into a buffer
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *json_buffer = malloc(file_size + 1);
    fread(json_buffer, 1, file_size, fp);
    fclose(fp);

    // Parse the JSON buffer into a cJSON object
    cJSON *root = cJSON_Parse(json_buffer);
    if (root == NULL) {
        printf("Error: unable to parse JSON.\n");
        exit(1);
    }

    // Create a new profile object and add it to the JSON object
    cJSON *profile = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "profiles", profile);

    // Add name and age fields to the new profile object
    cJSON_AddStringToObject(profile, "name", "Alice");
    cJSON_AddNumberToObject(profile, "age", 30);

    // Convert the cJSON object back to a JSON string
    char *new_json_buffer = cJSON_Print(root);
    cJSON_Delete(root);

    // Open the JSON file for writing
    fp = fopen("example.json", "w");
    if (fp == NULL) {
        printf("Error: unable to open file.\n");
        exit(1);
    }

    // Write the new JSON string to the file
    fwrite(new_json_buffer, 1, strlen(new_json_buffer), fp);
    fclose(fp);

    // Free the memory allocated for the JSON buffers
    free(json_buffer);
    free(new_json_buffer);

    return 0;
}
