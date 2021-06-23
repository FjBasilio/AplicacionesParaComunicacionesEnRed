
#include "miUDP_DNS.h"

///***************************FORMATO DE INDICADORES***********************

//[1] Sol_o_Rep        //0= solicitud, respuesta=1
//[3] Codigo_Op        //0x00=solicitud,0x01=solicitud inversa,0x02=solicitud estado del servidor
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

///prototipos
int enviarSolicitud(unsigned char* nombre_dominio,Direccion dir_envio,Direccion dir_recibe);

DNS solicitudDNS(unsigned char id,unsigned char* nombre,unsigned char tipo,unsigned char clase);
int actualizacionDNS(DNS dns);

unsigned char* formatoNombreDNS(unsigned char* nombre);
unsigned char* formatoDNSNombre(unsigned char* nombre_dns);

void imprimeTrama(unsigned char* trama,int tam);
unsigned char* int_to_unchar(int int_num);
int unchar_to_int(unsigned char* unchar_num);

//Envia una solicitud y espera a recibir una respuesta
int enviarSolicitud(unsigned char* nombre_dominio,Direccion dir_envio,Direccion dir_recibe){

    DNS dns_sol=solicitudDNS(0x10,nombre_dominio,0x01,0x01);
    printf("\nEnviando solicitud\n");
    if (enviar(dir_envio,dns_sol)==-1){
            perror("Error al enviar solicitud");
            return -1;
        }else{
            printf("\nSe envio solicitud");
        }

    DNS dns_res=recibir(dir_recibe);
    if (dns_res==NULL){
            perror("Error al recibir respuesta");
            return -1;
        }else{
            printf("\nSe recibio respuesta");
        }

    actualizacionDNS(dns_res);

    return 0;
}



