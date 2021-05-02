#include "miUDP.h"

//*****PROTOTIPOS******///
unsigned char * Struct_RRQ(unsigned char *name_file);
unsigned char * Struct_WRQ(unsigned char *name_file);
unsigned char * Struct_DATA(unsigned char * num_block,unsigned char *data);
unsigned char * Struct_ACK(unsigned char *num_block);
unsigned char * Struct_ERROR(unsigned char *num_block,unsigned char *error);

int EnviarPeticion(int opcion,unsigned char* name_file,Direccion dir_envio,Direccion dir_recibe);
unsigned char* EsperandoPeticiones(Direccion direc);
int ProcesarPaqueteRecibido(unsigned char* paquete,Direccion dir_envio,Direccion  dir_recibe);

unsigned char* ObtenerNombreFile(unsigned char* paq);
unsigned char* ObtenerDatos(unsigned char* paq);
unsigned char* ObtenerCodigoOp(unsigned char* paq);
unsigned char* int_to_unchar(int int_num);
int unchar_to_int(unsigned char* unchar_num);

unsigned char* EnviaArchivo(FILE* file,Direccion dir_envio,Direccion  dir_recibe);
unsigned char* RecibiendoArchivo(FILE* file,Direccion dir_envio,Direccion  dir_recibe);


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
int EnviarPeticion(int opcion,unsigned char* name_file,Direccion dir_envio,Direccion dir_recibe){
    unsigned char* paq;
    FILE* file;

    switch (opcion)
    {
    case 1:
        /* code peticion de lectura a DESTINATARIO (cliente recibe archivo) */

        paq=Struct_RRQ(name_file);
        enviar(dir_envio,paq);

        //al enviar la peticion inmediatamente el servidor
        //nos responde con el primer blocke

        //se procede a abrir el archivo en modo escritura
        file=fopen(name_file,"w");

        if(file==NULL){
            //en caso de existir un error de hace saber  
            printf("Error al abrir archivo de envio.\n");
            //se limpia memoria
            free(paq);
            free(file);
            return -1;
        }
        //si no hay error se reciben los datos
        RecibiendoArchivo(file,dir_envio,dir_recibe);
        

        break;

    case 2:
        /* peticion de escritura el DESTINATARIO (cliente manda archivo)*/

        paq=Struct_WRQ(name_file);
        enviar(dir_envio,paq);
        
        //al enviar la peticion de escritura se debe esperar 
        //un ACK de confirmacion

        //si se recibe un ACK se procede con el envio del archivo
        //se procede a abrir el archivo en modo lectura
        file=fopen(name_file,"r");

        if(file==NULL){
            //en caso de existir un error de hace saber  
            printf("Error al abrir archivo de envio.\n");
            //se limpia memoria
            free(paq);
            free(file);
            return -1;
        }
        //si no hay error se envia
        EnviaArchivo(file,dir_envio,dir_recibe);

        break;
    
    default:
        return -1;
    }
    //se limpia memoria
    free(paq);
    free(file);

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
    
    //se hace una conversion a int para procesar la peticiones en el case
    int codigo=unchar_to_int(ObtenerCodigoOp(paquete));

    //se obtiene el nombre del archivo 
    unsigned char* name_file=ObtenerNombreFile(paquete);

    FILE* file;
    unsigned char* ack;

    //se limpia memoria
    free(paquete);
    free(name_file);
    free(file);
    free(ack);

    switch (codigo)
    {
    case 1:
        /* code solicitud de lectura (sevidor envia archivo)*/ 

        //mientras no exista algun problema se le mandara
        //el archivo al solicitante

        //en el caso de no existir el archivo se envia un 
        //paquete de ERROR

        //se procede a abrir el archivo en modo lectura
        file=fopen(name_file,"r");

        if(file==NULL){
            //en caso de existir un error de hace saber  
            printf("Error al abrir archivo de envio.\n");
            //se limpia memoria
            free(paquete);
            free(name_file);
            free(file);
            free(ack);
            return -1;
        }
        //si no hay error se envia
        EnviaArchivo(file,dir_envio,dir_recibe);


        break;
    case 2:
        /* code solicitud de escritura (servidor recibe archivo)*/

        //se envia ACK de reconocimiento y de confirmacion
        ack=Struct_ACK(int_to_unchar(0));
        enviar(dir_envio,ack);


        //mientras no exista algun problema se le recibira 
        //el archivo del solicitante

        //se procede a abrir el archivo en modo escritura
        file=fopen(name_file,"r");

        if(file==NULL){
            //en caso de existir un error de hace saber  
            printf("Error al abrir archivo de envio.\n");
            //se limpia memoria
            free(paquete);
            free(name_file);
            free(file);
            free(ack);
            return -1;
        }
        //si no hay error se reciben los datos
        RecibiendoArchivo(file,dir_envio,dir_recibe);

        break;
    
    default:
        return -1;
    }

    //se limpia memoria
    free(paquete);
    free(name_file);
    free(file);
    free(ack);

}
//**********************FIN DE LAS FUNCIONES PARA PETICIONES***********************//

