/* 
 * File:   esteganografia.c
 * Author: paulker
 *
 * Created on November 8, 2016, 3:45 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * 
 */
typedef struct CabeceraBMP {
    /* 2 bytes de identificación */
    uint32_t size; /* Tamaño del archivo */
    uint16_t resv1; /* Reservado */
    uint16_t resv2; /* Reservado */
    uint32_t offset; /* Offset hasta hasta los datos de imagen */
} CabeceraBMP;

typedef struct InfoCabeceraBMP {
    uint32_t headersize; /* Tamaño de la cabecera */
    uint32_t width; /* Ancho */
    uint32_t height; /* Alto */
    uint16_t planes; /* Planos de color (Siempre 1) */
    uint16_t bpp; /* bits por pixel */
    uint32_t compress; /* compresión */
    uint32_t imgsize; /* tamaño de los datos de imagen */
    uint32_t bpmx; /* Resolución X en bits por metro */
    uint32_t bpmy; /* Resolución Y en bits por metro */
    uint32_t colors; /* colors used en la paleta */
    uint32_t imxtcolors; /* Colores importantes. 0 si son todos */
} InfoCabeceraBMP;

void mostrarDatosCabeceraBMP(CabeceraBMP * datos) {
    printf("\nsize of file: %u", datos->size);
    printf("\nreservado1: %d", datos->resv1);
    printf("\nreservado2: %d", datos->resv2);
    printf("\noffset: %u", datos->offset);
}

void mostrarInfoCabeceraBMO(InfoCabeceraBMP * datos) {
    printf("\ntamaño de la cabecera: %u", datos->headersize);
    printf("\nancho: %u", datos->width);
    printf("\nAlto: %u", datos->height);
    printf("\nPlanos de color: %d", datos->planes);
    printf("\nBits por pixel: %d", datos->bpp);
    printf("\ncompresion: %u", datos->compress);
    printf("\nSize datos de la img: %u", datos->imgsize);
    printf("\nResolucion en X: %u", datos->bpmx);
    printf("\nResolucion en Y: %u", datos->bpmy);
    printf("\nColores: %u", datos->colors);
    printf("\nColores importantes: %u", datos->imxtcolors);




}

void cifrar(int leido, char * bytes, char * texto, char * oculto) {
    int index = 0;
    while (index < leido) {
        //proceso de sacar los binarios de los bytes para pasarlo en los 
        //bits menos significativos 
        printf("%d ", (int) bytes[index]);
        index++;
    }
}



int ocultar_datos();

unsigned char * leer_imagen(char * ruta_imagen, InfoCabeceraBMP * infoCabeceraBmp, CabeceraBMP * cabecera) {

    FILE * imagen;

    unsigned char *imgdata; //datos de imagen 
    uint16_t type; // 2 bytes indentificativos


    imagen = fopen(ruta_imagen, "r");
    if (!imagen) return NULL; // si no se puede leer no hay imagen

    //se leeen los dos primeros bytes para la identificacion
    fread(&type, sizeof (uint16_t), 1, imagen);
    printf("\nEste es el vbalor de type: %d\n", type);
    if (type != 0x4D42) {
        //aqui se comprueba el formato sino no es BMP retorna NULL
        fclose(imagen);
        return NULL;
    }
    //ahora se procede a leer la cabecera del archivo completa
    //    fread(&cabecera, sizeof (CabeceraBMP), 1, imagen);
    fread(cabecera, sizeof (CabeceraBMP), 1, imagen);
    printf("\nData cabecera: \n");
    //    mostrarDatosCabeceraBMP(&cabecera);
    mostrarDatosCabeceraBMP(cabecera);

    //ahora se lee la info de la cabecera de informacion completa
    fread(infoCabeceraBmp, sizeof (InfoCabeceraBMP), 1, imagen);
    printf("\ninfo cabecera: \n");
    mostrarInfoCabeceraBMO(infoCabeceraBmp);
    //ahora se reserva la memoria para la imagen 
    imgdata = (unsigned char*) malloc(infoCabeceraBmp->imgsize);

    /* se pone el cursor de lectura del archivo donde empiezan
     * los datos de imagen, lo indica el offset de la cabecera
     * */
    fseek(imagen, (*cabecera).offset, SEEK_SET);

    //ahora se lle los datos de la imagen tantos bytes como imgsize
    fread(imgdata, infoCabeceraBmp->imgsize, 1, imagen);

    //se cierra el archivo de la imagen
    fclose(imagen);

    //se retorna la imagen
    return imgdata;

}

