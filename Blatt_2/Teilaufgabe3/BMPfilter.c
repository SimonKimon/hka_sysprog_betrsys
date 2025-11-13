#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <math.h>

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

// -----------------------------------------------------------
// vordefinierte Filter (3x3-Kernel)
const int smooth[3][3] = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1}
};
const int sharp[3][3] = {
    {0, -1, 0},
    {-1, 5, -1},
    {0, -1, 0}
};
const int edge[3][3] = {
    {0, 1, 0},
    {1, -4, 1},
    {0, 1, 0}
};
const int emboss[3][3] = {
    {2, 1, 0},
    {1, 1, -1},
    {0, -1, -2}
};

// -----------------------------------------------------------
// Hilfsfunktion: wähle Kernel
const int (*selectKernel(const char *name))[3] {
    if (strcmp(name, "smooth") == 0) return smooth;
    if (strcmp(name, "sharp")  == 0) return sharp;
    if (strcmp(name, "edge")   == 0) return edge;
    if (strcmp(name, "emboss") == 0) return emboss;
    fprintf(stderr, "Error: Unknown filter '%s'\n", name);
    exit(EXIT_FAILURE);
}

// -----------------------------------------------------------
// clamp-Funktion, damit RGB-Werte 0–255 bleiben
static inline uint8_t clamp(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (uint8_t)value;
}

// -----------------------------------------------------------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.bmp> <filter>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *inputFile = argv[1];
    const int (*kernel)[3] = selectKernel(argv[2]);

    // BMP-Datei öffnen
    int fd = open(inputFile, O_RDONLY);
    if (fd < 0) { perror("open"); exit(EXIT_FAILURE); }

    // Header lesen
    bmpHeader header;
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        perror("read header");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Validierungen
    if (header.signature != 0x4D42) {
        fprintf(stderr, "Error: Not a BMP file.\n");
        exit(EXIT_FAILURE);
    }
    if (header.bitCount != 24 || header.compression != 0) {
        fprintf(stderr, "Error: Only uncompressed 24-bit BMPs supported.\n");
        exit(EXIT_FAILURE);
    }
    if (header.width < 3 || header.height < 3) {
        fprintf(stderr, "Error: Image too small (<3x3)\n");
        exit(EXIT_FAILURE);
    }

    // Dateigröße prüfen
    off_t fileSize = lseek(fd, 0, SEEK_END);
    lseek(fd, header.offset, SEEK_SET);
    size_t dataSize = fileSize - header.offset;

    // Speicher für Bilddaten anlegen
    uint8_t *data = malloc(dataSize);
    if (!data) { perror("malloc"); exit(EXIT_FAILURE); }

    if (read(fd, data, dataSize) != dataSize) {
        perror("read image data");
        free(data);
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    int width = header.width;
    int height = header.height;
    int newWidth = width - 2;
    int newHeight = height - 2;

    // Neues Bild nach Filter
    uint8_t *newData = malloc(newWidth * newHeight * 3);
    if (!newData) { perror("malloc newData"); exit(EXIT_FAILURE); }

    // Faltung anwenden
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumB = 0, sumG = 0, sumR = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int px = x + kx;
                    int py = y + ky;

                    int index = (py * width + px) * 3;
                    uint8_t b = data[index + 0];
                    uint8_t g = data[index + 1];
                    uint8_t r = data[index + 2];

                    int kVal = kernel[ky + 1][kx + 1];
                    sumB += b * kVal;
                    sumG += g * kVal;
                    sumR += r * kVal;
                }
            }

            int idxOut = ((y - 1) * newWidth + (x - 1)) * 3;
            newData[idxOut + 0] = clamp(sumB);
            newData[idxOut + 1] = clamp(sumG);
            newData[idxOut + 2] = clamp(sumR);
        }
    }

    // Neuen Header anpassen
    bmpHeader newHeader = header;
    newHeader.width  = newWidth;
    newHeader.height = newHeight;
    newHeader.imageSize = newWidth * newHeight * 3;
    newHeader.size = newHeader.imageSize + header.offset;

    // Neue Datei schreiben
    int outFd = open("output.bmp", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (outFd < 0) { perror("open output"); exit(EXIT_FAILURE); }

    write(outFd, &newHeader, sizeof(newHeader));
    lseek(outFd, newHeader.offset, SEEK_SET);
    write(outFd, newData, newHeader.imageSize);

    close(outFd);
    free(data);
    free(newData);

    printf("Filter '%s' applied successfully! Output: output.bmp\n", argv[2]);
    return 0;
}

//ChatGPT's Werk funktioniert aber auch nicht genau.