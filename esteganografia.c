/*
 * File:   esteganografia.c
 * Author: Paul y Cindy
 *
 * Created on November 8, 2016, 3:45 AM
 */

#include <stdio.h> //Libreria para el manejo de archivos
#include <stdlib.h> //Libreria estandar
#include <stdint.h> //Libreria para el manejo de tipos de datos con valores de bits definidos

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
//se declara en este punto el metodo porque se usa antes de definirce el metodo
unsigned char extractor();

unsigned char * leer_imagen(char * ruta_imagen, InfoCabeceraBMP * infoCabeceraBmp, CabeceraBMP * cabecera) {
    printf("\nEntro a leer la imagen para devolver los datos");
    FILE * imagen;

    unsigned char *imgdata; //datos de imagen
    uint16_t type; // 2 bytes indentificativos del formato


    imagen = fopen(ruta_imagen, "r");
    if (!imagen) return NULL; // si no se puede leer no hay imagen

    //se leeen los dos primeros bytes para la identificacion
    fread(&type, sizeof (uint16_t), 1, imagen);

    if (type != 0x4D42) {
        //Hexadecimal:  0x4D42  = caracteres: BM
        //aqui se comprueba el formato sino no es BMP retorna NULL
        fclose(imagen);
        return NULL;
    }
    //ahora se procede a leer la cabecera del archivo completa

    fread(cabecera, sizeof (CabeceraBMP), 1, imagen);
//        printf("\nData cabecera: \n");
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

    //ahora se le los datos de la imagen tantos bytes como imgsize
    fread(imgdata, infoCabeceraBmp->imgsize, 1, imagen);

    //se cierra el archivo de la imagen
    fclose(imagen);

    //se retorna los datos de colores de la imagen
    return imgdata;

}

