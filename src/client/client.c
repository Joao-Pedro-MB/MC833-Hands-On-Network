#include <stdio.h>
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

cJSON * format_message(char * Command, char * Field, char * Comparison_method, char * Value) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "Command", Command);
    cJSON_AddStringToObject(root, "Field", Field);
    cJSON_AddStringToObject(root, "Comparison Method", Comparison_method);
    cJSON_AddStringToObject(root, "Value", Value);
    return root;
}

cJSON * create_new_user() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%s",client_input);

    return format_message("create-profile", NULL, NULL, client_input);
}

cJSON * show_all_profiles() {
    return format_message("show-all", NULL, NULL, NULL);
}

cJSON * search_group_of_profiles() {
    char field[100], comparison_method[100], value[100];
    printf( "Type the field, comparison method(>, <, <=, >=, =, !=) and value devided by commas (ex: Age, >, 29):\n");
    scanf ("%s,%s,%s",field, comparison_method, value);

    return format_message("search-batch", field, comparison_method, value);
}

cJSON * find_profile() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%s",client_input);

    return format_message("search-profile", NULL, NULL, client_input);
}

cJSON * delete_profile() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%s",client_input);

    return format_message("delete-profile", NULL, NULL, client_input);
}

int main () {
    int client_input_int;

    printf( "Escolha a ação que deseja realizar digitando o número correspondente:\n\
     1 - cadastrar um novo perfil utilizando o email como identificador;\n\
     2 - listar perfis com base em um critério (maior, menor, igual, contém, não contém);\n\
     3 - listar todas as informações de todos os perfis;\n\
     4 - dado o email de um perfil, retornar suas informações;\n\
     5 - remover um perfil;\n");
    scanf ("%d",&client_input_int);

   printf( "\nYou entered: %d\n", client_input_int);

   cJSON * request;

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

    use_socket(request);
}
