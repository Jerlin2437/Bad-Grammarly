#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 16
#endif

#define FALSE 0
#define TRUE 1

static int row_pos;
static int col_pos;

// this function will be to take the inputted word and test whether or not it is in the dictionary or not
// return val: int for true or false
int validate_word(char *word) {

}

void parse_line(char *path, char *line) {
    // printf("row |%d| --> %s\n",row_pos, line);

    // edge case: whitespace before '\0' <-- null terminator (these two kind of go hand in hand)
    // edge case: line starts with white space and line is only whitespace
    // edge case: lots of white space between words

    // printf("col_pos: %d\n", col_pos);

    int offset = 0;
    char *word;
    char *ptr = line;
    char *prev = ptr;
    while (*ptr != '\0') {
        // printf("col_pos (inside loop): %d\n", col_pos);
        if (isspace(*ptr)) { // if we found a whitespace character take the found word and validate it
            // NOTE: will need to work something in here where it deals with more than 1 whitespace
            // character after finding a word, something like:
            // if (isspace(*(ptr + 1)) && *ptr != '\0') {
            //     while (isspace(*ptr)) {
            //         ptr += 1;
            //         col_pos += 1;
            //     }
            //     prev = ptr;
            // } else {
            //     ptr += 1;
            //     prev = ptr;
            //     col_pos += 1;
            // }
            // if (*ptr == '\0') {
            //     return; // or break maybe ??? idk yet
            // }

            int word_size = col_pos - offset;
            offset = col_pos;
            // printf("word_size: %d\n", word_size);
            // printf("offset: %d\n", offset);
            word = (char *) malloc(sizeof(char) * word_size);

            // i think either strncpy or memcpy works here
            strncpy(word, prev, word_size);
            // memcpy(word, prev, col_pos);
            word[word_size-1] = '\0';
            printf("%s\n", word);
            // for (int i = 0; i < 7; i++) {
            //     printf("%c", word[i]);
            // }
            // int result = validate_word(word); /////////////////// this line is where we'll pass the word to validate if its in the dict.
            free(word);
            // return;
            // printf("col_pos (inside if): %d\n", col_pos);

            // NOTE: might need to work samething  here where it deals with more than 1 whitespace
            // character after finding a word, something like (but above might already take care of that)
            ptr += 1;
            prev = ptr;
            col_pos += 1;
        }
        // if (*ptr == '\0') {

        // }
        ptr     += 1;
        col_pos += 1;
        // need to move prev pointer (or maybe pointers) correctly here
    }

    // prints the last word
    if (*ptr == '\0') {
        int word_size = col_pos - offset;
        offset = col_pos;
        word = (char *) malloc(sizeof(char) * word_size);
        strncpy(word, prev, word_size);
        word[word_size-1] = '\0';
        printf("%s\n", word);
        free(word);
    }
    
}

void parse_file(char *path) {

    // printf("from parse file: %s\n", path);

    row_pos = 1;
    col_pos = 1;

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror(path);
        exit(EXIT_FAILURE);
    }

    int buflength = BUFSIZE;
    char *buf = malloc(buflength);
    if (buf == NULL) {
        fprintf(stderr, "Error: Memory allocation failure.\n");
        exit(EXIT_FAILURE);
    }

    int pos = 0;
    int bytes;
    int line_start;

    while ((bytes = read(fd, buf + pos, buflength - pos)) > 0) {
        line_start = 0;
        int bufend = pos + bytes;
        while (pos < bufend) {
            if (buf[pos] == '\n') {
                // we found a line
                buf[pos] = '\0';
                parse_line(path, buf + line_start);
                row_pos += 1;
                line_start = pos + 1;
            }
            pos += 1;
        }
        // no partial line
		if (line_start == pos) {
			pos = 0;
			// printf("no partial line\n");
			// partial line
			// move segment to start of buffer and refill remaining buffer
		} else if (line_start > 0) {
			int segment_length = pos - line_start;
			memmove(buf, buf + line_start, segment_length);
			pos = segment_length;
			// if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
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
        parse_line(path, buf + line_start);
        row_pos += 1;
    }

    close(fd);
    free(buf);
}