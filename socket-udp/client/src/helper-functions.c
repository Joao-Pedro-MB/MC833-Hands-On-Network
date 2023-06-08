#include "client-socket.h"

void parse_string(char* input, char field[100], char operation[100], char* value) {
    // Find the first space character in the input string
    char * space_pos = strchr(input, ' ');
    if (space_pos == NULL) {
        printf("Invalid input string\n");
        return;
    }

    // Copy the field string up to the first space character
    strncpy(field, input, space_pos - input);
    field[space_pos - input] = '\0';

    // Find the second space character in the input string
    char * second_space_pos = strchr(space_pos + 1, ' ');
    if (second_space_pos == NULL) {
        printf("Invalid input string\n");
        return;
    }

    // Copy the operation string between the first and second space characters
    strncpy(operation, space_pos + 1, second_space_pos - space_pos - 1);
    operation[second_space_pos - space_pos - 1] = '\0';

    // Copy the value string after the second space character
    strcpy(value, second_space_pos + 1);
}

char * format_message(int command, char field[100], char comparison_method[100], char * value) {
    char string_command[2];
    sprintf(string_command, "%d", command);
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "command", string_command);
    cJSON_AddStringToObject(root, "field", field);
    cJSON_AddStringToObject(root, "operation", comparison_method);
    cJSON_AddStringToObject(root, "value", value);

    char * answer = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    return answer;
}

size_t get_image(char * image_name, char * buffer) {
    FILE * file = fopen(image_name, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    fread(buffer, 1, file_size, file);
    fclose(file);
    return file_size;
}

void write_image(char * buffer, size_t size) {

    FILE * file  = fopen("./client/image/destination.jpg", "wb");
    if (file == NULL) {
        perror("Error opening destination file");
        exit(1);
    }

    fwrite(buffer, 1, size, file);
    fclose(file);
}

void parse_response(char * response) {
    cJSON * root = cJSON_Parse(response);
    cJSON * message = cJSON_GetObjectItem(root, "Message");
    cJSON * status = cJSON_GetObjectItem(root, "Status");

    if (status->valueint == 200){
        printf("Message: %s\n", message->valuestring);

    } else {
        printf("Error: %s, Message: %s\n", status->valuestring, message->valuestring);
        
    } 
}

void receive_answer(struct Packet packets[], int num_packets) {
    if (num_packets == 0) {
        printf("No packets received\n");
        return;
    } else if (num_packets == 1) {
        parse_response(packets[0].data);
        return;
    } 
}