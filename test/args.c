#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc < 2){
        printf("Provide some command-line arguments\n");
    } else {
        int i;
        printf("------------------------------------------------\n");
        for (i = 0; i < argc; ++i) {
            printf("Argument No %d: %s\n", i, argv[i]);
        }
        printf("------------------------------------------------\n");
    }
}