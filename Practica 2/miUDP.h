#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

//Estructura que se mandara al los hilos, ya que solo aceptan un parametro
typedef struct datos_dir{
        int udp_socket; //socket asociado a la direccion
        struct sockaddr_in *dir; //direccion, esta sera dinamica
    }*Direccion;


//Regresa un descriptor de socket al mandar a llamar a la funcion
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
//Esta funcion es usada por EL CLIENTE y SERVIDOR para definir sus propias direcciones locales
//Recibe un numero entero que indicara el puerto 
struct sockaddr_in *crearLocal(int puerto){
    struct sockaddr_in *servidor=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    servidor->sin_family=AF_INET; /* address family: AF_INET */
    servidor->sin_port=htons(puerto);   /* port in network byte order*/
    servidor->sin_addr.s_addr=INADDR_ANY;   /* internet address */
    return servidor;
}
//Esta funcion es usada por EL CLIENTE
//Recibe el puerto y la direccion ip el formato de cadena
struct sockaddr_in *crearRemota(int puerto,char ips_serv[]){
    struct sockaddr_in *remota=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    remota->sin_family=AF_INET; /* address family: AF_INET */
    remota->sin_port=htons(puerto);  /* es el puerto por defecto por donde se manda mensaje*/
    remota->sin_addr.s_addr=inet_addr(ips_serv);  /*ip del servidor*/
    return remota;
}
//Esta funcion es usada por EL SERVIDOR
//regresa una estructura vacia la cual sera llenado por una funcion al momento de recibir un paquete
struct sockaddr_in *crearCliente(){
    struct sockaddr_in *cliente=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    // cliente.sin_family=AF_INET; /* address family: AF_INET */
    // cliente.sin_port=htons(53);  /* es el puerto por defecto por donde se manda mensaje*/
    // cliente.sin_addr.s_addr=inet_addr("8.8.8.8");  /*ip del servidor*/
    return cliente; //no se llena, solo se declara la estructura
}

//asocia el socket enviado y la estructura de la direccion local
int crearBind(int socket,struct sockaddr_in *dir){

    int lbind=bind(socket,(struct sockaddr*)dir,sizeof(*dir)); //se necesita el tamanio donde apunto
    if(lbind==-1){
        perror("Error en bind.");
        exit(0);
    }
    else{
        perror("Exito en bind.");
    }
    return lbind;
}
// Esta funcion se dedica a recibir paquetes
// Esta asociado a la Direccion que contiene:
// el socked y la direccion ip del origen, 
// regresa 0 si tuvo exito
// si hay un error regresa -1
int recibir(Direccion direc){
    unsigned char msj_recv[512];  //512 por defecto
    struct sockaddr_in dir_temp=*(direc->dir);
    int socket=direc->udp_socket;
    int len_dir=sizeof(dir_temp);

    int lrecv=recvfrom(socket,msj_recv, 512,0,(struct sockaddr *)&dir_temp,&len_dir);
    if(lrecv==-1){
        printf("\nError al recibir.\n");
        return -1;
    }
    else{
        printf("\n--->:%s",msj_recv);
        *(direc->dir)=dir_temp; // esto es necesario para el servidor, debe de obtener los tatos del remitente
        return 0;
    }
    
}

// Esta funcion se dedicara a enviar paquetes 
// asociando a la Direccion que contiene:
// socket, el mensaje y la direccion ip a la que se envia
// regresa 0 si tuvo exito
// si hay un error regresa -1
int enviar(Direccion direc,unsigned char msj[]){

    struct sockaddr_in dir_temp=*(direc->dir);
    int socket=direc->udp_socket;

    //pruebas
    //printf("\npuerto salida:%d",ntohs(dir_temp.sin_port));
    //printf("\nip:%s",inet_ntoa(dir_temp.sin_addr));


    int tam=sendto(socket,msj,strlen(msj)+1,0,(struct sockaddr *)&dir_temp,sizeof(dir_temp));
    if(tam==-1){
        printf("\nError en enviar.\n");
        return -1;
    }
    else{
        //perror("Exito en enviar.");
        return 0;
    }
}