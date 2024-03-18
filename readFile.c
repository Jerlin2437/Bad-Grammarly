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

char* strip(char *word) {
    char *end;

    // Trim leading characters
    while (strchr("'\"([{", *word)) word++;

    // Find the end of the string
    end = word + strlen(word) - 1;

    // Trim trailing non-alpha characters
    while (end > word && !isalpha((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';
    return word;
}

// Function to convert word to lowercase for comparison
char* to_lowercase(const char *word) {
    char *lowercase = malloc(strlen(word) + 1);
    if (!lowercase) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; word[i]; i++) {
        lowercase[i] = tolower(word[i]);
    }
    lowercase[strlen(word)] = '\0';

    return lowercase;
}
int is_all_caps(const char *str) {
    // Iterate through each character in the string
    for (int i = 0; str[i] != '\0'; i++) {
        // If it's an alphabetic character and not uppercase, return false
        if (isalpha((unsigned char)str[i]) && !isupper((unsigned char)str[i])) {
            return FALSE;
        }
    }
    // If we reach here, all alphabetic characters are uppercase
    return TRUE;
}

char* lowercase_first_letter_new_string(const char *str) {
    // Allocate memory for the new string
    char *new_str = malloc(strlen(str) + 1);
    
    if (!new_str) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(new_str, str);
    
    if (isupper((unsigned char)new_str[0])) {
        new_str[0] = tolower((unsigned char)new_str[0]);
    }
    
    return new_str;
}

char* uppercase_first_letter_new_string(const char *str) {
    char *new_str = malloc(strlen(str) + 1);
    
    if (!new_str) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(new_str, str);
    
    if (isupper((unsigned char)new_str[0])) {
        new_str[0] = toupper((unsigned char)new_str[0]);
    }
    
    return new_str;
}

int validate_component(char *component) {
    if (DEBUG) {
        printf("Validating Word: %s\n", component);
    }

    // check og
    if (binary_search(wordCount, dict_array, component) != -1) {
        return TRUE;
    }

    char *lowercase_first_letter_component = lowercase_first_letter_new_string(component);
    if (binary_search(wordCount, dict_array, lowercase_first_letter_component) != -1) {
        free(lowercase_first_letter_component);
        return TRUE;
    }
    free(lowercase_first_letter_component);  // Ensure this free happens in all cases

    //if all caps
    if (is_all_caps(component)){
       if(linear_search(wordCount, dict_array, component) != -1){
        return TRUE;
       }
    }
    return FALSE;
}

char *strdup(const char *c)
{
    char *dup = malloc(strlen(c) + 1);

    if (dup != NULL)
       strcpy(dup, c);

    return dup;
}
// Enhanced validate_word function
int validate_word(char *word, char *path, int word_start_col) {
    word = strip(word);
 
    char* original = strdup(word);
    if (word == NULL) {
       
        return FALSE;
    }
    // Check for hyphenated words
    char *hyphenated = strtok(word, "-");
    // printf("hyphen: %s\n", hyphenated);
    while (hyphenated) {
        if (validate_component(hyphenated) == FALSE) {
            printf("%s (%d,%d): %s\n", path, row_pos, word_start_col, original);
            free(original);
            return FALSE;
        }
        hyphenated = strtok(NULL, "-");
    }
    free (original);
    return TRUE;
}

void parse_line(char *path, char *line, int* status) {
    
  

    int word_start_pos = 1;
    int count = 1; // counting letters in word
    char *word;
    char *ptr = line;
    char *prev = ptr;
    while (*ptr != '\0') {
        // col_pos = 1;

        while (isspace(*ptr) && *ptr != '\0') {
            ptr += 1;
            col_pos += 1;
        }
        prev = ptr;

        if (*ptr == '\0') break;

        word_start_pos = col_pos;
        count = 1;                  // reset word count per word
        while (!isspace(*ptr) && *ptr != '\0') {
            count += 1;
            ptr += 1;
            col_pos += 1;
        }

        if (*ptr == '\0') break;

        // if (count > 0) {
        //     word = (char *) malloc(sizeof(char) * count);
        //     word[count-1] = '\0';
        //     int result = validate_word(word, path, word_start_pos);
        //     free(word);
        // }
        word = (char *) malloc(sizeof(char) * count);
        strncpy(word, prev, count);
        word[count-1] = '\0';
        int result = validate_word(word, path, word_start_pos);
        free(word);
        if (result == FALSE){
            *status = 0;
        }
        

    }

    if (prev == ptr) return;

 
    if (*ptr == '\0') {


        word = (char *) malloc(sizeof(char) * count);
        strncpy(word, prev, count);
        word[count-1] = '\0';
        int result = validate_word(word, path, word_start_pos);
        free(word);
        
        if (result == FALSE){
            *status = 0;
        }
    }
    
    // col_pos = 1;
}

void parse_file(char *path, int* status) {

    // // printf("from parse file: %s\n", path);

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
                parse_line(path, buf + line_start, status);
                col_pos = 1;
                // printf("from inside while: %s\n", buf + line_start);
                row_pos += 1;
                line_start = pos + 1;
            }
            pos += 1;
        }
        // no partial line
		if (line_start == pos) {
			pos = 0;
			// printf("no partial line: %s\n", buf);
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
        parse_line(path, buf, status);
        // printf("%s\n", buf);
        row_pos += 1;
    }

    close(fd);
    free(buf);
}