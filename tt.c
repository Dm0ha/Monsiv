/*
Transpotition table
Same implementation as Stockfish but converted to C
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

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

void TTEntry_save(TTEntry* entry, uint64_t k, int16_t v, bool pv, uint8_t b, uint8_t d, uint16_t m, int16_t ev, TranspositionTable* tt) {
    if (m || (uint32_t)k != entry->key16)
        entry->move16 = (uint16_t)m;

    if ((uint32_t)k != entry->key16 || entry->depth8 < d) {
        entry->key16     = (uint32_t)k;
        entry->depth8    = d;
        entry->genBound8 = (uint8_t)(tt->generation8 | (pv ? 1 : 0) << 2 | b);
        entry->value16   = v;
        entry->eval16    = ev;
    }
}

void TranspositionTable_resize(TranspositionTable* tt, size_t mbSize) {
    free(tt->table);
    tt->clusterCount = mbSize * 1024 * 1024 / sizeof(Cluster);
    tt->table = malloc(tt->clusterCount * sizeof(Cluster));
    if (!tt->table)
    {
        fprintf(stderr, "Failed to allocate %zuMB for transposition table.\n", mbSize);
        exit(EXIT_FAILURE);
    }
    memset(tt->table, 0, tt->clusterCount * sizeof(Cluster));
}

TTEntry* TranspositionTable_probe(TranspositionTable* tt, uint64_t key, bool* found) {
    TTEntry* tte = &tt->table[(uint32_t)key % tt->clusterCount].entry[0];
    uint32_t key16 = (uint32_t)key;
    for (int i = 0; i < CLUSTER_SIZE; ++i)
        if (tte[i].key16 == key16)
        {
            tte[i].genBound8 = (uint8_t)(tt->generation8 | (tte[i].genBound8 & (255 - 1)));
            *found = true;
            return &tte[i];
        }

    TTEntry* replace = tte;
    for (int i = 1; i < CLUSTER_SIZE; ++i)
        if (replace->depth8 - ((256 + tt->generation8 - replace->genBound8) & 255)
            > tte[i].depth8 - ((256 + tt->generation8 - tte[i].genBound8) & 255))
            replace = &tte[i];

    *found = false;
    return replace;
}

int TranspositionTable_hashfull(TranspositionTable* tt) {
    int cnt = 0;
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < CLUSTER_SIZE; ++j) {
            cnt += tt->table[i].entry[j].depth8 && (tt->table[i].entry[j].genBound8 & 255) == tt->generation8;
        }     
    }
    return cnt / CLUSTER_SIZE;
}
