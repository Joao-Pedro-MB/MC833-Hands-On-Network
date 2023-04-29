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
    if (database == NULL) {
        printf("Error: unable to parse JSON.\n");
        exit(1);
    }

    return database;
}

int write_database(cJSON * database) {
    // Open the JSON file for writing
    FILE *fp = fopen("./server/database/database.json", "w");
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

char * create_profile(cJSON * request, cJSON * database) {
    printf("create_profile() called\n");

    cJSON *request_value = cJSON_GetObjectItem(request, "value");
    cJSON * parsed_value = cJSON_Parse(request_value);
    printf("try to get object\n");
    cJSON * age = cJSON_GetObjectItem(parsed_value, "age");
    printf("string of age: %s\n", age->valuestring);

    cJSON * json_response, * new_profile = cJSON_CreateObject();
    cJSON_AddStringToObject(new_profile, "name", cJSON_GetObjectItem(parsed_value, "name")->valuestring);
    cJSON_AddNumberToObject(new_profile, "age", atoi(cJSON_GetObjectItem(parsed_value, "age")->valuestring));
    cJSON_AddStringToObject(new_profile, "email", cJSON_GetObjectItem(parsed_value, "email")->valuestring);
    cJSON_AddStringToObject(new_profile, "city", cJSON_GetObjectItem(parsed_value, "city")->valuestring);
    cJSON_AddStringToObject(new_profile, "state", cJSON_GetObjectItem(parsed_value, "state")->valuestring);
    cJSON_AddStringToObject(new_profile, "scholarity", cJSON_GetObjectItem(parsed_value, "scholarity")->valuestring);
    cJSON_AddStringToObject(new_profile, "skills", cJSON_GetObjectItem(parsed_value, "skills")->valuestring);
    cJSON_AddNumberToObject(new_profile, "graduationYear", atoi(cJSON_GetObjectItem(parsed_value, "graduationYear")->valuestring));

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

    char * string_response = cJSON_Print(json_response);
    
    cJSON_Delete(profiles_array);
    cJSON_Delete(new_profile);
    cJSON_Delete(json_response);

    return string_response;
}

char * search(cJSON * request, cJSON * database) {
    printf("search() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * field = cJSON_GetObjectItem(request, "field");
    cJSON * value = cJSON_GetObjectItem(request, "value");

    cJSON * profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");

    if (strcmp(field->valuestring, "All") == 0) {
        cJSON_AddItemToObject(json_response, "Profiles", profiles_array);
        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "All profiles");
    }

    else if (strcmp(field->valuestring, "Skills") == 0  && value != NULL) {

        cJSON * response_array = cJSON_CreateArray();

        cJSON* item;

        cJSON_ArrayForEach(item, profiles_array) {

            if (strstr(cJSON_GetObjectItem(item, "skills")->valuestring, value->valuestring)) {

                cJSON * user_profile = cJSON_CreateObject();

                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddItemToArray(response_array, user_profile);
                
            }

        }

        cJSON_AddItemToObject(json_response, "Profiles", response_array);
        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profiles found by Skill");
        cJSON_Delete(item);

    }

    else if (strcmp(field->valuestring, "Scholarity") == 0 && value != NULL) {

        cJSON * response_array = cJSON_CreateArray();

        cJSON* item;

        cJSON_ArrayForEach(item, profiles_array) {

            if (strcmp(cJSON_GetObjectItem(item, "scholarity")->valuestring, value->valuestring) == 0) {

                cJSON * user_profile = cJSON_CreateObject();

                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddItemToArray(response_array, user_profile);
                
            }

        }

        cJSON_AddItemToObject(json_response, "Profiles", response_array);
        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profiles found by Scholarity");
        cJSON_Delete(item);

    }

    else if (strcmp(field->valuestring, "GraduationYear") == 0  && value != NULL) {

        cJSON * response_array = cJSON_CreateArray();

        cJSON* item;

        cJSON_ArrayForEach(item, profiles_array) {

            if (cJSON_GetObjectItem(item, "graduationYear")->valueint == atoi(value->valuestring)) {
                
                cJSON * user_profile = cJSON_CreateObject();

                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddItemToArray(response_array, user_profile);

            }

        }

        cJSON_AddItemToObject(json_response, "Profiles", response_array);
        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profiles found by Graduation Year");
        cJSON_Delete(item);

    } 
    
    else {
        json_response = create_error_response(400, "Invalid request");
    }

    char * string_response = cJSON_Print(json_response);

    cJSON_Delete(json_response);

    return string_response;
}

char * search_profile(cJSON * request, cJSON * database) {
    printf("search_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * email = cJSON_GetObjectItem(request, "value");

    if (email == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } 
    
    else {

        cJSON * profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");
        cJSON * user_profile = cJSON_CreateObject();

        cJSON* item;
        
        int found = -1;

        cJSON_ArrayForEach(item, profiles_array) {

            if (strcmp(cJSON_GetObjectItem(item, "email")->valuestring, email->valuestring) == 0) {
                found = 1;
                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddNumberToObject(user_profile, "age", cJSON_GetObjectItem(item, "age")->valueint);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddStringToObject(user_profile, "city", cJSON_GetObjectItem(item, "city")->valuestring);
                cJSON_AddStringToObject(user_profile, "state", cJSON_GetObjectItem(item, "state")->valuestring);
                cJSON_AddStringToObject(user_profile, "scholarity", cJSON_GetObjectItem(item, "scholarity")->valuestring);
                cJSON_AddStringToObject(user_profile, "skills", cJSON_GetObjectItem(item, "skills")->valuestring);
                cJSON_AddNumberToObject(user_profile, "graduationYear", cJSON_GetObjectItem(item, "graduationYear")->valueint);
            }

        }

        cJSON_Delete(item);

        if (found == 1) {
            cJSON_AddItemToObject(json_response, "Profile", user_profile);
            cJSON_AddNumberToObject(json_response, "Status", 200);
            cJSON_AddStringToObject(json_response, "Message", "User found");
        } else {
            json_response = create_error_response(404, "User not found");
        }
        
        char * string_response = cJSON_Print(json_response);

        cJSON_Delete(json_response);
        cJSON_Delete(profiles_array);

        return string_response;

        }

}

char * delete_profile(cJSON * request, cJSON * database) {
    printf("delete_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * email = cJSON_GetObjectItem(request, "value");

    if (email == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {

        cJSON *profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");

        cJSON* item;
        int index = 0;
        int target_index = -1;

        cJSON_ArrayForEach(item, profiles_array) {

            if (strcmp(cJSON_GetObjectItem(item, "email")->valuestring, email->valuestring) == 0) {
                target_index = index;
            }

            index += 1;
        }

        cJSON_Delete(item);

        if (target_index > -1) {

            cJSON_DeleteItemFromArray(profiles_array, target_index);

            int err = write_database(database);

            if (err != 0) {
                json_response = create_error_response(400, "Invalid request");
            } else {
                cJSON_AddNumberToObject(json_response, "Status", 200);
                cJSON_AddStringToObject(json_response, "Message", "Profile deleted");
            }

        } else {
            json_response = create_error_response(404, "User not found");
        }
        
        char * string_response = cJSON_Print(json_response);

        cJSON_Delete(profiles_array);
        cJSON_Delete(json_response);

        return string_response;
    }

}

char * answer_request(char * request) {
    cJSON *database = access_database();

    printf("answer_request() called\n");
    printf("request: %s\n", request);

    cJSON * json_request = cJSON_Parse(request);
    cJSON * command = cJSON_GetObjectItem(json_request, "command");
    int command_int = atoi(command->valuestring);

    char * json_response;

    switch (command_int) {
        case CREATE_PROFILE:
            printf("Creating profile \n");
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
            json_response = cJSON_Print(create_error_response(400, "Invalid command"));
            break;
    };


    cJSON_Delete(json_request);

    return json_response;
}

int main() {

    start_server();

    return 0;
}