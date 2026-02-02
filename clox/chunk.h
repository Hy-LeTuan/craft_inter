#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum
{
    OP_RETURN,
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE
} OpCode;

typedef struct
{
    int capacity;
    int* encoded_lines;
} LineArray;

typedef struct
{
    int count;
    int capacity;
    uint8_t* code;

    // TODO: change the lines array such that lines[i] show how many chunks belong to the line i
    // then, when getLine is called, we walk along this array until the index of the operation is
    // reached, since they are always in increasing order
    LineArray lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk* chunk, int offset);

void initLineArray(LineArray* array);
void freeLineArray(LineArray* array);

#endif
