#include "server-socket.h"
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

cJSON* access_database() {
    cJSON* database = NULL;
    FILE* fp = NULL;
    long file_size = 0;
    char* json_buffer = NULL;

    // Open the JSON file for reading
    fp = fopen("./server/database/database.json", "r");
    if (fp == NULL) {
        printf("Error: unable to access database.\n");
        exit(1);
    }

    // Get the size of the file
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory for the JSON buffer
    json_buffer = malloc(file_size + 1);
    if (json_buffer == NULL) {
        printf("Error: unable to allocate memory.\n");
        fclose(fp);
        exit(1);
    }

    // Read the contents of the file into the buffer
    fread(json_buffer, 1, file_size, fp);
    fclose(fp);

    // Parse the JSON buffer into a cJSON object
    database = cJSON_Parse(json_buffer);
    if (database == NULL) {
        printf("Error: unable to parse JSON.\n");
        free(json_buffer);
        exit(1);
    }

    // Free the memory allocated for the JSON buffer
    free(json_buffer);

    return database;
}

int convert_ASCII(char * input) {
    int i = 0, sum = 0;
    while (input[i] != '\0') {
        sum += input[i];
        i++;
    }
    return sum;
}

int compare_strings(cJSON* base, cJSON* operation, cJSON* target) {
    int operation_int = convert_ASCII(operation->valuestring);
    switch (operation_int) {
        case EQUAL:
            return (strcmp(target->valuestring, base->valuestring) == 0);
        case NOT_EQUAL:
            return (strcmp(target->valuestring, base->valuestring) != 0);
        default:
            return -1;
    }
}

int compare_ints(cJSON* base, cJSON* operation, cJSON* target) {
    int base_int = atoi(base->valuestring);
    int target_int = atoi(target->valuestring);
    printf("base: %d, target: %d\n", base_int, target_int);
    switch (convert_ASCII(operation->valuestring)) {
        case GREATER:
            return (target_int > base_int);
        case LESS:
            return (target_int < base_int);
        case EQUAL:
            return (target_int == base_int);
        case NOT_EQUAL:
            return (target_int != base_int);
        case GREATER_EQUAL:
            return (target_int >= base_int);
        case LESS_EQUAL:
            return (target_int <= base_int);
        default:
            return -1;
    }
}

int find_word(char * data, cJSON * operation, char * target) {
    int i, j, found = 0, flag = 1;
    int operation_int = convert_ASCII(operation->valuestring);

    if (operation_int != EQUAL && operation_int != NOT_EQUAL) {
        return -1;
    }
    if (operation_int == NOT_EQUAL) {
        flag = 0;
    }

    for(i=0; i<=strlen(data)-strlen(target); i++) {
        found = 1;
        for(j=0; j<strlen(target); j++) {
            if(data[i+j] != target[j]) {
                found = 0;
                break;
            }
        }
        if(found) {
            return (found && flag);
        }
    }
    return !flag;
}

int write_database(cJSON* database) {
    int status = 0;
    FILE* fp = NULL;
    char* new_json_str = NULL;
    size_t str_len;

    // Open the JSON file for writing
    fp = fopen("./server/database/database.json", "w");
    if (fp == NULL) {
        printf("Error: unable to open file.\n");
        status = 1;
        goto cleanup;
    }

    // Convert the cJSON object to a JSON string
    new_json_str = cJSON_PrintUnformatted(database);
    if (new_json_str == NULL) {
        printf("Error: unable to convert cJSON to JSON string.\n");
        status = 1;
        goto cleanup;
    }

    // Write the new JSON string to the file
    str_len = strlen(new_json_str);
    if (fwrite(new_json_str, 1, str_len, fp) != str_len) {
        printf("Error: unable to write JSON string to file.\n");
        status = 1;
        goto cleanup;
    }

    // Close the file
    fclose(fp);

cleanup:
    // Free the memory allocated for the JSON string
    if (new_json_str != NULL) {
        free(new_json_str);
    }

    return status;
}

