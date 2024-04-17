// name: 楼天驰
// id: 522031910290

#include "cachelab.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

char *programName = NULL;

// Usage Info 
void helpInfo()
{
    char *p = programName;
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <tracefile>\n", p);
    printf("Options:\n");
    printf("  -h              Print this help message.\n");
    printf("  -v              Optional verbose flag.\n");
    printf("  -s <num>        Number of set index bits.\n");
    printf("  -E <num>        Number of lines per set.\n");
    printf("  -b <num>        Number of block offset bits.\n");
    printf("  -t <tracefile>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", p);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", p);

}

void unknowOpt(char *err)
{
    printf("%s: invalid option -- '%s'", programName, err);
}

// define the types needed
typedef struct mem_set
{
    int maxLine, maxBlock;
    long *lines;
    // 最低位为valid，其他为tag
    int *recentUse;
} type_set;

typedef struct mem
{
    int currentTime;
    int hit_times, miss_times, evic_times;
    int maxSetNum;
    type_set **sets;
} type_mem;

// Initialization and memory restore functions.
void setInit(int E, int b, type_set *set)
{
    set->maxLine = E;
    set->maxBlock = (1 << b);
    set->recentUse = calloc(E, sizeof(int));
    set->lines = calloc(E, sizeof(long));
}

void memInit(int s, int E, int b, type_mem *mem)
{
    mem->maxSetNum = (1 << s);
    mem->evic_times = mem->hit_times = mem->miss_times = 0;
    mem->currentTime = 0;
    mem->sets = calloc(mem->maxSetNum, sizeof(type_set*));
    for (int i = 0; i < mem->maxSetNum; i++) {
        mem->sets[i] = malloc(sizeof(type_set));
        setInit(E, b, mem->sets[i]);
    }
}

void setFree(type_set *set)
{
    free(set->recentUse);
    free(set->lines);
}

void memFree(type_mem *mem)
{
    for (int i = 0; i < mem->maxSetNum; i++) {
        setFree(mem->sets[i]);
        free(mem->sets[i]);
    }
    free(mem->sets);
    free(mem);
}

// The key function.
// Use setID to get the set to visit. Check every line in that set.
// There are 3 possible situations:
//   1. There is line with correct tag: Just return and add hit times.
//   2. There is a line haven't been used: Add miss times and return.
//   3. Choose the least used line to be evicted: Miss and eviction.

int loadOrStore(int setID, long tag, type_mem *mem)
{
    mem->currentTime++;
    type_set *setVi = mem->sets[setID];
    for (int i = 0; i < setVi->maxLine; i++) {
        if ((setVi->lines[i] & 1) && (setVi->lines[i] >> 1) == tag) {
            mem->hit_times++;
            setVi->recentUse[i] = mem->currentTime;
            return 0;
        }
    }
    mem->miss_times++;
    int lineID = -1;
    for (int i = 0; i < setVi->maxLine; i++) {
        if ((setVi->lines[i] & 1) == 0) {
            setVi->lines[i] = ((tag << 1) | 1);
            setVi->recentUse[i] = mem->currentTime;
            return 1;
        }
        else if (lineID == -1 || setVi->recentUse[lineID] > setVi->recentUse[i])
            lineID = i;
    }
    mem->evic_times++;
    setVi->lines[lineID] = ((tag << 1) | 1);
    setVi->recentUse[lineID] = mem->currentTime;
    return 2;
}

int modify(int setID, long tag, type_mem *mem)
{
    int x = 0;
    x += loadOrStore(setID, tag, mem);
    x += loadOrStore(setID, tag, mem);
    return x;
}

int main(int argc, char **argv)
{
    programName = argv[0];
    if (argc == 1) {
        helpInfo();
        return 0;
    }
// Handle the program settings.
    int verboseFlag = 0, sNum = 0, ENum = 0, bNum = 0;
    char *readPath;
    int readingFail = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            helpInfo();
            return 0;
        }
        if (strcmp(argv[i], "-v") == 0) {
            verboseFlag = 1;
        }
        else if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc) {
                readingFail = 1;
                break;
            }
            sNum = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-E") == 0) {
            if (i + 1 >= argc) {
                readingFail = 1;
                break;
            }
            ENum = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-b") == 0) {
            if (i + 1 >= argc) {
                readingFail = 1;
                break;
            }
            bNum = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                readingFail = 1;
                break;
            }
            readPath = argv[i + 1];
            i++;
        }
        else {
            helpInfo();
            return 0;
        }
    }
    if (readingFail == 1 || sNum == 0 || ENum == 0 || bNum == 0) {
        helpInfo();
        return 0;
    }
    
// start reading from file and handle the instructions
    FILE *in = fopen(readPath, "r");
    if (in == NULL) {
        printf("%s: No such file or directory\n", readPath);
        return 0;
    }

    type_mem *memory;
    memory = malloc(sizeof(type_mem));
    memInit(sNum, ENum, bNum, memory);

    char ins;
    long addr;
    int size;
    long tag, set;
    long mask = (1 << (sNum + bNum)) - 1;
    while (fscanf(in, "%c %lx,%d\n", &ins, &addr, &size) != -1) {
        if (ins != 'M' && ins != 'S' && ins != 'L')
            continue;
        tag = addr >> (sNum + bNum);
        set = (addr & mask) >> bNum;
        int res;
// if '-v' is included, output the result depend on the instruction.
// Otherwise, just simulate the behavior.
        switch (ins)
        {
        case 'M':
            res = modify(set, tag, memory);
            if (verboseFlag) {
                switch (res)
                {
                case 0:
                    printf("hit hit\n");
                    break;
                case 1:
                    printf("miss hit\n");
                    break;                    
                case 2:
                    printf("evic hit\n");
                    break;
                default:
                    break;
                }
            }
            break;
        case 'L':
        case 'S':
            res = loadOrStore(set, tag, memory);
            if (verboseFlag) {
                switch (res)
                {
                case 0:
                    printf("hit\n");
                    break;
                case 1:
                    printf("miss\n");
                    break;                    
                case 2:
                    printf("evic\n");
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    printSummary(memory->hit_times, memory->miss_times, memory->evic_times);
    memFree(memory);
    fclose(in);
    return 0;
}
