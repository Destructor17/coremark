#include "coremark.h"
#include "../core/include/core.h"
#include <stdlib.h>

static ee_u16 list_known_crc[] = {(ee_u16)0xd4b0, (ee_u16)0x3340,
                                  (ee_u16)0x6a79, (ee_u16)0xe714,
                                  (ee_u16)0xe3c1};
static ee_u16 matrix_known_crc[] = {(ee_u16)0xbe52, (ee_u16)0x1199,
                                    (ee_u16)0x5608, (ee_u16)0x1fd7,
                                    (ee_u16)0x0747};
static ee_u16 state_known_crc[] = {(ee_u16)0x5e47, (ee_u16)0x39bf,
                                   (ee_u16)0xe5a4, (ee_u16)0x8e3a,
                                   (ee_u16)0x8d84};
void *iterate(void *pres) {
    ee_u32 i;
    ee_u16 crc;
    core_results *res = (core_results *)pres;
    ee_u32 iterations = res->iterations;
    res->crc = 0;
    res->crclist = 0;
    res->crcmatrix = 0;
    res->crcstate = 0;

    for (i = 0; i < iterations; i++) {
        crc = core_bench_list(res, 1);
        res->crc = crcu16(crc, res->crc);
        crc = core_bench_list(res, -1);
        res->crc = crcu16(crc, res->crc);
        if (i == 0)
            res->crclist = res->crc;
    }
    return NULL;
}

void coremarkInitializationStep() {
    core_results result;

    result.seed1 = 0x3415;
    result.seed2 = 0x3415;
    result.seed3 = 0x66;
    result.iterations = 1;
    result.size = 256 * 1024;

    result.execs = ALL_ALGORITHMS_MASK;

    for (int i = 0; i < NUM_ALGORITHMS; i++) {
        ee_u32 ctx;
        if ((1 << (ee_u32)i) & result.execs) {
            result.memblock[i + 1] = portable_malloc(result.size);
        }
    }

    if (result.execs & ID_LIST) {
        result.list =
            core_list_init(result.size, result.memblock[1], result.seed1);
    }
    if (result.execs & ID_MATRIX) {
        core_init_matrix(result.size, result.memblock[2],
                         (ee_s32)result.seed1 | (((ee_s32)result.seed2) << 16),
                         &(result.mat));
    }
    if (result.execs & ID_STATE) {
        core_init_state(result.size, result.seed1, result.memblock[3]);
    }

    start_time();
    iterate(&result);
    stop_time();

    CORE_TICKS total_time = get_time();

    char str[256];
    sprintf(str, "CoreMark Size    : %lu\n", (long unsigned)result.size);
    webrogue_core_print(str);
    sprintf(str, "Total ticks      : %lu\n", (long unsigned)total_time);
    webrogue_core_print(str);
    sprintf(str, "Total time (secs): %f\n", time_in_secs(total_time));
    webrogue_core_print(str);
    while (1) {
        webrogue_core_interrupt();

        size_t eventCount;
        webrogue_event *events = webrogue_core_get_events(&eventCount);
        for (size_t i = 0; i < eventCount; i++)
            if (events[i].type == Close)
                exit(0);
    }
}

void init_mod_coremark() {
    webrogue_core_add_initialization_step(coremarkInitializationStep);
}