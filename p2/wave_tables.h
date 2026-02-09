#ifndef WAVE_TABLES_H
#define WAVE_TABLES_H

#include <stdint.h>

#define WAVE_TABLE_LEN (100U)

extern const uint16_t sine_table[WAVE_TABLE_LEN];
extern const uint16_t tri_table[WAVE_TABLE_LEN];
extern const uint16_t saw_table[WAVE_TABLE_LEN];

#endif