int menu() {
    system("clear"); // para linux
//    system("cls"); // para windows
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

char * path_imagen(char * ruta, int size, char * path){
    int x = 0;
    int index = 0;
    while(x<size){
        if(ruta[x]=='\\'){
            index = x;
        }
        if(ruta[x]=='p'&&ruta[x-1]=='m'&&ruta[x-2]=='b'&&ruta[x-3]=='.'){
            break;
        }
        x++;
    }

    for(x=0;x<=index;x++){
        path[x] = ruta[x];
    }
    path[index+1]='\0';


}

void crear_imagen(unsigned char * dataimg, InfoCabeceraBMP * infoCabeceraBmp, CabeceraBMP * cabecera) {
    FILE * imgSalida;
    //aqui se abre el archivo con tipo de apertura W para sobre escribirse si existe
    imgSalida = fopen("Salida.bmp", "w");
    //luego se cierra
    fclose(imgSalida);
    //se abre nuevamente para asegurar la escritura de forma binaria
    imgSalida = fopen("Salida.bmp", "rb+");
    printf("\nentro a crear la imagen\n");
//    uint16_t type;
//    type = 0x4D42;
    //Formato de la cabecera de BMP en caracteres
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
    int x=0;
    /* se recorre el puntero de dataimg para ir escribiendo caracter por caracter
     * e insertar estos valores en la imagen
     */
    for(x=0;x<infoCabeceraBmp->imgsize;x++){
     fputc(dataimg[x],imgSalida);
    }

//    fwrite(dataimg, sizeof (unsigned char*), infoCabeceraBmp->imgsize, imgSalida);

    printf("\nEscribio los datos de la imagen\n");
    //aqui se cierra el archivo de la imagen
    fclose(imgSalida);
    printf("\nAcabo de terminar de escribir la imagen\n");
}

void crear_archivo(unsigned char * imgdata, CabeceraBMP * cabecera) {
    FILE * texto_datos;
    //se abre para asegurar la sobreescritura si existe
    texto_datos = fopen("texto.txt", "w");
    //se cierra el archivo con ese formato de apertura
    fclose(texto_datos);
    //se abre nuevamente para asegurar escritura de forma binaria

    texto_datos = fopen("texto.txt", "rb+");
    //de la cabecera saco el valor de datos a leer
    int cantidad_datos = cabecera->resv1; //valor de bytes a leer
    //se crea un arreglo con el tamaño de los bytes a leer
    unsigned char texto[cantidad_datos];
    int x = 0;
    int index = 0;
    //obtencion de datos con el metodo extractor el cual lee los bytes
    //de la imagen y usa el bit menos significativo para contruir el byte
    // del archivo a extraer
    for (x = 0; x < cantidad_datos; x++) {
        texto[x] = extractor(imgdata, index);
        index += 8;
    }

    //ahora que se leyeron los datos vamos a escribirlos en el archivo
    fwrite(texto, sizeof (unsigned char), cantidad_datos, texto_datos);
    //se cierra el archivo de salida
    fclose(texto_datos);
    //este metodo debe de recibir los datos de la imagen y poder obtener la info

}

void insertar(unsigned char insertar,unsigned char * valores, int index) {
    //este metodo recibe un caracter a insertar, un arreglo de caracteres
    //los cuales se van a alterar y un indice por donde se debe recorrer
    //el arreglo de valores
    int x = 0;
    //este comparador se usar para hacer un (Y) logico
    //el valor de este comparador es:
    //1000 0000 un byte que equuivale a -128 o 128 sin signo
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

int main(int argc, char** argv) {
    int opcion;
    int error = 0;
    FILE * imagen;
    InfoCabeceraBMP info;
    CabeceraBMP cabecera;
    unsigned char * dataimg;
    char ruta[100];
    char path[100];
    do {

        opcion = menu();
        switch (opcion) {
            case 1:
                //para obligar a colocar la variable en su valor original
                error =0;
                //ocultar informacion en la imagen

                //leer los datos
                printf("\ningresa la ruta de la imagen: ");
                scanf("%s", ruta);
                printf("\nEl path de la imagen es: \n");
                path_imagen(ruta,100, path);
                printf("%s\n",path);
                imagen = fopen(ruta, "rb");


                //metodo para leer la cabecera
                dataimg = leer_imagen(ruta, &info, &cabecera);
                if (dataimg == NULL) {
                    printf("\nERROR EN EL FORMATO DE IMAGEN\n");
                } else {
                    printf("\nFORMATO SI ES VALIDO---------\n");


                    //********************************************
                    //********************************************

                    FILE * texto;
                    char ruta_texto[30];
                    while(error==0){
                        printf("\nIngresa la ruta del texto: ");
                        scanf("%s", ruta_texto);
                        texto = fopen(ruta_texto, "rb");
                        if(texto==NULL){
                            printf("\nError archivo no encontrado o archivo no valido");
                            //para liberar el buffer del teclado
                            fflush(stdin);
                        }else{
                            error = 1;
                        }
                    }

                    //primero se valida que el texto quepa dentro de la imagen
                    printf("\nLeyo el archivo y ahora leera el tamaño\n");
                    int size_img = info.imgsize;
                    int size_texto = 0;
                    fseek(texto, 0L, SEEK_END);
                    size_texto = ftell(texto);
                    rewind(texto);
                    unsigned char datos_texto[size_texto];
                    printf("\nsize_texto: %d x8: %d imagen: %d", size_texto, (size_texto * 8), size_img);
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
                //para ayudar a colocar la variable en su valor original
                error = 0;
                while(error == 0){
                    printf("\ningresa la ruta de la imagen: ");
                    scanf("%s", ruta);
                    imagen = fopen(ruta, "rb");

                    if(imagen == NULL){
                        printf("\nError imagen no valida o archivo no encontrado");
                        fflush(stdin);
                    }else{
                        error = 1;
                    }
                }
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
                    error = 2;
                }
                //**************************************
                //**************************************
                if(error==2){
                    mostrarDatosCabeceraBMP(&cabecera);
                    printf("\n");
                    crear_archivo(dataimg, &cabecera);
                    printf("\nTermino de  leer el archivo y sacarlo");
                }
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
