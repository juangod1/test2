#ifndef PROTOS_TPE_MAIN_H
#define PROTOS_TPE_MAIN_H

typedef const int file_descriptor;

#define MUA_PORT 1110
#define ORIGIN_PORT  110
#define SELECT_FILE_DESCRIPTOR_AMOUNT 2

file_descriptor setup_MUA_socket();
file_descriptor setup_origin_socket(char * origin_address);
void error();
void run_server(file_descriptor MUA_sock, file_descriptor Origin_sock);
int max(int a, int b);
int findMax(int * a, int size);

int proxy_main();
#endif //PROTOS_TPE_MAIN_H
