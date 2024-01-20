#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define CLUSTER_SIZE 3

typedef struct {
    uint32_t key16;
    uint8_t depth8;
    uint8_t genBound8;
    int16_t value16;
    int16_t eval16;
    uint16_t move16;
} TTEntry;

typedef struct {
    TTEntry entry[CLUSTER_SIZE];
} Cluster;

typedef struct {
    Cluster* table;
    size_t clusterCount;
    uint8_t generation8;
} TranspositionTable;

void TTEntry_save(TTEntry* entry, uint64_t k, int16_t v, bool pv, uint8_t b, uint8_t d, uint16_t m, int16_t ev, TranspositionTable* tt);
void TranspositionTable_resize(TranspositionTable* tt, size_t mbSize);
TTEntry* TranspositionTable_probe(TranspositionTable* tt, uint64_t key, bool* found);
int TranspositionTable_hashfull(TranspositionTable* tt);

#endif // TRANSPOSITION_TABLE_H
