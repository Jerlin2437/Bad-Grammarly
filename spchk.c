#define _GNU_SOURCE

#include "dictionary.h"
#include "readFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef DEBUG
#define DEBUG 1
#endif
static char *file_paths;

//Only a function declaration so far of readFile, which readsTxt file and executes it
void readFile(const char *filePath);


//works - j 3.13
void findTxtFiles(const char *dirPath) {
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        fprintf(stderr, "Cannot open directory '%s'\n", dirPath);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // Skip the current and parent directory entries
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            // Calls subdir if a subdir exists
            findTxtFiles(fullPath);
        } else if (entry->d_type == DT_REG) {
            // Check if the file name ends with ".txt" and does not start with '.'
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".txt") == 0 && entry->d_name[0] != '.') {
                printf("Found text file: %s\n", fullPath);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {

    // check if dictionary was provided
    if (argc < 2) {
        fprintf(stderr, "Error: Missing a Dictionary\n");
        return 1;
    }

    // check if at least one text file to test was provided
    if (argc < 3) {
        fprintf(stderr, "Error: provide at least one file to test.\n");
        exit(EXIT_FAILURE);
    }

    // open and process dictionary
    /////////// NOTE: we might have to account for argv[1] being a path to a dict outside the working directory /////////////
    int word_count = 0;
    int fdDict = open(argv[1], O_RDONLY);
    if (fdDict < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    char **dictionary = read_dictionary(fdDict, &word_count);

    if (DEBUG) {
        printf("Total words read: %d\n", word_count);
        for (int i = 0; i < word_count && i < word_count; i++) {
            printf("%s\n", dictionary[i]);
        }
    }

// This block of code discerns whether or not argv[x] is a file or directory
    struct stat path_stat;
    for (int i = 2; i < argc; i++) {
        if (stat(argv[i], &path_stat) == -1) {
            perror("stat failed");
            continue;
        }

        if (S_ISREG(path_stat.st_mode)) {
            
            // It's a regular file
            printf("We've found a regular file: %s\n", argv[i]);
           // readFile(argv[i]);
        } else if (S_ISDIR(path_stat.st_mode)) {
            // It's a directory
            findTxtFiles(argv[i]);
        } else {
            fprintf(stderr, "%s is not a regular file or directory\n", argv[i]);
        }
    }

//prints out txt files in given direrctory (argv2)
 
//works -j 3/13
    // int result = binary_search(word_count, dictionary, "ABSTRACT");
    // if (result >= 0) {
    //     printf("FOUND\n");
    // } else {
    //     printf("FALSE\n");
    // }

    for (int i = 0; i < word_count; i++) {
        free(dictionary[i]);
    }

    free(file_paths);
    free(dictionary);
    close(fdDict);
    return 0;
}
