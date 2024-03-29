#ifndef PSEUDO_RANDOM_H
#define PSEUDO_RANDOM_H

static unsigned int seed = 0;

int rand(){
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed / 65536) % 32768;
}

void srand(unsigned int c_seed) {
    seed = c_seed;
}

int rand_range(int min, int max) {
    return min + rand() % (max - min);
}

void random_seed_by_cpu_cycle(){
    unsigned int cycle_seed = 0;
    __asm__ volatile ("rdtsc" : "=a" (cycle_seed));
    srand(cycle_seed);
}


#endif // PSEUDO_RANDOM_H
