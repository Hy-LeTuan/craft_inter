#include "chunk.h"
#include "vm.h"

int main(int argc, const char* argv[])
{
    initVM();
    Chunk chunk;
    initChunk(&chunk);

    // add a constant, retrieve index in values array
    int constant = addConstant(&chunk, 2.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    // all binary operations have to be written to the bytecode in reverse polish notation, where
    // the operands appear first, followed by the operator
    // because of thhe stack, the operations can appear sequentially
    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_ADD, 123);

    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    // divide the result of the add operation and the newly added 5.6
    writeChunk(&chunk, OP_DIVIDE, 123);

    // negate the result of the division
    writeChunk(&chunk, OP_NEGATE, 123);

    writeChunk(&chunk, OP_RETURN, 123);

    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);

    return 0;
}
