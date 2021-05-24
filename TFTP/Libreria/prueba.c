#include <stdio.h>
#include "miTFTP.h"

int main(){
    printf("\ninicio de la prueba:");

    TFTP_Struct rrq,wrq,data,ack,error;
    rrq=Struct_RRQ("prueba.mp3");
    wrq=Struct_WRQ("comprobar.doc");
    data=Struct_DATA(int_to_unchar(64),"mensaje dentro del la estructura data");
    ack=Struct_ACK(int_to_unchar(22));
    error=Struct_ERROR(int_to_unchar(7),"mensaje de error XX");


    printf("\n\nRRQ:");
    // MostrarTFTP_Struct(rrq);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(rrq)));
    printf("\nArchivo:%s\nTam:%ld",ObtenerNombreFile(rrq),strlen(ObtenerNombreFile(rrq)));

    printf("\n\nWRQ:");
    // MostrarTFTP_Struct(wrq);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(wrq)));
    printf("\nArchivo:%s\nTam:%ld",ObtenerNombreFile(wrq),strlen(ObtenerNombreFile(wrq)));

    printf("\n\nDATA:");
    // MostrarTFTP_Struct(data);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(data)));
    printf("\nBloque:%d",unchar_to_int(ObtenerBloque(data)));
    printf("\nDatos:%s",ObtenerDatos(data));

    printf("\n\nACK:");
    // MostrarTFTP_Struct(ack);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(ack)));
    printf("\nBloque:%d",unchar_to_int(ObtenerBloque(ack)));

    printf("\n\nERROR:");
    // MostrarTFTP_Struct(error);
    printf("\nCodeOP:%d",unchar_to_int(ObtenerCodigoOp(error)));
    printf("\nCodigo Error:%d",unchar_to_int(ObtenerCodigoErr(error)));
    printf("\nMensaje Error:%s",ObtenerMsjErr(error));

    puts("\n\n");

    return 0;
}