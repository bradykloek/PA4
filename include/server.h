#ifndef SERVER_H
#define SERVER_H

#include "utils.h"

// the marketplace inventory
// populated from items.csv at startup
struct item inventory[MAX_ITEMS];
int num_items = 0;

// mutex to protect inventory from concurrent access
pthread_mutex_t inventory_lock = PTHREAD_MUTEX_INITIALIZER;

void printSyntax();

#endif
