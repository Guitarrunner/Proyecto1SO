
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define SMO_NAME "SHAREDMEM"
#define SMO_SIZE 1000000
#define MAX_LEN 800

// VARIABLES GLOBALES
//  Size buffer reconstruir imagen
int SIZE_BUFFER = 0;
// Pixeles en imagen
int PIXELS = 0;
int width, height, comp;

// Cifrado usando XOR
char *XORCipher(char *data, char *key, int dataLen, int keyLen)
{
    char *output = (char *)malloc(sizeof(char) * dataLen);

    for (int i = 0; i < dataLen; ++i)
    {
        output[i] = data[i] ^ key[i % keyLen];
    }

    return output;
}

//-----------------------------------------------------------------------------------------
// Lee lo que haya en memoria
int readBuf(int page)
{
    // AQUI SE LEE
    return 0;
}

//-----------------------------------------------------------------------------------------
int writeBuf(char *buf, int pag, unsigned char *hora)
{
    // AQUI SE ESCRIBE
    return 0;
}

// Crea la memoria compartida
void createSM()
{
    int fd = shm_open(SMO_NAME, O_CREAT | O_RDWR, 00600);
    if (fd == -1)
    {
        printf("Error en creacion de memoria");
        exit(1);
    }

    close(fd);
}

// Decodificador
char *decoder(int modo)
{
    printf("Digite la clave de decodificacion:\n");
    char *key = malloc(sizeof(char) * MAX_LEN);
    fgets(key, MAX_LEN, stdin);

    //-----------------------------------------------------------------------------------------
    // ESTE CHUNK DEBE DE VENIR DE LA LECTURA DE DATOS, 10 es placeholder, deberia de aumentar segun se lee mas
    char chunkPix[10];

    int keyLen = strlen(key);
    int dataLen = strlen(chunkPix);
    char *decoded = XORCipher(chunkPix, key, dataLen, keyLen);

    char img[SIZE_BUFFER];
    int parcial = 0;
    int count = 0;
    bool found = false;
    int space = 0;

    // Lee suficiente como haya en chunkPix o toda la imagen si esta completa, la idea es que chunk pix vaya creciendo segun se lean mas partes
    for (int i = 0; i < strlen(decoded) || space < SIZE_BUFFER; i = i + 1)
    {
        if (decoded[i] == ',' || decoded[i] == 0)
        {
            if (parcial == 48)
                parcial = 0;
            img[space] = parcial;
            space++;
            parcial = 0;
            count = 0;
            found = false;
        }
        else
        {
            if (found == false)
            {
                if (decoded[i + 1] == ',')
                {
                    parcial = decoded[i];
                    found = true;
                    continue;
                }
                if (decoded[i] == '-')
                {
                    if (decoded[i + 2] == ',')
                    {
                        parcial = -1 * decoded[i + 1];
                        found = true;
                        continue;
                    }
                    if (decoded[i + 3] == ',')
                    {
                        parcial = -1 * (decoded[i + 2] * 10 + decoded[i + 1]);
                        found = true;
                        continue;
                    }
                    if (decoded[i + 4] == ',')
                    {
                        parcial = -1 * (decoded[i + 3] * 100 + decoded[i + 2] * 10 + decoded[i + 1]);
                        found = true;
                        continue;
                    }
                }
                else
                {
                    if (decoded[i + 2] == ',')
                    {
                        parcial = decoded[i + 1] * 10 + decoded[i];
                        found = true;
                        continue;
                    }
                    if (decoded[i + 3] == ',')
                    {
                        parcial = decoded[i + 2] * 100 + decoded[i + 1] * 10 + decoded[i];
                        found = true;
                        continue;
                    }
                }
            }
        }
    }

    stbi_write_png("result.png", width, height, comp, img, width * comp);
    stbi_image_free(decoded);

    free(decoded);
}

int readPhoto(int modo, int sleepTime)
{
    printf("Digite el nombre del archivo por guardar:\n");
    char *filename = malloc(sizeof(char) * 800);
    fgets(filename, 800, stdin);
    char *data = stbi_load("monster.png", &width, &height, &comp, 0);
    if (data)
    {
        createSM();
        const long pixels = width * height;
        int len = pixels * comp;
        PIXELS = len;
        char *key = "secret";
        int keyLen = strlen(key);
        int contador = 0;
        for (int i = 0; i < (len); i = i + 1)
        {
            if (data[i] < 0)
                contador = contador + 1;
            if (fabs(data[i]) >= 100)
                contador = contador + 1;
            if (fabs(data[i]) >= 10)
                contador = contador + 1;
            if (fabs(data[i]) >= 0)
                contador = contador + 2;
        }

        SIZE_BUFFER = contador;
        int pag = 0;
        for (int i = 0; i < (len + 10); i = i + 10)
        {
            if(modo==1)sleep(sleepTime);
            if(modo==2)getc(stdin);;
            char chunkPix[50] = "";
            char s1[3];
            for (int j = 0; j < 10; j++)
            {
                // Une 10 pixeles por chunk
                if (i + j < (len + len % 10))
                {
                    sprintf(s1, "%d,", data[i + j]);
                    strcat(chunkPix, s1);
                }
                else
                {
                    continue;
                }
            }

            // Agrega la pagina al chunk
            pag = pag + 1;
            // Adding time
            unsigned char *time = ("%s", __TIMESTAMP__);

            int dataLen = strlen(chunkPix);
            char *chunkCodificado = XORCipher(chunkPix, key, dataLen, keyLen);

            //-----------------------------------------------------------------------------------------
            // writeBuf(chunkCodificado, pag, hora);
            printf("Pagina: %d, archivo codificado %s\n", pag, chunkCodificado);
        }
        free(data);
    }

    return 0;
}
int main()
{
    readPhoto(1, 1);
    readPhoto(2, 0);

    return 0;
}
