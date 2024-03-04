// dictionary.c
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *allocate_and_copy_word(const char *word) {
    char *new_word = (char *)malloc(strlen(word) + 1);  // +1 for the null terminator
    if (new_word != NULL) {
        strcpy(new_word, word);
    }
    return new_word;
}

char **read_dictionary(const char *path, int *word_count) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Can't open dictionary\n");
        return NULL;
    }

    char **words = (char **)malloc(INITIAL_ARRAY_SIZE * sizeof(char *));
    if (words == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    char buffer[MAX_WORD_LENGTH];
    int count = 0;

    while (fgets(buffer, MAX_WORD_LENGTH, file) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        words[count] = allocate_and_copy_word(buffer);
        if (words[count] == NULL) {
            fprintf(stderr, "Memory allocation for word failed\n");
            fclose(file);
            for (int i = 0; i < count; i++) {
                free(words[i]);
            }
            free(words);
            return NULL;
        }
        count++;
    }

    fclose(file);
    *word_count = count;
    return words;
}

// test push