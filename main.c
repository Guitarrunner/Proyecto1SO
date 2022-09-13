#include <stdio.h>
 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
 
 char* XORCipher(char* data, char* key, int dataLen, int keyLen) {
	char* output = (char*)malloc(sizeof(char) * dataLen);

	for (int i = 0; i < dataLen; ++i) {
		output[i] = data[i] ^ key[i % keyLen];
	}

	return output;
}

char* encoder(char* codePart,char* key){
    char* response = XORCipher(codePart,key,strlen(codePart),strlen(key));
    return response;
}

int main(void) {
    // Image's variables
    int width, height, comp;
    unsigned char *data = stbi_load("Josu.jpg", &width, &height, &comp, 0);
    const long pixels = width*height;
    if (data) {

        printf("width = %d, height = %d, comp = %d (channels)\n", width, height, comp);
        // Private key for crypt
        char* key = "secret";
        int keyLen = strlen(key);
        //Index
        int page = 0;
        //Stop condition
        int len = pixels*comp;

        //Vars for encrypt
        char chunkPix[22]="";
        char s2[2];
        char s1[2];
        for (int i = 0; i < len; i=i+10) {
            //Bug with this for
            // for (int j = 0; j < 10; j++){
            //     //Joins 10 pixels in one chunk
            //     if(i+j<len){
            //         sprintf(s1, "%d", data[i+j]);
            //         strcat(chunkPix, s1);
            //     }
                
            // }
            //Adding page
            sprintf(s2, "%d", page);
            strcat(chunkPix, s2);
            unsigned char* time = ("%s",__TIMESTAMP__);
            int size = 22 + sizeof(time);
            char final[size];
            //Adding time
            strcat(final,chunkPix);
            strcat(final,time);
            int dataLen = strlen(final);
            char* lastCoded = XORCipher(final,key,dataLen,keyLen);

            printf("%s\n",final);
            printf("%d\n",i);
            page++;
        }

    }
    return 0;
}

