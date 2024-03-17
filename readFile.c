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

// char* toUpperCase(char *word) {

//     if (word == NULL || *word == '\0') return NULL;

//     while (*word) {
//         *word = toupper(*word);
//         word += 1;
//     }
//     return word;
// }

// char* toLowercase(char *word) {
//     if (word == NULL) {
//         return NULL; // Return if the string is NULL
//     }

//     // Convert each character to lowercase
//     while (*word) {
//         *word = tolower(*word);
//         word += 1;
//     }

//     return word;
// }

char* toUppercase(const char *str) {
    if (str == NULL) {
        return NULL; // Return NULL if the input string is NULL
    }

    // Allocate memory for the new string
    size_t length = strlen(str);
    char *uppercaseStr = (char*)malloc(length + 1); // +1 for the null terminator
    if (uppercaseStr == NULL) {
        return NULL; // Return NULL if memory allocation fails
    }

    // Convert each character to uppercase
    for (size_t i = 0; i < length; i++) {
        uppercaseStr[i] = toupper(str[i]);
    }
    uppercaseStr[length] = '\0'; // Null-terminate the string

    return uppercaseStr;
}

char* toLowercase(const char *str) {
    if (str == NULL) {
        return NULL; // Return NULL if the input string is NULL
    }

    // Allocate memory for the new string
    size_t length = strlen(str);
    char *lowercaseStr = (char*)malloc(length + 1); // +1 for the null terminator
    if (lowercaseStr == NULL) {
        return NULL; // Return NULL if memory allocation fails
    }

    // Convert each character to lowercase
    for (size_t i = 0; i < length; i++) {
        lowercaseStr[i] = tolower(str[i]);
    }
    lowercaseStr[length] = '\0'; // Null-terminate the string

    return lowercaseStr;
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

// char* capitalizeFirstLetterOnly(char *str) {
//     if (str == NULL || *str == '\0') {
//         return NULL; // Return if the string is empty or NULL
//     }

//     // Capitalize the first letter
//     *str = toupper(*str);
    
//     // Convert the rest of the letters to lowercase
//     while (*(++str)) {
//         *str = tolower(*str);
//     }
//     return str;
// }

char* capitalizeFirstLetter(const char *str) {
    if (str == NULL || *str == '\0') {
        return NULL; // Return NULL if the input string is empty or NULL
    }

    // Allocate memory for the new string
    size_t length = strlen(str);
    char *newStr = (char*)malloc(length + 1); // +1 for the null terminator
    if (newStr == NULL) {
        return NULL; // Return NULL if memory allocation fails
    }

    // Capitalize the first letter
    newStr[0] = toupper(str[0]);

    // Convert the rest of the letters to lowercase
    for (size_t i = 1; i < length; i++) {
        newStr[i] = tolower(str[i]);
    }
    newStr[length] = '\0'; // Null-terminate the string

    return newStr;
}

int contains_apostrophe(char *word) {
    char *ptr = word;
    while (*ptr != '\0') {
        if (*ptr == '\'') {
            return TRUE;
        }
        ptr += 1;
    }
    return FALSE;
}

int has_weird_caps(char *word) {
    // int letter_count = 0;
    // int num_cap_letters = 0;
    // char *ptr = word;
    // while (*ptr != '\0') {
    //     letter_count += 1;
    //     ptr += 1;
    // }

    // // printf("letter count: %d\n", letter_count);
    // char *ptr2 = word;
    // int count = 0;
    // while (count < letter_count) {
    //     if (isupper(*ptr2)) {
    //         num_cap_letters += 1;
    //     }
    //     count += 1;
    //     ptr2 += 1;
    // }

    // if (num_cap_letters == letter_count) {
    //     return FALSE;
    // } else {
    //     return TRUE;
    // }

    if (word == NULL) {
        return FALSE; // Return false if the input string is NULL
    }

    int capitalCount = 0;
    int letterCount = 0;

    while (*word) {
        if (isalpha(*word)) {
            letterCount++; // Increment letter count if character is a letter
            if (isupper(*word)) {
                capitalCount++; // Increment capital letter count if character is uppercase
            }
        }
        word++;
    }

    int result = (capitalCount == letterCount);
    if (result == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
    // return capitalCount == letterCount; // Return true if capital letter count equals letter count
}

int validate_component(char *component) {
    if (DEBUG) {
        printf("Validating Word: %s\n", component);
    }

    if (has_weird_caps(component) == TRUE) {            // if given a word like McDonald, check McDonald and MACDONALD 

        if (binary_search(wordCount, dict_array, component, 0) != -1) {
            return TRUE;
        }
        char *uppercase = toUppercase(component);
        if (binary_search(wordCount, dict_array, uppercase, 0) != -1) {
            free(uppercase);
            return TRUE;
        }
        free(uppercase);
        return FALSE;

    } else {                                    // if given a word like one, want to check if one, One, or ONE exists in dict.
        // check lowercased word:

        // printf("---------executed on word is: %s\n", component);

        char *lowered = toLowercase(component);
        if (binary_search(wordCount, dict_array, lowered, 0) != -1) {       // if it's found return true
            free(lowered);
            return TRUE;
        }
        free(lowered);
        // check the first letter capitalized version:
        char *capFirstLetter = capitalizeFirstLetter(component);
        if (binary_search(wordCount, dict_array, capFirstLetter, 0) != -1) {    // if it's found return true
            free(capFirstLetter);
            return TRUE;
        }
        free(capFirstLetter);
        // check all caps version
        char *uppercase = toUppercase(component);
        if (binary_search(wordCount, dict_array, uppercase, 0) != -1) {
            free(uppercase);
            return TRUE;
        }
        free(uppercase);
        return FALSE;
    }

    // check og word
    // if (binary_search(wordCount, dict_array, component, 0) != -1) {
    //     return TRUE;
    // }

    // //lowercases the first letter
    // char *lowercase_first_letter_component = lowercase_first_letter_new_string(component);
    // if (binary_search(wordCount, dict_array, lowercase_first_letter_component, 0) != -1) {
    //     free(lowercase_first_letter_component);
    //     return TRUE;
    // }
    // free(lowercase_first_letter_component);  // Ensure this free happens in all cases
    
    // if (is_all_caps(component)) { // if all caps

    //     //char *lowercase_component = to_lowercase(component);
    //     //printf("LowerCase: %s\n", lowercase_component);
    //     int result = binary_search(wordCount, dict_array, component, 1) != -1;
    //     //free(lowercase_component);  // Free lowercase_component after it's no longer needed
    //     if (result) {
    //         return TRUE;
    //     }

    //     char *lowercase_component = to_lowercase(component);
    //     //printf("LowerCase: %s\n", lowercase_component);
    //     result = binary_search(wordCount, dict_array, lowercase_component, 1) != -1;
        
    //     if (result) {
    //         free(lowercase_component); 
    //         return TRUE;
    //     }

    //     char *firstUppercase = uppercase_first_letter_new_string(lowercase_component);
    //     result = binary_search(wordCount, dict_array, firstUppercase, 1) != -1;
        
    //     if (result) {
    //         free(lowercase_component); 
    //         free(firstUppercase);
    //         return TRUE;
    //     }
    //     free(lowercase_component); 
    //     free(firstUppercase);
    // // }
    // } else if(contains_apostrophe(component) == TRUE && has_weird_caps(component) == TRUE) {

    //     return 0;

    // } else if(has_weird_caps(component) == TRUE) {

    //     // perform bin search on the word itself:
    //     int result = binary_search(wordCount, dict_array, component, 0);
    //     if (result == FALSE) { // did not find og word
    //         return FALSE;
    //     } 
        
    //     // else run bin search on all caps version of word
    //     char* upper_str = toUpperCase(component);
    //     int res = binary_search(wordCount, dict_array, upper_str, 0);
    //     if (res == FALSE) {
    //         return FALSE;
    //     } else {
    //         return TRUE;
    //     }

    //     // return;
    // } else if(contains_apostrophe(component)) {
    //     return 0;
    // }

    // in case like word passed in is: aPPLe, it should accept aPPLe and APPLE

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
    int word_start_pos = 1;
    int count = 1; // counting letters in word
    char *word;
    char *ptr = line;
    char *prev = ptr;
    while (*ptr != '\0') {

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

        word = (char *) malloc(sizeof(char) * count);
        strncpy(word, prev, count);
        word[count-1] = '\0';
        int result = validate_word(word, path, word_start_pos);
        free(word);
        
    }

    if (prev == ptr) return;

    // NOTE: will need to check this after reworking above
    if (*ptr == '\0') {

        word = (char *) malloc(sizeof(char) * count);
        strncpy(word, prev, count);
        word[count-1] = '\0';
        int result = validate_word(word, path, word_start_pos);
        free(word);

    }
 
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