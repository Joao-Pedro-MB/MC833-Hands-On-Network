#include "server-socket.h"
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

cJSON * access_database() {
    // Open the JSON file for reading
    FILE *fp = fopen("./server/database/database.json", "r");
    if (fp == NULL) {
        printf("Error: unable to access database.\n");
        exit(1);
    }

    // Read the contents of the file into a buffer
    printf("Reading database file...\n");
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    printf("defined file_size\n");
    fseek(fp, 0, SEEK_SET);
    printf("File size: bytes");
    char *json_buffer = malloc(file_size + 1);
    fread(json_buffer, 1, file_size, fp);
    fclose(fp);

    // Parse the JSON buffer into a cJSON object
    cJSON *database = cJSON_Parse(json_buffer);
    printf("%s\n", cJSON_Print(database));
    if (database == NULL) {
        printf("Error: unable to parse JSON.\n");
        exit(1);
    }

    return database;
}

int write_database(cJSON * database) {
    // Open the JSON file for writing
    FILE *fp = fopen("../database/database.json", "w");
    if (fp == NULL) {
        printf("Error: unable to open file.\n");
        exit(1);
    }

     char *new_json_str = cJSON_PrintUnformatted(database);

    // Write the new JSON string to the file
    fwrite(new_json_str, 1, strlen(new_json_str), fp);
    fclose(fp);

    // Free the memory allocated for the JSON buffers
    free(new_json_str);

    return 0;
}

cJSON * create_error_response(int status, char * message) {
    cJSON * json_response = cJSON_CreateObject();

    cJSON_AddNumberToObject(json_response, "Status", status);
    cJSON_AddStringToObject(json_response, "Message", message);

    return json_response;
}

cJSON * create_profile(cJSON * request, cJSON * database) {
    printf("create_profile() called\n");

    cJSON * json_response, * new_profile = cJSON_CreateObject();
    cJSON_AddStringToObject(new_profile, "name", "julia");
    cJSON_AddNumberToObject(new_profile, "age", 25);
    cJSON_AddStringToObject(new_profile, "email", "julia@example.com");
    cJSON_AddStringToObject(new_profile, "city", "+1 555-123-4567");
    cJSON_AddStringToObject(new_profile, "state", "+1 555-123-4567");
    cJSON_AddStringToObject(new_profile, "scholarity", "+1 555-123-4567");
    cJSON_AddStringToObject(new_profile, "skills", "+1 555-123-4567");

    cJSON *profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");

    cJSON_AddItemToArray(profiles_array, new_profile);

    int err = write_database(database);

    if (err != 0) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        
        cJSON * json_response = cJSON_CreateObject();
        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile created");
    }

    cJSON_Delete(profiles_array);
    cJSON_Delete(new_profile);
    return json_response;
}

cJSON * search(cJSON * request, cJSON * database) {
    printf("search() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * name = cJSON_GetObjectItem(request, "Name");
    cJSON * age = cJSON_GetObjectItem(request, "Age");
    cJSON * email = cJSON_GetObjectItem(request, "Email");
    cJSON * phone = cJSON_GetObjectItem(request, "Phone");

    if (name == NULL && age == NULL && email == NULL && phone == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        cJSON * json_response = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile found");
    }

    return json_response;
}

cJSON * search_profile(cJSON * request, cJSON * database) {
    printf("search_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * name = cJSON_GetObjectItem(request, "Name");
    cJSON * age = cJSON_GetObjectItem(request, "Age");
    cJSON * email = cJSON_GetObjectItem(request, "Email");
    cJSON * phone = cJSON_GetObjectItem(request, "Phone");

    if (name == NULL && age == NULL && email == NULL && phone == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        cJSON * json_response = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile found");
    }

    return json_response;
}

cJSON * delete_profile(cJSON * request, cJSON * database) {
    printf("delete_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * name = cJSON_GetObjectItem(request, "Name");
    cJSON * age = cJSON_GetObjectItem(request, "Age");
    cJSON * email = cJSON_GetObjectItem(request, "Email");
    cJSON * phone = cJSON_GetObjectItem(request, "Phone");

    if (name == NULL && age == NULL && email == NULL && phone == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        cJSON * json_response = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile deleted");
    }

    return json_response;
}

char * answer_request(char * request) {
    cJSON *database = access_database();

    printf("answer_request() called\n");
    printf("request: %s\n", request);

    cJSON * json_request = cJSON_Parse(request);
    cJSON * command = cJSON_GetObjectItem(json_request, "Command");

    cJSON * json_response = cJSON_CreateObject();

    switch (command->valueint) {
        case CREATE_PROFILE:
            json_response = create_profile(json_request, database);
            break;

        case SEARCH_BATCH:
            json_response = search(json_request, database);
            break;

        case LIST_ALL:
            json_response = search(json_request, database);
            break;

        case FIND_PROFILE:
            json_response = search_profile(json_request, database);
            break;

        case DELETE_PROFILE:
            json_response = delete_profile(json_request, database);
            break;

        default:
            json_response = create_error_response(400, "Invalid command");
            break;
    };

    write_database(database);
    char * response = cJSON_Print(json_response);

    cJSON_Delete(json_response);
    cJSON_Delete(json_request);

    return response;
}

int main() {

    start_server();

    return 0;
}