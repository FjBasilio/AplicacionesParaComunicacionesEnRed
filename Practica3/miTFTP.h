#include "miUDP.h"

//*****PROTOTIPOS******///
unsigned char * Struct_RRQ(unsigned char *name_file);
unsigned char * Struct_WRQ(unsigned char *name_file);
unsigned char * Struct_DATA(unsigned char * num_block,unsigned char *data);
unsigned char * Struct_ACK(unsigned char *num_block);
unsigned char * Struct_ERROR(unsigned char *num_block,unsigned char *error);

int ProcesarEnvioPeticion(int opcion,Direccion dir_envio,Direccion dir_recibe);
unsigned char* EsperandoPeticiones(Direccion direc);
int ProcesarPaqueteRecibido(unsigned char* paquete,Direccion dir_envio,Direccion  dir_recibe);

unsigned char* ObtenerNombreFile(unsigned char* paq);
unsigned char* int_to_unchar(int int_num);
int unchar_to_int(unsigned char* unchar_num);

int EnviaArchivo(unsigned char* name_file,Direccion dir_envio,Direccion  dir_recibe);
int RecibiendoArchivo(unsigned char* name_file,Direccion dir_envio,Direccion  dir_recibe);


//**************************INICIO DE LAS ESTRUCTURAS **************************//

//Esctructura de Solicitud de lectura
unsigned char * Struct_RRQ(unsigned char *name_file){
    unsigned char paq[516];
    for (int i = 0; i < 516; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    //unsigned char* code=htons(1);
    unsigned char code[2]={0x00,0x01};
    unsigned char mode[]="octet";
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,name_file,strlen((char*)name_file));
    ptr=ptr+strlen((char*)name_file);
    memcpy(paq+ptr,0x00,1);
    ptr=ptr+1;
    memcpy(paq+ptr,mode,strlen((char*)mode));
    ptr=ptr+strlen((char*)mode);
    memcpy(paq+ptr,0x00,1);
    ptr=ptr+1; //Tamanio final de la estructura

    unsigned char *pack=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(pack,paq,ptr);
    return pack;
}

//Estrcutura de Solicitud de escritura
unsigned char * Struct_WRQ(unsigned char *name_file){
    unsigned char paq[516];
    for (int i = 0; i < 516; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    unsigned char mode[]="octet";
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,name_file,strlen((char*)name_file));
    ptr=ptr+strlen((char*)name_file);
    memcpy(paq+ptr,0x00,1);
    ptr=ptr+1;
    memcpy(paq+ptr,mode,strlen((char*)mode));
    ptr=ptr+strlen((char*)mode);
    memcpy(paq+ptr,0x00,1);
    ptr=ptr+1;

    unsigned char *pack=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(pack,paq,ptr);
    return pack;
}
//Esctructura de Paquete de datos
unsigned char * Struct_DATA(unsigned char * num_block,unsigned char *data){
    unsigned char paq[516];
    for (int i = 0; i < 516; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,num_block,2);
    ptr=ptr+2;
    memcpy(paq+ptr,data,strlen((char*)data));
    ptr=ptr+strlen((char*)data);
    
    unsigned char *pack=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(pack,paq,ptr);
    return pack;
}
//Estructura de Paquete de confirmacion o Reconocimiento
unsigned char * Struct_ACK(unsigned char *num_block){
    unsigned char paq[516];
    for (int i = 0; i < 516; i++){
        paq[i]=0x00;
    }
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,num_block,2);
    ptr=ptr+2;

    unsigned char *pack=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(pack,paq,ptr);
    return pack;
}
//Estructura de Paquete de error
unsigned char * Struct_ERROR(unsigned char *num_block,unsigned char *error){
    unsigned char paq[516];
    for (int i = 0; i < 516; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,num_block,2);
    ptr=ptr+2;
    memcpy(paq+ptr,0x00,1);
    ptr=ptr+1;

    unsigned char *pack=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(pack,paq,ptr);
    return pack;
}
//**************************FIN DE LAS ESTRUCTURAS **************************//


