#include "miDNS.h"

void nombre_formato_dns();

int main(){
    unsigned char num[]={0x01,0x01};
    int* bits=toBinary(num);
    for (int i = 0; i < 16; i++)
    {
        printf("%d",bits[i]);
    }
    
    unsigned char* num2=toInt(bits);
    printf("\n%.2hx %.2hx",num2[0],num2[1]);
    
    return 0;
}


void nombre_formato_dns(){
    
    unsigned char nombre_dns[50];
    printf("\nEscriba un nombre de dominio: ");
    scanf("%s",nombre_dns);

    unsigned char* rcv=formatoNombreDNS(nombre_dns);

    printf("\nEl formato seria:");

    for (int i = 0; i < strlen(rcv); i++)
    {
        fprintf(stdout,"\n[%d]=%c,%d,%x",i,rcv[i],rcv[i],rcv[i]);
    }
    puts("\n");

}