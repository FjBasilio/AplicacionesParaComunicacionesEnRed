#include "miUDP.h"

//*****PROTOTIPOS******///
int Struct_RRQ(unsigned char *name_file);
int Struct_WRQ(unsigned char *name_file);
int Struct_DATA(short int num_block,unsigned char *data);
int Struct_ACK(unsigned char *num_block);
int Struct_ERROR(unsigned char *num_block,unsigned char *error);
int EsperandoPeticiones(Direccion direc);

//**************************FIN DE LAS ESTRUCTURAS **************************//

//Esctructura de Solicitud de lectura
int Struct_RRQ(unsigned char *name_file){
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
    return;
}

//Estrcutura de Solicitud de escritura
int Struct_WRQ(unsigned char *name_file){
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
    return;
}
//Esctructura de Paquete de datos
int Struct_DATA(short int num_block,unsigned char *data){
    unsigned char paq[516];
    for (int i = 0; i < 516; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,(unsigned char*)num_block,2);
    ptr=ptr+2;
    memcpy(paq+ptr,data,strlen((char*)data));
    ptr=ptr+strlen((char*)data);
    
    unsigned char *pack=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(pack,paq,ptr);
    return;
}
//Estructura de Paquete de confirmacion o Reconocimiento
int Struct_ACK(unsigned char *num_block){
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
    return;
}
//Estructura de Paquete de error
int Struct_ERROR(unsigned char *num_block,unsigned char *error){
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
    return;
}
//**************************FIN DE LAS ESTRUCTURAS **************************//

//************************FUNCIONES PARA PETICIONES**************************//
int EsperandoPeticiones(Direccion direc){
    while(1){
        recibir(direc);
        
        ProcesarPaqueteRecibido(paquete);
    }
}

int ProcesarPaqueteRecibido(unsigned char* paquete){
    //se extrae el codigo de operacion del paquete para saber la peticion
    unsigned char code[2];

    memcpy(code,paquete,2);
    //se hace una conversion a int para rpocesar la peticiones en el case
    int codigo=htons(code);

    switch (codigo)
    {
    case 1:
        /* code solicitud de lectura*/
        SolicitudLectura(paquete);
        break;
    case 2:
        /* code solicitud de escritura*/
        SolicitudEscritura(paquete);
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
//***************************FUNCIONES DE SOLICITUDES******************************//

int SolicitudLectura(unsigned char* paq){
    unsigned char cero1,cero2;
    unsigned char caracter;
    unsigned char code[2];
    unsigned char mode[]="octet";
    unsigned char* name_file=(unsigned char)malloc(sizeof(unsigned char)*214);

    int ptr=0;
    memcpy(code,paq+ptr,2);
    ptr=ptr+2;

    // se lee el nombre del archivo
    do{
        caracter==paq[ptr];
        
    }while (caracter == 0x00);
    

    //se lee el modo de transferencia

    while (/* condition */){
        /* code */
    }
    

    

}

int SolicitudEscritura(unsigned char* paq){

}

int EnviaArchivo(){

}

int RecibiendoArchivo(){
    
}

int AlmacenaArchivo(){

}