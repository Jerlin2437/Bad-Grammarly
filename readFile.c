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
static int word_start_pos;

char* strip(char *word) {
    char *end;

    // Trim leading characters
    while (strchr("'\"([{", *word)) word++;

    // Trim trailing punctuation
    end = word + strlen(word) - 1;
    while (end > word && ispunct((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

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
    if (binary_search(wordCount, dict_array, component, 0) != -1) {
        return TRUE;
    }

    //lowercases the first letter
    char *lowercase_first_letter_component = lowercase_first_letter_new_string(component);
    if (binary_search(wordCount, dict_array, lowercase_first_letter_component, 0) != -1) {
        free(lowercase_first_letter_component);
        return TRUE;
    }
    free(lowercase_first_letter_component);  // Ensure this free happens in all cases

    //if all caps
    if (is_all_caps(component)){
        //char *lowercase_component = to_lowercase(component);
        //printf("LowerCase: %s\n", lowercase_component);
        int result = binary_search(wordCount, dict_array, component, 1) != -1;
        //free(lowercase_component);  // Free lowercase_component after it's no longer needed
        if (result) {
            return TRUE;
        }

        char *lowercase_component = to_lowercase(component);
        //printf("LowerCase: %s\n", lowercase_component);
        result = binary_search(wordCount, dict_array, lowercase_component, 1) != -1;
        
        if (result) {
            free(lowercase_component); 
            return TRUE;
        }

        char *firstUppercase = uppercase_first_letter_new_string(lowercase_component);
        result = binary_search(wordCount, dict_array, firstUppercase, 1) != -1;
        
        if (result) {
            free(lowercase_component); 
            free(firstUppercase);
            return TRUE;
        }
        free(lowercase_component); 
        free(firstUppercase);
    }

}


// Enhanced validate_word function
int validate_word(char *word, char *path, int word_start_col) {
    word = strip(word);

    // Check for hyphenated words
    char *hyphenated = strtok(word, "-");
    // printf("hyphen: %s\n", hyphenated);
    while (hyphenated) {
        if (validate_component(hyphenated) == FALSE) {
            printf("%s (%d,%d): %s\n", path, row_pos, word_start_col, word);
            return FALSE;
        }
        hyphenated = strtok(NULL, "-");
    }

    return TRUE;
}

void parse_line(char *path, char *line) {
    
    // // edge case: whitespace before '\0' <-- null terminator (these two kind of go hand in hand)
    // // edge case: line starts with white space and line is only whitespace
    // // edge case: lots of white space between words
    
    // // printf("col_pos: %d\n", col_pos);
    // int offset = 0;
    // char *word;
    // char *ptr = line;
    // char *prev = ptr;
    // while (*ptr != '\0') {
    //     // printf("col_pos (inside loop): %d\n", col_pos);
    //     if (isspace(*ptr)) { // if we found a whitespace character take the found word and validate it
    //         // NOTE: will need to work something in here where it deals with more than 1 whitespace
    //         // character after finding a word, something like:
    //         // if (isspace(*(ptr + 1)) && *ptr != '\0') {
    //         //     while (isspace(*ptr)) {
    //         //         ptr += 1;
    //         //         col_pos += 1;
    //         //     }
    //         //     prev = ptr;
    //         // } else {
    //         //     ptr += 1;
    //         //     prev = ptr;
    //         //     col_pos += 1;
    //         // }
    //         // if (*ptr == '\0') {
    //         //     return; // or break maybe ??? idk yet
    //         // }
    //         word_start_pos += 1;
    //         int word_size = col_pos - offset;
    //         offset = col_pos;
    //         // printf("word_size: %d\n", word_size);
    //         // printf("offset: %d\n", offset);
    //         word = (char *) malloc(sizeof(char) * word_size);
    //         // i think either strncpy or memcpy works here
    //         strncpy(word, prev, word_size);
    //         // memcpy(word, prev, col_pos);
    //         word[word_size-1] = '\0';
    //         printf("%s\n", word);
    //         // for (int i = 0; i < 7; i++) {
    //         //     printf("%c", word[i]);
    //         // }
    //         // int result = validate_word(word, path, word_start_pos); /////////////////// this line is where we'll pass the word to validate if its in the dict.
    //         // int result = validate_word(word);
    //         free(word);
    //         // return;
    //         // printf("col_pos (inside if): %d\n", col_pos);
    //         // NOTE: might need to work samething  here where it deals with more than 1 whitespace
    //         // character after finding a word, something like (but above might already take care of that)
    //         ptr += 1;
    //         prev = ptr;
    //         col_pos += 1;
    //     }
    //     // if (*ptr == '\0') {
    //     // }
    //     ptr     += 1;
    //     col_pos += 1;
    //     // need to move prev pointer (or maybe pointers) correctly here
    // }
    // // prints the last word
    // if (*ptr == '\0') {
    //     int word_size = col_pos - offset;
    //     offset = col_pos;
    //     word = (char *) malloc(sizeof(char) * word_size);
    //     strncpy(word, prev, word_size);
    //     word[word_size-1] = '\0';
    //     printf("%s\n", word);
    //     free(word);
    // }


    // printf("Line to parse: %s\n", line);
    // printf("row |%d| --> %s\n",row_pos, line);

    int word_start_pos = 1;
    int count = 1; // counting letters in word
    int offset = 0;
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
        


        // if (!isspace(*ptr)) { // if we found a whitespace character take the found word and validate it
            
        //     // NOTE: will need to work something in here where it deals with more than 1 whitespace
        //     // character after finding a word, something like:
        //     // if (isspace(*ptr) && *ptr != '\0') {
        //     //     while (isspace(*ptr)) {
        //     //         ptr += 1;
        //     //         col_pos += 1;
        //     //     }
        //     //     prev = ptr;
        //     // } else {
        //     //     ptr += 1;
        //     //     prev = ptr;
        //     //     col_pos += 1;
        //     // }
        //     // if (*ptr == '\0') {
        //     //     return; // or break maybe ??? idk yet
        //     // }

        //     word_start_pos = col_pos;
        //     int word_size = col_pos - offset;
        //     offset = col_pos;
        //     // printf("word_size: %d\n", word_size);
        //     // printf("offset: %d\n", offset);
        //     word = (char *) malloc(sizeof(char) * word_size);
        //     strncpy(word, prev, word_size);
        //     word[word_size-1] = '\0';

        //     // printf("inside: %s\n", word);
        //     // int result = validate_word(word);
        //     int result = validate_word(word, path, word_start_pos);
        //     // if (result == FALSE) {
        //     //     printf("%s (%d,%d): %s\n", path, row_pos, word_start_pos, word);
        //     // }

        //     free(word);
        //     word_start_pos += 1;
        //     ptr += 1;
        //     prev = ptr;
        //     col_pos += 1;
        // }
        // ptr     += 1;
        // col_pos += 1;
    }

    if (prev == ptr) return;

    // NOTE: will need to check this after reworking above
    if (*ptr == '\0') {


        word = (char *) malloc(sizeof(char) * count);
        strncpy(word, prev, count);
        word[count-1] = '\0';
        int result = validate_word(word, path, word_start_pos);
        free(word);

        // int word_size = col_pos - offset;
        // offset = col_pos;
        // word = (char *) malloc(sizeof(char) * word_size);
        // strncpy(word, prev, word_size);
        // // word[word_size-1] = '\0';
        // // word[word_size] = '\0';
        // // printf("out: %s\n", word);
        // // int result = validate_word(word);
        // int result = validate_word(word, path, word_start_pos);
        // free(word);
        // col_pos = 1;
    }
    // col_pos = 1;
}

void parse_file(char *path) {

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
                parse_line(path, buf + line_start);
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
        parse_line(path, buf);
        // printf("%s\n", buf);
        row_pos += 1;
    }

    close(fd);
    free(buf);
}