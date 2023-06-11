#include "client-socket.h"

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
        printf("Error: %d, Message: %s\n", status->valueint, message->valuestring);
        
    } 
}

void receive_answer(struct Packet packets[], int num_packets) {
    printf("num_packets: %d\n", num_packets);
    if (num_packets == 0) {
        printf("No packets received\n");
        return;
    } else if (num_packets == 1) {
        parse_response(packets[0].data);
        return;
    } 
}