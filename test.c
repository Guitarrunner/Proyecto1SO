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
//Aqui se puede probar la codificacion y decodificacion
char *XORCipher(char *data, char *key, int dataLen, int keyLen)
{
    char *output = (char *)malloc(sizeof(char) * dataLen);

    for (int i = 0; i < dataLen; ++i)
    {
        output[i] = data[i] ^ key[i % keyLen];
    }

    return output;
}

int main(){
    int width, height, comp;
    printf("Digite el nombre del archivo por guardar:\n");
    char *filename = malloc(sizeof(char) * 800);
    fgets(filename, 800, stdin);
    char *data = stbi_load(filename, &width, &height, &comp, 0);
    const long pixels = width * height;
    int len = pixels * comp;

    int counter = 0;
    for (int i = 0; i < (len); i = i + 1)
    {
        if (data[i] < 0)
            counter = counter + 1;
        if (fabs(data[i]) >= 100)
            counter = counter + 1;
        if (fabs(data[i]) >= 10)
            counter = counter + 1;
        if (fabs(data[i]) >= 0)
            counter = counter + 2;
    }

    char chunkPix[counter];
    char s1[3];
    for (int i = 0; i < (len); i = i + 1)
    {

        sprintf(s1, "%d,", data[i]);
        strcat(chunkPix, s1);
    }
    char *key = "secret";
    int keyLen = strlen(key);
    int dataLen = strlen(chunkPix);
    char *lastCoded = XORCipher(chunkPix, key, dataLen, keyLen);


    int dataLen2 = strlen(lastCoded);
    char *decoded = XORCipher(lastCoded, key, dataLen2, keyLen);
    char img[len];
    int parcial = 0;
    int count = 0;
    bool found = false;
    int space = 0;
    for (int i = 0; i < strlen(decoded) || space<len; i = i + 1)
    {   
        if (decoded[i] == ',' || decoded[i]==0)
        {
            if (parcial==48)parcial=0;
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
    free(lastCoded);
    free(data);

    exit(1);
    return 0;
}