#include "../Libreria/miTFTP.h"

int Trabajando(Direccion dir_envio,Direccion  dir_recibe);

int main(){

    //se crea un socket
    int udp_socket = crearSocket(); 
    //se crean los argumentos para una sola estructura
    Direccion dir_envio=(Direccion)malloc(sizeof(struct datos_dir));
    Direccion dir_recibe=(Direccion)malloc(sizeof(struct datos_dir));
    
    struct sockaddr_in *local_cliente=crearLocal(0);
    struct sockaddr_in *remota=crearRemota(69,"192.168.1.150");
    
    int lbind=crearBind(udp_socket,local_cliente);
    
    dir_envio->udp_socket=udp_socket;
    dir_envio->dir=remota;

    dir_recibe->udp_socket=udp_socket;
    dir_recibe->dir=remota;

    Trabajando(dir_envio,dir_recibe);


    return 0;
}

int Trabajando(Direccion dir_envio,Direccion  dir_recibe){
    unsigned char name_file[50];
    int opcion=0;
    printf("Peticion de lectura: 1 \nPeticion de escritura:2\n");
    scanf("%d",&opcion);

    printf("Escriba el nombre del archivo: ");
    scanf("%s",name_file);
    
    printf("=> %s, tamaño:%d\n",name_file,strlen(name_file));

    printf("Se envio la peticion.\n");
    EnviarPeticion(opcion,name_file,dir_envio,dir_recibe);

}


