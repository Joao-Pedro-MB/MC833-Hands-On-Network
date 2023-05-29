#include "client-socket.h"

/*
• cadastrar um novo perfil utilizando o email como identificador;
• listar todas as pessoas (email e nome) formadas em um determinado curso;
• listar todas as pessoas (email e nome) que possuam uma determinada habilidade;
• listar todas as pessoas (email, nome e curso) formadas em um determinado ano;
• listar todas as informações de todos os perfis;
• dado o email de um perfil, retornar suas informações;
• remover um perfil a partir de seu identificador (email);
*/

#define BUFFER_SIZE 100000

void parse_string(char* input, char* field, char* operation, char* value) {
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

char * format_message(int command, char * field, char * comparison_method, char * value) {
    char string_command[2];
    sprintf(string_command, "%d", command);
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "command", string_command);
    cJSON_AddStringToObject(root, "field", field);
    cJSON_AddStringToObject(root, "operation", comparison_method);
    cJSON_AddStringToObject(root, "value", value);

    char * answer = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (field != NULL) {
        free(field);
    }
    if (comparison_method != NULL) {
        free(comparison_method);
    }
    if (value != NULL) {
        free(value);
    }

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
    cJSON *root = cJSON_Parse(response);
    cJSON* message = cJSON_GetObjectItem(root, "Message");
    cJSON* status = cJSON_GetObjectItem(root, "Status");

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
    } else {
        //it is a image
    }
}

char * create_new_user() {
    char email[100], name[100], age[100], city[100], state[100], scholarity[100], graduationYear[10], skills[1000];
    printf( "Type new user's email:\n");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = '\0';

    printf( "Type new user's name:\n");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf( "Type new user's age:\n");
    fgets(age, sizeof(age), stdin);
    age[strcspn(age, "\n")] = '\0';

    printf( "Type new user's city:\n");
    fgets(city, sizeof(city), stdin);
    city[strcspn(city, "\n")] = '\0';

    printf( "Type new user's state initials (ex: SP):\n");
    fgets(state, sizeof(state), stdin);
    state[strcspn(state, "\n")] = '\0';

    printf( "Type new user's scholarity level (ex: Computer Science Bachelor):\n");
    fgets(scholarity, sizeof(scholarity), stdin);
    scholarity[strcspn(scholarity, "\n")] = '\0';

    printf( "Type new user's graduation year (it can be in the future):\n");
    fgets(graduationYear, sizeof(graduationYear), stdin);
    graduationYear[strcspn(graduationYear, "\n")] = '\0';

    printf( "Type de new user's skills separated by comma (ex: Java, Python, C, ...):\n");
    fgets(skills, sizeof(skills), stdin);
    skills[strcspn(skills, "\n")] = '\0';

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", name);
    cJSON_AddStringToObject(root, "age", age);
    cJSON_AddStringToObject(root, "email", email);
    cJSON_AddStringToObject(root, "city", city);
    cJSON_AddStringToObject(root, "state", state);
    cJSON_AddStringToObject(root, "scholarity", scholarity);
    cJSON_AddStringToObject(root, "graduationYear", graduationYear);
    cJSON_AddStringToObject(root, "skills", skills);
    char * message = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    return format_message(CREATE_PROFILE, NULL, NULL, message);
}

char * show_all_profiles() {
    char* field;
    field = (char *) malloc(10 * sizeof(char));
  
  strncpy(field, "All", 3);
  field[4] = '\0';
    return format_message(LIST_ALL, field, NULL, NULL);
}

char * search_group_of_profiles() {
    char* field = (char*) malloc(100*sizeof(char));
    char* operation = (char*) malloc(100*sizeof(char));
    char* value = (char*) malloc(100*sizeof(char));
    char* input = (char*) malloc(1000*sizeof(char));
    printf( "Type the field, comparison method(>, <, ==, <=, >=, !=) and value devided by space (ex: Age > 29):\n");
    
    fgets(input, 1000, stdin);
    input[strcspn(input, "\n")] = '\0';
    parse_string(input, field, operation, value);

    return format_message(SEARCH_BATCH, field, operation, value);
}

char * find_profile() {
    char* client_input;
    client_input = (char*) malloc(100*sizeof(char));
    printf( "Type the user's email:\n");
    scanf ("%s",client_input);

    return format_message(FIND_PROFILE, NULL, NULL, client_input);
}

char * add_picture() {
    char image[BUFFER_SIZE];
    char * client_input;
    client_input = (char*) malloc(100*sizeof(char));
    printf( "Type the image name and extension (ex: image.png):\n");
    scanf("%s",client_input);
    printf("aqui embaixo deveria aparecer o input do usuario:\n");
    printf("%s\n", client_input);


    //size_t bytesRead = get_image("./client/image/silver-gull.jpg", image);
    printf("aqui significa que conseguiu ler\n");
    //write_image(image, bytesRead);
    printf("aqui significa q conseguiu escrever:\n");
    //image[bytesRead] = '\0';
    return format_message(ADD_PICTURE, NULL, NULL, client_input);
}

char * delete_profile() {
    char* client_input;
    client_input = (char*) malloc(100*sizeof(char));
    printf( "Type the user's email:\n");
    scanf("%s",client_input);

    return format_message(DELETE_PROFILE, NULL, NULL, client_input);
}

int main() {
    int client_input_int;
    char trash[2];
    

    printf( "Escolha a ação que deseja realizar digitando o número correspondente:\n\
     1 - cadastrar um novo perfil utilizando o email como identificador;\n\
     2 - listar perfis com base em um critério (>, <, ==, >=, <=, !=);\n\
     3 - listar todas as informações de todos os perfis;\n\
     4 - dado o email de um perfil, retornar suas informações;\n\
     5 - enviar uma foto ao servidor;\n\
     6 - remover um perfil;\n");
    scanf("%d",&client_input_int);
    trash[0] = getchar();

   char * request;

    switch (client_input_int) {
        case 1:
            request = create_new_user();
            break;

        case 2:
            request = search_group_of_profiles();
            break;

        case 3:
            request = show_all_profiles();
            break;

        case 4:
            request = find_profile();
            break;

        case 5:
            request = add_picture();
            break;
        case 6:
            request = delete_profile();
            break;

        default:
            printf("You entered an invalid command. Please try again.");
            exit(1);
    };

    printf("Request: %s\n", request);

    int err = use_socket(request, client_input_int == 5);
    free(request);

    if (err != 0) {
        printf("Error: %d\n", err);
        exit(1);
    }

    err = start_listener();
    if (err != 0) {
        printf("Error in listener: %d\n", err);
        exit(1);
    }
}
