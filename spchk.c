#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 100
#define INITIAL_ARRAY_SIZE 104335 //104335 words in /usr/share/dict/words dictionary
/*
This implementation of dictionary reading involves mallocing an array of size 104335
 This is the max num of words of the dictionary file in /usr/share/dict/words
 I am not sure if we have to use that as our dictionary but its there
 Right as of 2/27, this dictionary reading has been completed. Have not extensively tested besides just printing it all out
The implementaiton mallocs every NEW WORD read and sets buffer size to MAX_WORD_LENGTH
 This is stored in the dictionary malloced array - char** dictionary
 Yes, this was like half chatGPTed
 */
static char dict[INITIAL_ARRAY_SIZE][MAX_WORD_LENGTH];

char *allocate_and_copy_word(const char *word) {
    char *new_word = (char *)malloc(strlen(word) + 1); // +1 for the null terminator
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
        free(words);
        fclose(file);
        return NULL;
    }
    char buffer[MAX_WORD_LENGTH];
    int count = 0;
    int capacity = INITIAL_ARRAY_SIZE;

    while (fgets(buffer, MAX_WORD_LENGTH, file) != NULL) {
        // Remove newline character
        buffer[strcspn(buffer, "\n")] = 0;

        if (count >= capacity) {
            /*
             * This resizes array. This will likely be a performance hit but essentially makes our dictionary an arraylist
             */
            capacity *= 2;
            char ** words = (char **)realloc(words, capacity * sizeof(char *));
            if (words == NULL) {
                fprintf(stderr, "Memory reallocation failed\n");
                fclose(file);
                return NULL;
            }
        }

        words[count] = allocate_and_copy_word(buffer);
        if (words[count] == NULL) {
            fprintf(stderr, "Memory allocation for word failed\n");
            fclose(file);
            return NULL;
        }

        count++;
    }

    fclose(file);
    *word_count = count;
    return words;
}
int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Error: Missing a Dictionary\n");
        return 1;
    }
    int word_count = 0;
    char **dictionary = read_dictionary(argv[1], &word_count);

    //prints first 10 words
    printf("Total words read: %d\n", word_count);
    for (int i = 0; i < word_count && i < 10; i++) {
        printf("%s\n", dictionary[i]);
    }

    for (int i = 0; i < word_count; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
    return 0;
}