//************************FUNCIONES PARA PETICIONES**************************//
//Esta funcion es para EL CLIENTE
int ProcesarEnvioPeticion(int opcion,Direccion dir_envio,Direccion dir_recibe){
    unsigned char name_file[200];
    unsigned char* paq;

    switch (opcion)
    {
    case 1:
        /* code peticion de lectura a DESTINATARIO */
        

        printf("Escriba el nombre del archivo:");
        scanf("%s",name_file);

        paq=Struct_RRQ(name_file);
        enviar(dir_envio,paq);

        //al enviar la peticion inmediatamente
        //se recibe una respuesta y se procesara

        RecibiendoArchivo(name_file,dir_envio,dir_recibe);
        

        break;

    case 2:
        /* peticion de escritura el DESTINATARIO */

        printf("Escriba el nombre del archivo:");
        scanf("%s",name_file);

        paq=Struct_WRQ(name_file);
        enviar(dir_envio,paq);
        
        //al enviar la peticion de escritura se debe esperar 
        //un ACK de confirmacion

        //si se recibe un ACK se procede con el envio de archivos
        EnviaArchivo(name_file,dir_envio,dir_recibe);

        break;
    
    default:
        break;
    }

}

//Esta funcion es para EL SERVIDOR
unsigned char* EsperandoPeticiones(Direccion direc){

    unsigned char* paquete;

    while(1){
        //regresara un paquete en memoria dinamica

        paquete=recibir(direc);

        return paquete;
    }
}
//Esta funcion es para EL SERVIDOR
int ProcesarPaqueteRecibido(unsigned char* paquete,Direccion dir_envio,Direccion  dir_recibe){
    //se extrae el codigo de operacion del paquete para saber la peticion
    unsigned char code[2];

    memcpy(code,paquete,2);
    //se hace una conversion a int para rpocesar la peticiones en el case
    int codigo=unchar_to_int(code);

    //se obtiene el nombre del archivo 
    //el nombre esta en memoria dinamica
    unsigned char* name_file=ObtenerNombreFile(paquete);

    unsigned char* ack;

    switch (codigo)
    {
    case 1:
        /* code solicitud de lectura*/

        //mientras no exista algun problema se le mandara
        //el archivo al solicitante
        EnviaArchivo(name_file,dir_envio,dir_recibe);


        break;
    case 2:
        /* code solicitud de escritura*/

        //se envia ACK de reconocimiento y de confirmacion
        ack=Struct_ACK(int_to_unchar(0));
        enviar(dir_envio,ack);


        //mientras no exista algun problema se le recibira 
        //el archivo del solicitante
        RecibiendoArchivo(name_file,dir_envio,dir_recibe);


        break;
    case 3:
        /* code paquete de datos*/
        break;
    case 4:
        /* code paquete de confirmacion*/
        break;
    case 5:
        /* code paquete de erroro*/
        break;
    
    default:
        break;
    }

}
//**********************FIN DE LAS FUNCIONES PARA PETICIONES***********************//

//***************************FUNCIONES AUXILIARES******************************//


unsigned char* ObtenerNombreFile(unsigned char* paq){
    unsigned char cero1,cero2;
    unsigned char caracter;
    unsigned char code[2];
    unsigned char mode[]="octet";
    unsigned char* name_file=(unsigned char*)malloc( sizeof(unsigned char) * ( strlen(paq)-2 ));

    int ptr=0;
    memcpy(code,paq+ptr,2);
    ptr=ptr+2;

    // se lee el nombre del archivo

    
    for (int i=0 ;paq[ptr]!= 0x00;i++){

        caracter==paq[ptr++];
        //print("%hhx",caracter);

        //captura del nombre del archivo
        name_file[i]=caracter;
        
    }
    

    //se lee el modo de transferencia

    for (int i=0 ;paq[ptr]!= 0x00;i++){

        caracter==paq[ptr++];
        //print("%hhx",caracter);
        
    }

    //tenemos el nombre del archivo
    //se retorna el nombre del archivo
    return name_file;

}

int AlmacenaArchivo(){

}
//convertira un int a una cadena unsigned char de 2 bytes 
unsigned char* int_to_unchar(int int_num){

    unsigned char* unchar_num=(unsigned char*)malloc(sizeof(unsigned char)*2);

    return unchar_num;
}
//convierte una cadena de unsigned char de 2 bytes a int
int unchar_to_int(unsigned char* unchar_num){
    int int_num;

    return int_num;
}
//***************************FIN DE FUNCIONES AUXILIARES******************************//


