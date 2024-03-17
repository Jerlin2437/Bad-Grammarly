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
#define DEBUG 0
#endif

static char **file_paths;
static int num_files = 0;

void save_path(char *path) {
    // printf("path passed to save_path: %s\n", path);
    file_paths = realloc(file_paths, sizeof(char *) * (num_files + 1));
    file_paths[num_files] = path;
    // printf("after reallocation: %s\n", file_paths[num_files]);
    // if (file_paths == NULL) {
    //     file_paths = (char **) malloc(sizeof(char *));
    //     if (file_paths == NULL) {
    //         fprintf(stderr, "Error: memory allocation error.\n");
    //         exit(EXIT_FAILURE);
    //     }
    //     file_paths[num_files++] = path;
    // } else {
    //     file_paths = realloc(file_paths, sizeof(char *) * (num_files + 1));
    //     if (file_paths == NULL) {
    //         fprintf(stderr, "Error: Memory reallocation/allocation error.\n");
    //         exit(EXIT_FAILURE);
    //     }
    //     file_paths[num_files++] = path;
    // }
    num_files += 1;
}

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
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".txt") == 0 && entry->d_name[0] != '.') {
                printf("Found text file: %s, now parsing...\n", fullPath);
                // save_path(fullPath);
                parse_file(fullPath);
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
    int word_counts = 0;
    int fdDict = open(argv[1], O_RDONLY);
    if (fdDict < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    char **dictionary = read_dictionary(fdDict, &word_counts);

    if (DEBUG) {
        printf("Total words read in spchk.c: %d\n", word_counts);
        for (int i = 0; i < word_counts && i < word_counts; i++) {
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
            parse_file(argv[i]);
           // readFile(argv[i]);
        } else if (S_ISDIR(path_stat.st_mode)) {
            // It's a directory
            printf("We've found a directory: %s\n", argv[i]);
            findTxtFiles(argv[i]);
        } else {
            fprintf(stderr, "%s is not a regular file or directory\n", argv[i]);
        }
    }

    //prints out txt files in given direrctory (argv2)
    
    //works -j 3/13
    // int result = binary_search(word_counts, dictionary, "ABSTRACT");
    // if (result >= 0) {
    //     printf("FOUND\n");
    // } else {
    //     printf("FALSE\n");
    // }

    //////// prints to see if saved_files works
    // char **ptr = file_paths;
    // int count = 0;
    // while (count < num_files) {
    //     printf("%s\n", *ptr);
    //     ptr += 1;
    //     count+= 1;
    // }
    // printf("num_files: %d\n", num_files);

    ////////

    for (int i = 0; i < word_counts; i++) {
        free(dictionary[i]);
    }

    free(file_paths);
    free(dictionary);
    close(fdDict);
    return 0;
}
