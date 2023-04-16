#include "server-socket.h"
cJSON * create_profile(cJSON * request) {
    printf("create_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * name = cJSON_GetObjectItem(request, "Name");
    cJSON * age = cJSON_GetObjectItem(request, "Age");
    cJSON * email = cJSON_GetObjectItem(request, "Email");
    cJSON * phone = cJSON_GetObjectItem(request, "Phone");

    if (name == NULL || age == NULL || email == NULL || phone == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        cJSON * json_response = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile created");
    }

    return json_response;
}

cJSON * search(cJSON * request) {
    printf("search() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * name = cJSON_GetObjectItem(request, "Name");
    cJSON * age = cJSON_GetObjectItem(request, "Age");
    cJSON * email = cJSON_GetObjectItem(request, "Email");
    cJSON * phone = cJSON_GetObjectItem(request, "Phone");

    if (name == NULL && age == NULL && email == NULL && phone == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        cJSON * json_response = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile found");
    }

    return json_response;
}

cJSON * search_profile(cJSON * request) {
    printf("search_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * name = cJSON_GetObjectItem(request, "Name");
    cJSON * age = cJSON_GetObjectItem(request, "Age");
    cJSON * email = cJSON_GetObjectItem(request, "Email");
    cJSON * phone = cJSON_GetObjectItem(request, "Phone");

    if (name == NULL && age == NULL && email == NULL && phone == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        cJSON * json_response = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile found");
    }

    return json_response;
}

cJSON * delete_profile(cJSON * request) {
    printf("delete_profile() called\n");

    cJSON * json_response = cJSON_CreateObject();

    cJSON * name = cJSON_GetObjectItem(request, "Name");
    cJSON * age = cJSON_GetObjectItem(request, "Age");
    cJSON * email = cJSON_GetObjectItem(request, "Email");
    cJSON * phone = cJSON_GetObjectItem(request, "Phone");

    if (name == NULL && age == NULL && email == NULL && phone == NULL) {
        json_response = create_error_response(400, "Invalid request");
    } else {
        cJSON * json_response = cJSON_CreateObject();

        cJSON_AddNumberToObject(json_response, "Status", 200);
        cJSON_AddStringToObject(json_response, "Message", "Profile deleted");
    }

    return json_response;
}

cJSON * create_error_response(int status, char * message) {
    cJSON * json_response = cJSON_CreateObject();

    cJSON_AddNumberToObject(json_response, "Status", status);
    cJSON_AddStringToObject(json_response, "Message", message);

    return json_response;
}

char * answer_request(char * request) {
    printf("answer_request() called\n");
    printf("request: %s\n", request);

    cJSON * json_request = cJSON_Parse(request);
    cJSON * command = cJSON_GetObjectItem(json_request, "Command");

    cJSON * json_response = cJSON_CreateObject();

    switch (command->valueint) {
        case CREATE_PROFILE:
            json_response = create_profile(json_request);
            break;

        case SEARCH_BATCH:
            json_response = search(json_request);
            break;

        case LIST_ALL:
            json_response = search(json_request);
            break;

        case FIND_PROFILE:
            json_response = search_profile(json_request);
            break;

        case DELETE_PROFILE:
            json_response = delete_profile(json_request);
            break;

        default:
            json_response = create_error_response(400, "Invalid command");
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