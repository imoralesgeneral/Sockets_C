#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_USUARIOS 3

int clientes_conectados=0;
int conexion_servidor, puerto;
struct sockaddr_in servidor; 
char buffer[100];
int cliente_conexion[MAX_USUARIOS];

struct usuario {
    char nombre[10];
    struct sockaddr_in netID;
    socklen_t longc;
} usuarios[MAX_USUARIOS];

void conectarCliente() {
    while(clientes_conectados < MAX_USUARIOS) {
        listen(conexion_servidor, MAX_USUARIOS);
        usuarios[clientes_conectados].longc = sizeof(usuarios[clientes_conectados].netID);
        cliente_conexion[clientes_conectados] = accept(conexion_servidor, (struct sockaddr *)&usuarios[clientes_conectados].netID, &usuarios[clientes_conectados].longc);
        if(cliente_conexion[clientes_conectados] < 0)
        {
            printf("Error al conectar con el cliente\n");
            close(conexion_servidor);
        }
        recv(cliente_conexion[clientes_conectados], buffer, 100, 0);
        strcpy(usuarios[clientes_conectados].nombre,buffer);
        bzero((char *)&buffer, sizeof(buffer));
        send(cliente_conexion[clientes_conectados], "Bienvenido\n", 13, 0);
        printf("Conectando con %s:%d -- Usuario: %s\n", inet_ntoa(usuarios[clientes_conectados].netID.sin_addr), htons(usuarios[clientes_conectados].netID.sin_port), usuarios[clientes_conectados].nombre);
        clientes_conectados = clientes_conectados+1;
    }
}

void intercambioMensajes(long arg) {
    long usuarioID = (long) arg;
    int exit = 0;
    while(exit == 0 && clientes_conectados > 0) {        
        char *salir = "exit";
        char *blank = "";
        if(recv(cliente_conexion[usuarioID], buffer, 100, 0) < 0)
        {
            printf("Error al recibir los datos\n");
            close(conexion_servidor);
            clientes_conectados = clientes_conectados -1;
        }
        else 
        {
            if(strncmp(buffer, salir,4) == 0) {
                clientes_conectados = clientes_conectados -1;
                send(cliente_conexion[usuarioID], "Desconectado\n", 13, 0);
                printf("%s se ha desconectado\n", usuarios[usuarioID].nombre);
                bzero((char *)&buffer, sizeof(buffer));
                exit = 1;
                continue;
            } else if(strncmp(buffer, blank,4) == 0) {
                bzero((char *)&buffer, sizeof(buffer));
                exit = 1;
                continue;
            } else {
                printf("%s --> %s\n", usuarios[usuarioID].nombre, buffer);
                send(cliente_conexion[usuarioID], "Recibido\n", 13, 0);
            }
        }
        bzero((char *)&buffer, sizeof(buffer));host
    }    
}

int main(int argc, char **argv) {
    
    pid_t conex;
    int stat_conex;
    pid_t pids[MAX_USUARIOS];
    int stat[MAX_USUARIOS];
    
    // Creo el socket en el servidor
    puerto = 6000;
    conexion_servidor = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *)&servidor, sizeof(servidor));
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    servidor.sin_addr.s_addr = INADDR_ANY;
    if(bind(conexion_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
    {
       printf("Error al asociar el puerto");
       close(conexion_servidor);
       return 1;
    }    
    
    for(int i=0; i<MAX_USUARIOS; i++) 
    {
        conectarCliente();
    }
    
    for(long i=0; i<MAX_USUARIOS; i++) 
    {
        if((pids[i]=fork()) == 0) {
            intercambioMensajes(i);
        } 
    }
    for(long i=0; i<MAX_USUARIOS; i++) 
    {
        wait(&stat[i]);
    }
    
    close(conexion_servidor);
    return 0;
}


