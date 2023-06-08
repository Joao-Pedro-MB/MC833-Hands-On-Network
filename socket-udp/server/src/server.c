#include "server-socket.h"

char * create_profile(cJSON * request, cJSON * database) {
    cJSON * request_value = cJSON_GetObjectItem(request, "value");
    cJSON * parsed_value = cJSON_Parse(request_value->valuestring);
    cJSON * new_profile = cJSON_CreateObject();

    // create new profile JSON object
    cJSON_AddStringToObject(new_profile, "name", cJSON_GetObjectItem(parsed_value, "name")->valuestring);
    cJSON_AddStringToObject(new_profile, "age", cJSON_GetObjectItem(parsed_value, "age")->valuestring);
    cJSON_AddStringToObject(new_profile, "email", cJSON_GetObjectItem(parsed_value, "email")->valuestring);
    cJSON_AddStringToObject(new_profile, "city", cJSON_GetObjectItem(parsed_value, "city")->valuestring);
    cJSON_AddStringToObject(new_profile, "state", cJSON_GetObjectItem(parsed_value, "state")->valuestring);
    cJSON_AddStringToObject(new_profile, "scholarity", cJSON_GetObjectItem(parsed_value, "scholarity")->valuestring);
    cJSON_AddStringToObject(new_profile, "graduationYear", cJSON_GetObjectItem(parsed_value, "graduationYear")->valuestring);
    cJSON_AddStringToObject(new_profile, "skills", cJSON_GetObjectItem(parsed_value, "skills")->valuestring);

    // add new profile to database
    cJSON* profiles_array = cJSON_GetObjectItemCaseSensitive(database, "profiles");
    cJSON_AddItemToArray(profiles_array, new_profile);
    printf("Added new profile to database\n");

    int err = write_database(database);

    char * json_response = NULL;
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

char * get_image(cJSON * request, cJSON * database) {
    printf("get_image() called\n");

    cJSON * image_name = cJSON_GetObjectItem(request, "value");

    if (image_name == NULL) {
        return create_error_response(400, "Invalid request");
    }

    char image_path[18] = "./server/image/";

    char * image_path_name = malloc(strlen(image_path) + strlen(image_name->valuestring) + 1);
    strcpy(image_path_name, image_path);
    strcat(image_path_name, image_name->valuestring);

    return image_path_name;    
}

int answer_request(struct Packet packets[], int num_packets, char ** json_response) {
    cJSON * database = access_database();
    char *request;

    printf("answer_request() called\n");

    if (num_packets <= 1){
        request = packets[0].data;
    }
    printf("request: %s\n", request);
    cJSON * json_request = cJSON_Parse(request);
    cJSON * command = cJSON_GetObjectItem(json_request, "command");
    int command_int = atoi(command->valuestring);

    int is_image = 0;

    switch (command_int) {
        case CREATE_PROFILE:
            *json_response = create_profile(json_request, database);
            break;

        case SEARCH_BATCH:
            *json_response = search(json_request, database);
            break;

        case LIST_ALL:
            *json_response = search(json_request, database);
            break;

        case FIND_PROFILE:
            *json_response = search_profile(json_request, database);
            break;

        case GET_IMAGE:
            *json_response = get_image(json_request, database);
            is_image = 1;
            break;

        case DELETE_PROFILE:
            *json_response = delete_profile(json_request, database);
            break;

        default:
            *json_response = create_error_response(400, "Invalid command");
            break;
    };

    printf("arrived in send response\n");
    return is_image;
}

int main() {

    start_server();

    return 0;
}