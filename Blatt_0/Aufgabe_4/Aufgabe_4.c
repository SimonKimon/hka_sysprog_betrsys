#include <stdio.h>

int main() {
    char* str;

    printf("Enter a string: ");
    scanf(" %[^\n]", str);

    while (*str != '\0') {
        if (*str == ' ') {
            str++;
            printf("\n");
        }
        printf("%c", *str);
        str++;
    }
    return 0;
}