//regresa una solicitud DNS, ingresando un id, un numbre de dominio , el tipo y su clase
DNS solicitudDNS(unsigned char id,unsigned char* nombre,unsigned char tipo,unsigned char clase){

    //ENCABEZADO DNS
    unsigned char ID_Transaccion[]={0x00,0x00};//el cliente decide el id
    unsigned char Indicadores[]={0x00,0x00}; //definen si es solicitud o respuesta o autoridades
    unsigned char Cont_Peticiones[]={0x00,0x00};//contador de peticiones mismo url
    unsigned char Cont_RR_Respuestas[]={0x00,0x00}; //contador de respuestas
    unsigned char Cont_RR_Autoridad[]={0x00,0x00};//contador de autoridades
    unsigned char Cont_RR_Adicionales[]={0x00,0x00};//contadores adicionales

    //ENTRADA DE SOLICITUD
    unsigned char* Nombre_Peticion;//longitud variable
    unsigned char Tipo_Peticion[]={0x00,0x00};//longitu fija
    unsigned char Clase_Peticion[]={0x00,0x00};//longitud fija

    DNS dns=NuevoDNS();

    //MODIFICACIONES AL ENCABEZADO DNS
    //el cliente elige el id
    ID_Transaccion[1]=id;
    //se desea recursion se manda en 1
    Indicadores[0]=0x01;
    //como es la primera peticion se pone como 1   
    Cont_Peticiones[1]=0x01;

    //como es una solicitud lo demas ya esta inicializado

    unsigned char cero[]={0x00};

    unsigned char temp[512];
    for (int i = 0; i < 512; i++){temp[i]=0x00;}

    int ptr=0;

    //LLENANDO LA ENTRADA DE SOLICITUD
    Nombre_Peticion=formatoNombreDNS(nombre);
    Tipo_Peticion[1]=tipo;
    Clase_Peticion[1]=clase;

    //llenando el arreglo DNS

    memcpy(temp+ptr,ID_Transaccion,2);
    ptr+=2;
    memcpy(temp+ptr,Indicadores,2);
    ptr+=2;
    memcpy(temp+ptr,Cont_Peticiones,2);
    ptr+=2;
    memcpy(temp+ptr,Cont_RR_Respuestas,2);
    ptr+=2;
    memcpy(temp+ptr,Cont_RR_Autoridad,2);
    ptr+=2;
    memcpy(temp+ptr,Cont_RR_Adicionales,2);
    ptr+=2;

    memcpy(temp+ptr, Nombre_Peticion,strlen(Nombre_Peticion));
    ptr+=strlen(Nombre_Peticion);
    memcpy(temp+ptr,cero,1);//para identificar el fin del nombre de dominio
    ptr=ptr+1;
    memcpy(temp+ptr,Tipo_Peticion,2);
    ptr+=2;
    memcpy(temp+ptr,Clase_Peticion,2);
    ptr+=2;

    unsigned char* temp_dns=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(temp_dns,temp,ptr);
    dns->array_dns=temp_dns;
    dns->longitud_array=ptr;

    //ENCABEZADO DNS
    printf("\n****************Solicitud DNS****************\n");
    printf("\n----------------Encabezado DNS----------------\n");
    printf("\n\tId de transaccion:%.2hx %.2hx", ID_Transaccion[0],ID_Transaccion[1]);
    printf("\n\tIndicadores:%d %d",Indicadores[0],Indicadores[1]);
    printf("\n\tContador RR de peticiones:%d ",unchar_to_int(Cont_Peticiones));
    printf("\n\tContador RR de respuestas:%d",unchar_to_int(Cont_RR_Respuestas));
    printf("\n\tContador RR de autoridad:%d ",unchar_to_int(Cont_RR_Autoridad));
    printf("\n\tContador RR de adicionales:%d ",unchar_to_int(Cont_RR_Adicionales));

    //ENTRADA DE SOLICITUD
    printf("\n--------------Entrada de solicitud-------------\n");
    printf("\n\tNombre de peticion:%s ",formatoDNSNombre(Nombre_Peticion));
    printf("\n\tTipo de peticion:%d ",unchar_to_int(Tipo_Peticion));
    printf("\n\tClase de peticion:%d ",unchar_to_int(Clase_Peticion));
    printf("\n\n**************Fin Solicitud DNS**************\n");


    imprimeTrama(dns->array_dns,dns->longitud_array);
    return dns;
}
//cuando se recive una respuesta esta funcion se encarga de ilustrar los datos
int actualizacionDNS(DNS dns){
    //ENCABEZADO DNS
    unsigned char ID_Transaccion[]={0x00,0x00};//el cliente decide el id
    unsigned char Indicadores[]={0x00,0x00}; //definen si es solicitud o respuesta o autoridades //ESTRUCTURA DEFINIDA EN BITS
    unsigned char Cont_Peticiones[]={0x00,0x00};//contador de peticiones mismo url
    unsigned char Cont_RR_Respuestas[]={0x00,0x00}; //contador de respuestas
    unsigned char Cont_RR_Autoridad[]={0x00,0x00};//contador de autoridades
    unsigned char Cont_RR_Adicionales[]={0x00,0x00};//contadores adicionales

    //ENTRADA DE SOLICITUD
    unsigned char Nombre_Peticion[512]; // longitud variable
    for (int i = 0; i < 512; i++){Nombre_Peticion[i]=0x00;}
    unsigned char Tipo_Peticion[]={0x00,0x00};//longitu fija // 0x01=registro host,0x02=registro servidor nombres,0x05 registro alis,0x0C=registro solicutud invers
    unsigned char Clase_Peticion[]={0x00,0x00};//longitud fija

    // Estructura de Recursos RR 
    unsigned char Nombre_RR[512]; // longitud variable, nombre del dominio que solicitamos
    for (int i = 0; i < 512; i++){Nombre_RR[i]=0x00;}
    unsigned char Tipo_Registro[]={0x00,0x00}; //logitud fija
    unsigned char Clase_Registro[]={0x00,0x00}; //logitud fija
    unsigned char RR_TTL[]={0x00,0x00,0x00,0x00}; //logitud fija, tiempo de vida en seg
    unsigned char Tam_Datos_RR[]={0x00,0x00}; //logitud fija, tamaño de la respuesta
    unsigned char Datos_RR[50]; //logitud variable IP del dominio solicitado
    for (int i = 0; i < 50; i++){Datos_RR[i]=0x00;}

    unsigned char cero[]={0x00};
    unsigned char* temp=dns->array_dns;
    int ptr=0;

    memcpy(ID_Transaccion,temp+ptr,2);
    ptr+=2;
    memcpy(Indicadores,temp+ptr,2);
    ptr+=2;
    memcpy(Cont_Peticiones,temp+ptr,2);
    ptr+=2;
    memcpy(Cont_RR_Respuestas,temp+ptr,2);
    ptr+=2;
    memcpy(Cont_RR_Autoridad,temp+ptr,2);
    ptr+=2;
    memcpy(Cont_RR_Adicionales,temp+ptr,2);
    ptr+=2;

    printf("\n ptr=%d",ptr);
    int i=0;
    //para leer el nombre DNS (hay un descuadre)
    while(temp[ptr]!=0x00){
        
        Nombre_Peticion[i]=temp[ptr];
        printf("\n%d=>%c  ptr=%d",i,Nombre_Peticion[i],ptr);
        i++;
        ptr+=1;
    }
    imprimeTrama(temp,ptr);
    ptr+=1;//se incrementa para saltar el 0x00 NULL
    printf("\ntemp=>%.2hx  ptr=%d",temp[ptr],ptr);
    memcpy(Tipo_Peticion,temp+ptr,2);
    ptr+=2;
    memcpy(Clase_Peticion,temp+ptr,2);
    ptr+=2;

    i=0;
    //para leer el nombre DNS (hay un descuadre)
    while(temp[ptr]!=0x00){
        Nombre_RR[i]=temp[ptr];
        //printf("\n%d=>%c  ptr=%d",i,Nombre_Peticion[i],ptr);
        i++;
        ptr+=1;
    }
    ptr+=1;//se incrementa para saltar el 0x00 NULL

    memcpy(Tipo_Registro,temp+ptr,2);
    ptr+=2;
    memcpy(Clase_Registro,temp+ptr,2);
    ptr+=2;
    memcpy(RR_TTL,temp+ptr,2);
    ptr+=2;
    memcpy(Tam_Datos_RR,temp+ptr,2);
    ptr+=2;

    int tamdatos=unchar_to_int(Tam_Datos_RR);

    for (size_t i = 0; i < tamdatos; i++){
        Datos_RR[i++]=temp[ptr];
        ptr+=1;
    }
    //ENCABEZADO DNS
    printf("\n****************Actualizacion DNS****************\n");
    printf("\n------------------Encabezado DNS----------------\n");
    printf("\n\tId de transaccion:%.2hx %.2hx", ID_Transaccion[0],ID_Transaccion[1]);
    printf("\n\tIndicadores:%d %d",Indicadores[0],Indicadores[1]);
    printf("\n\tContador RR de peticiones:%d ",unchar_to_int(Cont_Peticiones));
    printf("\n\tContador RR de respuestas:%d",unchar_to_int(Cont_RR_Respuestas));
    printf("\n\tContador RR de autoridad:%d ",unchar_to_int(Cont_RR_Autoridad));
    printf("\n\tContador RR de adicionales:%d ",unchar_to_int(Cont_RR_Adicionales));

    //ENTRADA DE SOLICITUD
    printf("\n---------------Entrada de solicitud-------------\n");
    printf("\n\tNombre de peticion:%s ",formatoDNSNombre(Nombre_Peticion));
    printf("\n\tTipo de peticion:%d ",unchar_to_int(Tipo_Peticion));
    printf("\n\tClase de peticion:%d ",unchar_to_int(Clase_Peticion));
    
    imprimeTrama(dns->array_dns,dns->longitud_array);
    return 0;
    // Estructura de Recursos RR 
    printf("\n-------------Estructura de Recursos RR-----------\n");
    printf("\n\tNombre de RR:%s ",formatoDNSNombre(Nombre_RR));
    printf("\n\tTipo de Registro:%d ",unchar_to_int(Tipo_Registro));
    printf("\n\tClase de Registro:%d ",unchar_to_int(Clase_Registro));
    printf("\n\tTiempo de respuesta: %d:%d:%d:%d Seg. ",RR_TTL[0],RR_TTL[1],RR_TTL[2],RR_TTL[3]);
    printf("\n\tTamaño de los Datos RR:%d ",unchar_to_int(Tam_Datos_RR));
    printf("\n\tDatos_RR:%d ",unchar_to_int(Datos_RR));

    printf("\n**************Fin actualizacion DNS**************\n");

    imprimeTrama(dns->array_dns,dns->longitud_array);

    //formatoDNSNombre(Nombre_Peticion);
    //formatoDNSNombre(Nombre_RR);
    
}
//se encarga de convertir el nombre de nominio a su formato de solicitud
unsigned char* formatoNombreDNS(unsigned char* nombre){
    unsigned char* nombre_aux=nombre;
    unsigned char nombre_temp[512];
    for (int i = 0; i < 512; i++){nombre_temp[i]=0x00;}
    
    int ptr_A=0,contador,ptr_B=0,nombre_tam=strlen(nombre_aux);
    //printf("\nTamaño:%d",nombre_tam);

    while ((ptr_A-1)!=nombre_tam){
        //nos indicara el numero de caracteres antes de encontrar el punto

        contador=0;
        //buscando la aparicion del punto
        contador=strcspn(nombre_aux+ptr_A,".");
        //printf("\nptr_B=%d,ptr_A=%d,Cont=%d",ptr_B,ptr_A,contador);
        //anteponiendo el numero despues la cadena
        nombre_temp[ptr_B++]=(unsigned char)contador;
        //se pasa a la siguiente posicion y se coloca los caracteres antes del punto
        memcpy(nombre_temp+ptr_B,nombre_aux+ptr_A,contador);

        ptr_A=ptr_A+contador+1;//se suma 1 para pasar despues del punto
        ptr_B=ptr_B+contador;
    
    }
    unsigned char* form_nombre_DNS=(unsigned char*)malloc(sizeof(unsigned char)*strlen(nombre_temp));

    return memcpy(form_nombre_DNS,nombre_temp,strlen(nombre_temp));
}
//convierte el formato se solicitor o respuesta a un formato comun de lectura
unsigned char* formatoDNSNombre(unsigned char* nombre_dns){
    unsigned char nombre_temp[512];
    for (int i = 0; i < 512; i++){nombre_temp[i]=0x00;}

    int num,i,j=0,indice=0;
    while(1){

        num=(int)nombre_dns[j++];
        //printf("\n num:%d",num);
        i = 0;
        while(i < num){
            nombre_temp[indice+i]=nombre_dns[j];
            //printf("\nnombre_temp[%d]=%c ,indice: %d, i:%d, j:%d",indice+i,nombre_temp[indice+i],indice,i,j);
            i++;
            j++;
        }
        indice+=i;
        if(nombre_dns[j]==0x00){break;}
        
        nombre_temp[indice]=(unsigned char)'.';
        //printf("\nnombre_temp[%d]=%c ,indice: %d, i:%d, j:%d",indice,nombre_temp[indice],indice,i,j);
        indice++;
        i++;
        

    }
    unsigned char* form_nombre=(unsigned char*)malloc(sizeof(unsigned char)*j);
    
    return memcpy(form_nombre,nombre_temp,j);
}
//imprime las tramas tanto de las solicitudes o las actualizaciones
void imprimeTrama(unsigned char* trama,int tam){

    puts("\n");
    for (int i = 0; i < tam;)
    {
        //printf("  %.2hx[%d]",trama[i],trama[i]);
        printf("  %.2hx",trama[i]);
        i++;
        if (i%16==0){
            puts("\n");
        }
        
    }
    puts("\n");
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
    //printf("\nn1=> %d ,n2=> %d\n\n",n1,n2);
    n=n1+n2;
    //printf("\nn=%d",n);
    return n;
}

