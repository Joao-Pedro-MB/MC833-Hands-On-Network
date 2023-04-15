#include "server-socket.h"

void test_server(void) {
    printf("test_server() called\n");

};

char * answer_request(char * request) {
    printf("answer_request() called\n");
    printf("request: %s\n", request);

    cJSON * json_request = cJSON_Parse(request);
    cJSON * command = cJSON_GetObjectItem(json_request, "Command");


    cJSON * json_response = cJSON_CreateObject();

    switch (command->valueint) {
        case CREATE_PROFILE:
            //TODO: create new profile
            break;

        case SEARCH_BATCH:
            //TODO: search group of profiles
            break;

        case LIST_ALL:
            //TODO: show all profiles
            break;

        case FIND_PROFILE:
            //TODO: find profile
            break;

        case DELETE_PROFILE:
            //TODO: delete profile
            break;

        default:
            //TODO: error
            break;
    };

    char * response = cJSON_Print(json_response);

    cJSON_Delete(json_response);
    cJSON_Delete(json_request);

    return response;
}

int main() {
    start_server();

    return 0;
}