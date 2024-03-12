#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef DEBUG
#define DEBUG 0
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: Missing a Dictionary\n");
        return 1;
    }

    int word_count = 0;

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    char **dictionary = read_dictionary(fd, &word_count);

    if (DEBUG) {
        printf("Total words read: %d\n", word_count);
        for (int i = 0; i < word_count && i < word_count; i++) {
            printf("%s\n", dictionary[i]);
        }
    }

    int result = binary_search(fd, dictionary, "APPLE");
    if (result >= 0) {
        printf("FOUND\n");
    } else {
        printf("FALSE\n");
    }

    for (int i = 0; i < word_count; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
    close(fd);
    return 0;
}
