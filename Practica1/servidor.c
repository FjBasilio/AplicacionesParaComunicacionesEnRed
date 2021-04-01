#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <pthread.h>

int crearSocket();
struct sockaddr_in crearServidor();
struct sockaddr_in crearCliente();
int crearBind(int socket,struct sockaddr_in dir);
struct sockaddr_in  recibir(int socket,struct sockaddr_in dir);
void enviar(int socket,unsigned char msj[],struct sockaddr_in dir);

typedef struct datos_dir{
        int udp_socket;
        struct sockaddr_in cliente;
    }*Datos;

void* Hilo_Envio(void* datos_void){
    Datos datos=(Datos)datos_void;
    unsigned char msj[100];
    
    while(1){
        printf("\nServidor:");
        fflush(stdin);
        fgets(msj,100,stdin);
        
        enviar(datos->udp_socket,msj,datos->cliente);
    }
}

void* Hilo_Recibe(void* datos_void){
    Datos datos=(Datos)datos_void;
    while(1){
        recibir(datos->udp_socket,datos->cliente);
        fflush(stdout);
    }
}

int main(){
    int bolean_hilo_envio,bolean_hilo_recibe;
    pthread_t hilo_envio,hilo_recibe;
    int udp_socket = crearSocket();
    
    struct sockaddr_in servidor=crearServidor();
    struct sockaddr_in clientevacio=crearCliente(); 

    Datos datos_envio=(Datos)malloc(sizeof(struct datos_dir));
    Datos datos_recibe=(Datos)malloc(sizeof(struct datos_dir));
    int lbind=crearBind(udp_socket,servidor);
    struct sockaddr_in clientefijo1=recibir(udp_socket,clientevacio);
    struct sockaddr_in clientefijo2=clientefijo1;
    datos_envio->udp_socket=udp_socket;
    datos_envio->cliente=clientefijo1;

    datos_recibe->udp_socket=udp_socket;
    datos_recibe->cliente=clientefijo2;
    

    // escribir mensaje a enviar
    
    if(pthread_create(&hilo_envio,NULL,&Hilo_Envio,(void*)datos_envio)){
        perror("\nError al crear el hilo\n");
    }

    if(pthread_create(&hilo_recibe,NULL,&Hilo_Recibe,(void*)datos_recibe)){
        perror("\nError al crear el hilo\n");
    }
        
    while(1); //ciclo

    close(udp_socket);
    return 0;
}

int crearSocket(){
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp_socket==-1){
        perror("Error al abrir el socket, buscar el error en el manual (man socket)\n");
        exit(0);
    }
    else{
        perror("Exito al abrir el socket.");
    }

    return udp_socket;
}

struct sockaddr_in crearServidor(){
    struct sockaddr_in servidor;
    servidor.sin_family=AF_INET; /* address family: AF_INET */
    servidor.sin_port=htons(8080);   /* port in network byte order*/
    servidor.sin_addr.s_addr=INADDR_ANY;   /* internet address */
    return servidor;
}

struct sockaddr_in crearCliente(){
    struct sockaddr_in cliente;
    //cliente.sin_family=AF_INET; /* address family: AF_INET */
    //cliente.sin_port=htons(53);  /* es el puerto por defecto por donde se manda mensaje*/
    //cliente.sin_addr.s_addr=inet_addr("8.8.8.8");  /*ip del servidor*/
    return cliente; //no se llena, solo se declara la estructura
}

int crearBind(int socket,struct sockaddr_in dir){

    int lbind=bind(socket,(struct sockaddr*)&dir,sizeof(dir));
    if(lbind==-1){
        perror("Error en bind.");
        exit(0);
    }
    else{
        perror("Exito en bind.");
    }
    return lbind;
}

struct sockaddr_in recibir(int socket,struct sockaddr_in cli){
    unsigned char msj_recv[512];
    int len_cli=sizeof(cli);
    int lrecv=recvfrom(socket,msj_recv, 512,0,(struct sockaddr *)&cli,&len_cli); //512 por defecto
    if(lrecv==-1){
        perror("Error al recibir.");
        exit(0);
    }
    else{
        printf("\nMensaje de cliente:%s",msj_recv);
        fflush(stdout);
        //printf("\nPuerto Cliente:%d",htons(cli.sin_port));
    }
    return cli;
}


void enviar(int socket,unsigned char msj[],struct sockaddr_in cli){
    //printf("\n%.4x",cli.sin_family);
    //printf("\n%.4x",cli.sin_port);
    
    int tam=sendto(socket,msj,strlen(msj)+1,0,(struct sockaddr *)&cli,sizeof(cli));
    if(tam==-1){
        perror("Error en enviar.");
        exit(0);
    }
    else{
        //perror("Exito en enviar.");
    }
}