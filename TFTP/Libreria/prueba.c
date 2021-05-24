#include <stdio.h>
#include "miTFTP.h"

int main(){
    printf("\ninicio de la prueba:");

    TFTP_Struct rrq,wrq,data,ack,error;
    rrq=Struct_RRQ("prueba.mp3");
    wrq=Struct_WRQ("prueba.mp3");
    data=Struct_DATA(int_to_unchar(64),"mensaje dentro del la esctructura data");
    ack=Struct_ACK(int_to_unchar(22));
    error=Struct_ERROR(int_to_unchar(7),"mensaje de error dentro de la estructura error");


    printf("\n\nRRQ:");
    MostrarTFTP_Struct(rrq);
    unsigned char* un_code1=ObtenerCodigoOp(rrq);
    //int code1=unchar_to_int(un_code1);
    printf("\nCodeOP:%x %x",un_code1[0],un_code1[1]);
    //printf("\nCodeOP:%s",ObtenerCodigoOp(rrq));
    printf("\nArchivo:$s",ObtenerNombreFile(rrq));

    printf("\n\nWRQ:");
    //MostrarTFTP_Struct(wrq);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(wrq)));
    printf("\nArchivo:$s",ObtenerNombreFile(wrq));

    printf("\n\nDATA:");
    //MostrarTFTP_Struct(data);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(data)));
    printf("\nBloque:%d",unchar_to_int(ObtenerBloque(data)));
    printf("\nDatos:%s",ObtenerDatos(data));

    printf("\n\nACK:");
    //MostrarTFTP_Struct(ack);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(ack)));
    printf("\nBloque:%d",unchar_to_int(ObtenerBloque(ack)));

    printf("\n\nERROR:");
    //MostrarTFTP_Struct(error);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(error)));
    printf("\nCodigo Error:%d",unchar_to_int(ObtenerCodigoErr(error)));
    printf("\nMensaje Error:%s",ObtenerMsjErr(error));

    

    return 0;
}