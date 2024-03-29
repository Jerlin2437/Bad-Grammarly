#ifndef DICTIONARY_H
#define DICTIONARY_H
#endif

#define MAX_WORD_LENGTH 100
#define INITIAL_ARRAY_SIZE 104335 // Adjust as necessary

extern int wordCount;
extern char **dict_array;
char **read_dictionary(int, int *);
int find_dictlength(int);
int binary_search(int, char **, char *);
int linear_search(int, char**, char*);