//***************************FUNCIONES AUXILIARES******************************//

//se dedicara a obtener los nombres de las peticiones recibidas
unsigned char* ObtenerNombreFile(unsigned char* paq){
    
    unsigned char aux[512];

    // se lee el nombre del archivo
    for(int i = 0;paq[1]==0x00;i++){
        aux[i]=paq[i+2];
    }

    unsigned char* name_file=(unsigned char*)malloc( sizeof(unsigned char) * strlen(aux));
    
    //se retorna el nombre del archivo
    return strcpy(name_file,aux);

}
//Extrae los datos de un paquete
unsigned char* ObtenerDatos(unsigned char* paq){

    unsigned char aux[512];
    int tam_paq=strlen(paq);

    // se lee los datos, empieza a partir de 4 bytes
    for(int i = 4;i==tam_paq;i++){
        aux[i-4]=paq[i];
    }

    unsigned char* datos=(unsigned char*)malloc(sizeof(unsigned char)*512);

    //se retornan los datos
    return strcpy(datos,aux);
}

unsigned char* ObtenerCodigoOp(unsigned char* paq){

    unsigned char* code=(unsigned char*)malloc(sizeof(unsigned char)*2);
    memcpy(code,paq,2);
    return code;
}


//convertira un int a una cadena unsigned char de 2 bytes 
unsigned char* int_to_unchar(int int_num){
    
    unsigned char* unchar_num=(unsigned char*)malloc(sizeof(unsigned char)*2);

    unchar_num[0]=int_num>>8;
    unchar_num[1]=(unsigned char)int_num;

    return unchar_num;
}
//convierte una cadena de unsigned char de 2 bytes a int
int unchar_to_int(unsigned char* unchar_num){
    int n,n1,n2;

    n1=unchar_num[0]<<8;
    n2=(int)unchar_num[1];
    printf("n1=> %d ,n2=> %d\n\n",n1,n2);
    n=n1+n2;

    return n;
}
//***************************FIN DE FUNCIONES AUXILIARES******************************//


//***********************FUNCIONES DE ENVIO Y RECIBIMIENTO DE ARCHIVOS********************//
//*******************Puede ser usada tanto apara el SERVIDOR Y CLIENTE********************//
unsigned char* EnviaArchivo(FILE* file,Direccion dir_envio,Direccion  dir_recibe){
    
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
    //indicara cuando el archivo se haya terminado de leer y a su ves enviado
    int terminacion=1;

    //numero bloque
    int bloque=1;

    //paquete que se envia
    unsigned char* paq_salida;

    //paquete que se recibe
    unsigned char* paq_entrada;

    //datos que se envian, se llenan con datos del archivo
    unsigned char* data;

    //almacenamiento del caracter temporal
    unsigned char c;

    while (terminacion){
        data=(unsigned char*)malloc(sizeof(unsigned char)*512);

        //La escritura de hace despues de los 4 bytes que ya se ocuparon hasta 512
        for(int i=4 ;(feof(file) == 0) && (i<512); i++){ //mientras que no se detecte el fin de archivo

            c=fgetc(file); //se toma los caracteres en orden
            data[i]=c;
            //printf("=>%c-%x-%d\n",c,c,c);
            
        }

        //se arma la estructura DATA despues de leer el archivo
        paq_salida=Struct_DATA(int_to_unchar(bloque++),data);
        
        //se envia el DATA
        enviar(dir_envio,paq_salida);

        //se recibe el ACK correspondiente con el numero de bloque
        paq_entrada=recibir(dir_recibe);

        //se limpian los apuntadores para evitar traslape de datos
        free(paq_salida);
        free(paq_entrada);
        free(data);
    }
    
    

}

unsigned char* RecibiendoArchivo(FILE* file,Direccion dir_envio,Direccion  dir_recibe){
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
    int bloque=1;

    //paquete que se envia
    unsigned char* paq_salida;

    //datos extraidos del paquete recibido
    unsigned char* data;

    //paquete que se recibe
    unsigned char* paq_entrada;

    while (terminacion){
        //se recibe un paquete tipo DATA
        paq_entrada=recibir(dir_recibe);
        //se extraen los datos
        data=ObtenerDatos(paq_entrada);
        fputs(data,file);

        //Se arma el ACK respetivo al numero de bloque y se envia
        paq_salida=Struct_ACK(int_to_unchar(bloque++));
        enviar(dir_envio,paq_salida);

        free(paq_salida);
        free(paq_entrada);
        free(data);
    }

    
    
}

//*********************FIN DE FUNCIONES DE ENVIO Y RECIBIMIENTO DE ARCHIVOS******************//
