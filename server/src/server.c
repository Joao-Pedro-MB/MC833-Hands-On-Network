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

    switch (command->valuestring) {
        case "create-profile":
            //TODO: create new profile
            break;

        case "search-batch":
            //TODO: search group of profiles
            break;

        case "show-all":
            //TODO: show all profiles
            break;

        case "search-profile":
            //TODO: find profile
            break;

        case "delete-profile":
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