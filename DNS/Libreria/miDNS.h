
#include "miUDP_DNS.h"
#include <math.h>

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

///*********************************PROTOTIPOS********************************
int enviarSolicitud(unsigned char* nombre_dominio,Direccion dir_envio,Direccion dir_recibe);

DNS solicitudDNS(unsigned char id,unsigned char* nombre,unsigned char tipo,unsigned char clase);
int actualizacionDNS(DNS dns);

unsigned char* NombreToTrama(unsigned char* nombre);
unsigned char* TramaToNombre(unsigned char* nombre_dns);

void imprimeTrama(unsigned char* trama,int tam);
unsigned char* int_to_unchar(int int_num);
int unchar_to_int(unsigned char* unchar_num);
int* toBinary(unsigned char* unchar_num);
unsigned char* toInt(int* bites);
int pot(int base, int exponente);
int esApuntador(unsigned char* unchar_num);
unsigned char* ArmaDominios(unsigned char* trama,int ptr,int* num_saltos);
///*******************************FIN PROTOTIPOS*******************************

//-----------Envia una solicitud y espera a recibir una respuesta--------------
int enviarSolicitud(unsigned char* nombre_dominio,Direccion dir_envio,Direccion dir_recibe){
    //se crea un DNS con solicitud 
    DNS dns_sol=solicitudDNS(0x10,nombre_dominio,0x01,0x01);
    printf("\nEnviando solicitud");
    //se envia el DNS 
    if (enviar(dir_envio,dns_sol)==-1){
            perror("\nError al enviar solicitud\n");
            return -1;
        }else{
            printf("\nSe envio solicitud");
        }
    //como se tuvo exito en el envio se espera la actualizacion DNS
    DNS dns_res=recibir(dir_recibe);
    if (dns_res==NULL){
            perror("\nError al recibir respuesta\n");
            return -1;
        }else{
            printf("\nSe recibio respuesta\n");
        }
    //como se tuvo exito al recibir el DNS lo analizamos
    actualizacionDNS(dns_res);

    return 0;
}



