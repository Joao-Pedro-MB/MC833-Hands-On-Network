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
    char * result = malloc(400);
    cJSON * image_name = cJSON_GetObjectItem(request, "value");

    if (image_name == NULL) {
        return create_error_response(400, "Invalid request");
    }

    // Create a string variable
    sprintf(result, "./server/image/%s.jpg", (cJSON_GetObjectItem(request, "value")->valuestring));

    return result;
}

static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

int base64_index(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    }
    if (c == '+') {
        return 62;
    }
    if (c == '/') {
        return 63;
    }
    return -1; // Invalid character
}

unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length) {
    if (input_length % 4 != 0) {
        return NULL;
    }

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') {
        (*output_length)--;
    }
    if (data[input_length - 2] == '=') {
        (*output_length)--;
    }

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) {
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : base64_index(data[i++]);
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : base64_index(data[i++]);
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : base64_index(data[i++]);
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : base64_index(data[i++]);

        uint32_t triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;

        if (j < *output_length) {
            decoded_data[j++] = (triple >> 16) & 0xFF;
        }
        if (j < *output_length) {
            decoded_data[j++] = (triple >> 8) & 0xFF;
        }
        if (j < *output_length) {
            decoded_data[j++] = triple & 0xFF;
        }
    }

    return decoded_data;
}

char * add_image(cJSON * request, cJSON * database) {
    printf("enter the add image funtion\n");

    char image_path[400] = "";
    char * image = cJSON_GetObjectItem(request, "value")->valuestring;

    // Create a string variable
    sprintf(image_path, "./server/image/%s.jpg", (cJSON_GetObjectItem(request, "field")->valuestring));

    FILE* image_file;

    image_file = fopen(image_path, "wb");
    if (image_file == NULL) {
        printf("Failed to open output file.\n");
        exit(1);
    }

        // Base64 decoding
    size_t decoded_length = 0;
    unsigned char *decoded_data = base64_decode(image, strlen(image), &decoded_length);

    // Write the buffer to the output file
    int elements_written = fwrite(decoded_data, sizeof(char), decoded_length, image_file);
    free(decoded_data);
    free(image);


    if (elements_written != decoded_length) {
        printf("Error writing to the file.\n");
        fclose(image_file);
        return format_response(500,"Internl server error");
    }

    fclose(image_file);
    return format_response(200,"Image added successfully");


}

int build_request(struct Packet packets[], int num_packets, char * request) {
    printf("build_request() called\n");

    printf("num_packets: %d\n", num_packets);

    if (num_packets == 0) {
        return 1;
    }

    char tmp_request[500000] = "";
    for (int i = 0; i < num_packets; i++) {
        
        printf("packet size| %ld\n", strlen(packets[i].data));
        if (i < num_packets - 1) {
            packets[i].data[4096] = '\0';
        }
        printf("new packet size| %ld\n", strlen(packets[i].data));
        strcat(tmp_request, packets[i].data);
    }

    sprintf(request, "%s", tmp_request);

    printf("request size: %ld\n", strlen(request));
    printf("request: %s\n", request);
    return 0;
}

int answer_request(struct Packet packets[], int num_packets, char ** json_response) {
    printf("answer_request() called\n");
    cJSON * database = access_database();
    char request[BUFFER_SIZE] = "";
    int err = 0;

    // check request integrity and build it
    err = build_request(packets, num_packets, request);

    cJSON * json_request = cJSON_Parse(request);
    printf("json_request: %s\n", cJSON_PrintUnformatted(json_request));
    cJSON * command = cJSON_GetObjectItem(json_request, "command");


    int command_int = atoi(command->valuestring);

    int is_image = 0;

    switch (command_int) {
        case CREATE_PROFILE:
            printf("creating profile\n");
            *json_response = create_profile(json_request, database);
            break;

        case SEARCH_BATCH:
            printf("searching batch\n");
            *json_response = search(json_request, database);
            break;

        case LIST_ALL:
            printf("listing all\n");
            *json_response = search(json_request, database);
            break;

        case FIND_PROFILE:
            printf("finding profile\n");
            *json_response = search_profile(json_request, database);
            break;

        case GET_IMAGE:
            printf("getting profile image\n");
            *json_response = get_image(json_request, database);
            is_image = 1;
            break;

        case DELETE_PROFILE:
            printf("deleting profile\n");
            *json_response = delete_profile(json_request, database);
            break;

        case ADD_IMAGE:
            printf("adding image\n");
            *json_response = add_image(json_request, database);
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