char* create_error_response(int status, const char* message) {
    cJSON* json_response = cJSON_CreateObject();
    char * response = NULL;
    if (json_response == NULL) {
        printf("Error: unable to create JSON object.\n");
        return NULL;
    }

    cJSON_AddNumberToObject(json_response, "Status", status);
    cJSON_AddStringToObject(json_response, "Message", message);

    response = cJSON_PrintUnformatted(json_response);
    cJSON_Delete(json_response);

    return response;
}

char* format_response(int status, char* message) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "status", status);
    cJSON_AddStringToObject(root, "message", message);
    char* json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

char* compare_database(cJSON* field, cJSON* operation, cJSON* value, cJSON* profiles_array) {
    cJSON* response_array = cJSON_CreateArray();
    int flag = 0;

    if (strcmp(field->valuestring, "age") == 0 || strcmp(field->valuestring, "graduationYear") == 0){
        cJSON* item = NULL;
        cJSON_ArrayForEach(item, profiles_array) {
            flag = compare_ints(value, operation, cJSON_GetObjectItem(item, field->valuestring));
            if (flag == 1) { // accessing valuestring property of the field
                cJSON* user_profile = cJSON_CreateObject();
                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddStringToObject(user_profile, "scholarity", cJSON_GetObjectItem(item, "scholarity")->valuestring); // fix the typo
                cJSON_AddItemToArray(response_array, user_profile);
            } else if (flag == -1) {
                return create_error_response(500, "Invalid operation");
            }
        }
    
    } else if (strcmp(field->valuestring, "skills") == 0) { // using strcmp() instead of string comparison operator
        cJSON* item = NULL;
        cJSON_ArrayForEach(item, profiles_array) {
            flag = find_word(cJSON_GetObjectItem(item, field->valuestring)->valuestring, operation, value->valuestring);
            if (flag == 1) {
                cJSON* user_profile = cJSON_CreateObject();
                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddItemToArray(response_array, user_profile);
            } else if (flag == -1) {
                return create_error_response(500, "Invalid operation");
            }
        }
    } else {
        cJSON* item = NULL;
        cJSON_ArrayForEach(item, profiles_array) {
            flag = compare_strings(cJSON_GetObjectItem(item, field->valuestring), operation, value);
            if (flag == 1) { // accessing valuestring property of the field
                cJSON* user_profile = cJSON_CreateObject();
                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddItemToArray(response_array, user_profile);
            
            } else if (flag == -1) {
                return create_error_response(500, "Invalid operation");
            }
        }
    }
    char* response_str = cJSON_Print(response_array);
    cJSON_Delete(response_array);
    return format_response(200, response_str);
}

char* create_profile(cJSON* request, cJSON* database) {
    cJSON* request_value = cJSON_GetObjectItem(request, "value");
    cJSON* parsed_value = cJSON_Parse(request_value->valuestring);
    cJSON* new_profile = cJSON_CreateObject();

    cJSON_AddStringToObject(new_profile, "name", cJSON_GetObjectItem(parsed_value, "name")->valuestring);
    cJSON_AddStringToObject(new_profile, "age", cJSON_GetObjectItem(parsed_value, "age")->valuestring);
    cJSON_AddStringToObject(new_profile, "email", cJSON_GetObjectItem(parsed_value, "email")->valuestring);
    cJSON_AddStringToObject(new_profile, "city", cJSON_GetObjectItem(parsed_value, "city")->valuestring);
    cJSON_AddStringToObject(new_profile, "state", cJSON_GetObjectItem(parsed_value, "state")->valuestring);
    cJSON_AddStringToObject(new_profile, "scholarity", cJSON_GetObjectItem(parsed_value, "scholarity")->valuestring);
    cJSON_AddStringToObject(new_profile, "graduationYear", cJSON_GetObjectItem(parsed_value, "graduationYear")->valuestring);
    cJSON_AddStringToObject(new_profile, "skills", cJSON_GetObjectItem(parsed_value, "skills")->valuestring);

    cJSON* profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");
    cJSON_AddItemToArray(profiles_array, new_profile);
    printf("Added new profile to database\n");

    int err = write_database(database);

    char* json_response = NULL;
    if (err != 0) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        json_response = format_response(200, "Profile created");
    }

    cJSON_Delete(new_profile);
    cJSON_Delete(request_value);
    cJSON_Delete(parsed_value);

    return json_response;
}

