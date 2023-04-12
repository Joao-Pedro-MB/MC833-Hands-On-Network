#include <stdio.h>


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
}

void search_group_of_profiles() {
    char field[100], comparison_method[100], value[100];
    printf( "Type the field, comparison method(greater than, less than, equal, different, contains, excludes) and value devided by commas (ex: Age, greater than, 29):\n");
    scanf ("%s,%s,%s",&field, &comparison_method, &value);
}

void find_profile() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%d",&client_input);
}

void delete_profile() {
    char client_input[100];
    printf( "Type de new user's email:\n");
    scanf ("%d",&client_input);
}