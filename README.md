# Hash Table Project
### Overview
- This project is an implementation of a Hash Table in C, using open addressing with double hashing for collision handling. The project includes functionality to insert, search, and delete key-value pairs, and to manage dynamic resizing of the table based on its load factor.

- This project features a Command Line Interface (CLI) that allows users to interact with the hash table and perform various operations such as insertion, search, deletion, and viewing the current count and size of the table.

### Features
- Insertion: Insert key-value pairs into the hash table.
- Searching: Retrieve the value associated with a given key.
- Deletion: Remove a key-value pair from the hash table.
- Dynamic Resizing:
  - Automatically resizes up if load factor exceeds 70%.
  - Automatically resizes down if load factor falls below 10%.
- CLI Interface: User-friendly command-line interface for interacting with the hash table.
- Collision Handling: Uses double hashing to resolve hash collisions.
- Efficient Memory Management: Implements memory allocation and reallocation with error checking.

### Project Structure
- "hash_table.c" : Contains the main implementation of the hash table, including item creation, hash functions, insertion, search, deletion, and resizing functions.
- "main.c" : Provides a CLI interface for users to interact with the hash table and perform operations.
- "prime.c" : Includes helper functions to find the next prime number for resizing the hash table efficiently.

### Compilation and Usage (with CMake)

1. **Configure and Build the Project**

- To build the project using CMake, first generate build files:

  ```bash
  cmake -B build -DCMAKE_BUILD_TYPE=Debug/Release
  cmake --build build
  cd build
  ./main
### CLI Options
- Upon running the program, the following options are available:

1. Insert: Add a key-value pair to the table.
2. Search: Search for the value associated with a specific key.
3. Delete: Delete a key-value pair by specifying the key.
4. Count: View the current count of items in the table.
5. Current Size: View the current allocated size of the hash table.
6. Exit: Terminate the program.

### Notes
- This hash table implementation does not support handling of multiple values for the same key.
- The project demonstrates basic hash table functionality, suitable for educational purposes and small-scale applications.
- Dynamic resizing helps maintain efficient space usage and quick access time.
