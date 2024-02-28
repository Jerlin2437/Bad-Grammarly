
#ifndef DICTIONARY_H
#define DICTIONARY_H

#define MAX_WORD_LENGTH 100
#define INITIAL_ARRAY_SIZE 104335 // Adjust as necessary

char *allocate_and_copy_word(const char *word);
char **read_dictionary(const char *path, int *word_count);

#endif
