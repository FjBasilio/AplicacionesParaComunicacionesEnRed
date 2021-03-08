#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

int crearSocket();
struct sockaddr_in crearLocal();
struct sockaddr_in crearRemota();
int crearBind(int socket,struct sockaddr_in dir);
void enviar(int socket,unsigned char msj[],int tam_msj,struct sockaddr_in remota);


int main(){
    unsigned char msj[100]="una cadena 123467890";
    
    int udp_socket = crearSocket();
    
    struct sockaddr_in local=crearLocal();
    struct sockaddr_in remota=crearRemota();

    int lbind=crearBind(udp_socket,local);

    enviar(udp_socket,msj,20,remota);

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
    remota.sin_port=htons(53);  /* es el puerto por defecto por donde se manda mensaje*/
    remota.sin_addr.s_addr=inet_addr("8.8.8.8");  /*ip del servidor*/
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
void enviar(int socket,unsigned char msj[],int tam_msj,struct sockaddr_in remota){

    int tam=sendto(socket,msj,tam_msj,0,(struct sockaddr *)&remota,sizeof(remota));
    if(tam==-1){
        perror("Error en enviar.");
        exit(0);
    }
    else{
        perror("Exito en enviar.");
    }
}