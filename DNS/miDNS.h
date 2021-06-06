
#include <stdlib.h>

///***************************FORMATO DE INDICADORES***********************

//[1] Sol_o_Rep        //0= solicitud, respuesta=1
//[3] Codigo_Op      //0x00=solicitud,0x01=solicitud inversa,0x02=solicitud estado del servidor
//[1] Resp_Autoridad   //1=si nos responde una autoridad
//[1] Truncado;        //en caso de que sea 1 de debera de usar tcp y no udp
//[1] Deseo_Recursion; //se manda en 1 para que investigue correctamente
                        //si es 0 nos regresara lista de los servidores en los cuales estara nuestra respuesta
//-----------------------------
//[1] Recursion_Disponibilidad; //si se regresa uno significa que la recursion fue aceptada y se hizo bien el trabajo
//[3] Reservado;        //
//---------------------------
//[4] Codigo_Retorno;   // 0= respuesta correcta, 3=error de nombres o el nombre no existe

///****************TOTAL 16 BITS*********************************************************

typedef struct encabezado_dns{
    unsigned char ID_Transaccion[2]={0x00,0x00};//el cliente decide el id
    unsigned char Indicadores[2]={0x00,0x00}; //definen si es solicitud o respuesta o autoridades
    unsigned char Cont_Peticiones[2]={0x00,0x00};//contador de peticiones mismo url
    unsigned char Cont_RR_Respuestas[2]={0x00,0x00}; //contador de respuestas
    unsigned char Cont_RR_Autoridad[2]={0x00,0x00};//contador de autoridades
    unsigned char Cont_RR_Adicionales[2]={0x00,0x00};//contadores adicionales
}*EncabezadoDNS;

typedef struct entrada_solicitud{
    unsigned char* Nombre_Peticion;//longitud variable
    unsigned char Tipo_Peticion[2]={0x00,0x00};;//longitu fija
    unsigned char Clase_Peticion[2]={0x00,0x00};;//longitud fija
}*EntradaSolicitud;



typedef struct consulta_dns{
    EncabezadoDNS encabezado_DNS; //longitud fija
    EntradaSolicitud entrada_solicitud; //longitud variable
    unsigned char* array_consulta_dns; //arreglo asociada a la estructura
}*ConsultaDNS;

typedef struct mi_dns{
    ConsultaDNS consultaDNS;
}*MiDNS;




MiDNS NuevoDNS(){

    MiDNS consulta=(MiDNS)malloc(sizeof(mi_dns));

    return consulta;
}

MiDNS creandoSolicitudDNS(unsigned char id,unsigned char nombre,unsigned char tipo,unsigned char clase){

    

    MiDNS dns=NuevoDNS();
    //el cliente elige el id
    dns->consultaDNS->encabezado_DNS->ID_Transaccion[1]=id;

    //se desea recursion se manda en 1
    dns->consultaDNS->encabezado_DNS->Indicadores[0]=0x01;

    //como es la primera peticion se pone como 1   
    dns->consultaDNS->encabezado_DNS->Cont_Peticiones[1]=0x01;

    //como es una solicitud lo demas ya esta inicializado

    //dns->consultaDNS->entrada_solicitud->Nombre_Peticion;
    dns->consultaDNS->entrada_solicitud->Tipo_Peticion[1]=tipo;
    dns->consultaDNS->entrada_solicitud->Clase_Peticion[1]=clase;


}

// Estructura de Recursos RR 

unsigned char* RR(){
    unsigned char* Nombre_RR
    unsigned char* Tipo_Registro
    unsigned char* Clase_Registro
    unsigned char* RR_TTL
    unsigned char* Tam_Datos_RR
    unsigned char* Datos_RR
}

// Estructura del la actualizacion de DNS
unsigned char* ActualizacionDNS(unsigned char* consulta_paq){
    unsigned char*
    unsigned char*
    unsigned char*
    unsigned char*


}
