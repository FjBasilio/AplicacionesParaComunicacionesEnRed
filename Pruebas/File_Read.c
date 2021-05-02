#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
    unsigned char nombre_file[20];
    unsigned char buffer[200];
    unsigned char c='0';


    printf("Nombre del Archivo: ");
    scanf("%s",nombre_file);
    printf("=> %s, tamaño:%d",nombre_file,strlen(nombre_file));

    FILE* file=fopen(nombre_file,"r");

    if(file==NULL){
        printf("Error al abrir archivo.\n");
    }
    else {
        
        for(int i=0 ;feof(file) == 0; i++){ //mientras que no se detecte el fin de archivo

            c=fgetc(file); //se toma los caracteres en orden
            buffer[i]=c;
            printf("=>%c-%x-%d\n",c,c,c);
            //sleep(1);
        }
    }

    //printf("EOF=%d",EOF);
    //se reinicia el puntero para volver a leer
    rewind(file);

    puts(buffer);

    FILE* bin_file=fopen("destino.txt","w");
    fputs(buffer,bin_file);

    return 0;
}