#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Memory block structure for leak tracking
typedef struct MemoryBlock {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    struct MemoryBlock* next;
} MemoryBlock;

MemoryBlock* memory_block_head = NULL;

// Custom memory allocation wrapper
void* tracked_malloc(size_t size, const char* filename, int line) {
    void* ptr = malloc(size);
    if (ptr != NULL) {
        MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
        block->ptr = ptr;
        block->size = size;
        block->file = filename;
        block->line = line;

        pthread_mutex_lock(&mutex);
        block->next = memory_block_head;
        memory_block_head = block;
        pthread_mutex_unlock(&mutex);
    }
    return ptr;
}

// Custom memory deallocation wrapper
void tracked_free(void* ptr) {
    if (ptr != NULL) {
        pthread_mutex_lock(&mutex);
        MemoryBlock* prev = NULL;
        MemoryBlock* curr = memory_block_head;
        while (curr != NULL && curr->ptr != ptr) {
            prev = curr;
            curr = curr->next;
        }

        if (curr != NULL) {
            if (prev != NULL) {
                prev->next = curr->next;
            } else {
                memory_block_head = curr->next;
            }
            free(curr);
        }
        pthread_mutex_unlock(&mutex);
        free(ptr);
    }
}

// Detect memory leaks by checking all allocated blocks
void detect_memory_leaks(const char* file_name) {
    pthread_mutex_lock(&mutex);
    MemoryBlock* curr = memory_block_head;
    int leak_count = 0;
    while (curr != NULL) {
        if (strcmp(curr->file, file_name) == 0) {
            fprintf(stderr, "Memory leak detected: %zu bytes at %s:%d\n", curr->size, curr->file, curr->line);
            leak_count++;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&mutex);
    if (leak_count == 0) {
        fprintf(stderr, "No memory leaks detected in file: %s\n", file_name);
    } else {
        fprintf(stderr, "Total %d memory leak(s) detected in file: %s\n", leak_count, file_name);
    }
}

// Inside analyze_memory_leaks_example()
void analyze_memory_leaks_example() {
    // Allocate memory but do not free it (simulate a memory leak)
    int* arr1 = (int*)tracked_malloc(100 * sizeof(int), __FILE__, __LINE__);
    int* arr2 = (int*)tracked_malloc(50 * sizeof(int), __FILE__, __LINE__);
    tracked_free(arr2); // Correctly freed arr2, but arr1 is still leaked

    // Detect and report any leaks (arr1 should be detected as leaked)
    detect_memory_leaks(__FILE__);
}



// Read and display system-wide memory information
void analyze_system_memory() {
    FILE *meminfo_file;
    char line[256];
    meminfo_file = fopen("/proc/meminfo", "r");
    if (meminfo_file == NULL) {
        perror("Error opening /proc/meminfo");
        return;
    }
    printf("System-wide Memory Information:\n");
    while (fgets(line, sizeof(line), meminfo_file)) {
        printf("%s", line);
    }
    fclose(meminfo_file);
}

// Display memory usage of a process
void display_memory_usage(pid_t pid) {
    char command[50];
    sprintf(command, "pmap -x %d", pid);
    printf("Process-wise memory usage:\n");
    fflush(stdout);
    int status = system(command);
    if (status == -1) {
        printf("Failed to retrieve memory usage.\n");
    }
}

// Analyze memory of the current process
void analyze_process_memory() {
    int *ptr = malloc(1000 * sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    pid_t pid = getpid();
    display_memory_usage(pid);

    for (int i = 0; i < 1000; i++) {
        ptr[i] = i;
    }

    display_memory_usage(pid);
    free(ptr);
}

// Display virtual memory mapping
void display_memory_mapping() {
    FILE *fp;
    char line[256];
    fp = fopen("/proc/self/maps", "r");
    if (fp == NULL) {
        printf("Failed to open /proc/self/maps\n");
        return;
    }
    printf("Virtual Memory Mapping:\n");
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }
    fclose(fp);
}

// Process a C source file to find memory allocations and deallocations
void processFile(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        return;
    }

    char line[256];
    int lineNum = 0;

    while (fgets(line, sizeof(line), file)) {
        lineNum++;

        if (strstr(line, "malloc")) {
            tracked_malloc(10, filePath, lineNum); // Example allocation
        } else if (strstr(line, "free")) {
            tracked_free(NULL); // Example deallocation
        }
    }

    fclose(file);
}

// Recursively process files in a directory
void processDirectory(const char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (stat(fullPath, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                processDirectory(fullPath);
            } else if (S_ISREG(statbuf.st_mode)) {
                if (strstr(entry->d_name, ".c")) {
                    processFile(fullPath);
                }
            }
        }
    }

    closedir(dir);
}

// Main analysis function based on user choice
void analyze_memory(int choice) {
    switch(choice) {
        case 1:
            analyze_system_memory();
            break;
        case 2:
            analyze_process_memory();
            break;
        case 3:
            display_memory_mapping();
            break;
        case 4:
            // Start analysis from root directory
            processDirectory("/");
            break;
        case 5:
            analyze_memory_leaks_example();
            break;
        case 6:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice\n");
    }
}

// Main entry point
int main() {
    int choice;
    while (1) {
        printf("\nMemory Analysis Menu:\n");
        printf("1. System memory\n");
        printf("2. Process memory\n");
        printf("3. Virtual memory mapping\n");
        printf("4. Memory allocation\n");
        printf("5. Memory leak analysis\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");
        scanf("%d", &choice);
        
                if (choice == 6) {
            break; // This will exit the while loop when the user chooses 6
        }

        analyze_memory(choice);
    }
    printf("Program exited.\n");
    return 0;
}
