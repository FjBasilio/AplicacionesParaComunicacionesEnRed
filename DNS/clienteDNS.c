#include "./Libreria/miDNS.h"

void Trabajando(Direccion dir_envio,Direccion dir_recibe);;

int main(){

    //se crea un socket
    int udp_socket = crearSocket(); 
    //se crean los argumentos para cada hilo 
    Direccion dir_envio=(Direccion)malloc(sizeof(struct datos_dir));
    Direccion dir_recibe=(Direccion)malloc(sizeof(struct datos_dir));
    
    struct sockaddr_in *local_cliente=crearLocal(0);
    struct sockaddr_in *remota=crearRemota(53,"200.56.224.11");//148.204.103.2
    
    int lbind=crearBind(udp_socket,local_cliente);
    
    dir_envio->udp_socket=udp_socket;
    dir_envio->dir=remota;

    dir_recibe->udp_socket=udp_socket;
    dir_recibe->dir=remota;
    Trabajando(dir_envio,dir_recibe);

    return 0;
}

void Trabajando(Direccion dir_envio,Direccion dir_recibe){

    unsigned char nombre_dominio[50];
    printf("\nEscriba un nombre de dominio: ");
    scanf("%s",nombre_dominio);

    enviarSolicitud(nombre_dominio,dir_envio,dir_recibe);

    printf("Terminado\n");
}