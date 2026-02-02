#include "chunk.h"
#include "memory.h"

#include <string.h>
#include <stdio.h>

void initChunk(Chunk* chunk)
{
    chunk->capacity = 0;
    chunk->count = 0;
    chunk->code = NULL;

    initLineArray(&chunk->lines);
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeLineArray(&chunk->lines);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line)
{
    if (chunk == NULL)
    {
        return;
    }

    if (chunk->count + 1 >= chunk->capacity)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    if (chunk->lines.capacity <= line)
    {
        int oldCapacity = chunk->lines.capacity;
        chunk->lines.capacity = line + 1;
        chunk->lines.encoded_lines =
          GROW_ARRAY(int, chunk->lines.encoded_lines, oldCapacity, line + 1);

        memset(chunk->lines.encoded_lines + oldCapacity, 0,
          sizeof(int) * (chunk->lines.capacity - oldCapacity));
    }

    chunk->code[chunk->count] = byte;
    chunk->lines.encoded_lines[line]++;
    chunk->count++;
}

int addConstant(Chunk* chunk, Value value)
{
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void initLineArray(LineArray* array)
{
    array->capacity = 0;
    array->encoded_lines = NULL;
}

void freeLineArray(LineArray* array)
{
    FREE_ARRAY(Value, array->encoded_lines, array->capacity);
    initLineArray(array);
}

int getLine(Chunk* chunk, int offset)
{
    int index = 1;
    int offset_count = 0;

    do
    {
        offset_count += chunk->lines.encoded_lines[index];
        index++;
    } while (offset_count == 0 || offset_count < offset);

    return index - 1;
}
