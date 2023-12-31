

/* User Project Memory Mapping 
    => the #define directives help in creating symbolic names for memory addresses

    | Address (Bytes)        | Function               |
    |------------------------|------------------------|
    | 0x30000000 - 0x30003FFF| synap_matrix           |
    | 0x30004000 - 0x3000400B| param0                 |
    | 0x30004010 - 0x3000401B| param1                 |
    | 0x30004020 - 0x3000402B| param2                 |
    | ...                    | ...                    |
    | 0x300040F0 - 0x300040FB| param31                |
    | 0x30008000 - 0x30008003| neuron_spike_out       |
*/

// This include is relative to $CARAVEL_PATH (see Makefile)
#include <defs.h>
#include <stub.c>

#include <stdint.h>
// #include "SNN_data.h"

#define NUM_CORES 2
#define NEURONS_PER_CORE 32
#define AXONS_PER_CORE 256

#define SYNAP_MATRIX_BASE       0x30000000
#define PARAM_BASE              0x30004000
#define NEURON_SPIKE_OUT_BASE   0x30008000


/* Pointers for User Project Memory Mapping 
    - Each pointer is assigned to the corresponding user project address on the wishbone bus
    - we can read from/write to the wishbone bus through the pointers
*/
#define SYNAP_MATRIX_PTR        ((volatile uint32_t*)SYNAP_MATRIX_BASE)
#define PARAM_PTR               ((volatile uint32_t*)PARAM_BASE)
#define NEURON_SPIKE_OUT_PTR    ((volatile uint32_t*)NEURON_SPIKE_OUT_BASE)



/* PASTE SNN_DATA HERE */
typedef struct {
    int8_t membrane_potential;
    int8_t reset_posi_potential;
    int8_t reset_nega_potential;
    int8_t weights[4];
    int8_t leakage_value;
    int8_t positive_threshold;
    int8_t negative_threshold;
    uint8_t axon_dest;
} Neuron;

typedef struct {
    Neuron neurons[NEURONS_PER_CORE];
    uint32_t synapse_connection[AXONS_PER_CORE];
} Core;

typedef struct {
    int8_t dx;
    int8_t dy;
    uint8_t axon_dest;
} Packet;

