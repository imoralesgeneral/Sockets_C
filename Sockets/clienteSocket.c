#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct sockaddr_in cliente; // Estructura con info para la conexión
struct hostent *servidor; // Estructura con info del host
int puerto, conexion;
char buffer[100];


void asignarNombre() {
        printf("> Introduzca su nombre: ");
        fgets(buffer, 100, stdin);
        send(conexion,buffer,100,0);
        bzero(buffer,100);
        recv(conexion,buffer,100,0);
        printf("%s",buffer);
}

void intercambioMensajes() {
    int exit = 0;
    char *salir = "exit";
    while(exit == 0) {
            printf("> ");
            fgets(buffer, 100, stdin);
            send(conexion,buffer,100,0);
            if(strncmp(buffer, salir,4) == 0) {
                exit = 1;
            }
            bzero(buffer,100);
            recv(conexion,buffer,100,0);
            printf("%s",buffer);
    }
}

int main(int argc, char **argv) {   
    
    servidor = gethostbyname("localhost");
    if(servidor == NULL)
    {
        printf("Host no conocido");
        return 1;
    }
    puerto=6000;
    conexion = socket(AF_INET, SOCK_STREAM, 0); // Asigno socket
    bzero((char *)&cliente, sizeof((char *)&cliente)); // relleno la estructura cliente con 0s
    cliente.sin_family = AF_INET;
    cliente.sin_port = htons(puerto);
    servidor->h_addr_list = (char **)&cliente.sin_addr.s_addr;
    
    if(connect(conexion,(struct sockaddr *)&cliente, sizeof(cliente)) < 0)
    {
        printf("Error al conectar con el servidor\n");
        close(conexion);
        return 1;
    } else 
    {
        asignarNombre();
    }
    
    intercambioMensajes();
    
    return 0;
}
