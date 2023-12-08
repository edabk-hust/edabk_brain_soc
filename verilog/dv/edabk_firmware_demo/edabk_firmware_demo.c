

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

Core cores[NUM_CORES] = {
    { // Core 0
        .neurons = {
            { 0, 0, -23, {0, 0, -19, 0}, 0, -18, 0, 0 },
            { -17, 0, 0, {-16, 1, 0, 5}, 1, 0, 6, 1 },
            { 0, 7, 1, {0, 8, 1, 0}, 9, 1, 0, 10 },
            { 1, 0, 11, {1, 0, 12, 1}, 0, 13, 1, 0 },
            { 14, 1, 0, {15, 1, 0, 16}, 1, 0, 33, 1 },
            { 0, 34, 1, {0, 35, 1, 0}, 36, 1, 0, 37 },
            { 1, 0, 38, {1, 0, 39, 1}, 0, 40, 1, 0 },
            { 41, 1, 0, {42, 1, 0, 43}, 1, 0, 44, 1 },
            { 0, 57, 1, {0, 61, 1, 0}, 62, 1, 0, 63 },
            { 1, 0, 64, {1, 0, 84, 1}, 0, 85, 1, 0 },
            { 111, 1, 0, {112, 1, 0, 113}, 1, 0, -118, 1 },
            { 0, -117, 1, {0, -116, 1, 0}, -90, 1, 0, 167 },
            { 1, 0, -63, {1, 0, -62, 1}, 0, -61, 1, 0 },
            { -35, 1, 0, {-34, 1, 0, -33}, 1, 0, -7, 1 },
            { 0, -6, 1, {0, -5, 1, 0}, -4, 1, 0, 253 },
            { 1, 0, -2, {2, 0, 17, 2}, 0, 18, 2, 0 },
            { 19, 2, 0, {45, 2, 0, 46}, 2, 0, 47, 2 },
            { 0, 73, 2, {0, 74, 2, 0}, 75, 2, 0, 76 },
            { 2, 0, 77, {2, 0, 78, 2}, 0, 101, 2, 0 },
            { 102, 2, 0, {103, 2, 0, 104}, 2, 0, 105, 2 },
            { 0, 106, 2, {0, 107, 2, 0}, 108, 2, 0, 109 },
            { 2, 0, 110, {2, 0, 111, 2}, 0, 112, 2, 0 },
            { 113, 2, 0, {114, 2, 0, -125}, 2, 0, -124, 2 },
            { 0, -123, 2, {0, -122, 2, 0}, -121, 2, 0, 136 },
            { 2, 0, -119, {2, 0, -118, 2}, 0, -117, 2, 0 },
            { -116, 2, 0, {-115, 2, 0, -114}, 2, 0, -93, 2 },
            { 0, -92, 2, {0, -91, 2, 0}, -90, 2, 0, 167 },
            { 2, 0, -88, {2, 0, -87, 2}, 0, -86, 2, 0 },
            { -85, 2, 0, {-64, 2, 0, -63}, 2, 0, -62, 2 },
            { 0, -60, 2, {0, -59, 2, 0}, -58, 2, 0, 199 },
            { 2, 0, -36, {2, 0, -35, 2}, 0, -34, 2, 0 },
            { -33, 2, 0, {-32, 2, 0, -31}, 2, 0, -30, 2 },
        },
        .synapse_connection = {
            0x0002E300,
            0xF90002F8,
            0x02FA0002,
            0x0002FB00,
            0xFD0002FC,
            0x03FE0002,
            0x00031000,
            0x12000311,
            0x03140003,
            0x00031500,
            0x17000316,
            0x032C0003,
            0x00032D00,
            0x2F00032E,
            0x03300003,
            0x00033100,
            0x33000332,
            0x03480003,
            0x00034900,
            0x4B00034A,
            0x034C0003,
            0x00034D00,
            0x6500034E,
            0x03660003,
            0x00036700,
            0x69000368,
            0x00830003,
            0x0000D300,
            0xD50000D4,
            0x00EC0000,
            0x0000ED00,
            0xEF0000EE,
            0x00F00000,
            0x0000F100,
            0xF30000F2,
            0x00F40000,
            0x0001F500,
            0x24000123,
            0x01250001,
            0x00013C00,
            0x3E00013D,
            0x013F0001,
            0x00014000,
            0x42000141,
            0x01430001,
            0x00014400,
            0x56000145,
            0x01570001,
            0x00015800,
            0x5A000159,
            0x015B0001,
            0x00015C00,
            0x5E00015D,
            0x015F0001,
            0x00016000,
            0x70000161,
            0x01710001,
            0x00017200,
            0x74000173,
            0x01750001,
            0x00017900,
            0x7C00017B,
            0x017D0001,
            0x00017E00,
            0x8D00018C,
            0x018E0001,
            0x00018F00,
            0x98000195,
            0x01990001,
            0x00019A00,
            0xA80001A7,
            0x01A90001,
            0x0001B200,
            0xB40001B3,
            0x01B50001,
            0x0001B600,
            0xC40001C3,
            0x01C50001,
            0x0001C700,
            0xC90001C8,
            0x01CA0001,
            0x0001CB00,
            0xCD0001CC,
            0x01CE0001,
            0x0001CF00,
            0xD10001D0,
            0x01DF0001,
            0x0001E000,
            0xE20001E1,
            0x01E30001,
            0x0001E400,
            0xE60001E5,
            0x01E70001,
            0x0001E800,
            0xEA0001E9,
            0x01EB0001,
            0x0001EC00,
            0xFE0001FD,
            0x02FF0001,
            0x00020200,
            0x04000203,
            0x02050002,
            0x00020600,
            0x14000213,
            0x02150002,
            0x00021700,
            0x19000218,
            0x021A0002,
            0x00021B00,
            0x1D00021C,
            0x021E0002,
            0x00021F00,
            0x21000220,
            0x022F0002,
            0x00023000,
            0x32000231,
            0x02330002,
            0x00023400,
            0x36000235,
            0x02370002,
            0x00023800,
            0x3A000239,
            0x023B0002,
            0x00023C00,
            0x4E00024D,
            0x024F0002,
            0x00025000,
            0x52000251,
            0x02530002,
            0x00025400,
            0x56000255,
            0x02570002,
            0x00026E00,
            0x7000026F,
            0x02710002,
            0x00028A00,
            0x8C00028B,
            0x02A50002,
            0x0002A600,
            0xA80002A7,
            0x02C00002,
            0x0002C100,
            0xC30002C2,
            0x02DB0002,
            0x0002DC00,
            0xDE0002DD,
            0x02F70002,
            0x0002F800,
            0xFA0002F9,
            0x03100003,
            0x00031100,
            0x13000312,
            0x032B0003,
            0x00032C00,
            0x2E00032D,
            0x03470003,
            0x00034800,
            0x4A000349,
            0x03620003,
            0x00036300,
            0x65000364,
            0x037E0003,
            0x00037F00,
            0x99000380,
            0x039A0003,
            0x00039B00,
            0xB500039C,
            0x03B60003,
            0x0003B700,
            0x000000D2,
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
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 64 },
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 65 },
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 66 },
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 67 },
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 68 },
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 69 },
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 70 },
            { 0, 0, 0, {1, -127, 1, -127}, 0, 0, 0, 71 },
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