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
