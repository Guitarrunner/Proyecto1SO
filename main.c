
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

#define SMO_NAME "SHAREDMEM"
#define SMO_SIZE 1000000
int offset = 0;

char *XORCipher(char *data, char *key, int dataLen, int keyLen)
{
    char *output = (char *)malloc(sizeof(char) * dataLen);

    for (int i = 0; i < dataLen; ++i)
    {
        output[i] = data[i] ^ key[i % keyLen];
    }

    return output;
}

char *encoder(char *codePart, char *key)
{
    char *response = XORCipher(codePart, key, strlen(codePart), strlen(key));
    return response;
}

int readBuf()
{
    char *ptr;
    struct stat smo;

    int fd = shm_open(SMO_NAME, O_RDONLY, 00400);
    if (fd == -1)
    {
        printf("Error en abrir memoria %s\n", strerror(errno));
        exit(1);
    }

    if (fstat(fd, &smo) == -1)
    {
        printf(" Error en busqueda de puntero \n");
        exit(1);
    }

    ptr = mmap(NULL, smo.st_size, PROT_READ, MAP_SHARED, fd, 0);
    offset = offset + smo.st_size;
    if (ptr == MAP_FAILED)
    {
        printf("Error en lectura: %s\n", strerror(errno));
        exit(1);
    }

    printf("%s \n", ptr);

    // // Private key for crypt
    // char *key = "secret";
    // int keyLen = strlen(key);
    // int dataLen = strlen(ptr);
    // char *lastCoded = XORCipher(ptr, key, dataLen, keyLen);
    
    // printf("%s \n", lastCoded);

    close(fd);
}

int writeBuf(char *buf)
{
    int fd = shm_open(SMO_NAME, O_RDWR, 0);

    if (fd == -1)
    {
        printf("Error en abrir de memoria");
        exit(1);
    }

    char *ptr;

    ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {   
        printf("Error en escritura: %s\n", strerror(errno));
        exit(1);
    }

    memcpy(ptr, &buf, sizeof(buf));
    printf("%d \n", (int)sizeof(buf));

    close(fd);
    return 0;
}

void createSM(int SIZE)
{
    int fd = shm_open(SMO_NAME, O_CREAT | O_RDWR, 00600);
    if (fd == -1)
    {
        printf("Error en creacion de memoria");
        exit(1);
    }

    if (ftruncate(fd, SIZE) == -1)
    {
        printf("Error en asignacion de memoria");
        exit(1);
    }
    close(fd);
}

int readPhoto(void)
{
    // Image's variables
    int width, height, comp;
    unsigned char *data = stbi_load("test.jpg", &width, &height, &comp, 0);
    const long pixels = width * height;

    createSM(pixels);
    if (data)
    {

        printf("width = %d, height = %d, comp = %d (channels)\n", width, height, comp);
        // Private key for crypt
        char *key = "secret";
        int keyLen = strlen(key);
        // Index
        int page = 0;
        // Stop condition
        int len = pixels * comp;

        // Vars for encrypt
        char chunkPix[22] = "";
        char s2[2];
        char s1[2];
        for (int i = 0; i < len; i = i + 10)
        {
            // Bug with this for
            //  for (int j = 0; j < 10; j++){
            //      //Joins 10 pixels in one chunk
            //      if(i+j<len){
            //          sprintf(s1, "%d", data[i+j]);
            //          strcat(chunkPix, s1);
            //      }

            // }
            // Adding page
            sprintf(s2, "%d", page);
            strcat(chunkPix, s2);
            unsigned char *time = ("%s", __TIMESTAMP__);
            int size = 22 + sizeof(time);
            char final[size];
            // Adding time
            strcat(final, chunkPix);
            strcat(final, time);
            int dataLen = strlen(final);
            char *lastCoded = XORCipher(final, key, dataLen, keyLen);

            writeBuf(lastCoded);

            printf("%s\n", lastCoded);
            printf("%d\n", i);
            page++;
        }
        readBuf();
    }
    return 0;
}



int main()
{

    readPhoto();
    return 0;
}