const Core cores[NUM_CORES] = {
    { // Core 0
        .neurons = {
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 0 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 1 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 2 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 3 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 4 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 5 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 6 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 7 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
        },
        .synapse_connection = {
            0x000000DF,
            0x0000000C,
            0x00000073,
            0x00000060,
            0x000000BE,
            0x00000061,
            0x00000096,
            0x00000028,
            0x00000096,
            0x000000E8,
            0x00000097,
            0x00000009,
            0x000000F2,
            0x00000048,
            0x00000070,
            0x0000000B,
            0x000000DC,
            0x00000000,
            0x000000DE,
            0x00000066,
            0x00000061,
            0x000000FA,
            0x00000005,
            0x000000F2,
            0x0000003F,
            0x000000A0,
            0x00000061,
            0x0000009C,
            0x00000073,
            0x0000008B,
            0x0000005B,
            0x0000002A,
            0x00000017,
            0x00000061,
            0x0000001B,
            0x00000032,
            0x00000096,
            0x000000B1,
            0x000000D1,
            0x00000005,
            0x000000BA,
            0x00000047,
            0x000000B4,
            0x00000003,
            0x000000FC,
            0x00000002,
            0x000000AE,
            0x000000D4,
            0x00000020,
            0x000000F7,
            0x00000015,
            0x0000008A,
            0x000000B5,
            0x000000C2,
            0x00000075,
            0x00000090,
            0x000000F4,
            0x0000003A,
            0x0000008F,
            0x00000059,
            0x00000001,
            0x00000020,
            0x000000FA,
            0x00000020,
            0x00000053,
            0x000000F4,
            0x00000016,
            0x0000006D,
            0x00000040,
            0x00000006,
            0x00000015,
            0x000000D2,
            0x0000004C,
            0x00000072,
            0x00000029,
            0x00000065,
            0x00000023,
            0x000000D6,
            0x00000001,
            0x0000008A,
            0x00000065,
            0x00000098,
            0x00000065,
            0x000000BA,
            0x00000075,
            0x0000008B,
            0x0000002B,
            0x000000AF,
            0x00000003,
            0x000000F8,
            0x0000001B,
            0x00000069,
            0x00000090,
            0x00000063,
            0x00000000,
            0x000000F6,
            0x00000041,
            0x000000B6,
            0x00000005,
            0x00000092,
            0x0000006F,
            0x000000D0,
            0x0000001B,
            0x00000055,
            0x00000009,
            0x000000F5,
            0x00000045,
            0x00000082,
            0x00000065,
            0x0000009A,
            0x00000074,
            0x0000009A,
            0x0000002F,
            0x000000EC,
            0x0000007C,
            0x0000009D,
            0x000000F5,
            0x00000070,
            0x0000009F,
            0x00000068,
            0x00000011,
            0x000000E6,
            0x00000010,
            0x000000B6,
            0x00000041,
            0x000000EF,
            0x00000049,
            0x00000036,
            0x00000069,
            0x00000030,
            0x000000E3,
            0x000000DE,
            0x00000085,
            0x000000D6,
            0x00000025,
            0x0000009E,
            0x00000065,
            0x00000032,
            0x0000006D,
            0x0000008A,
            0x000000D8,
            0x00000071,
            0x00000002,
            0x0000005D,
            0x000000D1,
            0x00000098,
            0x0000008E,
            0x000000AB,
            0x0000009C,
            0x000000AC,
            0x00000043,
            0x000000AA,
            0x00000056,
            0x00000033,
            0x00000009,
            0x000000B6,
            0x0000004B,
            0x000000F4,
            0x00000063,
            0x000000FC,
            0x00000020,
            0x000000AE,
            0x00000049,
            0x0000009E,
            0x00000025,
            0x000000B2,
            0x00000065,
            0x00000050,
            0x00000076,
            0x00000007,
            0x000000F1,
            0x000000AC,
            0x000000FB,
            0x0000005B,
            0x0000002D,
            0x000000B2,
            0x00000075,
            0x000000AA,
            0x00000001,
            0x000000AA,
            0x00000013,
            0x000000B3,
            0x00000049,
            0x00000077,
            0x0000004B,
            0x000000FC,
            0x00000021,
            0x000000FE,
            0x00000001,
            0x000000A8,
            0x000000CD,
            0x000000BE,
            0x00000004,
            0x0000003B,
            0x000000D9,
            0x000000BD,
            0x00000026,
            0x000000B5,
            0x000000AD,
            0x000000C5,
            0x000000DD,
            0x00000043,
            0x000000C8,
            0x00000064,
            0x000000D2,
            0x000000BC,
            0x00000050,
            0x000000AB,
            0x000000C5,
            0x00000032,
            0x00000048,
            0x000000F2,
            0x000000DB,
            0x000000C5,
            0x00000057,
            0x00000013,
            0x0000000D,
            0x000000BA,
            0x00000046,
            0x000000BB,
            0x00000045,
            0x00000024,
            0x000000D0,
            0x00000043,
            0x0000006C,
            0x00000085,
            0x00000062,
            0x000000E9,
            0x000000B5,
            0x000000D7,
            0x00000048,
            0x00000070,
            0x000000B5,
            0x000000B0,
            0x00000031,
            0x0000007C,
            0x0000001B,
            0x000000A8,
            0x00000052,
            0x0000004A,
            0x00000034,
            0x00000036,
            0x00000040,
            0x000000CD,
            0x0000004A,
            0x000000FF,
            0x00000013,
            0x0000007A,
            0x00000008,
            0x0000004A,
            0x00000048,
            0x00000013,
            0x00000059,
            0x000000A8,
            0x000000E4,
            0x00000017,
        }
    },
    { // Core 1
        .neurons = {
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 64 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 65 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 66 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 67 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 68 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 69 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 70 },
            { 0, 0, 0, {1, -1, 1, -1}, 0, 0, 0, 71 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
            { 0, 0, 0, {0, 0, 0, 0}, 0, 1, 0, 0 },
        },
        .synapse_connection = {
            0x000000B7,
            0x00000088,
            0x00000037,
            0x000000C4,
            0x000000A2,
            0x00000044,
            0x000000FB,
            0x00000047,
            0x00000004,
            0x00000076,
            0x00000088,
            0x0000006C,
            0x00000061,
            0x000000E7,
            0x00000019,
            0x000000C0,
            0x00000050,
            0x00000080,
            0x00000083,
            0x0000004D,
            0x00000002,
            0x0000008D,
            0x00000072,
            0x00000016,
            0x00000028,
            0x00000075,
            0x000000E9,
            0x00000048,
            0x000000B3,
            0x00000040,
            0x000000F1,
            0x0000000C,
            0x000000F1,
            0x00000002,
            0x000000E3,
            0x000000B3,
            0x00000050,
            0x000000AE,
            0x00000009,
            0x0000009D,
            0x00000038,
            0x00000044,
            0x0000001A,
            0x000000C4,
            0x0000002D,
            0x00000084,
            0x0000008A,
            0x00000034,
            0x000000BD,
            0x00000009,
            0x00000048,
            0x00000017,
            0x00000049,
            0x000000CF,
            0x000000A5,
            0x00000008,
            0x00000073,
            0x0000000C,
            0x000000F7,
            0x00000008,
            0x000000F5,
            0x00000018,
            0x000000C1,
            0x000000B3,
            0x0000005C,
            0x000000B7,
            0x000000EF,
            0x00000001,
            0x000000C7,
            0x00000027,
            0x0000001A,
            0x00000000,
            0x000000EF,
            0x000000D0,
            0x000000AD,
            0x00000011,
            0x0000004C,
            0x00000073,
            0x000000CC,
            0x00000052,
            0x00000080,
            0x0000007E,
            0x000000E1,
            0x0000001C,
            0x000000F7,
            0x00000008,
            0x00000055,
            0x000000B8,
            0x00000047,
            0x00000038,
            0x00000044,
            0x000000BB,
            0x0000006C,
            0x000000AE,
            0x000000A1,
            0x00000024,
            0x00000090,
            0x000000AA,
            0x000000CF,
            0x00000018,
            0x000000AF,
            0x00000030,
            0x000000AD,
            0x00000006,
            0x00000099,
            0x00000037,
            0x00000044,
            0x0000007E,
            0x00000080,
            0x000000BE,
            0x00000044,
            0x000000A8,
            0x00000047,
            0x000000AA,
            0x00000044,
            0x00000030,
            0x00000047,
            0x000000BB,
            0x00000049,
            0x00000033,
            0x00000048,
            0x000000BF,
            0x00000049,
            0x0000008F,
            0x000000C4,
            0x000000FD,
            0x00000047,
            0x000000B8,
            0x000000CF,
            0x00000084,
            0x00000009,
            0x000000A6,
            0x000000A9,
            0x0000004E,
            0x00000080,
            0x000000FE,
            0x00000001,
            0x000000BF,
            0x00000044,
            0x000000A8,
            0x00000057,
            0x00000082,
            0x00000042,
            0x00000012,
            0x000000CE,
            0x00000037,
            0x00000084,
            0x000000B6,
            0x00000009,
            0x000000CE,
            0x00000071,
            0x000000F1,
            0x0000000D,
            0x000000B8,
            0x00000057,
            0x000000A8,
            0x0000004B,
            0x000000D4,
            0x000000CB,
            0x00000006,
            0x00000019,
            0x0000004E,
            0x00000000,
            0x0000008E,
            0x00000000,
            0x000000AB,
            0x00000056,
            0x000000A9,
            0x00000054,
            0x00000090,
            0x00000080,
            0x0000005C,
            0x00000084,
            0x0000007F,
            0x00000000,
            0x0000006F,
            0x00000030,
            0x000000AA,
            0x00000051,
            0x000000E2,
            0x0000005F,
            0x000000B6,
            0x00000043,
            0x000000AC,
            0x00000072,
            0x000000C4,
            0x00000049,
            0x000000E4,
            0x000000BB,
            0x00000084,
            0x00000016,
            0x000000C1,
            0x00000035,
            0x000000D1,
            0x0000007C,
            0x00000098,
            0x000000A2,
            0x000000D8,
            0x000000A6,
            0x0000005E,
            0x00000080,
            0x000000BF,
            0x000000A0,
            0x0000004E,
            0x00000010,
            0x0000004E,
            0x0000004C,
            0x000000EA,
            0x00000054,
            0x00000025,
            0x00000062,
            0x0000008A,
            0x00000042,
            0x00000064,
            0x00000002,
            0x00000081,
            0x0000005B,
            0x00000011,
            0x0000007F,
            0x00000082,
            0x00000066,
            0x000000C3,
            0x000000A2,
            0x00000011,
            0x000000A6,
            0x0000005F,
            0x00000062,
            0x000000D9,
            0x00000070,
            0x000000CF,
            0x000000B0,
            0x000000E7,
            0x00000050,
            0x00000040,
            0x00000000,
            0x0000009A,
            0x0000006F,
            0x000000BD,
            0x0000009C,
            0x00000039,
            0x000000C6,
            0x0000004D,
            0x00000098,
            0x00000053,
            0x000000DE,
            0x00000085,
            0x0000000C,
            0x000000D1,
            0x000000A2,
            0x00000011,
            0x000000A4,
            0x00000035,
            0x000000F4,
            0x0000000B,
            0x000000F0,
            0x0000000B,
        }
    }
};

