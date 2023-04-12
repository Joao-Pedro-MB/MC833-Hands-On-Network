#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int main() {
    // Open the JSON file for reading
    FILE *fp = fopen("example.json", "r");
    if (!fp) {
        printf("Failed to open file\n");
        return 1;
    }

    // Read the JSON data from the file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *json_data = malloc(file_size + 1);
    fread(json_data, 1, file_size, fp);
    fclose(fp);

    // Parse the JSON data into a cJSON object
    cJSON *root = cJSON_Parse(json_data);
    free(json_data);

    // Encode the cJSON object as a JSON string
    char *encoded_json = cJSON_Print(root);

    // Decode the JSON string back into a cJSON object
    cJSON *decoded_root = cJSON_Parse(encoded_json);

    // Modify the decoded cJSON object
    cJSON *new_element = cJSON_CreateNumber(42);
    cJSON_AddItemToObject(decoded_root, "new_element", new_element);

    // Re-encode the modified cJSON object as a JSON string
    char *new_encoded_json = cJSON_Print(decoded_root);

    // Open the JSON file for writing
    fp = fopen("example.json", "w");
    if (!fp) {
        printf("Failed to open file\n");
        return 1;
    }

    // Write the new JSON data to the file
    fwrite(new_encoded_json, 1, strlen(new_encoded_json), fp);
    fclose(fp);

    // Clean up the cJSON objects and JSON strings
    cJSON_Delete(root);
    cJSON_Delete(decoded_root);
    free(encoded_json);
    free(new_encoded_json);

    return 0;
}
