#include "miUDP.h"

//Esctructura de Solicitud de lectura
int Struct_RRQ(unsigned char *paq,unsigned char *name_file){
    for (int i = 0; i < 512; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
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
    ptr=ptr+1;

    return;
}

//Estrcutura de Solicitud de escritura
int Struct_WRQ(unsigned char *paq,unsigned char *name_file){
    for (int i = 0; i < 512; i++){
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
    return;
}
//Esctructura de Paquete de datos
int Struct_DATA(unsigned char *paq,int num_block,unsigned char *data){
    for (int i = 0; i < 512; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    unsigned char block[2]={0x00,0x00};
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,block,2);
    ptr=ptr+2;
    memcpy(paq+ptr,data,strlen((char*)data));
    ptr=ptr+strlen((char*)data);
    
    return;
}
//Estructura de Paquete de confirmacion
int Struct_ACK(unsigned char *paq,int num_block){
    for (int i = 0; i < 512; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    unsigned char block[2]={0x00,0x00};
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,block,2);
    ptr=ptr+2;
    return;
}
//Estructura de Paquete de error
int Struct_ERROR(unsigned char *paq,int num_block,unsigned char *error){
    for (int i = 0; i < 512; i++){
        paq[i]=0x00;
    }
    
    //unsigned char= 8 bits = 1 byte
    unsigned char code[2]={0x00,0x02};
    unsigned char block[2]={0x00,0x00};
    int ptr=0;
    memcpy(paq+ptr,code,2);
    ptr=ptr+2;
    memcpy(paq+ptr,block,2);
    ptr=ptr+2;
    memcpy(paq+ptr,0x00,1);
    ptr=ptr+1;
    return;
}
