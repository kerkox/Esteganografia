/* 
 * File:   esteganografia.c
 * Author: paulker
 *
 * Created on November 8, 2016, 3:45 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 
 */

void cifrar(int leido, char * bytes, char * texto char * oculto) {
    int index = 0;
    while (index < leido) {
        //proceso de sacar los binarios de los bytes para pasarlo en los 
        //bits menos significativos 
        printf("%d ", (int) bytes[index]);
        index++;
    }
}

int validar_formato(char * ruta, int size) {
    int valido = 0;
    int index = 0;
    char formato[3];
    int x = 0;

    while (index < size) {
        if (ruta[index] == '.') break;
        index++;
    }
    strcpy(formato, "jpg");
    for (x = 0; x < 3; x++) {
        if (ruta[index] != formato[x]) break;
        if (x == 2) valido = 1;
    }
    if (valido != 1) {
        strcpy(formato, "JPG");
        for (x = 0; x < 3; x++) {
            if (ruta[index] != formato[x]) break;
            if (x == 2) valido = 1;
        }
    }


    return valido;
}

void ocultar_datos();

int menu(){
    system("clear");
    printf("Esteganografia con imagenes\n\n");
    printf("1. Ocultar Informacion en Imagen\n");
    printf("2. Obtener Informacion de Imagen\n");
    printf("3. salir\n");
    printf("\nIngresa la opcion: ");
    int opcion=-1;
    scanf("%d", &opcion);
    
    if(opcion<1||opcion>3) opcion=-1;
    
    return opcion;
}

int main(int argc, char** argv) {
    int opcion;
    do{
    opcion = menu();
    }while(opcion!=3);
    
    while (getchar() != '\n');
    return (EXIT_SUCCESS);
}

void ocultar_datos(){
    FILE * imagen;
    FILE * texto;
    FILE * imagen_cifrada;
    char ruta[30];
    char ruta_texto[30];
    int tama = 8;
    int leido = 0;
    int index = 0;
    char bytes[tama];
    char bytes_texto[(tama / 8)];

    int formato_valido = 0;
    while (formato_valido) {
        printf("Ingresa la ruta de la imagen: ");
        scanf("%s", ruta);
        formato_valido = validar_formato(ruta, sizeof (ruta));
    }

    printf"\nIngresa el archivo a ocultar: ");
    scanf("%s", ruta_texto);

    imagen = fopen(ruta, "rb");
    texto = fopen(ruta_texto, "rb");
    //primero se debe de preguntar el tamaño para saber si el texto 
    //cabe en la imagen
    int size_imagen = 0;
    int size_texto = 0;

    fseek(imagen, 0L, SEEK_END);
    size_imagen = ftell(imagen);
    rewind(imagen);

    fseek(texto, 0L, SEEK_END);
    size_texto = ftell(texto);
    rewind(texto);

    //ahora aqui va la pregunta para saber el tamaño
    if ((size_texto * 8) > size_imagen) {
        printf("\nEl tamaño de la imagen es muy pequeño para ocultar la info\n");
        return 0; // para terminar el proceso de cifrado
    }





    while (!feof(imagen)) {
        //aqui vamos a leer los bytes del archivo

        leido = fread(bytes, sizeof (char), tama, imagen);
        fread(bytes_texto, sizeof(char),(tama/8), texto);
        char oculto[leido];
        if (!feof(texto)) {
            cifrar(leido, bytes, oculto);
            fwrite(oculto, sizeof (char), leido, imagen_cifrada);
        }else{
            //aqui para cuando ya termino de escribir los bytes del archivo de texto
            fwrite(bytes, sizeof (char), leido, imagen_cifrada);
        }
        


    }


}