/* FUNCTIONS FOR READ/WRITE OPERATION VIA WB PORT */

uint32_t read_32bit_from_mem(const volatile uint32_t* base, uint32_t offset) {
    return *(base + offset);
}

void write_32_bit_to_mem(volatile uint32_t* base, uint32_t offset, uint32_t data) {
    *(base + offset) = data;
}

/* FUNCTIONS FOR SENDING NEURON_DATA TO SOC */

void send_synapse_connection_to_mem(uint8_t core_index, volatile uint32_t* base_addr) {
    // Assuming core_index is within valid range (0 to NUM_CORES - 1)

    // Calculate the offset based on the core_index
    uint32_t offset = core_index * (4 * AXONS_PER_CORE);

    // Send synapse connection data to memory in 32-bit batch
    for (uint32_t i = 0; i < AXONS_PER_CORE; ++i) {
        uint32_t synapse_connection_value = cores[core_index].synapse_connection[i];
        write_32_bit_to_mem(base_addr, offset + i * 4, synapse_connection_value);
    }
}

void send_neuron_params_to_mem(uint8_t core_index, volatile uint32_t* base_addr) {
    // Assuming core_index is within valid range (0 to NUM_CORES - 1)

    // Calculate the offset based on the core_index and adjusted formula
    uint32_t offset = core_index * (11 * NEURONS_PER_CORE);

    // Send neuron parameters to memory in 32-bit batches
    for (uint32_t i = 0; i < NEURONS_PER_CORE; ++i) {
        Neuron* current_neuron = &cores[core_index].neurons[i];

        // Calculate the offset for the current neuron
        uint32_t neuron_offset = offset + i * 11;

        // Concatenate parameters based on reversed hardware bit order
        uint32_t batch1 = ((uint32_t)current_neuron->leakage_value << 24) |
                          ((uint32_t)current_neuron->negative_threshold << 16) |
                          ((uint32_t)current_neuron->positive_threshold << 8) |
                          (uint32_t)current_neuron->membrane_potential;

        uint32_t batch2 = ((uint32_t)current_neuron->weights[3] << 24) |
                          ((uint32_t)current_neuron->weights[2] << 16) |
                          ((uint32_t)current_neuron->weights[1] << 8) |
                          (uint32_t)current_neuron->weights[0];

        uint32_t batch3 = ((uint32_t)current_neuron->reset_nega_potential << 24) |
                          ((uint32_t)current_neuron->reset_posi_potential << 16) |
                          (uint32_t)current_neuron->axon_dest;

        // Write the concatenated batches to memory
        write_32_bit_to_mem(base_addr, neuron_offset, batch1);
        write_32_bit_to_mem(base_addr, neuron_offset + 4, batch2);
        write_32_bit_to_mem(base_addr, neuron_offset + 8, batch3);
    }
}

