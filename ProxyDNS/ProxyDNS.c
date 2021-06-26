#include <stdio.h>
#include "./Libreria/miDNS.h"

int ServidorProxyDNS();

DNS ClienteProxyDNS(DNS dns_sol);


int main(){

    ServidorProxyDNS();

}


int ServidorProxyDNS(){

    int udp_socket = crearSocket();
    //se crean los argumentos para una estructura
    Direccion dir_envio=(Direccion)malloc(sizeof(struct datos_dir));
    Direccion dir_recibe=(Direccion)malloc(sizeof(struct datos_dir));

    struct sockaddr_in *local_servidor=crearLocal(53);
    struct sockaddr_in *cliente=crearCliente(); 

    int lbind=crearBind(udp_socket,local_servidor);

    dir_envio->udp_socket=udp_socket;
    dir_envio->dir=cliente;

    dir_recibe->udp_socket=udp_socket;
    dir_recibe->dir=cliente;

    DNS dns_sol,dns_res;
    while (1){   
        //socket donde se recibiran las solicitudes
        dns_sol=EsperandoSolicitudes(dir_recibe);
        printf("\nSolicitud por Proxy");
        dns_res=ClienteProxyDNS(dns_sol);
        if (enviar(dir_envio,dns_sol)==-1){
            perror("\nError al enviar actualizacion al cliente\n");
            return -1;
        }else{
            printf("\nSe envio actualizacion al cliente");
        }
    }
    
}


///Este cliente realizara la consulta por el usuario
//tambien filtrara cuales seran los nombres de dominios validor
//regresara una respuesta ya sea alterada o valida 
//para que el servidor se lo mande al cliente
DNS ClienteProxyDNS(DNS dns_sol){
    //***************CREANDO AL CLIENTE******************
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
    //****************************************************

    //************nombres de dominio que estan prohibidos****************
    unsigned char* Lista_Negra[100];

    Lista_Negra[0]="www.infonavit.com.mx";
    Lista_Negra[1]="www.instagram.com.mx";
    Lista_Negra[2]="ipn";
    Lista_Negra[3]="unam";
    Lista_Negra[4]="facebook";
    //definiremos la ip del servidor de sustitucion 
    unsigned char IP_sust[]={0xff,0xff,0xff,0xff};
    //********************************************************************

    //obtendremos el nombre de dominio que solicito el usuario
    //y tambn mostrara la solicitud DNS en consola
    unsigned char* nombre_dns_usuario=UsuarioSolicitudDNS(dns_sol);
    
    //aun asi realizamos la solicitud DNS tomando el rol del usuario
    //y obtendremos la IP del sominoop que solicito el usuario
    DNS dns_res=EnviarSolicitud(nombre_dns_usuario,dir_envio,dir_recibe);

    //se valida si el nombre de dominio solicitado por el susuario esta en la lista negra
    for (int i = 0; i < strlen(*Lista_Negra); i++){

        //en caso de encontrar el nombre de dominio en la lista negra
        if(1){
            printf("\nNombre de dominio solicitado:%s no valido",nombre_dns_usuario);
            CrearActualizacionDNS(IP_sust,dns_sol);
        }
    }
    //en caso de no encontrarlo en la lista negra se deja pasar la actualizacion DNS
    return dns_res;
    
}