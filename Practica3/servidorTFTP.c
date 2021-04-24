#include "miTFTP.h"

int main(){

    int udp_socket = crearSocket();
    //se crean los argumentos para una estructura
    Direccion dir_envio=(Direccion)malloc(sizeof(struct datos_dir));
    Direccion dir_recibe=(Direccion)malloc(sizeof(struct datos_dir));

    struct sockaddr_in *local_servidor=crearLocal(8080);
    struct sockaddr_in *cliente=crearCliente(); 

    int lbind=crearBind(udp_socket,local_servidor);

    dir_envio->udp_socket=udp_socket;
    dir_envio->dir=cliente;

    dir_recibe->udp_socket=udp_socket;
    dir_recibe->dir=cliente;

    return 0;
}

