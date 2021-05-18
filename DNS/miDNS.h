


// Esctructura del la Solicitud/Consulta de DNS
unsigned char* ConsultaDNS(unsigned char* ){
    unsigned char* Encabezado_DNS; //struct
    unsigned char* Entrada_Solicitud; //struct
    
}
// Estructura del encabezado DNS
unsigned char* EncabezadoDNS(){
    unsigned char* ID_Transaccion;
    unsigned char* Indicadores; //struct
    unsigned char* Cont_Repeticiones;
    unsigned char* Cont_RR_Respuestas; //struct
    unsigned char* Cont_RR_Autoridad;//struct
    unsigned char* Cont_RR_Adicionales;//struct

}
// estructura de la Entrada de Solicitud
unsigned char* EntradaSolicitud(){
    unsigned char* Nombre_Peticion //longitud variable
    unsigned char* Tipo_Peticion
    unsigned char* Clase_Peticion
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

// Estructura de los Indicadores
unsigned char* DefinirIndicadores(){
    unsigned char* Sol_Rep
    unsigned char* Codigo_Op
    unsigned char* Resp_Autoridad
    unsigned char* Truncado
    unsigned char* Dese_Recursion
    unsigned char* Recursion_Disponibilidad
    unsigned char* Reservado
    unsigned char* Codigo_Retorno

}
// Estructura del la actualizacion de DNS
unsigned char* ActualizacionDNS(unsigned char* consulta_paq){
    unsigned char*
    unsigned char*
    unsigned char*
    unsigned char*


}




cliente
www.google.com.mx
tipo de solicitudad 

solo esperalemos






servidor

leer el tipo de registro que eligio el tiempo
llegan la solicitud de registro depende de lo que pida el cliente