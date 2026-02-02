#include "debug.h"

#include "chunk.h"
#include "value.h"

#include <stdio.h>

void disassembleChunk(Chunk* chunk, const char* name)
{
    printf("== %s ==\n", name);
    printf("chunk count: %d\n", chunk->count);

    for (int offset = 0; offset < chunk->count;)
    {
        offset = disassembleInstructions(chunk, offset);
    }
}

static int simpleInstruction(const char* name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset)
{
    printf("%s\n", name);

    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");

    return offset + 2;
}

int disassembleInstructions(Chunk* chunk, int offset)
{
    printf("%04d ", offset);

    if (offset > 0 && getLine(chunk, offset) == getLine(chunk, offset - 1))
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", getLine(chunk, offset));
    }

    uint8_t instruction = chunk->code[offset];

    switch (instruction)
    {
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
        case OP_ADD:
            return simpleInstruction("OP_ADD", offset);
        case OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
