#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#ifndef DEBUG
#define DEBUG 0
#endif

static int num_text_files;
static char *file_paths;
static int file_pos_count = 0;

void save_paths(char *path) {
    file_paths[file_pos_count++] = strdup(path);
    if (file_paths[file_pos_count - 1] == NULL) {
        perror("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
}

void traverse_dirs(char *path) {
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..'
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Construct the full path of the entry
            char *fullpath = malloc(strlen(path) + strlen(entry->d_name) + 2);  // +2 for '/' and '\0'
            if (fullpath == NULL) {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
            sprintf(fullpath, "%s/%s", path, entry->d_name);

            // If the entry is a subdirectory, recursively traverse it
            struct stat entryStat;
            if (stat(fullpath, &entryStat) == 0 && S_ISDIR(entryStat.st_mode)) {
                traverseDirectories(fullpath);
            } else if (strstr(entry->d_name, ".txt") != NULL) {
                // Print the path of the text file
                // printf("Text file: %s\n", fullpath);
                // save_path(fullpath);
                num_text_files += 1;
            }

            // Free the allocated memory
            free(fullpath);
        }
    }

    // Close the directory
    closedir(dir);
}

void count_text_files_from_path(char *path) {
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..'
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Construct the full path of the entry
            char *fullpath = malloc(strlen(path) + strlen(entry->d_name) + 2);  // +2 for '/' and '\0'
            if (fullpath == NULL) {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
            sprintf(fullpath, "%s/%s", path, entry->d_name);

            // If the entry is a subdirectory, recursively traverse it
            struct stat entryStat;
            if (stat(fullpath, &entryStat) == 0 && S_ISDIR(entryStat.st_mode)) {
                traverseDirectories(fullpath);
            } else if (strstr(entry->d_name, ".txt") != NULL) {
                // Print the path of the text file
                // printf("Text file: %s\n", fullpath);
                // save_path(fullpath);
                num_text_files += 1;
            }

            // Free the allocated memory
            free(fullpath);
        }
    }

    // Close the directory
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
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    char **dictionary = read_dictionary(fd, &word_count);

    if (DEBUG) {
        printf("Total words read: %d\n", word_count);
        for (int i = 0; i < word_count && i < word_count; i++) {
            printf("%s\n", dictionary[i]);
        }
    }

    // process text files passed in:

    file_paths = (char *) malloc (num_text_files * sizeof(char *));
    if (file_paths == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(dictionary);
        close(fd);
        return NULL;
    }

    for (int i = 1; i <= argc; i++) {
        
    }



    // int result = binary_search(fd, dictionary, "APPLE");
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
    close(fd);
    return 0;
}
