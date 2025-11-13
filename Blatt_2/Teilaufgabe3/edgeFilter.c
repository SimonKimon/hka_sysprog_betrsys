#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
//------------------------------------------------------------------------------------------------
#define BMP_HEADER_SIZE 54

#pragma pack(push, 1)
typedef struct {
    uint16_t signature;
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;

    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPelsPerMeter;
    int32_t yPelsPerMeter;
    uint32_t clrUsed;
    uint32_t clrImportant;
} bmpHeader;
#pragma pack(pop)

int main() {
    bmpHeader header;

    //open data
    int fd = open("A.bmp", O_RDWR);

    //check if data was found
    if (fd < 0) {
        perror("Error: Failed to open file");
        exit(EXIT_FAILURE);
    }

    //read data
    ssize_t bytesRead = read(fd, &header, sizeof(bmpHeader));

    //check if header is 54 bytes
    if (bytesRead < sizeof(bmpHeader)) {
        fprintf(stderr, "Error: Not enough bytes read (%zd)", bytesRead);
        close(fd);
        exit(EXIT_FAILURE);
    }

    //look for signature in BMP data
    if (header.signature != 0x4D42) {
        fprintf(stderr, "Error: No BMP signature found (read: 0x%X)", header.signature);
        close(fd);
        exit(EXIT_FAILURE);
    }

    //check for 24bits/pixel
    if (header.bitCount != 24) {
        fprintf(stderr, "Error: Not 24 bit/pixel (read: %d)", header.bitCount);
        close(fd);
    }

    //check if no compression
    if (header.compression != 0) {
        fprintf(stderr, "Error: Compression used (read: %d)", header.compression);
        close(fd);
    }

    //check if no colorUsed
    if (header.clrUsed != 0) {
        fprintf(stderr, "Error: Color used (read: %d)", header.clrUsed);
        close(fd);
    }

    //check if height and width is more than 3 pixel
    if (header.height < 3 || header.width < 3) {
        fprintf(stderr, "Error: Image too small (read: %dx%d)", header.width, header.height);
        close(fd);
    }



    //create 3D-Array
    uint8_t pixelData[header.width][header.height][3];

    //set offset to start at pixel data
    off_t pos = header.offset;
    lseek(fd, pos, SEEK_SET);

    //read pixelData and safe it in Array pixelData[][][]
    for (int j = 0; j < header.height; ++j) {
        for (int i = 0; i < header.width; ++i) {
            // lese 3 Byte = 1 Pixel (B, G, R)
            if (read(fd, pixelData[j][i], 3) != 3) {
                fprintf(stderr, "Error reading pixel (%d,%d)\n", i, j);
                close(fd);
                exit(EXIT_FAILURE);
            }
            printf("b:%d,g:%d,r:%d | ",pixelData[i][j][0],pixelData[i][j][1],pixelData[i][j][2]);
        }
        printf("\n");
    }

    //read BMP file closed
    close(fd);

    //define filter matrix
    int8_t filterMatrix[3][3] = {
        { 0,-1, 0},
        {-1, 5,-1},
        { 0,-1, 0}
    };

    //calculate matrix with pixel data
    for (int j = 1; j < header.height - 1; ++j) {
        for (int i = 1; i < header.width - 1; ++i) {
            for (int p = -1; p <= 1; ++p) {
                for (int q = -1; q <= 1; ++q) {
                    pixelData[i - p][j - q][0] *= filterMatrix[p][q];
                    pixelData[i - p][j - q][1] *= filterMatrix[p][q];
                    pixelData[i - p][j - q][2] *= filterMatrix[p][q];
                }
            }
            //printf("b:%d,g:%d,r:%d | ",pixelData[i][j][0],pixelData[i][j][1],pixelData[i][j][2]);
        }
        printf("\n");
    }

    //????????????????????????????????????????????????????????????????????????????????????????????

}