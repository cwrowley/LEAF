#include <leaf.h>

const int MEM_SIZE = 32 * 1024;
char mem[MEM_SIZE];

using namespace leaf;

int main()
{
    LEAF leaf(44100.0f, mem, MEM_SIZE);
    printf("LEAF initialized with main pool of %zu bytes\n", leaf.mempool()->getSize());

    Cycle *c = Cycle::create(leaf);
    c->setFreq(440.0f);
    float output = c->tick();
    printf("Cycle tick output: %f\n", output);
    Cycle::destroy(c);
    printf("Cycle object freed\n");
    return 0;
}