char * search(cJSON * request, cJSON * database) {
    printf("search() called\n");

    cJSON * field = cJSON_GetObjectItem(request, "field");
    cJSON * value = cJSON_GetObjectItem(request, "value");
    cJSON * operation = cJSON_GetObjectItem(request, "operation");

    cJSON * profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");

    if (strcmp(field->valuestring, "All") == 0) {
        return format_response(200, cJSON_PrintUnformatted(profiles_array));

    } else {
        return compare_database(field, operation, value, profiles_array);

    }
}

char * search_profile(cJSON * request, cJSON * database) {
    printf("search_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * email = cJSON_GetObjectItem(request, "value");

    if (email == NULL) {
        cJSON_Delete(json_response);
        return create_error_response(400, "Invalid request");

    } else {

        cJSON * profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");
        cJSON * user_profile = cJSON_CreateObject();

        cJSON* item;
        
        int found = -1;

        cJSON_ArrayForEach(item, profiles_array) {

            if (strcmp(cJSON_GetObjectItem(item, "email")->valuestring, email->valuestring) == 0) {
                found = 1;
                cJSON_AddStringToObject(user_profile, "name", cJSON_GetObjectItem(item, "name")->valuestring);
                cJSON_AddStringToObject(user_profile, "age", cJSON_GetObjectItem(item, "age")->valuestring);
                cJSON_AddStringToObject(user_profile, "email", cJSON_GetObjectItem(item, "email")->valuestring);
                cJSON_AddStringToObject(user_profile, "city", cJSON_GetObjectItem(item, "city")->valuestring);
                cJSON_AddStringToObject(user_profile, "state", cJSON_GetObjectItem(item, "state")->valuestring);
                cJSON_AddStringToObject(user_profile, "scholarity", cJSON_GetObjectItem(item, "scholarity")->valuestring);
                cJSON_AddStringToObject(user_profile, "skills", cJSON_GetObjectItem(item, "skills")->valuestring);
                cJSON_AddStringToObject(user_profile, "graduationYear", cJSON_GetObjectItem(item, "graduationYear")->valuestring);
            }

        }

        cJSON_Delete(item);
        cJSON_Delete(profiles_array);
        if (found == 1) {
            return format_response(200, cJSON_PrintUnformatted(user_profile));
        } else {
            return create_error_response(404, "User not found");
        }

    }

}

char * delete_profile(cJSON * request, cJSON * database) {
    printf("delete_profile() called\n");

    cJSON* email = cJSON_GetObjectItemCaseSensitive(request, "value");
    if (!cJSON_IsString(email)) {
        return create_error_response(400, "Invalid request");
    }

    // Search for the profile with the matching email in the profiles array of the database cJSON object
    cJSON* profiles = cJSON_GetObjectItemCaseSensitive(database, "profiles");
    if (!cJSON_IsArray(profiles)) {
        return create_error_response(400, "Invalid database format: profiles field is missing or not an array");
    }
    cJSON* profile = NULL;
    int index = 0;
    cJSON_ArrayForEach(profile, profiles) {
        cJSON* emailField = cJSON_GetObjectItemCaseSensitive(profile, "email");
        if (cJSON_IsString(emailField) && (strcmp(email->valuestring, emailField->valuestring) == 0)) {
            // Found the profile with the matching email, remove it from the profiles array
            cJSON_DeleteItemFromArray(profiles, index);
            write_database(database);
            return format_response(200,"Profile deleted");
        }
        index++;
    }

    // If we reach this point, the profile was not found
    return format_response(400,"Profile not found");

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

    printf("json_response: %s\n", json_response);

    return json_response;
}

int main() {

    start_server();

    return 0;
}