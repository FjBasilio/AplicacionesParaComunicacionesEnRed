#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

int crearSocket();
struct sockaddr_in crearLocal();
struct sockaddr_in crearRemota();
int crearBind(int socket,struct sockaddr_in dir);
void recibir(int socket,struct sockaddr_in dir);
void enviar(int socket,unsigned char msj[],struct sockaddr_in dir);


int main(){
    unsigned char msj[100]="una cadena desde cliente";
    
    int udp_socket = crearSocket();
    
    struct sockaddr_in local=crearLocal();
    struct sockaddr_in remota=crearRemota();

    int lbind=crearBind(udp_socket,local);
    // escribir mensaje a enviar
    enviar(udp_socket,msj,remota);

    recibir(udp_socket,remota);//capturar mensaje/respuesta del servidor
    
    
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

struct sockaddr_in crearLocal(){
    struct sockaddr_in local;
    local.sin_family=AF_INET; /* address family: AF_INET */
    local.sin_port=htons(0);   /* port in network byte order, se asigna con htons un puerto automatico */
    local.sin_addr.s_addr=INADDR_ANY;   /* internet address */
    return local;
}

struct sockaddr_in crearRemota(){
    struct sockaddr_in remota;
    remota.sin_family=AF_INET; /* address family: AF_INET */
    remota.sin_port=htons(8080);  /* es el puerto por defecto por donde se manda mensaje*/
    remota.sin_addr.s_addr=inet_addr("192.168.1.129");  /*ip del servidor*/
    return remota;
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
void enviar(int socket,unsigned char msj[],struct sockaddr_in dir){

    int tam=sendto(socket,msj,strlen(msj)+1,0,(struct sockaddr *)&dir,sizeof(dir));
    if(tam==-1){
        perror("Error en enviar.");
        exit(0);
    }
    else{
        perror("Exito en enviar.");
    }
}

void recibir(int socket,struct sockaddr_in dir){
    unsigned char msj_recv[512];
    int len_dir=sizeof(dir);
    int lrecv=recvfrom(socket,msj_recv, 512,0,(struct sockaddr *)&dir,&len_dir); //512 por defecto
    if(lrecv==-1){
        perror("Error al recibir.");
        exit(0);
    }
    else{
        printf("\nMensaje:%s",msj_recv);
    }
}