#include "server-socket.h"

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

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    *output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) {
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        encoded_data[j++] = base64_chars[(triple >> 18) & 0x3F];
        encoded_data[j++] = base64_chars[(triple >> 12) & 0x3F];
        encoded_data[j++] = base64_chars[(triple >> 6) & 0x3F];
        encoded_data[j++] = base64_chars[triple & 0x3F];
    }

    // Pad the remaining bytes with '=' characters
    for (size_t padding = 0; padding < (3 - (input_length % 3)) % 3; padding++) {
        encoded_data[*output_length - 1 - padding] = '=';
    }

    return encoded_data;
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

cJSON * access_database() {
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

char * create_error_response(int status, const char * message) {
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

char * format_response(int status, char* message) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "status", status);
    cJSON_AddStringToObject(root, "message", message);
    char* json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_str;
}

char * compare_database(cJSON* field, cJSON* operation, cJSON* value, cJSON* profiles_array) {
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