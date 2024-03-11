// dictionary.c
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static char **dict_array;

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH
#define BUFLENGTH 16
#endif

char *allocate_and_copy_word(const char *word) {
    char *new_word = (char *)malloc(strlen(word) + 1);  // +1 for the null terminator
    if (new_word != NULL) {
        strcpy(new_word, word);
    }
    return new_word;
}

int find_dictlength(int fd, char *path) {
    int result = 0;
    int buflength = BUFLENGTH;
	char *buf = malloc(BUFLENGTH);
	int pos = 0;
	int bytes;
	int line_start;
	while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
		line_start = 0;
		int bufend = pos + bytes;
		while (pos < bufend) {
			if (buf[pos] == '\n') {
				// we found a line, starting at line_start ending before pos
				buf[pos] = '\0';
				// use_line(arg, buf + line_start);
				line_start = pos + 1;
                result += 1;
			}
			pos++;
		}
		// no partial line
		if (line_start == pos) {
			pos = 0;
			// partial line
			// move segment to start of buffer and refill remaining buffer
		} else if (line_start > 0) {
			int segment_length = pos - line_start;
			memmove(buf, buf + line_start, segment_length);
			pos = segment_length;
			if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
			// partial line filling entire buffer
		} else if (bufend == buflength) {
			buflength *= 2;
			buf = realloc(buf, buflength);
			// if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
		}
	}
	// partial line in buffer after EOF
	if (pos > 0) {
		if (pos == buflength) {
			buf = realloc(buf, buflength + 1);
		}
		buf[pos] = '\0';
		// use_line(arg, buf + line_start);
	}
	free(buf);
}

char **read_dictionary(const char *path, int *word_count) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror(path);
        exit(EXIT_FAILURE);
    }

    // char **words = (char **)malloc(INITIAL_ARRAY_SIZE * sizeof(char *));
	int numlines = find_dictlength(fd, path);
	dict_array = (char **) malloc(numlines * sizeof(char *));
    if (dict_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        // fclose(file);
        return NULL;
    }

	qsort(dict_array);

    char buffer[MAX_WORD_LENGTH];
    int count = 0;

    // while (fgets(buffer, MAX_WORD_LENGTH, file) != NULL) {
    //     buffer[strcspn(buffer, "\n")] = 0;
    //     words[count] = allocate_and_copy_word(buffer);
    //     if (words[count] == NULL) {
    //         fprintf(stderr, "Memory allocation for word failed\n");
    //         fclose(file);
    //         for (int i = 0; i < count; i++) {
    //             free(words[i]);
    //         }
    //         free(words);
    //         return NULL;
    //     }
    //     count++;
    // }

    // fclose(file);
    *word_count = count;
    return words;
}

// test push