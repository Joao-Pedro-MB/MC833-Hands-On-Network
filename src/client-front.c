#include <stdio.h>

/*
• cadastrar um novo perfil utilizando o email como identificador;
• listar todas as pessoas (email e nome) formadas em um determinado curso;
• listar todas as pessoas (email e nome) que possuam uma determinada habilidade;
• listar todas as pessoas (email, nome e curso) formadas em um determinado ano;
• listar todas as informações de todos os perfis;
• dado o email de um perfil, retornar suas informações;
• remover um perfil a partir de seu identificador (email);
*/

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

    switch (client_input_int) {
        case 1:
            create_new_user();
            break;

        case 2:
            search_group_of_profiles();
            break;

        case 3:
            show_all_profiles();
            break;

        case 4:
            find_profile();
            break;

        case 5:
            delete_profile();
            break;

        default:
            printf("You entered an invalid command. Please try again.");
            exit(1);
    };
}

void create_new_user() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%d",&client_input);

    char * request = format_message("create-profile", NULL, NULL, client_input);
}

void show_all_profiles() {
    char * request = format_message("show-all", NULL, NULL,  NULL, NULL);
}

void search_group_of_profiles() {
    char field[100], comparison_method[100], value[100];
    printf( "Type the field, comparison method(greater than, less than, equal, different, contains, excludes) and value devided by commas (ex: Age, greater than, 29):\n");
    scanf ("%s,%s,%s",&field, &comparison_method, &value);

    char * request = format_message("search-batch", field, comparison_method, value);
}

void find_profile() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%d",&client_input);

    char * request = format_message("search-profile", NULL, NULL, client_input);
}

void delete_profile() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%d",&client_input);

    char * request = format_message("delete-profile", NULL, NULL, client_input);
}

char * format_message(char * Command, char * Field, char * Comparison_method, char * Value) {
    char * message = malloc(1000);
    sprintf(message, "{\"Command\": %s, \"Field\": %s, \"Comparison_Method\": %s, \"Value\": %s}", action, field, comparison_method, value);
    return message;
}