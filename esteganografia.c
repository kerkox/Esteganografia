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


unsigned char extractor();

unsigned char * leer_imagen(char * ruta_imagen, InfoCabeceraBMP * infoCabeceraBmp, CabeceraBMP * cabecera) {
    printf("\nEntro a leer laimagen para devolver los datos");
    FILE * imagen;

    unsigned char *imgdata; //datos de imagen 
    uint16_t type; // 2 bytes indentificativos del formato


    imagen = fopen(ruta_imagen, "r");
    if (!imagen) return NULL; // si no se puede leer no hay imagen

    //se leeen los dos primeros bytes para la identificacion
    fread(&type, sizeof (uint16_t), 1, imagen);

    if (type != 0x4D42) {
        //aqui se comprueba el formato sino no es BMP retorna NULL
        fclose(imagen);
        return NULL;
    }
    //ahora se procede a leer la cabecera del archivo completa
    //    fread(&cabecera, sizeof (CabeceraBMP), 1, imagen);
    fread(cabecera, sizeof (CabeceraBMP), 1, imagen);
    //    printf("\nData cabecera: \n");
    //    mostrarDatosCabeceraBMP(&cabecera);
    //        mostrarDatosCabeceraBMP(cabecera);

    //ahora se lee la info de la cabecera de informacion completa
    fread(infoCabeceraBmp, sizeof (InfoCabeceraBMP), 1, imagen);
    //    printf("\ninfo cabecera: \n");
    //        mostrarInfoCabeceraBMO(infoCabeceraBmp);
    //ahora se reserva la memoria para la imagen 
    imgdata = (unsigned char*) malloc(infoCabeceraBmp->imgsize);

    /* se pone el cursor de lectura del archivo donde empiezan
     * los datos de imagen, lo indica el offset de la cabecera
     * */
    fseek(imagen, cabecera->offset, SEEK_SET);

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

void crear_imagen(unsigned char * dataimg, InfoCabeceraBMP * infoCabeceraBmp, CabeceraBMP * cabecera) {
    FILE * imgSalida;
    imgSalida = fopen("Salida.bmp", "w+");
    printf("\nentro a crear la imagen\n");
    uint16_t type;
    type = 0x4D42;
    char formato[2] = {'B', 'M'};
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

//void leer_datos_img(unsigned char * dataimg, InfoCabeceraBMP * info, CabeceraBMP * cabecera) {
//    char ruta[30];
//    FILE * imagen;
//    //leer los datos
//    printf("\ningresa la ruta de la imagen: ");
//    scanf("%s", ruta);
//    imagen = fopen(ruta, "rb");
//    int size_imagen = 0;
//    int size_texto = 0;
//
//    fseek(imagen, 0L, SEEK_END);
//    size_imagen = ftell(imagen);
//    rewind(imagen);
//    fclose(imagen);
//
//    //metodo para leer la cabecera
//    dataimg = leer_imagen(ruta, info, cabecera);
//    if (dataimg == NULL) {
//        printf("\nERROR EN EL FORMATO DE IMAGEN\n");
//    } else {
//        printf("\nFORMATO SI ES VALIDO---------\n");
//
//    }
//}

void crear_archivo(unsigned char * imgdata, CabeceraBMP * cabecera) {
    FILE * texto_datos;
    texto_datos = fopen("texto.txt", "w");
    //de la cabecera saco el valor de datos a leer
    int cantidad_datos = cabecera->resv1; //valor de datos a leer
    unsigned char texto[cantidad_datos];
    int x = 0;
    //    for (x = 0; x < cantidad_datos; x++) {
    //        texto[x] = imgdata[x];
    //    }
    //proceso de obtencion del ultimo bit para construir los datos
    //***********************************************************
    int index = 0;
    for (x = 0; x < cantidad_datos; x++) {
        texto[x] = extractor(imgdata, index);
        index += 8;
    }

    //ahora que se leyeron los datos vamos a escribirlos en el archivo
    fwrite(texto, sizeof (unsigned char), cantidad_datos, texto_datos);


    fclose(texto_datos);
    //este metodo debe de recibir los datos de la imagen y poder obtener la info

}

void insertar(unsigned char insertar, char * valores, int index) {
    int x = 0;
    char comparador = 128;

    for (x = index; x < (index + 8); x++) {

        valores[x] >>= 1;

        valores[x] <<= 1;

        if (insertar & comparador) {
            valores[x] += 1;
        } else {
            valores[x] += 0;
        }
        insertar <<= 1;
    }


}

unsigned char extractor(char * valores, int index) {
    unsigned char resultado = 0;

    int x = index;
    char extractor = 1;

    for (x = index; x < (index + 8); x++) {
        //ahora se hace el desplazamiento para dejar el siguiente valor
        resultado <<= 1;
        //se hace la comparacion para sacar el ultimo bit
        if (valores[x] & extractor) {
            resultado += 1;

        } else {
            resultado += 0;

        }
    }

    return resultado;

}

//void mostrar_valores(char * valores, int index) {
//    printf("\nvalores: ");
//    int x;
//    for (x = index; x < (index + 8); x++) {
//        printf("%d ", valores[x]);
//    }
//    printf("\n");
//}

//esto retorna 1 en caso de exito 
//y 0 en caso de falla

//int insertar_datos_img(unsigned char * dataimg, InfoCabeceraBMP * infoCabeceraBmp, CabeceraBMP * cabecera) {
//    printf("\nIngreso a insertar_datos_img");
//    //variables para el uso:
//    char * datos_texto;
//    FILE * texto;
//    char ruta[30];
//    printf("\nIngresa la ruta del texto: ");
//    scanf("%s", ruta);
//    texto = fopen(ruta, "rb");
//    //primero se valida que el texto quepa dentro de la imagen
//    printf("\nLeyo el archivo y ahora leera el tamaño\n");
//    int size_img = infoCabeceraBmp->imgsize;
//    printf("\nSi pudo leer el tamaño\n");
//    int size_texto = 0;
//    printf("\nLinea 1");
//    fseek(texto, 0L, SEEK_END);
//    printf("\nLinea 2");
//    size_texto = ftell(texto);
//    printf("\nLinea 3");
//    rewind(texto);
//    printf("size_texto: %d x8: %d imagen: %d", size_texto, (size_texto * 8), size_img);
//    if ((size_texto * 8) > size_img) {
//        //        printf("\nEl tamaño del texto es muy grande para la imagen\n");
//        perror("\nEl tamaño del texto es muy grande para la imagen\n");
//        return 0;
//    }
//
//    //aqui se almacena el tamaño del texto que va a insertar 
//    cabecera->resv1 = size_texto;
//
//    //se leen los datos del archivo de texto para ser insertados en la imagen
//    fread(datos_texto, sizeof (char), size_texto, texto);
//    int x = 0;
//    int index = 0;
//    for (x = 0; x < size_texto; x++) {
//        insertar(datos_texto[x], dataimg, index);
//        index += 8;
//    }
//
//    //aqui termina de insertar los datos dentro de la imagen
//    return 1; //salio con exito
//}

int main(int argc, char** argv) {
    int opcion;
    FILE * imagen;
    InfoCabeceraBMP info;
    CabeceraBMP cabecera;
    unsigned char * dataimg;
    char ruta[30];
    do {

        opcion = menu();
        switch (opcion) {
            case 1:
                //ocultar informacion en la imagen

                //leer los datos
                printf("\ningresa la ruta de la imagen: ");
                scanf("%s", ruta);
                imagen = fopen(ruta, "rb");


                //metodo para leer la cabecera
                dataimg = leer_imagen(ruta, &info, &cabecera);
                if (dataimg == NULL) {
                    printf("\nERROR EN EL FORMATO DE IMAGEN\n");
                } else {
                    printf("\nFORMATO SI ES VALIDO---------\n");


                    //********************************************
                    //********************************************
                    int error = 1;

                    FILE * texto;
                    char ruta_texto[30];
                    printf("\nIngresa la ruta del texto: ");
                    scanf("%s", ruta_texto);
                    texto = fopen(ruta_texto, "rb");
                    //primero se valida que el texto quepa dentro de la imagen
                    printf("\nLeyo el archivo y ahora leera el tamaño\n");
                    int size_img = info.imgsize;
                    printf("\nSi pudo leer el tamaño\n");
                    int size_texto = 0;
                    printf("\nLinea 1");
                    fseek(texto, 0L, SEEK_END);
                    printf("\nLinea 2");
                    size_texto = ftell(texto);
                    printf("\nLinea 3");
                    rewind(texto);
                    unsigned char datos_texto[size_texto];
                    printf("size_texto: %d x8: %d imagen: %d", size_texto, (size_texto * 8), size_img);
                    if ((size_texto * 8) > size_img) {
                        //        printf("\nEl tamaño del texto es muy grande para la imagen\n");
                        perror("\nEl tamaño del texto es muy grande para la imagen\n");
                        error = 0;
                    }

                    if (error == 1) {
                        //aqui se almacena el tamaño del texto que va a insertar 
                        cabecera.resv1 = size_texto;

                        //se leen los datos del archivo de texto para ser insertados en la imagen
                        fread(datos_texto, sizeof (unsigned char), size_texto, texto);
                        int x = 0;
                        int index = 0;
                        for (x = 0; x < size_texto; x++) {
                            insertar(datos_texto[x], dataimg, index);
                            index += 8;
                        }
                        fclose(texto);
                        //ahora se crea la imagen con los datos>
                        crear_imagen(dataimg, &info, &cabecera);


                    }

                }


                while (getchar() != '\n');
                break;
            case 2:

                printf("\ningresa la ruta de la imagen: ");
                scanf("%s", ruta);
                imagen = fopen(ruta, "rb");
                int size_imagen = 0;
                int size_texto = 0;

                fseek(imagen, 0L, SEEK_END);
                size_imagen = ftell(imagen);
                rewind(imagen);
                fclose(imagen);

                //metodo para leer la cabecera
                dataimg = leer_imagen(ruta, &info,&cabecera);
                if (dataimg == NULL) {
                    printf("\nERROR EN EL FORMATO DE IMAGEN\n");
                } else {
                    printf("\nFORMATO SI ES VALIDO---------\n");

                }
                //**************************************
                //**************************************
                mostrarDatosCabeceraBMP(&cabecera);
                printf("\n");
                crear_archivo(dataimg, &cabecera);
                printf("\nTermino de  leer el archivo y sacarlo");
                while (getchar() != '\n');
                break;

        }
        printf("\n");

        while (getchar() != '\n');

    } while (opcion != 3);
    //    free(dataimg);
    while (getchar() != '\n');
    return (EXIT_SUCCESS);
}
