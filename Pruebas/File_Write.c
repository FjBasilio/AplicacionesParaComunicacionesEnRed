#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
    //guardado del nombre del archivo
    unsigned char name_file[30];
    unsigned char buffer[200];
    unsigned char c;

    printf("Nombre del Archivo: ");
    scanf("%s",name_file);

    printf("=> %s",name_file);

    FILE* file=fopen(name_file,"w");

    if(file==NULL){
        printf("Error al abrir archivo.\n");
    }
    else {
        //se lee una linea en la consola y se escribe en el archivo
        c=fgetc(stdin);
        
    }

    fputs(buffer,file);

    return 0;
}