void read_neuron_params_from_mem(uint8_t core_index, volatile uint32_t* base_addr) {
    // Assuming core_index is within valid range (0 to NUM_CORES - 1)

    // Calculate the offset based on the core_index and adjusted formula
    uint32_t offset = core_index * (11 * NEURONS_PER_CORE);

    // Read neuron parameters from memory in 32-bit batches
    for (uint32_t i = 0; i < NEURONS_PER_CORE; ++i) {
        Neuron* current_neuron = &cores[core_index].neurons[i];

        // Calculate the offset for the current neuron
        uint32_t neuron_offset = offset + i * 11;

        // Read the concatenated batches from memory
        uint32_t batch1 = read_32bit_from_mem(base_addr + neuron_offset, 0);
        uint32_t batch2 = read_32bit_from_mem(base_addr + neuron_offset + 4, 0);
        uint32_t batch3 = read_32bit_from_mem(base_addr + neuron_offset + 8, 0);

        // Extract individual parameters from batches
        current_neuron->leakage_value = (int8_t)(batch1 >> 24);
        current_neuron->negative_threshold = (int8_t)((batch1 >> 16) & 0xFF);
        current_neuron->positive_threshold = (int8_t)((batch1 >> 8) & 0xFF);
        current_neuron->membrane_potential = (int8_t)(batch1 & 0xFF);

        current_neuron->weights[3] = (int8_t)(batch2 >> 24);
        current_neuron->weights[2] = (int8_t)((batch2 >> 16) & 0xFF);
        current_neuron->weights[1] = (int8_t)((batch2 >> 8) & 0xFF);
        current_neuron->weights[0] = (int8_t)(batch2 & 0xFF);

        current_neuron->reset_nega_potential = (int8_t)(batch3 >> 24);
        current_neuron->reset_posi_potential = (int8_t)((batch3 >> 16) & 0xFF);
        current_neuron->axon_dest = (uint8_t)(batch3 & 0xFF);
    }
}


