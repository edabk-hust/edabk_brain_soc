

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
#include "SNN_data.h"

#define NUM_CORES 5
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