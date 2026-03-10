#include <leaf.h>

#define MEM_SIZE (32 * 1024)
char mem[MEM_SIZE];

int main()
{
    LEAF leaf;
    LEAF_init(&leaf, 44100.0f, mem, MEM_SIZE);
    printf("LEAF initialized with main pool of %zu bytes\n", leaf_pool_get_size(&leaf));

    tCycle *c;
    tCycle_init(&c, &leaf);
    tCycle_setFreq(c, 440.0f);
    float output = tCycle_tick(c);
    printf("Cycle tick output: %f\n", output);
    tCycle_free(&c);
    printf("Cycle object freed\n");
    return 0;
}