void main() {    

    ///////////////////////////////// CONFIG FOR SOC
	// Set up the housekeeping SPI to be connected internally, so that it doesn't conflict with user area GPIO pins	
    reg_spi_enable = 1;
    // Setup reg_wb_enable = 1 to enable connection to wishbone port in user project 
    reg_wb_enable = 1;    

    // Set config registers of I/O pads to mode GPIO_MODE_MGMT_STD_OUTPUT
    //  <=> configures the upper 16 of the first 32 GPIO pads as outputs from the managent SoC
    reg_mprj_io_31 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_30 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_29 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_28 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_27 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_26 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_25 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_24 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_23 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_22 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_21 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_20 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_19 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_18 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_17 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_16 = GPIO_MODE_MGMT_STD_OUTPUT;

    /* Apply I/O mode configuration by initiating the serial transfer on the shift register
    (reg_mprj_xfer=1) and wait till it finishes (reg_mprj_xfer == 0)*/
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);

    ////////////////////////////////// 

    // Reference for each pointer's value in memory
    uint32_t synapMatrixValue = *SYNAP_MATRIX_PTR;
    uint32_t paramValue = *PARAM_PTR;
    uint32_t neuronSpikeOutValue = *NEURON_SPIKE_OUT_PTR;

    // 

    // Flag start of the test
	reg_mprj_datal = 0xAB600000;

    /* SEND NEURON_DATA OF A CORE TO MEM */
    send_synapse_connection_to_mem(0, SYNAP_MATRIX_PTR);
    reg_mprj_datal = 0xAB610000;
}