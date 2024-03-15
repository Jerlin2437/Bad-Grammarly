# Bad-Grammarly
# SystemsProgramming
# Ayden Lyubimov, Net ID: acl176
# Jerlin Yuen, Net Id: jy823

## Project Overview
Bad-Grammarly is a spell checking tool designed for systems programming. The project is structured into three main source files to manage different aspects of the application efficiently:

- `spchk.c`: The main driver file that handles directory traversal and argument parsing.
- `dictionary.c`: Manages the dictionary data structure, including its creation, population, and searching functionality.
- `readFile.c`: Responsible for parsing input files and performing the spell-check operation.

## Design Notes and Limitations

- The program is designed to work with plaintext `.txt` files and a newline-separated dictionary file.
- The dictionary file is also expected to be correct, meaning the program does not handle punctuation or other in the dictionary file.
- Currently, the spell checker handles basic word validation, including handling hyphenated words and different capitalizations.
- The system does not automatically correct errors but identifies potentially misspelled words based on the provided dictionary.

## Specific Design Notes
#### dictionary.c 
- The dictionary is read from a file specified at runtime from `spchk.c`, allowing for different dictionaries throughout separate runs.
- `dict_array` and each string within `dict_array` is allocated with `malloc`, meaning we had to consider memory management to avoid leaks. 
- Error handling is implemented to address potential issues like file read errors or memory allocation failures.
- The highly modular design, and splitting up the dictionary into three parts, `read_dictionary`, `find_dictlength`, and `binary_search` allow for easy updates such as supporting different dictionary formats or introducing more efficient search algorithms.


#### readFile.c
- The module is designed to handle various file sizes and formats, provided the content is text-based and uses a character encoding compatible with the C standard library's reading functions. While we use POSIX to handle reading and it does not require files to be text-based, the subsequent buffer and validation actions do.
- Memory management occurs in dynamically allocating and freeing memory for processing words and lines from the input files.
- The parsing logic accommodates multiple edge cases, including lines that start or end with whitespace, lines with only whitespace, and handling of words separated by multiple whitespace characters.
- Handling Special Text Cases:
    - **Trailing and Leading Characters**: Strips unwanted characters from the beginning and end of words.
    - **Hyphenated Words**: Recognizes hyphenated words by treating each component as a separate word for spell checking purposes.
    - **Capitalization**: Supports validating words regardless of their case but accounts for dictionary's case sensitivity for certain worsd like `McDonald` or acronyms.
#### spchk.c
- `spchk.c` is designed to allow input for both single files and directories.
-  Error handling mechanisms for issues like unreadable files or directories, invalid arguments, or problems encountered during the spell checking process.
- `spchk.c` calls on `dictionary.c` for dictionary management and `readFile.c` for parsing and checking the spelling of text files.


## Test Scenarios and Test Cases
Please use the dictionary, `./fullDict`, to test. 

To ensure the reliability of Bad-Grammarly, consider the following test scenarios and cases:

1. **Single File Test**: `test1.txt`, is a single file containing a few spelling errors
2. **Directory Test**: `subDir` is a directory consisting of another directory called `subsubDir` and files inside both directories. The program should execute all `.txt` files inside all subdirectories while ignoring other files or .txt files starting with a `.`
3. **Hyphenation and Capitalization**: `test2.txt` Test words with hyphens and different capitalizations to ensure they are validated correctly.
4. **Punctuation Handling**: `test3.txt` Tests if leading and trailing punctuation is correctly ignored during spell checking.
