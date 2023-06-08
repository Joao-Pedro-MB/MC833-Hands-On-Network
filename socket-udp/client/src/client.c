#include "client-socket.h"

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

// todo: organize this function
char * show_all_profiles() {
    char field[10];
  
  strncpy(field, "All", 3);
  field[4] = '\0';
    return format_message(LIST_ALL, field, NULL, NULL);
}

// todo improve the comparision method for spaced values
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

char * add_picture() {
    char image[BUFFER_SIZE], client_input[100];

    // receive the image name
    printf( "Type the image name and extension (ex: image.png):\n");
    scanf("%s",client_input);

    // return the payload and commands to be sent to the server
    return format_message(ADD_PICTURE, NULL, NULL, client_input);
}

char * delete_profile() {
    char client_input[100];
    
    // receive key for profile deletion
    printf( "Type the user's email:\n");
    scanf("%s",client_input);

    // return the payload and commands to be sent to the server
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

    int err = use_socket(request, client_input_int == 5);
    free(request);

    if (err != 0) {
        printf("Error: %d\n", err);
        exit(1);
    }

}
