#include "gd.h"

// use static allocation to avoid having to free from heap
gd_memory_t allocate() {
    gd_memory_t memory;

    const size_t entriesSizeBytes = 17000; // enough for 9 bit code
    static char entryBytes[entriesSizeBytes];
    memory.entries.sizeBytes = entriesSizeBytes;
    memory.entries.memoryBytes = entryBytes;

    const size_t stringSizeBytes = 16000;
    static char stringsBytes[stringSizeBytes];
    memory.strings.sizeBytes = stringSizeBytes;
    memory.strings.memoryBytes = stringsBytes;

    // return a copy
    return memory;
}