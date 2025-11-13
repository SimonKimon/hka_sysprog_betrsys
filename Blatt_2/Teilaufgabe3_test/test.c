#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
//------------------------------------------------------------------------------------------------
#define BUFFER_SIZE 256

void readPrintData() {
    int openData = open("test.txt", O_RDWR);
    if (openData < 0) {
        if (errno == ENOENT) {
            printf("\nError: File does not exist, Code: %d", errno);
        } else {
            printf("\nError: Failed to open file, Code: %d", errno);
        }
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(openData, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    if (bytes_read < 0) {
        perror("Error: Failed to read file");
        close(openData);
        exit(EXIT_FAILURE);
    }
    close(openData);
}
//------------------------------------------------------------------------------------------------
typedef struct {
    int id;
    char name[100];
    float salary;
} Employee;

void writeData() {
    Employee emp1, emp2;
    emp1.id = 1;
    strcpy(emp1.name, "Vallah Habibi");
    emp1.salary = 50000.0;

    //Create and open file
    int fd = open("employee.bin", O_CREAT | O_RDWR, 0644);

    //Write the struct to the file
    write(fd, &emp1, sizeof(Employee));

    //Use lseek to move the file position back to the start
    lseek(fd, 0, SEEK_SET);

    //Read the struct from the file
    read(fd, &emp2, sizeof(Employee));

    close(fd);

    //Print the data read from the file
    printf("ID: %d\nName: %s\nSalary: %f\n", emp2.id, emp2.name, emp2.salary);
}
//------------------------------------------------------------------------------------------------
int main() {
    //readPrintData();
    writeData();
}
