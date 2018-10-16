#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include "include/main.h"
#include "include/options.h"
#include "../Shared/include/executionValidator.h"
#include "../Shared/include/lib.h"

#define MAX_STREAMS 64;
#define MAX_BUFFER 1024;
void prepareForSending(char **username, char **password);

//config socket_config;
struct sockaddr_in addr;

int main(int argc, char ** argv)
{
    initialize_options();

    //Saludo al usuario y le informo que se va a intentar hacer una conexion al proxy
    printf("Hello! Starting connection...\n");
    //Waiting for conection
    int fd = createConnection();

    char status = 0;//0 desconectado, 1 conectado, 2 quitting.
    while(status!=2)
    {
        //Tengo que loggearme
        requestForLogin(&status);
        if(status == 1)
        {
            //Me conecte exitosamente entonces entro en otro modo
            interaction(fd);
        }
    }
    //CierreDeConexion
    closeConnection();
    //Saludo de despedida
    printf("Goodbye, hope to see you soon!\n");
    return 0;
}

void initialize_options()
{
    __bzero((void*)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9090);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
}
int createConnection()
{
    int fd, ret;
    struct sctp_initmsg initmsg;
    struct sctp_event_subscribe events;


    //Creo una conexion SCTP con los valores (default 9090, 127.0.0.1)
    if((fd = socket(AF_UNSPEC,SOCK_STREAM,IPPROTO_SCTP)) == -1 )
    {
    printf("An error has ocurred while creating SCTP socket\n");
    perror("socket");
    exit(1);
    }
//    //Configuro la cantidad de streams disponible para el socket
//    memset(&initmsg,0, sizeof(struct sctp_initmsg));
//    initmsg.sinit_num_ostreams  = MAX_STREAMS;
//    initmsg.sinit_max_instreams = MAX_STREAMS;
//    initmsg.sinit_max_attempts  = MAX_STREAMS;
//    ret = setsockopt(fd,IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg));
//    if(ret<0)
//    {
//        perror("setsockopt SCTP_INITMSG");
//        exit(1);
//    }
//
//    //Configuro los eventos
//    events.sctp_association_event = 1;
//    events.sctp_data_io_event = 1;
//    ret = setsockopt(fd,IPPROTO_SCTP,SCTP_EVENTS, &events, sizeof(events));
//    if(ret<0)
//    {
//        perror("setsockopt SCTP_EVENTS");
//        exit(1);
//    }
    //Realizo la conexion
    if((ret = connect(fd,(struct sockaddr*)&addr, sizeof(addr))) == -1)
    {
    printf(("An error has ocurred while connecting the SCTP socket\n"));
    perror("connect");
    exit(1);
    }
    return fd;
}
void requestForLogin(char* status)
{
    //Hago la solicitud al servidor
    if (requestLoginToProxy())
    {
        //Respuesta positiva
        loginSuccess(status);
    } else
    {
        //Respuesta negativa
        loginError(status);
    }
}

void loginError(char* status)
{
    //Aviso que no se puedo autenticar
    printf("Login failed. Do you wish to retry? [Y/n]: ");
    //Pregunto si quiere volver a intentar o si quiere quitear
    char * input = calloc(1,INITIAL_INPUT_SIZE);
    size_t count = fetchLineFromStdin(&input,INITIAL_INPUT_SIZE);
    if(count!=1)
    {
        printf("Please input only Y/n");
    } else
    {
        if(*input == 'Y' || *input == 'y')
        {
            requestForLogin(status);
        }
        else if(*input == 'N' || *input == 'n')
        {
            //Si quitea hago *status = 2;
            *status = 2;
        }
    }
    free(input);
}

void loginSuccess(char* status)
{
    //Aviso que se conecto
    printf("Login succesful\n");
    //Seteo variable para que salga del while
    *status = 1;
}

char requestLoginToProxy(int fd){
    char * usernameInput = calloc(1,INITIAL_INPUT_SIZE);
    char * passwordInput = calloc(1,INITIAL_INPUT_SIZE);
    //Le solicito un usuario
    printf("Please enter your username: ");
    fetchLineFromStdin(&usernameInput,INITIAL_INPUT_SIZE);
    printf("+OK\n");
    //Le solicito una contraseña
    printf("Please enter your password: ");
    fetchLineFromStdin(&passwordInput,INITIAL_INPUT_SIZE);
    prepareForSending(&usernameInput,&passwordInput);

    //En la conexion 9090 le envia con USER name el parametro obtenido del usuario
    int ret = sctp_sendmsg(fd,usernameInput,sizeof(usernameInput), NULL, 0, 0, 0, 0, 0, 0);
    if(ret == -1)
    {
        printf("An error has ocurred sending USER info\n");
        perror("sctp_sendmsg");
        exit(1);
    }
    ret = sctp_sendmsg(fd,passwordInput,sizeof(passwordInput), NULL, 0, 0, 0, 0, 0, 0);
    if(ret == -1)
    {
        printf("An error has ocurred sending PASS info\n");
        perror("sctp_sendmsg");
        exit(1);
    }
    //Luego le envia la contraseña con PASS string
    //Parsea la respuesta del proxy
    //Devuelve 1 si fue exitoso
    //Devuelve 0 si falla
    //Libera memoria
    free(usernameInput);
    free(passwordInput);
    return 1;
}

void prepareForSending(char **username, char **password) {
    char * user = calloc(1,INITIAL_INPUT_SIZE + 6);
    strcpy(user,"USER: ");
    strcat(user, *username);
    strcpy(*username,user);
    char * pass = calloc(1,INITIAL_INPUT_SIZE + 5);
    strcpy(pass,"PASS: ");
    strcat(pass, *password);
    strcpy(*password,pass);
    free(user);
    free(pass);
}

void interaction(int fd)
{
    //Aca se que estoy logeado y quiero procesar acciones del usuario con una maquina de estados
    //Leer que es lo que quiere hacer
//    char quit = 0;
//    while(!quit) {
//        function command;
//        char* parameter = "";
//        //Llamar a la funcion de pablo para leer de input
//        //Llamar a la funcion de alguien para parsear lo que me dio pablo
//        switch (command) {
//            case EXIT:
//                quit = 1;
//                break;
//            case STATS:
//                //Hacer request al proxy por los stats
//                //Reportar la respuesta
//                break;
//            case COMMAND:
//                //Hacer request para cambio de transformacion.
//                //Como esta es una funcion critica hay que agregar cosas en el header como etag para diferenciar entre
//                //pedidos de distintos usuarios.
//                //Reportar respuesta
//                break;
//            default:
//                //Input incorrecto
//                break;
//        }
//    }
    char buffer[1024];

    while(1)
    {
        if(fgets(buffer,1024,stdin))
        {
            closeConnection();
            exit(-1);
        }
        buffer[strcspn(buffer,"\r\n")];
        size_t length = strlen(buffer);
        int ret = sctp_sendmsg(fd,(void*)buffer,length,NULL, 0, 0, 0, 0, 0, 0);
        if(ret == -1)
        {
            printf("An error has ocurred sending the message\n");
            perror("sctp_sendmsg");
            exit(1);
        }
    }
}
void closeConnection()
{

}
