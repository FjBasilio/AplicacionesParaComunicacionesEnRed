#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "miUDP.h"

//declaracion de las funciones
void* Hilo_Envio(void* void_direc);
void* Hilo_Recibe(void* void_direc);

int main(){
    //se declaran las variables de los hilos 
    pthread_t hilo_envio,hilo_recibe;
    //se crea un socket
    int udp_socket = crearSocket(); 
    //se crean los argumentos para cada hilo 
    Direccion dir_envio=(Direccion)malloc(sizeof(struct datos_dir));
    Direccion dir_recibe=(Direccion)malloc(sizeof(struct datos_dir));
    
    struct sockaddr_in *local_cliente=crearLocal(0);
    struct sockaddr_in *remota=crearRemota(69,"192.168.1.150");
    
    int lbind=crearBind(udp_socket,local_cliente);
    
    dir_envio->udp_socket=udp_socket;
    dir_envio->dir=remota;

    dir_recibe->udp_socket=udp_socket;
    dir_recibe->dir=remota;
    //se crea el hilo de envio de paquetes
    if(pthread_create(&hilo_envio,NULL,&Hilo_Envio,(void*)dir_envio)!=0){
        perror("\nError al crear el hilo\n");
    }    
    //se crea el hilo que recibe paquetes
    if(pthread_create(&hilo_recibe,NULL,&Hilo_Recibe,(void*)dir_recibe)!=0){
        perror("\nError al crear el hilo\n");
    }
    int b=pthread_join(hilo_recibe,NULL); //si se termina antes el ilo de espera se cierra el programa
    //int a=pthread_join(hilo_envio,NULL);
    //se cierra el socket
    close(udp_socket);
    return 0;
}
//funcion que ejecutara el hilo de envio de paquetes
void* Hilo_Envio(void* void_direc){
    Direccion direc=(Direccion)void_direc;
    unsigned char msj[512];

    int ban=0;
    while(ban >= 0){
        
        
        printf("\nTus:");
        fgets(msj,512,stdin);
        fflush(stdin);
        ban=enviar(direc,msj);
        
    }
    
}
//funcion que ejecutara el hilo que recibe paquetes
void* Hilo_Recibe(void* void_direc){
    Direccion direc=(Direccion)void_direc;
    
    unsigned char* mensaje="0";
    while( strcmp(mensaje,"-1") != 0){

        fflush(stdout);
        mensaje=recibir(direc);
        fprintf(stdout,"\n--->:%s",mensaje);   
        printf("Tu:"); 
        
    }
}