//----regresa una solicitud DNS, ingresando un id, un numbre de dominio , el tipo y su clase-----
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

    //se crea la estructura DNS
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
    Nombre_Peticion=NombreToTrama(nombre);
    Tipo_Peticion[1]=tipo;
    Clase_Peticion[1]=clase;

    //llenando el arreglo DNS
    memcpy(temp+ptr,ID_Transaccion,2);ptr+=2;
    memcpy(temp+ptr,Indicadores,2);ptr+=2;
    memcpy(temp+ptr,Cont_Peticiones,2);ptr+=2;
    memcpy(temp+ptr,Cont_RR_Respuestas,2);ptr+=2;
    memcpy(temp+ptr,Cont_RR_Autoridad,2);ptr+=2;
    memcpy(temp+ptr,Cont_RR_Adicionales,2);ptr+=2;

    memcpy(temp+ptr, Nombre_Peticion,strlen(Nombre_Peticion));ptr+=strlen(Nombre_Peticion);
    memcpy(temp+ptr,cero,1);ptr+=1;//para identificar el fin del nombre de dominio
    memcpy(temp+ptr,Tipo_Peticion,2);ptr+=2;
    memcpy(temp+ptr,Clase_Peticion,2);ptr+=2;

    unsigned char* temp_dns=(unsigned char*)malloc(sizeof(unsigned char)*ptr);
    memcpy(temp_dns,temp,ptr);
    dns->array_dns=temp_dns;
    dns->longitud_array=ptr;
    ////mostrando los datos que se envian
    //ENCABEZADO DNS
    printf("\n****************Solicitud DNS****************\n");
    printf("\n----------------Encabezado DNS----------------");
    printf("\n\tId de transaccion:%.2hx %.2hx", ID_Transaccion[0],ID_Transaccion[1]);
    printf("\n\tIndicadores:%d %d",Indicadores[0],Indicadores[1]);
    int ind_1=0,ind_2=0;
    unsigned char aux[]={0x00,Indicadores[0]};
    unsigned char aux2[]={0x00,Indicadores[1]};
    int *Ind_bin=toBinary(Indicadores);
    printf("\n\t\tSolicitud/respuesta:%d",Ind_bin[0]);
    printf("\n\t\tCodigo de operacion:%d %d %d %d",Ind_bin[1],Ind_bin[2],Ind_bin[3],Ind_bin[4]);
    printf("\n\t\tRespuesta de autoridad:%d",Ind_bin[5]);
    printf("\n\t\tTruncado:%d",Ind_bin[6]);
    printf("\n\t\tDeseo de Recursion:%d",Ind_bin[7]);
    printf("\n\t\tRecursion Disponible:%d",Ind_bin[8]);
    printf("\n\t\tReservado:%d %d %d",Ind_bin[9],Ind_bin[10],Ind_bin[11]);
    printf("\n\t\tCodigo de retorno:%d %d %d %d",Ind_bin[12],Ind_bin[13],Ind_bin[14],Ind_bin[15]);
    printf("\n\tContador RR de peticiones:%d ",unchar_to_int(Cont_Peticiones));
    printf("\n\tContador RR de respuestas:%d",unchar_to_int(Cont_RR_Respuestas));
    printf("\n\tContador RR de autoridad:%d ",unchar_to_int(Cont_RR_Autoridad));
    printf("\n\tContador RR de adicionales:%d ",unchar_to_int(Cont_RR_Adicionales));

    //ENTRADA DE SOLICITUD
    printf("\n--------------Entrada de solicitud-------------");
    printf("\n\tNombre de peticion:%s ",TramaToNombre(Nombre_Peticion));
    printf("\n\tTipo de peticion:%d ",unchar_to_int(Tipo_Peticion));
    printf("\n\tClase de peticion:%d ",unchar_to_int(Clase_Peticion));
    printf("\n\n**************Fin Solicitud DNS**************\n");
    //Se imprime la trama que se va a enviar
    imprimeTrama(dns->array_dns,dns->longitud_array);
    return dns;
}
//-----cuando se recibe una respuesta esta funcion se encarga de ilustrar los datos----
int actualizacionDNS(DNS dns){
    //ENCABEZADO DNS
    unsigned char ID_Transaccion[]={0x00,0x00};//el cliente decide el id
    unsigned char Indicadores[]={0x00,0x00}; //definen si es solicitud o respuesta o autoridades //ESTRUCTURA DEFINIDA EN BITS
    unsigned char Cont_Peticiones[]={0x00,0x00};//contador de peticiones mismo url
    unsigned char Cont_RR_Respuestas[]={0x00,0x00}; //contador de respuestas
    unsigned char Cont_RR_Autoridad[]={0x00,0x00};//contador de autoridades
    unsigned char Cont_RR_Adicionales[]={0x00,0x00};//contadores adicionales

    //ENTRADA DE SOLICITUD
    unsigned char* Nombre_Peticion; // longitud variable
    unsigned char Tipo_Peticion[]={0x00,0x00};//longitu fija // 0x01=registro host,0x02=registro servidor nombres,0x05 registro alis,0x0C=registro solicutud invers
    unsigned char Clase_Peticion[]={0x00,0x00};//longitud fija

    // Estructura de Recursos RR 
    unsigned char Apuntador[]={0x00,0x00};
    unsigned char* Nombre_RR; // longitud variable, nombre del dominio que solicitamos
    unsigned char Tipo_Registro[]={0x00,0x00}; //logitud fija
    unsigned char Clase_Registro[]={0x00,0x00}; //logitud fija
    unsigned char RR_TTL[]={0x00,0x00,0x00,0x00}; //logitud fija, tiempo de vida en seg
    unsigned char Tam_Datos_RR[]={0x00,0x00}; //logitud fija, tamaño de la respuesta
    unsigned char* Datos_RR; //logitud variable 
    unsigned char IP4[]={0x00,0x00,0x00,0x00};//IP del dominio solicitado

    unsigned char cero[]={0x00};
    unsigned char* temp=dns->array_dns;
    int num_saltos=0,ptr_aux=0,ptr=0;
        //con forma se va avanzando en la trama se va haciendo la lectura de los datos y a su ves mostrando
    //ENCABEZADO DNS
    printf("\n****************Actualizacion DNS****************\n");
    printf("\n------------------Encabezado DNS----------------");
    //printf("\nptr=%d",ptr);
    memcpy(ID_Transaccion,temp+ptr,2);ptr+=2;
    printf("\n\tId de transaccion:%.2hx %.2hx", ID_Transaccion[0],ID_Transaccion[1]);

    //printf("\nptr=%d",ptr);
    memcpy(Indicadores,temp+ptr,2);ptr+=2;
    printf("\n\tIndicadores:%d %d",Indicadores[0],Indicadores[1]);
    int *Ind_bin=toBinary(Indicadores);
    printf("\n\t\tSolicitud/respuesta:%d",Ind_bin[0]);
    printf("\n\t\tCodigo de operacion:%d %d %d %d",Ind_bin[1],Ind_bin[2],Ind_bin[3],Ind_bin[4]);
    printf("\n\t\tRespuesta de autoridad:%d",Ind_bin[5]);
    printf("\n\t\tTruncado:%d",Ind_bin[6]);
    printf("\n\t\tDeseo de Recursion:%d",Ind_bin[7]);
    printf("\n\t\tRecursion Disponible:%d",Ind_bin[8]);
    printf("\n\t\tReservado:%d %d %d",Ind_bin[9],Ind_bin[10],Ind_bin[11]);
    printf("\n\t\tCodigo de retorno:%d %d %d %d",Ind_bin[12],Ind_bin[13],Ind_bin[14],Ind_bin[15]);

    //printf("\nptr=%d",ptr);
    memcpy(Cont_Peticiones,temp+ptr,2);ptr+=2;
    printf("\n\tContador RR de peticiones:%d ",unchar_to_int(Cont_Peticiones));

    //printf("\nptr=%d",ptr);
    memcpy(Cont_RR_Respuestas,temp+ptr,2);ptr+=2;
    printf("\n\tContador RR de respuestas:%d",unchar_to_int(Cont_RR_Respuestas));

    //printf("\nptr=%d",ptr);
    memcpy(Cont_RR_Autoridad,temp+ptr,2);ptr+=2;
    printf("\n\tContador RR de autoridad:%d ",unchar_to_int(Cont_RR_Autoridad));

    //printf("\nptr=%d",ptr);
    memcpy(Cont_RR_Adicionales,temp+ptr,2);ptr+=2;
    printf("\n\tContador RR de adicionales:%d ",unchar_to_int(Cont_RR_Adicionales));


    //ENTRADA DE SOLICITUD
    printf("\n---------------Entrada de solicitud-------------");

    //printf("\nptr=%d",ptr);
    Nombre_Peticion=ArmaDominios(temp,ptr,&num_saltos);ptr+=num_saltos;
    printf("\n\tNombre de peticion:%s ",TramaToNombre(Nombre_Peticion));
    num_saltos=0;

    //printf("\nptr=%d",ptr);
    memcpy(Tipo_Peticion,temp+ptr,2);ptr+=2;
    printf("\n\tTipo de peticion:%d ",unchar_to_int(Tipo_Peticion));
    
    //printf("\nptr=%d",ptr);
    memcpy(Clase_Peticion,temp+ptr,2);ptr+=2;
    printf("\n\tClase de peticion:%d ",unchar_to_int(Clase_Peticion));
    
    do{
        // Estructura de Recursos RR 
        printf("\n-------------Estructura de Recursos RR-----------");
        //checamos si hay un apuntador 
        //printf("\nptr=%d",ptr);
        Nombre_RR=ArmaDominios(temp,ptr,&num_saltos);ptr+=num_saltos; //se regresa un numero de saltos que se tiene que dar
        printf("\n\tNombre de RR:%s ",TramaToNombre(Nombre_RR));
        num_saltos=0;
        //printf("\nptr=%d",ptr);
        memcpy(Tipo_Registro,temp+ptr,2);ptr+=2;
        printf("\n\tTipo de Registro:%d ",unchar_to_int(Tipo_Registro));

        //printf("\nptr=%d",ptr);
        memcpy(Clase_Registro,temp+ptr,2);ptr+=2;
        printf("\n\tClase de Registro:%d ",unchar_to_int(Clase_Registro));
        
        //printf("\nptr=%d",ptr);
        memcpy(RR_TTL,temp+ptr,4);ptr+=4;
        printf("\n\tTiempo de respuesta: %d:%d:%d:%d Seg. ",RR_TTL[0],RR_TTL[1],RR_TTL[2],RR_TTL[3]);

        //printf("\nptr=%d",ptr);
        memcpy(Tam_Datos_RR,temp+ptr,2);ptr+=2;
        printf("\n\tTamaño de los Datos RR:%d ",unchar_to_int(Tam_Datos_RR));

        int tamdatos=unchar_to_int(Tam_Datos_RR);
        num_saltos=0;
        //printf("\nptr=%d",ptr);
        if(tamdatos==4){
            memcpy(IP4,temp+ptr,4);ptr+=4;
            printf("\n\tDatos RR:%d.%d.%d.%d ",IP4[0],IP4[1],IP4[2],IP4[3]);
        }else{
            Datos_RR=ArmaDominios(temp,ptr,&num_saltos);ptr+=num_saltos;
            printf("\n\tDatos RR:%s ",TramaToNombre(Datos_RR));
        }

    }while(ptr<dns->longitud_array);

    printf("\n\n**************Fin actualizacion DNS**************\n");
    imprimeTrama(dns->array_dns,dns->longitud_array);
    
}
//se encarga de convertir el nombre de Dominio a su formato de la trama
unsigned char* NombreToTrama(unsigned char* nombre){
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
unsigned char* TramaToNombre(unsigned char* nombre_dns){
    unsigned char nombre_temp[512];
    for (int i = 0; i < 512; i++){nombre_temp[i]=0x00;}

    int num,i,j=0,indice=0;
    while(1){
        num=(int)nombre_dns[j++];
        //printf("\nnum:%d",num);
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
        //printf("  %.2hx[%d]",trama[i],i);
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

//convierte un unchar a formato binario en string de enteros
//unchar de 2 bytes---binario de 16 caracteres
int* toBinary(unsigned char* unchar_num){
    int num=unchar_to_int(unchar_num);
    int* bites=(int*)malloc(sizeof(int)*16);
    for(int i = 0; i < 16; i++){bites[i]=0;}

    for(int i = 15; i >=0; i--){
        bites[i]=num%2;
        num=num/2;
    }
    return bites;
}
//convierte de un formato binario de enteros a unchar
//unchar de 2 bytes---binario de 16 caracteres
unsigned char* toInt(int* bites){
    int cont=0,num=0;
    
    for(int i = 15; i >=0; i--){
        if(bites[i]==1){
            num=num+pot(2,cont);
            //printf("\nnum=%d",num);
        }
        cont++;
    }

    return int_to_unchar(num);
}

//regresa la potencia
int pot(int base, int exponente){
    int res=1;
    while (exponente--){
        res=res*base;
    }
    return res;
}

//a traves de un unchar de 2 bytes sabremos si es un apuntador
//si es puntador regresara la posicion, caso contrario -1
int esApuntador(unsigned char* unchar_num){
    int* bites=toBinary(unchar_num);
    //for (int i = 0; i < 16; i++){printf("%d",bites[i]);}
    
    unsigned char* uc_num;
    if(bites[0]==1 && bites[1]==1){
        bites[0]=0;
        bites[1]=0;
        uc_num=toInt(bites);
        return unchar_to_int(uc_num);
    }
    return -1;
}

//arma un nombre de dominio usando la recursividad en casod e encontrar apuntadores
//regresa el nombre de domino y las posiciones que tenemmos que avanzar 
//se da por hecho que regresara un nombre de dominio
unsigned char* ArmaDominios(unsigned char* trama,int ptr,int* num_saltos){
    //unsigned char nombre_temp[512];
    unsigned char* nombre_temp=(unsigned char*)malloc(sizeof(unsigned char)*1);
    *nombre_temp=0x00;
    unsigned char Apuntador[]={0x00,0x00};
    unsigned char* aux;
    //for (int i = 0; i < 512; i++){nombre_temp[i]=0x00;}
    int ptr_aux,num_saltos_aux,i=0;

    
    memcpy(Apuntador,trama+ptr,2);
    ptr_aux=esApuntador(Apuntador);
    //printf("\n\t\tptr=%d\tptr_aux=%d  condicion=%d",ptr,ptr_aux,trama[ptr]!=0x00);
        if(ptr_aux!=-1){
            //si hay un apuntador
            //printf("\n\t\tValor apuntador:%d",ptr_aux);
            //guardamos la cadena generada
            aux=ArmaDominios(trama,ptr_aux,num_saltos);
            //printf("\nUn regreso");
            //saltamos el apuntador y el null, sobreescribiendo los saltos
            *num_saltos=2; 
            //printf("\nUn regreso,saltos=%d",*num_saltos);
            return aux;

        }else{ 
            //en el caso de no existir apuntador
            nombre_temp[0]=trama[ptr];
            //printf("\n\t\tptr=%d=>%.2hx",ptr,trama[ptr]);
            if(trama[ptr]!=0x00){
                aux=strcat(nombre_temp,ArmaDominios(trama,ptr+1,num_saltos));
                *num_saltos=*num_saltos+1;//sumando saltos pendientes
                //printf("\nUn regreso,saltos=%d",*num_saltos);
                return aux;
            }
            else{
                //printf("\nComienza return");
                *num_saltos=*num_saltos+1;//sumando saltos pendientes
                return trama+ptr;
            }
        }
}
