# Virtual Memory Dashboard

## Overview

The **Virtual Memory Dashboard** is a memory analysis tool designed to assist developers in tracking memory usage, detecting memory leaks, and displaying system and process memory information in real time. This tool provides various memory-related insights, including system-wide memory details, memory usage of specific processes, memory mapping, and detection of memory leaks.

The program uses custom memory allocation functions to track allocations and deallocations, provides detailed system and process memory information, and even allows recursive processing of C source files to detect memory allocation patterns.

## Features

1. **System Memory Analysis**: Displays system-wide memory information, such as total, free, and available memory, by reading the `/proc/meminfo` file.
2. **Process Memory Usage**: Displays memory usage of a specific process using the `pmap` command.
3. **Virtual Memory Mapping**: Shows the virtual memory mapping of the current process by reading `/proc/self/maps`.
4. **Memory Leak Detection**: Tracks memory allocations and deallocations within the program, reporting any memory leaks found.
5. **C Source File Processing**: Recursively processes directories and C source files to find memory allocations (`malloc`) and deallocations (`free`).
6. **Interactive Menu**: Provides a user-friendly interactive menu to choose between different memory analysis options.

## Features in Detail

### System Memory Analysis

This feature reads the `/proc/meminfo` file, which contains information about the system's memory usage. The output includes:
- Total memory
- Free memory
- Available memory
- Buffered memory
- Cached memory

### Process Memory Usage

You can analyze the memory usage of a specific process by providing its PID. The program uses the `pmap` tool to show detailed memory usage statistics for the specified process.

### Virtual Memory Mapping

Displays the virtual memory mapping of the current process by reading from `/proc/self/maps`. This shows how the virtual memory of the process is organized into different segments (heap, stack, etc.).

### Memory Leak Detection

Using custom memory allocation and deallocation wrappers (`tracked_malloc` and `tracked_free`), the tool tracks memory blocks allocated in the program and detects any unfreed memory, reporting potential memory leaks.

### C Source File Processing

The program recursively processes C source files within a directory to detect `malloc` and `free` calls, helping you identify where memory allocations and deallocations occur in the code.

### Interactive Menu

A simple command-line interface that allows users to choose from the various available memory analysis options:
- Option 1: System memory analysis
- Option 2: Process memory analysis
- Option 3: Virtual memory mapping
- Option 4: Analyze memory allocations in C source files
- Option 5: Detect memory leaks
- Option 6: Exit the program

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/nerdylelouch/OSlab-partB.git
   cd OSlab-partB
   ```
2. Compile the program:
    ```bash
    gcc -o a memoryanalysis.c -lpthread
    ```
3. Run the program:
   ```bash
   ./a
    ```
## Usage
When you run the program, you will be presented with the following interactive menu:
```markdown
Memory Analysis Menu:
1. System memory
2. Process memory
3. Virtual memory mapping
4. Memory allocation
5. Memory leak analysis
6. Exit
```
Select an option (1-6) to perform the corresponding memory analysis operation.
## Example of Memory Leak Detection
To demonstrate memory leak detection, the program includes a sample function `analyze_memory_leaks_example()` where memory is allocated but not freed, causing a memory leak. When you select the option for memory leak analysis, the program will detect and report the leak.

## Contributing
If you find any issues or would like to contribute to improving this tool, feel free to fork the repository, make changes, and create a pull request.

## License
This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.
