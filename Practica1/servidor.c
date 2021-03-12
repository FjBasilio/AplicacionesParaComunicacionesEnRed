#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

int crearSocket();
struct sockaddr_in crearServidor();
struct sockaddr_in crearCliente();
int crearBind(int socket,struct sockaddr_in dir);
struct sockaddr_in  recibir(int socket,struct sockaddr_in dir);
void enviar(int socket,unsigned char msj[],struct sockaddr_in dir);

//struct sockaddr_in cliente;

int main(){
    unsigned char msj[100]="una cadena desde servidor";
    
    int udp_socket = crearSocket();
    
    struct sockaddr_in servidor=crearServidor();
    struct sockaddr_in cliente=crearCliente();

    int lbind=crearBind(udp_socket,servidor);

    cliente=recibir(udp_socket,cliente); //capturando mensaje/respuesta del cliente
    // escribir mensaje/respuesta
    
    enviar(udp_socket,msj,cliente);

    //close(udp_socket);
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
        printf("\nMensaje:%s",msj_recv);
        printf("\nPuerto Cliente:%d",htons(cli.sin_port));
    }
    return cli;
}


void enviar(int socket,unsigned char msj[],struct sockaddr_in cli){
    printf("\n%.4x",cli.sin_family);
    printf("\n%.4x",cli.sin_port);
    //cliente.sin_family=AF_INET; /* address family: AF_INET */
    //cliente.sin_port=htons(53);  /* es el puerto por defecto por donde se manda mensaje*/
    //cliente.sin_addr.s_addr=inet_addr("8.8.8.8");

    int tam=sendto(socket,msj,strlen(msj)+1,0,(struct sockaddr *)&cli,sizeof(cli));
    if(tam==-1){
        perror("Error en enviar.");
        exit(0);
    }
    else{
        perror("Exito en enviar.");
    }
}