//***********************FUNCIONES DE ENVIO Y RECIBIMIENTO DE ARCHIVOS********************//
//*******************Puede ser usada tanto apara el SERVIDOR Y CLIENTE********************//
int EnviaArchivo(unsigned char* name_file,Direccion dir_envio,Direccion  dir_recibe){
    //indicara cuando el archivo se haya terminado de leer y a su ves enviado
    int terminacion=1;

    //empieza la comunicacion como indica el protocolo TFTP


    // DESTINATARIO                             REMITENTE
    //    |   (Se recibe la peticion) =>>            |  ya se recibio (servidor)
    //    |                                          |
    //    |                                          |
    //    |   <<=(Se envia DATA blocke 1)            | aqui empieza el while
    //    |                                          |
    //    |                                          |
    //    |    (Se recibe ACK 1) =>>                 |
    //    |                                          |
    //    |                                          |
    //    |   <<=(Se envia DATA blocke 2)            |
    //    |                                          |
    //    |                                          |
    //    |    (Se recibe ACK 2) =>>                 |
    //    |                                          |
    //    |                                          |
    //    |   <<=(Se envia DATA blocke 3)            |
    //    |                                          |
    //    |                                          |
    //    |    (Se recibe ACK 3) =>>                 |
    //    |                                          |
    //    |                                          |
    //    |   <<=(Se envia DATA blocke 4)            |
    //    |                                          |
    //    |                                          |
    //    |    (Se recibe ACK 4) =>>                 |
    //    |                                          |
    //    |                                          |
    //    |   <<=(Se envia DATA blocke n)            |
    //    |                                          |
    //    |                                          |
    //    |    (Se recibe ACK n) =>>                 |aqui se rompe el while
    //    |                                          |
    //    |                                          |

    //numero bloque
    int blocke=1;

    //paquete que se envia
    unsigned char paq_salida;

    //paquete que se recibe
    unsigned char* paq_entrada;

    //datos que se envian, se llenan con datos del archivo
    unsigned char* data;

    while (terminacion){
        //se arma la estructura DATA conforma leemos el archivo
        paq_salida=Struct_DATA(int_to_unchar(blocke),data);
        //se envia el DATA
        enviar(dir_envio,paq_salida);

        //se recibe el ACK correspondiente con el numero de bloque
        paq_entrada=recibir(dir_recibe);
    }
    
    

}

int RecibiendoArchivo(unsigned char* name_file,Direccion dir_envio,Direccion  dir_recibe){
    //indicara cuando el archivo se haya terminado de leer y a su ves enviado
    int terminacion=1;


    //empieza la comunicacion como indica el protocolo TFTP

    
    // DESTINATARIO                             REMITENTE
    //    |   (Se recibe la peticion) =>>            |  ya se recibio (servidor)
    //    |                                          |
    //    |                                          |
    //    |   <<=(Se envia ACK 0)                    | Ya se envio (servidor)
    //    |                                          |
    //    |                                          |
    //    |   (Se recibe DATA blocke 1)=>>           |aqui empieza el while
    //    |                                          |
    //    |                                          |
    //    |    <<=(Se envia ACK 1)                   |
    //    |                                          |
    //    |                                          |
    //    |   (Se recibe DATA blocke 2)=>>           |
    //    |                                          |
    //    |                                          |
    //    |    <<=(Se envia ACK 2)                   |
    //    |                                          |
    //    |                                          |
    //    |   (Se recibe DATA blocke 3)=>>           |
    //    |                                          |
    //    |                                          |
    //    |    <<=(Se envia ACK 3)                   |
    //    |                                          |
    //    |                                          |
    //    |   (Se recibe DATA blocke 4)=>>           |
    //    |                                          |
    //    |                                          |
    //    |    <<=(Se envia ACK 4)                   |
    //    |                                          |
    //    |                                          |
    //    |   (Se recibe DATA blocke n)=>>           |
    //    |                                          |
    //    |                                          |
    //    |    <<=(Se envia ACK n)                   |
    //    |                                          |
    //    |                                          |
    
    //numero bloque
    int blocke=1;

    //paquete que se envia
    unsigned char* paq_salida;

    //paquete que se recibe
    unsigned char* paq_entrada;

    while (terminacion){
        //se recibe un paquete tipo DATA
        paq_entrada=recibir(dir_recibe);
        
        //se procede a armar el archivo

        //Se arma el ACK respetivo al numero de bloque y se envia
        paq_salida=Struct_ACK(int_to_unchar(blocke));
        enviar(dir_envio,paq_salida);

        
    }

    
    
}

//*********************FIN DE FUNCIONES DE ENVIO Y RECIBIMIENTO DE ARCHIVOS******************//
