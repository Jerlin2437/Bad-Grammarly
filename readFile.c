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
    char *word;
    char *ptr = line;
    char *prev = ptr;
    while (*ptr != '\0') {
        ptr     += 1;
        col_pos += 1;
        if (isspace(*ptr)) { // if we found a whitespace character take the found word and validate it
            word = (char *) malloc(sizeof(char) * col_pos + 1);

            // i think either strncpy or memcpy works here
            strncpy(word, prev, col_pos);
            // memcpy(word, prev, col_pos);
            word[col_pos] = '\0';
            // printf("%s\n", word);
            int result = validate_word(word);
            free(word);
            // return;
            // printf("col_pos: %d\n", col_pos);
        }
        // need to move prev pointer (or maybe pointers) correctly here
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