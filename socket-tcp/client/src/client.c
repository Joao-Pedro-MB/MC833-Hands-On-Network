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

char * create_new_user() {
    char email[100], name[100], age[100], city[100], state[100], scholarity[100], graduationYear[10], skills[1000];

    // receive all client inputs
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


    // create json object
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", name);
    cJSON_AddStringToObject(root, "age", age);
    cJSON_AddStringToObject(root, "email", email);
    cJSON_AddStringToObject(root, "city", city);
    cJSON_AddStringToObject(root, "state", state);
    cJSON_AddStringToObject(root, "scholarity", scholarity);
    cJSON_AddStringToObject(root, "graduationYear", graduationYear);
    cJSON_AddStringToObject(root, "skills", skills);

    // formt the json object to string
    char * message = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    // return the payload and commands to be sent to the server
    return format_message(CREATE_PROFILE, NULL, NULL, message);
}

char * show_all_profiles() {
    char field[100];
  
  strncpy(field, "All", 3);
  field[3] = '\0';
    return format_message(LIST_ALL, field, NULL, NULL);
}

char * search_group_of_profiles() {
    char field[100], operation[100], value[100], input[1000];

    // receive the query from the client
    printf( "Type the field, comparison method(>, <, ==, <=, >=, !=) and value devided by space (ex: Age > 29):\n");
    
    fgets(input, 1000, stdin);
    input[strcspn(input, "\n")] = '\0';
    parse_string(input, field, operation, value);

    // return the payload and commands to be sent to the server
    return format_message(SEARCH_BATCH, field, operation, value);
}

char * find_profile() {
    char client_input[100];

    // receive key for profile search
    printf( "Type the user's email:\n");
    scanf ("%s",client_input);

    // return the payload and commands to be sent to the server
    return format_message(FIND_PROFILE, NULL, NULL, client_input);
}

char * delete_profile() {
    char client_input[100];
    
    // receive key for profile deletion
    printf( "Type the user's email:\n");
    scanf("%s",client_input);

    // return the payload and commands to be sent to the server
    return format_message(DELETE_PROFILE, NULL, NULL, client_input);
}

int main (int argc, char *argv[]) {
    int client_input_int;
    char trash[2];
    

    printf( "Escolha a ação que deseja realizar digitando o número correspondente:\n\
     1 - cadastrar um novo perfil utilizando o email como identificador;\n\
     2 - listar perfis com base em um critério (>, <, ==, >=, <=, !=);\n\
     3 - listar todas as informações de todos os perfis;\n\
     4 - dado o email de um perfil, retornar suas informações;\n\
     5 - remover um perfil;\n");
    scanf ("%d",&client_input_int);
    trash[0] = getchar();

   char * request;
   char response[MAXDATASIZE];

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
            request = delete_profile();
            break;

        default:
            printf("You entered an invalid command. Please try again.");
            exit(1);
    };

    int err = use_socket(request, response, argc, argv);
    free(request);

    if (err != 0) {
        printf("Error: %d\n", err);
        exit(1);
    }
    parse_response(response);
}
