#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//este programa copia cualtier tipo de archivo, provado con omagenes musica y pdfs
int main(){
    unsigned char name_file_origen[20];
    unsigned char name_file_destino[20];
    unsigned char buffer[2048];
    unsigned char c='0';


    printf("Nombre del Archivo Origen:");
    scanf("%s",name_file_origen);

    FILE* file_origen=fopen(name_file_origen,"rb");

    if(file_origen==NULL){
        printf("Error al abrir archivo origen. O no existe\n");
    }
    else {

        printf("Nombre del Archivo Destino:");
        scanf("%s",name_file_destino);

        FILE* file_destino=fopen(name_file_destino,"wb");

        if(file_origen==NULL){
            printf("Error al abrir archivo destino. O no existe\n");
        }
        else {

            //numero de bytes que se fragmentara el archivo
            //cada cuantos bytes se leera el archivo
            int fragmento=1;
            //numero de bytes leidos que se guardaran el el buffer
            int num=512;
            //numero de bytes que se leyeron
            int cantidad;

            while (!feof(file_origen)){
                
                cantidad=fread(buffer,fragmento,num,file_origen);

                fwrite(buffer,fragmento,cantidad,file_destino);

                printf("\nnum=%d",num);
                printf("\ncantidad=%d",cantidad);
            }
        }

    }

    puts("\n");

    return 0;
}