int menu() {
    system("clear");
    printf("Esteganografia con imagenes\n\n");
    printf("1. Ocultar Informacion en Imagen\n");
    printf("2. Obtener Informacion de Imagen\n");
    printf("3. salir\n");
    printf("\nIngresa la opcion: ");
    int opcion = -1;
    scanf("%d", &opcion);

    if (opcion < 1 || opcion > 3) opcion = -1;

    return opcion;
}

void crear_imagen(unsigned char * dataimg, int size, InfoCabeceraBMP * infoCabeceraBmp, CabeceraBMP * cabecera) {
    FILE * imgSalida;
    imgSalida = fopen("Salida.bmp", "w+");
    printf("\nentro a crear la imagen\n");
    uint16_t type;
    type = 0x4D42;
    char formato[2] = {'B','M'};
    //se escribe que tipo de archivo es:
    fwrite(formato, sizeof (char), 2, imgSalida);
    printf("\nEscribio el tipo de archivo\n");
    //aqui se escriben las cabeceras
    fwrite(cabecera, sizeof (CabeceraBMP), 1, imgSalida);
    printf("\nEscribio la cabecera\n");
    fwrite(infoCabeceraBmp, sizeof (InfoCabeceraBMP), 1, imgSalida);
    printf("\nEscribio la info de la cabecera\n");
    //luego se escriben los datos
    fwrite(dataimg, sizeof (unsigned char*), infoCabeceraBmp->imgsize, imgSalida);
    printf("\nEscribio los datos de la imagen\n");
    fclose(imgSalida);
    printf("\nAcabo de terminar de escribir la imagen\n");
}

int main(int argc, char** argv) {
    int opcion;
    //    char raro = '§';
    FILE * imagen;
    InfoCabeceraBMP info;
    CabeceraBMP cabecera;
    unsigned char * dataimg;
    char ruta[30];
    do {

        opcion = menu();
        switch (opcion) {
            case 1:
                break;
            case 2:
                //leer los datos
                printf("ingresa la ruta de la imagen: ");
                scanf("%s", ruta);
                imagen = fopen(ruta, "rb");
                int size_imagen = 0;
                int size_texto = 0;

                fseek(imagen, 0L, SEEK_END);
                size_imagen = ftell(imagen);
                rewind(imagen);
                fclose(imagen);

                //metodo para leer la cabecera
                dataimg = leer_imagen(ruta, &info, &cabecera);
                if (dataimg == NULL) {
                    printf("\nERROR EN EL FORMATO DE IMAGEN\n");
                } else {
                    printf("\nFORMATO SI ES VALIDO---------\n");
                    int x = 0;
                    printf("\n");
                    
                    while (x <= 10) {
                        printf("%d ", dataimg[x]);
                        x++;
                    }
                    printf("\nPaso el ciclo\n");
                    crear_imagen(dataimg, size_imagen, &info, &cabecera);
                }
                printf("\n");
                while (getchar() != '\n');
                printf("\nSe salto el primer pause");
                while (getchar() != '\n');

                break;

        }

    } while (opcion != 3);

    while (getchar() != '\n');
    return (EXIT_SUCCESS);
}

int ocultar_datos() {
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
        //        formato_valido = validar_formato(ruta, sizeof (ruta));
        formato_valido = 0;
    }

    printf("\nIngresa el archivo a ocultar: ");
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
        return -1; // para terminar el proceso de cifrado
    }





    while (!feof(imagen)) {
        //aqui vamos a leer los bytes del archivo

        leido = fread(bytes, sizeof (char), tama, imagen);
        fread(bytes_texto, sizeof (char), (tama / 8), texto);
        char oculto[leido];
        if (!feof(texto)) {
            cifrar(leido, bytes, bytes_texto, oculto);
            fwrite(oculto, sizeof (char), leido, imagen_cifrada);
        } else {
            //aqui para cuando ya termino de escribir los bytes del archivo de texto
            fwrite(bytes, sizeof (char), leido, imagen_cifrada);
        }

    }
    return 0;


}

