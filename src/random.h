#ifndef RANDOM_H
#define RANDOM_H

// Random int (inclusive)
int randint(int min, int max);
char randchar();
float random();

#endif // RANDOM_H

#ifdef RANDOM_IMPLEMENTATION
#include <stdlib.h>

int randint(int min, int max) {
    return min + rand() % (max - min + 1);
}

char randchar() {
    return randint(0, 256);
}

float random() {
    return randint(0, 100) / 100;
}

#endif
