#define _GNU_SOURCE

#include "dictionary.h"
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
                /////////// NOTE: I COMMENTED THIS OUT BECAUSE TRAVERSE DIRECTORIES DOES NOT EXIST YET
                //traverseDirectories(fullpath);
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
        fprintf(stderr, "Cannot open directory '%s'\n", path);
        return;
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

                /////////// NOTE: I COMMENTED THIS OUT BECAUSE TRAVERSE DIRECTORIES DOES NOT EXIST YET
                //traverseDirectories(fullpath);
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

    // process text files passed in:
    file_paths = (char *) malloc (num_text_files * sizeof(char *));
    if (file_paths == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(dictionary);
        close(fdDict);
        return NULL;
    }

    for (int i = 1; i <= argc; i++) {
        
    }

//prints out txt files in given direrctory (argv2)
    findTxtFiles(argv[2]);
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
